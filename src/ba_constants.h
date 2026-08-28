// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ba_constants.h
 * @brief Static pool of Boolean-algebra constants indexed by type.
 *
 * `ba_constants<node>` maintains a global pool of constants and their
 * corresponding tree nodes, avoiding duplicate allocations for the same
 * constant value and type.
 */

#ifndef __IDNI__TAU__BA_CONSTANTS_H__
#define __IDNI__TAU__BA_CONSTANTS_H__

#include "tau_tree.h"

namespace idni::tau_lang {

/**
 * @brief Strip surrounding whitespace and one pair of enclosing braces
 * (and, optionally, one pair of matching quotes) from a BA constant source.
 *
 * BA1-29: shared preamble of parse_qlt, parse_qint and parse_nlang.
 * parse_hsb intentionally takes pre-stripped input (hsb literals may
 * themselves be brace-delimited sets), see hsb.tmpl.h.
 */
inline std::string strip_ba_constant_source(std::string s,
	bool strip_quotes = false)
{
	auto trim = [](std::string& x) {
		x.erase(0, x.find_first_not_of(" \t\n\r"));
		auto last = x.find_last_not_of(" \t\n\r");
		if (last != std::string::npos) x = x.substr(0, last + 1);
	};
	trim(s);
	if (!s.empty() && s.front() == '{' && s.back() == '}')
		s = s.substr(1, s.size() - 2);
	trim(s);
	if (strip_quotes) {
		if (s.size() >= 2 && s.front() == '"' && s.back() == '"')
			s = s.substr(1, s.size() - 2);
		else if (s.size() >= 2 && s.front() == '\'' && s.back() == '\'')
			s = s.substr(1, s.size() - 2);
	}
	return s;
}

/**
 * @brief Static pool of BA constants, providing deduplication and type-indexed lookup.
 *
 * All methods are static; the pool is shared across the lifetime of the program.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct ba_constants {
	using constant = typename node::constant;
	using constant_with_type = std::pair<constant, tref>;

	/**
	 * @brief Fetch (or create) the tree node for @p constant of the given @p type_id.
	 * @param constant Constant value to look up or insert.
	 * @param type_id BA type identifier.
	 * @return Tree node wrapping the constant.
	 */
	static tref get(const constant& constant, size_t type_id);

	/**
	 * @brief Fetch (or create) the tree node for @p constant whose type is @p type_tree.
	 * @param constant Constant value.
	 * @param type_tree Tree node identifying the BA type.
	 * @return Tree node wrapping the constant.
	 */
	static tref get(const constant& constant, tref type_tree);

	// insert the constant given by the string of a type id to the pool
	//static tref get(const std::string& constant, size_t type_id);

	/**
	 * @brief Retrieve the raw constant value by its pool index @p constant_id.
	 * @param constant_id One-based id into the constant pool (0 is invalid).
	 * @return The constant variant value at that index.
	 */
	static constant get(size_t constant_id);

	/**
	 * @brief Parse a constant from its source representation.
	 *
	 * Calls the BA parser for @p type_tree to convert @p constant_source into a
	 * typed constant, or returns `std::nullopt` if parsing fails.
	 * @param constant_source Source text to parse.
	 * @param type_tree Tree node identifying the BA type.
	 * @param options Ignored by every current specialization (BA2-21: kept for signature stability only).
	 * @return Parsed constant-with-type pair, or `std::nullopt` on failure.
	 */
	static std::optional<typename node::constant_with_type> get(
		const std::string& constant_source,
		tref type_tree,
		const std::string options = ""
	);

	// TODO (HIGH) add a similar method that takes a type (pair of ba_id and
	// subtype) to parse the constant instead of type name

	/**
	 * @brief Dump the entire constant pool to @p os.
	 * @param os Output stream to write to.
	 * @return Reference to @p os.
	 */
	static std::ostream& dump(std::ostream& os);

	/** @brief Dump the constant pool to a new `std::string`. */
	static std::string dump_to_str();

	/** @brief Release all entries from the constant pool. */
	static void cleanup();

private:
	using constant_pool = std::vector<std::pair<constant, size_t>>;
	static constant_pool& C();  // pool of constants
	static htrefs& T();         // pool of constant tree nodes with type info (htref to survive gc)
	// BA2-5: set by cleanup(); a get() afterwards is a programming error --
	// fail loudly instead of silently re-pooling into a freshly reset pool.
	inline static bool poisoned = false;
	// O(1) lookup index: (constant, type_id) -> position in C(). C()
	// stays the id-ordered store (get(constant_id) reads it); this map
	// only accelerates get(constant, type_id), which used to scan the
	// pool linearly with full BA equality per entry on the step() hot
	// path. Same never-destroyed accessor pattern as C()/T() so all
	// three pools share one lifetime.
	struct pooled_key_hash {
		size_t operator()(const std::pair<constant, size_t>& p) const {
			size_t seed = std::hash<constant>{}(p.first);
			hash_combine(seed, p.second);
			return seed;
		}
	};
	using pool_index = std::unordered_map<std::pair<constant, size_t>,
		size_t, pooled_key_hash>;
	static pool_index& index_();
};

} // namespace idni::tau_lang

#include "ba_constants.tmpl.h"

#endif // __IDNI__TAU__BA_CONSTANTS_H__
