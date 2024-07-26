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
#include "satisfiability.h"
#include "normal_forms.h"

// in what follows we use the algorithms and notations of TABA book (cf.
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
		auto g = replace_with(f, x, _1<BAs...>);
		auto h = replace_with(f, x, _0<BAs...>);
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

// TODO (HIGH) implement members
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

	minterm_iterator(const nso<BAs...>& f);
	minterm_iterator();
	minterm_iterator<BAs...> &operator++();
	minterm_iterator<BAs...> operator++(int);
	bool operator==(const minterm_iterator<BAs...>&) const;
	bool operator!=(const minterm_iterator<BAs...>& that) const;
	bool operator==(const sentinel& that) const;
	bool operator!=(const sentinel& that) const;
	const minterm<BAs...>& operator*() const;

private:
	using choice = std::pair<nso<BAs...>, typename std::vector<nso<BAs...>>::iterator>;
	static const std::vector<nso<BAs...>> bools {_0<BAs...>, _1<BAs...>};
	std::stack<choice> choices;
	nso<BAs...> f;
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

// TODO (HIGH) implement members
template<typename...BAs>
class minterm_system_iterator {
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

	minterm_system_iterator(const inequality_system<BAs...>& sys);
	minterm_system_iterator();
	minterm_system_iterator<BAs...> &operator++();
	minterm_system_iterator<BAs...> operator++(int);
	bool operator==(const minterm_system_iterator<BAs...>&) const;
	bool operator!=(const minterm_system_iterator<BAs...>& that) const;
	bool operator==(const sentinel &that) const;
	bool operator!=(const sentinel &that) const;
	const minterm_system<BAs...>& operator*() const;

private:
	// IDEA maybe is better to keep a vector of minterm ranges instead of
	// the original inequality system
	inequality_system<BAs...>& sys;
	std::vector<minterm_iterator<BAs...>> minter_iterators;
};

template<typename...BAs>
class minterm_system_range {
public:

	explicit minterm_system_range(const inequality_system<BAs...>& sys);

	bool empty() {
		return sys.size() == 0;
	}

	minterm_system_iterator<BAs...> begin() {
		minterm_system_iterator<BAs...> begin(sys);
		return begin;
	}

	minterm_system_iterator<BAs...>::sentinel end() {
		return minterm_system_iterator<BAs...>::end;
	}

private:
	inequality_system<BAs...> sys;
};

template<typename...BAs>
bool has_solution(const minterm_system<BAs...>& sys) {
	// TODO (HIGH) implement this function
	return false;
}

template<typename...BAs>
minterm_system<BAs...> make_minterm_system_disjoint(const minterm_system<BAs...>& sys) {
	// TODO (HIGH) implement this function
	return sys;
}

template<typename...BAs>
solution<BAs...> solve(const minterm_system<BAs...>& sys) {
	// To solve the minterm system, we use the Corollary 3.2 (of Taba Book),
	// the splitters to compute proper c_i's, and finally, use find_solution
	// to compute one solution of the resulting system of equalities (squeezed).
	if (!has_solution(sys)) return {};
		// TODO (HIGH) implement this block
	return {};
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
		auto solution = solve(ms);
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
	//
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