/**
 * @file tau_tree_builders.h
 * @brief Free-function builder API mirroring `tree<node>` static builders.
 *
 * Provides free-function wrappers for every `tree<node>::build_*` static
 * method so callers can use `build_wff_or<node>(l, r)` syntax when they do
 * not have a `tree<node>` object handy.
 */

// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__TAU_TREE_BUILDERS_H__
#define __IDNI__TAU__TAU_TREE_BUILDERS_H__

#include "tau_tree.h"

namespace idni::tau_lang {

/** @brief Assign stable canonical ids to all quantifiers in @p fm. */
template <NodeType node>
tref canonize_quantifier_ids(tref fm);

// ---------------------------------------------------------------------------
// Constant / leaf builders (functional API mirrors of tree<node>::_0 etc.)
// ---------------------------------------------------------------------------

/** @brief Return the BA zero constant for @p type_id. */
template <NodeType node> tref _0(size_t type_id);
/** @brief Return the BA one constant for @p type_id. */
template <NodeType node> tref _1(size_t type_id);
/** @brief Return the wff `F` constant. */
template <NodeType node> tref _F();
/** @brief Return the wff `T` constant. */
template <NodeType node> tref _T();
/** @brief Return the trimmed BA zero for @p type_id. */
template <NodeType node> tref _0_trimmed(size_t type_id);
/** @brief Return the trimmed BA one for @p type_id. */
template <NodeType node> tref _1_trimmed(size_t type_id);
/** @brief Return the trimmed wff `F`. */
template <NodeType node> tref _F_trimmed();
/** @brief Return the trimmed wff `T`. */
template <NodeType node> tref _T_trimmed();

/** @brief Return the `tree` for BA zero of @p type_id. */
template <NodeType node> const tree<node>& get_0(size_t type_id);
/** @brief Return the `tree` for BA one of @p type_id. */
template <NodeType node> const tree<node>& get_1(size_t type_id);
/** @brief Return the `tree` for wff `F`. */
template <NodeType node> const tree<node>& get_F();
/** @brief Return the `tree` for wff `T`. */
template <NodeType node> const tree<node>& get_T();
/** @brief Return the trimmed `tree` for BA zero of @p type_id. */
template <NodeType node> const tree<node>& get_0_trimmed(size_t type_id);
/** @brief Return the trimmed `tree` for BA one of @p type_id. */
template <NodeType node> const tree<node>& get_1_trimmed(size_t type_id);
/** @brief Return the trimmed `tree` for wff `F`. */
template <NodeType node> const tree<node>& get_F_trimmed();
/** @brief Return the trimmed `tree` for wff `T`. */
template <NodeType node> const tree<node>& get_T_trimmed();

// ---------------------------------------------------------------------------
// Formula (wff) builders
// ---------------------------------------------------------------------------

/** @brief Build `sometimes n`. */
template <NodeType node> tref build_wff_sometimes(tref n);
/** @brief Build `always n`. */
template <NodeType node> tref build_wff_always(tref n);
/** @brief Build `x ? y : z`. */
template <NodeType node> tref build_wff_conditional(tref x, tref y, tref z);
/** @brief Build `all bound_var. subformula`. */
template <NodeType node> tref build_wff_all(tref bound_var, tref subformula, bool calculate_quant_id = true);
/** @brief Build `all bound_vars. subformula`. */
template <NodeType node> tref build_wff_all_many(const trefs& bound_vars, tref subformula);
/** @brief Build `ex bound_var. subformula`. */
template <NodeType node> tref build_wff_ex(tref bound_var, tref subformula, bool calculate_quant_id = true);
/** @brief Build `ex bound_vars. subformula`. */
template <NodeType node> tref build_wff_ex_many(const trefs& bound_vars, tref subformula);
/** @brief Build `l -> r`. */
template <NodeType node> tref build_wff_imply(tref l, tref r);
/** @brief Build `l <- r`. */
template <NodeType node> tref build_wff_rimply(tref l, tref r);
/** @brief Build `l <-> r`. */
template <NodeType node> tref build_wff_equiv(tref l, tref r);
/** @brief Build `l | r`. */
template <NodeType node> tref build_wff_or(tref l, tref r);
/** @brief Build disjunction over @p wffs. */
template <NodeType node> tref build_wff_or(const auto& wffs);
/** @brief Build `l ^ r`. */
template <NodeType node> tref build_wff_xor(tref l, tref r);
/** @brief Build `l & r`. */
template <NodeType node> tref build_wff_and(tref l, tref r);
/** @brief Build conjunction over @p wffs. */
template <NodeType node> tref build_wff_and(const auto& wffs);
/** @brief Build `! n`. */
template <NodeType node> tref build_wff_neg(tref n);

// ---------------------------------------------------------------------------
// Constraint builders
// ---------------------------------------------------------------------------

/** @brief Build `ctnvar != num`. */
template <NodeType node> tref build_wff_ctn_neq(tref ctnvar, tref num);
/** @brief Build `ctnvar == num`. */
template <NodeType node> tref build_wff_ctn_eq(tref ctnvar, tref num);
/** @brief Build `ctnvar >= num`. */
template <NodeType node> tref build_wff_ctn_gteq(tref ctnvar, tref num);
/** @brief Build `ctnvar > num`. */
template <NodeType node> tref build_wff_ctn_gt(tref ctnvar, tref num);
/** @brief Build `ctnvar <= num`. */
template <NodeType node> tref build_wff_ctn_lteq(tref ctnvar, tref num);
/** @brief Build `ctnvar < num`. */
template <NodeType node> tref build_wff_ctn_lt(tref ctnvar, tref num);

// ---------------------------------------------------------------------------
// Term relational builders (wff using bf)
// ---------------------------------------------------------------------------

/** @brief Build `x[y, z]` interval formula. */
template <NodeType node> tref build_bf_interval(tref x, tref y, tref z);
/** @brief Build `l = r`. */
template <NodeType node> tref build_bf_eq(tref l, tref r);
/** @brief Build `l = 0`. */
template <NodeType node> tref build_bf_eq_0(tref l);
/** @brief Build `l = 1`. */
template <NodeType node> tref build_bf_eq_1(tref l);
/** @brief Build `l != r`. */
template <NodeType node> tref build_bf_neq(tref l, tref r);
/** @brief Build `l != 0`. */
template <NodeType node> tref build_bf_neq_0(tref l);
/** @brief Build `l <= r`. */
template <NodeType node> tref build_bf_lteq(tref l, tref r);
/** @brief Build `!(l <= r)`. */
template <NodeType node> tref build_bf_nlteq(tref l, tref r);
/** @brief Build `l > r`. */
template <NodeType node> tref build_bf_gt(tref l, tref r);
/** @brief Build `!(l > r)`. */
template <NodeType node> tref build_bf_ngt(tref l, tref r);
/** @brief Build `l >= r`. */
template <NodeType node> tref build_bf_gteq(tref l, tref r);
/** @brief Build `!(l >= r)`. */
template <NodeType node> tref build_bf_ngteq(tref l, tref r);
/** @brief Build `l < r`. */
template <NodeType node> tref build_bf_lt(tref l, tref r);
/** @brief Build `!(l < r)`. */
template <NodeType node> tref build_bf_nlt(tref l, tref r);

// ---------------------------------------------------------------------------
// Term (bf) builders
// ---------------------------------------------------------------------------

/** @brief Build `fall l r`. */
template <NodeType node> tref build_bf_fall(tref l, tref r);
/** @brief Build `fex l r`. */
template <NodeType node> tref build_bf_fex(tref l, tref r);
/** @brief Build `l | r`. */
template <NodeType node> tref build_bf_or(tref l, tref r);
/** @brief Build disjunction of @p bfs with type @p type_id. */
template <NodeType node> tref build_bf_or(const auto& bfs, size_t type_id);
/** @brief Build `l nor r`. */
template <NodeType node> tref build_bf_nor(tref l, tref r);
/** @brief Build `l ^ r`. */
template <NodeType node> tref build_bf_xor(tref l, tref r);
/** @brief Build `l xnor r`. */
template <NodeType node> tref build_bf_xnor(tref l, tref r);
/** @brief Build `l & r`. */
template <NodeType node> tref build_bf_and(tref l, tref r);
/** @brief Build conjunction of @p bfs with type @p type_id. */
template <NodeType node> tref build_bf_and(const auto& bfs, size_t type_id);
/** @brief Build `l nand r`. */
template <NodeType node> tref build_bf_nand(tref l, tref r);
/** @brief Build `! l`. */
template <NodeType node> tref build_bf_neg(tref l);
/** @brief Build `l << r`. */
template <NodeType node> tref build_bf_shl(tref l, tref r);
/** @brief Build `l >> r`. */
template <NodeType node> tref build_bf_shr(tref l, tref r);
/** @brief Build `l + r`. */
template <NodeType node> tref build_bf_add(tref l, tref r);
/** @brief Build `l - r`. */
template <NodeType node> tref build_bf_sub(tref l, tref r);
/** @brief Build `l * r`. */
template <NodeType node> tref build_bf_mul(tref l, tref r);
/** @brief Build `l / r`. */
template <NodeType node> tref build_bf_div(tref l, tref r);
/** @brief Build `l % r`. */
template <NodeType node> tref build_bf_mod(tref l, tref r);
/** @brief Build a cast of @p operand to @p target_type_id. */
template <NodeType node> tref build_bf_cast(tref operand, size_t target_type_id);

// ---------------------------------------------------------------------------
// Terminal, variable and constant builders
// ---------------------------------------------------------------------------

/** @brief Build the `T` constant for type @p ba_tid. */
template <NodeType node> tref build_bf_t_type(size_t ba_tid);
/** @brief Build the `T` constant for type named @p type. */
template <NodeType node> tref build_bf_t_type(const std::string& type);
/** @brief Build the `F` constant for type @p ba_tid. */
template <NodeType node> tref build_bf_f_type(size_t ba_tid);
/** @brief Build the `F` constant for type named @p type. */
template <NodeType node> tref build_bf_f_type(const std::string& type);
/** @brief Build a raw BA-constant node. */
template <NodeType node> tref build_ba_constant(const typename node::constant& constant, size_t ba_type_id);
/** @brief Build a `bf` BA-constant node, optionally with right sibling @p right. */
template <NodeType node> tref build_bf_ba_constant(const typename node::constant& constant, size_t ba_type_id, tref right = nullptr);
/** @brief Build a bitvector BA-constant node. */
template <NodeType node> tref build_bv_ba_constant(const typename node::constant& constant, size_t ba_type_id);
/** @brief Build an uninterpreted constant node from names @p n1, @p n2. */
template <NodeType node> tref build_bf_uconst(const std::string& n1, const std::string& n2, size_t type_id);
/** @brief Build a variable-name node from string id @p sid. */
template <NodeType node> tref build_var_name(size_t sid);
/** @brief Build a variable-name node from @p name. */
template <NodeType node> tref build_var_name(const std::string& name);
/** @brief Build a variable-name node with auto-generated name from @p index. */
template <NodeType node> tref build_var_name_indexed(size_t index);
/** @brief Build a canonised I/O variable from @p name. */
template <NodeType node> tref build_canonized_io_var(const std::string& name);
/** @brief Build a typed variable with name node @p var_name_node. */
template <NodeType node> tref build_variable(tref var_name_node, size_t type_id);
/** @brief Build a typed variable from @p name. */
template <NodeType node> tref build_variable(const std::string& name, size_t type_id);
/** @brief Build a typed variable with auto name. */
template <NodeType node> tref build_variable(size_t type_id);
/** @brief Build a bf-level variable with name node. */
template <NodeType node> tref build_bf_variable(tref var_name_node, size_t type_id);
/** @brief Build a bf-level variable from @p name. */
template <NodeType node> tref build_bf_variable(const std::string& name, size_t type_id);
/** @brief Build a bf-level variable with auto name. */
template <NodeType node> tref build_bf_variable(size_t type_id);
/** @brief Build an input I/O variable. */
template <NodeType node> tref build_in_var(tref var_name_node, size_t type_id);
/** @brief Build an input I/O variable with explicit offset. */
template <NodeType node> tref build_in_var(tref var_name_node, tref offset_node, size_t type_id);
/** @brief Build an input variable at position @p pos. */
template <NodeType node> tref build_in_var_at_n(tref var_name_node, int_t pos, size_t type_id);
/** @brief Build an input variable at position @p pos from @p name. */
template <NodeType node> tref build_in_var_at_n(const std::string& name, int_t pos, size_t type_id);
/** @brief Build an indexed input variable at position @p pos. */
template <NodeType node> tref build_in_var_at_n_indexed(size_t index, int_t pos, size_t type_id);
/** @brief Build an input variable at time @p t. */
template <NodeType node> tref build_in_var_at_t(tref var_name_node, size_t type_id, std::string t = "t");
/** @brief Build an indexed input variable at time @p t. */
template <NodeType node> tref build_in_var_at_t_indexed(size_t index, size_t type_id, std::string t = "t");
/** @brief Build an input variable at `t - shift`. */
template <NodeType node> tref build_in_var_at_t_minus(tref var_name_node, size_t shift, size_t type_id, std::string t = "t");
/** @brief Build an input variable at `t - shift` from @p var_name. */
template <NodeType node> tref build_in_var_at_t_minus(const std::string& var_name, size_t shift, size_t type_id, std::string t = "t");
/** @brief Build an indexed input variable at `t - shift`. */
template <NodeType node> tref build_in_var_at_t_minus_indexed(size_t index, size_t shift, size_t type_id, std::string t = "t");
/** @brief Build an output I/O variable. */
template <NodeType node> tref build_out_var(tref var_name_node, size_t type_id);
/** @brief Build an output I/O variable with explicit offset. */
template <NodeType node> tref build_out_var(tref var_name_node, tref offset_node, size_t type_id);
/** @brief Build an output variable at position @p pos. */
template <NodeType node> tref build_out_var_at_n(tref var_name_node, int_t pos, size_t type_id);
/** @brief Build an output variable at position @p pos from @p var_name. */
template <NodeType node> tref build_out_var_at_n(const std::string& var_name, int_t pos, size_t type_id);
/** @brief Build an indexed output variable at position @p pos. */
template <NodeType node> tref build_out_var_at_n_indexed(size_t index, int_t pos, size_t type_id);
/** @brief Build an output variable at time @p t. */
template <NodeType node> tref build_out_var_at_t(tref var_name_node, size_t type_id, std::string t = "t");
/** @brief Build an indexed output variable at time @p t. */
template <NodeType node> tref build_out_var_at_t_indexed(size_t index, size_t type_id, std::string t = "t");
/** @brief Build an output variable at `t - shift`. */
template <NodeType node> tref build_out_var_at_t_minus(tref var_name_node, size_t shift, size_t type_id, std::string t = "t");
/** @brief Build an output variable at `t - shift` from @p var_name. */
template <NodeType node> tref build_out_var_at_t_minus(const std::string& var_name, size_t shift, size_t type_id, std::string t = "t");
/** @brief Build an indexed output variable at `t - shift`. */
template <NodeType node> tref build_out_var_at_t_minus_indexed(size_t index, size_t shift, size_t type_id, std::string t = "t");

/** @brief Build a full `spec` tree from recurrence relation @p nso_rr. */
template <NodeType node> tref build_spec(const rr<node>& nso_rr);

// ---------------------------------------------------------------------------
// Reference and symbol builders
// ---------------------------------------------------------------------------

/** @brief Build a symbol node from string id @p sid. */
template<NodeType node> tref build_sym(size_t sid);
/** @brief Build a symbol node from @p sym_name. */
template<NodeType node> tref build_sym(const std::string& sym_name);
/** @brief Build a ref-offsets node from @p offsets vector. */
template<NodeType node> tref build_ref_offsets(const trefs& offsets);
/** @brief Build a ref-offsets node from @p offset string. */
template<NodeType node> tref build_ref_offsets(const std::string& offset);
/** @brief Build a ref-offsets node from @p offsets string vector. */
template<NodeType node> tref build_ref_offsets(const std::vector<std::string>& offsets);
/** @brief Build a shift-offset node for @p var at @p shift. */
template<NodeType node> tref build_ref_shift_offset(tref var, size_t shift);
/** @brief Build a shift-offset node for @p var_name at @p shift with type @p type_id. */
template<NodeType node> tref build_ref_shift_offset(const std::string& var_name, size_t type_id, size_t shift);
/** @brief Build a `ref_args` node from @p args. */
template<NodeType node> tref build_ref_args(const trefs& args);
/** @brief Build a `ref_args` node from @p arg_names with type @p type_id. */
template<NodeType node> tref build_ref_args(const std::vector<std::string>& arg_names, size_t type_id);
/** @brief Build a reference `sym(args)`. */
template<NodeType node> tref build_ref(tref sym, const trefs& args);
/** @brief Build a reference from @p sym_name. */
template<NodeType node> tref build_ref(const std::string& sym_name, const trefs& args);
/** @brief Build a full RR reference `sym[offsets](args)`. */
template<NodeType node> tref build_rr_ref(tref sym, const trefs& offsets, const trefs& args);
/** @brief Build `sym_name[offsets](args)`. */
template<NodeType node> tref build_rr_ref(const std::string& sym_name, const trefs& offsets, const trefs& args);
/** @brief Build `sym_name[offset](args)` from a string offset. */
template<NodeType node> tref build_rr_ref(const std::string& sym_name, const std::string offset, const trefs& args);
/** @brief Build `sym_name[offset](args)` from an integer offset. */
template<NodeType node> tref build_rr_ref(const std::string& sym_name, size_t offset, const trefs& args);
/** @brief Build `sym[offset - shift](args)`. */
template<NodeType node> tref build_rr_ref(tref sym, tref offset, size_t shift, const trefs& args);
/** @brief Build `sym_name[offset - shift](args)`. */
template<NodeType node> tref build_rr_ref(const std::string& sym_name, tref offset, size_t shift, const trefs& args);
/** @brief Build `sym_name[offset_str - shift](args)`. */
template<NodeType node> tref build_rr_ref(const std::string& sym_name, const std::string& offset, size_t shift, const trefs& args);


} // namespace idni::tau_lang

#include "tau_tree_builders.tmpl.h"

#endif // __IDNI__TAU__TAU_TREE_BUILDERS_H__