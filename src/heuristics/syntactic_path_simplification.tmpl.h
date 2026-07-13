// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "syntactic_path_simplification"

namespace idni::tau_lang {

// Forward declaration: defined in normal_forms.tmpl.h, included after this file.
template <NodeType node>
tref normalize_atomic_formula_operators(tref fm);

// ── Internal helpers ─────────────────────────────────────────────────────────

/**
 * @internal
 * @brief Strict ordering used to sort conjuncts: equality atoms (`bf_eq`)
 * sort before non-equality atoms; among same-kind atoms, `subtree_less`
 * breaks ties.
 * @tparam node Tree node type.
 * @param l Left atom.
 * @param r Right atom.
 * @return `true` if @p l strictly precedes @p r.
 *
 * @par Example
 * @code{.cpp}
 * // "x = 0" (equality) precedes "y != 0" (non-equality) (see
 * // tests/integration/test_integration-heuristics-syntactic_path_simplification.cpp:12-17).
 * tref eq_atm  = get_nso_rr("x = 0.").value().main->get();
 * tref neq_atm = get_nso_rr("y != 0.").value().main->get();
 * CHECK( syntactic_path_simplification_wff_comp<node_t>(eq_atm, neq_atm) == true );
 * CHECK( syntactic_path_simplification_wff_comp<node_t>(neq_atm, eq_atm) == false );
 * @endcode
 * @endinternal
 */
template <NodeType node>
bool syntactic_path_simplification_wff_comp(tref l, tref r) {
	using tau = tree<node>;
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

/**
 * @internal
 * @brief Per-conjunction-path contradiction detection for wff: within each
 * top-level `wff_and`, collects each conjunct as a `true`/`false`
 * assignment (treating a bare atom as `true`, a negated atom as `false`),
 * substitutes them into the rest of the conjunction, and collapses to `F`
 * if any conjunct contradicts another. Does not descend into `wff_or` or
 * push negations.
 * @tparam node Tree node type.
 * @param root Formula to scan.
 * @return @p root with contradictory conjunctions collapsed to `F`.
 *
 * @par Example
 * @code{.cpp}
 * // "x = 0 && !(x = 0)" collapses to F (see
 * // tests/integration/test_integration-heuristics-syntactic_path_simplification.cpp:62-66).
 * tref fm = get_nso_rr("x = 0 && !(x = 0).").value().main->get();
 * tref res = syntactic_path_simplification_simplify_wff<node_t>(fm);
 * CHECK( tau::get(res).equals_F() );
 * @endcode
 * @endinternal
 */
template <NodeType node>
tref syntactic_path_simplification_simplify_wff(tref root) {
	using tau = tree<node>;
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
		std::ranges::stable_sort(sorted_assms,
			syntactic_path_simplification_wff_comp<node>);
		tref assms = tau::build_wff_and(sorted_assms);
		// Make sure to not revisit built assumptions
		if (tau::get(simp) == get_T<node>())
			skip = tau::get(assms).first();
		else skip = assms;
		// Conjunct assumptions with simplified tree
		return tau::build_wff_and(assms, simp);
	};
	auto visit = [&skip](tref n) {
		if (skip != nullptr && tau::get(n) == tau::get(skip))
			return skip = nullptr, false;
		return is_formula<node>(n);
	};
	return pre_order<node>(root).template
		apply_unique<synt_path_simp_m>(down, visit);
}

/**
 * @internal
 * @brief The bf-term analog of `syntactic_path_simplification_simplify_wff`:
 * within each top-level `bf_and`, collects factor/negated-factor
 * assignments and collapses to `0` if a contradiction (a factor and its
 * negation both present) is found. Does not descend into `bf_or`.
 * @tparam node Tree node type.
 * @param root Boolean term to scan.
 * @return @p root with contradictory conjunctions collapsed to `0`.
 *
 * @par Example
 * @code{.cpp}
 * // "x & x'" collapses to 0 (see
 * // tests/integration/test_integration-heuristics-syntactic_path_simplification.cpp:111-115).
 * tref fm = get_bf_nso_rr("", "x & x'").value().main->get();
 * tref res = syntactic_path_simplification_simplify_bf<node_t>(fm);
 * CHECK( tau::get(res).equals_0() );
 * @endcode
 * @endinternal
 */
template <NodeType node>
tref syntactic_path_simplification_simplify_bf(tref root) {
	using tau = tree<node>;
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
			while_is_boolean_operation<node>);
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
		// Make sure to not revisit built assumptions
		if (tau::get(simp).equals_1())
			skip = tau::get(assms).first();
		else skip = assms;
		// Conjunct assumptions with simplified tree
		return tau::build_bf_and(assms, simp);
	};
	auto visit = [&](tref n) {
		if (skip != nullptr && tau::get(n) == tau::get(skip))
			return skip = nullptr, false;
		return is_boolean_operation<node>(n) || is<node, tau::bf>(n);
	};
	return pre_order<node>(root).template
		apply_unique<synt_path_simp_m>(down, visit);
}

// ── Public functions ──────────────────────────────────────────────────────────

template <NodeType node>
tref syntactic_path_simplification(tref fm) {
	using tau = tree<node>;
	DBG(LOG_DEBUG << "Syntactic_path_simplification on " << LOG_FM(fm) << "\n";)
	tref res = nullptr;
	if (tau::get(fm).is_term()) {
		if (tau::get(fm).equals_0() || tau::get(fm).equals_1())
			return fm;
		// Resolve contradictions
		fm = push_negation_in<node, false>(fm);
		fm = syntactic_path_simplification_simplify_bf<node>(fm);
		// Resolve tautologies
		fm = push_negation_in<node, false>(tau::build_bf_neg(fm));
		fm = syntactic_path_simplification_simplify_bf<node>(fm);
		res = push_negation_in<node, false>(tau::build_bf_neg(fm));
	} else {
		if (tau::get(fm).equals_F() || tau::get(fm).equals_T())
			return fm;
		// Resolve contradictions
		fm = normalize_atomic_formula_operators<node>(to_nnf<node>(fm));
		fm = syntactic_path_simplification_simplify_wff<node>(fm);
		// Resolve tautologies
		fm = normalize_atomic_formula_operators<node>(to_nnf<node>(tau::build_wff_neg(fm)));
		fm = syntactic_path_simplification_simplify_wff<node>(fm);
		res = to_nnf<node>(tau::build_wff_neg(fm));
	}
	DBG(LOG_DEBUG << "Syntactic_path_simplification result: " << LOG_FM(res) << "\n";)
	return res;
}

template <NodeType node>
tref syntactic_path_simplification_unsat_on_unchanged_negations(tref fm) {
	using tau = tree<node>;
	if (tau::get(fm).is_term()) {
		if (tau::get(fm).equals_0() || tau::get(fm).equals_1())
			return fm;
		// Resolve contradictions
		return syntactic_path_simplification_simplify_bf<node>(fm);
	} else {
		// Resolve contradiction
		return syntactic_path_simplification_simplify_wff<node>(fm);
	}
}

} // namespace idni::tau_lang
