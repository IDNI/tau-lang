// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ba_types_inference.h
 * @brief BA type inference for Tau formula trees.
 *
 * Provides `infer_ba_types` which propagates and unifies Boolean-algebra
 * type ids throughout a formula tree, using `type_scoped_resolver` internally.
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
#include "ba_types.h"

namespace idni::tau_lang {

/**
 * @brief Options controlling type inference behaviour.
 */
struct type_inference_options {
	bool use_defaults = true; ///< Assign unresolved nodes to the `tau` type if `true`.
};

/// Seeding of the type resolver from a global scope (`infer_ba_types`
/// with a `global_scope`). The inference of a formula depends on the
/// scope entries of the streams the formula mentions (as io variables or
/// as the streams its input and output definitions declare) and on no
/// other, so the resolver is seeded with those only, one lookup per
/// mentioned stream. 0 = the whole scope is walked; 1 = by mention, the
/// default;
/// 2 = shadow: seed by mention, infer once more from the whole scope and
/// count every disagreement of the inferred tree or of a mentioned
/// stream's type in `type_scope_seed_mismatches`. The environment
/// variable TAU_TYPE_SCOPE_SEED (0, 1 or 2; any other value selects 0)
/// overrides the flag.
inline int type_scope_seed = 1;
/// Calls seeded by mention, and disagreements the shadow mode found.
inline size_t type_scope_seed_hits = 0;
inline size_t type_scope_seed_mismatches = 0;
inline int type_scope_seed_mode();

/// Merge the streams of an inference result into a global type scope: the
/// result seeded by mention holds the mentioned and the newly typed streams
/// and no other, so it is merged into the scope; a stream the inference
/// typed anew takes its new type.
template <NodeType node>
void merge_type_scope(subtree_map<node, size_t>& scope,
	const subtree_map<node, size_t>& inferred);

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