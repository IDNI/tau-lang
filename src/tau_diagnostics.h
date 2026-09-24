// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__TAU_DIAGNOSTICS_H__
#define __IDNI__TAU__TAU_DIAGNOSTICS_H__

#include "utility/diagnostics.h"
#include "parser_strings.h"
#include <optional>
#include <string>

namespace idni::tau_lang {

namespace diag = ::idni::diagnostics;

template <typename T>
using result = diag::result<T>;

using report = diag::report;
using code = diag::code;
using attr = diag::attr;
using label = idni::parser_strings::label;
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
	static constexpr sv failed_to_calculate_initial_spec
		= "Failed to calculate initial spec";
	static constexpr sv no_input_provided = "No input provided";
	static constexpr sv auto_continue_is_false = "Auto continue is false";
	static constexpr sv no_solution_found = "No solution found";
	static constexpr sv invalid_arguments = "Invalid argument(s)";
	static constexpr sv normalization_failed = "Normalization failed";
	static constexpr sv failed_to_apply_definitions
		= "Failed to apply definitions";
	static constexpr sv invalid_formula = "Invalid formula";
	static constexpr sv failed_to_parse_formula_or_term
		= "Failed to parse formula or term";
	static constexpr sv substitution_failed = "Substitution failed";
	static constexpr sv spec_contains_free_variables
		= "Spec contains free variables";
	static constexpr sv could_not_normalize_for_satisfiability
		= "Could not normalize the formula; its satisfiability "
		  "cannot be decided";
	static constexpr sv is_tau_impl_no_verdict_validity
		= "is_tau_impl returned neither a value nor an error while "
		  "checking validity";
	static constexpr sv specification_could_not_be_compiled
		= "the specification could not be compiled";
	static constexpr sv failed_to_write_outputs = "Failed to write outputs";
	static constexpr sv bdd_node_table_exhausted
		= "bdd node table exhausted: a node did not fit, so the result "
		  "is unknown and no answer is given";
	static constexpr sv no_ba_element_assigned_to_output
		= "No Boolean algebra element assigned to output";
	static constexpr sv execution_stopped_on_failed_step
		= "Execution stopped on a failed step";
	static constexpr sv failed_to_read_step_input
		= "Failed to read step input";
	static constexpr sv algorithm_d_no_verdict
		= "Algorithm D: the product game could not be built; no verdict";
	static constexpr sv unknown_realizability_no_verdict
		= "UNKNOWN: the synthesis backend failed or produced no "
		  "verdict; realizability could not be decided";
	static constexpr sv unknown_realizability_timed_out
		= "UNKNOWN: the synthesis backend failed, timed out or "
		  "refused the formula; realizability could not be decided";
	static constexpr sv unknown_satisfiability_no_verdict
		= "UNKNOWN: the synthesis backend failed or produced no "
		  "verdict; satisfiability could not be decided";
	static constexpr sv non_temporal_normalization_failed
		= "non-temporal normalization failed";
	static constexpr sv a_e_both_polarity_unsound
		= "A/E under a both-polarity connective (<->, ^, conditional "
		  "guard) has no sound LTL encoding here";
	static constexpr sv invalid_ba_type_id
		= "the Boolean-algebra type id is invalid";
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

// @p n's value for attr @p lbl, or nullopt when @p n does not carry it.
// The one place that indexes a node's attrs by label; every other lookup
// below is built on this.
inline std::optional<int64_t> node_attr_value(const report& rep,
	const diag::node& n, idni::int_t lbl)
{
	for (uint8_t a = 0; a < n.attr_cnt; ++a) {
		const auto& at = rep.attrs()[n.attr_off + a];
		if (at.key == lbl) return at.value;
	}
	return std::nullopt;
}

// node_attr_value's result decoded as interned text. A text label --
// name, value, type_name, path, expected -- stores an interned key.
// is_text_label decides which, never the sign of the value.
inline std::optional<std::string> node_attr_text(const report& rep,
	const diag::node& n, idni::int_t lbl)
{
	auto v = node_attr_value(rep, n, lbl);
	if (!v) return std::nullopt;
	return std::string(rep.str(static_cast<idni::int_t>(*v)));
}

// The value of the first attr tagged @p lbl found anywhere in @p rep, or
// nullopt.
inline std::optional<int64_t> report_attr_value(const report& rep,
	idni::int_t lbl)
{
	for (const auto& n : rep.nodes())
		if (auto v = node_attr_value(rep, n, lbl)) return v;
	return std::nullopt;
}

// True when some node in @p rep carries an attr tagged @p lbl.
inline bool report_has_attr(const report& rep, idni::int_t lbl) {
	return report_attr_value(rep, lbl).has_value();
}

// True when a step stopped because it needs input. invalid_state is reserved
// for exactly that: a completed step never carries it.
inline bool step_awaiting_input(const report& rep) {
	return report_has_code(rep, code::invalid_state);
}

// Collapses newlines and caps the length of value text quoted in a
// diagnostic message, so a whole spec pasted as an input value does not
// flood the report.
inline std::string truncate_for_message(std::string_view v,
	size_t max_len = 120)
{
	std::string s;
	s.reserve(v.size());
	for (char c : v) s += (c == '\n' || c == '\r') ? ' ' : c;
	if (s.size() > max_len) s = s.substr(0, max_len) + "...";
	return s;
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

/// Like TAU_TRY_OR, but attaches @p attrs (a braced std::initializer_list<attr>)
/// to the synthesized error.
#define TAU_TRY_OR_ATTR(decl, expr, c, msg, attrs) \
	auto TAU_TRY_CONCAT(_tau_try_, __LINE__) = r.take_or_error(expr, c, msg, attrs); \
	if (!TAU_TRY_CONCAT(_tau_try_, __LINE__)) return r; \
	decl = std::move(*TAU_TRY_CONCAT(_tau_try_, __LINE__))

#endif // __IDNI__TAU__TAU_DIAGNOSTICS_H__
