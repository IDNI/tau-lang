// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

//#include "satisfiability.h"
#include "boolean_algebras/bool_ba.h"
#include "depreciating/normalizer_depreciating.h"

// TODO (LOW) consider move this test to integration tests
#include "test_helpers_depreciating.h"

#ifdef DEBUG
#include "depreciating/debug_helpers_depreciating.h"
#endif

TEST_SUITE("normal forms: mnf for wffs") {

	TEST_CASE("simple case: T") {
		const char* sample = "T.";
		auto src = make_tau_source(sample);
		auto statement = make_statement(src);
		statement = find_top(statement, is_non_terminal<tau_parser::wff, Bool>).value();
		auto result = to_mnf(reduce_across_bfs(statement, false));
		auto check = result
			| tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("simple case: F") {
		const char* sample = "F.";
		auto src = make_tau_source(sample);
		auto statement = make_statement(src);
		statement = find_top(statement, is_non_terminal<tau_parser::wff, Bool>).value();
		auto result = to_mnf(reduce_across_bfs(statement, false));
		auto check = result
			| tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("simple case: X = 0") {
		const char* sample = "X = 0.";
		auto src = make_tau_source(sample);
		auto statement = (make_statement(src)
			| tau_parser::spec
			| tau_parser::main
			| tau_parser::wff).value();
		auto result = to_mnf(reduce_across_bfs(statement, false));
		CHECK( statement == result );
	}

	TEST_CASE("simple case: X != 0") {
		const char* sample = "X != 0.";
		auto src = make_tau_source(sample);
		auto statement = (make_statement(src)
			| tau_parser::spec
			| tau_parser::main
			| tau_parser::wff).value();
		auto result = to_mnf(reduce_across_bfs(statement, false));
		auto check_eq = select_all(result, is_non_terminal<tau_parser::bf_eq, Bool>);
		auto check_neg = select_all(result, is_non_terminal<tau_parser::wff_neg, Bool>);
		CHECK( check_eq.size() == 1 );
		CHECK( check_neg.size() == 1 );
	}

	TEST_CASE("simple case: X = 0 && Y = 0") {
		const char* sample = "X = 0 && Y = 0.";
		auto src = make_tau_source(sample);
		auto statement = (make_statement(src)
			| tau_parser::spec
			| tau_parser::main
			| tau_parser::wff).value();
		auto result = to_mnf(reduce_across_bfs(statement, false));
		auto check_and = select_all(result, is_non_terminal<tau_parser::wff_and, Bool>);
		auto check_eq = select_all(result, is_non_terminal<tau_parser::bf_eq, Bool>);
		CHECK( check_and.size() == 1 );
		CHECK( check_eq.size() == 2 );
	}

	TEST_CASE("simple case: X != 0 && Y != 0") {
		const char* sample = "X != 0 && Y != 0.";
		auto src = make_tau_source(sample);
		auto statement = (make_statement(src)
			| tau_parser::spec
			| tau_parser::main
			| tau_parser::wff).value();
		auto result = to_mnf(reduce_across_bfs(statement, false));
		auto check_eq = select_all(result, is_non_terminal<tau_parser::bf_eq, Bool>);
		auto check_neg = select_all(result, is_non_terminal<tau_parser::wff_neg, Bool>);
		auto check_and = select_all(result, is_non_terminal<tau_parser::wff_and, Bool>);
		CHECK( check_eq.size() == 2 );
		CHECK( check_neg.size() == 2 );
		CHECK( check_and.size() == 1 );
	}

	TEST_CASE("simple case: X = 0 || Y = 0") {
		const char* sample = "X = 0 || Y = 0.";
		auto src = make_tau_source(sample);
		auto statement = (make_statement(src)
			| tau_parser::spec
			| tau_parser::main
			| tau_parser::wff).value();
		auto result = to_mnf(reduce_across_bfs(statement, false));
		auto check_eq = select_all(result, is_non_terminal<tau_parser::bf_eq, Bool>);
		auto check_or = select_all(result, is_non_terminal<tau_parser::wff_or, Bool>);
		CHECK( check_eq.size() == 2 );
		CHECK( check_or.size() == 1 );
	}
}

TEST_SUITE("normal forms: bf_reduce_canonical") {

	TEST_CASE("uninterpreted constants") {
		const char* sample = "(<:c>' & <:b>' & <:b> | <:c>' & <:b>' & <:c> & <:b>' | <:c>' & <:c> & <:b> & <:b> | <:c>' & <:c> & <:b> & <:c> & <:b>') & <:a> | (<:b>' & <:c>' & <:b> | <:b>' & <:c>' & <:c> & <:b>' | <:c> & <:b> & <:c>' & <:b> | <:c> & <:b> & <:c>' & <:c> & <:b>') & <:a>' = 0.";
		auto src = make_tau_source(sample);
		auto statement = (make_statement(src)
			| tau_parser::spec
			| tau_parser::main
			| tau_parser::wff).value();
		auto result = statement | bf_reduce_canonical<Bool>();
		CHECK( result == _T<Bool> );
	}
}

TEST_SUITE("normal forms: reduce_bf") {

	/*TEST_CASE("uninterpreted constants") {
		const char* sample = "(<:c>' & <:b>' & <:b> | <:c>' & <:b>' & <:c> & <:b>' | <:c>' & <:c> & <:b> & <:b> | <:c>' & <:c> & <:b> & <:c> & <:b>') & <:a> | (<:b>' & <:c>' & <:b> | <:b>' & <:c>' & <:c> & <:b>' | <:c> & <:b> & <:c>' & <:b> | <:c> & <:b> & <:c>' & <:c> & <:b>') & <:a>' = 0.";
		auto src = make_tau_source(sample);
		auto statement = (make_statement(src)
			| tau_parser::spec
			| tau_parser::main
			| tau_parser::wff).value();
		auto result = statement | reduce_bf<Bool>;
		CHECK( result == _F<Bool> );
	}*/
}

TEST_SUITE("normal forms: snf_bf") {

	TEST_CASE("uninterpreted constants") {
		const char* sample = "(<:c>' & <:b>' & <:b> | <:c>' & <:b>' & <:c> & <:b>' | <:c>' & <:c> & <:b> & <:b> | <:c>' & <:c> & <:b> & <:c> & <:b>') & <:a> | (<:b>' & <:c>' & <:b> | <:b>' & <:c>' & <:c> & <:b>' | <:c> & <:b> & <:c>' & <:b> | <:c> & <:b> & <:c>' & <:c> & <:b>') & <:a>' = 0.";
		auto src = make_tau_source(sample);
		auto statement = (make_statement(src)
			| tau_parser::spec
			| tau_parser::main
			| tau_parser::wff).value();
		auto result = snf_bf(statement);
		CHECK( result == _T<Bool> );
	}
}

TEST_SUITE("normal forms: dnf_bf") {

	TEST_CASE("uninterpreted constants") {
		const char* sample = "(<:c>' & <:b>' & <:b> | <:c>' & <:b>' & <:c> & <:b>' | <:c>' & <:c> & <:b> & <:b> | <:c>' & <:c> & <:b> & <:c> & <:b>') & <:a> | (<:b>' & <:c>' & <:b> | <:b>' & <:c>' & <:c> & <:b>' | <:c> & <:b> & <:c>' & <:b> | <:c> & <:b> & <:c>' & <:c> & <:b>') & <:a>' = 0.";
		auto src = make_tau_source(sample);
		auto statement = (make_statement(src)
			| tau_parser::spec
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::bf_eq
			| tau_parser::bf).value();
		auto result = to_dnf<false>(statement);
		CHECK( result == _0<Bool> );
	}

	/*TEST_CASE("uninterpreted constants") {
		const char* sample = " o1[2]' & ((<:a> & (<:a> & <:c> | <:b> & <:c>' "
			"& <:a>' | <:a> & <:b> & <:c>'))' & <:b> & (<:a> & <:c> | <:b> "
			"& <:c>' & <:a>' | <:a> & <:b> & <:c>')' | <:a> & (<:a> & <:c> "
			"| <:b> & <:c>' & <:a>' | <:a> & <:b> & <:c>') & (<:b> & (<:a> "
			"& <:c> | <:b> & <:c>' & <:a>' | <:a> & <:b> & <:c>')')') | o1[2] "
			"& ((<:a> & (<:a> & <:c> | <:b> & <:c>' & <:a>' | <:a> & <:b> "
			"& <:c>'))' & <:b> & (<:a> & <:c> | <:b> & <:c>' & <:a>' | <:a> "
			"& <:b> & <:c>')' | <:a> & (<:a> & <:c> | <:b> & <:c>' & <:a>' "
			"| <:a> & <:b> & <:c>') & (<:b> & (<:a> & <:c> | <:b> & <:c>' "
			"& <:a>' | <:a> & <:b> & <:c>')')')' = 0.";
		auto src = make_tau_source(sample);
		auto statement = (make_statement(src)
			| tau_parser::spec
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::bf_eq
			| tau_parser::bf).value();
		auto result = bf_boole_normal_form(statement);
		CHECK( result == _0<Bool> );
	}*/


}

TEST_SUITE("normal forms: onf") {

	/* TEST_CASE("T") {
		const char* sample = "T.";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_nso_rr_using_factory<sbf_ba_factory_t, sbf_ba>(sample_src, bf);
		auto nts = std::get<tau_source_sym>(sample_formula.main->value).nts;
		auto var = make_node<tau_sym<sbf_ba>>(tau_source_sym(tau_parser::variable, nts), {});
		auto result = onf(sample_formula.main, var);
		auto check = result | tau_parser::wff_t;
		CHECK( check.has_value() );
		CHECK( true );
	}*/
}

// TODO (MEDIUM) add tests for reduce_bf/wff
// TODO (MEDIUM) add tests for to_bdd_bf
// TODO (MEDIUM) add tests for minimize


// TODO (MEDIUM) add tests for dnf_bf
// TODO (MEDIUM) add tests for dnf_wff
// TODO (MEDIUM) add tests for cnf_bf
// TODO (MEDIUM) add tests for cnf_wff
// TODO (MEDIUM) add tests for nnf_bf
// TODO (MEDIUM) add tests for nnf_bf
// TODO (MEDIUM) add tests for sno_wwf
// TODO (MEDIUM) add tests for sno_wff
// TODO (MEDIUM) add tests for mnn_bf
// TODO (MEDIUM) add tests for mnn_wff
