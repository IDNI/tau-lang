// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentioned cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

#ifndef __NORMALIZER2_H__
#define __NORMALIZER2_H__

#include <string>
#include <optional>

#include "rewriting.h"
#include "nso_rr.h"


// TODO (MEDIUM) fix proper types (alias) at this level of abstraction
//
// We should talk about statement, nso_rr (nso_with_rr?), library, rule, builder,
// bindings, etc... instead of sp_tau_node,...


namespace idni::tau {

// tau system library, used to define the tau system of rewriting rules
#define RULE(name, code) const std::string name = code;

// IDEA (MEDIUM) add commutative rule and halve the number of rules if is performance friendly

// bf rules
RULE(BF_DISTRIBUTE_0, "(($X | $Y) & $Z) := (($X & $Z) | ($Y & $Z)).")
RULE(BF_DISTRIBUTE_1, "($X & ($Y | $Z)) := (($X & $Y) | ($X & $Z)).")
RULE(BF_PUSH_NEGATION_INWARDS_0, "~ ($X & $Y) := (~ $X | ~ $Y).")
RULE(BF_PUSH_NEGATION_INWARDS_1, "~ ($X | $Y) := (~ $X & ~ $Y).")
RULE(BF_ELIM_DOUBLE_NEGATION_0, "~ ~ $X :=  $X.")
RULE(BF_SIMPLIFY_ONE_0, "( 1 | $X ) := 1.")
RULE(BF_SIMPLIFY_ONE_1, "( $X | 1 ) := 1.")
RULE(BF_SIMPLIFY_ONE_2, "( 1 & $X ) := $X.")
RULE(BF_SIMPLIFY_ONE_3, "( $X & 1 ) := $X.")
RULE(BF_SIMPLIFY_ONE_4, "~ 1 := 0.")
RULE(BF_SIMPLIFY_ZERO_0, "( 0 & $X ) := 0.")
RULE(BF_SIMPLIFY_ZERO_1, "( $X & 0 ) := 0.")
RULE(BF_SIMPLIFY_ZERO_2, "( 0 | $X ) := $X.")
RULE(BF_SIMPLIFY_ZERO_3, "( $X | 0 ) := $X.")
RULE(BF_SIMPLIFY_ZERO_4, "~ 0 := 1.")
RULE(BF_SIMPLIFY_SELF_0, "( $X & $X ) := $X.")
RULE(BF_SIMPLIFY_SELF_1, "( $X | $X ) := $X.")
RULE(BF_SIMPLIFY_SELF_2, "( $X & ~ $X ) := 0.")
RULE(BF_SIMPLIFY_SELF_3, "( $X | ~ $X ) := 1.")
RULE(BF_SIMPLIFY_SELF_4, "( ~ $X & $X ) := 0.")
RULE(BF_SIMPLIFY_SELF_5, "( ~ $X | $X ) := 1.")

RULE(BF_FUNCTIONAL_QUANTIFIERS_0, "fall $X $Y := bf_remove_funiversal_cb $X $Y 1 0.")
RULE(BF_FUNCTIONAL_QUANTIFIERS_1, "fex $X $Y := bf_remove_fexistential_cb $X $Y 1 0.")
RULE(BF_SKIP_CONSTANTS_0, "({ $X } & $Y) := ($Y & { $X }).")

// bf definitions
RULE(BF_DEF_XOR, "( $X + $Y ) := (( $X & ~ $Y ) | ( ~ $X & $Y )).")

// bf callbacks
RULE(BF_CALLBACK_AND, "( { $X } & { $Y } ) := bf_and_cb $X $Y.")
RULE(BF_CALLBACK_OR, "( { $X } | { $Y } ) := bf_or_cb $X $Y.")
RULE(BF_CALLBACK_XOR, "( { $X } + { $Y } ) := bf_xor_cb $X $Y.")
RULE(BF_CALLBACK_NEG, "~ { $X } := bf_neg_cb $X.")
RULE(BF_CALLBACK_IS_ZERO, "{ $X } := bf_is_zero_cb { $X } 0.")
RULE(BF_CALLBACK_IS_ONE, "{ $X } := bf_is_one_cb { $X } 1.")

// speed up callbacks
RULE(BF_CALLBACK_CLASHING_SUBFORMULAS_0, "( $X & $Y ) :=  bf_has_clashing_subformulas_cb ( $X & $Y ) 0.")
RULE(BF_CALLBACK_HAS_SUBFORMULA_0, "( $X & $Y ) := bf_has_subformula_cb ( $X & $Y ) 0 0.")
RULE(WFF_CALLBACK_CLASHING_SUBFORMULAS_0, "( $X && $Y ) ::=  wff_has_clashing_subformulas_cb ( $X && $Y ) F.")
RULE(WFF_CALLBACK_HAS_SUBFORMULA_0, "( $X && $Y ) ::= wff_has_subformula_cb ( $X && $Y ) F F.")

// wff rules
RULE(WFF_DISTRIBUTE_0, "(($X || $Y) && $Z) ::= (($X && $Z) || ($Y && $Z)).")
RULE(WFF_DISTRIBUTE_1, "($X && ($Y || $Z)) ::= (($X && $Y) || ($X && $Z)).")
RULE(WFF_PUSH_NEGATION_INWARDS_0, "! ($X && $Y) ::= (! $X || ! $Y).")
RULE(WFF_PUSH_NEGATION_INWARDS_1, "! ($X || $Y) ::= (! $X && ! $Y).")
RULE(WFF_PUSH_NEGATION_INWARDS_2, "! ($X = 0) ::= ($X != 0).")
RULE(WFF_PUSH_NEGATION_INWARDS_3, "! ($X != 0) ::= ($X = 0).")
RULE(WFF_ELIM_DOUBLE_NEGATION_0, "! ! $X ::=  $X.")
RULE(WFF_ELIM_FORALL, "all $X $Y ::= ! ex $X ! $Y.")
RULE(WFF_SIMPLIFY_ONE_0, "( T || $X ) ::= T.")
RULE(WFF_SIMPLIFY_ONE_1, "( $X || T ) ::= T.")
RULE(WFF_SIMPLIFY_ONE_2, "( T && $X ) ::= $X.")
RULE(WFF_SIMPLIFY_ONE_3, "( $X && T ) ::= $X.")
RULE(WFF_SIMPLIFY_ONE_4, " ! T ::= F.")
RULE(WFF_SIMPLIFY_ZERO_0, "( F && $X ) ::= F.")
RULE(WFF_SIMPLIFY_ZERO_1, "( $X && F ) ::= F.")
RULE(WFF_SIMPLIFY_ZERO_2, "( F || $X ) ::= $X.")
RULE(WFF_SIMPLIFY_ZERO_3, "( $X || F ) ::= $X.")
RULE(WFF_SIMPLIFY_ZERO_4, "! F ::= T.")
RULE(WFF_SIMPLIFY_SELF_0, "( $X && $X ) ::= $X.")
RULE(WFF_SIMPLIFY_SELF_1, "( $X || $X ) ::= $X.")
RULE(WFF_SIMPLIFY_SELF_2, "( $X && ! $X ) ::= F.")
RULE(WFF_SIMPLIFY_SELF_3, "( $X || ! $X ) ::= T.")
RULE(WFF_SIMPLIFY_SELF_4, "( ! $X && $X ) ::= F.")
RULE(WFF_SIMPLIFY_SELF_5, "( ! $X || $X ) ::= T.")

// wff definitions of xor, ->, <- and <->.
RULE(WFF_DEF_XOR, "( $X ^ $Y ) ::= (( $X && ! $Y ) || ( ! $X && $Y )).")
RULE(WFF_DEF_CONDITIONAL, "( $X ? $Y : $Z) ::= (($X -> $Y) && (! $X -> $Z)).")
RULE(WFF_DEF_IMPLY, "( $X -> $Y ) ::= ( ! $X || $Y).")
RULE(WFF_DEF_EQUIV, "( $X <-> $Y ) ::= (( $X -> $Y ) && ( $Y -> $X )).")
RULE(WFF_DEF_BEX_0, "bex $X $Y ::= wff_remove_bexistential_cb $X $Y T F.")
RULE(WFF_DEF_BALL_0, "ball $X $Y ::=  wff_remove_buniversal_cb $X $Y T F.")

// additional wff dewfinitions (include wff formulas)
RULE(BF_DEF_LESS_EQUAL, "( $X <= $Y ) ::= ( ($X & ~ $Y) = 0).")
RULE(BF_DEF_LESS, "( $X < $Y ) ::= (( ($X & ~ $Y) = 0) && ( ($X + ~ $Y) != 0)).")
RULE(BF_DEF_GREATER, "( $X > $Y ) ::= ((( $X & ~ $Y ) != 0) || (( $X + ~ $Y) = 0)).")
RULE(BF_DEF_EQ, "( $X = $Y ) ::= (( $X + $Y ) = 0).")
RULE(BF_DEF_NEQ, "( $X != $Y ) ::= (( $X + $Y ) != 0).")

// wff callbacks
RULE(BF_CALLBACK_EQ, "( { $X } = 0 ) ::= bf_eq_cb $X T F.") // (T|F) is wff_(t|f)
RULE(BF_CALLBACK_NEQ, "( { $X } != 0 ) ::= bf_neq_cb $X T F.") // (T|F) is wff_(t|f)

RULE(BF_EQ_SIMPLIFY_0, "( 1 = 0 ) ::=  F.")
RULE(BF_EQ_SIMPLIFY_1, "( 0 = 0 ) ::= T.")
RULE(BF_NEQ_SIMPLIFY_0, "( 0 != 0 ) ::= F.")
RULE(BF_NEQ_SIMPLIFY_1, "( 1 != 0 ) ::= T.")

RULE(BF_POSITIVE_LITERAL_UPWARDS_0, "(($X != 0) && (($Y = 0) && ($Z != 0))) ::= (($Y = 0) && (($X != 0) && ($Z != 0))).")
RULE(BF_POSITIVE_LITERAL_UPWARDS_1, "(($X != 0) && (($Y != 0) && ($Z = 0))) ::= (($Z = 0) && (($X != 0) && ($Y != 0))).")
RULE(BF_POSITIVE_LITERAL_UPWARDS_2, "((($X = 0) && ( $Y != 0)) && ($Z != 0)) ::= (($X = 0) && (($Y != 0) && ($Z != 0))).")
RULE(BF_POSITIVE_LITERAL_UPWARDS_3, "((($X != 0) && ( $Y = 0)) && ($Z != 0)) ::= (($Y = 0) && (($X != 0) && ($Z != 0))).")
RULE(BF_POSITIVE_LITERAL_UPWARDS_4, "(($X != 0) && ( $Y = 0)) ::= (($Y = 0) && ($X != 0)).")
RULE(BF_SQUEEZE_POSITIVES_0, "(( $X = 0 ) && ($Y = 0)) ::= (( $X | $Y ) = 0).")
RULE(WFF_REMOVE_EX_0, "ex $X $Y ::= wff_remove_existential_cb $X $Y.")

// TODO (LOW) delete trivial quantified formulas (i.e. ∀x. F = no_x..., ).

// bf
template<typename... BAs>
// TODO (LOW) rename library with rwsys or another name
static auto apply_defs = make_library<BAs...>(
	// wff defs
	WFF_DEF_XOR
	+ WFF_DEF_CONDITIONAL
	+ WFF_DEF_IMPLY
	+ WFF_DEF_EQUIV
	// bf defs
	+ BF_DEF_XOR
	+ WFF_DEF_BEX_0
	+ WFF_DEF_BALL_0
);

template<typename... BAs>
static auto apply_defs_once = make_library<BAs...>(
	// wff defs
	BF_DEF_LESS_EQUAL
	+ BF_DEF_LESS
	+ BF_DEF_GREATER
	+ BF_DEF_EQ
	+ BF_DEF_NEQ
);

template<typename... BAs>
static auto elim_for_all = make_library<BAs...>(
	WFF_ELIM_FORALL
);

template<typename... BAs>
static auto to_dnf_wff = make_library<BAs...>(
	WFF_DISTRIBUTE_0
	+ WFF_DISTRIBUTE_1
	+ WFF_PUSH_NEGATION_INWARDS_0
	+ WFF_PUSH_NEGATION_INWARDS_1
	+ WFF_PUSH_NEGATION_INWARDS_2
	+ WFF_PUSH_NEGATION_INWARDS_3
	+ WFF_ELIM_DOUBLE_NEGATION_0
);

template<typename... BAs>
static auto to_dnf_bf = make_library<BAs...>(
	BF_DISTRIBUTE_0
	+ BF_DISTRIBUTE_1
	+ BF_PUSH_NEGATION_INWARDS_0
	+ BF_PUSH_NEGATION_INWARDS_1
	+ BF_ELIM_DOUBLE_NEGATION_0
);

template<typename... BAs>
static auto simplify_bf = make_library<BAs...>(
	BF_SIMPLIFY_ONE_0
	+ BF_SIMPLIFY_ONE_1
	+ BF_SIMPLIFY_ONE_2
	+ BF_SIMPLIFY_ONE_3
	+ BF_SIMPLIFY_ONE_4
	+ BF_SIMPLIFY_ZERO_0
	+ BF_SIMPLIFY_ZERO_1
	+ BF_SIMPLIFY_ZERO_2
	+ BF_SIMPLIFY_ZERO_3
	+ BF_SIMPLIFY_ZERO_4
	+ BF_SIMPLIFY_SELF_0
	+ BF_SIMPLIFY_SELF_1
	+ BF_SIMPLIFY_SELF_2
	+ BF_SIMPLIFY_SELF_3
	+ BF_SIMPLIFY_SELF_4
	+ BF_SIMPLIFY_SELF_5
);

template<typename... BAs>
static auto simplify_wff = make_library<BAs...>(
	WFF_SIMPLIFY_ONE_0
	+ WFF_SIMPLIFY_ONE_1
	+ WFF_SIMPLIFY_ONE_2
	+ WFF_SIMPLIFY_ONE_3
	+ WFF_SIMPLIFY_ONE_4
	+ WFF_SIMPLIFY_ZERO_0
	+ WFF_SIMPLIFY_ZERO_1
	+ WFF_SIMPLIFY_ZERO_2
	+ WFF_SIMPLIFY_ZERO_3
	+ WFF_SIMPLIFY_ZERO_4
	+ WFF_SIMPLIFY_SELF_0
	+ WFF_SIMPLIFY_SELF_1
	+ WFF_SIMPLIFY_SELF_2
	+ WFF_SIMPLIFY_SELF_3
	+ WFF_SIMPLIFY_SELF_4
	+ WFF_SIMPLIFY_SELF_5
);

template<typename... BAs>
static auto apply_cb = make_library<BAs...>(
	BF_CALLBACK_AND
	+ BF_CALLBACK_OR
	+ BF_CALLBACK_XOR
	+ BF_CALLBACK_NEG
	+ BF_CALLBACK_EQ
 	+ BF_CALLBACK_NEQ
);

template<typename... BAs>
static auto apply_speed_up_cb = make_library<BAs...>(
	BF_CALLBACK_CLASHING_SUBFORMULAS_0
	+ BF_CALLBACK_HAS_SUBFORMULA_0
	+ WFF_CALLBACK_CLASHING_SUBFORMULAS_0
	+ WFF_CALLBACK_HAS_SUBFORMULA_0
);

template<typename... BAs>
static auto clause_simplify_bf = make_library<BAs...>(
	BF_CALLBACK_CLASHING_SUBFORMULAS_0
	+ BF_CALLBACK_HAS_SUBFORMULA_0
);

template<typename... BAs>
static auto clause_simplify_wff = make_library<BAs...>(
	WFF_CALLBACK_CLASHING_SUBFORMULAS_0
	+ WFF_CALLBACK_HAS_SUBFORMULA_0
);

template<typename... BAs>
static auto squeeze_positives = make_library<BAs...>(
	BF_SQUEEZE_POSITIVES_0
);

template<typename... BAs>
static auto wff_remove_existential = make_library<BAs...>(
	WFF_REMOVE_EX_0
);

template<typename... BAs>
static auto bf_elim_quantifiers = make_library<BAs...>(
	BF_FUNCTIONAL_QUANTIFIERS_0
	+ BF_FUNCTIONAL_QUANTIFIERS_1
);

template<typename... BAs>
static auto trivialities = make_library<BAs...>(
	BF_EQ_SIMPLIFY_0
	+ BF_EQ_SIMPLIFY_1
	+ BF_NEQ_SIMPLIFY_0
	+ BF_NEQ_SIMPLIFY_1
);

template<typename... BAs>
static auto bf_positives_upwards = make_library<BAs...>(
	BF_POSITIVE_LITERAL_UPWARDS_0
	+ BF_POSITIVE_LITERAL_UPWARDS_1
	+ BF_POSITIVE_LITERAL_UPWARDS_2
	+ BF_POSITIVE_LITERAL_UPWARDS_3
	+ BF_POSITIVE_LITERAL_UPWARDS_4
);

// TODO (MEDIUM) clean execution api code
template<typename... BAs>
struct step {
	step(library<nso<BAs...>> lib): lib(lib) {}

	nso<BAs...> operator()(const nso<BAs...>& n) const {
		return nso_rr_apply(lib, n);
	}

	library<nso<BAs...>> lib;
};

template<typename step_t, typename...BAs>
struct steps {

	steps(std::vector<step_t> libraries) : libraries(libraries) {}
	steps(step_t library) {
		libraries.push_back(library);
	}

	nso<BAs...> operator()(const nso<BAs...>& n) const {
		if (libraries.empty()) return n;
		auto nn = n;
		for (auto& lib : libraries) nn = lib(nn);
		return nn;
	}

	std::vector<step_t> libraries;
};

template<typename step_t, typename... BAs>
struct repeat_each {

	repeat_each(steps<step_t, BAs...> s) : s(s) {}
	repeat_each(step_t s) : s(steps<step_t, BAs...>(s)) {}

	nso<BAs...> operator()(const nso<BAs...>& n) const {
		auto nn = n;
		for (auto& l: s.libraries) {
			std::set<nso<BAs...>> visited;
			while (true) {
				nn = l(nn);
				if (visited.find(nn) != visited.end()) break;
				visited.insert(nn);
			}
		}
		return nn;
	}

	steps<step_t, BAs...> s;
};

template<typename step_t, typename... BAs>
struct repeat_all {

	repeat_all(steps<step_t, BAs...> s) : s(s) {}
	repeat_all(step_t s) : s(steps<step_t, BAs...>(s)) {}

	nso<BAs...> operator()(const nso<BAs...>& n) const {
		auto nn = n;
		std::set<nso<BAs...>> visited;
		while (true) {
			for (auto& l: s.libraries) nn = l(nn);
			auto nnn = s(nn);
			if (nnn == nn) break;
			nn = nnn;
		}
		return nn;
	}

	steps<step_t, BAs...> s;
};

template<typename step_t, typename... BAs>
struct repeat_once {

	repeat_once(steps<step_t, BAs...> s) : s(s) {}
	repeat_once(step_t s) : s(steps<step_t, BAs...>(s)) {}

	nso<BAs...> operator()(const nso<BAs...>& n) const {
		auto nn = n;
		for(auto& l: s.libraries) {
			nn = l(nn);
		}
		return nn;
	}

	steps<step_t, BAs...> s;
};


template<typename...BAs>
steps<step<BAs...>, BAs...> operator|(const library<nso<BAs...>>& l, const library<nso<BAs...>>& r) {
	auto s = steps<step<BAs...>, BAs...>(step<BAs...>(l));
	s.libraries.push_back(r);
	return s;
}

template<typename step_t, typename...BAs>
steps<repeat_each<step_t, BAs...>, BAs...> operator|(const repeat_each<step_t, BAs...>& l, const repeat_each<step_t, BAs...>& r) {
	auto s = steps<repeat_each<step_t, BAs...>, BAs...>(l);
	s.libraries.push_back(r);
	return s;
}

template<typename step_t, typename...BAs>
steps<repeat_all<step_t, BAs...>, BAs...> operator|(const repeat_all<step_t, BAs...>& l, const repeat_all<step_t, BAs...>& r) {
	auto s = steps<repeat_all<step_t, BAs...>, BAs...>(l);
	s.libraries.push_back(r);
	return s;
}

template<typename step_t, typename... BAs>
steps<step_t, BAs...> operator|(const steps<step_t, BAs...>& s, const step_t& l) {
	auto ns = s;
	ns.libraries.push_back(l);
	return ns;
}

template<typename step_t, typename... BAs>
steps<step_t, BAs...> operator|(const steps<step_t, BAs...>& s, const library<nso<BAs...>>& l) {
	auto ns = s;
	ns.libraries.push_back(l);
	return ns;
}

template<typename... BAs>
steps<step<library<nso<BAs...>>, BAs...>, BAs...> operator|(const steps<step<library<nso<BAs...>>, BAs...>, BAs...>& s, const library<nso<BAs...>>& l) {
	auto ns = s;
	ns.libraries.push_back(l);
	return ns;
}

template<typename... BAs>
nso<BAs...> operator|(const nso<BAs...>& n, const library<nso<BAs...>>& l) {
	auto s = step<BAs...>(l);
	return s(n);
}

template<typename step_t, typename... BAs>
nso<BAs...> operator|(const nso<BAs...>& n, const steps<step_t, BAs...>& s) {
	return s(n);
}

template<typename step_t, typename... BAs>
nso<BAs...> operator|(const nso<BAs...>& n, const repeat_once<step_t, BAs...>& r) {
	return r(n);
}

template<typename step_t, typename... BAs>
nso<BAs...> operator|(const nso<BAs...>& n, const repeat_all<step_t, BAs...>& r) {
	return r(n);
}

template<typename step_t, typename... BAs>
nso<BAs...> operator|(const nso<BAs...>& n, const repeat_each<step_t, BAs...>& r) {
	return r(n);
}

// executes the normalizer on the given source code taking into account the
// bindings provided.
template<typename... BAs>
rr<nso<BAs...>> normalizer(std::string& source, bindings<BAs...>& binds) {
	auto form_source = make_tau_source(source);
	auto form = make_nso_rr_using_bindings(form_source, binds);
	return normalizer(form);
}

// executes the normalizer on the given source code taking into account the
// provided factory.
template<typename factory_t, typename... BAs>
rr<nso<BAs...>> normalizer(std::string& source, factory_t& factory) {
	auto form_source = make_tau_source(source);
	auto form = make_nso_rr_using_factory(form_source, factory);
	return normalizer(form);
}

template<typename... BAs>
static const auto is_not_eq_or_neq_to_zero_predicate = [](const nso<BAs...>& n) {
	auto check = (n | only_child_extractor<BAs...> || tau_parser::bf)[1] || tau_parser::bf_f;
	return check.empty();
};

template<typename... BAs>
using is_not_eq_or_neq_predicate_t = decltype(is_not_eq_or_neq_to_zero_predicate<BAs...>);

template<typename... BAs>
nso<BAs...> apply_definitions(const nso<BAs...>& form) {
	return nso_rr_apply_if(apply_defs_once<BAs...>, form, is_not_eq_or_neq_to_zero_predicate<BAs...>);
}

template<typename... BAs>
rr<nso<BAs...>> apply_definitions(const rr<nso<BAs...>>& rr_nso) {
	auto nmain = apply_definitions(rr_nso.main);
	rec_relations<nso<BAs...>> nrec_relations;
	for (const auto& r : rr_nso.rec_relations) {
		auto [matcher, body] = r;
		nrec_relations.emplace_back(matcher, apply_definitions(body));
	}
	return { nrec_relations, nmain };
}

template<typename ... BAs>
nso<BAs...> normalizer_step(const nso<BAs...>& form) {
	return form
		| repeat_all<step<BAs...>, BAs...>(
			step<BAs...>(apply_defs<BAs...>))
		| repeat_all<step<BAs...>, BAs...>(
			step<BAs...>(elim_for_all<BAs...>))
		| repeat_each<step<BAs...>, BAs...>(
			to_dnf_wff<BAs...>
			| simplify_wff<BAs...>
			| clause_simplify_wff<BAs...>)
		| repeat_all<step<BAs...>, BAs...>(
			bf_positives_upwards<BAs...>
			| squeeze_positives<BAs...>
			| wff_remove_existential<BAs...>)
		| repeat_all<step<BAs...>, BAs...>(
			bf_elim_quantifiers<BAs...>
			| to_dnf_bf<BAs...>
			| simplify_bf<BAs...>
			| apply_cb<BAs...>)
		| repeat_all<step<BAs...>, BAs...>(
			clause_simplify_bf<BAs...>
			| trivialities<BAs...>
			| to_dnf_wff<BAs...>
			| simplify_wff<BAs...>
			| clause_simplify_wff<BAs...>);
}

template<typename... BAs>
auto get_vars_from_nso(const nso<BAs...>& n) {
	return select_all(n, is_var_or_capture<BAs...>);
}

template <typename... BAs>
bool are_equivalent_nso(nso<BAs...> n1, nso<BAs...> n2) {
	auto vars1 = get_vars_from_nso(n1);
	auto vars2 = get_vars_from_nso(n2);
	std::set<nso<BAs...>> vars(vars1.begin(), vars1.end());
	vars.insert(vars2.begin(), vars2.end());

	nso<BAs...> wff = build_wff_equiv<BAs...>(n1, n2);
	if (vars.empty()) {
		auto check = normalizer_step(wff) | tau_parser::wff_t;
		return check.has_value();
	}

	for(auto& v: vars) wff = build_wff_all<BAs...>(v, wff);
	rr<nso<BAs...>> nso_rr{wff};
	auto normalized = normalizer(nso_rr);
	auto check = normalized | tau_parser::wff_t;
	return check.has_value();
}

template <typename... BAs>
auto is_equivalent_to_some_previous(const nso<BAs...>& n, std::vector<nso<BAs...>>& previous) {
	return std::any_of(previous.begin(), previous.end(), [n] (const nso<BAs...>& p) {
		return are_equivalent_nso<BAs...>(n, p);
	});
}

template <typename... BAs>
size_t get_max_loopback_in_rr(const nso<BAs...>& form) {
	size_t max = 0;
	for (const auto& offset: select_top(form, is_non_terminal<tau_parser::offsets, BAs...>)) {
		auto current = offset
			| tau_parser::offset
			| tau_parser::num
			| only_child_extractor<BAs...>
			| offset_extractor<BAs...>;
		max = current.has_value() ? std::max(max, current.value()) : max;
	}
	return max;
}

template<typename... BAs>
nso<BAs...> build_num_from_num(nso<BAs...> num, size_t value) {
	auto nts = std::get<tau_source_sym>(num->value).nts;
	auto digits = make_node<tau_sym<BAs...>>(tau_sym<BAs...>(value), {});
	return make_node<tau_sym<BAs...>>( tau_sym<BAs...>(tau_source_sym(tau_parser::num, nts)), {digits});
}

template<typename... BAs>
nso<BAs...> build_shift_from_shift(nso<BAs...> shift, size_t step) {
	auto num = shift | tau_parser::num | optional_value_extractor<nso<BAs...>>;
	auto offset = num | only_child_extractor<BAs...> | offset_extractor<BAs...> | optional_value_extractor<size_t>;
	if (step == offset) return shift | tau_parser::capture | optional_value_extractor<nso<BAs...>>;
	std::map<nso<BAs...>, nso<BAs...>> changes{{num, build_num_from_num<BAs...>(num, step - offset)}};
	return replace<nso<BAs...>>(shift, changes);
}

template<typename... BAs>
nso<BAs...> set_main_to_step(const nso<BAs...>& form, size_t step) {
	std::map<nso<BAs...>, nso<BAs...>> changes;
	for (const auto& offset: select_top(form, is_non_terminal<tau_parser::offsets, BAs...>)) {
		auto shift = offset | tau_parser::shift;
		if (!shift.has_value()) continue;
		auto nshift = build_shift_from_shift<BAs...>(shift.value(), step);
		changes[shift.value()] = nshift;
	}
	return replace<nso<BAs...>>(form, changes);
}

// REVIEW (HIGH) review overall execution
template <typename... BAs>
nso<BAs...> normalizer(const rr<nso<BAs...>>& rr_nso) {
	// IDEA extract this to an operator| overload

	DBG(std::cout << std::endl << "(I): -- Begin normalizer" << std::endl;)
	DBG(std::cout << "(I): -- Apply once definitions" << std::endl;)
	DBG(std::cout << "(F): " << rr_nso.main << std::endl;)

	auto applied_defs = apply_definitions(rr_nso);
	auto loopback = get_max_loopback_in_rr(applied_defs.main);

	std::vector<nso<BAs...>> previous;
	nso<BAs...> current;

	for (int i = loopback; ; i++) {
		current = set_main_to_step(applied_defs.main, i)
			| repeat_all<step<BAs...>, BAs...>(step<BAs...>(applied_defs.rec_relations));

		DBG(std::cout << "(I): -- Begin normalizer step" << std::endl;)
		DBG(std::cout << "(F): " << current << std::endl;)

		current = normalizer_step(current);
		if (is_equivalent_to_some_previous(current, previous)) break;
		else previous.push_back(current);

		DBG(std::cout << "(I): -- End normalizer step" << std::endl;)
	}

	DBG(std::cout << "(I): -- End normalizer" << std::endl;)
	DBG(std::cout << "(O): " << current << std::endl;)

	return current;
}

template <typename... BAs>
rr<nso<BAs...>> normalizer(const nso<BAs...>& form) {
	rr<nso<BAs...>> nso(form);
	return normalizer(nso);
}

} // namespace idni::tau

#endif // __NORMALIZER2_H__
