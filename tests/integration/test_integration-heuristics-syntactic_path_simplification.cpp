// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "heuristics/syntactic_path_simplification.h"

// ── syntactic_path_simplification_wff_comp ───────────────────────────────────

TEST_SUITE("syntactic_path_simplification_wff_comp") {

	TEST_CASE("equality < non-equality") {
		tref eq_atm  = get_nso_rr("x = 0.").value().main->get();
		tref neq_atm = get_nso_rr("y != 0.").value().main->get();
		CHECK(syntactic_path_simplification_wff_comp<node_t>(eq_atm, neq_atm)  == true);
		CHECK(syntactic_path_simplification_wff_comp<node_t>(neq_atm, eq_atm) == false);
	}

	TEST_CASE("equality is irreflexive") {
		tref atm = get_nso_rr("x = 0.").value().main->get();
		CHECK(syntactic_path_simplification_wff_comp<node_t>(atm, atm) == false);
	}

	TEST_CASE("non-equality is irreflexive") {
		tref atm = get_nso_rr("x != 0.").value().main->get();
		CHECK(syntactic_path_simplification_wff_comp<node_t>(atm, atm) == false);
	}

	TEST_CASE("two distinct equalities are strictly ordered") {
		tref atm_x = get_nso_rr("x = 0.").value().main->get();
		tref atm_y = get_nso_rr("y = 0.").value().main->get();
		bool xy = syntactic_path_simplification_wff_comp<node_t>(atm_x, atm_y);
		bool yx = syntactic_path_simplification_wff_comp<node_t>(atm_y, atm_x);
		CHECK(xy != yx);
	}

	TEST_CASE("two distinct non-equalities are strictly ordered") {
		tref atm_x = get_nso_rr("x != 0.").value().main->get();
		tref atm_y = get_nso_rr("y != 0.").value().main->get();
		bool xy = syntactic_path_simplification_wff_comp<node_t>(atm_x, atm_y);
		bool yx = syntactic_path_simplification_wff_comp<node_t>(atm_y, atm_x);
		CHECK(xy != yx);
	}
}

// ── syntactic_path_simplification_simplify_wff ───────────────────────────────

TEST_SUITE("syntactic_path_simplification_simplify_wff") {

	TEST_CASE("pure atom returned unchanged") {
		tref fm  = get_nso_rr("x = 0.").value().main->get();
		tref res = syntactic_path_simplification_simplify_wff<node_t>(fm);
		CHECK(tau::get(res) == tau::get(fm));
	}

	TEST_CASE("OR formula returned unchanged at top level") {
		tref fm  = get_nso_rr("x = 0 || y = 0.").value().main->get();
		tref res = syntactic_path_simplification_simplify_wff<node_t>(fm);
		CHECK(tau::get(res) == tau::get(fm));
	}

	TEST_CASE("x = 0 && !(x = 0) collapses to F") {
		tref fm  = get_nso_rr("x = 0 && !(x = 0).").value().main->get();
		tref res = syntactic_path_simplification_simplify_wff<node_t>(fm);
		CHECK(tau::get(res).equals_F());
	}

	TEST_CASE("duplicate conjunct is simplified") {
		tref fm  = get_nso_rr("x = 0 && y = 0 && x = 0.").value().main->get();
		tref res = syntactic_path_simplification_simplify_wff<node_t>(fm);
		// The duplicate x = 0 is removed; result should differ from input
		CHECK(tau::get(res) != tau::get(fm));
	}

	TEST_CASE("no negation: simple conjunction unchanged structure") {
		tref fm  = get_nso_rr("x = 0 && y = 0.").value().main->get();
		tref res = syntactic_path_simplification_simplify_wff<node_t>(fm);
		// No contradiction; formula may be restructured but is not F
		CHECK(!tau::get(res).equals_F());
	}
}

// ── syntactic_path_simplification_simplify_bf ────────────────────────────────

TEST_SUITE("syntactic_path_simplification_simplify_bf") {

	TEST_CASE("0 returned unchanged") {
		tref fm  = get_bf_nso_rr("", "0").value().main->get();
		tref res = syntactic_path_simplification_simplify_bf<node_t>(fm);
		CHECK(tau::get(res) == tau::get(fm));
	}

	TEST_CASE("1 returned unchanged") {
		tref fm  = get_bf_nso_rr("", "1").value().main->get();
		tref res = syntactic_path_simplification_simplify_bf<node_t>(fm);
		CHECK(tau::get(res) == tau::get(fm));
	}

	TEST_CASE("OR term returned unchanged at top level") {
		tref fm  = get_bf_nso_rr("", "x | y").value().main->get();
		tref res = syntactic_path_simplification_simplify_bf<node_t>(fm);
		CHECK(tau::get(res) == tau::get(fm));
	}

	TEST_CASE("x & x' collapses to 0") {
		tref fm  = get_bf_nso_rr("", "x & x'").value().main->get();
		tref res = syntactic_path_simplification_simplify_bf<node_t>(fm);
		CHECK(tau::get(res).equals_0());
	}

	TEST_CASE("x & z & x' collapses to 0") {
		tref fm  = get_bf_nso_rr("", "x & z & x'").value().main->get();
		tref res = syntactic_path_simplification_simplify_bf<node_t>(fm);
		CHECK(tau::get(res).equals_0());
	}
}

// ── syntactic_path_simplification_unsat_on_unchanged_negations ───────────────

TEST_SUITE("syntactic_path_simplification_unsat_on_unchanged_negations") {

	TEST_CASE("wff: x = 0 && !(x = 0) collapses to F") {
		tref fm  = get_nso_rr("x = 0 && !(x = 0).").value().main->get();
		tref res = syntactic_path_simplification_unsat_on_unchanged_negations<node_t>(fm);
		CHECK(tau::get(res).equals_F());
	}

	TEST_CASE("wff: NNF contradiction x = 0 && x != 0 not collapsed") {
		// In NNF form there is no explicit wff_neg, so no syntactic contradiction
		// is detectable by simplify_wff alone.
		tref fm  = get_nso_rr("x = 0 && x != 0.").value().main->get();
		tref res = syntactic_path_simplification_unsat_on_unchanged_negations<node_t>(fm);
		CHECK(!tau::get(res).equals_F());
	}

	TEST_CASE("bf: x & x' collapses to 0") {
		tref fm  = get_bf_nso_rr("", "x & x'").value().main->get();
		tref res = syntactic_path_simplification_unsat_on_unchanged_negations<node_t>(fm);
		CHECK(tau::get(res).equals_0());
	}

	TEST_CASE("bf: non-tautological OR is not turned into 1") {
		// unsat_on_unchanged_negations has no tautology pass.
		// A non-tautological OR (x | y) must not become 1.
		tref fm  = get_bf_nso_rr("", "x | y").value().main->get();
		tref res = syntactic_path_simplification_unsat_on_unchanged_negations<node_t>(fm);
		CHECK(!tau::get(res).equals_1());
	}
}

// ── syntactic_path_simplification (main entry point) ─────────────────────────

TEST_SUITE("syntactic_path_simplification") {
	TEST_CASE("1") {
		const char* sample = "x & (z' | (y & (k | x'))) & x | x & y | z & (z' | k) & z | (j & l) | k | (j & l)'";
		tref fm = get_bf_nso_rr("", sample).value().main->get();
		tref res = syntactic_path_simplification<node_t>(fm);
		CHECK((tau::get(res).to_str() == "1"));
	}
	TEST_CASE("2") {
		const char* sample = "x = 0 && (z != 0 || (y = 0 && (k = 0 || x != 0))) && x = 0 || x = 0 && y = 0 || z = 0 && (z != 0 || k = 0) && z = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = syntactic_path_simplification<node_t>(fm);
		CHECK( matches_to_str_to_any_of(res, {
			"x = 0 && (z != 0 || k = 0 && y = 0) || x = 0 && y = 0 || z = 0 && k = 0",
			"x = 0 && (z != 0 || y = 0 && k = 0) || y = 0 && x = 0 || z = 0 && k = 0",
			"x = 0 && (z != 0 || y = 0 && k = 0) || x = 0 && y = 0 || z = 0 && k = 0",
		}) );
	}
	TEST_CASE("3") {
		const char* sample = "x & (z' | (y & (k | x'))) & x | x & y | z & (z' | k) & z";
		tref fm = get_bf_nso_rr("", sample).value().main->get();
		tref res = syntactic_path_simplification<node_t>(fm);
		CHECK( matches_to_str_to_any_of(res, {
			"x&(z'|ky)|yx|zk",
			"x&(z'|yk)|xy|zk",
			"x&(z'|yk)|yx|zk",
			"x&(z'|ky)|xy|zk",
		}) );
	}
	TEST_CASE("4") {
		const char* sample = "x = 0 && (z != 0 || (y = 0 && (k = 0 || x != 0))) && x = 0 || x = 0 && y = 0 || z = 0 && (z != 0 || k = 0) && z = 0 || (j = 0 && l = 0) || k = 0 || !(j = 0 && l = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = syntactic_path_simplification<node_t>(fm);
		CHECK((tau::get(res).to_str() == "T"));
	}
	TEST_CASE("4_5") {
		const char* sample = "(ex x x = 0) && (ex x x != 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = syntactic_path_simplification<node_t>(fm);
		CHECK( matches_to_str_to_any_of(res, {
			"(ex b1 b1 != 0) && (ex b1 b1 = 0)",
			"(ex b1 b1 = 0) && (ex b1 b1 != 0)",
		}) );
	}
}
