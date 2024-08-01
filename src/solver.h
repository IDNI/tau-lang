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

#include <stack>

#include "tau.h"
#include "normal_forms.h"

// In what follows we use the algorithms and notations of TABA book (cf.
// Section 3.2).Chek (https://github.com/IDNI/tau-lang/blob/main/docs/taba.pdf)
// for the details.

using namespace idni::rewriter;
using namespace idni::tau;

namespace idni::tau {

template<typename...BAs>
using var = nso<BAs...>;

template<typename...BAs>
using minterm = nso<BAs...>;

template<typename...BAs>
using equality = nso<BAs...>;

template<typename...BAs>
using inequality = nso<BAs...>;

template<typename...BAs>
using system = std::pair<std::optional<equality<BAs...>>, std::set<inequality<BAs...>>>;

template<typename...BAs>
using inequality_system = std::set<inequality<BAs...>>;

template<typename...BAs>
using minterm_system = std::set<nso<BAs...>>;

template<typename...BAs>
using solution = std::map<var<BAs...>, nso<BAs...>>;

template<typename...BAs>
solution<BAs...> find_solution(const equality<BAs...>& eq) {
	auto has_no_var = [](const nso<BAs...>& f) {
		return !find(f, is_child_non_terminal<tau_parser::variable, BAs...>);
	};
	// We would use the algorithm subyaccent to the following theorem (of Taba Book):
	//
	// Theorem 3.1. For f (x,X) = xg (X) + x′h (X), let Z be a zero of
	// g (Z) h (Z) (which is guaranteed to exist by Boole’s consistency condition).
	// Then both f (h (Z) ,Z) = 0 and f (g′ (Z) ,Z) = 0.
	// find a variable, say x, in the equality
	auto f = eq | tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<nso<BAs...>>;
	if (auto x = find_top(f, is_child_non_terminal<tau_parser::variable, BAs...>); x) {
		// compute g(X) and h(X) from the equality by substituting x with 0 and 1
		// with x <- h(Z)
		auto g = replace_with(x, _1<BAs...>, f);
		auto h = replace_with(x, _0<BAs...>, f);
		if (has_no_var(f) && has_no_var(g)) return {{x, h}};
		auto restricted = find_solution(g & h);
		auto full = restricted;
		full[x] = replace_with(h, restricted);
		return full;
	}
	return {};
}

template<typename...BAs>
solution<BAs...> lgrs(const equality<BAs...>& f) {
	// We would use Lowenheim’s General Reproductive Solution (LGRS) as given
	// in the following theorem (of Taba Book):
	//
	// Theorem 1.8. Let f : Bn → B be a BF, and assume f (Z) = 0
	// for some Z ∈ Bn. Then the set {X ∈ Bn| (X) = 0} equals precisely
	// the image of ϕ : Bn → Bn defined by ϕ (X) = Zf (X) + Xf′ (X). Decyphering
	// the abuse of notation, this reads ϕ_i (X) = z_if (X)+x_if′ (X).
	auto z = find_solution(f);
	solution<BAs...> phi;
	for (auto& [x_i, z_i] : z)
		phi[x_i] = (z_i & f) + (x_i & ~f);
	return phi;
}

template<typename...BAs>
class minterm_iterator {
public:
	// iterator traits
	using difference_type = size_t;
	using value_type = minterm<BAs...>;
	using pointer = const minterm<BAs...>*;
	using reference = const minterm<BAs...>&;
	using iterator_category = std::input_iterator_tag;

	// sentinel class
	class sentinel {};
	static constexpr sentinel end{};

	minterm_iterator(const nso<BAs...>& f) {
		if (auto vars = select_top(f, is_child_non_terminal<tau_parser::variable, BAs...>); !vars.empty()) {
			// we start with the full bf...
			auto partial_bf = f;
			// ... and the first variable (for computing the first partial minterm)
			auto partial_minterm = ~(*vars.begin());
			for (auto& v : vars) {
				// we add the current choice to the list of choices...
				choices.emplace_back(v, false, partial_bf, partial_minterm);
				// ... and compute new values for the next one
				partial_bf = replace_with(v, _0<BAs...>, partial_bf) | simplify_bf<BAs...>;
				partial_minterm = partial_minterm & ~v;
			}
			if (auto minterm = make_current_minterm(); minterm != _0<BAs...>) current = minterm;
			else compute_next_choice();
		} else exhausted = true;
	}

	minterm_iterator();

	minterm_iterator<BAs...>& operator++() {
		if (exhausted) return *this;
		compute_next_choice();
		return *this;
	}

	minterm_iterator<BAs...> operator++(int) {
		return ++*this;
	}

	bool operator==(const minterm_iterator<BAs...>& that) const {
		return choices == that.choices;
	}

	bool operator!=(const minterm_iterator<BAs...>& that) const {
		return !(*this == that);
	}

	bool operator==(const sentinel&) const {
		return exhausted;
	}

	bool operator!=(const sentinel&) const {
		return !exhausted;
	}

	const minterm<BAs...>& operator*() const {
		return current;
	}

private:
	struct choice {
		var<BAs...> var;
		bool value;
		nso<BAs...> partial_bf;
		nso<BAs...> partial_minterm;
	};

	std::vector<choice> choices;
	minterm<BAs...> current;
	bool exhausted = false;

	nso<BAs...> make_current_minterm() {
		auto cte =  choices.back().value
			? replace_with(choices.back().var, _1<BAs...>, choices.back().partial_bf)
			: replace_with(choices.back().var, _0<BAs...>, choices.back().partial_bf);
		return (cte & choices.back().partial_minterm) | simplify_bf<BAs...>;
	}

	void compute_next_choice() {
		if (exhausted) return;
		// update the choices from right to left
		size_t last_changed_value = choices.size();
		while (last_changed_value > 0)  if (choices[--last_changed_value].value ^= true) break;
		// if all choices are exhausted, we are done
		if (last_changed_value == 0 && choices[0].value == false) { exhausted = true; return; }
		// otherwise we check if the current choice is valid
		update_choices_from(last_changed_value);
		if (auto minterm = make_current_minterm(); minterm != _0<BAs...>) { current = minterm; return; }
		compute_next_choice();
	}

	void update_choices_from(size_t start) {
		auto partial_bf = choices[start].partial_bf;
		auto partial_minterm = _1<BAs...>;
		for (size_t i = start; i < choices.size(); ++i) {
			choices[i].partial_bf = partial_bf;
			choices[i].partial_minterm = (choices[i].value ? ~choices[i].var : choices[i].var) & partial_minterm;
			partial_minterm = choices[i].partial_minterm;
			partial_bf = choices[i].value
				? replace_with(choices[i].var, _1<BAs...>, partial_bf) | simplify_bf<BAs...>
				: replace_with(choices[i].var, _0<BAs...>, partial_bf) | simplify_bf<BAs...>;
		}
	}
};

template<typename...BAs>
class minterm_range {
public:

	explicit minterm_range(const nso<BAs...>& f);

	bool empty() {
		return false;
	}

	minterm_iterator<BAs...> begin() {
		minterm_iterator<BAs...> begin(f);
		return begin;
	}

	minterm_iterator<BAs...>::sentinel end() {
		return minterm_iterator<BAs...>::end;
	}

private:
	nso<BAs...> f;
};

template<typename...BAs>
class minterm_inequality_system_iterator {
public:
	// iterator traits
	using difference_type = size_t;
	using value_type = minterm_system<BAs...>;
	using pointer = const minterm_system<BAs...>*;
	using reference = const minterm_system<BAs...>&;
	using iterator_category = std::input_iterator_tag;

	// sentinel class
	class sentinel {};
	static constexpr sentinel end{};

	minterm_inequality_system_iterator(const inequality_system<BAs...>& sys) {
		for (auto& ineq: sys) {
			auto f = ineq | tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<nso<BAs...>>;
			ranges.push_back(minterm_range(f));
		}
		for (auto& range: ranges) minterm_iterators.push_back(range.begin());
	}

	minterm_inequality_system_iterator();

	minterm_inequality_system_iterator<BAs...> &operator++() {
		size_t i = minterm_iterators.size() - 1;
		while (--i > 0) {
			if (++minterm_iterators[i] != ranges[i].end()) break;
			else minterm_iterators[i] = ranges[i].begin();
		}
		if (i == 0) minterm_iterators[0] = ranges[0].end();
		for (auto& it: minterm_iterators) *it = build_wff_neq(*it);
		return *this;
	}

	minterm_inequality_system_iterator<BAs...> operator++(int) {
		// TODO (MEDIUM) check if this is correct
		return *this;
	}

	bool operator==(const minterm_inequality_system_iterator<BAs...>& that) const {
		return this.ranges == that.ranges && this.minterm_iterators == that.minterm_iterators;
	}

	bool operator!=(const minterm_inequality_system_iterator<BAs...>& that) const {
		return !(*this == that);
	}

	bool operator==(const sentinel &that) const {
		return minterm_iterators.front() == ranges.front().end();
	}

	bool operator!=(const sentinel &that) const {
		return !(*this == that);
	}

	const minterm_system<BAs...>& operator*() const {
		return current;
	}

private:
	std::vector<minterm_range<BAs...>> ranges;
	std::vector<minterm_iterator<BAs...>> minterm_iterators;
	minterm_system<BAs...> current;
};

template<typename...BAs>
class minterm_inequality_system_range {
public:

	explicit minterm_inequality_system_range(const inequality_system<BAs...>& sys);

	bool empty() {
		return sys.size() == 0;
	}

	minterm_inequality_system_iterator<BAs...> begin() {
		minterm_inequality_system_iterator<BAs...> begin(sys);
		return begin;
	}

	minterm_inequality_system_iterator<BAs...>::sentinel end() {
		return minterm_inequality_system_iterator<BAs...>::end;
	}

private:
	inequality_system<BAs...> sys;
};

template<typename...BAs>
bool has_solution(const minterm_system<BAs...>& sys) {
	auto bs = _1<BAs...>;
	for (auto& m: sys) bs = bs & get_constant(m);
	return bs != _0<BAs...>;
}

template<typename...BAs>
minterm_system<BAs...> add_minterm_to_disjoint(const minterm_system<BAs...>& disjoint, const minterm<BAs...>& m) {
	minterm_system<BAs...> new_disjoint = disjoint;
	auto new_m = m;
	for (auto& d: disjoint) {
		if (auto d_cte = get_constant(d), new_m_cte = get_constant(new_m); d_cte & new_m_cte != false) {
			if (d_cte & ~new_m_cte != false) new_disjoint.insert(~new_m_cte & d);
			else if (new_m_cte & ~d_cte != false) {
				new_disjoint.insert(d);
				new_m = ~d_cte & new_m;
			} else {
				auto s = splitter(d_cte | tau_parser::bf_constant | optional_value_extractor<nso<BAs...>>);
				new_disjoint.insert(s & d);
				new_m = ~s & new_m;
			}
		} else new_disjoint.insert(d);
	}
	new_disjoint.insert(new_m);
	return new_disjoint;
}

template<typename...BAs>
minterm_system<BAs...> make_minterm_system_disjoint(const minterm_system<BAs...>& sys) {
	minterm_system<BAs...> disjoints;
	for (auto& it = sys.begin(); it != sys.end(); ++it) disjoints = add_minterm_to_disjoint(disjoints, *it);
	return disjoints;
}

template<typename...BAs>
nso<BAs...> get_constant(const minterm<BAs...>& m) {
	auto cte = find_top(m, is_child_non_terminal<tau_parser::constant, BAs...>);
	return cte ? cte : _1<BAs...>;
}

template<typename...BAs>
nso<BAs...> get_minterm(const minterm<BAs...>& m) {
	auto cte = find_top(m, is_child_non_terminal<tau_parser::constant, BAs...>);
	return cte ? replace_with(cte.value(), _1<BAs...>, m) : m;
}

template<typename...BAs>
solution<BAs...> solve_minterm_system(const minterm_system<BAs...>& sys) {
	// To solve the minterm system, we use the Corollary 3.2 (of Taba Book),
	// the splitters to compute proper c_i's, and finally, use find_solution
	// to compute one solution of the resulting system of equalities (squeezed).
	if (!has_solution(sys)) return {};
	equality<BAs...> eq = _F<BAs...>;
	for (auto& neq: make_minterm_system_disjoint(sys)) {
		auto cte = get_constant(neq);
		auto minterm = get_minterm(neq);
		eq = eq | (cte & ~minterm);
	}
	eq = build_wff_eq(eq);
	return find_solution(eq);
}

template<typename...BAs>
solution<BAs...> solve(const inequality_system<BAs...>& sys) {
	// Following Taba book:
	//
	// To solve  {h_i (T) ̸= 0}i∈I (and hence the original system whose solution
	// is then given by ϕ (T). To this end, for each h_i we find a
	// bitstring H_i s.t. h_i (H_i) ̸= 0. This is the same as writing h_i in minterm
	// normal form (alternatively DNF), choosing one minterm (which corresponds
	// to H_i), and h_i (H_i) will yield the coefficient of that minterm.
	// We now get a system of the form
	// 		X^H_i h_i (H_i) ̸= 0
	// (the “minterm system” hereby) which clearly depends on the choice of
	// H_i but any such single choice, if has a solution, will yield a solution to
	// the original system, and vice versa: if a solution to the original system
	// exists, then such a choice exists.
	// for each possible choice of H_i's, we try to solve the minterm system
	// using tthe above solve method.
	for (auto& ms: minterm_system_range(sys)) {
		auto solution = solve_minterm_system(ms);
		if (!solution.empty()) return solution;
	}
	return {};
}

template<typename...BAs>
solution<BAs...> solve(const system<BAs...>& sys) {
	// As in the Taba book, we consider
	// 		f (X) = 0
	//		{g_i (X) ̸= 0}i∈I
	// and let ϕ be the LGRS of f (wrt some arbitrarily chosen single zero of
	// f), and assume that a solution to the whole system, exists. Set h_i (X) =
	// g_i (ϕ (X)) and suppose T satisfies {h_i (T) ̸= 0}i∈I , then f (T) = 0 because
	// the LGRS is reproductive (cf. remark 1.2). So to solve the
	// original system we only need to solve {h_i (T) ̸= 0}i∈I and the solution
	// to the original system is then ϕ (T).

	// TODO (HIGH) check for constant equalities/inequalities and remove them if
	// they are true, return empty solution otherwise

	if (!sys.first) return solve(sys.second);
	auto phi = lgrs(sys.first.value());
	inequality_system<BAs...> inequalities;
	// for each inequality g_i we apply the transformation given by lgrs solution
	// of the equality
	for (auto& g_i: sys.second) inequalities.insert(replace(g_i, phi));
	// and finally solve the given system  of inequalities
	return solve(inequalities);
}

} // idni::tau namespace
#endif // __SOLVER_H__