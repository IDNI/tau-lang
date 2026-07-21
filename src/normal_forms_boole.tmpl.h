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
	p1 = syntactic_path_simplification_dnf<node>::unsat_on_unchanged_negations(p1);
	tref p2 = tau::get(term).replace(var, tau::_0(find_ba_type<node>(var)));
	// Ensure early detection of F
	p2 = syntactic_path_simplification_dnf<node>::unsat_on_unchanged_negations(p2);
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
	p1 = syntactic_path_simplification_dnf<node>::unsat_on_unchanged_negations(p1);
	tref p2 = tau::get(term).replace(vars[idx], tau::_0_trimmed(find_ba_type<node>(vars[idx])));
	// Ensure early detection of F
	p2 = syntactic_path_simplification_dnf<node>::unsat_on_unchanged_negations(p2);
	if (tau::get(p1) == tau::get(p2)) {
		DBG(LOG_TRACE << "Result: " << LOG_FM(p1) << "\n";)
		return rec_term_boole_decomposition<node>(p1, vars, idx + 1);
	}
	p1 = rec_term_boole_decomposition<node>(p1, vars, idx + 1);
	p2 = rec_term_boole_decomposition<node>(p2, vars, idx + 1);
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
	DBG(LOG_DEBUG << "Term_boole_decomposition result: " << LOG_FM(term) << "\n";)
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
	DBG(assert(tau::get(vars[idx]).child_is(tau::bf_eq));)
	tref p1 = tau::get(formula).replace(vars[idx], tau::_T());
	// Ensure early detection of F
	p1 = syntactic_path_simplification_dnf<node>::unsat_on_unchanged_negations(p1);
	tref p2 = tau::get(formula).replace(vars[idx], tau::_F());
	// Ensure early detection of F
	p2 = syntactic_path_simplification_dnf<node>::unsat_on_unchanged_negations(p2);
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
	DBG(LOG_DEBUG << "After syntactic_formula_simplification: " << LOG_FM(formula) << "\n";)
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
	DBG(LOG_DEBUG << "After term_boole_decomposition: " << LOG_FM(formula) << "\n";)
	// Step 3: Syntactically simplify resulting formula again after normalization of terms
	bnf = syntactic_formula_simplification<node>(bnf);
	DBG(LOG_DEBUG << "After syntactic_formula_simplification: " << LOG_FM(formula) << "\n";)
	// Step 4: Convert formula to Boole normal form
	// First get atomic formulas without !=
	auto is_atomic = [](tref n) {
		if (!tau::get(n).is(tau::wff)) return false;
		const tau& c = tau::get(n)[0];
		switch (c.value.nt) {
			case tau::bf_eq:
			case tau::bf_lt:
			case tau::bf_lteq: return true;
			default: return false;
		}
	};
	tref eq_bnf = normalize_atomic_formula_operators<node>(bnf);
	trefs atms = rewriter::select_top_until<node>(eq_bnf,
		is_atomic, is_quantifier<node>);
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
	eq_bnf = simplify_using_equality_dnf<node>::on(eq_bnf);
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
	DBG(LOG_DEBUG << "After syntactic_formula_simplification: " << LOG_FM(formula) << "\n";)
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

/**
 * @brief Performs a Boole decomposition step on the formula with the goal to push
 * the existential quantifier further in. Since the BDD variables are atomic formulas,
 * the procedure investigates the best Boole decomposition in each situation.
 * @tparam node Tree node type
 * @param ex_quant_fm Existentially quantified formula on which to perform Boole decomposition step
 * @param pool The pool of variables that can be used for the Boole decomposition
 * @param quant_pattern Map from quantified variables to their priority used for BDD variable ordering
 * @param excluded Collection of nodes that are not visited by anti prenex algorithm
 * @param no_atms Indicates if there are atomic formulas left to do Boole decomposition on
 * @return The resulting Boole decomposition with the existential quantifier pushed further in
 */
template<NodeType node>
tref ex_quantified_boole_decomposition(tref ex_quant_fm, auto& pool,
	auto& quant_pattern, subtree_set<node>* excluded, bool& no_atms) {
	using tau = tree<node>;
	DBG(assert(!tau::get(ex_quant_fm).find_top(is<node, tau::bf_neq>)));
	auto is_atomic = [](tref n) {
		if (!tau::get(n).is(tau::wff)) return false;
		const tau& c = tau::get(n)[0];
		switch (c.value.nt) {
			case tau::bf_eq:
			case tau::bf_lt:
			case tau::bf_lteq: return true;
			default: return false;
		}
	};
	// Get quantified variable and scoped formula
	tref var = tau::get(tau::bf, tau::trim2(ex_quant_fm));
	tref fm = tau::get(ex_quant_fm)[0].second();
	if (auto eliminated = ex_subs_based_elimination<node>(tau::trim(var), fm); eliminated != fm) {
		DBG(LOG_TRACE << "ex_quantified_boole_decomposition/eliminated: " <<
			LOG_FM(eliminated) << "\n";)
		return eliminated;
	}

	// Get atomic formulas from pool
	auto it = pool.find(ex_quant_fm);
	tref curr_pool = it != pool.end()
				 ? it->second
				 : tau::get(ex_quant_fm)[0].second();
	trefs atms = rewriter::select_top_until<node>(curr_pool,
		is_atomic, is_quantifier<node>);
	if (atms.empty()) {
		// std::cout << "ex_quant_fm: " << tau::get(ex_quant_fm) << "\n";
		no_atms = true;
		if (excluded) excluded->insert(ex_quant_fm);
		return ex_quant_fm;
	}
	// Sort the atomic formulas and get first
	tref atm = *std::ranges::min_element(atms,
		atm_formula_order_for_quant_elim<node>(quant_pattern));
	auto atm_type = tau::get(atm)[0].value.nt;

	// Try syntactic simplifications
	if (atm_type == tau::bf_eq || atm_type == tau::bf_neq) {
	tref func = tau::trim2(norm_equation<node>(atm));
	func = apply_xor_def<node>(func);
	// We use is_boolean_operation to enable the procedure on non-boolean functions
	tref func_v_0 = rewriter::replace_if<node>(func, var, tau::_0(find_ba_type<node>(var)), is_boolean_operation<node>);
	func_v_0 = syntactic_path_simplification_dnf<node>::on(func_v_0);
	tref func_v_1 = rewriter::replace_if<node>(func, var, tau::_1(find_ba_type<node>(var)), is_boolean_operation<node>);
	func_v_1 = syntactic_path_simplification_dnf<node>::on(func_v_1);
	// Check identically zero
	if (tau::get(func_v_0).equals_0() && tau::get(func_v_1).equals_0()) {
		return rewriter::replace<node>(ex_quant_fm, atm, tau::_T());
	}
	// Check identically one
	else if (tau::get(func_v_0).equals_1() && tau::get(func_v_1).equals_1()) {
		return rewriter::replace<node>(ex_quant_fm, atm, tau::_F());
	}
	// Check does not dependent on var
	else if (tau::get(func_v_0) == tau::get(func_v_1) && !contains<node>(func_v_0, var)) {
		tref l = rewriter::replace<node>(fm, atm, tau::_T());
		l = syntactic_path_simplification_dnf<node>::unsat_on_unchanged_negations(l);
		tref r = rewriter::replace<node>(fm, atm, tau::_F());
		r = syntactic_path_simplification_dnf<node>::unsat_on_unchanged_negations(r);
		if (tau::get(l) == tau::get(r)) return tau::build_wff_ex(tau::trim(var), l, false);
		atm = rewriter::replace<node>(atm, var, tau::_0(find_ba_type<node>(var)));
		return tau::build_wff_or(
			tau::build_wff_and(atm, tau::build_wff_ex(tau::trim(var), l, false)),
			tau::build_wff_and(tau::build_wff_neg(atm), tau::build_wff_ex(tau::trim(var), r, false))
			);
	}
	// Check has a unique zero
	func_v_1 = push_negation_in<node, false>(tau::build_bf_neg(func_v_1));
	if (tau::get(func_v_0) == tau::get(func_v_1) && !contains<node>(func_v_0, var)) {
		tref l = rewriter::replace<node>(fm, atm, tau::_T());
		l = rewriter::replace<node>(l, var, func_v_0);
		l = syntactic_path_simplification_dnf<node>::unsat_on_unchanged_negations(l);
		tref r = rewriter::replace<node>(fm, atm, tau::_F());
		r = syntactic_path_simplification_dnf<node>::unsat_on_unchanged_negations(r);
		if (tau::get(l) == tau::get(r)) {
			if (excluded) excluded->insert(l);
			return l;
		}
		tref boole_atm = tau::build_bf_eq(
		term_boole_decomposition<node>(tau::get(atm)[0].first(), tau::trim(var)),
		term_boole_decomposition<node>(tau::get(atm)[0].second(), tau::trim(var))
		);
		tref nr = tau::build_wff_ex(tau::trim(var),
					    tau::build_wff_and(tau::build_wff_neg(boole_atm), r), false);
		pool.insert_or_assign(nr,
			rewriter::replace<node>(curr_pool, atm, tau::_F()));
		atm = rewriter::replace<node>(atm, var, func_v_0);
		atm = tau::build_wff_and(atm, l);
		if (excluded) excluded->insert(atm);
		return tau::build_wff_or(atm, nr);
	}
	}
	// No simplification applied, build Boole decomposition
	tref l = rewriter::replace<node>(fm, atm, tau::_T());
	l = syntactic_path_simplification_dnf<node>::unsat_on_unchanged_negations(l);
	tref r = rewriter::replace<node>(fm, atm, tau::_F());
	r = syntactic_path_simplification_dnf<node>::unsat_on_unchanged_negations(r);
	if (tau::get(l) == tau::get(r)) return tau::build_wff_ex(tau::trim(var), l, false);
	tref boole_atm = tau::get(tau::wff, tau::get(atm_type,
		term_boole_decomposition<node>(tau::get(atm)[0].first(), tau::trim(var)),
		term_boole_decomposition<node>(tau::get(atm)[0].second(), tau::trim(var))
		));
	tref nl = tau::build_wff_ex(tau::trim(var),
				    tau::build_wff_and(boole_atm, l), false);
	tref nr = tau::build_wff_ex(tau::trim(var),
				    tau::build_wff_and(tau::build_wff_neg(boole_atm), r), false);
	// Update available pool for further BDD variables
	pool.insert_or_assign(nl,
		rewriter::replace<node>(curr_pool, atm, tau::_T()));
	pool.insert_or_assign(nr,
		rewriter::replace<node>(curr_pool, atm, tau::_F()));
	return tau::build_wff_or(nl, nr);
}

// TODO: How to adjust for bitvector that are boolean?
/**
 * @brief Eliminate the existential quantifier scoping a clause.
 * @tparam node Tree node type
 * @param ex_clause Existentially quantified clause
 * @param quant_eliminated Indicates whether the quantifier was successfully removed
 * @return The resulting clause after removing the existential quantifier
 */
// Computes the satisfying interval for ∃var. body where body is a conjunction
// of DLO comparisons.  Returns the interval if determined, nullopt otherwise.
// Use .is_empty() for ∃-satisfiability, .is_full() for ∀-tautology check.
template<NodeType node>
static std::optional<qlt> qlt_dlo_qe(tref var, tref body);

template <NodeType node>
tref treat_ex_quantified_clause(tref ex_clause, bool& quant_eliminated) {
	using tau = tree<node>;
	// Following Corollary 2.3 from Taba book from Ohad
	tref var = tau::trim2(ex_clause);
	tref formula = tau::get(ex_clause)[0].second();

	if (tau::get(formula).equals_T() || tau::get(formula).equals_F())
		return formula;
	tref new_fm = tau::_T();
	bool is_quant_removable_in_clause = true;
	trefs conjs = get_cnf_wff_clauses<node>(formula);
	// A conjunct containing the quantified variable that still holds a
	// binder or an unresolved reference anywhere blocks elimination:
	// equations under a surviving inner quantifier are not top-level
	// conjuncts of this clause, and the eliminator below would squeeze
	// them as if they were, silently dropping the inner binder and
	// leaking its variable free
	auto blocks_elimination = [](tref m) {
		return is_quantifier<node>(m) || is<node>(m, tau::wff_ref);
	};
	for (tref& conj : conjs) {
		if (!contains<node>(conj, var)) {
			new_fm = tau::build_wff_and(new_fm, conj);
			conj = tau::_T();
			continue;
		}
		if (tau::get(conj).find_top(blocks_elimination)) {
			// The quantified variable is entangled with a kept
			// quantifier or reference, so we cannot resolve the
			// quantifier in this clause
			is_quant_removable_in_clause = false;
		}
	}
	tref scoped_fm = tau::build_wff_and(conjs);
	if (!is_quant_removable_in_clause) {
		// Since we cannot remove the quantifier in this
		// clause it needs to be maintained
		quant_eliminated = false;
		return tau::build_wff_and(
			tau::build_wff_ex(var, scoped_fm, false), new_fm);
	}

	DBG(LOG_TRACE << "treat_ex_quantified_clause/scoped_fm: " << LOG_FM(scoped_fm) << "\n";)
	if (auto eliminated = ex_subs_based_elimination<node>(var, scoped_fm); eliminated != scoped_fm) {
		DBG(LOG_TRACE << "treat_ex_quantified_clause/eliminated: " << LOG_FM(eliminated) << "\n";)
		return tau::build_wff_and(eliminated, new_fm);
	}

	// Check that quantified variable appears
	if (tau::get(scoped_fm).equals_T()) return new_fm;
	// An existential over F is F, independently of the variable
	if (tau::get(scoped_fm).equals_F()) return tau::_F();

	// Check if quantified variable is bitvector
	if (is_bv_type_family<node>(tau::get(var).get_ba_type())) {
		bool closed_and_solvable = false;
		if (const trefs& free_vars = get_free_vars<node>(scoped_fm);
			(free_vars.empty() || (free_vars.size() == 1 &&
			tau::get(free_vars[0]) == tau::get(var)))
			&& is_bv_solvable_formula<node>(scoped_fm))
				closed_and_solvable = true;
		if (closed_and_solvable) {
			// By assumption quantifier is pushed in all the way
			// Closed bv formula, simplify to T/F -- but only on a
			// definite answer: cvc5 returning unknown, or translation
			// failing, means we cannot decide, not that it is false.
			auto status = bv_formula_sat_status<node>(tau::build_wff_ex(var, scoped_fm, false));
			if (status == bv_sat_status::sat) return new_fm;
			if (status == bv_sat_status::unsat) return tau::_F();
		}
		// Non-closed BV quantifier, or closed-but-undecided: try
		// predicate blasting to convert the BV existential to Boolean bit
		// quantifiers that the atomless-BA path can then eliminate.
		// bv_predicate_blasting already anti-prenexes each blasted atomic's
		// own freshly-introduced auxiliary quantifiers (scoped locally); the
		// anti_prenex_block call below is a separate concern: it attempts to
		// push/resolve `var`'s own quantifier (still bv-typed and left
		// untouched by blasting itself) now that the scope's arithmetic has
		// been simplified to boolean form, so nothing needs to be skipped
		// anymore.
		if (bv_blasting) {
			tref ex_fm = tau::build_wff_ex(var, scoped_fm, false);
			if (auto blasted = bv_predicate_blasting<node>(ex_fm);
					blasted && blasted != ex_fm) {
				tref cont = anti_prenex_block<node>(blasted, no_skip<node>);
				return tau::build_wff_and(cont, new_fm);
			}
		}
		// Quantifier is not resolvable
		quant_eliminated = false;
		return tau::build_wff_and(
			tau::build_wff_ex(var, scoped_fm, false), new_fm);
	}
	// DLO quantifier elimination for qlt (dense linear order over rationals).
	// Must run before the atomless-BA path for pure ordering atoms (bf_lt,
	// bf_gt, bf_lteq, bf_gteq): atomless-BA QE can't reason about ordering
	// and would return T incorrectly for those.
	// For other qlt constraints (e.g. meet-equality o1&i1={3}), qlt_dlo_qe
	// returns nullopt; fall through to atomless-BA QE which handles them.
	if (is_omcat_type_family<node>(tau::get(var).get_ba_type())) {
		if (auto interval = qlt_dlo_qe<node>(var, scoped_fm); interval)
			return !interval.value().is_empty() ? new_fm : tau::_F();
		// qlt_dlo_qe undetermined: fall through to atomless-BA QE.
		// (Quantifier-in-place fallback is NOT used: atomless-BA handles
		//  non-ordering constraints like meet-equality correctly.)
	}
	// Continue with quantifier elimination for atomless BA
	size_t type_v = find_ba_type<node>(var);
	tref f = squeeze_positives<node>(scoped_fm, type_v);
	tref f_0 = f ? rewriter::replace<node>(f, var, tau::_0_trimmed(type_v)) : tau::_0(type_v);
	// std::cout << "f_0: " << tau::get(f_0) << "\n";
	tref f_1 = f ? rewriter::replace<node>(f, var, tau::_1_trimmed(type_v)) : tau::_0(type_v);
	// std::cout << "f_1: " << tau::get(f_1) << "\n";
	// TODO: instead of != use !(=)
	trefs neqs = tau::get(scoped_fm).select_top(is<node, tau::bf_neq>);
	if (neqs.size()) {
		tref nneqs = tau::_T();
		for (tref neq : neqs) {
			// Convert to != 0
			neq = norm_trimmed_equation<node>(neq);
			// Get term
			tref g = tau::trim(neq);
			tref g_0 = rewriter::replace<node>(g, var,
						tau::_0_trimmed(find_ba_type<node>(var)));
			// std::cout << "g_0: " << tau::get(g_0) << "\n";
			tref g_1 = rewriter::replace<node>(g, var,
						tau::_1_trimmed(find_ba_type<node>(var)));
			// std::cout << "g_1: " << tau::get(g_1) << "\n";
			// If both are 1 then inequality is implied by f_0f_1 = 0
			if (tau::get(g_0).equals_1() && tau::get(g_1).equals_1())
				continue;
			// If f_0 is equal to f_1 we can use assumption f_0 = 0 and f_1 = 0
			if (tau::get(f_0) == tau::get(f_1)) {
				nneqs = tau::build_wff_and(nneqs,
					tau::build_bf_neq_0(
						tau::build_bf_or(g_0,
								 g_1)));
			} else if (tau::get(g_0) == tau::get(g_1)) {
				nneqs = tau::build_wff_and(nneqs,
					tau::build_bf_neq_0(g_0));
			} else nneqs = tau::build_wff_and(
				nneqs,
				tau::build_bf_neq_0(
					tau::build_bf_or(
						tau::build_bf_and(
						tau::build_bf_neg(f_1),
						g_1),
						tau::build_bf_and(
						tau::build_bf_neg(f_0),
						g_0)))
					);
		}
		new_fm = tau::build_wff_and(new_fm, tau::build_wff_and(
			tau::build_bf_eq_0(tau::build_bf_and(f_0, f_1)),
			nneqs));
	} else if (f) {
		new_fm = tau::build_wff_and(new_fm,
			tau::build_bf_eq_0(
			tau::build_bf_and(f_0, f_1)));
	}
	return term_boole_normal_form<node>(new_fm);
}

/**
 * @brief The procedure pushes all quantifiers that are present in formula as far
 * in as possible.
 * @tparam node Tree node type
 * @param formula The formula to apply the procedure to
 * @return The resulting formula
 */
template<NodeType node>
tref anti_prenex(tref formula) {
	using tau = tree<node>;
	subtree_unordered_map<node, tref> atomic_pool;
	subtree_unordered_map<node, int_t> quant_pattern;
	int_t qid = 1;
	auto inner_quant = [&](tref n) {
		bool quant_eliminated = true;
		subtree_set<node> excluded;
		auto anti_prenex_step = [&](tref n) {
			while (tau::get(n).child_is(tau::wff_ex)) {
				// TODO: if all atomic formulas are !=

				// If n is single DNF clause -> treat quantifier
				if (!tau::get(n).find_top(is<node, tau::wff_or>)) {
					DBG(LOG_TRACE << "Before treat_ex_quantified_clause: " << LOG_FM(n) << "\n";)
					n = treat_ex_quantified_clause<node>(n, quant_eliminated);
					// std::cout << "excluded: " << tau::get(n) << "\n";
					excluded.insert(n);
					DBG(LOG_TRACE << "After treat_ex_quantified_clause: " << LOG_FM(n) << "\n";)
					return n;
				}
				// Try push quant down
				// std::cout << "push n: " << tau::get(n) << "\n";
				auto pushed = push_existential_quantifier_one<node>(n, &excluded);
				if (pushed != n) {
					// TODO: sort disjunctions to priorities
					DBG(LOG_TRACE << "Before push_existential_quantifier_one: " << LOG_FM(n) << "\n";)
					DBG(LOG_TRACE << "After push_existential_quantifier_one: " << LOG_FM(pushed) << "\n";)
					return pushed;
				}
				// Smart Boole decomposition
				n = normalize_atomic_formula_operators<node>(n);
				DBG(LOG_TRACE << "Before ex_quantified_boole_decomposition: " << LOG_FM(n) << "\n";)
				bool no_atms = false;
				n = ex_quantified_boole_decomposition<node>(n,
					atomic_pool, quant_pattern, &excluded, no_atms);
				// Quantifier is pushed in as far as possible but cannot
				// be resolved yet
				if (no_atms) return n;
				n = to_nnf<node>(n);
				DBG(LOG_TRACE << "After ex_quantified_boole_decomposition: " << LOG_FM(n) << "\n";)
			}
			return n;
		};
		auto step_visit = [&] (tref t) {
			if (excluded.contains(t)) return false;
			return visit_wff<node>(t);
		};
		if (!is_child_quantifier<node>(n)) return n;
		// Here child is quantifier
		DBG(LOG_TRACE << "Inner_quant on " << LOG_FM(n) << "\n";)
#ifdef TAU_CACHE
		using cache_t = subtree_unordered_map<node, tref>;
		static cache_t& cache = tree<node>::template create_cache<cache_t>();
		if (auto it = cache.find(n); it != cache.end()) {
			// Update the quantifier pattern after elimination
			quant_pattern.insert_or_assign(tau::trim2(n), 0);
			return it->second;
		}
#endif // TAU_CACHE
		tref n_elim = syntactic_formula_simplification<node>(n);
		// squeeze_absorb refuses to descend into quantifier nodes (its own
		// traversal stops at is_quantifier), so it must be applied to the
		// quantifier's body/bound-var pair directly, not to the whole
		// quantified node -- otherwise the call below is a no-op.
		tref q_var = tau::trim2(n_elim);
		tref q_body = tau::get(n_elim)[0].second();
		tref squeezed_body = squeeze_absorb<node>(q_body, q_var);
		if (squeezed_body != q_body) {
			// Finish the reduction squeeze_absorb deliberately leaves
			// partial (e.g. "xyz & (xy)'" instead of dropping it once
			// xy = 0 is known) -- scoped to just this body, not the whole
			// formula, to avoid the blow-ups a formula-wide canonical
			// reduction causes elsewhere.
			squeezed_body = bf_reduce_canonical<node>()(squeezed_body);
			n_elim = is_child<node>(n_elim, tau::wff_all)
				? tau::build_wff_all(q_var, squeezed_body, false)
				: tau::build_wff_ex(q_var, squeezed_body, false);
		}
		DBG(LOG_TRACE << "After squeeze_absorb_down " << LOG_FM(n) << "\n";)
		tref res = nullptr;
		if (is_child<node>(n_elim, tau::wff_all)) {
			tref n_neg = to_nnf<node>(tau::build_wff_neg(n_elim));
			res = pre_order<node>(n_neg).
				apply_unique(anti_prenex_step, step_visit);
			res = to_nnf<node>(tau::build_wff_neg(res));
		} else {
			res = pre_order<node>(n_elim).
				apply_unique(anti_prenex_step, step_visit);
		}
		// Update the quantifier pattern after a successful elimination
		if (quant_eliminated)
			quant_pattern.insert_or_assign(tau::trim2(n_elim), 0);
#ifdef TAU_CACHE
		return cache.emplace(n, res).first->second;
#endif // TAU_CACHE
		return res;
	};
	auto visit = [&](tref n) {
		if (is_quantifier<node>(n))
			quant_pattern.insert_or_assign(tau::trim(n), qid++);
		return visit_wff<node>(n);
	};
	DBG(LOG_DEBUG << "Anti_prenex on " << LOG_FM(formula) << "\n";)
	// Initial simplification of formula
	formula = syntactic_formula_simplification<node>(formula);
	DBG(LOG_TRACE << "After syntactic_formula_simplification: " << LOG_FM(formula) << "\n";)
	// Apply anti prenex procedure
	formula = post_order<node>(formula).apply_unique(inner_quant, visit);
	DBG(LOG_TRACE << "Anti_prenex result: " << LOG_FM(formula) << "\n";)
	formula = syntactic_formula_simplification<node>(formula);
	DBG(LOG_DEBUG << "Anti_prenex result after syntactic simp: " << LOG_FM(formula) << "\n";)
	return formula;
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

template<NodeType node>
tref resolve_quantifiers(tref formula) {
	using tau = tree<node>;
	subtree_set<node> excluded;
	auto resolver = [&](tref n) {
		if (is_child_quantifier<node>(n)) {
			// Check if the formula is closed and proceed to eliminate
			// the quantifier
			tref var = tau::trim2(n);
			if (is_bv_type_family<node>(tau::get(var).get_ba_type())) {
				// A closed, purely bitvector formula is decided
				// directly by the solver. This is checked before
				// blasting: the solver handles the bitvector
				// arithmetic natively, while deciding the blasted
				// form (with its many auxiliary quantifiers) is
				// much harder for it. Blasting does not close a
				// formula, so the check would not succeed later.
				if (get_free_vars<node>(n).empty()
					&& is_bv_solvable_formula<node>(n)) {
					// Only commit to T/F on a definite answer: cvc5
					// returning unknown, or translation failing, means
					// we cannot decide, not that the formula is false.
					auto status = bv_formula_sat_status<node>(n);
					if (status == bv_sat_status::sat) return tau::_T();
					if (status == bv_sat_status::unsat) return tau::_F();
				}
				if (bv_blasting)
					if (auto blasted = bv_predicate_blasting<node>(n);
						blasted && blasted != n)
						return blasted;
				excluded.insert(n);
			} else if (is_omcat_type_family<node>(tau::get(var).get_ba_type())) {
				if (const trefs& free_vars = get_free_vars<node>(n);
					free_vars.empty()) {
					tref body = tau::get(n)[0].second();
					if (auto interval = qlt_dlo_qe<node>(var, body); interval) {
						if (tau::get(n).child_is(tau::wff_ex))
							// ∃var. body: T iff satisfying interval is non-empty
							return interval.value().is_empty() ? tau::_F() : tau::_T();
						else
							// ∀var. body: T iff satisfying interval covers full domain
							return interval.value().is_full() ? tau::_T() : tau::_F();
					}
					// Undetermined: fall through to general solver
				} else excluded.insert(n);
			}
		} else if (get_free_vars<node>(n).empty()
				   && (tau::get(n).child_is(tau::bf_eq)
				    || tau::get(n).child_is(tau::bf_neq)
				    || tau::get(n).child_is(tau::bf_lt)
				    || tau::get(n).child_is(tau::bf_gt)
				    || tau::get(n).child_is(tau::bf_lteq)
				    || tau::get(n).child_is(tau::bf_gteq)
				    || tau::get(n).child_is(tau::bf_nlt)
				    || tau::get(n).child_is(tau::bf_ngt)
				    || tau::get(n).child_is(tau::bf_nlteq)
				    || tau::get(n).child_is(tau::bf_ngteq))
				   && tau::get(n).find_top([](tref t) -> bool {
				    	const auto& tn = tau::get(t);
				    	return tn.is(tau::bf_cast)
				    	    || (tn.is(tau::ba_constant)
				    	        && is_bv_type_family<node>(tn.get_ba_type()));
				   })) {
			// Ground BV comparison formula (no quantifier, no free variables).
			// e.g. {2}:bv[4] < {3}:bv[4], or (bv[4]){2}:bv[2] = {2}:bv[4].
			// wff nodes don't propagate ba_type, so we detect BV context by
			// looking for bf_cast nodes or ba_constant nodes with a BV type.
			// Hooks don't evaluate ground BV comparisons (especially bf_lt/bf_gt
			// or bf_eq with bf_cast). CVC5 handles all of them correctly.
			if (is_bv_formula_sat<node>(n)) return tau::_T();
			else return tau::_F();
		}
		return n;
	};
	auto visit = [&](tref n) {
		if (excluded.contains(n)) return false;
		return visit_wff<node>(n);
	};
	return pre_order<node>(formula).apply_unique(resolver, visit);
}

/**
 * @tparam node Tree node type.
 * @param formula Formula to push the quantifier block into.
 * @param block Ordered list of existentially quantified variables to push.
 * @param used_atms Set of atomic formulas already consumed in the current recursion; updated in place.
 * @param quant_pattern Map from quantified variables to their priority for BDD variable ordering.
 * @param order Variable ordering relation used by `resolve_quantifiers2`.
 * @param skip Predicate identifying variables/atomic formulas this pass must
 *        not Boole-decompose (defaults to BV-typed nodes; diverted to
 *        predicate blasting instead once nothing else can be pushed).
 * @return Formula with the quantifier block pushed as far inward as possible.
 * @endinternal
 */
template<NodeType node>
tref anti_prenex_block(tref formula, const trefs& block,
	subtree_unordered_set<node>& used_atms,
	const auto& quant_pattern,
	const typename term_handle<node>::order& order,
	std::function<bool(tref)> skip) {
	using tau = tree<node>;
	// Once no non-skip-matched block variable occurs free anymore, the
	// remaining leaf is entirely skip-matched (e.g. bitvector) content:
	// wrap the whole block around it and try predicate blasting instead of
	// the BDD-based elimination below, which requires atomless-typed
	// quantified variables. bv_predicate_blasting already anti-prenexes
	// each blasted atomic's own freshly-introduced auxiliary quantifiers
	// (scoped locally); the anti_prenex_block call below is a separate
	// concern: it attempts to push/resolve the block's own quantifiers
	// (still bv-typed and left untouched by blasting itself) now that the
	// scope's arithmetic has been simplified to boolean form, so nothing
	// needs to be skipped anymore.
	auto blast_block = [&](tref dep_formula) -> tref {
		tref ex_fm = dep_formula;
		for (auto v = block.rbegin(); v != block.rend(); ++v)
			ex_fm = build_wff_ex<node>(*v, ex_fm, false);
		if (bv_blasting)
			if (auto blasted = bv_predicate_blasting<node>(ex_fm);
					blasted && blasted != ex_fm)
				return anti_prenex_block<node>(blasted, no_skip<node>);
		return ex_fm;
	};

	// Goal: push the quantifier block as far into clause as possible.
	// A block variable is counted here regardless of `skip`: dropping a
	// skip-matched variable's occurrence from this check would let it be
	// classified as "independent" below and moved outside the block's
	// quantifier scope, leaking it free.
	{
		bool has_var = false;
		const trefs& vars = get_free_vars<node>(formula);
		for (tref v : block) {
			if (hasbc(vars, v, tau::subtree_less)) {
				has_var = true;
				break;
			}
		}
		// Formula does not contain quantified vars
		if (!has_var) return formula;
	}

	const tau& ft = tau::get(formula);
	// Case disjunction
	if (ft.child_is(tau::wff_or)) {
		// Push block into disjunction
		tref c1 = anti_prenex_block<node>(
			ft[0].first(), block, used_atms, quant_pattern, order, skip);
		if (tau::get(c1).equals_T()) return _T<node>();
		return tau::build_wff_or(c1, anti_prenex_block<node>(
			ft[0].second(), block, used_atms, quant_pattern, order, skip));
	}

	// Case conjunction
	if (ft.child_is(tau::wff_and)) {
		trefs conjs = get_cnf_wff_clauses<node>(formula);
		trefs dep;
		for (tref &conj : conjs) {
			// See note above: check every block variable's actual
			// occurrence regardless of `skip`.
			bool has_var = false;
			const trefs& vars = get_free_vars<node>(conj);
			for (tref v : block) {
				if (hasbc(vars, v, tau::subtree_less)) {
					has_var = true;
					break;
				}
			}
			// Conjunct contains quantified var
			if (has_var) {
				dep.push_back(conj);
				conj = _T<node>();
			}
		}
		if (dep.empty()) return formula;
		formula = tau::build_wff_and(dep);
		// Var-free conjuncts; they must be re-attached to every result
		// built from the dependent part below
		const tref indep = tau::build_wff_and(conjs);
		// Check if dependent formula is clause -> push block into clause
		if (!tau::get(formula).find_top(is<node, tau::wff_or>)) {
			if (std::ranges::any_of(block, skip))
				return tau::build_wff_and(indep, blast_block(formula));
			return tau::build_wff_and(
				indep,
				resolve_quantifiers2<node>(
				push_ex_block_into_clause<node>(formula, block, order), order, skip));
		}
		// TODO: only != present
		// Using the available atomic formulas, do Boole decomposition on best fit
		auto is_atomic = [&used_atms, &skip](tref n) {
			if (!tau::get(n).is(tau::wff)) return false;
			// Exclude used atomic fms
			if (used_atms.contains(n)) return false;
			// Exclude atomic formulas the caller wants skipped (e.g. BV-typed)
			if (skip(n)) return false;
			const tau& c = tau::get(n)[0];
			switch (c.value.nt) {
				case tau::bf_eq:
				case tau::bf_lt:
				case tau::bf_lteq: return true;
				default: return false;
			}
		};
		// Collect current atms
		const trefs atms = rewriter::select_top_until<node>(formula,
			is_atomic, is_quantifier<node>);
		// No decomposable atomic formula available (e.g. only !=
		// equations, quantified subformulas, or exclusively skip-matched
		// atoms remain): try blasting if the block needs it, else keep
		// the block on the dependent part instead of dereferencing end()
		if (atms.empty()) {
			if (std::ranges::any_of(block, skip))
				return tau::build_wff_and(indep, blast_block(formula));
			for (auto v = block.rbegin(); v != block.rend(); ++v)
				formula = build_wff_ex<node>(*v, formula, false);
			return tau::build_wff_and(indep, formula);
		}
		// Sort the atomic formulas and get minimum
		tref atm = *std::ranges::min_element(atms,
			atm_formula_order_for_quant_elim<node>(quant_pattern));
		// TODO: Take care of unique zero of atm

		// Remove/add available atomic formulas in stack-like fashion
		used_atms.insert(atm);
		// Do Boole decomposition
		tref l = rewriter::replace<node>(formula, atm, tau::_T());
		tref r = rewriter::replace<node>(formula, atm, tau::_F());
		if (tau::get(l) == tau::get(r)) {
			tref res = anti_prenex_block(
				l, block, used_atms, quant_pattern, order, skip);
			used_atms.erase(atm);
			return tau::build_wff_and(indep, res);
		}
		tref nl = anti_prenex_block(
			tau::build_wff_and(atm, l), block, used_atms, quant_pattern, order, skip);
		if (tau::get(nl).equals_T()) {
			used_atms.erase(atm);
			// dep part is T, only the var-free part remains
			return indep;
		}
		tref nr = anti_prenex_block(
			tau::build_wff_and(tau::build_wff_neg(atm), r),
				block, used_atms, quant_pattern, order, skip);
		used_atms.erase(atm);
		return tau::build_wff_and(indep, tau::build_wff_or(nl, nr));
	}

	// Connective is not wff_and or wff_or (e.g. a single atom or a wff_ref)
	// -> try blasting if the block needs it, else quantifiers stay with formula
	if (std::ranges::any_of(block, skip))
		return blast_block(formula);
	for (auto v = block.rbegin(); v != block.rend(); ++v)
		formula = build_wff_ex<node>(*v, formula, false);
	return formula;
}

/**
 * @internal
 * @brief Processes one maximal same-type quantifier block rooted at `n`.
 *
 * Delegates the whole block (whether atomless, bitvector-typed, or mixed)
 * to the 6-arg `anti_prenex_block`, which splits disjunctions/conjuncts,
 * Boole-decomposes atomless content, and attempts predicate blasting on
 * whatever leaf clause still needs `skip`-matched (e.g. bitvector) content
 * resolved -- so a bitvector-typed block variable no longer bails out to
 * `anti_prenex` (the legacy step-based algorithm) up front. A residual
 * fallback to `anti_prenex` remains at the end of `resolve_ex_block` for
 * structural cases unrelated to bitvector content that its
 * push_existential_quantifier_one/ex_quantified_boole_decomposition
 * strategy resolves and this algorithm's disjunction/Boole decomposition
 * does not. ∃-blocks are pushed in directly; ∀-blocks are dualized to
 * ∃-blocks.
 * @tparam node Tree node type.
 * @param n Formula node whose direct child is a quantifier.
 * @return Formula with the quantifier block eliminated or pushed inward.
 * @endinternal
 */
// Processes one quantifier block rooted at n:
// - ∃-blocks: push into body with the 6-arg anti_prenex_block (which
//   handles skip-matched/bitvector content itself via blasting), resolve
//   remaining quantifiers, then fall back to anti_prenex for whatever
//   structural (non-bitvector) cases still need it. ∀-blocks are dualized:
//   ∀x φ ≡ ¬∃x ¬φ.
template<NodeType node>
tref process_quantifier_block(tref n, std::function<bool(tref)> skip = is_tref_bv_type_family<node>) {
	using tau = tree<node>;
	if (!is_child_quantifier<node>(n)) return n;

	// Collect the maximal same-type block at the top.
	trefs block_vars;
	const bool is_ex = is_child<node>(n, tau::wff_ex);
	tref curr = n;
	while (is_child_quantifier<node>(curr)) {
		if (is_child<node>(curr, tau::wff_ex) != is_ex) break;
		block_vars.push_back(tau::trim2(curr));
		curr = tau::get(curr)[0].second();
	}
	tref body = curr;

	// Trivial Skolemization: if every variable in this block is trivially
	// eliminable (see heuristics/trivial_skolem.h), skip BDD ordering,
	// Boole decomposition, and predicate blasting below entirely. Only
	// valid for ∃-blocks. normalize_atomic_formula_operators first so
	// bf_neq atoms (e.g. from an already-resolved inner block) are
	// canonical bf_eq before trivial_skolem_ex's bf_eq-only matcher looks
	// at them.
	//
	// Only adopt the result when ALL block variables were eliminated
	// (trivial_skolem_ex then returns the bare matrix, with no
	// re-quantification at all). If any variable survives,
	// trivial_skolem_ex re-wraps survivors via build_wff_ex_many, which
	// renames every surviving bound variable -- and that renaming is not
	// capture-safe against variables already free in the body (observed
	// to corrupt formulas containing leftover synthetic "bN"-named
	// witnesses from earlier quantifier elimination in this same
	// pipeline), so any such partial result is discarded here and the
	// block falls through to the existing pipeline unchanged.
	if (is_ex) {
		tref simplified = trivial_skolem_ex<node>(block_vars,
			normalize_atomic_formula_operators<node>(body));
		if (!is_child<node>(simplified, tau::wff_ex)) {
			block_vars.clear();
			return simplified;
		}
	}

	// Build BDD variable order (innermost = lowest index = highest priority).
	typename term_handle<node>::order ord;
	for (size_t i = 0; i < block_vars.size(); ++i)
		ord.emplace(block_vars[block_vars.size() - 1 - i],
			static_cast<int_t>(i));

	// Build quant_pattern (outermost = highest priority for Boole decomposition).
	subtree_unordered_map<node, int_t> qp;
	for (size_t i = 0; i < block_vars.size(); ++i)
		qp.emplace(block_vars[i],
			static_cast<int_t>(block_vars.size() - i));

	// Run the core ∃-block elimination pipeline on a pre-normalized body b.
	// normalize_atomic_formula_operators is called here so that
	// push_ex_block_into_clause always receives bf_eq atoms (not bf_neq).
	auto resolve_ex_block = [&](tref b) -> tref {
		b = normalize_atomic_formula_operators<node>(b);
		subtree_unordered_set<node> used_atms;
		tref r = anti_prenex_block<node>(b, block_vars, used_atms, qp, ord, skip);
		r = resolve_quantifiers2<node>(r, ord, skip);
		// Fallback for closed bv (sub-)formulas resolve_quantifiers2 leaves
		// untouched, and a final safety net for blasting failures: try
		// CVC5/blasting once more via the whole-tree resolve_quantifiers.
		if (tau::get(r).find_top(is_quantifier<node>))
			r = resolve_quantifiers<node>(r);
		// Final fallback: legacy anti_prenex is a structurally different
		// (step-based, single-quantifier-at-a-time) algorithm that can
		// still make progress here in cases unrelated to bitvector content
		// (blasting is now this algorithm's own job, via blast_block above)
		// -- e.g. wff_ref atoms re-wrapped by push_ex_block_into_clause,
		// where treat_ex_quantified_clause knows how to leave them
		// correctly alone, and other structural cases its
		// push_existential_quantifier_one/ex_quantified_boole_decomposition
		// strategy resolves but this algorithm's disjunction/Boole
		// decomposition does not (confirmed empirically: removing this
		// fallback regresses satisfiability1-3, solver, splitter(2),
		// interpreter, wff_normalization, and normal_forms tests, none of
		// which involve bitvector content).
		if (tau::get(r).find_top(is_quantifier<node>))
			r = anti_prenex<node>(r);
		return r;
	};

	tref result;
	if (is_ex) {
		result = resolve_ex_block(body);
	} else {
		// ∀-block: dualize to ∃-block on negated body, then negate result.
		tref pushed = resolve_ex_block(
			to_nnf<node>(tau::build_wff_neg(body)));
		result = normalize_atomic_formula_operators<node>(
			to_nnf<node>(tau::build_wff_neg(pushed)));
		// Same final fallback as above.
		if (tau::get(result).find_top(is_quantifier<node>))
			result = anti_prenex<node>(result);
	}
	return result;
}

/**
 * @internal
 * @brief Drives the full anti-prenex-block pipeline: NNF + syntactic simplification, substitution-based elimination, canonical operator normalization, then post-order application of `process_quantifier_block`.
 * @tparam node Tree node type.
 * @param formula Formula to process.
 * @return Formula with quantifiers pushed inward as far as possible using the block-based algorithm.
 * @endinternal
 */
template<NodeType node>
tref anti_prenex_block(tref formula) {
	return anti_prenex_block<node>(formula, is_tref_bv_type_family<node>);
}

template<NodeType node>
tref anti_prenex_block(tref formula, std::function<bool(tref)> skip) {
	using tau = tree<node>;

	// Short-circuit: quantifier-free formulas need no processing here.
	// to_nnf and syntactic_formula_simplification can alter tau_ba
	// sub-formulas that the caller expects to be left intact.
	// Use find_top_until to avoid descending into tau_ba sub-trees, which
	// may contain wff_all/wff_ex nodes over I/O variables internally.
	if (!tau::get(formula).find_top_until(is_quantifier<node>,
		[](tref n) { return !while_is_formula<node>(n); }))
		return formula;

	// Step 1: NNF + syntactic simplification
	formula = to_nnf<node>(formula);
	formula = syntactic_formula_simplification<node>(formula, skip);

	// Step 2: Substitution-based elimination (innermost first via post_order).
	// Attempts ex x (x = t && phi(x)) => phi(t) for each existential
	// conjunctive scope.
	auto subs_elim = [](tref n) -> tref {
		if (!is_child<node>(n, tau::wff_ex)) return n;
		tref var = tau::trim2(n);
		tref scope = tau::get(n)[0].second();
		if (tau::get(scope).find_top(is<node, tau::wff_or>)) return n;
		tref elim = ex_subs_based_elimination<node>(var, scope);
		return elim != scope ? elim : n;
	};
	formula = post_order<node>(formula).apply_unique(subs_elim);
	formula = syntactic_formula_simplification<node>(formula, skip);

	// Step 3: Normalize non-canonical operators (bf_neq → ¬(bf_eq),
	// bf_nlteq → bf_lt, etc.) so push_ex_block_into_clause sees only
	// canonical atoms.
	formula = normalize_atomic_formula_operators<node>(formula);

	// Step 4: Process each quantifier block in post-order (innermost first).
	// For each node whose direct child is a quantifier, identify the maximal
	// block of consecutive same-type quantifiers at the top, push the block
	// into the body using the 5-arg anti_prenex_block, then eliminate the
	// remaining quantifiers over atomic formulas via resolve_quantifiers2.
	// wff_all blocks are handled by negation (dualization): ∀x φ ≡ ¬∃x ¬φ.
	auto f = [&](tref n) -> tref {
		return process_quantifier_block<node>(n, skip);
	};
	formula = post_order<node>(formula).apply_unique(
		f, while_is_formula<node>);
	return syntactic_formula_simplification<node>(formula, skip);
}

/**
 * @internal
 * @brief Pushes a block of existential quantifiers into a single atomic clause.
 *
 * Separates positive (`= 0`) and negative (`!= 0`) equations, squeezes positives into one BDD quantifier call, and adjusts negatives accordingly.
 * @tparam node Tree node type.
 * @param clause Atomic clause (conjunction of equations) to push the block into.
 * @param block Ordered list of existentially quantified variables.
 * @param order Variable ordering relation (unused currently, reserved for future use).
 * @return Clause with the quantifier block absorbed or re-attached when not removable.
 * @endinternal
 */
// NOTE: BV-typed and non-atomless quantifier blocks are not handled here.
// The 6-arg anti_prenex_block's caller only reaches this function for a
// dependent clause containing no skip-matched (e.g. bitvector) block
// variable -- a skip-matched block is diverted to predicate blasting
// (anti_prenex_block's own blast_block helper) before this point.
template<NodeType node>
tref push_ex_block_into_clause(tref clause, const trefs& block,
	const typename term_handle<node>::order& /*order*/) {
	using tau = tree<node>;
	// The clause is assumed to not have negation pushed into equalities
	DBG(assert(!tau::get(clause).find_top(is<node, tau::bf_neq>)));

	if (tau::get(clause).equals_T() || tau::get(clause).equals_F())
		return clause;
	// If there are no quantifiers to remove, the clause can be returned
	if (block.empty()) return clause;

	// All types in the block and in the clause are the same, otherwise
	// the quantifiers have not been pushed in correctly
	size_t clause_type = tau::get(block.front()).get_ba_type();
#ifdef DEBUG
	auto type_check = [&](tref n) {
		if (size_t t = tau::get(n).get_ba_type(); t > 0) {
			assert(clause_type == t);
			return false;
		}
		return true;
	};
	pre_order<node>(clause).visit(type_check);
	for (tref v : block)
		if (size_t t = tau::get(v).get_ba_type(); t > 0)
			assert(clause_type == t);
#endif
	bool is_quant_removable_in_clause = true;
	trefs conjs = get_cnf_wff_clauses<node>(clause);
	trefs pos, neg;
	for (tref& conj : conjs) {
		// By assumption all conjuncts contain at least one variable from the block
#ifdef DEBUG
		bool var_contained = false;
		for (tref v : block) if (contains<node>(conj, v))
			var_contained = true;
		assert(var_contained);
#endif
		// Check that conjunct is not an unresolved reference
		const tau& c = tau::get(conj);
		if (c.child_is(tau::wff_ref) || is_child_quantifier<node>(conj)) {
			// If the reference contains a quantified variable at this point
			// we cannot resolve the quantifier in this clause
			is_quant_removable_in_clause = false;
		} else if (c.child_is(tau::bf_eq)) {
			// Convert to f = 0 and save only f
			pos.push_back(tau::trim2(norm_equation<node>(conj)));
			conj = _T<node>();
		} else if (c.child_is(tau::wff_neg) && c[0][0].child_is(tau::bf_eq)) {
			// Convert to f != 0 and save only f
			neg.push_back(tau::trim2(norm_equation<node>(c.trim2())));
			conj = _T<node>();
		} else {
			// Unrecognized conjunct (e.g. bf_lt, ¬ref): cannot safely
			// remove the quantifier block from this clause
			is_quant_removable_in_clause = false;
		}
	}
	// TODO: create non-removable clause part and continue with rest
	if (!is_quant_removable_in_clause) {
		// Since we cannot remove the quantifier in this
		// clause it needs to be maintained
		for (auto v = block.rbegin(); v != block.rend(); ++v)
			clause = build_wff_ex<node>(*v, clause, false);
		return clause;
	}
	// Squeeze positive atomic formulas together -> result is a lazy BDD
	// By assumption they all have the same type
	tref f = build_bf_or<node>(pos, clause_type);
	tref res = build_bf_eq_0<node>(f);
	for (auto v = block.rbegin(); v != block.rend(); ++v)
		res = build_wff_ex<node>(*v, res, false);
	// Update all negative atomic formulas -> result is a lazy BDD
	for (tref& g : neg) {
		g = tau::build_bf_and(tau::build_bf_neg(f), g);
		g = tau::build_wff_neg(tau::build_bf_eq_0(g));
		for (auto v = block.rbegin(); v != block.rend(); ++v)
			g = build_wff_ex<node>(*v, g, false);
	}
	// Return formula with quantifier prefixes
	return tau::build_wff_and(res, tau::build_wff_and(neg));
}



/**
 * @internal
 * @brief Resolve quantifiers with a custom variable ordering (default skip).
 * @tparam node Tree node type.
 * @param formula Formula containing quantifiers.
 * @param order Comparison relation for variable ordering.
 * @return Formula with quantifiers resolved.
 * @endinternal
 */
template<NodeType node>
tref resolve_quantifiers2(tref formula, const typename term_handle<node>::order& order) {
	return resolve_quantifiers2<node>(formula, order, is_tref_bv_type_family<node>);
}

/**
 * @internal
 * @brief Resolve quantifiers with a custom variable ordering.
 *
 * Variant of `resolve_quantifiers` that uses the provided `order` relation to
 * sort variables before the elimination step.
 * @tparam node Tree node type.
 * @param formula Formula containing quantifiers.
 * @param order Comparison relation for variable ordering.
 * @param skip Predicate identifying variables this pass must not eliminate
 *        (defaults to BV-typed nodes, which need the solver/blasting instead).
 * @return Formula with quantifiers resolved.
 * @endinternal
 */
template<NodeType node>
tref resolve_quantifiers2(tref formula, const typename term_handle<node>::order& order,
		std::function<bool(tref)> skip) {
	using tau = tree<node>;
	subtree_set<node> excluded;
	auto down_resolver = [&](tref n) {
		if (is_child_quantifier<node>(n)) {
			// Check if the formula is closed and proceed to eliminate
			// the quantifier
			if (skip(tau::trim2(n))) {
				if (const trefs& free_vars = get_free_vars<node>(n);
					free_vars.empty() && is_bv_solvable_formula<node>(n)) {
					// Closed bv formula with explicit bitwidth: simplify to
					// T/F, but only on a definite answer -- cvc5 returning
					// unknown, or translation failing, means we cannot
					// decide, not that the formula is false.
					auto status = bv_formula_sat_status<node>(n);
					if (status == bv_sat_status::sat) return tau::_T();
					if (status == bv_sat_status::unsat) return tau::_F();
					excluded.insert(n);
				} else excluded.insert(n);
			} // TODO: restrict to atomless types
			else if (!tau::get(n).find_top(is<node, tau::ref>)) {
				using bdd = term_handle<node>::tbdd;
				// Record quantifier block in quants
				typename term_handle<node>::quants quants;
				const tref original = n; // save before stripping quantifiers
				while (is_child_quantifier<node>(n)) {
					tref var = tau::trim2(n);
					if (is_child<node>(n, tau::wff_ex)) {
						quants.emplace_back(var, bdd::ex);
					} else {
						quants.emplace_back(var, bdd::all);
					}
					n = tau::get(n)[0].second();
				}
				// All quantifiers have been trimmed from tree here
				const tau& nn = tau::get(n);
				if (nn.child_is(tau::bf_eq)) {
					// Normalize A = B to (A XOR B) = 0 if not already in
					// canonical form (can occur in tau_ba Boole expansions).
					if (!tau::get(n)[0][1].equals_0())
						n = norm_equation<node>(n);
					// Flip quantifiers due to equality
					for (auto& [_,q] : quants) {
						q == bdd::ex ? q = bdd::all : q = bdd::ex;
					}
					// Get the term of the equality
					n = tau::trim2(n);
					size_t term_type = tau::get(n).get_ba_type();
					// TODO: Syntactically simplify result?
					n = term_handle<node>::build(n, order).
						bdd_quant(quants, order).
							to_tau_term(term_type);
					return tau::build_bf_eq_0(n);
				} else if (nn.child_is(tau::wff_neg) && nn[0][0].child_is(tau::bf_eq)) {
					// Trim negation
					n = tau::trim2(n);
					// Normalize A = B to (A XOR B) = 0 if needed.
					if (!tau::get(n)[0][1].equals_0())
						n = norm_equation<node>(n);
					// Get the term of the equality
					n = tau::trim2(n);
					size_t term_type = tau::get(n).get_ba_type();
					// TODO: Syntactically simplify result?
					n = term_handle<node>::build(n, order).
						bdd_quant(quants, order).
							to_tau_term(term_type);
					return tau::build_wff_neg(tau::build_bf_eq_0(n));
				} else {
					// Formula under the block is not a single atomic
					// (e.g. a conjunction of bf_lt / bf_gt atoms);
					// return original with quantifier prefix intact so
					// the caller's resolve_quantifiers fallback can handle it.
					return original;
				}
			}
		}
		return n;
	};
	auto visit = [&](tref n) {
		if (excluded.contains(n)) return false;
		return is_formula<node>(n);
	};
	return pre_order<node>(formula).apply_unique(down_resolver, visit);
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
