// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "backends/spot/spot.h" // Only for IDE resolution, not really needed.

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "spot"

#include <array>
#include <atomic>
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <optional>
#include <thread>

#ifndef __EMSCRIPTEN__
#include <fcntl.h>
#include <signal.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
extern char** environ;
#endif

#include "utility/temp_file.h"

namespace idni::tau_lang {

// ── argv / CSV helpers ───────────────────────────────────────────────────

// ltlsynt's --ins=/--outs= take a single comma-joined argument; this is the
// only place in the backend that needs it.
inline std::string csv_join(const std::vector<std::string>& v) {
	std::string s;
	for (size_t i = 0; i < v.size(); ++i) { if (i) s += ","; s += v[i]; }
	return s;
}

// A verdict line is "REALIZABLE\n<hoa>" or "UNREALIZABLE\n", anywhere else
// there is no verdict. Used only by `synthesize`.
struct verdict_line { bool realizable; std::string body; };

inline std::optional<verdict_line> parse_verdict_line(const std::string& out) {
	auto body = [&] {
		auto nl = out.find('\n');
		return nl == std::string::npos ? std::string() : out.substr(nl + 1);
	};
	if (out.compare(0, 12, "UNREALIZABLE") == 0)
		return verdict_line{false, body()};
	if (out.compare(0, 10, "REALIZABLE") == 0)
		return verdict_line{true, body()};
	return std::nullopt;
}

// ── spawn_capture ────────────────────────────────────────────────────────

#ifdef __EMSCRIPTEN__

inline result<std::string> spawn_capture(const std::vector<std::string>& argv,
	int, std::function<bool(int)>)
{
	result<std::string> r;
	// no process model under wasm; matches the not-on-PATH contract
	return r.with_error(code::not_found,
		"no process model is available under this build",
		{{label::name, argv.empty() ? std::string() : argv[0]}});
}

#else // POSIX

inline result<std::string> spawn_capture(const std::vector<std::string>& argv,
	int timeout_sec, std::function<bool(int)> exit_ok)
{
	result<std::string> r;

	if (argv.empty())
		return r.with_error(code::invalid_argument,
			"spawn_capture requires a non-empty argv");

	int pipefd[2];
	if (::pipe(pipefd) != 0) {
		return r.with_error(code::io_error, "failed to prepare the "
			"subprocess pipe", {{label::name, "pipe"}});
	}

	posix_spawn_file_actions_t fa;
	if (posix_spawn_file_actions_init(&fa) != 0) {
		::close(pipefd[0]); ::close(pipefd[1]);
		return r.with_error(code::io_error, "failed to prepare the "
			"subprocess",
			{{label::name, "posix_spawn_file_actions_init"}});
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
		if (rc == ENOENT)
			return r.with_error(code::not_found,
				"the command was not found on PATH",
				{{label::name, argv[0]}});
		return r.with_error(code::io_error, "failed to spawn the command",
			{{label::name, argv[0]}});
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

	// Stop the watchdog BEFORE the child is reaped: after waitpid returns
	// the pid may already belong to another process, and a poll firing in
	// that window would signal it instead. The pipe is drained, so the
	// child has already closed its stdout and is exiting either way.
	done.store(true);
	if (killer.joinable()) killer.join();
	int status = 0, wp;
	do { wp = ::waitpid(pid, &status, 0); } while (wp < 0 && errno == EINTR);
	if (wp < 0) {
		return r.with_error(code::io_error,
			"failed to wait for the subprocess",
			{{label::name, "waitpid"}});
	}

	LOG_DEBUG << "[spot] " << argv[0] << " exited, status=" << status
		<< ", stdout=" << out;

	if (WIFSIGNALED(status)) {
		int exit_code = 128 + WTERMSIG(status);
		if (exit_code == 143)
			return r.with_error(code::runtime_error,
				"the command was killed by the timeout watchdog",
				{{label::exit_code, exit_code},
				 {label::timeout, timeout_sec}});
		return r.with_error(code::runtime_error,
			"the command was killed by a signal",
			{{label::exit_code, exit_code}});
	}
	int exit_code = WEXITSTATUS(status);
	if (!exit_ok(exit_code))
		return r.with_error(code::runtime_error,
			"the command exited with an unexpected code",
			{{label::exit_code, exit_code}});
	return r.with_value(std::move(out));
}

#endif // __EMSCRIPTEN__

// ── public surface ───────────────────────────────────────────────────────

inline result<synthesis_verdict> synthesize(const std::string& formula,
	const std::vector<std::string>& ins,
	const std::vector<std::string>& outs, int timeout_sec)
{
	result<synthesis_verdict> r;

	// -F path avoids both shell-escaping and the Linux MAX_ARG_STRLEN
	// limit (131072) that an inline --formula="..." argument hits once a
	// formula grows (Algorithm B with many constants).
	TAU_TRY(auto tmp, fs::temp_file::create("tau_lang", formula + "\n"));

	std::vector<std::string> argv = {"ltlsynt", "-F", tmp.path()};
	std::string ins_str = csv_join(ins), outs_str = csv_join(outs);
	if (!ins_str.empty())  argv.push_back("--ins="  + ins_str);
	if (!outs_str.empty()) argv.push_back("--outs=" + outs_str);
	if (const char* simp = std::getenv("TAU_LTL_SIMPLIFICATION"); simp && *simp)
		argv.push_back("--simplification=" + std::string(simp));

	auto exit_ok = [](int c) { return c == 0 || c == 1; };
	auto spawned = spawn_capture(argv, timeout_sec, exit_ok);
	if (!spawned.has_value()) {
		// spawn_capture decided the process-level fact (not found, killed,
		// exited oddly) and recorded it; this adds what that means for a
		// realizability request, on top of the merged child report.
		bool not_found = report_has_code(spawned.report(), code::not_found);
		bool timed_out = report_has_attr(spawned.report(), label::timeout);
		r.merge(std::move(spawned));
		if (not_found)
			return r.with_error(code::solver_error,
				"ltlsynt not found on PATH; install Spot (>= 2.10) -- "
				"realizability is UNKNOWN");
		if (timed_out)
			return r.with_error(code::solver_error,
				"ltlsynt produced no verdict, killed by the ltl-timeout "
				"watchdog (--ltl-timeout / `set ltltimeout` / "
				"TAU_LTL_TIMEOUT_SEC); the realizability of this "
				"specification is UNKNOWN");
		return r.with_error(code::solver_error,
			"ltlsynt produced no verdict; the realizability of this "
			"specification is UNKNOWN");
	}

	auto verdict = parse_verdict_line(spawned.value());
	if (!verdict)
		return r.with_error(code::solver_error,
			"ltlsynt output carried no verdict line");
	return r.with_value(synthesis_verdict{verdict->realizable,
		verdict->realizable ? std::move(verdict->body) : std::string()});
}

inline result<std::string> synthesize_game(const std::string& formula,
	const std::vector<std::string>& ins,
	const std::vector<std::string>& outs, int timeout_sec)
{
	result<std::string> r;

	TAU_TRY(auto tmp, fs::temp_file::create("tau_lang_game", formula + "\n"));

	// --polarity=no: keep the genuine arena (every sys alternative, real
	// acceptance) instead of ltlsynt's constant-output-substituted one, so
	// a re-solve under a different feasibility notion is sound (ALG-D-28).
	// --decompose=no: a decomposed spec prints one game per part; the
	// product game needs a single game carrying every atom.
	std::vector<std::string> argv = {
	    "ltlsynt", "-F", tmp.path(),
	    "--polarity=no", "--decompose=no", "--print-game-hoa"};
	std::string ins_str = csv_join(ins), outs_str = csv_join(outs);
	if (!ins_str.empty())  argv.push_back("--ins="  + ins_str);
	if (!outs_str.empty()) argv.push_back("--outs=" + outs_str);

	auto exit_ok = [](int c) { return c == 0 || c == 1; };
	auto spawned = spawn_capture(argv, timeout_sec, exit_ok);
	if (!spawned.has_value()) {
		bool not_found = report_has_code(spawned.report(), code::not_found);
		bool timed_out = report_has_attr(spawned.report(), label::timeout);
		r.merge(std::move(spawned));
		if (not_found)
			return r.with_error(code::solver_error,
				"ltlsynt not found on PATH; install Spot (>= 2.10) -- "
				"the parity game could not be built");
		if (timed_out)
			return r.with_error(code::solver_error,
				"ltlsynt --print-game-hoa produced no game, killed by "
				"the ltl-timeout watchdog (--ltl-timeout / "
				"`set ltltimeout` / TAU_LTL_TIMEOUT_SEC)");
		return r.with_error(code::solver_error,
			"ltlsynt --print-game-hoa produced no game");
	}
	return r.with_value(std::move(spawned.value()));
}

inline result<std::string> to_dot(const std::string& hoa_text, int timeout_sec) {
	result<std::string> r;
	TAU_TRY(auto tmp, fs::temp_file::create("tau_strategy", hoa_text));
	TAU_TRY(auto dot, spawn_capture({"autfilt", "--dot", tmp.path()},
		timeout_sec, [](int c) { return c == 0; }));
	return r.with_value(std::move(dot));
}

inline result<bool> is_tautology(const std::string& formula, int timeout_sec) {
	result<bool> r;
	TAU_TRY(auto out, spawn_capture({"ltlfilt", "-f", formula}, timeout_sec,
		[](int c) { return c == 0; }));
	while (!out.empty() && std::isspace((unsigned char) out.back()))
		out.pop_back();
	return r.with_value(out == "1");
}

inline bool available() {
	static const bool avail = [] {
		auto probe = spawn_capture({"ltlsynt", "--version"}, 0,
			[](int) { return true; });
		return probe.has_value() ||
			!report_has_code(probe.report(), code::not_found);
	}();
	return avail;
}

} // namespace idni::tau_lang
