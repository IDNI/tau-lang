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

#ifndef __SOLVER_H__
#define __SOLVER_H__

#include "tau.h"
#include "satisfiability.h"
#include "normal_forms.h"

// in what follows we use the algorithms and notations of TABA book (cf.
// Section 3.2).Chek (https://github.com/IDNI/tau-lang/blob/main/docs/taba.pdf)
// for the details.

using namespace idni::rewriter;
using namespace idni::tau;

namespace idni::tau {

template<typename...BAs>
using clause = nso<BAs...>;

template<typename...BAs>
using var = nso<BAs...>;

template<typename...BAs>
using bound = std::variant<BAs...>;

template<typename...BAs>
using bounds = std::vector<bound<BAs...>>;

template<typename...BAs>
struct nlgeqs_clause {
	bounds<BAs...> lowers; // c's in the TABA book (cf. Section 3.2)
	bounds<BAs...> uppers; // d's in the TABA book (cf. Section 3.2)
};

template<typename...BAs>
using splitter_set = std::set<std::variant<BAs...>>;

template<typename...BAs>
static constexpr auto _leq = overloaded([]<typename T>(const T& l, const T& r) -> bool {
		return (~l & r = false);
	}, [](const auto&, const auto&) -> sp_tau_node<BAs...> { throw std::logic_error("wrong types"); });

template<typename...BAs>
static constexpr auto _eq = overloaded([]<typename T>(const T& l, const T& r) -> bool {
		// TODO (HIGH) fix this
		return (~l & r = false);
	}, [](const auto&, const auto&) -> sp_tau_node<BAs...> { throw std::logic_error("wrong types"); });

template<typename...BAs>
static constexpr auto _and = overloaded([]<typename T>(const T& l, const T& r) -> bool {
			return l & r;
	}, [](const auto&, const auto&) -> sp_tau_node<BAs...> { throw std::logic_error("wrong types"); });

template<typename...BAs>
static constexpr auto _or = overloaded([]<typename T>(const T& l, const T& r) -> bool {
			return l | r;
	}, [](const auto&, const auto&) -> sp_tau_node<BAs...> { throw std::logic_error("wrong types"); });

template<typename...BAs>
static constexpr auto _neg = [](const auto& l) -> std::variant<BAs...> { return ~l; };

static constexpr auto _is_zero = [](const auto& l) -> bool { return l == false; };

static constexpr auto _is_geq_zero = [](const auto& l) -> bool { return l == false; };

static constexpr auto _is_leq_one = [](const auto& l) -> bool { return l == false; };

// general comment regarding const use, we don't use const to decorate the splitters
// us they could involve side effects, and we don't want to restrict the user in
// any way. The user should be able to use any kind of splitter he/she wants.

// following Lemma 3.4 of TABA book we could compute maximal and minilmal solutions.
// However, in our case, as we have used the LGRS to remove the equality from
// our initial system, we have a = 0 and b = 1. Thus, there are no maximal
// or minimal solutions. We can directly compute the solution using the splitters.
//
// the following code only makes sense if we have maximal and minimal solutions, i.e.
// if we have a != 0 and b != 1, otherwise the checks are trivially satisfied.
//
// template<typename...BAs>
// bool has_clause_maximal_solution(nlgeqs_clause<BAs...> clause) {
// 	auto b = clause.interval.second;
// 	for (auto& c_i: clause.lowers) if (std::visit(_leq<BAs...>, c_i, b)) return false;
// 	return true;
// }
//
// template<typename...BAs>
// bool has_clause_minimal_solution(nlgeqs_clause<BAs...> clause) {
// 	auto a = clause.interval.first;
// 	for (auto& d_i: clause.uppers) if (std::visit(_leq<BAs...>, a, d_i)) return false;
// 	return true;
// }

// compute the splitter set of a system on negations of inequalities.
template<typename...BAs>
splitter_set<BAs...> compute_splitter_set(const nlgeqs_clause<BAs...>& clause) {
	// we follow Lemma 3.3 of TABA book taking into account the following: instead
	// of computing a set T of pairs of A's and B's according to the Lemma, we
	// directly compute C^AD^B and store it into the splitter set if not null,
	// otherwise we ommit it.
	//
	// that way we avoid computing the corresponding product C^AD^B again and also
	// could avoid computing splitters twice when different A's and B's have the
	// same product.
	splitter_set<BAs...> ss;
	for (size_t i = 0; i < clause.lowers.size(); ++i) {
		for (size_t j = 0; j < clause.uppers.size(); ++j) {
			std::variant<BAs...> result = clause.lowers[i];
			// compute a splitter set element such that it give us a partial solution
			// for the clause (awaiting computing the remaining part of the solution
			// depending on the d_j's)
			for (size_t ii = 0; ii < clause.lowers.size(); ++ii)
				// (result <= c_ii) <-> (result c_ii' = 0)
				if (std::visit(_leq<BAs...>, result, clause.lowers[ii]))
					// result <= c_ii, i.e. we must choose c_ii to avoid 0
					result = std::visit(_and<BAs...>, result, clause.lowers[ii]);
					// otherwise we must take the negation of c_ii
				else result = std::visit(_and<BAs...>, result, std::visit(_neg<BAs...>, clause.lowers[ii]));

			// if result d_j = 0 we must choose another d_j
			result = std::visit(_and<BAs...>, result, clause.uppers[j]);
			if (std::visit(_is_zero, result)) continue;

			// compute the rest ot the splitter set element taking into
			// account the d_j's to compute the final solution (avoiding 0)
			for (size_t jj = 0; jj < clause.uppers.size(); ++jj)
				// (result <= d_jj) <-> (result d_jj' = 0)
				if (std::visit(_leq<BAs...>, clause.uppers[j], clause.uppers[jj]))
					// result <= d_jj, i.e. we must choose d_jj to avoid 0
					result = std::visit(_and<BAs...>, result, clause.uppers[jj]);
					// otherwise we must take the negation of d_jj
				else result = std::visit(_and<BAs...>, result, std::visit(_neg<BAs...>, clause.uppers[jj]));

			// if the final result is not 0 we add it to the splitter set as
			// is part of the solution
			if (!std::visit(_is_zero, result)) ss.insert(result);
		}
	}
	return ss;
}

template<typename splitter_t, typename...BAs>
std::optional<std::variant<BAs...>> solve_clause_using_splitter(const nlgeqs_clause<BAs...>& clause, splitter_t& splitter) {
	// Following Lemma 3.4 of TABA book we could compute maximal and minilmal solutions.
	// However, in our case, as we have used the LGRS to remove the equality from
	// our initial system, we have a = 0 and b = 1. Thus, there are no maximal
	// or minimal solutions. We can directly compute the solution using the splitters.

	// the following code only makes sense if we have maximal and minimal solutions
	// if (has_clause_maximal_solution(clause)) return ...;
	// if (has_clause_minimal_solution(clause)) return ...;

	// in our case we can directly compute the solution using the splitter set
	// and adding the results of the splitter over the splitter set. We should
	// return {} if we are unable to compute a given splitter for whatever reason.
	std::variant<BAs...> result;
	for (auto& split: compute_splitter_set(clause)) {
		if (auto splitted = splitter(split); splitted.has_value())
			result = std::visit(_or<BAs...>, result, splitted.value());
		else return {};
	}
	return result;
}

template<typename...BAs>
std::pair<var<BAs...>, std::optional<nso<BAs...>>> eliminate_interval(const nso<BAs...>& clause) {
	auto interval = find_top(clause, is_non_terminal<tau_parser::bf_interval, BAs...>);
	auto var = interval | tau_parser::variable
		| optional_value_extractor<sp_tau_node<BAs...>>;
	auto bounds = interval || tau_parser::bf;
	// given a <= x <= b we can replace it by x = a x + b x'
	auto subs = trim(build_bf_xor_from_def<BAs...>(
		build_bf_and<BAs...>(bounds[0], var),
		build_bf_and<BAs...>(bounds[1], build_bf_neg<BAs...>(var))));
	auto form = replace(clause, {{var, subs}})
		| repeat_each<step<BAs...>, BAs...>(
			to_dnf_bf<BAs...>
			| simplify_bf<BAs...>
			| apply_cb<BAs...>
			| elim_eqs<BAs...>)
		| reduce_bf<BAs...>();
	return {var, form};
}

template<typename...BAs>
bounds<BAs...> compute_lower_bounds(const var<BAs...> v, const nso<BAs...>& clause) {
	bounds<BAs...> lowers;
	auto f_0 = replace(clause,  {{v, _0<BAs...>}})
		| repeat_each<step<BAs...>, BAs...>(
			simplify_bf<BAs...>
			| apply_cb<BAs...>
			| elim_eqs<BAs...>)
		| reduce_bf<BAs...>();
	for (auto& neq: select_all(f_0, is_non_terminal<tau_parser::bf_nleq, BAs...>)) {
		auto c_i = neq
			| tau_parser::bf
			| only_child_extractor<sp_tau_node<BAs...>>
			| ba_extractor<BAs...>
			| optional_value_extractor<sp_tau_node<BAs...>>;
		lowers.push_back(c_i);
	}
	return lowers;
}

template<typename...BAs>
bounds<BAs...> compute_upper_bounds(const var<BAs...>& v, const nso<BAs...>& clause) {
	bounds<BAs...> uppers;
	auto f_1 = replace(clause, {{v, _1<BAs...>}})
		| repeat_each<step<BAs...>, BAs...>(
			simplify_bf<BAs...>
			| apply_cb<BAs...>
			| elim_eqs<BAs...>)
		| reduce_bf<BAs...>();

	for (auto& neq: select_all(f_1, is_non_terminal<tau_parser::bf_nleq, BAs...>)) {
		auto neg_d_i = neq
			| tau_parser::bf
			| only_child_extractor<sp_tau_node<BAs...>>
			| ba_extractor<BAs...>
			| optional_value_extractor<sp_tau_node<BAs...>>;
		auto d_i = std::visit(_neg<BAs...>, neg_d_i);
		uppers.push_back(d_i);
	}
	return uppers;
}

template<typename splitter_t, typename...BAs>
std::optional<std::variant<BAs...>> solve_clause_using_splitter(const clause<BAs...>& clause, splitter_t splitter) {
	auto [var, simplified] = eliminate_interval(clause);
	auto nlg_eqs = { compute_lower_bounds(var, simplified), compute_upper_bounds(var, simplified) };
	return solve_clause_using_splitter(nlg_eqs, splitter);
}

// solves an univariate equation using splitters. We tacitely assume that the formula
// is quatifier-free.
// for the time beeing, we don't assume that the formula has been simplified, in dnf form,
// or in any other form. We may drop such assumption in the future.
template<typename splitter_t, typename...BAs>
std::optional<std::variant<BAs...>> solve_using_splitter(const nso<BAs...>& nso, splitter_t splitter) {
	auto var = find_top(nso, is_non_terminal<tau_parser::variable, BAs...>)
		| optional_value_extractor<sp_tau_node<BAs...>>;
	auto onf = onf_subformula(nso, var);
	// finally, for each clause we try to find a solution using the splitter
	for (auto& clause: get_dnf_clauses<tau_parser::wff>(onf))
		if (auto solution = solve_clause_using_splitter(clause, splitter);
			solution.has_value()) return solution.value();
	// we were unable to compute a proper splitter at some point which was crucial
	// for computing the solution, so we return {}. This is a failure.
	return {};
}

} // idni::tau namespace
#endif // __SOLVER_H__