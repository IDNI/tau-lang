// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "heuristics/simplify_using_equality.h"

// ── Helper ───────────────────────────────────────────────────────────────────

static auto make_uf() {
	return union_find_with_sets<
		decltype(simplify_using_equality_term_comp<node_t>), node_t>(
			simplify_using_equality_term_comp<node_t>);
}

// ── simplify_using_equality_term_comp ────────────────────────────────────────

TEST_SUITE("simplify_using_equality_term_comp") {

	TEST_CASE("0 < variable") {
		tref atm    = get_nso_rr("x = 0.").value().main->get();
		tref x_t    = tau::get(atm)[0].first();
		tref zero_t = tau::get(atm)[0].second();
		CHECK(simplify_using_equality_term_comp<node_t>(zero_t, x_t)   == true);
		CHECK(simplify_using_equality_term_comp<node_t>(x_t, zero_t)   == false);
	}

	TEST_CASE("0 < 1") {
		tref atm0   = get_nso_rr("x = 0.").value().main->get();
		tref atm1   = get_nso_rr("x = 1.").value().main->get();
		tref zero_t = tau::get(atm0)[0].second();
		tref one_t  = tau::get(atm1)[0].second();
		CHECK(simplify_using_equality_term_comp<node_t>(zero_t, one_t) == true);
		CHECK(simplify_using_equality_term_comp<node_t>(one_t, zero_t) == false);
	}

	TEST_CASE("1 < variable") {
		tref atm   = get_nso_rr("x = 1.").value().main->get();
		tref x_t   = tau::get(atm)[0].first();
		tref one_t = tau::get(atm)[0].second();
		CHECK(simplify_using_equality_term_comp<node_t>(one_t, x_t)   == true);
		CHECK(simplify_using_equality_term_comp<node_t>(x_t, one_t)   == false);
	}

	TEST_CASE("0 is irreflexive") {
		tref atm    = get_nso_rr("x = 0.").value().main->get();
		tref zero_t = tau::get(atm)[0].second();
		CHECK(simplify_using_equality_term_comp<node_t>(zero_t, zero_t) == false);
	}

	TEST_CASE("1 is irreflexive") {
		tref atm   = get_nso_rr("x = 1.").value().main->get();
		tref one_t = tau::get(atm)[0].second();
		CHECK(simplify_using_equality_term_comp<node_t>(one_t, one_t) == false);
	}

	TEST_CASE("variable is irreflexive") {
		tref atm = get_nso_rr("x = 0.").value().main->get();
		tref x_t = tau::get(atm)[0].first();
		CHECK(simplify_using_equality_term_comp<node_t>(x_t, x_t) == false);
	}

	TEST_CASE("two distinct variables have strict ordering") {
		tref atm_x = get_nso_rr("x = 0.").value().main->get();
		tref atm_y = get_nso_rr("y = 0.").value().main->get();
		tref x_t   = tau::get(atm_x)[0].first();
		tref y_t   = tau::get(atm_y)[0].first();
		bool xy    = simplify_using_equality_term_comp<node_t>(x_t, y_t);
		bool yx    = simplify_using_equality_term_comp<node_t>(y_t, x_t);
		CHECK(xy != yx);
	}
}

// ── simplify_using_equality_sort_atms ────────────────────────────────────────

TEST_SUITE("simplify_using_equality_sort_atms") {

	TEST_CASE("equality precedes inequality") {
		tref fm   = get_nso_rr("x != 0 && y = 0.").value().main->get();
		trefs conjs = get_cnf_wff_clauses<node_t>(fm);
		simplify_using_equality_sort_atms<node_t>(conjs);
		REQUIRE(conjs.size() == 2);
		CHECK( tau::get(conjs[0]).child_is(tau::bf_eq));
		CHECK(!tau::get(conjs[1]).child_is(tau::bf_eq));
	}

	TEST_CASE("all equalities precede all inequalities") {
		tref fm = get_nso_rr("x != 0 && y = 0 && z != 0 && w = 0.").value().main->get();
		trefs conjs = get_cnf_wff_clauses<node_t>(fm);
		simplify_using_equality_sort_atms<node_t>(conjs);
		REQUIRE(conjs.size() == 4);
		CHECK( tau::get(conjs[0]).child_is(tau::bf_eq));
		CHECK( tau::get(conjs[1]).child_is(tau::bf_eq));
		CHECK(!tau::get(conjs[2]).child_is(tau::bf_eq));
		CHECK(!tau::get(conjs[3]).child_is(tau::bf_eq));
	}

	TEST_CASE("equational assignment before non-assignment equality") {
		// "w = 0" is an equational assignment (LHS is a bare variable)
		// "xy = 0" is an equality but not an assignment (LHS is a product)
		tref fm = get_nso_rr("xy = 0 && w = 0.").value().main->get();
		trefs conjs = get_cnf_wff_clauses<node_t>(fm);
		simplify_using_equality_sort_atms<node_t>(conjs);
		REQUIRE(conjs.size() == 2);
		CHECK(is_equational_assignment<node_t>(conjs[0]));
		CHECK(!is_equational_assignment<node_t>(conjs[1]));
	}

	TEST_CASE("only equalities: all remain equalities") {
		tref fm = get_nso_rr("x = 0 && y = 0.").value().main->get();
		trefs conjs = get_cnf_wff_clauses<node_t>(fm);
		simplify_using_equality_sort_atms<node_t>(conjs);
		REQUIRE(conjs.size() == 2);
		CHECK(tau::get(conjs[0]).child_is(tau::bf_eq));
		CHECK(tau::get(conjs[1]).child_is(tau::bf_eq));
	}

	TEST_CASE("only inequalities: all remain inequalities") {
		tref fm = get_nso_rr("x != 0 && y != 0.").value().main->get();
		trefs conjs = get_cnf_wff_clauses<node_t>(fm);
		simplify_using_equality_sort_atms<node_t>(conjs);
		REQUIRE(conjs.size() == 2);
		CHECK(!tau::get(conjs[0]).child_is(tau::bf_eq));
		CHECK(!tau::get(conjs[1]).child_is(tau::bf_eq));
	}
}

// ── simplify_using_equality_direct_atm ───────────────────────────────────────

TEST_SUITE("simplify_using_equality_direct_atm") {

	TEST_CASE("non-equality atom returned unchanged") {
		tref atm    = get_nso_rr("x != 0.").value().main->get();
		tref result = simplify_using_equality_direct_atm<node_t>(atm);
		CHECK(tau::get(result) == tau::get(atm));
	}

	TEST_CASE("x = 0: variable already on left, returned unchanged") {
		tref atm    = get_nso_rr("x = 0.").value().main->get();
		tref result = simplify_using_equality_direct_atm<node_t>(atm);
		CHECK(tau::get(result) == tau::get(atm));
	}

	TEST_CASE("reversed equality 0 = x is reoriented to x = 0") {
		tref orig   = get_nso_rr("x = 0.").value().main->get();
		tref x_t    = tau::get(orig)[0].first();
		tref zero_t = tau::get(orig)[0].second();
		tref rev    = tau::build_bf_eq(zero_t, x_t);
		tref result = simplify_using_equality_direct_atm<node_t>(rev);
		CHECK(matches_to_str_to_any_of(result, { "x = 0" }));
	}

	TEST_CASE("reversed inequality 0 != x is reoriented to x != 0") {
		tref orig   = get_nso_rr("x != 0.").value().main->get();
		tref x_t    = tau::get(orig)[0].first();
		tref zero_t = tau::get(orig)[0].second();
		tref rev    = tau::build_bf_neq(zero_t, x_t);
		tref result = simplify_using_equality_direct_atm<node_t>(rev);
		CHECK(matches_to_str_to_any_of(result, { "x != 0" }));
	}

	TEST_CASE("idempotent for equality atom") {
		tref atm = get_nso_rr("xy = 0.").value().main->get();
		tref r1  = simplify_using_equality_direct_atm<node_t>(atm);
		tref r2  = simplify_using_equality_direct_atm<node_t>(r1);
		CHECK(tau::get(r1) == tau::get(r2));
	}
}

// ── simplify_using_equality_add_raw_equality ─────────────────────────────────

TEST_SUITE("simplify_using_equality_add_raw_equality") {

	TEST_CASE("T returns true") {
		auto uf  = make_uf();
		CHECK(simplify_using_equality_add_raw_equality<node_t>(uf, _T<node_t>()) == true);
	}

	TEST_CASE("F returns false") {
		auto uf  = make_uf();
		CHECK(simplify_using_equality_add_raw_equality<node_t>(uf, _F<node_t>()) == false);
	}

	TEST_CASE("x = y: both terms become connected") {
		auto uf  = make_uf();
		tref eq  = get_nso_rr("x = y.").value().main->get();
		tref x_t = tau::get(eq)[0].first();
		tref y_t = tau::get(eq)[0].second();
		bool ok  = simplify_using_equality_add_raw_equality<node_t>(uf, eq);
		CHECK(ok == true);
		CHECK(uf.connected(x_t, y_t));
	}

	TEST_CASE("x = x returns true with no connectivity contradiction") {
		auto uf  = make_uf();
		tref eq  = get_nso_rr("x = 0.").value().main->get();
		tref x_t = tau::get(eq)[0].first();
		bool ok  = simplify_using_equality_add_raw_equality<node_t>(uf, tau::build_bf_eq(x_t, x_t));
		CHECK(ok == true);
	}

	TEST_CASE("x = 0 then x = 1 is a contradiction") {
		auto uf  = make_uf();
		tref eq0 = get_nso_rr("x = 0.").value().main->get();
		tref eq1 = get_nso_rr("x = 1.").value().main->get();
		bool ok0 = simplify_using_equality_add_raw_equality<node_t>(uf, eq0);
		CHECK(ok0 == true);
		bool ok1 = simplify_using_equality_add_raw_equality<node_t>(uf, eq1);
		CHECK(ok1 == false);
	}

	TEST_CASE("x = y and y = z make x and z connected") {
		auto uf   = make_uf();
		tref eq1  = get_nso_rr("x = y.").value().main->get();
		tref eq2  = get_nso_rr("y = z.").value().main->get();
		tref x_t  = tau::get(eq1)[0].first();
		tref z_t  = tau::get(eq2)[0].second();
		simplify_using_equality_add_raw_equality<node_t>(uf, eq1);
		simplify_using_equality_add_raw_equality<node_t>(uf, eq2);
		CHECK(uf.connected(x_t, z_t));
	}
}

// ── simplify_using_equality_add_equality ─────────────────────────────────────

TEST_SUITE("simplify_using_equality_add_equality") {

	TEST_CASE("x = y (non-zero RHS) delegates to add_raw_equality") {
		auto uf  = make_uf();
		tref eq  = get_nso_rr("x = y.").value().main->get();
		tref x_t = tau::get(eq)[0].first();
		tref y_t = tau::get(eq)[0].second();
		bool ok  = simplify_using_equality_add_equality<node_t>(uf, eq);
		CHECK(ok == true);
		CHECK(uf.connected(x_t, y_t));
	}

	TEST_CASE("x = 0: term x connected to 0") {
		auto uf     = make_uf();
		tref eq     = get_nso_rr("x = 0.").value().main->get();
		tref x_t    = tau::get(eq)[0].first();
		tref zero_t = tau::get(eq)[0].second();
		bool ok     = simplify_using_equality_add_equality<node_t>(uf, eq);
		CHECK(ok == true);
		CHECK(uf.connected(x_t, zero_t));
	}

	TEST_CASE("x|y = 0 decomposes: x and y each connected to 0") {
		auto uf     = make_uf();
		tref eq     = get_nso_rr("x|y = 0.").value().main->get();
		tref eq_x   = get_nso_rr("x = 0.").value().main->get();
		tref eq_y   = get_nso_rr("y = 0.").value().main->get();
		tref x_t    = tau::get(eq_x)[0].first();
		tref y_t    = tau::get(eq_y)[0].first();
		tref zero_t = tau::get(eq_x)[0].second();
		bool ok     = simplify_using_equality_add_equality<node_t>(uf, eq);
		CHECK(ok == true);
		CHECK(uf.connected(x_t, zero_t));
		CHECK(uf.connected(y_t, zero_t));
	}

	TEST_CASE("x = 0 then x = 1 is a contradiction") {
		auto uf  = make_uf();
		tref eq0 = get_nso_rr("x = 0.").value().main->get();
		tref eq1 = get_nso_rr("x = 1.").value().main->get();
		simplify_using_equality_add_equality<node_t>(uf, eq0);
		CHECK(simplify_using_equality_add_equality<node_t>(uf, eq1) == false);
	}
}

// ── simplify_using_equality_simplify_equation ────────────────────────────────

TEST_SUITE("simplify_using_equality_simplify_equation") {

	TEST_CASE("T returned as-is") {
		auto uf  = make_uf();
		tref T   = _T<node_t>();
		tref res = simplify_using_equality_simplify_equation<node_t>(uf, T);
		CHECK(tau::get(res) == tau::get(T));
	}

	TEST_CASE("F returned as-is") {
		auto uf  = make_uf();
		tref F   = _F<node_t>();
		tref res = simplify_using_equality_simplify_equation<node_t>(uf, F);
		CHECK(tau::get(res) == tau::get(F));
	}

	TEST_CASE("no known equalities: equation returned unchanged") {
		auto uf  = make_uf();
		tref eq  = get_nso_rr("x = y.").value().main->get();
		tref res = simplify_using_equality_simplify_equation<node_t>(uf, eq);
		CHECK(tau::get(res) == tau::get(eq));
	}

	TEST_CASE("term known equal to 0 is replaced") {
		auto uf    = make_uf();
		tref eq_x0 = get_nso_rr("x = 0.").value().main->get();
		simplify_using_equality_add_raw_equality<node_t>(uf, eq_x0);
		tref eq_xy = get_nso_rr("x = y.").value().main->get();
		tref res   = simplify_using_equality_simplify_equation<node_t>(uf, eq_xy);
		// x was merged with 0 (0 < x), so x is replaced by 0; result differs from input
		CHECK(tau::get(res) != tau::get(eq_xy));
	}

	TEST_CASE("x = y known: x replaced by canonical representative in another eq") {
		auto uf   = make_uf();
		tref eq   = get_nso_rr("x = y.").value().main->get();
		simplify_using_equality_add_raw_equality<node_t>(uf, eq);
		// Now simplify "x = z": x should be replaced by the UF root (y or x)
		tref eq2  = get_nso_rr("x = z.").value().main->get();
		tref res  = simplify_using_equality_simplify_equation<node_t>(uf, eq2);
		// x and y are merged; the LHS of res should be the canonical rep
		tref lhs  = tau::get(res)[0].first();
		bool lhs_is_root = tau::get(lhs) == tau::get(uf.find(tau::get(eq)[0].first()));
		CHECK(lhs_is_root);
	}
}

// ── simplify_using_equality (main entry point) ───────────────────────────────

TEST_SUITE("simplify_using_equality") {
	TEST_CASE("1") {
		const char* sample = "xy|zx = 0 && xy = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK( matches_to_str_to_any_of(res, {
			"yx|xz = 0",
			"xy|xz = 0",
			"yx|zx = 0",
			"xy|zx = 0",
		}) );
	}
	TEST_CASE("2") {
		const char* sample = "(o1[1]' = 0 && s = 0 && o1[1] = 0 && y|y'w != 0 && y != 0 && w != 0 && z != 0 && o1[0]' = 0 || o1[0]o1[1]'|o1[0]' = 0 && s = 0 && (s = 0 && o1[1] = 0 && y|y'w != 0 && y != 0 && w != 0 && z != 0 || z|z's != 0 && s != 0 && y|y'w != 0 && w != 0 && (z != 0 || y = 0 || o1[1]' = 0) && (y != 0 || z = 0) || z|z's != 0 && y|y'w != 0 && y != 0 && w != 0 && (s != 0 || o1[1] = 0) && (z != 0 || o1[1]' = 0)) && o1[0]' != 0) && v != 0 && x != 0 && o1[0] != 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).equals_F());
	}
	TEST_CASE("3") {
		const char* sample = "(w != 0 || o1[0]'&x = 0) && x&o1[0]'|o2[0] = 0 && x'|o1[0] != 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).equals_F());
	}
	TEST_CASE("5") {
		const char* sample = "(w != 0 || o1[0]'&x = 0) && x&o1[0]'|o2[0] = 0 && x'|o1[0] != 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).equals_F());
	}
	TEST_CASE("6") {
		const char* sample = "(ex x x = 0) && (ex x x != 0) && ad != 0 && z = 0 && ad = z.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).equals_F());
	}
	TEST_CASE("7") {
		const char* sample = "xy = 0 && vw = 0 && (yw|xy|vw = 0 && xv|yw|xy|vw = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK( matches_to_str_to_any_of(res, {
			"yx = 0 && vw = 0 && wy = 0 && vx = 0",
			"xy = 0 && vw = 0 && yw = 0 && xv = 0",
			"yx = 0 && vw = 0 && yw = 0 && vx = 0",
			"yx = 0 && wv = 0 && yw = 0 && xv = 0",
			"yx = 0 && wv = 0 && yw = 0 && vx = 0",
			"xy = 0 && vw = 0 && wy = 0 && vx = 0",
			"yx = 0 && wv = 0 && wy = 0 && vx = 0",
			"xy = 0 && wv = 0 && wy = 0 && xv = 0",
		}) );
	}
	TEST_CASE("8") {
		const char* sample = "xyk|x'yk:bv[16] < 1 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && k < 1");
	}
	TEST_CASE("9") {
		const char* sample = "xyk|x'yk:bv[16] !< 1 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && k' = 0");
	}
	TEST_CASE("10") {
		const char* sample = "xyk|x'yk:bv[16] > 0 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && 0 < k");
	}
	TEST_CASE("11") {
		const char* sample = "xyk|x'yk:bv[16] !> 0 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && k = 0");
	}
	TEST_CASE("12") {
		const char* sample = "xyk|x'yk:bv[16] <= 0 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && k = 0");
	}
	TEST_CASE("13") {
		const char* sample = "xyk|x'yk:bv[16] !<= 0 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && k !<= 0");
	}
	TEST_CASE("14") {
		const char* sample = "xyk|x'yk:bv[16] >= 1 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && k' = 0");
	}
	TEST_CASE("15") {
		const char* sample = "xyk|x'yk:bv[16] !>= 1 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && 1 !<= k");
	}
	// ── 3-level nested OR: uf_stack balance and branch isolation ────────────────

	TEST_CASE("nested_or_3_identical_branches_simplified") {
		// Three identical branches; each has x=0 so y=x simplifies to y=0.
		// Exercises the uf_stack push/pop path for 3+ nested disjunctions and
		// verifies the stack remains balanced (caught by the DBG assert inside
		// simplify_using_equality).
		const char* sample =
			"(x = 0 && y = x) || ((x = 0 && y = x) || (x = 0 && y = x)).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).to_str() == "x = 0 && y = 0");
	}

	TEST_CASE("nested_or_3_branch_isolation") {
		// x=0 established in branch 1 must NOT propagate into branches 2/3.
		// If the uf_stack leaks across OR boundaries, y=x and z=x would be
		// incorrectly simplified to y=0 and z=0.
		const char* sample = "(x = 0) || (y = x || z = x).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		// y=x and z=x stay in some orientation — they must NOT become y=0/z=0
		CHECK(tau::get(res).to_str() == "x = 0 || x = y || z = x");
	}

	TEST_CASE("nested_or_3_distinct_branches_each_simplified") {
		// Three distinct branches: each has its own equality that simplifies
		// a different atom. The equalities must not bleed across branches.
		const char* sample =
			"(x = 0 && y = x) || ((x = 1 && z = x) || (x = 0 && w = x)).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).to_str() ==
			"x = 0 && y = 0 || x' = 0 && z = 1 || x = 0 && w = 0");
	}

	// ── IO variable all_inputs guard ─────────────────────────────────────────────

	// ── IO variable all_inputs guard ─────────────────────────────────────────────
	// The uf_find lambda inside simplify_using_equality_simplify_equation must
	// preserve subterms whose variables are ALL input streams. The time-offset
	// variable 't' inside io_var nodes (e.g. i1[t] → variable{io_var{..., t}})
	// must NOT be treated as a non-input variable — fix uses visit_subtree to
	// skip io_var subtrees so that 't' is never visited by the input_vars check.

	TEST_CASE("io_output_var_replaced_by_input_var") {
		// After establishing o1 ↔ i1, a second occurrence of o1 is substituted
		// with i1. This verifies the output stream IS simplified, and that the
		// input stream i1[t] contained in the result does NOT trigger the bug
		// (i.e. i1[t] is not itself further replaced once it appears as a result).
		const char* s = "o1[t] = i1[t] && o2[t] = o1[t].";
		tref fm = get_nso_rr(s).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).to_str() == "o1[t]:tau = i1[t]:tau && i1[t]:tau = o2[t]:tau");
	}

	TEST_CASE("io_output_var_replaced_when_equality_added_later") {
		// Same as above but the equalities are in reversed order: o2=o1 comes
		// first, then o1=i1 is added to the UF. o1 in the first conjunct is
		// substituted with i2's representative (o2 < o1 by subtree_less).
		const char* s = "o2[t] = o1[t] && o1[t] = i1[t].";
		tref fm = get_nso_rr(s).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).to_str() == "o1[t]:tau = o2[t]:tau && o2[t]:tau = i1[t]:tau");
	}

	TEST_CASE("io_input_var_not_substituted_despite_being_in_uf") {
		// After adding i1[t] = 0 to the UF, uf.find(i1[t]) = 0. Without the
		// all_inputs guard, o1[t] = i1[t] would be simplified to o1[t] = 0.
		// With the guard, i1[t] is preserved because it is an input stream.
		// The bug (before fix) was that the time-offset variable 't' inside
		// i1[t]'s io_var subtree was mistakenly treated as a non-input variable,
		// setting all_inputs = false and allowing the substitution.
		const char* s = "i1[t] = 0 && o1[t] = i1[t].";
		tref fm = get_nso_rr(s).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		// i1[t] must NOT be replaced by 0 in the second conjunct
		CHECK(tau::get(res).to_str() == "i1[t]:tau = 0 && o1[t]:tau = i1[t]:tau");
	}
}
