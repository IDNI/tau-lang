// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// The continuation of a specification of functional shape kept as the
// conjunction of its conjuncts' normal forms (satisfiability.h,
// `factorized_continuation`). The run of a specification must not depend
// on the switch: same outputs with the continuation normalized as one
// formula, kept factorized, and under the shadow mode, whose counters show
// that the two forms are equivalent wherever the shape applies.

#include "test_integration-interpreter_helper.h"

namespace {

// `factorized_continuation_mode` lets the environment variable
// TAU_FACTORIZED_CONTINUATION override the flag; run with both variables
// unset. The functional shape (`functional_continuation`) stays at 1.
struct factorized_mode {
	int saved = factorized_continuation;
	int saved_functional = functional_continuation;
	size_t saved_cap = max_flag_search_steps;
	explicit factorized_mode(int mode) {
		// Every run builds its continuation anew: the switch is part of
		// the verdict budget fingerprint, and the flag-search cap, which
		// none of these specifications reaches, is set to a value no
		// earlier run had.
		static size_t runs = 0;
		factorized_continuation = mode;
		functional_continuation = 1;
		max_flag_search_steps = saved_cap + ++runs;
		factorized_continuation_hits = 0;
		factorized_continuation_warmups = 0;
		factorized_continuation_mismatches = 0;
		factorized_continuation_undecided = 0;
	}
	~factorized_mode() {
		factorized_continuation = saved;
		functional_continuation = saved_functional;
		max_flag_search_steps = saved_cap;
	}
};

// Four steps of a specification reading `i1` and writing `o2`, both bv[8].
strings run_spec(const std::string& spec_src, int mode) {
	factorized_mode m(mode);
	bdd_init<Bool>();
	auto spec = create_spec(spec_src.c_str());
	io_context<node_t> ctx;
	strings i1_values = { "1", "5", "2", "7" };
	ctx.add_input("i1", bv_type_id<node_t>(8),
		std::make_shared<vector_input_stream>(i1_values));
	auto o2 = std::make_shared<vector_output_stream>();
	ctx.add_output("o2", bv_type_id<node_t>(8), o2);
	auto maybe_i = run<node_t>(spec, ctx, 4);
	REQUIRE(maybe_i.has_value());
	return o2->get_values();
}

struct outcome {
	strings one, factorized, shadow;
	size_t kept, warmups, mismatches, undecided;
};

// One specification through the three modes; the switch is part of the
// verdict budget fingerprint, so every mode builds the continuation anew.
outcome probe(const std::string& spec_src) {
	outcome o;
	o.one = run_spec(spec_src, 0);
	o.factorized = run_spec(spec_src, 1);
	o.shadow = run_spec(spec_src, 2);
	o.kept = factorized_continuation_hits;
	o.warmups = factorized_continuation_warmups;
	o.mismatches = factorized_continuation_mismatches;
	o.undecided = factorized_continuation_undecided;
	return o;
}

bool agree(const outcome& o) {
	return o.one == o.factorized && o.factorized == o.shadow
		&& o.mismatches == 0 && o.undecided == 0;
}

// Outputs are functions of the inputs.
const char* functional =
	"(o1[t]:bv[8] = i1[t]:bv[8] + { 1 }:bv[8]) && "
	"((o1[t]:bv[8] > { 3 }:bv[8]) ? (o2[t]:bv[8] = { 1 }:bv[8]) : (o2[t]:bv[8] = { 0 }:bv[8])).";

// State carried by a stream with an initial condition, read by a guard.
const char* state_stream =
	"(o1[0]:bv[8] = { 0 }:bv[8]) && (o1[t]:bv[8] = o1[t-1]:bv[8] + i1[t]:bv[8]) && "
	"((o1[t]:bv[8] > { 3 }:bv[8]) ? (o2[t]:bv[8] = { 1 }:bv[8]) : (o2[t]:bv[8] = { 0 }:bv[8])).";

// A chain of definitions reading each other: the normal form as one
// formula multiplies the cases along the chain. The last guard tests
// the value with a comparison, not with the equation that defines it (an
// equation the body also tests is a guard, not a definition).
const char* chain =
	"(o1[0]:bv[8] = { 0 }:bv[8]) && (o1[t]:bv[8] = o1[t-1]:bv[8] + { 1 }:bv[8]) && "
	"((o1[t-1]:bv[8] < { 2 }:bv[8]) ? (o3[t]:bv[8] = i1[t]:bv[8]) : ((o1[t-1]:bv[8] < { 4 }:bv[8]) ? (o3[t]:bv[8] = i1[t]:bv[8] + { 1 }:bv[8]) : (o3[t]:bv[8] = { 0 }:bv[8]))) && "
	"((o3[t]:bv[8] > { 3 }:bv[8]) ? (o4[t]:bv[8] = o3[t]:bv[8]) : (o4[t]:bv[8] = { 1 }:bv[8])) && "
	"((o4[t]:bv[8] < { 2 }:bv[8]) ? (o2[t]:bv[8] = o3[t]:bv[8]) : (o2[t]:bv[8] = o4[t]:bv[8] + { 2 }:bv[8])).";

// An initial condition at the second time point on a stream kept by an
// identity, consistent with the definitions for every input: the warm-up
// up to it is eliminated, the one at it keeps the reached conjuncts.
const char* initial_beyond_first =
	"(o3[1]:bv[8] = { 5 }:bv[8]) && (o3[t]:bv[8] = o3[t-1]:bv[8]) && "
	"(o1[t]:bv[8] = i1[t]:bv[8]) && (o2[t]:bv[8] = o1[t]:bv[8] + o3[t]:bv[8]).";

// Two initial conditions and a lookback of two.
const char* lookback_two =
	"(o1[0]:bv[8] = { 0 }:bv[8]) && (o1[1]:bv[8] = { 1 }:bv[8]) && "
	"(o1[t]:bv[8] = o1[t-1]:bv[8] + o1[t-2]:bv[8]) && (o2[t]:bv[8] = o1[t]:bv[8] + i1[t]:bv[8]).";

// Of the shape, but an initial condition pins a later output against the
// definitions: no run.
const char* pinned_against_definition =
	"(o1[0]:bv[8] = { 5 }:bv[8]) && (o1[1]:bv[8] = { 7 }:bv[8]) && (o1[t]:bv[8] = o1[t-1]:bv[8]) && (o2[t]:bv[8] = o1[t]:bv[8] + i1[t]:bv[8]).";

bool unsatisfiable(const std::string& spec_src, int mode) {
	factorized_mode m(mode);
	bdd_init<Bool>();
	auto spec = create_spec(spec_src.c_str());
	io_context<node_t> ctx;
	strings i1_values = { "1", "5" };
	ctx.add_input("i1", bv_type_id<node_t>(8),
		std::make_shared<vector_input_stream>(i1_values));
	auto o2 = std::make_shared<vector_output_stream>();
	ctx.add_output("o2", bv_type_id<node_t>(8), o2);
	auto maybe_i = run<node_t>(spec, ctx, 2);
	return !maybe_i.has_value() || o2->get_values().empty();
}

// A choice among two outputs: not of the shape, normalized as one formula
// in every mode.
const char* choice =
	"(o2[t]:bv[8] = { 0 }:bv[8] || o2[t]:bv[8] = { 1 }:bv[8]) && (o1[t]:bv[8] = i1[t]:bv[8]).";

} // namespace

TEST_SUITE("configuration") {
	TEST_CASE("bdd_init") { bdd_init<Bool>(); }
	TEST_CASE("logging") { logging::trace(); }
}

TEST_SUITE("factorized continuation") {

	TEST_CASE("a functional step: kept factorized, same outputs, equivalent forms") {
		outcome o = probe(functional);
		CHECK(agree(o));
		CHECK(o.kept >= 1);
		CHECK(o.one == strings{ "0", "1", "0", "1" });
	}

	TEST_CASE("state in a stream with an initial condition: the warm-up keeps the reached conjuncts") {
		outcome o = probe(state_stream);
		CHECK(agree(o));
		CHECK(o.kept >= 1);
		CHECK(o.warmups >= 1);
		CHECK(o.one.size() == 4);
	}

	TEST_CASE("an initial condition against the definitions: no run in every mode") {
		CHECK(unsatisfiable(pinned_against_definition, 1));
		CHECK(unsatisfiable(pinned_against_definition, 0));
		CHECK(unsatisfiable(pinned_against_definition, 2));
	}

	TEST_CASE("a chain of definitions reading each other") {
		outcome o = probe(chain);
		CHECK(agree(o));
		CHECK(o.kept >= 1);
		CHECK(o.one.size() == 4);
	}

	TEST_CASE("a lookback of two with two initial conditions: the iterate's quantified block is dropped, the warm-up keeps the reached conjuncts") {
		outcome o = probe(lookback_two);
		CHECK(agree(o));
		CHECK(o.kept >= 1);
		CHECK(o.warmups >= 1);
		CHECK(o.one.size() == 4);
	}

	TEST_CASE("an initial condition after the first time point: eliminated up to it, kept from it on") {
		outcome o = probe(initial_beyond_first);
		CHECK(agree(o));
		CHECK(o.kept >= 1);
		// kept at every time point from the initial condition to the one
		// the continuation is used verbatim from
		CHECK(o.warmups >= 1);
		CHECK(o.one == strings{ "0", "6", "10", "7" });
	}

	TEST_CASE("a choice among outputs is normalized as one formula in every mode") {
		outcome o = probe(choice);
		CHECK(agree(o));
		CHECK(o.kept == 0);
	}

	TEST_CASE("off normalizes as one formula: nothing is kept") {
		run_spec(functional, 0);
		CHECK(factorized_continuation_hits == 0);
	}
}
