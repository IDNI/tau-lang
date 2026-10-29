// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Test-point elimination of quantified bitvector variables compared only
// against constants (normalizer.tmpl.h, opt-in). The split is an identity on the
// formula, so normalization with the split enabled must agree with the
// pipeline without it -- both where the split applies and where it must
// decline (arithmetic occurrence; a tested set covering the whole domain).

#include "test_integration-interpreter_helper.h"

namespace {

struct split_config {
	split_config(bool on) { bv_case_split = on; }
	~split_config() { bv_case_split = false; }
};

tref normalized(const std::string& sample, bool on) {
	split_config c(on);
	auto wff = parse_wff_or_fail(sample);
	if (!wff) return nullptr;
	auto r = normalizer<node_t>(wff);
	return r.has_value() ? r.value() : nullptr;
}

std::string norm(const std::string& sample, bool on) {
	tref r = normalized(sample, on);
	return r ? tau::get(r).to_str() : "parse_error";
}

// Same verdict with and without the split: equal normal forms, or -- when the
// two pipelines pick different but equivalent spellings -- an equivalence of
// the two result trees that normalizes to T.
bool agree(const std::string& sample) {
	tref off = normalized(sample, false), on = normalized(sample, true);
	if (!off || !on) return false;
	if (tau::get(off) == tau::get(on)) return true;
	split_config c(false);
	auto eq_r = normalizer<node_t>(tau::build_wff_equiv(off, on));
	return eq_r.has_value() && tau::get(eq_r.value()).equals_T();
}

strings run_command_spec(bool on) {
	split_config c(on);
	// A conditional over a bitvector command with a lookback and an initial
	// condition: the boundary step quantifies the future command universally
	// and the future outputs existentially, which is where the split applies.
	auto spec = spec_or_fail(
		"(o1[0] = 0) && (i1[t] = { 1 }:bv[2] ? o1[t] = o1[t-1] : o1[t] = o1[t-1]').");
	io_context<node_t> ctx;
	strings i1_values = { "1", "2", "1", "0" };
	ctx.add_input("i1", bv_type_id<node_t>(2),
		std::make_shared<vector_input_stream>(i1_values));
	auto o1 = std::make_shared<vector_output_stream>();
	ctx.add_output("o1", tau_type_id<node_t>(), o1);
	auto maybe_i = run<node_t>(spec, ctx, 4);
	REQUIRE(maybe_i.has_value());
	return o1->get_values();
}

} // namespace


TEST_SUITE("bv case split") {

	TEST_CASE("existential over a tested command") {
		CHECK(agree("ex x ((x:bv[8] = { 1 }:bv[8] && a = 0) || (x:bv[8] != { 1 }:bv[8] && b = 0))"));
		CHECK(norm("ex x ((x:bv[8] = { 1 }:bv[8] && a = 0) || (x:bv[8] != { 1 }:bv[8] && b = 0))", true)
			== norm("a = 0 || b = 0", true));
	}

	TEST_CASE("universal over a tested command") {
		CHECK(agree("all x (x:bv[8] = { 1 }:bv[8] || a = 0)"));
		CHECK(norm("all x (x:bv[8] = { 1 }:bv[8] || a = 0)", true) == norm("a = 0", true));
	}

	TEST_CASE("two tested constants, both polarities") {
		CHECK(agree("ex x ((x:bv[8] = { 1 }:bv[8] || a = 0) && (x:bv[8] = { 2 }:bv[8] || b = 0) && (x:bv[8] != { 1 }:bv[8] || c = 0))"));
	}

	TEST_CASE("order tests cut the domain into cells") {
		// x must be 3: the interval (2, 4) has exactly one value, so the
		// implication fires and the formula is `a = 0`. (Not compared against
		// the pipeline without the switch here: on this shape -- a negated
		// equality next to order atoms that pin the variable -- it currently
		// normalizes to T, dropping the consequent; reported separately.)
		CHECK(norm("ex x (x:bv[4] > { 2 }:bv[4] && x:bv[4] < { 4 }:bv[4] && (x:bv[4] = { 3 }:bv[4] -> a = 0))", true)
			== norm("a = 0", true));
		CHECK(agree("ex x (x:bv[4] > { 2 }:bv[4] && x:bv[4] < { 4 }:bv[4] && a = 0)"));
		CHECK(norm("all x (x:bv[4] < { 3 }:bv[4] || x:bv[4] >= { 3 }:bv[4])", true) == "T");
		CHECK(norm("ex x (x:bv[4] < { 2 }:bv[4] && x:bv[4] > { 5 }:bv[4])", true) == "F");
		CHECK(norm("ex x (x:bv[4] <= { 2 }:bv[4] && x:bv[4] != { 0 }:bv[4] && x:bv[4] != { 1 }:bv[4] && x:bv[4] != { 2 }:bv[4])", true) == "F");
		// Boundary cells at both ends of the domain.
		CHECK(norm("all x (x:bv[2] = { 0 }:bv[2] || x:bv[2] > { 0 }:bv[2])", true) == "T");
		CHECK(norm("ex x (x:bv[2] > { 3 }:bv[2])", true) == "F");
	}

	TEST_CASE("independent parts are not multiplied") {
		// The conjunct about y does not mention x and must come out once.
		CHECK(agree("ex x (x:bv[4] = { 1 }:bv[4] && y:bv[4] + { 1 }:bv[4] = { 3 }:bv[4])"));
		CHECK(norm("ex x ((x:bv[4] = { 1 }:bv[4] || a = 0) && b = 0)", true)
			== norm("b = 0", true));
		CHECK(norm("all x ((x:bv[4] = { 1 }:bv[4] && a = 0) || b = 0)", true)
			== norm("b = 0", true));
	}

	TEST_CASE("declines on a comparison with a non-constant") {
		// A symbolic bound is left to the pipeline: same verdict either way.
		CHECK(agree("ex x (x:bv[4] = y:bv[4] && (x:bv[4] = { 1 }:bv[4] -> a = 0))"));
		CHECK(norm("all x (x:bv[4] = { 2 }:bv[4] || x:bv[4] != y:bv[4] || a = 0)", true)
			== norm("all x (x:bv[4] = { 2 }:bv[4] || x:bv[4] != y:bv[4] || a = 0)", false));
	}

	TEST_CASE("declines on a nested binder of the same variable") {
		// The inner x must not be substituted with the outer one.
		CHECK(agree("ex x (x:bv[4] = { 1 }:bv[4] && all x:bv[4] (x:bv[4] = { 1 }:bv[4] || a = 0))"));
		CHECK(norm("ex x (x:bv[4] = { 1 }:bv[4] && all x:bv[4] (x:bv[4] = { 1 }:bv[4] || a = 0))", true)
			== norm("a = 0", true));
	}

	// A binder of another variable of the same kind between the tested
	// variable and its occurrences: the split continues through it and
	// instantiates only the conjuncts that mention the tested variable,
	// instead of taking one copy of the whole scope per cell (GitHub #122).
	TEST_CASE("descends through a same-kind binder of another variable") {
		CHECK(agree("ex x ex y ((x:bv[4] = { 1 }:bv[4] || a = 0) && y:bv[4] = x:bv[4] && (y:bv[4] = { 1 }:bv[4] || b = 0))"));
		CHECK(agree("all x all y ((x:bv[4] = { 1 }:bv[4] && a = 0) || y:bv[4] != x:bv[4] || b = 0)"));
		// Two blocks, each testing its own command against constants, the
		// second reading the first's output.
		CHECK(agree("all i all j ex x ex y ("
			"(i:bv[1] = { 0 }:bv[1] -> x:bv[4] = { 2 }:bv[4]) && (i:bv[1] != { 0 }:bv[1] -> x:bv[4] = { 3 }:bv[4])"
			" && (j:bv[1] = { 0 }:bv[1] -> y:bv[4] = x:bv[4]) && (j:bv[1] != { 0 }:bv[1] -> y:bv[4] = { 4 }:bv[4])"
			" && (y:bv[4] = { 3 }:bv[4] -> a = 0))"));
		CHECK(norm("ex x ex y ((x:bv[4] = { 1 }:bv[4] || a = 0) && y:bv[4] = x:bv[4] && (y:bv[4] != { 1 }:bv[4] || b = 0))", true)
			== norm("a = 0 || b = 0", true));
	}

	TEST_CASE("does not descend through a binder of the other kind") {
		CHECK(agree("ex x all y ((x:bv[4] = { 1 }:bv[4] || a = 0) && (y:bv[4] = x:bv[4] || b = 0))"));
		CHECK(agree("all x ex y ((x:bv[4] = { 1 }:bv[4] && a = 0) || (y:bv[4] = x:bv[4] && b = 0))"));
	}

	// The descent instantiates only the dependent conjuncts: the instance
	// of a scope with a foreign same-kind binder carries that binder once,
	// not once per cell.
	TEST_CASE("the instance carries a foreign binder once") {
		split_config on(true);
		// y is compared with z, so its binder is not split and sits between
		// x and the conjuncts reading x; x is tested against two constants,
		// three cells.
		const char* s = "ex x ex y ((x:bv[4] = { 1 }:bv[4] || a = 0) && (x:bv[4] = { 3 }:bv[4] || y:bv[4] = z:bv[4]) && (y:bv[4] = { 2 }:bv[4] || c = 0))";
		tref fm = parse_wff_or_fail(s);
		REQUIRE( fm != nullptr );
		tref split = bv_case_split_quantifiers<node_t>(fm);
		REQUIRE( split != nullptr );
		CHECK( tau::get(split).find_top(is<node_t, tau::wff_ex>) != nullptr );
		CHECK( tau::get(split).select_all(is<node_t, tau::wff_ex>).size() == 1 );
		CHECK( agree(s) );
	}

	// The same subterm at several positions: the occurrence scan visits each
	// node once and the result is the one of the tree walk.
	TEST_CASE("shared subterms in the occurrence scan") {
		CHECK(agree("ex x ((x:bv[4] | y:bv[4] = { 3 }:bv[4] || x:bv[4] = { 1 }:bv[4])"
			" && (x:bv[4] | y:bv[4] = { 3 }:bv[4] || x:bv[4] = { 2 }:bv[4])"
			" && (x:bv[4] | y:bv[4] != { 3 }:bv[4] || a = 0))"));
	}

	TEST_CASE("declines on an arithmetic occurrence") {
		CHECK(agree("ex x (x:bv[4] = { 3 }:bv[4] && x:bv[4] + { 5 }:bv[4] = { 8 }:bv[4])"));
		CHECK(norm("ex x (x:bv[4] = { 3 }:bv[4] && x:bv[4] + { 5 }:bv[4] = { 8 }:bv[4])", true) == "T");
	}

	TEST_CASE("a tested set covering the domain leaves no interval cell") {
		CHECK(agree("all x (x:bv[1] = { 0 }:bv[1] || x:bv[1] = { 1 }:bv[1])"));
		CHECK(norm("all x (x:bv[1] = { 0 }:bv[1] || x:bv[1] = { 1 }:bv[1])", true) == "T");
		CHECK(norm("ex x (x:bv[1] != { 0 }:bv[1] && x:bv[1] != { 1 }:bv[1])", true) == "F");
	}

	// The runtime cap on tested constants declines the split above it: the
	// binder survives at cap 2 for a variable tested against three constants
	// and is eliminated when the cap is lifted.
	TEST_CASE("the cap on tested constants declines the split") {
		split_config on(true);
		const std::string fm = "ex x:bv[4] (x:bv[4] = { 1 }:bv[4]"
			" || x:bv[4] = { 2 }:bv[4] || x:bv[4] = { 3 }:bv[4])";
		const size_t saved = bv_case_split_max_tests;
		bv_case_split_max_tests = 2;
		tref declined = bv_case_split_quantifiers<node_t>(parse_wff_or_fail(fm));
		bv_case_split_max_tests = std::numeric_limits<size_t>::max();
		tref split = bv_case_split_quantifiers<node_t>(parse_wff_or_fail(fm));
		bv_case_split_max_tests = saved;
		REQUIRE( declined != nullptr );
		REQUIRE( split != nullptr );
		CHECK( tau::get(declined).find_top(is<node_t, tau::wff_ex>) != nullptr );
		CHECK( tau::get(split).find_top(is<node_t, tau::wff_ex>) == nullptr );
	}

	// Normalization leaves the typed `0` and `1` in place of the constants of
	// all clear and all set bits; they are tested values like any constant,
	// and `0` and `{ 0 }` are one cell.
	TEST_CASE("the typed 0 and 1 are tested values") {
		CHECK(agree("ex x (x:bv[4] = 0 && a = 0 || x:bv[4] = { 1 }:bv[4] && b = 0)"));
		CHECK(agree("all x (x:bv[4] != 1 || x:bv[4] = { 0 }:bv[4] || x:bv[4] = 0 || a = 0)"));
		split_config on(true);
		tref split = bv_case_split_quantifiers<node_t>(parse_wff_or_fail(
			"ex x (x:bv[4] = 0 && a = 0 || x:bv[4] = 1 && b = 0)"));
		REQUIRE( split != nullptr );
		CHECK( tau::get(split).find_top(is<node_t, tau::wff_ex>) == nullptr );
	}

	// Binders sharing one scope multiply each other's instances (GitHub
	// #203): the growth cap leaves the binders past it to the pipeline.
	TEST_CASE("the growth cap declines the binders past it") {
		split_config on(true);
		const std::string fm = "all x:bv[4] all y:bv[4] ("
			"(x:bv[4] = { 1 }:bv[4] && y:bv[4] = { 2 }:bv[4])"
			" || (x:bv[4] = { 3 }:bv[4] && y:bv[4] = { 4 }:bv[4]))";
		const size_t saved = bv_case_split_max_growth;
		bv_case_split_max_growth = 1;
		tref declined = bv_case_split_quantifiers<node_t>(parse_wff_or_fail(fm.c_str()));
		bv_case_split_max_growth = std::numeric_limits<size_t>::max();
		tref split = bv_case_split_quantifiers<node_t>(parse_wff_or_fail(fm.c_str()));
		bv_case_split_max_growth = saved;
		REQUIRE( declined != nullptr );
		REQUIRE( split != nullptr );
		const size_t kept = tau::get(declined)
			.select_all(is<node_t, tau::wff_all>).size();
		CHECK( kept == 2 );
		CHECK( tau::get(split).select_all(is<node_t, tau::wff_all>).size()
			< kept );
	}

	// A connective chain longer than the stack holds frames: the scope is
	// flattened without recursion (GitHub #205).
	TEST_CASE("a long connective chain in the scope") {
		split_config on(true);
		tref q = parse_wff_or_fail("ex x ((x:bv[4] = { 1 }:bv[4] || a = 0) && b = 0)");
		REQUIRE( q != nullptr );
		const tau& tq = tau::get(q);
		tref x = tq[0].first(), chain = tq[0].second();
		tref ind = tau::get(chain)[0].second();
		for (size_t i = 0; i < 1000000; ++i)
			chain = tau::build_wff_and(chain, ind);
		tref split = bv_case_split_quantifiers<node_t>(tau::build_wff_ex(x, chain, false));
		REQUIRE( split != nullptr );
		CHECK( tau::get(split).find_top(is<node_t, tau::wff_ex>) == nullptr );
	}

	TEST_CASE("run outputs are unchanged with the split enabled") {
		auto off = run_command_spec(false);
		auto on = run_command_spec(true);
		CHECK(off == on);
	}
}
