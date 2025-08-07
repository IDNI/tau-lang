// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cassert>

#include "doctest.h"
#include "runtime.h"
#include "hooks.h"
#include "normalizer.h"

using namespace idni::rewriter;
using namespace idni::tau_lang;

namespace testing = doctest;

bool check_hook(const char* sample, const char* expected) {
	auto tau_sample = make_nso_using_factory<
		tau_ba<sbf_ba>, sbf_ba>(sample, { .start = tau_parser::wff }).value();
	auto tau_expected = make_nso_using_factory<
		tau_ba<sbf_ba>, sbf_ba>(expected, { .start = tau_parser::wff }).value();

	#ifdef DEBUG
	std::string str(sample);
	std::cout << "sample: " << str << " expected: " << tau_expected << " got: " << tau_sample << "\n";
	#endif // DEBUG

	return tau_sample == tau_expected;
}

TEST_SUITE("configuration") {

	TEST_CASE("bdd_init") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("bv operator hooks") {

	TEST_CASE("!") {
		CHECK( check_hook("!(a =_ b)", "a !=_ b") );
		CHECK( check_hook("!(a !=_ b)", "a =_ b") );
		CHECK( check_hook("!(a <=_ b)", "a !<=_ b") );
		CHECK( check_hook("!(a !<=_ b)", "a <=_ b") );
		CHECK( check_hook("!(a <_ b)", "a !<_ b") );
		CHECK( check_hook("!(a !<_ b)", "a <_ b") );
		CHECK( check_hook("!(a >_ b)", "a !>_ b") );
		CHECK( check_hook("!(a !>_ b)", "a >_ b") );
		CHECK( check_hook("!(a >=_ b)", "a !>=_ b") );
		CHECK( check_hook("!(a !>=_ b)", "a >=_ b") );
	}
}
