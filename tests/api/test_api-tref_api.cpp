// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include <sstream>

#include "test_init.h"
#include "test_tau_helpers.h"

using tau_api = api<node_t>;

#include "test_api-fixture.h"

TEST_SUITE("Tau API - settings") {

	TEST_CASE("set_charvar") {
		tau_api::set_charvar(false);
		tref xyz = tau_api::get_term("xyz");
		CHECK(xyz); // term must be variable, because charvar is false
		CHECK(tau::get(xyz)[0].is(tau::variable));

		tau_api::set_charvar(true);
		xyz = tau_api::get_term("xyz");
		CHECK(xyz); // term must be bf_and, because charvar is true
		CHECK(tau::get(xyz)[0].is(tau::bf_and));
	}

	TEST_CASE("set_indenting") {
		// tau_api::set_indenting(true); // TODO
		tref fm = tau_api::get_formula("x = 0 && z = 0 || y = 1");
		std::string got = tau_api::to_str(fm);
		std::string expected = "x = 0 && z = 0 || y = 1";
		CHECK(got == expected);
		tau_api::set_indenting(false);
		got = tau_api::to_str(fm);
		expected = "x = 0 && z = 0 || y = 1";
		CHECK(got == expected);
	}

	TEST_CASE("set_highlighting") {
		// tau_api::set_highlighting(true); // TODO
		tref fm = tau_api::get_formula("x = 0 && z = 0 || y = 1");
		std::string got = tau_api::to_str(fm);
		std::string expected = "x = 0 && z = 0 || y = 1";
		CHECK(got == expected);
		tau_api::set_highlighting(false);
		fm = tau_api::get_formula("x = 0 && z = 0 || y = 1");
		got = tau_api::to_str(fm);
		expected = "x = 0 && z = 0 || y = 1";
		CHECK(got == expected);
	}

	TEST_CASE("set_severity") {
		auto original_severity = logging::level();
		tau_api::set_severity(boost::log::trivial::trace);
		CHECK(logging::level() == boost::log::trivial::trace);
		tau_api::set_severity(boost::log::trivial::debug);
		CHECK(logging::level() == boost::log::trivial::debug);
		tau_api::set_severity(boost::log::trivial::info);
		CHECK(logging::level() == boost::log::trivial::info);
		tau_api::set_severity(boost::log::trivial::error);
		CHECK(logging::level() == boost::log::trivial::error);
		tau_api::set_severity(original_severity);
		CHECK(logging::level() == original_severity);
	}

	// AP-34: set_json toggles measuring::operator() between the plain-text
	// and JSON renderings; nothing anywhere exercised it.
	TEST_CASE("set_json") {
		tref t = tau_api::get_formula("T");
		REQUIRE(t);

		tau_api::set_json(false);
		measuring text_m;
		CHECK(tau_api::sat(text_m, t));
		std::ostringstream text_out;
		text_out << text_m;
		CHECK(text_out.str().find('{') == std::string::npos);
		CHECK(text_out.str().find("sat") != std::string::npos);

		tau_api::set_json(true);
		measuring json_m;
		CHECK(tau_api::sat(json_m, t));
		std::ostringstream json_out;
		json_out << json_m;
		CHECK(json_out.str().find("\"measured\": \"sat\"") != std::string::npos);

		// restore the default so later tests are not affected
		tau_api::set_json(false);
	}
}

TEST_SUITE("Tau API - tref - parsing") {

	TEST_CASE_FIXTURE(api_fixture, "get_term") {
		for (const auto& term : terms) {
			DBG(TAU_LOG_TRACE << "get_term: " << term;)
			CHECK(tau_api::get_term(term));
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "get_formula") {
		for (const auto& formula : formulas) {
			DBG(TAU_LOG_TRACE << "get_formula: " << formula;)
			CHECK(tau_api::get_formula(formula));
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "get_function_def") {
		for (const auto& function_def : function_defs) {
			DBG(TAU_LOG_TRACE << "get_function_def: " << function_def;)
			CHECK(tau_api::get_function_def(function_def));
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "get_predicate_def") {
		for (const auto& predicate_def : predicate_defs) {
			DBG(TAU_LOG_TRACE << "get_predicate_def: " << predicate_def;)
			CHECK(tau_api::get_predicate_def(predicate_def));
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "get_stream_def") {
		for (const auto& stream_def : stream_defs) {
			DBG(TAU_LOG_TRACE << "get_stream_def: " << stream_def;)
			CHECK(tau_api::get_stream_def(stream_def));
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "get_spec") {
		for (const auto& spec : specs) {
			DBG(TAU_LOG_TRACE << "get_spec: " << spec;)
			CHECK(tau_api::get_spec(spec));
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "get_definition") {
		for (const auto& definition : predicate_defs) {
			DBG(TAU_LOG_TRACE << "get_definition - predicate_def: " << definition;)
			CHECK(tau_api::get_definition(definition));
		}
		for (const auto& function_def : function_defs) {
			DBG(TAU_LOG_TRACE << "get_definition - function_def: " << function_def;)
			CHECK(tau_api::get_definition(function_def));
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "get_definition with malformed input returns nullptr") {
		// Regression test for AP-N2: get_definition() used to dereference
		// the parse result unconditionally, crashing (DBG assert / null
		// deref in release) instead of returning nullptr on a malformed
		// definition string.
		CHECK(tau_api::get_definition("this is not a definition") == nullptr);
	}

	TEST_CASE_FIXTURE(api_fixture, "get_spec_or_term") {
		for (const auto& spec : specs) {
			DBG(TAU_LOG_TRACE << "get_spec_or_term - spec: " << spec;)
			tref s = tau_api::get_spec_or_term(spec);
			CHECK(s);
			CHECK(tau::get(s).is(tau::spec));
		}
		for (const auto& term : terms) {
			DBG(TAU_LOG_TRACE << "get_spec_or_term - term: " << term;)
			tref t = tau_api::get_spec_or_term(term);
			CHECK(t);
			CHECK(tau::get(t).is(tau::bf));
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "get_formula_or_term") {
		for (const auto& formula : formulas) {
			DBG(TAU_LOG_TRACE << "get_formula_or_term - formula: " << formula;)
			tref f = tau_api::get_formula_or_term(formula);
			CHECK(f);
			DBG(using node = node_t;)
			DBG(TAU_LOG_TRACE << "get_formula_or_term - formula: " << TAU_LOG_FM_DUMP(f);)
			CHECK(tau::get(f).is(tau::wff));
		}
		for (const auto& term : terms) {
			DBG(TAU_LOG_TRACE << "get_formula_or_term - term: " << term;)
			tref t = tau_api::get_formula_or_term(term);
			CHECK(t);
			CHECK(tau::get(t).is(tau::bf));
		}
	}
}

TEST_SUITE("Tau API - tref - querying") {

	TEST_CASE_FIXTURE(api_fixture, "contains") {
		DBG(using node = node_t;)
		tref t = tau_api::get_term("x + 0", false);
		DBG(TAU_LOG_TRACE << "contains on term: " << TAU_LOG_FM_DUMP(t);)
		CHECK(tau_api::contains(t, tau::bf));
		CHECK(tau_api::contains(t, tau::bf_add));
		CHECK(tau_api::contains(t, tau::variable));
		CHECK(tau_api::contains(t, tau::bf_f));
		CHECK(!tau_api::contains(t, tau::bf_t));
		CHECK(!tau_api::contains(t, tau::wff));
		CHECK(!tau_api::contains(t, tau::wff_f));
		CHECK(!tau_api::contains(t, tau::wff_t));

		tref f = tau_api::get_formula("x = 0 && y ^ 1 = 1", false);
		DBG(TAU_LOG_TRACE << "contains on formula: " << TAU_LOG_FM_DUMP(f);)
		CHECK(tau_api::contains(f, tau::wff));
		CHECK(tau_api::contains(f, tau::wff_and));
		CHECK(tau_api::contains(f, tau::bf_eq));
		CHECK(tau_api::contains(f, tau::bf_xor));
		CHECK(tau_api::contains(f, tau::variable));
		CHECK(tau_api::contains(f, tau::bf));
		CHECK(tau_api::contains(f, tau::bf_f));
		CHECK(tau_api::contains(f, tau::bf_t));
		CHECK(!tau_api::contains(f, tau::wff_t));
		CHECK(!tau_api::contains(f, tau::wff_f));
		CHECK(!tau_api::contains(f, tau::spec));
	}

	TEST_CASE_FIXTURE(api_fixture, "is_term") {
		for (const auto& term : terms) {
			DBG(TAU_LOG_TRACE << "is_term: " << term;)
			tref t = tau_api::get_formula_or_term(term);
			CHECK(tau_api::is_term(t));
			CHECK(!tau_api::is_formula(t));
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "is_formula") {
		for (const auto& formula : formulas) {
			DBG(TAU_LOG_TRACE << "is_formula: " << formula;)
			tref f = tau_api::get_formula_or_term(formula);
			CHECK(tau_api::is_formula(f));
			CHECK(!tau_api::is_term(f));
		}
	}

}

TEST_SUITE("Tau API - tref - using definitions") {
	// Definitions are registered into a process-wide, persistent registry
	// (definitions<node>::instance()) as soon as they are parsed, so each
	// test below uses symbol names not used anywhere else in this file to
	// avoid clashing with definitions registered by other test cases.
	TEST_CASE_FIXTURE(api_fixture, "apply_def") {
		tref def = tau_api::get_function_def("apply_def_f(x) := x + 1");
		REQUIRE(def);
		tref expr = tau_api::get_term("apply_def_f(t)", false);
		REQUIRE(expr);
		// before applying the definition, the call to `apply_def_f` is
		// still an unresolved reference
		CHECK(tau_api::contains(expr, tau::ref));
		tref applied = tau_api::apply_def(def, expr);
		REQUIRE(applied);
		CHECK(!tau_api::contains(applied, tau::ref));
	}
	TEST_CASE_FIXTURE(api_fixture, "apply_defs") {
		tref f = tau_api::get_function_def("apply_defs_f(x) := x + 1");
		tref g = tau_api::get_function_def("apply_defs_g(x) := x'");
		REQUIRE(f);
		REQUIRE(g);
		tref expr = tau_api::get_term(
			"apply_defs_f(t) + apply_defs_g(t)", false);
		REQUIRE(expr);
		CHECK(tau_api::contains(expr, tau::ref));
		tref applied = tau_api::apply_defs(
			subtree_set<node_t>{ f, g }, expr);
		REQUIRE(applied);
		CHECK(!tau_api::contains(applied, tau::ref));
	}
	TEST_CASE_FIXTURE(api_fixture, "apply_all_defs") {
		// apply_all_defs relies on rec_relations declared within the
		// expression itself (as opposed to apply_def/apply_defs, which
		// take the definitions explicitly), so a full spec is needed
		// here. It must be built via get_spec() specifically: get_spec()
		// returns a properly-typed `tau::spec` node, which is the shape
		// apply_all_defs' internals require to find the embedded
		// definitions -- a spec parsed via the generic tau::get() stays
		// wrapped in a `tau::start` node and is treated as an opaque,
		// definition-free expression instead. tau_spec parses one
		// spec_part per line, so the definition and the main clause must
		// be newline-separated (unlike the REPL's ". "-separated
		// multi-command syntax).
		tref spec = tau_api::get_spec(
			"apply_all_defs_f(x) := x'.\napply_all_defs_f(z) = 0.");
		REQUIRE(spec);
		CHECK(tau_api::contains(spec, tau::ref));
		tref applied = tau_api::apply_all_defs(spec);
		REQUIRE(applied);
		CHECK(tau_api::to_str(applied) == "z' = 0");
		CHECK(!tau_api::contains(applied, tau::ref));
	}
}

TEST_SUITE("Tau API - tref - printing") {
	TEST_CASE_FIXTURE(api_fixture, "print") {
		tref fm = tau_api::get_formula("x = 0 && y = 1");
		REQUIRE(fm);
		std::ostringstream os;
		tau_api::print(os, fm);
		CHECK(!os.str().empty());
		CHECK(os.str() == tau_api::to_str(fm));
		// a null expression must not crash and must print nothing
		std::ostringstream null_os;
		tau_api::print(null_os, nullptr);
		CHECK(null_os.str().empty());
	}
	TEST_CASE_FIXTURE(api_fixture, "to_str") {
		tref fm = tau_api::get_formula("x = 0 && y = 1");
		REQUIRE(fm);
		CHECK(tau_api::to_str(fm) == "x = 0 && y = 1");
		CHECK(tau_api::to_str(nullptr) == "");
	}
}

TEST_SUITE("Tau API - tref - substitution") {
	TEST_CASE_FIXTURE(api_fixture, "substitute") {
		tref x = tau_api::get_term("x");
		tref y = tau_api::get_term("y");
		tref expr = tau_api::get_term("x + 1");
		REQUIRE(x);
		REQUIRE(y);
		REQUIRE(expr);
		tref result = tau_api::substitute(expr, x, y);
		REQUIRE(result);
		CHECK(tau_api::to_str(result) == "y+1");
	}
	TEST_CASE_FIXTURE(api_fixture, "substitute map") {
		tref x = tau_api::get_term("x");
		tref y = tau_api::get_term("y");
		tref a = tau_api::get_term("a");
		tref b = tau_api::get_term("b");
		tref expr = tau_api::get_term("x + a");
		REQUIRE(x);
		REQUIRE(y);
		REQUIRE(a);
		REQUIRE(b);
		REQUIRE(expr);
		std::map<tref, tref> that_with{ { x, y }, { a, b } };
		tref result = tau_api::substitute(expr, that_with);
		REQUIRE(result);
		CHECK(tau_api::to_str(result) == "y+b");
	}

	// AP-6: substitute(tref, tref, tref) called is_term() on expr/that/with
	// unconditionally; is_term() dereferences its argument, so a null tref
	// asserted (debug) or was UB (release) instead of returning nullptr.
	TEST_CASE_FIXTURE(api_fixture, "substitute with a null argument returns nullptr") {
		tref x = tau_api::get_term("x");
		REQUIRE(x != nullptr);
		CHECK(tau_api::substitute(nullptr, x, x) == nullptr);
		CHECK(tau_api::substitute(x, nullptr, x) == nullptr);
		CHECK(tau_api::substitute(x, x, nullptr) == nullptr);
	}
}

TEST_SUITE("Tau API - tref - normal forms") {
	TEST_CASE_FIXTURE(api_fixture, "boole_normal_form") {
		tref fm = tau_api::get_formula("x = 0 && y = 1");
		REQUIRE(fm);
		tref bnf = tau_api::boole_normal_form(fm);
		REQUIRE(bnf);
		CHECK(tau_api::contains(bnf, tau::wff));
	}
	TEST_CASE_FIXTURE(api_fixture, "dnf") {
		tref fm = tau_api::get_formula("(x = 0 || y = 0) && z = 0");
		REQUIRE(fm);
		tref d = tau_api::dnf(fm);
		REQUIRE(d);
		// distributing && over || yields a top-level disjunction
		CHECK(tau_api::contains(d, tau::wff_or));
	}
	TEST_CASE_FIXTURE(api_fixture, "cnf") {
		tref fm = tau_api::get_formula("x = 0 || (y = 0 && z = 0)");
		REQUIRE(fm);
		tref c = tau_api::cnf(fm);
		REQUIRE(c);
		// distributing || over && yields a top-level conjunction
		CHECK(tau_api::contains(c, tau::wff_and));
	}
	TEST_CASE_FIXTURE(api_fixture, "nnf") {
		tref fm = tau_api::get_formula("!(x = 0 && y = 0)");
		REQUIRE(fm);
		tref n = tau_api::nnf(fm);
		REQUIRE(n);
		// De Morgan must have pushed the negation to the leaves, turning
		// the negated conjunction into a disjunction of negated atoms
		CHECK(tau_api::contains(n, tau::wff_or));
	}
}

TEST_SUITE("Tau API - tref - procedures") {
	TEST_CASE_FIXTURE(api_fixture, "simplify") {
		tref t = tau_api::get_term("x", false);
		REQUIRE(t);
		tref s = tau_api::simplify(t);
		REQUIRE(s);
		CHECK(tau_api::to_str(s) == "x");
		// simplify must be idempotent on an already-simplified term
		CHECK(tau_api::simplify(s) == s);
		CHECK(tau_api::simplify(nullptr) == nullptr);
	}
	TEST_CASE_FIXTURE(api_fixture, "infer") {
		tref t = tau_api::get_term("x", false);
		REQUIRE(t);
		tref inferred = tau_api::infer(t);
		REQUIRE(inferred);
		CHECK(tau_api::to_str(inferred) == "x");
		CHECK(tau_api::infer(nullptr) == nullptr);
	}
	TEST_CASE_FIXTURE(api_fixture, "syntactic_term_simplification") {
		tref t = tau_api::get_term("x & x", false);
		REQUIRE(t);
		tref s = tau_api::syntactic_term_simplification(t);
		REQUIRE(s);
		CHECK(tau_api::to_str(s) == "x");
	}
	TEST_CASE_FIXTURE(api_fixture, "syntactic_formula_simplification") {
		tref fm = tau_api::get_formula("x = 0 && x = 0", false);
		REQUIRE(fm);
		tref s = tau_api::syntactic_formula_simplification(fm);
		REQUIRE(s);
		CHECK(tau_api::to_str(s) == "x = 0");
	}
	TEST_CASE_FIXTURE(api_fixture, "normalize_term") {
		tref t = tau_api::get_term("x & x'");
		REQUIRE(t);
		tref n = tau_api::normalize_term(t);
		REQUIRE(n);
		CHECK(tau_api::to_str(n) == "0");
	}
	TEST_CASE_FIXTURE(api_fixture, "normalize_formula") {
		tref fm = tau_api::get_formula("x & x' = 0");
		REQUIRE(fm);
		tref n = tau_api::normalize_formula(fm);
		REQUIRE(n);
		CHECK(tau_api::to_str(n) == "T");
	}
	TEST_CASE_FIXTURE(api_fixture, "anti_prenex") {
		// r(x, y) is an undefined predicate reference, so the existential
		// cannot be resolved away as a trivially-closed subformula; it
		// must survive anti_prenex (possibly reordered/renamed).
		tref fm = tau_api::get_formula("(ex x r(x, y)) && z = 0");
		REQUIRE(fm);
		tref a = tau_api::anti_prenex(fm);
		REQUIRE(a);
		CHECK(tau_api::contains(a, tau::wff_ex));
	}
	TEST_CASE_FIXTURE(api_fixture, "eliminate_quantifiers") {
		tref fm = tau_api::get_formula("ex x x = 0");
		REQUIRE(fm);
		tref e = tau_api::eliminate_quantifiers(fm);
		REQUIRE(e);
		CHECK(!tau_api::contains(e, tau::wff_ex));
	}
	TEST_CASE_FIXTURE(api_fixture, "realizable") {
		// realizable/unrealizable require a plain formula (is_formula()
		// gates on a wff node), not a full multi-clause spec.
		tref fm = tau_api::get_formula("x = 0");
		REQUIRE(fm);
		CHECK(tau_api::realizable(fm));
	}
	TEST_CASE_FIXTURE(api_fixture, "unrealizable") {
		tref fm = tau_api::get_formula("F");
		REQUIRE(fm);
		CHECK(tau_api::unrealizable(fm));
	}
	TEST_CASE_FIXTURE(api_fixture, "sat") {
		tref t = tau_api::get_formula("T");
		tref f = tau_api::get_formula("F");
		REQUIRE(t);
		REQUIRE(f);
		CHECK(tau_api::sat(t));
		CHECK(!tau_api::sat(f));
	}
	TEST_CASE_FIXTURE(api_fixture, "unsat") {
		tref t = tau_api::get_formula("T");
		tref f = tau_api::get_formula("F");
		REQUIRE(t);
		REQUIRE(f);
		CHECK(tau_api::unsat(f));
		CHECK(!tau_api::unsat(t));
	}
	TEST_CASE_FIXTURE(api_fixture, "valid") {
		tref t = tau_api::get_formula("T");
		tref x = tau_api::get_formula("x = 0");
		REQUIRE(t);
		REQUIRE(x);
		CHECK(tau_api::valid(t));
		CHECK(!tau_api::valid(x));
	}
	TEST_CASE_FIXTURE(api_fixture, "valid_spec") {
		tref t = tau_api::get_formula("T");
		tref x = tau_api::get_formula("x = 0");
		REQUIRE(t);
		REQUIRE(x);
		CHECK(tau_api::valid_spec(t));
		// x = 0 is not logically equivalent to T
		CHECK(!tau_api::valid_spec(x));
	}
}

TEST_SUITE("Tau API - tref - solving") {
	TEST_CASE_FIXTURE(api_fixture, "solve") {
		tref eq = tau_api::get_formula("x | y = 0");
		REQUIRE(eq);
		auto solution = tau_api::solve(eq, solver_mode::general);
		REQUIRE(solution.has_value());
		CHECK(!solution.value().empty());
	}
	TEST_CASE_FIXTURE(api_fixture, "lgrs") {
		// NOTE: as of this writing, tau_api::lgrs(tref) aborts (assertion
		// failure in tree<node>::child_tree, hit inside its internal
		// norm_all_equations()/apply_all_xor_def() preprocessing) for
		// every non-null equation tried here -- reproducible directly via
		// the REPL's `lgrs` command too (e.g. `tau -e "lgrs x = y"`),
		// independent of this test change. That is a pre-existing bug in
		// src/ and out of scope for a tests-only change, so only the
		// documented graceful-failure path is exercised here; the
		// underlying `subtree_map` solving machinery itself is already
		// covered by the "solve" test above and by the "lgrs" test suite
		// in tests/integration/test_integration-solver.cpp (which calls
		// the free `lgrs<node>()` function directly, bypassing this
		// wrapper).
		auto solution = tau_api::lgrs(static_cast<tref>(nullptr));
		CHECK(!solution.has_value());
	}
}

TEST_SUITE("Tau API - tref - execution") {
	TEST_CASE_FIXTURE(api_fixture, "get_interpreter") {
		// NOTE: a `tau::spec`-typed tref built via get_spec() (as opposed
		// to a plain formula from get_formula()) currently makes
		// get_interpreter(tref) abort (assertion failure in
		// build_wff_neg), reproducible outside of this test too -- a
		// pre-existing bug in api<node>::get_nso_rr's `type == tau::spec`
		// dispatch branch, out of scope for a tests-only change. The
		// plain-formula path used by the REPL's `run` command works fine
		// and is what is exercised here; explicit `:tau` type
		// annotations are required since a bare formula (unlike a parsed
		// spec) never gets its stream types pushed into the global
		// io_context.
		tref fm = tau_api::get_formula("o[t]:tau = i[t]:tau");
		REQUIRE(fm);
		auto maybe_i = tau_api::get_interpreter(fm);
		CHECK(maybe_i.has_value());
	}
}

// AP-2: htref is a shared_ptr; every htref overload except nnf and
// eliminate_quantifiers dereferenced it (expr->get()) unconditionally, so a
// null htref was UB instead of a safe default return. A representative
// sample across the different return-type shapes (bool, htref, string,
// optional<map>) is checked here rather than exhaustively covering all ~26
// overloads.
TEST_SUITE("Tau API - htref - null guards (AP-2)") {
	// tau_api::is_term/dnf/solve/etc. are each overloaded on tref and on
	// htref; a bare `nullptr` literal is ambiguous between "null raw
	// pointer" and "null shared_ptr" but overload resolution picks the
	// tref version, silently missing the htref overload under test. Use
	// an explicit htref{} to unambiguously exercise the htref overload.
	TEST_CASE_FIXTURE(api_fixture, "bool-returning overloads reject a null htref") {
		CHECK(tau_api::is_term(htref{}) == false);
		CHECK(tau_api::is_formula(htref{}) == false);
		CHECK(tau_api::sat(htref{}) == false);
		CHECK(tau_api::valid(htref{}) == false);
	}

	TEST_CASE_FIXTURE(api_fixture, "htref-returning overloads reject a null htref") {
		CHECK(tau_api::dnf(htref{}) == nullptr);
		CHECK(tau_api::cnf(htref{}) == nullptr);
		CHECK(tau_api::simplify(htref{}) == nullptr);
	}

	TEST_CASE_FIXTURE(api_fixture, "to_str rejects a null htref") {
		CHECK(tau_api::to_str(htref{}) == "");
	}

	TEST_CASE_FIXTURE(api_fixture, "solve/lgrs reject a null htref") {
		CHECK(!tau_api::solve(htref{}, solver_mode::general).has_value());
		CHECK(!tau_api::lgrs(htref{}).has_value());
	}

	TEST_CASE_FIXTURE(api_fixture, "substitute rejects null htref arguments") {
		htref x = tau_api::geth_term("x");
		REQUIRE(x != nullptr);
		CHECK(tau_api::substitute(htref{}, x, x) == nullptr);
		CHECK(tau_api::substitute(x, htref{}, x) == nullptr);
		CHECK(tau_api::substitute(x, x, htref{}) == nullptr);
	}
}
