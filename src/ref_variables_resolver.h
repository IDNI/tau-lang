// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ref_variables_resolver.h
 * @brief Scoped-union-find-based tracking of variables used in `wff_ref` predicates.
 *
 * Identifies variables that cannot be eliminated during normalization because
 * they appear as arguments of an unresolved predicate reference (`wff_ref`), or
 * are entangled with such a variable through a shared atomic formula. Uses a
 * `scoped_union_find` (see `type_scoped_resolver.h` for the established pattern
 * this follows), unifying each atomic formula with its own free variables so
 * `used` propagates to every variable sharing an atom with a predicate
 * argument, not just the arguments themselves.
 *
 * Structurally this mirrors `normalizer_uf_arithmetic.h`; the differences are
 * that the seed is a `wff_ref` node rather than an arithmetic operator, and
 * that the atom/variable union is not restricted by BA type (predicate
 * arguments carry no type restriction analogous to bitvector arithmetic's).
 */

#ifndef __IDNI__TAU__REF_VARIABLES_RESOLVER_H__
#define __IDNI__TAU__REF_VARIABLES_RESOLVER_H__

#include <functional>
#include <map>
#include <memory>
#include <optional>

#include "tau_tree.h"
#include "union_find.h"
#include "ba_types.h"

namespace idni::tau_lang {

/**
 * @brief The two-element domain: `unused` (eliminable) or `used` (must be skipped).
 */
enum class ref_variable_kind { unused, used };

/**
 * @brief Join two `ref_variable_kind`s: unused+unused->unused, otherwise used.
 *
 * A 2-element join semilattice with `used` as the top element; always succeeds.
 */
inline ref_variable_kind unify(ref_variable_kind a, ref_variable_kind b) {
	return (a == ref_variable_kind::used || b == ref_variable_kind::used)
		? ref_variable_kind::used : ref_variable_kind::unused;
}

/**
 * @brief Scope-aware `ref_variable_kind` resolver, following `type_scoped_resolver`'s pattern.
 *
 * Wraps a `scoped_union_find<tref, idni::subtree_less<node>>` plus a
 * root->`ref_variable_kind` map. Unlike `type_scoped_resolver` (whose
 * BA-type-lattice `unify` can fail with `inference_error`),
 * `ref_variable_kind`'s join always succeeds, so there is no error type here.
 *
 * Naming note: `insert` here delegates to the underlying union-find's
 * `push` (current-scope-only, no search) -- the opposite of
 * `type_scoped_resolver::insert`, which delegates to the union-find's
 * `insert` (search-enclosing-then-global). `ref_variables_resolver` only
 * ever needs the direct-declare variant as its own public method.
 * @tparam node Tree node type satisfying `NodeType`.
 */
template<NodeType node>
struct ref_variables_resolver {
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
	element insert(tref n, ref_variable_kind k);
	/**
	 * @brief Return the joined `ref_variable_kind` of @p n's root (unseen defaults to `unused`).
	 * @param n Node to query; searched across enclosing scopes, falling back to global.
	 */
	ref_variable_kind kind_of(tref n);
	/**
	 * @brief Join @p k into @p n's root's kind.
	 * @return @p n's root element.
	 */
	element assign(tref n, ref_variable_kind k);
	/**
	 * @brief Union the sets containing @p a and @p b, joining their kinds.
	 * @return The merged set's root element.
	 */
	element merge(tref a, tref b);

	uf_t scoped;
	std::map<element, ref_variable_kind, scoped_less<tref, idni::subtree_less<node>>> kinds;
};

/**
 * @brief Collect variable/atomic-formula nodes in @p formula that are used in a predicate.
 *
 * Scopes on every quantifier (`wff_ex`/`wff_all`) so two unrelated quantifiers
 * binding the same variable name/`tref` never cross-contaminate. Seeds `used`
 * at every `wff_ref` and unions it with its free variables; separately unions
 * each atomic formula with its own free variables, so `used` propagates to
 * variables that merely share an atom with a predicate argument.
 * @tparam node Tree node type.
 * @param formula Formula to scan.
 * @return Set of variable/atomic-formula nodes resolved to `ref_variable_kind::used`.
 */
template <NodeType node>
subtree_unordered_set<node> collect_used_ref_variables(tref formula);

/**
 * @brief Build a `skip` predicate from `collect_used_ref_variables`'s result.
 * @tparam node Tree node type.
 * @param formula Formula to scan.
 * @return Predicate that is `true` for nodes which must not be eliminated.
 */
template <NodeType node>
std::function<bool(tref)> make_ref_variables_skip(tref formula);

} // namespace idni::tau_lang

#include "ref_variables_resolver.tmpl.h"

#endif // __IDNI__TAU__REF_VARIABLES_RESOLVER_H__
