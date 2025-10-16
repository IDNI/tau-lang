// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__BA_TYPES_H__
#define __IDNI__TAU__BA_TYPES_H__

#include "boolean_algebras/sbf_ba.h"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// ba type checking and propagation
template <NodeType node>
tref infer_ba_types(tref n);

// -----------------------------------------------------------------------------
// Type definitions of untyped, tau and nat

/**
 * @brief Creates the type tree associated with the type "tau"
 * @tparam Tree node type
 * @return Tree reference to type tree
 */
template <NodeType node>
tref tau_type();

/**
 * @brief Checks if t represents the tau type
 * @tparam node Tree node type
 * @param t Type tree object
 * @return If the type tree object represents tau
 */
template <NodeType node>
bool is_tau_type(tref t);

/**
 * @brief Creates the type tree associated with the type "nat"
 * @tparam Tree node type
 * @return Tree reference to type tree
 */
template <NodeType node>
tref nat_type();

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

/**
 * @brief Checks if t has untyped as type information
 * @tparam node Tree node type
 * @param t Type tree object
 * @return If the type tree object represents untyped
 */
template <NodeType node>
bool is_untyped(tref t);

// -----------------------------------------------------------------------------
// Type definitions for bitvector
// TODO: They must go into bv_ba.h -> currently causes compilation error

/**
 * @brief Creates the type tree for a bitvector given a bitwidth
 * @tparam node Tree node type
 * @return Tree reference representing the type for bitvectors
 */
template <NodeType node>
tref bv_type(unsigned short bitwidth = 16);

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

// Get the type tree corresponding to a type id
template <NodeType node>
tref get_ba_type_tree(size_t ba_type_id);

// get the type name from the type map id
template <NodeType node>
std::string get_ba_type_name(size_t ba_type_id);

// Check if type trees represent same type
template <NodeType node>
bool is_same_ba_type(tref t1, tref t2);

// print the type name to the stream
template <NodeType node>
std::ostream& print_ba_type(std::ostream& os, size_t ba_type_id);


} // namespace idni::tau_lang

#include "ba_types.tmpl.h"

#endif // __IDNI__TAU__BA_TYPES_H__
