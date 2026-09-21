// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file witness.h
 * @brief §3's witness steps: `TRY_WITNESS` and the case-pin matcher
 * `TRY_CASE_WITNESS`, each in its SPELLED and its COF mode, phase 2's deep
 * pass `TRY_WITNESS_DEEP` and phase 2's driver `ELIMINATE_BY_SUBSTITUTION`.
 *
 * ONE NOTION OF A PIN, TWO MATCHES FOR IT. The SPELLED mode asks
 * `find_pin_for(atom, x)` (simplify.h) — the PIN, the equation that solves a
 * variable, asked about the variable being eliminated. The COF mode asks
 * `cof` (shared/cofactors.h) for the same test on the cofactors the term's
 * BDD hands over (§6). Neither reads an equation's spelling: `x = t`,
 * `t = x` and `x + t = 0` are one term `TERM_OF(c)` to them, which is what
 * the call sites straddling `NORMALIZE_OPERATORS` (§3) need. An equation
 * under `¬` pins nothing in the ∃ sense, and is exactly what pins in the ∀
 * sense, where the deep pass works under `¬∃x¬`.
 *
 * NO BDD IN THE SPELLED MODE (§3: "nothing is BDD-backed there"). The entries
 * WITHOUT a `ctx` — `try_witness(x, ψ)`, `try_witness_deep`,
 * `try_case_witness(x, ψ)` and `eliminate_by_substitution`, all of them phase
 * 2's — Debug-assert that their formula carries no `BDD_ID` anywhere; a
 * backed term reaching one is a CALLER BUG, not a case to handle. The entries
 * WITH a `ctx` are phase 4's, where a conjunct's term is BDD-backed under
 * `ctx.order` and the BDD primitives assert the order themselves. Nothing
 * here calls a finish or spells a term out.
 *
 * WHAT MAY TOUCH A UNIT (§4) — a unit is a surviving binder, opaque to every
 * step: the substitution of `x` DESCENDS into its body, because the pinning
 * conjunct that licensed the substitution is consumed — dropped at phase 2 —
 * and an untouched inner occurrence would desynchronise from the outer
 * rewrite. Phase 0's binder ids make that descent capture-safe, and no bound
 * variable is ever renamed. What is opaque is the MATCH: a unit is no
 * conjunction and no disjunction, so it is never a pin and never a case-pin
 * branch.
 *
 * SIMPLIFICATION: `TRY_WITNESS` simplifies its result (invariant 6 — it hands
 * every caller a formula ready to recurse into); the deep pass does NOT,
 * phase 2's global `SIMPLIFY` follows it (§3, step 2).
 *
 * NEIGHBOURS IN `heuristics/`: `ex_subs_based_elimination.h` witnesses `∃x`
 * inside the one clause it is handed and never leaves it;
 * `trivial_skolem.h` drops a variable whose UNIQUE occurrence an
 * invertible-operator chain isolates. Neither has the pins, the case pins or
 * the descent of §3, so neither decides what these steps decide.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__WITNESS__WITNESS_H__
#define __IDNI__TAU__ANTI_PRENEX__WITNESS__WITNESS_H__

#include <optional>
#include <utility>
#include <vector>

#include "../foundations/fwd.h"
#include "../foundations/ctx.h"
#include "../foundations/dag.h"
#include "../foundations/options.h"
#include "../foundations/terms.h"
#include "../normalisers/joins.h"
#include "../normalisers/simplify.h"
#include "../shared/cofactors.h"

namespace idni::tau_lang::anti_prenexing {

/**
 * @brief §3 `TRY_WITNESS(x, ψ)`, SPELLED mode: the witnessed BODY of
 * `∃x.ψ`, or `nullopt`.
 *
 * `ψ` is a conjunction or a single conjunct; its top-level conjuncts
 * (`members`) are scanned with `find_pin_for` for one pinning `x`. Among
 * several: a STRICT pin first — one whose residual `p` folds to `0` — else
 * the smallest `‖f₁′‖` (`mem_size`), §3's tie-break, which is the cost every
 * later substitution of the witness pays. The result is `ψ` with `x` replaced
 * by that witness in EVERY conjunct, THE PINNING ONE INCLUDED — its image is
 * the residual `p = 0`, `T` for a strict pin, so no site emits a residual of
 * its own — and then `SIMPLIFY`d (invariant 6).
 *
 * A BINDER IS NEVER RETURNED and never built: only the caller knows what
 * failure means, and `ψ` is not a binder.
 */
template <NodeType node>
std::optional<tref> try_witness(tref x, tref psi);

/**
 * @brief §3 `TRY_WITNESS(x, ψ, ctx)`, COF mode: the witnessed BODY of
 * `∃x.ψ` for a `ψ` whose terms are BDD-backed under `ctx.order`, or
 * `nullopt`.
 *
 * Phase 4's match (§3, §6). The top-level conjuncts that are POSITIVE
 * equations are scanned — an equation under `¬` pins nothing in the ∃ sense
 * and an order atom never pins — and on each the term `TERM_OF(c)` is
 * cofactored by `x`: `cof_memo`'s key, which this site forms itself. `x` is
 * PINNED iff the record is USABLE and `f₀ ∪ f₁ = 1`, since Boole's expansion
 * puts the zeros of the term at `f₀ ≤ x ≤ f₁′`, an interval that test
 * collapses to a point. The witness is the LOWER end `f₁′`, which carries the
 * residual `p = f₀f₁` into every sibling's terms rather than leaving it there
 * as inert bulk, and the pin is STRICT when `p` folds to `0`. A conjunct
 * whose term does not mention `x` pins nothing and is skipped; one that hides
 * `x` inside a leaf — a reference argument, a functional quantifier's body —
 * is `usable = false` (§1, the LEAF HAZARD) and pins nothing either.
 *
 * What follows the match is the spelled mode's, above: a STRICT pin first,
 * else the smallest `‖f₁′‖`; the witness substituted for `x` in EVERY
 * conjunct, THE PINNING ONE INCLUDED; the result `SIMPLIFY`d (invariant 6).
 */
template <NodeType node>
std::optional<tref> try_witness(tref x, tref psi, ctx<node>& c);

/**
 * @brief §3 `TRY_WITNESS_DEEP(Q, x, Φ)`, phase 2's deep pass: the body of
 * `Qx.Φ` with the BINDER DELETED and the pin consumed, or `nullopt`. A pure
 * function of the binder node.
 *
 * One descent from `Φ` toward `x`'s occurrences, through ∧, ∨ and binders
 * only. A SPINE is a flattened `wff_and` for `Q = ∃` and a flattened `wff_or`
 * for `Q = ∀` — and EVERY OTHER NODE IS A ONE-MEMBER SPINE, so a lone `x = t`
 * body, or a bare guarded assignment, fires as well as one among siblings
 * (§3: what the loop's last arm declines is an atom WITHOUT a pin). At a
 * spine the pass rewrites, in the first of two shapes that fits:
 *
 *  - a PLAIN PIN: a member that is `x = t` (`Q = ∃`) or `x ≠ t` (`Q = ∀`),
 *    STRICT — the member is DROPPED, so a weak pin's residual would be lost —
 *    with `x ∉ FV(t)` and `FV(t) ∩ D = ∅`. The spine becomes its other
 *    members, each `[x ← t]`; on a one-member spine that join is EMPTY, which
 *    is `T` for `∃` and `F` for `∀`.
 *  - a CASE PIN: a member `M` that is an ∨-of-branches (`Q = ∀`: an
 *    ∧-of-branches) every branch of which pins `x` strictly, at most
 *    `case_max` branches, and whose WHOLE node avoids `D`. The spine becomes
 *    `⋁ᵢ ((dᵢ minus its pin) ∧ (spine minus M))[x ← tᵢ]`, dualised for `∀`.
 *
 * `D` collects the variables bound on the path PAST the first kind flip:
 * `Q`-kind binders before it commute above `Qx` and stay usable in `t`.
 * CONFINEMENT holds free of charge — the descent refuses a node where `x` is
 * free in two members — and is what makes the rewrite of the spine node sound
 * as a CONTENT replacement over `Φ`. The ∀ form is the ∃ form under `¬∃x¬`:
 * connectives and pin sense swapped, one argument covering both.
 *
 * NOT SIMPLIFIED: phase 2's global `SIMPLIFY` follows (§3, step 2).
 */
template <NodeType node>
std::optional<tref> try_witness_deep(quantifier<node> Q, tref x, tref phi);

/**
 * @brief §3 `TRY_CASE_WITNESS`'s answer: the branches `(dᵢ, tᵢ)` of the case
 * pin and the rest of `ψ` without it.
 *
 * `dᵢ` is the branch AS IT STANDS, its pinning conjunct included: at phase 4
 * the pin stays and becomes the branch's residual under `[x ← tᵢ]` (§3
 * `TRY_WITNESS`). `rest` is the ∧-join of `ψ`'s other conjuncts. No copy of
 * the rewrite is built here — the ladder (§6) emits it branch by branch, so a
 * deciding branch is the only copy paid for.
 */
template <NodeType node>
struct case_witness {
	std::vector<std::pair<tref, tref>> branches;
	tref rest = nullptr;
};

/**
 * @brief §3 `TRY_CASE_WITNESS(x, ψ)`, the MATCH alone: the case-pin conjunct
 * of `ψ` with the smallest `|D|` (`formula_size`), or `nullopt`.
 *
 * A CASE PIN for `x` is a disjunction `D = ⋁ᵢ dᵢ` every branch of which has a
 * conjunct pinning `x` strictly with `x ∉ FV(tᵢ)`, at most `case_max`
 * branches. ALL-OR-NOTHING: a branch without a pin would keep `∃x` alive
 * inside its copy. The match stays at `D`'s TOP branches (§3), and a branch
 * that is a unit is opaque (§4) — it is no conjunction, so it never
 * qualifies.
 *
 * The ∃ form, which is what phase 4 pushes; `try_witness_deep` matches the
 * same shape dualised.
 */
template <NodeType node>
std::optional<case_witness<node>> try_case_witness(tref x, tref psi);

/**
 * @brief §3 `TRY_CASE_WITNESS(x, ψ, ctx)`, COF mode: the same MATCH on a `ψ`
 * whose terms are BDD-backed under `ctx.order`, or `nullopt`.
 *
 * A branch `dᵢ` qualifies when one of its conjuncts is a positive equation
 * whose term pins `x` by `COF` (§6) — the match `try_witness`'s COF mode
 * reads, choosing among a branch's several pins as it does: a strict pin
 * first, else the smallest `‖f₁′‖`. A pin's `x ∉ FV(tᵢ)` comes with `usable`.
 * A branch that is not a conjunction is its own one-member view, and a unit
 * is opaque (§4), so it never qualifies. ALL-OR-NOTHING, as in the spelled
 * mode, and the conjunct with the smallest `|D|` wins.
 *
 * The threshold on the number of branches is `ctx.case_max`, the component's
 * knob, where the spelled mode reads the bare constant (§1 `K″`: phase 2
 * predates any ctx).
 */
template <NodeType node>
std::optional<case_witness<node>> try_case_witness(tref x, tref psi,
	ctx<node>& c);

/**
 * @brief §3 `ELIMINATE_BY_SUBSTITUTION(φ)`, phase 2: ONE pre-order pass in
 * which every binder node tries `try_witness_deep` and is replaced by its
 * result where it fires.
 *
 * The walk continues INTO a replacement — a binder the rewrite exposed is
 * tried in the same pass — and never enters a term. Memoised per node for the
 * call: a result at a binder node is a pure function of that node, so a
 * shared binder is tried once.
 */
template <NodeType node>
tref eliminate_by_substitution(tref phi);

} // namespace idni::tau_lang::anti_prenexing

#include "witness.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__WITNESS__WITNESS_H__
