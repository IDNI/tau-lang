// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_sbf_ba_helpers.h"

TEST_SUITE("formulas: no variables, no bindings and no quantifiers") {

	TEST_CASE("i1[t] = o1[t]") {
		auto nso_rr = get_nso_rr("( i1[t] = o1[t] ).");
		CHECK( nso_rr.has_value() );
		if (!nso_rr.has_value()) return;
		tref result = normalizer<node_t>(nso_rr.value());
		auto check1 = tt(result) | tau::wff_f;
		auto check2 = tt(result) | tau::wff_t;
		CHECK( !check1.has_value() );
		CHECK( !check2.has_value() );
	}
}
