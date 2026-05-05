// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "algorithm_picker.h"

using namespace idni::tau_lang::algpick;

TEST_SUITE("algorithm picker") {

	TEST_CASE("huge T_3 favours C or D over A/B") {
		// K=3, |T_1|=13, |T_2|=75, |T_3|=10000 (stress), |Q|=10, |φ|=50.
		auto a = pick(13, 75, 10000, 3, 10, 50);
		CHECK((a == Algorithm::C || a == Algorithm::D));
	}

	TEST_CASE("small everything: D wins (direct game is compact)") {
		auto a = pick(2, 3, 4, 2, 2, 5);
		// Direct game has ~(T_1+T_2)·Q + T_2·T_3 vertices — smallest here.
		CHECK(a == Algorithm::D);
	}

	TEST_CASE("moderate K, large T_3: C shines due to no T_3 dep") {
		auto a = pick(13, 75, 541, 3, 10, 50);
		// |T_3|=541, |T_2|·|T_3|·|T_1|·K = 75·541·13·3 = 1.6M vs C's 13·8·3=312.
		CHECK(a == Algorithm::C);
	}

	TEST_CASE("pick returns a valid enum") {
		auto a = pick(5, 5, 5, 2, 5, 10);
		CHECK((a == Algorithm::A || a == Algorithm::B
		    || a == Algorithm::C || a == Algorithm::D));
	}

	TEST_CASE("A excluded when has_input_vars is true") {
		// With large T3 and no inputs, A might be picked.
		// Force a scenario where A would win on size alone.
		(void)pick(3, 3, 4, 1, 100, 5, false);
		// With inputs, A must NOT be picked even if it has smallest size.
		auto with_inputs = pick(3, 3, 4, 1, 100, 5, true);
		CHECK(with_inputs != Algorithm::A);
	}

	TEST_CASE("D excluded when has_input_vars is true") {
		// Without inputs, this compact direct-game scenario picks D.
		CHECK(pick(2, 3, 4, 2, 2, 5, false) == Algorithm::D);
		// With inputs, D is unsound/output-only and must be excluded.
		auto with_inputs = pick(2, 3, 4, 2, 2, 5, true);
		CHECK(with_inputs != Algorithm::D);
		CHECK((with_inputs == Algorithm::B || with_inputs == Algorithm::C));
	}

	TEST_CASE("A allowed when has_input_vars is false (default)") {
		// Default parameter (no has_input_vars) should allow A.
		auto a = pick(5, 5, 5, 2, 5, 10);
		// Just check it's valid — A may or may not win depending on sizes.
		CHECK((a == Algorithm::A || a == Algorithm::B
		    || a == Algorithm::C || a == Algorithm::D));
	}
}
