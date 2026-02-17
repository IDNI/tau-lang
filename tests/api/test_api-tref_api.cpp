// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

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
		tref t = tau_api::get_term("x + 0");
		DBG(TAU_LOG_TRACE << "contains on term: " << TAU_LOG_FM_DUMP(t);)
		CHECK(tau_api::contains(t, tau::bf));
		CHECK(tau_api::contains(t, tau::bf_add));
		CHECK(tau_api::contains(t, tau::variable));
		CHECK(tau_api::contains(t, tau::bf_f));
		CHECK(!tau_api::contains(t, tau::bf_t));
		CHECK(!tau_api::contains(t, tau::wff));
		CHECK(!tau_api::contains(t, tau::wff_f));
		CHECK(!tau_api::contains(t, tau::wff_t));

		tref f = tau_api::get_formula("x = 0 && y ^ 1 = 1");
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
	TEST_CASE_FIXTURE(api_fixture, "apply_def") {
	}
	TEST_CASE_FIXTURE(api_fixture, "apply_defs") {
	}
	TEST_CASE_FIXTURE(api_fixture, "apply_all_defs") {
	}
}

TEST_SUITE("Tau API - tref - printing") {
	TEST_CASE_FIXTURE(api_fixture, "print") {
	}
	TEST_CASE_FIXTURE(api_fixture, "to_str") {
	}
}

TEST_SUITE("Tau API - tref - substitution") {
	TEST_CASE_FIXTURE(api_fixture, "substitute") {
	}
	TEST_CASE_FIXTURE(api_fixture, "substitute map") {
	}
}

TEST_SUITE("Tau API - tref - normal forms") {
	TEST_CASE_FIXTURE(api_fixture, "boole_normal_form") {
	}
	TEST_CASE_FIXTURE(api_fixture, "dnf") {
	}
	TEST_CASE_FIXTURE(api_fixture, "cnf") {
	}
	TEST_CASE_FIXTURE(api_fixture, "nnf") {
	}
}

TEST_SUITE("Tau API - tref - procedures") {
	TEST_CASE_FIXTURE(api_fixture, "simplify") {
	}
	TEST_CASE_FIXTURE(api_fixture, "infer") {
	}
	TEST_CASE_FIXTURE(api_fixture, "syntactic_term_simplification") {
	}
	TEST_CASE_FIXTURE(api_fixture, "syntactic_formula_simplification") {
	}
	TEST_CASE_FIXTURE(api_fixture, "normalize_term") {
	}
	TEST_CASE_FIXTURE(api_fixture, "normalize_formula") {
	}
	TEST_CASE_FIXTURE(api_fixture, "anti_prenex") {
	}
	TEST_CASE_FIXTURE(api_fixture, "eliminate_quantifiers") {
	}
	TEST_CASE_FIXTURE(api_fixture, "realizable") {
	}
	TEST_CASE_FIXTURE(api_fixture, "unrealizable") {
	}
	TEST_CASE_FIXTURE(api_fixture, "sat") {
	}
	TEST_CASE_FIXTURE(api_fixture, "unsat") {
	}
	TEST_CASE_FIXTURE(api_fixture, "valid") {
	}
	TEST_CASE_FIXTURE(api_fixture, "valid_spec") {
	}
}

TEST_SUITE("Tau API - tref - solving") {
	TEST_CASE_FIXTURE(api_fixture, "solve") {
	}
	TEST_CASE_FIXTURE(api_fixture, "lgrs") {
	}
}

TEST_SUITE("Tau API - tref - execution") {
	TEST_CASE_FIXTURE(api_fixture, "get_interpreter") {
	}
}