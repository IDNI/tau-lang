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
// RULE(BF_PUSH_NEGATION_INWARDS_2, "($X != $Y)' := $X = $Y.")
// RULE(BF_PUSH_NEGATION_INWARDS_3, "($X = $Y)' := $X != $Y.")
// RULE(BF_PUSH_NEGATION_INWARDS_4, "($X >= $Y)' := $X < $Y.")
// RULE(BF_PUSH_NEGATION_INWARDS_5, "($X > $Y)' := $X <= $Y.")
// RULE(BF_PUSH_NEGATION_INWARDS_6, "($X <= $Y)' := $X > $Y.")
// RULE(BF_PUSH_NEGATION_INWARDS_7, "($X < $Y)' := $X >= $Y.")

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


// TODO (LOW) delete trivial quantified formulas (i.e. ∀x. F = no_x..., ).


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
static auto neq_to_eq = make_library<BAs...>(
	WFF_PUSH_NEGATION_UPWARDS_0
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
		map<nso<BAs...>, nso<BAs...>> changes;
		bool no_disjunction = true;
		for (const auto& disjunct : select_all(nn, is_non_terminal<tau_parser::wff_or, BAs...>)) {
			no_disjunction = false;
            assert(disjunct->child.size() == 2);
			if (!is_non_terminal(tau_parser::wff_or, trim(disjunct->child[0])))
				changes[trim(disjunct->child[0])]= onf_subformula(trim(disjunct->child[0]));
            if (!is_non_terminal(tau_parser::wff_or, trim(disjunct->child[1])))
                changes[trim(disjunct->child[1])]= onf_subformula(trim(disjunct->child[1]));
		}
		if(no_disjunction) {
			changes[nn] = onf_subformula(nn);
		}
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
inline bool reduce_paths (vector<int_t>& i, vector<vector<int_t>>& paths, int_t p, bool surface = true) {
	// Candidate storage for path joining
	vector<pair<vector<int_t>*, int_t>> candidates;
	for (size_t j=0; j < paths.size(); ++j) {
		// Get Hamming distance between i and path and position of last difference
		// while different irrelevant variables make assignments incompatible
		int_t dist = 0, pos = 0;
		for (int_t k=0; k < p; ++k) {
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
				if(ranges::all_of(i, [](const auto el) {return el == 2;}))
					return paths = {}, true;
				// Continue with resulting assignment
				reduce_paths(i, paths, p, false);
			} else {
				// Resolve variable
				paths[j][pos] = 2;
				if(ranges::all_of(paths[j], [](const auto el) {return el == 2;}))
					return paths = {}, true;
				// Continue with resulting assignment
				reduce_paths(paths[j], paths, p, false);
			}
			return true;
		}
	}
	return false;
}

inline void join_paths (vector<vector<int_t>>& paths) {
	for (size_t i=0; i < paths.size(); ++i) {
		for (size_t j=i+1; j < paths.size(); ++j) {
			int_t dist = 0, pos = 0;
			bool subset_relation_decided = false, is_i_subset_of_j = true, subset_check = true;
			for (size_t k=0; k < paths[i].size(); ++k) {
			if (paths[i][k] == paths[j][k]) continue;
			else if (dist == 2) break;
			else if (paths[i][k] == 2) {
				if (!subset_relation_decided) {
					subset_relation_decided = true;
					is_i_subset_of_j = true;
				} else {
					if (!is_i_subset_of_j) {
						subset_check = false;
						break;
					}
				}
			}
			else if (paths[j][k] == 2) {
				if (!subset_relation_decided) {
					subset_relation_decided = true;
					is_i_subset_of_j = false;
				} else {
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
			} else {
				// Resolve variable in i
				paths[i][pos] = 2;
			}
		}
		}
	}
}

// Ordering function for variables from nso formula
template<typename... BAs>
auto lex_var_comp = [](const auto x, const auto y) {
#ifdef CACHE
	static map<pair<nso<BAs...>,nso<BAs...>>, bool> cache;
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

// Starting from variable at position p+1 in vars write to i which variables are irrelevant in assignment
template<typename... BAs>
void elim_vars_in_assignment (const auto& fm, const auto&vars, auto& i, const int_t p) {
	auto cvars = select_all(fm, is_child_non_terminal<tau_parser::variable, BAs...>);
	sort(cvars.begin(), cvars.end(), lex_var_comp<BAs...>);

	// Set irrelevant vars in assignment i to 2
	int_t v_iter = p+1, cv_iter = 0;
	while (v_iter < (int_t)vars.size()) {
		if (cv_iter < (int_t)cvars.size() && vars[v_iter] == cvars[cv_iter]) {
			++v_iter, ++cv_iter;
		} else {
			i[v_iter] = 2;
			++v_iter;
		}
	}
}

template<typename... BAs>
nso<BAs...> reduce2(const nso<BAs...>& fm, size_t type, bool is_cnf = false, bool all_reductions = true);

// Create assignment in formula and reduce resulting clause
template<typename... BAs>
bool assign_and_reduce(const nso<BAs...>& fm, const vector<nso<BAs...>>& vars, vector<int_t>& i, auto& dnf, int_t p) {
	// Check if all variables are assigned
	if((int_t)vars.size() == p) {
		// Normalize tau subformulas
		auto fm_simp = fm
					| (nso_transform<BAs...>)normalize_ba<BAs...>
		 			| repeat_all<step<BAs...>, BAs...>(to_nnf_bf<BAs...>)
		 			| repeat_all<step<BAs...>, BAs...>(nnf_to_dnf_bf<BAs...>);
		// Simplify coefficient
		fm_simp = reduce2(fm_simp, tau_parser::bf);

		// Do not add to dnf if the coefficient is 0
		if(is_non_terminal(tau_parser::bf_f, fm_simp->child[0]))
			return false;
		if(ranges::all_of(i, [](const auto el) {return el == 2;})) {
			//bool t = is_non_terminal(tau_parser::bf_t, fm->child[0]);
			return dnf.emplace(fm_simp, vector(0, i)), true;
		}

		auto it = dnf.find(fm_simp);
		if (it == dnf.end()) return dnf.emplace(fm_simp, vector(p==0?0:1, i)), false;
		else if (!reduce_paths(i, it->second, p)) {
			// Place coefficient together with variable assignment if no reduction happend
			it->second.push_back(i);
		} else erase_if(it->second, [](const auto& v){return v.empty();});
		return it->second.empty();
	}
	// variable was already eliminated
	if (i[p] == 2) {
		if (assign_and_reduce(fm, vars, i, dnf, p+1)) return true;
		i[p] = 0;
		return false;
	}
	// Substitute 1 and 0 for v and simplify
	const auto& v = vars[p];
	map<nso<BAs...>, nso<BAs...>> c = {{v, _1<BAs...>}};
	auto fm_v1 = replace(fm, c);
	c = {{v, _0<BAs...>}};
	auto fm_v0 = replace(fm, c);

	elim_vars_in_assignment<BAs...>(fm_v1, vars, i, p);
	if(fm_v1 == fm_v0) {
		i[p] = 2;
		if (assign_and_reduce(fm_v1, vars, i, dnf, p+1)) return true;
		i[p] = 0;
	} else {
		i[p] = 1;
		if (assign_and_reduce(fm_v1, vars, i, dnf, p+1)) return true;
		i[p] = 0;

		elim_vars_in_assignment<BAs...>(fm_v0, vars, i, p);
		i[p] = -1;
		if (assign_and_reduce(fm_v0, vars, i, dnf, p+1)) return true;
		i[p] = 0;
	}
	return false;
}

// Given a BF b, calculate the Boole normal form (DNF corresponding to the pathes to true in the BDD) of b
// where the variable order is given by the function lex_var_comp
template<typename... BAs>
nso<BAs...> bf_boole_normal_form (const nso<BAs...>& fm, bool make_paths_disjoint = false) {
	// Function can only be applied to a BF
	assert(is_non_terminal(tau_parser::bf, fm));
#ifdef TAU_CACHE
	static map<nso<BAs...>, nso<BAs...>> bf_cache_disjoint;
	static map<nso<BAs...>, nso<BAs...>> bf_cache_nondisjoint;
	if (make_paths_disjoint) {
		if (auto it = bf_cache_disjoint.find(fm);
			it != bf_cache_disjoint.end()) return it->second;
	} else {
		if (auto it = bf_cache_nondisjoint.find(fm);
			it != bf_cache_nondisjoint.end()) return it->second;
	}
#endif //TAU_CACHE
	// This defines the variable order used to calculate DNF
	// It is made canonical by sorting the variables
	auto is_var = [](const nso<BAs...>& n){return
		is_child_non_terminal(tau_parser::variable, n) ||
			is_child_non_terminal(tau_parser::uninterpreted_constant, n);};
	auto vars = select_top(fm, is_var);
	sort(vars.begin(), vars.end(), lex_var_comp<BAs...>);

	vector<int_t> i (vars.size()); // Record assignments of vars

	// Resulting DNF - make it ordered for consistency
	// Key is coefficient, value is possible variable assignments for coefficient
	map<nso<BAs...>, vector<vector<int_t>>> dnf;

	//Simplify formula as initial step
	auto fm_simp = fm;

	if(assign_and_reduce(fm_simp, vars, i, dnf, 0)) {
		assert(dnf.size() == 1);
		return dnf.begin()->first;
	}
	if(dnf.empty()) return _0<BAs...>;
	if (!make_paths_disjoint) {
		for (auto& [coeff, paths] : dnf) {
			join_paths(paths);
		}
	}
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
	if (make_paths_disjoint) {
		bf_cache_disjoint.emplace(fm, reduced_dnf);
		bf_cache_disjoint.emplace(reduced_dnf, reduced_dnf);
	}
	else {
		bf_cache_nondisjoint.emplace(fm, reduced_dnf);
		bf_cache_nondisjoint.emplace(reduced_dnf, reduced_dnf);
	}
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

inline bool is_contained_in (const vector<int_t>& i, auto& paths) {
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
vector<vector<int_t>> collect_paths(const nso<BAs...>& new_fm, bool wff, const auto& is_var, int_t var_size, auto& var_pos,
	bool& decided, bool is_cnf, bool all_reductions) {
	vector<vector<int_t>> paths;
	using tp = tau_parser;
	for (const auto& clause : get_leaves(new_fm, is_cnf ? (wff ? tp::wff_and : tp::bf_and) :
							(wff ? tp::wff_or : tp::bf_or), wff ? tp::wff : tp::bf)) {
		vector<int_t> i (var_size);
		for (int_t k = 0; k < var_size; ++k) i[k] = 2;
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
				if (i[var_pos[v]] == 1) {
					// clause is false for DNF, true for CNF
					clause_is_decided = true;
					return false;
				}
				i[var_pos[v]] = 0;
				return false;
			}
			if (is_var(n)) {
				if (i[var_pos[n]] == 0) {
					// clause is false for DNF, true for CNF
					clause_is_decided = true;
					return false;
				}
				i[var_pos[n]] = 1;
				return false;
			}
			else return true;
		};
		post_order_traverser<identity_t, decltype(var_assigner), nso<BAs...>>
			(rewriter::identity, var_assigner)(clause);
		if (clause_is_decided) continue;
		// There is at least one satisfiable clause
		decided = false;
		if (ranges::all_of(i, [](const auto el) {return el == 2;})) return {};

		if (is_contained_in(i, paths)) continue;
		if (all_reductions) {
			if (!reduce_paths(i, paths, var_size)) paths.emplace_back(move(i));
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
	nso<BAs...> reduced_fm;
	bool first = true;
	for (const auto& path : paths) {
		if (path.empty()) continue;
        bool first_var = true;
        nso<BAs...> var_path;
        for (size_t k=0; k < vars.size(); ++k) {
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
	return reduced_fm;
}

// Assume that fm is in DNF (or CNF -> set is_cnf to true)
template<typename... BAs>
nso<BAs...> reduce2(const nso<BAs...>& fm, size_t type, bool is_cnf, bool all_reductions) {
	auto is_var_wff = [](const auto& n) {
		using tp = tau_parser;
		assert(!is_non_terminal(tp::bf_neq, n));
		return is_child_non_terminal(tp::bf_eq, n)
			|| is_child_non_terminal(tp::wff_ref, n)
			|| is_child_non_terminal(tp::wff_ex, n)
			|| is_child_non_terminal(tp::wff_sometimes, n)
			|| is_child_non_terminal(tp::wff_always, n)
			|| is_child_non_terminal(tp::wff_all, n);
	};
	auto is_var_bf = [](const auto& n) {
		using tp = tau_parser;
		return is_child_non_terminal(tp::variable, n) ||
				is_child_non_terminal(tp::bf_ref, n) ||
				is_child_non_terminal(tp::bf_constant, n) ||
				is_child_non_terminal(tp::uninterpreted_constant, n);
	};
	assert(is_non_terminal(type, fm));
	// Pull negation out of equality
	bool wff = type == tau_parser::wff;
	auto new_fm = wff ? fm | repeat_all<step<BAs...>, BAs...>(neq_to_eq<BAs...>) : fm;
	vector<nso<BAs...>> vars = select_top(new_fm, is_var_wff);
	if (vars.empty()) {
		if (wff) return fm | repeat_all<step<BAs...>, BAs...>(simplify_wff<BAs...>);
		else return fm;
	}

	map<nso<BAs...>, int_t> var_pos;
	for (int_t k=0; k < (int_t)vars.size(); ++k)
		var_pos.emplace(vars[k], k);

	bool decided = true;
	auto paths = wff ? collect_paths(new_fm, wff, is_var_wff, vars.size(), var_pos,
				decided, is_cnf, all_reductions) :
			   collect_paths(new_fm, wff, is_var_bf, vars.size(), var_pos,
				decided, is_cnf, all_reductions);

	if (paths.empty()) {
		if (decided) return is_cnf ? (wff ? _T<BAs...> : _1<BAs...>) : (wff ? _F<BAs...> : _0<BAs...>);
		else return is_cnf ? (wff ? _F<BAs...> : _0<BAs...>) : (wff ? _T<BAs...> : _1<BAs...>);
	}
	if (all_reductions) join_paths(paths);

	auto reduced_fm = build_reduced_formula<BAs...>(paths, vars, is_cnf, wff);
	return wff ? reduced_fm | repeat_all<step<BAs...>, BAs...>(eq_to_neq<BAs...>) : reduced_fm;
}

template<typename... BAs>
struct wff_reduce_dnf {
	nso<BAs...> operator() (const nso<BAs...>& fm) const {
#ifdef TAU_CACHE
		static map<nso<BAs...>, nso<BAs...>> cache;
		if (auto it=cache.find(fm); it!=cache.end()) return it->second;
		auto r = reduce2(fm, tau_parser::wff);
		cache.emplace(fm, r);
		return cache.emplace(r, r).first->second;
#endif //TAU_CACHE
		return reduce2(fm, tau_parser::wff);
	}
};

template<typename... BAs>
struct wff_reduce_cnf {
	nso<BAs...> operator() (const nso<BAs...>& fm) const {
#ifdef TAU_CACHE
		static map<nso<BAs...>, nso<BAs...>> cache;
		if (auto it=cache.find(fm); it!=cache.end()) return it->second;
		auto r = reduce2(fm, tau_parser::wff, true);
		cache.emplace(fm, r);
		return cache.emplace(r, r).first->second;
#endif //TAU_CACHE
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

// A formula has a temporal variable if either it contains an io_var with a variable or capture
// or it contains a flag
template<typename... BAs>
bool has_temp_var (const nso<BAs...>& fm) {
	auto io_vars = select_top(fm, is_non_terminal<tau_parser::io_var, BAs...>);
	if (io_vars.empty()) return find_top(fm, is_non_terminal<tau_parser::flag, BAs...>).has_value();
	for (const auto& io_var : io_vars) {
		if (find_top(io_var, is_non_terminal<tau_parser::shift, BAs...>))
			return true;
		if (!find_top(io_var, is_non_terminal<tau_parser::num, BAs...>))
			return true;
	}
	return find_top(fm, is_non_terminal<tau_parser::flag, BAs...>).has_value();
}

// Assumes a sometimes formula in dnf with negation pushed in containing no wff_or with max nesting depth 1
template<typename... BAs>
nso<BAs...> extract_sometimes (nso<BAs...> fm) {
	std::map<nso<BAs...>, nso<BAs...>> l_changes = {};
	// Collect remaining nested "sometimes" formulas
	vector<nso<BAs...>> sometimes_extractions = {};
	for (const auto &inner_st: select_top(trim2(fm), is_child_non_terminal<tau_parser::wff_sometimes, BAs...>)) {
		l_changes[inner_st] = _T<BAs...>;
		sometimes_extractions.push_back(inner_st);
	}
	// Collect remaining nested "always" formulas
	vector<nso<BAs...>> always_extractions = {};
	for (const auto &inner_aw: select_top(trim2(fm), is_child_non_terminal<tau_parser::wff_always, BAs...>)) {
		l_changes[inner_aw] = _T<BAs...>;
		always_extractions.push_back(inner_aw);
	}
	// Apply always/sometimes extractions to fm
	if (!l_changes.empty()) fm = replace(fm, l_changes);

	vector<nso<BAs...>> extracted = {}, staying = {};
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
	vector<nso<BAs...>> sometimes_extractions = {};
	for (const auto &inner_st: select_top(trim2(fm), is_child_non_terminal<tau_parser::wff_sometimes, BAs...>)) {
		l_changes[inner_st] = _F<BAs...>;
		sometimes_extractions.push_back(inner_st);
	}
	// Collect remaining nested "always" formulas
	vector<nso<BAs...>> always_extractions = {};
	for (const auto &inner_aw: select_top(trim2(fm), is_child_non_terminal<tau_parser::wff_always, BAs...>)) {
		l_changes[inner_aw] = _F<BAs...>;
		always_extractions.push_back(inner_aw);
	}
	// Apply always/sometimes extractions to flat_st
	if (!l_changes.empty()) fm = replace(fm, l_changes);

	// Now extract from all disjuncts
	vector<nso<BAs...>> extracted = {}, staying = {};
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
		flat_st = flat_st | repeat_each<step<BAs...>, BAs...>(to_nnf_wff<BAs...>)
							| repeat_each<step<BAs...>, BAs...>(nnf_to_dnf_wff<BAs...>)
							| wff_reduce_dnf<BAs...>();
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
		flat_aw = flat_aw | repeat_each<step<BAs...>, BAs...>(to_nnf_wff<BAs...>)
							| repeat_each<step<BAs...>, BAs...>(nnf_to_cnf_wff<BAs...>)
							| wff_reduce_cnf<BAs...>();
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
		if (!find_top(fm, st_aw).has_value() && !has_temp_var(fm)) {
			return fm | repeat_each<step<BAs...>, BAs...>(to_nnf_wff<BAs...>)
					  	| repeat_each<step<BAs...>, BAs...>(nnf_to_dnf_wff<BAs...>)
						| wff_reduce_dnf<BAs...>();
		}
		// Scope formula under always if not already under sometimes or always
		nso<BAs...> res = !st_aw(fm) ? build_wff_always(fm) : fm;
		res = push_sometimes_always_in(res)
					// conversion to DNF necessary for pull_sometimes_always_out
					| repeat_each<step<BAs...>, BAs...>(to_nnf_wff<BAs...>)
					| repeat_each<step<BAs...>, BAs...>(nnf_to_dnf_wff<BAs...>)
					// Reduction done to normalize while sometimes/always are pushed in
					| wff_reduce_dnf<BAs...>();
		res = pull_sometimes_always_out(res) |
					repeat_each<step<BAs...>, BAs...>(simplify_wff<BAs...>);
		auto temp_inner = select_top(res, st_aw);
		if (temp_inner.empty()) return res;
		map<nso<BAs...>, nso<BAs...>> changes;
		for (const auto& f : temp_inner) {
			// Reduction done to normalize again now that sometimes/always are all the way out
			changes[trim2(f)] = trim2(f)
						| repeat_each<step<BAs...>, BAs...>(to_nnf_wff<BAs...>)
						| repeat_each<step<BAs...>, BAs...>(nnf_to_dnf_wff<BAs...>)
						| wff_reduce_dnf<BAs...>();
		}
		return replace(res, changes);
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
				nneqs = build_wff_and(nneqs, build_wff_neq(build_bf_or(
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
	scoped_fm = scoped_fm
	            // Reductions to prevent blow ups and achieve DNF
	            | bf_reduce_canonical<BAs...>()
	            | repeat_all<step<BAs...>, BAs...>(to_nnf_wff<BAs...>)
	            | repeat_all<step<BAs...>, BAs...>(nnf_to_dnf_wff<BAs...>)
	            | wff_reduce_dnf<BAs...>();
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
			if (res) res = build_wff_or(res, new_conjunct);
			else res = new_conjunct;
		}
		if (all_equal_zero) {
			nso<BAs...> new_func;
			for (const auto& d: conjuncts) {
				if (new_func) new_func = build_bf_or(new_func, trim2(d));
				else new_func = trim2(d);
			}
			new_func = build_wff_eq(new_func | bf_reduce_canonical<BAs...>());
			new_func = wff_remove_existential(trim2(inner_fm), new_func);
			if (res) res = build_wff_or(res, new_func);
			else res = new_func;
		}
		else {
			// Resolve quantified variable in scoped_fm
			if (res) res = build_wff_or(res, wff_remove_existential(trim2(inner_fm), clause));
			else res = wff_remove_existential(trim2(inner_fm), clause);
		}
	}
	return res;
}

template<typename... BAs>
nso<BAs...> eliminate_universal_quantifier(const auto& inner_fm, auto& scoped_fm) {
	scoped_fm = scoped_fm
	            // Reductions to prevent blow ups and achieve CNF
	            | bf_reduce_canonical<BAs...>()
	            | repeat_all<step<BAs...>, BAs...>(to_nnf_wff<BAs...>)
	            | repeat_all<step<BAs...>, BAs...>(nnf_to_cnf_wff<BAs...>)
	            | wff_reduce_cnf<BAs...>();
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
				auto new_d = build_wff_neg(d) | repeat_all<step<BAs...>, BAs...>(to_nnf_wff<BAs...>);
				new_d = build_wff_neg(wff_remove_existential(trim2(inner_fm), new_d))
				        | repeat_all<step<BAs...>, BAs...>(to_nnf_wff<BAs...>);
				if (new_disjunct) new_disjunct = build_wff_or(new_disjunct, new_d);
				else new_disjunct = new_d;
			}
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
			new_func = build_wff_neg(wff_remove_existential(trim2(inner_fm), new_func))
			           | repeat_all<step<BAs...>, BAs...>(to_nnf_wff<BAs...>);
			if (res) res = build_wff_and(res, new_func);
			else res = new_func;
		} else {
			// Turn universal into existential quantifier and eliminate
			auto new_clause = build_wff_neg(clause)
			                  | repeat_all<step<BAs...>, BAs...>(to_nnf_wff<BAs...>);
			new_clause = build_wff_neg(wff_remove_existential(trim2(inner_fm), new_clause))
			             | repeat_all<step<BAs...>, BAs...>(to_nnf_wff<BAs...>);
			if (!res) res = new_clause;
			else res = build_wff_and(res, new_clause);
		}
	}
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
	return post_order_recursive_traverser<nso<BAs...>>()(fm, is_not_bf, elim_quant);
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
			auto term = build_bf_and(exp);
			auto nn = build_bf_and(nn_cte, term);
			lits.insert(build_wff_neq(nn));
		}
		return build_wff_and(lits);
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
			return build_wff_neg(build_wff_or(negs));
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
				squeezed_negatives[negative_exponent].insert(build_wff_neg(build_wff_or(negatives))); continue;
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
		return build_wff_and(result);
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