// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__SATISFIABILITY_H__
#define __IDNI__TAU__SATISFIABILITY_H__

#include "normalizer.h"

namespace idni::tau_lang {

inline void print_fixpoint_info(const std::string& message,
	const std::string& result, const bool output);

template <NodeType node>
bool has_stream_flag(const tref& fm);

template <NodeType node>
int_t get_lookback_after_normalization(const trefs& io_vars);

// Check if a formula has a temporary output stream
// which are used for flag handling
template <NodeType node>
bool has_temporary_io_var(tref fm);

template <NodeType node>
tref transform_io_var(tref io_var, int_t time_point);

template <NodeType node>
tref existentially_quantify_output_streams(tref fm, const trefs& io_vars,
                                int_t time_point, const auto& initials);

template <NodeType node>
tref universally_quantify_input_streams(tref fm, const trefs& io_vars,
					int_t time_point, const auto& initials);

template <NodeType node>
tref calculate_ctn(tref constraint, int_t time_point);

template <NodeType node>
bool is_initial_ctn_phase(tref constraint, int_t time_point);

template <NodeType node>
tref fm_at_time_point(tref original_fm, const trefs &io_vars, int_t time_point);

template <NodeType node>
std::pair<tref, tref> build_initial_step_chi(tref chi, tref st,
	const trefs& io_vars, int_t time_point, auto& pholder_to_st);

template <NodeType node>
tref build_step(tref original_fm, tref prev_fm, const trefs &io_vars,
	const auto& initials, int_t step_num, int_t time_point, tref& cached_fm);

template <NodeType node>
tref build_step_chi(tref chi, tref st, tref prev_fm, const trefs& io_vars,
	const auto& initials, int_t step_num, int_t time_point, tref& cached_fm,
	auto& pholder_to_st);

// This method is designed to be called on the output of find_fixpoint_phi/chi
// when the run was started at the earliest well-defined time point
template <NodeType node>
bool is_run_satisfiable(tref fm);

// Assumption is that the provided fm is an unbound continuation
template <NodeType node>
tref get_uninterpreted_constants_constraints(tref fm, trefs& io_vars);

template <NodeType node>
std::pair<tref, int_t> find_fixpoint_phi(tref base_fm, tref ctn_initials,
	const trefs& io_vars, const auto& initials, int_t time_point);

template <NodeType node>
std::pair<tref, int_t> find_fixpoint_chi(tref chi_base, tref st,
	const trefs& io_vars, const auto& initials, int_t time_point);

template <NodeType node>
tref transform_back_non_initials(tref fm, const int_t highest_init_cond);

template <NodeType node>
tref build_flag_on_lookback(tref var_name_node, const std::string& var,
							const int_t lookback);

template <NodeType node>
tref build_prev_flag_on_lookback(tref io_var_node,
				const std::string& var, const int_t lookback);

template <NodeType node>
tref transform_ctn_to_streams(tref fm, tref& flag_initials,
	tref& flag_rules, const int_t lookback, const int_t start_time,
	bool reset_ctn_id = false);

// We assume that the formula has run through the normalizer before
// and is a single always statement
template <NodeType node>
tref always_to_unbounded_continuation(tref fm, const int_t start_time,
	const bool output);

// Creates a guard using the names of the input streams in uninterpreted constants
template <NodeType node>
tref create_guard(const trefs& io_vars, const int_t number);

// Assumes single normalized Tau DNF clause
template <NodeType node>
std::pair<tref, int_t> transform_to_eventual_variables(tref fm,
	bool reset_ctn_stream, const int_t start_time);

template <NodeType node>
tref add_st_ctn(tref st, const int_t timepoint, const int_t steps);

template <NodeType node>
tref make_initial_run(tref aw, const int_t max_st_lookback);

// Assumes that ubd_aw_continuation is the result of computing the unbounded always continuation of
// the always part of the output of "transform_to_eventual_variables" and
// that ev_var_flags is the sometimes part of "transform_to_eventual_variables"
template <NodeType node>
tref to_unbounded_continuation(tref ubd_aw_continuation,
	tref ev_var_flags, tref original_aw, const int_t start_time,
	const int_t max_st_lookback, const bool output);

template <NodeType node>
tref transform_to_execution(tref fm, const int_t start_time = 0,
					const bool output = false);

template <NodeType node>
bool is_tau_formula_sat(tref fm, const int_t start_time = 0,
	const bool output = false);

// Check for temporal formulas if f1 implies f2
template <NodeType node>
bool is_tau_impl(tref f1, tref f2);

// The formulas need to be closed
template <NodeType node>
bool are_tau_equivalent(tref f1, tref f2);

template <NodeType node>
tref simp_tau_unsat_valid(tref fm, const int_t start_time = 0,
				const bool output = false);

} // namespace idni::tau_lang

#include "satisfiability.tmpl.h"

#endif // __IDNI__TAU__SATISFIABILITY_H__