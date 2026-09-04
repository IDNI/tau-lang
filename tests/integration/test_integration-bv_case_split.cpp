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

tref parse_wff(const std::string& sample) {
	static tree<node_t>::get_options opts{ .parse = { .start = tree<node_t>::wff }};
	auto src = tree<node_t>::get(sample, opts);
	if (src == nullptr) TAU_LOG_ERROR << "Parsing failed for: " << sample;
	return src;
}

tref normalized(const std::string& sample, bool on) {
	split_config c(on);
	auto wff = parse_wff(sample);
	return wff ? normalizer<node_t>(wff) : nullptr;
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
	return tau::get(normalizer<node_t>(tau::build_wff_equiv(off, on))).equals_T();
}

strings run_command_spec(bool on) {
	split_config c(on);
	bdd_init<Bool>();
	// A conditional over a bitvector command with a lookback and an initial
	// condition: the boundary step quantifies the future command universally
	// and the future outputs existentially, which is where the split applies.
	auto spec = create_spec(
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

TEST_SUITE("configuration") {
	TEST_CASE("bdd_init") { bdd_init<Bool>(); }
	TEST_CASE("logging") { logging::trace(); }
}

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

	TEST_CASE("declines on an arithmetic occurrence") {
		CHECK(agree("ex x (x:bv[4] = { 3 }:bv[4] && x:bv[4] + { 5 }:bv[4] = { 8 }:bv[4])"));
		CHECK(norm("ex x (x:bv[4] = { 3 }:bv[4] && x:bv[4] + { 5 }:bv[4] = { 8 }:bv[4])", true) == "T");
	}

	TEST_CASE("a tested set covering the domain leaves no interval cell") {
		CHECK(agree("all x (x:bv[1] = { 0 }:bv[1] || x:bv[1] = { 1 }:bv[1])"));
		CHECK(norm("all x (x:bv[1] = { 0 }:bv[1] || x:bv[1] = { 1 }:bv[1])", true) == "T");
		CHECK(norm("ex x (x:bv[1] != { 0 }:bv[1] && x:bv[1] != { 1 }:bv[1])", true) == "F");
	}

	TEST_CASE("run outputs are unchanged with the split enabled") {
		auto off = run_command_spec(false);
		auto on = run_command_spec(true);
		CHECK(off == on);
	}
}
