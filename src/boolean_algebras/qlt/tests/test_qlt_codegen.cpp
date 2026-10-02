// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// codegen_constant_expr contract: a qlt constant reaches generated C++ as a
// self-contained tref expression rebuilding every piece of the set from exact
// qlt_rational(p, q) endpoints, never a rounded double.

#include "test_init.h"
#include "test_tau_helpers.h"
#include <sstream>
#include <string>

using namespace idni::tau_lang;

namespace {

bool has(const std::string& s, const std::string& pat) {
	return s.find(pat) != std::string::npos;
}

} // namespace

TEST_SUITE("qlt_codegen") {

	TEST_CASE("a set constant is rebuilt piece by piece, endpoints exact") {
		tref fm = api<node_t>::get_formula(
			"o1[t]:qlt = {[0,1000001/2000000) | 3}:qlt").value();
		tref cst = tau::get(fm).find_top([](tref n) {
			return tau::get(n).is_ba_constant(); });
		REQUIRE(cst != nullptr);
		auto e = ba_descriptor<qlt, node_t>::codegen_constant_expr(cst);
		REQUIRE(e.has_value());
		CHECK(has(*e, "qlt_rational(0, 1), ::idni::tau_lang::qlt_bound::CLOSED"));
		CHECK(has(*e, "qlt_rational(1000001, 2000000), "
			"::idni::tau_lang::qlt_bound::OPEN"));
		CHECK(has(*e, "qlt_rational(3, 1), ::idni::tau_lang::qlt_bound::CLOSED"));
	}
}
