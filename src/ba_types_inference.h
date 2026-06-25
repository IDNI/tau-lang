// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ba_types_inference.h
 * @brief BA type inference for Tau formula trees.
 *
 * Provides `infer_ba_types` which propagates and unifies Boolean-algebra
 * type ids throughout a formula tree, using `type_scoped_resolver` internally.
 * Also exposes common BV-width type-id shorthands (`bv8_type_id`, etc.).
 */

#ifndef __IDNI__TAU__RESOLVER_H__
#define __IDNI__TAU__RESOLVER_H__

#include <map>
#include <memory>
#include <vector>
#include <ranges>
#include <limits.h>

#include "tau_tree.h"
#include "union_find.h"
#include "boolean_algebras/bv_ba.h"
#include "ba_types.h"

namespace idni::tau_lang {

/** @brief Cached type id for bitvector-8 under node @p node. */
template<NodeType node>
static size_t bv8_type_id = ba_types<node>::id(bv_type<node>(8));
/** @brief Cached type id for bitvector-16 under node @p node. */
template<NodeType node>
static size_t bv16_type_id = ba_types<node>::id(bv_type<node>(16));
/** @brief Cached type id for bitvector-32 under node @p node. */
template<NodeType node>
static size_t bv32_type_id = ba_types<node>::id(bv_type<node>(32));

/**
 * @brief Options controlling type inference behaviour.
 */
struct type_inference_options {
	bool use_defaults = true; ///< Assign unresolved nodes to the `tau` type if `true`.
};

/**
 * @brief Infer and propagate BA type ids for all nodes in the tree @p n.
 *
 * Traverses @p n, assigns type ids compatible with each node's constraints,
 * and propagates across scopes. Unassigned nodes receive the `tau` type.
 * Returns `nullptr` as the first element on type conflict.
 *
 * @tparam node Tree node type.
 * @param n Root formula node to type-check.
 * @param global_scope Optional map of globally known type bindings.
 * @param definition_heads Optional list of definition heads whose types
 *        are already fixed.
 * @param options Inference control flags.
 * @return Pair of (annotated tree root, node→type-id map), or
 *         `{nullptr, {}}` on conflict.
 */
template <NodeType node>
std::pair<tref, subtree_map<node, size_t>> infer_ba_types(tref n,
	const subtree_map<node, size_t>* global_scope = nullptr,
	const std::vector<htref> *definition_heads = nullptr,
	const type_inference_options& options = {});

} // namespace idni::tau_lang

#include "ba_types_inference.tmpl.h"

#endif // __IDNI__TAU__RESOLVER_H__