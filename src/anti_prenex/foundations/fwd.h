// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file fwd.h
 * @brief Anti-prenexing foundations (layer 0): shared aliases and small value
 * types. Every file of `src/anti_prenex/` includes this one first.
 *
 * Spec references (`§n`) point into `anti_prenex.md`, the sole source of
 * truth. The design rulings D1–D5 and the cache-gating rule (Sep 7 2026)
 * are recorded in the layer-0 implementation plan, a working document kept
 * outside the repository; the headers cite them by name and state only what
 * the C++ adds: ownership, nullability, build-type behaviour, GC.
 *
 * Cache gating, in one paragraph (ruling, Sep 7 2026): every table is an
 * entry of ctx.h's `enum class table`. The six §1 result tables are caches and
 * exist under `#ifdef TAU_CACHE` only (OFF in Debug; no result may depend on
 * a hit). The structural per-node tables (`atoms_memo`, `size_memo`,
 * `neg_memo`, `negative_tree_memo`) are unconditional, like
 * `get_free_vars`' table, so their accessors may return references. The
 * member view is not cached at all. The `#ifdef` lives in ctx.tmpl.h's
 * `table_ptr` alone.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__FWD_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__FWD_H__

#include <functional>
#include <utility>
#include <vector>

#include "tau_tree.h"
#include "tau_bdd.h"

namespace idni::tau_lang::anti_prenexing {

/// §1: a block `X` — an ordered variable list, OUTERMOST FIRST (`X[0]` is
/// the outermost binder). Every block-valued parameter in the module is one.
using block = trefs;

/// A BA type id, as `tree<node>::get_ba_type()` returns and `ba_types<node>`
/// hands out. `0` is the codebase's untyped id.
using ba_type_id = size_t;

/// Kind of a formula binder (`wff_ex`/`wff_all`) or of a term-level functional
/// quantifier (`bf_fex`/`bf_fall`, §1).
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
/// SIMPLIFY"). Layer 1 supplies `simplify`; the default is the identity.
using simplify_formula_fn = std::function<tref(tref)>;

/// The identity, the default `simplify_formula_fn` (never an empty
/// `std::function`, which would throw when called).
inline const simplify_formula_fn identity_formula = [](tref t) { return t; };

/**
 * @brief A set of node handles stored as a table VALUE: sorted by
 * `subtree_less<node>` (binary-searchable, like `get_free_vars`' result).
 * `for_each_tref` opts the type into GC introspection (`HasForEachTref`), so
 * the collector pins and checks the members whether or not they are
 * subtrees of the key — a plain `trefs` value is deliberately NOT walked by
 * the tree's caches. The value type of `atoms_memo`.
 */
struct tref_set {
	trefs items;
	void for_each_tref(auto&& f) const { for (tref t : items) f(t); }
};

/**
 * @brief §1 `cof_memo` entry, the value of `COF(f, x)`: (f₀, f₁, p, usable, pin).
 *
 * `f0`/`f1`: the simplified cofactors by child selection (§1). `usable` iff
 * `x ∉ FV(f₀) ∪ FV(f₁)` (the leaf hazard). `p = SIMPLIFY_TERM(f₀·f₁)` when
 * usable, else ⊥ (`nullptr`). `pin` iff `usable ∧ f₀ ∪ f₁ = 1`; witness
 * `f₁′`, residual `p = 0`; STRICT iff `p` folds to `0` (§3 `TRY_WITNESS`).
 * The three terms are built by `COF`, not subtrees of the key, so the table
 * must pin them: `for_each_tref` opts this type into GC introspection.
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
