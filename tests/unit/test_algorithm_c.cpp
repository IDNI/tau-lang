// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for Algorithm C skeleton builder (#19).

#include "test_init.h"
#include "algorithm_c_skeleton.h"

using namespace idni::tau_lang::alg_c;

TEST_SUITE("Algorithm C skeleton builder") {

	TEST_CASE("trivial: T1_size=1, K=1, phi*=D_0") {
		auto b = build_algorithm_c_skeleton(1, 1, "D_0");
		// Inputs: A_{0,0}, A_{0,1} — 2 env props.
		CHECK(b.ins.size() == 2u);
		CHECK(b.ins[0] == "A_0_0");
		CHECK(b.ins[1] == "A_0_1");
		// Outputs: R_0, D_0 — 2 sys props.
		CHECK(b.outs.size() == 2u);
		CHECK(b.outs[0] == "R_0");
		CHECK(b.outs[1] == "D_0");
		// Formula mentions Φ_R, middle, and phi*.
		CHECK(b.formula.find("G") != std::string::npos);
		CHECK(b.formula.find("R_0") != std::string::npos);
		CHECK(b.formula.find("D_0") != std::string::npos);
		CHECK(b.formula.find("A_0_0") != std::string::npos);
	}

	TEST_CASE("T1_size=3, K=2: |A| = 3*4 = 12, |outs| = 3+2 = 5") {
		auto b = build_algorithm_c_skeleton(3, 2, "G D_0");
		CHECK(b.ins.size() == 12u);
		CHECK(b.outs.size() == 5u);
	}

	TEST_CASE("independence from |T_3|") {
		// Alg C's proposition count must be O(|T_1| * 2^K + K + |T_1|)
		// regardless of |T_3|.  We verify it matches the formula even for
		// parameters that would give a huge T_3.
		auto b = build_algorithm_c_skeleton(13, 3, "true");
		CHECK(b.ins.size() == 13u * 8u);      // 104
		CHECK(b.outs.size() == 13u + 3u);     // 16
	}

	TEST_CASE("Φ_R uniqueness appears in formula") {
		auto b = build_algorithm_c_skeleton(3, 1, "D_0");
		// Φ_R should include !(R_0 & R_1) and similar pairwise.
		CHECK(b.formula.find("!(R_0 & R_1)") != std::string::npos);
		CHECK(b.formula.find("!(R_0 & R_2)") != std::string::npos);
		CHECK(b.formula.find("!(R_1 & R_2)") != std::string::npos);
	}
}
