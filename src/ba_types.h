// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ba_types.h
 * @brief Boolean-algebra (BA) type registry, type-tree constructors, and query helpers.
 *
 * Defines type-tree builders (`tau_type`, `untyped_type`, etc.),
 * their corresponding `size_t`-id overloads, `is_*` predicates, and the
 * `ba_types<node>` registry struct that maps tree refs ↔ integer type ids.
 * Also provides the `unify` / `get_ba_type_id` / `find_ba_type` free-function API.
 */

#ifndef __IDNI__TAU__BA_TYPES_H__
#define __IDNI__TAU__BA_TYPES_H__

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// Type definitions of untyped, tau, bool and nat

/**
 * @brief Creates the type tree associated with the type "tau"
 * @tparam Tree node type
 * @return Tree reference to type tree
 */
template <NodeType node>
tref tau_type();

/** @brief Return the integer type id for the `tau` type under @p node. */
template <NodeType node>
inline size_t tau_type_id();

/**
 * @brief Checks if t represents the tau type
 * @tparam node Tree node type
 * @param t Type tree object
 * @return If the type tree object represents tau
 */
template <NodeType node>
bool is_tau_type(tref t);

/** @brief Return `true` if type id @p t represents the tau type. */
template <NodeType node>
bool is_tau_type(size_t t);

/**
 * @brief Creates the type tree associated with the type "nat"
 * @tparam Tree node type
 * @return Tree reference to type tree
 */
template <NodeType node>
tref nat_type();

/** @brief Return the integer type id for the `nat` type under @p node. */
template <NodeType node>
inline size_t nat_type_id();

/**
 * @brief Checks if t represents the nat type
 * @tparam node Tree node type
 * @param t Type tree object
 * @return If the type tree object represents nat
 */
template <NodeType node>
bool is_nat_type(tref t);

/**
 * @brief Creates the type tree associated with the type "untyped"
 * @tparam Tree node type
 * @return Tree reference to type tree
 */
template <NodeType node>
tref untyped_type();

/** @brief Return the integer type id for the `untyped` type under @p node. */
template <NodeType node>
inline size_t untyped_type_id();

/**
 * @brief Checks if t has untyped as type information
 * @tparam node Tree node type
 * @param t Type tree object
 * @return If the type tree object represents untyped
 */
template <NodeType node>
bool is_untyped(tref t);

/** @brief Return `true` if type id @p t represents the untyped kind. */
template <NodeType node>
bool is_untyped(size_t t);

// -----------------------------------------------------------------------------
// Deriving a BA's type surface from its descriptor's type_name
//
// Every unparameterized BA's type tree is `:name` and its type check is a
// comparison against that name, so a descriptor states the name once and these
// derive the rest. A BA whose types carry a subtype, and which therefore matches
// a family rather than a name, builds its own instead.
namespace ba_types_detail {

/** @brief Build the syntactic type tree `:name`. */
template <NodeType node>
tref make_syntactic_type_tree(const char* name);

// Templated on the owning BA so the memo is per algebra, as it was when each
// BA carried its own is_<ba>_type; one cache shared across names would answer
// a later name from an earlier name's entry.
/** @brief Return `true` if type tree @p t is named @p name. */
template <typename BA, NodeType node>
bool type_tree_name_is(tref t, const char* name);

/** @brief Return `true` if type id @p ba_type_id is named @p name. */
template <typename BA, NodeType node>
bool type_tree_name_is(size_t ba_type_id, const char* name);

} // namespace ba_types_detail

/**
 * @brief Registry mapping BA type trees ↔ integer ids for a specific node type.
 *
 * Scoped per `node` so each BA pack has its own independent id space.
 * @tparam node Tree node type satisfying `NodeType`.
 */
template <NodeType node>
struct ba_types {
	/** @brief Return the integer id for type tree @p ba_type, inserting it if absent. */
	static size_t id(tref ba_type);

	/** @brief Return the number of registered type trees (valid ids are 0..count()-1). */
	static size_t count();

	/** @brief Return the type tree corresponding to @p ba_type_id. */
	static tref type_tree(size_t ba_type_id);

	/** @brief Return the string name for @p ba_type_id. */
	static std::string name(size_t ba_type_id);

	/** @brief Print the type name for @p ba_type to @p os. */
	static std::ostream& print(std::ostream& os, size_t ba_type);

	/** @brief Dump the full type registry to @p os. */
	static std::ostream& dump(std::ostream& os);

	/** @brief Return a string dump of the full type registry. */
	static std::string dump_to_str();

private:
	/// @brief Per-id type-tree vector (index = ba_type id).
	static std::vector<htref>& type_trees();

	/// @brief Reverse map: type tree → ba_type id.
	static subtree_map<node, size_t>& type_tree_to_idx();
};

// ---------------------------------------------------------------------------
// Functional API for ba_types
// ---------------------------------------------------------------------------

/** @brief Return the integer id for BA type tree @p ba_type. */
template <NodeType node>
size_t get_ba_type_id(tref ba_type);

/** @brief Return the type tree for @p ba_type_id. */
template <NodeType node>
tref get_ba_type_tree(size_t ba_type_id);

/** @brief Return the string name for @p ba_type_id. */
template <NodeType node>
std::string get_ba_type_name(size_t ba_type_id);

/** @brief Return `true` if @p t1 and @p t2 represent the same BA type. */
template <NodeType node>
bool is_same_ba_type(tref t1, tref t2);

/**
 * @brief Unify type trees @p t1 and @p t2.
 * @return The more informative type if compatible, or `nullptr` on conflict.
 */
template <NodeType node>
tref unify(tref t1, tref t2);

/**
 * @brief Unify type ids @p tid1 and @p tid2.
 * @return The more informative type id if compatible, or `std::nullopt` on conflict.
 */
template <NodeType node>
std::optional<size_t> unify(size_t tid1, size_t tid2);

/**
 * @brief Unify the types of all nodes in @p ns against @p default_type.
 * @return Compatible type, or `nullptr` on conflict.
 */
template <NodeType node>
tref unify(const trefs& ns, tref default_type);

/** @brief Unify nodes from @p ns1 and @p ns2 against @p default_type. */
template <NodeType node>
tref unify(const trefs& ns1, const trefs& ns2, tref default_type);

/**
 * @brief Unify type ids in @p nids against @p default_type.
 * @return Compatible type id, or `std::nullopt` on conflict.
 */
template <NodeType node>
std::optional<size_t> unify(const std::vector<size_t>& nids, size_t default_type);

/** @brief Unify type ids from @p nids1 and @p nids2 against @p default_type. */
template <NodeType node>
std::optional<size_t> unify(const std::vector<size_t>& nids1, const std::vector<size_t>& nids2, size_t default_type);

/** @brief Return `true` if @p term has a non-zero BA type assigned. */
template <NodeType node>
bool has_ba_type (tref term);

/** @brief Return `true` if @p t has `ba_type == 0` and no directly typed structural child. */
template <NodeType node>
bool is_untyped_tref(tref t);

/** @brief Return the effective BA type id: from `ba_type` if non-zero, else from the typed child. */
template <NodeType node>
size_t get_effective_ba_type(tref t);

/** @brief Whether @p ba_type_id is one of the type ids core reserves for itself, which no BA owns. */
template <NodeType node>
bool is_reserved_ba_type(size_t ba_type_id);

/** @brief Whether @p name names one of the type trees core reserves for itself. */
template <NodeType node>
bool is_reserved_ba_type_name(const std::string& name);

/** @brief Whether @p name is a reserved type or a family name (e.g. "bv" for `bv[8]`) the configured pack owns. */
template <NodeType node>
bool pack_owns_ba_type_name(const std::string& name);

/** @brief Whether @p ba_type_id is reserved or owned by the configured pack, compared by family name. */
template <NodeType node>
bool pack_owns_ba_type(size_t ba_type_id);

/** @brief Refine @p type_id to its owner's default parameter if underspecified, else unchanged. */
template <NodeType node>
size_t pack_default_ba_type(size_t type_id);

/** @brief Search @p term and its children for any assigned BA type id; return 0 if not found. */
template <NodeType node>
size_t find_ba_type (tref term);

/** @brief Search @p term and its children for any BA type tree; return `nullptr` if not found. */
template <NodeType node>
tref find_ba_type_tree (tref term);

/** @brief find_ba_type(term), falling back to the configured pack's default BA type when unresolved. */
template <NodeType node>
size_t find_ba_type_or_default (tref term);

/** @brief Print the type name for @p ba_type_id to @p os. */
template <NodeType node>
std::ostream& print_ba_type(std::ostream& os, size_t ba_type_id);

/** @brief Return `true` if the binary operator @p op can be applied to nodes @p n and @p m. */
template <NodeType node>
bool is_buildable(size_t op, tref n, tref m);

// True if type T appears in the std::variant V (used for compile-time BA pack checks)
template<typename T, typename V>
struct ba_variant_includes : std::false_type {};
template<typename T, typename... Ts>
struct ba_variant_includes<T, std::variant<Ts...>>
    : std::bool_constant<(std::is_same_v<T, Ts> || ...)> {};
template<typename T, typename V>
inline constexpr bool ba_variant_includes_v = ba_variant_includes<T, V>::value;

} // namespace idni::tau_lang

#include "ba_types.tmpl.h"

#endif // __IDNI__TAU__BA_TYPES_H__
