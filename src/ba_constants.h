// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau.h"

#ifndef __IDNI__TAU__BA_CONSTANTS_H__
#define __IDNI__TAU__BA_CONSTANTS_H__

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// BA constants... static pool of BA constants
//   implements tref operator()(tref ba_constant_to_bind) interface
// 
template <typename... BAs>
struct ba_constants {
	using named_constants_map = std::map<std::string, tref>;
	using tau = tree<node<BAs...>>;

	inline static std::vector<size_t> types{};
	inline static std::map<size_t, size_t> types_map{};

	static size_t type_id(size_t type_sid);

	static size_t type_id(const std::string& type_name);

	static const std::string& type_name(size_t tid);

	static inline std::vector<std::variant<BAs...>> C{ };

	template <typename BA>
	static bool is(tref t);

	template <typename BA>
	static bool is(size_t constant_id);

	static std::variant<BAs...> get(size_t constant_id);

	template <typename BA>
	static BA get(size_t constant_id);

	template <typename BA>
	static BA get(tref t);

	static size_t get(const std::variant<BAs...>& b);

	static tref get(const std::variant<BAs...>& b, const std::string& type_name);

	static ba_constants<BAs...>& instance();

	ba_constants();
	ba_constants(const named_constants_map& named_constants);

	tref bind(const std::string& src, const std::string type_name);

	tref operator()(const std::string& src, const std::string type_name) {
		return bind(src, type_name); }

	// if binding fails, error is set to true
	bool error = false;
private:
	named_constants_map named_constants;
};

} // namespace idni::tau_lang

#include "ba_constants.tmpl.h"

#endif // __IDNI__TAU__BA_CONSTANTS_H__