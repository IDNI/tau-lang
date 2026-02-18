// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

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

template <NodeType node>
bool is_tau_type(size_t t);

/**
 * @brief Creates the type tree associated with the type "nat"
 * @tparam Tree node type
 * @return Tree reference to type tree
 */
template <NodeType node>
tref nat_type();

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
tref bv_type(unsigned short bitwidth = default_bv_size);

template <NodeType node>
inline size_t bv_type_id();

template <NodeType node>
size_t bv_type_id(unsigned short bitwidth);

template <NodeType node>
tref bv_base_type();

/**
 * @brief Checks if t represents a bitvector type
 * @tparam node Tree node type
 * @param t Type tree object
 * @return If the type tree object represents a bitvector
 */
template <NodeType node>
bool is_bv_type_family(tref t);

template <NodeType node>
bool is_bv_type_family(size_t ba_type_id);

template <NodeType node>
bool is_bv_base_type(tref t);

template <NodeType node>
bool is_bv_base_type(size_t ba_type_id);

template <NodeType node>
size_t get_bv_width(tref t);


// -----------------------------------------------------------------------------
// BA types
//   contains type map of BA types
// is templated by NodeType to scope it to the specific BAs... packed node tree
template <NodeType node>
struct ba_types {
	// get the type id from the type name string id
	static size_t id(tref ba_type);

	// Get the type tree corresponding to the id
	static tref type_tree(size_t ba_type_id);

	// get the type name from the type map id
	static std::string name(size_t ba_type_id);

	// print the type name to the stream
	static std::ostream& print(std::ostream& os, size_t ba_type);

	static std::ostream& dump(std::ostream& os);

	static std::string dump_to_str();

private:
	// type_tree (index = ba_type id)
	static std::vector<tref>& type_trees();

	// type_tree -> ba_type id
	static subtree_map<node, size_t>& type_tree_to_idx();
};

// -----------------------------------------------------------------------------
// functional API to ba_types

// get the type id from the type name string id
template <NodeType node>
size_t get_ba_type_id(tref ba_type);

// Get the ba type ids of references
template <NodeType node>
std::vector<size_t> get_ba_type_ids(trefs t);

// Get the type tree corresponding to a type id
template <NodeType node>
tref get_ba_type_tree(size_t ba_type_id);

// get the type name from the type map id
template <NodeType node>
std::string get_ba_type_name(size_t ba_type_id);

// Check if type trees represent same type
template <NodeType node>
bool is_same_ba_type(tref t1, tref t2);

// Checks if the base types of t1 and t2 agree
// and returns the type with more information.
// If the types do not allow this, returns nullptr
template <NodeType node>
tref unify(tref t1, tref t2);

// Checks if the base types of tid1 and tid2 agree
// and returns the type with more information.
// If the types do not allow this, returns nullptr
template <NodeType node>
std::optional<size_t> unify(size_t tid1, size_t tid2);

// Checks if the types of a vector of trefs are compatible with the supplied
// default type. If so, returns the type and nullptr otherwise.
template <NodeType node>
tref unify(const trefs& ns, tref default_type);

template <NodeType node>
tref unify(const trefs& ns1, const trefs& ns2, tref default_type);

// Checks if the types of a vector of trefs are compatible with the supplied
// default type. If so, returns type id and nat_type_id otherwise.
template <NodeType node>
std::optional<size_t> unify(const std::vector<size_t>& nids, size_t default_type);

template <NodeType node>
std::optional<size_t> unify(const std::vector<size_t>& nids1, const std::vector<size_t>& nids2, size_t default_type);

template <NodeType node>
bool has_ba_type (tref term);

template <NodeType node>
size_t find_ba_type (tref term);

template <NodeType node>
tref find_ba_type_tree (tref term);

// print the type name to the stream
template <NodeType node>
std::ostream& print_ba_type(std::ostream& os, size_t ba_type_id);

// Check if the op is buildable
template <NodeType node>
bool is_buildable(size_t op, tref n, tref m);

} // namespace idni::tau_lang

#include "ba_types.tmpl.h"

#endif // __IDNI__TAU__BA_TYPES_H__
