// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// ltl_aba_synthesis.tmpl.h - Spot subprocess integration, HOA/DPA parsing
// Split from ltl_aba.tmpl.h for readability.

namespace idni::tau_lang {

// ── Spot subprocess ───────────────────────────────────────────────────────────

// Spawn an external command directly via posix_spawnp (no shell), capture
// its stdout, and return {captured-output, exit-code}.
//
// Why not popen?  popen("cmd") forwards the argument to /bin/sh -c, which
// requires shell-escaping every interpolated value; it also doubles the
// process count, charges the cost of parsing the shell, and inherits the
// shell's signal handling.  posix_spawnp + execvp passes argv exactly as
// given, with no shell intermediating, which is both safer (no escaping
// bugs across `"`/`\\`/`$`/`` ` `` plus everything else) and a measurable
// fraction faster on the hot synthesis path.
//
// Conventions:
//   exit_code ==  127  -> command not found (matches the legacy run_cmd
//                         contract; callers test against 127 to print a
//                         "ltlsynt not on PATH" hint).
//   exit_code ==   -1  -> spawn or wait failed.
//   exit_code == >124  -> the optional `timeout_sec` wrapper killed the
//                         child (`timeout -k 0 N cmd ...`-style behaviour
//                         is implemented via SIGTERM after `timeout_sec`
//                         seconds).
//   stderr             -> redirected to /dev/null.
//
// The caller passes argv as a vector of C++ strings.  An empty
// `timeout_sec` (or 0) disables the kill-after-N-seconds watchdog.
static std::pair<std::string, int> spawn_capture(
    const std::vector<std::string>& argv,
    int timeout_sec = 0)
{
	if (argv.empty()) return {"", -1};

	int pipefd[2];
	if (::pipe(pipefd) != 0) return {"", -1};

	posix_spawn_file_actions_t fa;
	if (posix_spawn_file_actions_init(&fa) != 0) {
		::close(pipefd[0]); ::close(pipefd[1]);
		return {"", -1};
	}
	// Child: redirect stdout -> pipe write end; stderr -> /dev/null.
	posix_spawn_file_actions_addclose(&fa, pipefd[0]);
	posix_spawn_file_actions_adddup2 (&fa, pipefd[1], STDOUT_FILENO);
	posix_spawn_file_actions_addclose(&fa, pipefd[1]);
	posix_spawn_file_actions_addopen (&fa, STDERR_FILENO, "/dev/null",
	                                  O_WRONLY, 0);

	std::vector<char*> cargv;
	cargv.reserve(argv.size() + 1);
	for (const auto& s : argv) cargv.push_back(const_cast<char*>(s.c_str()));
	cargv.push_back(nullptr);

	pid_t pid;
	int rc = posix_spawnp(&pid, cargv[0], &fa, nullptr, cargv.data(), environ);
	posix_spawn_file_actions_destroy(&fa);
	::close(pipefd[1]);
	if (rc != 0) {
		::close(pipefd[0]);
		// posix_spawnp returns ENOENT (==2) when the binary isn't on PATH;
		// surface that as 127 to match the legacy `popen` contract.
		return {"", rc == ENOENT ? 127 : -1};
	}

	// Watchdog thread: SIGTERM the child after timeout_sec seconds.
	std::atomic<bool> done{false};
	std::thread killer;
	if (timeout_sec > 0) {
		killer = std::thread([pid, timeout_sec, &done]() {
			for (int i = 0; i < timeout_sec * 10 && !done.load(); ++i)
				::usleep(100'000);
			if (!done.load()) ::kill(pid, SIGTERM);
		});
	}

	// Drain pipe.
	std::string out;
	std::array<char, 4096> buf;
	for (;;) {
		ssize_t n = ::read(pipefd[0], buf.data(), buf.size());
		if (n > 0) out.append(buf.data(), buf.data() + n);
		else if (n == 0) break;
		else if (errno == EINTR) continue;
		else break;
	}
	::close(pipefd[0]);

	int status = 0;
	while (::waitpid(pid, &status, 0) < 0 && errno == EINTR) {}
	done.store(true);
	if (killer.joinable()) killer.join();

	int exit_code;
	if (WIFEXITED(status))        exit_code = WEXITSTATUS(status);
	else if (WIFSIGNALED(status)) exit_code = 128 + WTERMSIG(status);
	else                          exit_code = -1;
	return {out, exit_code};
}

// Legacy shim kept for the few sites still passing a pre-built shell
// command line (autfilt --dot, ltlfilt -f).  These paths will be migrated
// in a follow-up; for now they incur the popen-shell cost only on rarely-
// hit fallbacks.
static std::pair<std::string, int> run_cmd(const std::string& cmd) {
	std::array<char, 4096> buf;
	std::string result;
	FILE* raw = popen(cmd.c_str(), "r");
	if (!raw) throw std::runtime_error("popen() failed for: " + cmd);
	while (fgets(buf.data(), static_cast<int>(buf.size()), raw))
		result += buf.data();
	int status = pclose(raw);
	int exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
	return {result, exit_code};
}

// Write `content` to a fresh /tmp/<prefix>_XXXXXX path.  Caller owns
// removal.  Returns the absolute path on success, "" on failure.
static std::string write_tempfile(const std::string& prefix,
                                  const std::string& content)
{
	std::string tmpl = "/tmp/" + prefix + "_XXXXXX";
	std::vector<char> buf(tmpl.begin(), tmpl.end());
	buf.push_back('\0');
	int fd = ::mkstemp(buf.data());
	if (fd < 0) return "";
	const char* p = content.data();
	size_t left = content.size();
	while (left) {
		ssize_t n = ::write(fd, p, left);
		if (n < 0) { if (errno == EINTR) continue; ::close(fd); return ""; }
		p += n; left -= static_cast<size_t>(n);
	}
	::close(fd);
	return std::string(buf.data());
}

inline std::pair<bool, std::string> call_ltlsynt(
    const std::string& ltl_formula,
    const std::vector<std::string>& input_props,
    const std::vector<std::string>& output_props)
{
	// Build --ins and --outs CSV lists.
	std::string ins_str, outs_str;
	for (size_t i = 0; i < input_props.size(); ++i) {
		if (i) ins_str += ",";
		ins_str += input_props[i];
	}
	for (size_t i = 0; i < output_props.size(); ++i) {
		if (i) outs_str += ",";
		outs_str += output_props[i];
	}

	// Configurable timeout: TAU_LTL_TIMEOUT_SEC (default 60). 0 disables.
	int timeout_sec = 60;
	if (const char* env_sec = std::getenv("TAU_LTL_TIMEOUT_SEC")) {
		timeout_sec = std::atoi(env_sec);
		if (timeout_sec < 0) timeout_sec = 0;
	}

	// Always write the formula to a temp file and use `-F path`.  The old
	// inline `--formula="..."` path required shell-escaping every char and
	// was capped at the Linux MAX_ARG_STRLEN limit (131072) when the
	// formula grew (Algorithm B with many constants).  posix_spawn passes
	// argv vectors directly so neither limit applies, but writing the
	// formula to a file is still the simpler invariant — exactly one code
	// path, exactly one length cap (the filesystem's).
	std::string tmpfile_path = write_tempfile("tau_lang", ltl_formula + "\n");
	if (tmpfile_path.empty()) {
		LOG_ERROR << "[ltl_aba] failed to write temp file for ltlsynt input\n";
		return {false, ""};
	}

	auto build_argv = [&](const std::string& formula_path) {
		std::vector<std::string> argv = {"ltlsynt", "-F", formula_path};
		if (!ins_str.empty())  argv.push_back("--ins="  + ins_str);
		if (!outs_str.empty()) argv.push_back("--outs=" + outs_str);
		if (const char* simp = std::getenv("TAU_LTL_SIMPLIFICATION"))
			if (*simp) argv.push_back("--simplification=" + std::string(simp));
		return argv;
	};

	auto argv = build_argv(tmpfile_path);

	{
		std::string flat;
		for (const auto& a : argv) { flat += a; flat += ' '; }
		LOG_DEBUG << "[ltl_aba] calling: " << flat;
	}

	auto [out, exit_code] = spawn_capture(argv, timeout_sec);
	std::remove(tmpfile_path.c_str());
	LOG_DEBUG << "[ltl_aba] ltlsynt output (exit=" << exit_code << "): " << out;

	if (exit_code == 127) {
		LOG_ERROR << "[ltl_aba] ltlsynt not found on PATH. "
		             "Install Spot (>= 2.10) and ensure ltlsynt is on PATH.\n";
		return {false, ""};
	}
	if (out.empty() || out.substr(0, 12) == "UNREALIZABLE") {
		// Q40-UX3: on UNREAL, optionally produce env counter-strategy.
		// By determinacy of ω-regular two-player games, UNREAL means
		// ∃env.∀sys.¬φ.  So swapping roles (ins↔outs) and negating the
		// formula yields a game where env plays the role of sys; if that
		// is REALIZABLE, ltlsynt's output is env's winning strategy.
		if (const char* w = std::getenv("TAU_LTL_WITNESS");
		    w && *w && std::string(w) != "0")
		{
			std::string neg_path = write_tempfile(
			    "tau_lang_neg", "!(" + ltl_formula + ")\n");
			if (!neg_path.empty()) {
				std::vector<std::string> neg_argv = {
				    "ltlsynt", "-F", neg_path};
				// Swap --ins and --outs in the negated game.
				if (!outs_str.empty()) neg_argv.push_back("--ins="  + outs_str);
				if (!ins_str.empty())  neg_argv.push_back("--outs=" + ins_str);
				auto [neg_out, neg_rc] = spawn_capture(neg_argv, timeout_sec);
				std::remove(neg_path.c_str());
				if (neg_rc != 127 && neg_out.substr(0, 10) == "REALIZABLE") {
					std::string env_hoa = neg_out.substr(neg_out.find('\n') + 1);
					std::fprintf(stderr,
					    "=== ENV COUNTER-STRATEGY (UNREAL witness) ===\n%s\n",
					    env_hoa.c_str());
				}
			}
		}
		return {false, ""};
	}
	if (out.substr(0, 10) == "REALIZABLE") {
		std::string hoa = out.substr(out.find('\n') + 1);
		// Strategy export:
		//   TAU_LTL_EXPORT_STRATEGY=hoa  → print HOA to stderr
		//   TAU_LTL_EXPORT_STRATEGY=dot  → ask autfilt for dot, print to stderr
		//   TAU_LTL_EXPORT_STRATEGY_FILE=<path> → also write HOA to that path
		const char* export_fmt = std::getenv("TAU_LTL_EXPORT_STRATEGY");
		if (export_fmt && !hoa.empty()) {
			std::string fmt = export_fmt;
			if (fmt == "hoa") {
				std::fprintf(stderr, "=== STRATEGY HOA ===\n%s\n", hoa.c_str());
			} else if (fmt == "dot") {
				// Pipe HOA through autfilt --dot if available; else fall back to HOA.
				std::string tmp = "/tmp/tau_strategy_" + std::to_string(::getpid()) + ".hoa";
				if (FILE* f = std::fopen(tmp.c_str(), "w")) {
					std::fwrite(hoa.data(), 1, hoa.size(), f);
					std::fclose(f);
					auto [dot, rc] = run_cmd("autfilt --dot " + tmp + " 2>/dev/null");
					std::remove(tmp.c_str());
					if (rc == 0 && !dot.empty())
						std::fprintf(stderr, "=== STRATEGY DOT ===\n%s\n", dot.c_str());
					else
						std::fprintf(stderr, "=== STRATEGY HOA (dot unavailable) ===\n%s\n", hoa.c_str());
				}
			}
		}
		if (const char* path = std::getenv("TAU_LTL_EXPORT_STRATEGY_FILE"); path && *path) {
			if (FILE* f = std::fopen(path, "w")) {
				std::fwrite(hoa.data(), 1, hoa.size(), f);
				std::fclose(f);
			}
		}
		// Log state count at INFO level if possible (parse `States:` from HOA header).
		{
			auto pos = hoa.find("States:");
			if (pos != std::string::npos) {
				auto eol = hoa.find('\n', pos);
				std::string line = hoa.substr(pos, eol - pos);
				LOG_INFO << "[ltl_aba] strategy " << line;
			}
		}
		return {true, hoa};
	}
	return {false, ""};
}

// ── HOA parser ────────────────────────────────────────────────────────────────

inline HoaAutomaton parse_hoa(const std::string& hoa_text) {
	HoaAutomaton aut;
	std::istringstream ss(hoa_text);
	std::string line;

	bool in_body = false;
	int cur_state = -1;

	while (std::getline(ss, line)) {
		if (line.empty() || line == "--END--") continue;
		if (line == "--BODY--") { in_body = true; continue; }

		if (!in_body) {
			if (line.substr(0, 7) == "States:") {
				aut.num_states = std::stoi(line.substr(7));
				aut.edges.resize(aut.num_states);
				aut.state_accepting.resize(aut.num_states, false);
			} else if (line.substr(0, 6) == "Start:") {
				aut.initial_state = std::stoi(line.substr(6));
			} else if (line.substr(0, 3) == "AP:") {
				std::istringstream apl(line.substr(3));
				int n; apl >> n;
				for (int i = 0; i < n; ++i) {
					std::string ap; apl >> ap;
					// strip surrounding quotes
					if (ap.size() >= 2 && ap.front() == '"'
					    && ap.back()  == '"')
						ap = ap.substr(1, ap.size()-2);
					aut.aps.push_back(ap);
				}
			}
			continue;
		}

		// In body: lines starting with "State:" or "[guard] dst {acc}"
		if (line.substr(0, 6) == "State:") {
			// e.g. "State: 0" or "State: 0 {0}"
			std::istringstream sl(line.substr(6));
			sl >> cur_state;
			// Check for state-based acceptance marks
			if (line.find('{') != std::string::npos)
				aut.state_accepting[cur_state] = true;
			continue;
		}

		if (cur_state < 0 || line.front() != '[') continue;

		// Parse transition: [guard] dst [{acc}]
		size_t rb = line.find(']');
		if (rb == std::string::npos) continue;
		std::string guard = line.substr(1, rb - 1);
		std::istringstream tl(line.substr(rb + 1));
		int dst;
		if (!(tl >> dst)) continue;

		HoaEdge e;
		e.guard_label = guard;
		e.dst = dst;
		// Edge acceptance mark
		std::string rest; tl >> rest;
		e.accepting = (rest.find('{') != std::string::npos);
		// Also treat as accepting if destination is an accepting state
		// (will be fixed after all states are parsed)
		aut.edges[cur_state].push_back(e);
	}

	// Propagate state-based acceptance to edges (for Büchi with state marks)
	for (int s = 0; s < aut.num_states; ++s)
		for (auto& e : aut.edges[s])
			if (aut.state_accepting[e.dst])
				e.accepting = true;

	return aut;
}

// ── DPA extraction — Algorithm D Phase 1 ─────────────────────────────────────

inline std::string call_ltl2tgba_dpa(const std::string& ltl_formula) {
	// Use posix_spawnp + argv vector so the formula is passed verbatim
	// (no shell escaping; no MAX_ARG_STRLEN cap on a single quoted arg).
	int timeout_sec = 60;
	if (const char* env_sec = std::getenv("TAU_LTL_TIMEOUT_SEC")) {
		timeout_sec = std::atoi(env_sec);
		if (timeout_sec < 0) timeout_sec = 0;
	}
	std::vector<std::string> argv = {
	    "ltl2tgba", "--parity=min even", "-D", "--complete", "-f", ltl_formula
	};
	auto [out, _rc] = spawn_capture(argv, timeout_sec);
	return out;
}

inline DpaAutomaton parse_dpa_hoa(const std::string& hoa_text) {
	DpaAutomaton dpa;
	std::istringstream ss(hoa_text);
	std::string line;
	bool in_body = false;
	int cur_state = -1;

	while (std::getline(ss, line)) {
		if (line.empty() || line == "--END--") continue;
		if (line == "--BODY--") { in_body = true; continue; }

		if (!in_body) {
			if (line.substr(0, 7) == "States:") {
				dpa.num_states = std::stoi(line.substr(7));
				dpa.edges.resize(dpa.num_states);
			} else if (line.substr(0, 6) == "Start:") {
				dpa.initial_state = std::stoi(line.substr(6));
			} else if (line.substr(0, 3) == "AP:") {
				std::istringstream apl(line.substr(3));
				int n; apl >> n;
				for (int i = 0; i < n; ++i) {
					std::string ap; apl >> ap;
					if (ap.size() >= 2 && ap.front() == '"' && ap.back() == '"')
						ap = ap.substr(1, ap.size() - 2);
					dpa.aps.push_back(ap);
				}
			} else if (line.substr(0, 11) == "acc-name: p") {
				// "acc-name: parity min even N" — extract N
				std::istringstream al(line);
				std::string tok;
				int cnt = 0;
				while (al >> tok) {
					++cnt;
					if (cnt == 5) { // field after "parity min even"
						try { dpa.num_colors = std::stoi(tok); }
						catch (...) { LOG_ERROR << "HOA: failed to parse num_colors"; }
					}
				}
				dpa.min_even = (line.find("even") != std::string::npos);
			} else if (line.substr(0, 11) == "Acceptance:") {
				// Extract color count from "Acceptance: N ..."
				std::istringstream al(line.substr(11));
				al >> dpa.num_colors;
			}
			continue;
		}

		if (line.substr(0, 6) == "State:") {
			std::istringstream sl(line.substr(6));
			sl >> cur_state;
			continue;
		}
		if (cur_state < 0 || line.front() != '[') continue;

		size_t rb = line.find(']');
		if (rb == std::string::npos) continue;
		std::string guard = line.substr(1, rb - 1);
		std::istringstream tl(line.substr(rb + 1));
		int dst;
		if (!(tl >> dst)) continue;

		DpaEdge e;
		e.guard_label = guard;
		e.dst = dst;
		// Look for acceptance mark {N}
		std::string rest; tl >> rest;
		if (!rest.empty() && rest.front() == '{') {
			try {
				e.color = std::stoi(rest.substr(1));
			} catch (...) { LOG_ERROR << "HOA: failed to parse edge color"; }
		}
		dpa.edges[cur_state].push_back(e);
	}

	return dpa;
}

} // namespace idni::tau_lang
