// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file type_scoped_resolver.h
 * @brief Scope-aware type inference resolver for Tau formula tree nodes.
 *
 * `type_scoped_resolver<node>` wraps a `scoped_union_find` to propagate and
 * unify BA type ids for formula nodes across nested scopes. Free-function
 * helpers (`insert`, `open`, `open_same_type`, `merge`, `unify`) provide
 * convenient bulk operations used by the type-inference passes.
 */

#ifndef __IDNI__TAU__TYPE_SCOPED_RESOLVER_H__
#define __IDNI__TAU__TYPE_SCOPED_RESOLVER_H__

#include <map>
#include <memory>
#include <vector>
#include <ranges>
#include <limits.h>

#include "tau_tree.h"
#include "union_find.h"
#include "ba_types.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "resolver"

namespace idni::tau_lang {

/**
 * @brief Error value returned when two incompatible type ids are unified.
 */
struct inference_error {
	tref element;    ///< Formula node that triggered the conflict.
	size_t expected; ///< Type id that was expected.
	size_t found;    ///< Type id that was encountered.

	/// @brief Record a conflict on @p element (must not be null).
	inference_error(tref element, size_t expected, size_t found)
			: element(element), expected(expected), found(found) {
		DBG(assert(element != nullptr);)
	}
};

/**
 * @brief Scope-aware type resolver that unifies BA type ids for Tau formula nodes.
 *
 * Uses a `scoped_union_find` internally so that type bindings introduced in an
 * inner scope can be queried from outer scopes but do not leak after the scope
 * is closed.
 * @tparam node Tree node type satisfying `NodeType`.
 */
template<NodeType node>
struct type_scoped_resolver {
	using element = scoped_union_find<tref, idni::subtree_less<node>>::element;
	using scope = scoped_union_find<tref, idni::subtree_less<node>>::scope;
	using scope_error = scoped_union_find<tref, idni::subtree_less<node>>::scope_error;
	using type_id = size_t;

	using tau = tree<node>;
	using tt = tau::traverser;

	/**
	 * @brief Open a new nested scope, optionally pre-populating it with @p elements.
	 * @param elements Node-to-type-id pairs to insert into the new scope; the
	 *        ids are stored as given, without unification.
	 */
	void open(const subtree_map<node, type_id>& elements = {});
	/**
	 * @brief Close the innermost scope.
	 * @return `scope_error` if the global scope would be closed, else `std::nullopt`.
	 */
	std::optional<scope_error> close();
	/**
	 * @brief Insert node @p n into the current scope if not already present.
	 * @return The scoped element for @p n.
	 */
	element insert(tref n);
	/**
	 * @brief Return the type id assigned to @p n, or
	 * `untyped_type_id<node>()` if none is known.
	 *
	 * Not a const lookup: @p n is inserted into the current scope if
	 * absent, and an untyped entry is recorded for its root.
	 * @param n Formula node to query.
	 */
	type_id type_id_of(tref n);
	/**
	 * @brief Return the scope level at which @p n was registered.
	 *
	 * Not a const lookup: @p n is inserted into the current scope if
	 * absent.
	 * @param n Formula node to query.
	 */
	scope scope_of(tref n);
	/**
	 * @brief Assign type @p tid to node @p a.
	 *
	 * Inserts @p a into the current scope if absent and stores the type on
	 * its root, unified with the type the root already has.
	 * @return The resulting type id, or `inference_error` on conflict.
	 */
	std::variant<size_t, inference_error> assign(tref a, type_id tid);
	/**
	 * @brief Merge the type sets of @p a and @p b if their types are compatible.
	 * @return The unified type id, or `inference_error` on conflict.
	 */
	std::variant<size_t, inference_error> merge(tref a, tref b);
	/**
	 * @brief Merge all type sets in @p ts into a single compatible set.
	 * @return The unified type id (untyped for an empty @p ts), or
	 *         `inference_error` on first conflict.
	 */
	std::variant<size_t, inference_error> merge(const trefs& ts);
	/** @brief Return the node-to-type-id map for the current (innermost)
	 * scope; a node with no type reads as untyped and gets that entry. */
	subtree_map<node, type_id> current_types();
	/** @brief Return the node-to-type-id map for all active scopes combined;
	 * a node present in several scopes appears once. */
	subtree_map<node, type_id> all_types();

#ifdef DEBUG
	/// @brief Dump resolver internals to @p os for debugging.
	std::ostream& dump(std::ostream& os);
	/// @brief Return a string dump of resolver internals for debugging.
	std::string dump_to_str();
#endif // DEBUG

	/// Scoped union-find over the formula nodes.
	scoped_union_find<tref, idni::subtree_less<node>> scoped;
	std::map<element, type_id, scoped_less<tref, idni::subtree_less<node>>> type_ids; ///< Node → BA type id map.
};

// ---------------------------------------------------------------------------
// Convenience free-function helpers
//
// These accept either a std::initializer_list or a std::map<size_t, …> of
// node-to-type-id maps, so callers don't have to unpack them manually.

/**
 * @brief Insert all nodes from @p types into @p resolver without opening a scope.
 * @tparam node Tree node type.
 * @param resolver Target resolver.
 * @param types Map from BA-type id to a node→type-id sub-map.
 * @return First `inference_error` encountered, or `std::nullopt` on success.
 */
template<NodeType node>
std::optional<inference_error> insert(type_scoped_resolver<node>& resolver, const std::map<size_t, subtree_map<node, size_t>>& types);

/**
 * @brief Insert all nodes from each map in @p types into @p resolver.
 * @tparam node Tree node type.
 * @param resolver Target resolver.
 * @param types List of node→type-id maps to bulk-insert.
 * @return First `inference_error` encountered, or `std::nullopt` on success.
 */
template<NodeType node>
std::optional<inference_error> insert(type_scoped_resolver<node>& resolver, const std::initializer_list<subtree_map<node, size_t>>& types);

/**
 * @brief Open a new scope in @p resolver and insert all nodes in @p ns with
 * type id @p type.
 * @tparam node Tree node type.
 */
template<NodeType node>
void open(type_scoped_resolver<node>& resolver, const std::initializer_list<trefs>& ns, size_t type);

/**
 * @brief Open a new scope in @p resolver and insert all nodes in @p ns with
 * the type id of the type tree @p type.
 * @tparam node Tree node type.
 */
template<NodeType node>
void open(type_scoped_resolver<node>& resolver, const std::initializer_list<trefs>& ns, tref type);

/**
 * @brief Open a new scope in @p resolver and bulk-insert @p types (map
 * variant), each node with its own type id and without unification.
 * @tparam node Tree node type.
 */
template<NodeType node>
void open(type_scoped_resolver<node>& resolver, const std::map<size_t, subtree_map<node, size_t>>& types);

/**
 * @brief Open a new scope in @p resolver and bulk-insert @p types (list
 * variant), each node with its own type id and without unification.
 * @tparam node Tree node type.
 */
template<NodeType node>
void open(type_scoped_resolver<node>& resolver, const std::initializer_list<subtree_map<node, size_t>>& types);

/**
 * @brief Assign @p inferred_type to every ref in @p refs IN THE CURRENT scope.
 *
 * Despite the name, this overload never opens a scope; it inserts each
 * ref and assigns the type where it stands, unified with any type the ref
 * already has.
 * @tparam node Tree node type.
 * @return @p inferred_type, or `inference_error` on the first conflict.
 */
template<NodeType node>
std::variant<size_t, inference_error> open_same_type(type_scoped_resolver<node>& resolver, const subtree_set<node>& refs,
		size_t inferred_type);

/**
 * @brief Unify @p inferred_type with every type listed in @p types, then
 * open a new scope registering every listed node with the unified type.
 *
 * The caller closes the scope. Nothing is opened on a conflict.
 * @tparam node Tree node type.
 * @return Unified type id, or `inference_error` on conflict.
 */
template<NodeType node>
std::variant<size_t, inference_error> open_same_type(type_scoped_resolver<node>& resolver, const std::map<size_t, subtree_map<node, size_t>>& types,
		size_t inferred_type);

/**
 * @brief Unify @p inferred_type with every type listed in @p types, in
 * order, then open a new scope registering each listed node with the type
 * unified up to and including that node.
 *
 * The caller closes the scope. Nothing is opened on a conflict.
 * @tparam node Tree node type.
 * @return The fully unified type id, or `inference_error` on conflict.
 */
template<NodeType node>
std::variant<size_t, inference_error> open_same_type(type_scoped_resolver<node>& resolver, const std::initializer_list<subtree_map<node, size_t>>& types,
		size_t inferred_type);

/**
 * @brief Merge all nodes listed in @p types into one set of @p resolver
 * (list variant); the listed type ids are not read.
 * @tparam node Tree node type.
 * @return Unified type id, or `inference_error` on conflict.
 */
template<NodeType node>
std::variant<size_t, inference_error> merge(type_scoped_resolver<node>& resolver, const std::initializer_list<subtree_map<node, size_t>>& types);

/**
 * @brief Merge all nodes listed in @p types into one set of @p resolver
 * (map variant); the listed type ids are not read.
 * @tparam node Tree node type.
 * @return Unified type id, or `inference_error` on conflict.
 */
template<NodeType node>
std::variant<size_t, inference_error> merge(type_scoped_resolver<node>& resolver, const std::map<size_t, subtree_map<node, size_t>>& types);

/**
 * @brief Unify @p default_type with every type id listed in @p types.
 *
 * Touches no resolver.
 * @tparam node Tree node type.
 * @return Unified type id (@p default_type when @p types is empty), or
 *         `inference_error` on conflict, whose `expected` is the listed type
 *         and `found` the type unified so far.
 */
template<NodeType node>
std::variant<size_t, inference_error> unify(const std::map<size_t, subtree_map<node, size_t>>& types, size_t default_type);

} // namespace idni::tau_lang

#include "type_scoped_resolver.tmpl.h"

#endif // __IDNI__TAU__TYPE_SCOPED_RESOLVER_H__
