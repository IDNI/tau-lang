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
template <typename data_t, class less_t = std::less<data_t>, class merge_less_t = std::less<data_t>>
struct union_find_by_less : public union_find<data_t, less_t> {
private:
	static const merge_less_t comp;

public:
	// Union the two sets containing x and y (union by rank)
	data_t merge(data_t x, data_t y) {
		auto root_x = this->root(x), root_y = this->root(y);
		if (root_x == root_y) return root_x;
		// Union by rank
		if (comp(root_x, root_y)) {
			this->operator[](root_x) = root_y;
			return root_y;
		} else if (comp(root_y, root_x)) {
			this->operator[](root_y) = root_x;
			return root_x;
		}
		this->operator[](root_y) = root_x;
		return root_x;
	}
};

} // idni::tau_lang

#endif // __IDNI__TAU__UNION_FIND_H__
