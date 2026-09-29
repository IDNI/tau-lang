// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file bounded_call.h
 * @brief Work that may not return, run in a child process under a
 * wall-clock bound.
 *
 * A solver that has no interrupt and whose own resource limits are unsound
 * can still be bounded from outside: the process forks, the child does the
 * work and writes one byte of answer to a pipe, and the parent kills the
 * child when the bound passes. The child is a copy of the process, so the
 * work reads the parent's state as it stood at the fork and nothing it does
 * reaches the parent.
 *
 * A bound that passes gives no answer. The owner of the work says so with
 * @ref note_time_budget_exhausted, naming the budget, and the boundary of the
 * unit of work (`with_budget`, the REPL) turns the whole unit into an
 * UNKNOWN error: whatever read the missing answer computed nothing to trust.
 * A caller that reads a missing answer as "undecided" and nothing else can
 * keep its result with @ref time_budget_handled.
 *
 * Without a process model (wasm, Windows) the work runs in the process,
 * unbounded.
 */

#ifndef __IDNI__TAU__BOUNDED_CALL_H__
#define __IDNI__TAU__BOUNDED_CALL_H__

#include <chrono>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>

#if !defined(__EMSCRIPTEN__) && !defined(_WIN32)
#define TAU_HAS_BOUNDED_CALL 1
#include <cerrno>
#include <csignal>
#include <poll.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#ifdef __linux__
#include <sys/prctl.h>
#endif
#endif

namespace idni::tau_lang {

struct bounded_outcome {
	enum kind { done, timed_out, failed } status = failed;
	uint8_t value = 0;
	double seconds = 0;
};

/// Whether @ref run_bounded can bound the work it runs.
constexpr bool bounded_calls_available() {
#ifdef TAU_HAS_BOUNDED_CALL
	return true;
#else
	return false;
#endif
}

/**
 * @brief Runs @p work in a child process and waits at most @p timeout_ms
 * milliseconds for its answer.
 *
 * `done` carries the answer; `timed_out` means the child was killed at the
 * bound; `failed` means no child could be made or it died without
 * answering. With no process model, @p work runs in the process.
 */
inline bounded_outcome run_bounded(const std::function<uint8_t()>& work,
	uint64_t timeout_ms)
{
	using clock = std::chrono::steady_clock;
	const auto start = clock::now();
	bounded_outcome out;
	auto finish = [&](bounded_outcome::kind k) {
		out.status = k;
		out.seconds = std::chrono::duration<double>(
			clock::now() - start).count();
		return out;
	};
#ifdef TAU_HAS_BOUNDED_CALL
	int fd[2];
	if (::pipe(fd) != 0) return finish(bounded_outcome::failed);
	const pid_t parent = ::getpid();
	const auto deadline = start + std::chrono::milliseconds(timeout_ms);
	const pid_t pid = ::fork();
	if (pid < 0) {
		::close(fd[0]); ::close(fd[1]);
		return finish(bounded_outcome::failed);
	}
	if (pid == 0) {
		// The child leaves through _exit: no destructor, atexit handler or
		// stream flush of the parent's state runs twice. It must not
		// outlive the parent, killed without a chance to kill it, nor
		// its own bound.
#ifdef __linux__
		::prctl(PR_SET_PDEATHSIG, SIGKILL);
#endif
		std::thread([parent, deadline] {
			for (;;) {
				if (::getppid() != parent || clock::now() > deadline
					+ std::chrono::seconds(1)) ::_exit(1);
				std::this_thread::sleep_for(
					std::chrono::milliseconds(100));
			}
		}).detach();
		::close(fd[0]);
		const uint8_t v = work();
		ssize_t w;
		do w = ::write(fd[1], &v, 1); while (w < 0 && errno == EINTR);
		::_exit(w == 1 ? 0 : 1);
	}
	::close(fd[1]);
	bool answered = false, timed_out = false;
	for (;;) {
		// rounded up, so the child is never killed before the bound
		const auto left = std::chrono::ceil<std::chrono::milliseconds>(
			deadline - clock::now()).count();
		if (left <= 0) { timed_out = true; break; }
		pollfd p{ fd[0], POLLIN, 0 };
		const int n = ::poll(&p, 1, left > INT32_MAX ? INT32_MAX : (int)left);
		if (n < 0 && errno == EINTR) continue;
		if (n <= 0) { timed_out = n == 0; break; }
		ssize_t r;
		do r = ::read(fd[0], &out.value, 1); while (r < 0 && errno == EINTR);
		answered = r == 1;
		break;
	}
	::close(fd[0]);
	if (!answered) ::kill(pid, SIGKILL);
	int st = 0;
	while (::waitpid(pid, &st, 0) < 0 && errno == EINTR) {}
	if (answered) return finish(bounded_outcome::done);
	return finish(timed_out ? bounded_outcome::timed_out
		: bounded_outcome::failed);
#else
	(void)timeout_ms;
	out.value = work();
	return finish(bounded_outcome::done);
#endif
}

/// The message of the last budget that ran out in the current unit of work;
/// empty when none did.
inline std::string& time_budget_exhausted() {
	static std::string message;
	return message;
}

/// Records that work ran past its time budget, described by @p message.
inline void note_time_budget_exhausted(std::string message) {
	if (time_budget_exhausted().empty())
		time_budget_exhausted() = std::move(message);
}

/// Returns and clears the message of @ref time_budget_exhausted.
inline std::string take_time_budget_exhausted() {
	std::string m;
	m.swap(time_budget_exhausted());
	return m;
}

/// While set, the bounded questions asked share one budget, counted from
/// this time instead of from each question's start.
inline std::optional<std::chrono::steady_clock::time_point>&
	shared_budget_start()
{
	static std::optional<std::chrono::steady_clock::time_point> start;
	return start;
}

/// When a question with a budget of @p budget, asked now, runs out.
inline std::chrono::steady_clock::time_point budget_deadline(
	std::chrono::steady_clock::duration budget)
{
	const auto& shared = shared_budget_start();
	return (shared ? *shared : std::chrono::steady_clock::now()) + budget;
}

/**
 * @brief Keeps the budgets that run out in its scope from reaching the
 * boundary of the unit of work; with `share`, the questions asked in it
 * share one budget.
 *
 * Only for a caller that reads every missing answer in its scope as
 * "undecided" and derives no verdict from it; what ran out before the scope
 * opened still reaches the boundary.
 */
struct time_budget_handled {
	std::string outer = take_time_budget_exhausted();
	std::optional<std::chrono::steady_clock::time_point> outer_start
		= shared_budget_start();
	explicit time_budget_handled(bool share = false) {
		if (share) shared_budget_start() = std::chrono::steady_clock::now();
	}
	time_budget_handled(const time_budget_handled&) = delete;
	time_budget_handled& operator=(const time_budget_handled&) = delete;
	/// Whether a budget ran out in the scope so far.
	bool ran_out() const { return !time_budget_exhausted().empty(); }
	~time_budget_handled() {
		time_budget_exhausted() = std::move(outer);
		shared_budget_start() = outer_start;
	}
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOUNDED_CALL_H__
