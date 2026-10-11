// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file bounded_child.h
 * @brief Work sent as text to a new process of the running program, under a
 * wall-clock bound.
 *
 * The process model for a platform without fork. The program runs itself
 * with the hidden verb of bounded_call.h, the request as stdin, and kills the
 * child when the bound passes.
 */

#ifndef __IDNI__TAU__BOUNDED_CHILD_H__
#define __IDNI__TAU__BOUNDED_CHILD_H__

#include <charconv>
#include <chrono>
#include <string>

#include "bounded_call.h"
#include "spawn_capture.h"
#include "utility/temp_file.h"

namespace idni::tau_lang {

/**
 * @brief Runs the job @p kind on @p request in a new process of this program
 * and waits at most @p timeout_ms milliseconds for its answer.
 *
 * `done` carries the answer; `timed_out` means the child was killed at the
 * bound; `failed` means no child could be made, or it ended without a valid
 * answer. Every status but `done` carries the error that says why: the
 * report of the temp file or of `spawn_capture`, with the exit code and the
 * output of the child.
 * @param kind A job registered with register_bounded_child.
 * @param request The text the job reads.
 * @param timeout_ms Wall-clock bound in milliseconds, counted from the call.
 * @return The outcome, with the answer in `value` when `status == done`.
 */
inline bounded_outcome run_bounded_child(const std::string& kind,
	const std::string& request, uint64_t timeout_ms)
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
	if (!bounded_children_available()) {
		out.rep.error(code::not_found, "no program is known to run "
			"a bounded child", {{ label::name, kind }});
		return finish(bounded_outcome::failed);
	}
	auto tmp = fs::temp_file::create("tau_bounded", request);
	if (!tmp.has_value()) {
		out.rep.append(std::move(tmp).report());
		return finish(bounded_outcome::failed);
	}
	// whole seconds, rounded up, so the child is never killed before the bound
	const uint64_t secs = (timeout_ms + 999) / 1000;
	spawn_options opts;
	opts.stdin_path = tmp.value().path();
	auto r = spawn_capture({ bounded_child_program(),
		std::string(bounded_child_verb), kind },
		secs > INT32_MAX ? INT32_MAX : static_cast<int>(secs),
		[](int c) { return c == 0; }, opts);
	if (!r.has_value()) {
		const bool timed_out = report_has_attr(r.report(), label::timeout);
		out.rep.append(std::move(r).report());
		return finish(timed_out ? bounded_outcome::timed_out
			: bounded_outcome::failed);
	}
	const std::string& text = r.value();
	const char* end = text.data() + text.size();
	while (end > text.data() && (end[-1] == '\n' || end[-1] == ' ')) --end;
	unsigned v = 0;
	auto [p, ec] = std::from_chars(text.data(), end, v);
	if (ec != std::errc{} || p != end || v > 255) {
		out.rep.error(code::runtime_error, "the answer of the bounded "
			"child is not a number", {{ label::name, kind },
			{ label::value, truncate_for_message(text) }});
		return finish(bounded_outcome::failed);
	}
	out.value = static_cast<uint8_t>(v);
	return finish(bounded_outcome::done);
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOUNDED_CHILD_H__
