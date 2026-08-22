// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// ltl_aba_synthesis.tmpl.h - Spot subprocess integration, HOA/DPA parsing
// Split from ltl_aba.tmpl.h for readability.

namespace idni::tau_lang {

// ── Spot subprocess ───────────────────────────────────────────────────────────

// LS-9: one parser for the TAU_LTL_TIMEOUT_SEC watchdog (default 60s;
// explicit "0" disables). Garbage keeps the DEFAULT instead of atoi's 0
// silently removing the wall-clock cap on external ltlsynt/ltl2tgba.
// SY-R5: range garbage is clamped to one day with the same warning --
// `(int) v` used to turn 2^32 into 0 (watchdog silently OFF) and 2^31
// into a negative, and values near INT_MAX overflowed the poll bound.
inline constexpr long ltl_timeout_sec_max = 86400;

inline int ltl_timeout_sec() {
	int timeout_sec = 60;
	if (const char* env_sec = std::getenv("TAU_LTL_TIMEOUT_SEC")) {
		char* end = nullptr;
		errno = 0;
		long v = std::strtol(env_sec, &end, 10);
		if (end == env_sec || *end != '\0' || v < 0 || errno == ERANGE) {
			TAU_LOG_WARNING << "TAU_LTL_TIMEOUT_SEC='" << env_sec
				<< "' is not a non-negative number; keeping the default "
				<< timeout_sec << "s";
		} else if (v > ltl_timeout_sec_max) {
			TAU_LOG_WARNING << "TAU_LTL_TIMEOUT_SEC=" << v
				<< " exceeds the maximum; clamping to "
				<< ltl_timeout_sec_max << "s";
			timeout_sec = (int) ltl_timeout_sec_max;
		} else timeout_sec = (int) v;
	}
	return timeout_sec;
}


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
			const long long polls = 10LL * timeout_sec;
			for (long long i = 0; i < polls && !done.load(); ++i)
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

// (LS-21: the legacy popen-shell `run_cmd` shim was removed once its last
// caller, the strategy dot export, moved to write_tempfile + spawn_capture.)

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
	int timeout_sec = ltl_timeout_sec();

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

	// LT-7: only exit 127 used to be special-cased, so a watchdog kill
	// (128 + SIGTERM = 143) or a usage error fell into the `out.empty()`
	// branch below and every caller read the result as a definitive
	// UNREALIZABLE.  A slow-but-realizable specification therefore got a
	// wrong answer with nothing but a LOG_DEBUG trace behind it.
	switch (classify_spot_exit(exit_code, out)) {
	case spot_exit_kind::not_found:
		// IN-N1: a missing backend is no verdict either; returning
		// {false, ""} here made every caller print "UNREALIZABLE".
		LOG_ERROR << "[ltl_aba] ltlsynt not found on PATH. "
		             "Install Spot (>= 2.10) and ensure ltlsynt is on PATH.\n";
		throw ltl_synthesis_error("ltlsynt not found on PATH; install "
			"Spot (>= 2.10) -- realizability is UNKNOWN");
	case spot_exit_kind::failed: {
		std::string msg = "ltlsynt produced no verdict (exit "
		                + std::to_string(exit_code) + ")";
		if (exit_code == 143)
			msg += " — killed by the TAU_LTL_TIMEOUT_SEC watchdog ("
			     + std::to_string(timeout_sec) + "s)";
		LOG_ERROR << "[ltl_aba] " << msg
		          << "; the realizability of this specification is UNKNOWN\n";
		throw ltl_synthesis_error(msg);
	}
	case spot_exit_kind::ok:
		break;
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
					auto nl = neg_out.find('\n');
					std::string env_hoa = nl == std::string::npos
						? std::string() : neg_out.substr(nl + 1);
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
				// LS-21: mkstemp-style tempfile + argv spawn (no
				// predictable /tmp name, no shell string concat).
				std::string tmp = write_tempfile("tau_strategy", hoa);
				if (!tmp.empty()) {
					auto [dot, rc] = spawn_capture(
					    {"autfilt", "--dot", tmp}, timeout_sec);
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
	// SY-R4: output that starts with neither verdict line is no verdict
	// (a crashed or foreign binary on PATH printing something else with
	// exit 0); it used to fall through as UNREALIZABLE.
	LOG_ERROR << "[ltl_aba] ltlsynt output carried no verdict line; the "
	             "realizability of this specification is UNKNOWN\n";
	throw ltl_synthesis_error("ltlsynt output carried no verdict line");
}

// ── HOA parser ────────────────────────────────────────────────────────────────

inline HoaAutomaton parse_hoa(const std::string& hoa_text) {
	HoaAutomaton aut;
	std::istringstream ss(hoa_text);
	std::string line;

	bool in_body = false, seen_states = false, seen_end = false;
	int cur_state = -1;
	// A strategy with more states than this is not something ltlsynt
	// produces for any specification this pipeline builds; an absurd
	// count is a garbled header, not an automaton (SY-R3).
	constexpr long max_states = 1L << 22;

	while (std::getline(ss, line)) {
		if (line.empty()) continue;
		// SY-4: the automaton ends here; anything after it (a second
		// automaton, stray diagnostics) must not be spliced in.
		if (line == "--END--") { seen_end = true; break; }
		if (line == "--BODY--") { in_body = true; continue; }

		if (!in_body) {
			if (line.substr(0, 7) == "States:") {
				// LA-8 / SY-1 / SY-R3: a garbled or absurd state
				// count used to become the EMPTY automaton,
				// which the verdict layer reads as trivially
				// REALIZABLE. A strategy has at least one state;
				// refuse anything else.
				long n = -1;
				try {
					n = std::stol(line.substr(7));
				} catch (const std::exception&) {
					n = -1;
				}
				if (n < 1 || n > max_states)
					throw ltl_synthesis_error(
						"malformed HOA strategy: bad "
						"state count '" + line + "'");
				aut.num_states = (int) n;
				seen_states = true;
				aut.edges.resize(aut.num_states);
				aut.state_accepting.resize(aut.num_states, false);
			} else if (line.substr(0, 6) == "Start:") {
				try {
					aut.initial_state =
						std::stoi(line.substr(6));
				} catch (const std::exception&) {
					aut.initial_state = 0;
				}
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
			// LT-10: bound the state number before indexing
			if (cur_state < 0
				|| cur_state >= (int) aut.num_states) {
				cur_state = -1;
				continue;
			}
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

		// LT-10: an edge to an out-of-range destination is dropped
		if (dst < 0 || dst >= (int) aut.num_states) continue;
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

	// LA-8 / SY-1: no header, or a header with no body, is a truncated
	// or garbled strategy (a timed-out or crashed ltlsynt, a stub on
	// PATH) -- not an automaton. Refuse rather than execute an
	// unconstrained program.
	if (!seen_states || !in_body)
		throw ltl_synthesis_error(std::string("malformed HOA strategy: ")
			+ (!seen_states ? "no `States:` header" : "no `--BODY--`")
			+ (seen_end ? "" : " (and no `--END--`)"));
	(void) seen_end;

	// Propagate state-based acceptance to edges (for Büchi with state marks)
	for (int s = 0; s < aut.num_states; ++s)
		for (auto& e : aut.edges[s])
			if (aut.state_accepting[e.dst])
				e.accepting = true;

	return aut;
}

// ── Algorithm D: ltlsynt → parity game ───────────────────────────────────────
//
// Declared in algorithm_d_game.h and defined here so it can use the same
// tempfile + posix_spawn mechanism `call_ltlsynt` uses (LS-10).
//
// It used to be popen + an inline `--formula="…"` with hand-rolled escaping of
// only `" \ $ \``.  That is the exact pattern `call_ltlsynt` retired: a single
// argument is capped at the Linux MAX_ARG_STRLEN of 131072, so a grown φ*
// (Algorithm B with many constants, or the semantic-PWR fallback) hits E2BIG
// and comes back as an empty game — which every caller reads as
// "unrealizable".  `-F path` has no such cap and needs no escaping at all.

namespace alg_d {

inline SynthGame call_ltlsynt_game(
	const std::string& phi_prop,
	const std::vector<std::string>& ins,
	const std::vector<std::string>& outs)
{
	// Cache: avoid re-running ltlsynt on identical (formula, ins, outs).
	struct game_cache_key {
		std::string formula;
		std::vector<std::string> ins, outs;
		bool operator==(const game_cache_key& o) const {
			return formula == o.formula && ins == o.ins && outs == o.outs;
		}
	};
	struct game_cache_hash {
		size_t operator()(const game_cache_key& k) const {
			size_t h = std::hash<std::string>{}(k.formula);
			for (auto& s : k.ins)  h ^= std::hash<std::string>{}(s) + 0x9e3779b9 + (h << 6) + (h >> 2);
			for (auto& s : k.outs) h ^= std::hash<std::string>{}(s) + 0x517cc1b7 + (h << 6) + (h >> 2);
			return h;
		}
	};
	static std::unordered_map<game_cache_key, SynthGame, game_cache_hash> cache;
	game_cache_key key{phi_prop, ins, outs};
	if (auto it = cache.find(key); it != cache.end()) return it->second;

	// Configurable timeout (same env var as call_ltlsynt).
	int timeout_sec = ltl_timeout_sec();

	std::string tmpfile_path = write_tempfile("tau_lang_game", phi_prop + "\n");
	if (tmpfile_path.empty()) {
		LOG_ERROR << "[ltl_aba] failed to write temp file for ltlsynt input\n";
		return {};  // transient — don't cache
	}

	std::vector<std::string> argv = {
	    "ltlsynt", "-F", tmpfile_path, "--print-game-hoa"};
	auto csv = [](const std::vector<std::string>& v) {
		std::string r;
		for (size_t i = 0; i < v.size(); ++i) { if (i) r += ","; r += v[i]; }
		return r;
	};
	if (!ins.empty())  argv.push_back("--ins="  + csv(ins));
	if (!outs.empty()) argv.push_back("--outs=" + csv(outs));

	auto [hoa, exit_code] = spawn_capture(argv, timeout_sec);
	std::remove(tmpfile_path.c_str());

	// SY-R1: a timeout, a missing binary or a usage error used to come
	// back as the EMPTY game, which every caller (Algorithm D, the
	// semantic-PWR fallback) reads as a definitive UNREALIZABLE. Classify
	// like call_ltlsynt and throw: no verdict is not a verdict. Nothing
	// transient is cached.
	switch (classify_spot_exit(exit_code, hoa)) {
	case spot_exit_kind::not_found:
		LOG_ERROR << "[ltl_aba] ltlsynt not found on PATH. "
		             "Install Spot (>= 2.10) and ensure ltlsynt is on PATH.\n";
		throw ltl_synthesis_error("ltlsynt not found on PATH; install "
			"Spot (>= 2.10) -- the parity game could not be built");
	case spot_exit_kind::failed: {
		std::string msg = "ltlsynt --print-game-hoa produced no game "
			"(exit " + std::to_string(exit_code) + ")";
		if (exit_code == 143)
			msg += " — killed by the TAU_LTL_TIMEOUT_SEC watchdog ("
			     + std::to_string(timeout_sec) + "s)";
		LOG_ERROR << "[ltl_aba] " << msg << "\n";
		throw ltl_synthesis_error(msg);
	}
	case spot_exit_kind::ok:
		break;
	}
	auto result = parse_synth_game_hoa(hoa);
	cache[key] = result;
	return result;
}

} // namespace alg_d

// (SY-R2: the "Algorithm D Phase 1" pair call_ltl2tgba_dpa / parse_dpa_hoa
// was deleted -- zero production callers, superseded by the game-HOA path in
// alg_d::call_ltlsynt_game / parse_synth_game_hoa, and it had missed the
// LT-10 hardening.)

} // namespace idni::tau_lang
