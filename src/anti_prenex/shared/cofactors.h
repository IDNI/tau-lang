// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file cofactors.h
 * @brief §6 `COF` and §7 `DISCHARGE`: the two places where a block variable
 * meets the BDD backing a term — the memoised record of cofactoring one
 * variable out, and the emission of a whole quantified block as an atom.
 *
 * Both work on the §1 TERM REPRESENTATION (terms.h): every variable of a
 * prepared term at a Boolean position is a decision variable, so cofactoring
 * is child selection and quantifying is one BDD operation. What neither
 * reaches is the LEAF HAZARD — a block variable that also sits inside a leaf,
 * in a reference argument or a foreign-typed subterm — and `COF` reports that
 * as `usable = false` rather than silently answering about the occurrences it
 * could see.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__SHARED__COFACTORS_H__
#define __IDNI__TAU__ANTI_PRENEX__SHARED__COFACTORS_H__

#include "../foundations/fwd.h"
#include "../foundations/ctx.h"
#include "../foundations/dag.h"
#include "../foundations/terms.h"

namespace idni::tau_lang::anti_prenexing {

/**
 * @brief §6 `COF(f, x)`: the record of cofactoring the term `f` by the
 * variable `x`, memoised in §1 `cof_memo` — THE ONE RECIPE on backed terms,
 * for a block variable of the push and for a free variable of §3
 * `SIMPLIFY`'s propagation alike. It takes no ctx.
 *
 * `f₀` and `f₁` are the RESTRICTS of `f`'s BDD (§1), child selection with
 * no `SIMPLIFY_TERM`; every other term of the `cof_entry` (fwd.h) is a BDD
 * operation on them: the pin test on their union, the residual `p` their
 * product — both through `SIMPLIFY_TERM` — and a pin's witness `f₁′` the
 * complemented BDD of `f₁`, STRICT when `p` folds to `0`. A usable record
 * carries `p` whether it pins or not.
 *
 * `f` IS THE KEY, exactly as it arrives (§1 cache scope): pass the backed
 * regime's `TERM_OF` of the atom, not the atom. `cof_memo` is GLOBAL: the
 * entry is a pure function of `(f, x)`, and nothing of the component enters
 * it, every BDD being built under the one global order. The table is a
 * cache, absent in Debug builds, so nothing here may depend on a hit.
 *
 * `x` must be free in `f` (Debug-asserted): an `x`-free term pins nothing.
 * `x` need not be a DECISION variable — when it is not, child selection
 * returns `f` unchanged, both cofactors keep `x` and the record is unusable,
 * which is exactly what the leaf hazard means.
 */
template <NodeType node>
cof_entry cof(tref f, tref x);

/**
 * @brief §7 `DISCHARGE(∃X.atom, ctx)`: the atom a whole quantified block
 * comes out as.
 *
 * `∃X. term = 0` is `∀_X term = 0` and `∃X. term ≠ 0` is `∃_X term ≠ 0`; the
 * caller passes the term and its SIGN (`negative` for the second shape,
 * `¬(term = 0)`), because it holds both and the atom is built once, here.
 * An empty `X` is legal and quantifies nothing.
 *
 * TWO MODES. In the ordinary one the block is discharged: ONE BDD
 * quantification of `term`, no expansion into `2^|X|` terms. Under
 * `ctx.keep_functional`, when `X`'s levels are the lowest of `term`, the
 * block is decided HERE, at its emission: the canonical chain over `term` is
 * built, `ctx.kf` is asked ONCE on it, and a yes ALLOWS keeping it, SLID to
 * `X`'s levels (§1 transparency) — the variables above stay decision
 * structure, each piece over `X`'s levels is one leaf, a closed piece folded
 * to its constant. A no, or a sub-block with block variables pending below
 * `X`'s levels, is quantified at once, so no emission hides a block variable.
 * Either way the atom is `SIMPLIFY_ATOM`'d before it goes out, and a negative
 * one is `¬(h = 0)`, never `h ≠ 0` (invariant 4).
 *
 * `DISCHARGE` has no memo of its own (§1 ctx table): the quantification is
 * covered by the library's memos, the chain constructor is O(|X|), the slide
 * is one walk over the term's BDD, and `kf`, a pure function of the chain, is
 * asked once per emission.
 */
template <NodeType node>
tref discharge(tref term, bool negative, const block& X, ctx<node>& c);

} // namespace idni::tau_lang::anti_prenexing

#include "cofactors.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__SHARED__COFACTORS_H__
