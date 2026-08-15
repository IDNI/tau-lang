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
		CHECK(tau_api::sat("x = 0"));
		CHECK(tau_api::realizable("x = 0"));
		CHECK(!tau_api::unsat("x = 0"));
		CHECK(!tau_api::unrealizable("x = 0"));

		// a genuine contradiction
		CHECK(!tau_api::sat("x = 0 && x != 0"));
		CHECK(tau_api::unsat("x = 0 && x != 0"));
		CHECK(!tau_api::realizable("x = 0 && x != 0"));
		CHECK(tau_api::unrealizable("x = 0 && x != 0"));

		// a tautology
		CHECK(tau_api::valid("x = x"));
		CHECK(tau_api::valid_spec("x = x"));
		CHECK(!tau_api::valid("x = 0"));
		CHECK(!tau_api::valid_spec("x = 0"));
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


	// §4e item 8: api<node>::solve(const string&) rendered every solved
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


TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
