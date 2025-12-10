// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__TESTS__TEST_BOOL_HELPERS_H__
#define __IDNI__TAU__TESTS__TEST_BOOL_HELPERS_H__

// helper types and functions for tau with just Bool BA as tree<node<Bool>>
#define bas_pack bv, Bool
#include "test_helpers.h"

namespace idni::tau_lang {

tref bool_type() {
	tref type = tau::get(tau::type, "bool");
	return tau::get(tau::typed, type);
}

template<>
struct nso_factory<bv, Bool> {

	static std::vector<std::string> types() { return { "bool" }; }

	static tref default_type() { return bool_type(); }

	static std::string one(const tref) { return "1"; }

	static std::string zero(const tref) { return "0"; }

	static std::variant<bv, Bool> splitter_one(const std::string) { return Bool(true);}

	// There is no tau_ba
	static tref unpack_tau_ba(const std::variant<bv, Bool>&) { return nullptr; }

	static std::variant<bv, Bool> pack_tau_ba(tref) { return {}; }

	static std::variant<bv, Bool> to_base_ba_type(tref) {
		return { Bool() };
	}
};

template <>
std::optional<typename ba_constants<node<bv, Bool>>::constant_with_type> ba_constants<node<bv, Bool>>::get(
		const std::string& constant_source,
		[[maybe_unused]] tref type_tree,
		[[maybe_unused]] const std::string options) {
	if (constant_source == "1" || constant_source == "true")
		return ba_constants<node<bv, Bool>>::constant_with_type{ Bool(true), bool_type() };
	if (constant_source == "0" || constant_source == "false")
		return ba_constants<node<bv, Bool>>::constant_with_type{ Bool(false), bool_type() };
	return {};
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__TESTS__TEST_BOOL_HELPERS_H__
