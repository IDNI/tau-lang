// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__TESTS__TEST_BOOL_HELPERS_H__
#define __IDNI__TAU__TESTS__TEST_BOOL_HELPERS_H__

// helper types and functions for tau with just Bool BA as tree<node<Bool>>
#define bas_pack Bool
#include "test_helpers.h"

namespace idni::tau_lang {

template<>
struct nso_factory<Bool> {

	optional<variant<Bool>> parse(
		const string& src, const string) const
	{
		if (src == "true") return Bool(true);
		if (src == "false") return Bool(false);
		return {};
	}

	tref binding(const string& src, const string& type_name) const {
		if (type_name != "bool") return nullptr;
		auto opt = parse(src, type_name);
		if (!opt.has_value()) return nullptr;
		return ba_constants_binder<node<Bool>>::instance()
						.bind(opt.value(), "bool");
	}

	vector<string> types() const { return { "bool" }; }

	string default_type() const { return "bool"; }

	string one(const string) const { return "true"; }

	string zero(const string) const { return "false"; }

	tref splitter_one(const string) const { return nullptr; }

	// There is no tau_ba
	optional<Bool> unpack_tau_ba(const variant<Bool>&) const { return {}; }

	static nso_factory<Bool>& instance() {
		static nso_factory<Bool> factory;
		return factory;
	}
private:
	nso_factory() {};
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__TESTS__TEST_BOOL_HELPERS_H__
