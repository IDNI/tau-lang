// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __BLOCK_ATOM_PROFILE_H__
#define __BLOCK_ATOM_PROFILE_H__

#include <functional>

#include "tau_tree.h"

namespace idni::tau_lang {

/**
 * @internal
 * @brief Sign census of the atomic formulas a quantifier block scopes over.
 *
 * Chapter 5's steps 2a and 2b are guarded by whole-formula sign predicates:
 * 2a fires when *every* atom is negated, 2b when *no* atom is negated. This
 * struct answers both, and additionally reports whether the formula holds any
 * `skip`-matched (e.g. bitvector) node at all -- neither fast path may fire in
 * that case, because both rewrite atoms that `skip` reserves for predicate
 * blasting or the solver.
 * @tparam node Tree node type.
 * @endinternal
 */
template<NodeType node>
struct block_atom_profile {
	/// Count of `f = 0` atoms.
	size_t positives = 0;
	/// Count of `!(f = 0)` atoms.
	size_t negatives = 0;
	/// Count of atoms neither fast path can handle: `bf_lt`, `bf_lteq`,
	/// `wff_ref`, nested quantifiers, negations of any of those.
	size_t others = 0;
	/// Any `skip`-matched node anywhere in the formula.
	bool skip_content = false;

	/// Paper step 2a's guard.
	bool all_negated() const {
		return !skip_content && others == 0
			&& positives == 0 && negatives > 0;
	}
	/// Paper step 2b's guard.
	bool all_positive() const {
		return !skip_content && others == 0
			&& negatives == 0 && positives > 0;
	}
};

/**
 * @internal
 * @brief Computes the sign census of @p formula's atomic formulas.
 *
 * Descends through `wff_and`/`wff_or` only; `wff_t`/`wff_f` are ignored (they
 * are not atoms). Everything else is counted and not descended into, so an
 * atom nested under a quantifier is counted once as `others` rather than
 * having its own atoms folded into the census of the enclosing scope.
 *
 * @p formula is expected to have been through
 * `normalize_atomic_formula_operators`, so negative atoms appear as
 * `!(f = 0)` and never as `bf_neq`.
 * @tparam node Tree node type.
 * @param formula Matrix of a quantifier block.
 * @param skip Predicate marking content this pass must not rewrite.
 * @return The census.
 * @endinternal
 */
template<NodeType node>
block_atom_profile<node> profile_block_atoms(tref formula,
	const std::function<bool(tref)>& skip);

} // namespace idni::tau_lang

#include "block_atom_profile.tmpl.h"

#endif // __BLOCK_ATOM_PROFILE_H__
