// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef TAU_UNION_FIND_WITH_SETS_H
#define TAU_UNION_FIND_WITH_SETS_H

#include "tau_tree.h"

namespace idni::tau_lang {
/**
 * @brief Union find data structure with set retrieval for tau formulas
 * @tparam comp Comparison function used to decide root after merge,
 * taking the smaller as new root
 * @tparam node Type of tree node
 */
template <typename comp, NodeType node>
struct union_find_with_sets {
	using tau = tree<node>;
private:
	subtree_unordered_map<node, tref> parent;
	subtree_unordered_map<node, tref> next;
	// The comparator used in order to decide the new root of a merged set
	// The smaller root is chosen
	const comp& _comp;

public:
	explicit union_find_with_sets(const comp& c) : _comp(c) {}

	void insert(tref x) {
		if (parent.contains(x)) return;
		parent.emplace(x, x);
		next.emplace(x,x);
	}

	// Find the root of the set containing x while inserting x if not present
	tref find(tref x) {
		if (auto it = parent.find(x); it == parent.end())
			return insert(x), x;
		else if (tau::get(it->second) != tau::get(x)) {
			// Path compression
			it->second = find(it->second);
			return it->second;
		} else return it->second;
	}

	bool contains (tref x) {
		return parent.contains(x);
	}

	// Union the two sets containing x and y
	void merge(tref x, tref y) {
		tref root_x = find(x);
		tref root_y = find(y);
		// sets are already equal
		if (tau::get(root_x) == tau::get(root_y)) return;

		if (_comp(root_x, root_y)) {
			// root_x becomes new root
			parent[root_y] = root_x;
		}
		else if (_comp(root_y, root_x)) {
			// root_y becomes the new root
			parent[root_x] = root_y;
		}
		// We do not merge equal comparing elements
		else return;

		// Update linking
		auto next_y = next.find(root_y);
		auto next_x = next.find(root_x);
		tref tmp = next_x->second;
		next_x->second = next_y->second;
		next_y->second = tmp;
	}

	// Check if two elements are in the same set
	bool connected(tref x, tref y) {
		return tau::get(find(x)) == tau::get(find(y));
	}

	// Get all elements in the same set as x
	trefs get_set(tref x) {
		trefs component {x};
		auto it = next.find(x);
		if (it == next.end()) return component;
		tref current = it->second;
		while (tau::get(current) != tau::get(x)) {
			component.push_back(current);
			current = next.find(current)->second;
		}
		return component;
	}

	void clear () {
		parent.clear();
		next.clear();
	}
};

}

#endif //TAU_UNION_FIND_WITH_SETS_H