// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

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
bool type_scoped_resolver<node>::open(const subtree_map<node, typename type_scoped_resolver<node>::type_id>& elements) {
	scoped.open();
	for (auto [n, tid] : elements) {
		auto element = scoped.push(n);
		type_ids.emplace(element, tid);
	}
	return true;
}

template<NodeType node>
bool type_scoped_resolver<node>::close() {
	return scoped.close();
}

template<NodeType node>
typename type_scoped_resolver<node>::element type_scoped_resolver<node>::insert(tref n) {
	return scoped.insert(n);
}

template<NodeType node>
typename type_scoped_resolver<node>::type_id type_scoped_resolver<node>::type_id_of(tref n) {
	auto root = scoped.root(scoped.insert(n));
	if(auto it = type_ids.find(root); it != type_ids.end())
		return it->second;
	 // has no type info, type as untyped
	return type_ids.emplace(root, untyped_type_id<node>()).first->second;
}

template<NodeType node>
typename type_scoped_resolver<node>::scope type_scoped_resolver<node>::scope_of(tref n) {
	auto element = scoped.insert(n);
	return element.first;
}

template<NodeType node>
bool type_scoped_resolver<node>::assign(tref n, typename type_scoped_resolver<node>::type_id tid) {
	auto element = scoped.insert(n);
	auto root = scoped.root(element);
	if (auto it = type_ids.find(root); it != type_ids.end()) {
		auto merged_tid = unify<node>(it->second, tid);
		if (!merged_tid) return false; // conflicting type info
		type_ids.insert_or_assign(root, merged_tid.value());
		return true;
	}
	type_ids.insert_or_assign(element, tid);
	return true;
}

template<NodeType node>
bool type_scoped_resolver<node>::merge(tref a, tref b) {
	auto type_a = type_id_of(a);
	auto type_b = type_id_of(b);
	auto merged = unify<node>(type_a, type_b);
	if (!merged) return false; // conflicting type info
	auto new_root = scoped.merge(a, b);
	type_ids.insert_or_assign(new_root, merged.value());
	DBG(LOG_TRACE << "type_scoped_resolver/merge: "
		<< LOG_FM(a) << ":" << ba_types<node>::name(type_a)
		<< " (scope " << scoped.insert(a).first << ")"
		<< " <-> "
		<< LOG_FM(b) << ":" << ba_types<node>::name(type_b)
		<< " (scope " << scoped.insert(b).first << ")\n";)
	return true;
}

template<NodeType node>
bool type_scoped_resolver<node>::merge(const trefs& ts) {
	if (ts.size() < 2) return true;
	for (size_t i = 1; i < ts.size(); ++i)
		if (!merge(ts[0], ts[i])) return false;
	return true;
}

template<NodeType node>
subtree_map<node, typename type_scoped_resolver<node>::type_id> type_scoped_resolver<node>::current_types() {
	subtree_map<node, typename type_scoped_resolver<node>::type_id> current_types;
	auto current_scope = scoped.scopes.back();
	DBG(LOG_TRACE << "type_scoped_resolver/current_types/current_scope: " << current_scope << "\n";)
	for(auto [scoped, _] : scoped.uf)
		if (scoped.first == current_scope) {
			current_types[scoped.second] = type_id_of(scoped.second);
			DBG(LOG_TRACE << "\t" << LOG_FM_DUMP(scoped.second)
				<< " : " << ba_types<node>::name(current_types[scoped.second]) << "\n";)
		}
	return current_types;
}

template<NodeType node>
subtree_map<node, typename type_scoped_resolver<node>::type_id> type_scoped_resolver<node>::all_types() {
	subtree_map<node, typename type_scoped_resolver<node>::type_id> all_types;
	for(auto [scoped, _] : scoped.uf)
		all_types.insert_or_assign(scoped.second, type_id_of(scoped.second));
	return all_types;
}

#ifdef DEBUG

template<NodeType node>
std::ostream& type_scoped_resolver<node>::dump(std::ostream& os) {
	os << "scopes: ";
	for (auto scope: scoped.scopes)
		os << scope << ", ";
	os << "\n";
	for (auto [e,_]: scoped.uf) {
		auto type = type_ids.at(e);
		os << "\tscope: " << e.first << ", tref: " << LOG_FM(e.second)
			<< ", type: " << ba_types<node>::name(type) << "\n";
	}
	return os;
}

template<NodeType node>
std::string type_scoped_resolver<node>::dump_to_str() {
	std::stringstream ss;
	return dump(ss), ss.str();
}

#endif // DEBUG

// Helper functions

template<NodeType node>
bool insert(type_scoped_resolver<node>& resolver, const std::map<size_t, subtree_map<node, size_t>>& types) {
	for (auto [_, typeables] : types) {
		for (auto [t, type] : typeables) {
			resolver.insert(t);
			if (!resolver.assign(t, type)) return false;
		}
	}
	return true;
}

template<NodeType node>
bool insert(type_scoped_resolver<node>& resolver, const std::initializer_list<subtree_map<node, size_t>>& types) {
	for (auto typeables : types) {
		for (auto [t, type] : typeables) {
			resolver.insert(t);
			if (!resolver.assign(t, type)) return false;
		}
	}
	return true;
}

template<NodeType node>
bool open(type_scoped_resolver<node>& resolver, const std::initializer_list<trefs>& ns, size_t type) {
	subtree_map<node, size_t> scoped;
	for (auto typeables : ns)
		for (auto t : typeables)
			scoped[t] = type;
	return resolver.open(scoped);
}

template<NodeType node>
bool open(type_scoped_resolver<node>& resolver, const std::initializer_list<trefs>& ns, tref type) {
	auto type_id = ba_types<node>::type_id_from_ba_type(type);
	return open<node>(resolver, ns, type_id);
}

template<NodeType node>
bool open(type_scoped_resolver<node>& resolver, const std::map<size_t, subtree_map<node, size_t>>& types) {
	subtree_map<node, size_t> scoped;
	for (auto [_, typeables] : types)
		for (auto [t, type] : typeables)
			scoped[t] = type;
	return resolver.open(scoped);
}

template<NodeType node>
bool open(type_scoped_resolver<node>& resolver, const std::initializer_list<subtree_map<node, size_t>>& types) {
	subtree_map<node, size_t> scoped;
	for (auto typeables : types)
		for (auto [t, type] : typeables)
			scoped[t] = type;
	return resolver.open(scoped);
}

template<NodeType node>
bool open_same_type(type_scoped_resolver<node>& resolver, const std::map<size_t, subtree_map<node, size_t>>& types,
		size_t inferred_type) {
	subtree_map<node, size_t> scoped;
	//auto inferred_type = untyped_type_id<node>();
	for (auto [_, typeables] : types) {
		for (auto [t, type] : typeables) {
			if (!unify<node>(type, inferred_type)) return false;
			scoped[t] = inferred_type;
		}
	}
	return resolver.open(scoped);
}

template<NodeType node>
bool open_same_type(type_scoped_resolver<node>& resolver, const std::initializer_list<subtree_map<node, size_t>>& types,
		size_t inferred_type) {
	subtree_map<node, size_t> scoped;
	//auto inferred_type = untyped_type_id<node>();
	for (auto typeables : types) {
		for (auto [t, type] : typeables) {
			if(!unify<node>(inferred_type, type)) return false;
			scoped[t] = inferred_type;
		}
	}
	return resolver.open(scoped);
}

template<NodeType node>
std::optional<size_t> unify(const std::map<size_t, subtree_map<node, size_t>>& types, size_t default_type) {
	auto unified_type = default_type;
	for (auto [_, typeables] : types) {
		for (auto [_, type] : typeables) {
			if (auto new_unified_type = unify<node>(unified_type, type); new_unified_type) {
				unified_type = new_unified_type.value();
			} else return std::nullopt; // incompatible types
		}
	}
	return { unified_type };
}

template<NodeType node>
bool merge(type_scoped_resolver<node>& resolver, const std::initializer_list<subtree_map<node, size_t>>& types) {
	trefs mergeables;
	for (auto typeables : types)
		for (auto [t, _] : typeables)
			mergeables.push_back(t);
	return resolver.merge(mergeables);
}


template<NodeType node>
bool merge(type_scoped_resolver<node>& resolver, const std::map<size_t, subtree_map<node, size_t>>& types) {
	trefs mergeables;
	for (auto [_, typeables] : types)
		for (auto [t, _] : typeables)
			mergeables.push_back(t);
	return resolver.merge(mergeables);
}

} // namespace idni::tau_lang
