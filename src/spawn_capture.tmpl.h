// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "spawn_capture.h" // Only for IDE resolution, not really needed.

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "spawn_capture"

#include <algorithm>
#include <atomic>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <thread>

#if defined(__EMSCRIPTEN__)
// no process model
#elif defined(_WIN32) && !defined(__CYGWIN__)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <fcntl.h>
#include <signal.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
extern char** environ;
#endif

#include "self_exe_path.h"

namespace idni::tau_lang {

// A package installs the Spot tools in <prefix>/libexec/tau/spot. tau is in
// <prefix>/bin, or at the root of a macOS package.
inline std::vector<std::string> spot_package_dirs() {
	std::string self = self_exe_path();
	if (self.empty()) return {};
	namespace fs = std::filesystem;
	fs::path exe_dir = fs::path(self).parent_path();
	return { (exe_dir / ".." / "libexec" / "tau" / "spot").lexically_normal().string(),
		(exe_dir / "libexec" / "tau" / "spot").string() };
}

// A report text label needs text: the parser's report cannot hold an empty
// string under one, so a silent child says so instead.
inline std::string spawn_output_text(const std::string& out) {
	return out.empty() ? std::string("(no output)")
		: truncate_for_message(out);
}

// ── spawn_capture ────────────────────────────────────────────────────────

#if defined(__EMSCRIPTEN__)

inline result<std::string> spawn_capture(const std::vector<std::string>& argv,
	int, std::function<bool(int)>, const spawn_options&)
{
	result<std::string> r;
	// the same refusal as the POSIX path; an empty name would also be an
	// attr value the report cannot hold
	if (argv.empty())
		return r.with_error(code::invalid_argument,
			"spawn_capture requires a non-empty argv");
	// no process model under wasm; matches the not-on-PATH contract
	return r.with_error(code::not_found,
		"no process model is available under this build",
		{{label::name, argv[0]}});
}

#elif defined(_WIN32) && !defined(__CYGWIN__)

// @p arg quoted for a CreateProcess command line (the MSVC runtime's argv
// rules: backslashes doubled only before a quote or the closing quote);
// unchanged when it has no blank and no quote.
inline std::string win_quote_arg(const std::string& arg) {
	if (arg.find_first_of(" \t\n\v\"") == std::string::npos)
		return arg;
	std::string out = "\"";
	for (size_t i = 0; i < arg.size(); ++i) {
		if (arg[i] == '\\') {
			size_t n = 0;
			while (i + n < arg.size() && arg[i + n] == '\\')
				++n;
			if (i + n == arg.size() || arg[i + n] == '"')
				out.append(n * 2, '\\');
			else out.append(n, '\\');
			i += n - 1;
		} else if (arg[i] == '"') out += "\\\"";
		else out += arg[i];
	}
	return out + '"';
}

// PATH first, then TAU_SPOT_BIN (the store package's bin dir, set by
// configure), then the Spot folder of a package. Never link Spot; only exec
// ltlsynt/autfilt/ltlfilt. SearchPathA with a null base searches the current
// directory before PATH, so the walk is explicit: an ltlsynt.exe dropped
// beside tau must not be picked up. A @p name with a path separator is
// taken as is. Writes the full path into @p exe (size @p exe_sz) and returns
// true when found; false when not found or the path does not fit.
inline bool win_find_exe(const std::string& name, char* exe, DWORD exe_sz) {
	if (name.find_first_of("\\/") != std::string::npos) {
		if (GetFileAttributesA(name.c_str()) == INVALID_FILE_ATTRIBUTES)
			return false;
		if (name.size() >= exe_sz) return false;
		memcpy(exe, name.c_str(), name.size() + 1);
		return true;
	}
	bool ends_exe = name.size() >= 4
		&& name.compare(name.size() - 4, 4, ".exe") == 0;
	auto search_in = [&](const std::string& dir) -> bool {
		if (dir.empty()) return false;
		std::string full = dir;
		if (full.back() != '\\' && full.back() != '/')
			full += '\\';
		full += name;
		if (!ends_exe) full += ".exe";
		if (GetFileAttributesA(full.c_str()) == INVALID_FILE_ATTRIBUTES)
			return false;
		if (full.size() >= exe_sz) return false;
		memcpy(exe, full.c_str(), full.size() + 1);
		return true;
	};
	auto env_var = [](const char* var, std::string& out) -> bool {
		DWORD need = GetEnvironmentVariableA(var, nullptr, 0);
		if (need == 0) return false;
		std::vector<char> buf(need);
		if (!GetEnvironmentVariableA(var, buf.data(), need)) return false;
		out.assign(buf.data(), need - 1);
		return true;
	};
	std::string path;
	if (env_var("PATH", path)) {
		size_t start = 0;
		for (;;) {
			size_t sep = path.find(';', start);
			std::string dir = path.substr(start,
				sep == std::string::npos ? std::string::npos
					: sep - start);
			if (search_in(dir)) return true;
			if (sep == std::string::npos) break;
			start = sep + 1;
		}
	}
	std::string spot_bin;
	if (env_var("TAU_SPOT_BIN", spot_bin) && search_in(spot_bin)) return true;
	for (const auto& dir : spot_package_dirs())
		if (search_in(dir)) return true;
	return false;
}

inline result<std::string> spawn_capture(const std::vector<std::string>& argv,
	int timeout_sec, std::function<bool(int)> exit_ok,
	const spawn_options& opts)
{
	result<std::string> r;

	if (argv.empty())
		return r.with_error(code::invalid_argument,
			"spawn_capture requires a non-empty argv");

	char exe[MAX_PATH];
	if (!win_find_exe(argv[0], exe, MAX_PATH))
		return r.with_error(code::not_found,
			"the command was not found on PATH",
			{{label::name, argv[0]}});

	std::string cmdline = win_quote_arg(exe);
	for (size_t i = 1; i < argv.size(); ++i) {
		cmdline += ' ';
		cmdline += win_quote_arg(argv[i]);
	}

	SECURITY_ATTRIBUTES sa{};
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	HANDLE rd = nullptr, wr = nullptr;
	if (!CreatePipe(&rd, &wr, &sa, 0))
		return r.with_error(code::io_error, "failed to prepare the "
			"subprocess pipe", {{label::name, "CreatePipe"}});
	SetHandleInformation(rd, HANDLE_FLAG_INHERIT, 0);

	HANDLE in_file = INVALID_HANDLE_VALUE, err_file = INVALID_HANDLE_VALUE;
	if (!opts.stdin_path.empty()) {
		in_file = CreateFileA(opts.stdin_path.c_str(), GENERIC_READ,
			FILE_SHARE_READ, &sa, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, nullptr);
		if (in_file == INVALID_HANDLE_VALUE) {
			CloseHandle(rd); CloseHandle(wr);
			return r.with_error(code::io_error,
				"failed to open the subprocess stdin file",
				{{label::name, "CreateFileA"},
				 {label::value, opts.stdin_path}});
		}
	}
	if (!opts.stderr_path.empty()) {
		err_file = CreateFileA(opts.stderr_path.c_str(), GENERIC_WRITE,
			FILE_SHARE_READ, &sa, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, nullptr);
		if (err_file == INVALID_HANDLE_VALUE) {
			CloseHandle(rd); CloseHandle(wr);
			if (in_file != INVALID_HANDLE_VALUE) CloseHandle(in_file);
			return r.with_error(code::io_error,
				"failed to open the subprocess stderr file",
				{{label::name, "CreateFileA"},
				 {label::value, opts.stderr_path}});
		}
	}

	HANDLE nul = CreateFileA("NUL", GENERIC_WRITE, FILE_SHARE_WRITE,
		&sa, OPEN_EXISTING, 0, nullptr);

	STARTUPINFOA si{};
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = in_file != INVALID_HANDLE_VALUE
		? in_file : GetStdHandle(STD_INPUT_HANDLE);
	si.hStdOutput = wr;
	if (err_file != INVALID_HANDLE_VALUE) si.hStdError = err_file;
	else if (opts.merge_stderr) si.hStdError = wr;
	else si.hStdError = (nul != INVALID_HANDLE_VALUE) ? nul : wr;

	PROCESS_INFORMATION pi{};
	std::vector<char> cl(cmdline.begin(), cmdline.end());
	cl.push_back('\0');
	BOOL ok = CreateProcessA(exe, cl.data(), nullptr, nullptr, TRUE,
		CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi);
	CloseHandle(wr);
	if (in_file != INVALID_HANDLE_VALUE) CloseHandle(in_file);
	if (err_file != INVALID_HANDLE_VALUE) CloseHandle(err_file);
	if (nul != INVALID_HANDLE_VALUE) CloseHandle(nul);
	if (!ok) {
		CloseHandle(rd);
		return r.with_error(code::io_error, "failed to spawn the command",
			{{label::name, argv[0]}});
	}
	CloseHandle(pi.hThread);

	std::atomic<bool> done{false};
	std::atomic<bool> watchdog_fired{false};
	std::thread killer;
	if (timeout_sec > 0) {
		HANDLE proc = pi.hProcess;
		killer = std::thread([proc, timeout_sec, &done, &watchdog_fired]() {
			const long long polls = 10LL * timeout_sec;
			for (long long i = 0; i < polls && !done.load(); ++i)
				Sleep(100);
			if (!done.load() && TerminateProcess(proc, 128 + 15))
				watchdog_fired.store(true);
		});
	}

	std::string out;
	char buf[4096];
	DWORD got = 0;
	for (;;) {
		if (!ReadFile(rd, buf, sizeof(buf), &got, nullptr) || got == 0)
			break;
		out.append(buf, buf + got);
	}
	CloseHandle(rd);

	done.store(true);
	if (killer.joinable()) killer.join();
	WaitForSingleObject(pi.hProcess, INFINITE);
	DWORD ec = 0;
	GetExitCodeProcess(pi.hProcess, &ec);
	CloseHandle(pi.hProcess);
	// MinGW Spot writes text-mode CRLF; hoa.tgf and line parsers want LF.
	out.erase(std::remove(out.begin(), out.end(), '\r'), out.end());
	LOG_DEBUG << "[spot] " << argv[0] << " exited, status=" << ec
		<< ", stdout=" << out;

	// the flag, not the code: a child may exit 143 on its own
	int exit_code = (int)ec;
	if (watchdog_fired.load())
		return r.with_error(code::runtime_error,
			"the command was killed by the timeout watchdog",
			{{label::exit_code, exit_code},
			 {label::timeout, timeout_sec},
			 {label::value, spawn_output_text(out)}});
	if (!exit_ok(exit_code))
		return r.with_error(code::runtime_error,
			"the command exited with an unexpected code",
			{{label::exit_code, exit_code},
			 {label::value, spawn_output_text(out)}});
	return r.with_value(std::move(out));
}

#else // POSIX

inline result<std::string> spawn_capture(const std::vector<std::string>& argv,
	int timeout_sec, std::function<bool(int)> exit_ok,
	const spawn_options& opts)
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
	// Child: stdout -> pipe write end; stderr -> the named file, the stdout
	// pipe (merged), or /dev/null; stdin -> the named file.
	posix_spawn_file_actions_addclose(&fa, pipefd[0]);
	posix_spawn_file_actions_adddup2 (&fa, pipefd[1], STDOUT_FILENO);
	if (!opts.stderr_path.empty()) {
		posix_spawn_file_actions_addopen(&fa, STDERR_FILENO,
			opts.stderr_path.c_str(),
			O_WRONLY | O_CREAT | O_TRUNC, 0644);
	} else if (opts.merge_stderr) {
		posix_spawn_file_actions_adddup2 (&fa, pipefd[1], STDERR_FILENO);
	} else {
		posix_spawn_file_actions_addopen (&fa, STDERR_FILENO, "/dev/null",
		                                  O_WRONLY, 0);
	}
	posix_spawn_file_actions_addclose(&fa, pipefd[1]);
	if (!opts.stdin_path.empty())
		posix_spawn_file_actions_addopen(&fa, STDIN_FILENO,
			opts.stdin_path.c_str(), O_RDONLY, 0);

	std::vector<char*> cargv;
	cargv.reserve(argv.size() + 1);
	for (const auto& s : argv) cargv.push_back(const_cast<char*>(s.c_str()));
	cargv.push_back(nullptr);

	// The child leads its own process group, so the watchdog reaches the
	// processes it starts too: one of them left alive would hold the pipe
	// open and the read below would wait for it.
	posix_spawnattr_t attr;
	if (posix_spawnattr_init(&attr) != 0) {
		posix_spawn_file_actions_destroy(&fa);
		::close(pipefd[0]); ::close(pipefd[1]);
		return r.with_error(code::io_error, "failed to prepare the "
			"subprocess", {{label::name, "posix_spawnattr_init"}});
	}
	posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETPGROUP);
	posix_spawnattr_setpgroup(&attr, 0);
	pid_t pid;
	int rc = posix_spawnp(&pid, cargv[0], &fa, &attr, cargv.data(), environ);
	if (rc == ENOENT && argv[0].find('/') == std::string::npos) {
		// posix_spawnp reads PATH only; the store package's bin dir is
		// named by TAU_SPOT_BIN, which configure exports, and a package
		// holds the tools in its own Spot folder.
		std::vector<std::string> dirs;
		if (const char* bin = ::getenv("TAU_SPOT_BIN"); bin && *bin)
			dirs.emplace_back(bin);
		for (auto& dir : spot_package_dirs()) dirs.push_back(std::move(dir));
		std::string full;
		for (const auto& dir : dirs) {
			full = dir + "/" + argv[0];
			if (::access(full.c_str(), X_OK) != 0) continue;
			cargv[0] = const_cast<char*>(full.c_str());
			rc = posix_spawnp(&pid, cargv[0], &fa, &attr,
				cargv.data(), environ);
			break;
		}
	}
	posix_spawnattr_destroy(&attr);
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

	// Watchdog thread: after timeout_sec seconds SIGTERM the child's
	// process group, and SIGKILL it when it is still running 2 s later
	// (a child may ignore SIGTERM). The group stays valid until the child
	// is reaped, which happens only after the watchdog stops.
	std::atomic<bool> done{false}, fired{false};
	std::thread killer;
	if (timeout_sec > 0) {
		killer = std::thread([pid, timeout_sec, &done, &fired]() {
			auto wait = [&done](long long polls) {
				for (long long i = 0; i < polls && !done.load(); ++i)
					::usleep(100'000);
				return !done.load();
			};
			if (!wait(10LL * timeout_sec)) return;
			fired.store(true);
			::kill(-pid, SIGTERM);
			if (wait(20)) ::kill(-pid, SIGKILL);
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

	if (fired.load())
		return r.with_error(code::runtime_error,
			"the command was killed by the timeout watchdog",
			{{label::exit_code, WIFSIGNALED(status)
				? 128 + WTERMSIG(status) : WEXITSTATUS(status)},
			 {label::timeout, timeout_sec},
			 {label::value, spawn_output_text(out)}});
	if (WIFSIGNALED(status)) {
		int exit_code = 128 + WTERMSIG(status);
		return r.with_error(code::runtime_error,
			"the command was killed by a signal",
			{{label::exit_code, exit_code},
			 {label::value, spawn_output_text(out)}});
	}
	int exit_code = WEXITSTATUS(status);
	if (!exit_ok(exit_code))
		return r.with_error(code::runtime_error,
			"the command exited with an unexpected code",
			{{label::exit_code, exit_code},
			 {label::value, spawn_output_text(out)}});
	return r.with_value(std::move(out));
}

#endif // spawn_capture platforms

} // namespace idni::tau_lang
