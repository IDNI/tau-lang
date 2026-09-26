// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// The step of a specification of functional shape evaluated from its
// definitions (satisfiability.h, `functional_step_evaluation`). The outputs
// of a run must not depend on the switch: same outputs with every step
// solved, with the steps evaluated, and under the shadow mode, whose
// counters show that the evaluated values are the solved ones.

#include "test_integration-interpreter_helper.h"

namespace {

// `functional_step_evaluation_mode` lets the environment variable
// TAU_FUNCTIONAL_STEP_EVALUATION override the flag; run with the variables
// of the series unset. The functional shape and the factorized continuation
// stay at 1.
struct evaluation_mode {
	int saved = functional_step_evaluation;
	int saved_functional = functional_continuation;
	int saved_factorized = factorized_continuation;
	explicit evaluation_mode(int mode) {
		functional_step_evaluation = mode;
		functional_continuation = 1;
		factorized_continuation = 1;
		functional_step_evaluation_hits = 0;
		functional_step_evaluation_fallbacks = 0;
		functional_step_evaluation_mismatches = 0;
		functional_step_evaluation_undecided = 0;
	}
	~evaluation_mode() {
		functional_step_evaluation = saved;
		functional_continuation = saved_functional;
		factorized_continuation = saved_factorized;
	}
};

// Six steps of a specification reading `i1` and writing `o2`, both bv[8].
strings run_spec(const std::string& spec_src, int mode) {
	evaluation_mode m(mode);
	bdd_init<Bool>();
	auto spec = create_spec(spec_src.c_str());
	io_context<node_t> ctx;
	strings i1_values = { "1", "5", "2", "7", "0", "4" };
	ctx.add_input("i1", bv_type_id<node_t>(8),
		std::make_shared<vector_input_stream>(i1_values));
	auto o2 = std::make_shared<vector_output_stream>();
	ctx.add_output("o2", bv_type_id<node_t>(8), o2);
	auto maybe_i = run<node_t>(spec, ctx, 6);
	REQUIRE(maybe_i.has_value());
	return o2->get_values();
}

struct outcome {
	strings solved, evaluated, shadow;
	size_t steps, fallbacks, mismatches, undecided;
};

// One specification through the three modes; the counters are read after
// the shadow run.
outcome probe(const std::string& spec_src) {
	outcome o;
	o.solved = run_spec(spec_src, 0);
	o.evaluated = run_spec(spec_src, 1);
	o.shadow = run_spec(spec_src, 2);
	o.steps = functional_step_evaluation_hits;
	o.fallbacks = functional_step_evaluation_fallbacks;
	o.mismatches = functional_step_evaluation_mismatches;
	o.undecided = functional_step_evaluation_undecided;
	return o;
}

bool agree(const outcome& o) {
	return o.solved == o.evaluated && o.evaluated == o.shadow
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

// A chain of definitions reading each other; the last guard tests the
// value with a comparison, not with the equation that defines it (an
// equation the body also tests is a guard, not a definition).
const char* chain =
	"(o1[0]:bv[8] = { 0 }:bv[8]) && (o1[t]:bv[8] = o1[t-1]:bv[8] + { 1 }:bv[8]) && "
	"((o1[t-1]:bv[8] < { 2 }:bv[8]) ? (o3[t]:bv[8] = i1[t]:bv[8]) : ((o1[t-1]:bv[8] < { 4 }:bv[8]) ? (o3[t]:bv[8] = i1[t]:bv[8] + { 1 }:bv[8]) : (o3[t]:bv[8] = { 0 }:bv[8]))) && "
	"((o3[t]:bv[8] > { 3 }:bv[8]) ? (o4[t]:bv[8] = o3[t]:bv[8]) : (o4[t]:bv[8] = { 1 }:bv[8])) && "
	"((o4[t]:bv[8] < { 2 }:bv[8]) ? (o2[t]:bv[8] = o3[t]:bv[8]) : (o2[t]:bv[8] = o4[t]:bv[8] + { 2 }:bv[8])).";

// Two initial conditions and a lookback of two.
const char* lookback_two =
	"(o1[0]:bv[8] = { 0 }:bv[8]) && (o1[1]:bv[8] = { 1 }:bv[8]) && "
	"(o1[t]:bv[8] = o1[t-1]:bv[8] + o1[t-2]:bv[8]) && (o2[t]:bv[8] = o1[t]:bv[8] + i1[t]:bv[8]).";

// An initial condition at the second time point on a stream kept by an
// identity, consistent with the definitions for every input: the steps
// up to it are solved, the later ones evaluated.
const char* initial_beyond_first =
	"(o3[1]:bv[8] = { 5 }:bv[8]) && (o3[t]:bv[8] = o3[t-1]:bv[8]) && "
	"(o1[t]:bv[8] = i1[t]:bv[8]) && (o2[t]:bv[8] = o1[t]:bv[8] + o3[t]:bv[8]).";

// A guard made of a conjunction, and a guard comparing an output of the
// same time point with a constant.
const char* guards =
	"(o1[t]:bv[8] = i1[t]:bv[8]) && "
	"((o1[t]:bv[8] > { 1 }:bv[8] && o1[t]:bv[8] < { 6 }:bv[8]) ? (o3[t]:bv[8] = o1[t]:bv[8] + { 10 }:bv[8]) : (o3[t]:bv[8] = { 0 }:bv[8])) && "
	"((o3[t]:bv[8] < { 1 }:bv[8]) ? (o2[t]:bv[8] = { 9 }:bv[8]) : (o2[t]:bv[8] = o3[t]:bv[8])).";

// A choice among two outputs: not of the shape, solved in every mode.
const char* choice =
	"(o2[t]:bv[8] = { 0 }:bv[8] || o2[t]:bv[8] = { 1 }:bv[8]) && (o1[t]:bv[8] = i1[t]:bv[8]).";

} // namespace

TEST_SUITE("configuration") {
	TEST_CASE("bdd_init") { bdd_init<Bool>(); }
	TEST_CASE("logging") { logging::trace(); }
}

TEST_SUITE("functional step evaluation") {

	TEST_CASE("a functional step: evaluated, same outputs, no mismatch") {
		outcome o = probe(functional);
		CHECK(agree(o));
		CHECK(o.steps == 6);
		CHECK(o.fallbacks == 0);
		CHECK(o.solved == strings{ "0", "1", "0", "1", "0", "1" });
	}

	TEST_CASE("state in a stream with an initial condition") {
		outcome o = probe(state_stream);
		CHECK(agree(o));
		CHECK(o.steps >= 5);
		CHECK(o.fallbacks == 0);
		CHECK(o.solved.size() == 6);
	}

	TEST_CASE("a chain of definitions reading each other") {
		outcome o = probe(chain);
		CHECK(agree(o));
		CHECK(o.steps >= 5);
		CHECK(o.fallbacks == 0);
		CHECK(o.solved.size() == 6);
	}

	TEST_CASE("a lookback of two with two initial conditions") {
		outcome o = probe(lookback_two);
		CHECK(agree(o));
		// the continuation is used verbatim from the third time point on
		CHECK(o.steps == 3);
		CHECK(o.fallbacks == 0);
		CHECK(o.solved.size() == 6);
	}

	TEST_CASE("an initial condition beyond the first time point") {
		outcome o = probe(initial_beyond_first);
		CHECK(agree(o));
		CHECK(o.steps == 4);
		CHECK(o.fallbacks == 0);
		CHECK(o.solved == strings{ "0", "6", "10", "7", "12", "5" });
	}

	TEST_CASE("a conjunction as a guard and a guard on an output of the time point") {
		outcome o = probe(guards);
		CHECK(agree(o));
		CHECK(o.steps == 6);
		CHECK(o.fallbacks == 0);
		CHECK(o.solved == strings{ "9", "15", "12", "9", "9", "14" });
	}

	TEST_CASE("a choice among outputs is solved in every mode") {
		outcome o = probe(choice);
		CHECK(agree(o));
		CHECK(o.steps == 0);
		CHECK(o.fallbacks == 0);
	}

	TEST_CASE("off solves every step: nothing evaluated") {
		run_spec(functional, 0);
		CHECK(functional_step_evaluation_hits == 0);
	}
}
