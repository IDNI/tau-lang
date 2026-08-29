// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include <sstream>

#include "test_init.h"
#include "test_tau_helpers.h"

using tau_api = api<node_t>;

#include "test_api-fixture.h"

TEST_SUITE("Tau API - settings") {

	TEST_CASE("set_charvar") {
		tau_api::set_charvar(false);
		auto r = tau_api::get_term("xyz");
		CHECK(r.has_value());
		tref xyz = r.value();
		CHECK(xyz); // term must be variable, because charvar is false
		CHECK(tau::get(xyz)[0].is(tau::variable));

		tau_api::set_charvar(true);
		r = tau_api::get_term("xyz");
		CHECK(r.has_value());
		xyz = r.value();
		CHECK(xyz); // term must be bf_and, because charvar is true
		CHECK(tau::get(xyz)[0].is(tau::bf_and));
	}

	TEST_CASE("set_indenting") {
		// tau_api::set_indenting(true); // TODO
		auto r = tau_api::get_formula("x = 0 && z = 0 || y = 1");
		CHECK(r.has_value());
		tref fm = r.value();
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
		auto r = tau_api::get_formula("x = 0 && z = 0 || y = 1");
		CHECK(r.has_value());
		tref fm = r.value();
		std::string got = tau_api::to_str(fm);
		std::string expected = "x = 0 && z = 0 || y = 1";
		CHECK(got == expected);
		tau_api::set_highlighting(false);
		r = tau_api::get_formula("x = 0 && z = 0 || y = 1");
		CHECK(r.has_value());
		fm = r.value();
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
}

TEST_SUITE("Tau API - tref - parsing") {

	TEST_CASE_FIXTURE(api_fixture, "get_term") {
		for (const auto& term : terms) {
			DBG(TAU_LOG_TRACE << "get_term: " << term;)
			CHECK(tau_api::get_term(term).has_value());
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "get_formula") {
		for (const auto& formula : formulas) {
			DBG(TAU_LOG_TRACE << "get_formula: " << formula;)
			CHECK(tau_api::get_formula(formula).has_value());
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "get_function_def") {
		for (const auto& function_def : function_defs) {
			DBG(TAU_LOG_TRACE << "get_function_def: " << function_def;)
			CHECK(tau_api::get_function_def(function_def).has_value());
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "get_predicate_def") {
		for (const auto& predicate_def : predicate_defs) {
			DBG(TAU_LOG_TRACE << "get_predicate_def: " << predicate_def;)
			CHECK(tau_api::get_predicate_def(predicate_def).has_value());
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "get_stream_def") {
		for (const auto& stream_def : stream_defs) {
			DBG(TAU_LOG_TRACE << "get_stream_def: " << stream_def;)
			CHECK(tau_api::get_stream_def(stream_def).has_value());
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "get_spec") {
		for (const auto& spec : specs) {
			DBG(TAU_LOG_TRACE << "get_spec: " << spec;)
			CHECK(tau_api::get_spec(spec).has_value());
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "get_definition") {
		for (const auto& definition : predicate_defs) {
			DBG(TAU_LOG_TRACE << "get_definition - predicate_def: " << definition;)
			CHECK(tau_api::get_definition(definition).has_value());
		}
		for (const auto& function_def : function_defs) {
			DBG(TAU_LOG_TRACE << "get_definition - function_def: " << function_def;)
			CHECK(tau_api::get_definition(function_def).has_value());
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "get_definition with malformed input returns an error") {
		// Regression test for AP-N2: get_definition() used to dereference
		// the parse result unconditionally, crashing (DBG assert / null
		// deref in release) instead of returning nullptr on a malformed
		// definition string.
		CHECK(!tau_api::get_definition("this is not a definition").has_value());
	}

	TEST_CASE_FIXTURE(api_fixture, "get_spec_or_term") {
		for (const auto& spec : specs) {
			DBG(TAU_LOG_TRACE << "get_spec_or_term - spec: " << spec;)
			auto r = tau_api::get_spec_or_term(spec);
			CHECK(r.has_value());
			tref s = r.value();
			CHECK(s);
			CHECK(tau::get(s).is(tau::spec));
		}
		for (const auto& term : terms) {
			DBG(TAU_LOG_TRACE << "get_spec_or_term - term: " << term;)
			auto r = tau_api::get_spec_or_term(term);
			CHECK(r.has_value());
			tref t = r.value();
			CHECK(t);
			CHECK(tau::get(t).is(tau::bf));
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "get_formula_or_term") {
		for (const auto& formula : formulas) {
			DBG(TAU_LOG_TRACE << "get_formula_or_term - formula: " << formula;)
			auto r = tau_api::get_formula_or_term(formula);
			CHECK(r.has_value());
			tref f = r.value();
			CHECK(f);
			DBG(using node = node_t;)
			DBG(TAU_LOG_TRACE << "get_formula_or_term - formula: " << TAU_LOG_FM_DUMP(f);)
			CHECK(tau::get(f).is(tau::wff));
		}
		for (const auto& term : terms) {
			DBG(TAU_LOG_TRACE << "get_formula_or_term - term: " << term;)
			auto r = tau_api::get_formula_or_term(term);
			CHECK(r.has_value());
			tref t = r.value();
			CHECK(t);
			CHECK(tau::get(t).is(tau::bf));
		}
	}
}

TEST_SUITE("Tau API - tref - querying") {

	TEST_CASE_FIXTURE(api_fixture, "contains") {
		DBG(using node = node_t;)
		auto tr = tau_api::get_term("x + 0", false);
		CHECK(tr.has_value());
		tref t = tr.value();
		DBG(TAU_LOG_TRACE << "contains on term: " << TAU_LOG_FM_DUMP(t);)
		CHECK(tau_api::contains(t, tau::bf));
		CHECK(tau_api::contains(t, tau::bf_add));
		CHECK(tau_api::contains(t, tau::variable));
		CHECK(tau_api::contains(t, tau::bf_f));
		CHECK(!tau_api::contains(t, tau::bf_t));
		CHECK(!tau_api::contains(t, tau::wff));
		CHECK(!tau_api::contains(t, tau::wff_f));
		CHECK(!tau_api::contains(t, tau::wff_t));

		auto fr = tau_api::get_formula("x = 0 && y ^ 1 = 1", false);
		CHECK(fr.has_value());
		tref f = fr.value();
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
			auto r = tau_api::get_formula_or_term(term);
			CHECK(r.has_value());
			tref t = r.value();
			CHECK(tau_api::is_term(t));
			CHECK(!tau_api::is_formula(t));
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "is_formula") {
		for (const auto& formula : formulas) {
			DBG(TAU_LOG_TRACE << "is_formula: " << formula;)
			auto r = tau_api::get_formula_or_term(formula);
			CHECK(r.has_value());
			tref f = r.value();
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
		auto def_r = tau_api::get_function_def("apply_def_f(x) := x + 1");
		REQUIRE(def_r.has_value());
		tref def = def_r.value();
		auto expr_r = tau_api::get_term("apply_def_f(t)", false);
		REQUIRE(expr_r.has_value());
		tref expr = expr_r.value();
		// before applying the definition, the call to `apply_def_f` is
		// still an unresolved reference
		CHECK(tau_api::contains(expr, tau::ref));
		auto applied_r = tau_api::apply_def(def, expr);
		REQUIRE(applied_r.has_value());
		tref applied = applied_r.value();
		CHECK(!tau_api::contains(applied, tau::ref));
	}
	TEST_CASE_FIXTURE(api_fixture, "apply_defs") {
		auto f_r = tau_api::get_function_def("apply_defs_f(x) := x + 1");
		auto g_r = tau_api::get_function_def("apply_defs_g(x) := x'");
		REQUIRE(f_r.has_value());
		REQUIRE(g_r.has_value());
		tref f = f_r.value(), g = g_r.value();
		auto expr_r = tau_api::get_term(
			"apply_defs_f(t) + apply_defs_g(t)", false);
		REQUIRE(expr_r.has_value());
		tref expr = expr_r.value();
		CHECK(tau_api::contains(expr, tau::ref));
		auto applied_r = tau_api::apply_defs(
			subtree_set<node_t>{ f, g }, expr);
		REQUIRE(applied_r.has_value());
		tref applied = applied_r.value();
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
		auto spec_r = tau_api::get_spec(
			"apply_all_defs_f(x) := x'.\napply_all_defs_f(z) = 0.");
		REQUIRE(spec_r.has_value());
		tref spec = spec_r.value();
		CHECK(tau_api::contains(spec, tau::ref));
		auto applied_r = tau_api::apply_all_defs(spec);
		REQUIRE(applied_r.has_value());
		tref applied = applied_r.value();
		CHECK(tau_api::to_str(applied) == "z' = 0");
		CHECK(!tau_api::contains(applied, tau::ref));
	}
	// AP1-4: apply_all_defs used to route through apply_defs({}), which
	// never consulted the global definitions store -- a ref whose
	// definition was registered via get_definition() survived unexpanded
	// into every downstream pipeline (dnf/cnf/nnf/solve/lgrs).
	TEST_CASE_FIXTURE(api_fixture,
			"apply_all_defs expands globally registered defs") {
		REQUIRE(tau_api::get_definition(
			"apply_all_defs_glob(x) := x'").has_value());
		auto expr_r = tau_api::get_formula(
			"apply_all_defs_glob(z) = 0");
		REQUIRE(expr_r.has_value());
		tref expr = expr_r.value();
		CHECK(tau_api::contains(expr, tau::ref));
		auto applied_r = tau_api::apply_all_defs(expr);
		REQUIRE(applied_r.has_value());
		tref applied = applied_r.value();
		CHECK(!tau_api::contains(applied, tau::ref));
		CHECK(tau_api::to_str(applied) == "z' = 0");
	}
}

TEST_SUITE("Tau API - tref - printing") {
	TEST_CASE_FIXTURE(api_fixture, "print") {
		auto fm_r = tau_api::get_formula("x = 0 && y = 1");
		REQUIRE(fm_r.has_value());
		tref fm = fm_r.value();
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
		auto fm_r = tau_api::get_formula("x = 0 && y = 1");
		REQUIRE(fm_r.has_value());
		tref fm = fm_r.value();
		CHECK(tau_api::to_str(fm) == "x = 0 && y = 1");
		CHECK(tau_api::to_str(nullptr) == "");
	}
}

TEST_SUITE("Tau API - tref - substitution") {
	TEST_CASE_FIXTURE(api_fixture, "substitute") {
		auto x_r = tau_api::get_term("x");
		auto y_r = tau_api::get_term("y");
		auto expr_r = tau_api::get_term("x + 1");
		REQUIRE(x_r.has_value());
		REQUIRE(y_r.has_value());
		REQUIRE(expr_r.has_value());
		tref x = x_r.value(), y = y_r.value(), expr = expr_r.value();
		auto sub_r = tau_api::substitute(expr, x, y);
		REQUIRE(sub_r.has_value());
		tref result = sub_r.value();
		CHECK(tau_api::to_str(result) == "y+1");
	}
	TEST_CASE_FIXTURE(api_fixture, "substitute map") {
		auto x_r = tau_api::get_term("x");
		auto y_r = tau_api::get_term("y");
		auto a_r = tau_api::get_term("a");
		auto b_r = tau_api::get_term("b");
		auto expr_r = tau_api::get_term("x + a");
		REQUIRE(x_r.has_value());
		REQUIRE(y_r.has_value());
		REQUIRE(a_r.has_value());
		REQUIRE(b_r.has_value());
		REQUIRE(expr_r.has_value());
		tref x = x_r.value(), y = y_r.value(), a = a_r.value(), b = b_r.value();
		tref expr = expr_r.value();
		std::map<tref, tref> that_with{ { x, y }, { a, b } };
		auto sub_r = tau_api::substitute(expr, that_with);
		REQUIRE(sub_r.has_value());
		tref result = sub_r.value();
		CHECK(tau_api::to_str(result) == "y+b");
	}

	// AP-6: substitute(tref, tref, tref) called is_term() on expr/that/with
	// unconditionally; is_term() dereferences its argument, so a null tref
	// asserted (debug) or was UB (release) instead of returning nullptr.
	TEST_CASE_FIXTURE(api_fixture, "substitute with a null argument returns nullptr") {
		auto x_r = tau_api::get_term("x");
		REQUIRE(x_r.has_value());
		tref x = x_r.value();
		REQUIRE(x != nullptr);
		CHECK(!tau_api::substitute(nullptr, x, x).has_value());
		CHECK(!tau_api::substitute(x, nullptr, x).has_value());
		CHECK(!tau_api::substitute(x, x, nullptr).has_value());
	}
}

TEST_SUITE("Tau API - tref - normal forms") {
	TEST_CASE_FIXTURE(api_fixture, "boole_normal_form") {
		auto fm_r = tau_api::get_formula("x = 0 && y = 1");
		REQUIRE(fm_r.has_value());
		auto bnf_r = tau_api::boole_normal_form(fm_r.value());
		REQUIRE(bnf_r.has_value());
		CHECK(tau_api::contains(bnf_r.value(), tau::wff));
	}
	TEST_CASE_FIXTURE(api_fixture, "dnf") {
		auto fm_r = tau_api::get_formula("(x = 0 || y = 0) && z = 0");
		REQUIRE(fm_r.has_value());
		auto d_r = tau_api::dnf(fm_r.value());
		REQUIRE(d_r.has_value());
		// distributing && over || yields a top-level disjunction
		CHECK(tau_api::contains(d_r.value(), tau::wff_or));
	}
	TEST_CASE_FIXTURE(api_fixture, "cnf") {
		auto fm_r = tau_api::get_formula("x = 0 || (y = 0 && z = 0)");
		REQUIRE(fm_r.has_value());
		auto c_r = tau_api::cnf(fm_r.value());
		REQUIRE(c_r.has_value());
		// distributing || over && yields a top-level conjunction
		CHECK(tau_api::contains(c_r.value(), tau::wff_and));
	}
	TEST_CASE_FIXTURE(api_fixture, "nnf") {
		auto fm_r = tau_api::get_formula("!(x = 0 && y = 0)");
		REQUIRE(fm_r.has_value());
		auto n_r = tau_api::nnf(fm_r.value());
		REQUIRE(n_r.has_value());
		// De Morgan must have pushed the negation to the leaves, turning
		// the negated conjunction into a disjunction of negated atoms
		CHECK(tau_api::contains(n_r.value(), tau::wff_or));
	}
}

TEST_SUITE("Tau API - tref - procedures") {
	TEST_CASE_FIXTURE(api_fixture, "simplify") {
		auto t_r = tau_api::get_term("x", false);
		REQUIRE(t_r.has_value());
		tref t = t_r.value();
		auto s_r = tau_api::simplify(t);
		REQUIRE(s_r.has_value());
		tref s = s_r.value();
		CHECK(tau_api::to_str(s) == "x");
		// simplify must be idempotent on an already-simplified term
		auto s2_r = tau_api::simplify(s);
		REQUIRE(s2_r.has_value());
		CHECK(s2_r.value() == s);
		CHECK(!tau_api::simplify(nullptr).has_value());
	}
	TEST_CASE_FIXTURE(api_fixture, "infer") {
		auto t_r = tau_api::get_term("x", false);
		REQUIRE(t_r.has_value());
		tref t = t_r.value();
		auto inferred_r = tau_api::infer(t);
		REQUIRE(inferred_r.has_value());
		CHECK(tau_api::to_str(inferred_r.value()) == "x");
		CHECK(!tau_api::infer(nullptr).has_value());
	}
	TEST_CASE_FIXTURE(api_fixture, "syntactic_term_simplification") {
		auto t_r = tau_api::get_term("x & x", false);
		REQUIRE(t_r.has_value());
		auto s_r = tau_api::syntactic_term_simplification(t_r.value());
		REQUIRE(s_r.has_value());
		CHECK(tau_api::to_str(s_r.value()) == "x");
	}
	TEST_CASE_FIXTURE(api_fixture, "syntactic_formula_simplification") {
		auto fm_r = tau_api::get_formula("x = 0 && x = 0", false);
		REQUIRE(fm_r.has_value());
		auto s_r = tau_api::syntactic_formula_simplification(fm_r.value());
		REQUIRE(s_r.has_value());
		CHECK(tau_api::to_str(s_r.value()) == "x = 0");
	}
	TEST_CASE_FIXTURE(api_fixture, "normalize_term") {
		auto t_r = tau_api::get_term("x & x'");
		REQUIRE(t_r.has_value());
		auto n_r = tau_api::normalize_term(t_r.value());
		REQUIRE(n_r.has_value());
		CHECK(tau_api::to_str(n_r.value()) == "0");
	}
	TEST_CASE_FIXTURE(api_fixture, "normalize_formula") {
		auto fm_r = tau_api::get_formula("x & x' = 0");
		REQUIRE(fm_r.has_value());
		auto n_r = tau_api::normalize_formula(fm_r.value());
		REQUIRE(n_r.has_value());
		CHECK(tau_api::to_str(n_r.value()) == "T");
	}
	TEST_CASE_FIXTURE(api_fixture, "anti_prenex") {
		// r(x, y) is an undefined predicate reference, so the existential
		// cannot be resolved away as a trivially-closed subformula; it
		// must survive anti_prenex (possibly reordered/renamed).
		auto fm_r = tau_api::get_formula("(ex x r(x, y)) && z = 0");
		REQUIRE(fm_r.has_value());
		auto a_r = tau_api::anti_prenex(fm_r.value());
		REQUIRE(a_r.has_value());
		CHECK(tau_api::contains(a_r.value(), tau::wff_ex));
	}
	TEST_CASE_FIXTURE(api_fixture, "eliminate_quantifiers") {
		auto fm_r = tau_api::get_formula("ex x x = 0");
		REQUIRE(fm_r.has_value());
		auto e_r = tau_api::eliminate_quantifiers(fm_r.value());
		REQUIRE(e_r.has_value());
		CHECK(!tau_api::contains(e_r.value(), tau::wff_ex));
	}
	TEST_CASE_FIXTURE(api_fixture, "realizable") {
		// realizable/unrealizable require a plain formula (is_formula()
		// gates on a wff node), not a full multi-clause spec.
		auto fm_r = tau_api::get_formula("x = 0");
		REQUIRE(fm_r.has_value());
		auto r = tau_api::realizable(fm_r.value());
		REQUIRE(r.has_value());
		CHECK(r.value());
	}
	TEST_CASE_FIXTURE(api_fixture, "unrealizable") {
		auto fm_r = tau_api::get_formula("F");
		REQUIRE(fm_r.has_value());
		auto r = tau_api::unrealizable(fm_r.value());
		REQUIRE(r.has_value());
		CHECK(r.value());
	}
	TEST_CASE_FIXTURE(api_fixture, "sat") {
		auto t_r = tau_api::get_formula("T");
		auto f_r = tau_api::get_formula("F");
		REQUIRE(t_r.has_value());
		REQUIRE(f_r.has_value());
		auto sat_t = tau_api::sat(t_r.value());
		auto sat_f = tau_api::sat(f_r.value());
		REQUIRE(sat_t.has_value());
		REQUIRE(sat_f.has_value());
		CHECK(sat_t.value());
		CHECK(!sat_f.value());
	}
	TEST_CASE_FIXTURE(api_fixture, "unsat") {
		auto t_r = tau_api::get_formula("T");
		auto f_r = tau_api::get_formula("F");
		REQUIRE(t_r.has_value());
		REQUIRE(f_r.has_value());
		auto unsat_f = tau_api::unsat(f_r.value());
		auto unsat_t = tau_api::unsat(t_r.value());
		REQUIRE(unsat_f.has_value());
		REQUIRE(unsat_t.has_value());
		CHECK(unsat_f.value());
		CHECK(!unsat_t.value());
	}
	TEST_CASE_FIXTURE(api_fixture, "valid") {
		auto t_r = tau_api::get_formula("T");
		auto x_r = tau_api::get_formula("x = 0");
		REQUIRE(t_r.has_value());
		REQUIRE(x_r.has_value());
		auto valid_t = tau_api::valid(t_r.value());
		auto valid_x = tau_api::valid(x_r.value());
		REQUIRE(valid_t.has_value());
		REQUIRE(valid_x.has_value());
		CHECK(valid_t.value());
		CHECK(!valid_x.value());
	}
	TEST_CASE_FIXTURE(api_fixture, "valid_spec") {
		auto t_r = tau_api::get_formula("T");
		auto x_r = tau_api::get_formula("x = 0");
		REQUIRE(t_r.has_value());
		REQUIRE(x_r.has_value());
		auto valid_t = tau_api::valid_spec(t_r.value());
		auto valid_x = tau_api::valid_spec(x_r.value());
		REQUIRE(valid_t.has_value());
		REQUIRE(valid_x.has_value());
		CHECK(valid_t.value());
		// x = 0 is not logically equivalent to T
		CHECK(!valid_x.value());
	}
}

TEST_SUITE("Tau API - tref - solving") {
	TEST_CASE_FIXTURE(api_fixture, "solve") {
		auto eq_r = tau_api::get_formula("x | y = 0");
		REQUIRE(eq_r.has_value());
		auto solution = tau_api::solve(eq_r.value(), solver_mode::general);
		REQUIRE(solution.has_value());
		CHECK(!solution.value().empty());
	}
	TEST_CASE_FIXTURE(api_fixture, "lgrs") {
		// FIXED (issue-60): this used to abort for every non-null
		// equation. The abort was src/api.tmpl.h's non-Boolean screen
		//     tau::get(eq)[0] ... tau::get(eq)[1]
		// where `eq` is the whole wff. For a single equality that wff has
		// exactly ONE child, so [1] was null and operator[] ->
		// child_tree() tripped assert(c != nullptr)
		// (src/tau_tree.tmpl.h:579). The line above had already extracted
		// the equality into `equality`, whose two children are the sides
		// meant to be checked; the screen now indexes that instead.
		auto eq_r = tau_api::get_formula("x | y = 0");
		REQUIRE(eq_r.has_value());
		auto solution = tau_api::lgrs(eq_r.value());
		REQUIRE(solution.has_value());
		CHECK(!solution.value().empty());
	}
	TEST_CASE_FIXTURE(api_fixture, "lgrs rejects a null tref") {
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
		auto fm_r = tau_api::get_formula("o[t]:tau = i[t]:tau");
		REQUIRE(fm_r.has_value());
		auto maybe_i = tau_api::get_interpreter(fm_r.value());
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
		CHECK(!tau_api::sat(htref{}).has_value());
		CHECK(!tau_api::valid(htref{}).has_value());
	}

	TEST_CASE_FIXTURE(api_fixture, "htref-returning overloads reject a null htref") {
		CHECK(!tau_api::dnf(htref{}).has_value());
		CHECK(!tau_api::cnf(htref{}).has_value());
		CHECK(!tau_api::simplify(htref{}).has_value());
	}

	TEST_CASE_FIXTURE(api_fixture, "to_str rejects a null htref") {
		CHECK(tau_api::to_str(htref{}) == "");
	}

	TEST_CASE_FIXTURE(api_fixture, "solve/lgrs reject a null htref") {
		CHECK(!tau_api::solve(htref{}, solver_mode::general).has_value());
		CHECK(!tau_api::lgrs(htref{}).has_value());
	}

	TEST_CASE_FIXTURE(api_fixture, "substitute rejects null htref arguments") {
		auto x_r = tau_api::geth_term("x");
		REQUIRE(x_r.has_value());
		htref x = x_r.value();
		REQUIRE(x != nullptr);
		CHECK(!tau_api::substitute(htref{}, x, x).has_value());
		CHECK(!tau_api::substitute(x, htref{}, x).has_value());
		CHECK(!tau_api::substitute(x, x, htref{}).has_value());
	}
}

// AP1-30 + AP1-31(a): direct add_definition and get_stream_def edge coverage.
TEST_SUITE("Tau API - tref - definition/stream edges") {
	TEST_CASE_FIXTURE(api_fixture, "add_definition: null args return an error, "
			"first real id is 1-based") {
		CHECK( !tau_api::add_definition(nullptr, nullptr).has_value() );
		auto def_r = tau_api::get_definition("ap130_f(x) := x'");
		REQUIRE( def_r.has_value() );
		tref def = def_r.value();
		// AP1-6: a registered definition's id is always > 0
		auto id_r = tau_api::add_definition(
			tau::get(def).first(), tau::get(def).second());
		REQUIRE( id_r.has_value() );
		CHECK( id_r.value() > 0 );
	}

	TEST_CASE_FIXTURE(api_fixture, "get_stream_def: malformed input "
			"returns nullptr (AP1-2 pinned)") {
		CHECK( !tau_api::get_stream_def("not a stream def").has_value() );
		CHECK( !tau_api::get_stream_def("").has_value() );
	}
}
