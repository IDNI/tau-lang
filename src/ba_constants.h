// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_tree.h"

#ifndef __IDNI__TAU__BA_CONSTANTS_H__
#define __IDNI__TAU__BA_CONSTANTS_H__

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// BA constants... static pool of BA constants
//   implements tref operator()(constant_source, type_name) binding interface
// - create custom nso_factory<BA> specialization for your BA type for factory binding
// - and/or provide map of named constants for named binding
template <typename... BAs>
struct ba_constants {
	using named_constants_map = std::map<std::string, tref>;
	using tau = tree<node<BAs...>>;

	// binds the constant value of a type name to a tau tree node
	static tref get(const std::variant<BAs...>& b,
						const std::string& type_name);

	// get the constant variant value from constant id
	static std::variant<BAs...> get(size_t constant_id);
	// std::get<BA> shortcut from constant id
	template <typename BA>
	static BA get(size_t constant_id);
	// std::get<BA> shortcut from constant tree node
	template <typename BA>
	static BA get(tref t);

	// check if the constant tree node is of a BA type
	template <typename BA>
	static bool is(tref t);
	// check if the constant id is of a BA type
	template <typename BA>
	static bool is(size_t constant_id);

	// get the constant id from the type name
	static size_t type_id(const std::string& type_name);
	// get the constant id from the type name string id
	static size_t type_id(size_t type_sid);
	// get the type name from the type map id
	static const std::string& type_name(size_t tid);

	// singleton instance for factory binding
	static ba_constants<BAs...>& instance();

	// default constructor with factory binding
	ba_constants();

	// constructor with named constants map for named binding with factory binding
	ba_constants(const named_constants_map& named_constants);

	// binds the constant source of a type name to a tau tree node
	tref bind(const std::string& src, const std::string& type_name);

	// binder interface operator
	tref operator()(const std::string& src, const std::string& type_name);

	// if binding fails, error is set to true
	bool error = false;

private:
	// binds the constant value of an untyped type to a tau tree node
	static size_t get(const std::variant<BAs...>& b);

	static inline std::vector<std::variant<BAs...>> C{ };
	inline static std::vector<size_t> types{};
	inline static std::map<size_t, size_t> types_map{};
	named_constants_map named_constants;
};

} // namespace idni::tau_lang

#include "ba_constants.tmpl.h"

#endif // __IDNI__TAU__BA_CONSTANTS_H__