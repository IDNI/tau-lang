// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__TESTS__TEST_BOOL_HELPERS_H__
#define __IDNI__TAU__TESTS__TEST_BOOL_HELPERS_H__

// helper types and functions for tau with just Bool BA as tree<node<Bool>>
#define bas_pack Bool
#include "test_helpers.h"

namespace idni::tau_lang {

template<>
struct nso_factory<Bool> {

	optional<constant_with_type<Bool>> parse(
		const string& constant_source, const string) const
	{
		if (constant_source == "1" || constant_source == "true")
			return constant_with_type<Bool>{ Bool(true), "bool" };
		if (constant_source == "0" || constant_source == "false")
			return constant_with_type<Bool>{ Bool(false), "bool" };
		return {};
	}

	vector<string> types() const { return { "bool" }; }

	string default_type() const { return "bool"; }

	string one(const string) const { return "1"; }

	string zero(const string) const { return "0"; }

	constant_with_type<Bool> splitter_one(const string) const {
		return { Bool(true), "bool" };
	}

	// There is no tau_ba
	optional<rr> unpack_tau_ba(const variant<Bool>&) const { return {}; }

	static nso_factory<Bool>& instance() {
		static nso_factory<Bool> factory;
		return factory;
	}
private:
	nso_factory() {};
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__TESTS__TEST_BOOL_HELPERS_H__
