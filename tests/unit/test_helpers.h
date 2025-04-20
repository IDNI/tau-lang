// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__TESTS__UNIT__TEST_HELPERS_H__
#define __IDNI__TAU__TESTS__UNIT__TEST_HELPERS_H__

#include <cassert>
#include "doctest.h"

#include "boolean_algebras/sbf_ba.h"

using namespace std;
using namespace idni;
using namespace idni::tau_lang;

namespace testing = doctest;

using bac = ba_constants<Bool>;
using bacb = ba_constants_binder<Bool>;
using btree = tree<node<Bool>>;

template <BAsPack... BAs>
tref make(const std::string& s, tau_parser::parse_options opts = {}) {
	return tree<node<BAs...>>::get(s, opts);
}

template <BAsPack... BAs>
typename tree<node<BAs...>>::traverser make_tt(const std::string& s,
	tau_parser::parse_options opts = {})
{
	return typename tree<node<BAs...>>::traverser(make<BAs...>(s, opts));
}

template <BAsPack... BAs>
optional<variant<BAs...>> get_constant(const std::string& s, const std::string& type_name) {
	return nso_factory<BAs...>::instance().parse(s, type_name);
}

namespace idni::tau_lang {

template<>
struct nso_factory<Bool> {

	std::optional<std::variant<Bool>> parse(
		const std::string& src, const std::string) const
	{
		if (src == "true") return Bool(true);
		if (src == "false") return Bool(false);
		return {};
	}

	tref binding(const std::string& src, const std::string& type_name) const {
		if (type_name != "bool") return nullptr;
		auto opt = parse(src, type_name);
		if (!opt.has_value()) return nullptr;
		return bacb::instance().bind(opt.value(), "bool");
	}

	std::vector<std::string> types() const { return { "bool" }; }

	std::string default_type() const { return "bool"; }

	std::string one(const std::string) const { return "true"; }

	std::string zero(const std::string) const { return "false"; }

	tref splitter_one(const std::string) const { return nullptr; }

	std::optional<Bool> unpack_tau_ba(const std::variant<Bool>&) const {
		return {};     // There is no tau_ba
	}

	static nso_factory<Bool>& instance() {
		static nso_factory<Bool> factory;
		return factory;
	}
private:
	nso_factory() {};
};

template<>
struct nso_factory<sbf_ba, Bool> {
	std::optional<std::variant<sbf_ba, Bool>> parse(
		const std::string&, const std::string) const
	{
		throw std::logic_error("not implemented");
	}

	tref binding(const std::string& src, const std::string& type_name) const {
		
		if (type_name == "sbf") {
			if (auto opt = sbf_ba_factory<sbf_ba, Bool>::instance()
						.parse(src); opt.has_value()) 
				return ba_constants_binder<sbf_ba, Bool>
					::instance().bind(opt.value(), "sbf");
		} else if (type_name == "bool")
			return nso_factory<Bool>::instance().binding(src, "bool");
		return nullptr;
	}

	std::vector<std::string> types() const { return { "sbf", "bool" }; }

	std::string default_type() const { return "sbf"; }

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

	static nso_factory<sbf_ba, Bool>& instance() {
		static nso_factory<sbf_ba, Bool> factory;
		return factory;
	}

private:

	nso_factory() {};
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__TESTS__UNIT__TEST_HELPERS_H__