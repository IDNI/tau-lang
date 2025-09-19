// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_tau_helpers.h"
#include "resolver.h"

TEST_SUITE("Configuration") {
	TEST_CASE("logging") {
		logging::trace();
	}
}

TEST_SUITE("new_infer_ba_types") {

	tref parse(const std::string& sample) {
		auto opts = tau::get_options{
			.parse = { .start = tau::wff },
			.infer_ba_types = false,
			.reget_with_hooks = false
		};
		auto src = tree<node_t>::get(sample, opts);
		if (src == nullptr) {
			TAU_LOG_ERROR << "Parsing failed for: " << sample;
		}
		return src;
	}

	bool check_vars(tref inferred, std::vector<std::pair<std::string, type_t>>& expected) {
		auto vars = tau::get(inferred).select_top(is<node_t, tau::variable>);
		if (vars.empty() && expected.size() > 0) {
			TAU_LOG_ERROR << "No variables found in.";
			return false;
		}
		if (vars.size() != expected.size()) {
			TAU_LOG_ERROR << "Expected " << expected.size()
				<< " variables, found " << vars.size();
			return false;
		}
		for (size_t i = 0; i < expected.size(); i++) {
			auto [expected_name, expected_type] = expected[i];
			auto name = get_var_name<node_t>(vars[i]);
			if (name != expected_name) {
				TAU_LOG_ERROR << "Variable " << (i+1)
					<< " expected name '" << expected_name
					<< "', found '" << name;
				return false;
			}
			size_t vtype = tau::get(vars[i]).get_ba_type();
			if (vtype != expected_type.first) {
				TAU_LOG_ERROR << "Variable '" << name
				<< "' expected type id " << expected_type.first
				<< ", found " << vtype;
				return false;
			}
			auto vsubtype = tt(vars[i]) | tau::subtype | tt::ref;
			if (vsubtype != expected_type.second) {
				TAU_LOG_ERROR << "Variable '" << name
					<< "' expected type '" << expected_type.second
					<< "', found '" << vsubtype;
				return false;
			}
		}
		return true;
	}

	bool check_ctes(tref inferred, std::vector<type_t>& expected) {
		auto ctes = tau::get(inferred).select_top(is<node_t, tau::bf_constant>);
		if (ctes.empty() && expected.size() > 0) {
			TAU_LOG_ERROR << "No constants found";
			return false;
		}
		if (ctes.size() != expected.size()) {
			TAU_LOG_ERROR << "Expected " << expected.size()
				<< " constants, found " << ctes.size();
			return false;
		}
		for (size_t i = 0; i < expected.size(); i++) {
			size_t ctype = tau::get(ctes[i]).get_ba_type();
			if (ctype != expected[i].first) {
				TAU_LOG_ERROR << "Constant '" << ctes[i]
					<< "' expected type id " << expected[i].first
					<< ", found " << ctype;
				return false;
			}
			auto csubtype = tt(ctes[i]) | tau::subtype | tt::ref;
			if (csubtype != expected[i].second) {
				TAU_LOG_ERROR << "Constant '" << ctes[i]
					<< "' expected subtype '" << expected[i].second
					<< "', found '" << tau::get(csubtype).dump_to_str() ;
				return false;
			}
		}
		return true;
	}

	static type_t tau_type = {get_ba_type_id<node_t>("tau"), nullptr};
	static type_t sbf_type = {get_ba_type_id<node_t>("sbf"), nullptr};

	TEST_CASE("simple variable assignment") {
		tref parsed = parse("x = 1");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", tau_type}
		};
		CHECK( check_vars(inferred, expected) );
	}
}