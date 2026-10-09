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
 * Without fork (Windows) a program that runs itself as a child
 * (@ref enable_bounded_children) sends a registered job as text to a new
 * process of itself instead (bounded_child.h). Without either (wasm, a
 * library in another program) the work runs in the process, unbounded.
 */

#ifndef __IDNI__TAU__BOUNDED_CALL_H__
#define __IDNI__TAU__BOUNDED_CALL_H__

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <optional>
#include <string>
#include <string_view>

#include "self_exe_path.h"
#include "tau_diagnostics.h"

#if defined(_WIN32)
#include <fcntl.h>
#include <io.h>
#endif

#if !defined(__EMSCRIPTEN__) && !defined(_WIN32)
#define TAU_HAS_BOUNDED_CALL 1
#include <cerrno>
#include <csignal>
#include <cstring>
#include <poll.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#ifdef __linux__
#include <sys/prctl.h>
#endif
#endif

namespace idni::tau_lang {

/// What @ref run_bounded got from its work: the status, the one-byte answer
/// (meaningful only when `status == done`), the wall-clock seconds spent,
/// and an error that says why when the status is not `done`.
struct bounded_outcome {
	enum kind { done, timed_out, failed } status = failed;
	uint8_t value = 0;
	double seconds = 0;
	diag::report rep;
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
 * @param work The computation; it runs in a forked child, so its side effects
 * never reach the caller (except with no process model).
 * @param timeout_ms Wall-clock bound in milliseconds, counted from the call.
 * @return The outcome, with the answer in `value` when `status == done`.
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
	if (::pipe(fd) != 0) {
		out.rep.error(code::io_error, "the pipe to a bounded child "
			"could not be made", {{ label::value, std::strerror(errno) }});
		return finish(bounded_outcome::failed);
	}
	const pid_t parent = ::getpid();
	const auto deadline = start + std::chrono::milliseconds(timeout_ms);
	const pid_t pid = ::fork();
	if (pid < 0) {
		const int e = errno;
		::close(fd[0]); ::close(fd[1]);
		out.rep.error(code::runtime_error, "the bounded child could not "
			"be started", {{ label::value, std::strerror(e) }});
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
	if (timed_out) {
		out.rep.error(code::runtime_error, "the bounded child was killed "
			"at its bound (ms)", {{ label::timeout, timeout_ms }});
		return finish(bounded_outcome::timed_out);
	}
	// a shell's convention: a signal shows as 128 plus its number
	const int status = WIFSIGNALED(st) ? 128 + WTERMSIG(st)
		: WIFEXITED(st) ? WEXITSTATUS(st) : -1;
	out.rep.error(code::runtime_error, "the bounded child ended without "
		"an answer", {{ label::exit_code, status }});
	return finish(bounded_outcome::failed);
#else
	(void)timeout_ms;
	out.value = work();
	return finish(bounded_outcome::done);
#endif
}

/// The hidden first argument that makes a program a bounded child.
inline constexpr std::string_view bounded_child_verb = "--tau-bounded-child";

/// A job a bounded child runs: the request text in, the one-byte answer out;
/// nullopt when the request is not one the job reads.
using bounded_child_job = std::optional<uint8_t> (*)(const std::string&);

/// The jobs a bounded child of this program knows, by kind.
inline std::map<std::string, bounded_child_job, std::less<>>&
	bounded_child_jobs()
{
	static std::map<std::string, bounded_child_job, std::less<>> jobs;
	return jobs;
}

/// Makes @p job known to a bounded child as @p kind.
inline void register_bounded_child(std::string kind, bounded_child_job job) {
	bounded_child_jobs()[std::move(kind)] = job;
}

/// The program a bounded child runs; empty when none is known.
inline std::string& bounded_child_program() {
	static std::string program;
	return program;
}

/// Lets this program run itself as a bounded child. Only a program whose
/// main() first calls @ref bounded_child_main may call it.
inline void enable_bounded_children() {
	bounded_child_program() = self_exe_path();
}

/// Whether a job can run in a bounded child of this program.
inline bool bounded_children_available() {
	return !bounded_child_program().empty();
}

/**
 * @brief Runs the job a bounded child was started for, when it was.
 *
 * With @ref bounded_child_verb and a kind as the arguments, the job of that
 * kind reads stdin and its answer goes to stdout as a number.
 * @return -1 when @p argv does not start a bounded child; else the exit
 * code: 0 with an answer, 2 for an unknown kind or a refused request.
 */
inline int bounded_child_main(int argc, char** argv) {
	if (argc < 2 || argv[1] != bounded_child_verb) return -1;
	if (argc != 3) return 2;
	auto it = bounded_child_jobs().find(std::string_view(argv[2]));
	if (it == bounded_child_jobs().end()) return 2;
#if defined(_WIN32)
	// text mode would turn CR LF into LF and stop at a Ctrl-Z byte
	_setmode(_fileno(stdin), _O_BINARY);
#endif
	const std::string request((std::istreambuf_iterator<char>(std::cin)),
		std::istreambuf_iterator<char>());
	const auto answer = it->second(request);
	if (!answer) return 2;
	std::cout << static_cast<unsigned>(*answer) << '\n' << std::flush;
	return 0;
}

/// The message of the first budget that ran out in the current unit of
/// work; empty when none did.
inline std::string& time_budget_exhausted() {
	static std::string message;
	return message;
}

/// The reports of the work that ran past its budgets in the current unit of
/// work, which say why: the error of a killed or a dead child.
inline diag::report& time_budget_report() {
	static diag::report rep;
	return rep;
}

/// Records that work ran past its time budget, described by @p message, and
/// why, in @p detail; the first message of a unit of work is kept, later
/// ones are dropped, and every detail is kept.
inline void note_time_budget_exhausted(std::string message,
	diag::report detail = {})
{
	if (time_budget_exhausted().empty())
		time_budget_exhausted() = std::move(message);
	time_budget_report().append(std::move(detail));
}

/// Returns and clears the message of @ref time_budget_exhausted.
inline std::string take_time_budget_exhausted() {
	std::string m;
	m.swap(time_budget_exhausted());
	return m;
}

/// Returns and clears @ref time_budget_report.
inline diag::report take_time_budget_report() {
	diag::report r = std::move(time_budget_report());
	time_budget_report() = diag::report{};
	return r;
}

/// While set, the deadline every bounded question asked shares, in place
/// of each question's own budget.
inline std::optional<std::chrono::steady_clock::time_point>& shared_deadline() {
	static std::optional<std::chrono::steady_clock::time_point> deadline;
	return deadline;
}

/// While a deadline is shared, the most any one question asked may take.
inline std::chrono::steady_clock::duration& shared_question_budget() {
	static std::chrono::steady_clock::duration budget{};
	return budget;
}

/// When a question with a budget of @p budget, asked now, runs out; in a
/// scope that shares a deadline, that deadline, or the scope's own budget
/// of one question when that ends first.
inline std::chrono::steady_clock::time_point budget_deadline(
	std::chrono::steady_clock::duration budget)
{
	const auto now = std::chrono::steady_clock::now();
	const auto& shared = shared_deadline();
	if (!shared) return now + budget;
	return std::min(*shared, now + shared_question_budget());
}

/**
 * @brief Keeps the budgets that run out in its scope from reaching the
 * boundary of the unit of work; with a `share`d budget, the questions asked
 * in it all end by the time that budget, counted from the scope's opening,
 * passes, and each takes at most `per_question`.
 *
 * Only for a caller that reads every missing answer in its scope as
 * "undecided" and derives no verdict from it; what ran out before the scope
 * opened still reaches the boundary.
 */
struct time_budget_handled {
	using duration = std::chrono::steady_clock::duration;
	std::string outer = take_time_budget_exhausted();
	diag::report outer_report = take_time_budget_report();
	std::optional<std::chrono::steady_clock::time_point> outer_deadline
		= shared_deadline();
	duration outer_question = shared_question_budget();
	/// Takes over the budget state of the enclosing scope; @p share, when
	/// given, opens a shared deadline that far from now, and @p per_question
	/// (default: @p share) caps each question within it.
	explicit time_budget_handled(std::optional<duration> share = {},
		std::optional<duration> per_question = {})
	{
		if (!share) return;
		shared_deadline() = std::chrono::steady_clock::now() + *share;
		shared_question_budget() = per_question.value_or(*share);
	}
	time_budget_handled(const time_budget_handled&) = delete;
	time_budget_handled& operator=(const time_budget_handled&) = delete;
	/// Whether a budget ran out in the scope so far.
	bool ran_out() const { return !time_budget_exhausted().empty(); }
	/// Restores the enclosing scope's budget state, dropping what ran out here.
	~time_budget_handled() {
		time_budget_exhausted() = std::move(outer);
		// TODO (HIGH) dropped error: the reports of what ran out in the scope -- the caller reads its missing answers as "undecided", and an error would make that answer an error.
		time_budget_report() = std::move(outer_report);
		shared_deadline() = outer_deadline;
		shared_question_budget() = outer_question;
	}
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOUNDED_CALL_H__
