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

// Static message strings, mirroring idni::parser_strings::messages. Use
// these instead of repeating the same literal across error sites.
struct messages {
	using sv = std::string_view;
	static constexpr sv failed_to_parse_spec
		= "Failed to parse spec";
	static constexpr sv normalization_produced_no_formula
		= "normalization produced no formula";
	static constexpr sv non_temp_normalization_produced_no_formula
		= "non-temporal normalization produced no formula";
	static constexpr sv temp_normalization_produced_no_formula
		= "temporal normalization produced no formula";
	static constexpr sv execution_transform_produced_no_formula
		= "execution transform produced no formula";
	static constexpr sv failed_to_build_rewriting_rules
		= "Failed to build rewriting rules from specification";
	static constexpr sv failed_to_substitute = "Failed to substitute";
	static constexpr sv failed_to_calculate_initial_spec
		= "Failed to calculate initial spec";
	static constexpr sv no_input_provided = "No input provided";
	static constexpr sv auto_continue_is_false = "Auto continue is false";
	static constexpr sv no_solution_found = "No solution found";
	static constexpr sv internal_error_in_solver
		= "Internal error in solver";
};

// Gates per-rule application/hit accounting in nso_rr_apply(rule, tref);
// off by default since it costs a map lookup per rewrite. Set alongside the
// REPL/CLI "benchmarks" option so a benchmark run also gets rule counts.
inline bool rule_counting = false;

// True when the report carries a node tagged @p c.
inline bool report_has_code(const report& rep, code c) {
	for (const auto& n : rep.nodes()) if (n.tag == c) return true;
	return false;
}

// True when a step stopped because it needs input. invalid_state is reserved
// for exactly that: a completed step never carries it.
inline bool step_awaiting_input(const report& rep) {
	return report_has_code(rep, code::invalid_state);
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

/// Like TAU_TRY, but for a malformed child (neither value nor error): synthesizes
/// @p c / @p msg via `result::take_or_error` instead of `merge_take`. Same
/// scope_guard/early-return contract as TAU_TRY.
#define TAU_TRY_OR(decl, expr, c, msg) \
	auto TAU_TRY_CONCAT(_tau_try_, __LINE__) = r.take_or_error(expr, c, msg); \
	if (!TAU_TRY_CONCAT(_tau_try_, __LINE__)) return r; \
	decl = std::move(*TAU_TRY_CONCAT(_tau_try_, __LINE__))

#endif // __IDNI__TAU__TAU_DIAGNOSTICS_H__
