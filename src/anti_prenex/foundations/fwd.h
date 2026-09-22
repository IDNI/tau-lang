// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file fwd.h
 * @brief Anti-prenexing foundations: shared aliases and small value types.
 * Every file of `src/anti_prenex/` includes this one first.
 *
 * A `§n` reference points into `anti_prenex.md`, the specification this
 * module implements and the sole source of truth for the algorithm. The
 * headers state only what the C++ adds on top of it: ownership, nullability,
 * build-type behaviour and garbage collection.
 *
 * CACHE GATING, the rule the whole module follows. Every memo table is an
 * entry of ctx.h's `enum class table`. The five §1 result tables are caches:
 * they exist under `#ifdef TAU_CACHE` only, which is off in Debug builds, so
 * no result may depend on a hit. The structural per-node tables
 * (`atoms_memo`, `size_memo`, `neg_memo`, `negative_tree_memo`) hold facets
 * of a single node, always exist, and may therefore hand out references, like
 * `get_free_vars`' own table. The `#ifdef` itself appears in exactly one
 * place: ctx.tmpl.h's `table_ptr`.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__FWD_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__FWD_H__

#include <functional>
#include <utility>
#include <vector>

#include "tau_tree.h"
#include "tau_bdd.h"

namespace idni::tau_lang::anti_prenexing {

/// §1 BLOCK: the variables of one quantifier block, as an ordered list,
/// OUTERMOST FIRST — `X[0]` is the outermost binder. Every block-valued
/// parameter in the module is one.
using block = trefs;

/// A Boolean-algebra (BA) type id, as `tree<node>::get_ba_type()` returns it
/// and `ba_types<node>` hands it out. `0` is the codebase's untyped id.
using ba_type_id = size_t;

/// Kind of a quantifier, `∃` or `∀`. Covers a formula binder (`wff_ex` /
/// `wff_all`) and a term-level FUNCTIONAL QUANTIFIER (`bf_fex` / `bf_fall`,
/// §1) alike. It is the BDD library's own enum — `tau_term_bdd<node>::ex` and
/// `::all`, an unscoped enum inside that class — so a kind reaches
/// `bdd_quant` and `build_functional_quantifiers` unchanged and no
/// translation table is needed anywhere. The alias only keeps signatures
/// readable.
template <NodeType node>
using quantifier = typename tau_term_bdd<node>::Quantifier;

/// The three answers of `ASK` (§7); the value type of §1 `solver_memo`.
enum class answer { sat, unsat, unknown };

/// §1 `ctx.order` and `ctx.prio`: a variable → integer rank map. The BDD
/// library's own map type, so an order can be handed to it unchanged. The two
/// conventions are opposite: in `order` an inner variable has the LOWER rank,
/// in `prio` the HIGHER one. Ranks run 1..|P| (§5).
template <NodeType node>
using var_order = typename term_handle<node>::order;

/// §1 `keep_functional`: the ONE callback `ANTI_PRENEX` takes, a pure
/// predicate on a NODE, asked once per site. Per CHAIN
/// (`RESOLVE_FUNCTIONAL_PLAIN` at entry, `RESOLVE_FUNCTIONAL_BDD` at a
/// component's close, §3) it is handed the canonical chain's term node —
/// prefix outermost first with the kinds, body below it, both read off with
/// `strip_chain` (terms.h).
/// Per COMPONENT of a block (§5) it is handed `REWRAP(body, X)`: the ∃ wrap of
/// the whole block, its variables outermost first, over the body as that
/// component's setup finds it — the dualised matrix for a ∀ run, the earlier
/// components' results already in place. A node, rather than a prefix or a
/// variable list, so that the policy can look at what is quantified.
template <NodeType node>
using keep_functional_fn = std::function<bool(tref)>;

/// The default `keep_functional_fn`: keep nothing, whatever the node. Never
/// an empty `std::function`, which would throw when called.
template <NodeType node>
inline const keep_functional_fn<node> keep_no_functional =
	[](tref) { return false; };

/**
 * @brief A set of node handles stored as a table VALUE: a vector kept sorted
 * by `subtree_less<node>`, hence binary-searchable, like `get_free_vars`'
 * result. The value type of `atoms_memo`.
 *
 * `for_each_tref` opts the type into the garbage collector's introspection
 * (`HasForEachTref`), so the collector pins and checks the members whether or
 * not they are subtrees of the key. A plain `trefs` value is deliberately NOT
 * walked by the tree's caches, which is why this wrapper exists.
 */
struct tref_set {
	trefs items;
	void for_each_tref(auto&& f) const { for (tref t : items) f(t); }
};

/**
 * @brief §1 `cof_memo` entry: the value of `COF(f, x)`, the record of
 * cofactoring the term `f` by the variable `x`.
 *
 * `f0` and `f1` are the simplified cofactors `f[x ← 0]` and `f[x ← 1]`, taken
 * by child selection in the BDD (§1) or, at §3 `SIMPLIFY`'s pin match, by
 * substitution on a plain term. `usable` holds iff
 * `x ∉ FV(f₀) ∪ FV(f₁)`; otherwise `x` still hides inside a leaf — the LEAF
 * HAZARD of §1 — and the record must not be used. `p` is the residual
 * `SIMPLIFY_TERM(f₀·f₁)` when `usable`, and `nullptr` (⊥) otherwise. `pin`
 * holds iff `usable` and `f₀ ∪ f₁ = 1`; a pin gives §3 `TRY_WITNESS` the
 * witness `f₁′` and the residual equation `p = 0`, and is STRICT when `p`
 * folds to `0`, so that the residual vanishes.
 *
 * The three terms are built by `COF` and are not subtrees of the key, so the
 * table has to pin them against garbage collection: `for_each_tref` opts this
 * type into the collector's introspection.
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
