// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __SOLVER_H__
#define __SOLVER_H__

#include <stack>

#include "tau_ba.h"
#include "normal_forms.h"
#include "bdd_handle.h"
#include "splitter.h"

#ifdef DEBUG
#include "debug_helpers.h"
#endif // DEBUG

// In what follows we use the algorithms and notations of TABA book (cf.
// Section 3.2).Chek (https://github.com/IDNI/tau-lang/blob/main/docs/taba.pdf)
// for the details.

namespace idni::tau_lang {

template<typename...BAs>
using typed_nso = tau<BAs...>;
template<typename...BAs>
using var = tau<BAs...>;
template<typename...BAs>
using minterm = tau<BAs...>;

template<typename...BAs>
using equality = tau<BAs...>;

template<typename...BAs>
using inequality = tau<BAs...>;

template<typename...BAs>
using equation = tau<BAs...>;

template<typename...BAs>
using equations = std::set<tau<BAs...>>;

template<typename...BAs>
using equation_system = std::pair<std::optional<equality<BAs...>>, std::set<inequality<BAs...>>>;

template<typename...BAs>
using inequality_system = std::set<inequality<BAs...>>;

template<typename...BAs>
using minterm_system = std::set<inequality<BAs...>>;

template<typename...BAs>
using solution = std::map<var<BAs...>, tau<BAs...>>;

using type = std::string;

static const type default_type = "";

template<typename...BAs>
solution<BAs...> make_removed_vars_solution(const std::vector<var<BAs...>>& originals, const tau<BAs...>& gh) {
	solution<BAs...> solution;
	for (size_t i = 1; i < originals.size(); ++i) solution[originals[i]] = _0<BAs...>;
	auto remaing = select_top(gh, is_child_non_terminal<tau_parser::variable, BAs...>);
	for (auto& v: remaing) solution.erase(v);
	return solution;
}

template<typename...BAs>
std::optional<solution<BAs...>> find_solution(const equality<BAs...>& eq) {
	// We would use the algorithm subyaccent to the following theorem (of Taba Book):
	//
	// Theorem 3.1. For f (x,X) = xg (X) + x′h (X), let Z be a zero of
	// g (Z) h (Z) (which is guaranteed to exist by Boole’s consistency condition).
	// Then both f (h (Z) ,Z) = 0 and f (g′ (Z) ,Z) = 0.
	// find a variable, say x, in the equality
	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "find_solution/eq: " << eq;
	#endif // DEBUG

	auto has_no_var = [](const tau<BAs...>& f) {
		return !find_top(f, is_child_non_terminal<tau_parser::variable, BAs...>);
	};

	if (!(eq | tau_parser::bf_eq).has_value()) {
		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "find_solution/solution[no_eq]: {}";
		#endif // DEBUG

		return {};
	}

	auto f = eq | tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<tau<BAs...>>;

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "find_solution/f: " << f;
	#endif // DEBUG

	if (auto vars = select_top(f, is_child_non_terminal<tau_parser::variable, BAs...>); !vars.empty()) {
		// compute g(X) and h(X) from the equality by substituting x with 0 and 1
		// with x <- h(Z)
		auto g = replace_with(vars[0], _1<BAs...>, f) | bf_reduce_canonical<BAs...>();
		auto h = replace_with(vars[0], _0<BAs...>, f) | bf_reduce_canonical<BAs...>();
		auto gh = (g & h) | bf_reduce_canonical<BAs...>();
		auto solution = make_removed_vars_solution(vars, gh);

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "find_solution/var[0]: " << vars[0] << "\n"
			<< "find_solution/g: " << g << "\n"
			<< "find_solution/h: " << h << "\n"
			<< "find_solution/gh: " << gh << "\n"
			<< "find_solution/solution[removed_vars]: ";
		for (const auto& [k, v]: solution)
			BOOST_LOG_TRIVIAL(trace)
				<< "\t" << k << " := " << v << " ";
		#endif // DEBUG

		if (has_no_var(gh)) {
			if (gh != _0<BAs...>) {
				#ifdef DEBUG
				BOOST_LOG_TRIVIAL(trace)
					<< "find_solution/solution[gh_no_var,gh_!=_0]: {}";
				#endif // DEBUG

				return {};
			}
			else {
				auto changes = solution;
				solution[vars[0]] = h != _0<BAs...> ? replace(h, changes) : replace(~g, changes) | bf_reduce_canonical<BAs...>();

				#ifdef DEBUG
				BOOST_LOG_TRIVIAL(trace)
					<< "find_solution/solution[gh_no_var,gh=0]: ";
				for (const auto& [k, v]: solution)
					BOOST_LOG_TRIVIAL(trace)
						<< "\t" << k << " := " << v << " ";
				#endif // DEBUG

				return solution;
			}
		}
		if (auto restricted = find_solution(build_wff_eq(gh)); restricted.has_value()) {
			solution.insert(restricted.value().begin(), restricted.value().end());
			auto restricted_copy = restricted;
			if (auto nn = replace(h, restricted.value()) | bf_reduce_canonical<BAs...>(); nn != _0<BAs...>)
				solution[vars[0]] = nn;
			else solution[vars[0]] = replace(~g, restricted_copy.value()) | bf_reduce_canonical<BAs...>();

			#ifdef DEBUG
			BOOST_LOG_TRIVIAL(trace)
				<< "find_solution/solution[general]: ";
			for (const auto& [k, v]: solution)
				BOOST_LOG_TRIVIAL(trace)
					<< "\t" << k << " := " << v << " ";
			#endif // DEBUG

			return solution;
		}
	}

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "find_solution/solution[no_var]: {}";
	#endif // DEBUG

	return {};
}

template<typename...BAs>
std::optional<solution<BAs...>> lgrs(const equality<BAs...>& equality) {
	// We would use Lowenheim’s General Reproductive Solution (LGRS) as given
	// in the following theorem (of Taba Book):
	//
	// Theorem 1.8. Let f : Bn → B be a BF, and assume f (Z) = 0
	// for some Z ∈ Bn. Then the set {X ∈ Bn| f(X) = 0} equals precisely
	// the image of ϕ : Bn → Bn defined by ϕ (X) = Zf (X) + Xf′ (X). Decyphering
	// the abuse of notation, this reads ϕ_i (X) = z_i f (X)+x_i f′ (X).

	auto s = find_solution(equality);
	if (!s.has_value()) {

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "lgrs/solution: {}";
		#endif // DEBUG

		return {};
	}
	auto f = equality
		| tau_parser::bf_eq
		| tau_parser::bf
		| optional_value_extractor<tau<BAs...>>;
	solution<BAs...> phi;
	for (auto& [x_i, z_i] : s.value())
		phi[x_i] = ((z_i & f) + (x_i & ~f)) | bf_reduce_canonical<BAs...>();

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "lgrs/equality: " << equality << "\n"
		<< "lgrs/solution: ";
	for (const auto& [k, v] : phi)
		BOOST_LOG_TRIVIAL(trace)
			<< "\t" << k << " := " << v << " ";
	auto copy = phi;
	auto check = snf_wff(replace(equality, copy));
	BOOST_LOG_TRIVIAL(trace)
		<< "lgrs/check: " << check << "\n";
	#endif // DEBUG

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

	minterm_iterator(const tau<BAs...>& f) {
		if (auto vars = select_top(f, is_child_non_terminal<tau_parser::variable, BAs...>); !vars.empty()) {
			// we start with the full bf...
			auto partial_bf = f;
			// ... and the first variable (for computing the first partial minterm)
			auto partial_minterm = _1<BAs...>;
			for (auto& v : vars) {
				// we add the current choice to the list of choices...
				partial_minterm = partial_minterm & ~v;
				choices.emplace_back(v, false, partial_bf, partial_minterm);
				partial_bf = replace_with(v, _0<BAs...>, partial_bf);
				// ... and compute new values for the next one
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
		tau<BAs...> partial_bf;
		tau<BAs...> partial_minterm;

		bool operator==(const choice&) const = default;
		bool operator!=(const choice&) const = default;
	};

	std::vector<choice> choices;
	minterm<BAs...> current;
	bool exhausted = false;

	tau<BAs...> make_current_minterm() {
		auto cte =  choices.back().value
			? replace_with(choices.back().var, _1<BAs...>, choices.back().partial_bf)
			: replace_with(choices.back().var, _0<BAs...>, choices.back().partial_bf);
		auto current = (cte & choices.back().partial_minterm);

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "make_current_minterm/current: " << current;
		#endif // DEBUG

		return current;
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
		if (start == 0) {
			choices[0].partial_minterm = choices[0].value ? choices[0].var : ~choices[0].var;
			++start;
		}
		for (size_t i = start; i < choices.size(); ++i) {
			choices[i].partial_minterm = (choices[i].value ? choices[i].var : ~choices[i].var)
				& choices[i - 1].partial_minterm;
			choices[i].partial_bf = choices[i - 1].value
				? replace_with(choices[i - 1].var, _1<BAs...>, choices[i - 1].partial_bf)
				: replace_with(choices[i - 1].var, _0<BAs...>, choices[i - 1].partial_bf);
			// if current partial bf is 0, we can skip the rest of the choices
			// as the corresponding minterms will be 0.
			if (choices[i].partial_bf == _0<BAs...>) {
				for (size_t j = i + 1; j < choices.size(); ++j) {
					choices[j].partial_bf = _0<BAs...>;
					choices[j].value = true;
				}
				return;
			}
		}
	}
};

template<typename...BAs>
class minterm_range {
public:

	explicit minterm_range(const tau<BAs...>& f): f (f) {}

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
	const tau<BAs...> f;
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
			auto f = neq | tau_parser::bf_neq | tau_parser::bf | optional_value_extractor<tau<BAs...>>;
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
		current = make_current_minterm_system();
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

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "make_current_minterm_system/minterms: ";
		for (const auto& minterm: minterms)
			BOOST_LOG_TRIVIAL(trace)
				<< "\t" << minterm;
		#endif // DEBUG

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
tau<BAs...> get_constant(const minterm<BAs...>& m) {
	auto cte = find_top(m, is_child_non_terminal<tau_parser::bf_constant, BAs...>);
	return cte ? cte.value() : _1<BAs...>;
}

template<typename...BAs>
tau<BAs...> get_minterm(const minterm<BAs...>& m) {
	auto is_literal = [](const tau<BAs...> n) {
		return is_non_terminal<tau_parser::bf>(n)
			&& !is_child_non_terminal<tau_parser::bf_constant>(n)
			&& !is_child_non_terminal<tau_parser::bf_and>(n);
	};
	auto literals = select_top(m, is_literal);
	std::set<tau<BAs...>> all_literals(literals.begin(), literals.end());
	return build_bf_and<BAs...>(all_literals);
}

template<typename...BAs>
std::set<tau<BAs...>> get_exponent(const tau<BAs...>& n) {
	auto is_bf_literal = [](const auto& n) -> bool {
		return (n | tau_parser::variable).has_value()
			|| (n | tau_parser::bf_neg | tau_parser::bf | tau_parser::variable).has_value();
	};
	auto all_vs = select_top(n, is_bf_literal);
	return std::set<tau<BAs...>>(all_vs.begin(), all_vs.end());
}

template<typename...BAs>
minterm_system<BAs...> add_minterm_to_disjoint(const minterm_system<BAs...>& disjoint,
		const minterm<BAs...>& m, const tau<BAs...>& splitter_one) {
	minterm_system<BAs...> new_disjoint;
	auto new_m = m;
	for (auto& d: disjoint) {
		// case 1
		if (get_exponent(d) == get_exponent(m)) {
			new_disjoint.insert(d);
			continue;
		}
		if (auto d_cte = get_constant(d), new_m_cte = get_constant(new_m);
				(d_cte & new_m_cte) != false) {
			// case 2
			if ((d_cte & ~new_m_cte) != false)
				new_disjoint.insert(~new_m_cte & d);
			// case 3
			else if ((~d_cte & new_m_cte) != false) {
				new_disjoint.insert(d);
				new_m = ~d_cte & new_m;
			// case 4
			} else {
				auto s = d_cte == _1<BAs...>
					// case 4.1
					? splitter_one
					// case 4.2
					: splitter(d_cte
						| tau_parser::bf_constant
						| optional_value_extractor<tau<BAs...>>);
				new_disjoint.insert(s & d);
				new_m = ~s & new_m;
			}
		// case 5
		} else new_disjoint.insert(d);
	}
	new_disjoint.insert(new_m);
	return new_disjoint;
}

template<typename...BAs>
minterm_system<BAs...> make_minterm_system_disjoint(const minterm_system<BAs...>& sys,
		const tau<BAs...>& splitter_one) {
	minterm_system<BAs...> disjoints;
	for (auto it = sys.begin(); it != sys.end(); ++it)
		disjoints = add_minterm_to_disjoint<BAs...>(disjoints, *it, splitter_one);
	return disjoints;
}

template<typename...BAs>
std::optional<solution<BAs...>> solve_minterm_system(const minterm_system<BAs...>& system,
		const tau<BAs...>& splitter_one) {
	// To solve the minterm system, we use the Corollary 3.2 (of Taba Book),
	// the splitters to compute proper c_i's, and finally, use find_solution
	// to compute one solution of the resulting system of equalities (squeezed).

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "solve_minterm_system/system: ";
	for (const auto& minterm : system)
		BOOST_LOG_TRIVIAL(trace)
			<< "\t" << minterm;
	#endif // DEBUG

	// We know the system has a solution as we only iterate over non-negative
	// minterms (which trivially satisfy the condition of Theorem 3.3)
	equality<BAs...> eq = _0<BAs...>;
	for (auto& neq: make_minterm_system_disjoint<BAs...>(system, splitter_one)) {

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "solve_minterm_system/neq: " << neq;
		#endif // DEBUG

		auto nf = neq
			| tau_parser::bf_neq
			| tau_parser::bf
			| optional_value_extractor<tau<BAs...>>
			| bf_reduce_canonical<BAs...>();
		auto cte = get_constant(nf);
		auto minterm = get_minterm(nf);
		eq = eq | (cte & ~minterm);
	}
	eq = build_wff_eq(eq);
	return find_solution(eq);
}

template<typename...BAs>
std::optional<solution<BAs...>> solve_inequality_system(const inequality_system<BAs...>& system,
		const tau<BAs...>& splitter_one) {
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

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "solve_inequality_system/system: ";
	for (const auto& inequality : system)
		BOOST_LOG_TRIVIAL(trace)
			<< "\t" << inequality;
	#endif // DEBUG

	//for (auto& ms: minterm_inequality_system_range<BAs...>(system)) {
	for (auto it = minterm_inequality_system_iterator<BAs...>(system); it != minterm_inequality_system_iterator<BAs...>::end; ++it) {

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "solve_inequality_system/minterm system: ";
		for (const auto& minterm : *it)
			BOOST_LOG_TRIVIAL(trace)
				<< "\t" << minterm;
		#endif // DEBUG

		auto solution = solve_minterm_system<BAs...>(*it, splitter_one);
		if (solution.has_value()) return solution;
	}

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "solve_inequality_system/solution: {}";
	#endif // DEBUG

	return {};
}

template<typename...BAs>
std::optional<solution<BAs...>> solve_system(const equation_system<BAs...>& system,
		const tau<BAs...>& splitter_one) {
	// As in the Taba book, we consider
	// 		f (X) = 0
	//		{g_i (X) ̸= 0}i∈I
	// and let ϕ be the LGRS of f (wrt some arbitrarily chosen single zero of
	// f), and assume that a solution to the whole system, exists. Set h_i (X) =
	// g_i (ϕ (X)) and suppose T satisfies {h_i (T) ̸= 0}i∈I , then f (ϕ (T)) = 0 because
	// the LGRS is reproductive (cf. remark 1.2). So to solve the
	// original system we only need to solve {h_i (T) ̸= 0}i∈I and the solution
	// to the original system is then ϕ (T).

	// TODO (HIGH) check for constant equalities/inequalities and remove them if
	// they are true, return empty solution otherwise

	#ifdef DEBUG
	if (system.first.has_value())
		BOOST_LOG_TRIVIAL(trace)
			<< "solve_system/eq: " << system.first.value();
	if (!system.second.empty()) {
		BOOST_LOG_TRIVIAL(trace)
			<< "solve_system/inequalities: ";
		std::cout << "solve_system/inequalities: ";
		for (const auto& inequality : system.second)
			BOOST_LOG_TRIVIAL(trace)
				<< "\t" << inequality;
	}
	#endif // DEBUG

	if (!system.first) return solve_inequality_system<BAs...>(system.second, splitter_one);
	if (system.second.empty()) return find_solution(system.first.value());
	auto phi = lgrs(system.first.value());
	if (!phi.has_value()) return {};
	inequality_system<BAs...> inequalities;
	// for each inequality g_i we apply the transformation given by lgrs solution
	// of the equality
	for (auto& g_i: system.second) {
		auto nphi = phi.value(), ng_i = replace(g_i, nphi);
		if (ng_i == _F<BAs...>) {

			#ifdef DEBUG
			BOOST_LOG_TRIVIAL(trace)
				<< "solve_system/inequality_solution: {}";
			#endif // DEBUG

			return {};
		}
		else if (ng_i == _T<BAs...>) continue;
		inequalities.insert(ng_i);
	}
	// solve the given system  of inequalities
	auto inequality_solution = solve_inequality_system<BAs...>(inequalities, splitter_one);
	if (!inequality_solution.has_value()) {

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "solve_system/inequality_solution: {}";
		#endif // DEBUG

		return {};
	}
	// and finally, apply the solution to lgrs solution to get the final one (ϕ (T)).
	// Solutions coming from inequality_solution for variables appearing also
	// in the equality part will be replaced in the next step
	solution<BAs...> solution = inequality_solution.value();

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "solve_system/inequality_solution: ";
	#endif // DEBUG

	// Now we need to add solutions for variables in the lgrs
	for (auto& [var, func]: phi.value()) {
		auto copy = inequality_solution.value();
		auto func_with_neq_assgm = replace(func, copy);
		// Now assign the remaining variables to 0 and compute
		// resulting value for var
		auto free_vars = get_free_vars_from_nso(func_with_neq_assgm);
		if (!free_vars.empty()) {
			std::map<tau<BAs...>, tau<BAs...>> free_var_assgm;
			for (const auto& free_var : free_vars)
				free_var_assgm.emplace(free_var, _0_trimmed<BAs...>);
			solution[var] = replace(func_with_neq_assgm, free_var_assgm) |
					bf_reduce_canonical<BAs...>();
		} else solution[var] = func_with_neq_assgm | bf_reduce_canonical<BAs...>();
	}
	return solution;
}

template<typename...BAs>
std::optional<solution<BAs...>> solve(const equations<BAs...>& eqs, const tau<BAs...>& splitter_one) {
	// split among equalities and inequalities
	equation_system<BAs...> system;
	for (const auto& eq: eqs) {
		if (is_child_non_terminal<tau_parser::bf_eq, BAs...>(eq)) {
			if (!system.first.has_value())
				system.first = std::optional<equality<BAs...>>(eq);
			else {
				// squeeze the equalities
				auto l = system.first.value()
					| tau_parser::bf_eq
					| tau_parser::bf
					| optional_value_extractor<tau<BAs...>>;
				auto r = eq
					| tau_parser::bf_eq
					| tau_parser::bf
					| optional_value_extractor<tau<BAs...>>;
				system.first = build_wff_eq(l | r);
			}
		}
		else system.second.insert(eq);
	}
	return solve_system<BAs...>(system, splitter_one);
}

// entry point for the solver
template<typename...BAs>
std::optional<solution<BAs...>> solve(const tau<BAs...>& form,
		const std::string& type = "") {
	if (form == _T<BAs...>) return { solution<BAs...>() };
	auto splitter_one = nso_factory<BAs...>::instance().splitter_one(type);

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "solve/form: " << form;
	#endif // DEBUG

	auto dnf = form | bf_reduce_canonical<BAs...>();
	for (auto& clause: get_leaves(form, tau_parser::wff_or, tau_parser::wff)) {
		auto is_equation = [](const tau<BAs...>& n) {
			return is_child_non_terminal<tau_parser::bf_eq, BAs...>(n)
			|| is_child_non_terminal<tau_parser::bf_neq, BAs...>(n);
		};
		auto eqs = select_top(clause, is_equation);
		if (eqs.empty()) continue;
		auto solution = solve<BAs...>(
			std::set<tau<BAs...>>(eqs.begin(), eqs.end()), splitter_one);
		if (solution.has_value()) return solution;
	}
	return {};
}

} // idni::tau_lang namespace
#endif // __SOLVER_H__