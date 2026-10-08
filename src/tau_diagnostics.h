// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__TAU_DIAGNOSTICS_H__
#define __IDNI__TAU__TAU_DIAGNOSTICS_H__

#include "utility/diagnostics.h"
#include "parser_strings.h"
#include <optional>
#include <string>

namespace idni::tau_lang {

/// Short alias of the parser's diagnostics namespace.
namespace diag = ::idni::diagnostics;

/// A value of type @p T or an error, with the report that explains it.
template <typename T>
using result = diag::result<T>;

/// Structured diagnostics: nodes tagged with a code, with attrs and timed scopes.
using report = diag::report;
/// The tag of a report node; its top two bits encode the severity.
using code = diag::code;
/// A labelled value attached to a report node.
using attr = diag::attr;
/// The attr labels shared with the parser.
using label = idni::parser_strings::label;
/// The output slot of one severity band (a stream or a callable).
using diag_sink = diag::sink;
/// One sink per severity band (error, warning, info).
using diag_sinks = diag::sinks;

/// @brief Static message strings, mirroring idni::parser_strings::messages.
/// Use these instead of repeating the same literal across error sites.
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
	static constexpr sv generated_constant_too_large = "A value the "
		"solver built passed the constant size budget (maxconstantsize, "
		"--max-constant-size; 0 = unlimited), so no solution was found "
		"within it";
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
	static constexpr sv unknown_ctl_star_e_witness
		= "UNKNOWN: the CTL* reduction is unrealizable, but an E "
		  "witness over a past operator ranges over every input "
		  "branch, which is stricter than E; realizability could "
		  "not be decided";
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
	static constexpr sv ba_option_without_type_prefix
		= "The option of the algebra does not carry its type prefix";
};

/// @brief Gates per-rule application/hit accounting in nso_rr_apply(rule, tref),
/// which the normalizer flushes into its report; off by default since it costs
/// a map lookup per rewrite. The CLI sets it from its "benchmarks" option, so a
/// benchmark run (file spec or REPL) also gets rule counts.
inline bool rule_counting = false;

/// @brief Whether @p rep carries a node tagged @p c.
/// @param rep The report to search.
/// @param c The code to look for.
/// @return True when some node of @p rep has tag @p c.
inline bool report_has_code(const report& rep, code c) {
	for (const auto& n : rep.nodes()) if (n.tag == c) return true;
	return false;
}

/// @brief The raw value of attr @p lbl on node @p n.
/// The one place that indexes a node's attrs by label; every other lookup
/// below is built on this.
/// @param rep The report owning @p n and its attrs.
/// @param n A node of @p rep.
/// @param lbl The attr label.
/// @return The stored value (an interned key for a text label), or nullopt
///         when @p n does not carry @p lbl.
inline std::optional<int64_t> node_attr_value(const report& rep,
	const diag::node& n, idni::int_t lbl)
{
	for (uint8_t a = 0; a < n.attr_cnt; ++a) {
		const auto& at = rep.attrs()[n.attr_off + a];
		if (at.key == lbl) return at.value;
	}
	return std::nullopt;
}

/// @brief node_attr_value's result decoded as interned text.
/// A text label -- name, value, type_name, path, expected -- stores an
/// interned key; is_text_label decides which, never the sign of the value.
/// The caller must pass a text label: the value is decoded unconditionally.
/// @param rep The report owning @p n and its string table.
/// @param n A node of @p rep.
/// @param lbl A text attr label.
/// @return The text, or nullopt when @p n does not carry @p lbl.
inline std::optional<std::string> node_attr_text(const report& rep,
	const diag::node& n, idni::int_t lbl)
{
	auto v = node_attr_value(rep, n, lbl);
	if (!v) return std::nullopt;
	return std::string(rep.str(static_cast<idni::int_t>(*v)));
}

/// @brief The value of the first attr tagged @p lbl found anywhere in @p rep.
/// @param rep The report to search, in node order.
/// @param lbl The attr label.
/// @return The raw value, or nullopt when no node carries @p lbl.
inline std::optional<int64_t> report_attr_value(const report& rep,
	idni::int_t lbl)
{
	for (const auto& n : rep.nodes())
		if (auto v = node_attr_value(rep, n, lbl)) return v;
	return std::nullopt;
}

/// @brief Whether some node in @p rep carries an attr tagged @p lbl.
/// @param rep The report to search.
/// @param lbl The attr label.
/// @return True when the attr is found.
inline bool report_has_attr(const report& rep, idni::int_t lbl) {
	return report_attr_value(rep, lbl).has_value();
}

/// @brief Whether a step stopped because it needs input.
/// code::invalid_state is reserved for exactly that: a completed step never
/// carries it.
/// @param rep The step's report.
/// @return True when @p rep carries a code::invalid_state node.
inline bool step_awaiting_input(const report& rep) {
	return report_has_code(rep, code::invalid_state);
}

/// @brief Collapses newlines and caps the length of value text quoted in a
/// diagnostic message, so a whole spec pasted as an input value does not
/// flood the report.
/// @param v The text to quote.
/// @param max_len Characters kept before the "..." suffix is appended.
/// @return @p v with each CR/LF replaced by a space, cut to @p max_len
///         characters plus "..." when longer.
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

/// Token-pastes @p a and @p b without expanding them first.
#define TAU_TRY_CONCAT_INNER(a, b) a##b
/// Token-pastes @p a and @p b after expanding them (used with __LINE__).
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

/// Like TAU_TRY, for a `result<void>` child: always merges its report into
/// `r` and returns `r` on failure. Binds nothing, so it is a plain statement
/// (same scope_guard/early-return contract as TAU_TRY).
#define TAU_TRY_VOID(expr) \
	if (!r.merge_ok(expr)) return r

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
