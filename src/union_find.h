// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__UNION_FIND_H__
#define __IDNI__TAU__UNION_FIND_H__

namespace idni::tau_lang {

/**
 * @brief Union find data structure with set retrieval for tau formulas
 * @tparam comp Comparison function used to decide root after merge,
 * taking the smaller as new root
 * @tparam node Type of tree node
 */
template <typename data_t, class less_t = std::less<data_t>>
struct union_find {
private:
	std::map<data_t, data_t> parent;
	std::map<data_t, size_t> rank; // rank for union by rank

public:

	// Insert element x if not present
	data_t insert(data_t x) {
		if (parent.contains(x)) return x;
		parent.emplace(x, x);
		rank[x] = 0;
		return x;
	}

	// Returns a map from root to set of elements in each set, the sets
	// are ordered using the less_t comparator
	std::map<data_t, std::set<data_t, less_t>> get_sets() {
		std::map<data_t, std::set<data_t, less_t>> sets;
		for (const auto& [elem, _] : parent) {
			data_t r = root(elem);
			sets[r].insert(elem);
		}
		return sets;
	}

	// Find the root of the set containing x while inserting x if not present
	data_t root(data_t x) {
		if (auto it = parent.find(x); it == parent.end())
			return insert(x), x;
		else if (it->second != x) {
			// Path compression
			it->second = root(it->second);
			return it->second;
       	} else return it->second;
	}

	// Union the two sets containing x and y (union by rank)
	data_t merge(data_t x, data_t y) {
		auto root_x = root(x), root_y = root(y);
		if (root_x == root_y) return root_x;
		// Union by rank
		if (rank[root_x] < rank[root_y]) {
			parent[root_x] = root_y;
			return root_y;
		} else if (rank[root_x] > rank[root_y]) {
			parent[root_y] = root_x;
			return root_x;
		}
		parent[root_y] = root_x;
		rank[root_x]++;
		return root_x;
	}

	// Check if two elements are in the same set
	bool connected(data_t x, data_t y) {
		return root(x) == root(y);
	}

	// We allow access to the underlying parent container for iteration
	// over the elements in the union-find structure
	bool contains(const data_t& x) {
		return parent.contains(x);
	}

	typename std::map<data_t, data_t>::iterator equal_range(const data_t& x) {
		return parent.equal_range(x);
	}

	typename std::map<data_t, data_t>::iterator lower_bound(const data_t& x) {
		return parent.lower_bound(x);
	}

	typename std::map<data_t, data_t>::iterator find(const data_t& x) {
		return parent.find(x);
	}

	typename std::map<data_t, data_t>::iterator end() {
		return parent.end();
	}

	typename std::map<data_t, data_t>::iterator begin() {
		return parent.begin();
	}

	typename std::map<data_t, data_t>::iterator rend() {
		return parent.rend();
	}

	typename std::map<data_t, data_t>::iterator rbegin() {
		return parent.rbegin();
	}

	size_t size() const {
		return parent.size();
	}
};

} // idni::tau_lang

#endif // __IDNI__TAU__UNION_FIND_H__
