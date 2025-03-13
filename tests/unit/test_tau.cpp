// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cassert>

#include "doctest.h"

#include "test_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau_lang;

namespace testing = doctest;

TEST_SUITE("something") {

	TEST_CASE("something") {
		CHECK( true );
	}
}
