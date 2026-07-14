// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file normalizer_uf_arithmetic.h
 * @brief Scoped-union-find-based bitvector-arithmetic taint tracking.
 *
 * Reimplements the intent of `heuristics/bv_arithmetic_scan.h`'s
 * `make_bv_arithmetic_skip` using a `scoped_union_find` (see
 * `type_scoped_resolver.h` for the established pattern this follows),
 * unifying each atomic formula with its own BV-typed free variables so
 * arithmetic taint propagates to all of them, not just the direct operands
 * of an arithmetic operator. Standalone: not wired into the normalizer.
 */

#ifndef __IDNI__TAU__NORMALIZER_UF_ARITHMETIC_H__
#define __IDNI__TAU__NORMALIZER_UF_ARITHMETIC_H__

#include <functional>
#include <map>
#include <memory>
#include <optional>

#include "tau_tree.h"
#include "union_find.h"
#include "ba_types.h"

namespace idni::tau_lang {

/** @brief The two-element taint domain: `logical` (untainted) or `arithmetic` (tainted). */
enum class arith_kind { logical, arithmetic };

/**
 * @brief Join two `arith_kind`s: logical+logical->logical, otherwise arithmetic.
 *
 * A 2-element join semilattice with `arithmetic` as the top element; always succeeds.
 */
inline arith_kind unify(arith_kind a, arith_kind b) {
	return (a == arith_kind::arithmetic || b == arith_kind::arithmetic)
		? arith_kind::arithmetic : arith_kind::logical;
}

/**
 * @brief Scope-aware `arith_kind` resolver, following `type_scoped_resolver`'s pattern.
 *
 * Wraps a `scoped_union_find<tref, idni::subtree_less<node>>` plus a
 * root->`arith_kind` map. Unlike `type_scoped_resolver` (whose BA-type-lattice
 * `unify` can fail with `inference_error`), `arith_kind`'s join always
 * succeeds, so there is no error type here.
 *
 * Naming note: `insert` here delegates to the underlying union-find's
 * `push` (current-scope-only, no search) -- the opposite of
 * `type_scoped_resolver::insert`, which delegates to the union-find's
 * `insert` (search-enclosing-then-global). `bv_arithmetic_resolver` only
 * ever needs the direct-declare variant as its own public method.
 * @tparam node Tree node type satisfying `NodeType`.
 */
template<NodeType node>
struct bv_arithmetic_resolver {
	using uf_t = scoped_union_find<tref, idni::subtree_less<node>>;
	using element = typename uf_t::element;
	using scope = typename uf_t::scope;

	/** @brief Open a new nested scope. */
	void open();
	/** @brief Close the innermost scope. */
	std::optional<typename uf_t::scope_error> close();
	/**
	 * @brief Declare @p n as new in the current (innermost) scope with initial kind @p k.
	 * @return The scoped element for @p n.
	 */
	element insert(tref n, arith_kind k);
	/**
	 * @brief Return the joined `arith_kind` of @p n's root (untyped defaults to `logical`).
	 * @param n Node to query; searched across enclosing scopes, falling back to global.
	 */
	arith_kind kind_of(tref n);
	/**
	 * @brief Join @p k into @p n's root's kind.
	 * @return @p n's root element.
	 */
	element assign(tref n, arith_kind k);
	/**
	 * @brief Union the sets containing @p a and @p b, joining their kinds.
	 * @return The merged set's root element.
	 */
	element merge(tref a, tref b);

	uf_t scoped;
	std::map<element, arith_kind, scoped_less<tref, idni::subtree_less<node>>> kinds;
};

/**
 * @brief Collect bitvector-arithmetic-tainted variable/atomic-formula nodes in @p formula.
 *
 * Scopes on every quantifier (`wff_ex`/`wff_all`) so two unrelated quantifiers
 * binding the same variable name/`tref` never cross-taint each other. Unions
 * each atomic formula with its own BV-typed free variables, so taint
 * propagates to variables entangled in an arithmetic equation even when they
 * aren't a direct operand of the arithmetic operator.
 * @tparam node Tree node type.
 * @param formula Formula to scan.
 * @return Set of variable/atomic-formula nodes resolved to `arith_kind::arithmetic`.
 */
template <NodeType node>
subtree_unordered_set<node> collect_bv_arithmetic_taint_uf(tref formula);

/**
 * @brief Build a `skip` predicate from `collect_bv_arithmetic_taint_uf`'s result.
 * @tparam node Tree node type.
 * @param formula Formula to scan.
 * @return Predicate suitable as `anti_prenex_block`'s `skip` argument.
 */
template <NodeType node>
std::function<bool(tref)> make_bv_arithmetic_skip_uf(tref formula);

} // namespace idni::tau_lang

#include "normalizer_uf_arithmetic.tmpl.h"

#endif // __IDNI__TAU__NORMALIZER_UF_ARITHMETIC_H__
