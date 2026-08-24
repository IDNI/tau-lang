// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

// ── matches_tree_mod_and_or / canonicalize_tref ──────────────────────────────
//
// Exercises the tree-based comparison in test_helpers.h that replaced the
// enumerated-string-orderings in test_tau_bdd.cpp,
// test_integration-heuristics-simplify_using_equality.cpp and
// test_integration-heuristics-syntactic_path_simplification.cpp. The
// negative cases are the point of this suite: a canonicalizer that
// collapses everything to the same key would make every positive case
// below pass too.

TEST_SUITE("matches_tree_mod_and_or") {

	TEST_CASE("identical formula matches") {
		tref fm = tau::get("x = 0 || y = 0", parse_wff());
		CHECK(matches_wff_mod_and_or(fm, "x = 0 || y = 0"));
	}

	TEST_CASE("top-level OR operands reordered still match") {
		tref fm = tau::get("x = 0 || y = 0", parse_wff());
		CHECK(matches_wff_mod_and_or(fm, "y = 0 || x = 0"));
	}

	TEST_CASE("top-level AND operands reordered still match") {
		tref fm = tau::get("x = 0 && y = 0", parse_wff());
		CHECK(matches_wff_mod_and_or(fm, "y = 0 && x = 0"));
	}

	TEST_CASE("AND/OR reordered at two nesting levels still match") {
		tref fm = tau::get(
			"x = 0 && (y = 0 || z = 0) || w = 0", parse_wff());
		CHECK(matches_wff_mod_and_or(fm,
			"w = 0 || (z = 0 || y = 0) && x = 0"));
	}

	TEST_CASE("undelimited bf_and juxtaposition reordered still matches") {
		tref fm = tau::get("xy", parse_bf());
		CHECK(matches_bf_mod_and_or(fm, "yx"));
	}

	TEST_CASE("undelimited bf_and reordered inside bf_or still matches") {
		tref fm = tau::get("xy|zk", parse_bf());
		CHECK(matches_bf_mod_and_or(fm, "kz|yx"));
	}

	// ── negative controls ────────────────────────────────────────────────
	// Prove the helper rejects genuinely different formulas, not just that
	// it accepts reordered ones -- a canonicalizer that always returns the
	// same key would pass every case above vacuously.

	TEST_CASE("negative: different OR operand does not match") {
		tref fm = tau::get("x = 0 || y = 0", parse_wff());
		CHECK_FALSE(matches_wff_mod_and_or(fm, "x = 0 || z = 0"));
	}

	TEST_CASE("negative: different AND operand does not match") {
		tref fm = tau::get("x = 0 && y = 0", parse_wff());
		CHECK_FALSE(matches_wff_mod_and_or(fm, "x = 0 && z = 0"));
	}

	TEST_CASE("negative: different nesting (associativity) does not match") {
		// (x=0 && y=0) || z=0 is not the same formula as x=0 && (y=0 ||
		// z=0), despite sharing every atom and connective -- the
		// canonicalizer must not conflate genuinely different structure.
		tref fm = tau::get("(x = 0 && y = 0) || z = 0", parse_wff());
		CHECK_FALSE(matches_wff_mod_and_or(fm,
			"x = 0 && (y = 0 || z = 0)"));
	}

	TEST_CASE("equality between two plain variables ignores orientation") {
		// Both operands fall into term_comp's same, final category (a
		// plain variable, neither io nor an uninterpreted constant), so
		// their order is a content-hash tie-break (subtree_less), not
		// canonical -- .local/build-emscripten.md §4i's residual
		// paragraph. "x = y" and "y = x" are the same case, not two.
		tref fm = tau::get("x = y", parse_wff());
		CHECK(matches_wff_mod_and_or(fm, "y = x"));
	}

	TEST_CASE("negative: equality across term_comp categories keeps"
		" orientation")
	{
		// An output stream variable and an input stream variable are
		// different term_comp categories with a documented priority
		// order (§4i), so this orientation is meant to be canonical --
		// swapping it is a real bug, not noise, and must still fail.
		// Built through get_nso_rr rather than parse_wff() directly: a
		// bare parse leaves io_var's data field (which is_input_variable/
		// is_output_variable key off) unset, so both operands would read
		// as plain variables and the test would pass vacuously.
		tref fm = get_nso_rr("o1[t] = i1[t].").value().main->get();
		tref exp = get_nso_rr("i1[t] = o1[t].").value().main->get();
		CHECK_FALSE(matches_tree_mod_and_or(fm, exp));
	}

	TEST_CASE("negative: undelimited bf_and with a different operand"
		" does not match")
	{
		tref fm = tau::get("xy", parse_bf());
		CHECK_FALSE(matches_bf_mod_and_or(fm, "xz"));
	}

	TEST_CASE("negative: extra operand does not match") {
		tref fm = tau::get("x = 0 || y = 0", parse_wff());
		CHECK_FALSE(matches_wff_mod_and_or(fm, "x = 0 || y = 0 || z = 0"));
	}
}
