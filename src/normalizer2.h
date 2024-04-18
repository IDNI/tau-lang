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
#include <boost/log/trivial.hpp>

#include "parser.h"
#include "nso_rr.h"

#ifdef DEBUG
#include "debug_helpers.h"
#endif // DEBUG

// TODO (MEDIUM) fix proper types (alias) at this level of abstraction
//
// We should talk about statement, nso_rr (nso_with_rr?), library, rule, builder,
// bindings, etc... instead of sp_tau_node,...

using namespace idni::rewriter;

namespace idni::tau {

// tau system library, used to define the tau system of rewriting rules
#define RULE(name, code) const std::string name = code;

// IDEA (MEDIUM) add commutative rule and halve the number of rules if is performance friendly

// bf rules
RULE(BF_TO_DNF_0, "($X | $Y) & $Z := $X & $Z | $Y & $Z.")
RULE(BF_TO_DNF_1, "$X & ($Y | $Z) := $X & $Y | $X & $Z.")
RULE(BF_PUSH_NEGATION_INWARDS_0, "($X & $Y)' := $X' | $Y'.")
RULE(BF_PUSH_NEGATION_INWARDS_1, "($X | $Y)' := $X' & $Y'.")
RULE(BF_ELIM_DOUBLE_NEGATION_0, "$X'' :=  $X.")
RULE(BF_SIMPLIFY_ONE_0, "1 | $X := 1.")
RULE(BF_SIMPLIFY_ONE_1, "$X | 1 := 1.")
RULE(BF_SIMPLIFY_ONE_2, "1 & $X := $X.")
RULE(BF_SIMPLIFY_ONE_3, "$X & 1 := $X.")
RULE(BF_SIMPLIFY_ONE_4, "1' := 0.")
RULE(BF_SIMPLIFY_ZERO_0, "0 & $X := 0.")
RULE(BF_SIMPLIFY_ZERO_1, "$X & 0 := 0.")
RULE(BF_SIMPLIFY_ZERO_2, "0 | $X := $X.")
RULE(BF_SIMPLIFY_ZERO_3, "$X | 0 := $X.")
RULE(BF_SIMPLIFY_ZERO_4, "0' := 1.")
RULE(BF_SIMPLIFY_SELF_0, "$X & $X := $X.")
RULE(BF_SIMPLIFY_SELF_1, "$X | $X := $X.")
RULE(BF_SIMPLIFY_SELF_2, "$X & $X' := 0.")
RULE(BF_SIMPLIFY_SELF_3, "$X | $X' := 1.")
RULE(BF_SIMPLIFY_SELF_4, "$X' & $X := 0.")
RULE(BF_SIMPLIFY_SELF_5, "$X' | $X := 1.")

RULE(BF_FUNCTIONAL_QUANTIFIERS_0, "fall $X $Y := bf_remove_funiversal_cb $X $Y 1 0.")
RULE(BF_FUNCTIONAL_QUANTIFIERS_1, "fex $X $Y := bf_remove_fexistential_cb $X $Y 1 0.")
RULE(BF_SKIP_CONSTANTS_0, "{ $X } & $Y := $Y & { $X }.")

// bf definitions
RULE(BF_DEF_XOR, "$X + $Y := $X & $Y' | $X' & $Y.")

// bf callbacks
RULE(BF_CALLBACK_AND, "{ $X } & { $Y } := bf_and_cb $X $Y.")
RULE(BF_CALLBACK_OR, "{ $X } | { $Y } := bf_or_cb $X $Y.")
RULE(BF_CALLBACK_XOR, "{ $X } + { $Y } := bf_xor_cb $X $Y.")
RULE(BF_CALLBACK_NEG, "{ $X }' := bf_neg_cb $X.")
RULE(BF_CALLBACK_IS_ZERO, "{ $X } := bf_is_zero_cb { $X } 0.")
RULE(BF_CALLBACK_IS_ONE, "{ $X } := bf_is_one_cb { $X } 1.")

// wff rules
RULE(WFF_TO_DNF_0, "($X || $Y) && $Z ::= $X && $Z || $Y && $Z.")
RULE(WFF_TO_DNF_1, "$X && ($Y || $Z) ::= $X && $Y || $X && $Z.")
RULE(WFF_PUSH_NEGATION_INWARDS_0, "!($X && $Y) ::= ! $X || !$Y.")
RULE(WFF_PUSH_NEGATION_INWARDS_1, "!($X || $Y) ::= ! $X && !$Y.")
RULE(WFF_PUSH_NEGATION_INWARDS_2, "!($X = 0) ::= $X != 0.")
RULE(WFF_PUSH_NEGATION_INWARDS_3, "!($X != 0) ::= $X = 0.")
RULE(WFF_ELIM_DOUBLE_NEGATION_0, "! ! $X ::=  $X.")
RULE(WFF_ELIM_FORALL, "all $X $Y ::= ! ex $X !$Y.")
RULE(WFF_SIMPLIFY_ONE_0, "T || $X ::= T.")
RULE(WFF_SIMPLIFY_ONE_1, "$X || T ::= T.")
RULE(WFF_SIMPLIFY_ONE_2, "T && $X ::= $X.")
RULE(WFF_SIMPLIFY_ONE_3, "$X && T ::= $X.")
RULE(WFF_SIMPLIFY_ONE_4, " ! T ::= F.")
RULE(WFF_SIMPLIFY_ZERO_0, "F && $X ::= F.")
RULE(WFF_SIMPLIFY_ZERO_1, "$X && F ::= F.")
RULE(WFF_SIMPLIFY_ZERO_2, "F || $X ::= $X.")
RULE(WFF_SIMPLIFY_ZERO_3, "$X || F ::= $X.")
RULE(WFF_SIMPLIFY_ZERO_4, "! F ::= T.")
RULE(WFF_SIMPLIFY_SELF_0, "$X && $X ::= $X.")
RULE(WFF_SIMPLIFY_SELF_1, "$X || $X ::= $X.")
RULE(WFF_SIMPLIFY_SELF_2, "$X && ! $X ::= F.")
RULE(WFF_SIMPLIFY_SELF_3, "$X || ! $X ::= T.")
RULE(WFF_SIMPLIFY_SELF_4, "!$X && $X ::= F.")
RULE(WFF_SIMPLIFY_SELF_5, "!$X || $X ::= T.")

// wff definitions of xor, ->, <- and <->.
RULE(WFF_DEF_XOR, "$X ^ $Y ::= $X && !$Y || !$X && $Y.")
RULE(WFF_DEF_CONDITIONAL, "$X ? $Y : $Z ::= ($X -> $Y) && (!$X -> $Z).")
RULE(WFF_DEF_IMPLY, "$X -> $Y ::= !$X || $Y.")
RULE(WFF_DEF_EQUIV, "$X <-> $Y ::= ($X -> $Y) && ($Y -> $X).")
RULE(WFF_DEF_BEX_0, "bool_ex $X $Y ::= wff_remove_bexistential_cb $X $Y T F.")
RULE(WFF_DEF_BALL_0, "bool_all $X $Y ::=  wff_remove_buniversal_cb $X $Y T F.")

// additional wff dewfinitions (include wff formulas)
RULE(BF_DEF_LESS_EQUAL, "$X <= $Y ::= $X & $Y' = 0.")
RULE(BF_DEF_LESS, "$X < $Y ::= ($X & $Y' = 0) && ($X + $Y' != 0).")
RULE(BF_DEF_GREATER, "$X > $Y ::= ($X & $Y' != 0) || ($X + $Y' = 0).")
// we must expand the xor as its definition has been allready processed
RULE(BF_DEF_EQ, "$X = $Y ::= $X & $Y' | $X' & $Y = 0.")
RULE(BF_DEF_NEQ, "$X != $Y ::= $X & $Y' | $X' & $Y != 0.")

// wff callbacks
RULE(BF_CALLBACK_EQ, "{ $X } = 0 ::= bf_eq_cb $X T F.") // (T|F) is wff_(t|f)
RULE(BF_CALLBACK_NEQ, "{ $X } != 0 ::= bf_neq_cb $X T F.") // (T|F) is wff_(t|f)

RULE(BF_EQ_SIMPLIFY_0, "1 = 0 ::=  F.")
RULE(BF_EQ_SIMPLIFY_1, "0 = 0 ::= T.")
RULE(BF_NEQ_SIMPLIFY_0, "0 != 0 ::= F.")
RULE(BF_NEQ_SIMPLIFY_1, "1 != 0 ::= T.")

RULE(BF_POSITIVE_LITERAL_UPWARDS_0, "($X != 0) && ($Y  = 0) && ($Z != 0) ::= ($Y = 0) && ($X != 0) && ($Z != 0).")
RULE(BF_POSITIVE_LITERAL_UPWARDS_1, "($X != 0) && ($Y != 0) && ($Z  = 0) ::= ($Z = 0) && ($X != 0) && ($Y != 0).")
RULE(BF_POSITIVE_LITERAL_UPWARDS_2, "($Z != 0) && ($X  = 0) && ($Y != 0) ::= ($Z != 0) && ($X = 0) && ($Y != 0).")
RULE(BF_POSITIVE_LITERAL_UPWARDS_3, "($Z != 0) && ($X != 0) && ($Y  = 0) ::= ($Z != 0) && ($Y = 0) && ($X != 0).")
RULE(BF_POSITIVE_LITERAL_UPWARDS_4, "($X != 0) && ($Y  = 0) ::= ($Y = 0) && ($X != 0).")
RULE(BF_SQUEEZE_POSITIVES_0, "($X = 0) && ($Y = 0) ::= $X | $Y = 0.")
RULE(WFF_REMOVE_EX_0, "ex $X $Y ::= wff_remove_existential_cb $X $Y.")

RULE(BF_ELIM_PARENTHESIS,  "($X)   := $X.")
RULE(WFF_ELIM_PARENTHESIS, "($X)  ::= $X.")
RULE(TAU_ELIM_PARENTHESIS, "($X) :::= $X.")

// TODO (LOW) delete trivial quantified formulas (i.e. ∀x. F = no_x..., ).

template<typename... BAs>
static auto elim_parenthesis = make_library<BAs...>(
	BF_ELIM_PARENTHESIS
	+ WFF_ELIM_PARENTHESIS
	+ TAU_ELIM_PARENTHESIS
);

template<typename step_t, typename... BAs>
struct repeat_all;
template<typename... BAs>
struct step;

template<typename... BAs>
rule<nso<BAs...>> matcher_apply(const rule<nso<BAs...>>& r,
	const library<nso<BAs...>>& rules)
{
	return rule<nso<BAs...>>{
		r.first | repeat_all<step<BAs...>, BAs...>(step<BAs...>(rules)),
		r.second};
}

template<typename... BAs>
library<nso<BAs...>> matchers_apply(const library<nso<BAs...>>& lib,
	const library<nso<BAs...>>& rules)
{
	library<nso<BAs...>> result;
	for (const auto& l : lib) result.push_back(matcher_apply(l, rules));
	return result;
}

template<typename... BAs>
library<nso<BAs...>> make_library_matchers_apply(const std::string& source,
	const library<nso<BAs...>>& rules)
{
	return matchers_apply(make_library<BAs...>(source), rules);
}

template<typename... BAs>
library<nso<BAs...>> make_library_elim_parenthesis(const std::string& source) {
	return make_library_matchers_apply<BAs...>(source, elim_parenthesis<BAs...>);
}

template<typename... BAs>
// TODO (LOW) rename library with rwsys or another name
static auto apply_defs = make_library_elim_parenthesis<BAs...>(
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
static auto apply_defs_once = make_library_elim_parenthesis<BAs...>(
	// wff defs
	BF_DEF_LESS_EQUAL
	+ BF_DEF_LESS
	+ BF_DEF_GREATER
	+ BF_DEF_EQ
	+ BF_DEF_NEQ
);

template<typename... BAs>
static auto elim_for_all = make_library_elim_parenthesis<BAs...>(
	WFF_ELIM_FORALL
);

template<typename... BAs>
static auto to_dnf_wff = make_library_elim_parenthesis<BAs...>(
	WFF_TO_DNF_0
	+ WFF_TO_DNF_1
	+ WFF_PUSH_NEGATION_INWARDS_0
	+ WFF_PUSH_NEGATION_INWARDS_1
	+ WFF_PUSH_NEGATION_INWARDS_2
	+ WFF_PUSH_NEGATION_INWARDS_3
	+ WFF_ELIM_DOUBLE_NEGATION_0
);

template<typename... BAs>
static auto to_dnf_bf = make_library_elim_parenthesis<BAs...>(
	BF_TO_DNF_0
	+ BF_TO_DNF_1
	+ BF_PUSH_NEGATION_INWARDS_0
	+ BF_PUSH_NEGATION_INWARDS_1
	+ BF_ELIM_DOUBLE_NEGATION_0
);

template<typename... BAs>
static auto simplify_bf = make_library_elim_parenthesis<BAs...>(
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
static auto simplify_wff = make_library_elim_parenthesis<BAs...>(
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
static auto apply_cb = make_library_elim_parenthesis<BAs...>(
	BF_CALLBACK_AND
	+ BF_CALLBACK_OR
	+ BF_CALLBACK_XOR
	+ BF_CALLBACK_NEG
	+ BF_CALLBACK_EQ
 	+ BF_CALLBACK_NEQ
);

template<typename... BAs>
static auto squeeze_positives = make_library_elim_parenthesis<BAs...>(
	BF_SQUEEZE_POSITIVES_0
);

template<typename... BAs>
static auto wff_remove_existential = make_library_elim_parenthesis<BAs...>(
	WFF_REMOVE_EX_0
);

template<typename... BAs>
static auto bf_elim_quantifiers = make_library_elim_parenthesis<BAs...>(
	BF_FUNCTIONAL_QUANTIFIERS_0
	+ BF_FUNCTIONAL_QUANTIFIERS_1
);

template<typename... BAs>
static auto trivialities = make_library_elim_parenthesis<BAs...>(
	BF_EQ_SIMPLIFY_0
	+ BF_EQ_SIMPLIFY_1
	+ BF_NEQ_SIMPLIFY_0
	+ BF_NEQ_SIMPLIFY_1
);

template<typename... BAs>
static auto bf_positives_upwards = make_library_elim_parenthesis<BAs...>(
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

template<tau_parser::nonterminal type, typename... BAs>
void get_literals(const nso<BAs...>& clause, std::set<nso<BAs...>>& literals) {
	BOOST_LOG_TRIVIAL(trace) << "(I) get_bf_literals of: " << clause;
	if constexpr (type == tau_parser::bf) {
		if (auto check = clause | tau_parser::bf_and; check.has_value())
			for (auto& c : check || tau_parser::bf)
				get_literals<type, BAs...>(c, literals);
		else {
			literals.insert(clause);
			BOOST_LOG_TRIVIAL(trace) << "(I) found literal: " << clause;
		}
	} else {
		if (auto check = clause | tau_parser::wff_and; check.has_value())
			for (auto& c : check || tau_parser::wff)
				get_literals<type, BAs...>(c , literals);
		else {
			literals.insert(clause);
			BOOST_LOG_TRIVIAL(trace) << "(I) found literal: " << clause;
		}
	}
}

template<tau_parser::nonterminal type, typename... BAs>
std::set<nso<BAs...>> get_literals(const nso<BAs...>& clause) {
	std::set<nso<BAs...>> literals;
	get_literals<type, BAs...>(clause, literals);
	return literals;
}

template<tau_parser::nonterminal type, typename... BAs>
std::pair<std::set<nso<BAs...>>, std::set<nso<BAs...>>> get_positive_negative_literals(const nso<BAs...> clause) {
	std::set<nso<BAs...>> positives;
	std::set<nso<BAs...>> negatives;

	for(auto& l: get_literals<type, BAs...>(clause)) {
		if constexpr (type == tau_parser::bf) {
			if (auto check = l | tau_parser::bf_neg; !check.has_value()) {
				positives.insert(l);
				BOOST_LOG_TRIVIAL(trace) << "(I) found positive: " << l << std::endl;
			} else {
				negatives.insert(l);
				BOOST_LOG_TRIVIAL(trace) << "(I) found negative: " << l << std::endl;
			}
		} else {
			if (auto check = l | tau_parser::bf_neq; !check.has_value()) {
				positives.insert(l);
				BOOST_LOG_TRIVIAL(trace) << "(I) found positive: " << l << std::endl;
			} else {
				negatives.insert(l);
				BOOST_LOG_TRIVIAL(trace) << "(I) found negative: " << l << std::endl;
			}
		}
	}

	return {positives, negatives};
}

template<tau_parser::nonterminal type, typename... BAs>
std::set<nso<BAs...>> get_dnf_clauses(const nso<BAs...>& n, std::set<nso<BAs...>> clauses = {}) {
	if constexpr (type == tau_parser::bf)
		if (auto check = n | tau_parser::bf_or; check.has_value())
			for (auto& clause: check || tau_parser::bf)
				clauses = get_dnf_clauses<type, BAs...>(clause, clauses);
		else
			clauses.insert(n);
	else
		if (auto check = n | tau_parser::wff_or; check.has_value())
			for (auto& clause: check || tau_parser::wff)
				clauses = get_dnf_clauses<type, BAs...>(clause, clauses);
		else
			clauses.insert(n);

	#ifdef DEBUG
	if (clauses.empty()) BOOST_LOG_TRIVIAL(trace) << "(I) found clause: " << n << std::endl;
	else for (auto& clause: clauses) BOOST_LOG_TRIVIAL(trace) << "(I) found clause: " << clause;
	#endif // DEBUG

	return clauses;
}

template<tau_parser::nonterminal type, typename... BAs>
std::optional<nso<BAs...>> build_dnf_clause_from_literals(const std::set<nso<BAs...>>& positives, const std::set<nso<BAs...>>& negatives) {
	if (positives.empty() && negatives.empty()) {
		BOOST_LOG_TRIVIAL(debug) << "(F) {}";
		return {};
	}

	std::vector<nso<BAs...>> literals;
	literals.insert(literals.end(), positives.begin(), positives.end());
	literals.insert(literals.end(), negatives.begin(), negatives.end());

	if (literals.size() == 1) return literals[0];
	auto clause = literals[0];
	for (size_t i = 1; i < literals.size(); ++i)
		if constexpr (type == tau_parser::bf) clause = build_bf_and(clause, literals[i]);
		else clause = build_wff_and(clause, literals[i]);

	BOOST_LOG_TRIVIAL(debug) << "(I) " << clause;
	return { clause };
}


template<tau_parser::nonterminal type, typename... BAs>
std::optional<nso<BAs...>> to_minterm(const nso<BAs...>& clause) {
	auto [positives, negatives] = get_positive_negative_literals<type, BAs...>(clause);
	if constexpr (type == tau_parser::bf) {
		for (auto& negation: negatives) {
			auto negated = negation | tau_parser::bf_neg | tau_parser::bf | optional_value_extractor<sp_tau_node<BAs...>>;
			for (auto& positive: positives) {
				BOOST_LOG_TRIVIAL(trace) << "(I) are literals " << positive << " and " << negation << " clashing? ";
				if (positive == _0<BAs...>) {
					BOOST_LOG_TRIVIAL(trace) << "yes" << std::endl;
					return {};
				} else if (positive == negated) {
					BOOST_LOG_TRIVIAL(trace) << "yes" << std::endl;
					return {};
				} else {
					BOOST_LOG_TRIVIAL(trace) << "no" << std::endl;
				}
			}
		}
	} else {
		for (auto& negation: negatives) {
			auto neq_bf = negation | tau_parser::bf_neq | tau_parser::bf | optional_value_extractor<sp_tau_node<BAs...>>;
			for (auto& positive: positives) {
				auto eq_bf = positive | tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<sp_tau_node<BAs...>>;
				BOOST_LOG_TRIVIAL(trace) << "(I) are literals " << positive << " and " << negation << " clashing: ";
				if (eq_bf == _F<BAs...>) {
					BOOST_LOG_TRIVIAL(trace) << "yes" << std::endl;
					return {};
				} if (eq_bf == neq_bf) {
					BOOST_LOG_TRIVIAL(trace) << "yes" << std::endl;
					return {};
				} else {
					BOOST_LOG_TRIVIAL(trace) << "no" << std::endl;
				}
			}
		}
	}
	return build_dnf_clause_from_literals<type, BAs...>(positives, negatives);
}

template<tau_parser::nonterminal type, typename... BAs>
nso<BAs...> build_dnf_from_clauses(const std::set<nso<BAs...>>& clauses) {
	if constexpr (type == tau_parser::bf) {
		if (clauses.empty()) {
			BOOST_LOG_TRIVIAL(debug) << "(F) " << _0<BAs...>;
			return _0<BAs...>;
		}
	} else {
		if (clauses.empty()) {
			BOOST_LOG_TRIVIAL(debug) << "(F) " << _F<BAs...>;
			return _F<BAs...>;
		}
	}
	if (clauses.size() == 1) return *clauses.begin();
	auto dnf = *clauses.begin();
	for (auto& clause: clauses)
		if constexpr (type == tau_parser::bf) dnf = build_bf_or(dnf, clause);
		else dnf = build_wff_or(dnf, clause);

	BOOST_LOG_TRIVIAL(debug) << "(F) " << dnf;
	return dnf;
}

template<tau_parser::nonterminal type, typename... BAs>
nso<BAs...> to_mnf(const nso<BAs...>& form) {
	std::set<nso<BAs...>> clauses;
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin simplifying of " << form;
	for (auto& clause: get_dnf_clauses<type, BAs...>(form))
		if (auto dnf = to_minterm<type, BAs...>(clause); dnf) clauses.insert(dnf.value());
	auto dnf = build_dnf_from_clauses<type, BAs...>(clauses);
	BOOST_LOG_TRIVIAL(debug) << "(I) -- End simplifying";
	return dnf;
}

template<tau_parser::nonterminal type, typename... BAs>
struct to_mnfs {

	nso<BAs...> operator()(const nso<BAs...>& form) const {
		std::map<nso<BAs...>, nso<BAs...>> changes;
		// for all type dnfs do...
		for (auto& dnf: select_top(form, is_non_terminal<type, BAs...>)) {
			auto simplified = to_mnf<type, BAs...>(dnf);
			if (simplified != dnf) changes[dnf] = simplified;
		}
		return replace(form, changes);
	}
};

template<typename... BAs>
using to_mnf_bf = to_mnfs<tau_parser::bf, BAs...>;

template<typename... BAs>
using to_mnf_wff = to_mnfs<tau_parser::wff, BAs...>;

template<typename... BAs>
nso<BAs...> operator|(const nso<BAs...>& n, const to_mnf_bf<BAs...>& r) {
	return r(n);
}

template<typename... BAs>
nso<BAs...> operator|(const nso<BAs...>& n, const to_mnf_wff<BAs...>& r) {
	return r(n);
}

// TODO (MEDIUM) implement the removal supeefluous vars
// If some var is an input in the tau lang, but is redundant,we dont even read that input.
// In order to know if a formula is independent in some var (it may appear, but it's still
// be independant independent of it, p.e. xy+xy' is just x, same for xy|xy'...) we could
// use the derivative (f(x) does not depend on x iff f(0)+f(1)=0). So we go over all atomic
// formulas that contain a certain var, and if for all of them f(0)+f(1)=0, then it's completely
// independent in that var. Moreover, ofc we want to push quantifiers inside and simplify atomic
// formulas to not mention vars that they dont depend on (otherwise imagine doing quantifier
// elimination for y in xy+xy' which doesnt even depend on y).
// The way to do it is as follows. we do one-step bdd conversion, wrt x only.
// so f(x)=xf(1)+x'f(0). now f doesnt depend on x iff f(1)=f(0)

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
auto apply_elim_parenthesis = repeat_all<step<BAs...>, BAs...>(
	step<BAs...>(elim_parenthesis<BAs...>));

template<typename... BAs>
nso<BAs...> apply_definitions(const nso<BAs...>& form) {
	return nso_rr_apply_if(apply_defs_once<BAs...>, form, is_not_eq_or_neq_to_zero_predicate<BAs...>);
}

template<typename... BAs>
rr<nso<BAs...>> apply_definitions(const rr<nso<BAs...>>& nso_rr) {
	auto nmain = apply_definitions(nso_rr.main
		| apply_elim_parenthesis<BAs...>);
	rec_relations<nso<BAs...>> nrec_relations;
	for (const auto& r : nso_rr.rec_relations) {
		auto [matcher, body] = r;
		nrec_relations.emplace_back(matcher
				| apply_elim_parenthesis<BAs...>,
			apply_definitions(body));
	}
	return { nrec_relations, nmain };
}

// IDEA (HIGH) rewrite steps as a tuple to optimize the execution
template<typename ... BAs>
nso<BAs...> normalizer_step(const nso<BAs...>& form) {
	static std::map<nso<BAs...>, nso<BAs...>> cache;
	if (auto it = cache.find(form); it != cache.end()) return it->second;
	auto result = form
		| apply_elim_parenthesis<BAs...>
		| repeat_all<step<BAs...>, BAs...>(
			step<BAs...>(apply_defs<BAs...>))
		| apply_elim_parenthesis<BAs...>
		| repeat_all<step<BAs...>, BAs...>(
			step<BAs...>(elim_for_all<BAs...>))
		| repeat_each<step<BAs...>, BAs...>(
			to_dnf_wff<BAs...>
			| simplify_wff<BAs...>)
		| to_mnf_wff<BAs...>()
		| repeat_all<step<BAs...>, BAs...>(
			bf_positives_upwards<BAs...>
			| elim_parenthesis<BAs...>
			| squeeze_positives<BAs...>
			| wff_remove_existential<BAs...>)
		| repeat_all<step<BAs...>, BAs...>(
			bf_elim_quantifiers<BAs...>
			| to_dnf_bf<BAs...>
			| simplify_bf<BAs...>
			| apply_cb<BAs...>)
		| to_mnf_bf<BAs...>()
		| repeat_all<step<BAs...>, BAs...>(
			trivialities<BAs...>
			| simplify_bf<BAs...>
			| simplify_wff<BAs...>);
	cache[form] = result;
	return result;
}

// TODO (LOW) refactor and clean this structure
template<typename... BAs>
struct free_vars_collector {

	free_vars_collector(std::set<nso<BAs...>>& free_vars) : free_vars(free_vars) {}

	nso<BAs...> operator()(const nso<BAs...>& n) {
		if (is_quantifier<BAs...>(n)) {
			// IDEA using quantified_variable => variable | capture would simplify the code
			auto var = find_top(n, is_var_or_capture<BAs...>);
			if (var.has_value()) {
				if (auto it = free_vars.find(var.value()); it != free_vars.end())
					free_vars.erase(it);
			}
			BOOST_LOG_TRIVIAL(trace) << "(I) -- removing quantified var: " << var.value();
		}
		if (is_var_or_capture<BAs...>(n)) {
			if (auto check = n
						| tau_parser::io_var | only_child_extractor<BAs...> | tau_parser::offset
						| only_child_extractor<BAs...>;
					check.has_value() && is_var_or_capture<BAs...>(check.value())) {
				auto var = check.value();
				if (auto it = free_vars.find(var); it != free_vars.end()) {
					free_vars.erase(it);
					BOOST_LOG_TRIVIAL(trace) << "(I) -- removing var: " << var;
				}
			}
			free_vars.insert(n);
			BOOST_LOG_TRIVIAL(trace) << "(I) -- inserting var: " << n;
		}
		return n;
	}

	std::set<nso<BAs...>>& free_vars;
};

template<typename... BAs>
auto get_vars_from_nso(const nso<BAs...>& n) {
	return select_top(n, is_var_or_capture<BAs...>);
}

template<typename... BAs>
auto get_free_vars_from_nso(const nso<BAs...>& n) {
	BOOST_LOG_TRIVIAL(trace) << "(I) -- Begin get_free_vars_from_nso of " << n;
	std::set<nso<BAs...>> free_vars;
	free_vars_collector<BAs...> collector(free_vars);
	post_order_traverser<
			identity_t<nso<BAs...>>,
			free_vars_collector<BAs...>,
			nso<BAs...>>(
		idni::rewriter::identity<nso<BAs...>>, collector)(n);
	BOOST_LOG_TRIVIAL(trace) << "(I) -- Begin get_free_vars_from_nso";
	return free_vars;
}

template <typename... BAs>
bool are_nso_equivalent(nso<BAs...> n1, nso<BAs...> n2) {
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin are_nso_equivalent";
	BOOST_LOG_TRIVIAL(trace) << "(I) -- n1 " << n1;
	BOOST_LOG_TRIVIAL(trace) << "(I) -- n2 " << n2;

	if (n1 == n2) {
		BOOST_LOG_TRIVIAL(debug) << "(I) -- End are_nso_equivalent: true";
		return true;
	}

	nso<BAs...> wff = build_wff_equiv<BAs...>(n1, n2);
	auto vars = get_free_vars_from_nso(wff);
	for(auto& v: vars) wff = build_wff_all<BAs...>(v, wff);
	BOOST_LOG_TRIVIAL(trace) << "(I) -- wff: " << wff;

	auto normalized = normalizer_step<BAs...>(wff);
	auto check = normalized | tau_parser::wff_t;
	BOOST_LOG_TRIVIAL(debug) << "(I) -- End are_nso_equivalent: " << check.has_value();

	return check.has_value();
}

template <typename... BAs>
auto is_nso_equivalent_to_any_of(nso<BAs...>& n, std::vector<nso<BAs...>>& previous) {
	return std::any_of(previous.begin(), previous.end(), [n] (nso<BAs...>& p) {
		return are_nso_equivalent<BAs...>(n, p);
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
nso<BAs...> build_main_step(const nso<BAs...>& form, size_t step) {
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
nso<BAs...> normalizer(const rr<nso<BAs...>>& nso_rr) {
	// IDEA extract this to an operator| overload

	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin normalizer";
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Apply once definitions";

	auto applied_defs = apply_definitions(nso_rr);

	BOOST_LOG_TRIVIAL(debug) << "(I) -- Applied once definitions";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << applied_defs;

	auto loopback = get_max_loopback_in_rr(applied_defs.main);

	std::vector<nso<BAs...>> previous;
	nso<BAs...> current;

	for (int i = loopback; ; i++) {
		current = build_main_step(applied_defs.main, i)
			| repeat_all<step<BAs...>, BAs...>(step<BAs...>(applied_defs.rec_relations));

		BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin normalizer step";
		BOOST_LOG_TRIVIAL(debug) << "(F) " << current;

		current = normalizer_step(current);
		if (is_nso_equivalent_to_any_of(current, previous)) break;
		else previous.push_back(current);

		BOOST_LOG_TRIVIAL(debug) << "(I) -- End normalizer step";
	}

	BOOST_LOG_TRIVIAL(debug) << "(I) -- End normalizer";
	BOOST_LOG_TRIVIAL(debug) << "(O) " << current;

	return current;
}

template <typename... BAs>
nso<BAs...> normalizer(const nso<BAs...>& form) {
	rr<nso<BAs...>> nso_rr(form);
	return normalizer(nso_rr);
}

} // namespace idni::tau

#endif // __NORMALIZER2_H__
