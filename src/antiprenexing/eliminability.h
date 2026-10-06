// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file eliminability.h
 * @brief Per-block union-find analysis deciding which block variables may be
 * eliminated, and why the others may not.
 *
 * The analysis works per union-find *component*, not per clause, so a
 * reference freezes only the variables that actually reach it.
 *
 * Unlike its `block_*` / `boole_*` siblings, which self-include their `.tmpl.h`,
 * this header does not: `normal_forms.h` includes `eliminability.tmpl.h` after
 * `normal_forms.tmpl.h`, because the definitions here are consumed by
 * `antiprenexing.tmpl.h`, which has that ordering requirement.
 */

#ifndef __IDNI__TAU__ANTIPRENEXING_ELIMINABILITY_H__
#define __IDNI__TAU__ANTIPRENEXING_ELIMINABILITY_H__

#include <algorithm>
#include <map>
#include <optional>

#include "tau_tree.h"
#include "union_find.h"
#include "union_find_with_sets.h"
#include "ba_types.h"

namespace idni::tau_lang {

/**
 * @brief Why a block variable may or may not be eliminated.
 *
 * The distinction that matters most is `frozen` ("do not touch") versus
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
	arithmetic  = 2, ///< Bitvector arithmetic blasting cannot express --
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
 * @brief Does @p form carry a constant (or typed `T`/`F`) of a Boolean
 * algebra whose type does not declare arithmetic operators?
 *
 * Such content is invisible to an arithmetic BA's own solver (e.g. bv's
 * cvc5-backed one), so no arithmetic scope sharing a formula with it can ever
 * be decided by that solver -- callers use this to tell "the solver owns this
 * arithmetic content" apart from "nothing here will ever decide it". Untyped
 * `T`/`F` carries no algebra of its own and does not count. Routed through
 * `pack_type_has_arith_ops` rather than any one BA's own type-family test, so
 * it holds for whichever BA(s) of the pack declare `arith_ops`.
 * @tparam node Tree node type.
 * @param form Formula to scan.
 * @return `true` if a constant of a non-arithmetic-owning algebra occurs in
 * @p form.
 */
template <NodeType node>
bool has_foreign_arith_constant(tref form);

/**
 * @brief Formula-wide inputs the per-block analysis cannot derive from a block.
 *
 * `arith_is_solver_owned` is genuinely a property of the whole formula: a
 * constant of another Boolean algebra (a `:tau` spec constant, say) is one
 * the arithmetic BA's own solver cannot translate at all, so *no* arithmetic
 * scope anywhere in the formula will ever be decided by it. Computed once at
 * pipeline entry with `!has_foreign_arith_constant<node>(form)` and carried
 * down.
 */
template <NodeType node>
struct analysis_context {
	/// `false` when the formula holds a constant the arithmetic solver
	/// cannot translate; see the struct comment.
	bool arith_is_solver_owned = true;
};

/**
 * @brief Verdict-per-node analysis result.
 *
 * `verdicts` holds every var and atom analysed; `members` groups the same
 * nodes by verdict. `arith_floor`, when set, makes any node whose type's
 * owning BA declares `arith_ops` (`pack_type_has_arith_ops`; bv is the only
 * in-tree algebra declaring it today) at least `blasteable` even if the
 * analysis never keyed it, since `find_top(skip)` may probe arbitrary subtree
 * nodes, not only analysed vars/atoms.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct eliminability {
	/// Explicit verdict of every analysed variable and atom.
	subtree_unordered_map<node, elim_verdict> verdicts;
	/// The nodes of `verdicts`, grouped by verdict.
	std::map<elim_verdict, subtree_unordered_set<node>> members;
	/// Floor unanalysed arith-typed nodes to `blasteable`.
	bool arith_floor = false;

	/**
	 * @brief Verdict of @p n: its explicit entry if analysed, else
	 * `blasteable` for an arith-typed node under `arith_floor`, else
	 * `eliminable`.
	 * @param n Node to query; null yields `eliminable`.
	 * @return The verdict of @p n.
	 */
	elim_verdict verdict_of(tref n) const {
		// A null tref has no verdict to look up, and hashing it
		// (subtree_unordered_map's key hash) dereferences the node --
		// guard before that lookup rather than crash on it.
		if (!n) return elim_verdict::eliminable;
		// An explicit verdict beats the floor in BOTH directions: an
		// analysed node recorded `eliminable` stays eliminable even if its
		// type declares arith_ops. This is the whole mechanism behind
		// "arith-typed variables that appear only in atoms that are purely
		// BA are also eliminable" (user directive 2026-08-14):
		// `detail::atom_arith_verdict` seeds such an atom `eliminable` and
		// the analysis records it -- and its variables -- explicitly, while
		// every arith-typed node the analysis never keyed still floors to
		// `blasteable`.
		if (auto it = verdicts.find(n); it != verdicts.end())
			return it->second;
		if (arith_floor && pack_type_has_arith_ops<node>(
			tree<node>::get(n).get_ba_type()))
			return elim_verdict::blasteable;
		return elim_verdict::eliminable;
	}
	/// @brief `true` unless @p n is `eliminable` (`verdict_of`).
	bool skip(tref n) const {
		return verdict_of(n) != elim_verdict::eliminable;
	}
	/**
	 * @brief `true` if @p f holds any content this pass must not rewrite --
	 * the subtree form of `skip`, and the only correct way to ask it.
	 *
	 * `find_top(skip)` is NOT that form. `skip` floors on a node's BA TYPE,
	 * and inside an arith-typed atom the terms, the constants and the `bf`
	 * wrappers all carry that type, while the atom itself is a `wff` whose
	 * own type id is 0. So a plain `find_top(skip)` reports skip content
	 * inside an atom this analysis explicitly classified `eliminable`, and
	 * every pure-BA arith-typed clause would still be diverted to blasting --
	 * the directive defeated by its own probe.
	 *
	 * This walks the same subtree but stops at an atom the analysis covers:
	 * one holding at least one arith-typed free variable, all of whose
	 * arith-typed free variables carry an EXPLICIT `eliminable` verdict. Such
	 * an atom is pure-BA by construction (the analyses union each atom with
	 * its arith-typed variables, so one arithmetic or frozen atom anywhere in
	 * a variable's component lifts every variable of that component above
	 * `eliminable`), and the floored nodes below it are exactly its own terms.
	 *
	 * Keyed on the VARIABLES rather than on the atom's own recorded verdict:
	 * variable trefs survive the rewrites between analysis and use
	 * (`to_nnf`, `normalize_atomic_formula_operators`, Boole splits), whereas
	 * a rebuilt atom loses its entry and would silently fall back to the
	 * floor.
	 *
	 * Under `none()` nothing is ever skipped, and under `arith_only()` no
	 * explicit verdicts exist, so no atom is ever covered.
	 * @param f Subtree to scan.
	 * @return `true` if some node of @p f outside a covered atom is skipped.
	 */
	bool has_skip_content(tref f) const;
	/**
	 * @brief `true` if @p f holds any node carrying an explicit `frozen`
	 * verdict -- a reference or a kept binder somewhere in its scope.
	 *
	 * Memoized per queried @p f (`frozen_memo`), because it is called at
	 * several `blast_block` call sites during one `anti_prenex_block`
	 * invocation and is a full subtree walk each time.
	 *
	 * Unlike `has_skip_content`, this needs no `covers_atom`-style prune:
	 * `frozen` is only ever recorded as an EXPLICIT verdict (seeded at an
	 * unresolved reference or a kept binder, `eliminability.tmpl.h`'s
	 * `analyse_block`/`analyse_formula`) -- it is never produced by
	 * `arith_floor`, which only ever floors to `blasteable`. So there is no
	 * floor to distinguish from an analysed atom's own terms, and a plain
	 * `verdict_of(n) == frozen` hit test is exact.
	 * @param f Subtree to scan.
	 * @return `true` if some node of @p f has verdict `frozen`.
	 */
	bool has_frozen(tref f) const;
	/// Everything-eliminable instance: skips nothing at all.
	static eliminability none() { return {}; }
	/// Instance with no explicit verdicts and `arith_floor` set: skips
	/// exactly the arith-typed nodes.
	static eliminability arith_only() { eliminability e; e.arith_floor = true; return e; }

private:
	/**
	 * @internal
	 * @brief `true` if @p n is an atom whose whole subtree `has_skip_content`
	 * may stop at. See there for what it means and why it keys on variables.
	 * @endinternal
	 */
	bool covers_atom(tref n) const;
	/// @internal Per-`f` memo backing `has_frozen`. @endinternal
	mutable subtree_unordered_map<node, bool> frozen_memo;
};

/**
 * @brief Result of analysing one quantifier block over its own body.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct block_eliminability : eliminability<node> {
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
	 * counted here.
	 *
	 * The `bf_ref` case deserves a note: `analyse_block`'s
	 * `is_ref_fm` recognises only `wff_ref`, so `g(y) = 0` is classified an
	 * ordinary eliminable atom. That is handled, not a gap:
	 * `eliminate_block_over_clause` prunes vacuous binders so such a clause
	 * reaches the single-variable squeeze, which substitutes constants INTO
	 * reference arguments (`g(0)`, `g(1)`) and settles it -- pinned by
	 * "a term containing a bf_ref still normalizes"
	 * (test_integration-normalizer_helpers.cpp).
	 */
	bool has_reference() const { return has_ref; }

	/// Per block variable, the conjuncts in its union-find component.
	subtree_unordered_map<node, trefs> components;
	/// Backing flag of `has_reference`.
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
 * @return `true` if @p l orders before @p r under `subtree_less`.
 */
template <NodeType node>
bool eliminability_comp(tref l, tref r);

/**
 * @brief Scope-aware resolver over the `elim_verdict` join semilattice.
 *
 * A scope-tagged union-find plus a root->kind map over the `elim_verdict`
 * lattice (`join`, not `unify`); `analyse_formula` runs two instances, one
 * for reference usage and one for arithmetic content.
 *
 * `insert` delegates to the underlying union-find's `push`
 * (current-scope-only, no search) -- the opposite of `type_scoped_resolver`'s
 * `insert`, which searches enclosing scopes before falling back to global.
 * @tparam node Tree node type satisfying `NodeType`.
 */
template<NodeType node>
struct scoped_verdict_resolver {
	/// Underlying scoped union-find.
	using uf_t = scoped_union_find<tref, idni::subtree_less<node>>;
	/// A (scope, node) element of `uf_t`.
	using element = typename uf_t::element;
	/// A scope id of `uf_t`.
	using scope = typename uf_t::scope;

	/** @brief Open a new nested scope. */
	void open();
	/**
	 * @brief Close the innermost scope.
	 * @return The union-find's scope error, or nullopt on success.
	 */
	std::optional<typename uf_t::scope_error> close();
	/**
	 * @brief Declare @p n as new in the current (innermost) scope with initial kind @p k.
	 * @param n Node to declare.
	 * @param k Initial kind.
	 * @return The scoped element for @p n.
	 */
	element insert(tref n, elim_verdict k);
	/**
	 * @brief Return the joined `elim_verdict` of @p n's root (unseen defaults to `eliminable`).
	 *
	 * Inserts @p n, and an `eliminable` kind for its root, when unseen.
	 * @param n Node to query; searched across enclosing scopes, falling back to global.
	 * @return The kind of @p n's root.
	 */
	elim_verdict kind_of(tref n);
	/**
	 * @brief Join @p k into @p n's root's kind.
	 * @param n Node whose root is updated; inserted when unseen.
	 * @param k Kind to join in.
	 * @return @p n's root element.
	 */
	element assign(tref n, elim_verdict k);
	/**
	 * @brief Union the sets containing @p a and @p b, joining their kinds.
	 * @param a First node.
	 * @param b Second node.
	 * @return The merged set's root element.
	 */
	element merge(tref a, tref b);

	/// The scoped union-find.
	uf_t scoped;
	/// Kind of each root element.
	std::map<element, elim_verdict, scoped_less<tref, idni::subtree_less<node>>> kinds;
};

/**
 * @brief Analyse @p form as a whole, producing one verdict per variable and
 * atomic formula (and predicate reference) it contains.
 *
 * One `pre_order` traversal drives TWO `scoped_verdict_resolver`s in
 * lockstep, because the two propagation domains must stay separate for
 * precision:
 * - the *ref* resolver seeds
 *   `frozen` at every `wff_ref` and unions each `wff_ref`/atom with ALL its
 *   free variables;
 * - the *arith* resolver seeds
 *   `blasteable` or `arithmetic` at atoms holding an arithmetic operator
 *   (`eliminable` otherwise) and unions each atom with only its free
 *   variables whose type's owning BA declares `arith_ops`
 *   (`pack_type_has_arith_ops`).
 *
 * Both resolvers open/close a scope at every quantifier and insert its bound
 * variable into both, so two unrelated binders of the same name never
 * cross-contaminate. Every node either resolver's scope registers is
 * snapshotted -- on scope close, and once more for the global scope after the
 * traversal -- with `verdict_of(n) = join(ref-resolver's kind_of(n),
 * arith-resolver's kind_of(n))` (both resolvers hold `elim_verdict` directly, so
 * this join subsumes the boolean "does the ref side say frozen" check).
 *
 * `res.arith_floor` is set from `ctx.arith_is_solver_owned` before returning.
 * @tparam node Tree node type.
 * @param form Formula to analyse.
 * @param ctx Formula-wide inputs.
 * @return The verdicts of the variables, atoms and references of @p form.
 */
template <NodeType node>
eliminability<node> analyse_formula(tref form, const analysis_context<node>& ctx);

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
 * `eliminable`. An atom's seed comes from its arithmetic operators alone
 * (`blasteable` or `arithmetic`), so an arith-typed atom with no arithmetic
 * operator is seeded `eliminable`; @p ctx.arith_is_solver_owned does not
 * change the seeds.
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
 * @return The verdicts and, per block variable, its component's conjuncts;
 * empty when @p block_vars is empty.
 */
template <NodeType node>
block_eliminability<node> analyse_block(const trefs& block_vars,
	const trefs& conjuncts, const analysis_context<node>& ctx);

} // namespace idni::tau_lang

#endif // __IDNI__TAU__ANTIPRENEXING_ELIMINABILITY_H__
