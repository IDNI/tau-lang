// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

// Task 10: end-to-end ADT integration coverage. The same "type Point =
// {a: sbf, b: sbf}" tuple type is exercised through normalize, sat/unsat,
// solve and the interpreter, borrowing each area's own integration fixture
// idiom rather than inventing new plumbing:
//   - normalize: test_integration-wff_normalization.cpp (get_nso_rr +
//     normalizer<node_t>)
//   - sat/unsat: test_integration-satisfiability1.cpp (get_nso_rr +
//     is_tau_formula_sat<node_t>)
//   - solve:     test_integration-solver.cpp's "solve" suite (get_nso_rr +
//     solve<node_t> with solver_options)
//   - interpreter: test_integration-interpreter.cpp (io_context<node_t> +
//     run<node_t> + vector_input_stream/vector_output_stream)
//
// All four fixtures share the SAME node_t (tau_ba<bv, sbf_ba>, bv, sbf_ba,
// via test_tau_helpers.h), so this file needs only that one include -- the
// per-suite "_helper.h" files are deliberately NOT included here: several of
// them (test_integration-satisfiability_helper.h,
// test_integration-interpreter_helper.h) define a free function
// `tref create_spec(const char*)` with the identical signature, and including
// more than one in the same translation unit would be an ODR violation.
//
// Every spec string below was verified against ./build-Debug/tau -e first
// (transcripts in task-10-report.md).

// Mirrors test_integration-solver.cpp's own check_solution helper (that
// file defines it locally in a .cpp, not a shared header, so it cannot be
// included -- reproduced verbatim here instead of inventing a different
// inspection idiom): substitute the solution back into the equation and
// confirm the result normalizes to T.
static bool check_adt_solution(tref eq, const solution<node_t>& sol) {
	tref substitution = rewriter::replace<node_t>(eq, sol);
	tref check = normalizer<node_t>(substitution);
	return tau::get(check).equals_T();
}

TEST_SUITE("adt integration") {

	TEST_CASE("normalize: flattened equals hand-flattened") {
		// The ADT-typed spec and its hand-flattened equivalent (using the
		// "x.a:sbf, x.b:sbf" quantifier-binder surface syntax the flattener
		// itself produces -- see test_adt_flatten.cpp's "annotation
		// propagates in scope" case) must normalize to the same result.
		// Comparing normalized RESULTS to each other (rather than either one
		// against a hardcoded expected string) sidesteps disjunct/conjunct
		// ordering differences between the two derivations, and avoids
		// tripping matches_to_any_of's DEBUG-only "expected[0] must be
		// canonical" check in test_helpers.h (not invoked here at all).
		bdd_init<Bool>();
		auto adt_nso = get_nso_rr(
			"type Point = {a: sbf, b: sbf}. "
			"ex x:Point (x = 0 || x != 1).");
		auto hand_nso = get_nso_rr(
			"ex x.a:sbf, x.b:sbf "
			"((x.a = 0 && x.b = 0) || (x.a != 1 || x.b != 1)).");
		REQUIRE( adt_nso.has_value() );
		REQUIRE( hand_nso.has_value() );
		tref adt_res = normalizer<node_t>(adt_nso.value());
		tref hand_res = normalizer<node_t>(hand_nso.value());
		REQUIRE( adt_res != nullptr );
		REQUIRE( hand_res != nullptr );
		CHECK( tau::get(adt_res).to_str() == tau::get(hand_res).to_str() );
	}

	TEST_CASE("sat: satisfiable tuple spec") {
		// ex x:Point x = 0 always has the all-zero tuple as a witness.
		bdd_init<Bool>();
		tref spec = get_nso_rr(
			"type Point = {a: sbf, b: sbf}. "
			"ex x:Point (x = 0).").value().main->get();
		CHECK( is_tau_formula_sat<node_t>(spec) );
	}

	TEST_CASE("sat: unsatisfiable tuple spec") {
		// A real contradiction: no Point can be both = 0 and != 0.
		bdd_init<Bool>();
		tref spec = get_nso_rr(
			"type Point = {a: sbf, b: sbf}. "
			"ex x:Point (x = 0 && x != 0).").value().main->get();
		CHECK( !is_tau_formula_sat<node_t>(spec) );
	}

	TEST_CASE("solver: ADT-typed unknowns") {
		// `x:Point = x` fixes x's ADT type via an equality against the bare
		// occurrence (rather than `ex x:Point ...`, which would bind x) so
		// solve() sees it as a free variable to solve for -- mirroring
		// tests/repl/commands/test_repl-adt.cmake's adt-solve case, verified
		// there to report the solution per flattened component (x.a, x.b).
		bdd_init<Bool>();
		tref form = get_nso_rr(
			"type Point = {a: sbf, b: sbf}. "
			"x:Point = x && x.a = 1 && x.b = 0.").value().main->get();
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(sbf_type<node_t>()),
			.mode = solver_mode::general
		};
		bool solve_error = false;
		auto solution = solve<node_t>(form, options, solve_error);
		REQUIRE( !solve_error );
		REQUIRE( solution.has_value() );

		// Solution-inspection API #1: substituting the solution back into
		// the equation and normalizing must yield T (check_adt_solution,
		// mirrored from test_integration-solver.cpp's check_solution).
		CHECK( check_adt_solution(form, solution.value()) );

		// Solution-inspection API #2: the map entries for x.a/x.b
		// themselves, checked with the same .equals_1()/.equals_0() bf
		// predicates tests/unit/test_io_context.cpp uses to read back a
		// concrete bit value -- this is what actually pins "x.a -> 1,
		// x.b -> 0" (check_adt_solution alone would also pass for, say, a
		// solution that left x.a/x.b unsubstituted free variables whose
		// equation happens to normalize away to T some other way).
		tref xa = build_bf_variable<node_t>("x.a", sbf_type_id<node_t>());
		tref xb = build_bf_variable<node_t>("x.b", sbf_type_id<node_t>());
		auto ita = solution.value().find(xa);
		auto itb = solution.value().find(xb);
		REQUIRE( ita != solution.value().end() );
		REQUIRE( itb != solution.value().end() );
		CHECK( tau::get(ita->second).equals_1() );
		CHECK( tau::get(itb->second).equals_0() );
	}

	TEST_CASE("interpreter: mixed-BA tuple round trip") {
		// Every other automated ADT case is all-sbf; this is the automated
		// stand-in for the demo's Segment (bv[8] tag + sbf member): two
		// DIFFERENT algebras riding one physical stream, distributed to
		// differently-typed member vars and collected back. Note the
		// emitted bv leaves use bv's canonical DECIMAL print ("1", "254"),
		// not the input's #b spelling (verified live against
		// ./build-Debug/tau -X before pinning).
		bdd_init<Bool>();
		io_context<node_t> ctx;
		tref parsed = tau::get(
			"type Rec = {tag: bv[8], a: sbf}. "
			"i:Rec := in console. o:Rec := out console. "
			"o[t] = i[t].",
			{ .context = &ctx });
		REQUIRE( parsed != nullptr );
		tref spec = get_nso_rr<node_t>(ctx, parsed).value().main->get();
		strings i_values = {
			"{ tag: \"#b00000001\", a: \"1\" }",
			"{ tag: \"#b11111110\", a: \"0\" }"
		};
		ctx.add_input("i", tau_type_id<node_t>(),
			std::make_shared<vector_input_stream>(i_values));
		auto o = std::make_shared<vector_output_stream>();
		ctx.add_output("o", tau_type_id<node_t>(), o);
		auto maybe_i = run<node_t>(spec, ctx, 2);
		CHECK( maybe_i.has_value() );
		auto o_values = o->get_values();
		REQUIRE( o_values.size() == 2 );
		CHECK( o_values[0] == "{ tag: \"1\", a: \"1\" }" );
		CHECK( o_values[1] == "{ tag: \"254\", a: \"0\" }" );
	}

	TEST_CASE("interpreter: tuple io round trip") {
		// Task 8's own unit test spec ("tuple input distributes and tuple
		// output collects", tests/unit/test_interpreter.cpp's "adt
		// interpreter" suite), run here through the INTEGRATION fixture's
		// own io_context<node_t> + run<node_t> pattern (as used throughout
		// test_integration-interpreter.cpp) instead of tau_api::get_interpreter.
		//
		// Unlike a plain (non-ADT) io var, an ADT-typed `in console`/
		// `out console` def needs the io_context passed at PARSE time
		// (`.context = &ctx`): adt_flatten_rewrite_io_def
		// (src/adt/adt_flatten.tmpl.h) is what registers the grouped stream
		// layout into ctx.adt_streams/ctx.inputs, and it fails the whole
		// parse (returns nullptr) if handed no context at all. That rules
		// out this file's own create_spec(ctx, sample)-shaped helper (like
		// test_integration-interpreter_helper.h's), which only forwards ctx
		// to get_nso_rr, not to the tau::get() parse call itself -- so the
		// parse and the io-var resolution are done as two explicit steps
		// below instead.
		bdd_init<Bool>();
		io_context<node_t> ctx;
		tref parsed = tau::get(
			"type Point = {a: sbf, b: sbf}. "
			"i:Point := in console. o:Point := out console. "
			"o[t] = i[t].",
			{ .context = &ctx });
		REQUIRE( parsed != nullptr );
		tref spec = get_nso_rr<node_t>(ctx, parsed).value().main->get();

		strings i_values = {
			"{ a: \"0\", b: \"1\" }",
			"{ a: \"1\", b: \"0\" }"
		};
		ctx.add_input("i", tau_type_id<node_t>(),
			std::make_shared<vector_input_stream>(i_values));
		auto o = std::make_shared<vector_output_stream>();
		ctx.add_output("o", tau_type_id<node_t>(), o);

		auto maybe_i = run<node_t>(spec, ctx, 2);
		CHECK( maybe_i.has_value() );

		// Validated on step count (two tuple literals consumed/produced) and
		// output VALUES, not on run<node_t>'s own return/exit status -- per
		// this branch's run-benchmark-fixtures convention (see
		// project-run-benchmark-fixtures memory).
		auto o_values = o->get_values();
		REQUIRE( o_values.size() == i_values.size() );
		CHECK( o_values == i_values );
	}
}
