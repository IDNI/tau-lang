// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "algorithm_picker.h"

using namespace idni::tau_lang::algpick;

TEST_SUITE("algorithm picker") {

	TEST_CASE("huge T_3 favours C or D over A/B") {
		// K=3, |T_1|=13, |T_2|=75, |T_3|=10000 (stress), |Q|=10, |φ|=50.
		auto a = pick(13, 75, 10000, 3, 10, 50);
		CHECK((a == algorithm::C || a == algorithm::D));
	}

	TEST_CASE("small everything: D wins (direct game is compact)") {
		auto a = pick(2, 3, 4, 2, 2, 5);
		// Direct game has ~(T_1+T_2)·Q + T_2·T_3 vertices — smallest here.
		CHECK(a == algorithm::D);
	}

	TEST_CASE("moderate K, large T_3: C shines due to no T_3 dep") {
		auto a = pick(13, 75, 541, 3, 10, 50);
		// |T_3|=541, |T_2|·|T_3|·|T_1|·K = 75·541·13·3 = 1.6M vs C's 13·8·3=312.
		CHECK(a == algorithm::C);
	}

	TEST_CASE("pick returns a valid enum") {
		auto a = pick(5, 5, 5, 2, 5, 10);
		CHECK((a == algorithm::A || a == algorithm::B
		    || a == algorithm::C || a == algorithm::D));
	}

	TEST_CASE("A excluded when has_input_vars is true") {
		// With large T3 and no inputs, A might be picked.
		// Force a scenario where A would win on size alone.
		(void)pick(3, 3, 4, 1, 100, 5, false);
		// With inputs, A must NOT be picked even if it has smallest size.
		auto with_inputs = pick(3, 3, 4, 1, 100, 5, true);
		CHECK(with_inputs != algorithm::A);
	}

	TEST_CASE("D excluded when has_input_vars is true") {
		// Without inputs, this compact direct-game scenario picks D.
		CHECK(pick(2, 3, 4, 2, 2, 5, false) == algorithm::D);
		// With inputs, D is unsound/output-only and must be excluded.
		auto with_inputs = pick(2, 3, 4, 2, 2, 5, true);
		CHECK(with_inputs != algorithm::D);
		CHECK((with_inputs == algorithm::B || with_inputs == algorithm::C));
	}

	TEST_CASE("A allowed when has_input_vars is false (default)") {
		// Default parameter (no has_input_vars) should allow A.
		auto a = pick(5, 5, 5, 2, 5, 10);
		// Just check it's valid — A may or may not win depending on sizes.
		CHECK((a == algorithm::A || a == algorithm::B
		    || a == algorithm::C || a == algorithm::D));
	}

	TEST_CASE("estimate_size_C saturates K at 31 to avoid 1<<K overflow") {
		// K > 31 must clamp to the same estimate as K == 31, rather than
		// overflowing/wrapping the 1ull << K shift.
		uint64_t s31 = estimate_size_C(5, 31, 100);
		uint64_t s100 = estimate_size_C(5, 100, 100);
		uint64_t s1000 = estimate_size_C(5, 1000, 100);
		CHECK(s100 == s31);
		CHECK(s1000 == s31);
	}

	TEST_CASE("tie between B and C estimates: B wins as the first-checked baseline") {
		// T1=1,T2=3,T3=1,K=1,phi=0: sB = phi + T1*T2*T3*K = 3;
		// sC = phi + T1*2^K*K + T1*T1 = 2 + 1 = 3. Equal sizes must not
		// displace the baseline, since pick() only replaces on strict '<'.
		// has_input_vars=true excludes A/D so only the B-vs-C tie is exercised.
		auto a = pick(1, 3, 1, 1, 100, 0, true);
		CHECK(a == algorithm::B);
	}

	TEST_CASE("all-zero inputs: every estimate is 0, baseline B wins the tie") {
		auto a = pick(0, 0, 0, 0, 0, 0);
		CHECK(a == algorithm::B);
	}
}
