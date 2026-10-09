// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Runs one question in a forked child under a wall-clock limit and an
// address-space cap (setrlimit), so a hang, a blow-up or a crash costs only
// that instance. The question arms the cap itself once its input is parsed
// (cap_memory). The child's stdout goes to stderr, keeping stdout for the
// CSV; its verdict comes back through a pipe.

#ifndef __IDNI__TAU__BV_NO_ARITH_BENCH__RUNNER_H__
#define __IDNI__TAU__BV_NO_ARITH_BENCH__RUNNER_H__

#include <chrono>
#include <cstddef>
#include <functional>
#include <string>

namespace bv_bench {

/// A child's answer. `value` is one word: `T`, `F`, or the question's own
/// word for anything else; `binders` counts the quantifiers left.
struct verdict {
	std::string value;
	size_t binders = 0;
	double ms = 0;
};

struct limits {
	double timeout_s;
	size_t mem_mb;
};

struct child_run {
	enum class end { answered, timeout, out_of_memory, error };
	end how;
	verdict answer;       // when answered
	std::string reason;   // when error
	double ms;            // wall time seen by the parent
};

child_run run_in_child(const limits& lim, const std::function<verdict()>& question);

/// In a child of run_in_child: caps its address space at the present size
/// plus `mem_mb`. A question calls it after the parse, which a large formula
/// would otherwise spend the whole cap on.
void cap_memory();

double ms_since(std::chrono::steady_clock::time_point start);

} // namespace bv_bench

#endif // __IDNI__TAU__BV_NO_ARITH_BENCH__RUNNER_H__
