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

	bool check(const std::string sample, std::vector<std::pair<std::string, type_t>>& expected) {
		auto opts = tau::get_options{
			.parse = { .start = tau::wff },
			.infer_ba_types = false,
			.reget_with_hooks = false
		};
		auto src = tree<node_t>::get(sample, opts);

		if (src == nullptr) {
			TAU_LOG_ERROR << "Parsing failed for: " << sample;
			return false;
		}

		auto inferred = new_infer_ba_types<node_t>(src);

		if (inferred == nullptr) {
			TAU_LOG_ERROR << "Type inference failed for: " << sample;
			return false;
		}
		auto vars = tau::get(inferred).select_top(is<node_t, tau::variable>);
		if (vars.empty() && expected.size() > 0) {
			TAU_LOG_ERROR << "No variables found in: " << sample;
			return false;
		}
		if (vars.size() != expected.size()) {
			TAU_LOG_ERROR << "Expected " << expected.size()
				<< " variables, found " << vars.size()
				<< " in: " << sample;
			return false;
		}
		for (const auto& [name, type] : expected) {
			auto it = std::find_if(vars.begin(), vars.end(),
				[&name](const auto& v) {
					return get_var_name<node_t>(v) == name;
				});
			if (it == vars.end()) {
				TAU_LOG_ERROR << "Variable '" << name
					<< "' not found in: " << sample;
				return false;
			}
			size_t vtype = tau::get(*it).get_ba_type();
			if (vtype != type.first) {
				TAU_LOG_ERROR << "Variable '" << name
				<< "' expected type id " << type.first
				<< ", found " << vtype
				<< " in: " << sample;
				return false;
			}
			auto vsubtype = tt(*it) | tau::subtype | tt::ref;
			if (vsubtype != type.second) {
				TAU_LOG_ERROR << "Variable '" << name
					<< "' expected type '" << type.second
					<< "', found '" << vsubtype
					<< "' in: " << sample;
				return false;
			}
		}
		return true;
	}

	static type_t tau_type = {get_ba_type_id<node_t>("tau"), nullptr};
	static type_t sbf_type = {get_ba_type_id<node_t>("sbf"), nullptr};

	TEST_CASE("simple variable assignment") {
		std::string sample = "x = 1";
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", tau_type}
		};
		CHECK( check(sample, expected) );
	}
}