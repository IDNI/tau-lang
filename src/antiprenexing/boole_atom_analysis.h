// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __BOOLE_ATOM_ANALYSIS_H__
#define __BOOLE_ATOM_ANALYSIS_H__

#include "tau_tree.h"

namespace idni::tau_lang {

/**
 * @internal
 * @brief Chapter 5 step 2h's case discrimination for one atom and one
 * variable, by cofactors.
 *
 * Given a pivot atom `f(x,X) = 0` chosen in step 2e, the paper rewrites the
 * Boole decomposition into `γ₁ ∨ … ∨ γ₅`, four of whose branches are decided
 * by cheap tests on `f`'s cofactors `f[x←0]` and `f[x←1]`, and only the fifth
 * of which needs the full recursion.
 * @endinternal
 */
enum class boole_atom_case {
	/// γ₂: `f ≡ 0`, so the atom is `T` for every `x`.
	identically_zero,
	/// γ₃: `f ≡ 1`, so the atom is `F` for every `x`.
	identically_one,
	/// γ₄: `f[x←0] = f[x←1]` and `x` is gone -- `f` does not depend on `x`.
	independent,
	/// γ₁: `f[x←0] = f[x←1]'` -- `x := f[x←0]` is the atom's only witness.
	unique_zero,
	/// γ₅: none of the above; the general decomposition is needed.
	general,
};

/**
 * @internal
 * @brief Cofactor verdict for a pivot atom.
 * @tparam node Tree node type.
 * @endinternal
 */
template<NodeType node>
struct boole_atom_analysis {
	boole_atom_case kind = boole_atom_case::general;
	/// `f[x←0]`, path-simplified. Null when the atom is not a `bf_eq`.
	tref cofactor_0 = nullptr;
	/// `f[x←1]`, path-simplified. Same nullity rule.
	tref cofactor_1 = nullptr;
};

/**
 * @internal
 * @brief Classifies @p atm with respect to @p var per chapter 5 step 2h.
 *
 * Returns `general` for anything that is not a `bf_eq`-headed atomic formula,
 * so `bf_lt`/`bf_lteq` pivots fall straight through to the existing recursion.
 * Substitution is done with `replace_if(..., is_boolean_operation)` so that
 * non-Boolean sub-terms are left alone, matching the legacy implementation in
 * `ex_quantified_boole_decomposition`.
 *
 * The caller must have established that @p var is an *active* (non-`skip`)
 * block variable and that `skip(atm)` is false: this function performs
 * Boolean cofactoring, which is exactly what `skip` reserves for the solver.
 * @tparam node Tree node type.
 * @param atm Pivot atomic formula.
 * @param var Variable to cofactor on, in the raw form block variables carry
 *        (i.e. `tau::trim2` of the quantifier node); it is wrapped in a `bf`
 *        internally, as the legacy code does.
 * @return The verdict and, where meaningful, the two cofactors.
 * @endinternal
 */
template<NodeType node>
boole_atom_analysis<node> analyze_boole_atom(tref atm, tref var);

} // namespace idni::tau_lang

#include "boole_atom_analysis.tmpl.h"

#endif // __BOOLE_ATOM_ANALYSIS_H__
