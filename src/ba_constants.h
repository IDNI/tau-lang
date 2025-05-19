// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__BA_CONSTANTS_H__
#define __IDNI__TAU__BA_CONSTANTS_H__

#include "tau_tree.h"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// BA constants
// static pool of BA constants
//   implements tref operator()(constant_source, type_name) binding interface
template <NodeType node>
struct ba_constants {
	using bas_variant = typename node::bas_variant;

	// pair of constant id and type id
	using typed_constant = std::pair<size_t, size_t>;

	// insert the constant value of a type name to the pool
	static typed_constant get(const bas_variant& b,
				const std::string& ba_type_name);

	// insert the constant value of a type id to the pool
	static typed_constant get(const bas_variant& b, size_t ba_type);

	// get the constant variant value by constant id
	static bas_variant get(size_t constant_id);

	// std::get<BA> shortcut from constant id
	template <typename BA>
	static BA get(size_t constant_id);

	// std::get<BA> shortcut from constant tree node
	template <typename BA>
	static BA get(tref t);

	// check if the constant id is of a BA type
	template <typename BA>
	static bool is(size_t constant_id);

	// get the type id from the constant id
	static size_t type_of(size_t constant_id);

	// print the constant value to the stream
	static std::ostream& print(std::ostream& os, size_t constant_id);
	// print the constant value to the stream
	static std::ostream& print_constant(std::ostream& os, size_t constant_id);
	static std::ostream& dump(std::ostream& os);
	static std::string dump_to_str();

private:
	// internal insertion of a constant into the pool
	static size_t get(const bas_variant& b);

	inline static std::vector<bas_variant> C;     // pool of constants
	inline static std::map<size_t, size_t> ba_type_map;       // constant_id -> ba_type id
};

// -----------------------------------------------------------------------------
// BA constants binder
// - create custom nso_factory<BA> specialization for your BA type for factory binding
// - and/or provide map of named constants for named binding
template <NodeType node>
struct ba_constants_binder {
	using bas_variant = typename node::bas_variant;

	// constant id and ba type id
	using typed_constant = std::pair<size_t, size_t>;

	// named constants map
	using named_constants_map = std::map<std::string, typed_constant>;

	// factory binding
	ba_constants_binder();

	// named binding with factory binding
	ba_constants_binder(const named_constants_map& named_constants);

	// binds the constant of a type into a tree, usually called from nso_factory
	tref bind(const bas_variant& constant, const std::string& ba_type_name);

	// binds the constant of a type into a tree, usually called internally
	tref bind(const bas_variant& constant, size_t ba_type);

	// binder interface operator
	tref operator()(const std::string& src, const std::string& ba_type_name);

	// singleton instance for factory binding
	static ba_constants_binder<node>& instance();

	// if binding fails, error is set to true
	bool error = false;

private:
	// tref bind(const typed_constant& constant_with_type);

	named_constants_map named_constants;
};

} // namespace idni::tau_lang

#include "ba_constants.tmpl.h"

#endif // __IDNI__TAU__BA_CONSTANTS_H__