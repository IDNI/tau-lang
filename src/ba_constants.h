// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ba_constants.h
 * @brief Static pool of Boolean-algebra constants indexed by type.
 *
 * `ba_constants<node>` maintains a global pool of constants and their
 * corresponding tree nodes, avoiding duplicate allocations for the same
 * constant value and type. A sweep of the tree store frees every constant
 * that no surviving tree node names, so the pool holds only what is in use.
 */

#ifndef __IDNI__TAU__BA_CONSTANTS_H__
#define __IDNI__TAU__BA_CONSTANTS_H__

#include <mutex>
#include <optional>
#include <stdexcept>
#include <unordered_set>
#include <vector>

#include "tau_tree.h"
#include "tau_diagnostics.h"

namespace idni::tau_lang {

/**
 * @brief Strip surrounding whitespace and one pair of enclosing braces
 * (and, optionally, one pair of matching quotes) from a BA constant source.
 *
 * Shared preamble of parse_qlt, parse_qint and parse_nlang. parse_hsb
 * does not use it: hsb literals may themselves be brace-delimited sets,
 * see hsb.tmpl.h.
 * @param s Constant source text.
 * @param strip_quotes Also strip one pair of matching `"` or `'` quotes
 * left after the braces are removed.
 * @return The stripped text; @p s trimmed when it has no enclosing braces.
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
 * All methods are static; the pool is shared by every interpreter of the
 * process. The pool holds no handle to the tree nodes it builds: a constant
 * lives while a tree node that names its id survives the sweeps, and the
 * owner of a sweep frees the others with collect(). A freed id is reused.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct ba_constants {
	using constant = typename node::constant;
	using constant_with_type = std::pair<constant, tref>;

	/**
	 * @brief Fetch (or create) the tree node for @p constant of the given @p type_id.
	 * The pair (@p constant, @p type_id) is interned: a repeat returns the
	 * same node in O(1) average time. Must not be called after cleanup().
	 * @param constant Constant value to look up or insert.
	 * @param type_id BA type identifier.
	 * @return Tree node wrapping the constant; never null.
	 */
	static tref get(const constant& constant, size_t type_id);

	/**
	 * @brief Fetch (or create) the tree node for @p constant whose type is @p type_tree.
	 * @param constant Constant value.
	 * @param type_tree Tree node identifying the BA type.
	 * @return Tree node wrapping the constant; never null.
	 */
	static tref get(const constant& constant, tref type_tree);

	// insert the constant given by the string of a type id to the pool
	//static tref get(const std::string& constant, size_t type_id);

	/**
	 * @brief Retrieve the raw constant value by its pool index @p constant_id.
	 *
	 * An out-of-range or zero id is an out-of-range report, never a throw.
	 * @param constant_id One-based id into the constant pool (0 is invalid).
	 * @return The constant variant value at that index, or an out_of_range
	 * error.
	 */
	static result<constant> get(size_t constant_id);

	/**
	 * @brief Parse a constant from its source representation.
	 *
	 * Calls the BA parser for @p type_tree to convert @p constant_source into a
	 * typed constant. The returned result carries a report: on refusal it names
	 * why, and an unowned @p type_tree comes back malformed (no value, no
	 * error) rather than as a parse failure.
	 * @param constant_source Source text to parse.
	 * @param type_tree Tree node identifying the BA type.
	 * @param options Ignored; kept for signature stability only.
	 * @return Parsed constant-with-type pair, or a report explaining the refusal.
	 */
	static result<typename node::constant_with_type> get(
		const std::string& constant_source,
		tref type_tree,
		const std::string options = ""
	);

	/**
	 * @brief Parse the value of an input stream of type @p type_tree.
	 *
	 * Takes the bare value (`3`) and the constant as a spec writes it
	 * (`{ 3 }` or `{ 3 }:qlt`): the braces are removed, and a type after
	 * them must name the stream's type or its family (`:bv` for `bv[8]`).
	 * @param value Source text of the value.
	 * @param type_tree Tree node identifying the stream's BA type.
	 * @return Parsed constant-with-type pair, or a report explaining the
	 * refusal; a type that names another type is a type error.
	 */
	static result<typename node::constant_with_type> get_input(
		const std::string& value, tref type_tree);

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

	/**
	 * @brief Release all entries from the constant pool.
	 *
	 * Clears the constant store and the lookup index together. A get() of
	 * a constant afterwards is a programming error, caught by an assert in
	 * builds with DEBUG.
	 */
	static void cleanup();

	/** @brief Number of constants in the pool. */
	static size_t size();

	/**
	 * @brief Free every constant that no node of @p survivors names.
	 *
	 * The owner of a sweep calls this after a sweep that completed, with
	 * the full set of nodes that survived it; tree::clear_caches() also
	 * runs the gc callbacks, with sets that are not survivor sets, which is
	 * why this is not one of them. A constant handed out since the
	 * previous call survives this one, so a get() racing with a sweep never
	 * loses the id it returns.
	 * @param survivors Every node of the store after the sweep.
	 */
	static void collect(const std::unordered_set<tref>& survivors);

private:
	/// One pool entry. `built` caches the tree node get() built for the
	/// entry; a gc callback clears it when a sweep frees the node.
	struct slot {
		std::optional<std::pair<constant, size_t>> value;
		tref built = nullptr;
		size_t used_at = 0; ///< collect() count when last handed out
	};
	using constant_pool = std::vector<slot>;
	static constant_pool& C();  // pool of constants, by id - 1
	static std::vector<size_t>& free_ids();
	static std::mutex& mutex();
	/// Registers, once, the gc callback that clears `built` of the slots
	/// whose node a sweep freed.
	static void forget_built_nodes();
	inline static size_t collects = 0;
	// O(1) lookup index: (constant, type_id) -> position in C(). C()
	// stays the id-ordered store (get(constant_id) reads it); this map
	// only accelerates get(constant, type_id) on the step() hot path.
	// Same never-destroyed accessor pattern as C() so the pools share
	// one lifetime.
	struct pooled_key_hash {
		size_t operator()(const std::pair<constant, size_t>& p) const {
			std::uint64_t seed = std::hash<constant>{}(p.first);
			hash_combine(seed, p.second);
			return static_cast<size_t>(seed);
		}
	};
	using pool_index = std::unordered_map<std::pair<constant, size_t>,
		size_t, pooled_key_hash>;
	static pool_index& index_();
	// Set by cleanup(); a get() afterwards is a programming error, caught
	// by an assert in builds with DEBUG instead of silently re-pooling.
	inline static bool poisoned = false;
};

} // namespace idni::tau_lang

#include "ba_constants.tmpl.h"

#endif // __IDNI__TAU__BA_CONSTANTS_H__
