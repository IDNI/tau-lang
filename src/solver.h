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
#include "nso_rr.h"
#include "normal_forms.h"
#include "splitter.h"

#ifdef DEBUG
#include "debug_helpers.h"
#endif // DEBUG

// In what follows we use the algorithms and notations of TABA book (cf.
// Section 3.2).Chek (https://github.com/IDNI/tau-lang/blob/main/docs/taba.pdf)
// for the details.

using namespace idni::rewriter;
using namespace idni::tau;

namespace idni::tau {

template<typename...BAs>
using typed_nso = nso<BAs...>;

template<typename...BAs>
using var = nso<BAs...>;

template<typename...BAs>
using minterm = nso<BAs...>;

template<typename...BAs>
using equality = nso<BAs...>;

template<typename...BAs>
using inequality = nso<BAs...>;

template<typename...BAs>
using equation_system = std::pair<std::optional<equality<BAs...>>, std::set<inequality<BAs...>>>;

template<typename...BAs>
using inequality_system = std::set<inequality<BAs...>>;

template<typename...BAs>
using minterm_system = std::set<nso<BAs...>>;

template<typename...BAs>
using solution = std::map<var<BAs...>, nso<BAs...>>;

template<typename...BAs>
solution<BAs...> make_removed_vars_solution(const std::vector<var<BAs...>>& originals, const nso<BAs...>& gh) {
	solution<BAs...> solution;
	for (size_t i = 1; i < originals.size(); ++i) solution[originals[i]] = _0<BAs...>;
	#ifdef DEBUG
	std::cout << "originals solution: " << solution << std::endl;
	#endif // DEBUG
	auto remaing = select_top(gh, is_child_non_terminal<tau_parser::variable, BAs...>);
	for (auto& v: remaing) solution.erase(v);
	#ifdef DEBUG
	std::cout << "remaing solution: " << solution << std::endl;
	#endif // DEBUG
	return solution;
}

template<typename...BAs>
solution<BAs...> find_solution(const equality<BAs...>& eq) {
	auto has_no_var = [](const nso<BAs...>& f) {
		return !find_top(f, is_child_non_terminal<tau_parser::variable, BAs...>);
	};
	// We would use the algorithm subyaccent to the following theorem (of Taba Book):
	//
	// Theorem 3.1. For f (x,X) = xg (X) + x′h (X), let Z be a zero of
	// g (Z) h (Z) (which is guaranteed to exist by Boole’s consistency condition).
	// Then both f (h (Z) ,Z) = 0 and f (g′ (Z) ,Z) = 0.
	// find a variable, say x, in the equality
	auto f = eq | tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<nso<BAs...>>;
	#ifdef DEBUG
	std::cout << "f: " << f << std::endl;
	#endif // DEBUG
	if (auto vars = select_top(f, is_child_non_terminal<tau_parser::variable, BAs...>); !vars.empty()) {
		// compute g(X) and h(X) from the equality by substituting x with 0 and 1
		// with x <- h(Z)
		auto g = replace_with(vars[0], _1<BAs...>, f) | bf_reduce_canonical<BAs...>();
		auto h = replace_with(vars[0], _0<BAs...>, f) | bf_reduce_canonical<BAs...>();
		#ifdef DEBUG
		std::cout << "g: " << g << std::endl;
		std::cout << "h: " << h << std::endl;
		#endif // DEBUG
		auto gh = (g & h) | repeat_all<step<BAs...>, BAs...>(
			elim_bf_constant_01<BAs...>
			| simplify_bf<BAs...>);
;
		auto solution = make_removed_vars_solution(vars, gh);
		#ifdef DEBUG
		std::cout << "gh: " << gh << std::endl;
		std::cout << "solution: " << solution << std::endl;
		#endif // DEBUG
		if (has_no_var(gh)) {
			if (gh != _0<BAs...>) return {};
			else {
				solution[vars[0]] = h != _0<BAs...> ? h : ~g | bf_reduce_canonical<BAs...>();
				#ifdef DEBUG
				std::cout << "solution: " << solution << std::endl;
				#endif // DEBUG
				return solution;
			}
		}
		if (auto restricted = find_solution(build_wff_eq(gh)); !restricted.empty()) {
			#ifdef DEBUG
			std::cout << "restricted: " << restricted << std::endl;
			#endif // DEBUG
			solution.insert(restricted.begin(), restricted.end());
			if (auto nn = replace(h, restricted) | bf_reduce_canonical<BAs...>(); nn != _0<BAs...>)
				solution[vars[0]] = nn;
			else solution[vars[0]] = ~g | bf_reduce_canonical<BAs...>();
			#ifdef DEBUG
			std::cout << "solution: " << solution << std::endl;
			#endif // DEBUG
			return solution;
		}
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
			// if the current choices correspond to a proper minterm, we update the current
			// minterm, otherwise we compute the next valid choice
			if (auto minterm = make_current_minterm(); minterm != _0<BAs...>) current = minterm;
			else make_next_choice();
		// otherwise, i.e. no vars, we return an empty iterator as we have no vars.
		} else exhausted = true;
	}

	minterm_iterator();

	minterm_iterator<BAs...>& operator++() {
		if (exhausted) return *this;
		make_next_choice();
		return *this;
	}

	minterm_iterator<BAs...> operator++(int) {
		return ++*this;
	}

	bool operator==(const minterm_iterator<BAs...>& that) const = default;
	bool operator!=(const minterm_iterator<BAs...>& that) const = default;

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

		bool operator==(const choice&) const = default;
		bool operator!=(const choice&) const = default;
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

	void make_next_choice() {
		if (exhausted) return;
		// update the choices from right to left
		size_t last_changed_value = choices.size();
		while (last_changed_value > 0)  if (choices[--last_changed_value].value ^= true) break;
		// if all choices are exhausted, we are done
		if (last_changed_value == 0 && choices[0].value == false) { exhausted = true; return; }
		// we update the choices from the last changed value till the end
		update_choices_from(last_changed_value);
		// if the current minterm is valid, we update current field, otherwise...
		if (auto minterm = make_current_minterm(); minterm != _0<BAs...>) { current = minterm; return; }
		// ... we try again with the next choice
		make_next_choice();
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

	explicit minterm_range(const nso<BAs...>& f): f (f) {}

	bool empty() {
		return false;
	}

	minterm_iterator<BAs...> begin() {
		minterm_iterator<BAs...> begin(f);
		return begin;
	}

	minterm_iterator<BAs...>::sentinel end() const {
		return minterm_iterator<BAs...>::end;
	}

	bool operator==(const minterm_range<BAs...>&) const = default;
	bool operator!=(const minterm_range<BAs...>&) const = default;

private:
	const nso<BAs...> f;
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
		if (sys.empty()) { exhausted = true; return; }
		// for each inequality in the system, we create a minterm range
		for (auto& neq: sys) {
			auto f = neq | tau_parser::bf_neq | tau_parser::bf | optional_value_extractor<nso<BAs...>>;
			ranges.push_back(minterm_range(f));
		}
		// we initialize the minterm iterators
		for (auto& range: ranges) {
			minterm_iterators.push_back(range.begin());
			if (minterm_iterators.back() == range.end()) {
				exhausted = true;
				break;
			}
		}
		make_current_minterm_system();
	}

	minterm_inequality_system_iterator();

	minterm_inequality_system_iterator<BAs...> &operator++() {
		if (exhausted) return *this;
		make_next_choice();
		return *this;
	}

	minterm_inequality_system_iterator<BAs...> operator++(int) {
		return ++*this;
	}

	bool operator==(const minterm_inequality_system_iterator<BAs...>& that) const = default;
	bool operator!=(const minterm_inequality_system_iterator<BAs...>& that) const = default;

	bool operator==(const sentinel&) const {
		return exhausted;
	}

	bool operator!=(const sentinel&) const {
		return !exhausted;
	}

	const minterm_system<BAs...>& operator*() const {
		return current;
	}

private:
	std::vector<minterm_range<BAs...>> ranges;
	std::vector<minterm_iterator<BAs...>> minterm_iterators;
	minterm_system<BAs...> current;
	bool exhausted = false;

	minterm_system<BAs...> make_current_minterm_system() {
		minterm_system<BAs...> minterms;
		for (auto& it: minterm_iterators) minterms.insert(build_wff_neq(*it));
		return minterms;
	}

	void make_next_choice() {
		if (exhausted) return;
		size_t last_changed_value = minterm_iterators.size();
		while (last_changed_value > 0) {
			--last_changed_value;
			if (++minterm_iterators[last_changed_value] == ranges[last_changed_value].end())
				minterm_iterators[last_changed_value] = ranges[last_changed_value].begin();
			else break;
		}
		if (last_changed_value == 0 && minterm_iterators[0] == ranges[0].begin()) {
			exhausted = true; return;
		}
		current = make_current_minterm_system();
	}
};

template<typename...BAs>
class minterm_inequality_system_range {
public:

	explicit minterm_inequality_system_range(const inequality_system<BAs...>& sys): sys(sys) {};

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
nso<BAs...> get_constant(const minterm<BAs...>& m) {
	auto cte = find_top(m, is_child_non_terminal<tau_parser::bf_constant, BAs...>);
	return cte ? cte.value() : _1<BAs...>;
}

template<typename...BAs>
nso<BAs...> get_minterm(const minterm<BAs...>& m) {
	auto is_literal = [](const nso<BAs...> n) {
		return is_non_terminal<tau_parser::bf>(n)
			&& !is_child_non_terminal<tau_parser::bf_constant>(n)
			&& !is_child_non_terminal<tau_parser::bf_and>(n);
	};
	auto literals = select_top(m, is_literal);
	#ifdef DEBUG
	std::cout << "get_minterm literals: "; for (auto& l: literals) std::cout << l << " "; std::cout << std::endl;
	#endif // DEBUG
	set<nso<BAs...>> all_literals(literals.begin(), literals.end());
	return build_bf_and(all_literals);
}

template<typename...BAs>
bool has_solution(const minterm_system<BAs...>& sys) {
	auto bs = _1<BAs...>;
	for (auto& m: sys) bs = bs & get_constant(m);

	#ifdef DEBUG
	std::cout << "has_solution: " << bs << std::endl;
	#endif // DEBUG

	return bs != _0<BAs...>;
}

template<typename...BAs>
std::set<nso<BAs...>> get_exponent(const nso<BAs...>& n) {
	auto is_bf_literal = [](const auto& n) -> bool {
		return (n | tau_parser::variable).has_value()
			|| (n | tau_parser::bf_neg | tau_parser::bf | tau_parser::variable).has_value();
	};
	auto all_vs = select_top(n, is_bf_literal);
	return std::set<nso<BAs...>>(all_vs.begin(), all_vs.end());
}

template<typename B, typename...BAs>
minterm_system<BAs...> add_minterm_to_disjoint(const minterm_system<BAs...>& disjoint, const minterm<BAs...>& m) {
	minterm_system<BAs...> new_disjoint = disjoint;
	auto new_m = m;
	#ifdef DEBUG
	std::cout << "add_minterm_to_disjoint m: " << m << std::endl;
	#endif // DEBUG
	for (auto& d: disjoint) {
		#ifdef DEBUG
		std::cout << "add_minterm_to_disjoint d: " << d << std::endl;
		#endif // DEBUG
		if (get_exponent(d) == get_exponent(m)) {
			new_disjoint.insert(d);
			continue;
		}
		if (auto d_cte = get_constant(d), new_m_cte = get_constant(new_m); (d_cte & new_m_cte) != false) {
			if ((d_cte & ~new_m_cte) != false) new_disjoint.insert(~new_m_cte & d);
			else if ((new_m_cte & ~d_cte) != false) {
				new_disjoint.insert(d);
				new_m = ~d_cte & new_m;
			} else {
				auto s = d_cte == _1<BAs...> ? tau_bad_splitter<BAs...>()
					: splitter(d_cte | tau_parser::bf_constant | optional_value_extractor<nso<BAs...>>);
				new_disjoint.insert(s & d);
				new_m = ~s & new_m;
			}
		} else new_disjoint.insert(d);
	}
	new_disjoint.insert(new_m);
	return new_disjoint;
}

template<typename B, typename...BAs>
minterm_system<BAs...> make_minterm_system_disjoint(const minterm_system<BAs...>& sys) {
	minterm_system<BAs...> disjoints;
	for (auto it = sys.begin(); it != sys.end(); ++it) disjoints = add_minterm_to_disjoint<B, BAs...>(disjoints, *it);

	#ifdef DEBUG
	std::cout << "make_minterm_system_disjoint: "; for (auto& m: disjoints) std::cout << m << " "; std::cout << std::endl;
	#endif // DEBUG

	return disjoints;
}

template<typename B, typename...BAs>
solution<BAs...> solve_minterm_system(const minterm_system<BAs...>& sys) {
	// To solve the minterm system, we use the Corollary 3.2 (of Taba Book),
	// the splitters to compute proper c_i's, and finally, use find_solution
	// to compute one solution of the resulting system of equalities (squeezed).
	if (!has_solution(sys)) return {};
	#ifdef DEBUG
	std::cout << "solve_minterm_system sys: "; for (auto& m: sys) std::cout << m << " "; std::cout << std::endl;
	#endif // DEBUG

	equality<BAs...> eq = _0<BAs...>;
	for (auto& neq: make_minterm_system_disjoint<B, BAs...>(sys)) {
		# ifdef DEBUG
		std::cout << "solve_minterm_system neq: " << neq << std::endl;
		#endif // DEBUG
		auto nf = neq | tau_parser::bf_neq | tau_parser::bf | bf_reduce_canonical<BAs...>()
			| optional_value_extractor<nso<BAs...>>;
		auto cte = get_constant(nf);
		auto minterm = get_minterm(nf);
		#ifdef DEBUG
		std::cout << "solve_minterm_system cte: " << cte << std::endl;
		std::cout << "solve_minterm_system minterm: " << minterm << std::endl;
		#endif // DEBUG
		eq = eq | (cte & ~minterm);
	}
	eq = build_wff_eq(eq);

	#ifdef DEBUG
	std::cout << "solve_minterm_system eq: " << eq << std::endl;
	#endif // DEBUG

	return find_solution(eq);
}

template<typename B, typename...BAs>
solution<BAs...> solve_inequality_system(const inequality_system<BAs...>& sys) {
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
	for (auto& ms: minterm_inequality_system_range(sys)) {
		auto solution = solve_minterm_system<B, BAs...>(ms);
		if (!solution.empty()) return solution;
	}
	return {};
}

template<typename B, typename...BAs>
solution<BAs...> solve_system(const equation_system<BAs...>& sys) {
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

	if (!sys.first) return solve_inequality_system<B, BAs...>(sys.second);
	auto phi = lgrs(sys.first.value());
	inequality_system<BAs...> inequalities;
	// for each inequality g_i we apply the transformation given by lgrs solution
	// of the equality
	for (auto& g_i: sys.second) inequalities.insert(replace(g_i, phi));
	// and finally solve the given system  of inequalities
	return solve_inequality_system<B, BAs...>(inequalities);
}

} // idni::tau namespace
#endif // __SOLVER_H__