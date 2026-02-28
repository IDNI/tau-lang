// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_Bool_helpers.h"

#include "tau_tree.h"

TEST_SUITE("Semantic checks") {

	TEST_CASE("missplaced fallback") {
		logging::trace();

		const string input = "g[n](x):tau := g[n-1](x) fallback 1.";
		auto prr = parse_rec_relations();
		tref parsed = tau::get(input, prr);
		CHECK( parsed == nullptr );
		logging::info();
	}
}
