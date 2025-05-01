// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__TESTS__UNIT__TEST_BOOL_HELPERS_H__
#define __IDNI__TAU__TESTS__UNIT__TEST_BOOL_HELPERS_H__

// helper types and functions for tau with just Bool BA as tree<node<Bool>>

namespace idni::tau_lang {

using bnode = node<Bool>;
using tau = tree<bnode>;
using tt = tau::traverser;
using bac = ba_constants<Bool>;
using bacb = ba_constants_binder<Bool>;

tau::parse_options parse_library() {
	static tau::parse_options opts(
		tau_parser::parse_options{ .start = tau::library });
	return opts;
}

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
		return ba_constants_binder<Bool>::instance()
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

// nso_factory for tests with <sbf_ba, Bool>
template<>
struct nso_factory<sbf_ba, Bool> {
	optional<variant<sbf_ba, Bool>> parse(
		const string&, const string) const
	{
		throw logic_error("not implemented");
	}

	tref binding(const string& src, const string& type_name) const {
		
		if (type_name == "sbf") {
			if (auto opt = sbf_ba_factory<sbf_ba, Bool>::instance()
						.parse(src); opt.has_value()) 
				return ba_constants_binder<sbf_ba, Bool>
					::instance().bind(opt.value(), "sbf");
		} else if (type_name == "bool")
			return nso_factory<Bool>::instance().binding(src, "bool");
		return nullptr;
	}

	vector<string> types() const { return { "sbf", "bool" }; }

	string default_type() const { return "sbf"; }

	string one(const string) const { throw logic_error("not implemented"); }

	string zero(const string) const { throw logic_error("not implemented");}

	tref splitter_one(const string) const {
		throw logic_error("not implemented");
	}

	// There is no tau_ba
	optional<Bool> unpack_tau_ba(const variant<Bool>&) const { return {}; }

	static nso_factory<sbf_ba, Bool>& instance() {
		static nso_factory<sbf_ba, Bool> factory;
		return factory;
	}
private:
	nso_factory() {};
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__TESTS__UNIT__TEST_BOOL_HELPERS_H__
