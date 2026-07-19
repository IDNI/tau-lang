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
	if (auto it = cache.find(std::make_pair(tau::trim_right_sibling(atomic_fm),
		tau::trim_right_sibling(var))); it != end(cache))
		return it->second;
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
		return denorm_equation<node>(
			tau::get(tau::wff, tau::get(atm_type, func1, func2)));
	// Simplify func2
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
#ifdef TAU_CACHE
	cache.emplace(std::make_pair(tau::trim_right_sibling(res),
		tau::trim_right_sibling(var)), res);
	return cache.emplace(std::make_pair(tau::trim_right_sibling(atomic_fm),
		tau::trim_right_sibling(var)), res).first->second;
#endif // TAU_CACHE
	return res;
}

/**
 * @brief Simplify using assumptions from equalities in formula.
 * @tparam node Type of tree node
 */
template <NodeType node>
struct simplify_using_equality_dnf {
	using tau = tree<node>;
	// TODO: For variables, make lower time step < higher time step
	// Create comparator function that orders bfs by making constants smallest
	// We have 0 < 1 < bf_constant < uninterpreted_constant < input stream < output stream < variable < rest by subtree_less
	static bool term_comp(tref l, tref r) {
		if (tau::get(l).equals_0()) {
			if (!tau::get(r).equals_0()) return true;
			else return false;
		}
		if (tau::get(r).equals_0()) return false;
		if (tau::get(l).equals_1()) {
			if (!tau::get(r).equals_1()) return true;
			else return false;
		}
		if (tau::get(r).equals_1()) return false;
		DBG(assert(tau::get(l).has_child() && tau::get(r).has_child()));
		const tau& lc = tau::get(l)[0];
		const tau& rc = tau::get(r)[0];
		if (lc.is(tau::ba_constant)) {
			if (!rc.is(tau::ba_constant)) return true;
			else return tau::subtree_less(l,r);
		}
		if (rc.is(tau::ba_constant)) return false;
		if (lc.is(tau::variable)) {
			if (rc.is(tau::variable)) {
				// Check for uninterpreted constant
				if (lc[0].is(tau::uconst_name)) {
					if (!rc[0].is(tau::uconst_name))
						return true;
					else return tau::subtree_less(l,r);
				}
				if (rc[0].is(tau::uconst_name)) return false;
				if (lc.is_input_variable()) {
					if (!rc.is_input_variable())
						return true;
					else return tau::subtree_less(l, r);
				}
				if (rc.is_input_variable()) return false;
				if (lc.is_output_variable()) {
					if (!rc.is_output_variable())
						return true;
					else return tau::subtree_less(l, r);
				}
				if (rc.is_output_variable()) return false;
				return tau::subtree_less(l,r);
			} else return true;
		}
		if (rc.is(tau::variable)) return false;
		// TODO: also use free_vars count once constant time
		return tau::subtree_less(l,r);
	};

	// Given a formula, traverse the formula and
	// apply equality simplifications along the way
	static tref on (tref fm) {
		// Push negation all the way in
		fm = to_nnf<node>(fm);
		if (tau::get(fm).equals_T() || tau::get(fm).equals_F())
			return fm;
		DBG(LOG_DEBUG << "Simplify_using_equality on " << LOG_FM(fm) << "\n";)
		// Create union find data structure to hold equality information
		auto uf = union_find_with_sets<decltype(term_comp), node>(term_comp);
		// Create stack of union find data structures
		std::vector<union_find_with_sets<decltype(term_comp), node>> uf_stack;
		uf_stack.emplace_back(std::move(uf));

		// We need to mark disjunctions that do not cause a push to the
		// stack, in order to make sure they are not popped later
		// due to intermediate simplifications
		subtree_unordered_set<node> mark;
		// Traverse the formula: on encounter of = or !=, first apply simplification and, if equality, add it
		auto f = [&uf_stack, &mark](tref n, tref parent) {
			if (!is<node>(n, tau::wff)) return n;
			const tau& cn = tau::get(n)[0];
			if (parent != nullptr && is<node>(parent, tau::wff_or)) {
				if (!cn.is(tau::wff_or)) {
					// Push new uf to stack
					uf_stack.push_back(uf_stack.back());
				} else mark.insert(parent);
			}
			if (cn.is(tau::bf_eq)) {
				n = syntactic_atomic_formula_simplification<node>(n);
				n = direct_atm(n);
				tref s = simplify_equation(uf_stack.back(), n);
				// If equation was simplified away
				if (!is_child<node>(s, tau::bf_eq)) return s;
				if (add_equality(uf_stack.back(), s)) return s;
				else return _F<node>();
			} else if (is_atomic_fm<node>(n)) {
				n = syntactic_atomic_formula_simplification<node>(n);
				n = direct_atm(n);
				return simplify_equation(uf_stack.back(), n);
			} else if (cn.is(tau::wff_and)) {
				// Check if conjunction was already processed
				if (mark.contains(n)) return n;
				// We need to reorder all conjunctions in order
				// to correctly collect all equalities
				trefs conjs = get_cnf_wff_clauses<node>(n);
				sort_atms(conjs);
				n = conjs[0];
				for (size_t i = 1; i < conjs.size(); ++i) {
					n = tau::build_wff_and(n, conjs[i]);
					mark.insert(n);
				}
				return n;
			} else return n;
		};
		auto up = [&uf_stack, &mark](tref n, tref parent) {
			if (!is<node>(n, tau::wff)) return n;
			if (parent != nullptr && is<node>(parent, tau::wff_or)) {
				if (!is_child<node>(n, tau::wff_or)) {
					// If parent was marked, child was a disjunction
					// before and nothing was pushed on stack
					if (auto it = mark.find(parent); it != mark.end()) {
						mark.erase(it);
					} else uf_stack.pop_back();
				} else {
					// Encounter of disjunction below disjunction
					// If not marked, this happened due to simplification
					// and we can safely pop
					if (auto it = mark.find(parent); it == mark.end())
						uf_stack.pop_back();
					else mark.erase(it);
				}
			}
			return n;
		};
		auto visit = [](tref n) {
			if (is_quantifier<node>(n)) return false;
			if (is_temporal_quantifier<node>(n)) return false;
			return visit_wff<node>(n);
		};
		fm = pre_order<node>(fm).apply(f, visit, up);
		DBG(LOG_DEBUG << "Simplify_using_equality result: " << LOG_FM(fm) << "\n";)
		// In DEBUG make sure that push and pop on stack is correct
		DBG(assert(uf_stack.size() == 1);)
		return fm;
	}

	// Given an equality, add it to the given union find data structure
	// Returns false, if a resulting equality set contains a contradiction
	static bool add_raw_equality (auto& uf, tref eq) {
		if (tau::get(eq).equals_T()) return true;
		if (tau::get(eq).equals_F()) return false;
		DBG(LOG_TRACE << "Adding equality: " << tau::get(eq) << "\n";)
		// Here we know that eq is still bf_eq
		// We both add a = b and a' = b' in order to detect syntactic contradictions
		tref left_hand_side = tau::trim2(eq);
		tref right_hand_side = tau::get(tau::trim(eq)).child(1);
		tref left_hand_side_neg =
			push_negation_in<node, false>(build_bf_neg<node>(left_hand_side));
		tref right_hand_side_neg =
			push_negation_in<node, false>(build_bf_neg<node>(right_hand_side));

		size_t type_id = find_ba_type<node>(eq);
		uf.merge(left_hand_side, right_hand_side);
		uf.merge(left_hand_side_neg, right_hand_side_neg);
		uf.merge(build_bf_and<node>(left_hand_side, right_hand_side_neg), _0<node>(type_id));
		uf.merge(build_bf_and<node>(right_hand_side, left_hand_side_neg), _0<node>(type_id));
		uf.merge(build_bf_or<node>(left_hand_side, right_hand_side_neg), _1<node>(type_id));
		uf.merge(build_bf_or<node>(right_hand_side, left_hand_side_neg), _1<node>(type_id));
		// Check if syntactic contradiction occurred
		return !uf.connected(left_hand_side, left_hand_side_neg)
				&& !uf.connected(right_hand_side, right_hand_side_neg)
					&& !uf.connected(_0<node>(type_id), _1<node>(type_id));
	}

	// In case eq is of form f = 0, unsqueeze disjunctions in f
	static bool add_equality(auto& uf, tref eq) {
		if (const tau& c = tau::get(eq)[0]; c[1].equals_0()) {
			tref func = push_negation_in<node, false>(c.first());
			bool valid = true;
			for (tref disj : get_dnf_bf_clauses<node>(func)) {
				valid = valid && add_raw_equality(uf,
					tau::build_bf_eq_0(disj));
			}
			return valid;
		} else return add_raw_equality(uf, eq);
	}

	// Given current equalities in union find, simplify the equation
	static tref simplify_equation (auto& uf, tref eq) {
		DBG(LOG_TRACE << "Simplifying: " << tau::get(eq) << "\n";)
		if (tau::get(eq).equals_T() || tau::get(eq).equals_F()) return eq;
		auto uf_find = [&uf](tref n) {
			if (uf.contains(n)) {
				// Do not replace n if all variables are inputs
				bool all_inputs = false;
				auto input_vars = [&all_inputs](tref cur) {
					if (tau::get(cur).is(tau::variable)) {
						if (tau::get(cur).is_input_variable()) {
							all_inputs = true;
							return true;
						}
						// different variable found
						all_inputs = false;
						return false;
					}
					return true;
				};
				pre_order<node>(n).search_unique(input_vars);
				return all_inputs ? n : uf.find(n);
			}
			return n;
		};
		tref simp_eq = pre_order<node>(eq).apply(uf_find);
		DBG(LOG_TRACE << "Simplified to: " << tau::get(simp_eq) << "\n";)
		if (tau::get(simp_eq) != tau::get(eq))
			return simp_eq;
		else return eq;
	}

private:
	static void sort_atms (auto& conjs) {
		auto eq_comp = [](tref l, tref r) {
			const tau& nl = tau::get(l);
			const tau& nr = tau::get(r);
			if (nl.child_is(tau::bf_eq)) {
				if (!nr.child_is(tau::bf_eq)) return true;
				// Both are equalities
				// Sort assignments to front
				if (is_equational_assignment<node>(l))
					return !is_equational_assignment<node>(r);
				return false;
			} else return false;
		};
		std::ranges::sort(conjs, eq_comp);
	}

	static tref direct_atm(tref atm) {
		const tau& c = tau::get(atm)[0];
		if (!c.is(tau::bf_eq) && !c.is(tau::bf_neq)) return atm;
		const size_t sym = c.get_type();
		tref c1 = tau::get(atm)[0].first();
		tref c2 = tau::get(atm)[0].second();
		if (term_comp(c1, c2))
			return tau::get(tau::wff, tau::get(sym, c2, c1));
		return atm;
	}
};

/**
 * @brief Syntactic simplifications for a formula or a term based on its paths.
 * @tparam node Type of tree node
 */
template <NodeType node>
class syntactic_path_simplification_dnf {
	using tau = tree<node>;

	static bool wff_comp (tref l, tref r) {
		if (tau::get(l).child_is(tau::bf_eq))
			if (tau::get(r).child_is(tau::bf_eq))
				return tau::subtree_less(l,r);
			else return true;
		else {
			if (tau::get(r).child_is(tau::bf_eq))
				return false;
			else return tau::subtree_less(l, r);
		}
	}

	static tref simplify_wff (tref root) {
		tref skip = nullptr;
		auto down = [&skip](tref n) {
			// Skip intermediate nodes
			if (!tau::get(n).is(tau::wff)) return n;
			const tau& t_n = tau::get(n)[0];
			// Skip or
			if (t_n.is(tau::wff_or))
				return n;
			// If no conjunction is found, no simplification is possible
			if (!t_n.is(tau::wff_and)) return n;
			subtree_map<node, tref> assignments;
			// Get all conjuncted assumptions
			for (tref l : get_cnf_wff_clauses<node>(n)) {
				if (tau::get(l).child_is(tau::wff_or))
					continue;
				// Treat subtree l as variable
				if (tau::get(l).child_is(tau::wff_neg))
					assignments.emplace(tau::trim2(l), _F<node>());
				else assignments.emplace( l, _T<node>());
			}
			tref simp = rewriter::replace(n, assignments);
			// If simp is false, current branch is not sat
			if (tau::get(simp).equals_F()) {
				// Remove branch
				return _F<node>();
			}
			// Rebuild assumptions
			trefs sorted_assms;
			sorted_assms.reserve(assignments.size());
			for (auto [v, k] : assignments)
				if (k == tau::_F())
					sorted_assms.push_back(tau::build_wff_neg(v));
				else sorted_assms.push_back(v);
			// Canonical sorting
			std::ranges::stable_sort(sorted_assms, wff_comp);
			tref assms = tau::build_wff_and(sorted_assms);
			// Make sure to not revisit build assumptions
			if (tau::get(simp) == get_T<node>())
				skip = tau::get(assms).first();
			else skip = assms;
			// Conjunct assumptions with simplified tree
			return tau::build_wff_and(assms, simp);
		};
		auto visit = [&skip](tref n) {
			if (skip != nullptr && tau::get(n) == tau::get(skip))
				return skip = nullptr, false;
			return visit_wff<node>(n);
		};
		return pre_order<node>(root).template
			apply_unique<synt_path_simp_m>(down, visit);
	}

	static tref simplify_bf(tref root) {
		tref skip = nullptr;
		auto down = [&skip](tref n) {
			// Skip intermediate nodes
			if (!tau::get(n).is(tau::bf)) return n;
			const tau& t_n = tau::get(n)[0];
			// Skip or and xor
			if (t_n.is(tau::bf_or) || t_n.is(tau::bf_xor))
				return n;
			// If no conjunction is found, no simplification is possible
			if (!t_n.is(tau::bf_and)) return n;
			subtree_map<node, tref> assignments;
			// Get all conjuncted assumptions
			for (tref l : get_cnf_bf_clauses<node>(n)) {
				if (tau::get(l).child_is(tau::bf_or))
					continue;
				if (tau::get(l).child_is(tau::bf_xor))
					continue;
				// Treat subtree l as variable
				if (tau::get(l).child_is(tau::bf_neg))
					assignments.emplace(tau::trim2(l),
						_0<node>(find_ba_type<node>(l)));
				else assignments.emplace( l,
					_1<node>(find_ba_type<node>(l)));
			}
			tref simp = rewriter::replace_if(n, assignments,
				is_boolean_operation<node>);
			// If simp is false, current branch is not sat
			if (tau::get(simp).equals_0()) {
				// Remove branch
				return simp;
			}
			// Rebuild assumptions
			trefs sorted_assms (assignments.size());
			// Canonical sorting
			std::ranges::partial_sort_copy(std::views::keys(assignments),
				sorted_assms, tau::subtree_less);
			tref assms = _1<node>(find_ba_type<node>(n));
			for (tref a : sorted_assms) {
				assms = tau::get(assignments[a]).equals_1()
						? tau::build_bf_and(assms, a)
						: tau::build_bf_and(assms,
							tau::build_bf_neg(a));
			}
			// Make sure to not revisit build assumptions
			if (tau::get(simp).equals_1())
				skip = tau::get(assms).first();
			else skip = assms;
			// Conjunct assumptions with simplified tree
			return tau::build_bf_and(assms, simp);
		};
		auto visit = [&](tref n) {
			if (skip != nullptr && tau::get(n) == tau::get(skip))
				return skip = nullptr, false;
			return is_boolean_operation<node>(n);
		};
		return pre_order<node>(root).template
			apply_unique<synt_path_simp_m>(down, visit);
	}

public:
	/**
	 * @brief Simplify contradictions on the formula that appear along a path.
	 * Then do the same for the negated formula, resolving simple tautologies.
	 * The procedure pushes negations all the way in.
	 * @param fm Formula to simplify
	 * @return Simplified formula
	 */
	static tref on (tref fm) {
		DBG(LOG_DEBUG << "Syntactic_path_simplification on " << LOG_FM(fm) << "\n";)
		tref res = nullptr;
		if (tau::get(fm).is_term()) {
			if (tau::get(fm).equals_0() || tau::get(fm).equals_1())
				return fm;
			// Resolve contradictions
			fm = push_negation_in<node, false>(fm);
			fm = simplify_bf(fm);
			// Resolve tautologies
			fm = push_negation_in<node, false>(tau::build_bf_neg(fm));
			fm = simplify_bf(fm);
			res = push_negation_in<node, false>(tau::build_bf_neg(fm));
		} else {
			if (tau::get(fm).equals_F() || tau::get(fm).equals_T())
				return fm;
			// Resolve contradictions
			fm = normalize_atomic_formula_operators<node>(to_nnf<node>(fm));
			fm = simplify_wff(fm);
			// Resolve tautologies
			fm = normalize_atomic_formula_operators<node>(to_nnf<node>(tau::build_wff_neg(fm)));
			fm = simplify_wff(fm);
			res = to_nnf<node>(tau::build_wff_neg(fm));
		}
		DBG(LOG_DEBUG << "Syntactic_path_simplification result: " << LOG_FM(res) << "\n";)
		return res;
	}

	/**
	 * @brief Only simplify contradictions on paths of formula while not manipulating
	 * present negations.
	 * @param fm Formula to simplify
	 * @return Simplified formula
	 */
	static tref unsat_on_unchanged_negations (tref fm) {
		if (tau::get(fm).is_term()) {
			if (tau::get(fm).equals_0() || tau::get(fm).equals_1())
				return fm;
			// Resolve contradictions
			return simplify_bf(fm);
		} else {
			// Resolve contradiction
			return simplify_wff(fm);
		}
	}
};

/**
 * @brief Function to apply syntactical simplifications to formula in almost
 * linear time in the formula size and the number of paths found in terms
 * @tparam node tree node type
 * @param formula The formula to simplify
 * @param skip Predicate identifying content this pass must not touch
 *        (defaults to BV-typed nodes); accepted for interface consistency
 *        with `anti_prenex_block`'s other steps, but currently unused --
 *        neither `simplify_using_equality_dnf` nor
 *        `syntactic_path_simplification_dnf` has a BV-specific check to guard.
 * @return The simplified formula
 */
template<NodeType node>
tref syntactic_formula_simplification(tref formula,
		[[maybe_unused]] std::function<bool(tref)> skip) {
	formula = simplify_using_equality_dnf<node>::on(formula);
	return syntactic_path_simplification_dnf<node>::on(formula);
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
		} else return false; // compare equal
	} else return false; // compare equal
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
	DBG(using tau = tree<node>;)
	DBG(assert(tau::get(l).child_is(tau::bf_eq));)
	DBG(assert(tau::get(r).child_is(tau::bf_eq));)
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
		if (free_vars_l < free_vars_r) return true;
		return false;
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
	tref func1 = syntactic_path_simplification_dnf<node>::on(
		tau::get(atomic_formula)[0].first());
	tref func2 = syntactic_path_simplification_dnf<node>::on(
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

template <NodeType node>
tref apply_assms(tref eq, const auto& assms, auto& joins, trefs& additions, bool dual = false) {
	using tau = tree<node>;
	DBG(assert(tau::get(eq)[0][1].equals_0());)
	const trefs& fv = get_free_vars<node>(eq);
	DBG(assert(tau::get(eq).child_is(tau::bf_eq) ||
		tau::get(eq).child_is(tau::bf_neq));)
	const bool is_eq_pos = tau::get(eq).child_is(tau::bf_eq);
	bool joined = false;
	for (tref assm : assms.back()) {
		// Check for overlap between current assumption and eq
		const trefs& fv_a = get_free_vars<node>(assm);
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
				}
				eq = tau::build_bf_neq_0(tau::build_bf_and(
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
	const trefs& fv = get_free_vars<node>(eq);
	DBG(assert(tau::get(eq).child_is(tau::bf_eq) ||
		tau::get(eq).child_is(tau::bf_neq));)
	const bool is_eq_pos = tau::get(eq).child_is(tau::bf_eq);
	for (tref assm : assms.back()) {
		// Check for overlap between current assumption and eq
		const trefs& fv_a = get_free_vars<node>(assm);
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
					const trefs& fv = get_free_vars<node>(conj);
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
				const trefs& fv1 = get_free_vars<node>(conjs[i]);
				if (fv1.empty()) continue;
				for (size_t j = i+1; j < pos_eq_idx; ++j) {
					// Get free variables at i + 1
					const trefs& fv2 = get_free_vars<node>(conjs[j]);
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
					const trefs& fv = get_free_vars<node>(disj);
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
				const trefs& fv1 = get_free_vars<node>(disjs[i]);
				if (fv1.empty()) continue;
				for (size_t j = i+1; j < neg_eq_idx; ++j) {
					// Get free variables at i + 1
					const trefs& fv2 = get_free_vars<node>(disjs[j]);
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
	// Disable intermediate simplifications for the moment
	tau::use_hooks = false;
	tref res = pre_order<node>(formula).apply(f, visit, up);
	DBG(assert(assms.size() == 1);)
	DBG(assert(dual_assms.size() == 1);)
	// Re-enable intermediate simplifications
	tau::use_hooks = true;
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
	// Disable intermediate simplifications for the moment
	tau::use_hooks = false;
	tref res = pre_order<node>(formula).apply(f, visit, up);
	DBG(assert(assms.size() == 1);)
	DBG(assert(dual_assms.size() == 1);)
	// Re-enable intermediate simplifications
	tau::use_hooks = true;
	DBG(LOG_DEBUG << "Ended squeeze_absorb");
	return tau::reget(res);
}

} // namespace idni::tau_lang
