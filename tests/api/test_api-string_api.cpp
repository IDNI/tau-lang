// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

using tau_api = api<node_t>;

#include "test_api-fixture.h"

TEST_SUITE("Tau API - string") {
	TEST_CASE_FIXTURE(api_fixture, "Querying") {
		for (const auto& term : terms) {
			CHECK(tau_api::is_term(term));
			CHECK(!tau_api::is_formula(term));
		}
		for (const auto& formula : formulas) {
			CHECK(!tau_api::is_term(formula));
			CHECK(tau_api::is_formula(formula));
		}
	}

	// apply_defs()/apply_all_defs() parse via get_spec_or_term(), which
	// tries get_spec() first; a bare formula like "x = 0" parses fine as
	// a one-line spec (spec(main(wff(...)))). Because that tree contains
	// no `ref`, api<node>::get_nso_rr's no-ref branch kept the whole
	// spec-shaped tree as nso_rr.main instead of unwrapping it the way
	// its ref branch does (via tau_lang::get_nso_rr's main -> wff/bf
	// navigation), so nso_rr_apply carried the spec shape through and
	// to_str() rendered it with spec grammar's trailing '.' -- unlike
	// every neighbouring string overload (substitute, dnf, cnf, nnf, ...)
	// which route through get_formula_or_term() and never carry the
	// artifact. Fixed by unwrapping the spec shape in the string overload
	// itself, right before to_str(); apply_def/apply_all_defs share the
	// same underlying apply_defs(defs, string) so both are covered.
	TEST_CASE_FIXTURE(api_fixture, "apply_defs/apply_all_defs on a bare formula") {
		auto all = tau_api::apply_all_defs("x = 0");
		REQUIRE(all.has_value());
		CHECK(all.value() == "x = 0");

		auto some = tau_api::apply_defs(std::set<std::string>{}, "x = 0");
		REQUIRE(some.has_value());
		CHECK(some.value() == "x = 0");
	}

	// Input that genuinely carries a spec's own inline definitions must
	// still round-trip correctly: it reaches api<node>::get_nso_rr's ref
	// branch (a `ref` to apply_all_defs_f is present), which already
	// unwraps to the bare main formula via tau_lang::get_nso_rr, so this
	// path was correct both before and after the fix above -- pinned here
	// so the string-overload change above cannot regress it. Mirrors the
	// tref-level "apply_all_defs" case in test_api-tref_api.cpp, whose
	// spec (apply_all_defs_f(x) := x'.\napply_all_defs_f(z) = 0.) already
	// established z' = 0 as the correct unwrapped result.
	TEST_CASE_FIXTURE(api_fixture, "apply_all_defs on a spec with real definitions") {
		auto applied = tau_api::apply_all_defs(
			"str_apply_all_defs_f(x) := x'.\n"
			"str_apply_all_defs_f(z) = 0.");
		REQUIRE(applied.has_value());
		CHECK(applied.value() == "z' = 0");
	}

	// AP-9: sat/realizable/unrealizable/valid/valid_spec routed a string
	// through get_spec_or_term(), wrapping it in a `spec` node. The
	// tref-level sat()/realizable() gate on is_formula() (a bare wff), so
	// every well-formed formula silently reported unsat/unrealizable
	// regardless of content; valid()/valid_spec() have no such gate, so
	// instead the whole-query BA fast path fed the still-`spec`-wrapped
	// tree to build_wff_neg(), which asserts on a bare wff and aborted.
	// Fixed by routing through get_formula_or_term(), like the
	// neighbouring string overloads (substitute, dnf, cnf, nnf, ...).
	TEST_CASE_FIXTURE(api_fixture, "sat/unsat/realizable/unrealizable/valid on well-formed input") {
		CHECK(tau_api::sat("x = 0").value());
		CHECK(tau_api::realizable("x = 0").value());
		CHECK(!tau_api::unsat("x = 0").value());
		CHECK(!tau_api::unrealizable("x = 0").value());

		// a genuine contradiction
		CHECK(!tau_api::sat("x = 0 && x != 0").value());
		CHECK(tau_api::unsat("x = 0 && x != 0").value());
		CHECK(!tau_api::realizable("x = 0 && x != 0").value());
		CHECK(tau_api::unrealizable("x = 0 && x != 0").value());

		// a tautology
		CHECK(tau_api::valid("x = x").value());
		CHECK(tau_api::valid_spec("x = x").value());
		CHECK(!tau_api::valid("x = 0").value());
		CHECK(!tau_api::valid_spec("x = 0").value());
	}

	// boole_normal_form(const string&) routed through
	// get_spec_or_term(), so a one-line formula parsed as a spec node
	// (spec(main(wff(...)))) rather than a bare wff; that wrapped tree
	// reached tau_lang::boole_normal_form() -> syntactic_formula_simplification()
	// -> syntactic_path_simplification_dnf::on() -> build_wff_neg(), whose
	// assertion requires a bare wff and aborted the process for ordinary
	// well-formed input. Fixed by routing through get_formula_or_term(),
	// like dnf/cnf/nnf.
	// Only asserting a value here already proves the abort is gone (an
	// abort would take the whole test binary down), but pin the content
	// too, not merely that a value came back.
	TEST_CASE_FIXTURE(api_fixture, "boole_normal_form on well-formed input") {
		auto bnf = tau_api::boole_normal_form("x = 0 && y = 1");
		REQUIRE(bnf.has_value());
		CHECK(bnf.value() == "x = 0 && y' = 0");
	}


	// api<node>::solve(const string&) rendered every solved
	// value with the generic bf-constant spelling ("0"/"1"), unlike the
	// REPL's solve/lgrs commands (print_solver_cmd_solution ->
	// serialize_constant), which use the declared type's own literal
	// (tau's is "F"/"T"). Pin the actual formatted text here so a
	// regression to "0"/"1" fails loudly -- the malformed-input tests
	// above never exercised this positive case.
	TEST_CASE_FIXTURE(api_fixture, "solve/lgrs render the declared type's own literal") {
		auto zero = tau_api::solve("x = 0", solver_mode::general);
		REQUIRE(zero.has_value());
		CHECK(zero.value() == std::map<std::string, std::string>{
			{ "x", "F" } });

		auto nonzero = tau_api::solve("x != 0", solver_mode::general);
		REQUIRE(nonzero.has_value());
		CHECK(nonzero.value() == std::map<std::string, std::string>{
			{ "x", "T" } });

		auto lg = tau_api::lgrs("x = 0");
		REQUIRE(lg.has_value());
		CHECK(lg.value() == std::map<std::string, std::string>{
			{ "x", "F" } });
	}
}

// AP-8: only get_interpreter (below) had negative/malformed-input tests;
// every other string-API parse entry point was untested for the malformed
// or empty-string case. Each of these mirrors the style of the two
// get_interpreter negative tests: feed a syntactically invalid (or empty)
// string in and check the function fails gracefully (no crash, no value)
// instead of asserting/crashing on the null tref that the underlying parse
// produces.
TEST_SUITE("Tau API - string - malformed input") {
	static const strings malformed = { "", "x ) ( invalid !!!", "o[t] =" };

	TEST_CASE_FIXTURE(api_fixture, "is_term / is_formula reject malformed input") {
		for (const auto& s : malformed) {
			CHECK(!tau_api::is_term(s));
			CHECK(!tau_api::is_formula(s));
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "apply_def/apply_defs/apply_all_defs reject malformed input") {
		for (const auto& s : malformed) {
			CHECK(!tau_api::apply_def(s, "x + 1").has_value());
			CHECK(!tau_api::apply_def("f(x) := x + 1", s).has_value());
			CHECK(!tau_api::apply_defs(std::set<std::string>{ s }, "x + 1")
				.has_value());
			CHECK(!tau_api::apply_all_defs(s).has_value());
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "substitute rejects malformed input") {
		for (const auto& s : malformed) {
			CHECK(!tau_api::substitute(s, "x", "y").has_value());
			CHECK(!tau_api::substitute("x", s, "y").has_value());
			CHECK(!tau_api::substitute("x", "x", s).has_value());
			CHECK(!tau_api::substitute(
				s, std::map<std::string, std::string>{ { "x", "y" } })
				.has_value());
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "normal forms reject malformed input") {
		for (const auto& s : malformed) {
			CHECK(!tau_api::boole_normal_form(s).has_value());
			CHECK(!tau_api::dnf(s).has_value());
			CHECK(!tau_api::cnf(s).has_value());
			CHECK(!tau_api::nnf(s).has_value());
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "procedures reject malformed input") {
		for (const auto& s : malformed) {
			CHECK(!tau_api::syntactic_term_simplification(s).has_value());
			CHECK(!tau_api::syntactic_formula_simplification(s).has_value());
			CHECK(!tau_api::normalize_term(s).has_value());
			CHECK(!tau_api::normalize_formula(s).has_value());
			CHECK(!tau_api::anti_prenex(s).has_value());
			CHECK(!tau_api::eliminate_quantifiers(s).has_value());
			CHECK(!tau_api::simplify(s).has_value());
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "formula checks reject malformed input") {
		for (const auto& s : malformed) {
			// realizable/valid/valid_spec/sat all bottom out on a
			// parse failure, reported as a structured error rather
			// than a value.
			CHECK(!tau_api::realizable(s).has_value());
			CHECK(!tau_api::valid(s).has_value());
			CHECK(!tau_api::valid_spec(s).has_value());
			CHECK(!tau_api::sat(s).has_value());
			// AP1-11: malformed input is INVALID, not
			// "unsatisfiable" -- unrealizable/unsat now report an
			// error for it, matching the htref overloads, instead
			// of claiming a verdict for garbage.
			CHECK(!tau_api::unrealizable(s).has_value());
			CHECK(!tau_api::unsat(s).has_value());
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "solve/lgrs reject malformed input") {
		for (const auto& s : malformed) {
			CHECK(!tau_api::solve(s, solver_mode::general).has_value());
			CHECK(!tau_api::lgrs(s).has_value());
		}
	}
}

TEST_SUITE("Tau API - string - execution") {

	TEST_CASE("handle syntax error") {
		auto maybe_i = tau_api::get_interpreter("o[t] =");
		CHECK(!maybe_i.has_value());
	}

#ifdef TAU_PACK_HAS_BA_SBF
	TEST_CASE("handle type error") {
		auto maybe_i = tau_api::get_interpreter("o[t]:tau = i[t]:sbf");
		CHECK(!maybe_i.has_value());
	}

	TEST_CASE("reset_definitions lets a stream be retyped") {
		tau_api::reset_definitions();
		auto first = tau_api::get_interpreter("o7[t]:sbf = i7[t]:sbf.");
		REQUIRE(first.has_value());
		CHECK(!tau_api::get_interpreter("o7[t]:tau = o7[t]:tau.")
			.has_value());

		tau_api::reset_definitions();
		CHECK(tau_api::get_interpreter("o7[t]:tau = o7[t]:tau.")
			.has_value());

		// The interpreter built before the reset keeps its own streams.
		auto& i = first.value();
		auto maybe_inputs = tau_api::get_inputs_for_step(i);
		REQUIRE(maybe_inputs.has_value());
		auto& inputs = maybe_inputs.value();
		REQUIRE(inputs.size() == 1);
		std::map<stream_at, std::string> assigned;
		for (const auto& at : inputs) assigned[at] = "1";
		CHECK(tau_api::step(i, assigned).has_value());
		tau_api::reset_definitions();
	}
#endif // TAU_PACK_HAS_BA_SBF


	TEST_CASE("using get_inputs_for_step") {

		// Make the interpreter for a given specification as a string
		auto maybe_i = tau_api::get_interpreter("o[t] = i[t].");
		CHECK(maybe_i.has_value());
		auto& i = maybe_i.value();

		std::vector<std::string> collected_outputs;

		size_t step = 0;
		while (step < 3) {

			DBG(TAU_LOG_TRACE << "Step " << step;)
			step++;

			// Find out what inputs are expected for the next step
			auto maybe_inputs = tau_api::get_inputs_for_step(i);
			CHECK(maybe_inputs.has_value());
			auto& inputs = maybe_inputs.value();
#ifdef DEBUG
			TAU_LOG_TRACE << "Inputs:";
			if (inputs.empty()) TAU_LOG_TRACE << "No inputs";
			for (auto& input_at : inputs)
				TAU_LOG_TRACE << input_at.name << "[" << input_at.time_point<< "]";
#endif

			// Assign values to the inputs (spec has only `i` input stream)
			std::map<stream_at, std::string> assigned_inputs;
			for (auto& input_at : inputs) {
				assigned_inputs[input_at] = step % 2 == 0 ? "T." : "F.";
				DBG(TAU_LOG_TRACE << "Assigned " << input_at.name << "[" << input_at.time_point << "] = `" << assigned_inputs[input_at] << "`";)
			}

			// Step the interpreter with the assigned values
			// and retrieve outputs
			auto maybe_outputs = tau_api::step(i, assigned_inputs);
			CHECK(maybe_outputs.has_value());
			auto outputs = maybe_outputs.value();

#ifdef DEBUG
			TAU_LOG_TRACE << "Outputs:";
			if (outputs.empty()) TAU_LOG_TRACE << "No outputs";
#endif
			// Collect the outputs (we have only one output stream `o`)
			for (auto& [output_at, value] : outputs) {
				collected_outputs.push_back(value);
				DBG(TAU_LOG_TRACE << output_at.name << "[" << output_at.time_point << "] = `" << value << "`";)
			}
		}
#ifdef DEBUG
		std::stringstream ss;
		for (size_t i = 0; i < collected_outputs.size(); ++i)
			ss << (i > 0 ? ", " : " ") << collected_outputs[i];
		TAU_LOG_TRACE << "Collected outputs: " << ss.str();
#endif
		CHECK(collected_outputs == std::vector<std::string>({ "F", "T", "F" }));
	}

	// get_inputs_for_step now goes through calculate_initial_spec() like
	// step() does, so a caller can tell "the spec never initialized" apart
	// from "this step genuinely needs no inputs" via has_value().
	TEST_CASE("get_inputs_for_step returns a result carrying the inputs") {
		auto maybe_i = tau_api::get_interpreter("o[t] = i[t].");
		REQUIRE(maybe_i.has_value());
		auto& i = maybe_i.value();

		auto inputs = tau_api::get_inputs_for_step(i);
		REQUIRE(inputs.has_value());
		REQUIRE(inputs.value().size() == 1);
		CHECK(inputs.value()[0].name == "i");
	}

	TEST_CASE("current_spec and spec_revision follow applied updates") {
		auto maybe_i = tau_api::get_interpreter("u[t] = i[t].");
		REQUIRE(maybe_i.has_value());
		auto& i = maybe_i.value();

		// feeds every input of the next step the same value
		auto submit = [&i](const std::string& value) {
			std::map<stream_at, std::string> assigned;
			auto inputs = tau_api::get_inputs_for_step(i);
			REQUIRE(inputs.has_value());
			for (auto& at : inputs.value())
				assigned[at] = value;
			return tau_api::step(i, assigned);
		};

		CHECK(tau_api::spec_revision(i) == 0);
		const std::string initial = tau_api::current_spec(i);
		CHECK(!initial.empty());

		// a step that carries no update leaves both untouched
		CHECK(submit("T.").has_value());
		CHECK(tau_api::spec_revision(i) == 0);
		CHECK(tau_api::current_spec(i) == initial);

		// an applied update bumps the revision exactly once
		CHECK(submit("u[t] = i[t] | i[t-1].").has_value());
		CHECK(tau_api::spec_revision(i) == 1);
		const std::string updated = tau_api::current_spec(i);
		CHECK(updated != initial);

		// and a plain step after it still does not
		CHECK(submit("F.").has_value());
		CHECK(tau_api::spec_revision(i) == 1);
		CHECK(tau_api::current_spec(i) == updated);
	}

	// reset() clears the execution snapshot and takes time back to 0; the
	// memo that remembers which time point step_spec was built for must go
	// with it, or the first step after the reset returns early on the EMPTY
	// step_spec and the "complete outputs" fallback emits default-zero
	// outputs instead of replaying the spec.
	TEST_CASE("reset replays the spec from time 0 instead of the stale memo") {
		auto maybe_i = tau_api::get_interpreter("o[t] = i[t].");
		REQUIRE(maybe_i.has_value());
		auto& i = maybe_i.value();

		auto submit = [&i](const std::string& value) -> std::string {
			std::map<stream_at, std::string> assigned;
			auto inputs = tau_api::get_inputs_for_step(i);
			REQUIRE(inputs.has_value());
			for (auto& at : inputs.value())
				assigned[at] = value;
			auto outs = tau_api::step(i, assigned);
			REQUIRE(outs.has_value());
			REQUIRE(outs.value().size() == 1);
			return outs.value().begin()->second;
		};

		const std::string first = submit("T.");
		CHECK(first == "T");
		i.reset();
		CHECK(i.time_point == 0);
		// The same input at time 0 again must produce the same output.
		CHECK(submit("T.") == first);
		CHECK(submit("F.") == "F");
	}

	TEST_CASE("a rejected update leaves spec_revision and current_spec alone") {
		auto maybe_i = tau_api::get_interpreter("u[t] = i[t].");
		REQUIRE(maybe_i.has_value());
		auto& i = maybe_i.value();

		auto submit = [&i](const std::string& value) {
			std::map<stream_at, std::string> assigned;
			auto inputs = tau_api::get_inputs_for_step(i);
			REQUIRE(inputs.has_value());
			for (auto& at : inputs.value())
				assigned[at] = value;
			return tau_api::step(i, assigned);
		};

		// advance so the absolute positions below read before the start
		CHECK(submit("T.").has_value());
		CHECK(submit("T.").has_value());

		const size_t revision = tau_api::spec_revision(i);
		const std::string spec = tau_api::current_spec(i);

		// update() refuses this one ("invalid memory access was found"):
		// the step still succeeds, but nothing may move
		CHECK(submit("u[-2] = i[-2].").has_value());
		CHECK(tau_api::spec_revision(i) == revision);
		CHECK(tau_api::current_spec(i) == spec);
	}

	TEST_CASE("with remapped streams") {

		// Remap input and output streams from default console to custom streams
		std::vector<std::string> i_values = { "T", "F", "T" };
		auto i_stream = std::make_shared<vector_input_stream>(i_values);
		auto o_stream = std::make_shared<vector_output_stream>();
		interpreter_options opts;
		opts.input_remaps["i"] = i_stream;
		opts.output_remaps["o"] = o_stream;

		// Make the interpreter for a given specification as string
		auto maybe_i = tau_api::get_interpreter("o[t] = i[t].", opts);
		CHECK(maybe_i.has_value());
		auto& i = maybe_i.value();

		size_t step = 0;
		while (step < 3) {

			DBG(TAU_LOG_TRACE << "Step " << step;)
			step++;

			// Step the interpreter and retrieve outputs
			auto maybe_outputs = tau_api::step(i);
			CHECK(maybe_outputs.has_value());
			auto outputs = maybe_outputs.value();

#ifdef DEBUG
			TAU_LOG_TRACE << "Outputs:";
			if (outputs.empty()) TAU_LOG_TRACE << "No outputs";
			for (auto& [output_at, value] : outputs)
				TAU_LOG_TRACE << output_at.name << "[" << output_at.time_point << "] = `" << value << "`";
#endif
		}

		auto o_values = o_stream->get_values();
#ifdef DEBUG
		std::stringstream ss;
		ss << "Input values: [ ";
		for (size_t t = 0; t < i_values.size(); ++t)
			ss << (t > 0 ? ", " : " ") << i_values[t];
		ss << " ]";
		TAU_LOG_TRACE << ss.str();
		ss.str("");
		ss << "Output values: [ ";
		for (size_t t = 0; t < o_values.size(); ++t)
			ss << (t > 0 ? ", " : " ") << o_values[t];
		ss << " ]";
		TAU_LOG_TRACE << ss.str();
#endif
		CHECK( o_values.size() == i_values.size() );
		CHECK( o_values == i_values );
	}
}

// NF-1 / NF-2 regression: api::boole_normal_form(const std::string&) used
// get_spec_or_term, so it handed a `spec` node to a function that requires a
// wff and aborted on *every* well-formed input; and boole_normal_form's own
// is_atomic admits bf_lt/bf_lteq while its two consumers asserted bf_eq, so a
// bitvector comparison aborted too. Both were Debug-only aborts (the asserts do
// not exist in Release) and neither had a test, because the only existing test
// for this entry point feeds it malformed strings.
TEST_SUITE("Tau API - boole_normal_form regressions") {

	TEST_CASE_FIXTURE(api_fixture, "NF-1: valid input no longer aborts") {
		auto eq = tau_api::boole_normal_form("x = 0");
		REQUIRE( eq.has_value() );
		CHECK( *eq == "x = 0" );
		auto taut = tau_api::boole_normal_form("x = 0 || x != 0");
		REQUIRE( taut.has_value() );
		CHECK( *taut == "T" );
		auto contra = tau_api::boole_normal_form("x = 0 && x != 0");
		REQUIRE( contra.has_value() );
		CHECK( *contra == "F" );
		auto reduced = tau_api::boole_normal_form("xy|xy' = 0");
		REQUIRE( reduced.has_value() );
		CHECK( *reduced == "x = 0" );
	}

#ifdef TAU_PACK_HAS_BA_BV
	TEST_CASE_FIXTURE(api_fixture, "NF-2: bitvector comparisons no longer abort") {
		// Non-bv `<` is expanded by the construction hooks, but a bv one is
		// not, so it reaches the BDD-variable selection as a bf_lt.
		for (const char* s : { "x:bv[8] < y:bv[8]", "x:bv[8] <= y:bv[8]",
					"x:bv[8] > y:bv[8]", "x:bv[8] != y:bv[8]",
					"x:bv[8] < y:bv[8] && z = 0" }) {
			CAPTURE(s);
			auto r = tau_api::boole_normal_form(s);
			CHECK( r.has_value() );
		}
	}
#endif // TAU_PACK_HAS_BA_BV

	TEST_CASE_FIXTURE(api_fixture, "malformed input still rejected") {
		for (const char* s : { "", "x ) ( invalid !!!", "o[t] =" })
			CHECK( !tau_api::boole_normal_form(s).has_value() );
	}
}

// Coverage-driven additions (2026-08-01). api.tmpl.string.h measured 73.4% line
// / 31.0% branch -- the worst branch coverage of any file in src/. The cause is
// visible in the suites above: every string-API entry point had *malformed*
// input tests (AP-8) but almost none had a test for the SUCCESS return, so the
// `if (tref x = ...; x) return to_str(x);` line of each wrapper was cold.
//
// These suites cover those success returns. Where the exact output string is
// stable it is asserted directly, in the style of the boole_normal_form
// regressions above; otherwise the assertion is the round-trip invariant that
// actually matters for a string API -- what comes out must parse back in.
TEST_SUITE("Tau API - string - normal form success paths") {

	TEST_CASE_FIXTURE(api_fixture, "dnf / cnf / nnf return re-parseable formulas") {
		for (const char* s : { "x = 0 && (y = 0 || z = 0)",
					"x = 0 || y = 0",
					"!(x = 0 && y = 0)" }) {
			CAPTURE(s);
			auto d = tau_api::dnf(s);
			REQUIRE( d.has_value() );
			CHECK( tau_api::is_formula(*d) );
			auto c = tau_api::cnf(s);
			REQUIRE( c.has_value() );
			CHECK( tau_api::is_formula(*c) );
			auto n = tau_api::nnf(s);
			REQUIRE( n.has_value() );
			CHECK( tau_api::is_formula(*n) );
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "normalize_formula reduces a tautology") {
		auto taut = tau_api::normalize_formula("x = 0 || x != 0");
		REQUIRE( taut.has_value() );
		CHECK( *taut == "T" );
		auto contra = tau_api::normalize_formula("x = 0 && x != 0");
		REQUIRE( contra.has_value() );
		CHECK( *contra == "F" );
	}

	TEST_CASE_FIXTURE(api_fixture, "normalize_term returns a re-parseable term") {
		auto t = tau_api::normalize_term("x & x");
		REQUIRE( t.has_value() );
		CHECK( tau_api::is_term(*t) );
	}

	TEST_CASE_FIXTURE(api_fixture, "syntactic simplification success paths") {
		auto term = tau_api::syntactic_term_simplification("x & x");
		REQUIRE( term.has_value() );
		CHECK( tau_api::is_term(*term) );

		auto fm = tau_api::syntactic_formula_simplification(
			"x = 0 && x = 0");
		REQUIRE( fm.has_value() );
		CHECK( tau_api::is_formula(*fm) );
	}

	TEST_CASE_FIXTURE(api_fixture, "simplify success path") {
		auto s = tau_api::simplify("x & x", true);
		REQUIRE( s.has_value() );
		auto s2 = tau_api::simplify("x & x", false);
		REQUIRE( s2.has_value() );
	}
}

TEST_SUITE("Tau API - string - quantifier handling success paths") {

	TEST_CASE_FIXTURE(api_fixture, "anti_prenex returns a re-parseable formula") {
		auto a = tau_api::anti_prenex("ex x (x = 0 && y = 0)");
		REQUIRE( a.has_value() );
		CHECK( tau_api::is_formula(*a) );
	}

	// eliminate_quantifiers chains apply_all_defs -> anti_prenex ->
	// resolve_quantifiers; all three success lines were uncovered.
	TEST_CASE_FIXTURE(api_fixture, "eliminate_quantifiers removes the quantifier") {
		auto r = tau_api::eliminate_quantifiers("ex x x = 0");
		REQUIRE( r.has_value() );
		CHECK( tau_api::is_formula(*r) );
		CHECK( r->find("ex ") == std::string::npos );

		auto r2 = tau_api::eliminate_quantifiers("all x (x = 0 || x != 0)");
		REQUIRE( r2.has_value() );
		CHECK( r2->find("all ") == std::string::npos );
	}
}

TEST_SUITE("Tau API - string - substitution success paths") {

	TEST_CASE_FIXTURE(api_fixture, "substitute one term for another") {
		auto r = tau_api::substitute("x + y", "x", "z");
		REQUIRE( r.has_value() );
		CHECK( tau_api::is_term(*r) );
		CHECK( r->find('z') != std::string::npos );
		CHECK( r->find('x') == std::string::npos );
	}

	// The map overload loops, substituting each pair in turn.
	TEST_CASE_FIXTURE(api_fixture, "substitute several terms at once") {
		auto r = tau_api::substitute("x + y",
			std::map<std::string, std::string>{ {"x", "z"}, {"y", "w"} });
		REQUIRE( r.has_value() );
		CHECK( tau_api::is_term(*r) );
		CHECK( r->find('z') != std::string::npos );
		CHECK( r->find('w') != std::string::npos );
	}

	TEST_CASE_FIXTURE(api_fixture, "substitute rejects malformed arguments") {
		// Each of the three parse sites in the three-argument overload.
		CHECK( !tau_api::substitute("! ) (", "x", "z").has_value() );
		CHECK( !tau_api::substitute("x + y", "! ) (", "z").has_value() );
		CHECK( !tau_api::substitute("x + y", "x", "! ) (").has_value() );
	}

	TEST_CASE_FIXTURE(api_fixture, "substitute rejects a malformed pair") {
		CHECK( !tau_api::substitute("x + y",
			std::map<std::string, std::string>{ {"x", "! ) ("} })
			.has_value() );
		CHECK( !tau_api::substitute("x + y",
			std::map<std::string, std::string>{ {"! ) (", "z"} })
			.has_value() );
	}

	// apply_defs' success return was uncovered: the existing tests only feed
	// it malformed definitions.
	TEST_CASE_FIXTURE(api_fixture, "apply_defs expands a valid definition") {
		auto r = tau_api::apply_defs(
			std::set<std::string>{ "f(x) := x + 1" }, "f(y)");
		REQUIRE( r.has_value() );
		CHECK( !r->empty() );
	}
}

TEST_SUITE("Tau API - string - solving success paths") {

	// solve/lgrs each build a std::map<std::string, std::string> from the
	// tref-level solution; those five-line blocks were entirely uncovered.
	TEST_CASE_FIXTURE(api_fixture, "solve returns a variable to value map") {
		auto s = tau_api::solve("x = 0");
		REQUIRE( s.has_value() );
		CHECK( !s->empty() );
		for (const auto& [var, val] : *s) {
			CAPTURE(var);
			CAPTURE(val);
			CHECK( !var.empty() );
		}
	}

	// The string-level solve(formula, mode) overload's success path.
	// Typed sbf so the untyped default (tau, whose zero renders as "F")
	// does not change what "0" is checking here.
	TEST_CASE_FIXTURE(api_fixture, "solve with a mode renders the solution to strings") {
		auto s = tau_api::solve("x:sbf = 0", solver_mode::general);
		REQUIRE( s.has_value() );
		REQUIRE( s.value().size() == 1 );
		CHECK( s.value().begin()->first == "x" );
		CHECK( s.value().begin()->second == "0" );
	}

	// api::lgrs is NOT called here with a single equality such as "x = 0",
	// because it aborts. That abort is a known pre-existing bug (see the note
	// on the "lgrs" case in tests/api/test_api-tref_api.cpp) and it is
	// reachable from the string API too, not only from the tref overload.
	//
	// LOCALIZED (2026-08-01): the abort is src/api.tmpl.h:498-499, which does
	//     tau::get(eq)[0] ... tau::get(eq)[1]
	// where `eq` is the whole wff. For a single equality that wff has exactly
	// ONE child, so [1] is null and tree::operator[] -> child_tree() trips
	// `assert(c != nullptr)` (src/tau_tree.tmpl.h:579). The line above it
	// already extracted the equality itself into `equality`, whose two
	// children are the sides being checked for non-Boolean operations.
	//
	// Verified by experiment: substituting `equality` for `eq` on those two
	// lines makes lgrs("x = 0") both stop aborting AND return a solution.
	// The fix is left to src/ owners -- this is a tests-only change -- but it
	// is a two-token edit, not the norm_all_equations()/apply_all_xor_def()
	// preprocessing issue the older note in test_api-tref_api.cpp guesses at.
	//
	// A disjunction takes an earlier exit and so is safe to call: `equality`
	// comes back null (the top node is wff_or, not bf_eq) and lgrs reports
	// "Invalid argument(s)" gracefully. That guard is what is covered here.
	TEST_CASE_FIXTURE(api_fixture, "lgrs rejects a non-equality gracefully") {
		CHECK( !tau_api::lgrs("x = 0 || y = 0").has_value() );
	}

	// A single equality is the ordinary case: lgrs builds the general
	// solution and renders it to strings. Typed sbf so the untyped default
	// (tau, whose zero renders as "F") does not change what "0" is checking.
	TEST_CASE_FIXTURE(api_fixture, "lgrs renders the solution to strings") {
		auto s = tau_api::lgrs("x:sbf = 0");
		REQUIRE( s.has_value() );
		REQUIRE( s.value().size() == 1 );
		CHECK( s.value().begin()->first == "x" );
		CHECK( s.value().begin()->second == "0" );
	}

	TEST_CASE_FIXTURE(api_fixture, "solve and lgrs reject malformed input") {
		for (const char* s : { "", "x ) ( invalid !!!" }) {
			CAPTURE(s);
			CHECK( !tau_api::solve(s).has_value() );
			CHECK( !tau_api::lgrs(s).has_value() );
		}
	}
}

TEST_SUITE("Tau API - string - step error paths") {

	// step() looks each supplied input name up in the interpreter's context
	// and bails when it is absent. Previously uncovered.
	TEST_CASE("step rejects an unknown input stream name") {
		auto maybe_i = tau_api::get_interpreter("o[t] = i[t].");
		REQUIRE( maybe_i.has_value() );
		auto& i = maybe_i.value();
		std::map<stream_at, std::string> inputs;
		inputs[stream_at{ "no_such_stream", 0 }] = "T.";
		CHECK( !tau_api::step(i, inputs).has_value() );
	}

	// A syntactically valid stream name with an unparseable value hits the
	// "Failed to parse input value" branch. The BA's own parse failure
	// must survive as a report node in its own right, and step's own
	// node must name which stream and time point were refused.
	TEST_CASE("step rejects an unparseable input value") {
		auto maybe_i = tau_api::get_interpreter("o[t] = i[t].");
		REQUIRE( maybe_i.has_value() );
		auto& i = maybe_i.value();
		std::map<stream_at, std::string> inputs;
		inputs[stream_at{ "i", 0 }] = "! ) ( not a constant";
		auto r = tau_api::step(i, inputs);
		CHECK( !r.has_value() );
		CHECK( report_has_code(r.report(), code::parse_error) );

		bool found_ba_cause = false;
		bool found_step_summary = false;
		bool found_name_attr = false;
		bool found_time_point_attr = false;
		for (const auto& n : r.report().nodes()) {
			if (n.tag != code::parse_error) continue;
			std::string msg(r.report().str(n.key));
			if (msg.starts_with("Failed to parse input value"))
				found_step_summary = true;
			else if (!msg.empty())
				found_ba_cause = true;
			if (auto name = node_attr_text(r.report(), n, label::name)) {
				found_name_attr = true;
				CHECK( *name == "i" );
			}
			if (auto tp = node_attr_value(r.report(), n, label::time_point)) {
				found_time_point_attr = true;
				CHECK( *tp == 0 );
			}
		}
		// the BA's own reason (e.g. a parser error) is preserved as its
		// own node, distinct from step's summary line
		CHECK( found_ba_cause );
		CHECK( found_step_summary );
		CHECK( found_name_attr );
		CHECK( found_time_point_attr );
	}

	// The "this" pseudo-stream is skipped rather than looked up.
	TEST_CASE("step skips the \"this\" pseudo-stream") {
		auto maybe_i = tau_api::get_interpreter("o[t] = i[t].");
		REQUIRE( maybe_i.has_value() );
		auto& i = maybe_i.value();
		std::map<stream_at, std::string> inputs;
		inputs[stream_at{ "this", 0 }] = "ignored";
		inputs[stream_at{ "i", 0 }] = "T.";
		CHECK( tau_api::step(i, inputs).has_value() );
	}

	// get_interpreter's own parse-failure branch: note that "o[t] =" (used by
	// the "handle syntax error" case above) still parses as a spec and fails
	// later, so a harder malformed string is needed to reach it.
	TEST_CASE("get_interpreter reports spec parse failure") {
		CHECK( !tau_api::get_interpreter("x ) ( invalid !!!").has_value() );
	}

	// An input value that parses to a tau constant holding an open (free
	// variable) formula is rejected.
	TEST_CASE("step rejects an input constant with an open tau formula") {
		auto maybe_i = tau_api::get_interpreter("o[t] = i[t].");
		REQUIRE( maybe_i.has_value() );
		auto& i = maybe_i.value();
		std::map<stream_at, std::string> inputs;
		inputs[stream_at{ "i", 0 }] = "o[t] = x";
		CHECK( !tau_api::step(i, inputs).has_value() );
	}

	// A step with no inputs computes its outputs but reports "do not
	// auto-continue" by returning empty (the REPL then asks the user).
	TEST_CASE("step without inputs does not auto-continue") {
		auto maybe_i = tau_api::get_interpreter("o7[t] = 0.");
		REQUIRE( maybe_i.has_value() );
		auto& i = maybe_i.value();
		std::map<stream_at, std::string> inputs;
		CHECK( !tau_api::step(i, inputs).has_value() );
	}

	// A step whose u output proposes an acceptable update routes through
	// interpreter::update (the string API's own update call site).
	TEST_CASE("step performs a proposed spec update") {
		// i9/o8, not i1/o1: this suite shares one io context, and the
		// witness suite below re-types i1 as :bv[24]
		auto maybe_i = tau_api::get_interpreter(
			"u[t] = i9[t] && o8[t] = 0.");
		REQUIRE( maybe_i.has_value() );
		auto& i = maybe_i.value();
		std::map<stream_at, std::string> inputs;
		inputs[stream_at{ "i9", 0 }] = "F";
		auto out0 = tau_api::step(i, inputs);
		REQUIRE( out0.has_value() );
		inputs.clear();
		inputs[stream_at{ "i9", 1 }] = "o8[t] = 0";
		auto out1 = tau_api::step(i, inputs);
		REQUIRE( out1.has_value() );
		// the accepted update echoes on the u output
		bool u_echoed = false;
		for (auto& [at, v] : out1.value())
			if (at.name == "u"
				&& v == "always o8[t]:tau = 0") u_echoed = true;
		CHECK( u_echoed );
	}
}

#ifdef TAU_PACK_HAS_BA_BV
// GitHub #89: the witness the interpreter picks for an output the spec leaves
// free depended on what else the process had done before -- the same spec
// text stepped with the same inputs gave o9 = 0 in a fresh process and o9 = 7
// after another bitvector spec had run first. Clause order in the normalized
// step formula follows node ordering, node ordering follows the node hash,
// and a bv constant's hash was std::hash<cvc5::Term>, i.e. the term's
// creation id. That is what made the value differ between drivers (CLI vs
// bindings vs a spec-update router) in the report: each front-end creates a
// different set of terms before the spec's constants. Hashing constants by
// content makes the order, and so the witness, a function of the spec alone.
TEST_SUITE("Tau API - witness stability (#89)") {
	static std::vector<std::string> drive(const char* spec,
		const std::vector<std::string>& i1_values)
	{
		auto maybe_i = tau_api::get_interpreter(spec);
		REQUIRE(maybe_i.has_value());
		auto& i = maybe_i.value();
		std::vector<std::string> out;
		for (size_t t = 0; t < i1_values.size(); ++t) {
			std::map<stream_at, std::string> in;
			in[stream_at("i1", t)] = i1_values[t];
			auto maybe_out = tau_api::step(i, in);
			REQUIRE(maybe_out.has_value());
			for (auto& [at, v] : maybe_out.value())
				if (at.name == "o9") out.push_back(v);
		}
		return out;
	}

	TEST_CASE("free-region witness does not depend on prior activity") {
		const char* spec7 = "always i1[t]:bv[24] > { #x0003e8 }:bv[24]"
			" -> o9[t]:bv[24] = { #x000007 }:bv[24].";
		const char* spec50 = "always i1[t]:bv[24] > { #x0003e8 }:bv[24]"
			" -> o9[t]:bv[24] = { #x000032 }:bv[24].";
		const std::vector<std::string> in{ "2000", "500", "0" };
		// Only t=0 is constrained (2000 > 1000). At t=1 and t=2 the
		// implication is vacuous, o9 is free, and every satisfying
		// value is equally legal -- so the witness itself is not
		// pinned. Pinning it meant re-pinning on every parser regen,
		// since nonterminal renumbering moves node hashes and so
		// clause order, and it differed per platform as well.
		// What must hold is that the witness is a function of the spec
		// alone: the same spec, with a different bv spec driven in
		// between, must answer the same. That is the #89 regression --
		// it produced 7, 7, 7 after other activity where the spec on
		// its own gave 7, 0, 0.
		auto before = drive(spec7, in);
		auto other = drive(spec50, in);
		auto after = drive(spec7, in);
		REQUIRE(before.size() == 3);
		REQUIRE(other.size() == 3);
		REQUIRE(after.size() == 3);
		INFO("before: " << before[0] << "," << before[1] << ","
			<< before[2] << " after: " << after[0] << ","
			<< after[1] << "," << after[2] << " other: "
			<< other[0] << "," << other[1] << "," << other[2]);
		// the constrained step is the spec's own constant, not a choice
		CHECK(before[0] == "7");
		CHECK(after[0] == "7");
		CHECK(other[0] == "50");
		// a free step may be any bv[24], so that whole range is the
		// witness's legal set -- checked rather than pinned
		auto in_bv24 = [](const std::string& v) {
			if (v.empty() || v.size() > 8) return false;
			if (v.find_first_not_of("0123456789") != std::string::npos)
				return false;
			return std::stoul(v) <= ((1UL << 24) - 1);
		};
		for (size_t t = 1; t < 3; ++t) {
			CAPTURE(t);
			CHECK(in_bv24(before[t]));
			CHECK(in_bv24(after[t]));
			CHECK(in_bv24(other[t]));
		}
		// same spec, different prior activity, same free region --
		// checked both ways round, so neither spec is the only one
		// whose stability is tested
		auto other_after = drive(spec50, in);
		REQUIRE(other_after.size() == 3);
		CHECK(before == after);
		CHECK(other == other_after);
	}
}

// SAT-1 regression: the string overloads of sat/unsat/valid/realizable parse
// through get_spec_or_term, which yields a `spec` root for any formula. The
// tref overloads then either rejected that root (realizable required a bare
// wff, so sat() answered false for every satisfiable formula) or handed it
// whole to the normalizer (get_nso_rr only unwrapped a spec containing a
// ref), where the syntactic simplifier negated a non-wff and aborted Debug.
// Every case below is decided through the REPL with the same answers.
TEST_SUITE("Tau API - string - sat/valid decide plain formulas") {

	TEST_CASE_FIXTURE(api_fixture, "SAT-1: sat and unsat decide") {
		CHECK( tau_api::sat("x = 0 || x != 0").value() );
		CHECK( !tau_api::sat("x = 0 && x != 0").value() );
		CHECK( tau_api::unsat("x = 0 && x != 0").value() );
		CHECK( tau_api::sat("ex x:bv[8] x = {0}:bv[8]").value() );
		CHECK( tau_api::sat("ex x:bv[8] (x = {0}:bv[8] && x / x = {255}:bv[8])").value() );
		CHECK( !tau_api::sat("ex x:bv[8] (x = {0}:bv[8] && x / x = {1}:bv[8])").value() );
	}

	TEST_CASE_FIXTURE(api_fixture, "SAT-1: valid decides, quantified bv included") {
		CHECK( tau_api::valid("x = 0 || x != 0").value() );
		CHECK( !tau_api::valid("x = 0").value() );
		CHECK( tau_api::valid("all x:bv[8] (x != {0}:bv[8] -> x / x = {1}:bv[8])").value() );
		CHECK( !tau_api::valid("all x:bv[8] x / x = {1}:bv[8]").value() );
	}

	TEST_CASE_FIXTURE(api_fixture, "SAT-1: realizable decides") {
		CHECK( tau_api::realizable("x = 0 || x != 0").value() );
		CHECK( tau_api::unrealizable("x = 0 && x != 0").value() );
	}
}
#endif // TAU_PACK_HAS_BA_BV

TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
