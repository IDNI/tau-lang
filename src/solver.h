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
using var = nso<BAs...>;

template<typename...BAs>
using equality = nso<BAs...>;

template<typename...BAs>
using inequality = nso<BAs...>;

template<typename...BAs>
using system = std::pair<equality<BAs...>, std::set<inequality<BAs...>>>;

template<typename...BAs>
using inequality_system = std::set<inequality<BAs...>>;

template<typename...BAs>
using minterm_system = std::set<nso<BAs...>>;

template<typename...BAs>
using solution = std::map<var<BAs...>, nso<BAs...>>;

template<typename...BAs>
solution<BAs...> find_solution(const equality<BAs...>& n) {
	// We would use the algorithm subyaccent to the following theorem (of Taba Book):
	//
	// Theorem 3.1. For f (x,X) = xg (X) + x′h (X), let Z be a zero of
	// g (Z) h (Z) (which is guaranteed to exist by Boole’s consistency condition).
	// Then both f (h (Z) ,Z) = 0 and f (g′ (Z) ,Z) = 0.
	solution<BAs...> single_solution;
	// find a variable, say x, in the equality
	// compute g(X) and h(X) from the equality by substituting x with 0 and 1
	// if g(X) == h(X), return find_solution(h(X))
	// else return Z = find_solution(g(X)h(X)) and return the result combined
	// with x <- h(Z)
	return single_solution;
}

template<typename...BAs>
solution<BAs...> lgrs(const equality<BAs...>& n) {
	// We would use Lowenheim’s General Reproductive Solution (LGRS) as given
	// in the following theorem (of Taba Book):
	//
	// Theorem 1.8. Let f : Bn → B be a BF, and assume f (Z) = 0
	// for some Z ∈ Bn. Then the set {X ∈ Bn| (X) = 0} equals precisely
	// the image of ϕ : Bn → Bn defined by ϕ (X) = Zf (X) + Xf′ (X). Decyphering
	// the abuse of notation, this reads ϕi (X) = zif (X)+xif′ (X).
	auto single_solution = find_solution(n);
	solution<BAs...> phi;
	// create the solution from the found solution using above expressions
	return phi;
}

template<typename...BAs>
std::optional<solution<BAs...>> solve(const minterm_system<BAs...>& n) {
	// To solve the minterm system, we use the Corollary 3.2 (of Taba Book),
	// the splitters to compute proper c_i's, and finally, use find_solution
	// to compute one solution of the resulting system of equalities (squeezed).
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
	solution<BAs...> minterm_solution;
	// for each possible choice of H_i's, we try to solve the minterm system
	// using tthe above solve method.
	return minterm_solution;
}

template<typename...BAs>
solution<BAs...> solve(const system<BAs...>& sys) {
	auto s = lgrs(sys.first);
	inequality_system<BAs...> inequalities;
	// for each inequality g_i in sys.second consider h_i = replace(g_i, s).
	// solve the given system  of inequalities
	auto t = solve(inequalities);
	// we combine the solutions s and t, i.e., we apply the solution t over
	// the values on s.
	solution<BAs...> combined_solution;
	return combined_solution;
}

} // idni::tau namespace
#endif // __SOLVER_H__