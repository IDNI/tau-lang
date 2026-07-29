// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "antiprenexing/antiprenexing.h"

namespace idni::tau_lang {

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "anti_prenex"

/** @internal @copydoc no_skip @endinternal */
template <NodeType node>
bool no_skip(tref) {
	return false;
}

// TODO: How to adjust for bitvector that are boolean?
/**
 * @internal
 * @brief Eliminate the existential quantifier scoping a clause.
 * @tparam node Tree node type
 * @param ex_clause Existentially quantified clause
 * @param quant_eliminated Indicates whether the quantifier was successfully removed
 * @return The resulting clause after removing the existential quantifier
 * @endinternal
 */
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

/** @internal @copydoc resolve_quantifiers @endinternal */
template<NodeType node>
tref resolve_quantifiers(tref formula) {
using tau = tree<node>;
	subtree_set<node> excluded;
	auto down_resolver = [&](tref n) {
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

} // namespace idni::tau_lang
