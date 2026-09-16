// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file witness.h
 * @brief §3's witness steps: `TRY_WITNESS` in its SPELLED mode, phase 2's
 * deep pass `TRY_WITNESS_DEEP`, the case-pin matcher `TRY_CASE_WITNESS` and
 * phase 2's driver `ELIMINATE_BY_SUBSTITUTION`.
 *
 * ONE MATCH UNDERNEATH: `find_pin_for(atom, x)` (simplify.h), the PIN — the
 * equation that solves a variable — asked about the variable being
 * eliminated. Nothing here reads an equation's spelling: `x = t`, `t = x` and
 * `x + t = 0` are one term `TERM_OF(c)` to it, which is what the call sites
 * straddling `NORMALIZE_OPERATORS` (§3) need. An equation under `¬` pins
 * nothing in the ∃ sense, and is exactly what pins in the ∀ sense, where the
 * deep pass works under `¬∃x¬`.
 *
 * NO BDD IN SPELLED MODE (§3: "nothing is BDD-backed there"). Every entry
 * here Debug-asserts that its formula carries no `BDD_ID` anywhere; a backed
 * term reaching one is a CALLER BUG, not a case to handle. Nothing here calls
 * a finish or spells a term out.
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
#include "../foundations/dag.h"
#include "../foundations/options.h"
#include "../foundations/terms.h"
#include "../normalisers/joins.h"
#include "../normalisers/simplify.h"

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
