// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "solver.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "solver"

// In what follows we use the algorithms and notations of TABA book (cf.
// Section 3.2).Chek (https://github.com/IDNI/tau-lang/blob/main/docs/taba.pdf)
// for the details.

namespace idni::tau_lang {

// not used?
// template <NodeType node>
// solution<node> make_removed_vars_solution(const std::vector<var>& originals, tref gh) {
// 	solution<node> solution;
// 	for (size_t i = 1; i < originals.size(); ++i) solution[originals[i]] = _0<node>;
// 	// FIXME convert vars to a set
// 	auto remaing = select_top(gh, is_child_non_terminal<tau::variable, node>);
// 	for (auto& v: remaing) solution.erase(v);
// 	return solution;
// }

template <NodeType node>
std::optional<solution<node>> find_solution(equality eq,
	solution<node> substitution, solver_mode mode)
{
	using tau = tree<node>;
	using tt = tau::traverser;
	// We would use the algorithm subyaccent to the following theorem (of Taba Book):
	//
	// Theorem 3.1. For f (x,X) = xg (X) + x′h (X), let Z be a zero of
	// g (Z) h (Z) (which is guaranteed to exist by Boole’s consistency condition).
	// Then both f (h (Z) ,Z) = 0 and f (g′ (Z) ,Z) = 0.
	// find a variable, say x, in the equality
	DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
				<< " find_solution/eq: " << TAU_TO_STR(eq);)
	auto has_no_var = [](tref f) {
		return !tau::get(f).find_top(is_child<node, tau::variable>);
	};

	if (!(tt(eq) | tau::bf_eq).has_value()) {
		DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
				<< " find_solution/solution[no_eq]: {}";)
		return {};
	}

	tref f = tt(eq) | tau::bf_eq | tau::bf | tt::ref;

	DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
				<< " find_solution/f: " << TAU_TO_STR(f);)

	// FIXME convert vars to a set
	if (trefs vars = tau::get(f)
		.select_top(is_child<node, tau::variable>); vars.size())
	{
		// compute g(X) and h(X) from the equality by substituting x with 0 and 1
		// with x <- h(Z)
		tref g = rewriter::replace_with<node>(vars[0], tau::_1(), f);
		tref h = rewriter::replace_with<node>(vars[0], tau::_0(), f);
		tref gh = tt(tau::get(g) & tau::get(h))
			| bf_reduce_canonical<node>() | tt::ref;

		DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
			<< " find_solution/var[0]: " << TAU_TO_STR(vars[0]) << "\n"
			<< "solver.tmpl.h:" << __LINE__
			<< " find_solution/g: " << TAU_TO_STR(g) << "\n"
			<< "solver.tmpl.h:" << __LINE__
			<< " find_solution/h: " << TAU_TO_STR(h) << "\n"
			<< "solver.tmpl.h:" << __LINE__
			<< " find_solution/gh: " << TAU_TO_STR(gh) << "\n";)

		if (has_no_var(gh)) {
			if (!tau::get(gh).equals_0()) {
				DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
					<< " find_solution/solution[gh_no_var,gh_!=_0]: {}";)
				return {};
			}
			else {
				substitution[vars[0]] = tt(mode
						== solver_mode::maximum
					? rewriter::replace<node>(
						(~tau::get(g)).get(),
								substitution)
					: rewriter::replace<node>(h,
								substitution))
					| bf_reduce_canonical<node>() | tt::ref;

				#ifdef DEBUG
				LOG_TRACE << "solver.tmpl.h:" << __LINE__
					<< " find_solution/solution[gh_no_var,gh=0]: ";
				for (const auto& [k, v]: substitution)
					LOG_TRACE
						<< "solver.tmpl.h:" << __LINE__
						<< "\t" << TAU_TO_STR(k)
						<< " := " << TAU_TO_STR(v) << " ";
				#endif // DEBUG

				return substitution;
			}
		}
		if (auto restricted = find_solution<node>(build_bf_eq<node>(gh),
						substitution, mode); restricted)
		{
			//solution.insert(restricted.value().begin(), restricted.value().end());
			substitution[vars[0]] =
				tt(mode == solver_mode::maximum
					? rewriter::replace<node>(
						(~tau::get(g)).get(),
							restricted.value())
					: rewriter::replace<node>(h,
							restricted.value()))
				| bf_reduce_canonical<node>() | tt::ref;

			#ifdef DEBUG
			LOG_TRACE << "solver.tmpl.h:" << __LINE__
				<< " find_solution/substitution[general]: ";
			for (const auto& [k, v]: substitution)
				LOG_TRACE << "solver.tmpl.h:" << __LINE__
					<< "\t" << TAU_TO_STR(k)
					<< " := " << TAU_TO_STR(v) << " ";
			#endif // DEBUG

			return substitution;
		}
	}

	DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
			<< " find_solution/substitution[no_var]: {}";)
	return {};
}

template <NodeType node>
trefs get_variables(equality eq) {
	using tau = tree<node>;
	return tau::get(eq).select_top(is_child<node, tau::variable>);
}

template <NodeType node>
trefs get_variables(const equation_system<node>& system) {
	trefs vars;
	if (system.first.has_value()) {
		trefs vs = get_variables<node>(system.first.value());
		vars.insert(vars.end(), vs.begin(), vs.end());
	}
	for (inequality t : system.second) {
		trefs vs = get_variables<node>(t);
		vars.insert(vars.end(), vs.begin(), vs.end());
	}
	return vars;
}

template <NodeType node>
std::optional<solution<node>> find_maximal_solution(const equation_system<node>& system) {
	using tau = tree<node>;
	trefs vars = get_variables<node>(system);
	if (vars.empty()) return solution<node>();
	auto substitution = solution<node>();
	for (tref var : vars) substitution[var] = tau::_1();
	return (system.first)
		? find_solution<node>(system.first.value(), substitution,
							solver_mode::maximum)
		: substitution;
}

template <NodeType node>
std::optional<solution<node>> find_minimal_solution(
	const equation_system<node>& system)
{
	using tau = tree<node>;
	trefs vars = get_variables<node>(system);
	if (vars.empty()) return solution<node>();
	auto substitution = solution<node>();
	for (tref var : vars) substitution[var] = tau::_0();
	return (system.first)
		? find_solution<node>(system.first.value(), substitution,
							solver_mode::minimum)
		: substitution;
}

template <NodeType node>
std::optional<solution<node>> find_solution(equality eq) {
	using tau = tree<node>;
	trefs vars = get_variables<node>(eq);
	if (vars.empty()) return solution<node>();
	auto substitution = solution<node>();
	for (tref var : vars) substitution[var] = tau::_1();
	return find_solution<node>(eq, substitution, solver_mode::maximum);
}

template <NodeType node>
std::optional<solution<node>> lgrs(equality eq) {
	// We would use Lowenheim’s General Reproductive Solution (LGRS) as given
	// in the following theorem (of Taba Book):
	//
	// Theorem 1.8. Let f : Bn → B be a BF, and assume f (Z) = 0
	// for some Z ∈ Bn. Then the set {X ∈ Bn| f(X) = 0} equals precisely
	// the image of ϕ : Bn → Bn defined by ϕ (X) = Zf (X) + Xf′ (X). Decyphering
	// the abuse of notation, this reads ϕ_i (X) = z_i f (X)+x_i f′ (X).

	using tau = tree<node>;
	using tt = tau::traverser;
	if (tau::get(eq).equals_T()) {
		DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
					<< " lgrs/solution: {}";)
		return solution<node>();
	}

	DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
				<< " lgrs/eq: " << TAU_TO_STR(eq) << "\n";)

	auto s = find_solution<node>(eq);
	if (!s.has_value()) {
		DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
					<< " lgrs/no solution";)

		return {};
	}
	tref f = tt(eq) | tau::bf_eq | tau::bf | tt::ref;
	solution<node> phi;
	for (auto [x_i, z_i] : s.value())
		phi[x_i] = tt((tau::get(z_i) & tau::get(f))
			+ (tau::get(x_i) & ~tau::get(f)))
				| bf_reduce_canonical<node>() | tt::ref;

	#ifdef DEBUG
	LOG_TRACE << "solver.tmpl.h:" << __LINE__
			<< " lgrs/equality: " << TAU_TO_STR(eq) << "\n"
		<< "solver.tmpl.h:" << __LINE__ << " lgrs/solution: ";
	for (auto [k, v] : phi)
		LOG_TRACE
			<< "solver.tmpl.h:" << __LINE__ << "\t" << TAU_TO_STR(k) << " := " << TAU_TO_STR(v) << " ";
	tref check = snf_wff<node>(rewriter::replace<node>(eq, phi));
	LOG_TRACE << "solver.tmpl.h:" << __LINE__
			<< " lgrs/check: " << TAU_TO_STR(check) << "\n";
	#endif // DEBUG

	return phi;
}

template <NodeType node>
struct minterm_iterator {
	// iterator traits
	using difference_type = size_t;
	using value_type = minterm;
	using pointer = const minterm*;
	using reference = const minterm&;
	using iterator_category = std::input_iterator_tag;

	using tau = tree<node>;
	using tt = tau::traverser;

	// sentinel class
	class sentinel {};
	static constexpr sentinel end{};

	minterm_iterator(tref f) {
		// FIXME convert vars to a set
		if (trefs vars = tau::get(f)
				.select_top(is_child<node, tau::variable>);
			vars.size())
		{
			// we start with the full bf...
			tref partial_bf = f;
			// ... and the first variable (for computing the first partial minterm)
			tref partial_minterm = tau::_1();
			for (tref v : vars) {
				// we add the current choice to the list of choices...
				partial_minterm = (tau::get(partial_minterm)
					& ~tau::get(v)).get();
				choices.emplace_back(v, false, partial_bf,
							partial_minterm);
				partial_bf = rewriter::replace_with<node>(v,
					tau::_0(), partial_bf);
				// ... and compute new values for the next one
			}
			// if the current choices correspond to a proper minterm, we update the current
			// minterm, otherwise we compute the next valid choice
			if (tref minterm = make_current_minterm();
				!tau::get(minterm).equals_0()) current = minterm;
			else make_next_choice();
		// otherwise, i.e. no vars, we return an empty iterator as we have no vars.
		} else exhausted = true;
	}

	minterm_iterator();

	minterm_iterator<node>& operator++() {
		if (exhausted) return *this;
		make_next_choice();
		return *this;
	}

	minterm_iterator<node> operator++(int) {
		return ++*this;
	}

	bool operator==(const minterm_iterator<node>& that) const = default;
	bool operator!=(const minterm_iterator<node>& that) const = default;

	bool operator==(const sentinel&) const {
		return exhausted;
	}

	bool operator!=(const sentinel&) const {
		return !exhausted;
	}

	minterm operator*() const {
		return current;
	}

private:
	struct choice {
		tref var;
		bool value;
		tref partial_bf;
		tref partial_minterm;

		bool operator==(const choice&) const = default;
		bool operator!=(const choice&) const = default;
	};

	std::vector<choice> choices;
	tref current;
	bool exhausted = false;

	tref make_current_minterm() {
		tref cte = choices.back().value
			? rewriter::replace_with<node>(choices.back().var,
				tau::_1(), choices.back().partial_bf)
			: rewriter::replace_with<node>(choices.back().var,
				tau::_0(), choices.back().partial_bf);
		tref current = (tau::get(cte) & tau::get(choices.back()
							.partial_minterm)).get();

		DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
			<< " make_current_minterm/current: "
			<< TAU_TO_STR(current);)

		return current;
	}

	void make_next_choice() {
		if (exhausted) return;
		// update the choices from right to left
		size_t last_changed_value = choices.size();
		while (last_changed_value > 0)
			if (choices[--last_changed_value].value ^= true) break;
		// if all choices are exhausted, we are done
		if (last_changed_value == 0 && choices[0].value == false) {
			exhausted = true; return;
		}
		// we update the choices from the last changed value till the end
		update_choices_from(last_changed_value);
		// if the current minterm is valid, we update current field, otherwise...
		if (tref mt = make_current_minterm();
			!tau::get(mt).equals_0()) { current = mt; return; }
		// ... we try again with the next choice
		make_next_choice();
	}

	void update_choices_from(size_t start) {
		if (start == 0) {
			choices[0].partial_minterm = choices[0].value
					? choices[0].var
					: (~tau::get(choices[0].var)).get();
			++start;
		}
		for (size_t i = start; i < choices.size(); ++i) {
			choices[i].partial_minterm = ((choices[i].value
					? tau::get(choices[i].var)
					: ~tau::get(choices[i].var))
				& tau::get(choices[i - 1].partial_minterm)).get();
			choices[i].partial_bf = choices[i - 1].value
				? rewriter::replace_with<node>(
					choices[i - 1].var, tau::_1(),
					choices[i - 1].partial_bf)
				: rewriter::replace_with<node>(
					choices[i - 1].var, tau::_0(),
					choices[i - 1].partial_bf);
			// if current partial bf is 0, we can skip the rest of the choices
			// as the corresponding minterms will be 0.
			if (tau::get(choices[i].partial_bf).equals_0()) {
				for (size_t j = i + 1; j < choices.size(); ++j) {
					choices[j].partial_bf = tau::_0();
					choices[j].value = true;
				}
				return;
			}
		}
	}
};

template <NodeType node>
struct minterm_range {
	explicit minterm_range(tref f): f (f) {}

	bool empty() { return false; }

	minterm_iterator<node> begin() {
		minterm_iterator<node> begin(f);
		return begin;
	}

	minterm_iterator<node>::sentinel end() const {
		return minterm_iterator<node>::end;
	}

	bool operator==(const minterm_range<node>&) const = default;
	bool operator!=(const minterm_range<node>&) const = default;

private:
	const tref f;
};

template <NodeType node>
struct minterm_inequality_system_iterator {

	using tau = tree<node>;
	using tt = tau::traverser;

	// iterator traits
	using difference_type = size_t;
	using value_type = minterm_system<node>;
	using pointer = const minterm_system<node>*;
	using reference = const minterm_system<node>&;
	using iterator_category = std::input_iterator_tag;

	// sentinel class
	struct sentinel {};
	static constexpr sentinel end{};

	minterm_inequality_system_iterator(const inequality_system<node>& sys) {
		if (sys.empty()) { exhausted = true; return; }
		// for each inequality in the system, we create a minterm range
		for (tref neq : sys) {
			tref f = tt(neq) | tau::bf_neq | tau::bf | tt::ref;
			ranges.push_back(minterm_range<node>(f));
		}
		// we initialize the minterm iterators
		for (auto& range : ranges) {
			minterm_iterators.push_back(range.begin());
			if (minterm_iterators.back() == range.end()) {
				exhausted = true;
				break;
			}
		}
		current = make_current_minterm_system();
	}

	minterm_inequality_system_iterator();

	minterm_inequality_system_iterator<node> &operator++() {
		if (exhausted) return *this;
		make_next_choice();
		return *this;
	}

	minterm_inequality_system_iterator<node> operator++(int) {
		return ++*this;
	}

	bool operator==(const minterm_inequality_system_iterator<node>& that) const = default;
	bool operator!=(const minterm_inequality_system_iterator<node>& that) const = default;

	bool operator==(const sentinel&) const {
		return exhausted;
	}

	bool operator!=(const sentinel&) const {
		return !exhausted;
	}

	const minterm_system<node>& operator*() const {
		return current;
	}

private:
	std::vector<minterm_range<node>> ranges;
	std::vector<minterm_iterator<node>> minterm_iterators;
	minterm_system<node> current;
	bool exhausted = false;

	minterm_system<node> make_current_minterm_system() {
		minterm_system<node> minterms;
		for (auto& it : minterm_iterators)
			minterms.insert(build_bf_neq<node>(*it));

		#ifdef DEBUG
		LOG_TRACE << "solver.tmpl.h:" << __LINE__
				<< " make_current_minterm_system/minterms: ";
		for (tref minterm : minterms) LOG_TRACE << "solver.tmpl.h:"
			<< __LINE__ << "\t" << TAU_TO_STR(minterm);
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

template <NodeType node>
class minterm_inequality_system_range {
public:

	explicit minterm_inequality_system_range(
		const inequality_system<node>& sys): sys(sys) {};

	bool empty() { return sys.empty(); }

	minterm_inequality_system_iterator<node> begin() {
		minterm_inequality_system_iterator<node> begin(sys);
		return begin;
	}

	minterm_inequality_system_iterator<node>::sentinel end() {
		return minterm_inequality_system_iterator<node>::end;
	}

private:
	inequality_system<node> sys;
};

template <NodeType node>
tref get_constant(minterm m) {
	using tau = tree<node>;
	//auto cte = find_top(m, is_child_non_terminal<tau::bf_constant, node>);
	//return cte ? cte.value() : _1<node>;
	auto is_bf_constant = [](const auto& n) -> bool {
		return is_child<node, tau::bf_constant>(n);
	};
	// FIXME convert vars to a set
	trefs all_vs = tau::get(m).select_top(is_bf_constant);
	return build_bf_and<node>(all_vs);
}

template <NodeType node>
typename tree<node>::subtree_set get_exponent(tref n) {
	using tau = tree<node>;
	using tt = tau::traverser;
	auto is_bf_literal = [](tref n) -> bool {
		return (tt(n) | tau::variable).has_value()
			|| (tt(n) | tau::bf_neg | tau::bf | tau::variable)
								.has_value();
	};
	// FIXME convert vars to a set
	trefs all_vs = tau::get(n).select_top(is_bf_literal);
	return typename tree<node>::subtree_set(all_vs.begin(), all_vs.end());
}

template <NodeType node>
tref get_minterm(minterm m) {
	return build_bf_and<node>(get_exponent<node>(m));
}

template <NodeType node>
std::optional<minterm_system<node>> add_minterm_to_disjoint(
	const minterm_system<node>& disjoint, minterm m,
	const solver_options& options)
{
	using tau = tree<node>;
	using tt = tau::traverser;

	minterm_system<node> new_disjoint;
	tref new_m = m;

	for (tref d : disjoint) {
		const auto& new_m_cte = tau::get(get_constant<node>(new_m));
		auto new_m_exp = get_exponent<node>(new_m);

		#ifdef DEBUG
		LOG_TRACE << "solver.tmpl.h:" << __LINE__
				<< " add_minterm_to_disjoint/d: " << TAU_TO_STR(d) << "\n"
			<< "solver.tmpl.h:" << __LINE__
				<< " add_minterm_to_disjoint/new_m: " << TAU_TO_STR(new_m) << "\n"
			<< "solver.tmpl.h:" << __LINE__
				<< " add_minterm_to_disjoint/new_m_cte: " << new_m_cte.dump_to_str() << "\n"
			<< "solver.tmpl.h:" << __LINE__
				<< " add_minterm_to_disjoint/new_m_exp:\n";
		for (tref e : new_m_exp) LOG_TRACE << "solver.tmpl.h:"
					<< __LINE__ << "\t" << TAU_TO_STR(e);
		#endif // DEBUG

		// case 1
		if (get_exponent<node>(d) == new_m_exp) {

			DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
				<< " add_minterm_to_disjoint/[case1]/new_disjoint: "
				<< TAU_TO_STR(d) << "\n";)

			new_disjoint.insert(d);
			continue;
		}
		const auto& d_cte = tau::get(get_constant<node>(d));
		if ((d_cte & new_m_cte) != false) {
			// case 2
			if ((d_cte & ~new_m_cte) != false) {
				const auto& x = ~new_m_cte & tau::get(d);
				new_disjoint.insert(x.get());

				DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
					<< " add_minterm_to_disjoint/[case2]/new_disjoint: "
					<< x.dump_to_str() << "\n";)

			// case 3
			} else if ((~d_cte & new_m_cte) != false) {
				new_disjoint.insert(d);
				new_m = tt(~d_cte & tau::get(new_m))
					| bf_reduce_canonical<node>() | tt::ref;

				DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
					<< " add_minterm_to_disjoint/[case3]/new_disjoint: "
					<< TAU_TO_STR(d) << "\n"
					<< "solver.tmpl.h:" << __LINE__
					<< " add_minterm_to_disjoint/[case3]/new_m: "
					<< TAU_TO_STR(new_m) << "\n";)

			// case 4
			} else {
				// otherwise, go with the splitters
				DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
					<< " add_minterm_to_disjoint/[case4]/d_cte: "
					<< d_cte.dump_to_str() << "\n";)

				tref s = d_cte.equals_1()
					// case 4.1
					? options.splitter_one
					// case 4.2
					: splitter(tau::get(tt(d_cte)
						| tau::bf_constant
						| tt::ref)).get();

				DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
					<< " add_minterm_to_disjoint/[case4]/s: "
					<< TAU_TO_STR(s) << "\n";)

				const auto& st = tau::get(s);
				new_disjoint.insert((st & tau::get(d)).get());
				new_m = tt(~st & tau::get(new_m))
					| bf_reduce_canonical<node>() | tt::ref;

				DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
					<< " add_minterm_to_disjoint/[case4]/new_disjoint: "
					<< (st & tau::get(d)).dump_to_str() << "\n"
					<< "solver.tmpl.h:" << __LINE__
					<< " add_minterm_to_disjoint/[case4]/new_m: "
					<< TAU_TO_STR(new_m) << "\n";)
			}
		// case 5
		} else {
			new_disjoint.insert(d);

			DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
				<< " add_minterm_to_disjoint/[case5]/new_disjoint: "
				<< TAU_TO_STR(d) << "\n";)
		}
	}
	new_disjoint.insert(new_m);
	return new_disjoint;
}

template <NodeType node>
std::optional<minterm_system<node>> make_minterm_system_disjoint(
	const minterm_system<node>& sys, const solver_options& options)
{
	#ifdef DEBUG
	using tau = tree<node>;
	LOG_TRACE << "solver.tmpl.h:" << __LINE__
		<< " make_minterm_system_disjoint/system: ";
	for (minterm t : sys) LOG_TRACE
		<< "solver.tmpl.h:" << __LINE__ << "\t" << TAU_TO_STR(t);
	#endif // DEBUG

	minterm_system<node> disjoints;
	for (auto it = sys.begin(); it != sys.end(); ++it)
		if (auto new_disjoints = add_minterm_to_disjoint<node>(
						disjoints, *it, options);
			new_disjoints)
				disjoints = new_disjoints.value();
		else return {};

	#ifdef DEBUG
	LOG_TRACE << "solver.tmpl.h:" << __LINE__
		<< " make_minterm_system_disjoint/disjoints: ";
	for (minterm t : disjoints) LOG_TRACE
		<< "solver.tmpl.h:" << __LINE__ << "\t" << TAU_TO_STR(t);
	#endif // DEBUG

	return disjoints;
}

template <NodeType node>
std::optional<solution<node>> solve_minterm_system(
	const minterm_system<node>& system, const solver_options& options)
{
	// To solve the minterm system, we use the Corollary 3.2 (of Taba Book),
	// the splitters to compute proper c_i's, and finally, use find_solution
	// to compute one solution of the resulting system of equalities (squeezed).

	using tau = tree<node>;
	using tt = tau::traverser;

	#ifdef DEBUG
	LOG_TRACE << "solver.tmpl.h:" << __LINE__
		<< " solve_minterm_system/system: ";
	for (minterm t : system) LOG_TRACE
		<< "solver.tmpl.h:" << __LINE__ << "\t" << TAU_TO_STR(t);
	#endif // DEBUG

	// We know the system has a solution as we only iterate over non-negative
	// minterms (which trivially satisfy the condition of Theorem 3.3)
	equality eq = tau::_0();
	auto disjoint_minterms = make_minterm_system_disjoint<node>(system, options);
	if (!disjoint_minterms.has_value()) return {};

	for (tref neq : disjoint_minterms.value()) {

		DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
			<< " solve_minterm_system/neq: " << TAU_TO_STR(neq);)

		tref nf = tt(neq) | tau::bf_neq | tau::bf
			| bf_reduce_canonical<node>() | tt::ref;

		DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
			<< " solve_minterm_system/nf: " << TAU_TO_STR(nf);)

		if (tau::get(nf).equals_0()) continue;

		tref cte = get_constant<node>(nf);

		DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
			<< " solve_minterm_system/cte: " << TAU_TO_STR(cte);)

		minterm t = get_minterm<node>(nf);

		DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
			<< " solve_minterm_system/minterm: " << TAU_TO_STR(t);)

		eq = (tau::get(eq) | (tau::get(cte) & ~tau::get(t))).get();

		DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
			<< " solve_minterm_system/eq[partial]: " << TAU_TO_STR(eq);)
	}

	DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
		<< " solve_minterm_system/eq[final]: " << TAU_TO_STR(eq);)

	eq = build_bf_eq<node>(eq);
	return find_solution<node>(eq);
}

template <NodeType node>
std::optional<solution<node>> solve_inequality_system(
	const inequality_system<node>& system, const solver_options& options)
{
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
	using tau = tree<node>;
	LOG_TRACE << "solver.tmpl.h:" << __LINE__
		<< " solve_inequality_system/system: ";
	for (inequality t : system) LOG_TRACE
		<< "solver.tmpl.h:" << __LINE__ << "\t" << TAU_TO_STR(t);
	#endif // DEBUG
	// If no inequality is contained, return an empty solution
	if (system.empty()) return solution<node>{};
	//for (auto& ms: minterm_inequality_system_range<node>(system)) {
	for (auto it = minterm_inequality_system_iterator<node>(system);
		it != minterm_inequality_system_iterator<node>::end; ++it)
	{
		#ifdef DEBUG
		LOG_TRACE << "solver.tmpl.h:" << __LINE__
			<< " solve_inequality_system/minterm system: ";
		for (minterm t : *it) LOG_TRACE
			<< "solver.tmpl.h:" << __LINE__ << "\t" << TAU_TO_STR(t);
		#endif // DEBUG

		auto solution = solve_minterm_system<node>(*it, options);
		if (solution.has_value()) return solution;
	}

	DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
		<< " solve_inequality_system/solution: {}";)

	return {};
}

template <NodeType node>
std::optional<solution<node>> solve_general_system(
	const equation_system<node>& system, const solver_options& options)
{
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

	using tau = tree<node>;
	using tt = tau::traverser;

	#ifdef DEBUG
	if (system.first.has_value()) LOG_TRACE
		<< "solver.tmpl.h:" << __LINE__ << " solve_system/eq: "
		<< TAU_TO_STR(system.first.value());
	if (!system.second.empty()) {
		LOG_TRACE << "solver.tmpl.h:" << __LINE__
			<< " solve_system/inequalities: ";
		for (inequality t : system.second) LOG_TRACE
			<< "solver.tmpl.h:" << __LINE__ << "\t" << TAU_TO_STR(t);
	}
	#endif // DEBUG

	if (!system.first)
		return solve_inequality_system<node>(system.second, options);
	if (system.second.empty())
		return find_solution<node>(system.first.value());

	auto phi = lgrs<node>(system.first.value());
	if (!phi.has_value()) return {};

	#ifdef DEBUG
	LOG_TRACE
		<< "solver.tmpl.h:" << __LINE__ << " solve_system/phi: ";
	for (const auto& [k, v]: phi.value()) LOG_TRACE
		<< "solver.tmpl.h:" << __LINE__ << "\t" << TAU_TO_STR(k)
						<< " := " << TAU_TO_STR(v);
	#endif // DEBUG

	inequality_system<node> inequalities;
	// for each inequality g_i we apply the transformation given by lgrs solution
	// of the equality
	for (tref g_i : system.second) {
		auto nphi = phi.value(),
			ng_i = tt(rewriter::replace<node>(g_i, nphi))
				| bf_reduce_canonical<node>() | tt::ref;
		if (tau::get(ng_i).equals_F()) {
			DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
				<< " solve_system/inequality_solution: {}";)

			return {};
		}
		else if (tau::get(ng_i).equals_T()) continue;

		DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
			<< " solve_system/inequality: " << TAU_TO_STR(ng_i);)

		inequalities.insert(ng_i);
	}

	#ifdef DEBUG
	LOG_TRACE << "solver.tmpl.h:" << __LINE__
		<< " solve_system/inequalities: ";
	for (inequality t : inequalities) LOG_TRACE
		<< "solver.tmpl.h:" << __LINE__ << "\t" << TAU_TO_STR(t);
	#endif // DEBUG


	// solve the given system  of inequalities
	auto inequality_solution =
			solve_inequality_system<node>(inequalities, options);
	if (!inequality_solution.has_value()) {
		DBG(LOG_TRACE << "solver.tmpl.h:" << __LINE__
			<< " solve_system/inequality_solution: {}";)

		return {};
	}

	#ifdef DEBUG
	LOG_TRACE << "solver.tmpl.h:" << __LINE__
		<< " solve_system/inequality_solution: ";
	for (auto [k, v]: inequality_solution.value()) LOG_TRACE
		<< "solver.tmpl.h:" << __LINE__ << "\t" << TAU_TO_STR(k)
						<< " := " << TAU_TO_STR(v);
	#endif // DEBUG

	// and finally, apply the solution to lgrs solution to get the final one (ϕ (T)).
	// Solutions coming from inequality_solution for variables appearing also
	// in the equality part will be replaced in the next step
	solution<node> solution = inequality_solution.value();

	// Now we need to add solutions for variables in the lgrs
	for (auto [var, func]: phi.value()) {
		tref func_with_neq_assgm = rewriter::replace<node>(func,
						inequality_solution.value());
		// Now assign the remaining variables to 0 and compute
		// resulting value for var
		solution[var] =	tt(replace_free_vars_by<node>(func_with_neq_assgm,
							tau::_0_trimmed()))
					| bf_reduce_canonical<node>() | tt::ref;
	}

	#ifdef DEBUG
	LOG_TRACE << "solver.tmpl.h:" << __LINE__
		<< " solve_system/inequality_solution: ";
	for (auto [k, v]: solution) LOG_TRACE
		<< "solver.tmpl.h:" << __LINE__ << "\t" << TAU_TO_STR(k)
						<< " := " << TAU_TO_STR(v);
	#endif // DEBUG

	return solution;
}

template <NodeType node>
bool check_extreme_solution(const equation_system<node>& system,
	const solution<node>& substitution)
{
	using tau = tree<node>;
	using tt = tau::traverser;
	#ifdef DEBUG
	if (system.first) LOG_TRACE
		<< "solver.tmpl.h:" << __LINE__ << " check_extreme_solution/eq: "
		<< TAU_TO_STR(system.first.value());
	for (inequality t : system.second) LOG_TRACE
		<< "solver.tmpl.h:" << __LINE__ << " check_extreme_solution/ineq: "
		<< TAU_TO_STR(t);
	for (auto [k, v]: substitution) LOG_TRACE
		<< "solver.tmpl.h:" << __LINE__ << " check_extreme_solution/substitution: "
		<< TAU_TO_STR(k) << " := " << TAU_TO_STR(v);
	#endif // DEBUG
	// We check if the solution satisfies the inequalities of the system
	for (inequality t : system.second) {
		tref value = tt(rewriter::replace<node>(t, substitution))
			| bf_reduce_canonical<node>() | tt::ref;
		if (tau::get(value).equals_F()) return false;
	}
	return true;
}

template <NodeType node>
std::optional<solution<node>> solve_maximum_system(
	const equation_system<node>& system)
{
	if (auto s = find_maximal_solution<node>(system); s)
		return check_extreme_solution<node>(system, s.value()) ? s
					: std::optional<solution<node>>();
	else return {};
}

template <NodeType node>
std::optional<solution<node>> solve_minimum_system(
	const equation_system<node>& system)
{
	if(auto s = find_minimal_solution<node>(system); s)
		return check_extreme_solution<node>(system, s.value()) ? s
					: std::optional<solution<node>>();
	else return {};
}

template <NodeType node>
std::optional<solution<node>> solve_system(const equation_system<node>& system,
					const solver_options& options)
{
	// we try to find a maximal solution
	if (options.mode != solver_mode::minimum) {
		if (auto solution = solve_maximum_system<node>(system); solution)
			return solution;
		else if (options.mode == solver_mode::maximum) return {};
	}
	// if it fails, we try a minimum solution
	if (auto solution = solve_minimum_system<node>(system); solution)
		return solution;
	else if (options.mode == solver_mode::minimum) return {};
	// if we have no equality we try to solve the inequalities
	if (!system.first.has_value())
		return solve_inequality_system<node>(system.second, options);
	// otherwise we try a general solution
	return solve_general_system<node>(system, options);
}

template <NodeType node>
std::optional<solution<node>> solve(const equations<node>& eqs,
					const solver_options& options)
{
	using tau = tree<node>;
	using tt = tau::traverser;
	// split among equalities and inequalities
	equation_system<node> system;
	for (tref eq : eqs) {
		if (tau::get(eq).child_is(tau::bf_eq)) {
			if (!system.first.has_value())
				system.first = std::optional<equality>(eq);
			else {
				// squeeze the equalities
				tref l = tt(system.first.value())
					| tau::bf_eq | tau::bf | tt::ref;
				tref r = tt(eq)
					| tau::bf_eq | tau::bf | tt::ref;
				system.first = build_bf_eq<node>(
					(tau::get(l) | tau::get(r)).get());
			}
		}
		else system.second.insert(eq);
	}
	return solve_system<node>(system, options);
}

// entry point for the solver
template <NodeType node>
std::optional<solution<node>> solve(tref form, const solver_options& options) {
	using tau = tree<node>;
	using tt = tau::traverser;
	if (tau::get(form).equals_T()) return { solution<node>() };

	#ifdef DEBUG
	LOG_TRACE
		<< "solver.tmpl.h:" << __LINE__ << " solve/form: " << TAU_TO_STR(form) << "\n"
		<< "solver.tmpl.h:" << __LINE__ << " solve/options/type: " << options.type << "\n";
	switch (options.mode) {
		case solver_mode::maximum: LOG_TRACE
			<< "solver.tmpl.h:" << __LINE__ << " solve/options.kind: maximum\n"; break;
		case solver_mode::minimum: LOG_TRACE
			<< "solver.tmpl.h:" << __LINE__ << " solve/options.kind: minimum"; break;
		default: LOG_TRACE
			<< "solver.tmpl.h:" << __LINE__ << " solve/options.kind: default\n"
			<< "solver.tmpl.h:" << __LINE__ << " solve/options.splitter_one:"
			<< options.splitter_one; break;
	}
	#endif // DEBUG

	tref dnf = tt(form) | bf_reduce_canonical<node>() | tt::ref;
	for (tref clause : get_leaves<node>(dnf, tau::wff_or)) {
		// Reject clause involving temporal quantification
		if (tau::get(clause).find_top(is_temporal_quantifier<node>)) {
			LOG_WARNING << "Skipped clause with temporal quantifier: " << TAU_TO_STR(clause);
			continue;
		}
		auto is_equation = [](tref n) {
			return tau::get(n).child_is(tau::bf_eq)
				|| tau::get(n).child_is(tau::bf_neq);
		};
		// FIXME convert vars to a set
		auto eqs = tau::get(clause).select_top(is_equation);
		if (eqs.empty()) continue;
		auto solution = solve<node>(
			equations<node>(eqs.begin(), eqs.end()), options);
		if (solution.has_value()) return solution;
	}
	return {};
}

} // idni::tau_lang namespace