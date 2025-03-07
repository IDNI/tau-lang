// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __NORMAL_FORMS_H__
#define __NORMAL_FORMS_H__

#include <list>
#include <numeric>

#include "boolean_algebras/nso_ba.h"
#include "execution.h"

#ifdef DEBUG
#include "debug_helpers.h"
#endif // DEBUG

namespace idni::tau_lang {

/**
 * This enum holds the possible memory slots for traversals using the
 * pre_order class
 */
enum MemorySlotPre {
	normalize_ba_m,
	push_negation_in_m,
	to_dnf2_m,
	to_cnf2_m,
	eliminate_quantifiers_m,
	anti_prenex_step_m
};

/**
 * This enum holds the possible memory slots for traversals using the
 * post_order class
 */
enum MemorySlotPost {
	anti_prenex_m
};

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
static auto push_sometimes_in = make_library<BAs...>(
	WFF_PUSH_SOMETIMES_INWARDS
);

template<typename... BAs>
static auto push_always_in = make_library<BAs...>(
	WFF_PUSH_ALWAYS_INWARDS
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

template <typename... BAs>
tau<BAs...> squeeze_wff (const tau<BAs...>& fm) {
	//$X = 0 && $Y = 0 ::= $X | $Y = 0
	// $X != 0 || $Y != 0 ::= $X | $Y != 0
	using p = tau_parser;
	auto f = [](const tau<BAs...>& n) {
		if (is_non_terminal(p::wff_and, n)) {
			const auto& e1 = trim(n);
			const auto& e2 = n->child[1];
			if (is_child_non_terminal(p::bf_eq, e1) &&
				is_child_non_terminal(p::bf_eq, e2)) {
				return trim(build_wff_eq(
					build_bf_or(trim2(e1), trim2(e2))));
				}
		}
		if (is_non_terminal(p::wff_or, n)) {
			const auto& e1 = trim(n);
			const auto& e2 = n->child[1];
			if (is_child_non_terminal(p::bf_neq, e1) &&
				is_child_non_terminal(p::bf_neq, e2)) {
				return trim(build_wff_neq(
					build_bf_or(trim2(e1), trim2(e2))));
				}
		}
		return n;
	};
	return post_order(fm).apply_unique(f, visit_wff<BAs...>);
}

template <typename... BAs>
tau<BAs...> unsqueeze_wff_pos (const tau<BAs...>& fm) {
	// $X | $Y = 0 ::= $X = 0 && $Y = 0
	using p = tau_parser;
	auto f = [](const tau<BAs...>& n) {
		if (is_non_terminal(p::bf_eq, n)) {
			const auto& e = trim2(n);
			if (is_non_terminal(p::bf_or, e)) {
				const auto& c1 = trim(e);
				const auto& c2 = e->child[1];
				return trim(build_wff_and(
					build_wff_eq(c1),
					build_wff_eq(c2)));
			}
		}
		return n;
	};
	return pre_order(fm).apply_unique(f, visit_wff<BAs...>, identity);
}

template <typename... BAs>
tau<BAs...> squeeze_wff_pos (const tau<BAs...>& fm) {
	// $X = 0 && $Y = 0 ::= $X | $Y = 0
	using p = tau_parser;
	auto f = [](const tau<BAs...>& n) {
		if (is_non_terminal(p::wff_and, n)) {
			const auto& e1 = trim(n);
			const auto& e2 = n->child[1];
			if (is_child_non_terminal(p::bf_eq, e1) &&
				is_child_non_terminal(p::bf_eq, e2)) {
				return trim(build_wff_eq(
					build_bf_or(trim2(e1), trim2(e2))));
			}
		}
		return n;
	};
	return post_order(fm).apply_unique(f, visit_wff<BAs...>);
}

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
// General operator for tau<BAs...> function application by pipe
template<typename... BAs>
using tau_f = tau<BAs...>(*)(const tau<BAs...>&);

template<typename... BAs>
tau<BAs...> operator| (const tau<BAs...>& fm, const tau_f<BAs...> func) {
	return func(fm);
}
// --------------------------------------------------------------

// This function traverses n and normalizes coefficients in a BF
template <typename... BAs>
tau<BAs...> normalize_ba(const tau<BAs...>& fm) {
	assert(is_non_terminal(tau_parser::bf, fm));
	auto norm_ba = [&](const auto& n) {
		using p = tau_parser;

		if (!is_child_non_terminal(p::bf_constant, n))
			return n;

		// Node has a Boolean algebra element
		auto ba_elem = n
			| p::bf_constant
			| p::constant
			| only_child_extractor<BAs...>
			| ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		auto res = normalize_ba(ba_elem);
		auto type = n | p::bf_constant | p::type;
		assert(type.has_value());
		return build_bf_constant(res, type.value());
	};
	return pre_order(fm).template
	apply_unique_until_change<MemorySlotPre::normalize_ba_m>(norm_ba);
}

template<typename... BAs>
struct bf_reduce_canonical;

template<tau_parser::nonterminal type, typename... BAs>
struct reduce {

	// TODO (VERY_HIGH) properly implement it
	tau<BAs...> operator()(const tau<BAs...>& form) const {
		std::map<tau<BAs...>, tau<BAs...>> changes;
		// for all type dnfs do...
		for (auto& dnf: select_top(form, is_non_terminal<type, BAs...>)) {
			auto simplified = simplify(dnf);
			if (simplified != dnf) changes[dnf] = simplified;
		}
		return replace(form, changes);
	}

private:

	void get_literals(const tau<BAs...>& clause, std::set<tau<BAs...>>& literals) const {
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

	std::set<tau<BAs...>> get_literals(const tau<BAs...>& clause) const {
		std::set<tau<BAs...>> literals;
		get_literals(clause, literals);
		return literals;
	}

	std::pair<std::set<tau<BAs...>>, std::set<tau<BAs...>>> get_positive_negative_literals(const tau<BAs...> clause) const {
		std::set<tau<BAs...>> positives;
		std::set<tau<BAs...>> negatives;

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

	std::set<tau<BAs...>> get_dnf_clauses(const tau<BAs...>& n, std::set<tau<BAs...>> clauses = {}) const {
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

	std::optional<tau<BAs...>> build_dnf_clause_from_literals(const std::set<tau<BAs...>>& positives, const std::set<tau<BAs...>>& negatives) const {
		if (positives.empty() && negatives.empty()) {
			BOOST_LOG_TRIVIAL(debug) << "(F) {}";
			return {};
		}

		std::vector<tau<BAs...>> literals;
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

	std::optional<tau<BAs...>> to_minterm(const tau<BAs...>& clause) const {
		auto [positives, negatives] = get_positive_negative_literals(clause);
		if constexpr (type == tau_parser::bf) {
			for (auto& negation: negatives) {
				auto negated = negation
					| tau_parser::bf_neg
					| tau_parser::bf
					| optional_value_extractor<tau<BAs...>>;
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
					| optional_value_extractor<tau<BAs...>>;
				for (auto& positive: positives) {
					auto eq_bf = positive
						| first_child_extractor<BAs...>
						| tau_parser::bf
						| optional_value_extractor<tau<BAs...>>;
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

	tau<BAs...> build_dnf_from_clauses(const std::set<tau<BAs...>>& clauses) const {
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

	tau<BAs...> simplify(const tau<BAs...>& form) const {
		std::set<tau<BAs...>> clauses;
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
tau<BAs...> operator|(const tau<BAs...>& n, const reduce_bf_t<BAs...>& r) {
	return r(n);
}

template<typename... BAs>
static const reduce<tau_parser::wff, BAs...> reduce_wff;
template<typename... BAs>
using reduce_wff_t = reduce<tau_parser::wff, BAs...>;

template<typename...BAs>
tau<BAs...> operator|(const tau<BAs...>& n, const reduce_wff_t<BAs...>& r) {
	return r(n);
}

// return the inner quantifier or the top wff if the formula is not quantified
template<typename...BAs>
std::pair<std::optional<tau<BAs...>>, tau<BAs...>> get_inner_quantified_wff(const tau<BAs...>& n) {
	// TODO (LOW) extract to a utils file
	auto quantified = [](const auto& n) -> bool {
		return is_non_terminal<tau_parser::wff_ex, BAs...>(n)
			|| is_non_terminal<tau_parser::wff_all, BAs...>(n)
			|| is_non_terminal<tau_parser::wff_sometimes, BAs...>(n)
			|| is_non_terminal<tau_parser::wff_always, BAs...>(n);
	};
	if (auto quantifier = find_bottom(n, quantified); quantifier) {
		return { quantifier | tau_parser::variable,
			quantifier | tau_parser::wff | optional_value_extractor<tau<BAs...>> };
	}
	return { {}, n };
}

template<typename...BAs>
struct onf_wff {

	onf_wff(const tau<BAs...>& _var) {
		if (!is_non_terminal(tau_parser::bf, _var))
			var = wrap(tau_parser::bf, _var);
		else var = _var;
	}

	tau<BAs...> operator()(const tau<BAs...>& n) const {
		auto [var, nn] = get_inner_quantified_wff(n);
		// We assume that the formula is in DNF. In particular nn is in DNF
		// For each disjunct calculate the onf
		std::map<tau<BAs...>, tau<BAs...>> changes;
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
	tau<BAs...> onf_subformula(const tau<BAs...>& n) const {
		auto has_var = [&](const auto& node){ return node == var; };

		auto eq = find_bottom(n, is_non_terminal<tau_parser::bf_eq, BAs...>);
		std::map<tau<BAs...>, tau<BAs...>> changes;
		if (eq && find_top(trim(eq.value()), has_var)) {
			auto eq_v = eq.value();
			assert(is_non_terminal(tau_parser::bf_f, trim(eq_v->child[1])));
			auto f_0 = replace(trim(eq_v), var, _0<BAs...>);
			f_0 = f_0 | bf_reduce_canonical<BAs...>();
			auto f_1 = replace(build_bf_neg(trim(eq_v)), var, _1<BAs...>)
				| bf_reduce_canonical<BAs...>();

			changes[eq_v] = trim(build_bf_interval(f_0, var, f_1));
		}
		for (const auto& neq: select_all(n, is_non_terminal<tau_parser::bf_neq, BAs...>)) {
			assert(is_non_terminal(tau_parser::bf_f, trim(neq->child[1])));
			if (!find_top(trim(neq), has_var)) continue;
			auto f_0 = replace(trim(neq), var, _0<BAs...>)
				| bf_reduce_canonical<BAs...>();
			auto f_1 = replace(build_bf_neg(trim(neq)), var, _1<BAs...>)
				| bf_reduce_canonical<BAs...>();
			changes[neq] = trim(build_wff_or(build_bf_nleq_upper(f_0, var), build_bf_nleq_lower(f_1, var)));
		}
		return replace(n, changes);
	}

	tau<BAs...> var;
};

template<typename... BAs>
using onf_wff_t = onf_wff<BAs...>;

template<typename...BAs>
tau<BAs...> operator|(const tau<BAs...>& n, const onf_wff_t<BAs...>& r) {
	return r(n);
}

template<typename... BAs>
static const auto is_not_eq_or_neq_to_zero_predicate = [](const tau<BAs...>& n) {
	if (!(is_non_terminal(tau_parser::bf_eq, trim(n)) || is_non_terminal(tau_parser::bf_neq, trim(n))))
		return true;
	auto check = (n | only_child_extractor<BAs...> || tau_parser::bf)[1] || tau_parser::bf_f;
	return check.empty();
};

template<typename... BAs>
using is_not_eq_or_neq_predicate_t = decltype(is_not_eq_or_neq_to_zero_predicate<BAs...>);

template<typename...BAs>
std::optional<tau<BAs...>> onf(const tau<BAs...>& n, const tau<BAs...>& var) {
	// FIXME take into account quiantifiers
	return n
		| repeat_all<step<BAs...>, BAs...>(to_dnf_wff<BAs...>)
		| onf_wff<BAs...>(var)
		| repeat_all<step<BAs...>, BAs...>(
			to_dnf_wff<BAs...>
			| simplify_wff<BAs...>);
}

template<typename...BAs>
tau<BAs...> dnf_wff(const tau<BAs...>& n) {
	auto [_, nn] = get_inner_quantified_wff(n);
	auto nform = nn
		| repeat_each<step<BAs...>, BAs...>(
			to_dnf_wff<BAs...>
			| simplify_wff<BAs...>
		);
	// finally, we also simplify the bf part of the formula
	auto dnf = dnf_bf(nform);
	return replace(n, nn, dnf);
}

template<typename...BAs>
tau<BAs...> dnf_bf(const tau<BAs...>& n) {
	return n | repeat_all<step<BAs...>, BAs...>(
			to_dnf_bf<BAs...>
			| elim_eqs<BAs...>)
		// TODO (MEDIUM) review after we fully normalize bf & wff
		| reduce_bf<BAs...>;
}

template<typename...BAs>
tau<BAs...> cnf_wff(const tau<BAs...>& n) {
	auto [var, nn] = get_inner_quantified_wff(n);
	auto wff = nn | repeat_each<step<BAs...>, BAs...>(
			to_cnf_wff<BAs...>
			| simplify_wff<BAs...>
		);
	// finally, we also simplify the bf part of the formula
	auto cnf = cnf_bf(wff);
	return replace(n, nn, cnf);
}

template<typename...BAs>
tau<BAs...> cnf_bf(const tau<BAs...>& n) {
	return n

		| repeat_all<step<BAs...>, BAs...>(
			to_cnf_bf<BAs...>
			| elim_eqs<BAs...>)
		// TODO (MEDIUM) review after we fully normalize bf & wff
		| reduce_bf<BAs...>;
}

template<typename...BAs>
tau<BAs...> nnf_bf(const tau<BAs...>& n) {
	return n | repeat_all<step<BAs...>, BAs...>(
			to_nnf_bf<BAs...>
			| elim_eqs<BAs...>)
		// TODO (MEDIUM) review after we fully normalize bf & wff
		| reduce_bf<BAs...>;
}

template<typename...BAs>
tau<BAs...> nnf_wff(const tau<BAs...>& n) {
	auto [_, nn] = get_inner_quantified_wff(n);
	auto nform = nn | repeat_each<step<BAs...>, BAs...>(
			to_nnf_wff<BAs...>
			| simplify_wff<BAs...>
		);
	// finally, we also simplify the bf part of the formula
	auto nnf = nnf_bf(nform);
	return replace(n, nn, nnf);
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
#ifdef TAU_CACHE
	static std::map<std::pair<tau<BAs...>, tau<BAs...>>, bool> cache;
	if (auto it = cache.find({x,y}); it != cache.end())
		return it->second;
#endif // TAU_CACHE
	auto xx = make_string(tau_node_terminal_extractor<BAs...>, x);
	auto yy = make_string(tau_node_terminal_extractor<BAs...>, y);
#ifdef TAU_CACHE
	std::pair<tau<BAs...>,tau<BAs...>> p(x,y);
	return cache.emplace(move(p), xx < yy).first->second;
#endif // TAU_CACHE
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
	// auto is_var = [](const tau<BAs...>& n){return
	// 	is_child_non_terminal(tau_parser::variable, n) ||
	// 		is_child_non_terminal(tau_parser::uninterpreted_constant, n);};
	auto cvars = select_all(fm, is_var);
	std::set<tau<BAs...>> cur_vars(std::make_move_iterator(cvars.begin()),
              std::make_move_iterator(cvars.end()));

	for (size_t v_iter = p+1; v_iter < vars.size(); ++v_iter) {
		if (!cur_vars.contains(vars[v_iter]))
			i[v_iter] = 2;
	}
}

// Declaration of functions used in assign_and_reduce which are implemented later
template<typename... BAs>
tau<BAs...> reduce2(const tau<BAs...>& fm, size_t type, bool is_cnf = false, bool all_reductions = true, bool enable_sort = true);
template<typename... BAs>
tau<BAs...> to_dnf2(const tau<BAs...>&, bool is_wff = true);
template<typename... BAs>
tau<BAs...> to_cnf2(const tau<BAs...>&, bool is_wff = true);
template<typename... BAs>
tau<BAs...> push_negation_in(const tau<BAs...>&, bool is_wff = true);

// Create assignment in formula and reduce resulting clause
template<typename... BAs>
bool assign_and_reduce(const tau<BAs...>& fm,
	const std::vector<tau<BAs...>>& vars,
	std::vector<int_t>& i, auto& dnf, const auto& is_var, int_t p = 0,
	bool is_wff = false)
{
	// Check if all variables are assigned
	if((int_t)vars.size() == p) {
		tau<BAs...> fm_simp;
		if (!is_wff) {
			// Do not add to dnf if the coefficient is 0
			if (fm == _0<BAs...>) return false;
			// fm is a Boolean function
			// Normalize tau subformulas
			fm_simp = normalize_ba<BAs...>(fm);
			if (fm_simp == _0<BAs...>) return false;
			fm_simp = to_dnf2(fm_simp, false);
			if (fm_simp == _0<BAs...>) return false;
			fm_simp = reduce2(fm_simp, tau_parser::bf);
			if (fm_simp == _0<BAs...>) return false;
		} else {
			if (fm == _F<BAs...>) return false;
			// fm is a Tau formula
			fm_simp = to_dnf2(fm);
			if (fm_simp == _F<BAs...>) return false;
			fm_simp = reduce2(fm_simp, tau_parser::wff);
			if (fm_simp == _F<BAs...>) return false;
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
	auto fm_v1 = replace(fm, v, t);
	auto fm_v0 = replace(fm, v, f);

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
tau<BAs...> bf_boole_normal_form (const tau<BAs...>& fm,
	bool make_paths_disjoint = false)
{
	// Function can only be applied to a BF
	assert(is_non_terminal(tau_parser::bf, fm));
#ifdef TAU_CACHE
	static std::map<std::pair<tau<BAs...>, bool>, tau<BAs...>> cache;
    if (auto it = cache.find(make_pair(fm, make_paths_disjoint));
    		it != cache.end()) return it->second;
#endif //TAU_CACHE
	// This defines the variable order used to calculate DNF
	// It is made canonical by sorting the variables
	auto is_var = [](const tau<BAs...>& n){return
		is_child_non_terminal(tau_parser::variable, n) ||
			is_child_non_terminal(tau_parser::uninterpreted_constant, n);};
	auto vars = select_top(fm, is_var);
	sort(vars.begin(), vars.end(), lex_var_comp<BAs...>);

	std::vector<int_t> i(vars.size()); // Record assignments of vars

	// Resulting DNF - make it ordered for consistency
	// Key is coefficient, value is possible variable assignments for coefficient
	unordered_tau_map<std::vector<std::vector<int_t>>, BAs...> dnf;

	if(assign_and_reduce(fm, vars, i, dnf, is_var, 0)) {
		assert(dnf.size() == 1);
		return dnf.begin()->first;
	}
	if(dnf.empty()) return _0<BAs...>;
	if (!make_paths_disjoint)
		for (auto& [coeff, paths] : dnf)
			join_paths(paths);

	// Convert map structure dnf back to rewrite tree
	tau<BAs...> reduced_dnf;
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
			tau<BAs...> var_path;
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
	tau<BAs...> operator() (const tau<BAs...>& fm) const {
		std::map<tau<BAs...>, tau<BAs...>> changes = {};
		for (const auto& bf: select_top(fm, is_non_terminal<tau_parser::bf, BAs...>)) {
			auto dnf = bf_boole_normal_form(bf);
			if (dnf != bf) changes[bf] = dnf;
		}
		if (changes.empty()) return fm;
		else return replace(fm, changes);
	}
};

template<typename... BAs>
tau<BAs...> operator|(const tau<BAs...>& fm, const bf_reduce_canonical<BAs...>& r) {
	return r(fm);
}

template<typename... BAs>
std::optional<tau<BAs...>> operator|(const std::optional<tau<BAs...>>& fm, const bf_reduce_canonical<BAs...>& r) {
	return fm.has_value() ? r(fm.value()) : std::optional<tau<BAs...>>{};
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
std::pair<std::vector<int_t>, bool> clause_to_vector(const tau<BAs...>& clause,
	const auto& var_pos, const bool wff, const bool is_cnf)
{
	using tp = tau_parser;
	std::vector<int_t> i (var_pos.size());
	for (size_t k = 0; k < var_pos.size(); ++k) i[k] = 2;
	bool clause_is_decided = false;
	auto var_assigner = [&](const tau<BAs...>& n) {
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
	pre_order(clause).visit_unique(var_assigner);
	return std::make_pair(move(i), clause_is_decided);
}

template<typename... BAs>
std::vector<std::vector<int_t>> collect_paths(const tau<BAs...>& new_fm, bool wff,
	const auto& vars, bool& decided, bool is_cnf, bool all_reductions = true) {
	std::vector<std::vector<int_t>> paths;
	unordered_tau_map<int_t, BAs...> var_pos;
	for (int_t k=0; k < (int_t)vars.size(); ++k)
		var_pos.emplace(vars[k], k);
	using tp = tau_parser;
	for (const auto& clause: get_leaves(
			new_fm, is_cnf
					? (wff ? tp::wff_and : tp::bf_and)
					: (wff ? tp::wff_or : tp::bf_or))) {
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
tau<BAs...> build_reduced_formula (const auto& paths, const auto& vars, bool is_cnf, bool wff) {
	if (paths.empty()) return is_cnf
					  ? (wff ? _T<BAs...> : _1<BAs...>)
					  : (wff ? _F<BAs...> : _0<BAs...>);
	tau<BAs...> reduced_fm = is_cnf
					 ? (wff ? _F<BAs...> : _0<BAs...>)
					 : (wff ? _T<BAs...> : _1<BAs...>);
	bool first = true;
	for (const auto& path : paths) {
		if (path.empty()) continue;
        bool first_var = true;
		tau<BAs...> var_path = is_cnf
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
	//TODO: avoid pattern matcher
	return reduced_fm | repeat_all<step<BAs...>, BAs...>(eq_to_neq<BAs...>);
}

template<typename... BAs>
tau<BAs...> sort_var (const tau<BAs...>& var) {
#ifdef TAU_CACHE
	static unordered_tau_map<tau<BAs...>, BAs...> cache;
	if (auto it = cache.find(var); it != end(cache))
		return it->second;
#endif // TAU_CACHE
	if (is_child_non_terminal(tau_parser::bf_eq, var)) {
		auto clauses = get_dnf_bf_clauses(trim2(var));
		std::ranges::sort(clauses);
		tau<BAs...> res;
		for (const auto& c : clauses) {
			auto lits = get_cnf_bf_clauses(c);
			std::ranges::sort(lits, lex_var_comp<BAs...>);
			if (res) res = build_bf_or(res, build_bf_and<BAs...>(lits));
			else res = build_bf_and<BAs...>(lits);
		}
		res = build_wff_eq(res);
#ifdef TAU_CACHE
		cache.emplace(res, res);
		return cache.emplace(var, res).first->second;
#endif // TAU_CACHE
		return res;
	}
	return var;
}

template<typename... BAs>
std::pair<std::vector<std::vector<int_t>>, std::vector<tau<BAs...>>> dnf_cnf_to_bdd(
	const tau<BAs...>& fm, size_t type, bool is_cnf = false,
	bool all_reductions = true, bool enable_sort = true)
{
	assert(is_non_terminal(type, fm));
	// Pull negation out of equality
	bool wff = type == tau_parser::wff;
	//TODO: avoid patternmatcher
	auto new_fm = wff ? fm | repeat_all<step<BAs...>, BAs...>(to_mnf_wff<BAs...>) : fm;
	if (wff) {
		// Make equalities canonical
		std::map<tau<BAs...>, tau<BAs...> > changes;
		auto eqs = select_top(new_fm, is_child_non_terminal<tau_parser::bf_eq, BAs...>);
		for (const auto& eq: eqs) {
			auto sorted_eq = sort_var(eq);
			if (sorted_eq != eq) {
				changes.emplace(eq, sorted_eq);
			}
		}
		new_fm = replace(new_fm, changes);
	}

	std::vector<tau<BAs...> > vars = wff
					    ? select_top(new_fm, is_wff_bdd_var)
					    : select_top(new_fm, is_bf_bdd_var);
	if (vars.empty()) {
		//assert(new_fm == _T<BAs...> || new_fm == _F<BAs...> ||
		//	new_fm == _1<BAs...> || new_fm == _0<BAs...>);
		// print_tau_tree(std::cout, new_fm);
		if (new_fm == _T<BAs...> || new_fm == _1<BAs...>) {
			if (is_cnf) return {};
			std::vector<std::vector<int_t>> paths;
			paths.emplace_back();
			return make_pair(std::move(paths), std::move(vars));
		} else {
			if (is_cnf) {
				std::vector<std::vector<int_t>> paths;
				paths.emplace_back();
				return make_pair(std::move(paths), std::move(vars));
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
	if (enable_sort) std::ranges::sort(paths);
	return make_pair(move(paths), move(vars));
}

template<typename... BAs>
tau<BAs...> group_dnf_expression (const tau<BAs...>& fm) {
#ifdef TAU_CACHE
		static unordered_tau_map<tau<BAs...>, BAs...> cache;
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

	std::vector<std::vector<tau<BAs...>>> atoms_of_clauses;
	for (const auto& clause : clauses) {
		auto atoms = wff ? get_cnf_wff_clauses(clause)
				     : get_cnf_bf_clauses(clause);
		if(wff) std::ranges::sort(atoms);
		else std::ranges::sort(atoms, lex_var_comp<BAs...>);
		atoms_of_clauses.emplace_back(move(atoms));
	}
	tau<BAs...> grouped_fm = wff ? _F<BAs...> : _0<BAs...>;
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
		std::vector<tau<BAs...>> common;
		std::ranges::set_intersection(atoms_of_clauses[i],
					 atoms_of_clauses[max_common.first],
					 back_inserter(common));

		tau<BAs...> cl1 = wff ? _T<BAs...> : _1<BAs...>;
		tau<BAs...> cl2 = wff ? _T<BAs...> : _1<BAs...>;
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
		tau<BAs...> grouped = wff ? build_wff_or(cl1, cl2) :
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

// Simplifies for example abx'|aby'|xy to ab|xy
// General idea is to eliminate (xyz...)' | xyz... after factorization
template<typename... BAs>
tau<BAs...> simp_general_excluded_middle (const tau<BAs...>& fm) {
	assert(is_non_terminal(tau_parser::bf, fm));
	bool was_simplified = false;
	auto grouped = group_dnf_expression(fm);
	auto clauses = get_dnf_bf_clauses(grouped);
	for (int_t i = 0; i < (int_t)clauses.size(); ++i) {
		bool erase_clause = false;
		auto cn = push_negation_in(build_bf_neg(clauses[i]), false);
		for (int_t j = 0; j < (int_t)clauses.size(); ++j) {
			if (i==j) continue;
			bool has_simp = false;
			// TODO: generalize to recursion on grouped expression, for example for abx' | aby' | axy
			auto cnf = get_cnf_bf_clauses(clauses[j]);
			for (size_t k = 0; k < cnf.size(); ++k) {
				// TODO: generalize to subset
				if (cnf[k] == cn) {
					cnf.erase(cnf.begin() + k);
					has_simp = true;
					was_simplified = true;
					if (cnf.empty()) {
						erase_clause = true;
					}
					break;
				}
			}
			if (has_simp) {
				clauses.erase(clauses.begin() + j);
				clauses.emplace_back(build_bf_and<BAs...>(cnf));
				--j;
				if (i >= j) --i;
			}
		}
		if (erase_clause) {
			clauses.erase(clauses.begin() + i);
			--i;
		}
	}
	return was_simplified ? to_dnf2(build_bf_or<BAs...>(clauses), false) : fm;
}

// Assume that fm is in DNF (or CNF -> set is_cnf to true)
// TODO: Normalize Tau constants in case type == bf
template<typename... BAs>
tau<BAs...> reduce2(const tau<BAs...>& fm, size_t type, bool is_cnf, bool all_reductions, bool enable_sort) {
	//TODO: cache enable_sort option
#ifdef TAU_CACHE
		static std::map<std::pair<tau<BAs...>, bool>, tau<BAs...>> cache;
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
tau<BAs...> reduce_terms (const tau<BAs...>& fm, bool with_sorting = false) {
    std::map<tau<BAs...>, tau<BAs...>> changes = {};
    for (const auto& bf: select_top(fm, is_non_terminal<tau_parser::bf, BAs...>)) {
        auto dnf = to_dnf2(bf, false);
        dnf = reduce2(dnf, tau_parser::bf, false, true, with_sorting);
        if (dnf != bf) changes[bf] = dnf;
    }
    if (changes.empty()) return fm;
    else return replace(fm, changes);
}

// Apply equalities across clauses for further simplifications
// Assumes that fm is in DNF with equalities pushed in and
// inequalities pushed out
template<typename... BAs>
tau<BAs...> apply_eqs_across_clauses (const tau<BAs...>& fm) {
	std::vector<tau<BAs...>> eq_reductions, neq_reductions;
	auto clauses = get_dnf_wff_clauses(fm);
	for (const auto& clause : clauses) {
		auto lits = get_cnf_wff_clauses(clause);
		if (lits.size() == 1) {
			if (is_child_non_terminal(tau_parser::bf_eq, lits[0]))
				eq_reductions.emplace_back(trim2(lits[0]));
			else if (is_child_non_terminal(tau_parser::bf_neq, lits[0])) {
				auto dnf_lits = get_dnf_bf_clauses(trim2(lits[0]));
				for (const auto& lit : dnf_lits)
					neq_reductions.emplace_back(lit);
			}
		}
	}
	if (eq_reductions.empty() && neq_reductions.empty()) return fm;
	tau<BAs...> new_fm;
	std::map<tau<BAs...>, tau<BAs...>> changes;
	for (const auto& clause : clauses) {
		auto neqs = select_top(clause, is_child_non_terminal<tau_parser::bf_neq, BAs...>);
		for (const auto& neq : neqs) {
			bool replace = false;
			auto neq_lits = get_dnf_bf_clauses(trim2(neq));
			for (const auto& eq : eq_reductions) {
				for (auto& neq_lit : neq_lits) {
					if (eq == neq_lit) {
						changes.emplace(neq, _T<BAs...>);
						replace = true;
					}
					if (replace) break;
				}
				if (replace) break;
			}
		}
	}
	if (!changes.empty()) {
		new_fm = replace(fm, changes);
		if (neq_reductions.empty()) return new_fm;
		clauses = get_dnf_wff_clauses(new_fm);
	}
	if (neq_reductions.empty()) return fm;
	changes.clear();
	for (const auto& clause : clauses) {
		auto eqs = select_top(clause, is_child_non_terminal<tau_parser::bf_eq, BAs...>);
		for (const auto& eq : eqs) {
			for (const auto& neq : neq_reductions) {
				if (trim2(eq) == neq) {
					changes.emplace(eq, _T<BAs...>);
					break;
				}
			}
		}
	}
	if (!changes.empty()) {
		if (new_fm) return replace(new_fm, changes);
		else return replace(fm, changes);
	}
	if (new_fm) return new_fm;
	else return fm;
}

template<typename... BAs>
std::vector<tau<BAs...>> push_eq_and_get_vars (tau<BAs...>& fm) {
 // First push in equalities all the way (bf != 0 is converted to !(bf = 0))
	//TODO: avoid pattern matcher
	fm = fm | repeat_all<step<BAs...>, BAs...>(
		unsqueeze_wff<BAs...> | to_mnf_wff<BAs...>);
	// Find atomic formulas
	return select_top(fm, is_wff_bdd_var);
}

// Requires fm to have Boolean functions in DNF and equalities pushed in
// vars contains variable order
template<typename... BAs>
std::vector<std::vector<int_t>> wff_to_bdd (const tau<BAs...>& fm, auto& vars = {}) {
	// Find atomic formulas
	auto pushed_in_fm = fm;
	if (vars.empty()) vars = push_eq_and_get_vars(pushed_in_fm);
	//TODO: Avoid pattern matcher
	else pushed_in_fm = fm | repeat_all<step<BAs...>, BAs...>(
		 	unsqueeze_wff<BAs...> | to_mnf_wff<BAs...>);
	std::vector<int_t> i (vars.size());
	std::map<tau<BAs...>, std::vector<std::vector<int_t>>> dnf;
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
std::vector<std::vector<std::vector<tau<BAs...>>>> get_cnf_inequality_lits(
	const tau<BAs...>& fm)
{
	// TODO: avoid pattern matcher
	auto neq_pushed_in = fm | repeat_all<step<BAs...>, BAs...>(
		 	unsqueeze_wff_neg<BAs...>);
	if (neq_pushed_in == _T<BAs...>) return {};
	// cout << "neq_pushed_in: " << neq_pushed_in << "\n";
	std::vector<std::vector<std::vector<tau<BAs...>>>> cnf_lits;
	for (const tau<BAs...>& clause : get_cnf_wff_clauses(neq_pushed_in)) {
		std::vector<std::vector<tau<BAs...>>> c;
		for (const tau<BAs...>& neqs : get_dnf_wff_clauses(clause)) {
			c.emplace_back(get_cnf_bf_clauses(trim2(neqs)));
		}
		cnf_lits.emplace_back(move(c));
	}
	return cnf_lits;
}

template<typename... BAs>
std::pair<std::vector<int_t>, bool> simplify_path(
	const std::vector<int_t>& path, std::vector<tau<BAs...> >& vars)
{
	// std::cout << vars << "\n";
	using tp = tau_parser;
	std::vector<std::vector<tau<BAs...>>> pos;
	// Build clause for non-equality terms
	// and new representation for equality terms
	tau<BAs...> clause = _T<BAs...>;
	tau<BAs...> pos_bf = _0<BAs...>;
	tau<BAs...> negs_wff = _T<BAs...>;
	for (size_t p = 0; p < path.size(); ++p) {
		if (path[p]== 2) continue;
		if (is_child_non_terminal(tp::bf_eq, vars[p])) {
			if (path[p] == -1) {
				auto vs = simp_general_excluded_middle(trim2(vars[p]));
				negs_wff = build_wff_and(negs_wff, build_wff_neq(vs));
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
	// std::cout << "pos_bf: " << pos_bf << "\n";
	pos_bf = reduce2(pos_bf, tau_parser::bf, false, true, false);
	pos_bf = simp_general_excluded_middle(pos_bf);
	// std::cout << "pos_bf after reduce: " << pos_bf << "\n";
	tau<BAs...> new_pos_bf;
	for (const auto& c : get_dnf_bf_clauses(pos_bf)) {
		pos.emplace_back(get_cnf_bf_clauses(c));
		if (new_pos_bf) new_pos_bf = build_wff_and(new_pos_bf, build_wff_eq(c));
		else new_pos_bf = build_wff_eq(c);
	}
	// std::cout << "New_pos_bf: " << new_pos_bf << "\n";
	clause = build_wff_and(clause, new_pos_bf);

	// cout << "Pos clause: " << clause << "\n";
	auto cnf_neq_lits = get_cnf_inequality_lits(negs_wff);
	// Check if any term in pos is subset of any term in negs
	for (const auto& p : pos) {
		for (auto& neq_clause : cnf_neq_lits){
			for (auto& n : neq_clause) {
				// Simplify cases p' = 0 && p != 0 and p = 0 && p' != 0
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
		for (int_t c2 = 0; c2 < (int_t)cnf_neq_lits.size(); ++c2) {
			if (c1 == c2) continue;
			bool is_impl = true;
			for (size_t i = 0; i < cnf_neq_lits[c1].size(); ++i) {
				if (cnf_neq_lits[c1][i].empty()) continue;
				bool exists_superset = false;
				for (size_t j = 0; j < cnf_neq_lits[c2].size(); ++j) {
					if (cnf_neq_lits[c2][j].empty()) continue;
					if (is_ordered_subset(cnf_neq_lits[c2][j], cnf_neq_lits[c1][i])) {
						exists_superset = true;
						break;
					}
				}
				// If not exists_superset, c1[i] is not a superset for some c2[j]
				// Hence, c1 cannot imply c2
				if (!exists_superset) {
					is_impl = false;
					break;
				}
			}
			if (is_impl) {
				// c1 implies c2
				cnf_neq_lits.erase(cnf_neq_lits.begin() + c2);
				if (c1 >= c2) --c1;
				break;
			}
		}
	}

	tau<BAs...> neq_cnf;
	for (const auto& neq_clause : cnf_neq_lits) {
		if (neq_clause.empty()) continue;
		tau<BAs...> neqs;
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
		// std::cout << "neq_cnf: " << neq_cnf << "\n";
		neq_cnf = reduce2(neq_cnf, tau_parser::wff, true, true, false);
		// std::cout << "neq_cnf after reduce: " << neq_cnf << "\n";
		//TODO: avoid pattern matcher
		neq_cnf = neq_cnf | repeat_all<step<BAs...>, BAs...>(
				 squeeze_wff_neg<BAs...> | to_mnf_wff<BAs...>);
		// cout << "neq_cnf simplified: " << neq_cnf << "\n";
		clause = build_wff_and(clause, neq_cnf);
	}
	// std::cout << "new clause: " << clause << "\n";
	auto new_vars = select_top(clause, is_wff_bdd_var);

	unordered_tau_map<size_t, BAs...> var_to_idx;
	for (size_t i=0; i<vars.size(); ++i)
		var_to_idx.emplace(vars[i], i);

	for (const auto& v : new_vars) {
		if (auto it = var_to_idx.find(v); it == end(var_to_idx)) {
			// First check if it is not sorted canonically
			auto sorted_v = sort_var(v);
			// std::cout << "old var: " << v << "\n";
            // std::cout << "new var: " << sorted_v << "\n";
            if (sorted_v != v && var_to_idx.contains(sorted_v)) {
                // Rename variable in current clause
                clause = replace(clause, v, sorted_v);
                continue;
            }
			if (sorted_v != v) {
				clause = replace(clause, v, sorted_v);
			}
			// There is a new variable
			assert(sorted_v != build_wff_eq(_T<BAs...>) && sorted_v != build_wff_eq(_F<BAs...>));
			vars.push_back(sorted_v);
			var_to_idx.emplace(sorted_v, vars.size() - 1);
		}
	}
	return clause_to_vector(clause, var_to_idx, true, false);
}

template<typename...BAs>
std::pair<tau<BAs...>, bool> group_paths_and_simplify(
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
		tau<BAs...> clause = _T<BAs...>;
		for (size_t k = 0; k < path.size(); ++k) {
			if (path[k] == -1 && is_child_non_terminal(tp::bf_eq, vars[k]))
				clause = build_wff_and(clause, build_wff_neq(trim2(vars[k])));
		}
		return clause;
	};
	tau<BAs...> result = _F<BAs...>;
	for (size_t i = 0; i < paths.size(); ++i) {
		// Now add from paths[i] and groups[i]
		tau<BAs...> neqs = neq_from_path(paths[i]);
		for (const auto& path : groups[i])
			neqs = build_wff_or(neqs, neq_from_path(path));
		if (!groups[i].empty()) {
			// simp to cnf
			neqs = to_cnf2(neqs, true);
			neqs = reduce2(neqs, tau_parser::wff, true);
			// push != out
			//TODO: avoid pattern matcher
			neqs = neqs | repeat_all<step<BAs...>, BAs...>(
			squeeze_wff_neg<BAs...>);
		}
		auto neq_clauses = get_cnf_wff_clauses(neqs);
		// std::cout << "neq_clauses: " << neq_clauses << "\n";
		// Simplify bfs and drop !=
		bool clause_false = false;
		for (auto& neq : neq_clauses) {
			if (neq == _T<BAs...>) continue;
			neq = reduce2(trim2(neq), tau_parser::bf);
			neq = simp_general_excluded_middle(neq);
			if (neq == _0<BAs...>) clause_false = true;
		}
		if (clause_false) continue;
		// std::cout << "neq_clause after reduce: " << neq_clauses << "\n";
		// Simplify neqs with assumptions from = part
		tau<BAs...> rest = _T<BAs...>;
		for (size_t k = 0; k < paths[i].size(); ++k) {
			if (paths[i][k] == 1) {
				if (is_child_non_terminal(tp::bf_eq, vars[k])) {
					for (const auto& clause : get_dnf_bf_clauses(trim2(vars[k]))) {
						// Here single assumption
						auto neg_eq = push_negation_in(build_bf_neg(clause), false);
						// std::cout << "neg_eq: " << neg_eq << "\n";
						for (auto& neq : neq_clauses) {
							if (neq == _T<BAs...> || neq == _1<BAs...>) continue;
							auto grouped_bf = group_dnf_expression(neq);
							// std::cout << "grouped_bf: " << grouped_bf << "\n";
							auto simp_neq = replace(grouped_bf, neg_eq, _1<BAs...>);
							// std::cout << "simp_neq: " << simp_neq << "\n";
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
	//TODO: avoid pattern matcher
	result = result | repeat_all<step<BAs...>, BAs...>(eq_to_neq<BAs...>);
	BOOST_LOG_TRIVIAL(debug) << "(I) End group_paths_and_simplify";
	return make_pair(result, is_simp);
}

template<typename... BAs>
tau<BAs...> reduce_across_bfs (const tau<BAs...>& fm, bool to_cnf) {
	// std::cout << "Start reduce_across_bfs\n";
	// std::cout << "(F) " << fm << "\n";
	BOOST_LOG_TRIVIAL(debug) << "(I) Start reduce_across_bfs with";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << fm;

	auto squeezed_fm = (to_cnf ? push_negation_in(build_wff_neg(fm)) : fm);
	// Squeeze all equalities and inequalities
	squeezed_fm = squeeze_wff(squeezed_fm);
	squeezed_fm = reduce_terms(to_dnf2(squeezed_fm));
	// std::cout << squeezed_fm << "\n";
	// We work with unsqueezed equality
	squeezed_fm  = unsqueeze_wff_pos(squeezed_fm);
    // std::cout << squeezed_fm << "\n";
	BOOST_LOG_TRIVIAL(debug) << "(I) Formula in DNF: " << squeezed_fm;
#ifdef TAU_CACHE
		static std::map<std::pair<tau<BAs...>, bool>, tau<BAs...>> cache;
		if (auto it = cache.find(make_pair(squeezed_fm, to_cnf)); it != end(cache))
			return it->second;
#endif // TAU_CACHE
	auto [paths, vars] = dnf_cnf_to_bdd<BAs...>(squeezed_fm, tau_parser::wff, false, true, false);
	// Vars might not be sorted canonically
	// std::cout << "original vars: " << vars << "\n";

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

			if (f) {
				assert(len_vars == vars.size());
				paths.erase(paths.begin()+i); --i;
			}
			else if (simp_path != paths[i]) {

				// std::cout << "(I) Path simplification happened: " << "\n";
				// std::vector<std::vector<int_t> > tmp1{paths[i]};
				// std::vector<std::vector<int_t> > tmp2{simp_path};
				// std::cout << "(F) Current path: " <<
				// 		build_reduced_formula<BAs...>(
				// 			tmp1, vars, false,
				// 			true)<< "\n";
				// std::cout << "(F) Simplified path: " <<
				// 		build_reduced_formula<BAs...>(
				// 			tmp2, vars, false,
				// 			true)<< "\n";
// #ifdef DEBUG
// 				BOOST_LOG_TRIVIAL(debug) << "(I) Path simplification happened: ";
// 				std::vector<std::vector<int_t> > tmp1{paths[i]};
// 				std::vector<std::vector<int_t> > tmp2{simp_path};
// 				BOOST_LOG_TRIVIAL(debug) << "(F) Current path: " <<
// 						build_reduced_formula<BAs...>(
// 							tmp1, vars, false,
// 							true);
// 				BOOST_LOG_TRIVIAL(debug) << "(F) Simplified path: " <<
// 						build_reduced_formula<BAs...>(
// 							tmp2, vars, false,
// 							true);
// #endif // DEBUG
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
		auto further_simp = apply_eqs_across_clauses(simp_fm);
		// std::cout << "further_simp: " << further_simp << "\n";
		if (further_simp != simp_fm) has_further_simp = true;
		simp_fm = further_simp;
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
	tau<BAs...> operator() (const tau<BAs...>& fm) const {
		return reduce2(fm, tau_parser::wff);
	}
};

template<typename... BAs>
struct wff_reduce_cnf {
	tau<BAs...> operator() (const tau<BAs...>& fm) const {
		return reduce2(fm, tau_parser::wff, true);
	}
};

template<typename... BAs>
tau<BAs...> operator|(const tau<BAs...>& fm, const wff_reduce_dnf<BAs...>& r) {
	return r(fm);
}

template<typename... BAs>
tau<BAs...> operator|(const tau<BAs...>& fm, const wff_reduce_cnf<BAs...>& r) {
	return r(fm);
}

template<typename... BAs>
tau<BAs...> conjunct_dnfs_to_dnf (const tau<BAs...>& d1, const tau<BAs...>& d2) {
	if (is_non_terminal(tau_parser::wff, d1)) {
		assert(is_non_terminal(tau_parser::wff, d2));
		tau<BAs...> res = _F<BAs...>;
		auto clauses_d1 = get_dnf_wff_clauses(d1);
		auto clauses_d2 = get_dnf_wff_clauses(d2);
		for (const auto& dis1 : clauses_d1)
			for (const auto& dis2 : clauses_d2)
				res = build_wff_or(res, build_wff_and(dis1, dis2));
		return res;
	} else {
		assert(is_non_terminal(tau_parser::bf, d1) && is_non_terminal(tau_parser::bf, d2));
		tau<BAs...> res = _0<BAs...>;
		auto clauses_d1 = get_dnf_bf_clauses(d1);
		auto clauses_d2 = get_dnf_bf_clauses(d2);
		for (const auto& dis1 : clauses_d1)
			for (const auto& dis2 : clauses_d2)
				res = build_bf_or(res, build_bf_and(dis1, dis2));
		return res;
	}
}

template<typename... BAs>
tau<BAs...> disjunct_cnfs_to_cnf (const tau<BAs...>& c1, const tau<BAs...>& c2) {
	if (is_non_terminal(tau_parser::wff, c1)) {
		assert(is_non_terminal(tau_parser::wff, c2));
		tau<BAs...> res = _T<BAs...>;
		auto clauses_c1 = get_cnf_wff_clauses(c1);
		auto clauses_c2 = get_cnf_wff_clauses(c2);
		for (const auto& dis1 : clauses_c1)
			for (const auto& dis2 : clauses_c2)
				res = build_wff_and(res, build_wff_or(dis1, dis2));
		return res;
	} else {
		assert(is_non_terminal(tau_parser::bf, c1) && is_non_terminal(tau_parser::bf, c2));
		tau<BAs...> res = _1<BAs...>;
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
tau<BAs...> push_negation_one_in(const tau<BAs...>& fm, bool is_wff = true) {
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
		if (is_child_non_terminal(p::wff_ex, c))
			return build_wff_all(trim2(c), build_wff_neg(trim(c)->child[1]));
		if (is_child_non_terminal(p::wff_all, c))
			return build_wff_ex(trim2(c), build_wff_neg(trim(c)->child[1]));
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
tau<BAs...> push_negation_in(const tau<BAs...>& fm, bool is_wff) {
	auto pn = [&is_wff](const auto& n) {
		return push_negation_one_in(n, is_wff);
	};
	// if (is_wff) return rewriter::pre_order(fm).template apply_unique<2>(pn);
	if (is_wff) return rewriter::pre_order(fm).template
	apply_unique<MemorySlotPre::push_negation_in_m>(
		pn, visit_wff<BAs...>, identity);
	else return rewriter::pre_order(fm).template
	apply_unique<MemorySlotPre::push_negation_in_m>(
		pn, all, identity);
}

// Conversion to dnf while applying reductions during the process
template<typename... BAs>
tau<BAs...> to_dnf2(const tau<BAs...>& fm, bool is_wff) {
	using p = tau_parser;
	auto layer_to_dnf = [&is_wff](const auto& n) {
		if (is_wff && is_non_terminal(p::wff, n)) {
			if (is_child_non_terminal(p::wff_and, n)) {
				auto conj = conjunct_dnfs_to_dnf(
					trim(n)->child[0],
					trim(n)->child[1]);
				// Perform simplification
				if (conj != n)
					return conj | wff_reduce_dnf<BAs...>();
				else return n;
			}
		} else if (!is_wff && is_non_terminal(p::bf, n)) {
			if (is_child_non_terminal(p::bf_and, n)) {
				auto conj = conjunct_dnfs_to_dnf(
					trim(n)->child[0],
					trim(n)->child[1]);
				// Perform simplification
				if (conj != n)
					return reduce2(conj, p::bf);
				else return n;
			}
		}
		return n;
	};
	auto pn = [&is_wff](const auto& n) {
		return push_negation_one_in(n, is_wff);
	};
	// if (is_wff) return pre_order(fm).template apply_unique<4>(pn, visit_wff<BAs...>, layer_to_dnf);
	if (is_wff) return pre_order(fm).template
	apply_unique<MemorySlotPre::to_dnf2_m>(
		pn, visit_wff<BAs...>, layer_to_dnf);
	else return pre_order(fm).template
	apply_unique<MemorySlotPre::to_dnf2_m>(
		pn, all, layer_to_dnf);
}

template<typename... BAs>
tau<BAs...> single_dnf_lift(const tau<BAs...>& fm) {
	using p = tau_parser;
	auto layer_to_dnf = [](const auto& n) {
		if (is_child_non_terminal(p::wff_and, n)) {
			// If left child is a disjunction
			if (is_child_non_terminal(p::wff_or, trim2(n))) {
				const auto& c = trim2(n);
				return build_wff_or (
					build_wff_and(trim2(c), trim(n)->child[1]),
					build_wff_and(trim(c)->child[1], trim(n)->child[1]));
			}
			// If right child is a disjunction
			if (is_child_non_terminal(p::wff_or, trim(n)->child[1])) {
				const auto& c = trim(n)->child[1];
				return build_wff_or (
					build_wff_and(trim2(n), trim2(c)),
					build_wff_and(trim2(n), trim(c)->child[1]));
			}
		}
		return n;
	};
	bool found_or = false;
	auto decend = [&found_or](const auto& n) {
		using p = tau_parser;
		if (found_or) return false;
		if (is_non_terminal(p::wff_or, n)) {
			found_or = true;
			return false;
		}
		if (is_non_terminal(p::bf, n))
			return false;
		return true;
	};
	return post_order(fm).apply_unique(layer_to_dnf, decend);
}

// Conversion to cnf while applying reductions during the process
template<typename... BAs>
tau<BAs...> to_cnf2(const tau<BAs...>& fm, bool is_wff) {
	using p = tau_parser;
	auto layer_to_cnf = [&is_wff](const auto& n) {
		if (is_wff && is_non_terminal(p::wff, n)) {
			if (is_child_non_terminal(p::wff_or, n)) {
				auto dis = disjunct_cnfs_to_cnf(
					trim(n)->child[0],
					trim(n)->child[1]);
				// Perform simplification
				if (dis != n)
					return dis | wff_reduce_cnf<BAs...>();
				else return n;
			}
		} else if (!is_wff){
			if (is_child_non_terminal(p::bf_or, n)) {
				auto dis = disjunct_cnfs_to_cnf(
					trim(n)->child[0],
					trim(n)->child[1]);
				// Perform simplification
				if (dis != n)
					return reduce2(dis, p::bf, true);
				else return n;
			}
		}
		return n;
	};
	auto pn = [&is_wff](const auto& n) {
		return push_negation_one_in(n, is_wff);
	};
	// if (is_wff) return pre_order(fm).template apply_unique<6>(pn, all, layer_to_cnf);
	if (is_wff) return pre_order(fm).template
	apply_unique<MemorySlotPre::to_cnf2_m>(
		pn, visit_wff<BAs...>, layer_to_cnf);
	else return pre_order(fm).template
	apply_unique<MemorySlotPre::to_cnf2_m>(
		pn, all, layer_to_cnf);
}

// Assumes that fm is a single DNF always clause
template<typename... BAs>
tau<BAs...> rm_temporary_lookback (const tau<BAs...>& fm) {
	auto io_vars = rewriter::select_top(fm,
		is_child_non_terminal<tau_parser::io_var, BAs...>);
	bool has_var = std::ranges::any_of(io_vars,
		[](const auto& el){return !is_io_initial(el);});
	int_t lookback = get_max_shift(io_vars, true);
	std::map<tau<BAs...>, tau<BAs...>> changes;
	tau<BAs...> max_temp;
	for (const auto& io_var : io_vars) {
		auto n = get_io_name(io_var);
		// Only eliminate lookback temporary variables
		if (n[0] == '_' && n[1] == 'l') {
			if (!has_var) changes.emplace(io_var, _0_trimmed<BAs...>);
			else if (lookback >= get_io_var_shift(io_var))
				changes.emplace(io_var, _0_trimmed<BAs...>);
			else {
				if (max_temp) {
					if (get_io_var_shift(max_temp) < get_io_var_shift(io_var)) {
						changes.emplace(max_temp, _0_trimmed<BAs...>);
						max_temp = io_var;
					} else changes.emplace(io_var, _0_trimmed<BAs...>);
				} else {
					max_temp = io_var;
				}
			}
		}
	}
	return replace(fm, changes);
}

// Assumes a single sometimes DNF clause with negation pushed in containing no wff_or with max nesting depth 1
template<typename... BAs>
tau<BAs...> extract_sometimes (tau<BAs...> fm) {
	std::map<tau<BAs...>, tau<BAs...>> l_changes = {};
	// Collect remaining nested "sometimes" formulas
	std::vector<tau<BAs...>> sometimes_extractions = {};
	for (const auto &inner_st: select_top(trim2(fm), is_child_non_terminal<tau_parser::wff_sometimes, BAs...>)) {
		l_changes[inner_st] = _T<BAs...>;
		sometimes_extractions.push_back(inner_st);
	}
	// Collect remaining nested "always" formulas
	std::vector<tau<BAs...>> always_extractions = {};
	for (const auto &inner_aw: select_top(trim2(fm), is_child_non_terminal<tau_parser::wff_always, BAs...>)) {
		l_changes[inner_aw] = _T<BAs...>;
		always_extractions.push_back(inner_aw);
	}
	// Apply always/sometimes extractions to fm
	if (!l_changes.empty()) fm = replace(fm, l_changes);

	tau<BAs...> extracted_fm = _T<BAs...>;
    for (const auto &se: sometimes_extractions)
		extracted_fm = build_wff_and(extracted_fm, se);
	for (const auto &ae : always_extractions)
		extracted_fm = build_wff_and(extracted_fm, ae);

	// Check if everything under sometimes was removed
	if (fm == _T<BAs...>) return extracted_fm;
	if (fm == _F<BAs...>) return _F<BAs...>;

	std::vector<tau<BAs...>> extracted = {}, staying = {};
	auto clauses = get_leaves(trim2(fm), tau_parser::wff_and);
	for (const auto& clause : clauses) {
		assert(!is_non_terminal(tau_parser::wff_sometimes, trim(clause)) &&
					!is_non_terminal(tau_parser::wff_always, trim(clause)));
		if(!has_temp_var(clause))
			extracted.push_back(clause);
		else staying.push_back(clause);
	}

	// From here we build the formula which we will return
    for (const auto &e: extracted) extracted_fm = build_wff_and(extracted_fm, e);

	if (staying.empty()) return extracted_fm;
	tau<BAs...> staying_fm = _T<BAs...>;
    for (const auto &s: staying)
         staying_fm = build_wff_and(staying_fm, s);

	if (extracted_fm == _T<BAs...>) return build_wff_sometimes(staying_fm);
	return build_wff_and(build_wff_sometimes(staying_fm), extracted_fm);
}

// Assumes a single DNF always clause in cnf with negation pushed in containing no wff_and with max nesting depth 1
template<typename... BAs>
tau<BAs...> extract_always (tau<BAs...> fm) {
	std::map<tau<BAs...>, tau<BAs...>> l_changes = {};
	// Collect remaining nested "sometimes" formulas
	std::vector<tau<BAs...>> sometimes_extractions = {};
	for (const auto &inner_st: select_top(trim2(fm), is_child_non_terminal<tau_parser::wff_sometimes, BAs...>)) {
		l_changes[inner_st] = _F<BAs...>;
		sometimes_extractions.push_back(inner_st);
	}
	// Collect remaining nested "always" formulas
	std::vector<tau<BAs...>> always_extractions = {};
	for (const auto &inner_aw: select_top(trim2(fm), is_child_non_terminal<tau_parser::wff_always, BAs...>)) {
		l_changes[inner_aw] = _F<BAs...>;
		always_extractions.push_back(inner_aw);
	}
	// Apply always/sometimes extractions to flat_st
	if (!l_changes.empty()) fm = replace(fm, l_changes);

	tau<BAs...> extracted_fm = _F<BAs...>;
    for (const auto &se: sometimes_extractions)
		extracted_fm = build_wff_or(extracted_fm, se);
	for (const auto &ae : always_extractions)
		extracted_fm = build_wff_or(extracted_fm, ae);

    // Check if everything under fm was removed
	if (fm == _F<BAs...>) return extracted_fm;
	if (fm == _T<BAs...>) return _T<BAs...>;

	// Now extract from all disjuncts
	std::vector<tau<BAs...>> extracted = {}, staying = {};
	auto clauses = get_leaves(trim2(fm), tau_parser::wff_or);
	for (const auto& clause : clauses) {
		assert(!is_non_terminal(tau_parser::wff_sometimes, trim(clause)) &&
					!is_non_terminal(tau_parser::wff_always, trim(clause)));
		if (!has_temp_var(clause))
			extracted.push_back(clause);
		else staying.push_back(clause);
	}

	// From here we build the formula to return based on the extractions
    for (const auto &e: extracted) extracted_fm = build_wff_or(extracted_fm, e);

	if (staying.empty()) return extracted_fm;
	tau<BAs...> staying_fm = _F<BAs...>;
    for (const auto &s: staying)
         staying_fm = build_wff_or(staying_fm, s);

	if (extracted_fm == _F<BAs...>) return build_wff_always(staying_fm);
	return build_wff_or(build_wff_always(staying_fm), extracted_fm);
}

// Recursively extract non-dependend formulas under sometimes
template<typename... BAs>
tau<BAs...> push_sometimes_always_in (tau<BAs...> fm, auto& visited) {
	std::map<tau<BAs...>, tau<BAs...>> g_changes = {};
	for (const auto &st : select_top_until(fm, is_child_non_terminal<tau_parser::wff_sometimes, BAs...>,
								is_child_non_terminal<tau_parser::wff_always, BAs...>)) {
		// Recursively denest sometimes and always statements contained in sometimes statement st
		auto flat_st = push_sometimes_always_in(trim2(st), visited);
		// Simplyfy current formula and convert to DNF
		// Reductions done in order to prevent blow up
		flat_st = build_wff_sometimes(to_dnf2(flat_st));
		flat_st = push_temp_in(flat_st, tau_parser::wff_sometimes, visited);
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
		auto flat_aw = push_sometimes_always_in(trim2(aw), visited);
		// std::cout << "After push_sometimes_always_in: " << flat_aw << "\n";
		// Simplyfy current formula and convert to CNF
		// Reductions done in order to prevent blow up
		flat_aw = build_wff_always(to_cnf2(flat_aw));
		// std::cout << "After to_cnf2: " << flat_aw << "\n";
		flat_aw = push_temp_in(flat_aw, tau_parser::wff_always, visited);
		// std::cout << "After push_temp_in: " << flat_aw << "\n";
		flat_aw = reduce2(flat_aw, tau_parser::wff, true);
		// std::cout << "After reduce2: " << flat_aw << "\n";
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
		// std::cout << "simp_aw: " << aw << "\n";
		auto simp_aw = extract_always(aw);
		if (aw != simp_aw) g_changes[aw] = simp_aw;
	}
	// Apply changes and return
	if (!g_changes.empty()) fm = replace(fm, g_changes);
	return fm;
}

// Shift the lookback in a formula
template<typename... BAs>
tau<BAs...> shift_io_vars_in_fm (const tau<BAs...>& fm, const auto& io_vars, const int_t shift) {
	if (shift <= 0) return fm;
	std::map<tau<BAs...>, tau<BAs...>> changes;
	for (const auto& io_var: io_vars) {
		// Skip initial conditions
		if (is_io_initial(io_var))
			continue;
		int_t var_shift = get_io_var_shift(io_var);
		if (io_var | tau_parser::io_var | tau_parser::in) {
			changes[io_var] = trim(build_in_variable_at_t_minus<BAs...>(
				get_io_name(io_var), var_shift + shift));
		} else {
			changes[io_var] = trim(build_out_variable_at_t_minus<BAs...>(
				get_io_name(io_var), var_shift + shift));
		}
	}
	return replace(fm, changes);
}

template<typename... BAs>
tau<BAs...> shift_const_io_vars_in_fm(const tau<BAs...>& fm,
					const auto& io_vars, const int_t shift) {
	if (shift <= 0) return fm;
	using p = tau_parser;
	std::map<tau<BAs...>, tau<BAs...>> changes;
	for (const auto& io_var : io_vars) {
		if (!is_io_initial(io_var))
			continue;
		int_t tp = get_io_time_point(io_var);
		// Make sure that the resulting time point is positive
		if (tp + shift < 0) return _F<BAs...>;
		if (io_var | p::io_var | p::in) {
			changes.emplace(
				io_var, trim(
					build_in_variable_at_n(
						get_tau_io_name(io_var),
						tp + shift)));
		} else {
			changes.emplace(
				io_var, trim(
					build_out_variable_at_n(
						get_tau_io_name(io_var),
						tp + shift)));
		}
	}
	return replace(fm, changes);
}

// Assumes a single DNF clause and normalizes the "always" parts into one
template<typename... BAs>
tau<BAs...> pull_always_out(const tau<BAs...>& fm) {
	std::map<tau<BAs...>, tau<BAs...>> l_changes = {};
	std::vector<tau<BAs...>> collected_always_fms;
	// Collect all always statments in the clause fm
	// by analyzing conjuncts
	auto clauses = get_leaves(fm, tau_parser::wff_and);
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
	tau<BAs...> always_part;
	int_t lookback = 0;
	bool first = true;
	for (auto& fa : collected_always_fms) {
		auto io_vars = select_top(fa,
			is_child_non_terminal<tau_parser::io_var, BAs...>);
		auto current_lb = get_max_shift(io_vars);
		if (first) { first = false; always_part = fa; }
		else {
			// Adjust lookback when joining always statements
			if (current_lb < lookback)
				fa = shift_io_vars_in_fm(fa, io_vars, lookback - current_lb);
			if (current_lb > lookback) {
				io_vars = select_top(always_part,
					is_child_non_terminal<tau_parser::io_var, BAs...>);
				always_part = shift_io_vars_in_fm(always_part, io_vars, current_lb - lookback);
			}
			always_part = build_wff_and(always_part, fa);
		}
		lookback = std::max(lookback, current_lb);
	}
	assert(!l_changes.empty());
	// Now remove all temporary lookback variables which are not needed anymore
	// after joining of the always statements
	always_part = rm_temporary_lookback(always_part);
	if (!has_temp_var(fm)) {
		// No input/output variable present, hence return without always
		return build_wff_and(always_part, replace(fm, l_changes));
	}
	return build_wff_and(build_wff_always(always_part), replace(fm, l_changes));
}

// We assume that there is no nesting of "sometimes" and "always" in fm
// and that fm is in DNF
template<typename... BAs>
tau<BAs...> pull_sometimes_always_out(tau<BAs...> fm) {
	std::map<tau<BAs...>, tau<BAs...>> changes = {};
	std::vector<tau<BAs...>> collected_no_temp_fms;
	tau<BAs...> pure_always_clause;
	// Collect all disjuncts which have temporal variables and call pull_always_out on the others
	auto clauses = get_leaves(fm, tau_parser::wff_or);
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
		tau<BAs...> no_temp_fm;
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
		fm = build_wff_or(fm, no_temp_fm);
	}
	return fm;
}

// The needed class in order to make sometimes/always normalization work with normalizer
template<typename... BAs>
struct sometimes_always_normalization_depreciated {
	tau<BAs...> operator() (const tau<BAs...>& fm) const {
		auto st_aw = [](const auto& n) {
			return is_child_non_terminal(tau_parser::wff_sometimes, n) ||
				is_child_non_terminal(tau_parser::wff_always, n);
		};
		// If there is no temporal quantifier and no temporal variable
		// Just convert to DNF and return

		if (!find_top(fm, st_aw).has_value() && !has_temp_var(fm)) {
			return reduce_across_bfs(fm, false);
		}

		// Scope formula under always if not already under sometimes or always
		tau<BAs...> res = !st_aw(fm) ? build_wff_always(fm) : fm;
		// conversion to DNF necessary for pull_sometimes_always_out
		std::set<tau<BAs...>> visited;
		res = to_dnf2(push_sometimes_always_in(res, visited));
		res = reduce2(res, tau_parser::wff);
		res = pull_sometimes_always_out(res) |
					repeat_each<step<BAs...>, BAs...>(simplify_wff<BAs...>);
		auto temp_inner = select_top(res, st_aw);
		if (temp_inner.empty()) return res;
		std::map<tau<BAs...>, tau<BAs...>> changes;
		for (const auto& f : temp_inner) {
			// Reduction done to normalize again now that sometimes/always are all the way out
			changes[trim2(f)] = reduce_across_bfs(trim2(f), false);
		}
		return reduce_across_bfs(replace(res, changes), false);
	}
};

// Adjust the lookback before conjunction of fm1 and fm2
template<typename...BAs>
tau<BAs...> always_conjunction (const tau<BAs...>& fm1_aw, const tau<BAs...>& fm2_aw) {
	using p = tau_parser;
	// Trim the always node if present
	auto fm1 = is_child_non_terminal(p::wff_always, fm1_aw) ? trim2(fm1_aw) : fm1_aw;
	auto fm2 = is_child_non_terminal(p::wff_always, fm2_aw) ? trim2(fm2_aw) : fm2_aw;
	auto io_vars1 = select_top(fm1, is_child_non_terminal<p::io_var, BAs...>);
	auto io_vars2 = select_top(fm2, is_child_non_terminal<p::io_var, BAs...>);
	// Get lookbacks
	int_t lb1 = get_max_shift(io_vars1);
	int_t lb2 = get_max_shift(io_vars2);
	if (lb1 < lb2) {
		// adjust fm1 by lb2 - lb1
		return build_wff_and(
			shift_io_vars_in_fm(fm1, io_vars1, lb2 - lb1),
			fm2);
	} else if (lb2 < lb1) {
		// adjust fm2 by lb1 - lb2
		return build_wff_and(fm1,
			shift_io_vars_in_fm(fm2, io_vars2, lb1 - lb2));
	} else {
		// no adjustment needed
		return build_wff_and(fm1, fm2);
	}
}


template<typename... BAs>
struct sometimes_always_normalization {
	tau<BAs...> operator() (const tau<BAs...>& fm) const {
		using p = tau_parser;
		auto st_aw = [](const auto& n) {
			return is_child_non_terminal(p::wff_sometimes, n) ||
				is_child_non_terminal(p::wff_always, n);
		};
		if (!find_top(fm, st_aw).has_value() && !has_temp_var(fm)) {
			return reduce_across_bfs(fm, false);
		}
		// Delete all always/sometimes if they scope no temporal variable
		auto temps = select_top(fm, st_aw);
		std::map<tau<BAs...>, tau<BAs...>> changes;
		for (const auto& temp : temps) {
			if (!has_temp_var(temp))
				changes.emplace(temp, trim2(temp));
		}
		auto clauses = get_dnf_wff_clauses(
			reduce_across_bfs(
				changes.empty()
					? fm
					: replace(fm, changes), false));
		tau<BAs...> res = _F<BAs...>;
		tau<BAs...> always_disjuncts = _F<BAs...>;
		for (const tau<BAs...>& clause : clauses) {
			// If clause does not contain sometimes/always but temporal variables, we add it to always_disjuncts
			if (!find_top(clause, st_aw)) {
				always_disjuncts = build_wff_or(always_disjuncts, clause);
				continue;
			}
			auto conjuncts = get_cnf_wff_clauses(clause);
			tau<BAs...> always_part = _T<BAs...>;
			tau<BAs...> staying = _T<BAs...>;
			for (const tau<BAs...>& conj : conjuncts) {
				if (!st_aw(conj))
					always_part = always_conjunction(always_part, conj);
				else if (is_child_non_terminal(p::wff_always, conj))
					always_part = always_conjunction(always_part, conj);
				else staying = build_wff_and(staying, conj);
			}
			always_part = build_wff_always(always_part);
			res = build_wff_or(res, build_wff_and(always_part, staying));
		}
		res = build_wff_or(build_wff_always(always_disjuncts), res);
		auto temp_inner = select_top(res, st_aw);
		if (temp_inner.empty()) return res;
		changes.clear();
		for (const auto& f : temp_inner) {
			// Reduction done to normalize again now that sometimes/always are all the way out
			changes[trim2(f)] = reduce_across_bfs(trim2(f), false);
		}
		return reduce_across_bfs(replace(res, changes), false);
	}
};

// Assumes that fm is normalized
template<typename... BAs>
tau<BAs...> pull_always_out_for_inf (const tau<BAs...>& fm) {
	using p = tau_parser;
	// Get all always statments on each clause and merge
	auto clauses = get_dnf_wff_clauses(fm);
	tau<BAs...> res = _F<BAs...>;
	tau<BAs...> non_temps = _F<BAs...>;
	tau<BAs...> last_always;
	for (const tau<BAs...>& clause : clauses) {
		// Clauses not containing temporal variables need to be added under always
		if (!has_temp_var(clause)) {
			non_temps = build_wff_or(non_temps, clause);
			continue;
		}
		auto conjuncts = get_cnf_wff_clauses(clause);
		tau<BAs...> always_statements = _T<BAs...>;
		tau<BAs...> staying = _T<BAs...>;
		for (const tau<BAs...>& conj : conjuncts) {
			if (is_child_non_terminal(p::wff_always, conj))
				always_statements = build_wff_and(always_statements, trim2(conj));
			else if (!is_child_non_terminal(p::wff_sometimes, conj))
				always_statements = build_wff_and(always_statements, conj);
			else staying = build_wff_and(staying, conj);
		}
		// Add the non temporal clauses under always
		last_always = reduce_across_bfs(always_statements, false);
		always_statements = build_wff_always(last_always);
		res = build_wff_or(res, build_wff_and(always_statements, staying));
	}
	if (last_always) {
		res = replace(res, last_always, build_wff_or(last_always, non_temps));
	} else {
		assert(res == _F<BAs...>);
		res = build_wff_always(non_temps);
	}
	return reduce_across_bfs(res, false);
}

template<typename... BAs>
tau<BAs...> operator|(const tau<BAs...>& fm, const sometimes_always_normalization<BAs...>& r) {
	return r(fm);
}

template <typename... BAs>
tau<BAs...> push_existential_quantifier_one (const tau<BAs...>& fm) {
	using p = tau_parser;
	assert(is_child_non_terminal(p::wff_ex, fm));
	const tau<BAs...> scoped_fm = trim(fm)->child[1];
	const tau<BAs...> quant_var = trim2(fm);

	if (is_child_non_terminal(p::wff_or, scoped_fm)) {
		// Push quantifier in
		const auto c0 = build_wff_ex(quant_var, trim2(scoped_fm));
		const auto c1 = build_wff_ex(quant_var, trim(scoped_fm)->child[1]);
		return build_wff_or(c0, c1);
	}
	else if (is_child_non_terminal(p::wff_and, scoped_fm)) {
		// Remove existential, if quant_var does not appear in clause
		auto clauses = get_cnf_wff_clauses(scoped_fm);
		tau<BAs...> no_q_fm = _T<BAs...>;
		for (tau<BAs...>& clause : clauses) {
			if (!contains(clause, quant_var)) {
				no_q_fm = build_wff_and(no_q_fm, clause);
				clause = _T<BAs...>;
			}
		}
		auto q_fm = build_wff_and<BAs...>(clauses);
		if (q_fm == _T<BAs...>) return scoped_fm;
		else if (no_q_fm == _T<BAs...>) return fm;
		else return build_wff_and(
			build_wff_ex(quant_var, q_fm), no_q_fm);
	}
	else if (is_child_non_terminal(p::wff_ex, scoped_fm)) {
		//other ex quant, hence can switch them
		const auto& c = build_wff_ex(quant_var, trim(scoped_fm)->child[1]);
		return build_wff_ex(trim2(scoped_fm), c);
	}
	// Else check if quant_var is contained in subtree
	else if (contains(scoped_fm, quant_var)) return fm;
	else return scoped_fm;
}

template <typename... BAs>
tau<BAs...> push_universal_quantifier_one (const tau<BAs...>& fm) {
	using p = tau_parser;
	assert(is_child_non_terminal(p::wff_all, fm));
	const tau<BAs...> scoped_fm = trim(fm)->child[1];
	const tau<BAs...> quant_var = trim2(fm);

	if (is_child_non_terminal(p::wff_and, scoped_fm)) {
		// Push quantifier in
		const auto c0 = build_wff_all(quant_var, trim2(scoped_fm));
		const auto c1 = build_wff_all(quant_var, trim(scoped_fm)->child[1]);
		return build_wff_and(c0, c1);
	}
	else if (is_child_non_terminal(p::wff_or, scoped_fm)) {
		// Remove existential, if quant_var does not appear in clause
		auto clauses = get_dnf_wff_clauses(scoped_fm);
		tau<BAs...> no_q_fm = _F<BAs...>;
		for (tau<BAs...>& clause : clauses) {
			if (!contains(clause, quant_var)) {
				no_q_fm = build_wff_or(no_q_fm, clause);
				clause = _F<BAs...>;
			}
		}
		auto q_fm = build_wff_or<BAs...>(clauses);
		if (q_fm == _F<BAs...>) return scoped_fm;
		else if (no_q_fm == _F<BAs...>) return fm;
		else return build_wff_or(
			build_wff_all(quant_var, q_fm), no_q_fm);
	}
	else if (is_child_non_terminal(p::wff_all, scoped_fm)) {
		//other all quant, hence can switch them
		const auto& c = build_wff_all(quant_var, trim(scoped_fm)->child[1]);
		return build_wff_all(trim2(scoped_fm), c);
	}
	// Else check if quant_var is contained in subtree
	else if (contains(scoped_fm, quant_var)) return fm;
	else return scoped_fm;
}

// Squeeze all equalities found in n
//TODO: make it type depended
template <typename... BAs>
std::optional<tau<BAs...>> squeeze_positives(const tau<BAs...>& n) {
	if (auto positives = select_top(n, is_non_terminal<tau_parser::bf_eq, BAs...>);
			positives.size() > 0) {
		for (auto& p: positives) p = trim(p);
		return build_bf_or<BAs...>(positives);
	}
	return {};
}

template<typename... BAs>
tau<BAs...> wff_remove_existential(const tau<BAs...>& var, const tau<BAs...>& wff) {
	// Following Corollary 2.3 from Taba book from Ohad
	std::map<tau<BAs...>, tau<BAs...>> changes;
	for (const auto& l: get_leaves(wff, tau_parser::wff_or)) {
		// if var does not appear in the clause, we can skip it
		if (!contains(l, var)) continue;
		// Get each conjunct in clause
		tau<BAs...> nl = _T<BAs...>;
		bool is_quant_removable_in_clause = true;
		auto conjs = get_cnf_wff_clauses(l);
		for (auto& conj : conjs) {
			if (!contains(conj, var)) {
				nl = build_wff_and(nl, conj);
				conj = _T<BAs...>;
				continue;
			}
			// Check if conjunct is of form = 0 or != 0
			if ((conj | tau_parser::bf_eq) || (conj | tau_parser::bf_neq))
				continue;
			// If the conjunct contains the quantified variable at this point
			// we cannot resolve the quantifier in this clause
			is_quant_removable_in_clause = false;
			break;
		}
		const auto new_l = build_wff_and<BAs...>(conjs);
		if (!is_quant_removable_in_clause) {
			// Since we cannot remove the quantifier in this
			// clause it needs to be maintained
			changes[l] = build_wff_and(build_wff_ex(var, new_l), nl);
			continue;
		}

		auto f = squeeze_positives(new_l);
		auto f_0 = f ? replace(f.value(), var, _0_trimmed<BAs...>) : _0<BAs...>;
		auto f_1 = f ? replace(f.value(), var, _1_trimmed<BAs...>) : _0<BAs...>;

		if (auto neqs = select_all(new_l, is_non_terminal<tau_parser::bf_neq, BAs...>); neqs.size() > 0) {
			auto nneqs = _T<BAs...>;
			for (auto& neq: neqs) {
				auto g = neq | tau_parser::bf | optional_value_extractor<tau<BAs...>>;
				auto g_0 = replace(g, var, _0_trimmed<BAs...>);
				auto g_1 = replace(g, var, _1_trimmed<BAs...>);
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
	return replace<tau<BAs...>>(wff, changes);
}

template<typename... BAs>
tau<BAs...> eliminate_existential_quantifier(const auto& inner_fm, auto& scoped_fm) {
	// Reductions to prevent blow ups and achieve DNF
	BOOST_LOG_TRIVIAL(debug) << "(I) Start existential quantifier elimination";
	BOOST_LOG_TRIVIAL(debug) << "(I) Quantified variable: " << trim2(inner_fm);
	BOOST_LOG_TRIVIAL(debug) << "(F) Quantified formula: " << scoped_fm;
	// scoped_fm = scoped_fm | bf_reduce_canonical<BAs...>();
	scoped_fm = reduce_across_bfs(scoped_fm, false);

#ifdef TAU_CACHE
		static std::map<std::pair<tau<BAs...>,tau<BAs...>>, tau<BAs...>> cache;
		if (auto it = cache.find(make_pair(inner_fm, scoped_fm)); it != end(cache))
			return it->second;
#endif // TAU_CACHE

	auto clauses = get_leaves(scoped_fm, tau_parser::wff_or);
	tau<BAs...> res;
	for (const auto& clause : clauses) {
		// Check if every conjunct in clause is of form f = 0 or f != 0
		auto conjuncts = get_leaves(clause, tau_parser::wff_and);
		bool all_equal_zero = true, all_unequal_zero = true;
		for (const auto& c : conjuncts) {
			if (!is_child_non_terminal(tau_parser::bf_eq, c))
				all_equal_zero = false;
			if (!is_child_non_terminal(tau_parser::bf_neq, c))
				all_unequal_zero = false;
		}
		if (all_unequal_zero) {
			tau<BAs...> new_conjunct;
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
			//TODO: If they have different type, seperate
			tau<BAs...> new_func;
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
tau<BAs...> eliminate_universal_quantifier(const auto& inner_fm, auto& scoped_fm) {
	BOOST_LOG_TRIVIAL(debug) << "(I) Start universal quantifier elimination";
	BOOST_LOG_TRIVIAL(debug) << "(I) Quantified variable: " << trim2(inner_fm);
	BOOST_LOG_TRIVIAL(debug) << "(F) Quantified formula: " << scoped_fm;
	// Reductions to prevent blow ups and achieve CNF
	// scoped_fm = scoped_fm | bf_reduce_canonical<BAs...>();
	scoped_fm = reduce_across_bfs(scoped_fm, true);
// Add cache after reductions; reductions are cached as well
#ifdef TAU_CACHE
		static std::map<std::pair<tau<BAs...>,tau<BAs...>>, tau<BAs...>> cache;
		if (auto it = cache.find(make_pair(inner_fm, scoped_fm)); it != end(cache))
			return it->second;
#endif // TAU_CACHE

	auto clauses = get_leaves(scoped_fm, tau_parser::wff_and);
	tau<BAs...> res;
	for (const auto &clause: clauses) {
		// Check if every disjunct in clause is of form f = 0 or f != 0
		auto disjuncts = get_leaves(clause, tau_parser::wff_or);
		bool all_equal_zero = true, all_unequal_zero = true;
		for (const auto &d: disjuncts) {
			if (!is_child_non_terminal(tau_parser::bf_eq, d))
				all_equal_zero = false;
			if (!is_child_non_terminal(tau_parser::bf_neq, d))
				all_unequal_zero = false;
		}
		if (all_equal_zero) {
			tau<BAs...> new_disjunct;
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
			tau<BAs...> new_func;
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
// and then eliminate them, returning a quantifier free formula
template<typename... BAs>
tau<BAs...> eliminate_quantifiers(const tau<BAs...>& fm) {
	// Lambda is applied to nodes of fm in post order after quantifiers have
	// been pushed in
	auto elim_quant = [](const tau<BAs...>& inner_fm) -> tau<BAs...> {
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
		if (is_ex_quant) {
			return eliminate_existential_quantifier<BAs...>(inner_fm, scoped_fm);
		}
		else {
			return eliminate_universal_quantifier<BAs...>(inner_fm, scoped_fm);
		}
	};
	unordered_tau_set<BAs...> excluded_nodes;
	auto push_quantifiers = [&excluded_nodes](const tau<BAs...>& n) {
		using p = tau_parser;
		if (is_child_non_terminal(p::wff_ex, n)) {
			auto pushed = push_existential_quantifier_one(n);
			if (pushed == n) {
				// Quantifier cannot be pushed deeper
				for (const auto& c : n->child)
					excluded_nodes.insert(c);
				return n;
			} else return pushed;
		} else if (is_child_non_terminal(p::wff_all, n)) {
			auto pushed = push_universal_quantifier_one(n);
			if (pushed == n) {
				// Quantifier cannot be pushed deeper
				for (const auto& c : n->child)
					excluded_nodes.insert(c);
				return n;
			} else return pushed;
		}
		return n;
	};
	auto visit = [&excluded_nodes](const tau<BAs...>& n) {
		using p = tau_parser;
		if (is_non_terminal(p::bf, n)) return false;
		// Do not visit subtrees below a maximally pushed quantifier
		if (excluded_nodes.contains(n)) return false;
		return true;
	};
	// Push quantifiers in during pre-order traversal
	// and eliminate quantifiers during the traversal back up (post-order)
	auto push_and_elim = [&elim_quant, &push_quantifiers, visit](const tau<BAs...>& n) {
		if (is_child_quantifier<BAs...>(n)) {
			return pre_order(n).template
			apply_unique<MemorySlotPre::eliminate_quantifiers_m>(
				push_quantifiers, visit, elim_quant);
		} else return n;
	};
	return post_order(fm).apply_unique(push_and_elim, visit_wff<BAs...>);
}

// fm is assumed to be quantifier free
template <typename... BAs>
tau<BAs...> get_eq_with_most_quant_vars (const tau<BAs...>& fm, const auto& quant_vars) {
	using p = tau_parser;
	// std::cout << "Begin get_eq_with_most_quant_vars with\n";
	// std::cout << "fm: " << fm << "\n";
	// std::cout << "quantified vars: " << quant_vars << "\n";
	tau<BAs...> eq_max_quants;
	int_t max_quants = 0;
	auto get_eq = [&](const tau<BAs...>& n) {
		if (is_non_terminal(p::bf_eq, n)) {
			// Found term
			// Get vars
			auto vars = select_top(n, is_non_terminal<p::variable, BAs...>);
			// Find overlap of vars and quant_vars
			int_t quants = 0;
			for (const auto& v : vars)
				if (quant_vars.contains(v))
					++quants;
			if (quants >= max_quants)
				max_quants = quants, eq_max_quants = n;
			// Dont go deeper
			return false;
		}
		// Go deeper
		return true;
	};
	pre_order(fm).visit(get_eq, visit_wff<BAs...>, identity);
	return wrap(p::wff, eq_max_quants);
}

template <typename... BAs>
std::pair<tau<BAs...>, bool> anti_prenex_finalize_ex (const tau<BAs...>& q, const tau<BAs...>& scoped_fm) {
	using p = tau_parser;
	// Check if single disjunct
	if (!find_top(scoped_fm, is_non_terminal<p::wff_or, BAs...>))
		return {wff_remove_existential(q, scoped_fm), true};
	// Check if all atomic fms are negative or if all are positive
	static unordered_tau_set<BAs...> mixed_eqs;
	if (mixed_eqs.contains(scoped_fm)) return {scoped_fm, false};
	bool all_atm_fm_neq = true, all_atm_fm_eq = true;
	auto check_atm_fms = [&all_atm_fm_neq, &all_atm_fm_eq, &q](const tau<BAs...>& n) {
		if (is_non_terminal(p::bf_eq, n) && contains(n, q)) {
			all_atm_fm_neq = false;
			return all_atm_fm_eq != false;
		} else if (is_non_terminal(p::bf_neq, n) && contains(n, q)) {
			all_atm_fm_eq = false;
			return all_atm_fm_neq != false;
		} else if (is_non_terminal(p::wff_ref, n)) {
			all_atm_fm_neq = false;
			all_atm_fm_eq = false;
			return false;
		}
		return true;
	};
	pre_order(scoped_fm).search_unique(check_atm_fms, visit_wff<BAs...>, identity);
	if (all_atm_fm_neq) {
		// std::cout << "all atomic fms are negative\n";
		// All atomic formulas are of the form !=
		auto elim_quants = [&q](const tau<BAs...>& n) {
			if (is_child_non_terminal(p::bf_neq, n) && contains(n, q)) {
				return wff_remove_existential(q, n);
			} else return n;
		};
		return {pre_order(scoped_fm).apply_unique_until_change(
			elim_quants, visit_wff<BAs...>, identity), true};
	} else if (all_atm_fm_eq) {
		// std::cout << "all atomic fms are positive\n";
		tau<BAs...> red = reduce_across_bfs(scoped_fm, false);
		return {wff_remove_existential(q, red), true};
	}
	mixed_eqs.insert(scoped_fm);
	return {scoped_fm, false};
}

template <typename... BAs>
tau<BAs...> anti_prenex (const tau<BAs...>& fm) {
	unordered_tau_set<BAs...> excluded_nodes, quant_vars;
	auto anti_prenex_step = [&excluded_nodes](const tau<BAs...>& n) {
		if (is_child_quantifier<BAs...>(n)) {
			// std::cout << "Start anti_prenex_step\n";
			// Try push quant down
			auto pushed = push_existential_quantifier_one(n);
			if (pushed != n) {
				// std::cout << "Pushed existential one:\n";
				// std::cout << "From " << n << "\n";
				// std::cout << "To " << pushed << "\n\n";
				return pushed;
			}
			tau<BAs...> scoped_fm = trim(n)->child[1];
			const auto& q_v = trim2(n);
			// Try apply finalize
			auto [r, suc] = anti_prenex_finalize_ex(q_v, scoped_fm);
			if (suc) {
				// r = reduce_across_bfs(r, false);
				// std::cout << "Finalized subtree:\n";
				// std::cout << "From " << n << "\n";
				// std::cout << "To " << r << "\n\n";
				for (const auto& child : r->child)
					excluded_nodes.insert(child);
				return r;
			}

			// Try bf_reduce_across for now
			tau<BAs...> res = eliminate_existential_quantifier<BAs...>(n, scoped_fm);
			for (const auto& child : res->child)
				excluded_nodes.insert(child);
			return res;

			/*// Boole decomposition
			auto eq = get_eq_with_most_quant_vars(scoped_fm, quant_vars);
			//TODO: absorbtions
			auto left = build_wff_and(eq, replace(scoped_fm, eq, _T<BAs...>));
			if (left == _F<BAs...>) {
				// Boole decomp does not create two branches
				assert(find_top(n, is_non_terminal<tau_parser::wff_or, BAs...>));
				auto dis = single_dis_lift(n);
				auto res = push_existential_quantifier_one(dis);
				// std::cout << "Single || lift:\n";
				// std::cout << "From " << n << "\n";
				// ptree(std::cout, n);
				// std::cout << "\n\n";
				// std::cout << "dis: " << dis << "\n";
				// std::cout << "To " << res << "\n\n";
				return res;
			}
			auto neq = build_wff_neq(trim2(eq));
			auto right = build_wff_and(neq, replace(scoped_fm, eq, _F<BAs...>));
			if (right == _F<BAs...>) {
				// Boole decomp does not create two branches
				assert(find_top(n, is_non_terminal<tau_parser::wff_or, BAs...>));
				auto dis = single_dis_lift(n);
				auto res = push_existential_quantifier_one(dis);
				// std::cout << "Single || lift:\n";
				// std::cout << "From " << n << "\n";
				// ptree(std::cout, n);
				// std::cout << "\n\n";
				// std::cout << "dis: " << dis << "\n";
				// std::cout << "To " << res << "\n\n";
				return res;
			}
			auto res = build_wff_or(
				build_wff_ex(q_v, left),
				build_wff_ex(q_v, right));
			// std::cout << "Boole decomposition:\n";
			// std::cout << "From " << n << "\n";
			// std::cout << "To " << res << "\n\n";
			return res;*/
		} else return n;
	};
	auto visit = [&excluded_nodes](const tau<BAs...>& n) {
		using p = tau_parser;
		if (is_non_terminal(p::bf, n))
			return false;
		if (excluded_nodes.contains(n)) return false;
		return true;
	};
	auto inner_quant = [&anti_prenex_step, &visit, &quant_vars](const tau<BAs...>& n) {
		if (is_child_quantifier<BAs...>(n)) {
			// TODO: implement universal quantifier explicitly
			using p = tau_parser;
			if (is_child_non_terminal(p::wff_all, n)) {
				// std::cout << "Before elimination:\n";
				// std::cout << n << "\n\n";
				auto n_neg = push_negation_in(build_wff_neg(n));
				auto res = pre_order(n_neg).template
				apply_unique<MemorySlotPre::anti_prenex_step_m>(
					anti_prenex_step, visit, identity);
				quant_vars.erase(trim2(n));
				res = push_negation_in(build_wff_neg(res));
				// std::cout << "After elimination:\n";
				// std::cout << res << "\n\n";
				// std::cout << "Simp dnf after elimination:\n";
				// std::cout << reduce_across_bfs(res, false) << "\n\n";
				// std::cout << "Simp cnf after elimination:\n";
				res = reduce_across_bfs(res, true);
				// std::cout << res << "\n\n";
				return res;
			} else {
				// std::cout << "Before elimination:\n";
				// std::cout << n << "\n\n";
				auto res = pre_order(n).template
				apply_unique<MemorySlotPre::anti_prenex_step_m>(
					anti_prenex_step, visit, identity);
				quant_vars.erase(trim2(n));
				// std::cout << "After elimination:\n";
				// std::cout << res << "\n\n";
				// std::cout << "Simp dnf after elimination:\n";
				// std::cout << reduce_across_bfs(res, false) << "\n\n";
				// std::cout << "Simp cnf after elimination:\n";
				res = reduce_across_bfs(res, false);
				// std::cout << res << "\n\n";
				return res;
			}
		} else return n;
	};
	auto visit_inner_quant = [&quant_vars](const tau<BAs...>& n) {
		using p = tau_parser;
		if (is_quantifier<BAs...>(n))
			quant_vars.insert(trim(n));
		if (is_non_terminal(p::bf, n))
			return false;
		return true;
	};
	auto nnf = push_negation_in(fm);
	return post_order(nnf).template
	apply_unique<MemorySlotPost::anti_prenex_m>(inner_quant, visit_inner_quant);
}

template <typename... BAs>
tau<BAs...> replace_free_vars_by (const tau<BAs...>& fm, const tau<BAs...>& val) {
	assert(!is_non_terminal(tau_parser::bf, val));
	auto free_vars = get_free_vars_from_nso(fm);
	if (!free_vars.empty()) {
		std::map<tau<BAs...>, tau<BAs...>> free_var_assgm;
		for (const auto& free_var : free_vars)
			free_var_assgm.emplace(free_var, val);
		return replace(fm, free_var_assgm);
	} else return fm;
}

// We assume that the input is a formula is in MNF (with no quantifiers whatsoever).
// We implicitly transformed into BDD form and compute one step of the SNF transformation.
template<typename...BAs>
struct to_snf_step {

	using var = tau<BAs...>;
	using vars = std::set<var>;
	using constant = std::variant<BAs...>;
	using exponent = std::set<var>;
	using literal = tau<BAs...>;
	using literals = std::set<literal>;
	using partition = std::map<exponent, literals>;
	using bdd_path = std::pair<partition /* positive */, partition /* negatives */>;

	tau<BAs...> operator()(const tau<BAs...>& form) const {
		// we select all literals, i.e. wff equalities or it negations.
		static const auto is_literal = [](const auto& n) -> bool {
			return (n | tau_parser::bf_eq).has_value();
		};
		if (auto literals = select_all(form, is_literal); !literals.empty()) {
			// we call the recursive method traverse to traverse all the paths
			// of the BDD.
			std::set<tau<BAs...>> remaining(literals.begin(), literals.end());
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

	tau<BAs...> bdd_path_to_snf(const bdd_path& path, const tau<BAs...>& form) const {
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

	tau<BAs...> traverse(const bdd_path& path, const literals& remaining, const tau<BAs...>& form) const {
		// we only cache results in release mode
		#ifdef TAU_CACHE
		static std::map<std::tuple<bdd_path, literals, tau<BAs...>>, tau<BAs...>> cache;
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

	exponent get_exponent(const tau<BAs...>& n) const {
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

	tau<BAs...> squeeze_positives(const std::set<literal>& positives, const exponent& exp) const {
		// find first element with non trivial constant
		auto first = std::find_if(positives.begin(), positives.end(), [&](const auto& l) {
			return get_constant(l).has_value();
		});
		// if there is no such element we return the first element
		if (first == positives.end()) return tau<BAs...>(*positives.begin());
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
	tau<BAs...> normalize(const bdd_path& path) const {
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

	tau<BAs...> normalize_positive(const bdd_path& path, const literal& positive) const {
		auto relative_path = get_relative_path(path, positive);
		return normalize(add_to_positive_path(relative_path, positive));
	}

	tau<BAs...> normalize_negative(const bdd_path& path, const literal& negative) const {
		auto relative_path = get_relative_path(path, negative);
		return normalize(add_to_negative_path(relative_path, negative));
	}
};

template<typename...BAs>
tau<BAs...> operator|(const tau<BAs...>& n, const to_snf_step<BAs...>& r) {
	return r(n);
}

template<typename...BAs>
tau<BAs...> snf_bf(const tau<BAs...>& n) {
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
tau<BAs...> snf_wff(const tau<BAs...>& n) {
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
	return replace(n, nn, second_step);
}

template<typename...BAs>
tau<BAs...> build_split_wff_using(tau_parser::nonterminal type, const tau<BAs...>& a, const tau<BAs...>& b) {
	// TODO (HIGH) check formulas, should depend on the type
	if (type == tau_parser::bf_eq) return build_wff_or(build_wff_and(a, b), build_wff_and(build_wff_neg(a), build_wff_neg(b)));
	else return build_wff_and(build_wff_or(a, build_wff_neg(b)), build_wff_or(build_wff_neg(a), b));
}

template<typename...BAs>
tau<BAs...> mnf_wff(const tau<BAs...>& n) {
	auto [_, nn] = get_inner_quantified_wff(n);
	auto mnf = nn | repeat_all<step<BAs...>, BAs...>(
			to_dnf_wff<BAs...>
			| to_mnf_wff<BAs...>)
		| repeat_all<step<BAs...>, BAs...>(
			to_mnf_wff<BAs...>)
		| reduce_wff<BAs...>;
	std::map<tau<BAs...>, tau<BAs...>> changes = {{nn, mnf}};
	return replace(n, nn, mnf);
}

template<typename...BAs>
tau<BAs...> mnf_bf(const tau<BAs...>& n) {
	return n | repeat_all<step<BAs...>, BAs...>(
			to_dnf_bf<BAs...>
			| elim_eqs<BAs...>)
		// TODO (MEDIUM) review after we fully normalize bf & wff
		| reduce_bf<BAs...>;
}

template<size_t type, typename...BAs>
tau<BAs...> anf(const tau<BAs...>& n) {
	// TODO (MEDIUM) write anf (using?)
	std::cout << "Not implemented yet." << std::endl;
	return n;
}

template<typename...BAs>
tau<BAs...> pnf(const tau<BAs...>& n) {
	// TODO (MEDIUM) write pnf (using?)
	std::cout << "Not implemented yet." << std::endl;
	return n;
}

} // namespace idni::tau_lang

#endif // __NORMAL_FORMS_H__