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

namespace idni::tau_lang {

template<typename data_t, typename kind_t>
struct scoped_resolver {
	using scope_t = size_t;
	using element_t = std::pair<scope_t, data_t>;
	using kinds_t = std::map<element_t, kind_t>;
	using union_find_t = union_find<element_t, std::less<element_t>>;

	union_find_t uf;
	scope_t current = 0;
	std::deque<size_t> scopes { current };
	std::map<element_t, kind_t> kinds_;
	data_t minimum;
	kind_t unknown;

	scoped_resolver(const data_t& minimum, const kind_t& unknown):
		minimum(minimum), unknown(unknown) {}

	void open(const std::map<data_t, kind_t>& kinds) {
		scopes.push_back(++current);
		for (const auto& [data, kind] : kinds) {
			uf.insert({current, data});
			kinds_.emplace({current, data}, kind);
		}
	}

	void close() {
		if (scopes.size() == 1) return;
		kinds_.erase(
			kinds_.lower_bound({current, minimum}),
			kinds_.end());
		scopes.pop_back();
		current = scopes.back();
	}

	void insert(const data_t& data) {
		static size_t global = 0;
		for(auto it = scopes.rbegin(); it != scopes.rend(); ++it)
			if (uf.contains(element_t{*it, data})) return;
		uf.insert({global, data});
		kinds_.emplace(element_t{global, data}, unknown);
	}

	kind_t type_of(const data_t& data) {
		for(auto it = scopes.rbegin(); it != scopes.rend(); ++it)
			if (uf.contains({*it, data}))
				return kinds_.find(uf.root({*it, data}))->second;
		return unknown;
	}

	bool same_kind(const data_t& data1, const data_t& data2) {
		return type_of(data1) == type_of(data2);
	}

	bool assign(const data_t& data, const kind_t& kind) {
		for(auto it = scopes.rbegin(); it != scopes.rend(); ++it)
			if (uf.contains({*it, data})) {
				if (auto current = kinds_.find(uf.root({*it, data}))->second;
						current != unknown && current != kind)
					return false;
				kinds_[uf.root({*it, data})] = kind;
				return true;
			}
		return false;
	}

	std::map<data_t, kind_t> kinds() {
		std::map<data_t, kind_t> result ;
		// Note that we will overwrite the kind of a data if it appears in
		// multiple scopes, keeping only the inner one.
		for(auto it = uf.begin(); it != uf.end(); ++it)
			if (kinds_.find(it->second) != kinds_.end())
				result[it->second.second] = type_of(it->second.second);
		return result;
	}
};

template<NodeType node>
using type_scopes = scoped_resolver<node, typename std::string>;
} // namespace idni::tau_lang

#endif // __IDNI__TAU__RESOLVER_H__