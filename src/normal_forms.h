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

#ifndef __NORMAL_FORMS_H__
#define __NORMAL_FORMS_H__

#include "nso_rr.h"
#include "execution.h"

#ifdef DEBUG
#include "debug_helpers.h"
#endif // DEBUG

using namespace idni::rewriter;
using namespace idni::tau;

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

// bf definitions
RULE(BF_DEF_XOR, "$X + $Y := $X & $Y' | $X' & $Y.")

// remove functional quantifiers
RULE(BF_FUNCTIONAL_QUANTIFIERS_0, "fall $X $Y := bf_remove_funiversal_cb $X $Y 1 0.")
RULE(BF_FUNCTIONAL_QUANTIFIERS_1, "fex $X $Y := bf_remove_fexistential_cb $X $Y 1 0.")

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

// wff definitions
RULE(WFF_DEF_XOR, "$X ^ $Y ::= $X && !$Y || !$X && $Y.")
RULE(WFF_DEF_CONDITIONAL, "$X ? $Y : $Z ::= ($X -> $Y) && (!$X -> $Z).")
RULE(WFF_DEF_IMPLY, "$X -> $Y ::= !$X || $Y.")
RULE(WFF_DEF_EQUIV, "$X <-> $Y ::= ($X -> $Y) && ($Y -> $X).")
RULE(WFF_DEF_BEX_0, "bool_ex $X $Y ::= wff_remove_bexistential_cb $X $Y T F.")
RULE(WFF_DEF_BALL_0, "bool_all $X $Y ::=  wff_remove_buniversal_cb $X $Y T F.")
// additional wff definitions (include wff formulas)
RULE(BF_DEF_LESS_EQUAL, "$X <= $Y ::= $X & $Y' = 0.")
RULE(BF_DEF_LESS, "$X < $Y ::= $X & $Y' = 0 && $X + $Y' != 0.")
RULE(BF_DEF_GREATER, "$X > $Y ::= $X & $Y' != 0 || $X + $Y' = 0.")
// we must expand the xor as its definition has been allready processed
RULE(BF_DEF_EQ, "$X = $Y ::= $X & $Y' | $X' & $Y = 0.")
RULE(BF_DEF_NEQ, "$X != $Y ::= $X & $Y' | $X' & $Y != 0.")

// wff callbacks
RULE(BF_CALLBACK_EQ, "{ $X } = 0 ::= bf_eq_cb $X T F.") // (T|F) is wff_(t|f)
RULE(BF_CALLBACK_NEQ, "{ $X } != 0 ::= bf_neq_cb $X T F.") // (T|F) is wff_(t|f)

// trivialities
RULE(BF_EQ_SIMPLIFY_0, "1 = 0 ::=  F.")
RULE(BF_EQ_SIMPLIFY_1, "0 = 0 ::= T.")
RULE(BF_NEQ_SIMPLIFY_0, "0 != 0 ::= F.")
RULE(BF_NEQ_SIMPLIFY_1, "1 != 0 ::= T.")

// bf conjunctive normal form
RULE(BF_TO_CNF_0, "$X & $Y | $Z := ($X | $Z) & ($Y | $Z).")
RULE(BF_TO_CNF_1, "$X | $Y & $Z := ($X | $Y) & ($X | $Z).")

// wff conjunctive normal form
RULE(WFF_TO_CNF_0, "$X && $Y || $Z ::= ($X || $Z) && ($Y || $Z).")
RULE(WFF_TO_CNF_1, "$X || $Y && $Z ::= ($X || $Y) && ($X || $Z).")

RULE(WFF_PUSH_NEGATION_UPWARDS_0, "$X != $Y ::= !($X = $Y).")
RULE(WFF_UNSQUEEZE_POSITIVES_0, "$X | $Y = 0 ::= $X = 0 && $Y = 0.")

// tau rules to dnf
RULE(TAU_TO_DNF_0, "($X ||| $Y) &&& $Z :::= $X &&& $Z ||| $Y &&& $Z.")
RULE(TAU_TO_DNF_1, "$X &&& ($Y ||| $Z) :::= $X &&& $Y ||| $X &&& $Z.")
RULE(TAU_PUSH_NEGATION_INWARDS_0, "!!! ($X &&& $Y) :::= !!! $X ||| !!! $Y.")
RULE(TAU_PUSH_NEGATION_INWARDS_1, "!!! ($X ||| $Y) :::= !!! $X &&& !!! $Y.")
RULE(TAU_ELIM_DOUBLE_NEGATION_0, "!!! !!! $X :::=  $X.")
RULE(TAU_SIMPLIFY_ONE_0, "{T} ||| $X :::= {T}.")

// wff conjunctive normal form
RULE(TAU_TO_CNF_0, "$X &&& $Y ||| $Z ::= ($X ||| $Z) &&& ($Y ||| $Z).")
RULE(TAU_TO_CNF_1, "$X ||| $Y &&& $Z ::= ($X ||| $Y) &&& ($X ||| $Z).")

// tau simplifications
RULE(TAU_SIMPLIFY_ONE_1, "$X ||| {T} :::= {T}.")
RULE(TAU_SIMPLIFY_ONE_2, "{T} &&& $X :::= $X.")
RULE(TAU_SIMPLIFY_ONE_3, "$X &&& {T} :::= $X.")
RULE(TAU_SIMPLIFY_ONE_4, "!!! {T} :::= {F}.")
RULE(TAU_SIMPLIFY_ZERO_0, "{F} &&& $X :::= {F}.")
RULE(TAU_SIMPLIFY_ZERO_1, "$X &&& {F} :::= {F}.")
RULE(TAU_SIMPLIFY_ZERO_2, "{F} ||| $X :::= $X.")
RULE(TAU_SIMPLIFY_ZERO_3, "$X ||| {F} :::= $X.")
RULE(TAU_SIMPLIFY_ZERO_4, "!!! {F} :::= {T}.")
RULE(TAU_SIMPLIFY_SELF_0, "$X &&& $X :::= $X.")
RULE(TAU_SIMPLIFY_SELF_1, "$X ||| $X :::= $X.")
RULE(TAU_SIMPLIFY_SELF_2, "$X &&& !!! $X :::= {F}.")
RULE(TAU_SIMPLIFY_SELF_3, "$X ||| !!! $X :::= {T}.")
RULE(TAU_SIMPLIFY_SELF_4, "!!! $X &&& $X :::= {F}.")
RULE(TAU_SIMPLIFY_SELF_5, "!!! $X ||| $X :::= {T}.")

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
static auto apply_wff_defs = make_library<BAs...>(
	WFF_DEF_XOR
	+ WFF_DEF_CONDITIONAL
	+ WFF_DEF_IMPLY
	+ WFF_DEF_EQUIV
);

template<typename... BAs>
static auto apply_bf_defs = make_library<BAs...>(
	BF_DEF_XOR
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
static auto to_dnf_bf = make_library<BAs...>(
	BF_TO_DNF_0
	+ BF_TO_DNF_1
	+ BF_PUSH_NEGATION_INWARDS_0
	+ BF_PUSH_NEGATION_INWARDS_1
	+ BF_ELIM_DOUBLE_NEGATION_0
);

template<typename... BAs>
static auto to_dnf_tau = make_library<BAs...>(
	TAU_TO_DNF_0
	+ TAU_TO_DNF_1
	+ TAU_PUSH_NEGATION_INWARDS_0
	+ TAU_PUSH_NEGATION_INWARDS_1
	+ TAU_ELIM_DOUBLE_NEGATION_0
);

template<typename... BAs>
static auto to_dnf_wff = make_library<BAs...>(
	WFF_TO_DNF_0
	+ WFF_TO_DNF_1
	+ WFF_PUSH_NEGATION_INWARDS_0
	+ WFF_PUSH_NEGATION_INWARDS_1
	+ WFF_PUSH_NEGATION_INWARDS_2
	+ WFF_PUSH_NEGATION_INWARDS_3
	+ WFF_ELIM_DOUBLE_NEGATION_0
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
static auto simplify_tau = make_library<BAs...>(
	TAU_SIMPLIFY_ONE_0
	+ TAU_SIMPLIFY_ONE_1
	+ TAU_SIMPLIFY_ONE_2
	+ TAU_SIMPLIFY_ONE_3
	+ TAU_SIMPLIFY_ONE_4
	+ TAU_SIMPLIFY_ZERO_0
	+ TAU_SIMPLIFY_ZERO_1
	+ TAU_SIMPLIFY_ZERO_2
	+ TAU_SIMPLIFY_ZERO_3
	+ TAU_SIMPLIFY_ZERO_4
	+ TAU_SIMPLIFY_SELF_0
	+ TAU_SIMPLIFY_SELF_1
	+ TAU_SIMPLIFY_SELF_2
	+ TAU_SIMPLIFY_SELF_3
	+ TAU_SIMPLIFY_SELF_4
	+ TAU_SIMPLIFY_SELF_5
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
static auto trivialities = make_library<BAs...>(
	BF_EQ_SIMPLIFY_0
	+ BF_EQ_SIMPLIFY_1
	+ BF_NEQ_SIMPLIFY_0
	+ BF_NEQ_SIMPLIFY_1
);

template<typename... BAs>
static auto bf_elim_quantifiers = make_library<BAs...>(
	BF_FUNCTIONAL_QUANTIFIERS_0
	+ BF_FUNCTIONAL_QUANTIFIERS_1
);

template<typename... BAs>
static auto to_mnf_wff = make_library<BAs...>(
	WFF_PUSH_NEGATION_UPWARDS_0
	+ WFF_UNSQUEEZE_POSITIVES_0
);

template<typename... BAs>
static auto to_cnf_wff = make_library<BAs...>(
	WFF_TO_CNF_0
	+ WFF_TO_CNF_1
	+ WFF_PUSH_NEGATION_INWARDS_0
	+ WFF_PUSH_NEGATION_INWARDS_1
	+ WFF_PUSH_NEGATION_INWARDS_2
	+ WFF_PUSH_NEGATION_INWARDS_3
	+ WFF_ELIM_DOUBLE_NEGATION_0
);

template<typename... BAs>
static auto to_cnf_tau = make_library<BAs...>(
	TAU_TO_CNF_0
	+ TAU_TO_CNF_1
	+ TAU_PUSH_NEGATION_INWARDS_0
	+ TAU_PUSH_NEGATION_INWARDS_1
	+ TAU_ELIM_DOUBLE_NEGATION_0
);

template<typename... BAs>
static auto to_cnf_bf = make_library<BAs...>(
	BF_TO_CNF_0
	+ BF_TO_CNF_1
	+ BF_PUSH_NEGATION_INWARDS_0
	+ BF_PUSH_NEGATION_INWARDS_1
	+ BF_ELIM_DOUBLE_NEGATION_0
);

template<typename...BAs>
static auto to_nnf_wff = make_library<BAs...>(
	WFF_PUSH_NEGATION_INWARDS_0
	+ WFF_PUSH_NEGATION_INWARDS_1
	+ WFF_PUSH_NEGATION_INWARDS_2
	+ WFF_PUSH_NEGATION_INWARDS_3
	+ WFF_ELIM_DOUBLE_NEGATION_0
);

template<typename...BAs>
static auto to_nnf_tau = make_library<BAs...>(
	TAU_PUSH_NEGATION_INWARDS_0
	+ TAU_PUSH_NEGATION_INWARDS_1
	+ TAU_ELIM_DOUBLE_NEGATION_0
);

template<typename...BAs>
static auto to_nnf_bf = make_library<BAs...>(
	BF_PUSH_NEGATION_INWARDS_0
	+ BF_PUSH_NEGATION_INWARDS_1
	+ BF_ELIM_DOUBLE_NEGATION_0
);

template<tau_parser::nonterminal type, typename... BAs>
struct reduce {

	// TODO (VERY_HIGH) properly implement it
	nso<BAs...> operator()(const nso<BAs...>& form) const {
		std::map<nso<BAs...>, nso<BAs...>> changes;
		// for all type dnfs do...
		for (auto& dnf: select_top(form, is_non_terminal<type, BAs...>)) {
			auto simplified = simplify(dnf);
			if (simplified != dnf) changes[dnf] = simplified;
		}
		return replace(form, changes);
	}

private:

	void get_literals(const nso<BAs...>& clause, std::set<nso<BAs...>>& literals) const {
		BOOST_LOG_TRIVIAL(trace) << "(I) get_bf_literals of: " << clause;
		if constexpr (type == tau_parser::bf) {
			if (auto check = clause | tau_parser::bf_and; check.has_value())
				for (auto& c : check || tau_parser::bf)
					get_literals(c, literals);
			else {
				literals.insert(clause);
				BOOST_LOG_TRIVIAL(trace) << "(I) found literal: " << clause;
			}
		} else {
			if (auto check = clause | tau_parser::wff_and; check.has_value())
				for (auto& c : check || tau_parser::wff)
					get_literals(c , literals);
			else {
				literals.insert(clause);
				BOOST_LOG_TRIVIAL(trace) << "(I) found literal: " << clause;
			}
		}
	}

	std::set<nso<BAs...>> get_literals(const nso<BAs...>& clause) const {
		std::set<nso<BAs...>> literals;
		get_literals(clause, literals);
		return literals;
	}

	std::pair<std::set<nso<BAs...>>, std::set<nso<BAs...>>> get_positive_negative_literals(const nso<BAs...> clause) const {
		std::set<nso<BAs...>> positives;
		std::set<nso<BAs...>> negatives;

		for(auto& l: get_literals(clause)) {
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

	std::set<nso<BAs...>> get_dnf_clauses(const nso<BAs...>& n, std::set<nso<BAs...>> clauses = {}) const {
		if constexpr (type == tau_parser::bf)
			if (auto check = n | tau_parser::bf_or; check.has_value())
				for (auto& clause: check || tau_parser::bf)
					clauses = get_dnf_clauses(clause, clauses);
			else
				clauses.insert(n);
		else
			if (auto check = n | tau_parser::wff_or; check.has_value())
				for (auto& clause: check || tau_parser::wff)
					clauses = get_dnf_clauses(clause, clauses);
			else
				clauses.insert(n);

		#ifdef DEBUG
		if (clauses.empty()) BOOST_LOG_TRIVIAL(trace) << "(I) found clause: " << n << std::endl;
		else for (auto& clause: clauses) BOOST_LOG_TRIVIAL(trace) << "(I) found clause: " << clause;
		#endif // DEBUG

		return clauses;
	}

	std::optional<nso<BAs...>> build_dnf_clause_from_literals(const std::set<nso<BAs...>>& positives, const std::set<nso<BAs...>>& negatives) const {
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

	std::optional<nso<BAs...>> to_minterm(const nso<BAs...>& clause) const {
		auto [positives, negatives] = get_positive_negative_literals(clause);
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
		return build_dnf_clause_from_literals(positives, negatives);
	}

	nso<BAs...> build_dnf_from_clauses(const std::set<nso<BAs...>>& clauses) const {
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
		auto dnf = *clauses.begin();
		auto it = ++clauses.begin();
		for (; it != clauses.end(); ++it)
			if constexpr (type == tau_parser::bf) dnf = build_bf_or(dnf, *it);
			else dnf = build_wff_or(dnf, *it);

		BOOST_LOG_TRIVIAL(debug) << "(F) " << dnf;
		return dnf;
	}

	nso<BAs...> simplify(const nso<BAs...>& form) const {
		std::set<nso<BAs...>> clauses;
		BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin simplifying of " << form;
		for (auto& clause: get_dnf_clauses(form))
			if (auto dnf = to_minterm(clause); dnf) clauses.insert(dnf.value());
		auto dnf = build_dnf_from_clauses(clauses);
		BOOST_LOG_TRIVIAL(debug) << "(I) -- End simplifying";
		return dnf;
	}
};

template<typename... BAs>
static const reduce<tau_parser::bf, BAs...> reduce_bf;
template<typename... BAs>
using reduce_bf_t = reduce<tau_parser::bf, BAs...>;

template<typename...BAs>
nso<BAs...> operator|(const nso<BAs...>& n, const reduce_bf_t<BAs...>& r) {
	return r(n);
}

template<typename... BAs>
static const reduce<tau_parser::wff, BAs...> reduce_wff;
template<typename... BAs>
using reduce_wff_t = reduce<tau_parser::wff, BAs...>;

template<typename...BAs>
nso<BAs...> operator|(const nso<BAs...>& n, const reduce_wff_t<BAs...>& r) {
	return r(n);
}

template<typename...BAs>
struct onf_wff {

	onf_wff(const nso<BAs...>& var) : var(var) {}

	nso<BAs...> operator()(const nso<BAs...>& n) const {
		auto pred = [this](const auto& n) {
			if (auto check = n | tau_parser::wff_ex | tau_parser::variable; check.has_value()) {
				return (var == check.value());
			}
			return false;
		};
		if (auto quantifier = find_bottom(n, pred); quantifier.has_value()) {
			auto sub_formula = quantifier | tau_parser::wff | optional_value_extractor<nso<BAs...>>;
			return onf_subformula(sub_formula);
		}
		return n;
	}

private:

	nso<BAs...> onf_subformula(const nso<BAs...>& n) const {
		auto eq = find_bottom(n, is_non_terminal<tau_parser::bf_eq, BAs...>);
		std::map<nso<BAs...>, nso<BAs...>> changes_0 = {{var, _0<BAs...>}};
		auto f_0 = replace((eq || tau_parser::bf)[0],  changes_0)
			| repeat_each<step<BAs...>, BAs...>(
				simplify_bf<BAs...>
				| apply_cb<BAs...>)
			| reduce_bf<BAs...>;
		std::map<nso<BAs...>, nso<BAs...>> changes_1 = {{var, _1<BAs...>}};
		auto f_1 = replace((eq || tau_parser::bf)[0],  changes_1)
			| repeat_each<step<BAs...>, BAs...>(
				simplify_bf<BAs...>
				| apply_cb<BAs...>)
			| reduce_bf<BAs...>;
		std::map<nso<BAs...>, nso<BAs...>> changes;
		auto eq_change = trim(build_bf_interval(f_0, var, f_1));
		changes[eq | optional_value_extractor<nso<BAs...>>] = eq_change;
		for (auto& neq: select_all(n, is_non_terminal<tau_parser::bf_neq, BAs...>)) {
			auto bounds = neq || tau_parser::bf;
			std::map<nso<BAs...>, nso<BAs...>> changes_neq_0 = {{var, _0<BAs...>}};
			auto f_0 = replace(bounds[0],  changes_neq_0)
				| repeat_each<step<BAs...>, BAs...>(
					simplify_bf<BAs...>
					| apply_cb<BAs...>)
				| reduce_bf<BAs...>;
			std::map<nso<BAs...>, nso<BAs...>> changes_neq_1 = {{var, _1<BAs...>}};
			auto f_1 = replace(bounds[0],  changes_neq_1)
				| repeat_each<step<BAs...>, BAs...>(
					simplify_bf<BAs...>
					| apply_cb<BAs...>)
				| reduce_bf<BAs...>;
			auto nleq_change = build_bf_or(build_bf_nleq_lower(f_0, var), build_bf_nleq_upper(f_1, var));
			changes[neq] = nleq_change;
		}
		return replace(n, changes) | repeat_all<step<BAs...>, BAs...>(to_dnf_wff<BAs...>);
	}

	nso<BAs...> var;
};

template<typename... BAs>
using onf_wff_t = onf_wff<BAs...>;

template<typename...BAs>
nso<BAs...> operator|(const nso<BAs...>& n, const onf_wff_t<BAs...>& r) {
	return r(n);
}

template<typename...BAs>
std::optional<nso<BAs...>> onf(const nso<BAs...>& n, const nso<BAs...>& var) {
	return n
		| apply_wff_defs<BAs...>
		| onf_wff<BAs...>(var)
		| repeat_all<step<BAs...>, BAs...>(
			to_dnf_wff<BAs...>
			| simplify_wff<BAs...>
			| trivialities<BAs...>);
}

template<typename...BAs>
nso<BAs...> dnf_wff(const nso<BAs...>& n) {
	auto quantified = [](const auto& n) -> bool {
		return (n | tau_parser::wff_ex).has_value() || (n | tau_parser::wff_all).has_value();
	};
	auto quantifier = find_bottom(n, quantified);
	auto nn = quantifier ? quantifier | tau_parser::wff | optional_value_extractor<nso<BAs...>> : n;
	auto nform = apply_once_definitions(nn)
		| repeat_each<step<BAs...>, BAs...>(
			apply_wff_defs<BAs...>
			| to_dnf_wff<BAs...>
			| simplify_wff<BAs...>
			| trivialities<BAs...>
		);
	// finally, we also simplify the bf part of the formula
	return dnf_bf(nform);
}

template<typename...BAs>
nso<BAs...> dnf_bf(const nso<BAs...>& n) {
	return n
		| repeat_all<step<BAs...>, BAs...>(
			apply_bf_defs<BAs...>
			| bf_elim_quantifiers<BAs...>)
		| repeat_all<step<BAs...>, BAs...>(
			to_dnf_bf<BAs...>
			| simplify_bf<BAs...>
			| apply_cb<BAs...>)
		| reduce_bf<BAs...>;
}

template<typename...BAs>
nso<BAs...> cnf_wff(const nso<BAs...>& n) {
	auto quantified = [](const auto& n) -> bool {
		return (n | tau_parser::wff_ex) || (n | tau_parser::wff_all);
	};
	auto quantifier = find_bottom(n, quantified);
	auto nn = quantifier ? quantifier | tau_parser::wff | optional_value_extractor<nso<BAs...>> : n;
	auto nform = apply_once_definitions(nn)
		| repeat_each<step<BAs...>, BAs...>(
			apply_wff_defs<BAs...>
			| to_cnf_wff<BAs...>
			| simplify_wff<BAs...>
			| trivialities<BAs...>
		);
	// finally, we also simplify the bf part of the formula
	return cnf_bf(nform);
}

template<typename...BAs>
nso<BAs...> cnf_bf(const nso<BAs...>& n) {
	return n
		| repeat_each<step<BAs...>, BAs...>(
			apply_bf_defs<BAs...>
			| bf_elim_quantifiers<BAs...>)
		| repeat_all<step<BAs...>, BAs...>(
			to_cnf_bf<BAs...>
			| simplify_bf<BAs...>
			| apply_cb<BAs...>);
}

template<typename...BAs>
nso<BAs...> nnf_wff(const nso<BAs...>& n) {
	auto quantified = [](const auto& n) -> bool {
		return (n | tau_parser::wff_ex) || (n | tau_parser::wff_all);
	};
	auto quantifier = find_bottom(n, quantified);
	auto nn = quantifier ? quantifier | tau_parser::wff | optional_value_extractor<nso<BAs...>> : n;
	auto nform = apply_once_definitions(nn)
		| repeat_each<step<BAs...>, BAs...>(
			apply_wff_defs<BAs...>
			| to_nnf_wff<BAs...>
			| simplify_wff<BAs...>
			| trivialities<BAs...>
		);
	// finally, we also simplify the bf part of the formula
	return nnf_wff<tau_parser::bf>(nform);
}

template<typename...BAs>
nso<BAs...> nnf_bf(const nso<BAs...>& n) {
	return n
		| repeat_each<step<BAs...>, BAs...>(
			apply_bf_defs<BAs...>
			| bf_elim_quantifiers<BAs...>)
		| repeat_all<step<BAs...>, BAs...>(
			to_nnf_bf<BAs...>
			| simplify_bf<BAs...>
			| apply_cb<BAs...>);
}

// we assume no functional quantifiers are present and all defs have being applyed
template<typename...BAs>
struct to_bdds {

	using vars = std::set<nso<BAs...>>;

	nso<BAs...> operator()(const nso<BAs...>& n) {
		std::map<nso<BAs...>, nso<BAs...>> changes;
		for (auto& bf: select_top(n, is_non_terminal<tau_parser::bf, BAs...>)) {
			auto vars = select_all(bf, is_non_terminal<tau_parser::var, BAs...>);
			auto vars_set = std::set<nso<BAs...>>(vars.begin(), vars.end());
			auto bdd = bf_to_bdd(vars_set, bf);
			changes[bf] = bdd;
		}
		return replace(n, changes);
	}

private:

	std::pair<nso<BAs...>, nso<BAs...>> split_using_var(const vars& var, const nso<BAs...>& form) {
		auto a = replace(form, { {var, trim(_0<BAs...>)} });
		auto b = replace(form, { {var, trim(_1<BAs...>)}});
		return std::make_pair(a, b);
	}

	std::pair<nso<BAs...>, vars> split_vars(const vars& vs) {
		vars rest(vs.begin(), vs.end() - 1);
		return { vs.back(), rest };
	}

	nso<BAs...> bf_to_bdd(const vars& vs, const nso<BAs...>& form) {
		if (vs.empty()) return form | repeat_all(apply_cb<BAs...>);
		auto [var, rest] = split_vars(vs);
		auto [a, b] = split_using_var(var, form);
		auto a_bdd = bf_to_bdd(rest, a);
		auto b_bdd = bf_to_bdd(rest, b);
		return (a_bdd == b_bdd) ? a_bdd	: build_bf_xor(
			build_bf_and(wrap(tau_parser::bf, var), a_bdd),
			build_bf_and(build_bf_neg(wrap(tau_parser::bf, var), b_bdd)));
	}
};

template<typename... BAs>
static const to_bdds<BAs...> to_bdds_bf;

template<typename... BAs>
using to_bdds_bf_t = to_bdds<BAs...>;

template<typename...BAs>
nso<BAs...> operator|(const nso<BAs...>& n, const to_bdds_bf_t<BAs...>& r) {
	return r(n);
}

// TODO (HIGH) We need an implementation for wff and another for tau
template<typename...BAs>
struct snf_wff {

	using var = nso<BAs...>;
	using vars = std::set<var>;
	using exponent = std::map<var, bool>;
	using constant = std::variant<BAs...>;
	using literal = nso<BAs...>;

	struct data {
		nso<BAs...> phi;
		std::set<nso<BAs...>> positives;
		std::set<nso<BAs...>> negatives;
	};

	nso<BAs...> operator()(const nso<BAs...>& n) {
		// simplify everything so we could remove later cases...
		// we start with phi and empty positives and negatives
		auto vars = select_all(n, is_non_terminal<tau_parser::bf_eq, BAs...>);
		auto vars_set = std::set<nso<BAs...>>(vars.begin(), vars.end());
		return step(vars, {n, {}, {}}).phi;
	}

private:

	// TODO (MEDIUM) this is shared with solver, we should move it to a common place
	static constexpr auto _leq = overloaded([]<typename T>(const T& l, const T& r) -> bool {
			return (~l & r = false);
		}, [](const auto&, const auto&) -> sp_tau_node<BAs...> { throw std::logic_error("wrong types"); });

	static constexpr auto _geq = overloaded([]<typename T>(const T& l, const T& r) -> bool {
			return !(~l & r = false) || ((l & ~r) | (~l & r) = false);
		}, [](const auto&, const auto&) -> sp_tau_node<BAs...> { throw std::logic_error("wrong types"); });

	static constexpr auto _or = overloaded([]<typename T>(const T& l, const T& r) -> bool {
			return l | r;
	}, [](const auto&, const auto&) -> sp_tau_node<BAs...> { throw std::logic_error("wrong types"); });

	exponent get_exponent(const vars& vs, const nso<BAs...>& n) {
		exponent exp;
		auto wff = n | tau_parser::wff;
		auto all_vs = select_top(wff, is_non_terminal<tau_parser::var, BAs...>);
		// we set all variables to true in the exponent
		for (auto& v: all_vs) exp[v] = true;
		// we change the negated ones to false
		auto negs = select_top(wff, is_non_terminal<tau_parser::bf_neg, BAs...>);
		for (auto& v: negs)
			if (auto check = v | is_non_terminal<tau_parser::bf_neg, BAs...>
					| is_non_terminal<tau_parser::var, BAs...>; check)
				exp[check.value()] = false;
		return exp;
	}

	std::optional<constant> get_constant(const nso<BAs...>& literal) {
		return find_top(literal, is_non_terminal<tau_parser::constant, BAs...>);
	}

	std::pair<std::optional<constant>, exponent> get_constant_and_exponent(const vars& vs, const nso<BAs...>& n) {
		return { get_constant(n), get_exponent(vs, n) };
	}

	bool is_disjunction_coefficients_less_equal(const vars& vs, const std::set<nso<BAs...>>& literals, constant& c, exponent& A) {
		// IDEA maybe use variant_ba
		std::variant<BAs...> disjunction;
		for (auto& l: literals) {
			auto [c_j, A_j] = get_constant_and_exponent(vs, l);
			auto cte = l | tau_parser::constant | ba_extractor<BAs...>;
			disjunction = std::visit(_or, disjunction, cte);
		}
		return (std::visit(_leq, disjunction, c));
	}

	bool is_disjunction_coefficients_greater_equal(const vars& vs, const std::set<nso<BAs...>>& literals, constant& c, exponent& A) {
		// IDEA maybe use variant_ba
		std::variant<BAs...> disjunction;
		for (auto& l: literals) {
			auto [c_j, A_j] = get_constant_and_exponent(vs, l);
			auto cte = l | tau_parser::constant | ba_extractor<BAs...>;
			disjunction = std::visit(_or, disjunction, cte);
		}
		return (std::visit(_geq, disjunction, c));
	}

	// compute new data using negatives
	data compute_phi(vars& vs, literal& l, data& d, constant& c_i, exponent& A_i) {
		auto ndata = d;
		for (auto n: ndata.negatives) {
			nso<BAs...> phi;
			if (get_exponent(vs, n) == A_i) {
				if (are_coefficient_less_equal(vs, ndata.negatives, c_i, A_i)) {
					phi = _F<BAs...>; break;
				} else if (is_disjunction_coefficients_greater_equal(vs, ndata.positives, c_i, A_i)) {
					phi = _T<BAs...>; break;
				} else {
					std::map<nso<BAs...>, nso<BAs...>> changes;
					// negation would also work as we have negation over the literal
					changes[l] = _T<BAs...>;
					auto npositives = ndata.positives;
					npositives.insert(l);
					ndata = step(vs, { replace(ndata.phi, changes), npositives, d.negatives });
				}
			}
		}
		return ndata.phi;
	}

	// compute new data using positives
	data compute_chi(vars& vs, literal& l, data& d, constant& c_i, exponent& A_i) {
		auto ndata = d;
		for (auto n: ndata.positives) {
			nso<BAs...> chi;
			if (get_exponent(vs, n) == A_i) {
				if (are_coefficient_greater_equal(vs, ndata.negatives, c_i)) {
					chi = _F<BAs...>; break;
				} else if (is_disjunction_coefficients_less_equal(vs, ndata.positives, c_i)) {
					chi = _T<BAs...>; break;
				} else {
					std::map<nso<BAs...>, nso<BAs...>> changes;
					// negation would also work as we have negation over the literal
					changes[l] = _F<BAs...>;
					auto nnegatives = ndata.positives;
					nnegatives.insert(l);
					ndata = step(vs, { replace(ndata.phi, changes), d.positives, nnegatives });
				}
			}
		}
		return ndata.phi;
	}

	data step(vars& vs, data& d) {
		std::map<nso<BAs...>, nso<BAs...>> changes;
		auto literal = find(d.phi, is_non_terminal<tau_parser::bf_eq, BAs...>);
		auto [c_i, A_i] = get_constant_and_exponent(vs, literal);
		auto phi = compute_phi(vs, d, c_i, A_i), chi = compute_chi(vs, d, c_i, A_i);
		return { build_wff_conditional(literal, phi.phi, chi.phi), d.positives, d.negatives };
	}
};

// compute the strong normalized form of a wff
template<typename...BAs>
static const snf_wff<BAs...> to_snf_wff;

template<typename...BAs>
using snf_wff_t = snf_wff<BAs...>;

template<typename...BAs>
nso<BAs...> operator|(const nso<BAs...>& n, const snf_wff_t<BAs...>& r) {
	return r(n);
}

template<typename...BAs>
nso<BAs...> build_split_wff_using(tau_parser::nonterminal type, const nso<BAs...>& a, const nso<BAs...>& b) {
	// TODO (HIGH) check formulas, should depend on the type
	if (type == tau_parser::bf_eq) return build_wff_or(build_wff_and(a, b), build_wff_and(build_wff_neg(a), build_wff_neg(b)));
	else return build_wff_and(build_wff_or(a, build_wff_neg(b)), build_wff_or(build_wff_neg(a), b));
}

template<typename...BAs>
nso<BAs...> minimize_wff(const nso<BAs...>& n) {
	auto form = n;
	for (auto& var: select_all(n, is_non_terminal<tau_parser::var, BAs...>)) {
		for (auto& c: get_dnf_clauses<tau_parser::wff, BAs...>(n)) {
			for (auto& l: get_literals<tau_parser::wff, BAs...>(c)) {
				auto type = (l | tau_parser::bf_eq) ? tau_parser::bf_eq : tau_parser::bf_neq;
				auto [a, b] = split_bf_using_var(var, n);
				// TODO (HIGH) take into account if we have eq or neq
				auto nl = build_split_wff_using(type, a, b);
			}
		}
	}
	// TODO (HIGH) change the return value
	return n;
}

template<typename...BAs>
nso<BAs...> mnf_wff(const nso<BAs...>& n) {
	// TODO (HIGH) this should be changed to use a proper mnf for wff
	return apply_once_definitions(n)
		| repeat_all<step<BAs...>, BAs...>(
			apply_wff_defs<BAs...>
			| to_dnf_wff<BAs...>
			| to_mnf_wff<BAs...>)
		| reduce_wff<BAs...>;
}

template<typename...BAs>
nso<BAs...> mnf_bf(const nso<BAs...>& n) {
	return n
		| repeat_each<step<BAs...>, BAs...>(
			apply_bf_defs<BAs...>
			| bf_elim_quantifiers<BAs...>)
		| repeat_all<step<BAs...>, BAs...>(
			to_dnf_bf<BAs...>
			| simplify_bf<BAs...>
			| apply_cb<BAs...>);
}

template<size_t type, typename...BAs>
nso<BAs...> anf(const nso<BAs...>& n) {
	// TODO (MEDIUM) write anf (using?)
	std::cout << "Not implemented yet." << std::endl;
	return n;
}

template<typename...BAs>
nso<BAs...> pnf(const nso<BAs...>& n) {
	// TODO (MEDIUM) write pnf (using?)
	std::cout << "Not implemented yet." << std::endl;
	return n;
}

} // namespace idni::tau

#endif // __NORMAL_FORMS_H__