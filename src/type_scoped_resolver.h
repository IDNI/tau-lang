// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__TYPE_SCOPED_RESOLVER_H__
#define __IDNI__TAU__TYPE_SCOPED_RESOLVER_H__

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

struct inference_error {
	tref element;
	size_t expected;
	size_t found;
};

template<NodeType node>
struct type_scoped_resolver {
	using element = scoped_union_find<tref, idni::subtree_less<node>>::element;
	using scope = scoped_union_find<tref, idni::subtree_less<node>>::scope;
	using scope_error = scoped_union_find<tref, idni::subtree_less<node>>::scope_error;
	using type_id = size_t;

	using tau = tree<node>;
	using tt = tau::traverser;

	void open(const subtree_map<node, type_id>& elements = {});
	std::optional<scope_error> close();
	element insert(tref n);
	type_id type_id_of(tref n);
	scope scope_of(tref n);
	std::variant<size_t, inference_error> assign(tref a, type_id tid);
	// merge two trefs if the types are compatible
	// returns true if merge was successful, false otherwise
	std::variant<size_t, inference_error> merge(tref a, tref b);
	std::variant<size_t, inference_error> merge(const trefs& ts);
	subtree_map<node, type_id> current_types();
	subtree_map<node, type_id> all_types();

#ifdef DEBUG
	std::ostream& dump(std::ostream& os);
	std::string dump_to_str();
#endif // DEBUG

	scoped_union_find<tref, idni::subtree_less<node>> scoped;
	std::map<element, type_id, scoped_less<tref, idni::subtree_less<node>>> type_ids; // map from tref to ba_type id
};

// Helper functions

// The inputs usually take as argument a std::initializer_list of maps
// that associate types (tau::variable, tau::ba_constant,...) to subtree_map of
// nodes to its type ids (tau, sbf,...) or maybe just a map of the former type.

template<NodeType node>
std::optional<inference_error> insert(type_scoped_resolver<node>& resolver, const std::map<size_t, subtree_map<node, size_t>>& types);

template<NodeType node>
std::optional<inference_error> insert(type_scoped_resolver<node>& resolver, const std::initializer_list<subtree_map<node, size_t>>& types);

template<NodeType node>
void open(type_scoped_resolver<node>& resolver, const std::initializer_list<trefs>& ns, size_t type);

template<NodeType node>
void open(type_scoped_resolver<node>& resolver, const std::initializer_list<trefs>& ns, tref type);

template<NodeType node>
void open(type_scoped_resolver<node>& resolver, const std::map<size_t, subtree_map<node, size_t>>& types);

template<NodeType node>
void open(type_scoped_resolver<node>& resolver, const std::initializer_list<subtree_map<node, size_t>>& types);

template<NodeType node>
std::variant<size_t, inference_error> open_same_type(type_scoped_resolver<node>& resolver, const subtree_set<node>& refs,
		size_t inferred_type);

template<NodeType node>
std::variant<size_t, inference_error> open_same_type(type_scoped_resolver<node>& resolver, const std::map<size_t, subtree_map<node, size_t>>& types,
		size_t inferred_type);

template<NodeType node>
std::variant<size_t, inference_error> open_same_type(type_scoped_resolver<node>& resolver, const std::initializer_list<subtree_map<node, size_t>>& types,
		size_t inferred_type);

template<NodeType node>
std::variant<size_t, inference_error> merge(type_scoped_resolver<node>& resolver, const std::initializer_list<subtree_map<node, size_t>>& types);


template<NodeType node>
std::variant<size_t, inference_error> merge(type_scoped_resolver<node>& resolver, const std::map<size_t, subtree_map<node, size_t>>& types);

template<NodeType node>
std::variant<size_t, inference_error> unify(const std::map<size_t, subtree_map<node, size_t>>& types, size_t default_type);

} // namespace idni::tau_lang

#include "type_scoped_resolver.tmpl.h"

#endif // __IDNI__TAU__TYPE_SCOPED_RESOLVER_H__
