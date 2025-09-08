// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include <map>
#include <memory>
#include <vector>
#include <ranges>
#include <limits.h>

#include "tau_tree.h"
#include "union_find.h"

namespace idni::tau_lang {

template<typename data_t, typename kind_t>
struct resolver {
	using scope_t = size_t;
	using element_t = std::pair<scope_t, data_t>;
	using kinds_t = std::map<element_t, kind_t>;
	using union_find_t = union_find<element_t, std::less<element_t>>;
	using resolver_t = resolver<data_t, kind_t>;

	union_find_t uf;
	scope_t current = 0;
	std::deque<size_t> scopes { current };
	std::map<element_t, kind_t> kinds_;
	data_t minimum;
	kind_t unknown;

	resolver(const data_t& minimum, const kind_t& unknown):
		minimum(minimum), unknown(unknown) {}

	void open(const std::map<data_t, kind_t>& kinds) {
		scopes.push_back(++current);
		for (const auto& [data, kind] : kinds)
			kinds_.emplace({current, data}, kind);
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
		return kinds_[uf.root({current, data})];
/*		for(auto it = scopes.rbegin(); it != scopes.rend(); ++it)
			if (uf.contains({*it, data}))
				return kinds_.find(uf.root({*it, data}))->second;
		return unknown;*/
	}

	kind_t type_of(const element_t& element) {
		return type_of(element.second);
	}

	bool merge(const data_t& data1, const data_t& data2) {
		auto kind1 = type_of(data1);
		auto kind2 = type_of(data2);
		if (kind1 != unknown && kind2 != unknown && kind1 != kind2)
			return false;
		auto kind = (kind1 == unknown) ? kind2 : kind1;
		uf.merge({current, data1}, {current, data2});
		return kinds_.emplace(uf.root({current, data1}), kind), true;
	}

	bool same_kind(const data_t& data1, const data_t& data2) {
		return type_of(data1) == type_of(data2);
	}

	bool assign(const data_t& data, const kind_t& kind) {
		for(auto it = scopes.rbegin(); it != scopes.rend(); ++it)
			if (auto uf_it = uf.find({*it, data}); uf_it != uf.end())
				return kinds_.emplace(uf.root({*it, data}), kind), true;
		return false;
	}

	std::map<data_t, kind_t> kinds() {
		std::map<data_t, kind_t> result ;
		// Note that we will overwrite the kind of a data if it appears in
		// multiple scopes, keeping only the inner one.
		for (auto it = kinds_.lower_bound({current, minimum}); it != kinds_.end(); ++it)
			result[it->first.second] = type_of(it->first.second);
		return result;
	}
};

template<NodeType node>
using type_resolver = resolver<node, typename std::string>;
} // namespace idni::tau_lang