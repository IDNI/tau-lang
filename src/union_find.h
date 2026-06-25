// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file union_find.h
 * @brief Union-find (disjoint-set) data structures for Tau formula terms.
 *
 * Provides `union_find`, `union_find_by_rank`, `union_find_by_less`,
 * `scoped_less`, and `scoped_union_find` — supporting equality propagation
 * and scope-aware disjoint-set operations used by the simplification passes.
 */

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
	/**
	 * @brief Insert @p x if not already present and return it.
	 *
	 * Unlike `std::map::insert`, always returns the element (existing or new).
	 * @param x Element to insert.
	 * @return @p x.
	 */
	data_t insert(data_t x) {
		if (this->find(x) != this->end()) return x;
		this->emplace(x, x);
		return x;
	}

	/**
	 * @brief Find the root of the set containing @p x, inserting @p x if absent.
	 *
	 * Applies path compression on traversal.
	 * @param x Element to find root for.
	 * @return Root representative of @p x's set.
	 */
	data_t root(data_t x) {
		if (auto it = this->find(x); it == this->end())
			return insert(x), x;
		else if (it->second != x) {
			// Path compression
			it->second = root(it->second);
			return it->second;
       	} else return it->second;
	}

	/**
	 * @brief Return `true` if @p x and @p y belong to the same set.
	 * @param x First element.
	 * @param y Second element.
	 */
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
	/**
	 * @brief Union the sets containing @p x and @p y using union by rank.
	 * @param x First element.
	 * @param y Second element.
	 * @return Root of the merged set.
	 */
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

	/**
	 * @brief Union the sets containing @p x and @p y, choosing the lesser root.
	 * @param x First element.
	 * @param y Second element.
	 * @return Root of the merged set (the lesser of the two roots per `less_t`).
	 */
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

/**
 * @brief Comparator for scope-tagged elements: order by scope first, then by `less_t`.
 * @tparam data_t Element type.
 * @tparam less_t Secondary comparator.
 */
template<typename data_t, class less_t = std::less<data_t>>
struct scoped_less {
	using scope = size_t;
	using element = std::pair<scope, data_t>;

	/** @brief Compare @p a and @p b lexicographically (scope first, then data). */
	bool operator()(const element& a, const element& b) const {
			static const less_t comp;
			if (a.first < b.first) return true;
			if (a.first > b.first) return false;
			// same scope
			return (comp(a.second, b.second));
	}
};

/**
 * @brief Scope-aware union-find supporting nested scopes.
 *
 * Elements inserted via `insert` are visible in all enclosing scopes.
 * Elements inserted via `push` are visible only in the current scope.
 * @tparam data_t Element type.
 * @tparam less_t Comparator for choosing the root on merge.
 */
template<typename data_t, class less_t = std::less<data_t>>
struct scoped_union_find {
	using scope = size_t;
	using element = std::pair<scope, data_t>;

	/** @brief Error type returned when closing a scope fails (unbalanced scopes). */
	struct scope_error {
		data_t element;
	};

	union_find_by_less<std::pair<size_t, data_t>, scoped_less<data_t, less_t>> uf;
	scope current = 0;
	std::deque<size_t> scopes { current };
	scope global = 0;

	/** @brief Construct with a single global scope active. */
	scoped_union_find() {}

	/** @brief Open a new nested scope. */
	void open() {
		current++;
		scopes.push_back(current);
	}

	/**
	 * @brief Close the current scope.
	 * @return `scope_error` if the global scope would be closed, else `std::nullopt`.
	 */
	std::optional<scope_error> close() {
		if (scopes.size() == 1) {
			// TODO (HIGH) Check proper closing scopes
			// DBG(LOG_ERROR << "scoped_union_find/close: cannot close global scope (unbalenced scopes)\n";)
			return std::nullopt; //scope_error{};
		}
		scopes.pop_back();
		return std::nullopt;
	}

	/** @brief Return the root of the scoped element @p e. */
	element root(const element& e) {
		return uf.root(e);
	}

	/**
	 * @brief Look up @p data in enclosing scopes; insert into global scope if absent.
	 * @param data Element to find or insert.
	 * @return Scoped element for @p data.
	 */
	element insert(const data_t& data) {
		for(auto it = scopes.rbegin(); it != scopes.rend(); ++it)
			if (auto eit = uf.find(element{*it, data}); eit != uf.end())
				return eit->first;
		return uf.insert(element{global, data});
	}

	/**
	 * @brief Insert @p data into the current (innermost) scope.
	 * @param data Element to push.
	 * @return Scoped element for @p data.
	 */
	element push(const data_t& data) {
		return uf.insert(element{current, data});
	}

	/**
	 * @brief Merge the sets containing @p d1 and @p d2.
	 * @return Root of the merged set.
	 */
	element merge(const data_t& d1, const data_t& d2) {
		auto e1 = insert(d1);
		auto e2 = insert(d2);
		return uf.merge(e1, e2);
	}

	/**
	 * @brief Return the scope level at which @p data was registered.
	 * @param data Element to query.
	 * @return Scope index (0 = global if not found in any scope).
	 */
	scope scope_of(const data_t& data) {
		for(auto it = scopes.rbegin(); it != scopes.rend(); ++it)
			if (auto eit = uf.find(element{*it, data}); eit != uf.end())
				return eit->first.first;
		return uf.insert(element{global, data}).first; // global scope
	}

	/**
	 * @brief Return `true` if @p data is present in any active scope.
	 * @param data Element to check.
	 */
	bool contains(const data_t& data) {
		for(auto it = scopes.rbegin(); it != scopes.rend(); ++it)
			if (uf.contains(element{*it, data}))
				return true;
		return false;
	}
};

} // idni::tau_lang

#endif // __IDNI__TAU__UNION_FIND_H__
