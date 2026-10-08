// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// The forked child runner; see runner.h. The pipe carries one line: `A value
// binders ms` for an answer, `M` for an exhausted heap, `E reason` for an
// exception.

#include "runner.h"

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <new>
#include <sstream>

#include <poll.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

namespace bv_bench {

namespace {

using steady = std::chrono::steady_clock;

// The child's end of the pipe; the new handler takes no argument.
int answer_fd = -1;

void write_all(int fd, const std::string& s) {
	const char* p = s.data();
	size_t left = s.size();
	while (left > 0) {
		const ssize_t n = write(fd, p, left);
		if (n < 0) {
			if (errno == EINTR) continue;
			return;
		}
		p += n;
		left -= static_cast<size_t>(n);
	}
}

// Called with the heap exhausted, so it must not allocate.
void out_of_memory() {
	static const char line[] = "M\n";
	[[maybe_unused]] const ssize_t n = write(answer_fd, line, sizeof line - 1);
	_exit(0);
}

[[noreturn]] void answer_and_exit(const std::function<verdict()>& question) {
	std::string line;
	try {
		const verdict v = question();
		std::ostringstream os;
		os << "A " << v.value << ' ' << v.binders << ' ' << v.ms << '\n';
		line = os.str();
	} catch (const std::bad_alloc&) {
		out_of_memory();
	} catch (const std::exception& e) {
		line = std::string("E ") + e.what() + '\n';
	} catch (...) {
		line = "E unknown exception\n";
	}
	write_all(answer_fd, line);
	// _exit: no static destructors and no flush of buffers the parent also holds.
	_exit(0);
}

child_run error(const std::string& reason, double ms) {
	return { child_run::end::error, {}, reason, ms };
}

} // namespace

double ms_since(steady::time_point start) {
	return std::chrono::duration<double, std::milli>(
		steady::now() - start).count();
}

child_run run_in_child(const limits& lim,
	const std::function<verdict()>& question)
{
	int fds[2];
	if (pipe(fds) != 0)
		return error(std::string("pipe: ") + std::strerror(errno), 0);
	std::cout.flush();
	std::cerr.flush();
	std::fflush(nullptr);
	const steady::time_point start = steady::now();
	const pid_t pid = fork();
	if (pid < 0) {
		close(fds[0]);
		close(fds[1]);
		return error(std::string("fork: ") + std::strerror(errno), 0);
	}
	if (pid == 0) {
		close(fds[0]);
		answer_fd = fds[1];
		dup2(STDERR_FILENO, STDOUT_FILENO);
		const rlim_t cap = static_cast<rlim_t>(lim.mem_mb) << 20;
		const rlimit as{ cap, cap };
		setrlimit(RLIMIT_AS, &as);
		std::set_new_handler(out_of_memory);
		answer_and_exit(question);
	}
	close(fds[1]);

	std::string buf;
	std::string failure;
	bool timed_out = false;
	char chunk[256];
	for (;;) {
		const double left = lim.timeout_s * 1000 - ms_since(start);
		if (left <= 0) { timed_out = true; break; }
		pollfd pfd{ fds[0], POLLIN, 0 };
		const int ready = poll(&pfd, 1,
			static_cast<int>(std::ceil(std::min(left, 1e9))));
		if (ready == 0) continue;
		if (ready > 0) {
			const ssize_t n = read(fds[0], chunk, sizeof chunk);
			if (n > 0) { buf.append(chunk, static_cast<size_t>(n)); continue; }
			if (n == 0) break;
		}
		if (errno == EINTR) continue;
		failure = std::strerror(errno);
		break;
	}
	close(fds[0]);
	if (timed_out || !failure.empty()) kill(pid, SIGKILL);
	int status = 0;
	while (waitpid(pid, &status, 0) < 0 && errno == EINTR) {}
	const double ms = ms_since(start);

	if (timed_out) return { child_run::end::timeout, {}, {}, ms };
	if (!failure.empty()) return error("pipe read: " + failure, ms);
	if (buf.starts_with("A ")) {
		std::istringstream is(buf.substr(2));
		verdict v;
		if (is >> v.value >> v.binders >> v.ms)
			return { child_run::end::answered, v, {}, ms };
	}
	if (buf == "M\n") return { child_run::end::out_of_memory, {}, {}, ms };
	if (buf.starts_with("E ")) {
		std::string reason = buf.substr(2);
		if (reason.ends_with('\n')) reason.pop_back();
		return error(reason, ms);
	}
	if (WIFSIGNALED(status))
		return error(std::string("signal ") + std::to_string(WTERMSIG(status))
			+ " (" + strsignal(WTERMSIG(status)) + ")", ms);
	return error("no answer, exit status "
		+ std::to_string(WEXITSTATUS(status)), ms);
}

} // namespace bv_bench
