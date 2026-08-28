// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "splitter.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "solver"

// In what follows we use the algorithms and notations of TABA book (cf.
// Section 3.2). Check (https://github.com/IDNI/tau-lang/blob/main/docs/taba.pdf)
// for the details.

namespace idni::tau_lang {

template <NodeType node>
std::optional<solution<node>> find_solution(equality eq,
	solution<node>& substitution, solver_mode mode)
{
	using tau = tree<node>;
	using tt = tau::traverser;
	// We would use the algorithm subyaccent to the following theorem (of Taba Book):
	//
	// Theorem 3.1. For f (x,X) = xg (X) + x′h (X), let Z be a zero of
	// g (Z) h (Z) (which is guaranteed to exist by Boole’s consistency condition).
	// Then both f (h (Z) ,Z) = 0 and f (g′ (Z) ,Z) = 0.
	// find a variable, say x, in the equality
	DBG(LOG_TRACE << "find_solution/eq: " << LOG_FM(eq);)
	auto has_no_var = [](tref f) {
		return !tau::get(f).find_top(is_child<node, tau::variable>);
	};

	if (!(tt(eq) | tau::bf_eq).has_value()) {
		DBG(LOG_TRACE << "find_solution/solution[no_eq]: {}";)
		return {};
	}

	tref f = tt(eq) | tau::bf_eq | tau::bf | tt::ref;

	DBG(LOG_TRACE << "find_solution/f: " << LOG_FM(f);)

	// FIXME convert vars to a set
	if (trefs vars = tau::get(f)
		.select_top(is_child<node, tau::variable>); vars.size())
	{
		// compute g(X) and h(X) from the equality by substituting x with 0 and 1
		// with x <- h(Z)
		tref g = rewriter::replace<node>(f, vars[0], tau::_1(find_ba_type<node>(vars[0])));
		tref h = rewriter::replace<node>(f, vars[0], tau::_0(find_ba_type<node>(vars[0])));
		tref gh = tt(tau::get(g) & tau::get(h))
			| bf_reduce_canonical<node>() | tt::ref;
#ifdef DEBUG
		LOG_TRACE << "find_solution/var[0]: " << LOG_FM(vars[0]);
		LOG_TRACE << "find_solution/g: "      << LOG_FM(g);
		LOG_TRACE << "find_solution/h: "      << LOG_FM(h);
		LOG_TRACE << "find_solution/gh: "     << LOG_FM(gh);
#endif // DEBUG

		if (has_no_var(gh)) {
			if (!tau::get(gh).equals_0()) {
				DBG(LOG_TRACE << "find_solution"
					<< "/solution[gh_no_var,gh_!=_0]: {}";)
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
				LOG_TRACE << "find_solution"
					<< "/solution[gh_no_var,gh=0]: ";
				for (const auto& [k, v] : substitution)
					LOG_TRACE<<LOG_FM(k)<<" := "<<LOG_FM(v);
#endif // DEBUG

				return substitution;
			}
		}
		if (auto restricted = find_solution<node>(build_bf_eq_0<node>(gh),
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
			LOG_TRACE << "find_solution/substitution[general]: ";
			for (const auto& [k, v] : substitution)
				LOG_TRACE << LOG_FM(k) << " := " << LOG_FM(v);
#endif // DEBUG

			return substitution;
		}
	}

	DBG(LOG_TRACE << "find_solution/substitution[no_var]: {}";)
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

// SO-3: a variable-free equation is not automatically satisfied -- reduce it
// and reject on F instead of reporting an empty solution for e.g. {c} = 0.
template <NodeType node>
bool var_free_holds(tref eq) {
	using tau = tree<node>;
	using tt = tau::traverser;
	tref v = tt(eq) | bf_reduce_canonical<node>() | tt::ref;
	return !tau::get(v).equals_F();
}

template <NodeType node>
std::optional<solution<node>> find_maximal_solution(const equation_system<node>& system) {
	using tau = tree<node>;
	trefs vars = get_variables<node>(system);
	if (vars.empty()) {
		if (system.first && !var_free_holds<node>(system.first.value()))
			return {};
		for (tref neq : system.second)
			if (!var_free_holds<node>(neq)) return {};
		return solution<node>();
	}
	auto substitution = solution<node>();
	for (tref var : vars) substitution[var] = tau::_1(find_ba_type<node>(var));
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
	if (vars.empty()) {
		if (system.first && !var_free_holds<node>(system.first.value()))
			return {};
		for (tref neq : system.second)
			if (!var_free_holds<node>(neq)) return {};
		return solution<node>();
	}
	auto substitution = solution<node>();
	for (tref var : vars) substitution[var] = tau::_0(find_ba_type<node>(var));
	return (system.first)
		? find_solution<node>(system.first.value(), substitution,
							solver_mode::minimum)
		: substitution;
}

template <NodeType node>
std::optional<solution<node>> find_solution(equality eq) {
	using tau = tree<node>;
	trefs vars = get_variables<node>(eq);
	if (vars.empty())
		return var_free_holds<node>(eq)
			? std::optional<solution<node>>{ solution<node>() }
			: std::nullopt;
	auto substitution = solution<node>();
	for (tref var : vars) substitution[var] = tau::_1(find_ba_type<node>(var));
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
		DBG(LOG_TRACE << "lgrs/solution: {}";)
		return solution<node>();
	}

	DBG(LOG_TRACE << "lgrs/eq: " << LOG_FM(eq) << "\n";)

	auto s = find_solution<node>(eq);
	if (!s.has_value()) {
		DBG(LOG_TRACE << "lgrs/no solution";)
		return {};
	}
	tref f = tt(eq) | tau::bf_eq | tau::bf | tt::ref;
	solution<node> phi;
	for (auto [x_i, z_i] : s.value())
		phi[x_i] = tt((tau::get(z_i) & tau::get(f))
			+ (tau::get(x_i) & ~tau::get(f)))
				| bf_reduce_canonical<node>() | tt::ref;

#ifdef DEBUG
	LOG_TRACE << "lgrs/equality: " << LOG_FM(eq);
	LOG_TRACE << "lgrs/solution: ";
	for (auto [k, v] : phi) LOG_TRACE << LOG_FM(k) << " := " << LOG_FM(v);
	tref check = normalizer<node>(rewriter::replace<node>(eq, phi));
	LOG_TRACE << "lgrs/check: " << LOG_FM(check) << "\n";
#endif // DEBUG

	return phi;
}

// SO-2: worst-case enumerates all 2^vars polarity combinations with no
// cutoff or timeout, and solver_options has no budget/deadline field.
// Capping this iterator would silently truncate the enumeration -- since
// callers treat it as exhaustive, that would trade a hang for a wrong
// (incomplete) solve result, so this is left undone pending a real
// tri-state (sat/unsat/unknown) result contract through the solver
// pipeline rather than a bolted-on iteration limit.
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
			tref partial_minterm = tau::_1(find_ba_type<node>(f));
			for (tref v : vars) {
				// we add the current choice to the list of choices...
				partial_minterm = (tau::get(partial_minterm)
					& ~tau::get(v)).get();
				choices.emplace_back(v, false, partial_bf,
							partial_minterm);
				partial_bf = rewriter::replace<node>(partial_bf,
					v, tau::_0(find_ba_type<node>(v)));
				DBG(LOG_TRACE << "minterm_iterator/partial_bf: "
					<< LOG_FM(partial_bf);)
				// ... and compute new values for the next one
			}
			// if the current choices correspond to a proper minterm, we update the current
			// minterm, otherwise we compute the next valid choice
			if (tref minterm = make_current_minterm();
				!tau::get(minterm).equals_0())
			{
				current = minterm;
				DBG(LOG_TRACE << "minterm_iterator/current: "
					<< LOG_FM(current);)
			}
			else make_next_choice();
		// otherwise, i.e. no vars, we return an empty iterator as we have no vars.
		} else exhausted = true;
	}

	minterm_iterator<node>& operator++() {
		if (exhausted) return *this;
		make_next_choice();
		return *this;
	}

	minterm_iterator<node> operator++(int) {
		minterm_iterator tmp = *this;
		++*this;
		return tmp;
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
			? rewriter::replace<node>(choices.back().partial_bf,
				choices.back().var, tau::_1(
					find_ba_type<node>(choices.back().var)))
			: rewriter::replace<node>(choices.back().partial_bf,
				choices.back().var, tau::_0(
					find_ba_type<node>(choices.back().var)));
		tref current = (tau::get(cte) & tau::get(choices.back()
						.partial_minterm)).get();

		DBG(LOG_TRACE << "make_current_minterm/current: "
							<< LOG_FM(current);)

		return current;
	}

	// SO-6: iterative, not self-recursive -- one recursion frame per
	// skipped zero-minterm overflowed the stack in debug builds for
	// formulas with many variables.
	void make_next_choice() {
		while (!exhausted) {
			// update the choices from right to left
			size_t last_changed_value = choices.size();
			while (last_changed_value > 0)
				if (choices[--last_changed_value].value ^= true)
					break;
			// if all choices are exhausted, we are done
			if (last_changed_value == 0
				&& choices[0].value == false) {
				exhausted = true; return;
			}
			// update the choices from the last changed one on
			update_choices_from(last_changed_value);
			// a valid (nonzero) minterm updates current; a zero
			// one loops to try the next choice
			if (tref mt = make_current_minterm();
				!tau::get(mt).equals_0()) {
				current = mt; return;
			}
		}
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
				? rewriter::replace<node>(choices[i - 1].partial_bf,
					choices[i - 1].var, tau::_1(
						find_ba_type<node>(choices[i - 1].var)))
				: rewriter::replace<node>(choices[i - 1].partial_bf,
					choices[i - 1].var, tau::_0(
						find_ba_type<node>(choices[i - 1].var)));
			// if current partial bf is 0, we can skip the rest of the choices
			// as the corresponding minterms will be 0.
			if (tau::get(choices[i].partial_bf).equals_0()) {
				for (size_t j = i + 1; j < choices.size(); ++j) {
					choices[j].partial_bf = tau::_0(find_ba_type<node>(choices[j].partial_bf));
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

	bool empty() { return begin() == end(); }

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

	minterm_inequality_system_iterator<node> &operator++() {
		if (exhausted) return *this;
		make_next_choice();
		return *this;
	}

	minterm_inequality_system_iterator<node> operator++(int) {
		minterm_inequality_system_iterator tmp = *this;
		++*this;
		return tmp;
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
			minterms.insert(build_bf_neq_0<node>(*it));

#ifdef DEBUG
		LOG_TRACE << " make_current_minterm_system/minterms: ";
		for (tref mt : minterms) LOG_TRACE << LOG_FM(mt);
#endif // DEBUG

		return minterms;
	}

	void make_next_choice() {
		if (exhausted) return;
		size_t last_changed_value = minterm_iterators.size();
		while (last_changed_value > 0) {
			--last_changed_value;
			if (++minterm_iterators[last_changed_value]
				== ranges[last_changed_value].end())
					minterm_iterators[last_changed_value]
					   = ranges[last_changed_value].begin();
			else break;
		}
		if (last_changed_value == 0
			&& minterm_iterators[0] == ranges[0].begin())
		{
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
tref get_constant(minterm m, size_t type_id) {
	using tau = tree<node>;
	//auto cte = find_top(m, is_child_non_terminal<tau::ba_constant, node>);
	//return cte ? cte.value() : _1<node>;
	auto is_ba_constant = [](const auto& n) -> bool {
		return is_child<node, tau::ba_constant>(n);
	};
	// FIXME convert vars to a set
	trefs all_vs = tau::get(m).select_top(is_ba_constant);
	return build_bf_and<node>(all_vs, type_id);
}

template <NodeType node>
subtree_set<node> get_exponent(tref n) {
	using tau = tree<node>;
	using tt = tau::traverser;
	auto is_bf_literal = [](tref n) -> bool {
		return (tt(n) | tau::variable).has_value()
			|| (tt(n) | tau::bf_neg | tau::bf | tau::variable)
								.has_value();
	};
	// FIXME convert vars to a set
	trefs all_vs = tau::get(n).select_top(is_bf_literal);
	return subtree_set<node>(all_vs.begin(), all_vs.end());
}

template <NodeType node>
tref get_minterm(minterm m, size_t type_id) {
	return build_bf_and<node>(get_exponent<node>(m), type_id);
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
		const auto& new_m_cte = tau::get(get_constant<node>(new_m, options.type_id));
		auto new_m_exp = get_exponent<node>(new_m);

#ifdef DEBUG
		LOG_TRACE << "add_minterm_to_disjoint/d:"     << LOG_FM(d);
		LOG_TRACE << "add_minterm_to_disjoint/new_m:" << LOG_FM(new_m);
		LOG_TRACE << "add_minterm_to_disjoint/new_m_cte:"
						<< LOG_FM(new_m_cte.get());
		LOG_TRACE << "add_minterm_to_disjoint/new_m_exp:";
		for (tref e : new_m_exp) LOG_TRACE << LOG_FM(e);
#endif // DEBUG

		// case 1
		if (std::ranges::equal(get_exponent<node>(d), new_m_exp, lcrs_tree<node>::subtree_equals)) {
			DBG(LOG_TRACE << "add_minterm_to_disjoint"
				<< "/[case1]/new_disjoint: " << LOG_FM(d);)

			new_disjoint.insert(d);
			continue;
		}
		const auto& d_cte = tau::get(get_constant<node>(d, options.type_id));
		if ((d_cte & new_m_cte) != false) {
			// case 2
			if ((d_cte & ~new_m_cte) != false) {
				const auto& x = ~new_m_cte & tau::get(d);
				new_disjoint.insert(x.get());

				DBG(LOG_TRACE << "add_minterm_to_disjoint"
					<< "/[case2]/new_disjoint: "
					<< LOG_FM(x.get()) << "\n";)

			// case 3
			} else if ((~d_cte & new_m_cte) != false) {
				new_disjoint.insert(d);
				new_m = tt(~d_cte & tau::get(new_m))
					| bf_reduce_canonical<node>() | tt::ref;

				DBG(LOG_TRACE
					<< " add_minterm_to_disjoint/[case3]"
					"/new_disjoint: " << LOG_FM(d)<<"\n"
				 <<" add_minterm_to_disjoint"
					"/[case3]/new_m: "<< LOG_FM(new_m);)

			// case 4
			} else {
				// otherwise, go with the splitters
				DBG(LOG_TRACE << "add_minterm_to_disjoint/"
					<< "[case4]/d_cte: "
					<< LOG_FM(d_cte.get());)

				tref s = d_cte.equals_1()
					// case 4.1
					? options.splitter_one
					// case 4.2
					// TODO (HIGH) replace call to tau_splitter with a
					// node::ba::splitter call and maybe remove the corresponding
					// tau_splitter method
					: tau_splitter(tau::get(tt(d_cte)
						| tau::ba_constant
						| tt::ref)).get();

				// no splitter available (e.g. options.splitter_one
				// unset for case 4.1) or the splitter degenerates to 0:
				// fail explicitly instead of dereferencing a null tref
				// or silently inserting a bogus 0 minterm.
				if (!s || tau::get(s).equals_0()) return {};

				DBG(LOG_TRACE << "add_minterm_to_disjoint"
					<< "/[case4]/s: " << LOG_FM(s) << "\n";)

				const auto& st = tau::get(s);
				new_disjoint.insert((st & tau::get(d)).get());
				new_m = tt(~st & tau::get(new_m))
					| bf_reduce_canonical<node>() | tt::ref;

				DBG(LOG_TRACE << "add_minterm_to_disjoint"
					<< "/[case4]/new_disjoint: "
					<< LOG_FM((st & tau::get(d)).get());)
				DBG(LOG_TRACE << "add_minterm_to_disjoint"
					<< "/[case4]/new_m: "<< LOG_FM(new_m);)
			}
		// case 5
		} else {
			new_disjoint.insert(d);

			DBG(LOG_TRACE << "add_minterm_to_disjoint"
				<< "/[case5]/new_disjoint: " << LOG_FM(d);)
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
	LOG_TRACE << "make_minterm_system_disjoint/system: ";
	for (minterm t : sys) LOG_TRACE << LOG_FM(t);
#endif // DEBUG

	minterm_system<node> disjoints;
	for (auto it = sys.begin(); it != sys.end(); ++it)
		if (auto new_disjoints = add_minterm_to_disjoint<node>(
						disjoints, *it, options);
			new_disjoints) disjoints = new_disjoints.value();
		else return {};

#ifdef DEBUG
	LOG_TRACE << "make_minterm_system_disjoint/disjoints: ";
	for (minterm t : disjoints) LOG_TRACE << LOG_FM(t);
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
	LOG_TRACE << "solve_minterm_system/system: ";
	for (minterm t : system) LOG_TRACE << LOG_FM(t);
#endif // DEBUG

	// We know the system has a solution as we only iterate over non-negative
	// minterms (which trivially satisfy the condition of Theorem 3.3)
	equality eq = tau::_0(options.type_id);
	auto disjoint_minterms = make_minterm_system_disjoint<node>(system, options);
	if (!disjoint_minterms.has_value()) return {};

	for (tref neq : disjoint_minterms.value()) {

		DBG(LOG_TRACE << "solve_minterm_system/neq: " << LOG_FM(neq);)

		tref nf = tt(neq) | tau::bf_neq | tau::bf
			| bf_reduce_canonical<node>() | tt::ref;

		DBG(LOG_TRACE << "solve_minterm_system/nf: " << LOG_FM(nf);)

		if (tau::get(nf).equals_0()) continue;

		tref cte = get_constant<node>(nf, options.type_id);

		DBG(LOG_TRACE << "solve_minterm_system/cte: " << LOG_FM(cte);)

		minterm t = get_minterm<node>(nf, options.type_id);

		DBG(LOG_TRACE << "solve_minterm_system/minterm: " <<LOG_FM(t);)

		eq = (tau::get(eq) | (tau::get(cte) & ~tau::get(t))).get();

		DBG(LOG_TRACE << "solve_minterm_system/eq[partial]: "
								<< LOG_FM(eq);)
	}

	DBG(LOG_TRACE << "solve_minterm_system/eq[final]: " << LOG_FM(eq);)

	eq = build_bf_eq_0<node>(eq);
	return find_solution<node>(eq);
}

// Splitter for a ba_constant coefficient, matching add_minterm_to_disjoint's
// case-4 call: options.splitter_one for the top element, tau_splitter
// otherwise. Re-normalizes first since red_and's AND of two DNF operands
// isn't itself in DNF, which tau_splitter requires.
template <NodeType node>
tref atomless_coefficient_splitter(tref cte, const solver_options& options) {
	using tau = tree<node>;
	using tt = tau::traverser;
	cte = tt(cte) | bf_reduce_canonical<node>() | tt::ref;
	if (tau::get(cte).equals_1()) return options.splitter_one;
	return tau_splitter(tau::get(tt(cte) | tau::ba_constant | tt::ref)).get();
}

// Bad-splitter fallback for a ba_constant coefficient: conjoins a fresh
// uninterpreted constant into cte (tau_splitter with splitter_type::bad).
// get_new_uninterpreted_constant's process-wide per-family floor
// (normalizer.tmpl.h) keeps the minted name globally fresh even though
// tau_splitter's temporal path only scans the clause it injects into.
// The caller (atomless_choose_value) still verifies properness itself.
template <NodeType node>
tref atomless_bad_splitter(tref cte) {
	using tau = tree<node>;
	using tt = tau::traverser;
	cte = tt(cte) | bf_reduce_canonical<node>() | tt::ref;
	return tau_splitter(tau::get(tt(cte) | tau::ba_constant | tt::ref),
		splitter_type::bad).get();
}

// Ledger-backed fast path for a per-coordinate exclusion system (Design A):
// a single variable `var`, every row a plain exclusion `var != v_j`. A
// ledger-tracked v_j needs zero solver decisions: freeness (TABA,
// Homomorphisms and Hemimorphisms) licenses treating any value disjoint
// from the ledger's fresh region as independent of everything committed so
// far; only external v_j (not ledger-tracked) get a real disjointness check.
// Row shape (`var != v_j` has complementary cofactors) is checked
// structurally via hash-consed compare, no solver call.
// Returns nullopt on any doubt at all, so the general path
// (atomless_witness/atomless_choose_value) always has the final word.
template <NodeType node>
std::optional<tref> atomless_choose_value_ledger(
	const std::vector<std::pair<tref, tref>>& cofactors, size_t type,
	const solver_options& options)
{
	using tau = tree<node>;
	using tt = tau::traverser;
	if (!options.ledger || cofactors.empty()) return std::nullopt;

	auto red_and = [&](tref a, tref b) {
		return tt(tau::get(a) & tau::get(b))
			| bf_reduce_canonical<node>() | tt::ref;
	};
	auto red_not = [&](tref a) {
		return tt(~tau::get(a)) | bf_reduce_canonical<node>() | tt::ref;
	};

	trefs external; // category (b): non-ledger, nonzero, non-one targets
	for (const auto& [c0, c1] : cofactors) {
		if (!tau::subtree_equals(red_not(c0), c1))
			return std::nullopt; // not exclusion-shaped
		if (tau::get(c0).equals_0() || tau::get(c0).equals_1())
			continue; // var!=0 / var!=1 are free (nonzero+proper mint below)
		if (options.ledger->is_committed(c0)) {
			// Monotone-scope check: structural only, no solver call --
			// append-only committed history makes this trivially true.
			DBG(if (auto gi = options.ledger->committed_generator(c0)) {
				trefs current;
				current.reserve(options.ledger->committed_index.size());
				for (auto& [k, idx] : options.ledger->committed_index)
					current.push_back(
						options.ledger->generators.at(idx).value);
				options.ledger->consult(*gi, current);
			})
			continue; // category (a): free by freeness
		}
		external.push_back(c0);
	}

	tref region = options.ledger->fresh_region
		? options.ledger->fresh_region->get() : nullptr;
	if (!region) region = tau::_1(type);
	for (tref v : external) region = red_and(region, red_not(v));
	if (tau::get(region).equals_0()) return std::nullopt; // real check

	// atomless_bad_splitter needs a ba_constant child, which the literal-1
	// sentinel (region on this run's very first mint) lacks; use
	// options.splitter_one directly for that case, same as
	// atomless_coefficient_splitter's own precedent.
	tref x = tau::get(region).equals_1()
		? options.splitter_one
		: atomless_bad_splitter<node>(region);
	if (!x || tau::get(x).equals_0()) return std::nullopt; // real check
	if (tau::get(red_and(region, red_not(x))).equals_0())
		return std::nullopt; // not a proper split (see atomless_bad_splitter)

	for (tref v : external) // real checks, category (b) only
		if (!tau::get(red_and(x, v)).equals_0()) return std::nullopt;

	return x;
}

// Whole-system entry point for the fast path above: `gs` (already XOR-
// folded, per solve_inequality_system_atomless) constrains exactly one
// variable `var` -- the shape ocltl_direct_decode_edge always hands in.
// Cofactor computation mirrors atomless_witness's own (rewriter::replace +
// bf_reduce_canonical, syntactic, no solver call); atomless_witness's own
// recursion is not needed since there is nothing left to eliminate after
// `var`.
template <NodeType node>
std::optional<solution<node>> atomless_exclusion_system_ledger(
	const trefs& gs, tref var, const solver_options& options)
{
	using tau = tree<node>;
	using tt = tau::traverser;
	size_t type = find_ba_type<node>(var);

	auto cofactor = [&](tref g, bool value) {
		tref v = value ? tau::_1(type) : tau::_0(type);
		tref r = rewriter::replace<node>(g, var, v);
		return tt(r) | bf_reduce_canonical<node>() | tt::ref;
	};

	std::vector<std::pair<tref, tref>> cofactors;
	cofactors.reserve(gs.size());
	for (tref g : gs)
		cofactors.emplace_back(cofactor(g, false), cofactor(g, true));

	auto x = atomless_choose_value_ledger<node>(cofactors, type, options);
	if (!x) return std::nullopt;

	solution<node> sol;
	sol[var] = *x;
	return sol;
}

// Registers a committed witness with the ledger and shrinks its fresh
// region to exclude it (syntactic only, no solver call): every later mint
// must stay disjoint from everything ever committed, regardless of which
// solver sub-path produced the value.
//
// Roots the new region as an htref (solver_types.h) so it survives GC
// across interpreter steps -- the ledger is invisible to
// interpreter::collect_live_refs's `keep` set, so a plain tref would
// dangle once a sweep runs.
template <NodeType node>
void ledger_commit_witness(fresh_element_ledger& ledger, tref value,
	size_t type)
{
	using tau = tree<node>;
	using tt = tau::traverser;
	ledger.register_committed(value);
	tref region = ledger.fresh_region
		? ledger.fresh_region->get() : nullptr;
	if (!region) region = tau::_1(type);
	tref next = tt(tau::get(region) & ~tau::get(value))
		| bf_reduce_canonical<node>() | tt::ref;
	ledger.fresh_region = tau::geth(next);
}

// Choose a value for `var` making every g_i(var, s') != 0, given ground
// cofactor pairs (c0_i, c1_i), none of which is (0,0) (TABA cor.
// Multivariate-BFs-over). Disjoint-representatives construction: build
// pairwise disjoint nonzero r_i <= b_i (b_i = c1_i if nonzero else c0_i)
// one row at a time, reusing or splitting existing representatives to keep
// them disjoint (an atomless BA always has a further splitter); x is the
// union of the c1-side representatives.
template <NodeType node>
std::optional<tref> atomless_choose_value(
	const std::vector<std::pair<tref, tref>>& cofactors, tref var,
	const solver_options& options)
{
	using tau = tree<node>;
	using tt = tau::traverser;
	size_t type = find_ba_type<node>(var);

	auto red_and = [&](tref a, tref b) {
		return tt(tau::get(a) & tau::get(b))
			| bf_reduce_canonical<node>() | tt::ref;
	};
	auto red_or = [&](tref a, tref b) {
		return tt(tau::get(a) | tau::get(b))
			| bf_reduce_canonical<node>() | tt::ref;
	};
	auto red_not = [&](tref a) {
		return tt(~tau::get(a)) | bf_reduce_canonical<node>() | tt::ref;
	};

	// Per-call memo for red_and(a,b)==0 (containment/overlap): equals_0() is
	// a fresh Tau-SAT call unless already in the global cache, and
	// logically-related pairs here reduce to distinct trefs so that cache
	// misses for them. Keyed on the unordered tref pair, scoped to this
	// call only.
	std::map<std::pair<uintptr_t, uintptr_t>, bool> and_zero_memo;
	auto is_and_zero = [&](tref a, tref b) -> bool {
		// Keyed on the tref bit pattern, not the pointers themselves:
		// comparing unrelated pointers with < is undefined behavior. This
		// key only affects cache lookup speed, not any decision.
		auto ua = reinterpret_cast<uintptr_t>(a);
		auto ub = reinterpret_cast<uintptr_t>(b);
		auto key = ua <= ub ? std::pair(ua, ub) : std::pair(ub, ua);
		if (auto it = and_zero_memo.find(key); it != and_zero_memo.end())
			return it->second;
		bool z = tau::get(red_and(a, b)).equals_0();
		and_zero_memo.emplace(key, z);
		return z;
	};

	std::vector<tref> reps;
	std::vector<bool> is_c1_side;
	reps.reserve(cofactors.size());
	is_c1_side.reserve(cofactors.size());
	tref reps_union = tau::_0(type);

	// Debug-only: unmutated rep -> ledger generator index, for consult().
	[[maybe_unused]] std::map<uintptr_t, size_t> ledger_gen_of;
	// Debug-only: every rep ever pushed, append-only, for consult().
	[[maybe_unused]] trefs exclusion_log;

	// A second globally-fresh splitter, independent of options.splitter_one,
	// computed lazily once and reused across every row of this call: covers
	// the case where a row's coefficient sits entirely on one side of
	// options.splitter_one, making it useless as a splitter for that row.
	tref splitter_two = nullptr;
	bool splitter_two_tried = false;
	auto get_splitter_two = [&]() -> tref {
		if (!splitter_two_tried) {
			splitter_two_tried = true;
			if (options.splitter_one)
				splitter_two = atomless_bad_splitter<node>(options.splitter_one);
		}
		return splitter_two;
	};

	for (const auto& [c0, c1] : cofactors) {
		bool c1_side = !tau::get(c1).equals_0();
		tref b = c1_side ? c1 : c0;
		if (tau::get(b).equals_0()) return {}; // (0,0) row: not satisfiable

		// Reuse an existing same-side representative already inside b: no
		// new disjoint slice, no splitter call, and rows sharing a trivial
		// cofactor (common once other variables' elimination collapses a
		// clause to a constant) collapse to a single split instead of one
		// per row.
		bool reused = false;
		tref not_b = red_not(b);
		for (size_t j = 0; j < reps.size() && !reused; ++j) {
			if (is_c1_side[j] != c1_side) continue;
			if (is_and_zero(reps[j], not_b)) {
				reused = true;
				// Monotone-scope check: reusing a ledger generator here
				// must see an exclusion set superset of its mint-time one.
				DBG(if (auto it = ledger_gen_of.find(
						reinterpret_cast<uintptr_t>(reps[j]));
						it != ledger_gen_of.end())
					options.ledger->consult(it->second, exclusion_log);)
			}
		}
		if (reused) continue;

		tref not_reps_union = red_not(reps_union);
		if (!is_and_zero(b, not_reps_union)) {
			tref outside = red_and(b, not_reps_union);
			// Record: reps (pre-push) is outside's mint-time exclusion set.
			if (options.ledger) {
				// This ledger entry outlives the call (per-run scope), so
				// root it against a later GC sweep (fresh_element_ledger::pin).
				options.ledger->pin(tau::geth(outside));
				[[maybe_unused]] size_t gi
					= options.ledger->mint(outside, reps);
				DBG(ledger_gen_of[reinterpret_cast<uintptr_t>(outside)] = gi;)
			}
			DBG(exclusion_log.push_back(outside);)
			reps.push_back(outside);
			is_c1_side.push_back(c1_side);
			reps_union = red_or(reps_union, outside);
			continue;
		}

		// b is fully covered by the reps chosen so far, so it overlaps one of
		// them: try tau_splitter, options.splitter_one, splitter_two, then
		// the bad splitter, on each overlapping rep in turn, verifying every
		// candidate (nonzero, proper subset of c) before trusting it.
		bool split_done = false;
		for (size_t j = 0; j < reps.size() && !split_done; ++j) {
			if (is_and_zero(b, reps[j])) continue;
			tref c = red_and(b, reps[j]);

			auto try_split = [&](tref s) {
				if (!s || tau::get(s).equals_0()) return false;
				if (is_and_zero(c, red_not(s))) return false;
				// s <= c <= reps[j], so reps_union is unchanged.
				DBG(ledger_gen_of.erase(reinterpret_cast<uintptr_t>(reps[j]));)
				reps[j] = red_and(reps[j], red_not(s));
				// The narrowed reps[j] is itself a rep any later mint must
				// be scoped disjoint from, same as any other pushed rep.
				DBG(exclusion_log.push_back(reps[j]);)
				reps.push_back(s);
				is_c1_side.push_back(c1_side);
				DBG(exclusion_log.push_back(s);)
				return split_done = true;
			};

			if (try_split(atomless_coefficient_splitter<node>(c, options)))
				break;
			if (options.splitter_one) {
				tref one = options.splitter_one;
				if (try_split(red_and(c, one))) break;
				if (try_split(red_and(c, red_not(one)))) break;
			}
			if (tref two = get_splitter_two(); two) {
				if (try_split(red_and(c, two))) break;
				if (try_split(red_and(c, red_not(two)))) break;
			}
			if (try_split(atomless_bad_splitter<node>(c))) break;
		}
		if (!split_done) return {}; // splitter machinery failure
	}

	tref x = tau::_0(type);
	for (size_t i = 0; i < reps.size(); ++i)
		if (is_c1_side[i]) x = red_or(x, reps[i]);

	// Defensive re-check: every row must be satisfied by construction above.
	for (const auto& [c0, c1] : cofactors) {
		if (!is_and_zero(x, c1)) continue;
		if (is_and_zero(red_not(x), c0)) return {};
	}
	return x;
}

// Per-variable elimination witness for a pure atomless inequality system:
// eliminate `var` via g_i(0,.)|g_i(1,.), recurse for a solution s', then
// pick var's value from the ground cofactors. Boole's expansion guarantees
// g_i not identically zero implies the eliminated form isn't either, so no
// per-step re-check is needed.
template <NodeType node>
std::optional<solution<node>> atomless_witness(const trefs& gs,
	const trefs& vars, const solver_options& options)
{
	using tau = tree<node>;
	using tt = tau::traverser;

	if (vars.empty()) return solution<node>{};

	tref var = vars.front();
	trefs rest_vars(vars.begin() + 1, vars.end());
	size_t type = find_ba_type<node>(var);

	auto cofactor = [&](tref g, bool value) {
		tref v = value ? tau::_1(type) : tau::_0(type);
		tref r = rewriter::replace<node>(g, var, v);
		return tt(r) | bf_reduce_canonical<node>() | tt::ref;
	};

	trefs c0s, c1s, gs_elim;
	c0s.reserve(gs.size()); c1s.reserve(gs.size()); gs_elim.reserve(gs.size());
	for (tref g : gs) {
		tref c0 = cofactor(g, false), c1 = cofactor(g, true);
		c0s.push_back(c0); c1s.push_back(c1);
		gs_elim.push_back(tt(tau::get(c0) | tau::get(c1))
			| bf_reduce_canonical<node>() | tt::ref);
	}

	auto rest = atomless_witness<node>(gs_elim, rest_vars, options);
	if (!rest.has_value()) return {};

	std::vector<std::pair<tref, tref>> cofactors;
	cofactors.reserve(gs.size());
	for (size_t i = 0; i < gs.size(); ++i) {
		tref c0 = rewriter::replace<node>(c0s[i], rest.value());
		c0 = tt(c0) | bf_reduce_canonical<node>() | tt::ref;
		tref c1 = rewriter::replace<node>(c1s[i], rest.value());
		c1 = tt(c1) | bf_reduce_canonical<node>() | tt::ref;
		cofactors.emplace_back(c0, c1);
	}

	auto value = atomless_choose_value<node>(cofactors, var, options);
	if (!value.has_value()) return {};

	solution<node> sol = rest.value();
	sol[var] = value.value();
	return sol;
}

// Deterministic ordering key for the atomless path: subtree_set's own
// hash-first order folds in a ba_constant's storage-table index, an
// allocation-order artifact that differs run to run for the same logical
// value, so re-sort by the pretty-printed form instead, which is stable.
// Scoped to the atomless entry point only.
template <NodeType node>
trefs atomless_stable_sort(trefs xs) {
	using tau = tree<node>;
	std::vector<std::pair<std::string, tref>> keyed;
	keyed.reserve(xs.size());
	for (tref x : xs) keyed.emplace_back(tau::get(x).to_str(), x);
	std::ranges::sort(keyed, [](const auto& a, const auto& b) {
		return a.first < b.first;
	});
	trefs sorted;
	sorted.reserve(xs.size());
	for (auto& [key, x] : keyed) sorted.push_back(x);
	return sorted;
}

template <NodeType node>
std::optional<solution<node>> solve_inequality_system_atomless(
	const inequality_system<node>& system, const solver_options& options)
{
	// TABA cor. Multivariate-BFs-over: over an atomless BA, {g_i != 0} has a
	// common nonzero iff no g_i is identically zero -- no product-of-choices
	// witness search, unlike the general minterm-odometer path below.
	using tau = tree<node>;
	using tt = tau::traverser;

	if (system.empty()) return solution<node>{};

	trefs gs;
	gs.reserve(system.size());
	for (tref neq : system) {
		// A caller may hand this as verbatim `l != r`, not canonical
		// `g != 0`: fold to g = l + r (XOR) unless r is already 0.
		tref bf_neq_node = tt(neq) | tau::bf_neq | tt::ref;
		tref l = tau::get(bf_neq_node).first();
		tref r = tau::get(bf_neq_node).second();
		gs.push_back(tau::get(r).equals_0() ? l
			: tt(tau::get(l) + tau::get(r))
				| bf_reduce_canonical<node>() | tt::ref);
	}
	// Row order feeds atomless_witness's cofactor order and hence
	// atomless_choose_value's greedy assignment order: re-sort to a
	// print-stable order, not system's run-dependent hash order.
	gs = atomless_stable_sort<node>(gs);

	// subtree_set is used only to dedup by structural equality; elimination
	// order always comes from the print-stable re-sort below, not this
	// set's run-dependent iteration order. Computed before the zero
	// pre-check so the ledger fast path (needs vars.size()==1) can run ahead.
	trefs vars;
	{
		subtree_set<node> vs;
		for (tref g : gs) {
			trefs found = tau::get(g)
				.select_top(is_child<node, tau::variable>);
			vs.insert(found.begin(), found.end());
		}
		vars.assign(vs.begin(), vs.end());
	}
	// Elimination order (atomless_witness always peels vars.front()) feeds
	// intermediate-expression size directly: fix it to the same print-stable
	// order as the rows, for the same reason.
	vars = atomless_stable_sort<node>(vars);

	// Ledger fast path (Design A): only for the single-variable exclusion
	// shape ocltl_direct_decode_edge hands in; runs ahead of the per-row
	// zero pre-check below, which is provably redundant for that row shape
	// (atomless_choose_value_ledger's own doc). Falls through unchanged on
	// any other shape.
	if (options.ledger && vars.size() == 1)
		if (auto fast = atomless_exclusion_system_ledger<node>(
				gs, vars.front(), options))
			return fast;

	for (tref g : gs)
		if (tau::get(bf_reduced_dnf<node>(g)).equals_0()) {
			DBG(LOG_TRACE << "solve_inequality_system_atomless"
				<< "/unsat[identically_zero]: " << LOG_FM(g);)
			return {};
		}

	auto sol = atomless_witness<node>(gs, vars, options);

#ifdef DEBUG
	if (sol.has_value()) {
		LOG_TRACE << "solve_inequality_system_atomless/solution: ";
		for (const auto& [k, v] : sol.value())
			LOG_TRACE << LOG_FM(k) << " := " << LOG_FM(v);
	} else LOG_TRACE << "solve_inequality_system_atomless/solution: {}";
#endif // DEBUG

	return sol;
}

template <NodeType node>
std::optional<solution<node>> solve_inequality_system(
	const inequality_system<node>& system, const solver_options& options)
{
	if (pack_type_is_atomless<node>(options.type_id))
		return solve_inequality_system_atomless<node>(system, options);

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
	LOG_TRACE << "solve_inequality_system/system: ";
	for (inequality t : system) LOG_TRACE << LOG_FM(t);
	#endif // DEBUG
	// If no inequality is contained, return an empty solution
	if (system.empty()) return solution<node>{};
	for (auto it = minterm_inequality_system_iterator<node>(system);
		it != minterm_inequality_system_iterator<node>::end; ++it)
	{
#ifdef DEBUG
		LOG_TRACE << "solve_inequality_system/minterm system: ";
		for (minterm t : *it) LOG_TRACE << LOG_FM(t);
#endif // DEBUG

		auto solution = solve_minterm_system<node>(*it, options);
		if (solution.has_value()) return solution;
	}

	DBG(LOG_TRACE << "solve_inequality_system/solution: {}";)

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
	if (system.first.has_value())
		LOG_TRACE << "solve_system/eq: "<<LOG_FM(system.first.value());
	if (!system.second.empty()) {
		LOG_TRACE << "solve_system/inequalities: ";
		for (inequality t : system.second) LOG_TRACE << LOG_FM(t);
	}
#endif // DEBUG

	if (!system.first)
		return solve_inequality_system<node>(system.second, options);
	if (system.second.empty())
		return find_solution<node>(system.first.value());

	auto phi = lgrs<node>(system.first.value());
	if (!phi.has_value()) return {};

#ifdef DEBUG
	LOG_TRACE << "solve_system/phi: ";
	for (const auto& [k, v]: phi.value())
		LOG_TRACE << LOG_FM(k) << " := " << LOG_FM(v);
#endif // DEBUG

	inequality_system<node> inequalities;
	// for each inequality g_i we apply the transformation given by lgrs solution
	// of the equality
	for (tref g_i : system.second) {
		auto nphi = phi.value();
		auto ng_i = tt(rewriter::replace<node>(g_i, nphi))
				| bf_reduce_canonical<node>() | tt::ref;
		if (tau::get(ng_i).equals_F()) {
			DBG(LOG_TRACE<<" solve_system/inequality_solution: {}";)

			return {};
		}
		else if (tau::get(ng_i).equals_T()) continue;

		DBG(LOG_TRACE << "solve_system/inequality: " << LOG_FM(ng_i);)

		inequalities.insert(ng_i);
	}

#ifdef DEBUG
	LOG_TRACE << "solve_system/inequalities: ";
	for (inequality t : inequalities) LOG_TRACE << LOG_FM(t);
#endif // DEBUG


	// solve the given system  of inequalities
	auto inequality_solution =
			solve_inequality_system<node>(inequalities, options);
	if (!inequality_solution.has_value()) {
		DBG(LOG_TRACE << "solve_system/inequality_solution: {}";)

		return {};
	}

#ifdef DEBUG
	LOG_TRACE << "solve_system/inequality_solution: ";
	for (auto [k, v]: inequality_solution.value())
		LOG_TRACE << LOG_FM(k) << " := " << LOG_FM(v);
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
		solution[var] =	tt(replace_free_vars_by<node>(
					func_with_neq_assgm, tau::_0_trimmed(
						find_ba_type<node>(func_with_neq_assgm))))
				| bf_reduce_canonical<node>() | tt::ref;
	}

#ifdef DEBUG
	LOG_TRACE << "solve_system/inequality_solution: ";
	for (auto [k, v]: solution)
		LOG_TRACE << LOG_FM(k) << " := " << LOG_FM(v);
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
	if (system.first) LOG_TRACE <<" check_extreme_solution/eq: "
		<< LOG_FM(system.first.value());
	for (inequality t : system.second) LOG_TRACE
		<< " check_extreme_solution/ineq: " << LOG_FM(t);
	for (auto [k, v]: substitution) LOG_TRACE <<
		" check_extreme_solution/substitution: "
		<< LOG_FM(k) << " := " << LOG_FM(v);
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

// Returns true if n is an ordering atom: wff with bf_lt/bf_gt/bf_lteq/bf_gteq
// child, including their negated forms (bf_nlt/bf_ngt/bf_nlteq/bf_ngteq),
// which is how normalization typically renders these comparisons.
template <NodeType node>
bool is_ordering_atom(tref n) {
	using tau = tree<node>;
	const auto& fm = tau::get(n);
	if (!fm.is(tau::wff) || !fm.has_child()) return false;
	auto op = fm[0].value.nt;
	return op == tau::bf_lt || op == tau::bf_gt
	    || op == tau::bf_lteq || op == tau::bf_gteq
	    || op == tau::bf_nlt || op == tau::bf_ngt
	    || op == tau::bf_nlteq || op == tau::bf_ngteq;
}

// omcat_solve_inequality_system dispatcher: asks the BA owning ba_type_id to
// solve a pure ordering system itself. Distinct from pack_solve, which answers
// with the pack's first solver regardless of type.
template <typename Node, typename BA>
static std::optional<solution<Node>> ba_omcat_solve_for(size_t ba_type_id,
	const inequality_system<Node>& sys, const solver_options& opts)
{
	if constexpr (ba_has_descriptor_v<Node, BA>
		&& requires { ba_descriptor<BA, Node>
			::omcat_solve_inequality_system(sys, opts); })
	{
		if (ba_descriptor<BA, Node>::owns_type(ba_type_id))
			return ba_descriptor<BA, Node>
				::omcat_solve_inequality_system(sys, opts);
	}
	return std::nullopt;
}

template <typename Node>
static std::optional<solution<Node>> pack_omcat_solve(size_t ba_type_id,
	const inequality_system<Node>& sys, const solver_options& opts)
{
	return pack_first_owner<Node>([&]<typename BA>()
		-> std::optional<solution<Node>> {
			return ba_omcat_solve_for<Node, BA>(ba_type_id, sys, opts);
		});
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
				system.first = build_bf_eq_0<node>(
					(tau::get(l) | tau::get(r)).get());
			}
		}
		else system.second.insert(eq);
	}
	// For an omcat ordering system, the owning BA's own solver is
	// authoritative: the BA-level solve_inequality_system cannot handle
	// bf_lt/bf_gt atoms. bf_neq atoms are compatible too: the owning BA's
	// quantifier elimination can puncture the satisfying interval at the
	// excluded point (the U/W execution encoding produces exactly this
	// mix, e.g. `{0} < o1[t] && o1[t] != {1/2}`).
	bool has_ordering = false;
	for (tref neq : system.second)
		if (is_ordering_atom<node>(neq)) { has_ordering = true; break; }
	if (has_ordering && !system.first.has_value()
	    && pack_type_is_non_aba_omcat<node>(options.type_id))
	{
		bool dlo_compatible = true;
		for (tref neq : system.second)
			if (!is_ordering_atom<node>(neq)
				&& !tau::get(neq).child_is(tau::bf_neq))
				{ dlo_compatible = false; break; }
		// A failed attempt (e.g. a bf_neq in XOR-encoded rather than
		// comparison form, which the owning BA's QE cannot read) falls
		// through to the solve-then-verify path below.
		if (dlo_compatible)
			if (auto s = pack_omcat_solve<node>(options.type_id,
					system.second, options); s)
				return s;
	}
	// SO-1: a system that still contains an ordering atom cannot be handed
	// to solve_system as-is: check_extreme_solution only rejects on
	// equals_F() after bf_reduce_canonical, so an extreme solution violating
	// the ordering atom passes silently -- and with no equality present,
	// solve_inequality_system's minterm_inequality_system_iterator
	// dereferences an empty traverser on the bf_gt node. Solve the system
	// WITHOUT its ordering atoms, then verify them against the solution:
	// substitution folds a ground singleton comparison to T/F at
	// construction (the owning BA's wff_lt hooks), so anything not
	// evaluating to T -- violated, undecided, or non-singleton -- declines.
	// Callers treat "no solution found" as unsolved, not unsat.
	if (has_ordering) {
		equation_system<node> rest;
		rest.first = system.first;
		inequality_system<node> ords;
		for (tref neq : system.second)
			if (is_ordering_atom<node>(neq)) ords.insert(neq);
			else rest.second.insert(neq);
		auto sol = solve_system<node>(rest, options);
		if (!sol) return {};
		for (tref ord : ords) {
			tref value = tt(rewriter::replace<node>(ord,
					sol.value()))
				| bf_reduce_canonical<node>() | tt::ref;
			if (!tau::get(value).equals_T()) return {};
		}
		return sol;
	}
	return solve_system<node>(system, options);
}

/**
 * @brief Check if adding the assignment var := term will introduce a loop given
 * the previous variable assignments.
 * @tparam node Tree node type
 * @param var_assignments A map that sends each variable to the set of variables reachable by assignments
 * @param var The current variable that is assigned
 * @param term The current term that is assigend
 * @return Whether adding the current variable assignment is valid
 */
template <NodeType node>
bool check_var_assignment(auto& var_assignments, tref var, tref term) {
	using tau = tree<node>;
	// If the variable is already assigned, we cannot add it
	if (var_assignments.contains(var)) return false;
	trefs term_vars = get_free_vars<node>(term);
	// Add bf node in order to align with var and term structure
	for (tref& tv : term_vars) tv = tau::get(tau::bf, tv);
	// Make sure that term does not contain var
	for (tref tv : term_vars) if (tau::get(tv) == tau::get(var))
		return false;
	// SO-2: close the new assignment's reach set transitively FIRST --
	// existing entries' sets are already closed (this function's
	// invariant), so one merge pass suffices. The previous code compared
	// only the term's DIRECT variables against predecessors and
	// propagated only those, which made cycle detection map-order
	// dependent (a chain p -> var -> q -> r -> p was accepted).
	subtree_set<node> term_cv(term_vars.begin(), term_vars.end());
	for (const auto& [v, cv] : var_assignments)
		if (term_cv.contains(v))
			for (tref el : cv) term_cv.insert(el);
	// var reachable from its own term through any chain -> cycle
	if (term_cv.contains(var)) return false;
	// A cycle also arises iff some assigned v reaches var while the term
	// (transitively) reaches v
	for (const auto& [v, cv] : var_assignments)
		if (cv.contains(var) && term_cv.contains(v)) return false;
	// Propagate the CLOSED reach set into every predecessor of var
	for (auto& [v, cv] : var_assignments)
		if (cv.contains(var))
			for (tref el : term_cv) cv.insert(el);
	// Finally add variable assignment
	var_assignments.emplace(var, std::move(term_cv));
	return true;
}

/**
 * @brief Add the assignment var := term to var_assignments while making sure that
 * assigned variables do not appear in any assigned term.
 * @tparam node Tree node type
 * @param var_assignments Map of assignments of terms to variables
 * @param var The new variable that is assigned to term
 * @param term The new term that is assigned to var
 */
template <NodeType node>
void normalize_and_add_assignment(subtree_map<node, tref>& var_assignments, tref var, tref term) {
	using tau = tree<node>;
	// For each variable in term, get replacement from var_assignment
	const trefs& term_vars = get_free_vars<node>(term);
	for (tref tv : term_vars) {
		tv = tau::get(tau::bf, tv);
		if (auto it = var_assignments.find(tv); it != var_assignments.end())
			term = rewriter::replace<node>(term, tv, it->second);
	}
	// Now term is replaced by assignments from var_assignment
	// Update current variable assignments with new term
	for (auto& [_, t] : var_assignments)
		t = rewriter::replace<node>(t, var, term);
	// Add assignment
	DBG(LOG_TRACE << "Adding: " << tau::get(var) << " := " << tau::get(term) << "\n";)
	var_assignments.emplace(var, term);
}

template <NodeType node>
bool has_compound_term_ops(tref f) {
	using tau = tree<node>;
	return tau::get(f).find_top([](tref n) -> bool {
		return is<node, tau::bf_add>(n) || is<node, tau::bf_sub>(n)
			|| is<node, tau::bf_mul>(n) || is<node, tau::bf_div>(n)
			|| is<node, tau::bf_mod>(n) || is<node, tau::bf_shl>(n)
			|| is<node, tau::bf_shr>(n) || is<node, tau::bf_nand>(n)
			|| is<node, tau::bf_nor>(n)  || is<node, tau::bf_xnor>(n)
			// A BA's own simplification may turn arithmetic into bitwise or
			// logical ops or casts; those count as compound too, so
			// conjs_only_pure_equality routes them to the pack solver, not LGRS.
			|| is<node, tau::bf_and>(n) || is<node, tau::bf_or>(n)
			|| is<node, tau::bf_neg>(n) || is<node, tau::bf_xor>(n)
			|| is<node, tau::bf_cast>(n);
	}) != nullptr;
}

template <NodeType node>
bool conjs_only_pure_equality(const subtree_set<node>& conjs) {
	using tau = tree<node>;
	for (tref conj : conjs) {
		if (!tau::get(conj).child_is(tau::bf_eq)) return false;
		if (has_compound_term_ops<node>(conj)) return false;
	}
	return !conjs.empty();
}

// entry point for the solver
template <NodeType node>
std::optional<solution<node>> solve(tref form, solver_options options, bool& error) {
	using tau = tree<node>;
	using tt = tau::traverser;
	error = false;  // Initialize error flag
	if (tau::get(form).equals_T()) return { solution<node>() };
	if (tau::get(form).equals_F()) return {};

#ifdef DEBUG
	LOG_TRACE << "solve/form: " << LOG_FM(form);
	switch (options.mode) {
		case solver_mode::maximum: LOG_TRACE
				<< " solve/options.kind: maximum"; break;
		case solver_mode::minimum: LOG_TRACE
				<< " solve/options.kind: minimum"; break;
		default: LOG_TRACE
			<< " solve/options.kind: default";
			LOG_TRACE << "solve/options.splitter_one:"
				<< options.splitter_one; break;
	}
	// Temporal quantifiers (always/sometimes) may wrap atomic equations
	// and are unwrapped per-conjunct after path splitting (lines 1226+).
	// Only flag truly unsupported temporal operators (U, F, R, W, S, T).
	{
		auto is_unsupported_temporal = [](tref n) {
			const auto& t = tree<node>::get(n);
			return t.is(tau::wff_F) || t.is(tau::wff_U)
				|| t.is(tau::wff_R) || t.is(tau::wff_W)
				|| t.is(tau::wff_S) || t.is(tau::wff_T);
		};
		assert(!tau::get(form).find_top(is_unsupported_temporal));
	}
#endif // DEBUG
	form = normalize_non_temp<node>(form);
	for (tref path : expression_paths<node>(form)) {
		// collect assignments, i.e. variable = expression
		// early to simplify solving

		// Find equations amounting to single variable assignments
		subtree_map<node, tref> var_assignments;
		subtree_map<node, subtree_set<node>> assignment_check;
		auto find_assigment = [&](tref n) {
			if (!is<node, tau::bf_eq>(n)) return true;
			// Do not extract variable assignments from an
			// arithmetic type due to possible quantification
			if (pack_type_has_arith_ops<node>(
					tau::get(n).get_ba_type()))
				return false;
			const tau& n_t = tau::get(n);
			if (n_t[0].child_is(tau::variable)) {
				// First child is a single variable
				tref var = n_t.first();
				tref term = n_t.second();
				if (check_var_assignment<node>(
					assignment_check, var, term))
					normalize_and_add_assignment<node>(
						var_assignments, var, term);
			} else if (n_t[1].child_is(tau::variable)) {
				// Second child is a single variable
				tref var = n_t.second();
				tref term = n_t.first();
				if (check_var_assignment<node>(
					assignment_check, var, term))
					normalize_and_add_assignment<node>(
						var_assignments, var, term);
			}
			return false;
		};
		pre_order<node>(path).visit_unique(
			find_assigment, while_is_formula<node>, identity);
		// Replace found variables with chosen terms
		DBG(LOG_DEBUG << "solve/Path before: " << tau::get(path) << "\n";)
		path = rewriter::replace(path, var_assignments);
		DBG(LOG_DEBUG << "solve/Path after: " << tau::get(path) << "\n";)
		if (tau::get(path).equals_F()) continue;

		// Partition all found atomic equations according to their type
		std::map<size_t, subtree_set<node>> type_partition;
		std::optional<size_t> bv_partition_key;
		// Partition types
		bool path_sat = false;
		for (tref conj : get_cnf_wff_clauses<node>(path)) {
			// If path is T, we skip in order to have empty type_partition
			if (tau::get(conj).equals_T()) {
				path_sat = true;
				continue;
			}
			// Unwrap temporal quantifiers (always/sometimes) to reach the atomic equation
			{
				const auto& t = tau::get(conj);
				if (t.is(tau::wff) && t[0].is(tau::wff_always))
					conj = t[0].first();
				else if (t.is(tau::wff) && t[0].is(tau::wff_sometimes))
					conj = t[0].first();
			}
			if (tau::get(conj).equals_T()) {
				path_sat = true;
				continue;
			}
			if (tau::get(conj).equals_F()) continue;
			size_t type = find_ba_type<node>(conj);
			if (!is_atomic_fm<node>(conj) && !(pack_type_has_arith_ops<node>(type) && is_child_quantifier<node>(conj))) {
				LOG_ERROR << "Found clause containing non-equation: " << TAU_TO_STR(path);
				error = true;
				break;
			}
			if (!pack_type_has_arith_ops<node>(type)) {
				conj = norm_equation<node>(conj);
				conj = apply_all_xor_def<node>(conj);
			} else {
				// Every bitvector width goes into ONE partition: a cast
				// lets the same variable occur in atoms of two widths
				// (`((bv[16]) d:bv[8]) ... && d:bv[8] != 0`), and solving
				// those atoms separately assigned d twice, the second
				// value silently overwriting the first. The bv branch
				// below regroups by width where it still matters (lgrs).
				if (!bv_partition_key) bv_partition_key = type;
				type = bv_partition_key.value();
			}
			if (auto it = type_partition.find(type); it != type_partition.end()) {
				it->second.insert(conj);
			} else type_partition.emplace(type, subtree_set<node>{conj});
		}
		if (error) return {};

		bool theory_sat = false, skip = false;
		solution<node> clause_solution;
		for (auto& [type, conjs] : type_partition) {
			// The options for the solver depend on the equation type
			solver_options op = options;
			tref type_tree = ba_types<node>::type_tree(type);
			op.type_id = get_ba_type_id<node>(type_tree);
			if (pack_type_has_arith_ops<node>(type_tree)) {
				if (conjs_only_pure_equality<node>(conjs)) {
					// Without arithmetic (a cast counts as arithmetic) no variable
					// spans two widths, so each width is an independent Boolean
					// algebra: squeeze and solve via lgrs per width.
					std::map<size_t, std::optional<equality>> squeezed_by_width;
					for (tref raw_eq : conjs) {
						tref conj = norm_equation<node>(raw_eq);
						conj = apply_all_xor_def<node>(conj);
						auto& squeezed = squeezed_by_width[find_ba_type<node>(raw_eq)];
						if (!squeezed.has_value()) {
							squeezed = conj;
						} else {
							tref l = tt(squeezed.value()) | tau::bf_eq | tau::bf | tt::ref;
							tref r = tt(conj)             | tau::bf_eq | tau::bf | tt::ref;
							squeezed = build_bf_eq_0<node>((tau::get(l) | tau::get(r)).get());
						}
					}
					DBG(assert(!squeezed_by_width.empty());)
					for (const auto& [_, squeezed] : squeezed_by_width) {
						DBG(assert(squeezed.has_value());)
						if (auto lgrs_sol = lgrs<node>(squeezed.value())) {
							theory_sat = true;
							for (const auto& [var, value] : lgrs_sol.value())
								clause_solution[var] = value;
						} else { skip = true; break; }
					}
				} else if constexpr (pack_has_arithmetic_theory_v<node>) {
					if (auto theory_solution = pack_solve<node>(tau::build_wff_and(conjs))) {
						theory_sat = true;
						for (const auto& [var, value]: theory_solution.value()) {
							clause_solution[var] = value;
						}
					} else skip = true; // theory part unsolved, skip this clause
				} else skip = true;
			} else {
				op.splitter_one = node::ba::splitter_one(type_tree);
				if (auto solution = solve<node>(conjs, op)) {
					for (const auto& [var, value]: solution.value()) {
						clause_solution[var] = value;
					}
				}
				else skip = true; // if we cannot solve, skip this clause
			}
			if (skip) break;
		}
		if (skip) continue;
		// It can happen that there is no free variable in bitvector formula
		// causing empty solutions which are still sat
		if (!clause_solution.empty() || theory_sat || path_sat) {
			// Add variables defined by assignments to solution
			for (auto& [v, a] : var_assignments) {
				// Apply the found solutions
				const trefs fv_a = get_free_vars<node>(a);
				for (tref fv : fv_a) {
					fv = tau::get(tau::bf, fv);
					// Skip already solved variables
					if (clause_solution.contains(fv)) continue;
					if (options.mode == minimum)
						clause_solution.emplace(fv, tau::_0(
							find_ba_type<node>(fv)));
					else clause_solution.emplace(fv, tau::_1(
						find_ba_type<node>(fv)));
				}
				a = rewriter::replace<node>(a, clause_solution);
				a = bf_reduced_dnf<node>(a);
				clause_solution.emplace(v, a);
			}
			return clause_solution;
		}
	}
	return {};
}

// (SO-7: the trefs overload of solve() was deleted -- zero callers.)



} // namespace idni::tau_lang