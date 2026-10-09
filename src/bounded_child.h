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
 * answer.
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
	if (!bounded_children_available()) return finish(bounded_outcome::failed);
	// TODO (HIGH) dropped error: the reports of the temp file and the child -- bounded_outcome carries no report.
	auto tmp = fs::temp_file::create("tau_bounded", request);
	if (!tmp.has_value()) return finish(bounded_outcome::failed);
	// whole seconds, rounded up, so the child is never killed before the bound
	const uint64_t secs = (timeout_ms + 999) / 1000;
	spawn_options opts;
	opts.stdin_path = tmp.value().path();
	auto r = spawn_capture({ bounded_child_program(),
		std::string(bounded_child_verb), kind },
		secs > INT32_MAX ? INT32_MAX : static_cast<int>(secs),
		[](int c) { return c == 0; }, opts);
	if (!r.has_value())
		return finish(report_has_attr(r.report(), label::timeout)
			? bounded_outcome::timed_out : bounded_outcome::failed);
	const std::string& text = r.value();
	const char* end = text.data() + text.size();
	while (end > text.data() && (end[-1] == '\n' || end[-1] == ' ')) --end;
	unsigned v = 0;
	auto [p, ec] = std::from_chars(text.data(), end, v);
	if (ec != std::errc{} || p != end || v > 255)
		return finish(bounded_outcome::failed);
	out.value = static_cast<uint8_t>(v);
	return finish(bounded_outcome::done);
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOUNDED_CHILD_H__
