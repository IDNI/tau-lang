// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// normal_forms_boole.tmpl.h - Boole decomposition, anti_prenex, quantifier elimination
// Split from normal_forms.tmpl.h for readability.

namespace idni::tau_lang {


/**
 * @brief Do a single Boole decomposition step on term given the provided variable.
 * Assumes that the decomposition is valid.
 * @tparam node Tree node type
 * @param term The term on which to apply Boole decomposition step
 * @param var The variable on which to do Boole decomposition
 * @return The resulting term
 */
template<NodeType node>
tref term_boole_decomposition(tref term, tref var) {
	using tau = tree<node>;
	DBG(assert(tau::get(var).is(tau::variable));)
	if (tau::get(term).find_top(is_non_boolean_term<node>)) {
		DBG(LOG_TRACE << "term_boole_decomposition/Non boolean term: "
			<< tau::get(term) << "\n");
		return term;
	}
	var = tau::get(tau::bf, var);
	tref p1 = tau::get(term).replace(var, tau::_1(find_ba_type<node>(var)));
	// Ensure early detection of F
	p1 = syntactic_path_simplification_unsat_on_unchanged_negations<node>(p1);
	tref p2 = tau::get(term).replace(var, tau::_0(find_ba_type<node>(var)));
	// Ensure early detection of F
	p2 = syntactic_path_simplification_unsat_on_unchanged_negations<node>(p2);
	if (tau::get(p1) == tau::get(p2)) return p1;
	// Build Boole decomposition step
	return tau::build_bf_or(
		tau::build_bf_and(var, p1), tau::build_bf_and(tau::build_bf_neg(var), p2)
	);
}

// Note: Recursion depth is bound by the number of variables, which should
// prevent a stack overflow due to tree size in all use cases
/**
 * @brief Recursively do Boole decomposition on term using the provided variables
 * starting at idx.
 * Assumes that the decomposition is valid for all provided variables.
 * @tparam node Tree node type
 * @param term Term on which to do Boole decomposition
 * @param vars The variables to do Boole decomposition on
 * @param idx The current variable index
 * @return The resulting Boole decomposition
 */
template<NodeType node>
tref rec_term_boole_decomposition(tref term, const trefs& vars, const int_t idx,
	const bool free_funcs = false) {
	using tau = tree<node>;
	DBG(LOG_TRACE << "Step on " << LOG_FM(term) << "\n";)
	if (tau::get(term).equals_0()) return term;
	if (tau::get(term).equals_1()) return term;
	if (idx == (int_t)vars.size()) {
		if (!free_funcs) {
			term = normalize_ba<node>(term);
			auto func_syms = tau::get(term).select_top(is<node, tau::bf_ref>);
			if (!func_syms.empty()) {
				std::ranges::sort(func_syms, tau::subtree_less);
				term = rec_term_boole_decomposition<node>(term, func_syms, 0, true);
			}
		}
		DBG(LOG_TRACE << "Result: " << LOG_FM(term) << "\n";)
		return term;
	}
	DBG(assert(tau::get(vars[idx]).is(tau::variable) || tau::get(vars[idx]).is(tau::bf_ref));)
	tref p1 = tau::get(term).replace(vars[idx], tau::_1_trimmed(find_ba_type<node>(vars[idx])));
	// Ensure early detection of F
	p1 = syntactic_path_simplification_unsat_on_unchanged_negations<node>(p1);
	tref p2 = tau::get(term).replace(vars[idx], tau::_0_trimmed(find_ba_type<node>(vars[idx])));
	// Ensure early detection of F
	p2 = syntactic_path_simplification_unsat_on_unchanged_negations<node>(p2);
	// free_funcs has to be forwarded: without it every leaf of the recursion
	// re-entered the !free_funcs block above -- another normalize_ba, another
	// select_top(bf_ref) and another nested decomposition -- and terminated
	// only because substituting a top-level bf_ref also removes the nested
	// ones, i.e. on an invariant nothing states.
	if (tau::get(p1) == tau::get(p2)) {
		DBG(LOG_TRACE << "Result: " << LOG_FM(p1) << "\n";)
		return rec_term_boole_decomposition<node>(p1, vars, idx + 1,
								free_funcs);
	}
	p1 = rec_term_boole_decomposition<node>(p1, vars, idx + 1, free_funcs);
	p2 = rec_term_boole_decomposition<node>(p2, vars, idx + 1, free_funcs);
	if (tau::get(p1) == tau::get(p2)) {
		DBG(LOG_TRACE << "Result: " << LOG_FM(p1) << "\n";)
		return p1;
	}
	tref var = tau::get(tau::bf, vars[idx]);
	// Build Boole decomposition
	if (tau::get(p1).equals_1())
		term = tau::build_bf_or(var,  p2);
	else if (tau::get(p2).equals_1())
		term = tau::build_bf_or(p1, tau::build_bf_neg(var));
	else term = tau::build_bf_or(tau::build_bf_and(var, p1),
		tau::build_bf_and(tau::build_bf_neg(var), p2));
	DBG(LOG_TRACE << "Result: " << LOG_FM(term) << "\n";)
	return term;
}

/**
 * @brief Convert term to Boole normal form. Also treats normalization of
 * encountered tau constants.
 * @tparam node Tree node type
 * @param term The term to do the Boole decomposition on
 * @return The resulting Boole decomposition
 */
template<NodeType node>
tref term_boole_decomposition(tref term) {
	using tau = tree<node>;

	DBG(LOG_DEBUG << "Term_boole_decomposition on " << LOG_FM(term) << "\n";)
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, tref>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(term); it != cache.end())
		return it->second;
#endif // TAU_CACHE
	if (tau::get(term).find_top(is_non_boolean_term<node>)) {
		DBG(LOG_TRACE << "term_boole_decomposition/Non boolean term: "
			<< tau::get(term) << "\n");
		[[maybe_unused]] const tref orig = term;
		tref simplified = node::ba::simplify_term(term);
		if (!simplified) {
			LOG_ERROR << "term_boole_decomposition: "
				"simplification failed (nullptr)\n";
			return term;
		}
		// Guard: if simplification didn't change the term or still
		// contains non-boolean ops, return as-is to avoid infinite loop.
		if (simplified == term ||
		    tau::get(simplified).find_top(is_non_boolean_term<node>)) {
			LOG_ERROR << "term_boole_decomposition: "
				"simplification could not eliminate "
				"non-boolean term\n";
			return term;
		}
		term = simplified;
		tref res = normalize_ba<node>(term);
		// Cache this branch too: it is the bv/tau-constant path, i.e. the
		// expensive one (simplify_term + normalize_ba), and callers hit it
		// repeatedly through apply_unique_until_change.
#ifdef TAU_CACHE
		cache.emplace(res, res);
		return cache.emplace(orig, res).first->second;
#endif // TAU_CACHE
		return res;
	}
	// Simple cases
	if (tau::get(term).equals_0() || tau::get(term).equals_1())
		return term;
	tref bd = push_negation_in<node, false>(term);
	auto vars = get_free_vars_appearance_order<node>(bd);
	// No free var, so no boole decomposition step
	if (vars.empty()) {
		tref simplified = node::ba::simplify_term(term);
		if (simplified) term = simplified;
		bd = normalize_ba<node>(bd);
		auto func_syms = tau::get(bd).select_top(is<node, tau::bf_ref>);
		std::ranges::sort(func_syms, tau::subtree_less);
		bd = rec_term_boole_decomposition<node>(bd, func_syms, 0);
#ifdef TAU_CACHE
		cache.emplace(bd, bd);
		return cache.emplace(term, bd).first->second;
#endif // TAU_CACHE
		return bd;
	}
	std::ranges::stable_sort(vars, variable_order_for_simplification<node>);
	bd = rec_term_boole_decomposition<node>(bd, vars, 0);
	DBG(LOG_DEBUG << "Term_boole_decomposition result: " << LOG_FM(bd) << "\n";)
#ifdef TAU_CACHE
	cache.emplace(bd, bd);
	return cache.emplace(term, bd).first->second;
#endif // TAU_CACHE
	return bd;
}

// Note: Recursion depth is bound by the number of variables, which should
// prevent a stack overflow due to tree size in all use cases
 /**
 * @brief Recursively do Boole decomposition on formula using the provided variables
 * starting at idx.
 * Assumes that the decomposition is valid for all provided variables.
 * @tparam node Tree node type
 * @param formula The formula to do Boole decomposition on
 * @param vars The variable to perform the Boole decomposition on
 * @param idx The current variable index
 * @return The resulting Boole decomposition
 */
template<NodeType node>
tref rec_boole_decomposition(tref formula, const trefs& vars, const int_t idx) {
	using tau = tree<node>;
	DBG(LOG_TRACE << "Step on " << LOG_FM(formula) << "\n";)
	if (tau::get(formula).equals_F()) return formula;
	if (tau::get(formula).equals_T()) return formula;
	if (idx == (int_t)vars.size()) {
		DBG(LOG_TRACE << "Result: " << LOG_FM(formula) << "\n";)
		return formula;
	}
	// Same three atom kinds as boole_normal_form's is_atomic: decomposing on
	// an order atom treats it as an opaque Boolean variable, which is sound
	// and is what Release has always done here (the assert is DBG-only).
	DBG(assert(is_atomic_bdd_var<node>(vars[idx]));)
	tref p1 = tau::get(formula).replace(vars[idx], tau::_T());
	// Ensure early detection of F
	p1 = syntactic_path_simplification_unsat_on_unchanged_negations<node>(p1);
	tref p2 = tau::get(formula).replace(vars[idx], tau::_F());
	// Ensure early detection of F
	p2 = syntactic_path_simplification_unsat_on_unchanged_negations<node>(p2);
	if (tau::get(p1) == tau::get(p2)) {
		DBG(LOG_TRACE << "Result: " << LOG_FM(p1) << "\n";)
		return rec_boole_decomposition<node>(p1, vars, idx + 1);
	}
	p1 = rec_boole_decomposition<node>(p1, vars, idx + 1);
	p2 = rec_boole_decomposition<node>(p2, vars, idx + 1);
	if (tau::get(p1) == tau::get(p2)) {
		DBG(LOG_TRACE << "Result: " << LOG_FM(p1) << "\n";)
		return p1;
	}
	// Build Boole decomposition
	if (tau::get(p1).equals_T())
		formula = tau::build_wff_or(vars[idx], p2);
	else if (tau::get(p2).equals_T())
		formula = tau::build_wff_or(p1, tau::build_wff_neg(vars[idx]));
	else formula = tau::build_wff_or(tau::build_wff_and(vars[idx], p1),
		tau::build_wff_and(tau::build_wff_neg(vars[idx]), p2));
	DBG(LOG_TRACE << "Result: " << LOG_FM(formula) << "\n";)
	return formula;
}

/**
 * This procedure converts the formula to Boole normal form. It also converts all
 * terms to Boole normal form.
 * @tparam node Tree node type
 * @param bnf The formula to convert to Boole normal form
 * @return The resulting Boole normal form
 */
template<NodeType node>
tref boole_normal_form(tref formula) {
	using tau = tree<node>;
	DBG(LOG_DEBUG << "Boole_normal_form on " << LOG_FM(formula) << "\n";)
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, tref>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(formula); it != cache.end())
		return it->second;
#endif // TAU_CACHE
	if (tau::get(formula).equals_T() || tau::get(formula).equals_F())
		return formula;
	// Step 1: Syntactically simplify formula
	tref bnf = syntactic_formula_simplification<node>(formula);
	DBG(LOG_DEBUG << "After syntactic_formula_simplification: " << LOG_FM(bnf) << "\n";)
	// Squeeze and absorb for additional simplifications during term normalization
	// -> causes mayor blow ups
	bnf = squeeze_absorb<node>(bnf);
	// Step 2: Traverse formula, simplify all encountered equations
	auto simp_eqs = [](tref n) {
		if (tau::get(n).child_is(tau::bf_eq)) {
			if (tau::get(n).equals_T() || tau::get(n).equals_F())
				return n;
			tref c1 = tau::get(n)[0].first();
			tref c2 = tau::get(n)[0].second();
			// Apply Boole decomposition
			c1 = term_boole_decomposition<node>(c1);
			c2 = term_boole_decomposition<node>(c2);
			return tau::build_bf_eq(c1, c2);
		} else if (tau::get(n).child_is(tau::bf_neq)) {
			if (tau::get(n).equals_T() || tau::get(n).equals_F())
				return n;
			tref c1 = tau::get(n)[0].first();
			tref c2 = tau::get(n)[0].second();
			// Apply Boole decomposition
			c1 = term_boole_decomposition<node>(c1);
			c2 = term_boole_decomposition<node>(c2);
			return tau::build_bf_neq(c1, c2);
		}
		return n;
	};
	bnf = pre_order<node>(bnf).apply_unique_until_change(simp_eqs, visit_wff<node>);
	DBG(LOG_DEBUG << "After term_boole_decomposition: " << LOG_FM(bnf) << "\n";)
	// Step 3: Syntactically simplify resulting formula again after normalization of terms
	bnf = syntactic_formula_simplification<node>(bnf);
	DBG(LOG_DEBUG << "After syntactic_formula_simplification: " << LOG_FM(bnf) << "\n";)
	// Step 4: Convert formula to Boole normal form
	// First get atomic formulas without !=
	tref eq_bnf = normalize_atomic_formula_operators<node>(bnf);
	trefs atms = rewriter::select_top_until<node>(eq_bnf,
		is_atomic_bdd_var<node>, is_quantifier<node>);
	// No variables for Boole decomposition
	if (atms.empty()) {
#ifdef TAU_CACHE
		cache.emplace(bnf, bnf);
		return cache.emplace(formula, bnf).first->second;
#endif // TAU_CACHE
		return bnf;
	}
	// Sort the BDD variables
	std::ranges::stable_sort(atms, atm_formula_order_for_simplification<node>);
	// Apply Boole decomposition
	eq_bnf = rec_boole_decomposition<node>(eq_bnf, atms, 0);
	// Convert !(=) to != again
	eq_bnf = to_nnf<node>(eq_bnf);
	eq_bnf = simplify_using_equality<node>(eq_bnf);
	DBG(LOG_DEBUG << "Boole_normal_form result: " << LOG_FM(eq_bnf) << "\n";)
#ifdef TAU_CACHE
	cache.emplace(eq_bnf, eq_bnf);
	return cache.emplace(formula, eq_bnf).first->second;
#endif // TAU_CACHE
	return eq_bnf;
}

template<NodeType node>
tref term_boole_normal_form(tref formula) {
	using tau = tree<node>;
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, tref>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(formula); it != cache.end())
		return it->second;
#endif // TAU_CACHE
	if (tau::get(formula).equals_T() || tau::get(formula).equals_F())
		return formula;
	// Step 1: Syntactically simplify formula
	tref tbnf = syntactic_formula_simplification<node>(formula);
	DBG(LOG_DEBUG << "After syntactic_formula_simplification: " << LOG_FM(tbnf) << "\n";)
	auto simp_eqs = [](tref n) {
		if (tau::get(n).child_is(tau::bf_eq)) {
			if (tau::get(n).equals_T() || tau::get(n).equals_F())
				return n;
			tref c1 = tau::get(n)[0].first();
			tref c2 = tau::get(n)[0].second();
			// Apply Boole decomposition
			c1 = term_boole_decomposition<node>(c1);
			c2 = term_boole_decomposition<node>(c2);
			return tau::build_bf_eq(c1, c2);
		} else if (tau::get(n).child_is(tau::bf_neq)) {
			if (tau::get(n).equals_T() || tau::get(n).equals_F())
				return n;
			tref c1 = tau::get(n)[0].first();
			tref c2 = tau::get(n)[0].second();
			// Apply Boole decomposition
			c1 = term_boole_decomposition<node>(c1);
			c2 = term_boole_decomposition<node>(c2);
			return tau::build_bf_neq(c1, c2);
		}
		return n;
	};
	tbnf = pre_order<node>(tbnf).apply_unique_until_change(simp_eqs, visit_wff<node>);
	DBG(LOG_DEBUG << "After term_boole_decomposition: " << LOG_FM(tbnf) << "\n";)
	// Step 3: Syntactically simplify resulting formula again after normalization of terms
	tbnf = syntactic_formula_simplification<node>(tbnf);
	DBG(LOG_DEBUG << "After syntactic_formula_simplification: " << LOG_FM(tbnf) << "\n";)
#ifdef TAU_CACHE
	cache.emplace(tbnf, tbnf);
	return cache.emplace(formula, tbnf).first->second;
#endif // TAU_CACHE
	return tbnf;
}

// DLO interval computation for (Q,<).
// Collects the satisfying interval I for ∃var. body where body is a
// conjunction of DLO comparison atoms "var op {c}:qlt" (c finite singleton).
// Returns the interval if determined, nullopt if undetermined.
//   ∃var. body satisfiable  ↔  !result.is_empty()
//   ∀var. body tautology    ↔   result.is_full()
template<NodeType node>
static std::optional<qlt> qlt_dlo_qe(tref var, tref body) {
	using tau = tree<node>;
	if constexpr (ba_variant_includes_v<qlt, typename tau::constant>) {
		qlt acc = qlt::top();
		bool undetermined = false;
		// Free-variable endpoint bounds (symbolic DLO reasoning).
		// Used to detect contradictions like (a < var && var < a) where a is free.
		// Keys: the RHS tree (the free-var side of the constraint).
		subtree_set<node> lower_strict;    // { fv : fv < var }
		subtree_set<node> lower_nonstrict; // { fv : fv <= var }
		subtree_set<node> upper_strict;    // { fv : var < fv }
		subtree_set<node> upper_nonstrict; // { fv : var <= fv }
		subtree_set<node> eq_free;         // { fv : var = fv }
		std::function<void(tref)> collect = [&](tref n) {
			if (!n || undetermined || acc.is_empty()) return;
			const auto& t = tau::get(n);
			if (t.equals_T()) return;
			if (t.equals_F()) { acc = qlt::bottom(); return; }
			if (!t.is(tau::wff)) { undetermined = true; return; }
			if (!t.has_child()) return;
			auto op = t[0].value.nt;
			if (op == tau::wff_and) {
				collect(t[0].first());
				collect(t[0].second());
				return;
			}
			// Helper lambda: given (raw_op, lhs, rhs, negate), accumulate the
			// corresponding DLO interval into acc. raw_op is the comparison
			// operator before direction-flip and optional negation.
			auto accumulate_interval = [&](size_t raw_op, tref lhs_t, tref rhs_t, bool negate) {
				bool var_in_lhs = contains<node>(lhs_t, var);
				bool var_in_rhs = contains<node>(rhs_t, var);
				if (!var_in_lhs && !var_in_rhs) return;
				if (var_in_lhs && var_in_rhs) { undetermined = true; return; }
				// var must be the only free variable on its side.
				// A compound like (o1 & i1) introduces i1 as an extra free var
				// and cannot be treated as a simple DLO constraint on o1.
				tref var_side = var_in_lhs ? lhs_t : rhs_t;
				for (tref sv : get_free_vars<node>(var_side))
					if (tau::get(sv) != tau::get(var)) { undetermined = true; return; }
				const auto& cst = tau::get(var_in_lhs ? rhs_t : lhs_t)[0];
				if (!cst.is_ba_constant()) {
					// Handle typed zero (bf_f = -∞) and typed one (bf_t = +∞) as DLO bounds
					if (cst.is(tau::bf_f) || cst.is(tau::bf_t)) {
						bool cst_is_min = cst.is(tau::bf_f); // bf_f = -∞, bf_t = +∞
						// bf_eq / bf_neq against a sentinel (bf_f/bf_t) don't fit the
						// open-interval DLO model — they assert the variable IS (or
						// ISN'T) the sentinel.  Return undetermined so the caller
						// falls through to BA-level satisfiability, which can
						// correctly discharge var = bf_t / var = bf_f by ∃-substitution.
						if (raw_op == tau::bf_eq || raw_op == tau::bf_neq) {
							undetermined = true; return;
						}
						// Determine if constraint is trivially satisfied (i.e., no restriction on x)
						// x > -∞, x >= -∞ are trivially true; x < +∞, x <= +∞ are trivially true
						bool trivially_sat;
						if (var_in_lhs) {
							trivially_sat = cst_is_min
								? (raw_op == tau::bf_gt || raw_op == tau::bf_gteq)
								: (raw_op == tau::bf_lt || raw_op == tau::bf_lteq);
						} else {
							// cst op x: -∞ < x, -∞ <= x are trivially true
							trivially_sat = cst_is_min
								? (raw_op == tau::bf_lt || raw_op == tau::bf_lteq)
								: (raw_op == tau::bf_gt || raw_op == tau::bf_gteq);
						}
						if (negate) trivially_sat = !trivially_sat;
						if (trivially_sat) return; // no constraint added
						acc = qlt::bottom(); return; // contradiction
					}
					// ∃x.(x ≠ free_var) ≡ T over DLO for any free_var.
					// bf_neq is symmetric so direction flip doesn't change it.
					if (raw_op == tau::bf_neq && !negate) return;
					// Symbolic free-variable endpoints: classify into lower/upper
					// bounds so we can detect contradictions like a<var && var<a.
					// Determine effective direction (normalise to var <op> fv).
					tref fv_tree = var_in_lhs ? rhs_t : lhs_t;
					auto eff_op = raw_op;
					if (var_in_rhs) {
						if      (raw_op == tau::bf_lt)   eff_op = tau::bf_gt;
						else if (raw_op == tau::bf_gt)   eff_op = tau::bf_lt;
						else if (raw_op == tau::bf_lteq) eff_op = tau::bf_gteq;
						else if (raw_op == tau::bf_gteq) eff_op = tau::bf_lteq;
					}
					if (negate) {
						if      (eff_op == tau::bf_lt)   eff_op = tau::bf_gteq;
						else if (eff_op == tau::bf_gt)   eff_op = tau::bf_lteq;
						else if (eff_op == tau::bf_lteq) eff_op = tau::bf_gt;
						else if (eff_op == tau::bf_gteq) eff_op = tau::bf_lt;
						else if (eff_op == tau::bf_eq)   eff_op = tau::bf_neq;
						else if (eff_op == tau::bf_neq)  eff_op = tau::bf_eq;
					}
					// eff_op is now in "var <op> fv" form.
					if      (eff_op == tau::bf_lt)    upper_strict.insert(fv_tree);
					else if (eff_op == tau::bf_lteq)  upper_nonstrict.insert(fv_tree);
					else if (eff_op == tau::bf_gt)    lower_strict.insert(fv_tree);
					else if (eff_op == tau::bf_gteq)  lower_nonstrict.insert(fv_tree);
					else if (eff_op == tau::bf_eq)    eq_free.insert(fv_tree);
					else if (eff_op == tau::bf_neq)   { /* var != fv: always sat over DLO */ }
					else { undetermined = true; }
					return;
				}
				auto cv = cst.get_ba_constant();
				if (!std::holds_alternative<qlt>(cv)) { undetermined = true; return; }
				const qlt& qba = std::get<qlt>(cv);
				if (qba.pieces.size() != 1) { undetermined = true; return; }
				const auto& piece = qba.pieces[0];
				if (piece.lo.val != piece.hi.val) { undetermined = true; return; }
				if (!piece.lo.val.is_finite()) { undetermined = true; return; }
				const qlt_rational& c = piece.lo.val;
				auto eff_op = raw_op;
				if (var_in_rhs) {
					if      (raw_op == tau::bf_lt)   eff_op = tau::bf_gt;
					else if (raw_op == tau::bf_gt)   eff_op = tau::bf_lt;
					else if (raw_op == tau::bf_lteq) eff_op = tau::bf_gteq;
					else if (raw_op == tau::bf_gteq) eff_op = tau::bf_lteq;
				}
				if (negate) {
					if      (eff_op == tau::bf_lt)   eff_op = tau::bf_gteq;
					else if (eff_op == tau::bf_gt)   eff_op = tau::bf_lteq;
					else if (eff_op == tau::bf_lteq) eff_op = tau::bf_gt;
					else if (eff_op == tau::bf_gteq) eff_op = tau::bf_lt;
					else if (eff_op == tau::bf_eq)   eff_op = tau::bf_neq;
					else if (eff_op == tau::bf_neq)  eff_op = tau::bf_eq;
				}
				qlt interval;
				auto neg_inf = qlt_rational::make_neg_inf();
				auto pos_inf = qlt_rational::make_pos_inf();
				qlt_piece p;
				if (eff_op == tau::bf_lt) {
					p.lo = qlt_endpoint{neg_inf, qlt_bound::OPEN};
					p.hi = qlt_endpoint{c, qlt_bound::OPEN};
					interval = qlt{{p}};
				} else if (eff_op == tau::bf_lteq) {
					p.lo = qlt_endpoint{neg_inf, qlt_bound::OPEN};
					p.hi = qlt_endpoint{c, qlt_bound::CLOSED};
					interval = qlt{{p}};
				} else if (eff_op == tau::bf_gt) {
					p.lo = qlt_endpoint{c, qlt_bound::OPEN};
					p.hi = qlt_endpoint{pos_inf, qlt_bound::OPEN};
					interval = qlt{{p}};
				} else if (eff_op == tau::bf_gteq) {
					p.lo = qlt_endpoint{c, qlt_bound::CLOSED};
					p.hi = qlt_endpoint{pos_inf, qlt_bound::OPEN};
					interval = qlt{{p}};
				} else if (eff_op == tau::bf_eq) {
					p.lo = qlt_endpoint{c, qlt_bound::CLOSED};
					p.hi = qlt_endpoint{c, qlt_bound::CLOSED};
					interval = qlt{{p}};
				} else if (eff_op == tau::bf_neq) {
					qlt_piece p1, p2;
					p1.lo = qlt_endpoint{neg_inf, qlt_bound::OPEN};
					p1.hi = qlt_endpoint{c, qlt_bound::OPEN};
					p2.lo = qlt_endpoint{c, qlt_bound::OPEN};
					p2.hi = qlt_endpoint{pos_inf, qlt_bound::OPEN};
					interval = qlt{{p1, p2}};
				} else { undetermined = true; return; }
				acc = acc & interval;
			};
			// Handle negated comparison: ¬(var op c) → var op' c
			if (op == tau::wff_neg) {
				tref inner = t[0].first();
				const auto& ti = tau::get(inner);
				if (!ti.is(tau::wff) || !ti.has_child()) {
					if (contains<node>(n, var)) undetermined = true;
					return;
				}
				auto iop = ti[0].value.nt;
				// Normalize NNF negated-comparison variants (bf_nXxx → positive)
				if      (iop == tau::bf_ngt)   iop = tau::bf_lteq;
				else if (iop == tau::bf_nlt)   iop = tau::bf_gteq;
				else if (iop == tau::bf_ngteq) iop = tau::bf_lt;
				else if (iop == tau::bf_nlteq) iop = tau::bf_gt;
				if (iop != tau::bf_lt  && iop != tau::bf_lteq &&
				    iop != tau::bf_gt  && iop != tau::bf_gteq &&
				    iop != tau::bf_eq  && iop != tau::bf_neq) {
					if (contains<node>(n, var)) undetermined = true;
					return;
				}
				accumulate_interval(iop, ti[0].first(), ti[0].second(), true);
				return;
			}
			// NNF converts ¬(x op c) to bf_nXxx(x,c). Normalize to positive form.
			if      (op == tau::bf_ngt)   op = tau::bf_lteq;
			else if (op == tau::bf_nlt)   op = tau::bf_gteq;
			else if (op == tau::bf_ngteq) op = tau::bf_lt;
			else if (op == tau::bf_nlteq) op = tau::bf_gt;
			if (op != tau::bf_lt  && op != tau::bf_lteq  &&
			    op != tau::bf_gt  && op != tau::bf_gteq  &&
			    op != tau::bf_eq  && op != tau::bf_neq) {
				if (contains<node>(n, var)) undetermined = true;
				return;
			}
			// Comparison atom: wff(bf_op(lhs_bf, rhs_bf))
			accumulate_interval(op, t[0].first(), t[0].second(), false);
		};
		collect(body);
		// Symbolic contradiction detection via free-variable endpoints.
		// (fv < var && var < fv)  → empty (fv < fv impossible).
		// (fv <= var && var < fv) → empty (fv <= var < fv impossible).
		// (fv < var && var <= fv) → empty.
		// (var = fv1 && var = fv2) with fv1 != fv2 syntactically → empty.
		// (var = fv && var < fv) → empty.
		// (var = fv && var > fv) → empty.
		auto has_same = [](const subtree_set<node>& a, const subtree_set<node>& b) {
			for (tref t : a) if (b.contains(t)) return true;
			return false;
		};
		if (has_same(lower_strict, upper_strict)
		 || has_same(lower_strict, upper_nonstrict)
		 || has_same(lower_nonstrict, upper_strict)
		 || has_same(lower_strict, eq_free)
		 || has_same(upper_strict, eq_free))
			acc = qlt::bottom();
		// Two distinct equalities to different free vars → contradictory iff
		// we can't prove they're equal. Stay undetermined in this case.
		if (eq_free.size() > 1) undetermined = true;
		// AN-1: symbolic endpoints are recorded above but never
		// intersected into `acc`, so a non-empty verdict is sound only
		// when they cannot shrink the interval to empty. A one-sided
		// symbolic bound is harmless over (Q,<) iff `acc` is unbounded
		// on that side (a point beyond any finite set of free endpoints
		// always exists). Anything whose truth depends on the ORDER of
		// free endpoints -- bounds on both sides, or an equality
		// combined with any other constraint -- is undetermined, not T:
		// `ex x (a < x && x < b)` is `a < b`, false at a = b.
		if (!undetermined && !acc.is_empty()) {
			const bool has_lower = !lower_strict.empty()
						|| !lower_nonstrict.empty();
			const bool has_upper = !upper_strict.empty()
						|| !upper_nonstrict.empty();
			if (!eq_free.empty()) {
				if (has_lower || has_upper || !acc.is_full())
					undetermined = true;
			} else if (has_lower && has_upper) {
				// Exception: a non-strict cycle through one
				// shared endpoint (fv <= var && var <= fv) is
				// satisfied by var := fv for ANY fv, provided
				// nothing else restricts var. Any other
				// two-sided combination depends on the free
				// endpoints' order.
				if (!(lower_strict.empty()
					&& upper_strict.empty()
					&& lower_nonstrict.size() == 1
					&& upper_nonstrict.size() == 1
					&& upper_nonstrict.contains(
						*lower_nonstrict.begin())
					&& acc.is_full()))
					undetermined = true;
			}
			else if (has_lower && !acc.pieces.back()
						.hi.val.is_pos_inf())
				undetermined = true;
			else if (has_upper && !acc.pieces.front()
						.lo.val.is_neg_inf())
				undetermined = true;
		}
		if (undetermined) {
			// If we already derived an empty interval symbolically, prefer that
			// (it is a definitive answer; the BA fallback would wrongly say SAT).
			if (acc.is_empty()) return acc;
			return std::nullopt;
		}
		return acc;
	}
	return std::nullopt;
}

/**
 * @brief Converts the temporal layer of a formula to reduced DNF, squeezes the always
 * statements and ensures that formulas containing temporal variables are
 * explicitly quantified while non-temporal formulas are not quantified temporally.
 * @tparam node Tree node type
 * @tparam normalize_scopes If true, temporally quantified formulas are converted to Boole normal form
 * @param fm The formula that is to be temporally normalized
 * @return The resulting formula after normalizing the temporal quantifiers
 */
template <NodeType node, bool normalize_scopes>
tref normalize_temporal_quantifiers(tref fm) {
	using tau = tree<node>;
	auto norm = [](tref arg) {
		return normalize_scopes
					? term_boole_normal_form<node>(arg)
					: arg;
	};
	auto st_aw = [](tref n) {
		return is_child<node>(n, tau::wff_sometimes)
			|| is_child<node>(n, tau::wff_always);
	};
	auto rm_temp_quant = [&st_aw](tref n) {
		if (st_aw(n)) return tau::trim2(n);
		return n;
	};
	// Full LTL operators (F, U, R, W) manage their own temporal scope;
	// do not wrap them in wff_always — pass through unchanged.
	auto is_ltl_op_node = [](tref n) {
		const auto& t = tree<node>::get(n);
		if (!t.has_child()) return false;
		auto nt = t[0].value.nt;
		return nt == tree<node>::wff_F || nt == tree<node>::wff_U
		    || nt == tree<node>::wff_R || nt == tree<node>::wff_W
		    || nt == tree<node>::wff_S || nt == tree<node>::wff_T;
	};
	if (tau::get(fm).find_top(is_ltl_op_node)) return fm;
	if (has_temp_var<node>(fm)) {
		const bool has_temp_quant = tau::get(fm).find_top(st_aw);
		if (has_temp_quant) {
			// By assumption, all temporal variables are explicitly
			// quantified by temporal quantifier without nesting.
			// DNF conversion is only done on temporal level
			fm = temporal_layer_to_dnf<node>(fm);
			// Simplify temporal layer
			fm = reduce<node>(fm);
			trefs clauses = get_dnf_wff_clauses<node>(fm);
			tref non_temp_clauses = tau::_F();
			tref res = tau::_F();
			for (tref clause : clauses) {
				if (!has_temp_var<node>(clause)) {
					// Remove all temporal quantifiers
					clause = pre_order<node>(clause).
							apply_unique(rm_temp_quant);
					non_temp_clauses = tau::build_wff_or(
						non_temp_clauses, clause);
					continue;
				}
				tref always_part = tau::_T();
				tref staying = tau::_T();
				// In each clause squeeze all always statements
				for (tref conj : get_cnf_wff_clauses<node>(clause)) {
					// All parts are temporally quantified
					// NOTE: conjuncts may be temporal without G/F wrapping (e.g. bare
					// constraints like [t>0], S/T operators, pointwise revision output);
					// the code below handles them correctly so the assertion is removed.
					if (!has_temp_var<node>(conj))
						always_part = tau::build_wff_and(
							always_part, rm_temp_quant(conj));
					// TODO: always conjunction is inefficient
					else if (!is_child<node>(conj, tau::wff_sometimes))
						always_part = always_conjunction<node>(
							always_part, conj);
					else staying = tau::build_wff_and(
						staying,
						tau::build_wff_sometimes(
							norm(tau::trim2(conj))));
				}
				always_part = tau::build_wff_always(norm(always_part));
				clause = tau::build_wff_and(always_part, staying);
				res = tau::build_wff_or(res, clause);
			}
			if (!tau::get(non_temp_clauses).equals_F()) {
				non_temp_clauses = tau::build_wff_always(
					norm(non_temp_clauses));
				res = tau::build_wff_or(res, non_temp_clauses);
			}
			return res;
		} else {
			// Temporal variable without temporal quantifier
			// By assumption we quantify fm universally
			return build_wff_always<node>(norm(fm));
		}
	} else {
		// No temporal variable, so no temporal quantifier needed
		fm = pre_order<node>(fm).apply_unique(rm_temp_quant);
		return norm(fm);
	}
}

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "to_snf"


template <NodeType node>
tref build_split_wff_using(typename node::type type, tref a, tref b) {
	using tau = tree<node>;
	// TODO (HIGH) check formulas, should depend on the type
	if (type == tau::bf_eq)
		return tau::build_wff_or(
			tau::build_wff_and(a, b),
			tau::build_wff_and(tau::build_wff_neg(a),
					   tau::build_wff_neg(b)));
	else return tau::build_wff_and(
		tau::build_wff_or(a, tau::build_wff_neg(b)),
		tau::build_wff_or(tau::build_wff_neg(a), b));
}

template <NodeType node, size_t type>
tref anf(tref n) {
	// ANF (Algebraic Normal Form / Zhegalkin polynomial):
	// XOR-of-AND representation. For BF: rewrite using p|q = p^q^(p&q)
	// and eliminate double negations.
	// TODO (MEDIUM) implement full ANF conversion
	LOG_ERROR << "ANF (Algebraic Normal Form) conversion is not yet "
		"implemented. Use 'dnf' or 'cnf' as an alternative.";
	return n;
}

template <NodeType node>
tref pnf(tref n) {
	// PNF (Prenex Normal Form): pull all quantifiers (all/ex) to the
	// outermost scope. Requires renaming bound variables to avoid capture.
	// TODO (MEDIUM) implement full PNF conversion
	LOG_ERROR << "PNF (Prenex Normal Form) conversion is not yet "
		"implemented. Use 'nnf' or 'qelim' as an alternative.";
	return n;
}

} // namespace idni::tau_lang
