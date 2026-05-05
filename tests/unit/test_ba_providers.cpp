// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// BAProvider / CompositeBAProvider conformance tests.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "composite_ba_provider.h"

using idni::tau_lang::BAProvider;
using idni::tau_lang::CompleteType;
using idni::tau_lang::CompositeBAProvider;
using idni::tau_lang::TypedContext;

TEST_SUITE("CompositeBAProvider — basic operations") {

	TEST_CASE("empty composite has size 0") {
		CompositeBAProvider c;
		CHECK(c.size() == 0);
	}

}
