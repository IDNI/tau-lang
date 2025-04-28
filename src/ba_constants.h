// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__BA_CONSTANTS_H__
#define __IDNI__TAU__BA_CONSTANTS_H__

#include "tau_tree.h"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// BA constants... static pool of BA constants
//   implements tref operator()(constant_source, type_name) binding interface
// - create custom nso_factory<BA> specialization for your BA type for factory binding
// - and/or provide map of named constants for named binding
template <typename... BAs>
requires BAsPack<BAs...>
struct ba_constants {

	// constant id and type id
	using typed_constant = std::pair<size_t, size_t>;

	// insert the constant value of a type name to the pool
	static typed_constant get(const std::variant<BAs...>& b,
				const std::string& type_name);

	// insert the constant value of a type id to the pool
	static typed_constant get(const std::variant<BAs...>& b,size_t type_id);

	// get the constant variant value by constant id
	static std::variant<BAs...> get(size_t constant_id);

	// std::get<BA> shortcut from constant id
	template <typename BA>
	requires OneOfBAs<BA, BAs...>
	static BA get(size_t constant_id);

	// std::get<BA> shortcut from constant tree node
	template <typename BA>
	requires OneOfBAs<BA, BAs...>
	static BA get(tref t);

	// check if the constant id is of a BA type
	template <typename BA>
	requires OneOfBAs<BA, BAs...>
	static bool is(size_t constant_id);

	// get the constant id from the type name string id
	static size_t type_id(size_t type_sid);

	// get the type id from the type name
	static size_t type_id(const std::string& type_name);

	// get the type name from the type map id
	static const std::string& type_name(size_t tid);

	// get the type id from the constant id
	static size_t type_of(size_t constant_id);

	// print the constant value to the stream
	static std::ostream& print(std::ostream& os, size_t constant_id);
	// print the constant value to the stream
	static std::ostream& print_constant(std::ostream& os, size_t constant_id);
	// print the type name to the stream
	static std::ostream& print_type(std::ostream& os, size_t type_id);

private:
	// internal insertion of a constant into the pool
	static size_t get(const std::variant<BAs...>& b);

	inline static std::vector<std::variant<BAs...>> C;     // pool of constants
	inline static std::vector<size_t> types;               // type_sids (index = type_id)
	inline static std::map<size_t, size_t> type_names_map; // type_sid -> type_id
	inline static std::map<size_t, size_t> type_map;       // constant_id -> type_id
};

template <typename... BAs>
requires BAsPack<BAs...>
struct ba_constants_binder {
	// constant id and type id
	using typed_constant = std::pair<size_t, size_t>;

	// named constants map
	using named_constants_map = std::map<std::string, typed_constant>;

	// factory binding
	ba_constants_binder();

	// named binding with factory binding
	ba_constants_binder(const named_constants_map& named_constants);

	// binds the constant of a type into a tree, usually called from nso_factory
	tref bind(const std::variant<BAs...>& constant, const std::string& type_name);

	// binds the constant of a type into a tree, usually called internally
	tref bind(const std::variant<BAs...>& constant, size_t type_id);

	// binder interface operator
	tref operator()(const std::string& src, const std::string& type_name);

	// singleton instance for factory binding
	static ba_constants_binder<BAs...>& instance();

	// if binding fails, error is set to true
	bool error = false;

private:
	// tref bind(const typed_constant& constant_with_type);

	named_constants_map named_constants;
};

} // namespace idni::tau_lang

#include "ba_constants.tmpl.h"

#endif // __IDNI__TAU__BA_CONSTANTS_H__