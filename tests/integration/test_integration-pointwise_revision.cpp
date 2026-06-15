// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Test cases for the bugs reported in private/review-pwr.md (deep review of
// the pointwise revision implementation). Each test asserts the *intended*
// behavior, so all of them are expected to FAIL until the corresponding bug
// is fixed.
//
// Coverage map (review section -> test case):
//   §2.1/§2.2 -> "2.2: lookback spec is revised pointwise, not replaced"
//   §2.3      -> "2.3: part merge must preserve lookback frames"
//   §2.4      -> "2.4: failed update must not leave interpreter half-updated"
//   §2.5      -> "2.5: fulfilled sometimes must not drop pending ones"
//   §2.6      -> "2.6: implied update must not recompute the continuation"
//                (no failing scenario could be constructed, see the test)
//   §2.9      -> "2.9: update emitted during initial lookback segment"
//
// Not covered (no observable behavior to test):
//   §2.7  - missing DBG assertion (single-clause precondition)
//   §2.8  - redundant duplicate satisfiability check (performance only)
//   §2.10 - code health items (duplicated trigger logic, logging, etc.)

#include "test_integration-interpreter_helper.h"

// returns true if values contains two consecutive entries equal to v
static bool has_consecutive_pair(const strings& values, const std::string& v) {
	for (size_t i = 0; i + 1 < values.size(); ++i)
		if (values[i] == v && values[i + 1] == v) return true;
	return false;
}

TEST_SUITE("pointwise revision bugs") {

	// §2.2 (and §2.1): the existential E in pointwise_revision quantifies
	// past output instances (o1[t-1]) in addition to current ones, so for
	// a lookback-carrying spec E degenerates to T and the revision
	// collapses to all-or-nothing replacement.
	//
	// Spec: toggle o1[t] = o1[t-1]' starting at 0. Update at step 1:
	// "o1[t] | i2[t] = 1" (only constrains o1 when i2 is 0). With i2 = 1
	// at every step the update is vacuous, so the pointwise revision must
	// keep the toggle running. Today the spec is replaced wholesale by the
	// update and o1 stops toggling (defaults to 0).
	TEST_CASE("2.2: lookback spec is revised pointwise, not replaced") {
		bdd_init<Bool>();
		auto spec = create_spec(
			"u[t] = i1[t] && o1[0] = 0 && o1[t] = o1[t-1]'.");
		strings i1_values = {
			"F", "o1[t] | i2[t] = 1", "F", "F", "F"
		};
		strings i2_values = { "T", "T", "T", "T", "T" };
		strings o1_expected = { "F", "T", "F", "T", "F" };
		io_context<node_t> ctx;
		auto i1 = std::make_shared<vector_input_stream>(i1_values);
		auto i2 = std::make_shared<vector_input_stream>(i2_values);
		auto o1 = std::make_shared<vector_output_stream>();
		auto u  = std::make_shared<vector_output_stream>();
		ctx.add_input( "i1", tau_type_id<node_t>(), i1);
		ctx.add_input( "i2", tau_type_id<node_t>(), i2);
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("u",  tau_type_id<node_t>(), u);
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK( maybe_i.has_value() );
		auto o1_values = o1->get_values();
		CHECK( o1_values == o1_expected );
	}

	// §2.3: unsqueeze_always merges the always bodies of previously
	// independent spec parts verbatim, without lookback adjustment. The
	// o1 part has lookback 2, the o2 part lookback 0; an update mentioning
	// both outputs forces the merge "always A && always B => always(A&&B)"
	// which silently drops B (o2[t] = i2[t]) for the first 2 steps of the
	// new execution frame.
	//
	// The update "o2[t] | i2[t]' | o1[t] | o1[t-2] = 1" is implied by
	// o2[t] = i2[t], so the revised spec must be semantically unchanged:
	// o2 must keep following i2 and o1 must keep its period-2 toggle.
	//
	// Notes on the expected traces: the global lookback is 2, so o2 is
	// genuinely unconstrained during the initial segment (steps 0-1,
	// defaulting to F); and due to §2.9 the update fed at step 2 is only
	// consumed at step 4 (inputs shift by the lookback) and applied at
	// time point 5. The intended behavior still demands o2[5] = i2[5]:
	// today the unadjusted merge drops o2[t] = i2[t] inside the new
	// 2-lookback frame and o2[5] falls back to F.
	TEST_CASE("2.3: part merge must preserve lookback frames") {
		bdd_init<Bool>();
		auto spec = create_spec(
			"u[t] = i1[t] && o1[0] = 0 && o1[1] = 0 && "
			"o1[t] = o1[t-2]' && o2[t] = i2[t].");
		strings i1_values = {
			"F", "F", "o2[t] | i2[t]' | o1[t] | o1[t-2] = 1",
			"F", "F", "F"
		};
		strings i2_values = { "T", "T", "T", "T", "T", "T" };
		strings o1_expected = { "F", "F", "T", "T", "F", "F" };
		strings o2_expected = { "F", "F", "T", "T", "T", "T" };
		io_context<node_t> ctx;
		auto i1 = std::make_shared<vector_input_stream>(i1_values);
		auto i2 = std::make_shared<vector_input_stream>(i2_values);
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		auto u  = std::make_shared<vector_output_stream>();
		ctx.add_input( "i1", tau_type_id<node_t>(), i1);
		ctx.add_input( "i2", tau_type_id<node_t>(), i2);
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		ctx.add_output("u",  tau_type_id<node_t>(), u);
		auto maybe_i = run<node_t>(spec, ctx, 6);
		CHECK( maybe_i.has_value() );
		auto o1_values = o1->get_values();
		CHECK( o1_values == o1_expected );
		auto o2_values = o2->get_values();
		CHECK( o2_values == o2_expected );
	}

	// §2.4 (transactional safety of the update commit, also coverage gap
	// #6: update introducing a new, previously-unseen output stream): an
	// update that adds the unrelated leftover part "o5[t] = o5[t-1]"
	// (lookback 1) must not disturb the o1 part. Today the commit is not
	// transactional: after the swap the recomputed lookback/streams leave
	// the interpreter inconsistent and o1 desynchronizes from i2 (the
	// trace arrives shifted by one step).
	TEST_CASE("2.4: failed update must not leave interpreter half-updated") {
		bdd_init<Bool>();
		auto spec = create_spec("u[t] = i1[t] && o1[t] = i2[t].");
		strings i1_values = {
			"F", "o5[t] = o5[t-1]", "F", "F"
		};
		strings i2_values = { "T", "F", "T", "F" };
		strings o1_expected = { "T", "F", "T", "F" };
		io_context<node_t> ctx;
		auto i1 = std::make_shared<vector_input_stream>(i1_values);
		auto i2 = std::make_shared<vector_input_stream>(i2_values);
		auto o1 = std::make_shared<vector_output_stream>();
		auto u  = std::make_shared<vector_output_stream>();
		ctx.add_input( "i1", tau_type_id<node_t>(), i1);
		ctx.add_input( "i2", tau_type_id<node_t>(), i2);
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("u",  tau_type_id<node_t>(), u);
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK( maybe_i.has_value() );
		auto o1_values = o1->get_values();
		CHECK( o1_values == o1_expected );
	}

	// §2.5: a "sometimes" obligation fulfilled before the update is
	// re-imposed on the revised spec; if it conflicts with the update, the
	// all-or-nothing retention check fails and *every* old sometimes is
	// dropped, including genuinely pending compatible ones.
	//
	// S1 = sometimes o1[t] = 1 is either fulfilled in the first steps or
	// made impossible by the update — in both cases it must not take the
	// pending S2 = sometimes o2[t] o2[t-1] = 1 down with it (covers both
	// §2.5 bullets: fulfilled-obligation removal and maximal-subset
	// retention). S2 cannot be fulfilled before step 3 (o2[0] = o2[1] =
	// 0), and the update ("always o1[t] = 0") is applied at time point 2,
	// so S2 is guaranteed to still be pending. S2 is compatible with the
	// revised always part, so two consecutive o2 = 1 must eventually
	// appear in the trace. Today S1 makes the all-or-nothing retention
	// check unsat and S2 is silently dropped together with it.
	TEST_CASE("2.5: fulfilled sometimes must not drop pending ones") {
		bdd_init<Bool>();
		auto spec = create_spec(
			"(always u[t] = i1[t]) && (sometimes o1[t] = 1) && "
			"(sometimes o2[t] o2[t-1] = 1) && "
			"(always (o1[t] o2[t] = 0 && o2[0] = 0 && o2[1] = 0)).");
		strings i1_values = {
			"o1[t] = 0", "F", "F", "F", "F", "F", "F"
		};
		io_context<node_t> ctx;
		auto i1 = std::make_shared<vector_input_stream>(i1_values);
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		auto u  = std::make_shared<vector_output_stream>();
		ctx.add_input( "i1", tau_type_id<node_t>(), i1);
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		ctx.add_output("u",  tau_type_id<node_t>(), u);
		auto maybe_i = run<node_t>(spec, ctx, 7);
		CHECK( maybe_i.has_value() );
		auto o1_values = o1->get_values();
		auto o2_values = o2->get_values();
		// the update must hold from time point 2 onwards
		for (size_t i = 2; i < o1_values.size(); ++i)
			CHECK( o1_values[i] == "F" );
		// the pending obligation S2 must still be fulfilled eventually
		CHECK( has_consecutive_pair(o2_values, "T") );
	}

	// §2.6: the "spec unchanged" check compares the memory-substituted
	// spec part against the normalized revision with subtree_equals, so
	// in principle a semantically implied update may not be recognized as
	// a no-op, needlessly recomputing the unbound continuation at
	// time_point (resetting the part's execution frame).
	//
	// NOTE: this is the one reported bug for which no failing behavior
	// could be constructed: stored spec parts stay in canonical normal
	// form (both at creation and after revisions, with ground residues of
	// memory substitution simplified on the fly), so the syntactic check
	// hit in every scenario tried (executed initial condition, unexecuted
	// future initial condition o1[5] = 1, plain toggle). The test is kept
	// as a regression contract: the update "o1[t] | o1[t-1] = 1" is
	// implied by the toggle (x | x' = 1), so the spec is unchanged and
	// the interpreter state after the run must be identical to the state
	// of a run without any update. It currently PASSES.
	TEST_CASE("2.6: implied update must not recompute the continuation") {
		bdd_init<Bool>();
		strings o1_expected = { "F", "T", "F", "T", "F" };
		auto run_with = [](const strings& i1_values, strings& o1_out) {
			auto spec = create_spec(
				"u[t] = i1[t] && o1[5] = 1 && o1[t] = o1[t-1]'.");
			io_context<node_t> ctx;
			auto i1 = std::make_shared<vector_input_stream>(i1_values);
			auto o1 = std::make_shared<vector_output_stream>();
			auto u  = std::make_shared<vector_output_stream>();
			ctx.add_input( "i1", tau_type_id<node_t>(), i1);
			ctx.add_output("o1", tau_type_id<node_t>(), o1);
			ctx.add_output("u",  tau_type_id<node_t>(), u);
			auto maybe_i = run<node_t>(spec, ctx, 5);
			o1_out = o1->get_values();
			return maybe_i;
		};
		strings o1_baseline, o1_updated;
		auto baseline = run_with(
			{ "F", "F", "F", "F", "F" }, o1_baseline);
		auto updated = run_with(
			{ "F", "o1[t] | o1[t-1] = 1", "F", "F", "F" }, o1_updated);
		REQUIRE( baseline.has_value() );
		REQUIRE( updated.has_value() );
		// the toggle must keep running in both cases
		CHECK( o1_baseline == o1_expected );
		CHECK( o1_updated == o1_expected );
		// and the no-op update must not have touched the unbound
		// continuation of any spec part
		const trefs& ubt_baseline = baseline.value().ubt_ctn;
		const trefs& ubt_updated  = updated.value().ubt_ctn;
		REQUIRE( ubt_baseline.size() == ubt_updated.size() );
		for (size_t i = 0; i < ubt_baseline.size(); ++i)
			CHECK( tau::subtree_equals(
				ubt_baseline[i], ubt_updated[i]) );
	}

	// §2.9: solution_with_max_update searches for u[time_point], but
	// during the initial lookback segment the step formula uses
	// u[time_point + lookback], so the update stream silently defaults to
	// 0 and updates are impossible in that window. The spec below has
	// global lookback 1 (via o2); the update for the (lookback-0) o1 part
	// arrives at step 0 and must be emitted on u and applied from step 1.
	TEST_CASE("2.9: update emitted during initial lookback segment") {
		bdd_init<Bool>();
		auto spec = create_spec(
			"u[t] = i1[t] && o1[t] = 0 && "
			"o2[0] = 0 && o2[t] = o2[t-1]'.");
		strings i1_values = { "o1[t] = 1", "F", "F", "F" };
		strings o1_expected = { "F", "T", "T", "T" };
		io_context<node_t> ctx;
		auto i1 = std::make_shared<vector_input_stream>(i1_values);
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		auto u  = std::make_shared<vector_output_stream>();
		ctx.add_input( "i1", tau_type_id<node_t>(), i1);
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		ctx.add_output("u",  tau_type_id<node_t>(), u);
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK( maybe_i.has_value() );
		auto u_values = u->get_values();
		REQUIRE( !u_values.empty() );
		// the update must be visible on the u stream at step 0
		CHECK( u_values[0] != "F" );
		auto o1_values = o1->get_values();
		CHECK( o1_values == o1_expected );
	}
}
