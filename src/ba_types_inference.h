// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

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

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "resolver"

namespace idni::tau_lang {

// Some type definitions
template<NodeType node>
static size_t bv8_type_id = ba_types<node>::id(bv_type<node>(8));
template<NodeType node>
static size_t bv16_type_id = ba_types<node>::id(bv_type<node>(16));
template<NodeType node>
static size_t bv32_type_id = ba_types<node>::id(bv_type<node>(32));

// Infers the types of variables and constants in the tree n. It assumes that
// the types of the scoped variables are known when closing the scope.
// If a variable or constant remains unassigned, it is assigned to tau.
// We assume that the types of the constants could also be propagated across
// scopes (in the future we will restrict it to equations)
// If conflicting type information is found, the function returns nullptr.
template <NodeType node>
std::pair<tref, subtree_map<node, size_t>> infer_ba_types(tref n, const subtree_map<node, size_t>& global_scope =
	subtree_map<node, size_t>());

} // namespace idni::tau_lang

#include "ba_types_inference.tmpl.h"

#endif // __IDNI__TAU__RESOLVER_H__