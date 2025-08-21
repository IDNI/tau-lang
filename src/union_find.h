// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef UNION_FIND_H
#define UNION_FIND_H

#include "nso_rr.h"

namespace idni::tau_lang {
/**
 * @brief Union find data structure with set retrieval for tau formulas
 * @tparam comp Comparison function used to decide root after merge,
 * taking the smaller as new root
 * @tparam node Type of tree node
 */
template <typename data_t, typename container_t, typename less_t>
struct union_find {
private:
	container_t parent;
	container_t next;
	// The comparator used in order to decide the new root of a merged set
	// The smaller root is chosen
	const less_t& _less;

public:
	explicit union_find(const less_t& l) : _less(l) {}

	void insert(data_t x) {
		if (parent.contains(x)) return;
		parent.emplace(x, x);
		next.emplace(x,x);
	}

	// Find the root of the set containing x while inserting x if not present
	data_t find(data_t x) {
		if (auto it = parent.find(x); it == parent.end())
			return insert(x), x;
		else if (it->second != x) {
                  // Path compression
                  it->second = find(it->second);
                  return it->second;
       		} else return it->second;
	}

	bool contains (data_t x) {
		return parent.contains(x);
	}

	// Union the two sets containing x and y
	void merge(data_t x, data_t y) {
		data_t root_x = find(x);
		data_t root_y = find(y);
		// sets are already equal
		if (root_x == root_y) return;

		if (_less(root_x, root_y))
			// root_x becomes new root
			parent[root_y] = root_x;
		else
			// root_y becomes the new root
			parent[root_x] = root_y;

		// Update linking
		auto next_y = next.find(root_y);
		auto next_x = next.find(root_x);
		tref tmp = next_x->second;
		next_x->second = next_y->second;
		next_y->second = tmp;
	}

	// Check if two elements are in the same set
	bool connected(data_t x, data_t y) {
		return find(x) == find(y);
	}

	// Get all elements in the same set as x
	std::vector<data_t> get_set(data_t x) {
		std::vector<data_t> component {x};
		data_t current = next.find(x)->second;
		while (current != x) {
			component.push_back(current);
			current = next.find(current)->second;
		}
		return component;
	}
};

} // idni::tau_lang

#endif //UNION_FIND_H
