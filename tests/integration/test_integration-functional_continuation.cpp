// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// The continuation fixpoint settled from the functional shape of the
// specification (satisfiability.h, `functional_continuation`). A
// specification of that shape is a fixpoint after the lookback by
// construction, so the run of a specification must not depend on the
// switch: same outputs settled from the shape, with the implication only,
// and under the shadow mode, whose counters show which checks the shape
// would settle and that it never contradicts the implication.

#include "test_integration-interpreter_helper.h"

namespace {

// `functional_continuation_mode` lets the environment variable
// TAU_FUNCTIONAL_CONTINUATION override the flag; run with it unset. Every
// run builds its continuation anew: the mode is part of the verdict budget
// fingerprint, and the flag-search cap, which none of these specifications
// reaches, is set to a value no earlier run had, so that two runs in the
// same mode do not share a memoized continuation either.
struct continuation_mode {
	int saved = functional_continuation;
	size_t saved_cap = max_flag_search_steps;
	explicit continuation_mode(int mode) {
		static size_t runs = 0;
		functional_continuation = mode;
		max_flag_search_steps = saved_cap + ++runs;
		functional_continuation_checks = 0;
		functional_continuation_shape_hits = 0;
		functional_continuation_mismatches = 0;
		functional_continuation_undecided = 0;
		functional_continuation_closure_skips = 0;
	}
	~continuation_mode() {
		functional_continuation = saved;
		max_flag_search_steps = saved_cap;
	}
};

// The io context of a specification reading `i1` and writing `o2`, both
// bv[8], with the outputs of the run.
struct fixture {
	io_context<node_t> ctx;
	std::shared_ptr<vector_output_stream> o2 =
		std::make_shared<vector_output_stream>();
	explicit fixture(strings i1_values = { "1", "5", "2", "7" }) {
		ctx.add_input("i1", bv_type_id<node_t>(8),
			std::make_shared<vector_input_stream>(i1_values));
		ctx.add_output("o2", bv_type_id<node_t>(8), o2);
	}
};

// Four steps through `run`, which hands the specification to
// `make_interpreter` as it is.
strings run_spec(const std::string& spec_src, int mode) {
	continuation_mode c(mode);
	bdd_init<Bool>();
	auto spec = create_spec(spec_src.c_str());
	fixture f;
	auto maybe_i = run<node_t>(spec, f.ctx, 4);
	REQUIRE(maybe_i.has_value());
	return f.o2->get_values();
}

// Four steps the way the API builds an interpreter: the specification is
// normalized first, and `make_interpreter` receives the form as written
// next to it (or nothing, when `as_written` is false).
strings run_spec_normalized(const std::string& spec_src, int mode,
	bool as_written)
{
	continuation_mode c(mode);
	bdd_init<Bool>();
	auto spec = create_spec(spec_src.c_str());
	auto normalized = normalizer<node_t>(spec);
	REQUIRE(normalized.has_value());
	fixture f;
	auto maybe_i = interpreter<node_t>::make_interpreter(normalized.value(),
		f.ctx, as_written ? spec : nullptr);
	REQUIRE(maybe_i.has_value());
	auto ran = maybe_i.value().run_loop(4);
	REQUIRE(ran.has_value());
	return f.o2->get_values();
}

struct shadow_counts { size_t checks, by_shape, mismatches, undecided; };

shadow_counts counts() {
	return { functional_continuation_checks,
		functional_continuation_shape_hits,
		functional_continuation_mismatches,
		functional_continuation_undecided };
}

// One specification through the three modes. The switch is part of the
// verdict budget fingerprint, so every mode builds the continuation anew
// and leaves its own counters behind.
struct outcome {
	strings shadow, off, on;
	shadow_counts c;
	std::string joined() const {
		std::string r;
		for (const auto& v : shadow) r += v + " ";
		return r;
	}
};

outcome probe(const std::string& spec_src) {
	outcome o;
	o.off = run_spec(spec_src, 0);
	CHECK(functional_continuation_checks == 0);
	o.on = run_spec(spec_src, 1);
	o.shadow = run_spec(spec_src, 2);
	o.c = counts();
	return o;
}

// The three modes agree on the outputs, and the shape never contradicts
// the implication, decided or not.
bool agree(const outcome& o) {
	return o.off == o.on && o.on == o.shadow
		&& o.c.mismatches == 0 && o.c.undecided == 0;
}

// Outputs are functions of the inputs.
const char* functional =
	"(o1[t]:bv[8] = i1[t]:bv[8] + { 1 }:bv[8]) && "
	"((o1[t]:bv[8] > { 3 }:bv[8]) ? (o2[t]:bv[8] = { 1 }:bv[8]) : (o2[t]:bv[8] = { 0 }:bv[8])).";

// State carried by a stream with an initial condition, read by a guard.
const char* state_stream =
	"(o1[0]:bv[8] = { 0 }:bv[8]) && (o1[t]:bv[8] = o1[t-1]:bv[8] + i1[t]:bv[8]) && "
	"((o1[t]:bv[8] > { 3 }:bv[8]) ? (o2[t]:bv[8] = { 1 }:bv[8]) : (o2[t]:bv[8] = { 0 }:bv[8])).";

// A value computed from the previous state, read by a guard: the shape
// whose implication puts the whole telescope in front of the solver.
const char* computed_from_state =
	"(o1[0]:bv[8] = { 0 }:bv[8]) && (o1[t]:bv[8] = o1[t-1]:bv[8] + { 1 }:bv[8]) && "
	"(o3[t]:bv[8] = o1[t-1]:bv[8] + i1[t]:bv[8]) && "
	"((o3[t]:bv[8] > { 3 }:bv[8]) ? (o2[t]:bv[8] = { 1 }:bv[8]) : (o2[t]:bv[8] = { 0 }:bv[8])).";

// A state stream kept by an identity, tested in a guard made of a
// conjunction: the normalizer propagates the identity into the guard and
// flattens its negation into disjuncts that read like equations of the
// state, so the shape is readable off the form as written only.
const char* identity_in_guard =
	"(o1[0]:bv[8] = { 0 }:bv[8]) && (o1[t]:bv[8] = o1[t-1]:bv[8]) && "
	"(((o1[t-1]:bv[8] != { 42 }:bv[8]) && (o1[t-1]:bv[8] != i1[t]:bv[8])) ? "
	"(o2[t]:bv[8] = { 1 }:bv[8]) : (o2[t]:bv[8] = { 0 }:bv[8])).";

// Two initial conditions and a lookback of two.
const char* lookback_two =
	"(o1[0]:bv[8] = { 0 }:bv[8]) && (o1[1]:bv[8] = { 1 }:bv[8]) && "
	"(o1[t]:bv[8] = o1[t-1]:bv[8] + o1[t-2]:bv[8]) && (o2[t]:bv[8] = o1[t]:bv[8] + i1[t]:bv[8]).";

// A choice among two outputs: not a function of the inputs, so not of the
// shape; the implication decides.
const char* choice =
	"(o2[t]:bv[8] = { 0 }:bv[8] || o2[t]:bv[8] = { 1 }:bv[8]) && (o1[t]:bv[8] = i1[t]:bv[8]).";

// A constraint on an output that holds on the reachable states only:
// `o1` stays below 5 because it starts at 0 and never changes. Not of the
// shape; the implication holds.
const char* reachable_only =
	"(o1[0]:bv[8] = { 0 }:bv[8]) && (o1[t]:bv[8] = o1[t-1]:bv[8]) && "
	"(o1[t]:bv[8] < { 5 }:bv[8]) && (o2[t]:bv[8] = o1[t]:bv[8] + i1[t]:bv[8]).";

// The negative control: a specification without a run must not be settled
// as one. `o12[t] = 0 && o12[t-1] = 1` has no run at all.
bool unsatisfiable(const std::string& spec_src, int mode) {
	continuation_mode c(mode);
	bdd_init<Bool>();
	auto spec = create_spec(spec_src.c_str());
	io_context<node_t> ctx;
	strings i1_values = { "1", "5" };
	ctx.add_input("i1", bv_type_id<node_t>(8),
		std::make_shared<vector_input_stream>(i1_values));
	auto o12 = std::make_shared<vector_output_stream>();
	ctx.add_output("o12", tau_type_id<node_t>(), o12);
	auto o2 = std::make_shared<vector_output_stream>();
	ctx.add_output("o2", bv_type_id<node_t>(8), o2);
	auto maybe_i = run<node_t>(spec, ctx, 2);
	return !maybe_i.has_value()
		|| (o12->get_values().empty() && o2->get_values().empty());
}

const char* no_run = "(o12[t]:tau = 0) && (o12[t-1]:tau = 1).";

// Of the shape, but an initial condition pins the first defined value to
// something else: no run either, and the shape must not settle the run
// check.
const char* pinned_against_definition =
	"(o1[0]:bv[8] = { 5 }:bv[8]) && (o1[1]:bv[8] = { 7 }:bv[8]) && (o1[t]:bv[8] = o1[t-1]:bv[8]) && (o2[t]:bv[8] = o1[t]:bv[8] + i1[t]:bv[8]).";

// The always-part of a specification as written, for the recognizer.
tref always_body(tref spec) { return functional_shape_body<node_t>(spec); }

bool shape(const char* spec_src) {
	tref body = always_body(create_spec(spec_src));
	return body && functional_step_shape<node_t>(body);
}

// Two always statements, merged by the normalizer into one.
const char* two_always =
	"(always (o1[t]:bv[8] = i1[t]:bv[8] + { 1 }:bv[8])) && "
	"(always ((o1[t]:bv[8] > { 3 }:bv[8]) ? (o2[t]:bv[8] = { 1 }:bv[8]) : (o2[t]:bv[8] = { 0 }:bv[8]))).";

} // namespace

TEST_SUITE("configuration") {
	TEST_CASE("bdd_init") { bdd_init<Bool>(); }
	TEST_CASE("logging") { logging::trace(); }
}

TEST_SUITE("functional shape") {

	TEST_CASE("equations and a conditional over inputs and the previous outputs") {
		CHECK(shape("always ((o1[t]:bv[8] = i1[t]:bv[8] + { 1 }:bv[8]) && ((o1[t]:bv[8] > { 3 }:bv[8]) ? (o2[t]:bv[8] = { 1 }:bv[8]) : (o2[t]:bv[8] = { 0 }:bv[8])))."));
		CHECK(shape("always ((o1[0]:bv[8] = { 0 }:bv[8]) && (o1[t]:bv[8] = o1[t-1]:bv[8] + i1[t]:bv[8]) && ((o1[t]:bv[8] > { 3 }:bv[8]) ? (o2[t]:bv[8] = { 1 }:bv[8]) : (o2[t]:bv[8] = { 0 }:bv[8])))."));
	}

	TEST_CASE("a nested conditional whose guard equates another output") {
		CHECK(shape("always ((o1[t]:bv[8] = i1[t]:bv[8]) && ((o1[t]:bv[8] = { 43 }:bv[8]) ? (o2[t]:bv[8] = { 0 }:bv[8]) : ((o1[t]:bv[8] + i1[t]:bv[8] > { 42 }:bv[8]) ? (o2[t]:bv[8] = { 42 }:bv[8]) : (o2[t]:bv[8] = o1[t]:bv[8] + i1[t]:bv[8]))))."));
	}

	TEST_CASE("a guard made of a disjunction of conjunctions stays one literal") {
		CHECK(shape("always ((o1[t]:bv[8] = i1[t]:bv[8]) && ((i1[t]:bv[8] != { 1 }:bv[8]) ? ((((i1[t]:bv[8] = { 2 }:bv[8]) && (o1[t]:bv[8] = i2[t]:bv[8])) || ((i1[t]:bv[8] = { 3 }:bv[8]) && (o1[t]:bv[8] = i3[t]:bv[8]))) ? (o2[t]:bv[8] = { 1 }:bv[8]) : (o2[t]:bv[8] = { 0 }:bv[8])) : (o2[t]:bv[8] = { 0 }:bv[8])))."));
	}

	TEST_CASE("a choice among outputs is no definition") {
		CHECK(!shape("always ((o2[t]:bv[8] = { 0 }:bv[8] || o2[t]:bv[8] = { 1 }:bv[8]) && (o1[t]:bv[8] = i1[t]:bv[8]))."));
	}

	TEST_CASE("a constraint on an output is no definition") {
		CHECK(!shape("always ((o1[0]:bv[8] = { 0 }:bv[8]) && (o1[t]:bv[8] = o1[t-1]:bv[8]) && (o1[t]:bv[8] < { 5 }:bv[8]) && (o2[t]:bv[8] = o1[t]:bv[8]))."));
	}

	TEST_CASE("two definitions of one output, and a cycle") {
		CHECK(!shape("always ((o1[t]:bv[8] = i1[t]:bv[8]) && (o1[t]:bv[8] = { 1 }:bv[8]) && (o2[t]:bv[8] = o1[t]:bv[8]))."));
		CHECK(!shape("always ((o1[t]:bv[8] = o2[t]:bv[8] + { 1 }:bv[8]) && (o2[t]:bv[8] = o1[t]:bv[8]))."));
	}

	TEST_CASE("several always statements, and a specification without one") {
		CHECK(shape(two_always));
		CHECK(shape(functional));
		CHECK(!shape("(always (o1[t]:bv[8] = i1[t]:bv[8])) && (sometimes (o1[t]:bv[8] = { 1 }:bv[8]))."));
	}

	TEST_CASE("a constant time constraint is not of the shape") {
		CHECK(!shape("always (([t < 3] -> (o2[t]:bv[8] = { 0 }:bv[8])) && ([t >= 3] -> (o2[t]:bv[8] = { 1 }:bv[8])))."));
	}

	TEST_CASE("a constraint without outputs must hold on its own") {
		CHECK(shape("always ((i1[t]:bv[8] & { #xFF }:bv[8] = i1[t]:bv[8]) && (o2[t]:bv[8] = i1[t]:bv[8]))."));
		CHECK(!shape("always ((i1[t]:bv[8] < { 5 }:bv[8]) && (o2[t]:bv[8] = i1[t]:bv[8]))."));
	}

	TEST_CASE("the shape is read off the form as written, not off the normalized one") {
		tref spec = create_spec(("always " + std::string(identity_in_guard)).c_str());
		CHECK(functional_step_shape<node_t>(always_body(spec)));
		auto normalized = normalizer<node_t>(spec);
		REQUIRE(normalized.has_value());
		CHECK(!functional_step_shape<node_t>(always_body(normalized.value())));
	}
}

TEST_SUITE("functional continuation") {

	TEST_CASE("a functional step: the shape settles every check, same outputs") {
		outcome o = probe(functional);
		INFO("outputs: " << o.joined());
		CHECK(agree(o));
		CHECK(o.c.checks >= 1);
		CHECK(o.c.by_shape == o.c.checks);
		CHECK(o.shadow == strings{ "0", "1", "0", "1" });
	}

	TEST_CASE("two always statements: the shape settles every check") {
		outcome o = probe(two_always);
		INFO("outputs: " << o.joined());
		CHECK(agree(o));
		CHECK(o.c.checks >= 1);
		CHECK(o.c.by_shape == o.c.checks);
		CHECK(o.shadow == strings{ "0", "1", "0", "1" });
	}

	TEST_CASE("state in a stream with an initial condition") {
		outcome o = probe(state_stream);
		INFO("outputs: " << o.joined());
		CHECK(agree(o));
		CHECK(o.c.checks >= 1);
		CHECK(o.c.by_shape == o.c.checks);
		CHECK(o.shadow.size() == 4);
	}

	TEST_CASE("a value computed from the previous state read by a guard") {
		outcome o = probe(computed_from_state);
		INFO("outputs: " << o.joined());
		CHECK(agree(o));
		CHECK(o.c.checks >= 1);
		CHECK(o.c.by_shape == o.c.checks);
		CHECK(o.shadow.size() == 4);
	}

	TEST_CASE("a lookback of two with two initial conditions") {
		outcome o = probe(lookback_two);
		INFO("outputs: " << o.joined());
		CHECK(agree(o));
		CHECK(o.c.checks >= 1);
		CHECK(o.c.by_shape == o.c.checks);
		CHECK(o.shadow.size() == 4);
	}

	TEST_CASE("an identity propagated into a guard: settled from the form as written, not from the normalized one") {
		strings off = run_spec_normalized(identity_in_guard, 0, true);
		strings on = run_spec_normalized(identity_in_guard, 1, true);
		CHECK(off == on);
		CHECK(on.size() == 4);
		run_spec_normalized(identity_in_guard, 2, true);
		shadow_counts with = counts();
		CHECK(with.checks >= 1);
		CHECK(with.by_shape == with.checks);
		CHECK(with.mismatches == 0);
		CHECK(with.undecided == 0);
		// The same specification without the form as written: the
		// normalized always-part is not of the shape, the implication
		// decides, and the outputs are the same.
		strings normalized_only = run_spec_normalized(identity_in_guard, 2, false);
		shadow_counts without = counts();
		CHECK(normalized_only == on);
		CHECK(without.checks >= 1);
		CHECK(without.by_shape == 0);
	}

	TEST_CASE("a choice among outputs: the implication decides, same outputs") {
		outcome o = probe(choice);
		INFO("outputs: " << o.joined());
		CHECK(agree(o));
		CHECK(o.c.checks >= 1);
		CHECK(o.c.by_shape == 0);
	}

	TEST_CASE("a constraint holding on the reachable states only: the implication decides") {
		outcome o = probe(reachable_only);
		INFO("outputs: " << o.joined());
		CHECK(agree(o));
		CHECK(o.c.checks >= 1);
		CHECK(o.c.by_shape == 0);
		CHECK(o.shadow.size() == 4);
	}

	TEST_CASE("a specification without a run is unsatisfiable in every mode") {
		CHECK(unsatisfiable(no_run, 2));
		CHECK(functional_continuation_shape_hits == 0);
		CHECK(unsatisfiable(no_run, 0));
		CHECK(unsatisfiable(no_run, 1));
	}

	TEST_CASE("an initial condition against the definitions: no run, in every mode") {
		CHECK(shape(("always " + std::string(pinned_against_definition)).c_str()));
		CHECK(unsatisfiable(pinned_against_definition, 1));
		CHECK(unsatisfiable(pinned_against_definition, 0));
		CHECK(unsatisfiable(pinned_against_definition, 2));
	}

	TEST_CASE("off keeps the implication: no check is counted") {
		run_spec(functional, 0);
		CHECK(functional_continuation_checks == 0);
	}
}
