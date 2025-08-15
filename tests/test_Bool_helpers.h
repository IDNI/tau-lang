// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__TESTS__TEST_BOOL_HELPERS_H__
#define __IDNI__TAU__TESTS__TEST_BOOL_HELPERS_H__

// helper types and functions for tau with just Bool BA as tree<node<Bool>>
#define bas_pack Bool
#include "test_helpers.h"

namespace idni::tau_lang {

template<>
struct nso_factory<Bool> {

	static vector<string> types() { return { "bool" }; }

	static string default_type() { return "bool"; }

	static string one(const string) { return "1"; }

	static string zero(const string) { return "0"; }

	static std::variant<Bool> splitter_one(const string) { return Bool(true);}

	// There is no tau_ba
	static tref unpack_tau_ba(const variant<Bool>&) { return nullptr; }

	static std::variant<Bool> pack_tau_ba(tref) { return {}; }
};

template <>
std::optional<typename ba_constants<node<Bool>>::constant_with_type> ba_constants<node<Bool>>::get(
		[[maybe_unused]] const std::string& constant_source,
		[[maybe_unused]] const std::string type_name,
		[[maybe_unused]] const std::string options) {
	if (constant_source == "1" || constant_source == "true")
		return ba_constants<node<Bool>>::constant_with_type{ Bool(true), "bool" };
	if (constant_source == "0" || constant_source == "false")
		return ba_constants<node<Bool>>::constant_with_type{ Bool(false), "bool" };
	return {};
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__TESTS__TEST_BOOL_HELPERS_H__
