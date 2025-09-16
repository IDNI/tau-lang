// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_tau_helpers.h"
#include "resolver.h"


TEST_SUITE("new_infer_ba_types") {

	bool check(const char* sample, std::vector<std::pair<std::string, type_t>>& expected) {
		auto src = tau::get(sample, tau::get_options{
			.parse = { .start = tau::wff },
			.reget_with_hooks = false });
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
		return false;
	}

	TEST_CASE("simple variable assignment") {
		char const* sample = "x = 1.";
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", {get_ba_type_id<node_t>("sbf"), nullptr}}
		};
		CHECK( check(sample, expected) );
	}
}