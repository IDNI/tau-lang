// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file eliminability.h
 * @brief Per-block union-find analysis deciding which block variables may be
 * eliminated, and why the others may not.
 *
 * Replaces the conjunct-level guards `eliminate_block_over_clause`'s
 * `is_quant_removable_in_clause` and `treat_ex_quantified_clause`'s
 * `blocks_elimination` used to apply. Those were all-or-nothing per clause;
 * this is per *component*, so a reference freezes only the variables that
 * actually reach it.
 *
 * Unlike its `block_*` / `boole_*` siblings, which self-include their `.tmpl.h`,
 * this header does not: `normal_forms.h` includes `eliminability.tmpl.h` after
 * `normal_forms.tmpl.h`, because from Task 2 onward the definitions here are
 * consumed by `antiprenexing.tmpl.h`, which has that ordering requirement.
 */

#ifndef __IDNI__TAU__ANTIPRENEXING_ELIMINABILITY_H__
#define __IDNI__TAU__ANTIPRENEXING_ELIMINABILITY_H__

#include <algorithm>

#include "tau_tree.h"
#include "union_find_with_sets.h"
#include "ba_types.h"
#include "normalizer_uf_arithmetic.h"

namespace idni::tau_lang {

/**
 * @brief Why a block variable may or may not be eliminated.
 *
 * The four meanings the single `skip` predicate used to conflate. The
 * distinction that matters most is `frozen` ("do not touch") versus
 * `blasteable` ("this has a destination -- send it there"): honouring a
 * `frozen`-style skip on bitvector content would disable the very blasting the
 * pipeline relies on.
 *
 * The underlying values encode the join order and are relied on by `join`.
 */
enum class elim_verdict {
	eliminable  = 0, ///< Nothing blocks removing this variable.
	blasteable  = 1, ///< Bitvector content blasting could turn into a regular
	                 ///< BA formula (the solver can also decide it).
	arithmetic  = 2, ///< Bitvector arithmetic blasting cannot express —
	                 ///< only cvc5 can decide it.
	frozen      = 3  ///< Entangled with an unresolved reference or a kept binder.
};

/**
 * @brief Join two verdicts. Total, always succeeds, no error type.
 *
 * Order, bottom to top:
 * `eliminable < blasteable < arithmetic < frozen`.
 *
 * `frozen` is top because a reference makes content untouchable by any
 * destination, the solver included. `arithmetic` sits above `blasteable`
 * because arithmetic the solver cannot express must not be routed to it -- the
 * narrower verdict wins where both apply.
 */
inline elim_verdict join(elim_verdict a, elim_verdict b) {
	return static_cast<elim_verdict>(
		std::max(static_cast<int>(a), static_cast<int>(b)));
}

/**
 * @brief Formula-wide inputs the per-block analysis cannot derive from a block.
 *
 * `bv_is_solver_owned` is genuinely a property of the whole formula: a
 * constant of another Boolean algebra (a `:tau` spec constant, say) is one
 * cvc5 cannot translate at all, so *no* bitvector scope anywhere in the
 * formula will ever be decided by the solver. Computed once at pipeline entry
 * with `!has_foreign_ba_constant<node>(form)` and carried down.
 */
template <NodeType node>
struct analysis_context {
	bool bv_is_solver_owned = true;
};

/**
 * @brief Result of analysing one quantifier block over its own body.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct block_eliminability {
	/** @brief Verdict for @p var; `eliminable` if it was not analysed. */
	elim_verdict verdict_of(tref var) const {
		if (auto it = verdicts.find(var); it != verdicts.end())
			return it->second;
		return elim_verdict::eliminable;
	}

	/** @brief Conjuncts in @p var's component; empty if it was not analysed. */
	const trefs& conjuncts_of(tref var) const {
		static const trefs none;
		if (auto it = components.find(var); it != components.end())
			return it->second;
		return none;
	}

	/**
	 * @brief `true` if a top-level `wff_ref` was seen directly under this
	 * block's conjuncts.
	 *
	 * A conservative signal, not an exhaustive scan: a reference pruned
	 * under a kept binder (the traversal stops at the binder and never
	 * looks inside it) and a `bf_ref` inside an atom's arguments are not
	 * counted here -- parity with the guard this analysis replaces, which
	 * did not distinguish them either.
	 *
	 * The `bf_ref` half of that parity deserves a note: `analyse_block`'s
	 * `is_ref_fm` recognises only `wff_ref`, so `g(y) = 0` is classified an
	 * ordinary eliminable atom. That is handled, not a gap:
	 * `eliminate_block_over_clause` prunes vacuous binders so such a clause
	 * reaches the single-variable squeeze, which substitutes constants INTO
	 * reference arguments (`g(0)`, `g(1)`) and settles it -- pinned by
	 * "a term containing a bf_ref still normalizes"
	 * (test_integration-normalizer_helpers.cpp).
	 */
	bool has_reference() const { return has_ref; }

	subtree_unordered_map<node, elim_verdict> verdicts;
	subtree_unordered_map<node, trefs> components;
	bool has_ref = false;
};

/**
 * @brief Comparator selecting a union-find root. Total over distinct nodes.
 *
 * `union_find_with_sets::merge` refuses to merge elements that compare equal
 * in both directions, so this must be a total order on distinct trefs --
 * `subtree_less` is. Declared as a free function template rather than a lambda
 * because `union_find_with_sets` stores its comparator **by reference**; a
 * temporary lambda would dangle.
 */
template <NodeType node>
bool eliminability_comp(tref l, tref r);

/**
 * @brief Classify each of @p block_vars against the atoms of @p conjuncts.
 *
 * One pass: every atomic formula, and every conjunct itself, is unioned with
 * its own free variables, so a verdict propagates to every variable sharing
 * an atom -- or sharing a non-atomic conjunct, such as a negated equation or
 * a disjunctive clause -- rather than only within an already-atomic conjunct.
 * That per-conjunct union is deliberately conservative: it merges all of a
 * conjunct's variables transitively, which is coarser than atom-level
 * sharing, but over-freezing is sound where under-freezing is not.
 *
 * `frozen` is seeded at every unresolved reference, at every kept binder, and
 * -- fail closed -- at every wff-level shape this analysis does not otherwise
 * recognise, so an unhandled shape cannot silently leave its variables
 * `eliminable`. @p ctx.bv_is_solver_owned is consulted when seeding a
 * bv-typed atom: `blasteable` only applies while it holds.
 *
 * Only a *bound* variable's own scope can constrain it -- it cannot occur
 * outside it -- so analysing the block body is not merely cheaper than
 * analysing the whole formula, it is equally informative and strictly fresher
 * (`process_quantifier_blocks` rebuilds the tree every round, and this map is
 * tref-keyed).
 *
 * @param block_vars Variables bound by the block.
 * @param conjuncts Top-level conjuncts of the block body.
 * @param ctx Formula-wide inputs.
 */
template <NodeType node>
block_eliminability<node> analyse_block(const trefs& block_vars,
	const trefs& conjuncts, const analysis_context<node>& ctx);

} // namespace idni::tau_lang

#endif // __IDNI__TAU__ANTIPRENEXING_ELIMINABILITY_H__
