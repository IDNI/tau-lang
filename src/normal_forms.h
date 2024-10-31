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

#include <list>
#include <numeric>

#include "nso_rr.h"
#include "execution.h"

#ifdef DEBUG
#include "debug_helpers.h"
#endif // DEBUG

namespace idni::tau {

// tau system library, used to define the tau system of rewriting rules
#define RULE(name, code) const std::string name = code;

// IDEA (MEDIUM) add commutative rule and halve the number of rules if is performance friendly

// bf rules
RULE(BF_TO_DNF_0, "($X | $Y) & $Z := $X & $Z | $Y & $Z.")
RULE(BF_TO_DNF_1, "$X & ($Y | $Z) := $X & $Y | $X & $Z.")
RULE(BF_PUSH_NEGATION_INWARDS_0, "($X & $Y)' := $X' | $Y'.")
RULE(BF_PUSH_NEGATION_INWARDS_1, "($X | $Y)' := $X' & $Y'.")

// bf callbacks
RULE(BF_CALLBACK_NORMALIZE, "{ $X } := bf_normalize_cb $X.")

// wff rules
RULE(WFF_TO_DNF_0, "($X || $Y) && $Z ::= $X && $Z || $Y && $Z.")
RULE(WFF_TO_DNF_1, "$X && ($Y || $Z) ::= $X && $Y || $X && $Z.")
RULE(WFF_PUSH_NEGATION_INWARDS_0, "!($X && $Y) ::= ! $X || !$Y.")
RULE(WFF_PUSH_NEGATION_INWARDS_1, "!($X || $Y) ::= ! $X && !$Y.")
RULE(WFF_PUSH_NEGATION_INWARDS_2, "!($X = 0) ::= $X != 0.")
RULE(WFF_PUSH_NEGATION_INWARDS_3, "!($X != 0) ::= $X = 0.")
RULE(WFF_PUSH_NEGATION_INWARDS_4, "! always $X ::= sometimes (! $X).")
RULE(WFF_PUSH_NEGATION_INWARDS_5, "! sometimes $X ::= always (! $X).")
RULE(WFF_SIMPLIFY_SOMETIMES_3,  "(sometimes $X) && (always $X) ::= always $X.")
RULE(WFF_SIMPLIFY_ALWAYS_3,		"(always $X) && (sometimes $X) ::= always $X.")
RULE(WFF_PUSH_SOMETIMES_INWARDS,"sometimes($X || $Y) ::= (sometimes $X) || (sometimes $Y).")
RULE(WFF_PUSH_ALWAYS_INWARDS,   "always($X && $Y) ::= (always $X) && (always $Y).")

// trivialities
RULE(BF_EQ_AND_SIMPLIFY_0, "$X != 0 && $X = 0 ::= F.")
RULE(BF_EQ_AND_SIMPLIFY_1, "$X = 0 && $X != 0 ::= F.")
RULE(BF_EQ_OR_SIMPLIFY_0, "$X != 0 || $X = 0 ::= T.")
RULE(BF_EQ_OR_SIMPLIFY_1, "$X = 0 || $X != 0 ::= T.")

// bf conjunctive normal form
RULE(BF_TO_CNF_0, "$X & $Y | $Z := ($X | $Z) & ($Y | $Z).")
RULE(BF_TO_CNF_1, "$X | $Y & $Z := ($X | $Y) & ($X | $Z).")

// wff conjunctive normal form
RULE(WFF_TO_CNF_0, "$X && $Y || $Z ::= ($X || $Z) && ($Y || $Z).")
RULE(WFF_TO_CNF_1, "$X || $Y && $Z ::= ($X || $Y) && ($X || $Z).")

RULE(WFF_PUSH_NEGATION_UPWARDS_0, "$X != 0 ::= !($X = 0).")
RULE(WFF_UNSQUEEZE_POSITIVES_0, "$X | $Y = 0 ::= $X = 0 && $Y = 0.")
RULE(WFF_UNSQUEEZE_NEGATIVES_0, "$X | $Y != 0 ::= $X != 0 || $Y != 0.")
RULE(WFF_SQUEEZE_POSITIVES_0, "$X = 0 && $Y = 0 ::= $X | $Y = 0.")
RULE(WFF_SQUEEZE_NEGATIVES_0, "$X != 0 || $Y != 0 ::= $X | $Y != 0.")

template<typename... BAs>
static auto to_dnf_bf = make_library<BAs...>(
	BF_TO_DNF_0
	+ BF_TO_DNF_1
	+ BF_PUSH_NEGATION_INWARDS_0
	+ BF_PUSH_NEGATION_INWARDS_1
);

template<typename... BAs>
static auto nnf_to_dnf_bf = make_library<BAs...>(
	BF_TO_DNF_0
	+ BF_TO_DNF_1
);

template<typename... BAs>
static auto nnf_to_dnf_wff = make_library<BAs...>(
	WFF_TO_DNF_0
	+ WFF_TO_DNF_1
	+ WFF_PUSH_SOMETIMES_INWARDS
);

template<typename... BAs>
static auto nnf_to_cnf_wff = make_library<BAs...>(
	WFF_TO_CNF_0
	+ WFF_TO_CNF_1
	+ WFF_PUSH_ALWAYS_INWARDS
);

// This set of rules can blow up due to the interaction between
// pushing negation in and distributing the "and" over the "or"
template<typename... BAs>
static auto to_dnf_wff = make_library<BAs...>(
	WFF_TO_DNF_0
	+ WFF_TO_DNF_1
	+ WFF_PUSH_NEGATION_INWARDS_0
	+ WFF_PUSH_NEGATION_INWARDS_1
	+ WFF_PUSH_NEGATION_INWARDS_2
	+ WFF_PUSH_NEGATION_INWARDS_3
	+ WFF_PUSH_NEGATION_INWARDS_4
	+ WFF_PUSH_NEGATION_INWARDS_5
	+ WFF_PUSH_ALWAYS_INWARDS
	+ WFF_PUSH_SOMETIMES_INWARDS
);

template<typename... BAs>
static auto simplify_wff = make_library<BAs...>(
	WFF_SIMPLIFY_SOMETIMES_3
	+ WFF_SIMPLIFY_ALWAYS_3
);

template<typename... BAs>
static auto apply_normalize = make_library<BAs...>(
	BF_CALLBACK_NORMALIZE
);

template<typename... BAs>
static auto elim_eqs = make_library<BAs...>(
	BF_EQ_AND_SIMPLIFY_0
	+ BF_EQ_AND_SIMPLIFY_1
	+ BF_EQ_OR_SIMPLIFY_0
	+ BF_EQ_OR_SIMPLIFY_1
);

template<typename... BAs>
static auto push_neg_for_snf = make_library<BAs...>(
	WFF_PUSH_NEGATION_UPWARDS_0
	+ WFF_PUSH_NEGATION_INWARDS_0
	+ WFF_PUSH_NEGATION_INWARDS_1
);

template<typename... BAs>
static auto simplify_snf = repeat_all<step<BAs...>, BAs...>(
	elim_eqs<BAs...>
	| simplify_wff<BAs...>
	| push_neg_for_snf<BAs...>);

template<typename... BAs>
static auto to_mnf_wff = make_library<BAs...>(
	WFF_PUSH_NEGATION_UPWARDS_0
);

template<typename... BAs>
static auto fix_neg_in_snf = make_library<BAs...>(
	WFF_PUSH_NEGATION_INWARDS_2
	+ WFF_PUSH_NEGATION_INWARDS_3
);

template<typename... BAs>
static auto unsqueeze_wff = make_library<BAs...>(
	WFF_UNSQUEEZE_POSITIVES_0
	+ WFF_UNSQUEEZE_NEGATIVES_0
);

template<typename... BAs>
static auto squeeze_wff = make_library<BAs...>(
	WFF_SQUEEZE_POSITIVES_0
	+ WFF_SQUEEZE_NEGATIVES_0
);

template<typename... BAs>
static auto unsqueeze_wff_pos = make_library<BAs...>(
	WFF_UNSQUEEZE_POSITIVES_0
);

template<typename... BAs>
static auto squeeze_wff_pos = make_library<BAs...>(
	WFF_SQUEEZE_POSITIVES_0
);

template<typename... BAs>
static auto unsqueeze_wff_neg = make_library<BAs...>(
	WFF_UNSQUEEZE_NEGATIVES_0
);

template<typename... BAs>
static auto squeeze_wff_neg = make_library<BAs...>(
	WFF_SQUEEZE_NEGATIVES_0
);

template<typename... BAs>
static auto eq_to_neq = make_library<BAs...>(
	WFF_PUSH_NEGATION_INWARDS_2
);

template<typename... BAs>
static auto to_cnf_wff = make_library<BAs...>(
	WFF_TO_CNF_0
	+ WFF_TO_CNF_1
	+ WFF_PUSH_NEGATION_INWARDS_0
	+ WFF_PUSH_NEGATION_INWARDS_1
	+ WFF_PUSH_NEGATION_INWARDS_2
	+ WFF_PUSH_NEGATION_INWARDS_3
	+ WFF_PUSH_NEGATION_INWARDS_4
	+ WFF_PUSH_NEGATION_INWARDS_5
	+ WFF_PUSH_ALWAYS_INWARDS
	+ WFF_PUSH_SOMETIMES_INWARDS
);

template<typename... BAs>
static auto to_cnf_bf = make_library<BAs...>(
	BF_TO_CNF_0
	+ BF_TO_CNF_1
	+ BF_PUSH_NEGATION_INWARDS_0
	+ BF_PUSH_NEGATION_INWARDS_1
);

template<typename...BAs>
static auto to_nnf_wff = make_library<BAs...>(
	WFF_PUSH_NEGATION_INWARDS_0
	+ WFF_PUSH_NEGATION_INWARDS_1
	+ WFF_PUSH_NEGATION_INWARDS_2
	+ WFF_PUSH_NEGATION_INWARDS_3
	+ WFF_PUSH_NEGATION_INWARDS_4
	+ WFF_PUSH_NEGATION_INWARDS_5
);

template<typename...BAs>
static auto to_nnf_bf = make_library<BAs...>(
	BF_PUSH_NEGATION_INWARDS_0
	+ BF_PUSH_NEGATION_INWARDS_1
);

// --------------------------------------------------------------
// General operator for nso<BAs...> function application by pipe
template<typename... BAs>
using nso_transform = nso<BAs...>(*)(const nso<BAs...>&);

template<typename... BAs>
nso<BAs...> operator| (const nso<BAs...>& fm, const nso_transform<BAs...> func) {
	return func(fm);
}
// --------------------------------------------------------------

template<typename... BAs>
struct bf_reduce_canonical;

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
					BOOST_LOG_TRIVIAL(trace) << "(I) found positive: " << l;
				} else {
					negatives.insert(l);
					BOOST_LOG_TRIVIAL(trace) << "(I) found negative: " << l;
				}
			} else {
				if (auto check = l | tau_parser::wff_neg; !check.has_value()) {
					positives.insert(l);
					BOOST_LOG_TRIVIAL(trace) << "(I) found positive: " << l;
				} else {
					negatives.insert(l);
					BOOST_LOG_TRIVIAL(trace) << "(I) found negative: " << l;
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
				auto negated = negation
					| tau_parser::bf_neg
					| tau_parser::bf
					| optional_value_extractor<sp_tau_node<BAs...>>;
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
				auto neq_bf = negation
					| tau_parser::wff_neg
					| tau_parser::wff
					| optional_value_extractor<sp_tau_node<BAs...>>;
				for (auto& positive: positives) {
					auto eq_bf = positive
						| first_child_extractor<BAs...>
						| tau_parser::bf
						| optional_value_extractor<sp_tau_node<BAs...>>;
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

// return the inner quantifier or the top wff if the formula is not quantified
template<typename...BAs>
std::pair<std::optional<nso<BAs...>>, nso<BAs...>> get_inner_quantified_wff(const nso<BAs...>& n) {
	// TODO (LOW) extract to a utils file
	auto quantified = [](const auto& n) -> bool {
		return is_non_terminal<tau_parser::wff_ex, BAs...>(n)
			|| is_non_terminal<tau_parser::wff_all, BAs...>(n)
			|| is_non_terminal<tau_parser::wff_sometimes, BAs...>(n)
			|| is_non_terminal<tau_parser::wff_always, BAs...>(n);
	};
	if (auto quantifier = find_bottom(n, quantified); quantifier) {
		return { quantifier | tau_parser::variable,
			quantifier | tau_parser::wff | optional_value_extractor<nso<BAs...>> };
	}
	return { {}, n };
}

template<typename...BAs>
struct onf_wff {

	onf_wff(const nso<BAs...>& _var) {
		if (!is_non_terminal(tau_parser::bf, _var))
			var = wrap(tau_parser::bf, _var);
		else var = _var;
	}

	nso<BAs...> operator()(const nso<BAs...>& n) const {
		auto [var, nn] = get_inner_quantified_wff(n);
		// We assume that the formula is in DNF. In particular nn is in DNF
		// For each disjunct calculate the onf
		std::map<nso<BAs...>, nso<BAs...>> changes;
		bool no_disjunction = true;
		for (const auto& disjunct : select_all(nn, is_non_terminal<tau_parser::wff_or, BAs...>)) {
			no_disjunction = false;
			assert(disjunct->child.size() == 2);
			if (!is_non_terminal(tau_parser::wff_or, trim(disjunct->child[0])))
				changes[trim(disjunct->child[0])]= onf_subformula(trim(disjunct->child[0]));
			if (!is_non_terminal(tau_parser::wff_or, trim(disjunct->child[1])))
				changes[trim(disjunct->child[1])]= onf_subformula(trim(disjunct->child[1]));
		}
		if (no_disjunction) changes[nn] = onf_subformula(nn);
		return replace(nn, changes);
	}

private:
	nso<BAs...> onf_subformula(const nso<BAs...>& n) const {
		auto has_var = [&](const auto& node){ return node == var; };

		auto eq = find_bottom(n, is_non_terminal<tau_parser::bf_eq, BAs...>);
		std::map<nso<BAs...>, nso<BAs...>> changes;
		if (eq && find_top(trim(eq.value()), has_var)) {
			auto eq_v = eq.value();
			assert(is_non_terminal(tau_parser::bf_f, trim(eq_v->child[1])));
			std::map<nso<BAs...>, nso<BAs...>> l_changes = {{var, _0<BAs...>}};
			auto f_0 = replace(trim(eq_v),  l_changes);
			f_0 = f_0 | bf_reduce_canonical<BAs...>();
			l_changes = {{var, _1<BAs...>}};
			auto f_1 = replace(build_bf_neg(trim(eq_v)),  l_changes)
				| bf_reduce_canonical<BAs...>();

			changes[eq_v] = trim(build_bf_interval(f_0, var, f_1));
		}
		for (const auto& neq: select_all(n, is_non_terminal<tau_parser::bf_neq, BAs...>)) {
			assert(is_non_terminal(tau_parser::bf_f, trim(neq->child[1])));
			if (!find_top(trim(neq), has_var)) continue;
			std::map<nso<BAs...>, nso<BAs...>> l_changes = {{var, _0<BAs...>}};
			auto f_0 = replace(trim(neq), l_changes)
				| bf_reduce_canonical<BAs...>();
			l_changes = {{var, _1<BAs...>}};
			auto f_1 = replace(build_bf_neg(trim(neq)), l_changes)
				| bf_reduce_canonical<BAs...>();
			changes[neq] = trim(build_wff_or(build_bf_nleq_upper(f_0, var), build_bf_nleq_lower(f_1, var)));
		}
		return replace(n, changes);
	}

	nso<BAs...> var;
};

template<typename... BAs>
using onf_wff_t = onf_wff<BAs...>;

template<typename...BAs>
nso<BAs...> operator|(const nso<BAs...>& n, const onf_wff_t<BAs...>& r) {
	return r(n);
}

template<typename... BAs>
static const auto is_not_eq_or_neq_to_zero_predicate = [](const nso<BAs...>& n) {
	if (!(is_non_terminal(tau_parser::bf_eq, trim(n)) || is_non_terminal(tau_parser::bf_neq, trim(n))))
		return true;
	auto check = (n | only_child_extractor<BAs...> || tau_parser::bf)[1] || tau_parser::bf_f;
	return check.empty();
};

template<typename... BAs>
using is_not_eq_or_neq_predicate_t = decltype(is_not_eq_or_neq_to_zero_predicate<BAs...>);

template<typename...BAs>
std::optional<nso<BAs...>> onf(const nso<BAs...>& n, const nso<BAs...>& var) {
	// FIXME take into account quiantifiers
	return n
		| repeat_all<step<BAs...>, BAs...>(to_dnf_wff<BAs...>)
		| onf_wff<BAs...>(var)
		| repeat_all<step<BAs...>, BAs...>(
			to_dnf_wff<BAs...>
			| simplify_wff<BAs...>);
}

template<typename...BAs>
nso<BAs...> dnf_wff(const nso<BAs...>& n) {
	auto [_, nn] = get_inner_quantified_wff(n);
	auto nform = nn
		| repeat_each<step<BAs...>, BAs...>(
			to_dnf_wff<BAs...>
			| simplify_wff<BAs...>
		);
	// finally, we also simplify the bf part of the formula
	auto dnf = dnf_bf(nform);
	std::map<nso<BAs...>, nso<BAs...>> changes = {{nn, dnf}};
	return replace(n, changes);
}

template<typename...BAs>
nso<BAs...> dnf_bf(const nso<BAs...>& n) {
	return n | repeat_all<step<BAs...>, BAs...>(
			to_dnf_bf<BAs...>
			| elim_eqs<BAs...>)
		// TODO (MEDIUM) review after we fully normalize bf & wff
		| reduce_bf<BAs...>;
}

template<typename...BAs>
nso<BAs...> cnf_wff(const nso<BAs...>& n) {
	auto [var, nn] = get_inner_quantified_wff(n);
	auto wff = nn | repeat_each<step<BAs...>, BAs...>(
			to_cnf_wff<BAs...>
			| simplify_wff<BAs...>
		);
	// finally, we also simplify the bf part of the formula
	auto cnf = cnf_bf(wff);
	std::map<nso<BAs...>, nso<BAs...>> changes = {{nn, cnf}};
	return replace(n, changes);
}

template<typename...BAs>
nso<BAs...> cnf_bf(const nso<BAs...>& n) {
	return n

		| repeat_all<step<BAs...>, BAs...>(
			to_cnf_bf<BAs...>
			| elim_eqs<BAs...>)
		// TODO (MEDIUM) review after we fully normalize bf & wff
		| reduce_bf<BAs...>;
}

template<typename...BAs>
nso<BAs...> nnf_bf(const nso<BAs...>& n) {
	return n | repeat_all<step<BAs...>, BAs...>(
			to_nnf_bf<BAs...>
			| elim_eqs<BAs...>)
		// TODO (MEDIUM) review after we fully normalize bf & wff
		| reduce_bf<BAs...>;
}

template<typename...BAs>
nso<BAs...> nnf_wff(const nso<BAs...>& n) {
	auto [_, nn] = get_inner_quantified_wff(n);
	auto nform = nn | repeat_each<step<BAs...>, BAs...>(
			to_nnf_wff<BAs...>
			| simplify_wff<BAs...>
		);
	// finally, we also simplify the bf part of the formula
	auto nnf = nnf_bf(nform);
	std::map<nso<BAs...>, nso<BAs...>> changes = {{nn, nnf}};
	return replace(n, changes);
}

// Reduce currrent dnf due to update by coeff and variable assignment i
inline bool reduce_paths (std::vector<int_t>& i,
	std::vector<std::vector<int_t>>& paths, int_t p, bool surface = true)
{
	for (size_t j = 0; j < paths.size(); ++j) {
		// Get Hamming distance between i and path and position of last difference
		// while different irrelevant variables make assignments incompatible
		int_t dist = 0, pos = 0;
		for (int_t k = 0; k < p; ++k) {
			if (i[k] == paths[j][k]) continue;
			else if (dist == 2) break;
			else if (i[k] == 2 || paths[j][k] == 2) { dist = 2; break; }
			else dist += 1, pos = k;
		}
		if (dist == 1) {
			// Remove i from paths if recursion depth is greater 0
			if(!surface) {
				paths[j] = {};
				// Resolve variable
				i[pos] = 2;
				if(std::ranges::all_of(i, [](const auto el) {return el == 2;}))
					return paths = {}, true;
				// Continue with resulting assignment
				reduce_paths(i, paths, p, false);
			} else {
				// Resolve variable
				paths[j][pos] = 2;
				if(std::ranges::all_of(paths[j], [](const auto el) {return el == 2;}))
					return paths = {}, true;
				// Continue with resulting assignment
				reduce_paths(paths[j], paths, p, false);
			}
			return true;
		}
	}
	return false;
}

inline void join_paths (std::vector<std::vector<int_t>>& paths) {
	for (int_t i = 0; i < (int_t)paths.size(); ++i) {
		for (int_t j = 0; j < (int_t)paths.size(); ++j) {
			if (i==j) continue;
			int_t dist = 0, pos = 0;
			bool subset_relation_decided = false, is_i_subset_of_j = true,
				subset_check = true, equal = true;
			for (size_t k=0; k < paths[i].size(); ++k) {
			if (paths[i][k] == paths[j][k]) continue;
			else if (dist == 2) break;
			else if (paths[i][k] == 2) {
				if (!subset_relation_decided) {
					subset_relation_decided = true;
					is_i_subset_of_j = true;
					if (paths[j][k] != 2)
						equal = false;
				} else {
					if (!is_i_subset_of_j) {
						subset_check = false;
						break;
					}
					if (paths[j][k] != 2)
						equal = false;
				}
			}
			else if (paths[j][k] == 2) {
				if (!subset_relation_decided) {
					subset_relation_decided = true;
					is_i_subset_of_j = false;
					// paths[i][k] != 2
					equal = false;
				} else {
					// paths[i][k] != 2
					equal = false;
					if (is_i_subset_of_j) {
						subset_check = false;
						break;
					}
				}
			}
			else dist += 1, pos = k;
		}
		if (subset_check && dist == 1) {
			if (is_i_subset_of_j) {
				// Resovle variable in paths
				paths[j][pos] = 2;
				if (equal) {
					paths.erase(paths.begin()+i);
					--i;
					break;
				}
			} else {
				// Resolve variable in i
				paths[i][pos] = 2;
			}
		} else if (subset_check && dist == 0) {
			// True subset relation between i and j
			if (is_i_subset_of_j) {
				// i -> j
				paths.erase(paths.begin()+j);
				if (i > j) --i;
				--j;
			} else {
				paths.erase(paths.begin()+i);
				if (j > i) --j;
				--i;
				break;
			}
		}
		}
	}
}

// Ordering function for variables from nso formula
template<typename... BAs>
auto lex_var_comp = [](const auto x, const auto y) {
#ifdef CACHE
	static std::map<std::pair<nso<BAs...>, nso<BAs...>>, bool> cache;
	if (auto it = cache.find({x,y}); it != cache.end())
		return it->second;
#endif // CACHE
	auto xx = make_string(tau_node_terminal_extractor<BAs...>, x);
	auto yy = make_string(tau_node_terminal_extractor<BAs...>, y);
#ifdef CACHE
	pair<nso<BAs...>,nso<BAs...>> p (x,y);
	return cache.emplace(move(p), xx < yy).first->second;
#endif // CACHE
	return xx < yy;
};

// In conversions to bdd, the following atomic formulas and terms are treated
// as variables
inline auto is_wff_bdd_var = [](const auto& n) {
		using tp = tau_parser;
		assert(!is_non_terminal(tp::bf_neq, n));
		return is_child_non_terminal(tp::bf_eq, n)
			|| is_child_non_terminal(tp::wff_ref, n)
			|| is_child_non_terminal(tp::wff_ex, n)
			|| is_child_non_terminal(tp::wff_sometimes, n)
			|| is_child_non_terminal(tp::wff_always, n)
			|| is_child_non_terminal(tp::wff_all, n)
			|| is_child_non_terminal(tp::constraint, n);
};

inline auto is_bf_bdd_var = [](const auto& n) {
		using tp = tau_parser;
		return is_child_non_terminal(tp::variable, n) ||
				is_child_non_terminal(tp::capture, n) ||
				is_child_non_terminal(tp::bf_ref, n) ||
				is_child_non_terminal(tp::bf_constant, n) ||
				is_child_non_terminal(tp::uninterpreted_constant, n);
};
// ------------------------------

// Starting from variable at position p+1 in vars write to i which variables are irrelevant in assignment
template<typename... BAs>
void elim_vars_in_assignment (const auto& fm, const auto& vars, auto& i,
	const int_t p, const auto& is_var)
{
	// auto is_var = [](const nso<BAs...>& n){return
	// 	is_child_non_terminal(tau_parser::variable, n) ||
	// 		is_child_non_terminal(tau_parser::uninterpreted_constant, n);};
	auto cvars = select_all(fm, is_var);
	std::set<nso<BAs...>> cur_vars(std::make_move_iterator(cvars.begin()),
              std::make_move_iterator(cvars.end()));

	for (size_t v_iter = p+1; v_iter < vars.size(); ++v_iter) {
		if (!cur_vars.contains(vars[v_iter]))
			i[v_iter] = 2;
	}
}

// Declaration of functions used in assign_and_reduce which are implemented later
template<typename... BAs>
nso<BAs...> reduce2(const nso<BAs...>& fm, size_t type, bool is_cnf = false, bool all_reductions = true, bool enable_sort = true);
template<typename... BAs>
nso<BAs...> to_dnf2(const nso<BAs...>&, bool is_wff = true);
template<typename... BAs>
nso<BAs...> to_cnf2(const nso<BAs...>&, bool is_wff = true);
template<typename... BAs>
nso<BAs...> push_negation_in(const nso<BAs...>&, bool is_wff = true);

// Create assignment in formula and reduce resulting clause
template<typename... BAs>
bool assign_and_reduce(const nso<BAs...>& fm,
	const std::vector<nso<BAs...>>& vars,
	std::vector<int_t>& i, auto& dnf, const auto& is_var, int_t p = 0,
	bool is_wff = false)
{
	// Check if all variables are assigned
	if((int_t)vars.size() == p) {
		nso<BAs...> fm_simp;
		if (!is_wff) {
			// fm is a Boolean function
			// Normalize tau subformulas
			fm_simp = fm | (nso_transform<BAs...>)normalize_ba<BAs...>;
			fm_simp = to_dnf2(fm_simp, false);
			fm_simp = reduce2(fm_simp, tau_parser::bf);

			// Do not add to dnf if the coefficient is 0
			if (is_non_terminal(tau_parser::bf_f, fm_simp->child[0]))
				return false;
		} else {
			// fm is a Tau formula
			fm_simp = to_dnf2(fm);
			fm_simp = reduce2(fm_simp, tau_parser::wff);
			if (is_child_non_terminal(tau_parser::wff_f, fm_simp))
				return false;
		}
		if(std::ranges::all_of(i, [](const auto el) {return el == 2;})) {
			//bool t = is_non_terminal(tau_parser::bf_t, fm->child[0]);
			return dnf.emplace(fm_simp, std::vector(0, i)), true;
		}

		auto it = dnf.find(fm_simp);
		if (it == dnf.end()) return dnf.emplace(fm_simp, std::vector(p==0?0:1, i)), false;
		else if (!reduce_paths(i, it->second, p)) {
			// Place coefficient together with variable assignment if no reduction happend
			it->second.push_back(i);
		} else erase_if(it->second, [](const auto& v){return v.empty();});
		return it->second.empty();
	}
	// variable was already eliminated
	if (i[p] == 2) {
		if (assign_and_reduce(fm, vars, i, dnf, is_var, p+1, is_wff)) return true;
		i[p] = 0;
		return false;
	}
	// Substitute 1 and 0 for v and simplify
	const auto& v = vars[p];
	auto t = is_wff ? _T<BAs...> : _1<BAs...>;
	auto f = is_wff ? _F<BAs...> : _0<BAs...>;
	std::map<nso<BAs...>, nso<BAs...>> c = {{v, t}};
	auto fm_v1 = replace(fm, c);
	c = {{v, f}};
	auto fm_v0 = replace(fm, c);

	elim_vars_in_assignment<BAs...>(fm_v1, vars, i, p, is_var);
	if(fm_v1 == fm_v0) {
		i[p] = 2;
		if (assign_and_reduce(fm_v1, vars, i, dnf, is_var, p+1, is_wff)) return true;
		i[p] = 0;
	} else {
		i[p] = 1;
		if (assign_and_reduce(fm_v1, vars, i, dnf, is_var, p+1, is_wff)) return true;
		i[p] = 0;

		elim_vars_in_assignment<BAs...>(fm_v0, vars, i, p, is_var);
		i[p] = -1;
		if (assign_and_reduce(fm_v0, vars, i, dnf, is_var, p+1, is_wff)) return true;
		i[p] = 0;
	}
	return false;
}

// Given a BF b, calculate the Boole normal form (DNF corresponding to the pathes to true in the BDD) of b
// where the variable order is given by the function lex_var_comp
template<typename... BAs>
nso<BAs...> bf_boole_normal_form (const nso<BAs...>& fm,
	bool make_paths_disjoint = false)
{
	// Function can only be applied to a BF
	assert(is_non_terminal(tau_parser::bf, fm));
#ifdef TAU_CACHE
	static std::map<std::pair<nso<BAs...>, bool>, nso<BAs...>> cache;
    if (auto it = cache.find(make_pair(fm, make_paths_disjoint));
    		it != cache.end()) return it->second;
#endif //TAU_CACHE
	// This defines the variable order used to calculate DNF
	// It is made canonical by sorting the variables
	auto is_var = [](const nso<BAs...>& n){return
		is_child_non_terminal(tau_parser::variable, n) ||
			is_child_non_terminal(tau_parser::uninterpreted_constant, n);};
	auto vars = select_top(fm, is_var);
	sort(vars.begin(), vars.end(), lex_var_comp<BAs...>);

	std::vector<int_t> i(vars.size()); // Record assignments of vars

	// Resulting DNF - make it ordered for consistency
	// Key is coefficient, value is possible variable assignments for coefficient
	std::map<nso<BAs...>, std::vector<std::vector<int_t>>> dnf;

	if(assign_and_reduce(fm, vars, i, dnf, is_var, 0)) {
		assert(dnf.size() == 1);
		return dnf.begin()->first;
	}
	if(dnf.empty()) return _0<BAs...>;
	if (!make_paths_disjoint)
		for (auto& [coeff, paths] : dnf)
			join_paths(paths);

	// Convert map structure dnf back to rewrite tree
	nso<BAs...> reduced_dnf;
	bool first = true;
	for (const auto& [coeff, paths] : dnf) {
		bool t = is_non_terminal(tau_parser::bf_t, coeff->child[0]);
		if (paths.empty()) {
			assert(!t);
			if (first) reduced_dnf = coeff;
			else reduced_dnf = build_bf_or(reduced_dnf, coeff);
			continue;
		}
		for (const auto& path : paths) {
			bool first_var = true;
			nso<BAs...> var_path;
			for (size_t k=0; k < vars.size(); ++k) {
				if (path[k] == 2) continue;
				if (first_var) var_path = path[k] == 1 ? vars[k] :
					build_bf_neg(vars[k]), first_var = false;
				else var_path = path[k] == 1 ? build_bf_and(var_path, vars[k]) :
					build_bf_and(var_path, build_bf_neg(vars[k]));
			}
			if (first) reduced_dnf = t ? var_path : build_bf_and(coeff, var_path), first = false;
			else reduced_dnf = t ? build_bf_or(reduced_dnf, var_path) :
				build_bf_or(reduced_dnf, build_bf_and(coeff, var_path));
		}
	}
#ifdef TAU_CACHE
		cache.emplace(make_pair(fm, make_paths_disjoint), reduced_dnf);
		cache.emplace(make_pair(reduced_dnf, make_paths_disjoint), reduced_dnf);
#endif //TAU_CACHE
	return reduced_dnf;
}

// The needed class in order to make bf_to_reduce_dnf work with rule applying process
template<typename... BAs>
struct bf_reduce_canonical {
	nso<BAs...> operator() (const nso<BAs...>& fm) const {
		std::map<nso<BAs...>, nso<BAs...>> changes = {};
		for (const auto& bf: select_top(fm, is_non_terminal<tau_parser::bf, BAs...>)) {
			auto dnf = bf_boole_normal_form(bf);
			if (dnf != bf) changes[bf] = dnf;
		}
		if (changes.empty()) return fm;
		else return replace(fm, changes);
	}
};

template<typename... BAs>
nso<BAs...> operator|(const nso<BAs...>& fm, const bf_reduce_canonical<BAs...>& r) {
	return r(fm);
}

template<typename... BAs>
std::optional<nso<BAs...>> operator|(const std::optional<nso<BAs...>>& fm, const bf_reduce_canonical<BAs...>& r) {
	return fm.has_value() ? r(fm.value()) : std::optional<nso<BAs...>>{};
}

inline bool is_contained_in (const std::vector<int_t>& i, auto& paths) {
	// Check if there is a containment of i in any path of paths
	for (auto& path : paths) {
		bool is_contained = true, is_i_smaller, containment_dir_known = false;
		for (int_t k = 0; k < (int_t)i.size(); ++k) {
			if (i[k] == path[k]) continue;
			else if (i[k] == 2) {
				if (containment_dir_known) {
					if (!is_i_smaller) { is_contained = false; break; }
				} else {
					containment_dir_known = true;
					is_i_smaller = true;
				}
			}
			else if (path[k] == 2) {
				if (containment_dir_known) {
					if (is_i_smaller) { is_contained = false; break; }
				} else {
					containment_dir_known = true;
					is_i_smaller = false;
				}
			}
			else if (i[k] != path[k]) { is_contained = false; break; }
		}
		if (is_contained) {
			if (is_i_smaller) {
				// keep i and delete current path
				path = {};
			} else return true;
		}
	}
	return false;
}

template<typename... BAs>
std::pair<std::vector<int_t>, bool> clause_to_vector(const nso<BAs...>& clause,
	const auto& var_pos, const bool wff, const bool is_cnf)
{
	using tp = tau_parser;
	std::vector<int_t> i (var_pos.size());
		for (size_t k = 0; k < var_pos.size(); ++k) i[k] = 2;
		bool clause_is_decided = false;
		auto var_assigner = [&](const nso<BAs...>& n) {
			if (clause_is_decided) return false;
			if (!is_cnf && is_non_terminal(wff ? tp::wff_f : tp::bf_f, n)) {
				clause_is_decided = true;
				return false;
			}
			if (is_cnf && is_non_terminal(wff ? tp::wff_t : tp::bf_t, n)) {
				clause_is_decided = true;
				return false;
			}
			if (is_non_terminal(wff ? tp::wff_neg : tp::bf_neg, n)) {
				auto v = trim(n);
				// Check if v is a T/F or 1/0
                if (v == _T<BAs...> || v == _1<BAs...>) {
                    if (!is_cnf) clause_is_decided = true;
                    return false;
                } else if (v == _F<BAs...> || v == _0<BAs...>) {
                    if (is_cnf) clause_is_decided = true;
                    return false;
                }
				auto it = var_pos.find(v);
				assert(it != var_pos.end());
				if (i[it->second] == 1) {
					// clause is false for DNF, true for CNF
					clause_is_decided = true;
					return false;
				}
				i[it->second] = -1;
				return false;
			}
			if (auto it = var_pos.find(n); it != var_pos.end()) {
				if (i[it->second] == -1) {
					// clause is false for DNF, true for CNF
					clause_is_decided = true;
					return false;
				}
				i[it->second] = 1;
				return false;
			}
			else return true;
		};
		rewriter::post_order_traverser<rewriter::identity_t,
			decltype(var_assigner), nso<BAs...>>(
				rewriter::identity, var_assigner)(clause);
	return std::make_pair(move(i), clause_is_decided);
}

template<typename... BAs>
std::vector<std::vector<int_t>> collect_paths(const nso<BAs...>& new_fm, bool wff,
	const auto& vars, bool& decided, bool is_cnf, bool all_reductions = true) {
	std::vector<std::vector<int_t>> paths;
	std::map<nso<BAs...>, int_t> var_pos;
	for (int_t k=0; k < (int_t)vars.size(); ++k)
		var_pos.emplace(vars[k], k);
	using tp = tau_parser;
	for (const auto& clause : get_leaves(new_fm, is_cnf ? (wff ? tp::wff_and : tp::bf_and) :
							(wff ? tp::wff_or : tp::bf_or), wff ? tp::wff : tp::bf)) {
		auto [i, clause_is_decided] = clause_to_vector(clause, var_pos, wff, is_cnf);
		if (clause_is_decided) continue;
		// There is at least one satisfiable clause
		decided = false;
		if (std::ranges::all_of(i, [](const auto el) {return el == 2;})) return {};
		if (all_reductions) {
			if (!reduce_paths(i, paths, vars.size())) paths.emplace_back(move(i));
			else {
				erase_if(paths, [](const auto& v){return v.empty();});
				if (paths.empty()) return {};
			}
		} else paths.emplace_back(move(i));
	}
	return paths;
}

template<typename... BAs>
nso<BAs...> build_reduced_formula (const auto& paths, const auto& vars, bool is_cnf, bool wff) {
	if (paths.empty()) return is_cnf
					  ? (wff ? _T<BAs...> : _1<BAs...>)
					  : (wff ? _F<BAs...> : _0<BAs...>);
	nso<BAs...> reduced_fm = is_cnf
					 ? (wff ? _F<BAs...> : _0<BAs...>)
					 : (wff ? _T<BAs...> : _1<BAs...>);
	bool first = true;
	for (const auto& path : paths) {
		if (path.empty()) continue;
        bool first_var = true;
		nso<BAs...> var_path = is_cnf
					       ? (wff ? _F<BAs...> : _0<BAs...>)
					       : (wff ? _T<BAs...> : _1<BAs...>);
        for (size_t k=0; k < vars.size(); ++k) {
        	assert(path.size() == vars.size());
            if (path[k] == 2) continue;
            if (first_var) var_path = path[k] == 1 ? vars[k] :
                wff ? build_wff_neg(vars[k]) : build_bf_neg(vars[k]), first_var = false;
            else {
                if (!is_cnf) {
	                if (wff)
	                	var_path = path[k] == 1 ? build_wff_and(var_path, vars[k]) :
						build_wff_and(var_path, build_wff_neg(vars[k]));
                	else var_path = path[k] == 1 ? build_bf_and(var_path, vars[k]) :
                		build_bf_and(var_path, build_bf_neg(vars[k]));
                }
                else {
                	if (wff)
                        var_path = path[k] == 1 ? build_wff_or(var_path, vars[k]) :
                        build_wff_or(var_path, build_wff_neg(vars[k]));
                	else var_path = path[k] == 1 ? build_bf_or(var_path, vars[k]) :
                		build_bf_or(var_path, build_bf_neg(vars[k]));
                }
            }
        }
        if (first) reduced_fm = var_path, first = false;
        else reduced_fm = is_cnf ? ( wff ? build_wff_and(reduced_fm, var_path) : build_bf_and(reduced_fm, var_path))
                            : ( wff ? build_wff_or(reduced_fm, var_path) : build_bf_or(reduced_fm, var_path));
	}
	assert(reduced_fm != nullptr);
	return reduced_fm | repeat_all<step<BAs...>, BAs...>(eq_to_neq<BAs...>);
}

template<typename... BAs>
std::pair<std::vector<std::vector<int_t>>, std::vector<nso<BAs...>>> dnf_cnf_to_bdd(
	const nso<BAs...>& fm, size_t type, bool is_cnf = false,
	bool all_reductions = true, bool enable_sort = true)
{
	assert(is_non_terminal(type, fm));
	// Pull negation out of equality
	bool wff = type == tau_parser::wff;
	auto new_fm = wff ? fm | repeat_all<step<BAs...>, BAs...>(to_mnf_wff<BAs...>) : fm;
	std::vector<nso<BAs...> > vars = wff
					    ? select_top(new_fm, is_wff_bdd_var)
					    : select_top(new_fm, is_bf_bdd_var);
	if (vars.empty()) {
		assert(new_fm == _T<BAs...> || new_fm == _F<BAs...> ||
			new_fm == _1<BAs...> || new_fm == _0<BAs...>);
		if (new_fm == _T<BAs...> || new_fm == _1<BAs...>) {
			if (is_cnf) return {};
			std::vector<std::vector<int_t>> paths;
			paths.emplace_back();
			return make_pair(move(paths), move(vars));
		} else {
			if (is_cnf) {
				std::vector<std::vector<int_t>> paths;
				paths.emplace_back();
				return make_pair(move(paths), move(vars));
			}
			return {};
		}
	}

	// Sort bf variables canonically
	if (!wff && enable_sort) std::ranges::sort(vars, lex_var_comp<BAs...>);

	bool decided = true;
	auto paths = collect_paths(new_fm, wff, vars, decided, is_cnf,
				   all_reductions);

	if (all_reductions) join_paths(paths);
	if (paths.empty())
		if (!decided)
			paths.emplace_back();
	std::ranges::sort(paths);
	return make_pair(move(paths), move(vars));
}

// Assume that fm is in DNF (or CNF -> set is_cnf to true)
// TODO: Normalize Tau constants in case type == bf
template<typename... BAs>
nso<BAs...> reduce2(const nso<BAs...>& fm, size_t type, bool is_cnf, bool all_reductions, bool enable_sort) {
#ifdef TAU_CACHE
		static std::map<std::pair<nso<BAs...>, bool>, nso<BAs...>> cache;
		if (auto it = cache.find(make_pair(fm, all_reductions)); it != end(cache))
			return it->second;
#endif // TAU_CACHE
	BOOST_LOG_TRIVIAL(trace) << "(I) Begin reduce2 with is_cnf set to " << is_cnf;
	BOOST_LOG_TRIVIAL(trace) << "(F) Formula to reduce: " << fm;
	auto [paths, vars] = dnf_cnf_to_bdd(fm, type, is_cnf, all_reductions, enable_sort);

    bool wff = type == tau_parser::wff;
	if (paths.empty()) {
		auto res = is_cnf ? (wff ? _T<BAs...> : _1<BAs...>) : (wff ? _F<BAs...> : _0<BAs...>);
#ifdef TAU_CACHE
		return cache.emplace(make_pair(fm, all_reductions), res).first->second;
#endif // TAU_CACHE
		return res;
	}
	if (paths.size() == 1 && paths[0].empty()) {
		auto res = is_cnf ? (wff ? _F<BAs...> : _0<BAs...>) : (wff ? _T<BAs...> : _1<BAs...>);
#ifdef TAU_CACHE
		return cache.emplace(make_pair(fm, all_reductions), res).first->second;
#endif // TAU_CACHE
		return res;
	}

	auto reduced_fm = build_reduced_formula<BAs...>(paths, vars, is_cnf, wff);
#ifdef TAU_CACHE
		return cache.emplace(make_pair(fm, all_reductions), reduced_fm).first->second;
#endif // TAU_CACHE
	BOOST_LOG_TRIVIAL(trace) << "(I) End reduce2";
	BOOST_LOG_TRIVIAL(trace) << "(F) Reduced formula: " << reduced_fm;
	return reduced_fm;
}

template<typename... BAs>
nso<BAs...> reduce_terms (const nso<BAs...>& fm) {
    std::map<nso<BAs...>, nso<BAs...>> changes = {};
    for (const auto& bf: select_top(fm, is_non_terminal<tau_parser::bf, BAs...>)) {
        auto dnf = to_dnf2(bf, false);
        dnf = reduce2(dnf, tau_parser::bf);
        if (dnf != bf) changes[bf] = dnf;
    }
    if (changes.empty()) return fm;
    else return replace(fm, changes);
}

template<typename... BAs>
nso<BAs...> group_dnf_expression (const nso<BAs...>& fm) {
#ifdef TAU_CACHE
		static std::map<nso<BAs...>, nso<BAs...>> cache;
		if (auto it = cache.find(fm); it != end(cache))
			return it->second;
#endif // TAU_CACHE
	BOOST_LOG_TRIVIAL(debug) << "(I) Begin group_dnf_expression";
	BOOST_LOG_TRIVIAL(debug) << "(F) Expression to factor: " << fm;
	auto count_common = [](const auto& v1, const auto& v2) {
		int_t count = 0;
		auto it1 = begin(v1);
		auto it2 = begin(v2);
		while (it1 != end(v1) && it2 != end(v2)) {
            if (*it1 < *it2) ++it1;
            else { if (!(*it2 < *it1)) {++count;++it1;}
                ++it2; } }
		return count;
	};

	bool wff = is_non_terminal(tau_parser::wff, fm);
	auto clauses = wff ? get_dnf_wff_clauses(fm)
			       : get_dnf_bf_clauses(fm);
	if (clauses.size() < 2) {
#ifdef TAU_CACHE
		return cache.emplace(fm, fm).first->second;
#endif // TAU_CACHE
		return fm;
	}

	std::vector<std::vector<nso<BAs...>>> atoms_of_clauses;
	for (const auto& clause : clauses) {
		auto atoms = wff ? get_cnf_wff_clauses(clause)
				     : get_cnf_bf_clauses(clause);
		if(wff) std::ranges::sort(atoms);
		else std::ranges::sort(atoms, lex_var_comp<BAs...>);
		atoms_of_clauses.emplace_back(move(atoms));
	}
	nso<BAs...> grouped_fm = wff ? _F<BAs...> : _0<BAs...>;
	for (int_t i = 0; i < (int_t)atoms_of_clauses.size(); ++i) {
		std::pair max_common = {0,0};
		for (size_t j = i+1; j < atoms_of_clauses.size(); ++j) {
			int_t count = count_common(atoms_of_clauses[i], atoms_of_clauses[j]);
			if (count > max_common.second) {
				max_common.first = j;
				max_common.second = count;
			}
		}
		if (max_common.first == 0) {
			auto atoms = wff ? build_wff_and<BAs...>(
						     atoms_of_clauses[i])
					     : build_bf_and<BAs...>(
						     atoms_of_clauses[i]);
			grouped_fm = wff ? build_wff_or(grouped_fm, atoms)
					     : build_bf_or(grouped_fm, atoms);
			continue;
		}
		std::vector<nso<BAs...>> common;
		std::ranges::set_intersection(atoms_of_clauses[i],
					 atoms_of_clauses[max_common.first],
					 back_inserter(common));

		nso<BAs...> cl1 = wff ? _T<BAs...> : _1<BAs...>;
		nso<BAs...> cl2 = wff ? _T<BAs...> : _1<BAs...>;
		size_t p = 0;
		for (const auto& atom : atoms_of_clauses[i]) {
			if (p < common.size() && common[p] == atom) ++p;
			else cl1 = wff ? build_wff_and(cl1, atom)
					   : build_bf_and(cl1, atom);
		}
		p = 0;
		for (const auto& atom : atoms_of_clauses[max_common.first]) {
			if (p < common.size() && common[p] == atom) ++p;
			else cl2 = wff ? build_wff_and(cl2, atom)
					   : build_bf_and(cl2, atom);
		}
		// We need the canonical order for the reduction in "group_paths_and_simplify"
		if (!lex_var_comp<BAs...>(cl1, cl2)) std::swap(cl1, cl2);
		nso<BAs...> grouped = wff ? build_wff_or(cl1, cl2) :
			build_bf_or<BAs...>(get_dnf_bf_clauses(
				to_dnf2(build_bf_or(cl1, cl2), false)));

		common.emplace_back(move(grouped));
		atoms_of_clauses[i] = move(common);
		atoms_of_clauses.erase(atoms_of_clauses.begin() + max_common.first);
		--i;
	}
	assert(grouped_fm != nullptr);
#ifdef TAU_CACHE
		cache.emplace(grouped_fm, grouped_fm);
		return cache.emplace(fm, grouped_fm).first->second;
#endif // TAU_CACHE
	BOOST_LOG_TRIVIAL(debug) << "(I) End group_dnf_expression";
	BOOST_LOG_TRIVIAL(debug) << "(F) Factored expression: " << grouped_fm;
	return grouped_fm;
}

template<typename... BAs>
std::vector<nso<BAs...>> push_eq_and_get_vars (nso<BAs...>& fm) {
 // First push in equalities all the way (bf != 0 is converted to !(bf = 0))
	fm = fm | repeat_all<step<BAs...>, BAs...>(
		unsqueeze_wff<BAs...> | to_mnf_wff<BAs...>);
	// Find atomic formulas
	return select_top(fm, is_wff_bdd_var);
}

// Requires fm to have Boolean functions in DNF and equalities pushed in
// vars contains variable order
template<typename... BAs>
std::vector<std::vector<int_t>> wff_to_bdd (const nso<BAs...>& fm, auto& vars = {}) {
	// Find atomic formulas
	auto pushed_in_fm = fm;
	if (vars.empty()) vars = push_eq_and_get_vars(pushed_in_fm);
	else pushed_in_fm = fm | repeat_all<step<BAs...>, BAs...>(
		 	unsqueeze_wff<BAs...> | to_mnf_wff<BAs...>);
	std::vector<int_t> i (vars.size());
	std::map<nso<BAs...>, std::vector<std::vector<int_t>>> dnf;
	// dnf empty means false and and size 1 with empty paths means true
	assign_and_reduce(pushed_in_fm, vars, i, dnf, is_wff_bdd_var, 0, true);
	assert((dnf.size() == 1 || dnf.empty()));
	if (dnf.empty()) return {};
	join_paths(dnf.begin()->second);

	if (dnf.begin()->second.empty())
		dnf.begin()->second.emplace_back();
	return dnf.begin()->second;
}

bool is_ordered_subset (const auto& v1, const auto& v2) {
	if (v1.size() > v2.size()) return false;
	if (v1.size() == 0) return true;
	size_t j = 0;
	for (size_t i=0; i < v2.size(); ++i) {
		if (v1[j] == v2[i]) ++j;
		if (j == v1.size()) return true;
	}
	return false;
}

template<typename... BAs>
std::vector<std::vector<std::vector<nso<BAs...>>>> get_cnf_inequality_lits(
	const nso<BAs...>& fm)
{
	if (fm == _T<BAs...>) return {};
	auto neq_pushed_in = fm | repeat_all<step<BAs...>, BAs...>(
		 	unsqueeze_wff_neg<BAs...>);
	// cout << "neq_pushed_in: " << neq_pushed_in << "\n";
	std::vector<std::vector<std::vector<nso<BAs...>>>> cnf_lits;
	for (const nso<BAs...>& clause : get_cnf_wff_clauses(neq_pushed_in)) {
		std::vector<std::vector<nso<BAs...>>> c;
		for (const nso<BAs...>& neqs : get_dnf_wff_clauses(clause)) {
			c.emplace_back(get_cnf_bf_clauses(trim2(neqs)));
		}
		cnf_lits.emplace_back(move(c));
	}
	return cnf_lits;
}

template<typename... BAs>
std::pair<std::vector<int_t>, bool> simplify_path(
	const std::vector<int_t>& path, std::vector<nso<BAs...> >& vars)
{
	// cout << "Vars: ";
	// for (const auto& var : vars) {
	// 	cout << var << ", ";
	// }
	// cout << "\n";
	using tp = tau_parser;
	std::vector<std::vector<nso<BAs...>>> pos;
	// Build clause for non-equality terms
	// and new representation for equality terms
	nso<BAs...> clause = _T<BAs...>;
	nso<BAs...> pos_bf = _0<BAs...>;
	nso<BAs...> negs_wff = _T<BAs...>;
	for (size_t p = 0; p < path.size(); ++p) {
		if (path[p]== 2) continue;
		if (is_child_non_terminal(tp::bf_eq, vars[p])) {
			if (path[p] == -1) {
				negs_wff = build_wff_and(negs_wff, build_wff_neq(trim2(vars[p])));
			} else pos_bf = build_bf_or(pos_bf, trim2(vars[p]));
		}
		else if (path[p] == -1)
			clause = build_wff_and(clause, build_wff_neg(vars[p]));
		else if (path[p] == 1)
			clause = build_wff_and(clause, vars[p]);
		else assert(false);
	}
	// Simplify equalities/inequalities
	// Here new variables can be created
	pos_bf = reduce2(pos_bf, tau_parser::bf, false, true, false);
	nso<BAs...> new_pos_bf;
	// negs_wff = reduce2(negs_wff, tau_parser::wff);
	for (const auto& c : get_dnf_bf_clauses(pos_bf)) {
		pos.emplace_back(get_cnf_bf_clauses(c));
		if (new_pos_bf) new_pos_bf = build_wff_and(new_pos_bf, build_wff_eq(c));
		else new_pos_bf = build_wff_eq(c);
	}
	clause = build_wff_and(clause, new_pos_bf);

	// cout << "Pos clause: " << clause << "\n";
	auto cnf_neq_lits = get_cnf_inequality_lits(negs_wff);
	// Check if any term in pos is subset of any term in negs
	for (const auto& p : pos) {
		for (auto& neq_clause : cnf_neq_lits){
			for (auto& n : neq_clause) {
				// Simplify cases p' = 0&& p != 0 and p = 0 && p' != 0
				if (p.size() == 1 && n.size() == 1) {
					if (is_child_non_terminal(tp::bf_neg, p[0])) {
						if (trim2(p[0]) == n[0]) { n[0] = _1<BAs...>; continue; }
					} else if (is_child_non_terminal(tp::bf_neg, n[0])) {
						if (p[0] == trim2(n[0])) { n[0] = _1<BAs...>; continue; }
					}
				}
				// Inequality n is unsat if p is subset
				if (is_ordered_subset(p, n)) {
					n = {};
				}
			}
			// If all inequalities of the cnf clause are unsat, this path is unsat
			if (std::ranges::all_of(neq_clause, [](const auto& el){return el.empty();})) {
				return {{}, true};
			}
		}
	}
	// Eliminate redundant != 0 in cnf
	for (int_t c1=0; c1 < (int_t)cnf_neq_lits.size(); ++c1) {
		for (size_t i = 0; i < cnf_neq_lits[c1].size(); ++i) {
			if (cnf_neq_lits[c1][i].empty()) continue;
			bool all_subset = true;
			for (int_t c2 = 0; c2 < (int_t)cnf_neq_lits.size(); ++c2) {
				if (c1 == c2) continue;
				for (size_t j = 0; j < cnf_neq_lits[c2].size(); ++j) {
					if (cnf_neq_lits[c2][j].empty()) continue;
					if (!is_ordered_subset(cnf_neq_lits[c1][i], cnf_neq_lits[c2][j])) {
						all_subset = false;
						break;
					}
				}
				if (all_subset == true) {
					cnf_neq_lits[c1].erase(cnf_neq_lits[c1].begin()+i);
					--i;
					break;
				}
			}
		}
	}

	nso<BAs...> neq_cnf;
	for (const auto& neq_clause : cnf_neq_lits) {
		if (neq_clause.empty()) continue;
		nso<BAs...> neqs;
        for (const auto& n : neq_clause) {
        	if (n.empty()) continue;
	        if(neqs) neqs = build_wff_or(neqs, build_wff_neq(build_bf_and<BAs...>(n)));
        	else neqs = build_wff_neq(build_bf_and<BAs...>(n));
        }
		if (neqs) {
			if(neq_cnf) neq_cnf = build_wff_and(neq_cnf, neqs);
			else neq_cnf = neqs;
		}
	}
	if (neq_cnf) {
		// Convert back to cnf and push inequalities back out
		neq_cnf = reduce2(neq_cnf, tau_parser::wff, true);
		neq_cnf = neq_cnf | repeat_all<step<BAs...>, BAs...>(
				 squeeze_wff_neg<BAs...> | to_mnf_wff<BAs...>);
		// cout << "neq_simplified: " << neq_cnf << "\n";
		clause = build_wff_and(clause, neq_cnf);
	}
	// cout << "Build clause: " << clause << "\n";
	auto new_vars = select_top(clause, is_wff_bdd_var);

	std::map<nso<BAs...>, size_t> var_to_idx;
	for (size_t i=0; i<vars.size(); ++i)
		var_to_idx.emplace(vars[i], i);

	for (const auto& v : new_vars) {
		if (auto it = var_to_idx.find(v); it == end(var_to_idx)) {
			// There is a new variable
			assert(v != build_wff_eq(_T<BAs...>) && v != build_wff_eq(_F<BAs...>));
			vars.push_back(v);
			var_to_idx.emplace(v, vars.size() - 1);
		}
	}
	return clause_to_vector(clause, var_to_idx, true, false);
}

template<typename...BAs>
std::pair<nso<BAs...>, bool> group_paths_and_simplify(
	std::vector<std::vector<int_t> >& paths, const auto& vars)
{
	BOOST_LOG_TRIVIAL(debug) << "(I) Start group_paths_and_simplify";
	using tp = tau_parser;
	if (paths.empty()) return {_F<BAs...>, false};
	if (paths.size() == 1 && paths[0].empty()) return {_T<BAs...>, false};
	auto is_neq = [&](const int_t path, const int_t idx) {
		if (is_child_non_terminal(tp::bf_eq, vars[idx]))
			return paths[path][idx] != 1;
		return false;
	};

	// Group those paths together which have the same non != part
	std::vector<std::vector<std::vector<int_t>>> groups (paths.size());
	bool is_simp = false;
	for (size_t i = 0; i < paths.size(); ++i) {
		for (size_t j = i+1; j < paths.size(); ++j) {
			// Check if paths agree on non != vars
			bool is_equal = true;
			for (size_t k = 0; k < paths[i].size(); ++k) {
				if (is_neq(i, k) && is_neq(j,k)) continue;
				if (paths[i][k] != paths[j][k]) {
					is_equal = false;
					break;
				}
			}
			// Here paths agree
			if (is_equal) {
				is_simp = true;
				groups[i].emplace_back(move(paths[j]));
				paths.erase(paths.begin()+j);
				--j;
			}
		}
	}

	// paths[0] is connected to all in groups[0]
	auto neq_from_path = [&](const auto& path) {
		nso<BAs...> clause = _T<BAs...>;
		for (size_t k = 0; k < path.size(); ++k) {
			if (path[k] == -1 && is_child_non_terminal(tp::bf_eq, vars[k]))
				clause = build_wff_and(clause, build_wff_neq(trim2(vars[k])));
		}
		return clause;
	};
	nso<BAs...> result = _F<BAs...>;
	for (size_t i = 0; i < paths.size(); ++i) {
		// Now add from paths[i] and groups[i]
		nso<BAs...> neqs = neq_from_path(paths[i]);
		for (const auto& path : groups[i])
			neqs = build_wff_or(neqs, neq_from_path(path));
		if (!groups[i].empty()) {
			// simp to cnf
			neqs = to_cnf2(neqs, true);
			neqs = reduce2(neqs, tau_parser::wff, true);
			// push != out
			neqs = neqs | repeat_all<step<BAs...>, BAs...>(
			squeeze_wff_neg<BAs...>);
		}
		auto neq_clauses = get_cnf_wff_clauses(neqs);
		// Simplify bfs and drop !=
		bool clause_false = false;
		for (auto& neq : neq_clauses) {
			if (neq == _T<BAs...>) continue;
			neq = reduce2(trim2(neq), tau_parser::bf);
			if (neq == _0<BAs...>) clause_false = true;
		}
		if (clause_false) continue;
		// Simplify neqs with assumptions from = part
		nso<BAs...> rest = _T<BAs...>;
		for (size_t k = 0; k < paths[i].size(); ++k) {
			if (paths[i][k] == 1) {
				if (is_child_non_terminal(tp::bf_eq, vars[k])) {
					for (const auto& clause : get_dnf_bf_clauses(trim2(vars[k]))) {
						// Here single assumption
						auto neg_eq = push_negation_in(build_bf_neg(clause), false);
						std::map<nso<BAs...>, nso<BAs...>> changes;
						changes.emplace(neg_eq, _1<BAs...>);
						for (auto& neq : neq_clauses) {
							if (neq == _T<BAs...> || neq == _1<BAs...>) continue;
							auto grouped_bf = group_dnf_expression(neq);
							auto simp_neq = replace(grouped_bf, changes);
							if (grouped_bf != simp_neq) {
								neq = to_dnf2(simp_neq, false);
								neq = reduce2(neq, tau_parser::bf);
								is_simp = true;
							}
						}
					}
				}
				rest = build_wff_and(rest, vars[k]);
			} else if (paths[i][k] == -1 &&
						!is_child_non_terminal(tp::bf_eq, vars[k]))
				rest = build_wff_and(rest, build_wff_neg(vars[k]));
		}
		for (auto& neq : neq_clauses) {
			if (neq == _T<BAs...>) continue;
			neq = build_wff_neq(neq);
		}

		result = build_wff_or(result,
			build_wff_and(rest, build_wff_and<BAs...>(neq_clauses)));
	}
	assert(result != nullptr);
	result = result | repeat_all<step<BAs...>, BAs...>(eq_to_neq<BAs...>);
	BOOST_LOG_TRIVIAL(debug) << "(I) End group_paths_and_simplify";
	return make_pair(result, is_simp);
}

template<typename... BAs>
nso<BAs...> reduce_across_bfs (const nso<BAs...>& fm, bool to_cnf) {
	BOOST_LOG_TRIVIAL(debug) << "(I) Start reduce_across_bfs with";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << fm;

	auto squeezed_fm = (to_cnf ? push_negation_in(build_wff_neg(fm)) : fm);
	// Squeeze all equalities and inequalities
	squeezed_fm = squeezed_fm | repeat_all<step<BAs...>, BAs...>(squeeze_wff<BAs...>);
	squeezed_fm = reduce_terms(to_dnf2(squeezed_fm));
	// We work with unsqueezed equality
	squeezed_fm  = squeezed_fm | repeat_all<step<BAs...>, BAs...>(unsqueeze_wff_pos<BAs...>);

	BOOST_LOG_TRIVIAL(debug) << "(I) Formula in DNF: " << squeezed_fm;

#ifdef TAU_CACHE
		static std::map<std::pair<nso<BAs...>, bool>, nso<BAs...>> cache;
		if (auto it = cache.find(make_pair(squeezed_fm, to_cnf)); it != end(cache))
			return it->second;
#endif // TAU_CACHE
	auto [paths, vars] = dnf_cnf_to_bdd<BAs...>(squeezed_fm, tau_parser::wff, false, true, false);

	if (paths.empty()) {
		auto res = to_cnf ? _T<BAs...> : _F<BAs...>;
#ifdef TAU_CACHE
		return cache.emplace(make_pair(squeezed_fm, to_cnf), res).first->second;
#endif // TAU_CACHE
		BOOST_LOG_TRIVIAL(debug) << "(I) End reduce_across_bfs";
		BOOST_LOG_TRIVIAL(debug) << "(F) " << res;
		return res;
	} else if (paths.size() == 1 && paths[0].empty()) {
		auto res = to_cnf ? _F<BAs...> : _T<BAs...>;
#ifdef TAU_CACHE
		return cache.emplace(make_pair(squeezed_fm, to_cnf), res).first->second;
#endif // TAU_CACHE
		BOOST_LOG_TRIVIAL(debug) << "(I) End reduce_across_bfs";
		BOOST_LOG_TRIVIAL(debug) << "(F) " << res;
		return res;
	}

#ifdef DEBUG
	// Check that vars are not T or F.
	for (auto& var : vars) {
		assert(var != _T<BAs...> && var != _F<BAs...>);
	}
#endif // DEBUG

	bool has_simp = true;
	while (has_simp) {
		BOOST_LOG_TRIVIAL(debug) << "(I) New fix point run";
		has_simp = false;
		for (int_t i = 0; i < (int_t)paths.size(); ++i) {
			auto len_vars = vars.size();
			auto [simp_path, f] = simplify_path<BAs...>(paths[i], vars);
			if (vars.size() != len_vars) {
				// New variable was created -> append 2 to each path
				for (size_t k = 0; k < vars.size() - len_vars; ++k)
					std::ranges::for_each(paths, [](auto& path){path.emplace_back(2);});
			}

			if (f) { paths.erase(paths.begin()+i); --i; }
			else if (simp_path != paths[i]) {
#ifdef DEBUG
				BOOST_LOG_TRIVIAL(debug) << "(I) Path simplification happened: ";
				std::vector<std::vector<int_t> > tmp1{paths[i]};
				std::vector<std::vector<int_t> > tmp2{simp_path};
				BOOST_LOG_TRIVIAL(debug) << "(F) Current path: " <<
						build_reduced_formula<BAs...>(
							tmp1, vars, false,
							true);
				BOOST_LOG_TRIVIAL(debug) << "(F) Simplified path: " <<
						build_reduced_formula<BAs...>(
							tmp2, vars, false,
							true);
#endif // DEBUG
				if (std::ranges::all_of( simp_path,
					[](const auto el) { return el == 2; })) {
					auto res = to_cnf ? _F<BAs...> : _T<BAs...>;
#ifdef TAU_CACHE
					return cache.emplace(
						make_pair(squeezed_fm, to_cnf), res).first->second;
#endif // TAU_CACHE
					BOOST_LOG_TRIVIAL(debug) << "(I) End reduce_across_bfs";
					BOOST_LOG_TRIVIAL(debug) << "(F) " << res;
					return res;
				}
				has_simp = true;
				paths[i] = move(simp_path);
			}
		}
		if (has_simp) join_paths(paths);
	}

	while (true) {
		auto [simp_fm, has_further_simp] =
			group_paths_and_simplify<BAs...>(paths, vars);
#ifdef DEBUG
		if (has_further_simp) {
			BOOST_LOG_TRIVIAL(debug) << "(F) Before factoring simplification: "
				<< build_reduced_formula<BAs...>(paths, vars, false, true);
			BOOST_LOG_TRIVIAL(debug) << "(F) After factoring simplification: " << simp_fm;
		}
#endif // DEBUG
		if (!has_further_simp) {
			assert(simp_fm != nullptr);
			auto res = to_cnf ? push_negation_in(build_wff_neg(simp_fm)) : simp_fm;
#ifdef TAU_CACHE
			return cache.emplace(make_pair(squeezed_fm, to_cnf), res).first->second;
#endif // TAU_CACHE
			BOOST_LOG_TRIVIAL(debug) << "(I) End reduce_across_bfs";
			BOOST_LOG_TRIVIAL(debug) << "(F) " << res;
			return res;
		}
		auto [simp_paths, simp_vars] =
			dnf_cnf_to_bdd(simp_fm, tau_parser::wff);
		paths = move(simp_paths);
		vars = move(simp_vars);
	}
}

template<typename... BAs>
struct wff_reduce_dnf {
	nso<BAs...> operator() (const nso<BAs...>& fm) const {
		return reduce2(fm, tau_parser::wff);
	}
};

template<typename... BAs>
struct wff_reduce_cnf {
	nso<BAs...> operator() (const nso<BAs...>& fm) const {
		return reduce2(fm, tau_parser::wff, true);
	}
};

template<typename... BAs>
nso<BAs...> operator|(const nso<BAs...>& fm, const wff_reduce_dnf<BAs...>& r) {
	return r(fm);
}

template<typename... BAs>
nso<BAs...> operator|(const nso<BAs...>& fm, const wff_reduce_cnf<BAs...>& r) {
	return r(fm);
}

template<typename... BAs>
nso<BAs...> conjunct_dnfs_to_dnf (const nso<BAs...>& d1, const nso<BAs...>& d2) {
	if (is_non_terminal(tau_parser::wff, d1)) {
		assert(is_non_terminal(tau_parser::wff, d2));
		nso<BAs...> res = _F<BAs...>;
		auto clauses_d1 = get_dnf_wff_clauses(d1);
		auto clauses_d2 = get_dnf_wff_clauses(d2);
		for (const auto& dis1 : clauses_d1)
			for (const auto& dis2 : clauses_d2)
				res = build_wff_or(res, build_wff_and(dis1, dis2));
		return res;
	} else {
		assert(is_non_terminal(tau_parser::bf, d1) && is_non_terminal(tau_parser::bf, d2));
		nso<BAs...> res = _0<BAs...>;
		auto clauses_d1 = get_dnf_bf_clauses(d1);
		auto clauses_d2 = get_dnf_bf_clauses(d2);
		for (const auto& dis1 : clauses_d1)
			for (const auto& dis2 : clauses_d2)
				res = build_bf_or(res, build_bf_and(dis1, dis2));
		return res;
	}
}

template<typename... BAs>
nso<BAs...> disjunct_cnfs_to_cnf (const nso<BAs...>& c1, const nso<BAs...>& c2) {
	if (is_non_terminal(tau_parser::wff, c1)) {
		assert(is_non_terminal(tau_parser::wff, c2));
		nso<BAs...> res = _T<BAs...>;
		auto clauses_c1 = get_cnf_wff_clauses(c1);
		auto clauses_c2 = get_cnf_wff_clauses(c2);
		for (const auto& dis1 : clauses_c1)
			for (const auto& dis2 : clauses_c2)
				res = build_wff_and(res, build_wff_or(dis1, dis2));
		return res;
	} else {
		assert(is_non_terminal(tau_parser::bf, c1) && is_non_terminal(tau_parser::bf, c2));
		nso<BAs...> res = _1<BAs...>;
		auto clauses_c1 = get_cnf_bf_clauses(c1);
		auto clauses_c2 = get_cnf_bf_clauses(c2);
		for (const auto& dis1 : clauses_c1)
			for (const auto& dis2 : clauses_c2)
				res = build_bf_and(res, build_bf_or(dis1, dis2));
		return res;
	}
}

// Can be used for Tau formula and Boolean function
template<typename... BAs>
nso<BAs...> push_negation_one_in(const nso<BAs...>& fm, bool is_wff = true) {
	using p = tau_parser;
	// Tau formula rules
	if (is_wff && is_child_non_terminal(p::wff_neg, fm)) {
		auto c = trim2(fm);
		if (is_child_non_terminal(p::wff_and, c))
			return build_wff_or(
					build_wff_neg(trim(c)->child[0]),
						build_wff_neg(trim(c)->child[1]));
		if (is_child_non_terminal(p::wff_or, c))
			return build_wff_and(
					build_wff_neg(trim(c)->child[0]),
						build_wff_neg(trim(c)->child[1]));
		if (is_child_non_terminal(p::bf_eq, c))
			return build_wff_neq(trim2(c));
		if (is_child_non_terminal(p::bf_neq, c))
			return build_wff_eq(trim2(c));
		if (is_child_non_terminal(p::wff_always, c))
			return build_wff_sometimes(build_wff_neg(trim2(c)));
		if (is_child_non_terminal(p::wff_sometimes, c))
			return build_wff_always(build_wff_neg(trim2(c)));
	}
	// Boolean function rules
	if (!is_wff && is_child_non_terminal(p::bf_neg, fm)) {
		auto c = trim2(fm);
		if (is_child_non_terminal(p::bf_and, c))
			return build_bf_or(build_bf_neg(trim(c)->child[0]),
				build_bf_neg(trim(c)->child[1]));
		if (is_child_non_terminal(p::bf_or, c)) {
			return build_bf_and(build_bf_neg(trim(c)->child[0]),
				build_bf_neg(trim(c)->child[1]));
		}
	}
	return fm;
}

// Can be used for Tau formula and Boolean function
template<typename... BAs>
nso<BAs...> push_negation_in(const nso<BAs...>& fm, bool is_wff) {
#ifdef TAU_CACHE
	static std::map<nso<BAs...>, nso<BAs...>> cache;
	if (auto it = cache.find(fm); it != cache.end())
		return it->second;
#endif
	auto new_fm = push_negation_one_in(fm, is_wff);
	std::vector<nso<BAs...>> new_c;
	for (const auto& c : new_fm->child) {
		new_c.emplace_back(push_negation_in(c, is_wff));
	}
#ifdef TAU_CACHE
	auto res = make_node(new_fm->value, new_c);
	cache.emplace(res, res);
	return cache.emplace(fm, res).first->second;
#endif
	return make_node(new_fm->value, new_c);
}

// Conversion to dnf while applying reductions during the process
template<typename... BAs>
nso<BAs...> to_dnf2(const nso<BAs...>& fm, bool is_wff) {
#ifdef TAU_CACHE
	static std::map<nso<BAs...>, nso<BAs...>> cache;
	if (auto it = cache.find(fm); it != cache.end())
		return it->second;
#endif
	using p = tau_parser;
	auto new_fm = push_negation_one_in(fm, is_wff);

	if (is_wff && is_non_terminal(p::wff, new_fm)) {
		if (is_child_non_terminal(p::wff_and, new_fm)) {
			auto conj = conjunct_dnfs_to_dnf(
				to_dnf2(trim(new_fm)->child[0], is_wff),
				to_dnf2(trim(new_fm)->child[1], is_wff));
			// Perform simplification
			if (conj != new_fm)
				new_fm = conj | wff_reduce_dnf<BAs...>();
		} else if (is_child_non_terminal(p::wff_or, new_fm)) {
			new_fm = build_wff_or(
				to_dnf2(trim(new_fm)->child[0], is_wff),
				to_dnf2(trim(new_fm)->child[1], is_wff));
		}
	} else if (!is_wff && is_non_terminal(p::bf, new_fm)) {
		if (is_child_non_terminal(p::bf_and, new_fm)) {
			auto conj = conjunct_dnfs_to_dnf(
				to_dnf2(trim(new_fm)->child[0], is_wff),
				to_dnf2(trim(new_fm)->child[1], is_wff));
			// Perform simplification
			if (conj != new_fm)
                new_fm = reduce2(conj, p::bf);
		} else if (is_child_non_terminal(p::bf_or, new_fm)) {
			new_fm = build_bf_or(
				to_dnf2(trim(new_fm)->child[0], is_wff),
				to_dnf2(trim(new_fm)->child[1], is_wff));
		}
	}
	assert(fm != nullptr);
#ifdef TAU_CACHE
	cache.emplace(new_fm, new_fm);
	return cache.emplace(fm, new_fm).first->second;
#endif
	return new_fm;
}

// Conversion to cnf while applying reductions during the process
template<typename... BAs>
nso<BAs...> to_cnf2(const nso<BAs...>& fm, bool is_wff) {
#ifdef TAU_CACHE
	static std::map<nso<BAs...>, nso<BAs...>> cache;
	if (auto it = cache.find(fm); it != cache.end())
		return it->second;
#endif
	using p = tau_parser;
	auto new_fm = push_negation_one_in(fm, is_wff);

	if (is_wff && is_non_terminal(p::wff, new_fm)) {
		if (is_child_non_terminal(p::wff_or, new_fm)) {
			auto dis = disjunct_cnfs_to_cnf(
				to_cnf2(trim(new_fm)->child[0], is_wff),
				to_cnf2(trim(new_fm)->child[1], is_wff));
			// Perform simplification
			if (dis != new_fm)
				new_fm = dis | wff_reduce_cnf<BAs...>();
		} else if (is_child_non_terminal(p::wff_and, new_fm)) {
			new_fm = build_wff_and(
				to_cnf2(trim(new_fm)->child[0], is_wff),
				to_cnf2(trim(new_fm)->child[1], is_wff));
		}
	} else if (!is_wff){
		if (is_child_non_terminal(p::bf_or, new_fm)) {
			auto dis = disjunct_cnfs_to_cnf(
				to_cnf2(trim(new_fm)->child[0], is_wff),
				to_cnf2(trim(new_fm)->child[1], is_wff));
			// Perform simplification
			if (dis != new_fm)
                new_fm = reduce2(dis, p::bf, true);
		} else if (is_child_non_terminal(p::bf_and, new_fm)) {
			new_fm = build_bf_and(
				to_cnf2(trim(new_fm)->child[0], is_wff),
				to_cnf2(trim(new_fm)->child[1], is_wff));
		}
	}
	assert(fm != nullptr);
#ifdef TAU_CACHE
	cache.emplace(new_fm, new_fm);
	return cache.emplace(fm, new_fm).first->second;
#endif
	return new_fm;
}

// A formula has a temporal variable if either it contains an io_var with a variable or capture
// or it contains a flag
template<typename... BAs>
bool has_temp_var (const nso<BAs...>& fm) {
	auto io_vars = select_top(fm, is_non_terminal<tau_parser::io_var, BAs...>);
	if (io_vars.empty()) return find_top(fm, is_non_terminal<tau_parser::constraint, BAs...>).has_value();
	for (const auto& io_var : io_vars) {
		if (find_top(io_var, is_non_terminal<tau_parser::shift, BAs...>))
			return true;
		if (!find_top(io_var, is_non_terminal<tau_parser::num, BAs...>))
			return true;
	}
	return find_top(fm, is_non_terminal<tau_parser::constraint, BAs...>).has_value();
}

// Assumes a sometimes formula in dnf with negation pushed in containing no wff_or with max nesting depth 1
template<typename... BAs>
nso<BAs...> extract_sometimes (nso<BAs...> fm) {
	std::map<nso<BAs...>, nso<BAs...>> l_changes = {};
	// Collect remaining nested "sometimes" formulas
	std::vector<nso<BAs...>> sometimes_extractions = {};
	for (const auto &inner_st: select_top(trim2(fm), is_child_non_terminal<tau_parser::wff_sometimes, BAs...>)) {
		l_changes[inner_st] = _T<BAs...>;
		sometimes_extractions.push_back(inner_st);
	}
	// Collect remaining nested "always" formulas
	std::vector<nso<BAs...>> always_extractions = {};
	for (const auto &inner_aw: select_top(trim2(fm), is_child_non_terminal<tau_parser::wff_always, BAs...>)) {
		l_changes[inner_aw] = _T<BAs...>;
		always_extractions.push_back(inner_aw);
	}
	// Apply always/sometimes extractions to fm
	if (!l_changes.empty()) fm = replace(fm, l_changes);

	std::vector<nso<BAs...>> extracted = {}, staying = {};
	auto clauses = get_leaves(trim2(fm), tau_parser::wff_and, tau_parser::wff);
	if (clauses.empty()) clauses.push_back(fm);
	for (const auto& clause : clauses) {
		assert(!is_non_terminal(tau_parser::wff_sometimes, trim(clause)) &&
					!is_non_terminal(tau_parser::wff_always, trim(clause)));
		if(!has_temp_var(clause))
			extracted.push_back(clause);
		else staying.push_back(clause);
	}

	// From here we build the formula which we will return
	nso<BAs...> extracted_fm;
	if (extracted.empty()) extracted_fm = _T<BAs...>;
	else {
		bool first = true;
		for (const auto &e: extracted) {
			if (first) { first = false; extracted_fm = e; }
			else extracted_fm = build_wff_and(extracted_fm, e);
		}
	}
	for (const auto &se: sometimes_extractions)
		extracted_fm = build_wff_and(extracted_fm, se);
	for (const auto &ae : always_extractions)
		extracted_fm = build_wff_and(extracted_fm, ae);
	nso<BAs...> staying_fm;
	if (staying.empty()) return extracted_fm;
	else {
		bool first = true;
		for (const auto &s: staying) {
			if (first) { first = false; staying_fm = s; }
			else staying_fm = build_wff_and(staying_fm, s);
		}
	}
	if (extracted_fm == _T<BAs...>) return build_wff_sometimes(staying_fm);
	return build_wff_and(build_wff_sometimes(staying_fm), extracted_fm);
}

// Assumes an always formula in cnf with negation pushed in containing no wff_and with max nesting depth 1
template<typename... BAs>
nso<BAs...> extract_always (nso<BAs...> fm) {
	std::map<nso<BAs...>, nso<BAs...>> l_changes = {};
	// Collect remaining nested "sometimes" formulas
	std::vector<nso<BAs...>> sometimes_extractions = {};
	for (const auto &inner_st: select_top(trim2(fm), is_child_non_terminal<tau_parser::wff_sometimes, BAs...>)) {
		l_changes[inner_st] = _F<BAs...>;
		sometimes_extractions.push_back(inner_st);
	}
	// Collect remaining nested "always" formulas
	std::vector<nso<BAs...>> always_extractions = {};
	for (const auto &inner_aw: select_top(trim2(fm), is_child_non_terminal<tau_parser::wff_always, BAs...>)) {
		l_changes[inner_aw] = _F<BAs...>;
		always_extractions.push_back(inner_aw);
	}
	// Apply always/sometimes extractions to flat_st
	if (!l_changes.empty()) fm = replace(fm, l_changes);

	// Now extract from all disjuncts
	std::vector<nso<BAs...>> extracted = {}, staying = {};
	auto clauses = get_leaves(trim2(fm), tau_parser::wff_or, tau_parser::wff);
	if (clauses.empty()) clauses.push_back(fm);
	for (const auto& clause : clauses) {
		assert(!is_non_terminal(tau_parser::wff_sometimes, trim(clause)) &&
					!is_non_terminal(tau_parser::wff_always, trim(clause)));

		if (!has_temp_var(clause))
			extracted.push_back(clause);
		else staying.push_back(clause);
	}

	// From here we build the formula to return based on the extractions
	nso<BAs...> extracted_fm;
	if (extracted.empty()) extracted_fm = _F<BAs...>;
	else {
		bool first = true;
		for (const auto &e: extracted) {
			if (first) { first = false; extracted_fm = e; }
			else extracted_fm = build_wff_or(extracted_fm, e);
		}
	}
	for (const auto &se: sometimes_extractions)
		extracted_fm = build_wff_or(extracted_fm, se);
	for (const auto &ae : always_extractions)
		extracted_fm = build_wff_or(extracted_fm, ae);
	nso<BAs...> staying_fm;
	if (staying.empty()) return extracted_fm;
	else {
		bool first = true;
		for (const auto &s: staying) {
			if (first) { first = false; staying_fm = s; }
			else staying_fm = build_wff_or(staying_fm, s);
		}
	}
	if (extracted_fm == _F<BAs...>) return build_wff_always(staying_fm);
	return build_wff_or(build_wff_always(staying_fm), extracted_fm);
}

// Recursively extract non-dependend formulas under sometimes
template<typename... BAs>
nso<BAs...> push_sometimes_always_in (nso<BAs...> fm) {
	std::map<nso<BAs...>, nso<BAs...>> g_changes = {};
	for (const auto &st : select_top_until(fm, is_child_non_terminal<tau_parser::wff_sometimes, BAs...>,
								is_child_non_terminal<tau_parser::wff_always, BAs...>)) {
		// Recursively denest sometimes and always statements contained in sometimes statement st
		auto flat_st = build_wff_sometimes(push_sometimes_always_in(trim2(st)));
		// Simplyfy current formula and convert to DNF
		// Reductions done in order to prevent blow up
		flat_st = to_dnf2(flat_st);
		flat_st = reduce2(flat_st, tau_parser::wff);
		if (flat_st != st) g_changes[st] = flat_st;
	}
	// Apply changes
	if (!g_changes.empty()) {
		fm = replace(fm, g_changes);
		g_changes = {};
	}
	for (const auto &st : select_top_until(fm, is_child_non_terminal<tau_parser::wff_sometimes, BAs...>,
								is_child_non_terminal<tau_parser::wff_always, BAs...>)) {
		// Here a formula under "sometimes" is of the form (phi_1 && ... && phi_n)
		// Pull out each phi_i that does not contain a time variable or is "sometimes/always"
		auto simp_st = extract_sometimes(st);
		if (st != simp_st) g_changes[st] = simp_st;
	}
	// Apply changes
	if (!g_changes.empty()) {
		fm = replace(fm, g_changes);
		g_changes = {};
	}
	for (const auto& aw : select_top_until(fm, is_child_non_terminal<tau_parser::wff_always, BAs...>,
								is_child_non_terminal<tau_parser::wff_sometimes, BAs...>)) {
		// Recursively denest sometimes and always statements contained in always statement aw
		auto flat_aw = build_wff_always(push_sometimes_always_in(trim2(aw)));
		// Simplyfy current formula and convert to CNF
		// Reductions done in order to prevent blow up
		// TODO: First push always in
		flat_aw = to_cnf2(flat_aw);
		flat_aw = reduce2(flat_aw, tau_parser::wff, true);
		// TODO: Then push always back out
		if (flat_aw != aw) g_changes[aw] = flat_aw;
	}
	// Apply changes
	if (!g_changes.empty()) {
		fm = replace(fm, g_changes);
		g_changes = {};
	}
	for (const auto& aw : select_top_until(fm, is_child_non_terminal<tau_parser::wff_always, BAs...>,
								is_child_non_terminal<tau_parser::wff_sometimes, BAs...> )) {
		// Here the formula under "always" is of the form (phi_1 || ... || phi_n)
		// Pull out each phi_i that does not contain a time variable or is "sometimes/always"
		auto simp_aw = extract_always(aw);
		if (aw != simp_aw) g_changes[aw] = simp_aw;
	}
	// Apply changes and return
	if (!g_changes.empty()) fm = replace(fm, g_changes);
	return fm;
}

// Assumes a single DNF clause and normalizes the "always" parts into one
template<typename... BAs>
nso<BAs...> pull_always_out(const nso<BAs...>& fm) {
	std::map<nso<BAs...>, nso<BAs...>> l_changes = {};
	std::vector<nso<BAs...>> collected_always_fms;
	// Collect all always statments in the clause fm
	// by analyzing conjuncts
	auto clauses = get_leaves(fm, tau_parser::wff_and, tau_parser::wff);
	if(clauses.empty()) clauses.push_back(fm);
	for (const auto& clause : clauses) {
		// if clause is a sometimes statement -> skip
		if (is_child_non_terminal(tau_parser::wff_sometimes, clause))
			continue;
		if (is_child_non_terminal(tau_parser::wff_always, clause)) {
			l_changes[clause] = _T<BAs...>;
			collected_always_fms.push_back(trim2(clause));
		}
		else {
			// In case there is no temporal quantifier in clause
			l_changes[clause] = _T<BAs...>;
			collected_always_fms.push_back(clause);
		}
	}
	if (collected_always_fms.empty()) return fm;
	// Rebuild formula based on the extraction
	nso<BAs...> always_part;
	bool first = true;
	for (const auto& fa : collected_always_fms) {
		if (first) {first = false; always_part = fa;}
		else always_part = build_wff_and(always_part, fa);
	}
	assert(!l_changes.empty());

	if (!has_temp_var(fm)) {
		// No input/output variable present, hence return without always
		return build_wff_and(always_part, replace(fm, l_changes));
	}
	return build_wff_and(build_wff_always(always_part), replace(fm, l_changes));
}

// We assume that there is no nesting of "sometimes" and "always" in fm
// and that fm is in DNF
template<typename... BAs>
nso<BAs...> pull_sometimes_always_out(nso<BAs...> fm) {
	std::map<nso<BAs...>, nso<BAs...>> changes = {};
	std::vector<nso<BAs...>> collected_no_temp_fms;
	nso<BAs...> pure_always_clause;
	// Collect all disjuncts which have temporal variables and call pull_always_out on the others
	auto clauses = get_leaves(fm, tau_parser::wff_or, tau_parser::wff);
	if(clauses.empty()) clauses.push_back(fm);
	for (const auto& clause : clauses) {
        auto r = pull_always_out(clause);
        if (!has_temp_var(r)) {
        	changes[clause] = _F<BAs...>;
        	collected_no_temp_fms.push_back(r);
        }
        else if (clause != r) {
        	if (is_child_non_terminal(tau_parser::wff_always, r))
        		pure_always_clause = trim2(r);
	        changes[clause] = r;
        } else if (is_child_non_terminal(tau_parser::wff_always, clause))
        	pure_always_clause = trim2(clause);
	}
	if (!changes.empty()) fm = replace(fm, changes);
	if(!collected_no_temp_fms.empty()) {
		nso<BAs...> no_temp_fm;
		bool first = true;
		for (const auto& f : collected_no_temp_fms) {
			if (first) {first = false; no_temp_fm = f;}
			else no_temp_fm = build_wff_or(no_temp_fm, f);
		}
		if (pure_always_clause) {
			changes = {};
			changes[pure_always_clause] = build_wff_or(pure_always_clause, no_temp_fm);
			return replace(fm, changes);
		}
		//no_temp_fm = build_wff_always(no_temp_fm);
		fm = build_wff_or(fm, no_temp_fm);
	}
	return fm;
}

// The needed class in order to make sometimes/always normalization work with normalizer
template<typename... BAs>
struct sometimes_always_normalization {
	nso<BAs...> operator() (const nso<BAs...>& fm) const {
		auto st_aw = [](const auto& n) {
			return is_child_non_terminal(tau_parser::wff_sometimes, n) ||
				is_child_non_terminal(tau_parser::wff_always, n);
		};
		// If there is no temporal quantifier and no temporal variable
		// Just convert to DNF and return

		if (!find_top(fm, st_aw).has_value() && !has_temp_var(fm))
			return reduce_across_bfs(fm, false);

		// Scope formula under always if not already under sometimes or always
		nso<BAs...> res = !st_aw(fm) ? build_wff_always(fm) : fm;
		// conversion to DNF necessary for pull_sometimes_always_out
		res = to_dnf2(push_sometimes_always_in(res));
		res = reduce2(res, tau_parser::wff);
		res = pull_sometimes_always_out(res) |
					repeat_each<step<BAs...>, BAs...>(simplify_wff<BAs...>);
		auto temp_inner = select_top(res, st_aw);
		if (temp_inner.empty()) return res;
		std::map<nso<BAs...>, nso<BAs...>> changes;
		for (const auto& f : temp_inner) {
			// Reduction done to normalize again now that sometimes/always are all the way out
			changes[trim2(f)] = reduce_across_bfs(trim2(f), false);
		}
		return reduce_across_bfs(replace(res, changes), false);
	}
};

template<typename... BAs>
nso<BAs...> operator|(const nso<BAs...>& fm, const sometimes_always_normalization<BAs...>& r) {
	return r(fm);
}

template<typename... BAs>
nso<BAs...> wff_remove_existential(const nso<BAs...>& var, const nso<BAs...>& wff) {
	auto squeeze_positives = [](const nso<BAs...>& n) -> std::optional<nso<BAs...>>{
		if (auto positives = select_top(n, is_non_terminal<tau_parser::bf_eq, BAs...>);
				positives.size() > 0) {
			std::set<sp_tau_node<BAs...>> bfs;
			for (auto& p: positives)
				bfs.insert(p | tau_parser::bf | optional_value_extractor<sp_tau_node<BAs...>>);
			return build_bf_or<BAs...>(bfs);
		}
		return {};
	};
	// Following Corollary 2.3 from Taba book from Ohad
	auto is_var = [&var](const auto& node){return node == var;};
	// if var does not appear in the formula, we can return the formula as is
	// if (!find_top(wff, is_var)) return wff;
	std::map<nso<BAs...>, nso<BAs...>> changes;
	for (const auto& l: get_leaves(wff, tau_parser::wff_or, tau_parser::wff)) {
		// if var does not appear in the clause, we can skip it
		if (!find_top(l, is_var)) continue;
		// Get each conjunct in clause
		nso<BAs...> nl = _T<BAs...>;
		bool is_quant_removable_in_clause = true;
		for (const auto& conj : get_leaves(l, tau_parser::wff_and, tau_parser::wff)) {
			// Check if conjunct is of form = 0 or != 0
			if ((conj | tau_parser::bf_eq) || (conj | tau_parser::bf_neq))
				continue;
			// If the conjunct contains the quantified variable at this point
			// we cannot resolve the quantifier in this clause
			if (find_top(conj, is_var)) {
				is_quant_removable_in_clause = false;
				break;
			}
			// conjunct does not depend on var
			nl = build_wff_and(nl, conj);
		}
		if (!is_quant_removable_in_clause) {
			// Since we cannot remove the quantifier in this
			// clause it needs to be maintained
			changes[l] = build_wff_ex(var, l);
			continue;
		}

		auto f = squeeze_positives(l);
		std::map<nso<BAs...>, nso<BAs...>> changes_0 = {{var, _0_trimmed<BAs...>}};
		std::map<nso<BAs...>, nso<BAs...>> changes_1 = {{var, _1_trimmed<BAs...>}};
		auto f_0 = f ? replace(f.value(), changes_0) : _0<BAs...>;
		auto f_1 = f ? replace(f.value(), changes_1) : _0<BAs...>;

		if (auto neqs = select_all(l, is_non_terminal<tau_parser::bf_neq, BAs...>); neqs.size() > 0) {
			auto nneqs = _T<BAs...>;
			for (auto& neq: neqs) {
				auto g = neq | tau_parser::bf | optional_value_extractor<sp_tau_node<BAs...>>;
				auto g_0 = replace(g, changes_0);
				auto g_1 = replace(g, changes_1);
				// If both are 1 then inequality is implied by f_0f_1 = 0
				if (g_0 == _1<BAs...> && g_1 == _1<BAs...>) continue;
				// If f_0 is equal to f_1 we can use assumption f_0 = 0 and f_1 = 0
				if (f_0 == f_1) {
					nneqs = build_wff_and(nneqs, build_wff_neq(build_bf_or(g_0, g_1)));
				} else if (g_0 == g_1) {
					nneqs = build_wff_and(nneqs, build_wff_neq(g_0));
				} else nneqs = build_wff_and(nneqs, build_wff_neq(build_bf_or(
                                      build_bf_and(build_bf_neg(f_1),	g_1),
                                      build_bf_and(build_bf_neg(f_0),	g_0))));
			}
			nl = build_wff_and(nl, build_wff_and(build_wff_eq(build_bf_and(f_0, f_1)), nneqs));
		} else if (f) {
			nl = build_wff_and(nl, build_wff_eq(build_bf_and(f_0, f_1)));
		}
		changes[l] = nl;
	}
	return replace<nso<BAs...>>(wff, changes);
}

template<typename... BAs>
nso<BAs...> eliminate_existential_quantifier(const auto& inner_fm, auto& scoped_fm) {
	// Reductions to prevent blow ups and achieve DNF
	BOOST_LOG_TRIVIAL(debug) << "(I) Start existential quantifier elimination";
	BOOST_LOG_TRIVIAL(debug) << "(I) Quantified variable: " << inner_fm;
	BOOST_LOG_TRIVIAL(debug) << "(F) Quantified formula: " << scoped_fm;
	// scoped_fm = scoped_fm | bf_reduce_canonical<BAs...>();
	scoped_fm = reduce_across_bfs(scoped_fm, false);

#ifdef TAU_CACHE
		static std::map<std::pair<nso<BAs...>,nso<BAs...>>, nso<BAs...>> cache;
		if (auto it = cache.find(make_pair(inner_fm, scoped_fm)); it != end(cache))
			return it->second;
#endif // TAU_CACHE

	auto clauses = get_leaves(scoped_fm, tau_parser::wff_or, tau_parser::wff);
	nso<BAs...> res;
	for (const auto& clause : clauses) {
		// Check if every conjunct in clause is of form f = 0 or f != 0
		auto conjuncts = get_leaves(clause, tau_parser::wff_and, tau_parser::wff);
		bool all_equal_zero = true, all_unequal_zero = true;
		for (const auto& c : conjuncts) {
			if (!is_child_non_terminal(tau_parser::bf_eq, c))
				all_equal_zero = false;
			if (!is_child_non_terminal(tau_parser::bf_neq, c))
				all_unequal_zero = false;
		}
		if (all_unequal_zero) {
			nso<BAs...> new_conjunct;
			// Push quantifier inside conjunction
			for (const auto& c : conjuncts) {
				auto new_c = wff_remove_existential(trim2(inner_fm), c);
				if (new_conjunct) new_conjunct = build_wff_and(new_conjunct, new_c);
				else new_conjunct = new_c;
			}
			if (new_conjunct == _T<BAs...>) return _T<BAs...>;
			if (res) res = build_wff_or(res, new_conjunct);
			else res = new_conjunct;
		}
		else if (all_equal_zero) {
			nso<BAs...> new_func;
			for (const auto& d: conjuncts) {
				if (new_func) new_func = build_bf_or(new_func, trim2(d));
				else new_func = trim2(d);
			}
			new_func = build_wff_eq(new_func | bf_reduce_canonical<BAs...>());
			new_func = wff_remove_existential(trim2(inner_fm), new_func);
			if (new_func == _T<BAs...>) return _T<BAs...>;
			if (res) res = build_wff_or(res, new_func);
			else res = new_func;
		}
		else {
			// Resolve quantified variable in scoped_fm
			auto rem_clause = wff_remove_existential(trim2(inner_fm), clause);
			if (rem_clause == _T<BAs...>) return _T<BAs...>;
			if (res) res = build_wff_or(res, rem_clause);
			else res = wff_remove_existential(trim2(inner_fm), clause);
		}
	}
	// Simplify elimination result
	res = reduce_across_bfs(res, false);
	BOOST_LOG_TRIVIAL(debug) << "(I) End existential quantifier elimination";
	BOOST_LOG_TRIVIAL(debug) << "(F)" << res;
#ifdef TAU_CACHE
	return cache.emplace(make_pair(inner_fm, scoped_fm), res).first->second;
#endif // TAU_CACHE
	return res;
}

template<typename... BAs>
nso<BAs...> eliminate_universal_quantifier(const auto& inner_fm, auto& scoped_fm) {
	BOOST_LOG_TRIVIAL(debug) << "(I) Start universal quantifier elimination";
	BOOST_LOG_TRIVIAL(debug) << "(I) Quantified variable: " << inner_fm;
	BOOST_LOG_TRIVIAL(debug) << "(F) Quantified formula: " << scoped_fm;
	// Reductions to prevent blow ups and achieve CNF
	// scoped_fm = scoped_fm | bf_reduce_canonical<BAs...>();
	scoped_fm = reduce_across_bfs(scoped_fm, true);
// Add cache after reductions; reductions are cached as well
#ifdef TAU_CACHE
		static std::map<std::pair<nso<BAs...>,nso<BAs...>>, nso<BAs...>> cache;
		if (auto it = cache.find(make_pair(inner_fm, scoped_fm)); it != end(cache))
			return it->second;
#endif // TAU_CACHE

	auto clauses = get_leaves(scoped_fm, tau_parser::wff_and, tau_parser::wff);
	nso<BAs...> res;
	for (const auto &clause: clauses) {
		// Check if every disjunct in clause is of form f = 0 or f != 0
		auto disjuncts = get_leaves(clause, tau_parser::wff_or, tau_parser::wff);
		bool all_equal_zero = true, all_unequal_zero = true;
		for (const auto &d: disjuncts) {
			if (!is_child_non_terminal(tau_parser::bf_eq, d))
				all_equal_zero = false;
			if (!is_child_non_terminal(tau_parser::bf_neq, d))
				all_unequal_zero = false;
		}
		if (all_equal_zero) {
			nso<BAs...> new_disjunct;
			// Push quantifier inside disjunction
			for (const auto &d: disjuncts) {
				auto new_d = push_negation_in(build_wff_neg(d));
				new_d = push_negation_in(
					build_wff_neg(wff_remove_existential(trim2(inner_fm), new_d)));
				if (new_disjunct) new_disjunct = build_wff_or(new_disjunct, new_d);
				else new_disjunct = new_d;
			}
			if (new_disjunct == _F<BAs...>) return _F<BAs...>;
			if (res) res = build_wff_and(res, new_disjunct);
			else res = new_disjunct;
		}
		if (all_unequal_zero) {
			nso<BAs...> new_func;
			for (const auto &d: disjuncts) {
				if (new_func) new_func = build_bf_or(new_func, trim2(d));
				else new_func = trim2(d);
			}
			new_func = build_wff_eq(new_func | bf_reduce_canonical<BAs...>());
			new_func = push_negation_in(
				build_wff_neg(wff_remove_existential(trim2(inner_fm), new_func)));
			if (new_func == _F<BAs...>) return _F<BAs...>;
			if (res) res = build_wff_and(res, new_func);
			else res = new_func;
		} else {
			// Turn universal into existential quantifier and eliminate
			auto new_clause = push_negation_in(build_wff_neg(clause));
			new_clause = push_negation_in(
				build_wff_neg(wff_remove_existential(trim2(inner_fm), new_clause)));
			if (res == _F<BAs...>) return _F<BAs...>;
			if (!res) res = new_clause;
			else res = build_wff_and(res, new_clause);
		}
	}
	// Simplify elimination result
	res = reduce_across_bfs(res, true);
	BOOST_LOG_TRIVIAL(debug) << "(I) End universal quantifier elimination";
    BOOST_LOG_TRIVIAL(debug) << "(F)" << res;
#ifdef TAU_CACHE
	return cache.emplace(make_pair(inner_fm, scoped_fm), res).first->second;
#endif // TAU_CACHE
	return res;
}

// Pushes all universal and existential quantifiers as deep as possible into the formula
template<typename... BAs>
nso<BAs...> eliminate_quantifiers(const nso<BAs...>& fm) {
	// Lambda is applied to nodes of fm in post order
	auto elim_quant = [](nso<BAs...> inner_fm, const auto& child) -> nso<BAs...> {
		// Update node according to current children
		if (inner_fm->child != child) inner_fm = make_node(inner_fm->value, child);
		// Find out if current node is a quantifier
		bool is_ex_quant;
		if (is_child_non_terminal(tau_parser::wff_ex, inner_fm))
			is_ex_quant = true;
		else if (is_child_non_terminal(tau_parser::wff_all, inner_fm))
			is_ex_quant = false;
		else return inner_fm;

		auto has_var = [&inner_fm](const auto& node)
					{return node == trim2(inner_fm);};
		auto scoped_fm = trim(inner_fm)->child[1];
		if (!find_top(scoped_fm, has_var)) {
			// If the scoped formula does not contain
			// the quantified variable, remove the quantifier
			return scoped_fm;
		}
		// Scoped formula contains the quantified variable
		if (is_ex_quant) return eliminate_existential_quantifier<BAs...>(inner_fm, scoped_fm);
		else return eliminate_universal_quantifier<BAs...>(inner_fm, scoped_fm);
	};
	auto is_not_bf = [](const nso<BAs...>& node){return !is_non_terminal(tau_parser::bf, node);};
	return rewriter::post_order_recursive_traverser<nso<BAs...>>()(fm, is_not_bf, elim_quant);
}



// We assume that the input is a formula is in MNF (with no quantifiers whatsoever).
// We implicitly transformed into BDD form and compute one step of the SNF transformation.
template<typename...BAs>
struct to_snf_step {

	using var = nso<BAs...>;
	using vars = std::set<var>;
	using constant = std::variant<BAs...>;
	using exponent = std::set<var>;
	using literal = nso<BAs...>;
	using literals = std::set<literal>;
	using partition = std::map<exponent, literals>;
	using bdd_path = std::pair<partition /* positive */, partition /* negatives */>;

	nso<BAs...> operator()(const nso<BAs...>& form) const {
		// we select all literals, i.e. wff equalities or it negations.
		static const auto is_literal = [](const auto& n) -> bool {
			return (n | tau_parser::bf_eq).has_value();
		};
		if (auto literals = select_all(form, is_literal); !literals.empty()) {
			// we call the recursive method traverse to traverse all the paths
			// of the BDD.
			std::set<nso<BAs...>> remaining(literals.begin(), literals.end());
			bdd_path path;
			return traverse(path, remaining, form)
				| bf_reduce_canonical<BAs...>() | reduce_wff<BAs...>;
		}
		return form;
	}

private:

	static constexpr auto _or = overloaded([]<typename T>(const T& l, const T& r) -> std::variant<BAs...> {
			return l | r;
	}, [](const auto&, const auto&) -> std::variant<BAs...> { throw std::logic_error("wrong types"); });

	static constexpr auto _and = overloaded([]<typename T>(const T& l, const T& r) -> std::variant<BAs...> {
			return l & r;
	}, [](const auto&, const auto&) -> std::variant<BAs...> { throw std::logic_error("wrong types"); });

	static constexpr auto _neg = overloaded([]<typename T>(const T& l) -> std::variant<BAs...> {
			return ~l;
	}, [](const auto&) -> std::variant<BAs...> { throw std::logic_error("wrong types"); });

	static constexpr auto _neq_zero = overloaded([]<typename T>(const T& l) -> bool {
			return l != false;
	}, [](const auto&) -> bool { throw std::logic_error("wrong types"); });

	static constexpr auto _leq = overloaded([]<typename T>(const T& l, const T& r) -> bool {
			return (l & ~r) == false;
	}, [](const auto&, const auto&) -> bool { throw std::logic_error("wrong types"); });

	nso<BAs...> bdd_path_to_snf(const bdd_path& path, const nso<BAs...>& form) const {
		// we simplify the constant part of the formula
		// TODO (HIGH) fix simplification
		auto simplified = form | simplify_snf<BAs...>;
		return build_wff_and(normalize(path), form) | simplify_snf<BAs...>;
	}

	bdd_path add_to_negative_path(const bdd_path& path, const literal& lit) const {
		bdd_path npath;
		npath.first = path.first;
		auto lit_exp = get_exponent(lit);

		for (auto& [exp, negatives]: path.second)
			if (exp != lit_exp) npath.second[exp] = negatives;
		if (!path.second.contains(lit_exp)) npath.second[lit_exp] = { lit };
		else {
			bool insert = true;
			for (auto& n: path.second.at(lit_exp)){
				// careful with the order of the statements
				if (is_less_eq_than(n, lit)) insert = false;
				if (is_less_eq_than(lit, n)) continue;
				npath.second[lit_exp].insert(n);
			}
			if (insert) npath.second[lit_exp].insert(lit);
		}
		return npath;
	}

	bdd_path add_to_positive_path(const bdd_path& path, const literal& lit) const {
		bdd_path npath;
		npath.second = path.second;
		auto lit_exp = get_exponent(lit);

		for (auto& [exp, positives]: path.first)
			if (exp != lit_exp) npath.first[exp] = positives;
		if (!path.first.contains(lit_exp)) npath.first[lit_exp] = { lit };
		else {
			bool insert = true;
			for (auto& n: path.first.at(lit_exp)) {
				// careful with the order of the statements
				if (is_less_eq_than(lit, n)) insert = false;
				if (is_less_eq_than(n, lit)) continue;
				npath.first[lit_exp].insert(n);
			}
			if (insert) npath.first[lit_exp].insert(lit);
		}
		return npath;
	}

	nso<BAs...> traverse(const bdd_path& path, const literals& remaining, const nso<BAs...>& form) const {
		// we only cache results in release mode
		#ifdef TAU_CACHE
		static std::map<std::tuple<bdd_path, literals, nso<BAs...>>, nso<BAs...>> cache;
		if (auto it = cache.find({path, remaining, form}); it != cache.end()) return it->second;
		#endif // TAU_CACHE

		if (remaining.empty()) return bdd_path_to_snf(path, form);

		auto lit = *remaining.begin();
		auto exponent = get_exponent(lit);
		auto f = normalize_negative(path, lit) == _F<BAs...> ? _F<BAs...> : replace_with(lit, _F<BAs...>, form) | simplify_snf<BAs...>;
		auto t = normalize_positive(path, lit) == _F<BAs...> ? _F<BAs...> : replace_with(lit, _T<BAs...>, form) | simplify_snf<BAs...>;

		if (f == _F<BAs...> && t == _F<BAs...>) {
			// we only cache results in release mode
			#ifdef TAU_CACHE
			cache[{path, remaining, form}] = _F<BAs...>;
			#endif // TAU_CACHE
			return _F<BAs...>;
		}

		literals nremaining(++remaining.begin(), remaining.end());

		if (f == t) return traverse(path, nremaining, t);

		auto t_snf = _F<BAs...>, f_snf = _F<BAs...>;

		if (f != _F<BAs...>) {
			auto f_path = add_to_negative_path(path, lit);
			f_snf = traverse(f_path, nremaining, f);
		}

		if (t != _F<BAs...>) {
			auto t_path = add_to_positive_path(path, lit);
			t_snf = traverse(t_path, nremaining, t);
		}

		if (f_snf == _F<BAs...>) {
			#ifdef TAU_CACHE
			cache[{path, remaining, form}] = t_snf;
			#endif // TAU_CACHE
			return t_snf;
		}

		if (t_snf == _F<BAs...>) {
			#ifdef TAU_CACHE
			cache[{path, remaining, form}] = f_snf;
			#endif // TAU_CACHE
			return f_snf;
		}

		auto result = build_wff_or(t_snf, f_snf);
		// we only cache results in release mode
		#ifdef TAU_CACHE
		cache[{path, remaining, form}] = result;
		#endif // TAU_CACHE
		return result;
	}

	exponent get_exponent(const nso<BAs...>& n) const {
		auto is_bf_literal = [](const auto& n) -> bool {
			return (n | tau_parser::variable).has_value()
				|| (n | tau_parser::bf_neg | tau_parser::bf | tau_parser::variable).has_value();
		};
		auto all_vs = select_top(n, is_bf_literal);
		return exponent(all_vs.begin(), all_vs.end());
	}

	std::optional<constant> get_constant(const literal& lit) const {
		return find_top(lit, is_non_terminal<tau_parser::constant, BAs...>)
			| only_child_extractor<BAs...> | ba_extractor<BAs...>;
	}

	partition make_partition_by_exponent(const literals& s) const {
		partition p;
		for (auto& e: s) p[get_exponent(e)].insert(e);
		return p;
	}

	nso<BAs...> squeeze_positives(const std::set<literal>& positives, const exponent& exp) const {
		// find first element with non trivial constant
		auto first = std::find_if(positives.begin(), positives.end(), [&](const auto& l) {
			return get_constant(l).has_value();
		});
		// if there is no such element we return the first element
		if (first == positives.end()) return nso<BAs...>(*positives.begin());
		// otherwise...
		auto first_cte = build_bf_constant(get_constant(*first).value());
		auto cte = std::accumulate(++positives.begin(), positives.end(), first_cte, [&](const auto& l, const auto& r) {
			auto l_cte = get_constant(l), r_cte = get_constant(r);
			if (l_cte && r_cte)
				return build_bf_constant(std::visit(_or, l_cte.value(), r_cte.value()));
			return l;
		});

		// return the conjunction of all the same exponent literals and the accumulated constant
		auto term = std::accumulate(exp.begin(), exp.end(), cte, [](const auto& l, const auto& r) {
			return build_bf_and(l, r);
		});
		// return the corresponding wff
		return build_wff_eq(term);
	}

	bool is_less_eq_than(const literal& ll, const exponent& le, const literal& rl, const exponent& re) const {
		return std::includes(le.begin(), le.end(), re.begin(), re.end())
			 && is_less_eq_than(ll, rl);
	}

	// remove redundant positives
	std::map<exponent, literal> remove_redundant_positives(const std::map<exponent, literal>& positives) const {
		std::map<exponent, literal> nonredundant;

		for (auto& [exp, lit]: positives) {
			bool insert = true;
			std::set<exponent> to_remove;
			for (auto& [e, l]: nonredundant) {
				if (is_less_eq_than(lit, exp, l, e)) {
					insert = false; break;
				} else if (is_less_eq_than(l, e, lit, exp)) {
					to_remove.insert(e);
				}
			}
			if (insert) nonredundant[exp] = lit;
			for (auto& e: to_remove) nonredundant.erase(e);
		}

		return nonredundant;
	}

	partition remove_redundant_negatives(const partition& negatives) const {
		partition nonredundant;

		for (auto& [exp, lits]: negatives) {
			for(auto& lit: lits) {
				bool insert = true;
				std::set<exponent> to_remove;
				for (auto& [e, ls]: nonredundant) {
					for( auto &l: ls) {
						if (is_less_eq_than(l, e, lit, exp)) {
							insert = false; break;
						} else if (is_less_eq_than(lit, exp, l, e)) {
							to_remove.insert(e);
						}
					}
				}
				if (insert) nonredundant[exp].insert(lit);
				for (auto& e: to_remove) nonredundant.erase(e);
			}
		}

		return nonredundant;
	}

	// squeezed positives as much as possible possible.
	std::map<exponent, literal> squeeze_positives(const partition& positives) const {
		// first we squeeze positives by exponent
		std::map<exponent, literal> squeezed;
		for (auto& [exponent, literals]: positives)
			squeezed[exponent] = squeeze_positives(literals, exponent);
		// then we remove redundant positives
		return remove_redundant_positives(squeezed);
	}

	bool is_less_eq_than(const literal& l, const literal& r) const {
		auto l_cte = get_constant(l);
		auto r_cte = get_constant(r);
		if (!l_cte) return !r_cte;
		if (!r_cte) return true;
		return std::visit(_leq, l_cte.value(), r_cte.value());
	}

	literal normalize(const literals& negatives, const literal& positive, const exponent& exp) const {
		// we tacitely assume that the positive literal has a constant
		// different from 1. Otherwise, the normalizer should already
		// return F.
		auto neg_positive_cte = build_bf_neg(build_bf_constant(get_constant(positive).value()));
		// now we conjunct the previous result with the constant of n
		literals lits; lits.insert(positive);
		for (auto& negative: negatives) {
			auto n_cte = build_bf_constant(get_constant(negative));
			auto nn_cte = n_cte ? build_bf_and(neg_positive_cte, n_cte.value()) : neg_positive_cte;
			auto term = build_bf_and<BAs...>(exp);
			auto nn = build_bf_and(nn_cte, term);
			lits.insert(build_wff_neq(nn));
		}
		return build_wff_and<BAs...>(lits);
	}

	// normalize each bdd path applying Corollary 3.1 from TABA book with few
	// improvements related to the handling of negative literals.
	nso<BAs...> normalize(const bdd_path& path) const {
		// if we have no positive literals we return the conjunction of all the
		// negative literals negated. PLease note that we store the positive
		// versions of the literals in the second component of the path. Thus,
		// we need to negate them or, equivalently, to build the conjunction of
		// them, we compute the negation of the the disjunction.
		if (path.first.empty()) {
			literals negs;
			for (auto& [_, lits]: remove_redundant_negatives(path.second))
				negs.insert(lits.begin(), lits.end());
			return build_wff_neg(build_wff_or<BAs...>(negs));
		}

		// otherwise, let us consider lits the set of literals to be returned
		// conjuncted.
		partition squeezed_negatives;
		// first we squeezed positive literals...
		auto squeezed_positives = squeeze_positives(path.first);
		// ...the negatives are already squeezed (by order)
		// for every negative class (same exponent) of literals...
		for (auto& [negative_exponent, negatives]: path.second) {
			// - if no positive literal has the same exponent as n, we add n to
			//   the literals
			if (!squeezed_positives.contains(negative_exponent)) {
				squeezed_negatives[negative_exponent].insert(build_wff_neg(build_wff_or<BAs...>(negatives))); continue;
			}
			// - if the positive literal has 1 as constant we return F,
			if (!get_constant(squeezed_positives[negative_exponent]).has_value())
				return _F<BAs...>;
            // otherwise we compute the new negated literal following the Corollary 3.1
			// from TABA book.
			squeezed_negatives[negative_exponent].insert(normalize(negatives, squeezed_positives.at(negative_exponent), negative_exponent));
		}
		// we remove redundant negatives
		squeezed_negatives = remove_redundant_negatives(squeezed_negatives);
		// and add the positive terms...
		literals result;
		for (auto [_, negatives]: squeezed_negatives)
			result.insert(negatives.begin(), negatives.end());

		for (auto [_, positive]: squeezed_positives)
			result.insert(positive);
		// and return the conjunction of all the lits
		return build_wff_and<BAs...>(result);
	}

	bdd_path get_relative_path(const bdd_path& path, const literal& lit) const {
		bdd_path relative_path;
		auto exp = get_exponent(lit);
		if (path.first.contains(exp)) relative_path.first[exp] = path.first.at(exp);
		if (path.second.contains(exp)) relative_path.second[exp] = path.second.at(exp);
		return relative_path;
	}

	nso<BAs...> normalize_positive(const bdd_path& path, const literal& positive) const {
		auto relative_path = get_relative_path(path, positive);
		return normalize(add_to_positive_path(relative_path, positive));
	}

	nso<BAs...> normalize_negative(const bdd_path& path, const literal& negative) const {
		auto relative_path = get_relative_path(path, negative);
		return normalize(add_to_negative_path(relative_path, negative));
	}
};

template<typename...BAs>
nso<BAs...> operator|(const nso<BAs...>& n, const to_snf_step<BAs...>& r) {
	return r(n);
}

template<typename...BAs>
nso<BAs...> snf_bf(const nso<BAs...>& n) {
	// TODO (HIGH) give a proper implementation (call to_bdd...)
	return n | bf_reduce_canonical<BAs...>()
		| repeat_all<step<BAs...>, BAs...>(
			to_dnf_bf<BAs...>
			| elim_eqs<BAs...>)
		// TODO (MEDIUM) review after we fully normalize bf & wff
		| reduce_bf<BAs...>;
}

// We mostly follow the Remark 3.5 from the TABA book. However, we deviate at
// some points. In particular, we assume that the formula is in MNF,
// instead of in MNF+BDD. The reason behind this is that we want to avoid the
// construction of the BDD form and then traverse it. Our aim is to build the
// BDD form and traverse it afterwards.
template<typename...BAs>
nso<BAs...> snf_wff(const nso<BAs...>& n) {
	auto [_, nn] = get_inner_quantified_wff(n);
	// in the first step we apply compute the SNF of the formula, as a result we get
	// the formula in SNF with positive equal exponent literals sqeezed.
	auto first_step = build_wff_neg(nn)
		| bf_reduce_canonical<BAs...>()
		| repeat_all<step<BAs...>, BAs...>(
			unsqueeze_wff<BAs...>
			| elim_eqs<BAs...>
			| simplify_wff<BAs...>
			| push_neg_for_snf<BAs...>)
		// TODO (LOW) Lucca thinks that maybe one call is enough
		| repeat_all<to_snf_step<BAs...>, BAs...>(to_snf_step<BAs...>());
	// in the second step we compute the SNF of the negation of the the result
	// of the first step in order to squeeze the negative equal exponent literals.
	// Note that in this case we don't need to unsqueeze the formula.
	auto second_step = build_wff_neg(first_step)
		| repeat_all<to_snf_step<BAs...>, BAs...>(to_snf_step<BAs...>())
		| repeat_all<step<BAs...>, BAs...>(
			elim_eqs<BAs...>
			| simplify_wff<BAs...>
			| fix_neg_in_snf<BAs...>)
		| bf_reduce_canonical<BAs...>();
	std::map<nso<BAs...>, nso<BAs...>> changes = {{nn, second_step}};
	return replace(n, changes);
}

template<typename...BAs>
nso<BAs...> build_split_wff_using(tau_parser::nonterminal type, const nso<BAs...>& a, const nso<BAs...>& b) {
	// TODO (HIGH) check formulas, should depend on the type
	if (type == tau_parser::bf_eq) return build_wff_or(build_wff_and(a, b), build_wff_and(build_wff_neg(a), build_wff_neg(b)));
	else return build_wff_and(build_wff_or(a, build_wff_neg(b)), build_wff_or(build_wff_neg(a), b));
}

template<typename...BAs>
nso<BAs...> mnf_wff(const nso<BAs...>& n) {
	auto [_, nn] = get_inner_quantified_wff(n);
	auto mnf = nn | repeat_all<step<BAs...>, BAs...>(
			to_dnf_wff<BAs...>
			| to_mnf_wff<BAs...>)
		| repeat_all<step<BAs...>, BAs...>(
			to_mnf_wff<BAs...>)
		| reduce_wff<BAs...>;
	std::map<nso<BAs...>, nso<BAs...>> changes = {{nn, mnf}};
	return replace(n, changes);
}

template<typename...BAs>
nso<BAs...> mnf_bf(const nso<BAs...>& n) {
	return n | repeat_all<step<BAs...>, BAs...>(
			to_dnf_bf<BAs...>
			| elim_eqs<BAs...>)
		// TODO (MEDIUM) review after we fully normalize bf & wff
		| reduce_bf<BAs...>;
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