// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include <stack>

#include "solver.h"

#ifdef DEBUG
#include "debug_helpers.h"
#endif // DEBUG

// In what follows we use the algorithms and notations of TABA book (cf.
// Section 3.2).Chek (https://github.com/IDNI/tau-lang/blob/main/docs/taba.pdf)
// for the details.

namespace idni::tau_lang {

template<typename...BAs>
solution<BAs...> make_removed_vars_solution(const std::vector<var<BAs...>>& originals, const tau_depreciating<BAs...>& gh) {
	solution<BAs...> solution;
	for (size_t i = 1; i < originals.size(); ++i) solution[originals[i]] = _0<BAs...>;
	// FIXME convert vars to a set
	auto remaing = select_top(gh, is_child_non_terminal<tau_parser::variable, BAs...>);
	for (auto& v: remaing) solution.erase(v);
	return solution;
}

template<typename...BAs>
std::optional<solution<BAs...>> find_solution(const equality<BAs...>& eq, solution<BAs...>& substitution, solver_mode mode) {
	// We would use the algorithm subyaccent to the following theorem (of Taba Book):
	//
	// Theorem 3.1. For f (x,X) = xg (X) + x′h (X), let Z be a zero of
	// g (Z) h (Z) (which is guaranteed to exist by Boole’s consistency condition).
	// Then both f (h (Z) ,Z) = 0 and f (g′ (Z) ,Z) = 0.
	// find a variable, say x, in the equality
	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "solver.tmpl.h:" << __LINE__ << " find_solution/eq: " << eq;
	#endif // DEBUG

	auto has_no_var = [](const tau_depreciating<BAs...>& f) {
		return !find_top(f, is_child_non_terminal<tau_parser::variable, BAs...>);
	};

	if (!(eq | tau_parser::bf_eq).has_value()) {
		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << " find_solution/solution[no_eq]: {}";
		#endif // DEBUG

		return {};
	}

	auto f = eq | tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<tau_depreciating<BAs...>>;

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "solver.tmpl.h:" << __LINE__ << " find_solution/f: " << f;
	#endif // DEBUG

	// FIXME convert vars to a set
	if (auto vars = select_top(f, is_child_non_terminal<tau_parser::variable, BAs...>); !vars.empty()) {
		// compute g(X) and h(X) from the equality by substituting x with 0 and 1
		// with x <- h(Z)
		auto g = replace_with(vars[0], _1<BAs...>, f);
		auto h = replace_with(vars[0], _0<BAs...>, f);
		auto gh = (g & h) | bf_reduce_canonical<BAs...>();

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << " find_solution/var[0]: " << vars[0] << "\n"
			<< "solver.tmpl.h:" << __LINE__ << " find_solution/g: " << g << "\n"
			<< "solver.tmpl.h:" << __LINE__ << " find_solution/h: " << h << "\n"
			<< "solver.tmpl.h:" << __LINE__ << " find_solution/gh: " << gh << "\n";
		#endif // DEBUG

		if (has_no_var(gh)) {
			if (gh != _0<BAs...>) {
				#ifdef DEBUG
				BOOST_LOG_TRIVIAL(trace)
					<< "solver.tmpl.h:" << __LINE__ << " find_solution/solution[gh_no_var,gh_!=_0]: {}";
				#endif // DEBUG

				return {};
			}
			else {
				substitution[vars[0]] = ( ( mode == solver_mode::maximum
						? replace(~g, substitution)
						: replace(h, substitution) )
					| bf_reduce_canonical<BAs...>() );

				#ifdef DEBUG
				BOOST_LOG_TRIVIAL(trace)
					<< "solver.tmpl.h:" << __LINE__ << " find_solution/solution[gh_no_var,gh=0]: ";
				for (const auto& [k, v]: substitution)
					BOOST_LOG_TRIVIAL(trace)
						<< "solver.tmpl.h:" << __LINE__ << "\t" << k << " := " << v << " ";
				#endif // DEBUG

				return substitution;
			}
		}
		if (auto restricted = find_solution(build_wff_eq(gh), substitution, mode); restricted) {
			//solution.insert(restricted.value().begin(), restricted.value().end());
			substitution[vars[0]] = ( ( mode == solver_mode::maximum
					? replace(~g, restricted.value())
					: replace(h, restricted.value()) )
				| bf_reduce_canonical<BAs...>() );

			#ifdef DEBUG
			BOOST_LOG_TRIVIAL(trace)
				<< "solver.tmpl.h:" << __LINE__ << " find_solution/substitution[general]: ";
			for (const auto& [k, v]: substitution)
				BOOST_LOG_TRIVIAL(trace)
					<< "solver.tmpl.h:" << __LINE__ << "\t" << k << " := " << v << " ";
			#endif // DEBUG

			return substitution;
		}
	}

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "solver.tmpl.h:" << __LINE__ << " find_solution/substitution[no_var]: {}";
	#endif // DEBUG

	return {};
}

template<typename...BAs>
std::vector<tau_depreciating<BAs...>> get_variables(const equality<BAs...>& eq) {
	return select_top(eq, is_child_non_terminal<tau_parser::variable, BAs...>);
}

template<typename...BAs>
std::vector<tau_depreciating<BAs...>> get_variables(const equation_system<BAs...>& system) {
	std::vector<tau_depreciating<BAs...>> vars;
	if (system.first.has_value()) {
		auto vs = get_variables(system.first.value());
		vars.insert(vars.end(), vs.begin(), vs.end());
	}
	for (const auto& inequality: system.second) {
		auto vs = get_variables(inequality);
		vars.insert(vars.end(), vs.begin(), vs.end());
	}
	return vars;
}

template<typename...BAs>
std::optional<solution<BAs...>> find_maximal_solution(const equation_system<BAs...>& system) {
	auto vars = get_variables(system);
	if (vars.empty()) return solution<BAs...>();
	auto substitution = solution<BAs...>();
	for (auto& var: vars) substitution[var] = _1<BAs...>;
	return (system.first)
		? find_solution(system.first.value(), substitution, solver_mode::maximum)
		: substitution;
}

template<typename...BAs>
std::optional<solution<BAs...>> find_minimal_solution(const equation_system<BAs...>& system) {
	auto vars = get_variables(system);
	if (vars.empty()) return solution<BAs...>();
	auto substitution = solution<BAs...>();
	for (auto& var: vars) substitution[var] = _0<BAs...>;
	return (system.first)
		? find_solution(system.first.value(), substitution, solver_mode::minimum)
		: substitution;
}

template<typename...BAs>
std::optional<solution<BAs...>> find_solution(const equality<BAs...>& eq) {
	auto vars = get_variables(eq);
	if (vars.empty()) return solution<BAs...>();
	auto substitution = solution<BAs...>();
	for (auto& var: vars) substitution[var] = _1<BAs...>;
	return find_solution(eq, substitution, solver_mode::maximum);
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

	if (equality == _T<BAs...>) {
		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << " lgrs/solution: {}";
		#endif // DEBUG
		return solution<BAs...>();
	}

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "solver.tmpl.h:" << __LINE__ << " lgrs/eq: " << equality << "\n";
	#endif // DEBUG

	auto s = find_solution(equality);
	if (!s.has_value()) {

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << " lgrs/no solution";
		#endif // DEBUG

		return {};
	}
	auto f = equality
		| tau_parser::bf_eq
		| tau_parser::bf
		| optional_value_extractor<tau_depreciating<BAs...>>;
	solution<BAs...> phi;
	for (auto& [x_i, z_i] : s.value())
		phi[x_i] = ((z_i & f) + (x_i & ~f)) | bf_reduce_canonical<BAs...>();

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "solver.tmpl.h:" << __LINE__ << " lgrs/equality: " << equality << "\n"
		<< "solver.tmpl.h:" << __LINE__ << " lgrs/solution: ";
	for (const auto& [k, v] : phi)
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << "\t" << k << " := " << v << " ";
	auto check = snf_wff(replace(equality, phi));
	BOOST_LOG_TRIVIAL(trace)
		<< "solver.tmpl.h:" << __LINE__ << " lgrs/check: " << check << "\n";
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

	minterm_iterator(const tau_depreciating<BAs...>& f) {
		// FIXME convert vars to a set
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
		tau_depreciating<BAs...> partial_bf;
		tau_depreciating<BAs...> partial_minterm;

		bool operator==(const choice&) const = default;
		bool operator!=(const choice&) const = default;
	};

	std::vector<choice> choices;
	minterm<BAs...> current;
	bool exhausted = false;

	tau_depreciating<BAs...> make_current_minterm() {
		auto cte =  choices.back().value
			? replace_with(choices.back().var, _1<BAs...>, choices.back().partial_bf)
			: replace_with(choices.back().var, _0<BAs...>, choices.back().partial_bf);
		auto current = (cte & choices.back().partial_minterm);

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << " make_current_minterm/current: " << current;
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

	explicit minterm_range(const tau_depreciating<BAs...>& f): f (f) {}

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
	const tau_depreciating<BAs...> f;
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
			auto f = neq | tau_parser::bf_neq | tau_parser::bf | optional_value_extractor<tau_depreciating<BAs...>>;
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
			<< "solver.tmpl.h:" << __LINE__ << " make_current_minterm_system/minterms: ";
		for (const auto& minterm: minterms)
			BOOST_LOG_TRIVIAL(trace)
				<< "solver.tmpl.h:" << __LINE__ << "\t" << minterm;
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
tau_depreciating<BAs...> get_constant(const minterm<BAs...>& m) {
	//auto cte = find_top(m, is_child_non_terminal<tau_parser::bf_constant, BAs...>);
	//return cte ? cte.value() : _1<BAs...>;
	auto is_bf_constant = [](const auto& n) -> bool {
		return is_child_non_terminal<tau_parser::bf_constant, BAs...>(n);
	};
	// FIXME convert vars to a set
	auto all_vs = select_top(m, is_bf_constant);
	return build_bf_and<BAs...>(all_vs);
}

template<typename...BAs>
std::set<tau_depreciating<BAs...>> get_exponent(const tau_depreciating<BAs...>& n) {
	auto is_bf_literal = [](const auto& n) -> bool {
		return (n | tau_parser::variable).has_value()
			|| (n | tau_parser::bf_neg | tau_parser::bf | tau_parser::variable).has_value();
	};
	// FIXME convert vars to a set
	auto all_vs = select_top(n, is_bf_literal);
	return std::set<tau_depreciating<BAs...>>(all_vs.begin(), all_vs.end());
}

template<typename...BAs>
tau_depreciating<BAs...> get_minterm(const minterm<BAs...>& m) {
	return build_bf_and<BAs...>(get_exponent(m));
}

template<typename...BAs>
std::optional<minterm_system<BAs...>> add_minterm_to_disjoint(
		const minterm_system<BAs...>& disjoint,	const minterm<BAs...>& m,
		const solver_options<BAs...>& options) {
	minterm_system<BAs...> new_disjoint;
	auto new_m = m;

	for (auto& d: disjoint) {
		auto new_m_cte = get_constant(new_m);
		auto new_m_exp = get_exponent(new_m);

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << " add_minterm_to_disjoint/d: " << d << "\n"
			<< "solver.tmpl.h:" << __LINE__ << " add_minterm_to_disjoint/new_m: " << new_m << "\n"
			<< "solver.tmpl.h:" << __LINE__ << " add_minterm_to_disjoint/new_m_cte: " << new_m_cte << "\n"
			<< "solver.tmpl.h:" << __LINE__ << " add_minterm_to_disjoint/new_m_exp:\n";
		for (const auto& e: new_m_exp)
			BOOST_LOG_TRIVIAL(trace)
				<< "solver.tmpl.h:" << __LINE__ << "\t" << e;
		#endif // DEBUG

		// case 1
		if (get_exponent(d) == new_m_exp) {

			#ifdef DEBUG
			BOOST_LOG_TRIVIAL(trace)
				<< "solver.tmpl.h:" << __LINE__ << " add_minterm_to_disjoint/[case1]/new_disjoint: " << d << "\n";
			#endif // DEBUG

			new_disjoint.insert(d);
			continue;
		}
		if (auto d_cte = get_constant(d); (d_cte & new_m_cte) != false) {
			// case 2
			if ((d_cte & ~new_m_cte) != false) {
				new_disjoint.insert(~new_m_cte & d);

				#ifdef DEBUG
				BOOST_LOG_TRIVIAL(trace)
					<< "solver.tmpl.h:" << __LINE__ << " add_minterm_to_disjoint/[case2]/new_disjoint: " << (~new_m_cte & d) << "\n";
				#endif // DEBUG

			// case 3
			} else if ((~d_cte & new_m_cte) != false) {
				new_disjoint.insert(d);
				new_m = (~d_cte & new_m) | bf_reduce_canonical<BAs...>();

				#ifdef DEBUG
				BOOST_LOG_TRIVIAL(trace)
					<< "solver.tmpl.h:" << __LINE__ << " add_minterm_to_disjoint/[case3]/new_disjoint: " << d << "\n"
					<< "solver.tmpl.h:" << __LINE__ << " add_minterm_to_disjoint/[case3]/new_m: " << new_m << "\n";
				#endif // DEBUG

			// case 4
			} else {

				// otherwise, go with the splitters
				#ifdef DEBUG
				BOOST_LOG_TRIVIAL(trace)
					<< "solver.tmpl.h:" << __LINE__ << " add_minterm_to_disjoint/[case4]/d_cte: " << d_cte << "\n";
				#endif // DEBUG

				auto s = d_cte == _1<BAs...>
					// case 4.1
					? options.splitter_one
					// case 4.2
					: splitter(d_cte
						| tau_parser::bf_constant
						| optional_value_extractor<tau_depreciating<BAs...>>);

				#ifdef DEBUG
				BOOST_LOG_TRIVIAL(trace)
					<< "solver.tmpl.h:" << __LINE__ << " add_minterm_to_disjoint/[case4]/s: " << s << "\n";
				#endif // DEBUG

				new_disjoint.insert(s & d);
				new_m = (~s & new_m) | bf_reduce_canonical<BAs...>();

				#ifdef DEBUG
				BOOST_LOG_TRIVIAL(trace)
					<< "solver.tmpl.h:" << __LINE__ << " add_minterm_to_disjoint/[case4]/new_disjoint: " << (s & d) << "\n"
					<< "solver.tmpl.h:" << __LINE__ << " add_minterm_to_disjoint/[case4]/new_m: " << new_m << "\n";
				#endif // DEBUG
			}
		// case 5
		} else {
			new_disjoint.insert(d);

			#ifdef DEBUG
			BOOST_LOG_TRIVIAL(trace)
				<< "solver.tmpl.h:" << __LINE__ << " add_minterm_to_disjoint/[case5]/new_disjoint: " << d << "\n";
			#endif // DEBUG
		}
	}
	new_disjoint.insert(new_m);
	return new_disjoint;
}

template<typename...BAs>
std::optional<minterm_system<BAs...>> make_minterm_system_disjoint(
		const minterm_system<BAs...>& sys, const solver_options<BAs...>& options) {

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "solver.tmpl.h:" << __LINE__ << " make_minterm_system_disjoint/system: ";
	for (const auto& minterm : sys)
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << "\t" << minterm;
	#endif // DEBUG

	minterm_system<BAs...> disjoints;
	for (auto it = sys.begin(); it != sys.end(); ++it)
		if (auto new_disjoints = add_minterm_to_disjoint<BAs...>(disjoints, *it, options); new_disjoints)
			disjoints = new_disjoints.value();
		else return {};

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "solver.tmpl.h:" << __LINE__ << " make_minterm_system_disjoint/disjoints: ";
	for (const auto& minterm : disjoints)
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << "\t" << minterm;
	#endif // DEBUG

	return disjoints;
}

template<typename...BAs>
std::optional<solution<BAs...>> solve_minterm_system(const minterm_system<BAs...>& system,
		const solver_options<BAs...>& options) {
	// To solve the minterm system, we use the Corollary 3.2 (of Taba Book),
	// the splitters to compute proper c_i's, and finally, use find_solution
	// to compute one solution of the resulting system of equalities (squeezed).

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "solver.tmpl.h:" << __LINE__ << " solve_minterm_system/system: ";
	for (const auto& minterm : system)
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << "\t" << minterm;
	#endif // DEBUG

	// We know the system has a solution as we only iterate over non-negative
	// minterms (which trivially satisfy the condition of Theorem 3.3)
	equality<BAs...> eq = _0<BAs...>;
	auto disjoint_minterms = make_minterm_system_disjoint<BAs...>(system, options);
	if (!disjoint_minterms.has_value()) return {};

	for (auto& neq: disjoint_minterms.value()) {

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << " solve_minterm_system/neq: " << neq;
		#endif // DEBUG

		auto nf = neq
			| tau_parser::bf_neq
			| tau_parser::bf
			| optional_value_extractor<tau_depreciating<BAs...>>
			| bf_reduce_canonical<BAs...>();

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << " solve_minterm_system/nf: " << nf;
		#endif // DEBUG

		if (nf == _0<BAs...>) continue;

		auto cte = get_constant(nf);

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << " solve_minterm_system/cte: " << cte;
		#endif // DEBUG

		auto minterm = get_minterm(nf);

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << " solve_minterm_system/minterm: " << minterm;
		#endif // DEBUG

		eq = eq | (cte & ~minterm);

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << " solve_minterm_system/eq[partial]: " << eq;
		#endif // DEBUG
	}

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "solver.tmpl.h:" << __LINE__ << " solve_minterm_system/eq[final]: " << eq;
	#endif // DEBUG

	eq = build_wff_eq(eq);
	return find_solution(eq);
}

template<typename...BAs>
std::optional<solution<BAs...>> solve_inequality_system(const inequality_system<BAs...>& system,
		const solver_options<BAs...>& options) {
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
		<< "solver.tmpl.h:" << __LINE__ << " solve_inequality_system/system: ";
	for (const auto& inequality : system)
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << "\t" << inequality;
	#endif // DEBUG
	// If no inequality is contained, return an empty solution
	if (system.empty()) return solution<BAs...>{};
	//for (auto& ms: minterm_inequality_system_range<BAs...>(system)) {
	for (auto it = minterm_inequality_system_iterator<BAs...>(system); it != minterm_inequality_system_iterator<BAs...>::end; ++it) {

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << " solve_inequality_system/minterm system: ";
		for (const auto& minterm : *it)
			BOOST_LOG_TRIVIAL(trace)
				<< "solver.tmpl.h:" << __LINE__ << "\t" << minterm;
		#endif // DEBUG

		auto solution = solve_minterm_system<BAs...>(*it, options);
		if (solution.has_value()) return solution;
	}

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "solver.tmpl.h:" << __LINE__ << " solve_inequality_system/solution: {}";
	#endif // DEBUG

	return {};
}

template<typename...BAs>
std::optional<solution<BAs...>> solve_general_system(const equation_system<BAs...>& system,
		const solver_options<BAs...>& options) {
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
			<< "solver.tmpl.h:" << __LINE__ << " solve_system/eq: " << system.first.value();
	if (!system.second.empty()) {
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << " solve_system/inequalities: ";
		for (const auto& inequality : system.second)
			BOOST_LOG_TRIVIAL(trace)
				<< "solver.tmpl.h:" << __LINE__ << "\t" << inequality;
	}
	#endif // DEBUG

	if (!system.first) return solve_inequality_system<BAs...>(system.second, options);
	if (system.second.empty()) return find_solution(system.first.value());

	auto phi = lgrs(system.first.value());
	if (!phi.has_value()) return {};

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "solver.tmpl.h:" << __LINE__ << " solve_system/phi: ";
	for (const auto& [k, v]: phi.value())
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << "\t" << k << " := " << v;
	#endif // DEBUG


	inequality_system<BAs...> inequalities;
	// for each inequality g_i we apply the transformation given by lgrs solution
	// of the equality
	for (auto& g_i: system.second) {
		auto nphi = phi.value(), ng_i = replace(g_i, nphi) | bf_reduce_canonical<BAs...>();
		if (ng_i == _F<BAs...>) {

			#ifdef DEBUG
			BOOST_LOG_TRIVIAL(trace)
				<< "solver.tmpl.h:" << __LINE__ << " solve_system/inequality_solution: {}";
			#endif // DEBUG

			return {};
		}
		else if (ng_i == _T<BAs...>) continue;

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << " solve_system/inequality: " << ng_i;
		#endif // DEBUG

		inequalities.insert(ng_i);
	}

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "solver.tmpl.h:" << __LINE__ << " solve_system/inequalities: ";
	for (const auto& inequality : inequalities)
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << "\t" << inequality;
	#endif // DEBUG


	// solve the given system  of inequalities
	auto inequality_solution = solve_inequality_system<BAs...>(inequalities, options);
	if (!inequality_solution.has_value()) {

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << " solve_system/inequality_solution: {}";
		#endif // DEBUG

		return {};
	}

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "solver.tmpl.h:" << __LINE__ << " solve_system/inequality_solution: ";
	for (const auto& [k, v]: inequality_solution.value())
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << "\t" << k << " := " << v;
	#endif // DEBUG

	// and finally, apply the solution to lgrs solution to get the final one (ϕ (T)).
	// Solutions coming from inequality_solution for variables appearing also
	// in the equality part will be replaced in the next step
	solution<BAs...> solution = inequality_solution.value();

	// Now we need to add solutions for variables in the lgrs
	for (auto& [var, func]: phi.value()) {
		auto func_with_neq_assgm = replace(func, inequality_solution.value());
		// Now assign the remaining variables to 0 and compute
		// resulting value for var
		solution[var] = replace_free_vars_by(func_with_neq_assgm,
			_0_trimmed<BAs...>) | bf_reduce_canonical<BAs...>();
	}

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "solver.tmpl.h:" << __LINE__ << " solve_system/inequality_solution: ";
	for (const auto& [k, v]: solution)
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << "\t" << k << " := " << v;
	#endif // DEBUG

	return solution;
}

template<typename...BAs>
bool check_extreme_solution(const equation_system<BAs...>& system,
		const solution<BAs...>& substitution) {
	#ifdef DEBUG
	if (system.first)
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << " check_extreme_solution/eq: " << system.first.value();
	for (const auto& inequality: system.second)
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << " check_extreme_solution/ineq: " << inequality;
	for (const auto& [k, v]: substitution)
		BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << " check_extreme_solution/substitution: " << k << " := " << v;
	#endif // DEBUG
	// We check if the solution satisfies the inequalities of the system
	for (const auto& inequality: system.second) {
		auto value = replace(inequality, substitution) | bf_reduce_canonical<BAs...>();
		if (value == _F<BAs...>)
			return false;
	}
	return true;
}

template<typename...BAs>
std::optional<solution<BAs...>> solve_maximum_system(const equation_system<BAs...>& system) {
	if (auto s = find_maximal_solution<BAs...>(system); s)
		return check_extreme_solution(system, s.value()) ? s : std::optional<solution<BAs...>>();
	else return {};
}

template<typename...BAs>
std::optional<solution<BAs...>> solve_minimum_system(const equation_system<BAs...>& system) {
	if(auto s = find_minimal_solution<BAs...>(system); s)
		return check_extreme_solution(system, s.value()) ? s : std::optional<solution<BAs...>>();
	else return {};
}

template<typename...BAs>
std::optional<solution<BAs...>> solve_system(const equation_system<BAs...>& system,
		const solver_options<BAs...>& options) {
	// we try to find a maximal solution
	if (options.mode != solver_mode::minimum) {
		if (auto solution = solve_maximum_system<BAs...>(system); solution)
			return solution;
		else if (options.mode == solver_mode::maximum) return {};
	}
	// if it fails, we try a minimum solution
	if (auto solution = solve_minimum_system<BAs...>(system); solution)
		return solution;
	else if (options.mode == solver_mode::minimum) return {};
	// if we have no equality we try to solve the inequalities
	if (!system.first.has_value()) return solve_inequality_system<BAs...>(system.second, options);
	// otherwise we try a general solution
	return solve_general_system<BAs...>(system, options);
}

template<typename...BAs>
std::optional<solution<BAs...>> solve(const equations<BAs...>& eqs,
		const solver_options<BAs...>& options) {
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
					| optional_value_extractor<tau_depreciating<BAs...>>;
				auto r = eq
					| tau_parser::bf_eq
					| tau_parser::bf
					| optional_value_extractor<tau_depreciating<BAs...>>;
				system.first = build_wff_eq(l | r);
			}
		}
		else system.second.insert(eq);
	}
	return solve_system<BAs...>(system, options);
}

// entry point for the solver
template<typename...BAs>
std::optional<solution<BAs...>> solve(const tau_depreciating<BAs...>& form,
		const solver_options<BAs...>& options) {
	if (form == _T<BAs...>) return { solution<BAs...>() };

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "solver.tmpl.h:" << __LINE__ << " solve/form: " << form << "\n"
		<< "solver.tmpl.h:" << __LINE__ << " solve/options/type: " << options.type << "\n";
	switch (options.mode) {
		case solver_mode::maximum: BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << " solve/options.kind: maximum\n"; break;
		case solver_mode::minimum: BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << " solve/options.kind: minimum"; break;
		default: BOOST_LOG_TRIVIAL(trace)
			<< "solver.tmpl.h:" << __LINE__ << " solve/options.kind: default\n"
			<< "solver.tmpl.h:" << __LINE__ << " solve/options.splitter_one:"
			<< options.splitter_one; break;
	}
	#endif // DEBUG

	auto dnf = form | bf_reduce_canonical<BAs...>();
	for (auto& clause: get_leaves(dnf, tau_parser::wff_or)) {
		// Reject clause involving temporal quantification
		if (find_top(clause, is_temporal_quantifier<BAs...>)) {
			BOOST_LOG_TRIVIAL(warning) << "(Warning) Skipped clause with temporal quantifier: " << clause;
			continue;
		}
		auto is_equation = [](const tau_depreciating<BAs...>& n) {
			return is_child_non_terminal<tau_parser::bf_eq, BAs...>(n)
			|| is_child_non_terminal<tau_parser::bf_neq, BAs...>(n);
		};
		// FIXME convert vars to a set
		auto eqs = select_top(clause, is_equation);
		if (eqs.empty()) continue;
		auto solution = solve<BAs...>(
			std::set<tau_depreciating<BAs...>>(eqs.begin(), eqs.end()), options);
		if (solution.has_value()) return solution;
	}
	return {};
}

} // idni::tau_lang namespace