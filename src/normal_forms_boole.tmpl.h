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
			LOG_DEBUG << "term_boole_decomposition: "
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
	// Squeeze and absorb for additional simplifications during term
	// normalization. NF-16: kept deliberately -- an older note warned of
	// major blow-ups here, but the current gates (510 tests incl. the
	// anti-prenex block work) pass with it unconditional; if a blow-up
	// reappears, this call is the first suspect.
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
	// Full LTL / CTL* operators manage their own temporal scope; do not
	// wrap them in wff_always — pass through unchanged. NF-6: A/E and
	// wff_semantic_neg belong here too (kept in sync with
	// is_temporal_quantifier) — without them a formula whose only
	// branching-time ops are A/E would fall into the always/sometimes
	// machinery, where those atoms are not BDD vars and get dropped.
	auto is_ltl_op_node = [](tref n) {
		const auto& t = tree<node>::get(n);
		if (!t.has_child()) return false;
		auto nt = t[0].value.nt;
		return nt == tree<node>::wff_F || nt == tree<node>::wff_U
		    || nt == tree<node>::wff_R || nt == tree<node>::wff_W
		    || nt == tree<node>::wff_S || nt == tree<node>::wff_T
		    || nt == tree<node>::wff_A || nt == tree<node>::wff_E
		    || nt == tree<node>::wff_semantic_neg;
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
#define LOG_CHANNEL_NAME "normal_forms" // NF-14: "to_snf" was a stale channel name


// (NF-7: build_split_wff_using deleted -- zero callers.)


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
