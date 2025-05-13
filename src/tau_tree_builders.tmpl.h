// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include <numeric>

#include "tau_tree.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "builders"

namespace idni::tau_lang {

template <NodeType node>
tref tree<node>::apply_builder(const rewriter::builder& b, trefs n) {
	subtree_map<node, tref> changes;
	trefs vars = (tt(b.first->get()) || capture).values();
	for (size_t i = 0; i < vars.size(); ++i) changes[vars[i]] = n[i];
	return rewriter::replace<node>(b.second->get(), changes);
}

//------------------------------------------------------------------------------
// builders

template <NodeType node>
tref _0() { return get_0<node>().get(); }
template <NodeType node>
tref _1() { return get_1<node>().get(); }
template <NodeType node>
tref _F() { return get_F<node>().get(); }
template <NodeType node>
tref _T() { return get_T<node>().get(); }

template <NodeType node>
tref _0_trimmed() { return get_0_trimmed<node>().get(); }
template <NodeType node>
tref _1_trimmed() { return get_1_trimmed<node>().get(); }
template <NodeType node>
tref _F_trimmed() { return get_F_trimmed<node>().get(); }
template <NodeType node>
tref _T_trimmed() { return get_T_trimmed<node>().get(); }

template <NodeType node>
const tree<node>& get_0() {
	using tau = tree<node>;
	static htree::sp cached = tau::geth(tau::get(tau::bf, _0_trimmed<node>()));
	return tau::get(cached);
}

template <NodeType node>
const tree<node>& get_1() {
	using tau = tree<node>;
	static htree::sp cached = tau::geth(tau::get(tau::bf, _1_trimmed<node>()));
	return tau::get(cached);
}

template <NodeType node>
const tree<node>& get_F() {
	using tau = tree<node>;
	static htree::sp cached = tau::geth(tau::get(tau::wff, _F_trimmed<node>()));
	return tau::get(cached);
}

template <NodeType node>
const tree<node>& get_T() {
	using tau = tree<node>;
	static htree::sp cached = tau::geth(tau::get(tau::wff, _T_trimmed<node>()));
	return tau::get(cached);
}

template <NodeType node>
const tree<node>& get_0_trimmed() {
	using tau = tree<node>;
	static htree::sp cached = tau::geth(tau::get(tau::bf_f));
	return tau::get(cached);
}

template <NodeType node>
const tree<node>& get_1_trimmed() {
	using tau = tree<node>;
	static htree::sp cached = tau::geth(tau::get(tau::bf_t));
	return tau::get(cached);
}

template <NodeType node>
const tree<node>& get_F_trimmed() {
	using tau = tree<node>;
	static htree::sp cached = tau::geth(tau::get(tau::wff_f));
	return tau::get(cached);
}

template <NodeType node>
const tree<node>& get_T_trimmed() {
	using tau = tree<node>;
	static htree::sp cached = tau::geth(tau::get(tau::wff_t));
	return tau::get(cached);
}

// -----------------------------------------------------------------------------
// formula builders

template <NodeType node>
tref build_wff_sometimes(tref l) {
	DBG(assert(l != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::wff_sometimes, l));
}

template <NodeType node>
tref build_wff_always(tref l) {
	DBG(assert(l != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::wff_always, l));
}

template <NodeType node>
tref build_wff_conditional(tref x, tref y, tref z) {
	DBG(assert(x != nullptr && y != nullptr && z != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::wff_and,
		build_wff_imply<node>(x, y),
		build_wff_imply<node>(build_wff_neg<node>(x), z)));
}

template <NodeType node>
tref build_wff_all(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::wff_all, l, r));
}

template <NodeType node>
tref build_wff_ex(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::wff_ex, l, r));
}

template <NodeType node>
tref build_wff_imply(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::wff_or,
		build_wff_neg<node>(l), r));
}

template <NodeType node>
tref build_wff_rimply(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::wff_or,
		build_wff_neg<node>(r), l));
}

template <NodeType node>
tref build_wff_equiv(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::wff_and,
		build_wff_imply<node>(l, r),
		build_wff_imply<node>(r, l)));
}

template <NodeType node>
tref build_wff_or(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::wff_or, l, r));
}

template <NodeType node>
tref build_wff_or(const auto& wffs) {
	return std::accumulate(wffs.begin(), wffs.end(), _F<node>(),
		[](tref l, tref r) { return build_wff_or<node>(l, r); });
}

template <NodeType node>
tref build_wff_xor_from_def(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::wff_or,
		build_wff_and<node>(build_wff_neg<node>(l), r),
		build_wff_and<node>(build_wff_neg<node>(r), l)));
}

template <NodeType node>
tref build_wff_xor(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::wff_or,
		build_wff_and<node>(build_wff_neg<node>(l), r),
		build_wff_and<node>(build_wff_neg<node>(r), l)));
}

template <NodeType node>
tref build_wff_and(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::wff_and, l, r));
}

template <NodeType node>
tref build_wff_and(const auto& wffs) {
	return std::accumulate(wffs.begin(), wffs.end(), _T<node>(),
		[](tref l, tref r) { return build_wff_and<node>(l, r); });
}

template <NodeType node>
tref build_wff_neg(tref l) {
	DBG(assert(l != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::wff_neg, l));
}

// -----------------------------------------------------------------------------
// constraint builders

template <NodeType node>
tref build_wff_ctn_gteq(tref ctnvar, tref num) {
	DBG(assert(ctnvar != nullptr && num != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::constraint,
		tau::get(tau::ctn_gteq, { ctnvar, num })));
}

template <NodeType node>
tref build_wff_ctn_gt(tref ctnvar, tref num) {
	DBG(assert(ctnvar != nullptr && num != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::constraint,
		tau::get(tau::ctn_gt, { ctnvar, num })));
}

template <NodeType node>
tref build_wff_ctn_lteq(tref ctnvar, tref num) {
	DBG(assert(ctnvar != nullptr && num != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::constraint,
		tau::get(tau::ctn_lteq, { ctnvar, num })));
}

template <NodeType node>
tref build_wff_ctn_lt(tref ctnvar, tref num) {
	DBG(assert(ctnvar != nullptr && num != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::constraint,
		tau::get(tau::ctn_lt, { ctnvar, num })));
}

template <NodeType node>
tref build_wff_ctn_eq(tref ctnvar, tref num) {
	DBG(assert(ctnvar != nullptr && num != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::constraint,
		tau::get(tau::ctn_eq, { ctnvar, num })));
}

template <NodeType node>
tref build_wff_ctn_neq(tref ctnvar, tref num) {
	DBG(assert(ctnvar != nullptr && num != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::constraint,
		tau::get(tau::ctn_neq, { ctnvar, num })));
}

// -----------------------------------------------------------------------------
// wff relational operators of terms

template <NodeType node>
tref build_bf_interval(tref x, tref y, tref z) {
	DBG(assert(x != nullptr && y != nullptr && z != nullptr);)
	return build_wff_and<node>(build_bf_lteq<node>(x, y),
		build_bf_lteq<node>(y, z));
}

template <NodeType node>
tref build_bf_eq(tref l, tref r) {
	// DBG(assert(is<node>(l, bf) && is<node>(r, bf));)
	DBG(assert(l != nullptr && r != nullptr);)
	auto left_side = build_bf_xor<node>(l,r);
	return build_bf_eq<node>(left_side);
}

template <NodeType node>
tref build_bf_eq(tref l) {
	DBG(assert(l != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::bf_eq, l, _0<node>()));
}

template <NodeType node>
tref build_bf_neq(tref l) {
	DBG(assert(l != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::bf_neq, l, _0<node>()));
}

template <NodeType node>
tref build_bf_lteq(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return build_bf_eq<node>(build_bf_and<node>(l, build_bf_neg<node>(r)));
}

template <NodeType node>
tref build_bf_nlteq(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return build_bf_neq<node>(build_bf_and<node>(l, build_bf_neg<node>(r)));
}

template <NodeType node>
tref build_bf_nlteq_lower(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return build_bf_neq<node>(build_bf_and<node>(r, build_bf_neg<node>(l)));
}

template <NodeType node>
tref build_bf_nlteq_upper(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return build_bf_neq<node>(build_bf_and<node>(l, build_bf_neg<node>(r)));
}

template <NodeType node>
tref build_bf_gt(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return build_bf_lt<node>(r, l);
}

template <NodeType node>
tref build_bf_ngt(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return build_wff_neg<node>(build_bf_gt<node>(l, r));
}

template <NodeType node>
tref build_bf_gteq(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return build_bf_lteq<node>(r, l);
}

template <NodeType node>
tref build_bf_ngteq(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return build_wff_neg<node>(build_bf_gteq<node>(l, r));
}

template <NodeType node>
tref build_bf_lt(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return build_bf_and<node>(
		build_bf_eq<node>(build_bf_and<node>(l, build_bf_neg<node>(r))),
		build_bf_neq<node>(build_bf_xor<node>(l, r)));
}

template <NodeType node>
tref build_bf_nlt(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return build_wff_neg<node>(build_bf_lt<node>(l, r));
}

// -----------------------------------------------------------------------------
// term builders

template <NodeType node>
tref build_bf_or(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::bf, tau::get(tau::bf_or, l, r));
}

template <NodeType node>
tref build_bf_or(const auto& bfs) {
	return std::accumulate(bfs.begin(), bfs.end(), _0<node>(),
		[](tref l, tref r) { return build_bf_or<node>(l, r); });
}

template <NodeType node>
tref build_bf_xor_from_def(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return build_bf_or<node>(
		build_bf_and<node>(build_bf_neg<node>(l), r),
		build_bf_and<node>(build_bf_neg<node>(r), l));
}

template <NodeType node>
tref build_bf_xor(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return build_bf_or<node>(
		build_bf_and<node>(build_bf_neg<node>(l), r),
		build_bf_and<node>(l, build_bf_neg<node>(r)));
}

template <NodeType node>
tref build_bf_and(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::bf, tau::get(tau::bf_and, l, r));
}

template <NodeType node>
tref build_bf_and(const auto& bfs) {
	return std::accumulate(bfs.begin(), bfs.end(), _1<node>(),
		[](tref l, tref r) { return build_bf_and<node>(l, r);});
}

template <NodeType node>
tref build_bf_neg(tref l) {
	DBG(assert(l != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::bf, tau::get(tau::bf_neg, l));
}

// -----------------------------------------------------------------------------
// terminals, variables and constants

template <NodeType node>
tref build_bf_t_type(size_t ba_tid) {
	DBG(assert(ba_tid > 0);)
	using tau = tree<node>;
	return tau::get(tau::bf, tau::get(node::ba_typed(tau::bf_t, ba_tid)));
}

template <NodeType node>
tref build_bf_t_type(const std::string& type) {
	return build_bf_t_type<node>(get_ba_type_id<node>(type));
}

template <NodeType node>
tref build_bf_f_type(size_t ba_tid) {
	DBG(assert(ba_tid > 0);)
	using tau = tree<node>;
	return tau::get(tau::bf, tau::get(node::ba_typed(tau::bf_f, ba_tid)));
}

template <NodeType node>
tref build_bf_f_type(const std::string& type) {
	return build_bf_f_type<node>(get_ba_type_id<node>(type));
}

template <NodeType node>
tref build_ba_constant(typename node::bas_variant v, size_t ba_tid) {
	return ba_constants_binder<node>::instance().bind(v, ba_tid);
}

template <NodeType node>
tref build_bf_ba_constant(typename node::bas_variant v, size_t ba_tid, tref r) {
	using tau = tree<node>;
	return tau::get(tau::bf, build_ba_constant<node>(v, ba_tid), r);
}

template <NodeType node>
tref build_bf_uconst(const std::string& n1,
	const std::string& n2)
{
	using tau = tree<node>;
	return tau::get(tau::bf, tau::get(tau::variable,
			tau::get(tau::uconst_name, n1 + ":" + n2)));
}

template <NodeType node>
tref build_var_name(size_t sid) {
	using tau = tree<node>;
	return tau::get(node(tau::var_name, sid));
}

template <NodeType node>
tref build_var_name(const std::string& name) {
	using tau = tree<node>;
	return tau::get(tau::var_name, name);
}

template <NodeType node>
tref build_var_name_indexed(size_t index) {
	std::stringstream name;
	return build_var_name<node>((name << "i" << index, name.str()));
}

template <NodeType node>
tref build_variable(tref var_name_node) {
	using tau = tree<node>;
	return tau::get(tau::variable, var_name_node);
}

template <NodeType node>
tref build_variable(const std::string& name) {
	using tau = tree<node>;
	return tau::get(tau::variable, build_var_name<node>(name));
}

template <NodeType node>
tref build_bf_variable(tref var_name_node) {
	using tau = tree<node>;
	return tau::get(tau::bf, build_variable<node>(var_name_node));
}

template <NodeType node>
tref build_bf_variable(const std::string& name) {
	using tau = tree<node>;
	return tau::get(tau::bf, build_variable<node>(name));
}

template <NodeType node>
tref build_in_var(tref var_name_node, tref offset_node) {
	using tau = tree<node>;
	return tau::get(tau::bf, tau::get(tau::variable,
		tau::get(node::input_variable(), var_name_node, offset_node)));
}

template <NodeType node>
tref build_in_var_at_n(tref var_name_node, int_t pos) {
	using tau = tree<node>;
	DBG(assert(var_name_node != nullptr);)
	DBG(assert(tau::get(var_name_node).is(tau::var_name));)
	return build_in_var<node>(var_name_node,
		tau::get(tau::offset, tau::get_integer(pos)));
}

template <NodeType node>
tref build_in_var_at_n(const std::string& name, int_t pos) {
	return build_in_var_at_n<node>(build_var_name<node>(name), pos);
}

template <NodeType node>
tref build_in_var_at_n_indexed(size_t index, int_t pos) {
	return build_in_var_at_n<node>(
		build_var_name_indexed<node>(index), pos);
}

template <NodeType node>
tref build_in_var_at_t(tref var_name_node, std::string t) {
	using tau = tree<node>;
	DBG(assert(var_name_node != nullptr);)
	DBG(assert(tau::get(var_name_node).is(tau::var_name));)
	return build_in_var<node>(var_name_node,
		tau::get(tau::offset, build_variable<node>(t)));
}

template <NodeType node>
tref build_in_var_at_t_indexed(size_t index, std::string t) {
	return build_in_var_at_t<node>(build_var_name_indexed<node>(index), t);
}

template <NodeType node>
tref build_in_var_at_t_minus(tref var_name_node, size_t shift, std::string t) {
	using tau = tree<node>;
	DBG(assert(var_name_node != nullptr);)
	DBG(assert(tau::get(var_name_node).is(tau::var_name));)
	return build_in_var<node>(var_name_node,
		tau::get(tau::offset, tau::get(tau::shift,
			tau::get(tau::variable, t), tau::get_num(shift))));
}

template <NodeType node>
tref build_in_var_at_t_minus(const std::string var_name, size_t shift, std::string t) {
	return build_in_var_at_t_minus<node>(
		build_var_name<node>(var_name), shift, t);
}

template <NodeType node>
tref build_in_var_at_t_minus_indexed(size_t index, size_t shift, std::string t) {
	return build_in_var_at_t_minus<node>(
		build_var_name_indexed<node>(index), shift, t);
}

template <NodeType node>
tref build_out_var(tref var_name_node, tref offset_node) {
	using tau = tree<node>;
	return tau::get(tau::bf, tau::get(tau::variable,
		tau::get(node::output_variable(), var_name_node, offset_node)));
}

template <NodeType node>
tref build_out_var_at_n(tref var_name_node, int_t pos) {
	using tau = tree<node>;
	return build_out_var<node>(var_name_node,
		tau::get(tau::offset, tau::get_integer(pos)));
}

template <NodeType node>
tref build_out_var_at_n(const std::string& name, int_t pos) {
	return build_out_var_at_n<node>(build_var_name<node>(name), pos);
}

template <NodeType node>
tref build_out_var_at_n_indexed(size_t index, int_t pos) {
	return build_out_var_at_n<node>(
		build_var_name_indexed<node>(index), pos);
}

template <NodeType node>
tref build_out_var_at_t(tref var_name_node, std::string t) {
	using tau = tree<node>;
	DBG(assert(var_name_node != nullptr);)
	DBG(assert(tau::get(var_name_node).is(tau::var_name));)
	return build_out_var<node>(var_name_node,
		tau::get(tau::offset, build_variable<node>(t)));
}

template <NodeType node>
tref build_out_var_at_t_indexed(size_t index, std::string t) {
	return build_out_var_at_t<node>(
		build_var_name_indexed<node>(index), t);
}

template <NodeType node>
tref build_out_var_at_t_minus(tref var_name_node, size_t shift, std::string t) {
	using tau = tree<node>;
	return build_out_var<node>(var_name_node,
		tau::get(tau::offset, tau::get(tau::shift,
			tau::get(tau::variable, t), tau::get_num(shift))));
}

template <NodeType node>
tref build_out_var_at_t_minus(const std::string& name, size_t shift, std::string t) {
	return build_out_var_at_t_minus<node>(build_var_name<node>(name), shift, t);
}

template <NodeType node>
tref build_out_var_at_t_minus_indexed(size_t index, size_t shift, std::string t) {
	return build_out_var_at_t_minus<node>(
		build_var_name_indexed<node>(index), shift, t);
}

// -----------------------------------------------------------------------------
// same builders on the tree API
// -----------------------------------------------------------------------------

template <NodeType node>
tref tree<node>::_0() { return tau_lang::_0<node>(); }
template <NodeType node>
tref tree<node>::_1() { return tau_lang::_1<node>(); }
template <NodeType node>
tref tree<node>::_F() { return tau_lang::_F<node>(); }
template <NodeType node>
tref tree<node>::_T() { return tau_lang::_T<node>(); }
template <NodeType node>
tref tree<node>::_0_trimmed() { return tau_lang::_0_trimmed<node>(); }
template <NodeType node>
tref tree<node>::_1_trimmed() { return tau_lang::_1_trimmed<node>(); }
template <NodeType node>
tref tree<node>::_F_trimmed() { return tau_lang::_F_trimmed<node>(); }
template <NodeType node>
tref tree<node>::_T_trimmed() { return tau_lang::_T_trimmed<node>(); }

template <NodeType node>
const tree<node>& tree<node>::get_0() { return tau_lang::get_0<node>(); }
template <NodeType node>
const tree<node>& tree<node>::get_1() { return tau_lang::get_1<node>(); }
template <NodeType node>
const tree<node>& tree<node>::get_F() { return tau_lang::get_F<node>(); }
template <NodeType node>
const tree<node>& tree<node>::get_T() { return tau_lang::get_T<node>(); }
template <NodeType node>
const tree<node>& tree<node>::get_0_trimmed() { return tau_lang::get_0_trimmed<node>(); }
template <NodeType node>
const tree<node>& tree<node>::get_1_trimmed() { return tau_lang::get_1_trimmed<node>(); }
template <NodeType node>
const tree<node>& tree<node>::get_F_trimmed() { return tau_lang::get_F_trimmed<node>(); }
template <NodeType node>
const tree<node>& tree<node>::get_T_trimmed() { return tau_lang::get_T_trimmed<node>(); }

// -----------------------------------------------------------------------------
// formula builders

template <NodeType node>
tref tree<node>::build_wff_sometimes(tref l) {
	return tau_lang::build_wff_sometimes<node>(l);
}

template <NodeType node>
tref tree<node>::build_wff_always(tref l) {
	return tau_lang::build_wff_always<node>(l);
}

template <NodeType node>
tref tree<node>::build_wff_conditional(tref x, tref y, tref z) {
	return tau_lang::build_wff_conditional<node>(x, y, z);
}

template <NodeType node>
tref tree<node>::build_wff_all(tref l, tref r) {
	return tau_lang::build_wff_all<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_wff_ex(tref l, tref r) {
	return tau_lang::build_wff_ex<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_wff_imply(tref l, tref r) {
	return tau_lang::build_wff_imply<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_wff_rimply(tref l, tref r) {
	return tau_lang::build_wff_rimply<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_wff_equiv(tref l, tref r) {
	return tau_lang::build_wff_equiv<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_wff_or(tref l, tref r) {
	return tau_lang::build_wff_or<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_wff_or(const auto& wffs) {
	return tau_lang::build_wff_or<node>(wffs);
}

template <NodeType node>
tref tree<node>::build_wff_xor_from_def(tref l, tref r) {
	return tau_lang::build_wff_xor_from_def<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_wff_xor(tref l, tref r) {
	return tau_lang::build_wff_xor<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_wff_and(tref l, tref r) {
	return tau_lang::build_wff_and<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_wff_and(const auto& wffs) {
	return tau_lang::build_wff_and<node>(wffs);
}

template <NodeType node>
tref tree<node>::build_wff_neg(tref l) {
	return tau_lang::build_wff_neg<node>(l);
}

// -----------------------------------------------------------------------------
// constraint builders

template <NodeType node>
tref tree<node>::build_wff_ctn_gteq(tref ctnvar, tref num) {
	return tau_lang::build_wff_ctn_gteq<node>(ctnvar, num);
}

template <NodeType node>
tref tree<node>::build_wff_ctn_gt(tref ctnvar, tref num) {
	return tau_lang::build_wff_ctn_gt<node>(ctnvar, num);
}

template <NodeType node>
tref tree<node>::build_wff_ctn_lteq(tref ctnvar, tref num) {
	return tau_lang::build_wff_ctn_lteq<node>(ctnvar, num);
}

template <NodeType node>
tref tree<node>::build_wff_ctn_lt(tref ctnvar, tref num) {
	return tau_lang::build_wff_ctn_lt<node>(ctnvar, num);
}

template <NodeType node>
tref tree<node>::build_wff_ctn_eq(tref ctnvar, tref num) {
	return tau_lang::build_wff_ctn_eq<node>(ctnvar, num);
}

template <NodeType node>
tref tree<node>::build_wff_ctn_neq(tref ctnvar, tref num) {
	return tau_lang::build_wff_ctn_neq<node>(ctnvar, num);
}

// -----------------------------------------------------------------------------
// wff relational operators of terms

template <NodeType node>
tref tree<node>::build_bf_interval(tref x, tref y, tref z) {
	return tau_lang::build_bf_interval<node>(x, y, z);
}

template <NodeType node>
tref tree<node>::build_bf_eq(tref l, tref r) {
	return tau_lang::build_bf_eq<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_eq(tref l) {
	return tau_lang::build_bf_eq<node>(l);
}

template <NodeType node>
tref tree<node>::build_bf_neq(tref l) {
	return tau_lang::build_bf_neq<node>(l);
}

template <NodeType node>
tref tree<node>::build_bf_lteq(tref l, tref r) {
	return tau_lang::build_bf_lteq<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_nlteq(tref l, tref r) {
	return tau_lang::build_bf_nlteq<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_nlteq_lower(tref l, tref r) {
	return tau_lang::build_bf_nlteq_lower<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_nlteq_upper(tref l, tref r) {
	return tau_lang::build_bf_nlteq_upper<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_gt(tref l, tref r) {
	return tau_lang::build_bf_gt<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_ngt(tref l, tref r) {
	return tau_lang::build_bf_ngt<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_gteq(tref l, tref r) {
	return tau_lang::build_bf_gteq<node>(r, l);
}

template <NodeType node>
tref tree<node>::build_bf_ngteq(tref l, tref r) {
	return tau_lang::build_bf_ngteq<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_lt(tref l, tref r) {
	return tau_lang::build_bf_lt<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_nlt(tref l, tref r) {
	return tau_lang::build_bf_nlt<node>(l, r);
}

// -----------------------------------------------------------------------------
// term builders

template <NodeType node>
tref tree<node>::build_bf_or(tref l, tref r) {
	return tau_lang::build_bf_or<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_or(const auto& bfs) {
	return tau_lang::build_bf_or<node>(bfs);
}

template <NodeType node>
tref tree<node>::build_bf_xor_from_def(tref l, tref r) {
	return tau_lang::build_bf_xor_from_def<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_xor(tref l, tref r) {
	return tau_lang::build_bf_xor<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_and(tref l, tref r) {
	return tau_lang::build_bf_and<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_and(const auto& bfs) {
	return tau_lang::build_bf_and<node>(bfs);
}

template <NodeType node>
tref tree<node>::build_bf_neg(tref l) {
	return tau_lang::build_bf_neg<node>(l);
}

// -----------------------------------------------------------------------------
// terminals, variables and constants

template <NodeType node>
tref tree<node>::build_bf_t_type(size_t ba_tid) {
	return tau_lang::build_bf_t_type<node>(ba_tid);
}

template <NodeType node>
tref tree<node>::build_bf_t_type(const std::string& type) {
	return tau_lang::build_bf_t_type<node>(get_ba_type_id<node>(type));
}

template <NodeType node>
tref tree<node>::build_bf_f_type(size_t ba_tid) {
	return tau_lang::build_bf_f_type<node>(ba_tid);
}

template <NodeType node>
tref tree<node>::build_bf_f_type(const std::string& type) {
	return tau_lang::build_bf_f_type<node>(get_ba_type_id<node>(type));
}

template <NodeType node>
tref tree<node>::build_ba_constant(node::bas_variant v, size_t ba_tid) {
	return tau_lang::build_ba_constant<node>(v, ba_tid);
}

template <NodeType node>
tref tree<node>::build_bf_ba_constant(node::bas_variant v, size_t ba_tid,
	tref r)
{
	return tau_lang::build_bf_ba_constant<node>(v, ba_tid, r);
}

template <NodeType node>
tref tree<node>::build_bf_uconst(const std::string& n1, const std::string& n2) {
	return tau_lang::build_bf_uconst<node>(n1, n2);
}

template <NodeType node>
tref tree<node>::build_var_name(size_t sid) {
	return tau_lang::build_var_name<node>(sid);
}

template <NodeType node>
tref tree<node>::build_var_name(const std::string& name) {
	return tau_lang::build_var_name<node>(name);
}

template <NodeType node>
tref tree<node>::build_var_name_indexed(size_t index) {
	return tau_lang::build_var_name_indexed<node>(index);
}

template <NodeType node>
tref tree<node>::build_variable(tref var_name_node) {
	return tau_lang::build_variable<node>(var_name_node);
}

template <NodeType node>
tref tree<node>::build_variable(const std::string& name) {
	return tau_lang::build_variable<node>(name);
}

template <NodeType node>
tref tree<node>::build_bf_variable(tref var_name_node) {
	return tau_lang::build_bf_variable<node>(var_name_node);
}

template <NodeType node>
tref tree<node>::build_bf_variable(const std::string& name) {
	return tau_lang::build_bf_variable<node>(name);
}

template <NodeType node>
tref tree<node>::build_in_var(tref var_name_node, tref offset_node) {
	return tau_lang::build_in_var<node>(var_name_node, offset_node);
}

template <NodeType node>
tref tree<node>::build_in_var_at_n(tref var_name_node, int_t pos) {
	return tau_lang::build_in_var_at_n<node>(var_name_node, pos);
}

template <NodeType node>
tref tree<node>::build_in_var_at_n(const std::string& name, int_t pos) {
	return tau_lang::build_in_var_at_n<node>(name, pos);
}

template <NodeType node>
tref tree<node>::build_in_var_at_n_indexed(size_t index, int_t pos) {
	return tau_lang::build_in_var_at_n_indexed<node>(index, pos);
}

template <NodeType node>
tref tree<node>::build_in_var_at_t(tref var_name_node, std::string t) {
	return tau_lang::build_in_var_at_t<node>(var_name_node, t);
}

template <NodeType node>
tref tree<node>::build_in_var_at_t_indexed(size_t index, std::string t) {
	return tau_lang::build_in_var_at_t_indexed<node>(index, t);
}

template <NodeType node>
tref tree<node>::build_in_var_at_t_minus(
	tref var_name_node, size_t shift, std::string t)
{
	return tau_lang::build_in_var_at_t_minus<node>(var_name_node, shift, t);
}

template <NodeType node>
tref tree<node>::build_in_var_at_t_minus(
	const std::string& name, size_t shift, std::string t)
{
	return tau_lang::build_in_var_at_t_minus<node>(name, shift, t);
}

template <NodeType node>
tref tree<node>::build_in_var_at_t_minus_indexed(
	size_t index, size_t shift, std::string t)
{
	return tau_lang::build_in_var_at_t_minus_indexed<node>(index, shift, t);
}

template <NodeType node>
tref tree<node>::build_out_var(tref var_name_node, tref offset_node) {
	return tau_lang::build_out_var<node>(var_name_node, offset_node);
}

template <NodeType node>
tref tree<node>::build_out_var_at_n(tref var_name_node, int_t pos) {
	return tau_lang::build_out_var_at_n<node>(var_name_node, pos);
}

template <NodeType node>
tref tree<node>::build_out_var_at_n(const std::string& name, int_t pos){
	return tau_lang::build_out_var_at_n<node>(name, pos);
}

template <NodeType node>
tref tree<node>::build_out_var_at_n_indexed(size_t index, int_t pos) {
	return tau_lang::build_out_var_at_n_indexed<node>(index, pos);
}

template <NodeType node>
tref tree<node>::build_out_var_at_t(tref var_name_node, std::string t) {
	return tau_lang::build_out_var_at_t<node>(var_name_node, t);
}

template <NodeType node>
tref tree<node>::build_out_var_at_t_indexed(size_t index, std::string t) {
	return tau_lang::build_out_var_at_t_indexed<node>(index, t);
}

template <NodeType node>
tref tree<node>::build_out_var_at_t_minus(
	tref var_name_node, size_t shift, std::string t)
{
	return tau_lang::build_out_var_at_t_minus<node>(var_name_node, shift, t);
}

template <NodeType node>
tref tree<node>::build_out_var_at_t_minus(
	const std::string& name, size_t shift, std::string t)
{
	return tau_lang::build_out_var_at_t_minus<node>(name, shift, t);
}

template <NodeType node>
tref tree<node>::build_out_var_at_t_minus_indexed(
	size_t index, size_t shift, std::string t)
{
	return tau_lang::build_out_var_at_t_minus_indexed<node>(index, shift, t);
}

} // idni::tau_lang namespace
