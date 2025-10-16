// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__BA_CONSTANTS_H__
#define __IDNI__TAU__BA_CONSTANTS_H__

#include "tau_tree.h"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// BA constants
// static pool of BA constants
template <NodeType node>
struct ba_constants {
	using constant = typename node::constant;
	using constant_with_type = std::pair<constant, tref>;

	// insert the constant value of a type id to the pool
	static tref get(const constant& constant, size_t type_id);

	// insert the constant value of a type to the pool
	static tref get(const constant& constant, tref type_tree);

	// insert the constant given by the string of a type id to the pool
	//static tref get(const std::string& constant, size_t type_id);

	// get the constant variant value by constant id
	static constant get(size_t constant_id);

	// get the constant variant from source
	static std::optional<typename node::constant_with_type> get(
		const std::string& constant_source,
		tref type_tree,
		const std::string options = ""
	);

	// TODO (HIGH) add a similar method that takes a type (pair of ba_id and
	// subtype) to parse the constant instead of type name

	// dump the constant pool to a stream
	static std::ostream& dump(std::ostream& os);

	// dump the constant pool to a string
	static std::string dump_to_str();

	// Clean up the pool
	static void cleanup();

private:
	inline static std::vector<constant> C;  // pool of constants
	inline static trefs T;                  // pool of constant tree nodes with type info
};

} // namespace idni::tau_lang

#include "ba_constants.tmpl.h"

#endif // __IDNI__TAU__BA_CONSTANTS_H__