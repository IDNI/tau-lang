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

#include "normalizer2.h"

#ifdef DEBUG
#include "debug_helpers.h"
#endif // DEBUG

using namespace idni::rewriter;
using namespace idni::tau;

namespace idni::tau {

// conjunctive normal form
RULE(BF_TO_CNF_0, "$X & $Y | $Z := ($X | $Z) & ($Y | $Z).")
RULE(BF_TO_CNF_1, "$X | $Y & $Z := ($X | $Y) & ($X | $Z).")
RULE(WFF_TO_CNF_0, "$X && $Y || $Z ::= ($X || $Z) && ($Y || $Z).")
RULE(WFF_TO_CNF_1, "$X || $Y && $Z ::= ($X || $Y) && ($X || $Z).")

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
static auto to_nnf_bf = make_library<BAs...>(
	BF_PUSH_NEGATION_INWARDS_0
	+ BF_PUSH_NEGATION_INWARDS_1
	+ BF_ELIM_DOUBLE_NEGATION_0
);

template<typename...BAs>
nso<BAs...> onf(const nso<BAs...>& n, const nso<BAs...>& var) {
	auto pred = [&var](const auto& n) {
		if (auto check = n | tau_parser::wff_ex | tau_parser::variable; check.has_value()) {
			return (var == check.value());
		}
		return false;
	};
	if (auto quantifier = find_bottom(n, pred); quantifier.has_value()) {
		auto sub_formula = quantifier | tau_parser::wff | optional_value_extractor<nso<BAs...>>;
		return onf_subformula(sub_formula, var);
	}
	return n;
}

template<typename...BAs>
nso<BAs...> onf_subformula(const nso<BAs...>& n, const nso<BAs...>& var) {
	auto eq = find_bottom(n, is_non_terminal<tau_parser::bf_eq, BAs...>);
	std::map<nso<BAs...>, nso<BAs...>> changes_0 = {{var, _0<BAs...>}};
	auto f_0 = replace((eq || tau_parser::bf)[0],  changes_0)
		| repeat_each<step<BAs...>, BAs...>(
			simplify_bf<BAs...>
			| apply_cb<BAs...>)
		| to_mnf_bf<BAs...>();
	std::map<nso<BAs...>, nso<BAs...>> changes_1 = {{var, _1<BAs...>}};
	auto f_1 = replace((eq || tau_parser::bf)[0],  changes_1)
		| repeat_each<step<BAs...>, BAs...>(
			simplify_bf<BAs...>
			| apply_cb<BAs...>)
		| to_mnf_bf<BAs...>();
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
			| to_mnf_bf<BAs...>();
		std::map<nso<BAs...>, nso<BAs...>> changes_neq_1 = {{var, _1<BAs...>}};
		auto f_1 = replace(bounds[0],  changes_neq_1)
			| repeat_each<step<BAs...>, BAs...>(
				simplify_bf<BAs...>
				| apply_cb<BAs...>)
			| to_mnf_bf<BAs...>();
		auto nleq_change = build_bf_or(build_bf_nleq_lower(f_0, var), build_bf_nleq_upper(f_1, var));
		changes[neq] = nleq_change;
	}
	return replace(n, changes);
}

template<size_t type, typename...BAs>
nso<BAs...> dnf(const nso<BAs...>& n) {
	if constexpr (type == tau_parser::wff) return n
		| repeat_each<step<BAs...>, BAs...>(step<BAs...>(
			to_dnf_wff<BAs...>));
	else return n
		| repeat_each<step<BAs...>, BAs...>(step<BAs...>(
			to_dnf_bf<BAs...>));
}

template<size_t type, typename...BAs>
nso<BAs...> cnf(const nso<BAs...>& n) {
	if constexpr (type == tau_parser::wff) return n
		| repeat_each<step<BAs...>, BAs...>(step<BAs...>(
			to_cnf_wff<BAs...>));
	else return n
		| repeat_each<step<BAs...>, BAs...>(step<BAs...>(
			to_cnf_bf<BAs...>));
}

template<size_t type, typename...BAs>
nso<BAs...> anf(const nso<BAs...>& n) {
	// TODO (MEDIUM) write anf (using?)
	std::cout << "Not implemented yet." << std::endl;
	return n;
}

template<size_t type, typename...BAs>
nso<BAs...> nnf(const nso<BAs...>& n) {
	if constexpr (type == tau_parser::wff)
		return n | repeat_each<step<BAs...>, BAs...>(
			step<BAs...>(to_nnf_wff<BAs...>)
		);
	else
		return n | repeat_each<step<BAs...>, BAs...>(
			step<BAs...>(to_nnf_bf<BAs...>)
		);
}

template<size_t type, typename...BAs>
nso<BAs...> pnf(const nso<BAs...>& n) {
	// TODO (MEDIUM) write pnf (using?)
	std::cout << "Not implemented yet." << std::endl;
	return n;
}

template<size_t type, typename...BAs>
nso<BAs...> mnf(const nso<BAs...>& n) {
	if constexpr (type == tau_parser::wff)
		return n | repeat_each<step<BAs...>, BAs...>(to_dnf_wff<BAs...>)
			| to_mnf_wff<BAs...>();
	else
		return n | repeat_each<step<BAs...>, BAs...>(to_dnf_bf<BAs...>)
			| to_mnf_bf<BAs...>();
}

} // namespace idni::tau

#endif // __NORMAL_FORMS_H__