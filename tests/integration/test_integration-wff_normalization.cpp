// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

TEST_SUITE("Normalizer") {
	TEST_CASE("1") {
		const char* sample = "all a,b,c,d a'c|b'd = 0 <-> a & b' & d | a' & c | b' & c' & d = 0.";
		CHECK( normalize_and_check(sample, tau::wff_t) );
	}
	TEST_CASE("2") {
		const char* sample = "all x all y ex z (x != y) -> (x < z && z < y).";
		CHECK( normalize_and_check(sample, tau::wff_f) );
	}
	TEST_CASE("3") {
		const char* sample = "all x all y ex z (x < y) -> (x < z && z < y).";
		CHECK( normalize_and_check(sample, tau::wff_t) );
	}
	TEST_CASE("4") {
		const char* sample = "all a all b all c all d all e all f (ax ^ bx' != cy ^ d'y' || ax ^ bx' = ey ^ fy') <-> (ax ^ bx' = ey ^ fy' || ax ^ bx' != cy ^ d'y').";
		CHECK( normalize_and_check(sample, tau::wff_t) );
	}
	TEST_CASE("5") {
		const char* sample = "all a all b all c all d all e all f (ax ^ bx' != cy ^ d'y' || ax ^ bx' = ey ^ fy') <-> (ax ^ bx' = ey ^ fy' || ax ^ bx' = cy ^ d'y').";
		CHECK( normalize_and_check(sample, tau::wff_f) );
	}
	TEST_CASE("6" * doctest::skip()) {
		// REVIEW (HIGH): Bool-type bf_lt variables are not yet supported by
		// resolve_quantifiers/resolve_quantifiers2 — this hangs indefinitely.
		// The formula should normalize to F once comparison handling is added.
		const char* sample = "all x ex y all z ex w all u ex v ((x<y && y<z) || (z<w && w<u)|| (u<v && v<x)).";
		CHECK( normalize_and_check(sample, tau::wff_f) );
	}
	TEST_CASE("7") {
		const char* sample = "ex t [t > 3].";
		CHECK( normalize_and_check(sample, tau::wff_always) );
	}
	TEST_CASE("8") {
		const char* sample = "{ !i5[t] = <:x> || o5[t] = <:y> } : tau = u[0].";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = normalize_non_temp<node_t>(fm);
		CHECK(tau::get(res).to_str() == "u[0]:tau = { always i5[t]:tau != <:x> || o5[t]:tau = <:y> }:tau");
	}
	// Block-driver tests: exercise anti_prenex_block through normalize
	TEST_CASE("ex_block_both_zero") {
		// ∃x ∃y. (x = 0 && y = 0) → T (pick x = 0, y = 0)
		CHECK( normalize_and_check("ex x ex y x = 0 && y = 0.", tau::wff_t) );
	}
	TEST_CASE("ex_block_contradiction") {
		// ∃x. (x = 0 && x != 0) → F (unsatisfiable)
		CHECK( normalize_and_check("ex x x = 0 && x != 0.", tau::wff_f) );
	}
	TEST_CASE("all_ex_witness") {
		// ∀x ∃y. (xy = 0 && x'y = 0) → T (y = 0 is always a witness)
		CHECK( normalize_and_check("all x ex y (xy = 0 && x'y = 0).", tau::wff_t) );
	}
	TEST_CASE("all_complement_tautology") {
		// ∀x. x'x = 0 → T (complement law, tautology)
		CHECK( normalize_and_check("all x x'x = 0.", tau::wff_t) );
	}
	TEST_CASE("all_nonzero_false") {
		// ∀x. x != 0 → F (x = 0 is a counterexample)
		CHECK( normalize_and_check("all x x != 0.", tau::wff_f) );
	}
	TEST_CASE("ex_quantifier_eliminated") {
		// ∃x. x|y = 0 → y = 0 (x is eliminated; result has no ex-quantifier)
		tref fm = get_nso_rr("ex x x|y = 0.").value().main->get();
		tref res = normalize_non_temp<node_t>(fm);
		CHECK(!tau::get(res).find_top(is<node_t, tau::wff_ex>));
	}
}

TEST_SUITE("squeeze_absorb") {
	TEST_CASE("1") {
		const char* sample = "(((xyz = 0 && xw = 0 && f(x)) || w = 0 || xyz != 0) && xy = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = squeeze_absorb<node_t>(fm, tau::build_variable("x", 1));
		CHECK(tau::get(res).to_str() == "xy = 0 && (xyz&(xy)' != 0 || w = 0 || xyz&(xyz&(xy)')'|xw|xy = 0 && f(x))");
	}
	TEST_CASE("2") {
		const char* sample = "(((xy) = 0 || f(x)) && (xy)' = 0) || w = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = squeeze_absorb<node_t>(fm, tau::build_variable("x", 1));
		CHECK(tau::get(res).to_str() == "w = 0 || (xy)' = 0 && f(x)");
	}
	TEST_CASE("3") {
		const char* sample = "(((xy) = 0 || f(x)) && (xy)' = 0) || w = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = squeeze_absorb<node_t>(fm);
		CHECK(tau::get(res).to_str() == "w = 0 || (xy)' = 0 && f(x)");
	}
	TEST_CASE("4") {
		const char* sample = "(((xyz = 0 && xw = 0 && f(x)) || w = 0 || xyz != 0) && xy = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = squeeze_absorb<node_t>(fm);
		CHECK(tau::get(res).to_str() == "xy = 0 && (xyz&(xy)' != 0 || w = 0 || xyz&(xyz&(xy)')'|xw|xy = 0 && f(x))");
	}
	TEST_CASE("5") {
		const char* sample = "xy = 0 && vw = 0 && (yw = 0 && xv = 0 || k = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = squeeze_absorb<node_t>(fm);
		CHECK(tau::get(res).to_str() == "xy = 0 && vw = 0 && (k = 0 || yw = 0 && xv = 0)");
	}
	TEST_CASE("6") {
		const char* sample = "fx != 0 || (g = 0 && fxy = 0 && fxw != 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = squeeze_absorb<node_t>(fm);
		CHECK(tau::get(res).to_str() == "fx != 0 || g = 0 && fxy&(fx)' = 0 && (fxw|fx)&(fxy&(fx)')' != 0");
	}
	TEST_CASE("7") {
		const char* sample = "fx = 0 && (g != 0 || fxy != 0 || fxw = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = squeeze_absorb<node_t>(fm);
		CHECK(tau::get(res).to_str() == "fx = 0 && (g != 0 || fxy&(fx)' != 0 || (fxw|fx)&(fxy&(fx)')' = 0)");
	}
	TEST_CASE("8") {
		const char* sample = "o1[4] = o1[3] && o2[0] != 0 && (o1[3] != o1[1] || o2[2] != 0 || o2[1] = 0) && (o2[1] != 0 || o2[2] = 0) && (o2[2] = 0 || o2[3] = 0 && o1[4] != o1[2]).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = squeeze_absorb<node_t>(fm);
		CHECK(tau::get(res).to_str() == "o2[0]:tau != 0 && o1[4]:tau = o1[3]:tau && (o2[2]:tau != 0 || o2[1]:tau = 0 || o1[3]:tau != o1[1]:tau) && (o2[1]:tau != 0 || o2[2]:tau = 0) && (o2[2]:tau = 0 || o2[3]:tau = 0 && o1[4]:tau != o1[2]:tau)");
	}
}

TEST_SUITE("boole_normal_form") {
	TEST_CASE("1") {
		const char* sample = "ab|ax|bx' != 0 || a = 0 && b = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = boole_normal_form<node_t>(fm);
		CHECK( matches_to_str_to_any_of(res, {
			// verified: valid ((xa'b|x'ab' = 0 || a&(x|b)|x'a'b != 0) <->
			// (ba'x|b'ax' = 0 || b&(a|x')|b'ax != 0)) => T
			"xa'b|x'ab' = 0 || a&(x|b)|x'a'b != 0",
			"ba'x|b'ax' = 0 || b&(a|x')|b'ax != 0",
			"x'b'a|xba' = 0 || b&(x'|a)|xb'a != 0",
			"b'ax'|ba'x = 0 || b&(a|x')|b'ax != 0",
			"b'x'a|bxa' = 0 || b&(x'|a)|b'xa != 0",
			"a'xb|ax'b' = 0 || a&(x|b)|a'x'b != 0",
		}) );
	}
	TEST_CASE("2") {
		const char* sample = "f(0, 0)f(0, 1) = 0 && f(1, 1)f(1, 0) = 0 && f(1, 0)f(1, 1)|f(0, 1)f(0, 0) != 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = boole_normal_form<node_t>(fm);
		CHECK(tau::get(res).equals_F());
	}
	// Inequality reasoning is not supported yet
	// TEST_CASE("3") {
	// 	const char* sample = "(x<y && y<z && x>z).";
	// 	tref fm = get_nso_rr(sample).value().main->get();
	// 	tref res = boole_normal_form<node_t>(fm);
	// 	std::cout << "res: " << tau::get(res) << "\n";
	// 	CHECK(true);
	// }
	TEST_CASE("4") {
		const char* sample = "xy = 0 && (abx' | by'a) != 0 && ab = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = boole_normal_form<node_t>(fm);
		CHECK(tau::get(res).equals_F());
	}
	TEST_CASE("5") {
		const char* sample = "!(xy = 0 && (abx' | by'a) != 0 && ab = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = boole_normal_form<node_t>(fm);
		CHECK(tau::get(res).equals_T());
	}
}

// Helper: returns true if `n` is a wff(wff_always(wff_f)) node — i.e. always(F).
static bool is_always_F(tref n) {
	if (!is_child<node_t>(n, tau::wff_always)) return false;
	return tau::get(tau::trim2(n)).equals_F();
}

TEST_SUITE("NormalizeTemporalQuantifiers") {
	// Bug (fixed): when all DNF clauses are temporal, non_temp_clauses stayed
	// _F() but was unconditionally wrapped in always(_F()) and OR'd into the
	// result, producing a spurious always(F) disjunct.

	// Note: the `wff_always` construction hook automatically simplifies
	// `always(F) = F`, and the `wff_or` hook simplifies `X || F = X`.  These
	// tests therefore catch the bug both at the structural level (no spurious
	// `always(F)` node exists in the result tree) and at the output level
	// (the result string equals the expected canonical form).

	TEST_CASE("purely_temporal_always_no_spurious_always_F") {
		// All clauses of `always o1[t] = 1.` are temporal.
		// Before the fix the code unconditionally appended always(non_temp_clauses)
		// when non_temp_clauses was still _F().  The hooks simplify that to _F()
		// and X||F to X, but the explicit guard added by the fix removes the
		// redundant call entirely, making the intent clear.
		auto nso = get_nso_rr("always o1[t] = 1.");
		REQUIRE(nso.has_value());
		tref fm = nso.value().main->get();
		tref result = normalize_temporal_quantifiers<node_t>(fm);
		REQUIRE(result != nullptr);
		// No spurious always(F) node anywhere in the result tree.
		CHECK(!tau::get(result).find_top(is_always_F));
		// Result must be a single always(...) formula — not a disjunction.
		CHECK(is_child<node_t>(result, tau::wff_always));
	}

	TEST_CASE("purely_temporal_sometimes_no_spurious_always_F") {
		// Same check for a purely-temporal `sometimes` formula.
		// sometimes(P) is canonicalized to !always(!P), so the result has
		// wff_neg at the top (not wff_always directly).
		auto nso = get_nso_rr("sometimes o1[t] = 1.");
		REQUIRE(nso.has_value());
		tref fm = nso.value().main->get();
		tref result = normalize_temporal_quantifiers<node_t>(fm);
		REQUIRE(result != nullptr);
		CHECK(!tau::get(result).find_top(is_always_F));
		// Result must not be false (formula is not trivially unsatisfiable).
		CHECK(!tau::get(result).equals_F());
	}

	TEST_CASE("purely_temporal_always_and_sometimes_no_spurious_always_F") {
		// Conjunction of always and sometimes — all clauses still temporal.
		// Parens required: without them the parser nests sometimes inside always.
		auto nso = get_nso_rr("(always o1[t] = 1) && (sometimes o2[t] = 0).");
		REQUIRE(nso.has_value());
		tref fm = nso.value().main->get();
		tref result = normalize_temporal_quantifiers<node_t>(fm);
		REQUIRE(result != nullptr);
		CHECK(!tau::get(result).find_top(is_always_F));
		// Result contains at least one always(...) subformula.
		CHECK(tau::get(result).find_top(is_child<node_t, tau::wff_always>));
	}

	// NF-22: a genuinely MIXED top-level disjunction — one disjunct is
	// already temporal (wrapped in `always`), the other has no temporal
	// variable at all. Unlike the purely-temporal cases above (where
	// non_temp_clauses in normalize_temporal_quantifiers stays _F() the
	// whole time), this actually populates non_temp_clauses alongside a
	// real temporal clause, which no other test exercises. Regression
	// target: the non-temporal disjunct must survive (wrapped in its own
	// `always(...)`, per normalizer.tmpl.h's non_temp_clauses handling)
	// instead of being silently dropped, and the temporal disjunct must
	// survive alongside it.
	//
	// A bare non-temporal top-level part (e.g. "x = 0") is rejected by the
	// parser's own "must be scoped by a temporal quantifier" check, so the
	// formula is built directly with the tau:: builders instead of parsing
	// it as a whole spec.
	TEST_CASE("mixed_temporal_and_non_temporal_disjunct_preserves_both_branches") {
		auto nso = get_nso_rr("always o1[t] = 1.");
		REQUIRE(nso.has_value());
		tref temporal_clause = nso.value().main->get();

		tref x = build_bf_variable<node_t>("x", tau_type_id<node_t>());
		tref non_temporal_clause = tau::build_bf_eq(x, tau::_0(tau_type_id<node_t>()));
		REQUIRE(!has_temp_var<node_t>(non_temporal_clause));

		tref fm = tau::build_wff_or(temporal_clause, non_temporal_clause);
		REQUIRE(has_temp_var<node_t>(fm));

		tref result = normalize_temporal_quantifiers<node_t>(fm);
		REQUIRE(result != nullptr);

		// No spurious always(F) node.
		CHECK(!tau::get(result).find_top(is_always_F));

		// Both branches must survive as separate always(...) disjuncts:
		// one carrying the temporal (o1) content, the other the
		// non-temporal (x) content wrapped in `always` by the
		// non_temp_clauses accumulation.
		trefs disjuncts = get_dnf_wff_clauses<node_t>(result);
		bool has_temporal_disjunct = false, has_wrapped_non_temporal_disjunct = false;
		for (tref d : disjuncts) {
			REQUIRE(is_child<node_t>(d, tau::wff_always));
			const std::string inner = tau::get(tau::trim2(d)).to_str();
			if (inner.find("o1") != std::string::npos) has_temporal_disjunct = true;
			if (inner.find("x") != std::string::npos)
				has_wrapped_non_temporal_disjunct = true;
		}
		CHECK(has_temporal_disjunct);
		CHECK(has_wrapped_non_temporal_disjunct);
	}
}

// Bitvector content sharing a formula with a foreign-typed (sbf) conjunct.
//
// A foreign-typed sibling conjunct inside a bv quantifier's scope makes the
// whole scope fail is_bv_solvable_formula, which used to divert it away from
// the solver and into blasting plus generic Boole decomposition -- hundreds of
// bv-typed atoms whose BDD leaves are backed by solver terms. Every case below
// ran for minutes (or never finished) before scope_out_independent_conjuncts
// lifted such conjuncts out; they now complete in milliseconds. They are
// therefore hang regressions first and value checks second: a regression shows
// up as the suite timing out, not as a wrong answer.
TEST_SUITE("Normalizer bv mixed-type") {

	// Every arithmetic operator, since each blasts to its own per-bit
	// recurrence and only the solver-first path avoids blasting them at all.
	TEST_CASE("bv_arith_with_sbf_conjunct") {
		for (const char* op : { "+", "-", "*", "/", "%", "<<", ">>",
					"!&", "!|", "!^" }) {
			const std::string sample = std::string(
				"ex x ex y (x:bv[8] ") + op
				+ " y:bv[8] = { 0 }:bv[8]) && s = 0.";
			CAPTURE(op);
			CHECK( normalize_and_check(sample.c_str(), "s = 0") );
		}
	}

	// Blasting cost grows with the bitwidth, so the widths are what turned a
	// slow case into an unbounded one; the solver is insensitive to them.
	TEST_CASE("bv_arith_with_sbf_conjunct_widths") {
		for (const char* w : { "8", "16", "32", "64" }) {
			const std::string ty = std::string(":bv[") + w + "]";
			const std::string sample = "ex x ex y (x" + ty + " + y"
				+ ty + " = { 0 }" + ty + ") && s = 0.";
			CAPTURE(w);
			CHECK( normalize_and_check(sample.c_str(), "s = 0") );
		}
	}

	// The lift must not change what the formula means: an unsatisfiable
	// bitvector part still forces F, and a conjunct that genuinely depends on
	// the bound variable must stay inside the scope.
	TEST_CASE("bv_arith_with_sbf_conjunct_unsat") {
		CHECK( normalize_and_check("ex x (x:bv[4] = { 3 }:bv[4] && x:bv[4]"
			" + { 5 }:bv[4] = { 9 }:bv[4]) && s = 0.", tau::wff_f) );
	}

	TEST_CASE("bv_arith_dependent_conjunct_kept_sat") {
		CHECK( normalize_and_check("ex x (x:bv[4] + { 1 }:bv[4] ="
			" { 2 }:bv[4] && x:bv[4] = { 1 }:bv[4]).", tau::wff_t) );
	}

	TEST_CASE("bv_arith_dependent_conjunct_kept_unsat") {
		CHECK( normalize_and_check("ex x (x:bv[4] + { 1 }:bv[4] ="
			" { 2 }:bv[4] && x:bv[4] = { 5 }:bv[4]).", tau::wff_f) );
	}

	// Universal blocks dualize to existential ones, so they take the same
	// path and need the same coverage.
	TEST_CASE("bv_arith_under_all_with_sbf_conjunct") {
		CHECK( normalize_and_check("all x (x:bv[8] + { 0 }:bv[8] ="
			" x:bv[8] && s = 0).", "s = 0") );
		CHECK( normalize_and_check("all x (x:bv[8] + { 1 }:bv[8] ="
			" x:bv[8] && s = 0).", tau::wff_f) );
	}

	// Several independent conjuncts must all be lifted, not just the first.
	TEST_CASE("bv_arith_with_two_sbf_conjuncts") {
		// verified: valid ((r = 0 && s = 0) <-> (s = 0 && r = 0)) => T
		CHECK( normalize_and_check("ex x ex y (x:bv[8] + y:bv[8] ="
			" { 0 }:bv[8]) && s = 0 && r = 0.",
			strings{"r = 0 && s = 0", "s = 0 && r = 0"}) );
	}

	// A scope left open by a free bitvector variable cannot be closed by
	// lifting, so it used to be blasted -- one auxiliary bit variable per
	// bit, eliminated pairwise, i.e. exponential in the bitwidth (bv[8] took
	// ~24s, bv[16] and up never finished). resolve_quantifiers now closes the
	// free variables both ways and asks the solver instead: `ex x (x + y = 0)`
	// is valid, so the whole scope collapses and only `s = 0` is left.
	TEST_CASE("bv_arith_open_scope_with_sbf_conjunct") {
		for (const char* w : { "8", "16", "32", "64" }) {
			const std::string ty = std::string(":bv[") + w + "]";
			const std::string sample = "ex x (x" + ty + " + y" + ty
				+ " = { 0 }" + ty + ") && s = 0.";
			CAPTURE(w);
			CHECK( normalize_and_check(sample.c_str(), "s = 0") );
		}
	}

	// Free bitvector variables with no quantifier at all must stay put: there
	// is nothing to decide, so the arithmetic atom survives verbatim.
	TEST_CASE("bv_arith_all_free_with_sbf_conjunct") {
		CHECK( normalize_and_check("x:bv[8] + y:bv[8] = { 0 }:bv[8]"
			" && s = 0.", strings{ "x+y = 0 && s = 0",
					       "s = 0 && x+y = 0" }) );
	}

	// Interleaved all/ex over bv comparison chains mixed with an sbf
	// conjunct: explosive before the lift even though no arithmetic is
	// involved. Depths 2/4/6 to catch a reintroduced dependence on
	// alternation depth.
	TEST_CASE("bv_alternating_quantifier_chain_with_sbf_conjunct") {
		CHECK( normalize_and_check("all x ex y (x:bv[8] < y:bv[8])"
			" && s = 0.", tau::wff_f) );
		CHECK( normalize_and_check("all a ex b all c ex d (a:bv[8] <"
			" b:bv[8] && b:bv[8] < c:bv[8] && c:bv[8] < d:bv[8])"
			" && s = 0.", tau::wff_f) );
		CHECK( normalize_and_check("all a ex b all c ex d all e ex f"
			" (a:bv[8] < b:bv[8] && b:bv[8] < c:bv[8] && c:bv[8] <"
			" d:bv[8] && d:bv[8] < e:bv[8] && e:bv[8] < f:bv[8])"
			" && s = 0.", tau::wff_f) );
	}
}

// Purely bitvector formulas whose quantification alternates, with no type
// mixing and no arithmetic. These reach cvc5 directly, and cvc5's default
// counterexample-guided instantiation only instantiates the innermost
// quantified subformula, which collapses on exactly this shape: the 4-level
// disjunctive chain over bv[8] took ~13s and the wider and deeper variants
// never finished. bv_formula_sat_status now turns `cegqi-innermost` off when
// the quantification alternates, which decides all of them in well under a
// second. Hang regressions first, value checks second.
TEST_SUITE("Normalizer bv alternating quantifiers") {

	// The Bernays-Schoenfinkel shape from tests/benchmark (a disjunction of
	// conjoined comparison steps closing a cycle), across bitwidths, since
	// the previous behaviour degraded sharply with width.
	TEST_CASE("bv_alternating_disjunctive_chain_widths") {
		for (const char* w : { "8", "16", "32", "64" }) {
			const std::string t = std::string(":bv[") + w + "]";
			const std::string sample =
				"all x ex y all z ex w ((x" + t + " < y" + t
				+ " && y" + t + " < z" + t + ") || (z" + t
				+ " < w" + t + " && w" + t + " < x" + t + ")).";
			CAPTURE(w);
			CHECK( normalize_and_check(sample.c_str(), tau::wff_f) );
		}
	}

	// Six alternating quantifiers: the variant tests/benchmark records as OOM
	// and TEST_CASE("6") above still skips for the atomless case.
	TEST_CASE("bv_alternating_disjunctive_chain_six_levels") {
		CHECK( normalize_and_check(
			"all x ex y all z ex w all u ex v ((x:bv[8] < y:bv[8] &&"
			" y:bv[8] < z:bv[8]) || (z:bv[8] < w:bv[8] && w:bv[8] <"
			" u:bv[8]) || (u:bv[8] < v:bv[8] && v:bv[8] < x:bv[8])).",
			tau::wff_f) );
	}

	// Both polarities, and both quantifier orders, so a strategy change that
	// only ever answered F would not pass.
	TEST_CASE("bv_alternating_quantifier_polarities") {
		CHECK( normalize_and_check("all x ex y (x:bv[8] <= y:bv[8]).",
			tau::wff_t) );
		CHECK( normalize_and_check("ex x all y (x:bv[8] <= y:bv[8]).",
			tau::wff_t) );
		CHECK( normalize_and_check("all x ex y (x:bv[8] != y:bv[8]).",
			tau::wff_t) );
		CHECK( normalize_and_check("all x ex y (x:bv[8] < y:bv[8]).",
			tau::wff_f) );
		CHECK( normalize_and_check("all x ex y (x:bv[8] < y:bv[8] ||"
			" x:bv[8] = { 255 }:bv[8]).", tau::wff_t) );
	}

	// Quantified arithmetic keeps the default strategy's strengths: turning
	// cegqi-innermost off costs it at most ~2x, and these are the cases that
	// would suffer most if a future change reached for a strategy (such as
	// cegqi-nested-qe) that cannot handle them at all.
	TEST_CASE("bv_alternating_quantifier_arithmetic_unaffected") {
		CHECK( normalize_and_check("all x ex y (x:bv[32] * y:bv[32] ="
			" { 1 }:bv[32]).", tau::wff_f) );
		CHECK( normalize_and_check("all x ex y (x:bv[32] / y:bv[32] ="
			" { 2 }:bv[32]).", tau::wff_f) );
		CHECK( normalize_and_check("all x ex y (x:bv[64] + y:bv[64] ="
			" { 0 }:bv[64]).", tau::wff_t) );
		CHECK( normalize_and_check("all x (x:bv[32] % { 4 }:bv[32] <"
			" { 4 }:bv[32]).", tau::wff_t) );
	}
}

// Coverage for the normalization behaviours the 2026-07-30 review found
// unpinned (report items TC-3, NZ-6 and the resolve_quantifiers open-scope
// branch listed in section 5.8).
TEST_SUITE("Normalizer bv undecidable and scoping") {

	// TC-3. The 21 bv test cases above all pin *decidable* formulas. This one
	// pins what happens when the closed bv scope cannot be decided at all: the
	// unresolved wff_ref is outside the cvc5 translator's reach, so the solver
	// shortcut does not apply and the quantifier must survive untouched rather
	// than collapsing to F. Normalization must also not abort.
	//
	// The route to that outcome changed with SO-2: `is_bv_solvable_formula` now
	// rejects the formula up front (it used to inspect only variable nodes, so
	// the ref's bv-typed arguments made the whole formula look solvable and the
	// failure surfaced as a "Failed to translate" log from inside the solver,
	// four times per normalization). The observable behaviour pinned here is
	// deliberately the same either way.
	TEST_CASE("undecidable closed bv scope keeps its quantifier") {
		tref fm = get_nso_rr("ex y ex x (x:bv[8] * y:bv[8] ="
			" { 1 }:bv[8] && q(x)).").value().main->get();
		REQUIRE( fm != nullptr );
		tref res = normalize_non_temp<node_t>(fm);
		REQUIRE( res != nullptr );
		CHECK( !tau::get(res).equals_F() );
		CHECK( !tau::get(res).equals_T() );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) != nullptr );
	}

	// `has_foreign_ba_constant` is what tells eliminate_bv_and_quantifiers
	// "cvc5 cannot read this formula at all" apart from "the solver owns this
	// bv content", and the second anti-prenex pass only skips bv-typed
	// content in the latter case. Getting this wrong is not a slowdown but a
	// wrong answer: see "nested conditionals over mixed tau/bv streams stay
	// sat" in test_integration-interpreter.cpp, where a skipped-and-therefore
	// unresolvable `all i2[1]:bv[8]` block made a satisfiable run report
	// "Tau specification is unexpectedly unsat".
	TEST_CASE("a foreign-algebra constant is detected") {
		// The `:tau` constant is kept in the tree by the `:tau` variable it
		// is equated with; a closed Tau atom would be decided at build time
		// and the constant would vanish before this could see it.
		tref mixed = get_nso_rr("y:tau = { o1[t] = 1 }:tau"
			" && x:bv[8] = { 1 }:bv[8].").value().main->get();
		REQUIRE( mixed != nullptr );
		REQUIRE( tau::get(mixed).find_top(
			is<node_t, tau::ba_constant>) != nullptr );
		CHECK( has_foreign_ba_constant<node_t>(mixed) );

		// bv constants are exactly what cvc5 reads, so a bv-only formula is
		// not foreign content -- and neither is untyped T/F, which carries no
		// algebra of its own.
		tref pure = get_nso_rr("x:bv[8] = { 1 }:bv[8]"
			" && y:bv[8] != { 0 }:bv[8].").value().main->get();
		REQUIRE( pure != nullptr );
		CHECK( !has_foreign_ba_constant<node_t>(pure) );
	}

	// NZ-6: scope_out_independent_conjuncts drops a binder whose scope never
	// mentions it, which is the branch a skip-matched (bv) block would
	// otherwise carry through the pipeline verbatim.
	TEST_CASE("a vacuous binder is dropped") {
		tref fm = get_nso_rr("ex x (y = 0).").value().main->get();
		tref res = scope_out_independent_conjuncts<node_t>(fm);
		REQUIRE( res != nullptr );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) == nullptr );
		CHECK( are_nso_equivalent<node_t>(res, fm) );
	}

	// AP-7. Every neighbouring pass documents the term guard as load-bearing:
	// select_innermost_blocks passes `until = !while_is_formula` with the note
	// "Do not descend into terms: tau_ba sub-trees carry their own
	// wff_ex/wff_all over I/O variables, which this pass must leave intact",
	// and anti_prenex_block uses find_top_until for the same reason.
	// scope_out_independent_conjuncts traversed with no visit_subtree at all,
	// so is_child_quantifier matched those internal binders and both the
	// vacuous-binder drop and the conjunct lift rebuilt the enclosing term --
	// which additionally loses inference-assigned bitwidths on bv-containing
	// scopes.
	TEST_CASE("a tau_ba constant's internal binders are left intact") {
		const char* sample = "{ !i5[t] = <:x> || o5[t] = <:y> } : tau = u[0].";
		tref fm = get_nso_rr(sample).value().main->get();
		REQUIRE( fm != nullptr );
		// Give the constant its internal `always` binder, the shape the
		// normalizer actually produces for it (see TEST_CASE("8")).
		tref with_binder = normalize_non_temp<node_t>(fm);
		REQUIRE( with_binder != nullptr );
		const std::string before = tau::get(with_binder).to_str();
		tref res = scope_out_independent_conjuncts<node_t>(with_binder);
		REQUIRE( res != nullptr );
		CHECK( tau::get(res).to_str() == before );
	}

	TEST_CASE("a vacuous universal binder is dropped too") {
		tref fm = get_nso_rr("all x (y = 0).").value().main->get();
		tref res = scope_out_independent_conjuncts<node_t>(fm);
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) == nullptr );
		CHECK( are_nso_equivalent<node_t>(res, fm) );
	}

	TEST_CASE("an independent conjunct is lifted, a dependent one is not") {
		tref fm = get_nso_rr("ex x (x y = 0 && z = 0).")
			.value().main->get();
		tref res = scope_out_independent_conjuncts<node_t>(fm);
		REQUIRE( res != nullptr );
		CHECK( are_nso_equivalent<node_t>(res, fm) );
		// z = 0 is out of the scope, so exactly one binder survives and the
		// lifted conjunct is a top-level conjunct.
		trefs conjs = get_cnf_wff_clauses<node_t>(res);
		CHECK( conjs.size() == 2 );
	}

	TEST_CASE("nothing is lifted when every conjunct depends on the binder") {
		tref fm = get_nso_rr("ex x (x y = 0 && x z = 0).")
			.value().main->get();
		CHECK( scope_out_independent_conjuncts<node_t>(fm) == fm );
	}

	// resolve_quantifiers' open-scope branch: `ex x (x + y = 0)` is valid for
	// every y, so closing the free variable universally lets the solver settle
	// it (the T direction). The F direction -- `ex Y n` unsat -- had no test.
	TEST_CASE("open bv scope: the valid direction collapses to T") {
		CHECK( normalize_and_check("ex x (x:bv[8] + y:bv[8] ="
			" { 0 }:bv[8]).", tau::wff_t) );
	}

	TEST_CASE("open bv scope: the unsatisfiable direction collapses to F") {
		// x < x is false for every x and every y, so `ex Y n` is unsat.
		CHECK( normalize_and_check("ex x (x:bv[8] < x:bv[8] && y:bv[8] <"
			" y:bv[8]).", tau::wff_f) );
	}
}
