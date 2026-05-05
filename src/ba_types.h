// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ba_types.h
 * @brief Boolean-algebra (BA) type registry, type-tree constructors, and query helpers.
 *
 * Defines type-tree builders (`tau_type`, `sbf_type`, `bv_type`, etc.),
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
// Type definitions for sbf
// TODO: They must go into sbf_ba.h -> currently causes compilation error

/**
 * @brief Create the type tree for the sbf type
 * @tparam node Tree node type
 * @return Tree reference representing sbf type tree
 */
template <NodeType node>
tref sbf_type();

/** @brief Return the integer type id for the `sbf` type under @p node. */
template <NodeType node>
inline size_t sbf_type_id();

/**
 * @brief Checks if t represents the sbf type
 * @tparam node Tree node type
 * @param t Type tree object
 * @return If the type tree object represents a sbf
 */
template <NodeType node>
bool is_sbf_type(tref t);

/** @brief Return `true` if type id @p t represents the sbf type. */
template <NodeType node>
bool is_sbf_type(size_t t);

// -----------------------------------------------------------------------------
// Type definitions for bool

/**
 * @brief Create the type tree for the bool type
 * @tparam node Tree node type
 * @return Tree reference representing bool type tree
 */
template <NodeType node>
tref bool_type();

/** @brief Return the integer type id for the `bool` type under @p node. */
template <NodeType node>
inline size_t bool_type_id();

/**
 * @brief Checks if t represents the bool type
 * @tparam node Tree node type
 * @param t Type tree object
 * @return If the type tree object represents a bool
 */
template <NodeType node>
bool is_bool_type(tref t);

/** @brief Return `true` if type id @p t represents the bool type. */
template <NodeType node>
bool is_bool_type(size_t t);

// -----------------------------------------------------------------------------
// Type definitions for bitvector
// TODO: They must go into bv_ba.h -> currently causes compilation error

/**
 * @brief Creates the type tree for a bitvector given a bitwidth
 * @tparam node Tree node type
 * @return Tree reference representing the type for bitvectors
 */
template <NodeType node>
tref bv_type(unsigned short bitwidth);

/** @brief Return the integer type id for a bitvector of @p bitwidth bits. */
template <NodeType node>
size_t bv_type_id(unsigned short bitwidth);

/**
 * @brief Checks if t represents a bitvector type
 * @tparam node Tree node type
 * @param t Type tree object
 * @return If the type tree object represents a bitvector
 */
template <NodeType node>
bool is_bv_type_family(tref t);

/** @brief Return `true` if type tree @p t belongs to the bitvector family (tref variant).
 *  @tparam node Tree node type.
 *  @param t tref.
 *  @return `true` if @p t is of bitvector type, else `false`.
*/
template <NodeType node>
bool is_tref_bv_type_family(tref t);

/** @brief Return `true` if type id @p ba_type_id belongs to the bitvector family. */
template <NodeType node>
bool is_bv_type_family(size_t ba_type_id);

/** @brief Return the bitwidth of the bitvector type tree @p t. */
template <NodeType node>
size_t get_bv_width(tref t);

/** @brief Return the bitwidth encoded in type id @p ba_type_id. */
template <NodeType node>
size_t get_bv_width(size_t ba_type_id);

// -----------------------------------------------------------------------------
// Type definitions for qint — atomless BA of rational intervals [x, y)
// (right-closed, left-open).  Constant syntax accepts both rationals (1/4)
// and decimals (0.25).

/**
 * @brief Create the type tree for the qint (rational intervals) type.
 * @tparam node Tree node type
 * @return Tree reference representing qint type tree
 */
template <NodeType node>
tref qint_type();

template <NodeType node>
inline size_t qint_type_id();

/**
 * @brief Checks if t represents the qint type.
 * @tparam node Tree node type
 * @param t Type tree object
 * @return true iff the type tree object represents qint
 */
template <NodeType node>
bool is_qint_type(tref t);

template <NodeType node>
bool is_qint_type(size_t t);

// -----------------------------------------------------------------------------
// Type definitions for qlt (Q,<)

/**
 * @brief Create the type tree for the qlt type
 * @tparam node Tree node type
 * @return Tree reference representing qlt type tree
 */
template <NodeType node>
tref qlt_type();

template <NodeType node>
inline size_t qlt_type_id();

/**
 * @brief Checks if t represents the qlt type
 * @tparam node Tree node type
 * @param t Type tree object
 * @return If the type tree object represents a qlt
 */
template <NodeType node>
bool is_qlt_type(tref t);

template <NodeType node>
bool is_qlt_type(size_t t);

// -----------------------------------------------------------------------------
// omcat (omega-categorical) type family
//
// An omega-categorical theory is one whose countable models are all isomorphic
// (e.g., (Q,<) — the dense linear order without endpoints).  The qlt type is
// the only concrete omcat type currently registered.  Use is_omcat_type_family
// rather than is_qlt_type in dispatch code so that future omcat types are
// handled automatically.

/**
 * @brief Checks if t represents any omega-categorical (omcat) type
 * @tparam node Tree node type
 * @param t Type tree object
 * @return true iff the type is any omega-categorical type (currently: qlt)
 */
template <NodeType node>
bool is_omcat_type_family(tref t);

template <NodeType node>
bool is_omcat_type_family(size_t t);



// -----------------------------------------------------------------------------
// Type definitions for nlang_ba

/**
 * @brief Create the type tree for the nlang type
 * @tparam node Tree node type
 * @return Tree reference representing nlang type tree
 */
template <NodeType node>
tref nlang_type();

template <NodeType node>
inline size_t nlang_type_id();

/**
 * @brief Checks if t represents the nlang type
 * @tparam node Tree node type
 * @param t Type tree object
 * @return If the type tree object represents an nlang
 */
template <NodeType node>
bool is_nlang_type(tref t);

template <NodeType node>
bool is_nlang_type(size_t t);

// -----------------------------------------------------------------------------
// Type definitions for hsb — atomless BA of lex-half-open polyhedra in R^d
// (LP_d^Q). Generalizes qint from 1D to d dimensions using canonical
// halfspaces with the lex-leading sign convention.

/**
 * @brief Create the type tree for the hsb type.
 * @tparam node Tree node type
 * @return Tree reference representing hsb type tree
 */
template <NodeType node>
tref hsb_type();

template <NodeType node>
inline size_t hsb_type_id();

/**
 * @brief Checks if t represents the hsb type.
 * @tparam node Tree node type
 * @param t Type tree object
 * @return true iff the type tree object represents hsb
 */
template <NodeType node>
bool is_hsb_type(tref t);

template <NodeType node>
bool is_hsb_type(size_t t);

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

/** @brief Search @p term and its children for any assigned BA type id; return 0 if not found. */
template <NodeType node>
size_t find_ba_type (tref term);

/** @brief Search @p term and its children for any BA type tree; return `nullptr` if not found. */
template <NodeType node>
tref find_ba_type_tree (tref term);

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
