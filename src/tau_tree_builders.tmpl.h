// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_tree.h"

namespace idni::tau_lang {

template <typename N>
tref tree<N>::apply_builder(const rewriter::builder& b, trefs n) {
	typename tree<node>::subtree_map changes;
	trefs vars = (tt(b.first->get()) || capture).values();
	for (size_t i = 0; i < vars.size(); ++i) changes[vars[i]] = n[i];
	return rewriter::replace<node>(b.second->get(), changes);
}

//------------------------------------------------------------------------------
// builders

template <typename N>
const tree<N>& tree<N>::_0_trimmed() {
	static htree::sp cached = geth(get(node(bf_f)));
	return get(cached);
}

template <typename N>
const tree<N>& tree<N>::_1_trimmed() {
	static htree::sp cached = geth(get(node(bf_t)));
	return get(cached);
}

template <typename N>
const tree<N>& tree<N>::_F_trimmed() {
	static htree::sp cached = geth(get(node(wff_f)));
	return get(cached);
}

template <typename N>
const tree<N>& tree<N>::_T_trimmed() {
	static htree::sp cached = geth(get(node(wff_t)));
	return get(cached);
}

template <typename N>
const tree<N>& tree<N>::_0() {
	static htree::sp cached = geth(get(node(bf), _0_trimmed().get()));
	return get(cached);
}

template <typename N>
const tree<N>& tree<N>::_1() {
	static htree::sp cached = geth(get(node(bf), _1_trimmed().get()));
	return get(cached);
}

template <typename N>
const tree<N>& tree<N>::_F() {
	static htree::sp cached = geth(get(node(wff), _F_trimmed().get()));
	return get(cached);
}

template <typename N>
const tree<N>& tree<N>::_T() {
	static htree::sp cached = geth(get(node(wff), _T_trimmed().get()));
	return get(cached);
}

template <typename N>
tref tree<N>::build_variable(const std::string& name) {
	return get(variable, name);
}

template <typename N>
tref tree<N>::build_in_var_name(size_t index) {
	std::stringstream var_name;
	var_name << "i" << index;
	return get(in_var_name, var_name.str());
}

template <typename N>
tref tree<N>::build_type(const std::string& name) {
	return get(type, name);
}

template <typename N>
tref tree<N>::build_bf_t_type(const std::string& type) {
	return get(bf, get(bf_t, build_type(type)));
}

template <typename N>
tref tree<N>::build_bf_t_type(tref type) {
	return get(bf, get(bf_t, type));
}

template <typename N>
tref tree<N>::build_bf_f_type(const std::string& type) {
	return get(bf, get(bf_f, build_type(type)));
}

template <typename N>
tref tree<N>::build_bf_f_type(tref type) {
	return get(bf, get(bf_f, type));
}

template <typename N>
tref tree<N>::build_in_variable_at_n(tref var_name, size_t num) {
	DBG(assert(get(var_name).is(var_name));)
	return get(bf, get(variable, get(io_var, get(in, {
				var_name, get(offset, get_integer(num)) }))));
}

template <typename N>
tref tree<N>::build_in_variable_at_n(size_t index, size_t num) {
	return build_in_variable_at_n(build_in_var_name(index), num);
}

template <typename N>
tref tree<N>::build_in_variable_at_n(const std::string& name, int_t pos) {
	return get(bf, get(variable, get(io_var, get(in, {
				get(in_var_name, name),
				get(offset, get_integer(pos)) }))));
}

template <typename N>
tref tree<N>::build_in_variable_at_t(tref var_name) {
	DBG(assert(get(var_name).is(in_var_name));)
	return get(bf, get(variable, get(io_var, get(in,
			var_name, get(offset, build_variable("t"))))));
}

template <typename N>
tref tree<N>::build_in_variable_at_t(size_t index) {
	return build_in_variable_at_t(build_in_var_name(index));
}

template <typename N>
tref tree<N>::build_in_variable_at_t_minus (const std::string& name,
	int_t shift)
{
	return get(bf, get(variable, get(io_var, get(in, {
		get(in_var_name, name),
		get(offset, get(shift, {get(variable, "t"),
			get_num(shift)})) }))));
}

template <typename N>
tref tree<N>::build_in_variable_at_t_minus(tref var_name, size_t num) {
	DBG(assert(get(var_name).is(in_var_name));)
	DBG(assert(num > 0);)
	return get(bf, get(variable, get(io_var, get( in, {
						var_name,
						get(offset, get(shift, {
							build_variable("t"),
							get_num(num) })) }))));
}

template <typename N>
tref tree<N>::build_in_variable_at_t_minus(size_t index, size_t num) {
	return build_in_variable_at_t_minus(build_in_var_name(index), num);
}

template <typename N>
tref tree<N>::build_out_var_name(size_t index) {
	std::stringstream var_name;
	var_name << "o" << index;
	return get(out_var_name, var_name.str());
}

template <typename N>
tref tree<N>::build_out_var_name(const std::string& name) {
	return get(out_var_name, name);
}

template <typename N>
tref tree<N>::build_out_variable_at_t(tref var_name) {
	DBG(assert(get(var_name).is(out_var_name));)
	return get(bf, get(variable, get(io_var, get(out, {
					var_name,
					get(offset, build_variable("t")) }))));
}

template <typename N>
tref tree<N>::build_out_variable_at_t(size_t index) {
	return build_out_variable_at_t(build_out_var_name(index));
}

template <typename N>
tref tree<N>::build_out_variable_at_n(tref var_name, size_t num) {
	DBG(assert(get(var_name).is(out_var_name));)
	return get(bf, get(variable, get(io_var, get(out, {
					var_name,
					get(offset, get_integer(num)) }))));
}

template <typename N>
tref tree<N>::build_out_variable_at_n(size_t index, size_t num) {
	return build_out_variable_at_n(build_out_var_name(index), num);
}

template <typename N>
tref tree<N>::build_out_variable_at_n(const std::string& name, const int_t pos){
	return get(bf, get(variable, get(io_var, get(out, {
					get(out_var_name, name),
					get(offset, get_integer(pos)) }))));
}

template <typename N>
tref tree<N>::build_out_variable_at_t_minus (const std::string& name,
	const int_t shift)
{
	return get(bf, get(variable, get(io_var, get(out, {
						get(out_var_name, name),
						get(offset, get(shift, {
							get(variable, "t"),
							get_num(shift)})) }))));
}

template <typename N>
tref tree<N>::build_out_variable_at_t_minus(tref var_name, size_t num) {
	DBG(assert(get(var_name).is(out_var_name));)
	DBG(assert(num > 0);)

	return get(bf, get(variable, get(io_var, get(out, {
						var_name,
						get(offset, get(shift, {
							build_variable("t"),
							get_num(num)})) }))));
}

template <typename N>
tref tree<N>::build_out_variable_at_t_minus(size_t index, size_t num) {
	return build_out_variable_at_t_minus(build_out_var_name(index), num);
}

// // ------ Helpers for variables having io_var as child ---------------
// template <typename N>
// auto is_io_initial (tref io_var) {
// 	return (trim2(io_var)->child[1] | integer).has_value();
// }

// template <typename N>
// auto is_io_shift (tref io_var) {
// 	return (trim2(io_var)->child[1] | shift).has_value();
// }

// template <typename N>
// auto get_io_time_point (tref io_var) {
// 	return int_extractor(trim(trim2(io_var)->child[1]));
// }

// template <typename N>
// auto get_io_shift (tref io_var) {
// 	return size_t_extractor(trim2(io_var)->child[1]->child[0]->child[1]->child[0]).value();
// }

// template <typename N>
// std::string get_io_name (tref io_var) {
// 	std::stringstream ss; ss << trim(trim2(io_var));
// 	return ss.str();
// }

// template <typename N>
// tref get_tau_io_name(tref io_var) {
// 	return trim(trim2(io_var));
// }

// template <typename N>
// int_t get_io_var_shift(tref io_var) {
// 	// If there is a shift
// 	if (is_io_shift(io_var))
// 		return get_io_shift(io_var);
// 	return 0;
// }

// int_t get_max_shift(const auto& io_vars, bool ignore_temps = false) {
// 	int_t max_shift = 0;
// 	for (const auto& v : io_vars) {
// 		if (ignore_temps && get_io_name(v)[0] == '_')
// 			continue;
// 		max_shift = std::max(max_shift, get_io_var_shift(v));
// 	}
// 	return max_shift;
// }

// template <typename N>
// int_t get_max_initial(const std::vector<tref>& io_vars) {
// 	int_t max_init = -1;
// 	for (tref v : io_vars) {
// 		if (is_io_initial(v)) {
// 			int_t init = get_io_time_point(v);
// 			max_init = std::max(max_init, init);
// 		}
// 	}
// 	return max_init;
// }

// -------------------------------------------------------------------

// template <typename N>
// tref build_bf_var(const std::string& name) {
// 	auto var = make_builder("( $X ) =: " + name + ".").second;
// 	return trim(var);
// }

template <typename N>
tref tree<N>::build_bf_constant(tref cte) {
	return get(bf, get(bf_constant, get(constant, cte)));
}

template <typename N>
tref tree<N>::build_bf_constant(tref cte, tref type) {
	return get(bf, get(bf_constant, get(constant, cte), type));
}

template <typename N>
tref tree<N>::build_bf_constant(tref cte, const std::string& type) {
	return get(bf, get(bf_constant, get(constant, cte), build_type(type)));
}

// template <typename N>
// tref tree<N>::build_bf_constant(const std::variant& v) {
// 	auto cte = rewriter::depreciating::make_node<tau_sym>(tau_sym(v), {});
// 	return build_bf_constant(cte);
// }

// template <typename N>
// tref tree<N>::build_bf_constant(const std::variant& v, tref type) {
// 	auto cte = rewriter::depreciating::make_node<tau_sym>(tau_sym(v), {});
// 	return build_bf_constant(cte, type);
// }

// template <typename N>
// tref tree<N>::build_bf_constant(const std::variant& v, const std::optional<tref>& type) {
// 	auto cte = rewriter::depreciating::make_node<tau_sym>(tau_sym(v), {});
// 	return type.has_value() ? build_bf_constant(cte, type.value()) : build_bf_constant(cte);
// }

// template <typename N>
// tref tree<N>::build_bf_and_constant( const auto& ctes, tref type)
// {
// 	if (ctes.empty()) return _1;

// 	auto cte = std::accumulate(++ctes.begin(), ctes.end(), *ctes.begin(),
// 		[&](tref l, tref r) { return l & r; });

// 	return build_bf_constant(cte, type);
// }

// template <typename N>
// tref tree<N>::build_bf_or_constant( const auto& ctes, tref type)
// {
// 	if (ctes.empty()) return _0;

// 	auto cte = std::accumulate(++ctes.begin(), ctes.end(), *ctes.begin(),
// 		[&](tref l, tref r) { return l | r; });

// 	return build_bf_constant(cte, type);
// }

// template <typename N>
// tref tree<N>::build_bf_constant(
// 	const std::optional<std::variant>& o)
// {
// 	return o.has_value() ? build_bf_constant(o.value())
// 				: std::optional<tref>();
// }

template <typename N>
tref tree<N>::build_bf_uniter_const(const std::string& n1,
	const std::string& n2)
{
	return get(bf, get(variable, get(uninterpreted_constant,
			get(uninter_const_name, n1 + ":" + n2))));
}

// wff factory method for building wff formulas
template <typename N>
tref tree<N>::build_wff_eq(tref l) {
	return get(wff, get(bf_eq, l, _0));
}

template <typename N>
tref tree<N>::build_wff_neq(tref l) {
	return get(wff, get(bf_neq, l, _0().get()));
}

template <typename N>
tref tree<N>::build_wff_and(tref l, tref r) {
	return get(wff, get(node::type::wff_and, l, r));
}

template <typename N>
tref tree<N>::build_wff_and(const auto& wffs) {
	return std::accumulate(wffs.begin(), wffs.end(), _T,
		[](tref l, tref r) {return build_wff_and(l, r);});
}

template <typename N>
tref tree<N>::build_wff_or(tref l, tref r) {
	return get(wff, get(wff_or, l, r));
}

template <typename N>
tref tree<N>::build_wff_or(const auto& wffs) {
	return std::accumulate(wffs.begin(), wffs.end(), _F().get(),
		[](tref l, tref r) { return build_wff_or(l, r);});
}

template <typename N>
tref tree<N>::build_wff_neg(tref l) {
	return get(wff, get(wff_neg, l));
}

template <typename N>
tref tree<N>::build_wff_xor_from_def(tref l, tref r) {
	return build_wff_or(build_wff_and(build_wff_neg(l), r),
		build_wff_and(build_wff_neg(r), l));
}

template <typename N>
tref tree<N>::build_wff_xor(tref l, tref r) {
	return build_wff_or(
		build_wff_and(build_wff_neg(l), r),
		build_wff_and(build_wff_neg(r), l));
}

template <typename N>
tref tree<N>::build_wff_imply(tref l, tref r) {
	return build_wff_or(build_wff_neg(l), r);
}

template <typename N>
tref tree<N>::build_wff_conditional(tref x, tref y, tref z) {
	return build_wff_and(build_wff_imply(x, y),
		build_wff_imply(build_wff_neg(x), z));
}

template <typename N>
tref tree<N>::build_wff_equiv(tref l, tref r) {
	return build_wff_and(build_wff_imply(l, r),
		build_wff_imply(r, l));
}

template <typename N>
tref tree<N>::build_wff_all(tref l, tref r) {
	return get(wff, get(wff_all, l, r));
}

template <typename N>
tref tree<N>::build_wff_ex(tref l, tref r) {
	return get(wff, get(wff_ex, l, r));
}

template <typename N>
tref tree<N>::build_wff_sometimes(tref l) {
	return get(wff, get(wff_sometimes, l));
}

template <typename N>
tref tree<N>::build_wff_always(tref l) {
	return get(wff, get(wff_always, l));
}

// bf factory method for building bf formulas
template <typename N>
tref tree<N>::build_bf_and(tref l, tref r) {
	return get(bf, get(bf_and, l, r));
}

template <typename N>
tref tree<N>::build_bf_and(const auto& bfs) {
	return std::accumulate(bfs.begin(), bfs.end(), _1,
		[](tref l, tref r) { return build_bf_and(l, r);});
}

template <typename N>
tref tree<N>::build_bf_or(tref l, tref r) {
	return get(bf, get(bf_or, l, r));
}

template <typename N>
tref tree<N>::build_bf_or(const auto& bfs) {
	return std::accumulate(bfs.begin(), bfs.end(), _0,
		[](tref l, tref r) { return build_bf_or(l, r); });
}

template <typename N>
tref tree<N>::build_bf_neg(tref l) {
	return get(bf, get(bf_neg, l));
}

template <typename N>
tref tree<N>::build_bf_xor_from_def(tref l, tref r) {
	return build_bf_or(
		build_bf_and(build_bf_neg(l), r),
		build_bf_and(build_bf_neg(r), l));
}

template <typename N>
tref tree<N>::build_bf_xor(tref l, tref r) {
	return build_bf_or(
		build_bf_and(build_bf_neg(l), r),
		build_bf_and(l, build_bf_neg(r)));
}

template <typename N>
tref tree<N>::build_wff_eq(tref l, tref r) {
// #ifdef DEBUG
// // 	assert(is_non_terminal(bf, l) && is_non_terminal(bf, r));
// #endif // DEBUG
	auto left_side = build_bf_xor(l,r);
	return build_wff_eq(left_side);
}

template <typename N>
tref tree<N>::build_bf_less(tref l, tref r) {
	return build_wff_and(
		build_wff_eq(
			build_bf_and(l, build_bf_neg(r))),
		build_wff_neq(build_bf_xor(l, r)));
}

template <typename N>
tref tree<N>::build_bf_nless(tref l, tref r) {
	return build_wff_neg(build_bf_less(l, r));
}

template <typename N>
tref tree<N>::build_bf_less_equal(tref l, tref r) {
	return build_wff_eq(build_bf_and(l, build_bf_neg(r)));
}

template <typename N>
tref tree<N>::build_bf_nleq(tref l, tref r) {
	return build_wff_neq(build_bf_and(l, build_bf_neg(r)));
}

template <typename N>
tref tree<N>::build_bf_interval(tref x, tref y, tref z) {
	return build_wff_and(build_bf_less_equal(x, y),
		build_bf_less_equal(y, z));
}

template <typename N>
tref tree<N>::build_bf_nleq_lower(tref l, tref r) {
	trefs args { trim(l), trim(r) };
	return apply_builder(bldr_bf_nleq_lower, args);
}

template <typename N>
tref tree<N>::build_bf_nleq_upper(tref l, tref r) {
	trefs args { trim(l), trim(r) };
	return apply_builder(bldr_bf_nleq_upper, args);
}

template <typename N>
tref tree<N>::build_bf_greater(tref l, tref r) {
	return build_bf_less(r, l);
}

template <typename N>
tref tree<N>::build_bf_ngreater(tref l, tref r) {
	return build_wff_neg(build_bf_greater(l, r));
}

template <typename N>
tref tree<N>::build_bf_greater_equal(tref l, tref r) {
	return build_bf_less_equal(r, l);
}

template <typename N>
tref tree<N>::build_bf_ngeq(tref l, tref r) {
	return build_wff_neg(build_bf_greater_equal(l, r));
}

template <typename N>
tref tree<N>::build_wff_ctn_greater_equal(tref ctnvar, tref num) {
	return get(wff, get(constraint, get(ctn_greater_equal, {ctnvar, num})));
}

template <typename N>
tref tree<N>::build_wff_ctn_greater(tref ctnvar, tref num) {
	return get(wff, get(constraint, get(ctn_greater, {ctnvar, num})));
}

template <typename N>
tref tree<N>::build_wff_ctn_less_equal(tref ctnvar, tref num) {
	return get(wff, get(constraint, get(ctn_less_equal, {ctnvar, num})));
}

template <typename N>
tref tree<N>::build_wff_ctn_less(tref ctnvar, tref num) {
	return get(wff, get(constraint, get(ctn_less, {ctnvar, num})));
}

template <typename N>
tref tree<N>::build_wff_ctn_eq(tref ctnvar, tref num) {
	return get(wff, get(constraint, get(ctn_eq, {ctnvar, num})));
}

template <typename N>
tref tree<N>::build_wff_ctn_neq(tref ctnvar, tref num) {
	return get(wff, get(constraint, get(ctn_neq, {ctnvar, num})));
}

//------------------------------------------------------------------------------

} // idni::tau_lang namespace
