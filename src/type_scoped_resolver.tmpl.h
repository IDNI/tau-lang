// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

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
void type_scoped_resolver<node>::open(const subtree_map<node, typename type_scoped_resolver<node>::type_id>& elements) {
	scoped.open();
	for (auto [n, tid] : elements) {
		auto element = scoped.push(n);
		type_ids.emplace(element, tid);
	}
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
std::variant<size_t, inference_error> type_scoped_resolver<node>::assign(tref n, typename type_scoped_resolver<node>::type_id tid) {
	auto element = scoped.insert(n);
	auto root = scoped.root(element);
	if (auto it = type_ids.find(root); it != type_ids.end()) {
		auto merged_tid = unify<node>(it->second, tid);
		if (!merged_tid) return inference_error{n, it->second, tid}; // conflicting type info
		type_ids.insert_or_assign(root, merged_tid.value());
		return merged_tid.value();
	}
	type_ids.insert_or_assign(element, tid);
	return tid;
}

template<NodeType node>
std::variant<size_t, inference_error> type_scoped_resolver<node>::merge(tref a, tref b) {
	auto type_a = type_id_of(a);
	auto type_b = type_id_of(b);
	auto merged = unify<node>(type_a, type_b);
	if (!merged) return inference_error{a, type_a, type_b}; // conflicting type info
	auto new_root = scoped.merge(a, b);
	type_ids.insert_or_assign(new_root, merged.value());
	DBG(LOG_TRACE << "type_scoped_resolver/merge: "
		<< LOG_FM(a) << ":" << ba_types<node>::name(type_a)
		<< " (scope " << scoped.insert(a).first << ")"
		<< " <-> "
		<< LOG_FM(b) << ":" << ba_types<node>::name(type_b)
		<< " (scope " << scoped.insert(b).first << ")\n";)
	return merged.value();
}

template<NodeType node>
std::variant<size_t, inference_error> type_scoped_resolver<node>::merge(const trefs& ts) {
	if (ts.size() < 2) return type_id_of(ts[0]);
	size_t type_id = 0;
	for (size_t i = 1; i < ts.size(); ++i) {
		const auto res = merge(ts[0], ts[i]);
		if (std::holds_alternative<inference_error>(res)) return inference_error{ts[i], type_id_of(ts[0]), type_id_of(ts[i])};
		type_id = std::get<size_t>(res);
	}
	return type_id;
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
std::optional<inference_error> insert(type_scoped_resolver<node>& resolver, const std::map<size_t, subtree_map<node, size_t>>& types) {
	for (auto [_, typeables] : types) {
		for (auto [t, type] : typeables) {
			resolver.insert(t);
			if (auto assigned = resolver.assign(t, type); std::holds_alternative<inference_error>(assigned)) return std::get<inference_error>(assigned);
		}
	}
	return std::nullopt;
}

template<NodeType node>
std::optional<inference_error> insert(type_scoped_resolver<node>& resolver, const std::initializer_list<subtree_map<node, size_t>>& types) {
	for (auto typeables : types) {
		for (auto [t, type] : typeables) {
			resolver.insert(t);
			if (auto assigned = resolver.assign(t, type); std::holds_alternative<inference_error>(assigned)) return std::get<inference_error>(assigned);
		}
	}
	return std::nullopt;
}

template<NodeType node>
void open(type_scoped_resolver<node>& resolver, const std::initializer_list<trefs>& ns, size_t type) {
	subtree_map<node, size_t> scoped;
	for (auto typeables : ns)
		for (auto t : typeables)
			scoped[t] = type;
	return resolver.open(scoped);
}

template<NodeType node>
void open(type_scoped_resolver<node>& resolver, const std::initializer_list<trefs>& ns, tref type) {
	auto type_id = ba_types<node>::type_id_from_ba_type(type);
	return open<node>(resolver, ns, type_id);
}

template<NodeType node>
void open(type_scoped_resolver<node>& resolver, const std::map<size_t, subtree_map<node, size_t>>& types) {
	subtree_map<node, size_t> scoped;
	for (auto [_, typeables] : types)
		for (auto [t, type] : typeables)
			scoped[t] = type;
	return resolver.open(scoped);
}

template<NodeType node>
void open(type_scoped_resolver<node>& resolver, const std::initializer_list<subtree_map<node, size_t>>& types) {
	subtree_map<node, size_t> scoped;
	for (auto typeables : types)
		for (auto [t, type] : typeables)
			scoped[t] = type;
	return resolver.open(scoped);
}

template<NodeType node>
std::variant<size_t, inference_error> open_same_type(type_scoped_resolver<node>& resolver, const subtree_set<node>& refs,
		size_t default_type) {
	for (auto t : refs) {
		resolver.insert(t);
		if (auto assigned = resolver.assign(t, default_type); std::holds_alternative<inference_error>(assigned))
			return std::get<inference_error>(assigned);
	}
	return default_type;
}

template<NodeType node>
std::variant<size_t, inference_error> open_same_type(type_scoped_resolver<node>& resolver, const std::map<size_t, subtree_map<node, size_t>>& types,
		size_t default_type) {
	subtree_set<node> scoped;
	size_t inferred_type = default_type;
	for (auto [_, typeables] : types) {
		for (auto [typeable, type] : typeables) {
			auto unified = unify<node>(inferred_type, type);
			if (!unified) return inference_error{typeable, inferred_type, type};
			else inferred_type = unified.value();
			scoped.insert(typeable);
		}
	}
	return open_same_type(resolver, scoped, inferred_type);
}

template<NodeType node>
std::variant<size_t, inference_error> open_same_type(type_scoped_resolver<node>& resolver, const std::initializer_list<subtree_map<node, size_t>>& types,
		size_t default_type) {
	subtree_map<node, size_t> scoped;
	size_t inferred_type = default_type;
	//auto inferred_type = untyped_type_id<node>();
	for (auto typeables : types) {
		for (auto [t, type] : typeables) {
			auto unified = unify<node>(inferred_type, type);
			if (!unified) return inference_error{t, inferred_type, type};
			else inferred_type = unified.value();
			scoped[t] = default_type;
		}
	}
	resolver.open(scoped);
	return default_type;
}

template<NodeType node>
std::variant<size_t, inference_error> merge(type_scoped_resolver<node>& resolver, const std::initializer_list<subtree_map<node, size_t>>& types) {
	trefs mergeables;
	for (auto typeables : types)
		for (auto [t, _] : typeables)
			mergeables.push_back(t);
	return resolver.merge(mergeables);
}

template<NodeType node>
std::variant<size_t, inference_error> merge(type_scoped_resolver<node>& resolver, const std::map<size_t, subtree_map<node, size_t>>& types) {
	trefs mergeables;
	for (auto [_, typeables] : types)
		for (auto [t, _] : typeables)
			mergeables.push_back(t);
	return resolver.merge(mergeables);
}

template<NodeType node>
std::variant<size_t, inference_error> unify(const std::map<size_t, subtree_map<node, size_t>>& types, size_t default_type) {
	auto unified_type = default_type;
	for (auto [_, typeables] : types) {
		for (auto [typeable, type] : typeables) {
			if (auto unified = unify<node>(unified_type, type); unified) {
				unified_type = unified.value();
			} else return inference_error{ typeable, unified_type, type}; // incompatible types
		}
	}
	return unified_type;
}

} // namespace idni::tau_lang
