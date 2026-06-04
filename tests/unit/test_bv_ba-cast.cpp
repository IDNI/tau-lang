// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "boolean_algebras/bv_ba.h"


TEST_SUITE("Configuration") {

	TEST_CASE("logging") {
		logging::trace();
	}

	TEST_CASE("bdd init") {
		bdd_init<Bool>();
	}
}

tref parse_wff(const std::string& sample) {
	auto opts = tau::get_options{
		.parse = { .start = tau::wff },
		.reget_with_hooks = true
	};
	tref src = tree<node_t>::get(sample, opts);
	if (src == nullptr) {
		TAU_LOG_ERROR << "Parsing failed for: " << sample;
	}
	return src;
}

TEST_SUITE("bv cast - zero extension") {

	// ((bv[16]) X:bv[8]) widens X from 8 to 16 bits (zero-fills high bits)
	TEST_CASE("zero-extend sat: ((bv[16]) X:bv[8]) = { 5 }:bv[16]") {
		auto src = parse_wff("((bv[16]) X:bv[8]) = { 5 }:bv[16]");
		CHECK( src != nullptr );
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// zero-extend produces a wider value; the narrow variable can't equal a
	// value that requires more than 8 bits (>255), so this is unsat
	TEST_CASE("zero-extend unsat: ((bv[16]) X:bv[8]) = { 256 }:bv[16]") {
		auto src = parse_wff("((bv[16]) X:bv[8]) = { 256 }:bv[16]");
		CHECK( src != nullptr );
		CHECK( is_bv_formula_unsat<node_t>(src) );
	}

	// valid: zero-extending anything keeps value in [0, 255]
	TEST_CASE("zero-extend valid: ((bv[16]) X:bv[8]) <= { 255 }:bv[16]") {
		auto src = parse_wff("((bv[16]) X:bv[8]) <= { 255 }:bv[16]");
		CHECK( src != nullptr );
		CHECK( is_bv_formula_valid<node_t>(src) );
	}

	// cast of constant: ((bv[16]) {5}:bv[8]) = {5}:bv[16]
	TEST_CASE("zero-extend constant: ((bv[16]) {5}:bv[8]) = { 5 }:bv[16]") {
		auto src = parse_wff("((bv[16]) { 5 }:bv[8]) = { 5 }:bv[16]");
		CHECK( src != nullptr );
		CHECK( is_bv_formula_valid<node_t>(src) );
	}
}

TEST_SUITE("bv cast - truncation") {

	// truncating to fewer bits discards high bits
	TEST_CASE("truncate sat: ((bv[8]) X:bv[16]) = { 5 }:bv[8]") {
		auto src = parse_wff("((bv[8]) X:bv[16]) = { 5 }:bv[8]");
		CHECK( src != nullptr );
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// truncation is always satisfiable (multiple inputs map to same output)
	TEST_CASE("truncate sat: ((bv[4]) X:bv[8]) = { 15 }:bv[4]") {
		auto src = parse_wff("((bv[4]) X:bv[8]) = { 15 }:bv[4]");
		CHECK( src != nullptr );
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// truncation of constant: low 8 bits of 0x1FF (511) = 0xFF (255)
	TEST_CASE("truncate constant: ((bv[8]) {255}:bv[16]) = { 255 }:bv[8]") {
		auto src = parse_wff("((bv[8]) { 255 }:bv[16]) = { 255 }:bv[8]");
		CHECK( src != nullptr );
		CHECK( is_bv_formula_valid<node_t>(src) );
	}
}

TEST_SUITE("bv cast - no-op (same width)") {

	TEST_CASE("same-width cast sat: ((bv[8]) X:bv[8]) = { 42 }:bv[8]") {
		auto src = parse_wff("((bv[8]) X:bv[8]) = { 42 }:bv[8]");
		CHECK( src != nullptr );
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}
}
