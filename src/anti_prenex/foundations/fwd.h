// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file fwd.h
 * @brief Anti-prenexing foundations (layer 0): shared aliases and small value
 * types, and the record of the five design decisions.
 *
 * Every file of `src/anti_prenex/` includes this one first. Spec references
 * (`§n`) point into `anti_prenex.md`; the package plan is
 * the layer-0 plan (kept outside the repository).
 *
 * DESIGN DECISIONS, ruled 2026-09-07 (a change needs a new ruling):
 *
 * D1  n-ary connectives on binary nodes. Tau's `wff_and`/`wff_or` are binary.
 *     The spec's n-ary ∧/∨ is a canonical RIGHT-NESTED chain: members fully
 *     flattened across the same connective, deduplicated, sorted in content
 *     order (§1). Two equal member sets give one hash-consed node. `members`
 *     (dag.h) is cached per node and is what every step reads; a chain's
 *     internal nodes are never queried directly. `|n|` counts a chain as ONE
 *     node, `1 + Σ|members|`. Membership during assembly is the join's own
 *     local set; the cached content-ordered member list serves outside reads
 *     by binary search. Output conjuncts are emitted in content order.
 *
 * D2  The BDD-backing boundary. Terms are BDD-backed per component
 *     (`PREPARE_TERMS`, §5) only while that component is pushed and
 *     eliminated; `finish_terms` (terms.h) converts every surviving `BDD_ID`
 *     term back to a plain `bf` when the component closes, after the size
 *     acceptance test. One BDD order is live at a time. The global tables are
 *     keyed by Tau nodes, never by BDD refs, and survive across runs PROVIDED
 *     the round trip is idempotent (the back-converted term prepares to the
 *     same BDD, hence the same `BDD_ID` node) — a terms.h requirement.
 *
 * D3  Block keys. `push_memo` and `elim_memo` are keyed by the RE-WRAPPED
 *     block formula plus the flag, `(REWRAP(φ, X), keep_functional)` — the
 *     spec's own convention for `quant_memo` and `solver_memo`. Key narrowing
 *     (`X ← X ∩ FV(φ)`) happens first, as §6 says. No interning helper.
 *
 * D4  Construction hooks ON by default. Nodes are built through `tau::get`
 *     with the hooks on, so a node's identity is its hooked form, the same
 *     identity every other pass sees. The joins' rules are a superset of the
 *     hook rules. dag.h carries a test that no hook reorders or restructures
 *     beyond returning an operand or a constant.
 *
 * D5  Counters only. Every knob (options.h) is a size or step counter; there
 *     is no wall-clock budget anywhere.
 *
 * CACHE GATING (ruling, same date): the spec's tables and per-node facet
 * tables are caches and are gated like the existing ones — a static
 * GC-registered `create_cache` table under `#ifdef TAU_CACHE`, nothing
 * otherwise; no result may depend on a hit. The `#ifdef` lives in ctx.h
 * alone. Per-pass memos the spec writes as local state (the driver's
 * post-order memo, substitution's one memo per rewrite, `TREE_CONDITION`'s
 * call-local memo) are unconditional.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__FWD_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__FWD_H__

#include <functional>
#include <optional>
#include <utility>
#include <vector>

#include "tau_tree.h"
#include "tau_bdd.h"

namespace idni::tau_lang::anti_prenexing {

/// §1: a block `X` — an ordered variable list, OUTERMOST FIRST (`X[0]` is
/// the outermost binder). Every block-valued parameter in the module is one.
using block = trefs;

/// Kind of a formula binder (`wff_ex`/`wff_all`) or of a term-level functional
/// quantifier (`bf_fex`/`bf_fall`, §1 "functional quantifiers are transparent").
enum class binder { ex, all };

/// The three answers of `ASK` (§7); the value type of §1 `solver_memo`.
enum class answer { sat, unsat, unknown };

/// §1 `ctx.order` and `ctx.prio`: variable → integer rank. The BDD library's
/// own map type, so an order can be handed to it unchanged. Conventions:
/// `order` inner → LOWER rank, `prio` inner → HIGHER rank, ranks 1..|P| (§5).
template <NodeType node>
using var_order = typename term_handle<node>::order;

/// Callback through which a term-level rewrite re-simplifies a FORMULA
/// argument of a reference it touched (§1: "a formula argument through
/// SIMPLIFY"). Layer 0 passes none, meaning the identity; layer 1 supplies
/// `simplify`.
using simplify_formula_fn = std::function<tref(tref)>;

/**
 * @brief §1 `cof_memo` entry, the value of `COF(f, x)`: (f₀, f₁, p, usable, pin).
 *
 * `f0 = SIMPLIFY_TERM(f[x←0])`, `f1 = SIMPLIFY_TERM(f[x←1])` — cofactors by
 * child selection (§1). `usable` iff `x ∉ FV(f₀) ∪ FV(f₁)` (the leaf hazard:
 * a block variable hidden in a reference leaf makes f₀/f₁ not cofactors at
 * all). `p = SIMPLIFY_TERM(f₀·f₁)` when usable, else ⊥ (`nullptr`): `f` has a
 * zero iff `p = 0`. `pin` iff `usable ∧ SIMPLIFY_TERM(f₀ ∪ f₁) = 1`: the
 * zeros of `f` fix `x`; witness `f₁′`, residual `p = 0`; STRICT iff `p`
 * folds to `0` (§3, `TRY_WITNESS`).
 *
 * The three terms are built by `COF`, not subtrees of the key, so the table
 * must pin them: `for_each_tref` opts this type into GC introspection
 * (`HasForEachTref`).
 */
struct cof_entry {
	tref f0 = nullptr;
	tref f1 = nullptr;
	tref p  = nullptr;
	bool usable = false;
	bool pin    = false;
	void for_each_tref(auto&& f) const {
		if (f0) f(f0);
		if (f1) f(f1);
		if (p)  f(p);
	}
};

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__FWD_H__
