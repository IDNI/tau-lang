// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_Bool_helpers.h"

// TEST_SUITE("configuration") {
// 	TEST_CASE("set trace logging level") {
// 		logging::trace();
// 	}
// }

TEST_SUITE("normal forms: mnf for wffs") {

	TEST_CASE("simple case: T") {
		const char* sample = "1";
		tau::parse_options opts;
		opts.start = tau::bf; 
		tref fm = tau::get(sample, opts);
		tau::get(fm).dump(std::cout << "fm: ") << "\n";
		CHECK( tau::get(fm)[0].is(tau::bf_t) );
	}

	TEST_CASE("simple case: T") {
		const char* sample = "T.";
		tref fm = tau::get(tau::get(sample))
			.find_top(is<tau::node, tau::wff>);
		fm = to_mnf<bnode>(reduce_across_bfs<bnode>(fm, false));
		CHECK( tau::get(fm)[0].is(tau::wff_t) );
	}

	TEST_CASE("simple case: F") {
		const char* sample = "F.";
		tref fm = tau::get(tau::get(sample))
			.find_top(is<tau::node, tau::wff>);
		fm = to_mnf<bnode>(reduce_across_bfs<bnode>(fm, false));
		CHECK( tau::get(fm)[0].is(tau::wff_f) );
	}

	TEST_CASE("simple case: X = 0") {
		const char* sample = "X = 0.";
		tref fm = tt(tau::get(sample))
			| tau::spec | tau::main | tau::wff | tt::ref;
		tref result = to_mnf<bnode>(reduce_across_bfs<bnode>(fm, false));
		CHECK( fm == result );
	}

	TEST_CASE("simple case: X != 0") {
		const char* sample = "X != 0.";

		tref fm = tt(tau::get(sample))
			| tau::spec | tau::main | tau::wff | tt::ref;
		fm = to_mnf<bnode>(reduce_across_bfs<bnode>(fm, false));
		trefs check_eq  = tau::get(fm).select_all(is<tau::node, tau::bf_eq>);
		trefs check_neg = tau::get(fm).select_all(is<tau::node, tau::wff_neg>);
		CHECK( check_eq.size() == 1 );
		CHECK( check_neg.size() == 1 );
	}

	TEST_CASE("simple case: X = 0 && Y = 0") {
		const char* sample = "X = 0 && Y = 0.";
		tref fm = tt(tau::get(sample))
			| tau::spec | tau::main | tau::wff | tt::ref;
		fm = to_mnf<bnode>(reduce_across_bfs<bnode>(fm, false));
		trefs check_and = tau::get(fm).select_all(is<tau::node, tau::wff_and>);
		trefs check_eq = tau::get(fm).select_all(is<tau::node, tau::bf_eq>);
		CHECK( check_and.size() == 1 );
		CHECK( check_eq.size() == 2 );
	}

	TEST_CASE("simple case: X != 0 && Y != 0") {
		const char* sample = "X != 0 && Y != 0.";
		tref fm = tt(tau::get(sample))
			| tau::spec | tau::main | tau::wff | tt::ref;
		fm = to_mnf<bnode>(reduce_across_bfs<bnode>(fm, false));
		trefs check_eq = tau::get(fm).select_all(is<tau::node, tau::bf_eq>);
		trefs check_neg = tau::get(fm).select_all(is<tau::node, tau::wff_neg>);
		trefs check_and = tau::get(fm).select_all(is<tau::node, tau::wff_and>);
		CHECK( check_eq.size() == 2 );
		CHECK( check_neg.size() == 2 );
		CHECK( check_and.size() == 1 );
	}

	TEST_CASE("simple case: X = 0 || Y = 0") {
		const char* sample = "X = 0 || Y = 0.";
		tref fm = tt(tau::get(sample))
			| tau::spec | tau::main | tau::wff | tt::ref;
		fm = to_mnf<bnode>(reduce_across_bfs<bnode>(fm, false));
		trefs check_eq = tau::get(fm).select_all(is<tau::node, tau::bf_eq>);
		trefs check_or = tau::get(fm).select_all(is<tau::node, tau::wff_or>);
		CHECK( check_eq.size() == 2 );
		CHECK( check_or.size() == 1 );
	}

}

TEST_SUITE("normal forms: bf_reduce_canonical") {


	TEST_CASE("uninterpreted constants") {
		const char* sample = "(<:c>' & <:b>' & <:b> | <:c>' & <:b>' & <:c> & <:b>' | <:c>' & <:c> & <:b> & <:b> | <:c>' & <:c> & <:b> & <:c> & <:b>') & <:a> | (<:b>' & <:c>' & <:b> | <:b>' & <:c>' & <:c> & <:b>' | <:c> & <:b> & <:c>' & <:b> | <:c> & <:b> & <:c>' & <:c> & <:b>') & <:a>' = 0.";
		tref fm = tt(tau::get(sample))
			| tau::spec | tau::main | tau::wff
			| bf_reduce_canonical<bnode>() | tt::ref;
		CHECK( tau::get(fm) == tau::get_T() );
	}

}

TEST_SUITE("normal forms: reduce_bf") {

	/*TEST_CASE("uninterpreted constants") {
		const char* sample = "(<:c>' & <:b>' & <:b> | <:c>' & <:b>' & <:c> & <:b>' | <:c>' & <:c> & <:b> & <:b> | <:c>' & <:c> & <:b> & <:c> & <:b>') & <:a> | (<:b>' & <:c>' & <:b> | <:b>' & <:c>' & <:c> & <:b>' | <:c> & <:b> & <:c>' & <:b> | <:c> & <:b> & <:c>' & <:c> & <:b>') & <:a>' = 0.";
		auto src = make_tau_source(sample);
		auto statement = (make_statement(src)
			| tau::spec
			| tau::main
			| tau::wff).value();
		auto result = statement | reduce_bf<Bool>;
		CHECK( result == _F<Bool> );
	}*/
}

TEST_SUITE("normal forms: snf_bf") {

	TEST_CASE("uninterpreted constants") {
		const char* sample = "(<:c>' & <:b>' & <:b> | <:c>' & <:b>' & <:c> & <:b>' | <:c>' & <:c> & <:b> & <:b> | <:c>' & <:c> & <:b> & <:c> & <:b>') & <:a> | (<:b>' & <:c>' & <:b> | <:b>' & <:c>' & <:c> & <:b>' | <:c> & <:b> & <:c>' & <:b> | <:c> & <:b> & <:c>' & <:c> & <:b>') & <:a>' = 0.";
		tref fm = tt(tau::get(sample))
			| tau::spec | tau::main | tau::wff
			| tt::f(snf_bf<bnode>) | tt::ref;
		CHECK( tau::get(fm) == tau::get_T() );
	}
}

TEST_SUITE("normal forms: dnf_bf") {

	TEST_CASE("uninterpreted constants") {
		const char* sample = "(<:c>' & <:b>' & <:b> | <:c>' & <:b>' & <:c> & <:b>' | <:c>' & <:c> & <:b> & <:b> | <:c>' & <:c> & <:b> & <:c> & <:b>') & <:a> | (<:b>' & <:c>' & <:b> | <:b>' & <:c>' & <:c> & <:b>' | <:c> & <:b> & <:c>' & <:b> | <:c> & <:b> & <:c>' & <:c> & <:b>') & <:a>' = 0.";
		tref fm = tt(tau::get(sample))
			| tau::spec | tau::main | tau::wff | tau::bf_eq
			| tau::bf | tt::f(to_dnf<bnode, false>) | tt::ref;
		CHECK( tau::get(fm) == tau::get_0() );
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
			| tau::spec
			| tau::main
			| tau::wff
			| tau::bf_eq
			| tau::bf).value();
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
		auto var = make_node<tau_sym<sbf_ba>>(tau_source_sym(tau::variable, nts), {});
		auto result = onf(sample_formula.main, var);
		auto check = result | tau::wff_t;
		CHECK( check.has_value() );
		CHECK( true );
	}*/
}

// // TODO (MEDIUM) add tests for reduce_bf/wff
// // TODO (MEDIUM) add tests for to_bdd_bf
// // TODO (MEDIUM) add tests for minimize


// // TODO (MEDIUM) add tests for dnf_bf
// // TODO (MEDIUM) add tests for dnf_wff
// // TODO (MEDIUM) add tests for cnf_bf
// // TODO (MEDIUM) add tests for cnf_wff
// // TODO (MEDIUM) add tests for nnf_bf
// // TODO (MEDIUM) add tests for nnf_bf
// // TODO (MEDIUM) add tests for sno_wwf
// // TODO (MEDIUM) add tests for sno_wff
// // TODO (MEDIUM) add tests for mnn_bf
// // TODO (MEDIUM) add tests for mnn_wff
