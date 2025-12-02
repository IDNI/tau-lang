// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

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

template<NodeType node>
struct type_scoped_resolver { 
	using element = scoped_union_find<tref, idni::subtree_less<node>>::element;
	using scope = scoped_union_find<tref, idni::subtree_less<node>>::scope;
	using type_id = size_t;
		
	using tau = tree<node>;
	using tt = tau::traverser;

	bool open(const subtree_map<node, type_id>& elements = {});
	void close();
	element insert(tref n);
	type_id type_id_of(tref n);
	scope scope_of(tref n);
	bool assign(tref a, type_id tid);
	// merge two trefs if the types are compatible
	// returns true if merge was successful, false otherwise
	bool merge(tref a, tref b);
	bool merge(const trefs& ts);
	subtree_map<node, type_id> current_types();
	subtree_map<node, type_id> all_types();
	 

#ifdef DEBUG
	std::ostream& dump(std::ostream& os);
	std::string dump_to_str();
#endif // DEBUG

	scoped_union_find<tref, idni::subtree_less<node>> scoped;
	std::map<element, type_id, scoped_less<tref, idni::subtree_less<node>>> type_ids; // map from tref to ba_type id
};

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