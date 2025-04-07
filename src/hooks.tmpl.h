// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include <optional>

#include "nso_rr.h"
#include "builders.h"
#include "z3_context.h"

namespace idni::tau_lang {

template <typename...BAs>
tau<BAs...> first_argument_formula(const rewriter::node<tau_sym<BAs...>>& n) {
	return n.child[0]->child[0];
}

template <typename...BAs>
tau<BAs...> first_argument_expression(const rewriter::node<tau_sym<BAs...>>& n){
	return n.child[0]->child[0]->child[0];
}

template <typename...BAs>
tau<BAs...> second_argument_formula(const rewriter::node<tau_sym<BAs...>>& n) {
	return n.child[0]->child[1];
}

template <typename...BAs>
tau<BAs...> second_argument_expression(const rewriter::node<tau_sym<BAs...>>& n)
{
	return n.child[0]->child[1]->child[0];
}

template <typename...BAs>
tau<BAs...> third_argument_formula(const rewriter::node<tau_sym<BAs...>>& n) {
	return n.child[0]->child[2];
}

template <typename...BAs>
tau<BAs...> third_argument_expression(const rewriter::node<tau_sym<BAs...>>& n){
	return n.child[0]->child[2]->child[0];
}

template <typename...BAs>
tau<BAs...> logic_operator(const rewriter::node<tau_sym<BAs...>>& n) {
	return n.child[0];
}

template <typename...BAs>
tau<BAs...> quantifier(const rewriter::node<tau_sym<BAs...>>& n) {
	return n.child[0];
}

template <typename...BAs>
tau<BAs...> quantified_formula(const rewriter::node<tau_sym<BAs...>>& n) {
	return n.child[0]->child[0];
}

template<typename...BAs>
std::optional<tau<BAs...>> type_of(const tau<BAs...>& e)
{
	return e | tau_parser::type;
}

template<typename...BAs>
bool unbinded_subexpressionns(const rewriter::node<tau_sym<BAs...>>& n) {
	auto l = first_argument_expression(n)
		| tau_parser::constant
		| tau_parser::binding;
	auto r = second_argument_expression(n)
		| tau_parser::constant
		| tau_parser::binding;
	return l.has_value() || r.has_value();
}

template<typename...BAs>
tau<BAs...> make_node_hook_cte_or(
	const rewriter::node<tau_sym<BAs...>>& n)
{
	auto l = first_argument_expression(n)
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	auto r = second_argument_expression(n)
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	auto type_l = type_of(first_argument_expression(n));
	auto type_r = type_of(second_argument_expression(n));
	auto type = type_l ? type_l : type_r;
	return l && r ? build_bf_constant<BAs...>(l.value() | r.value(), type)
		: std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template<typename...BAs>
tau<BAs...> build_bf_1(const rewriter::node<tau_sym<BAs...>>& n) {
	auto type_l = type_of(first_argument_expression(n));
	auto type_r = type_of(second_argument_expression(n));
	if (type_l == type_r && type_l) return build_bf_t_type<BAs...>(type_l.value());
	if (type_l == type_r) return _1<BAs...>;
	if (type_l && !type_r) return build_bf_t_type<BAs...>(type_l.value());
	if (!type_l && type_r) return build_bf_t_type<BAs...>(type_r.value());
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template<typename...BAs>
tau<BAs...> build_bf_0(const rewriter::node<tau_sym<BAs...>>& n) {
	auto type_l = type_of(first_argument_expression(n));
	auto type_r = type_of(second_argument_expression(n));
	if (type_l == type_r && type_l) return build_bf_f_type<BAs...>(type_l.value());
	if (type_l == type_r) return _0<BAs...>;
	if (type_l && !type_r) return build_bf_f_type<BAs...>(type_l.value());
	if (!type_l && type_r) return build_bf_f_type<BAs...>(type_r.value());
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename...BAs>
tau<BAs...> make_node_hook_bf_or(const rewriter::node<tau_sym<BAs...>>& n) {
	// RULE(UNBINDED, UNBINDED_SUBEXPRESSIONS, NODE)
	if (unbinded_subexpressionns(n)) return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
	//RULE(BF_SIMPLIFY_ONE_00, "1 | 1 := 1.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_bf_1<BAs...>(n);
	//RULE(BF_SIMPLIFY_ONE_01, "0 |  0:= 0.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_bf_0<BAs...>(n);
	//RULE(BF_SIMPLIFY_ONE_02, "1 | 0 := 1.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_bf_1<BAs...>(n);
	//RULE(BF_SIMPLIFY_ONE_03, "0 | 1 := 1.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_bf_1<BAs...>(n);
	//RULE(BF_SIMPLIFY_ONE_0, "1 | $X := 1.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n)))
		return first_argument_formula(n);
	//RULE(BF_SIMPLIFY_ONE_1, "$X | 1 := 1.")
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(BF_SIMPLIFY_ZERO_2, "0 | $X := $X.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(BF_SIMPLIFY_ZERO_3, "$X | 0 := $X.")
	if (is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return first_argument_formula(n);
	//RULE(BF_SIMPLIFY_SELF_1, "$X | $X := $X.")
	if (first_argument_formula(n) == second_argument_formula(n))
		return first_argument_formula(n);
	//RULE(BF_CALLBACK_OR, "{ $X } | { $Y } := bf_or_cb $X $Y.")
	if (is_non_terminal<tau_parser::bf_constant>(first_argument_expression(n))
		&& is_non_terminal<tau_parser::bf_constant>(second_argument_expression(n)))
		return make_node_hook_cte_or(n);
	//RULE(BF_SIMPLIFY_SELF_3, "$X | $X' := 1.")
	if (auto negated = second_argument_formula(n) | tau_parser::bf_neg |tau_parser::bf;
			negated && negated.value() == first_argument_formula(n))
		return _1<BAs...>;
	//RULE(BF_SIMPLIFY_SELF_5, "$X' | $X := 1.")
	if (auto negated = first_argument_formula(n) | tau_parser::bf_neg | tau_parser::bf;
			negated && negated.value() == second_argument_formula(n))
		return _1<BAs...>;
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename...BAs>
tau<BAs...> make_node_hook_cte_and(const rewriter::node<tau_sym<BAs...>>& n) {
	auto l = first_argument_expression(n)
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	auto r = second_argument_expression(n)
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	auto type_l = type_of(first_argument_expression(n));
	auto type_r = type_of(second_argument_expression(n));
	auto type = type_l ? type_l : type_r;
	return l && r ? build_bf_constant<BAs...>(l.value() & r.value(), type)
		: std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename... BAs>
tau<BAs...> make_node_hook_bf_and(const rewriter::node<tau_sym<BAs...>>& n) {
	// RULE(UNBINDED, UNBINDED_SUBEXPRESSIONS, NODE)
	if (unbinded_subexpressionns(n)) return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
	//RULE(BF_SIMPLIFY_ONE_00, "1 & 1 := 1.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_bf_1<BAs...>(n);
	//RULE(BF_SIMPLIFY_ONE_01, "0 & 0 := 0.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_bf_0<BAs...>(n);
	//RULE(BF_SIMPLIFY_ONE_02, "1 & 0 := 0.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_bf_0<BAs...>(n);
	//RULE(BF_SIMPLIFY_ONE_03, "0 & 1 := 0.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_bf_0<BAs...>(n);
	//RULE(BF_SIMPLIFY_ONE_2, "1 & $X := $X.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(BF_SIMPLIFY_ONE_3, "$X & 1 := $X.")
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return first_argument_formula(n);
	//RULE(BF_SIMPLIFY_ZERO_0, "0 & $X := 0.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n)))
		return first_argument_formula(n);
	//RULE(BF_SIMPLIFY_ZERO_1, "$X & 0 := 0.")
	if (is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(BF_SIMPLIFY_SELF_0, "$X & $X := $X.")
	if (first_argument_formula(n) == second_argument_formula(n))
		return first_argument_formula(n);
	//RULE(BF_CALLBACK_AND, "{ $X } & { $Y } := bf_and_cb $X $Y.")
	if (is_non_terminal<tau_parser::bf_constant>(first_argument_expression(n))
		&& is_non_terminal<tau_parser::bf_constant>(second_argument_expression(n)))
		return make_node_hook_cte_and(n);
	//RULE(BF_SIMPLIFY_SELF_2, "$X & $X' := 0.")
	if (auto negated = second_argument_formula(n) | tau_parser::bf_neg | tau_parser::bf;
			negated && negated.value() == first_argument_formula(n))
		return _0<BAs...>;
	//RULE(BF_SIMPLIFY_SELF_4, "$X' & $X := 0.")
	if (auto negated = first_argument_formula(n) | tau_parser::bf_neg | tau_parser::bf;
			negated && negated.value() == second_argument_formula(n))
		return _0<BAs...>;
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename...BAs>
tau<BAs...> make_node_hook_cte_neg(const rewriter::node<tau_sym<BAs...>>& n) {
	auto l = first_argument_expression(n)
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	auto type = type_of(first_argument_expression(n));
	return l ? build_bf_constant<BAs...>(~l.value(), type)
		: std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename... BAs>
tau<BAs...> make_node_hook_bf_neg(
	const rewriter::node<tau_sym<BAs...>>& n)
{
	//RULE(BF_SIMPLIFY_ONE_4, "1' := 0.")
	if (auto neg_one = logic_operator(n) | tau_parser::bf | tau_parser::bf_t;
			neg_one && is_non_terminal<tau_parser::bf_neg>(logic_operator(n))) {
		auto type = type_of(neg_one.value());
		return type ? build_bf_f_type(type.value()) : _0<BAs...>;
	}
	//RULE(BF_SIMPLIFY_ZERO_4, "0' := 1.")
	if (auto neg_zero = logic_operator(n) | tau_parser::bf | tau_parser::bf_f;
			neg_zero && is_non_terminal<tau_parser::bf_neg>(logic_operator(n))) {
		auto type = type_of(neg_zero.value());
		return type ? build_bf_t_type(type.value()) : _1<BAs...>;
		}
	//RULE(BF_ELIM_DOUBLE_NEGATION_0, "$X'' :=  $X.")
	if (auto double_neg = logic_operator(n) | tau_parser::bf | tau_parser::bf_neg; double_neg
			&& is_non_terminal<tau_parser::bf_neg>(logic_operator(n)))
		return double_neg.value()->child[0];
	//RULE(BF_CALLBACK_NEG, "{ $X }' := bf_neg_cb $X.")
	if (is_non_terminal<tau_parser::bf_constant>(first_argument_expression(n)))
		return make_node_hook_cte_neg(n);
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename...BAs>
tau<BAs...> make_node_hook_cte_xor(
	const rewriter::node<tau_sym<BAs...>>& n)
{
	auto l = first_argument_expression(n)
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	auto r = second_argument_expression(n)
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	auto type_l = type_of(first_argument_expression(n));
	auto type_r = type_of(second_argument_expression(n));
	auto type = type_l ? type_l : type_r;
	return l && r ? build_bf_constant<BAs...>(l.value() ^ r.value(), type)
		: std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);

}

template <typename... BAs>
tau<BAs...> make_node_hook_bf_xor(const rewriter::node<tau_sym<BAs...>>& n) {
	// RULE(UNBINDED, UNBINDED_SUBEXPRESSIONS, NODE)
	if (unbinded_subexpressionns(n)) return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
	//RULE(BF_SIMPLIFY_ONE_00, "1 ^ 1 := 0.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_bf_0<BAs...>(n);
	//RULE(BF_SIMPLIFY_ONE_01, "0 ^ 0 := 0.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_bf_0<BAs...>(n);
	//RULE(BF_SIMPLIFY_ONE_02, "1 ^ 0 := 1.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_bf_1<BAs...>(n);
	//RULE(BF_SIMPLIFY_ONE_03, "0 ^ 1 := 1.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_bf_1<BAs...>(n);
	//RULE(BF_SIMPLIFY_ONE_N, "1 ^ $X := $X'.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n)))
		return build_bf_neg(second_argument_formula(n));
	//RULE(BF_SIMPLIFY_ONE_N, "$X ^ 1 := $X'.")
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_bf_neg(first_argument_formula(n));
	//RULE(BF_SIMPLIFY_ZERO_N, "0 ^ $X := $X.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(BF_SIMPLIFY_ZERO_N, "$X ^ 0 := $X.")
	if (is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return first_argument_formula(n);
	//RULE(BF_SIMPLIFY_SELF_N, "$X ^ $X := 0.")
	if (first_argument_formula(n) == second_argument_formula(n))
		return _0<BAs...>;
	//RULE(BF_SIMPLIFY_SELF_2, "$X ^ $X' := 1.")
	if (auto negated = second_argument_formula(n) | tau_parser::bf_neg | tau_parser::bf;
			negated && negated.value() == first_argument_formula(n))
		return _1<BAs...>;
	//RULE(BF_SIMPLIFY_SELF_2, "$X' ^ $X := 1.")
	if (auto negated = first_argument_formula(n) | tau_parser::bf_neg | tau_parser::bf;
			negated && negated.value() == second_argument_formula(n))
		return _1<BAs...>;
	//RULE(BF_CALLBACK_XOR, "{ $X } ^ { $Y } := bf_xor_cb $X $Y.")
	if (is_non_terminal<tau_parser::bf_constant>(first_argument_expression(n))
		&& is_non_terminal<tau_parser::bf_constant>(second_argument_expression(n)))
		return make_node_hook_cte_xor(n);
	return  build_bf_xor<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

// Simplify constants being syntactically true or false
template <typename...BAs>
tau<BAs...> make_node_hook_cte(const rewriter::node<tau_sym<BAs...>>& n)
{
	auto l = n
		| tau_parser::bf_constant
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	if (l.has_value()) {
		auto typed = n | tau_parser::bf_constant | tau_parser::type;
		if (is_syntactic_zero(l.value()))
			return typed.has_value() ? build_bf_f_type(typed.value()) : _0<BAs...>;
		else if (is_syntactic_one(l.value()))
			return typed.has_value() ? build_bf_t_type(typed.value()) : _1<BAs...>;
	}
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_ctn(const rewriter::node<tau_sym<BAs...>>& n) {
	auto sp_n = make_shared<rewriter::node<tau_sym<BAs...>>>(n);
	auto num = find_top(sp_n, is_non_terminal<tau_parser::num, BAs...>).value();
	auto ctnvar = find_top(sp_n, is_non_terminal<tau_parser::ctnvar, BAs...>).value();
	auto op = non_terminal_extractor<BAs...>(trim2(sp_n)).value();
	switch (op) {
	case tau_parser::ctn_eq:
		return build_wff_and(build_wff_ctn_less_equal(ctnvar, num),
				build_wff_ctn_greater_equal(ctnvar, num));
	case tau_parser::ctn_neq:
		return build_wff_or(build_wff_ctn_less(ctnvar, num),
	 			build_wff_ctn_greater(ctnvar, num));
	}
	return sp_n;
}

template <typename...BAs>
tau<BAs...> make_node_hook_ctn_neg(const tau<BAs...>& n) {
	auto num = find_top(n, is_non_terminal<tau_parser::num, BAs...>).value();
	auto ctnvar = find_top(n, is_non_terminal<tau_parser::ctnvar, BAs...>).value();
	auto op = non_terminal_extractor<BAs...>(trim(n)).value();
	switch (op) {
		//RULE(BF_PUSH_NEGATION_INWARDS_2, "($X != $Y)' := $X = $Y.")
	case tau_parser::ctn_neq:
		return build_wff_ctn_eq<BAs...>(ctnvar, num);
		//RULE(BF_PUSH_NEGATION_INWARDS_3, "($X = $Y)' := $X != $Y.")
	case tau_parser::ctn_eq:
		return build_wff_ctn_neq<BAs...>(ctnvar, num);
		//RULE(BF_PUSH_NEGATION_INWARDS_4, "($X >= $Y)' := $X < $Y.")
	case tau_parser::ctn_greater_equal:
		return build_wff_ctn_less<BAs...>(ctnvar, num);
		//RULE(BF_PUSH_NEGATION_INWARDS_5, "($X > $Y)' := $X <= $Y.")
	case tau_parser::ctn_greater:
		return build_wff_ctn_less_equal<BAs...>(ctnvar, num);
		//RULE(BF_PUSH_NEGATION_INWARDS_6, "($X <= $Y)' := $X > $Y.")
	case tau_parser::ctn_less_equal:
		return build_wff_ctn_greater<BAs...>(ctnvar, num);
		//RULE(BF_PUSH_NEGATION_INWARDS_7, "($X < $Y)' := $X >= $Y.")
	case tau_parser::ctn_less:
		return build_wff_ctn_greater_equal<BAs...>(ctnvar, num);
	}
	// Can never happen - all cases exhausted
	assert(false);
	return n;
}

template <typename... BAs>
tau<BAs...> make_node_hook_bf(const rewriter::node<tau_sym<BAs...>>& n) {
	// if n is ref, capture, 0 or 1, we can return accordingly
	//if (n.child.size() != 1)
	//	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
	// otherwise we need to check the children
	if (is_non_terminal_node<BAs...>(n.child[0]))
		switch (std::get<tau_source_sym>(n.child[0]->value).n()) {
			case tau_parser::bf_or:
				return make_node_hook_bf_or<BAs...>(n);
			case tau_parser::bf_and:
				return make_node_hook_bf_and<BAs...>(n);
			case tau_parser::bf_neg:
				return make_node_hook_bf_neg<BAs...>(n);
			case tau_parser::bf_xor:
				return make_node_hook_bf_xor<BAs...>(n);
			case tau_parser::bf_constant:
				return make_node_hook_cte<BAs...>(n);
			default: return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
		}
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_and(const rewriter::node<tau_sym<BAs...>>& n) {
	//RULE(WFF_SIMPLIFY_ONE_2, "T && $X ::= $X.")
	if (is_non_terminal<tau_parser::wff_t>(first_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(WFF_SIMPLIFY_ONE_3, "$X && T ::= $X.")
	if (is_non_terminal<tau_parser::wff_t>(second_argument_expression(n)))
		return first_argument_formula(n);
	//RULE(WFF_SIMPLIFY_ZERO_0, "F && $X ::= F.")
	if (is_non_terminal<tau_parser::wff_f>(first_argument_expression(n)))
		return first_argument_formula(n);
	//RULE(WFF_SIMPLIFY_ZERO_1, "$X && F ::= F.")
	if (is_non_terminal<tau_parser::wff_f>(second_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(WFF_SIMPLIFY_SELF_0, "$X && $X ::= $X.")
	if (first_argument_formula(n) == second_argument_formula(n))
		return first_argument_formula(n);
	//RULE(WFF_SIMPLIFY_SELF_2, "$X && ! $X ::= F.")
	if (auto negated = second_argument_formula(n) | tau_parser::wff_neg | tau_parser::wff;
			negated && negated.value() == first_argument_formula(n))
		return _F<BAs...>;
	//RULE(WFF_SIMPLIFY_SELF_4, "!$X && $X ::= F.")
	if (auto negated = first_argument_formula(n) | tau_parser::wff_neg | tau_parser::wff;
			negated && negated.value() == second_argument_formula(n))
		return _F<BAs...>;
	//RULE(BF_EQ_AND_SIMPLIFY_0, "$X != 0 && $X = 0 ::= F.")
	//RULE(BF_EQ_AND_SIMPLIFY_1, "$X = 0 && $X != 0 ::= F.")
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_or(const rewriter::node<tau_sym<BAs...>>& n) {
	//RULE(WFF_SIMPLIFY_ONE_0, "T || $X ::= T.")
	if (is_non_terminal<tau_parser::wff_t>(first_argument_expression(n)))
		return first_argument_formula(n);
	//RULE(WFF_SIMPLIFY_ONE_1, "$X || T ::= T.")
	if (is_non_terminal<tau_parser::wff_t>(second_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(WFF_SIMPLIFY_ZERO_2, "F || $X ::= $X.")
	if (is_non_terminal<tau_parser::wff_f>(first_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(WFF_SIMPLIFY_ZERO_3, "$X || F ::= $X.")
	if (is_non_terminal<tau_parser::wff_f>(second_argument_expression(n)))
		return first_argument_formula(n);
	//RULE(WFF_SIMPLIFY_SELF_1, "$X || $X ::= $X.")
	if (first_argument_formula(n) == second_argument_formula(n))
		return first_argument_formula(n);
	//RULE(WFF_SIMPLIFY_SELF_3, "$X || ! $X ::= T.")
	if (auto negated = second_argument_formula(n) | tau_parser::wff_neg | tau_parser::wff;
			negated && negated.value() == first_argument_formula(n))
		return _T<BAs...>;
	//RULE(WFF_SIMPLIFY_SELF_5, "!$X || $X ::= T.")
	if (auto negated = first_argument_formula(n) | tau_parser::wff_neg | tau_parser::wff;
			negated && negated.value() == second_argument_formula(n))
		return _T<BAs...>;
	//RULE(BF_EQ_OR_SIMPLIFY_0, "$X != 0 || $X = 0 ::= T.")
	//RULE(BF_EQ_OR_SIMPLIFY_1, "$X = 0 || $X != 0 ::= T.")
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_neg(const rewriter::node<tau_sym<BAs...>>& n) {
	//RULE(WFF_SIMPLIFY_ONE_4, " ! T ::= F.")
	if (is_non_terminal<tau_parser::wff_t>(first_argument_expression(n)))
		return _F<BAs...>;
	//RULE(WFF_SIMPLIFY_ZERO_4, "! F ::= T.")
	if (is_non_terminal<tau_parser::wff_f>(first_argument_expression(n)))
		return _T<BAs...>;
	//RULE(WFF_ELIM_DOUBLE_NEGATION_0, "! ! $X ::=  $X.")
	if (auto double_neg = first_argument_formula(n) | tau_parser::wff_neg | tau_parser::wff; double_neg)
		return double_neg.value();
	if (is_non_terminal<tau_parser::constraint>(first_argument_expression(n))) {
		return make_node_hook_ctn_neg(first_argument_expression(n));
	}
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename...BAs>
tau<BAs...> make_node_hook_wff_eq_cte(const rewriter::node<tau_sym<BAs...>>& n){
	auto l = n
		| tau_parser::bf_eq
		| tau_parser::bf
		| tau_parser::bf_constant
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	//TODO: remove normalizer call
	if (l.has_value() && l.value() == false) return _T<BAs...>;
	else if (l.has_value()) return _F<BAs...>;
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template<typename... BAs>
tau<BAs...> build_wff_t(const rewriter::node<tau_sym<BAs...>>& n) {
	auto type_l = type_of(first_argument_expression(n));
	auto type_r = type_of(second_argument_expression(n));
	if (type_l == type_r && type_l) return _T<BAs...>;
	if (type_l == type_r) return _T<BAs...>;
	if (type_l && !type_r) return _T<BAs...>;
	if (!type_l && type_r) return _T<BAs...>;
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template<typename... BAs>
tau<BAs...> build_wff_f(const rewriter::node<tau_sym<BAs...>>& n) {
	auto type_l = type_of(first_argument_expression(n));
	auto type_r = type_of(second_argument_expression(n));
	if (type_l == type_r && type_l) return _F<BAs...>;
	if (type_l == type_r) return _F<BAs...>;
	if (type_l && !type_r) return _F<BAs...>;
	if (!type_l && type_r) return _F<BAs...>;
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template<typename... BAs>
tau<BAs...> make_node_hook_wff_eq(const rewriter::node<tau_sym<BAs...>>& n) {
	//RULE(BF_EQ_SIMPLIFY_0, "1 = 0 ::=  F.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_EQ_SIMPLIFY_1, "0 = 0 ::= T.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_DEF_SIMPLIFY_0, "0 = 1")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_DEF_SIMPLIFY_1, "1 = 1")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_DEF_SIMPLIFY_N, "{c} = 0 ::= ...."): this should never happen
	if(is_non_terminal<tau_parser::bf_constant>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return make_node_hook_wff_eq_cte(n);
	//RULE(BF_DEF_EQ, "$X = $Y ::= $X & $Y' | $X' & $Y = 0.")
	if (!is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_eq<BAs...>(
			build_bf_xor(first_argument_formula(n), second_argument_formula(n)));
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename...BAs>
tau<BAs...> make_node_hook_wff_neq_cte(const rewriter::node<tau_sym<BAs...>>& n)
{
	auto l = n
		| tau_parser::bf_neq
		| tau_parser::bf
		| tau_parser::bf_constant
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	//TODO: remove normalizer call
	if (l.has_value() && l.value() == false) return _F<BAs...>;
	else if (l.has_value()) return _T<BAs...>;
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_neq(const rewriter::node<tau_sym<BAs...>>& n) {
	//RULE(BF_NEQ_SIMPLIFY_0, "0 != 0 ::= F.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_NEQ_SIMPLIFY_1, "1 != 0 ::= T.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_NEQ_SIMPLIFY_2, "0 != 1 ::= T.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_NEQ_SIMPLIFY_3, "1 != 1 ::= F.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_DEF_SIMPLIFY_N, "{c} = 0 ::= ....")
	if(is_non_terminal<tau_parser::bf_constant>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return make_node_hook_wff_neq_cte(n);
	//RULE(BF_DEF_NEQ, "$X != $Y ::= $X & $Y' | $X' & $Y != 0.")
	if (!is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_neq<BAs...>(
			build_bf_xor(first_argument_formula(n), second_argument_formula(n)));
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_sometimes(
	const rewriter::node<tau_sym<BAs...>>& n)
{
	//RULE(WFF_SIMPLIFY_ONE_6, " sometimes T ::= T.")
	if (is_non_terminal<tau_parser::wff_t>(first_argument_expression(n)))
		return _T<BAs...>;
	//RULE(WFF_SIMPLIFY_ZERO_6, "sometimes F ::= F.")
	if (is_non_terminal<tau_parser::wff_f>(first_argument_expression(n)))
		return _F<BAs...>;
	//RULE(WFF_SIMPLIFY_SOMETIMES_1,  "sometimes sometimes $X ::= sometimes $X.")
	if (auto double_quantifier = first_argument_formula(n) | tau_parser::wff_sometimes; double_quantifier)
		return first_argument_formula(n);
	//RULE(WFF_SIMPLIFY_SOMETIMES_2,  "sometimes always $X ::= always $X.")
	if (auto double_quantifier = first_argument_formula(n) | tau_parser::wff_always;	double_quantifier)
		return first_argument_formula(n);
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_always(
	const rewriter::node<tau_sym<BAs...>>& n)
{
	//RULE(WFF_SIMPLIFY_ONE_5, " always T ::= T.")
	if (is_non_terminal<tau_parser::wff_t>(first_argument_expression(n)))
		return _T<BAs...>;
	//RULE(WFF_SIMPLIFY_ZERO_5, "always F ::= F.")
	if (is_non_terminal<tau_parser::wff_f>(first_argument_expression(n)))
		return _F<BAs...>;
	//RULE(WFF_SIMPLIFY_ALWAYS_1,     "always always $X ::= always $X.")
	if (auto double_quantifier = first_argument_formula(n) | tau_parser::wff_always;
			double_quantifier && is_non_terminal<tau_parser::wff_always>(quantifier(n)))
		return first_argument_formula(n);
	//RULE(WFF_SIMPLIFY_ALWAYS_2,     "always sometimes $X ::= sometimes $X.")
	if (auto double_quantifier = first_argument_formula(n) | tau_parser::wff_sometimes;
			double_quantifier && is_non_terminal<tau_parser::wff_always>(quantifier(n)))
		return first_argument_formula(n);
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_less(const rewriter::node<tau_sym<BAs...>>& n) {
	//RULE(BF_LESS_SIMPLIFY_20, "0 < 1 ::= T.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_LESS_SIMPLIFY_21, "1 < 0 ::= F.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_LESS_SIMPLIFY_0, "0 < 0 ::= F.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_DEF_SIMPLIFY_N, "1 < 1 ::= F.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_LESS_SIMPLIFY_0, "$X < 0 ::= F.") @CP
	if (is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return _F<BAs...>;
	//RULE(BF_DEF_SIMPLIFY_N, "$X < 1 ::= $X' != 0.")
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_neq<BAs...>(build_bf_neg(first_argument_formula(n)));

	return build_bf_less<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_nless(const rewriter::node<tau_sym<BAs...>>& n) {
	//RULE(BF_NLESS_SIMPLIFY_20, "0 !< 1 ::= F.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_NLESS_SIMPLIFY_21, "1 !< 0 ::= T.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_NLESS_SIMPLIFY_0, "0 !< 0 ::= T.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_DEF_SIMPLIFY_N, "1 !< 1 ::= T.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_NLESS_SIMPLIFY_0, "$X !< 0 ::= T.") @CP
	if (is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return _T<BAs...>;
	//RULE(BF_DEF_SIMPLIFY_N, "$X !< 1 ::= $X' = 0.")
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_eq<BAs...>(build_bf_neg(first_argument_formula(n)));
	return build_bf_nless<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_less_equal(
	const rewriter::node<tau_sym<BAs...>>& n)
{
	//RULE(BF_LESS_EQUAL_SIMPLIFY_2, "0 <= 1 ::= T.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_LESS_EQUAL_SIMPLIFY_0, "0 <= 0 ::= T.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_LESS_EQUAL_SIMPLIFY_1, "1 <= 0 ::= F.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_DEF_SIMPLIFY_N, "1 <= 1 ::= T.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_LESS_EQUAL_SIMPLIFY_0, "$X <= 1 ::= T.") @CP
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return _T<BAs...>;
	return build_bf_less_equal<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_nleq(const rewriter::node<tau_sym<BAs...>>& n) {
	//RULE(BF_NLEQ_SIMPLIFY_2, "0 !<= 1 ::= F.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_NLEQ_SIMPLIFY_0, "0 !<= 0 ::= F.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_NLEQ_SIMPLIFY_1, "1 !<= 0 ::= T.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_NLEQ_SIMPLIFY_0, "1 !<= 1 ::= F.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_NLEQ_SIMPLIFY_0, "$X !<= 1 ::= F.") @CP
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return _F<BAs...>;
	return build_bf_nleq<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_greater(const rewriter::node<tau_sym<BAs...>>& n)
{
	//RULE(BF_GREATER_SIMPLIFY_2, "1 > 0 ::= T.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_GREATER_SIMPLIFY_0, "0 > 0 ::= F.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_GREATER_SIMPLIFY_1, "0 > 1 ::= F.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_GREATER_SIMPLIFY_3, "1 > 1 ::= F.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_GREATER_SIMPLIFY_0, "$X > 1 ::= F.") @CP
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return _F<BAs...>;
	//RULE(BF_GREATER_SIMPLIFY_3, "0 > $X ::= F.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n)))
		return _F<BAs...>;
	return build_bf_greater<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_ngreater(
	const rewriter::node<tau_sym<BAs...>>& n)
{
	//RULE(BF_NGREATER_SIMPLIFY_2, "1 !> 0 ::= F.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_NGREATER_SIMPLIFY_1, "0 !> 0 ::= T.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_NGREATER_SIMPLIFY_0, "0 !> 1 ::= T.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_NGREATER_SIMPLIFY_3, "1 !> 1 ::= T.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_NGREATER_SIMPLIFY_0, "$X !> 1 ::= T.") @CP
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return _T<BAs...>;
	//RULE(BF_NGREATER_SIMPLIFY_3, "0 !> $X ::= T.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n)))
		return _T<BAs...>;
	return build_bf_ngreater<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template <typename... BAs>
inline tau<BAs...> make_node_hook_wff_greater_equal(
	const rewriter::node<tau_sym<BAs...>>& n)
{
	//RULE(BF_GREATER_EQUAL_SIMPLIFY_2, "1 >= 0 ::= T.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_GREATER_EQUAL_SIMPLIFY_1, "0 >= 0 ::= T.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_GREATER_EQUAL_SIMPLIFY_0, "0 >= 1 ::= F.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_GREATER_EQUAL_SIMPLIFY_3, "1 >= 1 ::= T.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_GREATER_EQUAL_SIMPLIFY_0, "$X >= 0 ::= T.") @CP
	if (is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return _T<BAs...>;
	//RULE(BF_GREATER_EQUAL_SIMPLIFY_2, "1 >= X ::= T.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n)))
		return _T<BAs...>;
	return build_bf_greater_equal<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_ngeq(const rewriter::node<tau_sym<BAs...>>& n) {
	//RULE(BF_NGEQ_SIMPLIFY_2, "1 !>= 0 ::= F.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_NGEQ_SIMPLIFY_1, "0 !>= 0 ::= F.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_NGEQ_SIMPLIFY_0, "0 !>= 1 ::= T.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_NGEQ_SIMPLIFY_3, "1 !>= 1 ::= F.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_NGEQ_SIMPLIFY_0, "$X !>= 0 ::= F.") @CP
	if (is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return _F<BAs...>;
	//RULE(BF_NGEQ_SIMPLIFY_1, "1 !>= $X ::= F.") @CP
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return _F<BAs...>;
	return build_bf_ngeq<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_interval(
	const rewriter::node<tau_sym<BAs...>>& n)
{
	return build_bf_interval<BAs...>(first_argument_formula(n), second_argument_formula(n), third_argument_formula(n));
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_xor(const rewriter::node<tau_sym<BAs...>>& n) {
	//RULE(BF_XOR_SIMPLIFY_0, "$X ^ F ::= $X.")
	if (is_non_terminal<tau_parser::wff_f>(second_argument_expression(n)))
		return first_argument_formula(n);
	//RULE(BF_XOR_SIMPLIFY_1, "F ^ $X ::= $X.")
	if (is_non_terminal<tau_parser::wff_f>(first_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(BF_XOR_SIMPLIFY_2, "$X ^ $X ::= F.")
	if (first_argument_formula(n) == second_argument_formula(n))
		return _F<BAs...>;
	//RULE(BF_XOR_SIMPLIFY_3, "$X ^ ! $X ::= T.")
	if (auto negated = second_argument_formula(n) | tau_parser::wff_neg | tau_parser::wff;
			negated && negated.value() == first_argument_formula(n))
		return _T<BAs...>;
	//RULE(BF_XOR_SIMPLIFY_4, "! $X ^ $X ::= T.")
	if (auto negated = first_argument_formula(n) | tau_parser::wff_neg | tau_parser::wff;
			negated && negated.value() == second_argument_formula(n))
		return _T<BAs...>;
	//RULE(BF_XOR_SIMPLIFY_5, "$X ^ T ::= ! $X.")
	if (is_non_terminal<tau_parser::wff_t>(second_argument_expression(n)))
		return build_wff_neg<BAs...>(first_argument_formula(n));
	//RULE(BF_XOR_SIMPLIFY_6, "T ^ $X ::= ! $X.")
	if (is_non_terminal<tau_parser::wff_t>(first_argument_expression(n)))
		return build_wff_neg<BAs...>(second_argument_formula(n));
	return build_wff_xor<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_conditional(
	const rewriter::node<tau_sym<BAs...>>& n)
{
	//RULE(WFF_CONDITIONAL_SIMPLIFY_0, "F ? $X : $Y ::= $Y.")
	if (is_non_terminal<tau_parser::wff_f>(first_argument_expression(n)))
		return third_argument_formula(n);
	//RULE(WFF_CONDITIONAL_SIMPLIFY_1, "T ? $X : $Y ::= $X.")
	if (is_non_terminal<tau_parser::wff_t>(first_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(WFF_CONDITIONAL_SIMPLIFY_4, "$X ? $Y : $Y ::= $Y.")
	if (second_argument_formula(n) == third_argument_formula(n))
		return third_argument_formula(n);
	return build_wff_conditional<BAs...>(first_argument_formula(n), second_argument_formula(n), third_argument_formula(n));
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_imply(const rewriter::node<tau_sym<BAs...>>& n) {
	//RULE(WFF_IMPLY_SIMPLIFY_0, "F -> $X ::= T.")
	if (is_non_terminal<tau_parser::wff_f>(first_argument_expression(n)))
		return _T<BAs...>;
	//RULE(WFF_IMPLY_SIMPLIFY_1, "T -> $X ::= $X.")
	if (is_non_terminal<tau_parser::wff_t>(first_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(WFF_IMPLY_SIMPLIFY_2, "$X -> F ::= ! $X.")
	if (is_non_terminal<tau_parser::wff_f>(second_argument_expression(n)))
		return build_wff_neg<BAs...>(first_argument_formula(n));
	//RULE(WFF_IMPLY_SIMPLIFY_3, "$X -> T ::= T.")
	if (is_non_terminal<tau_parser::wff_t>(second_argument_expression(n)))
		return _T<BAs...>;
	//RULE(WFF_IMPLY_SIMPLIFY_4, "$X -> $X ::= T.")
	if (first_argument_formula(n) == second_argument_formula(n))
		return _T<BAs...>;
	return build_wff_imply<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_rimply(const rewriter::node<tau_sym<BAs...>>& n) {
	//RULE(WFF_IMPLY_SIMPLIFY_0, "F -> $X ::= T.")
	if (is_non_terminal<tau_parser::wff_f>(second_argument_expression(n)))
		return _T<BAs...>;
	//RULE(WFF_IMPLY_SIMPLIFY_1, "T -> $X ::= $X.")
	if (is_non_terminal<tau_parser::wff_t>(second_argument_expression(n)))
		return first_argument_formula(n);
	//RULE(WFF_IMPLY_SIMPLIFY_2, "$X -> F ::= ! $X.")
	if (is_non_terminal<tau_parser::wff_f>(first_argument_expression(n)))
		return build_wff_neg<BAs...>(second_argument_formula(n));
	//RULE(WFF_IMPLY_SIMPLIFY_3, "$X -> T ::= T.")
	if (is_non_terminal<tau_parser::wff_t>(first_argument_expression(n)))
		return _T<BAs...>;
	//RULE(WFF_IMPLY_SIMPLIFY_4, "$X -> $X ::= T.")
	if (first_argument_formula(n) == second_argument_formula(n))
		return _T<BAs...>;
	return build_wff_imply<BAs...>(second_argument_formula(n), first_argument_formula(n));
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_equiv(const rewriter::node<tau_sym<BAs...>>& n) {
	//RULE(WFF_EQUIV_SIMPLIFY_0, "F <-> $X ::= ! $X.")
	if (is_non_terminal<tau_parser::wff_f>(first_argument_expression(n)))
		return build_wff_neg<BAs...>(second_argument_formula(n));
	//RULE(WFF_EQUIV_SIMPLIFY_1, "T <-> $X ::= $X.")
	if (is_non_terminal<tau_parser::wff_t>(first_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(WFF_EQUIV_SIMPLIFY_2, "$X <-> F ::= ! $X.")
	if (is_non_terminal<tau_parser::wff_f>(second_argument_expression(n)))
		return build_wff_neg<BAs...>(first_argument_formula(n));
	//RULE(WFF_EQUIV_SIMPLIFY_3, "$X <-> T ::= $X.")
	if (is_non_terminal<tau_parser::wff_t>(second_argument_expression(n)))
		return first_argument_formula(n);
	//RULE(WFF_EQUIV_SIMPLIFY_4, "$X <-> $X ::= T.")
	if (first_argument_formula(n) == second_argument_formula(n))
		return _T<BAs...>;
	//RULE(WFF_EQUIV_SIMPLIFY_5, "$X <-> ! $X ::= F.")
	if (auto negated = second_argument_formula(n) | tau_parser::wff_neg | tau_parser::wff;
			negated && negated.value() == first_argument_formula(n))
		return _F<BAs...>;
	//RULE(WFF_EQUIV_SIMPLIFY_6, "! $X <-> $X ::= F.")
	if (auto negated = first_argument_formula(n) | tau_parser::wff_neg | tau_parser::wff;
			negated && negated.value() == second_argument_formula(n))
		return _F<BAs...>;
	return build_wff_equiv<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff(const rewriter::node<tau_sym<BAs...>>& n) {
	switch (get_non_terminal_node(logic_operator(n))) {
		case tau_parser::wff_and:
			return make_node_hook_wff_and<BAs...>(n);
		case tau_parser::wff_or:
			return make_node_hook_wff_or<BAs...>(n);
		case tau_parser::wff_neg:
			return make_node_hook_wff_neg<BAs...>(n);
		case tau_parser::bf_eq:
			return make_node_hook_wff_eq<BAs...>(n);
		case tau_parser::bf_neq:
			return make_node_hook_wff_neq<BAs...>(n);
		case tau_parser::wff_sometimes:
			return make_node_hook_wff_sometimes<BAs...>(n);
		case tau_parser::wff_always:
			return make_node_hook_wff_always<BAs...>(n);
		case tau_parser::wff_xor:
			return make_node_hook_wff_xor<BAs...>(n);
		case tau_parser::wff_conditional:
			return make_node_hook_wff_conditional<BAs...>(n);
		case tau_parser::wff_imply:
			return make_node_hook_wff_imply<BAs...>(n);
		case tau_parser::wff_rimply:
			return make_node_hook_wff_rimply<BAs...>(n);
		case tau_parser::wff_equiv:
			return make_node_hook_wff_equiv<BAs...>(n);
		case tau_parser::constraint:
			return make_node_hook_wff_ctn<BAs...>(n);
		case tau_parser::bf_less:
			return make_node_hook_wff_less<BAs...>(n);
		case tau_parser::bf_nless:
			return make_node_hook_wff_nless<BAs...>(n);
		case tau_parser::bf_less_equal:
			return make_node_hook_wff_less_equal<BAs...>(n);
		case tau_parser::bf_nleq:
			return make_node_hook_wff_nleq<BAs...>(n);
		case tau_parser::bf_greater:
			return make_node_hook_wff_greater<BAs...>(n);
		case tau_parser::bf_ngreater:
			return make_node_hook_wff_ngreater<BAs...>(n);
		case tau_parser::bf_greater_equal:
			return make_node_hook_wff_greater_equal<BAs...>(n);
		case tau_parser::bf_ngeq:
			return make_node_hook_wff_ngeq<BAs...>(n);
		case tau_parser::bf_interval:
			return make_node_hook_wff_interval<BAs...>(n);
		default: return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
	}
}

template <typename... BAs>
tau<BAs...> make_node_hook_shift(const rewriter::node<tau_sym<BAs...>>& n) {
	// apply numerical simplifications
	using p = tau_parser;
	// This node must have two children
	// The first node is either p::variable, p::capture, p::num or p::integer
	// The second node must be p::num
	if (n.child.size() == 2) {
		int_t left = -1;
		const auto& c0 = n.child[0];
		if (is_non_terminal(p::integer, c0))
			left = int_extractor<BAs...>(c0);
		else if (is_non_terminal(p::num, c0))
			left = (int_t)(c0
				| only_child_extractor<BAs...>
				| size_t_extractor<BAs...>
				| optional_value_extractor<size_t>);
		if (left < 0) {
			assert(is_non_terminal(p::variable, c0) || is_non_terminal(p::capture, c0));
			return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
		}
		int_t right = -1;
		if (is_non_terminal(p::num, n.child[1]))
			right = (int_t) ( n.child[1]
				| only_child_extractor<BAs...>
				| size_t_extractor<BAs...>
				| optional_value_extractor<size_t>);
		if (right < 0) {
			// This is not allowed to happen
			assert(false);
			return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
		}
		if (left >= right) return build_int<BAs...>(left-right);
		// Return error
		return nullptr;
	} else { assert(false); }
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename... BAs>
tau<BAs...> make_node_hook_bitvector(const rewriter::node<tau_sym<BAs...>>& n) {
	if (is_z3_node(n.child[0])) return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
	// apply numerical simplifications
	using p = tau_parser;
	// get bitvector size
	size_t size = n.child.size() == 2
		? n.child[1]
			| only_child_extractor<BAs...>
			| size_t_extractor<BAs...>
			| optional_value_extractor<size_t>
		: sizeof(size_t) * 8;
	auto value = make_string(tau_node_terminal_extractor<BAs...>, n.child[0]);
	tau<BAs...> bvn;
	switch (get_non_terminal_node(n.child[0])) {
		case p::num : {
			int64_t ull = std::stoull(value);
			auto bv = z3_context.bv_val(ull, size);
			bvn = make_node<tau_sym<BAs...>>(bv, {});
			break;
		}
		case p::bits : {
			value.pop_back();
			auto bv = z3_context.bv_val(value.c_str(), size);
			bvn = make_node<tau_sym<BAs...>>(bv, {});
			break;
		}
		default: {
			assert(false);
		}
	}
	std::vector<tau<BAs...>> v{bvn};
	if (n.child.size() == 2) v.emplace_back(n.child[1]);
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(
		tau_parser::instance().literal(p::bitvector), v);
}

template <typename...BAs>
std::optional<tau<BAs...>> make_tau_node<BAs...>::operator()(
	const rewriter::node<tau_sym<BAs...>>& n)
{
	if (is_non_terminal_node<BAs...>(n)) {
		switch (get_non_terminal_node(n)) {
			case tau_parser::bf: {
				return make_node_hook_bf<BAs...>(n);
			}
			case tau_parser::wff: {
				return make_node_hook_wff<BAs...>(n);
			}
			case tau_parser::shift: {
				return make_node_hook_shift<BAs...>(n);
			}
			case tau_parser::bitvector : {
				return make_node_hook_bitvector<BAs...>(n);
			}
			default: return std::optional<tau<BAs...>>();
		}
	}
	return std::optional<tau<BAs...>>();
}

} // namespace idni::tau_lang

namespace idni::rewriter {

template <typename...BAs>
std::optional<idni::tau_lang::tau<BAs...>> make_node_hook<idni::tau_lang::tau_sym<BAs...>>::operator()(
		const rewriter::node<idni::tau_lang::tau_sym<BAs...>>& n) {
	static idni::tau_lang::make_tau_node<BAs...> hook;
	return hook(n);
}

} // namespace idni::rewriter
