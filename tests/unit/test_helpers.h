// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__TESTS__UNIT__TEST_HELPERS_H__
#define __IDNI__TAU__TESTS__UNIT__TEST_HELPERS_H__

#include <cassert>
#include "doctest.h"

#include "defs.h"
#include "init_log.h"

#include "tau_tree.h"
#include "boolean_algebras/variant_ba.h"
#include "boolean_algebras/bool_ba.h"
#include "boolean_algebras/bdds/bdd_handle.h"
#include "boolean_algebras/sbf_ba.h"

using namespace idni;
using namespace idni::tau_lang;
using namespace std;

namespace testing = doctest;

using bool_binder = ba_constants<Bool>;

struct named_binder_fixture {
	named_binder_fixture() : binder({
		{ "binding",  bool_binder::get(Bool(true),  "bool") },
		{ "binding2", bool_binder::get(Bool(false), "bool") } }) {}
	bool_binder binder;
};

struct bool_binder_fixture {
	bool_binder binder;
};

template <typename... BAs>
tref make(const std::string& s, tau_parser::parse_options opts = {}) {
	return tree<node<BAs...>>::get(s, opts);
}

template <typename... BAs>
typename tree<node<BAs...>>::traverser make_tt(const std::string& s,
	tau_parser::parse_options opts = {})
{
	return typename tree<node<BAs...>>::traverser(make<BAs...>(s, opts));
}

namespace idni::tau_lang {

template<>
struct nso_factory<Bool> {
	std::optional<std::variant<Bool>> parse(
		const std::string&, const std::string) const
	{
		throw std::logic_error("not implemented");
	}

	tref binding(const std::string&, const std::string& type_name) const {
		if (type_name != "bool") return nullptr;
		return bool_binder::get(Bool(true), "bool");
	}

	std::vector<std::string> types() const {
		return { "bool" };
	}

	std::string default_type() const {
		return "bool";
	}

	std::string one(const std::string) const {
		throw std::logic_error("not implemented");
	}

	std::string zero(const std::string) const {
		throw std::logic_error("not implemented");
	}

	tref splitter_one(const std::string) const {
		throw std::logic_error("not implemented");
	}

	std::optional<Bool> unpack_tau_ba(
		const std::variant<Bool>&) const {
		// There is no tau_ba
		return {};
	}

	static nso_factory<Bool>& instance() {
		static nso_factory<Bool> factory;
		return factory;
	}

private:

	nso_factory() {};
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__TESTS__UNIT__TEST_HELPERS_H__