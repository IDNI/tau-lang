// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__TAU_DIAGNOSTICS_H__
#define __IDNI__TAU__TAU_DIAGNOSTICS_H__

#include "utility/diagnostics.h"

namespace idni::tau_lang {

namespace diag = ::idni::diagnostics;

template <typename T>
using result = diag::result<T>;

using report = diag::report;
using code = diag::code;
using attr = diag::attr;
using diag_sink = diag::sink;
using diag_sinks = diag::sinks;

// Gates per-rule application/hit accounting in nso_rr_apply(rule, tref);
// off by default since it costs a map lookup per rewrite. Set alongside the
// REPL/CLI "benchmarks" option so a benchmark run also gets rule counts.
inline bool rule_counting = false;

inline bool step_awaiting_input(const report& rep) {
	for (const auto& n : rep.nodes())
		if (n.tag == code::invalid_state) return true;
	return false;
}

inline bool report_has_code(const report& rep, code c) {
	for (const auto& n : rep.nodes())
		if (n.tag == c) return true;
	return false;
}

} // namespace idni::tau_lang

#define TAU_TRY_CONCAT_INNER(a, b) a##b
#define TAU_TRY_CONCAT(a, b) TAU_TRY_CONCAT_INNER(a, b)

/// Sequential try-step for a function that owns a local `result<T> r;`.
/// `TAU_TRY(decl, expr)` evaluates `expr` (a `result<U>`), always merges its
/// report into `r`, and on success binds its value to `decl`; on failure it
/// returns `r` immediately. Use as a standalone statement (not as the
/// unbraced body of `if`/`else`/a loop). Put a `TAU_TRY` call inside the
/// same block as any `scope_guard` it should close on early return -- the
/// guard's destructor (or an explicit `.close()`) runs as part of that
/// block unwinding, the same as any other early `return r;`.
#define TAU_TRY(decl, expr) \
	auto TAU_TRY_CONCAT(_tau_try_, __LINE__) = r.merge_take(expr); \
	if (!TAU_TRY_CONCAT(_tau_try_, __LINE__)) return r; \
	decl = std::move(*TAU_TRY_CONCAT(_tau_try_, __LINE__))

#endif // __IDNI__TAU__TAU_DIAGNOSTICS_H__
