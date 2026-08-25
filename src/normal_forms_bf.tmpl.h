// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// normal_forms_bf.tmpl.h - BF simplification: syntactic simplification, squeeze_absorb
// Split from normal_forms.tmpl.h for readability.

namespace idni::tau_lang {

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "anti_prenex"

/**
 * @brief The procedure tries to detect, using 0/1 substitutions for the provided
 * variable and syntactic comparison, if the atomic formula is equivalent to T
 * or F or independent of the provided variable.
 * @tparam node Type of tree node
 * @param atomic_fm The atomic formula, ie an equation, to simplify
 * @param var The variable to base the simplifications on
 * @return The simplified atomic formula
 */
template<NodeType node>
tref syntactic_variable_simplification(tref atomic_fm, tref var) {
	using tau = tree<node>;
	using tt = tau::traverser;
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
	if (auto it = cache.find(key); it != end(cache)) return it->second;
	// Every non-trivial exit stores through this, in particular the func2 == 0
	// early return below: norm_equation brings every bf_eq/bf_neq to `f (!)= 0`,
	// so that return is the path taken by the vast majority of the calls, and
	// storing only at the end left the cache write-never for them.
	auto memo = [&key](tref r) { return cache.emplace(key, r).first->second; };
#else
	auto memo = [](tref r) { return r; };
#endif // TAU_CACHE
	// Return early if atomic_fm is either T or F
	if (tau::get(atomic_fm).equals_T() || tau::get(atomic_fm).equals_F())
		return atomic_fm;
	DBG(LOG_TRACE << "Syntactic_variable_simplification on " << LOG_FM(atomic_fm) << "\n";)
	DBG(LOG_TRACE << "with var: " << LOG_FM(var) << "\n";)
	var = tau::get(tau::bf, var);
	atomic_fm = gt_gteq_to_lt_lteq<node>(atomic_fm);
	atomic_fm = norm_equation<node>(atomic_fm);
	auto atm_type = tau::get(atomic_fm)[0].value.nt;
	tref func1 = tau::get(atomic_fm)[0].first();
	tref func2 = tau::get(atomic_fm)[0].second();
	// Make sure that it works only on Boolean parts by using replace_if
	tref func1_v_0 = rewriter::replace_if<node>(func1, var,
		_0<node>(find_ba_type<node>(var)), is_boolean_operation<node>);
	func1_v_0 = tt(func1_v_0) | bf_reduce_canonical<node>() | tt::ref;
	tref func1_v_1 = rewriter::replace_if<node>(func1, var,
		_1<node>(find_ba_type<node>(var)), is_boolean_operation<node>);
	func1_v_1 = tt(func1_v_1) | bf_reduce_canonical<node>() | tt::ref;
	// Is func syntactically identically 0
	if (tau::get(func1_v_0).equals_0() && tau::get(func1_v_1).equals_0())
		func1 = tau::_0(find_ba_type<node>(func1));
	// Is func syntactically identically 1
	else if (tau::get(func1_v_0).equals_1() && tau::get(func1_v_1).equals_1())
		func1 = tau::_1(find_ba_type<node>(func1));
	// func is not dependent on var
	else if (tau::get(func1_v_0) == tau::get(func1_v_1) && !contains<node>(func1_v_0, var))
		func1 = func1_v_0;
	if (tau::get(func2).equals_0())
		return memo(denorm_equation<node>(
			tau::get(tau::wff, tau::get(atm_type, func1, func2))));
	// Simplify func2. Reached only for bf_lt/bf_lteq atoms: norm_equation
	// zeroes the right-hand side of every bf_eq/bf_neq, which the return above
	// then catches.
	tref func2_v_0 = rewriter::replace_if<node>(func2, var,
		_0<node>(find_ba_type<node>(var)), is_boolean_operation<node>);
	func2_v_0 = tt(func2_v_0) | bf_reduce_canonical<node>() | tt::ref;
	tref func2_v_1 = rewriter::replace_if<node>(func2, var,
		_1<node>(find_ba_type<node>(var)), is_boolean_operation<node>);
	func2_v_1 = tt(func2_v_1) | bf_reduce_canonical<node>() | tt::ref;
	// Is func syntactically identically 0
	if (tau::get(func2_v_0).equals_0() && tau::get(func2_v_1).equals_0())
		func2 = tau::_0(find_ba_type<node>(func2));
	// Is func syntactically identically 1
	else if (tau::get(func2_v_0).equals_1() && tau::get(func2_v_1).equals_1())
		func2 = tau::_1(find_ba_type<node>(func2));
	// func is not dependent on var
	else if (tau::get(func2_v_0) == tau::get(func2_v_1) && !contains<node>(func2_v_0, var))
		func2 = func2_v_0;
	tref res = tau::get(tau::wff, tau::get(atm_type, func1, func2));
	DBG(LOG_TRACE << "Syntactic_variable_simplification result: " << LOG_FM(res) << "\n";)
	return memo(res);
}

// `simplify_using_equality_dnf` and `syntactic_path_simplification_dnf` used
// to live here as near-verbatim copies of the heuristics passes of the same
// name. Every fix landed on one copy only -- the heuristics one accumulated
// the io_var guard in `simplify_equation`, the release-mode union-find stack
// underflow guard, the gc-aware consequence cache, the issue-69 decline of
// `wff_imply`/`rimply`/`equiv`/`xor`/`conditional`, `while_is_boolean_operation`
// as the bf substitution descent predicate -- while this one, the copy
// production actually ran, got none of them. Both copies are deleted; the
// callers below now use `simplify_using_equality`
// (heuristics/simplify_using_equality.h), `syntactic_path_simplification` and
// `syntactic_path_simplification_unsat_on_unchanged_negations`
// (heuristics/syntactic_path_simplification.h), both pulled in ahead of this
// file by normal_forms.tmpl.h / normal_forms.h respectively.

/**
 * @brief Function to apply syntactical simplifications to formula in almost
 * linear time in the formula size and the number of paths found in terms
 * @tparam node tree node type
 * @param formula The formula to simplify
 * @return The simplified formula
 *
 * @note This pass takes no `skip` predicate, and deliberately so: it used to
 * accept one for interface consistency with `anti_prenex_block`'s other steps
 * and then discard it, which read as a guarantee it never gave. Neither
 * `simplify_using_equality` nor `syntactic_path_simplification` has a
 * BV-specific check to guard, so a caller that skips bitvector content
 * elsewhere in the pipeline must not assume this step leaves it alone.
 */
template<NodeType node>
tref syntactic_formula_simplification(tref formula) {
	formula = simplify_using_equality<node>(formula);
	return syntactic_path_simplification<node>(formula);
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
	// This also makes the relation a valid strict weak ordering: the previous
	// version returned false for every pair involving a non-io variable in
	// both directions, which made each non-io variable equivalent to each io
	// variable while io variables stayed strictly ordered among themselves --
	// so incomparability was not transitive and std::ranges::stable_sort's
	// precondition was violated, leaving the BDD variable order (and hence the
	// normal form) unspecified.
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
 * @tparam node Type of tree node
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
		// `./dev regen` with a newer pinned generator renumbers them and
		// silently reorders every id-based tie. The 8f1a74c1
		// regeneration did exactly that and re-rolled the
		// Boole-decomposition pivot choice into a hang (GitHub #70
		// family; bisected 2026-08-19). Ties therefore break on PRINTED
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
 * @return Simplified formula
 */
template<NodeType node>
tref syntactic_atomic_formula_simplification(tref atomic_formula) {
	using tau = tree<node>;
	DBG(LOG_TRACE << "Start syntactic_atomic_formula_simplification: "
		<< tau::get(atomic_formula) << "\n";)
	auto atm_type = tau::get(atomic_formula)[0].value.nt;
	// Bring an equation to !(=) 0
	atomic_formula = norm_equation<node>(atomic_formula);
	if (tau::get(atomic_formula).equals_T() ||
		tau::get(atomic_formula).equals_F()) return atomic_formula;
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
		atomic_formula =
			syntactic_variable_simplification<node>(atomic_formula, v);
	}
	DBG(LOG_TRACE << "End syntactic_atomic_formula_simplification: "
		<< tau::get(atomic_formula) << "\n";)
	return atomic_formula;
}

// Squeeze two equations equal/unequal to zero into one equation
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

template <NodeType node>
tref apply_assms(tref eq, const auto& assms, auto& joins, trefs& additions, bool dual = false) {
	using tau = tree<node>;
	DBG(assert(tau::get(eq)[0][1].equals_0());)
	const trefs fv = get_free_vars_or_refs<node>(eq);
	DBG(assert(tau::get(eq).child_is(tau::bf_eq) ||
		tau::get(eq).child_is(tau::bf_neq));)
	const bool is_eq_pos = tau::get(eq).child_is(tau::bf_eq);
	bool joined = false;
	for (tref assm : assms.back()) {
		// Check for overlap between current assumption and eq
		const trefs fv_a = get_free_vars_or_refs<node>(assm);
		if (const int_t count = get_ordered_overlap<node>(fv, fv_a); count > 1 ||
			(count == 1 && fv.size() == 1)) {
			// Apply assumption
			if (is_eq_pos) {
				if (dual) {
					eq = tau::build_bf_eq_0(tau::build_bf_and(
						tau::trim2(eq), tau::build_bf_neg(assm)));
				} else {
					DBG(LOG_TRACE << "Merging " << tau::get(assm) << " and " << tau::get(tau::trim2(eq)) << "\n";)
					joins.merge(assm, tau::trim2(eq));
					joined = true;
					eq = tau::build_bf_eq_0(tau::build_bf_or(
						tau::trim2(eq), assm));
				}
			} else {
				if (dual) {
					DBG(LOG_TRACE << "Merging " << tau::get(assm) << " and " << tau::get(tau::trim2(eq)) << "\n";)
					joins.merge(assm, tau::trim2(eq));
					joined = true;
					eq = tau::build_bf_neq_0(tau::build_bf_or(
						tau::trim2(eq), assm));
				// NF-4: else, not fall-through -- the dual arm
				// otherwise gets immediately AND-ed with the
				// assumption's negation ((f|A) & A' == f & A'),
				// contradicting the 3-arg overload and the
				// documented rule table (f|A only).
				} else eq = tau::build_bf_neq_0(tau::build_bf_and(
					tau::trim2(eq), tau::build_bf_neg(assm)));
			}
		} else if (count == 1 && is_eq_pos) {
			DBG(LOG_TRACE << "Merging " << tau::get(assm) << " and " << tau::get(tau::trim2(eq)) << "\n";)
			joins.merge(assm, tau::trim2(eq));
			joined = true;
		}
	}
	if (!joined) if ((is_eq_pos && !dual) || (!is_eq_pos && dual)) {
		DBG(LOG_TRACE << "Addition: " << tau::get(tau::trim2(eq)) << "\n";)
		additions.emplace_back(tau::trim2(eq));
	}
	return eq;
}

template <NodeType node>
tref apply_assms(tref eq, const auto& assms, bool dual = false) {
	using tau = tree<node>;
	DBG(assert(tau::get(eq)[0][1].equals_0());)
	const trefs fv = get_free_vars_or_refs<node>(eq);
	DBG(assert(tau::get(eq).child_is(tau::bf_eq) ||
		tau::get(eq).child_is(tau::bf_neq));)
	const bool is_eq_pos = tau::get(eq).child_is(tau::bf_eq);
	for (tref assm : assms.back()) {
		// Check for overlap between current assumption and eq
		const trefs fv_a = get_free_vars_or_refs<node>(assm);
		if (const int_t count = get_ordered_overlap<node>(fv, fv_a); count > 1 ||
			(count == 1 && fv.size() == 1)) {
			// Apply assumption
			if (is_eq_pos) {
				if (dual) eq = tau::build_bf_eq_0(tau::build_bf_and(
					tau::trim2(eq), tau::build_bf_neg(assm)));
				else eq = tau::build_bf_eq_0(tau::build_bf_or(
					tau::trim2(eq), assm));
				DBG(LOG_TRACE << "From assumption " << tau::get(assm) << " produce " << tau::get(eq) << "\n";)
			} else {
				if (dual) eq = tau::build_bf_neq_0(tau::build_bf_or(
					tau::trim2(eq), assm));
				else eq = tau::build_bf_neq_0(tau::build_bf_and(
					tau::trim2(eq), tau::build_bf_neg(assm)));
				DBG(LOG_TRACE << "From assumption " << tau::get(assm) << " produce " << tau::get(eq) << "\n";)
			}
		}
	}
	return eq;
}

template <NodeType node>
tref apply_assms(tref eq, tref assm, tref var, const bool dual = false) {
	using tau = tree<node>;
	DBG(assert(tau::get(eq)[0][1].equals_0());)
	const trefs& fv = get_free_vars<node>(eq);
	if (!std::binary_search(fv.begin(), fv.end(), var, tau::subtree_less))
		return eq;
	DBG(assert(tau::get(eq).child_is(tau::bf_eq) ||
		tau::get(eq).child_is(tau::bf_neq));)
	const bool is_eq_pos = tau::get(eq).child_is(tau::bf_eq);
	// Check for overlap between current assumption and eq
	const trefs& fv_a = get_free_vars<node>(assm);
	if (fv.size() == 1 || is_ordered_overlap_at_least<node>(2,fv, fv_a)) {
		// Apply assumption
		if (is_eq_pos) {
			if (dual) eq = tau::build_bf_eq_0(tau::build_bf_and(
				tau::trim2(eq), tau::build_bf_neg(assm)));
			else eq = tau::build_bf_eq_0(tau::build_bf_or(
				tau::trim2(eq), assm));
			DBG(LOG_TRACE << "From assumption " << tau::get(assm) << " produce " << tau::get(eq) << "\n";)
		} else {
			if (dual) eq = tau::build_bf_neq_0(tau::build_bf_or(
				tau::trim2(eq), assm));
			else eq = tau::build_bf_neq_0(tau::build_bf_and(
				tau::trim2(eq), tau::build_bf_neg(assm)));
			DBG(LOG_TRACE << "From assumption " << tau::get(assm) << " produce " << tau::get(eq) << "\n";)
		}
	}
	return eq;
}

template <NodeType node>
tref apply_assms(tref eq, tref assm, tref var, trefs& updates, const bool dual = false) {
	using tau = tree<node>;
	DBG(assert(tau::get(eq)[0][1].equals_0());)
	const trefs& fv = get_free_vars<node>(eq);
	if (!std::binary_search(fv.begin(), fv.end(), var, tau::subtree_less))
		return eq;
	DBG(assert(tau::get(eq).child_is(tau::bf_eq) ||
		tau::get(eq).child_is(tau::bf_neq));)
	const bool is_eq_pos = tau::get(eq).child_is(tau::bf_eq);
	if ((is_eq_pos && !dual) || (!is_eq_pos && dual))
		updates.emplace_back(eq);
	// Check for overlap between current assumption and eq
	const trefs& fv_a = get_free_vars<node>(assm);
	if (fv.size() == 1 || is_ordered_overlap_at_least<node>(2,fv, fv_a)) {
		// Apply assumption
		if (is_eq_pos) {
			if (dual) eq = tau::build_bf_eq_0(tau::build_bf_and(
				tau::trim2(eq), tau::build_bf_neg(assm)));
			else eq = tau::build_bf_eq_0(tau::build_bf_or(
				tau::trim2(eq), assm));
			DBG(LOG_TRACE << "From assumption " << tau::get(assm) << " produce " << tau::get(eq) << "\n";)
		} else {
			if (dual) eq = tau::build_bf_neq_0(tau::build_bf_or(
				tau::trim2(eq), assm));
			else eq = tau::build_bf_neq_0(tau::build_bf_and(
				tau::trim2(eq), tau::build_bf_neg(assm)));
			DBG(LOG_TRACE << "From assumption " << tau::get(assm) << " produce " << tau::get(eq) << "\n";)
		}
	}
	return eq;
}

template <NodeType node>
void update_assms(auto& assms, auto& joins, trefs& additions) {
	using tau = tree<node>;
	subtree_set<node> excluded;
	trefs& A = assms.back();
	for (size_t i = 0; i < A.size(); ++i) {
		if (excluded.contains(A[i])) {
			A.erase(A.begin()+i);
			DBG(assert(i > 0);)
			--i;
			continue;
		}
		// Get the set of joinable assumptions from joins
		trefs col = joins.get_set(A[i]);
		for (tref c : col) excluded.insert(c);
		DBG(LOG_TRACE << "Updated assumption from " << tau::get(A[i])
			<< " to " << tau::get(tau::build_bf_or(col,
				find_ba_type<node>(A[i]))) << "\n";)
		A[i] = tau::build_bf_or(col, find_ba_type<node>(A[i]));
	}
	for (tref add : additions) {
		DBG(LOG_TRACE << "Added to assumption: " << tau::get(add) << "\n";)
		A.emplace_back(add);
	}
}

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
	using tau = tree<node>;
	DBG(LOG_DEBUG << "Started squeeze_absorb");
	std::vector<trefs> assms { trefs {} };
	std::vector<trefs> dual_assms { trefs {} };
	subtree_unordered_set<node> mark;
	auto uf_comp = [](tref l, tref r) {
		return tau::subtree_less(l, r);
	};
	auto f = [&](tref n, tref parent) {
		if (!tau::get(n).is(tau::wff)) return n;
		const tau& cn = tau::get(n)[0];
		if (parent != nullptr && is<node>(parent, tau::wff_or)) {
			if (!cn.is(tau::wff_or)) {
				// Push new assumption to stack
				assms.push_back(assms.back());
			}
		} else if (parent != nullptr && is<node>(parent, tau::wff_and)) {
			if (!cn.is(tau::wff_and)) {
				// Push new assumption to stack
				dual_assms.push_back(dual_assms.back());
			}
		}
		// Try getting conjunctions
		if (trefs conjs = get_cnf_wff_clauses<node>(n); conjs.size() > 1) {
			// Sort the equations in conjs up front and prepare
			// union find to compute variable overlaps
			size_t eq_idx = 0;
			auto uf = union_find_with_sets<decltype(uf_comp), node>(uf_comp);
			for (tref& conj : conjs) {
				const tau& conj_t = tau::get(conj);
				if (conj_t.child_is(tau::bf_neq)) {
					// Only treat != 0 equations
					if (!conj_t[0][1].equals_0()) continue;
					std::swap(conj, conjs[eq_idx++]);
				} else if (conj_t.child_is(tau::bf_eq)) {
					// Only treat = 0 equations
					if (!conj_t[0][1].equals_0()) continue;
					// merge variables of = 0 equations
					const trefs fv = get_free_vars_or_refs<node>(conj);
					for (tref v : fv) uf.merge(fv[0], v);
					std::swap(conj, conjs[eq_idx++]);
				}
			}
			// Return early if no equation present
			if (eq_idx == 0) return n;
#ifdef DEBUG
			// Print sorted conjuncts
			LOG_TRACE << "Conjuncts with eqs first: ";
			for (tref el : conjs) LOG_TRACE << tau::get(el) << ", ";
#endif
			// Apply dual assumptions to equations
			for (size_t i = 0; i < eq_idx; ++i)
				conjs[i] = apply_assms<node>(conjs[i], dual_assms, true);
			// Squeeze = 0 equations that share a variable
			// First sort = 0 up front
			size_t pos_eq_idx = 0;
			for (size_t i = 0; i < eq_idx; ++i) {
				if (tau::get(conjs[i]).child_is(tau::bf_eq))
					std::swap(conjs[i], conjs[pos_eq_idx++]);
			}
			for (size_t i = 0; i+1 < pos_eq_idx; ++i) {
				// Get free variables at i
				const trefs fv1 = get_free_vars_or_refs<node>(conjs[i]);
				if (fv1.empty()) continue;
				for (size_t j = i+1; j < pos_eq_idx; ++j) {
					// Get free variables at i + 1
					const trefs fv2 = get_free_vars_or_refs<node>(conjs[j]);
					if (fv2.empty()) continue;
					// Squeeze overlapping terms
					if (uf.connected(fv1[0], fv2[0])) {
						conjs[i] = squeeze<node>(conjs[i], conjs[j]);
						conjs.erase(conjs.begin()+j);
						--j;
						--pos_eq_idx;
						--eq_idx;
					}
				}
			}
			// Apply assumptions to equations while updating them on = 0 equations
			uf.clear();
			trefs additions;
			for (size_t i = 0; i < pos_eq_idx; ++i)
				conjs[i] = apply_assms<node>(conjs[i], assms, uf, additions);
			update_assms<node>(assms, uf, additions);
			// Apply updated assumptions to != 0 equations
			for (size_t i = pos_eq_idx; i < eq_idx; ++i)
				conjs[i] = apply_assms<node>(conjs[i], assms);
			// Build result node
			n = conjs[0];
			for (size_t i = 1; i < eq_idx; ++i)
				n = tau::build_wff_and(n, conjs[i]);
			// Exclude equations from being visited again
			mark.insert(n);
			auto cr = tau::get(n).children();
			mark.insert(cr.begin(), cr.end());
			for (size_t i = eq_idx; i < conjs.size(); ++i)
				n = tau::build_wff_and(n, conjs[i]);
			return n;
		// Try getting disjunctions
		} else if (trefs disjs = get_dnf_wff_clauses<node>(n); disjs.size() > 1) {
			// Sort equations in disjs up front and prepare union find
			// to compute variable overlap
			size_t eq_idx = 0;
			auto uf = union_find_with_sets<decltype(uf_comp), node>(uf_comp);
			for (tref& disj : disjs) {
				const tau& disj_t = tau::get(disj);
				if (disj_t.child_is(tau::bf_eq)) {
					// Only treat = 0 equations
					if (!disj_t[0][1].equals_0()) continue;
					std::swap(disj, disjs[eq_idx++]);
				} else if (disj_t.child_is(tau::bf_neq)) {
					// Only treat != 0 equations
					if (!disj_t[0][1].equals_0()) continue;
					// merge variables of != 0 equations
					const trefs fv = get_free_vars_or_refs<node>(disj);
					for (tref v : fv) uf.merge(fv[0], v);
					std::swap(disj, disjs[eq_idx++]);
				}
			}
			// Return early if no equation present
			if (eq_idx == 0) return n;
#ifdef DEBUG
			// Print sorted conjuncts
			LOG_TRACE << "Disjuncts with eqs first: ";
			for (tref el : disjs) LOG_TRACE << tau::get(el) << ", ";
#endif
			// Apply assumptions to equations
			for (size_t i = 0; i < eq_idx; ++i)
				disjs[i] = apply_assms<node>(disjs[i], assms);
			// Dual squeeze != 0 equations that share a variable
			// First sort != 0 up front
			size_t neg_eq_idx = 0;
			for (size_t i = 0; i < eq_idx; ++i) {
				if (tau::get(disjs[i]).child_is(tau::bf_neq))
					std::swap(disjs[i], disjs[neg_eq_idx++]);
			}
			for (size_t i = 0; i+1 < neg_eq_idx; ++i) {
				// Get free variables at i
				const trefs fv1 = get_free_vars_or_refs<node>(disjs[i]);
				if (fv1.empty()) continue;
				for (size_t j = i+1; j < neg_eq_idx; ++j) {
					// Get free variables at i + 1
					const trefs fv2 = get_free_vars_or_refs<node>(disjs[j]);
					if (fv2.empty()) continue;
					// Squeeze overlapping terms
					if (uf.connected(fv1[0], fv2[0])) {
						disjs[i] = squeeze<node>(disjs[i], disjs[j]);
						disjs.erase(disjs.begin()+j);
						--j;
						--neg_eq_idx;
						--eq_idx;
					}
				}
			}
			// Apply dual assumptions to equations while updating them on != 0 equations
			uf.clear();
			trefs additions;
			for (size_t i = 0; i < neg_eq_idx; ++i)
				disjs[i] = apply_assms<node>(disjs[i],
					dual_assms, uf, additions, true);
			update_assms<node>(dual_assms, uf, additions);
			// Apply updated assumptions to = 0 equations
			for (size_t i = neg_eq_idx; i < eq_idx; ++i)
				disjs[i] = apply_assms<node>(disjs[i], dual_assms, true);
			// Build result node
			n = disjs[0];
			for (size_t i = 1; i < eq_idx; ++i)
				n = tau::build_wff_or(n, disjs[i]);
			// Exclude equations from being visited again
			mark.insert(n);
			auto cr = tau::get(n).children();
			mark.insert(cr.begin(), cr.end());
			for (size_t i = eq_idx; i < disjs.size(); ++i)
				n = tau::build_wff_or(n, disjs[i]);
			return n;
		} else {
			// No disjuncts or conjuncts present
			// Simply apply dual assumptions and assumptions
			return n;
		}
	};
	auto up = [&](tref n, tref parent) {
		if (!is<node>(n, tau::wff)) return n;
		if (parent != nullptr && is<node>(parent, tau::wff_or)) {
			if (!is_child<node>(n, tau::wff_or)) {
				assms.pop_back();
			}
		} else if (parent != nullptr && is<node>(parent, tau::wff_and)) {
			if (!is_child<node>(n, tau::wff_and)) {
				dual_assms.pop_back();
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
		DBG(assert(assms.size() == 1);)
		DBG(assert(dual_assms.size() == 1);)
	}
	// Hooks are back on here, which is what makes the `reget` below re-apply
	// the intermediate simplifications skipped during the traversal.
	DBG(LOG_DEBUG << "Ended squeeze_absorb");
	return tau::reget(res);
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
	using tau = tree<node>;
	DBG(LOG_DEBUG << "Started squeeze_absorb on var:" << tau::get(var));
	trefs assms {tau::_0(find_ba_type<node>(var))};
	trefs dual_assms {tau::_0(find_ba_type<node>(var))};
	subtree_unordered_set<node> mark;
	auto f = [&](tref n, tref parent) {
		if (!tau::get(n).is(tau::wff)) return n;
		const tau& cn = tau::get(n)[0];
		if (parent != nullptr && is<node>(parent, tau::wff_or)) {
			if (!cn.is(tau::wff_or)) {
				// Push new assumption to stack
				assms.push_back(assms.back());
			}
		} else if (parent != nullptr && is<node>(parent, tau::wff_and)) {
			if (!cn.is(tau::wff_and)) {
				// Push new assumption to stack
				dual_assms.push_back(dual_assms.back());
			}
		}
		// Try getting conjunctions
		if (trefs conjs = get_cnf_wff_clauses<node>(n); conjs.size() > 1) {
			// Sort the equations in conjs up front
			size_t eq_idx = 0;
			for (tref& conj : conjs) {
				const tau& conj_t = tau::get(conj);
				if (conj_t.child_is(tau::bf_neq) ||
					conj_t.child_is(tau::bf_eq)) {
					// Only treat (!)= 0 equations
					if (!conj_t[0][1].equals_0()) continue;
					std::swap(conj, conjs[eq_idx++]);
				}
			}
			// Return early if no equation present
			if (eq_idx == 0) return n;
#ifdef DEBUG
			// Print sorted conjuncts
			LOG_TRACE << "Conjuncts with eqs first: ";
			for (tref el : conjs) LOG_TRACE << tau::get(el) << ", ";
#endif
			// Apply dual assumptions to equations
			for (size_t i = 0; i < eq_idx; ++i)
				conjs[i] = apply_assms<node>(conjs[i],
					dual_assms.back(), var, true);
			// First sort = 0 up front
			size_t pos_eq_idx = 0;
			for (size_t i = 0; i < eq_idx; ++i) {
				if (tau::get(conjs[i]).child_is(tau::bf_eq))
					std::swap(conjs[i], conjs[pos_eq_idx++]);
			}
			// Squeeze = 0 equations that contain var
			for (size_t i = 0; i+1 < pos_eq_idx; ++i) {
				// Get free variables at i
				const trefs& fv1 = get_free_vars<node>(conjs[i]);
				if (fv1.empty()) continue;
				if (!std::binary_search(fv1.begin(),
					fv1.end(), var, tau::subtree_less))
					continue;
				for (size_t j = i+1; j < pos_eq_idx; ++j) {
					// Get free variables at i + 1
					const trefs& fv2 = get_free_vars<node>(conjs[j]);
					if (fv2.empty()) continue;
					if (!std::binary_search(fv2.begin(),
						fv2.end(), var, tau::subtree_less))
						continue;
					// Squeeze overlapping terms
					conjs[i] = squeeze<node>(conjs[i], conjs[j]);
					conjs.erase(conjs.begin()+j);
					--j;
					--pos_eq_idx;
					--eq_idx;
				}
			}
			// Apply assumptions to equations while updating them on = 0 equations
			trefs updates;
			for (size_t i = 0; i < pos_eq_idx; ++i)
				conjs[i] = apply_assms<node>(conjs[i],
					assms.back(), var, updates);
			for (tref upd : updates) {
				assms.back() = tau::build_bf_or(
					assms.back(), tau::trim2(upd));
			}
			// Apply updated assumptions to != 0 equations
			for (size_t i = pos_eq_idx; i < eq_idx; ++i)
				conjs[i] = apply_assms<node>(conjs[i],
					assms.back(), var);
			// Build result node
			n = conjs[0];
			for (size_t i = 1; i < eq_idx; ++i)
				n = tau::build_wff_and(n, conjs[i]);
			// Exclude equations from being visited again
			mark.insert(n);
			auto cr = tau::get(n).children();
			mark.insert(cr.begin(), cr.end());
			for (size_t i = eq_idx; i < conjs.size(); ++i)
				n = tau::build_wff_and(n, conjs[i]);
			return n;
		// Try getting disjunctions
		} else if (trefs disjs = get_dnf_wff_clauses<node>(n); disjs.size() > 1) {
			// Sort equations in disjs up front
			size_t eq_idx = 0;
			for (tref& disj : disjs) {
				const tau& disj_t = tau::get(disj);
				if (disj_t.child_is(tau::bf_eq) ||
					disj_t.child_is(tau::bf_neq)) {
					if (!disj_t[0][1].equals_0()) continue;
					std::swap(disj, disjs[eq_idx++]);
				}
			}
			// Return early if no equation present
			if (eq_idx == 0) return n;
#ifdef DEBUG
			// Print sorted conjuncts
			LOG_TRACE << "Disjuncts with eqs first: ";
			for (tref el : disjs) LOG_TRACE << tau::get(el) << ", ";
#endif
			// Apply assumptions to equations
			for (size_t i = 0; i < eq_idx; ++i)
				disjs[i] = apply_assms<node>(disjs[i],
					assms.back(), var);
			// First sort != 0 up front
			size_t neg_eq_idx = 0;
			for (size_t i = 0; i < eq_idx; ++i) {
				if (tau::get(disjs[i]).child_is(tau::bf_neq))
					std::swap(disjs[i], disjs[neg_eq_idx++]);
			}
			// Dual squeeze != 0 equations that contain var
			for (size_t i = 0; i+1 < neg_eq_idx; ++i) {
				// Get free variables at i
				const trefs& fv1 = get_free_vars<node>(disjs[i]);
				if (fv1.empty()) continue;
				if (!std::binary_search(fv1.begin(),
					fv1.end(), var, tau::subtree_less))
					continue;
				for (size_t j = i+1; j < neg_eq_idx; ++j) {
					// Get free variables at j
					const trefs& fv2 = get_free_vars<node>(disjs[j]);
					if (fv2.empty()) continue;
					if (!std::binary_search(fv2.begin(),
					fv2.end(), var, tau::subtree_less))
						continue;
					// Squeeze overlapping terms
					disjs[i] = squeeze<node>(disjs[i], disjs[j]);
					disjs.erase(disjs.begin()+j);
					--j;
					--neg_eq_idx;
					--eq_idx;
				}
			}
			// Apply dual assumptions to equations while updating them on != 0 equations
			trefs updates;
			for (size_t i = 0; i < neg_eq_idx; ++i)
				disjs[i] = apply_assms<node>(disjs[i],
					dual_assms.back(), var, updates, true);
			for (tref upd : updates) {
				dual_assms.back() = tau::build_bf_or(
					dual_assms.back(), tau::trim2(upd));
			}
			// Apply updated assumptions to = 0 equations
			for (size_t i = neg_eq_idx; i < eq_idx; ++i)
				disjs[i] = apply_assms<node>(disjs[i],
					dual_assms.back(), var, true);
			// Build result node
			n = disjs[0];
			for (size_t i = 1; i < eq_idx; ++i)
				n = tau::build_wff_or(n, disjs[i]);
			// Exclude equations from being visited again
			mark.insert(n);
			auto cr = tau::get(n).children();
			mark.insert(cr.begin(), cr.end());
			for (size_t i = eq_idx; i < disjs.size(); ++i)
				n = tau::build_wff_or(n, disjs[i]);
			return n;
		} else {
			// No disjuncts or conjuncts present
			// Simply apply dual assumptions and assumptions
			return n;
		}
	};
	auto up = [&](tref n, tref parent) {
		if (!is<node>(n, tau::wff)) return n;
		if (parent != nullptr && is<node>(parent, tau::wff_or)) {
			if (!is_child<node>(n, tau::wff_or)) {
				assms.pop_back();
			}
		} else if (parent != nullptr && is<node>(parent, tau::wff_and)) {
			if (!is_child<node>(n, tau::wff_and)) {
				dual_assms.pop_back();
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
		DBG(assert(assms.size() == 1);)
		DBG(assert(dual_assms.size() == 1);)
	}
	// Hooks are back on here, which is what makes the `reget` below re-apply
	// the intermediate simplifications skipped during the traversal.
	DBG(LOG_DEBUG << "Ended squeeze_absorb");
	return tau::reget(res);
}

} // namespace idni::tau_lang
