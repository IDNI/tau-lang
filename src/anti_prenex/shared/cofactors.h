// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file cofactors.h
 * @brief §6 `COF` and §7 `DISCHARGE`: the two places where a block variable
 * meets the BDD backing a term — the memoised record of cofactoring one
 * variable out, and the emission of a whole quantified block as an atom.
 *
 * Both work on the §1 TERM REPRESENTATION (terms.h): a term prepared for a
 * component has exactly the component's block as its decision variables, so
 * cofactoring is child selection and quantifying is one BDD operation. What
 * neither reaches is the LEAF HAZARD — a block variable that also sits inside
 * a leaf, in a reference argument or under a functional quantifier — and `COF`
 * reports that as `usable = false` rather than silently answering about the
 * occurrences it could see.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__SHARED__COFACTORS_H__
#define __IDNI__TAU__ANTI_PRENEX__SHARED__COFACTORS_H__

#include "../foundations/fwd.h"
#include "../foundations/ctx.h"
#include "../foundations/terms.h"
#include "../normalisers/simplify.h"

namespace idni::tau_lang::anti_prenexing {

/**
 * @brief §6 `COF(f, x, ctx)`: the record of cofactoring the term `f` by the
 * block variable `x`, memoised in §1 `cof_memo`.
 *
 * `f₀` and `f₁` are `SIMPLIFY_TERM` of the two cofactors, taken by CHILD
 * SELECTION in `f`'s BDD under `ctx.order`; the rest of the `cof_entry`
 * (fwd.h) is the pin test `pin_from_cofactors` (simplify.h) applies to them,
 * the same test the plain regime runs on substitution cofactors, recording
 * the result in this same table (`find_pin`, simplify.h). There is no
 * witness slot: a consumer that needs `f₁′` builds
 * `SIMPLIFY_TERM(¬f₁)` itself, and reads STRICT off `p = 0`.
 *
 * `f` IS THE KEY, exactly as it arrives (§1 cache scope) — a functional-
 * quantifier chain its leaves carry is part of it — so every consumer forms
 * the key itself with `TERM_OF` before calling. `cof_memo` is GLOBAL: the
 * entry is a pure function of `(f, x)`, and nothing of the component beyond
 * the order that `f` was built under enters it. The table is a cache, absent
 * in Debug builds, so nothing here may depend on a hit.
 *
 * `x` must be free in `f` (Debug-asserted): an `x`-free term pins nothing and
 * an entry for it would say nothing, so a scan skips such a conjunct instead
 * of asking. `x` need not be a DECISION variable, however — when it is not,
 * child selection returns `f` unchanged, both cofactors keep `x` and the
 * record is unusable, which is exactly what the leaf hazard means.
 */
template <NodeType node>
cof_entry cof(tref f, tref x, ctx<node>& c);

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
 * quantification of `term` under `ctx.order`, no expansion into `2^|X|`
 * terms. Under `ctx.keep_functional` it is kept instead, emitted as the
 * canonical functional-quantifier chain over `term` (§1) — the same block,
 * spelled rather than solved. Either way the atom is `SIMPLIFY_ATOM`'d before
 * it goes out, and a negative one is `¬(h = 0)`, never `h ≠ 0` (invariant 4).
 *
 * `DISCHARGE` has no memo of its own (§1 ctx table): the quantification is
 * covered by the library's memos and the chain constructor is O(|X|).
 */
template <NodeType node>
tref discharge(tref term, bool negative, const block& X, ctx<node>& c);

} // namespace idni::tau_lang::anti_prenexing

#include "cofactors.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__SHARED__COFACTORS_H__
