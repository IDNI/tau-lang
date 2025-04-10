#include "tau.h"

namespace idni::tau_lang {

template <typename N>
tref tree<N>::apply_builder(const rewriter::builder& b, trefs n) {
	typename tree<node>::subtree_map changes;
	trefs vars = (tt(b.first->get()) || node::type::capture).values();
	for (size_t i = 0; i < vars.size(); ++i) changes[vars[i]] = n[i];
	return rewriter::replace<node>(b.second->get(), changes);
}

//------------------------------------------------------------------------------
// builders

template <typename N>
tref tree<N>::build::variable(const std::string& name) {
	return get(variable, name);
}

template <typename N>
tref tree<N>::build::in_var_name(size_t index) {
	std::stringstream var_name;
	var_name << "i" << index;
	return get(node::type::in_var_name, var_name.str());
}

template <typename N>
tref tree<N>::build::type(const std::string& name) {
	return get(type, name);
}

template <typename N>
tref tree<N>::build::bf_t_type(const std::string& type) {
	return get(bf, get(bf_t, build::type(type)));
}

template <typename N>
tref tree<N>::build::bf_t_type(tref type) {
	return get(bf, get(bf_t, type));
}

template <typename N>
tref tree<N>::build::bf_f_type(const std::string& type) {
	return get(bf, get(bf_f, build::type(type)));
}

template <typename N>
tref tree<N>::build::bf_f_type(tref type) {
	return get(bf, get(bf_f, type));
}

template <typename N>
tref tree<N>::build::in_variable_at_n(tref var_name, size_t num) {
	DBG(assert(tree::get(var_name).is(node::type::var_name));)
	return get(bf, get(variable, get(io_var, get(in,
			get_string(in_var_name), get(offset, get_integer(num))))));
}

template <typename N>
tref tree<N>::build::in_variable_at_n(size_t index, size_t num) {
	return build::in_variable_at_n(build::in_var_name(index), num);
}

template <typename N>
tref tree<N>::build::in_variable_at_n(const std::string& name, const int_t pos) {
	auto var_name = get(in_var_name, name);
	auto offset = get(offset, get_integer(pos));
	return get(bf, get(variable,
		get(io_var, get(in, { var_name, offset }))));
}

template <typename N>
tref tree<N>::build::in_variable_at_t(tref var_name) {
	DBG(assert(tree::get(var_name).is(node::type::in_var_name));)
	return get(tau_parser::bf, get(
			tau_parser::variable, get(
				tau_parser::io_var, get(
					tau_parser::in,
						var_name, get(
						tau_parser::offset,
							build::variable("t"))))));
}

template <typename N>
tref tree<N>::build::in_variable_at_t(size_t index) {
	return build::in_variable_at_t(build::in_var_name(index));
}

template <typename N>
tref tree<N>::build::in_variable_at_t_minus (const std::string& name, int_t shift) {
	using p = tau_parser;
	auto var_name = get(p::in_var_name, name);
	auto shift_node = get(p::shift, {get(p::variable, "t"), get_num(shift)});
	auto offset = get(p::offset, shift_node);
	return get(p::bf, get(p::variable, get(p::io_var, get(p::in, { var_name, offset }))));
}

template <typename N>
tref tree<N>::build::in_variable_at_t_minus(tref var_name, size_t num) {
	DBG(assert(tree::get(var_name).is(node::type::in_var_name));)
	DBG(assert(num > 0);)

	return get(
		tau_parser::bf, get(
			tau_parser::variable, get(
				tau_parser::io_var, get(
					tau_parser::in,
						var_name, get(
						tau_parser::offset, get(
							tau_parser::shift,
								build::variable("t"),
								get_num(num)))))));
}

template <typename N>
tref tree<N>::build::in_variable_at_t_minus(size_t index, size_t num) {
	return build::in_variable_at_t_minus(build::in_var_name(index), num);
}

template <typename N>
tref tree<N>::build::out_var_name(size_t index) {
	std::stringstream var_name;
	var_name << "o" << index;
	return get(tau_parser::out_var_name, var_name.str());
}

template <typename N>
tref tree<N>::build::out_var_name(const std::string& name) {
	return get(tau_parser::out_var_name, name);
}

template <typename N>
tref tree<N>::build::out_variable_at_t(tref var_name) {
	DBG(assert(tree::get(var_name).is(node::type::out_var_name));)
	return get(
		tau_parser::bf,	get(
			tau_parser::variable, get(
				tau_parser::io_var, get(
					tau_parser::out,
						var_name, get(
						tau_parser::offset,
							build::variable("t"))))));
}

template <typename N>
tref tree<N>::build::out_variable_at_t(size_t index) {
	return build::out_variable_at_t(build::out_var_name(index));
}

template <typename N>
tref tree<N>::build::out_variable_at_n(tref var_name, size_t num) {
	DBG(assert(tree::get(var_name).is(node::type::out_var_name));)

	return get(
		tau_parser::bf,	get(
			tau_parser::variable, get(
				tau_parser::io_var, get(
					tau_parser::out,
						var_name, get(
						tau_parser::offset,
							get_integer(num))))));
}

template <typename N>
tref tree<N>::build::out_variable_at_n(size_t index, size_t num) {
	return build::out_variable_at_n(build::out_var_name(index), num);
}

template <typename N>
tref tree<N>::build::out_variable_at_n(const std::string& name, const int_t pos) {
	using p = tau_parser;
	auto var_name = get(p::out_var_name, name);
	auto offset = get(p::offset, get_integer(pos));
	return get(p::bf, get(p::variable,
		get(p::io_var, get(p::out, { var_name, offset }))));
}

template <typename N>
tref tree<N>::build::out_variable_at_t_minus (const std::string& name, const int_t shift) {
	using p = tau_parser;
	auto var_name = get(p::out_var_name, name);
	auto shift_node = get(p::shift, {get(p::variable, "t"), get_num(shift)});
	auto offset = get(p::offset, shift_node);
	return get(p::bf, get(p::variable, get(p::io_var, get(p::out, { var_name, offset }))));
}

template <typename N>
tref tree<N>::build::out_variable_at_t_minus(tref var_name, size_t num) {
	DBG(assert(tree::get(var_name).is(node::type::out_var_name));)
	DBG(assert(num > 0);)

	return get(
		tau_parser::bf,	get(
			tau_parser::variable, get(
				tau_parser::io_var, get(
					tau_parser::out,
						var_name, get(
						tau_parser::offset, get(
							tau_parser::shift,
								build::variable("t"),
								get_num(num)))))));
}

template <typename N>
tref tree<N>::build::out_variable_at_t_minus(size_t index, size_t num) {
	return build::out_variable_at_t_minus(build::out_var_name(index), num);
}

// // ------ Helpers for variables having io_var as child ---------------
// template <typename N>
// auto is_io_initial (tref io_var) {
// 	return (trim2(io_var)->child[1] | tau_parser::integer).has_value();
// }

// template <typename N>
// auto is_io_shift (tref io_var) {
// 	return (trim2(io_var)->child[1] | tau_parser::shift).has_value();
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
// tref build::bf_var(const std::string& name) {
// 	auto var = make_builder("( $X ) =: " + name + ".").second;
// 	return trim(var);
// }

template <typename N>
tref tree<N>::build::bf_constant(tref cte) {
	return get(
		tau_parser::bf, get(
			tau_parser::bf_constant, get(
				tau_parser::constant,
					cte)));
}

template <typename N>
tref tree<N>::build::bf_constant(tref cte, tref type) {
	return get(
		tau_parser::bf, get(
			tau_parser::bf_constant, get(
				tau_parser::constant,
					cte),
				type));
}

template <typename N>
tref tree<N>::build::bf_constant(tref cte, const std::string& type) {
	auto t = build::type(type);
	return get(tau_parser::bf,
		get(tau_parser::bf_constant,
			get(tau_parser::constant, cte), t));
}

// template <typename N>
// tref tree<N>::build::bf_constant(const std::variant& v) {
// 	auto cte = depreciating::rewriter::make_node<tau_sym>(tau_sym(v), {});
// 	return build::bf_constant(cte);
// }

// template <typename N>
// tref tree<N>::build::bf_constant(const std::variant& v, tref type) {
// 	auto cte = depreciating::rewriter::make_node<tau_sym>(tau_sym(v), {});
// 	return build::bf_constant(cte, type);
// }

// template <typename N>
// tref tree<N>::build::bf_constant(const std::variant& v, const std::optional<tref>& type) {
// 	auto cte = depreciating::rewriter::make_node<tau_sym>(tau_sym(v), {});
// 	return type.has_value() ? build::bf_constant(cte, type.value()) : build::bf_constant(cte);
// }

// template <typename N>
// tref tree<N>::build::bf_and_constant( const auto& ctes, tref type)
// {
// 	if (ctes.empty()) return _1;

// 	auto cte = std::accumulate(++ctes.begin(), ctes.end(), *ctes.begin(),
// 		[&](const auto& l, const auto& r) { return l & r; });

// 	return build::bf_constant(cte, type);
// }

// template <typename N>
// tref tree<N>::build::bf_or_constant( const auto& ctes, tref type)
// {
// 	if (ctes.empty()) return _0;

// 	auto cte = std::accumulate(++ctes.begin(), ctes.end(), *ctes.begin(),
// 		[&](const auto& l, const auto& r) { return l | r; });

// 	return build::bf_constant(cte, type);
// }

// template <typename N>
// tref tree<N>::build::bf_constant(
// 	const std::optional<std::variant>& o)
// {
// 	return o.has_value() ? build::bf_constant(o.value())
// 				: std::optional<tref>();
// }

template <typename N>
tref tree<N>::build::bf_uninter_const(const std::string& n1, const std::string& n2) {
	auto name = get(tau_parser::uninter_const_name, n1 + ":" + n2);
	return get(tau_parser::bf,
		get(tau_parser::variable,
			get(tau_parser::uninterpreted_constant, name)));
}

// wff factory method for building wff formulas
template <typename N>
tref tree<N>::build::wff_eq(tref l) {
	return get(tau_parser::wff, get(tau_parser::bf_eq, l, _0));
}

template <typename N>
tref tree<N>::build::wff_neq(tref l) {
	return get(tau_parser::wff, get(tau_parser::bf_neq, l, _0));
}

template <typename N>
tref tree<N>::build::wff_and(tref l,
	tref r)
{
	return get(tau_parser::wff, get(tau_parser::wff_and, l, r));
}

template <typename N>
tref tree<N>::build::wff_and(const auto& wffs) {
	return std::accumulate(wffs.begin(), wffs.end(), _T,
		[](const auto& l, const auto& r) {return build::wff_and(l, r);});
}

template <typename N>
tref tree<N>::build::wff_or(tref l,
	tref r)
{
	return get(tau_parser::wff, get(tau_parser::wff_or, l, r));
}

template <typename N>
tref tree<N>::build::wff_or(const auto& wffs) {
	return std::accumulate(wffs.begin(), wffs.end(), _F,
		[](const auto& l, const auto& r) { return build::wff_or(l, r);});
}

template <typename N>
tref tree<N>::build::wff_neg(tref l) {
	return get(tau_parser::wff, get(tau_parser::wff_neg, l));
}

template <typename N>
tref tree<N>::build::wff_xor_from_def(tref l,
	tref r)
{
	return build::wff_or(build::wff_and(build::wff_neg(l), r),
		build::wff_and(build::wff_neg(r), l));
}

template <typename N>
tref tree<N>::build::wff_xor(tref l,
	tref r)
{
	return build::wff_or(
		build::wff_and(build::wff_neg(l), r),
		build::wff_and(build::wff_neg(r), l));
}

template <typename N>
tref tree<N>::build::wff_imply(tref l,
	tref r)
{
	return build::wff_or(build::wff_neg(l), r);
}

template <typename N>
tref tree<N>::build::wff_conditional(tref x,
	tref y,
	tref z)
{
	return build::wff_and(build::wff_imply(x, y),
		build::wff_imply(build::wff_neg(x), z));
}

template <typename N>
tref tree<N>::build::wff_equiv(tref l,
	tref r)
{
	return build::wff_and(build::wff_imply(l, r),
		build::wff_imply(r, l));
}

template <typename N>
tref tree<N>::build::wff_all(tref l,
	tref r)
{
	return get(tau_parser::wff, get(tau_parser::wff_all, l, r));
}

template <typename N>
tref tree<N>::build::wff_ex(tref l,
	tref r)
{
	return get(tau_parser::wff, get(tau_parser::wff_ex, l, r));
}

template <typename N>
tref tree<N>::build::wff_sometimes(tref l) {
	return get(tau_parser::wff, get(tau_parser::wff_sometimes, l));
}

template <typename N>
tref tree<N>::build::wff_always(tref l) {
	return get(tau_parser::wff, get(tau_parser::wff_always, l));
}

// bf factory method for building bf formulas
template <typename N>
tref tree<N>::build::bf_and(tref l,
	tref r)
{
	return get(tau_parser::bf, get(tau_parser::bf_and, l, r));
}

template <typename N>
tref tree<N>::build::bf_and(const auto& bfs) {
	return std::accumulate(bfs.begin(), bfs.end(), _1,
		[](const auto& l, const auto& r) { return build::bf_and(l, r);});
}

template <typename N>
tref tree<N>::build::bf_or(tref l,
	tref r)
{
	return get(tau_parser::bf, get(tau_parser::bf_or, l, r));
}

template <typename N>
tref tree<N>::build::bf_or(const auto& bfs) {
	return std::accumulate(bfs.begin(), bfs.end(), _0,
		[](const auto& l, const auto& r) { return build::bf_or(l, r); });
}

template <typename N>
tref tree<N>::build::bf_neg(tref l) {
	return get(tau_parser::bf, get(tau_parser::bf_neg, l));
}

template <typename N>
tref tree<N>::build::bf_xor_from_def(tref l,
	tref r)
{
	return build::bf_or(
		build::bf_and(build::bf_neg(l), r),
		build::bf_and(build::bf_neg(r), l));
}

template <typename N>
tref tree<N>::build::bf_xor(tref l,
	tref r)
{
	return build::bf_or(
		build::bf_and(build::bf_neg(l), r),
		build::bf_and(l, build::bf_neg(r)));
}

template <typename N>
tref tree<N>::build::wff_eq(tref l, tref r) {
// #ifdef DEBUG
// 	using p = tau_parser;
// 	assert(is_non_terminal(p::bf, l) && is_non_terminal(p::bf, r));
// #endif // DEBUG
	auto left_side = build::bf_xor(l,r);
	return build::wff_eq(left_side);
}

template <typename N>
tref tree<N>::build::bf_less(tref l, tref r) {
	return build::wff_and(
		build::wff_eq(
			build::bf_and(l, build::bf_neg(r))),
		build::wff_neq(build::bf_xor(l, r)));
}

template <typename N>
tref tree<N>::build::bf_nless(tref l, tref r) {
	return build::wff_neg(build::bf_less(l, r));
}

template <typename N>
tref tree<N>::build::bf_less_equal(tref l, tref r) {
	return build::wff_eq(build::bf_and(l, build::bf_neg(r)));
}

template <typename N>
tref tree<N>::build::bf_nleq(tref l, tref r) {
	return build::wff_neq(build::bf_and(l, build::bf_neg(r)));
}

template <typename N>
tref tree<N>::build::bf_interval(tref x, tref y, tref z) {
	return build::wff_and(build::bf_less_equal(x, y),
		build::bf_less_equal(y, z));
}

template <typename N>
tref tree<N>::build::bf_nleq_lower(tref l, tref r) {
	trefs args { trim(l), trim(r) };
	return apply_builder(builder_bf_nleq_lower<N>, args);
}

template <typename N>
tref tree<N>::build::bf_nleq_upper(tref l, tref r) {
	trefs args { trim(l), trim(r) };
	return apply_builder(builder_bf_nleq_upper<N>, args);
}

template <typename N>
tref tree<N>::build::bf_greater(tref l, tref r) {
	return build::bf_less(r, l);
}

template <typename N>
tref tree<N>::build::bf_ngreater(tref l, tref r) {
	return build::wff_neg(build::bf_greater(l, r));
}

template <typename N>
tref tree<N>::build::bf_greater_equal(tref l, tref r) {
	return build::bf_less_equal(r, l);
}

template <typename N>
tref tree<N>::build::bf_ngeq(tref l, tref r) {
	return build::wff_neg(build::bf_greater_equal(l, r));
}

template <typename N>
tref tree<N>::build::wff_ctn_greater_equal(tref ctnvar, tref num) {
	return get(tau_parser::wff,
			get(tau_parser::constraint,
				get(tau_parser::ctn_greater_equal, {ctnvar, num})));
}

template <typename N>
tref tree<N>::build::wff_ctn_greater(tref ctnvar, tref num) {
	return get(tau_parser::wff,
			get(tau_parser::constraint,
				get(tau_parser::ctn_greater, {ctnvar, num})));
}

template <typename N>
tref tree<N>::build::wff_ctn_less_equal(tref ctnvar, tref num) {
	return get(tau_parser::wff,
			get(tau_parser::constraint,
				get(tau_parser::ctn_less_equal, {ctnvar, num})));
}

template <typename N>
tref tree<N>::build::wff_ctn_less(tref ctnvar, tref num) {
	return get(tau_parser::wff,
			get(tau_parser::constraint,
				get(tau_parser::ctn_less, {ctnvar, num})));
}

template <typename N>
tref tree<N>::build::wff_ctn_eq(tref ctnvar, tref num) {
	return get(tau_parser::wff,
			get(tau_parser::constraint,
				get(tau_parser::ctn_eq, {ctnvar, num})));
}

template <typename N>
tref tree<N>::build::wff_ctn_neq(tref ctnvar, tref num) {
	return get(tau_parser::wff,
			get(tau_parser::constraint,
				get(tau_parser::ctn_neq, {ctnvar, num})));
}

//------------------------------------------------------------------------------

} // idni::tau_lang namespace
