// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

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
			// realizable/valid/valid_spec/sat all bottom out on a null
			// tref, which every one of them treats as "not satisfied".
			CHECK(!tau_api::realizable(s));
			CHECK(!tau_api::valid(s));
			CHECK(!tau_api::valid_spec(s));
			CHECK(!tau_api::sat(s));
			// unrealizable/unsat are defined as the negation of the
			// above, so a malformed input reports true here.
			CHECK(tau_api::unrealizable(s));
			CHECK(tau_api::unsat(s));
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

	TEST_CASE("handle type error") {
		auto maybe_i = tau_api::get_interpreter("o[t]:tau = i[t]:sbf");
		CHECK(!maybe_i.has_value());
	}


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
			auto inputs = tau_api::get_inputs_for_step(i);
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
	// "Failed to parse input value" branch.
	TEST_CASE("step rejects an unparseable input value") {
		auto maybe_i = tau_api::get_interpreter("o[t] = i[t].");
		REQUIRE( maybe_i.has_value() );
		auto& i = maybe_i.value();
		std::map<stream_at, std::string> inputs;
		inputs[stream_at{ "i", 0 }] = "! ) ( not a constant";
		CHECK( !tau_api::step(i, inputs).has_value() );
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

	// The string-level solve(formula, mode) overload's success path: the
	// solution map is rendered to strings.
	TEST_CASE_FIXTURE(api_fixture, "solve with a mode renders the solution to strings") {
		auto s = tau_api::solve("x = 0", solver_mode::general);
		REQUIRE( s.has_value() );
		REQUIRE( s.value().size() == 1 );
		CHECK( s.value().begin()->first == "x" );
		CHECK( s.value().begin()->second == "0" );
	}

	// The string-level lgrs(equation) success path: the general solution
	// map is rendered to strings.
	TEST_CASE_FIXTURE(api_fixture, "lgrs renders the solution to strings") {
		auto s = tau_api::lgrs("x = 0");
		REQUIRE( s.has_value() );
		REQUIRE( s.value().size() == 1 );
		CHECK( s.value().begin()->first == "x" );
		CHECK( s.value().begin()->second == "0" );
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
		// Another bv spec runs first, so its constants get their cvc5
		// term ids before spec7's `7` does -- the report's driver
		// difference in miniature.
		auto other = drive(spec50, in);
		REQUIRE(other.size() == 3);
		CHECK(other[0] == "50");
		// The free-region witness must be what a fresh process gives
		// for this spec on its own. With content-hashed constants the
		// `o9 = 7` clause sorts first, and being satisfiable for every
		// input it is the path taken in the free region too. (Before the
		// fix this sequence produced 7, 7, 7 while a fresh process gave
		// 7, 0, 0 -- the report's split.)
		auto witness = drive(spec7, in);
		// The canonical free-region choice moves with every parser regen
		// (nonterminal renumbering changes node hashes and so clause
		// order). Re-pinned after the 2026-09-02 min/max regen: a fresh
		// process gives 7, 0, 0 for spec7 and 50, 50, 50 for spec50 on
		// their own (verified by driving each alone through the REPL),
		// and so must these post-activity runs. Re-pin whenever the
		// grammar is regenerated; the property under test is
		// fresh == post-activity, not the specific witness.
		INFO("witness: " << witness[0] << "," << witness[1] << ","
			<< witness[2] << " other: " << other[0] << ","
			<< other[1] << "," << other[2]);
		CHECK(witness == std::vector<std::string>({ "7", "0", "0" }));
		CHECK(other == std::vector<std::string>({ "50", "50", "50" }));
	}
}
