// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file union_find_with_sets.h
 * @brief Union-find with intrusive linked-list sets for Tau formula terms.
 *
 * `union_find_with_sets<comp, node>` tracks disjoint sets of `tref` tree nodes,
 * supports path-compressed find, rank-ordered merge, and O(n) iteration over
 * each set via a `next`-pointer ring — all keyed by structural identity.
 */

#ifndef TAU_UNION_FIND_WITH_SETS_H
#define TAU_UNION_FIND_WITH_SETS_H

#include "tau_tree.h"

namespace idni::tau_lang {
/**
 * @brief Union-find with per-set iteration for Tau formula tree nodes.
 *
 * Maintains disjoint sets of `tref` values with path-compressed `find` and
 * a circular `next`-pointer ring per set for O(n) set traversal.
 *
 * The `comp` functor determines the new root on merge: the lesser element
 * (per `comp`) becomes the representative.
 *
 * @tparam comp Strict-weak-ordering comparator used to choose the root on merge.
 * @tparam node Tree node type satisfying `NodeType`.
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
	/** @brief Construct using comparator @p c for root selection on merge. */
	explicit union_find_with_sets(const comp& c) : _comp(c) {}

	/** @brief Copy-construct, sharing the same comparator reference. */
	explicit union_find_with_sets(const union_find_with_sets& other) :
		parent(other.parent),
		next(other.next),
		_comp(other._comp) {}

	/** @brief Move-construct, transferring maps while keeping the comparator reference. */
	explicit union_find_with_sets(union_find_with_sets&& other) noexcept :
		parent(std::move(other.parent)),
		next(std::move(other.next)),
		_comp(other._comp) {}

	/** @brief Move-assign maps (comparator reference is unchanged). */
	union_find_with_sets& operator=(union_find_with_sets&& other) {
		parent = std::move(other.parent);
		next = std::move(other.next);
		return *this;
	}

	/**
	 * @brief Insert @p x as a singleton set; no-op if @p x is already present.
	 * @param x Tree node to insert.
	 */
	void insert(tref x) {
		if (parent.contains(x)) return;
		parent.emplace(x, x);
		next.emplace(x,x);
	}

	/**
	 * @brief Return the root of the set containing @p x, inserting @p x first if absent.
	 *
	 * Applies path compression on traversal.
	 * @param x Tree node to look up.
	 * @return Root representative of @p x's set.
	 */
	tref find(tref x) {
		if (auto it = parent.find(x); it == parent.end())
			return insert(x), x;
		else if (tau::get(it->second) != tau::get(x)) {
			// Path compression
			it->second = find(it->second);
			return it->second;
		} else return it->second;
	}

	/**
	 * @brief Return `true` if @p x is already present in the structure.
	 * @param x Tree node to check.
	 */
	bool contains (tref x) {
		return parent.contains(x);
	}

	/**
	 * @brief Union the sets containing @p x and @p y.
	 *
	 * The lesser root per `comp` becomes the representative. Elements that
	 * compare equal are not merged (their sets remain separate).
	 * @param x First element.
	 * @param y Second element.
	 * @return Root of the merged set, or the existing root if already equal.
	 */
	tref merge(tref x, tref y) {
		tref root_x = find(x);
		tref root_y = find(y);
		// sets are already equal
		if (tau::get(root_x) == tau::get(root_y)) return root_x;

		tref new_root = nullptr;
		if (_comp(root_x, root_y)) {
			// root_x becomes new root
			parent[root_y] = root_x;
			new_root = root_x;
		}
		else if (_comp(root_y, root_x)) {
			// root_y becomes the new root
			parent[root_x] = root_y;
			new_root = root_y;
		}
		// We do not merge equal comparing elements
		else return root_x;

		// Update linking
		auto next_y = next.find(root_y);
		auto next_x = next.find(root_x);
		tref tmp = next_x->second;
		next_x->second = next_y->second;
		next_y->second = tmp;
		return new_root;
	}

	/**
	 * @brief Return `true` if @p x and @p y belong to the same set.
	 * @param x First element.
	 * @param y Second element.
	 */
	bool connected(tref x, tref y) {
		return tau::get(find(x)) == tau::get(find(y));
	}

	/**
	 * @brief Return all elements in the same set as @p x.
	 *
	 * Traverses the circular `next`-pointer ring starting from @p x.
	 * @param x Starting element.
	 * @return Vector of all elements co-located with @p x.
	 */
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

	/**
	 * @brief Merge all sets from @p other into this structure.
	 * @param other Source union-find whose element-pair relationships to absorb.
	 */
	void merge(union_find_with_sets& other) {
		for (auto [v, p] : other.parent)
			merge(v, p);
	}

	/** @brief Remove all elements, resetting to the empty state. */
	void clear () {
		parent.clear();
		next.clear();
	}
};

}

#endif //TAU_UNION_FIND_WITH_SETS_H