// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__UNION_FIND_H__
#define __IDNI__TAU__UNION_FIND_H__

#include <deque>

namespace idni::tau_lang {

/**
 * @brief Union find data structure with set retrieval for tau formulas
 * @tparam comp Comparison function used to decide root after merge,
 * taking the smaller as new root
 * @tparam node Type of tree node
 */
template <typename data_t, class less_t = std::less<data_t>>
struct union_find : public std::map<data_t, data_t, less_t> {

	// Insert element x if not present. Note that the signature is different
	// from those of vector	::insert as we want to return the element
	// inserted/found
	data_t insert(data_t x) {
		if (this->find(x) != this->end()) return x;
		this->emplace(x, x);
		return x;
	}

	// Find the root of the set containing x while inserting x if not present
	data_t root(data_t x) {
		if (auto it = this->find(x); it == this->end())
			return insert(x), x;
		else if (it->second != x) {
			// Path compression
			it->second = root(it->second);
			return it->second;
       	} else return it->second;
	}

	// Check if two elements are in the same set
	bool connected(data_t x, data_t y) {
		return root(x) == root(y);
	}
};


/**
 * @brief Union find data structure with set retrieval for tau formulas
 * @tparam comp Comparison function used to decide root after merge,
 * taking the smaller as new root
 * @tparam node Type of tree node
 */
template <typename data_t, class less_t = std::less<data_t>>
struct union_find_by_rank : public union_find<data_t, less_t> {
private:
	std::map<data_t, size_t> rank; // rank for union by rank

public:
	// Union the two sets containing x and y (union by rank)
	data_t merge(data_t x, data_t y) {
		auto root_x = this->root(x), root_y = this->root(y);
		auto rank_x = rank.contains(root_x) ? rank[root_x] : 0;
		auto rank_y = rank.contains(root_y) ? rank[root_y] : 0;
		if (root_x == root_y) return root_x;
		// Union by rank
		if (rank_x < rank_y) {
			this->operator[](root_x) = root_y;
			return root_y;
		} else if (rank_x > rank_y) {
			this->operator[](root_y) = root_x;
			return root_x;
		}
		this->operator[](root_y) = root_x;
		rank[root_x] = rank.contains(root_x) ? rank[root_x] + 1 : 1;
		return root_x;
	}
};


/**
 * @brief Union find data structure with set retrieval for tau formulas
 * @tparam comp Comparison function used to decide root after merge,
 * taking the smaller as new root
 * @tparam node Type of tree node
 */
template <typename data_t, class less_t = std::less<data_t>>
struct union_find_by_less : public union_find<data_t, less_t> {

	// Union the two sets containing x and y (union by rank)
	data_t merge(data_t x, data_t y) {
		static const less_t comp;
		auto root_x = this->root(x), root_y = this->root(y);
		if (root_x == root_y) return root_x;
		// Union by rank
		if (comp(root_x, root_y)) {
			this->operator[](root_y) = root_x;
			return root_x;
		} else if (comp(root_y, root_x)) {
			this->operator[](root_x) = root_y;
			return root_y;
		}
		this->operator[](root_x) = root_y;
		return root_y;
	}
};


template<typename data_t, typename kind_t, class less_t = std::less<data_t>>
struct scoped_union_find {
	using scope_t = size_t;
	using element_t = std::pair<scope_t, data_t>;

	struct scoped_less {
		bool operator()(const element_t& a, const element_t& b) const {
				static const less_t comp;
				if (a.first < b.first) return true;
				if (a.first > b.first) return false;
				// same scope
				return (comp(a.second, b.second));
		}
	};

	using kinds_t = std::map<element_t, kind_t, scoped_less>;
	using union_find_by_less_t = union_find_by_less<element_t, scoped_less>;

	union_find_by_less_t uf;
	scope_t current = 0;
	std::deque<size_t> scopes_ { current };
	kinds_t kinds_;
	kind_t unknown;
	kind_t default_kind = unknown;

	scoped_union_find(const kind_t& unknown): unknown(unknown) {}

	void open(const std::map<data_t, kind_t, less_t>& kinds) {
		current++;
		scopes_.push_back(current);
		for (const auto& [data, kind] : kinds) {
			uf.insert({current, data});
			kinds_.emplace(element_t{current, data}, kind);
		}
	}

	void close() {
		if (scopes_.size() == 1) return;
		/*std::erase_if(kinds_, [&](const auto& e)
			{ return e.first.first == current; });*/
		scopes_.pop_back();
	}

	void insert(const data_t& data) {
		static size_t global = 0;
		for(auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
			if (uf.contains(element_t{*it, data})) return;
		uf.insert({global, data});
		kinds_.emplace(element_t{global, data}, unknown);
	}

	kind_t type_of(const data_t& datum) {
		for(auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
			if (uf.contains({*it, datum}))
				return kinds_.find(uf.root(element_t{*it, datum}))->second;
		return unknown;
	}

	std::vector<kind_t> types_of(const std::vector<data_t>& data) {
		std::vector<kind_t> result;
		for(auto datum: data) result.push_back(type_of(datum));
		return result;
	}

	scope_t scope_of(const data_t& data) {
		for(auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
			if (uf.contains({*it, data}))
				return (*it);
		return 0; // global scope
	}

	bool same_kind(const data_t& data1, const data_t& data2) {
		return type_of(data1) == type_of(data2);
	}

	bool assign(const data_t& data, const kind_t& kind) {
		for(auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
			if (uf.contains({*it, data})) {
				if (auto cur = kinds_.find(uf.root(element_t{*it, data}))->second;
						cur != unknown && cur != kind)
					return false;
				kinds_.insert_or_assign(uf.root(element_t{*it, data}), kind);
				return true;
			}
		return false;
	}

	bool assign(const data_t& data) {
		return assign(data, default_kind);
	}

	std::map<data_t, kind_t, less_t> kinds() {
		std::map<data_t, kind_t, less_t> result ;
		// Note that we will overwrite the kind of a data if it appears in
		// multiple scopes, keeping only the inner one.
		for(auto it = uf.begin(); it != uf.end(); ++it)
			if (kinds_.find(it->second) != kinds_.end())
				result[it->second.second] = type_of(it->second.second);
		return result;
	}

	std::map<data_t, kind_t, less_t> current_kinds() {
		std::map<data_t, kind_t, less_t> result ;
		auto scope = scopes_.back();
		for(auto [element, _] : uf)
			if (element.first == scope) result[element.second] = type_of(element.second);
		return result;
	}
};

} // idni::tau_lang

#endif // __IDNI__TAU__UNION_FIND_H__
