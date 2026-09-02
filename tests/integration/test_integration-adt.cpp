// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include <filesystem>
#include <fstream>

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

// Runs `spec_body` (preceded by `preamble`, e.g. a definition) over a
// two-line Point input file and returns the output file's lines, following
// the "interpreter: tuple io through file streams" case's own idiom: the
// type/def/stream declarations sit on SEPARATE lines, since two quoted file
// names on one line are a greedy-capture mis-parse. `tag` keeps concurrent
// cases' temp files apart.
static std::vector<std::string> run_point_spec(const std::string& tag,
	const std::string& preamble, const std::string& spec_body)
{
	namespace fs = std::filesystem;
	fs::path in_p  = fs::temp_directory_path() / ("tau_test_adt_" + tag + "_in.txt");
	fs::path out_p = fs::temp_directory_path() / ("tau_test_adt_" + tag + "_out.txt");
	{
		std::ofstream f(in_p);
		f << "{ a: \"1\", b: \"0\" }\n" << "{ a: \"0\", b: \"1\" }\n";
	}
	io_context<node_t> ctx;
	std::string spec_src =
		"type Point = {a: sbf, b: sbf}.\n"
		+ preamble +
		"i:Point := in file(\"" + in_p.string() + "\").\n"
		"o:Point := out file(\"" + out_p.string() + "\").\n"
		+ spec_body;
	tref parsed = tau::get(spec_src, { .context = &ctx });
	REQUIRE( parsed != nullptr );
	auto nso_rr = get_nso_rr<node_t>(ctx, parsed);
	REQUIRE( nso_rr.has_value() );
	// A spec carrying definitions needs its rec_relations expanded INTO the
	// main formula before the interpreter sees it -- an unexpanded `f(...)`
	// call leaves make_interpreter with an unresolvable ref and it returns
	// nullopt. Every production entry point does this (api::get_interpreter's
	// nso_rr_apply, which is also what `tau <spec-file>` reaches;
	// repl_evaluator::run_cmd's get_any), which is why the rest of this file
	// gets away with a bare `.main->get()`: those specs have no definitions,
	// and nso_rr_apply is a no-op on an empty rule set. Normalization is NOT
	// done here on purpose: make_interpreter normalizes its own argument.
	tref applied = nso_rr_apply<node_t>(nso_rr.value());
	REQUIRE( applied != nullptr );
	auto maybe_i = run<node_t>(applied, ctx, 2);
	CHECK( maybe_i.has_value() );
	std::vector<std::string> lines;
	{
		std::ifstream f(out_p.string());
		for (std::string l; std::getline(f, l);) lines.push_back(l);
	}
	std::error_code ec;
	fs::remove(in_p, ec); fs::remove(out_p, ec);
	return lines;
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

	TEST_CASE("interpreter: partial member copy defaults the others to 0") {
		// Time point 0 copies only member a; b is emitted as sbf's 0 even
		// though the input has b:"1" there. Time point 1 copies the whole
		// tuple. Defaulting relies on EVERY member appearing at SOME time
		// point of the spec -- see the companion "never-mentioned member"
		// case below for what happens when one doesn't. (Automates the
		// demo_4.1 partial-copy example, commit 308af029.)
		bdd_init<Bool>();
		io_context<node_t> ctx;
		tref parsed = tau::get(
			"type Point = {a: sbf, b: sbf}. "
			"i:Point := in console. o:Point := out console. "
			"(o[0].a = i[0].a) && o[1] = i[1].",
			{ .context = &ctx });
		REQUIRE( parsed != nullptr );
		tref spec = get_nso_rr<node_t>(ctx, parsed).value().main->get();
		strings i_values = {
			"{ a: \"1\", b: \"1\" }",
			"{ a: \"0\", b: \"1\" }"
		};
		ctx.add_input("i", tau_type_id<node_t>(),
			std::make_shared<vector_input_stream>(i_values));
		auto o = std::make_shared<vector_output_stream>();
		ctx.add_output("o", tau_type_id<node_t>(), o);
		auto maybe_i = run<node_t>(spec, ctx, 2);
		CHECK( maybe_i.has_value() );
		auto o_values = o->get_values();
		REQUIRE( o_values.size() == 2 );
		CHECK( o_values[0] == "{ a: \"1\", b: \"0\" }" ); // b defaulted, not copied
		CHECK( o_values[1] == "{ a: \"0\", b: \"1\" }" ); // full copy
	}

	TEST_CASE("interpreter: a member mentioned at NO time point drops the whole output") {
		// FIXME (MEDIUM): known gap (2026-08-07, see the partial-copy demo
		// commentary in demos/demo_4.1): o.b appears at no time point, so
		// rebuild_outputs (interpreter.tmpl.h, keyed on the spec's
		// current_outputs) never registers it, the shared adt_tuple_writer
		// never sees its component complete, and the run silently writes
		// NOTHING for o -- no error, no literal (reconfirmed live
		// 2026-08-17). When this is fixed (defaulting never-mentioned
		// members like partially-mentioned ones, or rejecting the spec),
		// update this test to the new contract; it exists so the change is
		// conscious.
		bdd_init<Bool>();
		io_context<node_t> ctx;
		tref parsed = tau::get(
			"type Point = {a: sbf, b: sbf}. "
			"i:Point := in console. o:Point := out console. "
			"o[0].a = i[0].a.",
			{ .context = &ctx });
		REQUIRE( parsed != nullptr );
		tref spec = get_nso_rr<node_t>(ctx, parsed).value().main->get();
		strings i_values = { "{ a: \"1\", b: \"1\" }" };
		ctx.add_input("i", tau_type_id<node_t>(),
			std::make_shared<vector_input_stream>(i_values));
		auto o = std::make_shared<vector_output_stream>();
		ctx.add_output("o", tau_type_id<node_t>(), o);
		auto maybe_i = run<node_t>(spec, ctx, 1);
		(void)maybe_i; // success/failure is not the contract being pinned
		CHECK( o->get_values().empty() ); // current behavior: silently nothing
	}

	TEST_CASE("interpreter: tuple io through file streams") {
		// Exercises rebuild_inputs'/rebuild_outputs' stream_id != 0 branch
		// (interpreter.tmpl.h): one physical FILE per tuple stream, wire
		// literals one per line -- previously only covered manually by
		// demos/demo_4.1 (file fixtures under demos/fixture/). The two
		// file() defs sit on SEPARATE lines: two quoted file names on one
		// line of a spec-start parse are a greedy-capture mis-parse
		// (file_name is printable+, parser/tau.tgf), rejected outright by
		// adt_flatten's upfront scan -- see test_adt_parsing.cpp's
		// "two file streams on one line" case.
		bdd_init<Bool>();
		namespace fs = std::filesystem;
		fs::path in_p  = fs::temp_directory_path() / "tau_test_adt_in.txt";
		fs::path out_p = fs::temp_directory_path() / "tau_test_adt_out.txt";
		{
			std::ofstream f(in_p);
			f << "{ a: \"0\", b: \"1\" }\n" << "{ a: \"1\", b: \"0\" }\n";
		}
		io_context<node_t> ctx;
		std::string spec_src =
			"type Point = {a: sbf, b: sbf}.\n"
			"i:Point := in file(\"" + in_p.string() + "\").\n"
			"o:Point := out file(\"" + out_p.string() + "\").\n"
			"o[t] = i[t].";
		tref parsed = tau::get(spec_src, { .context = &ctx });
		REQUIRE( parsed != nullptr );
		tref spec = get_nso_rr<node_t>(ctx, parsed).value().main->get();
		auto maybe_i = run<node_t>(spec, ctx, 2);
		CHECK( maybe_i.has_value() );
		std::vector<std::string> lines;
		{
			std::ifstream f(out_p.string());
			for (std::string l; std::getline(f, l);) lines.push_back(l);
		}
		REQUIRE( lines.size() == 2 );
		CHECK( lines[0] == "{ a: \"0\", b: \"1\" }" );
		CHECK( lines[1] == "{ a: \"1\", b: \"0\" }" );
		std::error_code ec;
		fs::remove(in_p, ec); fs::remove(out_p, ec);
	}

	// --- member OPERATIONS on tuple streams ---------------------------------
	// The three cases below go past the plain member COPY the tests above
	// pin: a complemented member, a permuted (crossed) copy, and a member
	// passed through a definition. All three were reported broken (unsat /
	// collapse to the straight copy plus an unparseable input wire literal /
	// "expected :sbf, found :Point"), and all three are symptoms of ONE
	// cause that lives outside this file: a run spec whose `type` declaration
	// is not visible to the parse that reads the spec leaves `i[t].a` an
	// unflattened member access on a tuple-typed io var. Cross-line REPL
	// visibility is what used to break that (fixed on this branch; pinned by
	// tests/repl/commands/test_repl-adt.cmake's adt-cross_line_* cases,
	// including a run over ADT streams). These integration cases parse the
	// type and the spec together, so they pin the OTHER half of the contract:
	// that with the type visible, a member operation, a permutation and a
	// definition call each flatten and execute correctly end to end.
	//
	// Each writes its own input file rather than reading a checked-in
	// fixture: tests/integration/test_files/ is only ever reached through a
	// working-directory-relative path (and only from commented-out code in
	// test_integration-interpreter.cpp), whereas the file-stream test above
	// already establishes the self-contained temp-file idiom this file uses.

	TEST_CASE("run: member operation on a tuple stream (complement)") {
		// Member a is complemented, member b copied, so the input
		// { a: "1", b: "0" } / { a: "0", b: "1" } must come out as
		// { a: "0", b: "0" } / { a: "1", b: "1" }. Reported symptom when the
		// member paths do not flatten: the whole always-spec normalizes to F
		// ("Tau specification is unsat") -- an operator applied to a member
		// is what tells the two sides apart, so a copy survives unflattened
		// where this does not.
		bdd_init<Bool>();
		auto lines = run_point_spec("memberop", "",
			"(o[t].a = i[t].a') && (o[t].b = i[t].b).");
		REQUIRE( lines.size() == 2 );
		CHECK( lines[0] == "{ a: \"0\", b: \"0\" }" );
		CHECK( lines[1] == "{ a: \"1\", b: \"1\" }" );
	}

	TEST_CASE("run: crossed member copy between tuple streams") {
		// o.a takes i.b and o.b takes i.a, so the two members swap. Reported
		// symptom when the member paths do not flatten: the spec collapses
		// to the STRAIGHT copy `o[t]:Point = i[t]:Point` (the permutation is
		// lost), and the input stream then rejects its own wire literal
		// ("Unexpected end of file"), because the tuple never got taken
		// apart into per-member streams.
		bdd_init<Bool>();
		auto lines = run_point_spec("crossed", "",
			"(o[t].a = i[t].b) && (o[t].b = i[t].a).");
		REQUIRE( lines.size() == 2 );
		CHECK( lines[0] == "{ a: \"0\", b: \"1\" }" );
		CHECK( lines[1] == "{ a: \"1\", b: \"0\" }" );
	}

	TEST_CASE("run: definition applied to a tuple stream member") {
		// Same outputs as the complement case, with the complement moved
		// into a definition. The head annotation `f(x):sbf` is REQUIRED and
		// is not part of what is pinned here: an UNtyped head applied to any
		// typed argument fails inference identically without ADTs in the
		// picture ("f(i[t]:sbf), expected :tau, found :sbf" for a plain
		// `i:sbf` stream), so it is a definition-typing rule, not a tuple
		// one. What this case pins is that the member path flattens INSIDE
		// the ref argument -- the reported symptom was inference failing on
		// an unflattened `f(i[t]:Point.a)`.
		bdd_init<Bool>();
		auto lines = run_point_spec("memberdef", "f(x):sbf := x'.\n",
			"(o[t].a = f(i[t].a)) && (o[t].b = i[t].b).");
		REQUIRE( lines.size() == 2 );
		CHECK( lines[0] == "{ a: \"0\", b: \"0\" }" );
		CHECK( lines[1] == "{ a: \"1\", b: \"1\" }" );
	}
}
