// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_tree.h"

namespace idni::tau_lang {

template <NodeType node>
tref tree<node>::apply_builder(const rewriter::builder& b, trefs n) {
	typename tree<node>::subtree_map changes;
	trefs vars = (tt(b.first->get()) || capture).values();
	for (size_t i = 0; i < vars.size(); ++i) changes[vars[i]] = n[i];
	return rewriter::replace<node>(b.second->get(), changes);
}

//------------------------------------------------------------------------------
// builders

template <NodeType node>
tref tree<node>::_0() { return get_0().get(); }
template <NodeType node>
tref tree<node>::_1() { return get_1().get(); }
template <NodeType node>
tref tree<node>::_F() { return get_F().get(); }
template <NodeType node>
tref tree<node>::_T() { return get_T().get(); }

template <NodeType node>
tref tree<node>::_0_trimmed() { return get_0_trimmed().get(); }
template <NodeType node>
tref tree<node>::_1_trimmed() { return get_1_trimmed().get(); }
template <NodeType node>
tref tree<node>::_F_trimmed() { return get_F_trimmed().get(); }
template <NodeType node>
tref tree<node>::_T_trimmed() { return get_T_trimmed().get(); }

template <NodeType node>
const tree<node>& tree<node>::get_0() {
	static htree::sp cached = geth(get(node(bf), _0_trimmed()));
	return get(cached);
}

template <NodeType node>
const tree<node>& tree<node>::get_1() {
	static htree::sp cached = geth(get(node(bf), _1_trimmed()));
	return get(cached);
}

template <NodeType node>
const tree<node>& tree<node>::get_F() {
	static htree::sp cached = geth(get(node(wff), _F_trimmed()));
	return get(cached);
}

template <NodeType node>
const tree<node>& tree<node>::get_T() {
	static htree::sp cached = geth(get(node(wff), _T_trimmed()));
	return get(cached);
}

template <NodeType node>
const tree<node>& tree<node>::get_0_trimmed() {
	static htree::sp cached = geth(get(node(bf_f)));
	return get(cached);
}

template <NodeType node>
const tree<node>& tree<node>::get_1_trimmed() {
	static htree::sp cached = geth(get(node(bf_t)));
	return get(cached);
}

template <NodeType node>
const tree<node>& tree<node>::get_F_trimmed() {
	static htree::sp cached = geth(get(node(wff_f)));
	return get(cached);
}

template <NodeType node>
const tree<node>& tree<node>::get_T_trimmed() {
	static htree::sp cached = geth(get(node(wff_t)));
	return get(cached);
}

template <NodeType node>
tref tree<node>::build_variable(const std::string& name) {
	return get(variable, name);
}

template <NodeType node>
tref tree<node>::build_in_var_name(size_t index) {
	std::stringstream var_name;
	var_name << "i" << index;
	return get(in_var_name, var_name.str());
}

template <NodeType node>
tref tree<node>::build_bf_t_type(const std::string& type) {
	return get(bf,
		get(node::ba_typed(bf_t, node::ba_constants_t::type_id(type))));
}

template <NodeType node>
tref tree<node>::build_bf_t_type(tref type) {
	DBG(assert(type != nullptr);)
	return get(bf, get(bf_t, type));
}

template <NodeType node>
tref tree<node>::build_bf_f_type(const std::string& type) {
	return get(bf,
		get(node::ba_typed(bf_f, node::ba_constants_t::type_id(type))));
}

template <NodeType node>
tref tree<node>::build_bf_f_type(tref type) {
	DBG(assert(type != nullptr);)
	return get(bf, get(bf_f, type));
}

template <NodeType node>
tref tree<node>::build_in_variable_at_n(tref var_name, size_t num) {
	DBG(assert(var_name != nullptr);)
	DBG(assert(get(var_name).is(var_name));)
	return get(bf, get(variable, get(io_var, get(in, {
				var_name, get(offset, get_integer(num)) }))));
}

template <NodeType node>
tref tree<node>::build_in_variable_at_n(size_t index, size_t num) {
	return build_in_variable_at_n(build_in_var_name(index), num);
}

template <NodeType node>
tref tree<node>::build_in_variable_at_n(const std::string& name, int_t pos) {
	return get(bf, get(variable, get(io_var, get(in, {
				get(in_var_name, name),
				get(offset, get_integer(pos)) }))));
}

template <NodeType node>
tref tree<node>::build_in_variable_at_t(tref var_name) {
	DBG(assert(var_name != nullptr);)
	DBG(assert(get(var_name).is(in_var_name));)
	return get(bf, get(variable, get(io_var, get(in,
			var_name, get(offset, build_variable("t"))))));
}

template <NodeType node>
tref tree<node>::build_in_variable_at_t(size_t index) {
	return build_in_variable_at_t(build_in_var_name(index));
}

template <NodeType node>
tref tree<node>::build_in_variable_at_t_minus (const std::string& name,
	int_t shift)
{
	return get(bf, get(variable, get(io_var, get(in,
		get(in_var_name, name),
		get(offset, get(shift, get(variable, "t"), get_num(shift)))))));
}

template <NodeType node>
tref tree<node>::build_in_variable_at_t_minus(tref var_name, size_t num) {
	DBG(assert(var_name != nullptr);)
	DBG(assert(get(var_name).is(in_var_name));)
	DBG(assert(num > 0);)
	return get(bf, get(variable, get(io_var, get(in, var_name,
			get(offset, get(shift, build_variable("t"),
						get_num(num)))))));
}

template <NodeType node>
tref tree<node>::build_in_variable_at_t_minus(size_t index, size_t num) {
	return build_in_variable_at_t_minus(build_in_var_name(index), num);
}

template <NodeType node>
tref tree<node>::build_out_var_name(size_t index) {
	std::stringstream var_name;
	var_name << "o" << index;
	return get(out_var_name, var_name.str());
}

template <NodeType node>
tref tree<node>::build_out_var_name(const std::string& name) {
	return get(out_var_name, name);
}

template <NodeType node>
tref tree<node>::build_out_variable_at_t(tref var_name) {
	DBG(assert(var_name != nullptr);)
	DBG(assert(get(var_name).is(out_var_name));)
	return get(bf, get(variable, get(io_var, get(out, var_name,
				get(offset, build_variable("t"))))));
}

template <NodeType node>
tref tree<node>::build_out_variable_at_t(size_t index) {
	return build_out_variable_at_t(build_out_var_name(index));
}

template <NodeType node>
tref tree<node>::build_out_variable_at_n(tref var_name, size_t num) {
	DBG(assert(var_name != nullptr);)
	DBG(assert(get(var_name).is(out_var_name));)
	return get(bf, get(variable, get(io_var, get(out, var_name,
					get(offset, get_integer(num))))));
}

template <NodeType node>
tref tree<node>::build_out_variable_at_n(size_t index, size_t num) {
	return build_out_variable_at_n(build_out_var_name(index), num);
}

template <NodeType node>
tref tree<node>::build_out_variable_at_n(const std::string& name, const int_t pos){
	return get(bf, get(variable, get(io_var, get(out, {
					get(out_var_name, name),
					get(offset, get_integer(pos)) }))));
}

template <NodeType node>
tref tree<node>::build_out_variable_at_t_minus (const std::string& name,
	const int_t shift)
{
	return get(bf, get(variable, get(io_var, get(out,
		get(out_var_name, name),
		get(offset, get(shift,
			get(variable, "t"),
			get_num(shift)))))));
}

template <NodeType node>
tref tree<node>::build_out_variable_at_t_minus(tref var_name, size_t num) {
	DBG(assert(get(var_name).is(out_var_name));)
	DBG(assert(num > 0);)
	return get(bf, get(variable, get(io_var, get(out, var_name,
		get(offset, get(shift,
					build_variable("t"),
					get_num(num)))))));
}

template <NodeType node>
tref tree<node>::build_out_variable_at_t_minus(size_t index, size_t num) {
	return build_out_variable_at_t_minus(build_out_var_name(index), num);
}

// template <NodeType node>
// tref tree<node>::build_bf_constant(tref cte) {
// 	DBG(assert(cte != nullptr);)
// 	return get(bf, get(bf_constant, get(constant, cte)));
// }

// template <NodeType node>
// tref tree<node>::build_bf_constant(tref cte, tref type) {
// 	DBG(assert(cte != nullptr);)
// 	return get(bf, get(bf_constant, get(constant, cte), type));
// }

// template <NodeType node>
// tref tree<node>::build_bf_constant(tref cte, const std::string& type) {
// 	DBG(assert(cte != nullptr);)
// 	return get(bf, get(bf_constant, get(constant, cte), build_type(type)));
// }

// template <NodeType node>
// tref tree<node>::build_bf_constant(const std::variant& v) {
// 	auto cte = rewriter::depreciating::make_node<tau_sym>(tau_sym(v), {});
// 	return build_bf_constant(cte);
// }

// template <NodeType node>
// tref tree<node>::build_bf_constant(const std::variant& v, tref type) {
// 	auto cte = rewriter::depreciating::make_node<tau_sym>(tau_sym(v), {});
// 	return build_bf_constant(cte, type);
// }

// template <NodeType node>
// tref tree<node>::build_bf_constant(const std::variant& v, const std::optional<tref>& type) {
// 	auto cte = rewriter::depreciating::make_node<tau_sym>(tau_sym(v), {});
// 	return type.has_value() ? build_bf_constant(cte, type.value()) : build_bf_constant(cte);
// }

// template <NodeType node>
// tref tree<node>::build_bf_and_constant( const auto& ctes, tref type)
// {
// 	if (ctes.empty()) return _1;

// 	auto cte = std::accumulate(++ctes.begin(), ctes.end(), *ctes.begin(),
// 		[&](tref l, tref r) { return l & r; });

// 	return build_bf_constant(cte, type);
// }

// template <NodeType node>
// tref tree<node>::build_bf_or_constant( const auto& ctes, tref type)
// {
// 	if (ctes.empty()) return _0;

// 	auto cte = std::accumulate(++ctes.begin(), ctes.end(), *ctes.begin(),
// 		[&](tref l, tref r) { return l | r; });

// 	return build_bf_constant(cte, type);
// }

// template <NodeType node>
// tref tree<node>::build_bf_constant(
// 	const std::optional<std::variant>& o)
// {
// 	return o.has_value() ? build_bf_constant(o.value())
// 				: std::optional<tref>();
// }

template <NodeType node>
tref tree<node>::build_bf_uninterpreted_constant(const std::string& n1,
	const std::string& n2)
{
	return get(bf, get(variable, get(uninterpreted_constant,
			get(uninter_const_name, n1 + ":" + n2))));
}

// wff factory method for building wff formulas
template <NodeType node>
tref tree<node>::build_wff_eq(tref l) {
	DBG(assert(l != nullptr);)
	return get(wff, get(bf_eq, l, _0()));
}

template <NodeType node>
tref tree<node>::build_wff_neq(tref l) {
	DBG(assert(l != nullptr);)
	return get(wff, get(bf_neq, l, _0()));
}

template <NodeType node>
tref tree<node>::build_wff_and(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return get(wff, get(node::type::wff_and, l, r));
}

template <NodeType node>
tref tree<node>::build_wff_and(const auto& wffs) {
	return std::accumulate(wffs.begin(), wffs.end(), _T(),
		[](tref l, tref r) { return build_wff_and(l, r); });
}

template <NodeType node>
tref tree<node>::build_wff_or(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return get(wff, get(wff_or, l, r));
}

template <NodeType node>
tref tree<node>::build_wff_or(const auto& wffs) {
	return std::accumulate(wffs.begin(), wffs.end(), _F(),
		[](tref l, tref r) { return build_wff_or(l, r); });
}

template <NodeType node>
tref tree<node>::build_wff_neg(tref l) {
	DBG(assert(l != nullptr);)
	return get(wff, get(wff_neg, l));
}

template <NodeType node>
tref tree<node>::build_wff_xor_from_def(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return build_wff_or(build_wff_and(build_wff_neg(l), r),
		build_wff_and(build_wff_neg(r), l));
}

template <NodeType node>
tref tree<node>::build_wff_xor(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return build_wff_or(
		build_wff_and(build_wff_neg(l), r),
		build_wff_and(build_wff_neg(r), l));
}

template <NodeType node>
tref tree<node>::build_wff_imply(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return build_wff_or(build_wff_neg(l), r);
}

template <NodeType node>
tref tree<node>::build_wff_conditional(tref x, tref y, tref z) {
	DBG(assert(x != nullptr && y != nullptr && z != nullptr);)
	return build_wff_and(build_wff_imply(x, y),
		build_wff_imply(build_wff_neg(x), z));
}

template <NodeType node>
tref tree<node>::build_wff_equiv(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return build_wff_and(build_wff_imply(l, r),
		build_wff_imply(r, l));
}

template <NodeType node>
tref tree<node>::build_wff_all(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return get(wff, get(wff_all, l, r));
}

template <NodeType node>
tref tree<node>::build_wff_ex(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return get(wff, get(wff_ex, l, r));
}

template <NodeType node>
tref tree<node>::build_wff_sometimes(tref l) {
	DBG(assert(l != nullptr);)
	return get(wff, get(wff_sometimes, l));
}

template <NodeType node>
tref tree<node>::build_wff_always(tref l) {
	DBG(assert(l != nullptr);)
	return get(wff, get(wff_always, l));
}

// bf factory method for building bf formulas
template <NodeType node>
tref tree<node>::build_bf_and(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return get(bf, get(bf_and, l, r));
}

template <NodeType node>
tref tree<node>::build_bf_and(const auto& bfs) {
	return std::accumulate(bfs.begin(), bfs.end(), _1(),
		[](tref l, tref r) { return build_bf_and(l, r);});
}

template <NodeType node>
tref tree<node>::build_bf_or(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return get(bf, get(bf_or, l, r));
}

template <NodeType node>
tref tree<node>::build_bf_or(const auto& bfs) {
	return std::accumulate(bfs.begin(), bfs.end(), _0(),
		[](tref l, tref r) { return build_bf_or(l, r); });
}

template <NodeType node>
tref tree<node>::build_bf_neg(tref l) {
	DBG(assert(l != nullptr);)
	return get(bf, get(bf_neg, l));
}

template <NodeType node>
tref tree<node>::build_bf_xor_from_def(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return build_bf_or(
		build_bf_and(build_bf_neg(l), r),
		build_bf_and(build_bf_neg(r), l));
}

template <NodeType node>
tref tree<node>::build_bf_xor(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return build_bf_or(
		build_bf_and(build_bf_neg(l), r),
		build_bf_and(l, build_bf_neg(r)));
}

template <NodeType node>
tref tree<node>::build_wff_eq(tref l, tref r) {
	// DBG(assert(is<node>(l, bf) && is<node>(r, bf));)
	DBG(assert(l != nullptr && r != nullptr);)
	auto left_side = build_bf_xor(l,r);
	return build_wff_eq(left_side);
}

template <NodeType node>
tref tree<node>::build_bf_less(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return build_wff_and(
		build_wff_eq(
			build_bf_and(l, build_bf_neg(r))),
		build_wff_neq(build_bf_xor(l, r)));
}

template <NodeType node>
tref tree<node>::build_bf_nless(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return build_wff_neg(build_bf_less(l, r));
}

template <NodeType node>
tref tree<node>::build_bf_less_equal(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return build_wff_eq(build_bf_and(l, build_bf_neg(r)));
}

template <NodeType node>
tref tree<node>::build_bf_nleq(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return build_wff_neq(build_bf_and(l, build_bf_neg(r)));
}

template <NodeType node>
tref tree<node>::build_bf_interval(tref x, tref y, tref z) {
	DBG(assert(x != nullptr && y != nullptr && z != nullptr);)
	return build_wff_and(build_bf_less_equal(x, y),
		build_bf_less_equal(y, z));
}

template <NodeType node>
tref tree<node>::build_bf_nleq_lower(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	trefs args { trim(l), trim(r) };
	return apply_builder(bldr_bf_nleq_lower, args);
}

template <NodeType node>
tref tree<node>::build_bf_nleq_upper(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	trefs args { trim(l), trim(r) };
	return apply_builder(bldr_bf_nleq_upper, args);
}

template <NodeType node>
tref tree<node>::build_bf_greater(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return build_bf_less(r, l);
}

template <NodeType node>
tref tree<node>::build_bf_ngreater(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return build_wff_neg(build_bf_greater(l, r));
}

template <NodeType node>
tref tree<node>::build_bf_greater_equal(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return build_bf_less_equal(r, l);
}

template <NodeType node>
tref tree<node>::build_bf_ngeq(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	return build_wff_neg(build_bf_greater_equal(l, r));
}

template <NodeType node>
tref tree<node>::build_wff_ctn_greater_equal(tref ctnvar, tref num) {
	DBG(assert(ctnvar != nullptr && num != nullptr);)
	return get(wff, get(constraint, get(ctn_greater_equal, {ctnvar, num})));
}

template <NodeType node>
tref tree<node>::build_wff_ctn_greater(tref ctnvar, tref num) {
	DBG(assert(ctnvar != nullptr && num != nullptr);)
	return get(wff, get(constraint, get(ctn_greater, {ctnvar, num})));
}

template <NodeType node>
tref tree<node>::build_wff_ctn_less_equal(tref ctnvar, tref num) {
	DBG(assert(ctnvar != nullptr && num != nullptr);)
	return get(wff, get(constraint, get(ctn_less_equal, {ctnvar, num})));
}

template <NodeType node>
tref tree<node>::build_wff_ctn_less(tref ctnvar, tref num) {
	DBG(assert(ctnvar != nullptr && num != nullptr);)
	return get(wff, get(constraint, get(ctn_less, {ctnvar, num})));
}

template <NodeType node>
tref tree<node>::build_wff_ctn_eq(tref ctnvar, tref num) {
	DBG(assert(ctnvar != nullptr && num != nullptr);)
	return get(wff, get(constraint, get(ctn_eq, {ctnvar, num})));
}

template <NodeType node>
tref tree<node>::build_wff_ctn_neq(tref ctnvar, tref num) {
	DBG(assert(ctnvar != nullptr && num != nullptr);)
	return get(wff, get(constraint, get(ctn_neq, {ctnvar, num})));
}

//------------------------------------------------------------------------------

} // idni::tau_lang namespace
