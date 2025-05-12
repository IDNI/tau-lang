// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__BA_TYPES_H__
#define __IDNI__TAU__BA_TYPES_H__

#include "tau_tree.h"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// ba and ref types inference (tau_tree_types.tmpl.h)
template <NodeType node>
tref infer_ba_types(tref n);

// -----------------------------------------------------------------------------
// BA types
//   contains type map of BA types
template <typename... BAs>
requires BAsPack<BAs...>
struct ba_types {
	// get the type id from the type name string id
	static size_t type_id(size_t ba_type_sid);

	// get the type id from the type name
	static size_t type_id(const std::string& ba_type_name);

	// get the type name from the type map id
	static const std::string& type_name(size_t ba_type);

	// print the type name to the stream
	static std::ostream& print_type(std::ostream& os, size_t ba_type);

private:
	inline static std::vector<size_t> types;               // type_sids (index = ba_type id)
	inline static std::map<size_t, size_t> type_names_map; // type_sid -> ba_type id
};

// -----------------------------------------------------------------------------
// functional API to ba_types

// get the type id from the type name string id
template <NodeType node>
size_t get_ba_type_id(size_t ba_type_sid);

// get the type id from the type name
template <NodeType node>
size_t get_ba_type_id(const std::string& ba_type_name);

// get the type name from the type map id
template <NodeType node>
std::string get_ba_type_name(size_t ba_type_id);

// print the type name to the stream
template <NodeType node>
std::ostream& print_ba_type(std::ostream& os, size_t ba_type_id);

} // namespace idni::tau_lang

#include "ba_types.tmpl.h"

#endif // __IDNI__TAU__BA_TYPES_H__
