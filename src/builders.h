// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __BUILDERS_H__
#define __BUILDERS_H__

#include "queries.h"
#include "language.h"

namespace idni::tau_lang {

// creates a specific builder from a tau.
template <typename... BAs>
builder<BAs...> make_builder(const tau<BAs...>& builder) {
	auto head = builder | tau_parser::builder_head
		| optional_value_extractor<tau<BAs...>>;
	auto type_node = builder
		| tau_parser::builder_body | only_child_extractor<BAs...>;
	auto type = type_node | non_terminal_extractor<BAs...>
		| optional_value_extractor<size_t>;
	switch (type) {
	case tau_parser::bf_builder_body:  return { head, type_node
		| tau_parser::bf
		| optional_value_extractor<tau<BAs...>>};
	case tau_parser::wff_builder_body: return { head, type_node
		| tau_parser::wff
		| optional_value_extractor<tau<BAs...>>};
	default: throw std::runtime_error("unknown builder type");
	};
}

// create a builder from a given tau source.
template <typename... BAs>
builder<BAs...> make_builder(sp_tau_source_node& tau_source) {
	auto builder = make_tau_code<BAs...>(tau_source);
	return make_builder(builder);
}

// make a builder from the given tau source string.
template <typename... BAs>
builder<BAs...> make_builder(const std::string& source) {
	auto tau_source = make_tau_source(source, {
						.start = tau_parser::builder });
	return make_builder<BAs...>(tau_source);
}

template <typename... BAs>
tau<BAs...> tau_apply_builder(const builder<BAs...>& b,
	std::vector<tau<BAs...>>& n)
{
	std::map<tau<BAs...>, tau<BAs...>> changes;
	std::vector<tau<BAs...>> vars = b.first || tau_parser::capture;
	for (size_t i = 0; i < vars.size(); ++i) changes[vars[i]] = n[i];
	return replace<tau<BAs...>>(b.second, changes);
}

// definitions of basic bf and wff
const std::string BLDR_BF_0 = "( $X ) =: 0.";
const std::string BLDR_BF_1 = "( $X ) =: 1.";
const std::string BLDR_WFF_F = "( $X ) =:: F.";
const std::string BLDR_WFF_T = "( $X ) =:: T.";

// definitions of wff builder rules
const std::string BLDR_WFF_EQ = "( $X ) =:: $X = 0.";
const std::string BLDR_WFF_NEQ = "( $X ) =:: $X != 0.";
const std::string BLDR_BF_NOT_LESS_EQUAL = "( $X $Y ) =:: $X !<= $Y.";
const std::string BDLR_BF_INTERVAL = "( $X $Y $Z ) =:: $X <= $Y <= $Z.";
const std::string BDLR_BF_NLEQ_UPPER = "( $X $Y ) =:: $X !<= $Y.";
const std::string BDLR_BF_NLEQ_LOWWER = "( $X $Y ) =:: $Y !<= $X.";
const std::string BLDR_WFF_ALL = "( $X $Y ) =:: all $X $Y.";
const std::string BLDR_WFF_EX = "( $X $Y ) =:: ex $X $Y.";
const std::string BLDR_WFF_SOMETIMES = "( $X ) =:: sometimes $X.";
const std::string BLDR_WFF_ALWAYS = "( $X ) =:: always $X.";

// definitions of bf builder rules
const std::string BLDR_BF_SPLITTER = "( $X ) =: S($X).";


// basic bf and wff builders
template <typename... BAs>
static auto bldr_bf_0 = make_builder<BAs...>(BLDR_BF_0);
template <typename... BAs>
static auto bldr_bf_1 = make_builder<BAs...>(BLDR_BF_1);
template <typename... BAs>
static auto bldr_wff_F = make_builder<BAs...>(BLDR_WFF_F);
template <typename... BAs>
static auto bldr_wff_T = make_builder<BAs...>(BLDR_WFF_T);

// wff builder
template <typename... BAs>
static auto bldr_wff_eq = make_builder<BAs...>(BLDR_WFF_EQ);
template <typename... BAs>
static auto bldr_bf_splitter = make_builder<BAs...>(BLDR_BF_SPLITTER);
template <typename... BAs>
static auto bldr_bf_not_less_equal =
				make_builder<BAs...>(BLDR_BF_NOT_LESS_EQUAL);
template <typename... BAs>
static auto bldr_bf_interval = make_builder<BAs...>(BDLR_BF_INTERVAL);
template <typename... BAs>
static auto bldr_bf_nleq_upper = make_builder<BAs...>(BDLR_BF_NLEQ_UPPER);
template <typename... BAs>
static auto bldr_bf_nleq_lowwer = make_builder<BAs...>(BDLR_BF_NLEQ_LOWWER);

// basic bf and wff constants
template <typename... BAs>
static const tau<BAs...> _0 = bldr_bf_0<BAs...>.second;

template <typename... BAs>
static const tau<BAs...> _0_trimmed = trim(_0<BAs...>);

template <typename... BAs>
static const tau<BAs...> _1 = bldr_bf_1<BAs...>.second;

template <typename... BAs>
static const tau<BAs...> _1_trimmed = trim(_1<BAs...>);

template <typename... BAs>
static const tau<BAs...> _F = bldr_wff_F<BAs...>.second;

template <typename... BAs>
static const tau<BAs...> _F_trimmed = trim(_F<BAs...>);

template <typename... BAs>
static const tau<BAs...> _T = bldr_wff_T<BAs...>.second;

template <typename... BAs>
static const tau<BAs...> _T_trimmed = trim(_T<BAs...>);

template <typename... BAs>
tau<BAs...> build_num(size_t num) {
	return wrap(
		tau_parser::num, rewriter::make_node<tau_sym<BAs...>>(
			tau_sym<BAs...>(num), {}));
}

template<typename... BAs>
tau<BAs...> build_int(int_t i) {
	using p = tau_parser;
	std::vector<tau<BAs...>> children;
	const bool n = i < 0;
	size_t u = n ? (size_t)(-i) : (size_t)i;
	if (n) {
		children.emplace_back(rewriter::make_node<tau_sym<BAs...>>(
				tau_source_sym('-'), {}));
	}
	children.emplace_back(rewriter::make_node<tau_sym<BAs...>>(
				tau_sym<BAs...>(u),{}));
	return wrap(p::integer, children);
}

template <typename... BAs>
tau<BAs...> build_variable(const std::string& name) {
	return wrap<BAs...>(tau_parser::variable, name);
}

template <typename... BAs>
tau<BAs...> build_in_var_name(const size_t& index) {
	std::stringstream var_name;
	var_name << "i" << index;
	return wrap<BAs...>(tau_parser::in_var_name, var_name.str());
}

template<typename... BAs>
tau<BAs...> build_type(const std::string& type) {
	return wrap<BAs...>(tau_parser::type, type);
}

template<typename... BAs>
tau<BAs...> build_bf_t_type(const std::string& type) {
	return wrap(
		tau_parser::bf, wrap(
			tau_parser::bf_t,
				build_type<BAs...>(type)));
}

template<typename... BAs>
tau<BAs...> build_bf_t_type(const tau<BAs...>& type) {
	return wrap(
		tau_parser::bf, wrap(
			tau_parser::bf_t,
				type));
}

template<typename... BAs>
tau<BAs...> build_bf_f_type(const std::string& type) {
	return wrap(
		tau_parser::bf, wrap(
			tau_parser::bf_f,
				build_type<BAs...>(type)));
}

template<typename... BAs>
tau<BAs...> build_bf_f_type(const tau<BAs...>& type) {
	return wrap(
		tau_parser::bf, wrap(
			tau_parser::bf_f,
				type));
}

template<typename... BAs>
tau<BAs...> build_in_variable_at_n(const tau<BAs...>& in_var_name, const size_t& num) {
	assert(is_non_terminal(tau_parser::in_var_name, in_var_name));

	return wrap(
		tau_parser::bf, wrap(
			tau_parser::variable, wrap(
				tau_parser::io_var,	wrap(
					tau_parser::in,
						in_var_name, wrap(
						tau_parser::offset,
							build_int<BAs...>(num))))));
}

template <typename... BAs>
tau<BAs...> build_in_variable_at_n(const size_t& index, const size_t& num) {
	return build_in_variable_at_n(build_in_var_name<BAs...>(index), num);
}

template<typename... BAs>
tau<BAs...> build_in_variable_at_n(const std::string& name, const int_t pos) {
	using p = tau_parser;
	auto var_name = wrap<BAs...>(p::in_var_name, name);
	auto offset = wrap<BAs...>(p::offset, build_int<BAs...>(pos));
	return wrap(p::bf, wrap(p::variable,
		wrap(p::io_var, wrap(p::in, { var_name, offset }))));
}

template <typename... BAs>
tau<BAs...> build_in_variable_at_t(const tau<BAs...>& in_var_name) {
	assert(is_non_terminal(tau_parser::in_var_name, in_var_name));

	return wrap(
		tau_parser::bf, wrap(
			tau_parser::variable, wrap(
				tau_parser::io_var,	wrap(
					tau_parser::in,
						in_var_name, wrap(
						tau_parser::offset,
							build_variable<BAs...>('t'))))));
}

template <typename... BAs>
tau<BAs...> build_in_variable_at_t(const size_t& index) {
	return build_in_variable_at_t(build_in_var_name<BAs...>(index));
}

template <typename... BAs>
tau<BAs...> build_in_variable_at_t_minus (const std::string& name, const int_t shift) {
	using p = tau_parser;
	auto var_name = wrap<BAs...>(p::in_var_name, name);
	auto shift_node = wrap<BAs...>(p::shift, {wrap<BAs...>(p::variable, "t"), build_num<BAs...>(shift)});
	auto offset = wrap<BAs...>(p::offset, shift_node);
	return wrap<BAs...>(p::bf, wrap<BAs...>(p::variable, wrap<BAs...>(p::io_var, wrap<BAs...>(p::in, { var_name, offset }))));
}

template <typename... BAs>
tau<BAs...> build_in_variable_at_t_minus(const tau<BAs...>& in_var_name, const size_t& num) {
	assert(is_non_terminal(tau_parser::in_var_name, in_var_name));
	assert(num > 0);

	return wrap(
		tau_parser::bf, wrap(
			tau_parser::variable, wrap(
				tau_parser::io_var,	wrap(
					tau_parser::in,
						in_var_name, wrap(
						tau_parser::offset, wrap(
							tau_parser::shift,
								build_variable<BAs...>('t'),
								build_num<BAs...>(num)))))));
}

template <typename... BAs>
tau<BAs...> build_in_variable_at_t_minus(const size_t& index, const size_t& num) {
	return build_in_variable_at_t_minus(build_in_var_name<BAs...>(index), num);
}

template <typename... BAs>
tau<BAs...> build_out_var_name(const size_t& index) {
	std::stringstream var_name;
	var_name << "o" << index;
	return wrap<BAs...>(tau_parser::out_var_name, var_name.str());
}

template <typename... BAs>
tau<BAs...> build_out_var_name(const std::string& name) {
	return wrap<BAs...>(tau_parser::out_var_name, name);
}

template <typename... BAs>
tau<BAs...> build_out_variable_at_t(const tau<BAs...>& out_var_name) {
	assert(is_non_terminal(tau_parser::out_var_name, out_var_name));
	return wrap(
		tau_parser::bf,	wrap(
			tau_parser::variable, wrap(
				tau_parser::io_var, wrap(
					tau_parser::out,
						out_var_name, wrap(
						tau_parser::offset,
							build_variable<BAs...>('t'))))));
}

template <typename... BAs>
tau<BAs...> build_out_variable_at_t(const size_t& index) {
	return build_out_variable_at_t(build_out_var_name<BAs...>(index));
}

template <typename... BAs>
tau<BAs...> build_out_variable_at_n(const tau<BAs...>& out_var_name, const size_t& num) {
	assert(is_non_terminal(tau_parser::out_var_name, out_var_name));

	return wrap(
		tau_parser::bf,	wrap(
			tau_parser::variable, wrap(
				tau_parser::io_var, wrap(
					tau_parser::out,
						out_var_name, wrap(
						tau_parser::offset,
							build_int<BAs...>(num))))));
}

template <typename... BAs>
tau<BAs...> build_out_variable_at_n(const size_t& index, const size_t& num) {
	return build_out_variable_at_n(build_out_var_name<BAs...>(index), num);
}

template<typename... BAs>
tau<BAs...> build_out_variable_at_n(const std::string& name, const int_t pos) {
	using p = tau_parser;
	auto var_name = wrap<BAs...>(p::out_var_name, name);
	auto offset = wrap<BAs...>(p::offset, build_int<BAs...>(pos));
	return wrap(p::bf, wrap(p::variable,
		wrap(p::io_var, wrap(p::out, { var_name, offset }))));
}

template <typename... BAs>
tau<BAs...> build_out_variable_at_t_minus (const std::string& name, const int_t shift) {
	using p = tau_parser;
	auto var_name = wrap<BAs...>(p::out_var_name, name);
	auto shift_node = wrap<BAs...>(p::shift, {wrap<BAs...>(p::variable, "t"), build_num<BAs...>(shift)});
	auto offset = wrap<BAs...>(p::offset, shift_node);
	return wrap<BAs...>(p::bf, wrap<BAs...>(p::variable, wrap<BAs...>(p::io_var, wrap<BAs...>(p::out, { var_name, offset }))));
}

template <typename... BAs>
tau<BAs...> build_out_variable_at_t_minus(const tau<BAs...>& out_var_name, const size_t& num) {
	assert(is_non_terminal(tau_parser::out_var_name, out_var_name));
	assert(num > 0);

	return wrap(
		tau_parser::bf,	wrap(
			tau_parser::variable, wrap(
				tau_parser::io_var, wrap(
					tau_parser::out,
						out_var_name, wrap(
						tau_parser::offset, wrap(
							tau_parser::shift,
								build_variable<BAs...>('t'),
								build_num<BAs...>(num)))))));
}

template <typename... BAs>
tau<BAs...> build_out_variable_at_t_minus(const size_t& index, const size_t& num) {
	return build_out_variable_at_t_minus(build_out_var_name<BAs...>(index), num);
}

// ------ Helpers for variables having io_var as child ---------------
template <typename... BAs>
auto is_io_initial (const tau<BAs...>& io_var) {
	return (trim2(io_var)->child[1] | tau_parser::integer).has_value();
}

template <typename... BAs>
auto is_io_shift (const tau<BAs...>& io_var) {
	return (trim2(io_var)->child[1] | tau_parser::shift).has_value();
}

template <typename... BAs>
auto get_io_time_point (const tau<BAs...>& io_var) {
	return int_extractor<BAs...>(trim(trim2(io_var)->child[1]));
}

template <typename... BAs>
auto get_io_shift (const tau<BAs...>& io_var) {
	return size_t_extractor<BAs...>(trim2(io_var)->child[1]->child[0]->child[1]->child[0]).value();
}

template <typename... BAs>
std::string get_io_name (const tau<BAs...>& io_var) {
	std::stringstream ss; ss << trim(trim2(io_var));
	return ss.str();
}

template<typename... BAs>
tau<BAs...> get_tau_io_name(const tau<BAs...>& io_var) {
	return trim(trim2(io_var));
}

template <typename... BAs>
int_t get_io_var_shift(const tau<BAs...>& io_var) {
	// If there is a shift
	if (is_io_shift(io_var))
		return get_io_shift(io_var);
	return 0;
}

int_t get_max_shift(const auto& io_vars, bool ignore_temps = false) {
	int_t max_shift = 0;
	for (const auto& v : io_vars) {
		if (ignore_temps && get_io_name(v)[0] == '_')
			continue;
		max_shift = std::max(max_shift, get_io_var_shift(v));
	}
	return max_shift;
}

template <typename... BAs>
int_t get_max_initial(const std::vector<tau<BAs...>>& io_vars) {
	int_t max_init = -1;
	for (const tau<BAs...>& v : io_vars) {
		if (is_io_initial(v)) {
			int_t init = get_io_time_point(v);
			max_init = std::max(max_init, init);
		}
	}
	return max_init;
}

// -------------------------------------------------------------------

template <typename... BAs>
tau<BAs...> build_bf_var(const std::string& name) {
	auto var = make_builder<BAs...>("( $X ) =: " + name + ".").second;
	return trim<BAs...>(var);
}

template <typename... BAs>
tau<BAs...> build_wff_var(const std::string& name) {
	auto var = make_builder<BAs...>("( $X ) =:: ?" + name + ".").second;
	return trim<BAs...>(var);
}

template <typename... BAs>
tau<BAs...> build_bf_constant(const tau<BAs...>& cte) {
	return wrap(
		tau_parser::bf, wrap(
			tau_parser::bf_constant, wrap(
				tau_parser::constant,
					cte)));
}

template<typename... BAs>
tau<BAs...> build_bf_constant(const tau<BAs...>& cte, const tau<BAs...>& type) {
	return wrap(
		tau_parser::bf, wrap(
			tau_parser::bf_constant, wrap(
				tau_parser::constant,
					cte),
				type));
}

template<typename... BAs>
tau<BAs...> build_bf_constant(const tau<BAs...>& cte, const std::string& type) {
	auto t = build_type<BAs...>(type);
	return wrap(tau_parser::bf,
		wrap(tau_parser::bf_constant,
			wrap(tau_parser::constant, cte), t));
}

template <typename... BAs>
tau<BAs...> build_bf_constant(const std::variant<BAs...>& v) {
	auto cte = rewriter::make_node<tau_sym<BAs...>>(tau_sym<BAs...>(v), {});
	return build_bf_constant<BAs...>(cte);
}

template<typename... BAs>
tau<BAs...> build_bf_constant(const std::variant<BAs...>& v, const tau<BAs...>& type) {
	auto cte = rewriter::make_node<tau_sym<BAs...>>(tau_sym<BAs...>(v), {});
	return build_bf_constant<BAs...>(cte, type);
}

template<typename... BAs>
tau<BAs...> build_bf_constant(const std::variant<BAs...>& v, const std::optional<tau<BAs...>>& type) {
	auto cte = rewriter::make_node<tau_sym<BAs...>>(tau_sym<BAs...>(v), {});
	return type.has_value() ? build_bf_constant<BAs...>(cte, type.value()) : build_bf_constant<BAs...>(cte);
}

template<typename... BAs>
tau<BAs...> build_bf_and_constant( const auto& ctes)
{
	if (ctes.empty()) return _1<BAs...>;

	auto cte = std::accumulate(++ctes.begin(), ctes.end(), *ctes.begin(),
		[&](const auto& l, const auto& r) { return l & r; });

	return build_bf_constant<BAs...>(cte);
}

template <typename... BAs>
tau<BAs...> build_bf_or_constant( const auto& ctes)
{
	if (ctes.empty()) return _0<BAs...>;

	auto cte = std::accumulate(++ctes.begin(), ctes.end(), *ctes.begin(),
		[&](const auto& l, const auto& r) { return l | r; });

	return build_bf_constant<BAs...>(cte);
}

template <typename... BAs>
std::optional<tau<BAs...>> build_bf_constant(
	const std::optional<std::variant<BAs...>>& o)
{
	return o.has_value() ? build_bf_constant(o.value())
				: std::optional<tau<BAs...>>();
}

template <typename... BAs>
tau<BAs...> build_bf_uniter_const(const std::string& n1, const std::string& n2) {
	auto name = wrap<BAs...>(tau_parser::uninter_const_name, n1 + ":" + n2);
	return wrap(tau_parser::bf,
		wrap(tau_parser::variable,
			wrap(tau_parser::uninterpreted_constant, name)));
}

// wff factory method for building wff formulas
template <typename... BAs>
tau<BAs...> build_wff_eq(const tau<BAs...>& l) {
	return wrap(tau_parser::wff, wrap(tau_parser::bf_eq, l, _0<BAs...>));
}

template <typename... BAs>
tau<BAs...> build_wff_neq(const tau<BAs...>& l) {
	return wrap(tau_parser::wff, wrap(tau_parser::bf_neq, l, _0<BAs...>));
}

template <typename... BAs>
tau<BAs...> build_wff_and(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return wrap(tau_parser::wff, wrap(tau_parser::wff_and, l, r));
}

template <typename... BAs>
tau<BAs...> build_wff_and(const auto& wffs) {
	return std::accumulate(wffs.begin(), wffs.end(), _T<BAs...>,
		[](const auto& l, const auto& r) {return build_wff_and<BAs...>(l, r);});
}

template <typename... BAs>
tau<BAs...> build_wff_or(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return wrap(tau_parser::wff, wrap(tau_parser::wff_or, l, r));
}

template <typename... BAs>
tau<BAs...> build_wff_or(const auto& wffs) {
	return std::accumulate(wffs.begin(), wffs.end(), _F<BAs...>,
		[](const auto& l, const auto& r) { return build_wff_or<BAs...>(l, r);});
}

template <typename... BAs>
tau<BAs...> build_wff_neg(const tau<BAs...>& l) {
	return wrap(tau_parser::wff, wrap(tau_parser::wff_neg, l));
}

template <typename... BAs>
tau<BAs...> build_wff_xor_from_def(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_wff_or<BAs...>(build_wff_and(build_wff_neg(l), r),
		build_wff_and(build_wff_neg(r), l));
}

template <typename... BAs>
tau<BAs...> build_wff_xor(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_wff_or(
		build_wff_and(build_wff_neg(l), r),
		build_wff_and(build_wff_neg(r), l));
}

template <typename... BAs>
tau<BAs...> build_wff_imply(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_wff_or<BAs...>(build_wff_neg<BAs...>(l), r);
}

template <typename... BAs>
tau<BAs...> build_wff_conditional(const tau<BAs...>& x,
	const tau<BAs...>& y,
	const tau<BAs...>& z)
{
	return build_wff_and<BAs...>(build_wff_imply<BAs...>(x, y),
		build_wff_imply<BAs...>(build_wff_neg<BAs...>(x), z));
}

template <typename... BAs>
tau<BAs...> build_wff_equiv(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_wff_and<BAs...>(build_wff_imply<BAs...>(l, r),
		build_wff_imply<BAs...>(r, l));
}

template <typename... BAs>
tau<BAs...> build_wff_all(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return wrap(tau_parser::wff, wrap(tau_parser::wff_all, l, r));
}

template <typename... BAs>
tau<BAs...> build_wff_ex(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return wrap(tau_parser::wff, wrap(tau_parser::wff_ex, l, r));
}

template <typename... BAs>
tau<BAs...> build_wff_sometimes(const tau<BAs...>& l) {
	return wrap(tau_parser::wff, wrap(tau_parser::wff_sometimes, l));
}

template <typename... BAs>
tau<BAs...> build_wff_always(const tau<BAs...>& l) {
	return wrap(tau_parser::wff, wrap(tau_parser::wff_always, l));
}

// bf factory method for building bf formulas
template <typename... BAs>
tau<BAs...> build_bf_and(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return wrap(tau_parser::bf, wrap(tau_parser::bf_and, l, r));
}

template <typename... BAs>
tau<BAs...> build_bf_and(const auto& bfs) {
	return std::accumulate(bfs.begin(), bfs.end(), _1<BAs...>,
		[](const auto& l, const auto& r) { return build_bf_and<BAs...>(l, r);});
}

template <typename... BAs>
tau<BAs...> build_bf_or(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return wrap(tau_parser::bf, wrap(tau_parser::bf_or, l, r));
}

template <typename... BAs>
tau<BAs...> build_bf_or(const auto& bfs) {
	return std::accumulate(bfs.begin(), bfs.end(), _0<BAs...>,
		[](const auto& l, const auto& r) { return build_bf_or<BAs...>(l, r); });
}

template <typename... BAs>
tau<BAs...> build_bf_neg(const tau<BAs...>& l) {
	return wrap(tau_parser::bf, wrap(tau_parser::bf_neg, l));
}

template <typename... BAs>
tau<BAs...> build_bf_xor_from_def(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_bf_or<BAs...>(
		build_bf_and(build_bf_neg(l), r),
		build_bf_and(build_bf_neg(r), l));
}

template <typename... BAs>
tau<BAs...> build_bf_xor(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_bf_or<BAs...>(
		build_bf_and<BAs...>(build_bf_neg<BAs...>(l), r),
		build_bf_and<BAs...>(l, build_bf_neg<BAs...>(r)));
}

template<typename... BAs>
tau<BAs...> build_wff_eq(const tau<BAs...>& l, const tau<BAs...>& r) {
#ifdef DEBUG
	using p = tau_parser;
	assert(is_non_terminal(p::bf, l) && is_non_terminal(p::bf, r));
#endif // DEBUG
	auto left_side = build_bf_xor(l,r);
	return build_wff_eq(left_side);
}

template <typename... BAs>
tau<BAs...> build_bf_less(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_wff_and<BAs...>(
		build_wff_eq<BAs...>(
			build_bf_and<BAs...>(l, build_bf_neg<BAs...>(r))),
		build_wff_neq<BAs...>(build_bf_xor<BAs...>(l, r)));
}

template <typename... BAs>
tau<BAs...> build_bf_nless(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_wff_neg<BAs...>(build_bf_less<BAs...>(l, r));
}

template <typename... BAs>
tau<BAs...> build_bf_less_equal(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_wff_eq<BAs...>(build_bf_and<BAs...>(l, build_bf_neg<BAs...>(r)));
}

template <typename... BAs>
tau<BAs...> build_bf_nleq(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_wff_neq<BAs...>(build_bf_and<BAs...>(l, build_bf_neg<BAs...>(r)));
}

template <typename... BAs>
tau<BAs...> build_bf_interval(const tau<BAs...>& x,
	const tau<BAs...>& y, const tau<BAs...>& z)
{
	return build_wff_and<BAs...>(build_bf_less_equal<BAs...>(x, y),
		build_bf_less_equal<BAs...>(y, z));
}

template <typename... BAs>
tau<BAs...> build_bf_nleq_lower(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	std::vector<tau<BAs...>> args {trim(l), trim(r)};
	return tau_apply_builder<BAs...>(bldr_bf_nleq_lowwer<BAs...>, args);
}

template <typename... BAs>
tau<BAs...> build_bf_nleq_upper(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	std::vector<tau<BAs...>> args {trim(l), trim(r)};
	return tau_apply_builder<BAs...>(bldr_bf_nleq_upper<BAs...>, args);
}

template <typename... BAs>
tau<BAs...> build_bf_greater(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_bf_less<BAs...>(r, l);
}

template <typename... BAs>
tau<BAs...> build_bf_ngreater(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_wff_neg<BAs...>(build_bf_greater<BAs...>(l, r));
}

template <typename... BAs>
tau<BAs...> build_bf_greater_equal(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_bf_less_equal<BAs...>(r, l);
}

template <typename... BAs>
tau<BAs...> build_bf_ngeq(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_wff_neg<BAs...>(build_bf_greater_equal<BAs...>(l, r));
}

template <typename... BAs>
tau<BAs...> build_wff_ctn_greater_equal(const tau<BAs...>& ctnvar,
	const tau<BAs...>& num) {
	return wrap(tau_parser::wff,
			wrap(tau_parser::constraint,
				wrap(tau_parser::ctn_greater_equal, {ctnvar, num})));
}

template <typename... BAs>
tau<BAs...> build_wff_ctn_greater(const tau<BAs...>& ctnvar,
	const tau<BAs...>& num) {
	return wrap(tau_parser::wff,
			wrap(tau_parser::constraint,
				wrap(tau_parser::ctn_greater, {ctnvar, num})));
}

template <typename... BAs>
tau<BAs...> build_wff_ctn_less_equal(const tau<BAs...>& ctnvar,
	const tau<BAs...>& num) {
	return wrap(tau_parser::wff,
			wrap(tau_parser::constraint,
				wrap(tau_parser::ctn_less_equal, {ctnvar, num})));
}

template <typename... BAs>
tau<BAs...> build_wff_ctn_less(const tau<BAs...>& ctnvar,
	const tau<BAs...>& num) {
	return wrap(tau_parser::wff,
			wrap(tau_parser::constraint,
				wrap(tau_parser::ctn_less, {ctnvar, num})));
}

template <typename... BAs>
tau<BAs...> build_wff_ctn_eq(const tau<BAs...>& ctnvar,
	const tau<BAs...>& num) {
	return wrap(tau_parser::wff,
			wrap(tau_parser::constraint,
				wrap(tau_parser::ctn_eq, {ctnvar, num})));
}

template <typename... BAs>
tau<BAs...> build_wff_ctn_neq(const tau<BAs...>& ctnvar,
	const tau<BAs...>& num) {
	return wrap(tau_parser::wff,
			wrap(tau_parser::constraint,
				wrap(tau_parser::ctn_neq, {ctnvar, num})));
}

} // namespace idni::tau_lang


#endif // __BUILDERS_H__