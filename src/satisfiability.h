// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file satisfiability.h
 * @brief Temporal satisfiability checking and fixpoint iteration for Tau formulas.
 *
 * Declares the public interface for checking satisfiability of temporal Tau
 * specifications: `is_tau_formula_sat`, `is_tau_impl`, `are_tau_equivalent`,
 * `transform_to_execution`, and `simp_tau_unsat_valid`, as well as the
 * supporting stream-transformation and fixpoint-iteration helpers used by
 * `tau.h` and `interpreter.impl.h`. Template implementations reside in
 * `satisfiability.tmpl.h`.
 */

#ifndef __IDNI__TAU__SATISFIABILITY_H__
#define __IDNI__TAU__SATISFIABILITY_H__

#include "normalizer.h"
//#include "boolean_algebras/bv_ba.h"

namespace idni::tau_lang {

/**
 * @brief Print a fixpoint status message to stdout or discard it.
 * @param message Label string for the message.
 * @param result Textual description of the fixpoint result.
 * @param output When `false`, suppress all output.
 */
inline void print_fixpoint_info(const std::string& message,
	const std::string& result, const bool output);

/**
 * @brief Check whether a formula contains any temporal stream flag.
 * @tparam node Tree node type.
 * @param fm Formula to inspect.
 * @return `true` if @p fm contains at least one stream flag.
 */
template <NodeType node>
bool has_stream_flag(const tref& fm);

/**
 * @brief Compute the maximum IO-variable lookback after normalization.
 * @tparam node Tree node type.
 * @param io_vars Collection of IO variable nodes.
 * @return The largest lookback offset found among @p io_vars.
 */
template <NodeType node>
int_t get_lookback_after_normalization(const trefs& io_vars);

/**
 * @brief Check if @p fm contains a temporary IO variable used for flag handling.
 * @tparam node Tree node type.
 * @param fm Formula to inspect.
 * @return `true` if a temporary output stream variable is present.
 */
template <NodeType node>
bool has_temporary_io_var(tref fm);

/**
 * @brief Rewrite @p io_var to its concrete form at @p time_point.
 * @tparam node Tree node type.
 * @param io_var IO variable node to transform.
 * @param time_point Current simulation time step.
 * @return Transformed IO variable node.
 */
template <NodeType node>
tref transform_io_var(tref io_var, int_t time_point);

/**
 * @brief Wrap output-stream variables of @p fm in existential quantifiers.
 * @tparam node Tree node type.
 * @param fm Formula whose outputs to quantify.
 * @param io_vars All IO variable nodes present in @p fm.
 * @param time_point Current simulation time step.
 * @param initials Set of initial-condition variable nodes.
 * @return Formula with output streams existentially quantified.
 */
template <NodeType node>
tref existentially_quantify_output_streams(tref fm, const trefs& io_vars,
                                int_t time_point, const auto& initials);

/**
 * @brief Wrap input-stream variables of @p fm in universal quantifiers.
 * @tparam node Tree node type.
 * @param fm Formula whose inputs to quantify.
 * @param io_vars All IO variable nodes present in @p fm.
 * @param time_point Current simulation time step.
 * @param initials Set of initial-condition variable nodes.
 * @return Formula with input streams universally quantified.
 */
template <NodeType node>
tref universally_quantify_input_streams(tref fm, const trefs& io_vars,
					int_t time_point, const auto& initials);

/**
 * @brief Evaluate the continuation constraint @p constraint at @p time_point.
 * @tparam node Tree node type.
 * @param constraint Continuation constraint formula.
 * @param time_point Time step at which to evaluate.
 * @return Instantiated constraint at @p time_point.
 */
template <NodeType node>
tref calculate_ctn(tref constraint, int_t time_point);

/**
 * @brief Return `true` if @p constraint is in the initial phase at @p time_point.
 * @tparam node Tree node type.
 * @param constraint Continuation constraint formula.
 * @param time_point Time step to check.
 * @return `true` if the constraint's initial conditions apply at @p time_point.
 */
template <NodeType node>
bool is_initial_ctn_phase(tref constraint, int_t time_point);

/**
 * @brief Instantiate @p original_fm for IO variables at @p time_point.
 * @tparam node Tree node type.
 * @param original_fm Formula template to instantiate.
 * @param io_vars IO variable nodes to substitute.
 * @param time_point Time step to substitute into IO variables.
 * @return Formula with IO variables instantiated at @p time_point.
 */
template <NodeType node>
tref fm_at_time_point(tref original_fm, const trefs &io_vars, int_t time_point);

/**
 * @brief Build the initial chi-step formula for the fixpoint iteration.
 * @tparam node Tree node type.
 * @param chi Initial chi formula.
 * @param st Stream-time formula.
 * @param io_vars All IO variable nodes.
 * @param time_point Starting time step.
 * @param pholder_to_st Map from placeholders to stream-time formulas (updated).
 * @return Pair (initial chi step formula, updated chi formula).
 */
template <NodeType node>
std::pair<tref, tref> build_initial_step_chi(tref chi, tref st,
	const trefs& io_vars, int_t time_point, auto& pholder_to_st);

/**
 * @brief Build the next iteration step formula.
 * @tparam node Tree node type.
 * @param original_fm The always-body formula.
 * @param prev_fm Previous step formula.
 * @param io_vars All IO variable nodes.
 * @param initials Initial-condition nodes.
 * @param step_num Current step index.
 * @param time_point Current simulation time.
 * @param cached_fm Reference to the cached formula (updated on change).
 * @return Next step formula.
 */
template <NodeType node>
tref build_step(tref original_fm, tref prev_fm, const trefs &io_vars,
	const auto& initials, int_t step_num, int_t time_point, tref& cached_fm);

/**
 * @brief Build the next chi-step formula during fixpoint iteration.
 * @tparam node Tree node type.
 * @param chi Current chi formula.
 * @param st Stream-time formula.
 * @param prev_fm Previous step formula.
 * @param io_vars All IO variable nodes.
 * @param initials Initial-condition nodes.
 * @param step_num Current step index.
 * @param time_point Current simulation time.
 * @param cached_fm Reference to the cached formula (updated on change).
 * @param pholder_to_st Map from placeholders to stream-time formulas (updated).
 * @return Next chi-step formula.
 */
template <NodeType node>
tref build_step_chi(tref chi, tref st, tref prev_fm, const trefs& io_vars,
	const auto& initials, int_t step_num, int_t time_point, tref& cached_fm,
	auto& pholder_to_st);

/**
 * @brief Return `true` if @p fm represents a satisfiable run.
 *
 * Designed to be called on the output of `find_fixpoint_phi`/`find_fixpoint_chi`
 * when the run was started at the earliest well-defined time point.
 * @tparam node Tree node type.
 * @param fm Fixpoint formula to test.
 * @return `true` if the run is satisfiable.
 */
template <NodeType node>
bool is_run_satisfiable(tref fm);

/**
 * @brief Compute constraints for uninterpreted constants in an unbounded continuation.
 *
 * Assumes @p fm is an unbound continuation formula.
 * @tparam node Tree node type.
 * @param fm Unbounded continuation formula.
 * @param io_vars IO variable nodes (updated with any new variables).
 * @param start_time Time step at which the continuation was started.
 * @return Formula constraining uninterpreted constants, or `T` if none exist.
 */
template <NodeType node>
tref get_uninterpreted_constants_constraints(tref fm, trefs& io_vars, int_t start_time);

/**
 * @brief Iterate to find the phi fixpoint.
 * @tparam node Tree node type.
 * @param base_fm Base formula to iterate from.
 * @param ctn_initials Initial continuation constraints.
 * @param io_vars All IO variable nodes.
 * @param initials Initial-condition nodes.
 * @param time_point Starting time step.
 * @return Pair (fixpoint formula, time step at convergence).
 */
template <NodeType node>
std::pair<tref, int_t> find_fixpoint_phi(tref base_fm, tref ctn_initials,
	const trefs& io_vars, const auto& initials, int_t time_point);

/**
 * @brief Iterate to find the chi fixpoint.
 * @tparam node Tree node type.
 * @param chi_base Initial chi formula.
 * @param st Stream-time formula.
 * @param io_vars All IO variable nodes.
 * @param initials Initial-condition nodes.
 * @param time_point Starting time step.
 * @return Pair (fixpoint formula, time step at convergence).
 */
template <NodeType node>
std::pair<tref, int_t> find_fixpoint_chi(tref chi_base, tref st,
	const trefs& io_vars, const auto& initials, int_t time_point);

/**
 * @brief Undo the temporal shift applied to non-initial IO variables.
 * @tparam node Tree node type.
 * @param fm Formula to transform back.
 * @param highest_init_cond Highest initial-condition time index.
 * @return Formula with shifted variables restored to their original offsets.
 */
template <NodeType node>
tref transform_back_non_initials(tref fm, const int_t highest_init_cond);

/**
 * @brief Build a flag formula for @p var at the given @p lookback depth.
 * @tparam node Tree node type.
 * @param var_name_node Tree node holding the variable name.
 * @param var Variable name string.
 * @param lookback Lookback depth for the flag.
 * @return Flag formula node.
 */
template <NodeType node>
tref build_flag_on_lookback(tref var_name_node, const std::string& var,
							const int_t lookback);

/**
 * @brief Build a previous-step flag formula for @p io_var_node at the given @p lookback depth.
 * @tparam node Tree node type.
 * @param io_var_node IO variable node.
 * @param var Variable name string.
 * @param lookback Lookback depth for the flag.
 * @return Previous-step flag formula node.
 */
template <NodeType node>
tref build_prev_flag_on_lookback(tref io_var_node,
				const std::string& var, const int_t lookback);

/**
 * @brief Rewrite continuation constraints to stream-variable form.
 * @tparam node Tree node type.
 * @param fm Formula to rewrite.
 * @param flag_initials Output: initial flag formulas generated.
 * @param flag_rules Output: flag recurrence rules generated.
 * @param lookback Maximum lookback used in the formula.
 * @param start_time Starting time step.
 * @param reset_ctn_id When `true`, reset the continuation-ID counter.
 * @return Rewritten formula with continuation constraints expressed as streams.
 */
template <NodeType node>
tref transform_ctn_to_streams(tref fm, tref& flag_initials,
	tref& flag_rules, const int_t lookback, const int_t start_time,
	bool reset_ctn_id = false);

/**
 * @brief Produce an unbounded continuation from a single normalized `always` formula.
 *
 * Assumes the formula has run through the normalizer and is a single `always` statement.
 * @tparam node Tree node type.
 * @param fm Single `always`-quantified formula (post-normalizer).
 * @param start_time Earliest allowed time step.
 * @param output When `true`, enable diagnostic output.
 * @return Unbounded continuation formula.
 */
template <NodeType node>
tref always_to_unbounded_continuation(tref fm, const int_t start_time,
	const bool output);

/**
 * @brief Build a stream guard from input-stream names in uninterpreted constants.
 * @tparam node Tree node type.
 * @param io_vars Input stream variable nodes.
 * @param number Guard index number.
 * @return Guard formula node.
 */
template <NodeType node>
tref create_guard(const trefs& io_vars, const int_t number);

/**
 * @brief Introduce eventual variables for a single normalized Tau DNF clause.
 *
 * Assumes a single normalized Tau DNF clause as input.
 * @tparam node Tree node type.
 * @param fm Single DNF clause (post-normalizer).
 * @param reset_ctn_stream When `true`, reset the continuation-stream counter.
 * @param start_time Starting time step.
 * @return Pair (rewritten formula, largest stream index used).
 */
template <NodeType node>
std::pair<tref, int_t> transform_to_eventual_variables(tref fm,
	bool reset_ctn_stream, const int_t start_time);

/**
 * @brief Conjoin a stream-time constraint to @p st at @p timepoint + @p steps.
 * @tparam node Tree node type.
 * @param st Current stream-time formula.
 * @param timepoint Base time point.
 * @param steps Number of steps to add to @p timepoint.
 * @return Updated stream-time formula with the new constraint.
 */
template <NodeType node>
tref add_st_ctn(tref st, const int_t timepoint, const int_t steps);

/**
 * @brief Construct the initial run formula from an `always` body.
 * @tparam node Tree node type.
 * @param aw Always-quantified formula.
 * @param max_st_lookback Maximum lookback in the stream-time formula.
 * @return Initial run formula.
 */
template <NodeType node>
tref make_initial_run(tref aw, const int_t max_st_lookback);

/**
 * @brief Combine an unbounded always continuation with eventual-variable flags.
 *
 * Assumes @p ubd_aw_continuation is the result of computing the unbounded always
 * continuation of the always part of `transform_to_eventual_variables`, and that
 * @p ev_var_flags is the sometimes part of `transform_to_eventual_variables`.
 * @tparam node Tree node type.
 * @param ubd_aw_continuation Unbounded continuation of the always part.
 * @param ev_var_flags Sometimes part (eventual variable flags).
 * @param original_aw Original always formula.
 * @param start_time Starting time step.
 * @param max_st_lookback Maximum lookback in the stream-time formula.
 * @param output When `true`, print diagnostic messages.
 * @return Combined unbounded continuation formula.
 */
template <NodeType node>
tref to_unbounded_continuation(tref ubd_aw_continuation,
	tref ev_var_flags, tref original_aw, const int_t start_time,
	const int_t max_st_lookback, const bool output);

/**
 * @brief Transform a normalized Tau formula into execution form.
 * @tparam node Tree node type.
 * @param fm Normalized Tau formula.
 * @param start_time Starting time step (default: 0).
 * @param output When `true`, print diagnostic messages (default: `false`).
 * @return Formula ready for step-by-step execution.
 */
template <NodeType node>
tref transform_to_execution(tref fm, const int_t start_time = 0,
					const bool output = false);

/**
 * @brief Check whether a Tau formula is satisfiable.
 * @tparam node Tree node type.
 * @param fm Tau formula to test.
 * @param start_time Starting time step (default: 0).
 * @param output When `true`, print diagnostic messages (default: `false`).
 * @return `true` if the formula is satisfiable.
 */
template <NodeType node>
bool is_tau_formula_sat(tref fm, const int_t start_time = 0,
	const bool output = false);

/**
 * @brief Check whether temporal formula @p f1 implies @p f2.
 * @tparam node Tree node type.
 * @param f1 Antecedent formula.
 * @param f2 Consequent formula.
 * @return `true` if every model of @p f1 satisfies @p f2.
 */
template <NodeType node>
bool is_tau_impl(tref f1, tref f2);

/**
 * @brief Check whether two closed temporal formulas are logically equivalent.
 *
 * The formulas must be closed (no free variables).
 * @tparam node Tree node type.
 * @param f1 First formula (closed).
 * @param f2 Second formula (closed).
 * @return `true` if @p f1 and @p f2 have identical models.
 */
template <NodeType node>
bool are_tau_equivalent(tref f1, tref f2);

/**
 * @brief Simplify @p fm by removing unsatisfiable or valid temporal sub-formulas.
 * @tparam node Tree node type.
 * @param fm Formula to simplify.
 * @param start_time Starting time step (default: 0).
 * @param output When `true`, print diagnostic messages (default: `false`).
 * @return Simplified formula.
 */
template <NodeType node>
tref simp_tau_unsat_valid(tref fm, const int_t start_time = 0,
				const bool output = false);

} // namespace idni::tau_lang

#include "satisfiability.tmpl.h"

#endif // __IDNI__TAU__SATISFIABILITY_H__
