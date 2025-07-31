// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__NORMALIZER_H__
#define __IDNI__TAU__NORMALIZER_H__

#include "normal_forms.h"

// TODO (MEDIUM) fix proper types (alias) at this level of abstraction
//
// We should talk about statement, nso_rr (nso_with_rr?), library, rule, builder,
// bindings, etc... instead of sp_tau_node,...

namespace idni::tau_lang {

// ref offset info. first is offset type (num/capture/shift/variable)
// and second is value of num, rr_dict id of capture or 0 for shift and variable
using offset_t = std::pair<tau_parser::nonterminal, size_t>;

template <NodeType node>
tref normalizer_step(tref form);

template <NodeType node>
tref normalize_non_temp(tref fm);

template <NodeType node>
trefs get_vars_from_nso(tref n);

template <NodeType node>
int_t get_new_var_id(tref fm);

template <NodeType node>
tref get_new_uninterpreted_constant(tref fm, const std::string& name, size_t type);

template <NodeType node>
std::pair<rr_sig, std::vector<offset_t>> get_ref_info(tref ref);

template <NodeType node>
tref get_ref(tref n);

template <NodeType node>
bool has_no_boolean_combs_of_models(tref n);

template <NodeType node>
bool is_non_temp_nso_satisfiable(tref n);

template <NodeType node>
bool is_non_temp_nso_unsat(tref n);

template <NodeType node>
bool are_nso_equivalent(tref n1, tref n2);

template <NodeType node>
bool is_nso_impl(tref n1, tref n2);

template <NodeType node>
bool are_bf_equal(tref n1, tref n2);

template <NodeType node>
bool is_bf_same_to_any_of(tref n, trefs& previous);

template <NodeType node>
tref normalize_with_temp_simp(tref fm);

template <NodeType node>
size_t get_max_lookback_in_rr(tref fm);

template <NodeType node>
tref build_shift_from_shift(tref shift, size_t step);

template <NodeType node>
tref build_main_step(tref form, size_t step);

template <NodeType node>
tref bf_normalizer_without_rec_relation(tref bf);

template <NodeType node>
tref bf_normalizer_with_rec_relation(const rr<node> &bf);

template <NodeType node>
tref build_enumerated_main_step(tref form, size_t i, size_t offset_arity);

template <NodeType node>
bool is_valid(const rr<node>& nso_rr);

template <NodeType node>
bool is_well_founded(const rr<node>& nso_rr);

template <NodeType node>
tref calculate_fixed_point(const rr<node>& nso_rr, tref form, typename node::type t,
	size_t offset_arity, tref fallback);

template <NodeType node>
tref calculate_all_fixed_points(const rr<node>& recrel);

template <NodeType node>
tref apply_rr_to_formula(const rr<node>& nso_rr);

template <NodeType node>
tref normalizer(const rr<node>& fm);

template <NodeType node>
tref normalizer(tref fm);

} // namespace idni::tau_lang

#include "normalizer.tmpl.h"

#endif // __IDNI__TAU__NORMALIZER_H__
