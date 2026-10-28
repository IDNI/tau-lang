// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// normal_forms_bf.tmpl.h - BF simplification: syntactic simplification, squeeze_absorb
// Split from normal_forms.tmpl.h for readability.

namespace idni::tau_lang {

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "normal_forms"

/// Cap on the BDD nodes `bf_var_dependence` builds before it answers
/// `unknown`, which leaves the variable to the slower simplifications;
/// 0 = unlimited. Set via `--bf-dependence-max-nodes`, REPL
/// `bfdependencemaxnodes` or `api::set_bf_dependence_max_nodes` (default
/// 65536).
inline size_t bf_dependence_max_nodes = size_t{1} << 16;

/// What bf_var_dependence could establish about a term and a variable.
enum class bf_dependence { unknown, depends, zero, one };

/**
 * @brief Exact, cheap classification of a Boolean function against @p var.
 *
 * Builds a reduced ordered BDD of @p term with @p var as the top BDD variable:
 * the term depends on @p var (as a Boolean function) iff the root tests it,
 * and is constant iff the root is a terminal. Only terms built from 0, 1,
 * variables, ', &, | and ^ are handled. Anything else, or a BDD larger than
 * `bf_dependence_max_nodes`, answers `unknown`.
 * @param term A bf term
 * @param var The variable wrapped in a bf node, as syntactic_variable_simplification
 * substitutes it
 * @return `depends`, `zero` or `one` when proven, `unknown` otherwise
 */
template<NodeType node>
bf_dependence bf_var_dependence(tref term, tref var) {
	using tau = tree<node>;
	const size_t cap = bf_dependence_max_nodes;
	// node 0 is the false terminal, node 1 the true terminal
	struct bdd_node { size_t v, lo, hi; };
	std::vector<bdd_node> nodes{ { SIZE_MAX, 0, 0 }, { SIZE_MAX, 1, 1 } };
	std::map<std::array<size_t, 3>, size_t> unique, memo;
	subtree_map<node, size_t> atoms;
	atoms.emplace(var, 0);
	auto mk = [&](size_t v, size_t lo, size_t hi) -> size_t {
		if (lo == hi) return lo;
		auto [it, added] = unique.emplace(std::array{ v, lo, hi }, nodes.size());
		if (added) nodes.push_back({ v, lo, hi });
		return it->second;
	};
	// op: 0 = and, 1 = or, 2 = xor
	std::function<size_t(size_t, size_t, size_t)> apply =
		[&](size_t op, size_t a, size_t b) -> size_t {
		if (a <= 1 && b <= 1) return op == 0 ? (a & b) : op == 1 ? (a | b) : (a ^ b);
		if (op == 0 && (a == 0 || b == 0)) return 0;
		if (op == 1 && (a == 1 || b == 1)) return 1;
		if (a > b) std::swap(a, b);
		auto key = std::array{ op, a, b };
		if (auto it = memo.find(key); it != memo.end()) return it->second;
		size_t v = std::min(nodes[a].v, nodes[b].v);
		auto lo = [&](size_t x) { return nodes[x].v == v ? nodes[x].lo : x; };
		auto hi = [&](size_t x) { return nodes[x].v == v ? nodes[x].hi : x; };
		size_t l = apply(op, lo(a), lo(b));
		size_t h = apply(op, hi(a), hi(b));
		size_t r = mk(v, l, h);
		memo.emplace(key, r);
		return r;
	};
	bool failed = false;
	std::unordered_map<tref, size_t> done;
	std::function<size_t(tref)> build = [&](tref n) -> size_t {
		if (failed) return 0;
		if (auto it = done.find(n); it != done.end()) return it->second;
		const tau& t = tau::get(n);
		size_t r = 0;
		if (!t.is(tau::bf) || !t.has_child()) return failed = true, 0;
		if (t.child_is(tau::variable)) {
			auto [it, added] = atoms.emplace(n, atoms.size());
			r = mk(it->second, 0, 1);
		} else if (t.child_is(tau::bf_t)) r = 1;
		else if (t.child_is(tau::bf_f)) r = 0;
		else if (t.child_is(tau::bf_neg)) r = apply(2, build(t[0].first()), 1);
		else if (t.child_is(tau::bf_and))
			r = apply(0, build(t[0].first()), build(t[0].second()));
		else if (t.child_is(tau::bf_or))
			r = apply(1, build(t[0].first()), build(t[0].second()));
		else if (t.child_is(tau::bf_xor))
			r = apply(2, build(t[0].first()), build(t[0].second()));
		else return failed = true, 0;
		if (cap && nodes.size() > cap) failed = true;
		done.emplace(n, r);
		return r;
	};
	size_t root = build(term);
	if (failed) return bf_dependence::unknown;
	if (root <= 1) return root ? bf_dependence::one : bf_dependence::zero;
	return nodes[root].v == 0 ? bf_dependence::depends : bf_dependence::unknown;
}

/**
 * @brief The procedure tries to detect, using 0/1 substitutions for the provided
 * variable and syntactic comparison, if the atomic formula is equivalent to T
 * or F or independent of the provided variable.
 * @tparam node Type of tree node
 * @param atomic_fm The atomic formula, ie an equation, to simplify
 * @param var The variable to base the simplifications on
 * @return The simplified atomic formula, or the report of a failed
 * `bf_reduce_canonical`.
 */
template<NodeType node>
result<tref> syntactic_variable_simplification(tref atomic_fm, tref var) {
	using tau = tree<node>;
	result<tref> r;
	DBG(assert(tau::get(var).is(tau::variable));)
#ifdef TAU_CACHE
	using cache_t = std::unordered_map<std::pair<tref, tref>, tref>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	// The key has to be built from the untouched inputs: both parameters are
	// rebound further down (var is wrapped into a bf, atomic_fm is rewritten
	// by gt_gteq_to_lt_lteq and norm_equation), so keying the store on the
	// rewritten values means an input that is not already in canonical form
	// can never hit its own entry.
	const std::pair<tref, tref> key { tau::trim_right_sibling(atomic_fm),
		tau::trim_right_sibling(var) };
	if (auto it = cache.find(key); it != end(cache))
		return r.with_value(it->second);
	// Every non-trivial exit stores through this, in particular the func2 == 0
	// early return below: norm_equation brings every bf_eq/bf_neq to `f (!)= 0`,
	// so that return is the path taken by the vast majority of the calls, and
	// storing only at the end left the cache write-never for them.
	auto memo = [&key, &r](tref v) {
		return r.with_value(cache.emplace(key, v).first->second);
	};
#else
	auto memo = [&r](tref v) { return r.with_value(v); };
#endif // TAU_CACHE
	// Return early if atomic_fm is either T or F
	if (tau::get(atomic_fm).equals_T() || tau::get(atomic_fm).equals_F())
		return r.with_value(atomic_fm);
	LOG_TRACE << "syntactic_variable_simplification/atomic_fm: "
		<< LOG_FM(atomic_fm);
	LOG_TRACE << "syntactic_variable_simplification/var: " << LOG_FM(var);
	var = tau::get(tau::bf, var);
	atomic_fm = gt_gteq_to_lt_lteq<node>(atomic_fm);
	atomic_fm = norm_equation<node>(atomic_fm);
	size_t atm_type = tau::get(atomic_fm)[0].value.get_nt();
	tref func1 = tau::get(atomic_fm)[0].first();
	tref func2 = tau::get(atomic_fm)[0].second();
	// The reduced DNF of a cofactor can be exponential (x1 ^ ... ^ xn), so
	// settle the outcome of the checks below from a BDD when it can. If
	// func1 depends on var, its cofactors differ as Boolean functions: their
	// reduced forms can neither coincide nor both be 0 or 1, so nothing
	// fires. If func1 is constant, both cofactors reduce to that constant.
	const auto dep1 = bf_var_dependence<node>(func1, var);
	if (dep1 == bf_dependence::zero)
		func1 = tau::_0(find_ba_type<node>(func1));
	else if (dep1 == bf_dependence::one)
		func1 = tau::_1(find_ba_type<node>(func1));
	else if (dep1 == bf_dependence::unknown) {
		// Make sure that it works only on Boolean parts by using replace_if
		tref func1_v_0 = rewriter::replace_if<node>(func1, var,
			_0<node>(find_ba_type<node>(var)), while_is_boolean_operation<node>);
		TAU_TRY(func1_v_0, bf_reduce_canonical<node>()(func1_v_0));
		tref func1_v_1 = rewriter::replace_if<node>(func1, var,
			_1<node>(find_ba_type<node>(var)), while_is_boolean_operation<node>);
		TAU_TRY(func1_v_1, bf_reduce_canonical<node>()(func1_v_1));
		// Is func syntactically identically 0
		if (tau::get(func1_v_0).equals_0() && tau::get(func1_v_1).equals_0())
			func1 = tau::_0(find_ba_type<node>(func1));
		// Is func syntactically identically 1
		else if (tau::get(func1_v_0).equals_1() && tau::get(func1_v_1).equals_1())
			func1 = tau::_1(find_ba_type<node>(func1));
		// func is not dependent on var
		else if (tau::get(func1_v_0) == tau::get(func1_v_1) && !contains<node>(func1_v_0, var))
			func1 = func1_v_0;
	}
	if (tau::get(func2).equals_0()) {
		tref res = denorm_equation<node>(
			tau::get(tau::wff, tau::get(atm_type, func1, func2)));
		LOG_TRACE << "syntactic_variable_simplification/res: " << LOG_FM(res);
		return memo(res);
	}
	// Simplify func2. Reached only for bf_lt/bf_lteq atoms: norm_equation
	// zeroes the right-hand side of every bf_eq/bf_neq, which the return above
	// then catches.
	const auto dep2 = bf_var_dependence<node>(func2, var);
	if (dep2 == bf_dependence::zero)
		func2 = tau::_0(find_ba_type<node>(func2));
	else if (dep2 == bf_dependence::one)
		func2 = tau::_1(find_ba_type<node>(func2));
	else if (dep2 == bf_dependence::unknown) {
		tref func2_v_0 = rewriter::replace_if<node>(func2, var,
			_0<node>(find_ba_type<node>(var)), while_is_boolean_operation<node>);
		TAU_TRY(func2_v_0, bf_reduce_canonical<node>()(func2_v_0));
		tref func2_v_1 = rewriter::replace_if<node>(func2, var,
			_1<node>(find_ba_type<node>(var)), while_is_boolean_operation<node>);
		TAU_TRY(func2_v_1, bf_reduce_canonical<node>()(func2_v_1));
		// Is func syntactically identically 0
		if (tau::get(func2_v_0).equals_0() && tau::get(func2_v_1).equals_0())
			func2 = tau::_0(find_ba_type<node>(func2));
		// Is func syntactically identically 1
		else if (tau::get(func2_v_0).equals_1() && tau::get(func2_v_1).equals_1())
			func2 = tau::_1(find_ba_type<node>(func2));
		// func is not dependent on var
		else if (tau::get(func2_v_0) == tau::get(func2_v_1) && !contains<node>(func2_v_0, var))
			func2 = func2_v_0;
	}
	tref res = tau::get(tau::wff, tau::get(atm_type, func1, func2));
	LOG_TRACE << "syntactic_variable_simplification/res: " << LOG_FM(res);
	return memo(res);
}

/**
 * @brief Function to apply syntactical simplifications to formula in almost
 * linear time in the formula size and the number of paths found in terms
 * @tparam node tree node type
 * @param formula The formula to simplify
 * @return The simplified formula, or the report of a failed
 * `simplify_using_equality`.
 *
 * @note This pass takes no `skip` predicate: neither
 * `simplify_using_equality` nor `syntactic_path_simplification` has a
 * BV-specific check to guard, so a caller that skips bitvector content
 * elsewhere in the pipeline must not assume this step leaves it alone.
 */
template<NodeType node>
result<tref> syntactic_formula_simplification(tref formula) {
	result<tref> r;
	TAU_TRY(formula, simplify_using_equality<node>(formula));
	return r.with_value(syntactic_path_simplification<node>(formula));
}

/**
 * @brief Comparator for the BDD variable order used during simplification of
 * terms. It is used with std::stable_sort in order to preserve initial order
 * in the found BDD variables.
 * @tparam node Type of tree node
 */
template<NodeType node>
auto variable_order_for_simplification = [](tref l, tref r) {
	// Order is: smaller time point first, if equal
	// constant input < constant output < input < output < other variable
	// while all variables are ordered by var_order
	using tau = tree<node>;
	DBG(assert(tau::get(l).is(tau::variable));)
	DBG(assert(tau::get(r).is(tau::variable));)
	// Reject equal
	if (tau::get(l) == tau::get(r)) return false;
	// Non-io variables form a single class ordered *after* every io variable,
	// as the comment above states ("... < input < output < other variable").
	// Keep it so: it makes the relation a strict weak ordering, which
	// std::ranges::stable_sort requires for a specified BDD variable order
	// (and hence normal form).
	if (!is_io_var<node>(l)) return false;
	if (!is_io_var<node>(r)) return true;
	if (is_io_var<node>(l)) {
		// Check if r is also stream
		if (is_io_var<node>(r)) {
			if (is_io_initial<node>(l)) {
				if (is_io_initial<node>(r)) {
					// Both are initial
					const int_t tl = get_io_time_point<node>(l),
						tr = get_io_time_point<node>(r);
					if (tl < tr) {
						// l is earlier
						return true;
					} else if (tl == tr) {
						// Check input < output
						if (tau::get(l).is_input_variable()) {
							if (tau::get(r).is_input_variable()) {
								// Both input -> compare equal
								return false;
							} else return true;
						} if (tau::get(r).is_input_variable()) {
							// Same time step, r is input and l is not
							return false;
						} else return false;
					} else return false;
				} else return true;
			} else if (is_io_initial<node>(r)) {
				// r initial but l not
				return false;
			} else {
				// Both are non initial
				const int_t sl = get_io_var_shift<node>(l),
					sr = get_io_var_shift<node>(r);
				if (sl > sr) {
					// l is earlier
					return true;
				} else if (sl == sr) {
					// Check input < output
					if (tau::get(l).is_input_variable()) {
						if (tau::get(r).is_input_variable()) {
							// Both input -> compare equal
							return false;
						} else return true;
					} if (tau::get(r).is_input_variable()) {
						// Same time step, r is input and l is not
						return false;
					} else return false;
				} else return false;
			}
		} else return false; // unreachable: handled by the guard above
	} else return false; // unreachable: handled by the guard above
};

/**
 * @brief Comparator for the BDD variable order used during simplification of a
 * formula. It is used with std::stable_sort in order to preserve initial order
 * in the found BDD variables.
 * @tparam node Type of tree node
 */
template<NodeType node>
auto atm_formula_order_for_simplification = [](tref l, tref r) {
	// Order is decided by three factors
	// 1) lowest time points in free variables have priority, then
	// 2) lowest highest time points in free variables and last
	// 3) number of free io variables
	// boole_normal_form's is_atomic admits bf_eq, bf_lt and bf_lteq, so all
	// three reach this comparator (a bv `<` is not expanded by the
	// construction hooks and survives intact). Ordering only reads free
	// variables, which every atom kind has.
	DBG(assert(is_atomic_bdd_var<node>(l));)
	DBG(assert(is_atomic_bdd_var<node>(r));)
	// For l
	std::pair<bool, int_t> low_t_l {true, 0}, high_t_l {true, 0};
	bool is_high_init = false;
	const trefs& free_vars_l = get_free_vars<node>(l);
	size_t io_free_var_size_l = 0;
	for (tref v : free_vars_l) {
		if (!is_io_var<node>(v)) continue;
		++io_free_var_size_l;
		if (is_io_initial<node>(v)) {
			const int_t t = get_io_time_point<node>(v);
			if (low_t_l.second >= t) {
				low_t_l.first = false;
				low_t_l.second = t;
			}
			if (high_t_l.second <= t || !is_high_init) {
				high_t_l.first = false;
				high_t_l.second = t;
				is_high_init = true;
			}
		} else {
			if (low_t_l.first)
				if (const int_t t = get_io_var_shift<node>(v);
					low_t_l.second < t)
					low_t_l.second = t;
			 if (high_t_l.first)
				if (const int_t t = get_io_var_shift<node>(v);
					high_t_l.second > t || !is_high_init) {
					high_t_l.second = t;
					is_high_init = true;
				}
		}
	}
	// For r
	std::pair<bool, int_t> low_t_r {true, 0}, high_t_r {true, 0};
	is_high_init = false;
	const trefs& free_vars_r = get_free_vars<node>(r);
	size_t io_free_var_size_r = 0;
	for (tref v : free_vars_r) {
		if (!is_io_var<node>(v)) continue;
		++io_free_var_size_r;
		if (is_io_initial<node>(v)) {
			const int_t t = get_io_time_point<node>(v);
			if (low_t_r.second >= t) {
				low_t_r.first = false;
				low_t_r.second = t;
			}
			if (high_t_r.second <= t || !is_high_init) {
				high_t_r.first = false;
				high_t_r.second = t;
				is_high_init = true;
			}
		} else {
			if (low_t_r.first)
				if (const int_t t = get_io_var_shift<node>(v);
					low_t_r.second < t)
					low_t_r.second = t;
			if (high_t_r.first)
				if (const int_t t = get_io_var_shift<node>(v);
					high_t_r.second > t || !is_high_init) {
					high_t_r.second = t;
					is_high_init = true;
				}
		}
	}
	// Check that both have io variables
	if (io_free_var_size_l == 0)
		return false;
	if (io_free_var_size_r == 0) {
		return true;
	}
	// l and r both have at least one io variable
	if (low_t_l.first < low_t_r.first) return true;
	if (low_t_r.first < low_t_l.first) return false;
	// Here low_t_l.first == low_t_r.first
	if (low_t_l.first) {
		// No initial condition, so higher lookback wins
		if (low_t_l.second > low_t_r.second) return true;
		if (low_t_r.second > low_t_l.second) return false;
		// Here lookback is same, compare highest time point
		// We also know that no initial condition is present
		if (high_t_l.second > high_t_r.second) return true;
		if (high_t_r.second > high_t_l.second) return false;
		// Here lookback is also same
		return io_free_var_size_l < io_free_var_size_r;
	} else {
		// Compare initial conditions
		if (low_t_l.second < low_t_r.second) return true;
		if (low_t_r.second < low_t_l.second) return false;
		// Smallest initials are same, compare highest, also initial conditions by assumption
		if (high_t_l.second < high_t_r.second) return true;
		if (high_t_r.second < high_t_l.second) return false;
		// Here highest initials are same
		return io_free_var_size_l < io_free_var_size_r;
	}
};

/**
 * @brief Comparator for the BDD variable order used during anti-prenex algorithm.
 *
 * Atoms whose deepest quantified variable is quantified deepest come first,
 * then equational assignments, then the larger shallowest quantifier depth;
 * ties break on the sorted printed names of the free variables and then on
 * the printed atom.
 * @tparam node Type of tree node
 * @param quant_pattern Map from each quantified variable to its quantifier
 * depth; a variable not in it counts as depth 0. Captured by reference, so it
 * must outlive the comparator.
 * @return A strict weak ordering `bool(tref, tref)` over atoms.
 */
template<NodeType node>
auto atm_formula_order_for_quant_elim(auto& quant_pattern) {
	auto comp = [&quant_pattern](tref l, tref r) {
		// Assignments to variables
		const bool is_assignment_l = is_equational_assignment<node>(l);
		const bool is_assignment_r = is_equational_assignment<node>(r);

		const trefs& free_vars_l = get_free_vars<node>(l);
		int_t min_l = 0, max_l = 0;
		bool is_min_init = false;
		for (tref v : free_vars_l) {
			auto it = quant_pattern.find(v);
			// Not contained, means not quantified
			if (it == quant_pattern.end()) {
				min_l = 0;
				is_min_init = true;
				continue;
			}
			if (it->second < min_l || !is_min_init)
				min_l = it->second, is_min_init = true;
			if (max_l < it->second) max_l = it->second;
		}
		const trefs& free_vars_r = get_free_vars<node>(r);
		int_t min_r = 0, max_r = 0;
		is_min_init = false;
		for (tref v : free_vars_r) {
			auto it = quant_pattern.find(v);
			// Not contained, means not quantified
			if (it == quant_pattern.end()) {
				min_r = 0;
				is_min_init = true;
				continue;
			}
			if (it->second < min_r || !is_min_init)
				min_r = it->second, is_min_init = true;
			if (max_r < it->second) max_r = it->second;
		}
		if (max_l > max_r) return true;
		if (max_r > max_l) return false;
		// Order assignments to front
		if (is_assignment_l && !is_assignment_r) return true;
		if (!is_assignment_l && is_assignment_r) return false;
		if (min_l > min_r) return true;
		if (min_r > min_l) return false;
		// Grammar-regeneration-stable tie-breaks. subtree_less compares
		// node values, which embed parser nonterminal NUMBERS: a
		// regeneration of the parser renumbers them and silently
		// reorders every id-based tie, and with it the
		// Boole-decomposition pivot choice, which can turn into a hang
		// (GitHub #70). Ties therefore break on PRINTED
		// form — surface syntax is the one ordering a regeneration
		// cannot change. Variable sets compare as name-sorted sequences
		// so the verdict is also independent of get_free_vars's internal
		// (subtree_less-sorted) vector order.
		auto var_names = [](const trefs& vs) {
			std::vector<std::string> ns;
			ns.reserve(vs.size());
			for (tref v : vs)
				ns.push_back(tree<node>::get(v).to_str());
			std::sort(ns.begin(), ns.end());
			return ns;
		};
		const auto ns_l = var_names(free_vars_l);
		const auto ns_r = var_names(free_vars_r);
		if (ns_l < ns_r) return true;
		if (ns_r < ns_l) return false;
		const std::string str_l = tree<node>::get(l).to_str();
		const std::string str_r = tree<node>::get(r).to_str();
		if (str_l < str_r) return true;
		if (str_r < str_l) return false;
		// Identically printed but physically distinct atoms: any pick is
		// equivalent for cost; subtree_less only keeps the order strict.
		return tree<node>::subtree_less(l, r);
	};
	return comp;
}

/**
 * @brief Applies syntactic simplifications to an atomic formula, ie an equation.
 * @tparam node Tree node type
 * @param atomic_formula Formula to simplify
 * @return Simplified formula, or the report of a failed
 * `syntactic_variable_simplification`.
 */
template<NodeType node>
result<tref> syntactic_atomic_formula_simplification(tref atomic_formula) {
	using tau = tree<node>;
	result<tref> r;
	LOG_TRACE << "syntactic_atomic_formula_simplification/atomic_formula: "
		<< LOG_FM(atomic_formula);
	size_t atm_type = tau::get(atomic_formula)[0].value.get_nt();
	// Bring an equation to !(=) 0
	atomic_formula = norm_equation<node>(atomic_formula);
	if (tau::get(atomic_formula).equals_T() ||
		tau::get(atomic_formula).equals_F())
		return r.with_value(atomic_formula);
	tref func1 = syntactic_path_simplification<node>(
		tau::get(atomic_formula)[0].first());
	tref func2 = syntactic_path_simplification<node>(
		tau::get(atomic_formula)[0].second());
	// Apply syntactic path simplification
	atomic_formula = tau::get(tau::wff,
		tau::get(atm_type, func1, func2));
	// Bring an equation back to its original form
	atomic_formula = denorm_equation<node>(atomic_formula);
	// Apply syntactic variable simplification for each found free variable
	auto& free_vars = get_free_vars<node>(atomic_formula);
	for (tref v : free_vars) {
		TAU_TRY(atomic_formula,
			syntactic_variable_simplification<node>(atomic_formula, v));
	}
	LOG_TRACE << "syntactic_atomic_formula_simplification/atomic_formula: "
		<< LOG_FM(atomic_formula);
	return r.with_value(atomic_formula);
}

/**
 * @brief Squeeze two equations of the same polarity, `f = 0` and `g = 0`
 * (or `f != 0` and `g != 0`), into one: `f | g = 0` (resp. `f | g != 0`).
 * @param eq1 First equation, normalized to a zero right-hand side.
 * @param eq2 Second equation, of the same kind as @p eq1.
 * @return The squeezed equation; nullptr when @p eq1 is neither kind.
 */
template <NodeType node>
tref squeeze(tref eq1, tref eq2) {
	using tau = tree<node>;
	// Both equations must be !(=) 0
	DBG(assert(tau::get(eq1)[0][1].equals_0() &&
		tau::get(eq2)[0][1].equals_0());)
	if (tau::get(eq1).child_is(tau::bf_eq)) {
		DBG(assert(tau::get(eq2).child_is(tau::bf_eq));)
		// Squeeze positive equations
		return tau::build_bf_eq_0(
			tau::build_bf_or(tau::trim2(eq1), tau::trim2(eq2)));
	} else if (tau::get(eq1).child_is(tau::bf_neq)) {
		DBG(assert(tau::get(eq2).child_is(tau::bf_neq));)
		// Squeeze negative equations
		return tau::build_bf_neq_0(
		tau::build_bf_or(tau::trim2(eq1), tau::trim2(eq2)));
	}
	DBG(assert(false);)
	return nullptr;
}

/**
 * @brief Comparison keys used by the no-var squeeze_absorb overload to
 * decide which "= 0"/"!= 0" conjuncts/disjuncts are related to one another.
 *
 * squeeze_absorb groups clauses by shared free variables via
 * get_free_vars(). Purely self-referential/uninterpreted atoms built from
 * constant arguments (e.g. `f(0,0)`) contain no plain variable leaves, so
 * get_free_vars() returns empty for them and squeeze_absorb would otherwise
 * be unable to relate such atoms to one another at all -- even when they
 * are the exact same reference. Fall back to the atom's own top-level
 * bf_ref occurrences as comparison keys in that case.
 * @param n The atom.
 * @return Its free variables, or, when it has none, its top-level bf_ref
 * occurrences (sorted, without duplicates).
 */
template <NodeType node>
trefs get_free_vars_or_refs(tref n) {
	using tau = tree<node>;
	const trefs& fv = get_free_vars<node>(n);
	if (!fv.empty()) return fv;
	subtree_set<node> refs;
	for (tref r : tau::get(n).select_top(is<node, tau::bf_ref>))
		refs.insert(r);
	return trefs(refs.begin(), refs.end());
}

/**
 * @brief Absorbs the assumption @p assm into @p eq by the rule table of
 * `squeeze_absorb`: an equation of the scope's own polarity (`= 0` in a
 * conjunction's scope, `!= 0` in the dual one) becomes `f | A`, one of the
 * other polarity `f & A'`.
 * @param eq Equation `f = 0` or `f != 0`.
 * @param assm Assumption term `A`.
 * @param is_eq_pos True when @p eq is `f = 0`.
 * @param dual True for the dual scope.
 * @return The rewritten equation.
 */
template <NodeType node>
tref absorb_assm(tref eq, tref assm, bool is_eq_pos, bool dual) {
	using tau = tree<node>;
	tref f = tau::trim2(eq);
	f = is_eq_pos == dual ? tau::build_bf_and(f, tau::build_bf_neg(assm))
		: tau::build_bf_or(f, assm);
	return is_eq_pos ? tau::build_bf_eq_0(f) : tau::build_bf_neq_0(f);
}

/**
 * @brief Absorbs the assumptions of the scope @p A into @p eq and, when
 * @p joins is given, records which assumptions @p eq joins.
 *
 * An assumption applies when it shares at least two free variables (or
 * bf_refs) with @p eq, or @p eq has only one and shares it. An equation of
 * the scope's own polarity that applies an assumption is merged with it in
 * @p joins, and so is a positive equation sharing a single variable; one that
 * joins nothing is appended to @p additions.
 * @param eq Equation `f = 0` or `f != 0`.
 * @param A The assumptions in scope.
 * @param dual True when the scope is the dual one (a disjunction's `!= 0`
 * assumptions instead of a conjunction's `= 0` ones).
 * @param joins Union-find over assumption terms, updated with the merges;
 * none by default.
 * @param additions Receives `f` of an equation of the scope's own polarity
 * that joined no assumption; given with @p joins.
 * @return The rewritten equation.
 */
template <NodeType node, typename Joins = std::nullptr_t>
tref apply_assms(tref eq, const trefs& A, bool dual = false,
	Joins* joins = nullptr, trefs* additions = nullptr)
{
	using tau = tree<node>;
	constexpr bool record = !std::is_same_v<Joins, std::nullptr_t>;
	DBG(assert(tau::get(eq)[0][1].equals_0());)
	const trefs fv = get_free_vars_or_refs<node>(eq);
	DBG(assert(tau::get(eq).child_is(tau::bf_eq) ||
		tau::get(eq).child_is(tau::bf_neq));)
	const bool is_eq_pos = tau::get(eq).child_is(tau::bf_eq);
	[[maybe_unused]] bool joined = false;
	for (tref assm : A) {
		// Check for overlap between current assumption and eq
		const trefs fv_a = get_free_vars_or_refs<node>(assm);
		if (const int_t count = get_ordered_overlap<node>(fv, fv_a); count > 1 ||
			(count == 1 && fv.size() == 1)) {
			if constexpr (record) if (is_eq_pos != dual) {
				joins->merge(assm, tau::trim2(eq));
				joined = true;
			}
			eq = absorb_assm<node>(eq, assm, is_eq_pos, dual);
		} else if (count == 1 && is_eq_pos) {
			if constexpr (record) {
				joins->merge(assm, tau::trim2(eq));
				joined = true;
			}
		}
	}
	if constexpr (record) if (!joined && is_eq_pos != dual)
		additions->emplace_back(tau::trim2(eq));
	return eq;
}

/**
 * @brief Absorbs the single assumption @p assm into @p eq by the rule table
 * of `squeeze_absorb(formula, var)`, when @p eq contains @p var and has only
 * one free variable or shares at least two with @p assm.
 * @param eq Equation `f = 0` or `f != 0`.
 * @param assm Assumption term.
 * @param var The shared variable.
 * @param updates When not null, receives @p eq (before rewriting) when it
 * contains @p var and has the scope's own polarity: the equations that feed
 * the next assumption.
 * @param dual True for the dual scope.
 * @return The rewritten equation, or @p eq unchanged.
 */
template <NodeType node>
tref apply_assm_on_var(tref eq, tref assm, tref var, trefs* updates,
	const bool dual)
{
	using tau = tree<node>;
	DBG(assert(tau::get(eq)[0][1].equals_0());)
	const trefs& fv = get_free_vars<node>(eq);
	if (!std::binary_search(fv.begin(), fv.end(), var, tau::subtree_less))
		return eq;
	DBG(assert(tau::get(eq).child_is(tau::bf_eq) ||
		tau::get(eq).child_is(tau::bf_neq));)
	const bool is_eq_pos = tau::get(eq).child_is(tau::bf_eq);
	if (updates && is_eq_pos != dual) updates->emplace_back(eq);
	// Check for overlap between current assumption and eq
	const trefs& fv_a = get_free_vars<node>(assm);
	if (fv.size() == 1 || is_ordered_overlap_at_least<node>(2,fv, fv_a))
		eq = absorb_assm<node>(eq, assm, is_eq_pos, dual);
	return eq;
}

/**
 * @brief Replaces each joined class of @p A by the disjunction of its members
 * (from @p joins), dropping the other members, then appends @p additions.
 * @param A The assumptions in scope, rewritten.
 * @param joins Union-find over assumption terms.
 * @param additions Terms to add as new assumptions.
 */
template <NodeType node>
void update_assms(trefs& A, auto& joins, trefs& additions) {
	using tau = tree<node>;
	subtree_set<node> excluded;
	for (size_t i = 0; i < A.size(); ++i) {
		if (excluded.contains(A[i])) {
			// The loop bound keeps i below A.size().
			erase_at(A, i);
			DBG(assert(i > 0);)
			--i;
			continue;
		}
		// Get the set of joinable assumptions from joins
		trefs col = joins.get_set(A[i]);
		for (tref c : col) excluded.insert(c);
		A[i] = tau::build_bf_or(col, find_ba_type<node>(A[i]));
	}
	for (tref add : additions) {
		A.emplace_back(add);
	}
}

namespace squeeze_detail {

/// The policy of `squeeze_absorb(formula)`: a scope holds a set of
/// assumptions, and two equations are related when a chain of `= 0` (or,
/// dually, `!= 0`) equations of the clause set links their variables.
template <NodeType node>
struct over_all_vars {
	using tau = tree<node>;
	using scope = trefs;
	struct less { bool operator()(tref l, tref r) const {
		return tau::subtree_less(l, r); } };
	less comp;
	union_find_with_sets<less, node> uf{ comp };
	trefs additions;

	scope initial() const { return {}; }
	// A fresh union-find per clause set: clear() would walk every bucket
	// the largest clause set left allocated.
	void begin() { uf = union_find_with_sets<less, node>(comp); }
	// Links the variables of an equation of the scope's own polarity.
	void note(tref eq) {
		const trefs fv = get_free_vars_or_refs<node>(eq);
		for (tref v : fv) uf.merge(fv[0], v);
	}
	tref key(tref eq) const {
		const trefs fv = get_free_vars_or_refs<node>(eq);
		return fv.empty() ? nullptr : fv[0];
	}
	bool related(tref k1, tref k2) { return uf.connected(k1, k2); }
	tref apply(tref eq, const scope& A, bool dual) const {
		return apply_assms<node>(eq, A, dual);
	}
	void begin_update() { uf.clear(); additions.clear(); }
	tref apply_update(tref eq, const scope& A, bool dual) {
		return apply_assms<node>(eq, A, dual, &uf, &additions);
	}
	void end_update(scope& A) { update_assms<node>(A, uf, additions); }
};

/// The policy of `squeeze_absorb(formula, var)`: a scope holds one
/// assumption term, and the equations containing `var` are related.
template <NodeType node>
struct over_var {
	using tau = tree<node>;
	using scope = tref;
	tref var;
	trefs updates;

	scope initial() const { return tau::_0(find_ba_type<node>(var)); }
	void begin() {}
	void note(tref) {}
	tref key(tref eq) const {
		const trefs& fv = get_free_vars<node>(eq);
		return std::binary_search(fv.begin(), fv.end(), var,
			tau::subtree_less) ? var : nullptr;
	}
	bool related(tref, tref) const { return true; }
	tref apply(tref eq, const scope& A, bool dual) const {
		return apply_assm_on_var<node>(eq, A, var, nullptr, dual);
	}
	void begin_update() { updates.clear(); }
	tref apply_update(tref eq, const scope& A, bool dual) {
		return apply_assm_on_var<node>(eq, A, var, &updates, dual);
	}
	void end_update(scope& A) {
		for (tref upd : updates) A = tau::build_bf_or(A, tau::trim2(upd));
	}
};

/**
 * @brief The traversal of both `squeeze_absorb` overloads, over the policy
 * @p p (`over_all_vars` or `over_var`), which decides what a scope holds and
 * which equations are related.
 */
template <NodeType node, typename P>
tref squeeze_absorb_impl(tref formula, P& p) {
	using tau = tree<node>;
	// [0]: the conjunctions' `= 0` assumptions, [1]: the disjunctions'
	// `!= 0` ones.
	std::vector<typename P::scope> assms[2] = {
		{ p.initial() }, { p.initial() } };
	subtree_unordered_set<node> mark;
	// Squeezes and absorbs the clauses @p cl of @p n, a conjunction when
	// d is false, a disjunction when d is true.
	auto squeeze_clauses = [&](tref n, trefs& cl, bool d) {
		const size_t own = d ? tau::bf_neq : tau::bf_eq;
		const size_t other = d ? tau::bf_eq : tau::bf_neq;
		// Sort the (!)= 0 equations in cl up front
		size_t eq_idx = 0;
		p.begin();
		for (tref& c : cl) {
			const tau& ct = tau::get(c);
			const bool is_own = ct.child_is(own);
			if (!is_own && !ct.child_is(other)) continue;
			if (!ct[0][1].equals_0()) continue;
			if (is_own) p.note(c);
			std::swap(c, cl[eq_idx++]);
		}
		// Return early if no equation present
		if (eq_idx == 0) return n;
		// Apply the other scope's assumptions to the equations
		for (size_t i = 0; i < eq_idx; ++i)
			cl[i] = p.apply(cl[i], assms[!d].back(), !d);
		// Sort the equations of the scope's own polarity up front
		size_t own_idx = 0;
		for (size_t i = 0; i < eq_idx; ++i) {
			if (tau::get(cl[i]).child_is(own))
				std::swap(cl[i], cl[own_idx++]);
		}
		// Squeeze those that are related
		for (size_t i = 0; i+1 < own_idx; ++i) {
			const tref k1 = p.key(cl[i]);
			if (!k1) continue;
			for (size_t j = i+1; j < own_idx; ++j) {
				const tref k2 = p.key(cl[j]);
				if (!k2 || !p.related(k1, k2)) continue;
				cl[i] = squeeze<node>(cl[i], cl[j]);
				// j is below own_idx, itself below cl.size().
				erase_at(cl, j);
				--j;
				--own_idx;
				--eq_idx;
			}
		}
		// Apply the scope's assumptions to them while updating the
		// assumptions with them
		p.begin_update();
		for (size_t i = 0; i < own_idx; ++i)
			cl[i] = p.apply_update(cl[i], assms[d].back(), d);
		p.end_update(assms[d].back());
		// Apply the updated assumptions to the other equations
		for (size_t i = own_idx; i < eq_idx; ++i)
			cl[i] = p.apply(cl[i], assms[d].back(), d);
		// Build result node
		auto join = [d](tref a, tref b) {
			return d ? tau::build_wff_or(a, b) : tau::build_wff_and(a, b);
		};
		n = cl[0];
		for (size_t i = 1; i < eq_idx; ++i) n = join(n, cl[i]);
		// Exclude equations from being visited again
		mark.insert(n);
		auto cr = tau::get(n).children();
		mark.insert(cr.begin(), cr.end());
		for (size_t i = eq_idx; i < cl.size(); ++i) n = join(n, cl[i]);
		return n;
	};
	auto f = [&](tref n, tref parent) {
		if (!tau::get(n).is(tau::wff)) return n;
		const tau& cn = tau::get(n)[0];
		if (parent != nullptr && is<node>(parent, tau::wff_or)) {
			if (!cn.is(tau::wff_or)) {
				// Push new assumption to stack
				assms[0].push_back(assms[0].back());
			}
		} else if (parent != nullptr && is<node>(parent, tau::wff_and)) {
			if (!cn.is(tau::wff_and)) {
				// Push new assumption to stack
				assms[1].push_back(assms[1].back());
			}
		}
		if (trefs conjs = get_cnf_wff_clauses<node>(n); conjs.size() > 1)
			return squeeze_clauses(n, conjs, false);
		if (trefs disjs = get_dnf_wff_clauses<node>(n); disjs.size() > 1)
			return squeeze_clauses(n, disjs, true);
		return n;
	};
	auto up = [&](tref n, tref parent) {
		if (!is<node>(n, tau::wff)) return n;
		if (parent != nullptr && is<node>(parent, tau::wff_or)) {
			if (!is_child<node>(n, tau::wff_or)) {
				assms[0].pop_back();
			}
		} else if (parent != nullptr && is<node>(parent, tau::wff_and)) {
			if (!is_child<node>(n, tau::wff_and)) {
				assms[1].pop_back();
			}
		}
		return n;
	};
	auto visit = [&mark](tref n) {
		if (is_quantifier<node>(n)) return false;
		if (is_temporal_quantifier<node>(n)) return false;
		if (mark.contains(n)) return false;
		return visit_wff<node>(n);
	};
	// Disable intermediate simplifications for the duration of the traversal.
	// The guard restores the previous value on every exit, including an
	// exception thrown out of `apply` -- this subsystem's bv paths do throw,
	// and an unwound `use_hooks = false` would disable hooks for the rest of
	// the process. Restoring rather than assigning `true` also leaves a caller
	// that deliberately disabled hooks alone.
	tref res = nullptr;
	{
		use_hooks_guard<node> hooks_off(false);
		res = pre_order<node>(formula).apply(f, visit, up);
		DBG(assert(assms[0].size() == 1);)
		DBG(assert(assms[1].size() == 1);)
	}
	// Hooks are back on here, which is what makes the `reget` below re-apply
	// the intermediate simplifications skipped during the traversal.
	return tau::reget(res);
}

} // namespace squeeze_detail

/**
 * @brief The procedure collects all = 0 and != 0 equations within scope
 * (conjunction and disjunction respectively) and squeezes
 * those together that share at least one variable. Each squeezed formula A is
 * then integrated into terms in scope that share at least 2 variables including
 * the provided one in the following way:
 * - if A is = 0 and given f = 0, we produce f|A = 0
 * - if A is = 0 and given f != 0, we produce f & A' != 0
 * - if A is != 0 and given f != 0, we produce f|A != 0
 * - if A is != 0 and given f = 0, we produce f & A' = 0
 * @tparam node Tree node type
 * @param formula The formula to apply the procedure to
 * @return The mutated formula
 */
template<NodeType node>
tref squeeze_absorb(tref formula) {
	LOG_TRACE << "squeeze_absorb/formula: " << LOG_FM(formula);
	squeeze_detail::over_all_vars<node> p;
	tref squeezed = squeeze_detail::squeeze_absorb_impl<node>(formula, p);
	LOG_TRACE << "squeeze_absorb/squeezed: " << LOG_FM(squeezed);
	return squeezed;
}

/**
 * @brief The procedure collects all = 0 and != 0 equations within scope
 * (conjunction and disjunction respectively) and squeezes
 * those together that share var. Each squeezed formula A is
 * then integrated into terms in scope that share at least 2 variables including
 * var in the following way:
 * - if A is = 0 and given f = 0, we produce f|A = 0
 * - if A is = 0 and given f != 0, we produce f & A' != 0
 * - if A is != 0 and given f != 0, we produce f|A != 0
 * - if A is != 0 and given f = 0, we produce f & A' = 0
 * @tparam node Tree node type
 * @param formula The formula to apply the procedure to
 * @param var The shared variable for squeeze/absorb
 * @return The mutated formula
*/
template<NodeType node>
tref squeeze_absorb(tref formula, tref var) {
	LOG_TRACE << "squeeze_absorb/formula: " << LOG_FM(formula);
	LOG_TRACE << "squeeze_absorb/var: " << LOG_FM(var);
	squeeze_detail::over_var<node> p{ var, {} };
	tref squeezed = squeeze_detail::squeeze_absorb_impl<node>(formula, p);
	LOG_TRACE << "squeeze_absorb/squeezed: " << LOG_FM(squeezed);
	return squeezed;
}

} // namespace idni::tau_lang
