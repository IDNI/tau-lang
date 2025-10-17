// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__TAU_TREE_BUILDERS_H__
#define __IDNI__TAU__TAU_TREE_BUILDERS_H__

#include "tau_tree.h"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// tree builders functional API
// -----------------------------------------------------------------------------

template <NodeType node>
tref _0();
template <NodeType node>
tref _1();
template <NodeType node>
tref _F();
template <NodeType node>
tref _T();
template <NodeType node>
tref _0_trimmed();
template <NodeType node>
tref _1_trimmed();
template <NodeType node>
tref _F_trimmed();
template <NodeType node>
tref _T_trimmed();

template <NodeType node>
const tree<node>& get_0();
template <NodeType node>
const tree<node>& get_1();
template <NodeType node>
const tree<node>& get_F();
template <NodeType node>
const tree<node>& get_T();
template <NodeType node>
const tree<node>& get_0_trimmed();
template <NodeType node>
const tree<node>& get_1_trimmed();
template <NodeType node>
const tree<node>& get_F_trimmed();
template <NodeType node>
const tree<node>& get_T_trimmed();

// -----------------------------------------------------------------------------
// formula builders

template <NodeType node>
tref build_wff_sometimes(tref n);

template <NodeType node>
tref build_wff_always(tref n);

template <NodeType node>
tref build_wff_conditional(tref x, tref y, tref z);

template <NodeType node>
tref build_wff_all(tref l, tref r);

template <NodeType node>
tref build_wff_ex(tref l, tref r);

template <NodeType node>
tref build_wff_imply(tref l, tref r);

template <NodeType node>
tref build_wff_rimply(tref l, tref r);

template <NodeType node>
tref build_wff_equiv(tref l, tref r);

template <NodeType node>
tref build_wff_or(tref l, tref r);

template <NodeType node>
tref build_wff_or(const auto& wffs);

template <NodeType node>
tref build_wff_xor_from_def(tref l, tref r);

template <NodeType node>
tref build_wff_xor(tref l, tref r);

template <NodeType node>
tref build_wff_and(tref l, tref r);

template <NodeType node>
tref build_wff_and(const auto& wffs);

template <NodeType node>
tref build_wff_neg(tref n);

// -----------------------------------------------------------------------------
// constraint builders

template <NodeType node>
tref build_wff_ctn_neq(tref ctnvar, tref num);

template <NodeType node>
tref build_wff_ctn_eq(tref ctnvar, tref num);

template <NodeType node>
tref build_wff_ctn_gteq(tref ctnvar, tref num);

template <NodeType node>
tref build_wff_ctn_gt(tref ctnvar, tref num);

template <NodeType node>
tref build_wff_ctn_lteq(tref ctnvar, tref num);

template <NodeType node>
tref build_wff_ctn_lt(tref ctnvar, tref num);

// -----------------------------------------------------------------------------
// term builders

template <NodeType node>
tref build_bf_interval(tref x, tref y, tref z);

template <NodeType node>
tref build_bf_eq(tref l, tref r);

template <NodeType node>
tref build_bf_eq(tref l);

template <NodeType node>
tref build_bf_neq(tref l);

template <NodeType node>
tref build_bf_lteq(tref l, tref r);

template <NodeType node>
tref build_bf_nlteq(tref l, tref r);

template <NodeType node>
tref build_bf_nlteq_lower(tref l, tref r);

template <NodeType node>
tref build_bf_nlteq_upper(tref l, tref r);

template <NodeType node>
tref build_bf_gt(tref l, tref r);

template <NodeType node>
tref build_bf_ngt(tref l, tref r);

template <NodeType node>
tref build_bf_gteq(tref l, tref r);

template <NodeType node>
tref build_bf_ngteq(tref l, tref r);

template <NodeType node>
tref build_bf_lt(tref l, tref r);

template <NodeType node>
tref build_bf_nlt(tref l, tref r);

// -----------------------------------------------------------------------------
// term builders

template <NodeType node>
tref build_bf_fall(tref l, tref r);

template <NodeType node>
tref build_bf_fex(tref l, tref r);

template <NodeType node>
tref build_bf_or(tref l, tref r);

template <NodeType node>
tref build_bf_or(const auto& bfs);

template <NodeType node>
tref build_bf_xor_from_def(tref l, tref r);

template <NodeType node>
tref build_bf_xor(tref l, tref r);

template <NodeType node>
tref build_bf_and(tref l, tref r);

template <NodeType node>
tref build_bf_and(const auto& bfs);

template <NodeType node>
tref build_bf_neg(tref n);

template <NodeType node>
tref build_bf_shl(tref l, tref r);

template <NodeType node>
tref build_bf_shr(tref l, tref r);

template <NodeType node>
tref build_bf_add(tref l, tref r);

template <NodeType node>
tref build_bf_sub(tref l, tref r);

template <NodeType node>
tref build_bf_mul(tref l, tref r);

template <NodeType node>
tref build_bf_div(tref l, tref r);

template <NodeType node>
tref build_bf_mod(tref l, tref r);

// -----------------------------------------------------------------------------
// terminals, variables and constants

template <NodeType node>
tref build_bf_t_type(size_t ba_tid);

template <NodeType node>
tref build_bf_t_type(const std::string& type);

template <NodeType node>
tref build_bf_f_type(size_t ba_tid);

template <NodeType node>
tref build_bf_f_type(const std::string& type);

template <NodeType node>
tref build_ba_constant(const typename node::constant& constant,
                       size_t ba_type_id);

template <NodeType node>
tref build_bf_ba_constant(const typename node::constant& constant,
                          size_t ba_type_id, tref right = nullptr);

/*
template <NodeType node>
tref build_bv_constant(const typename node::constant& constant, size_t ba_type_id);
*/

template <NodeType node>
tref build_bv_ba_constant(const typename node::constant& constant, size_t ba_type_id);

template <NodeType node>
tref build_bf_uconst(const std::string& n1, const std::string& n2, size_t type_id);

template <NodeType node>
tref build_var_name(size_t sid);

template <NodeType node>
tref build_var_name(const std::string& name);

template <NodeType node>
tref build_var_name_indexed(size_t index);

template <NodeType node>
tref build_variable(tref var_name_node, size_t type_id);

template <NodeType node>
tref build_variable(const std::string& name, size_t type_id);

template <NodeType node>
tref build_bf_variable(tref var_name_node, size_t type_id);

template <NodeType node>
tref build_bf_variable(const std::string& name, size_t type_id);

template <NodeType node>
tref build_in_var(tref var_name_node, tref offset_node, size_t type_id);

template <NodeType node>
tref build_in_var_at_n(tref var_name_node, int_t pos, size_t type_id);

template <NodeType node>
tref build_in_var_at_n(const std::string& name, int_t pos, size_t type_id);

template <NodeType node>
tref build_in_var_at_n_indexed(size_t index, int_t pos, size_t type_id);

template <NodeType node>
tref build_in_var_at_t(tref var_name_node, size_t type_id, std::string t = "t");

template <NodeType node>
tref build_in_var_at_t_indexed(size_t index, size_t type_id, std::string t = "t");

template <NodeType node>
tref build_in_var_at_t_minus(tref var_name_node, size_t shift, size_t type_id, std::string t = "t");

template <NodeType node>
tref build_in_var_at_t_minus(const std::string& var_name, size_t shift, size_t type_id, std::string t = "t");

template <NodeType node>
tref build_in_var_at_t_minus_indexed(size_t index, size_t shift, size_t type_id, std::string t = "t");

template <NodeType node>
tref build_out_var(tref var_name_node, tref offset_node, size_t type_id);

template <NodeType node>
tref build_out_var_at_n(tref var_name_node, int_t pos, size_t type_id);

template <NodeType node>
tref build_out_var_at_n(const std::string& var_name, int_t pos, size_t type_id);

template <NodeType node>
tref build_out_var_at_n_indexed(size_t index, int_t pos, size_t type_id);

template <NodeType node>
tref build_out_var_at_t(tref var_name_node, size_t type_id, std::string t = "t");

template <NodeType node>
tref build_out_var_at_t_indexed(size_t index, size_t type_id, std::string t = "t");

template <NodeType node>
tref build_out_var_at_t_minus(tref var_name_node, size_t shift, size_t type_id, std::string t = "t");

template <NodeType node>
tref build_out_var_at_t_minus(const std::string& var_name, size_t shift, size_t type_id, std::string t = "t");

template <NodeType node>
tref build_out_var_at_t_minus_indexed(size_t index, size_t shift, size_t type_id, std::string t = "t");

} // namespace idni::tau_lang

#include "tau_tree_builders.tmpl.h"

#endif // __IDNI__TAU__TAU_TREE_BUILDERS_H__