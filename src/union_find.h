// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef UNION_FIND_H
#define UNION_FIND_H

namespace idni::tau_lang {
/**
 * @brief Union find data structure with set retrieval for tau formulas
 * @tparam comp Comparison function used to decide root after merge,
 * taking the smaller as new root
 * @tparam node Type of tree node
 */
template <typename data_t, typename less_t>
struct union_find {
private:
	std::map<data_t, data_t> parent;

	// The comparator used in order to decide the new root of a merged set
	// The smaller root is chosen
	const less_t& _less;

public:
	explicit union_find(const less_t& l = std::less<data_t>()) : _less(l) {}

	data_t insert(data_t x) {
		if (parent.contains(x)) return x;
		return parent.emplace(x, x), x;
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

	// Union the two sets containing x and y
	void merge(data_t x, data_t y) {
		auto root_x = root(x), root_y = root(y);
		// sets are already equal
		if (root_x == root_y) return;
		if (_less(root_x, root_y))
			// root_x becomes new root
			parent[root_y] = root_x;
		else
			// root_y becomes the new root
			parent[root_x] = root_y;
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

#endif //UNION_FIND_H
