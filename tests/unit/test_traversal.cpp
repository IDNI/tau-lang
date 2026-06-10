// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_Bool_helpers.h"

TEST_SUITE("operator|") {

	TEST_CASE("match zero nodes") {
		const char* sample = "X & Y";
		auto pbf = parse_bf();
		auto rule = tt(tau::get(sample, pbf));
		CHECK( !(rule | tau::main) );
	}

	TEST_CASE("match one node") {
		const char* sample = "X & Y";
		auto pbf = parse_bf();
		auto rule = tt(tau::get(sample, pbf));
		CHECK( (rule | tau::bf_and).size() == 1 );
	}
}

TEST_SUITE("operator||") {

	TEST_CASE("match zero nodes") {
		const char* sample = "X & Y";
		auto pbf = parse_bf();
		auto rule = tt(tau::get(sample, pbf));
		CHECK( (rule || tau::wff).empty() );
	}

	TEST_CASE("match one node") {
		const char* sample = "X & Y";
		auto pbf = parse_bf();
		auto rule = tt(tau::get(sample, pbf));
		CHECK( (rule || tau::bf_and).size() == 1 );
	}

	TEST_CASE("match several nodes") {
		const char* sample = "X & Y";
		auto pbf = parse_bf();
		auto rule = tt(tau::get(sample, pbf));
		CHECK( (rule | tau::bf_and || tau::bf).size() == 2 );
	}
}

TEST_SUITE("wff/bf kind predicates") {

	TEST_CASE("is_wff_nt classifies wff-kind nonterminals") {
		CHECK( tau::is_wff_nt(tau::wff_and) );
		CHECK( tau::is_wff_nt(tau::wff_neg) );
		CHECK( tau::is_wff_nt(tau::wff_t) );
		CHECK( tau::is_wff_nt(tau::wff_f) );
		CHECK( tau::is_wff_nt(tau::wff_all) );
		CHECK( tau::is_wff_nt(tau::wff_sometimes) );
		CHECK( tau::is_wff_nt(tau::wff_ref) );
		CHECK( tau::is_wff_nt(tau::constraint) );
		// comparisons are wff-kind even though named bf_*
		CHECK( tau::is_wff_nt(tau::bf_eq) );
		CHECK( tau::is_wff_nt(tau::bf_neq) );
		CHECK( tau::is_wff_nt(tau::bf_interval) );
		CHECK( tau::is_wff_nt(tau::bf_lteq) );
		// terms are not wff-kind
		CHECK( !tau::is_wff_nt(tau::bf_and) );
		CHECK( !tau::is_wff_nt(tau::variable) );
		CHECK( !tau::is_wff_nt(tau::ba_constant) );
		CHECK( !tau::is_wff_nt(tau::bf_t) );
		CHECK( !tau::is_wff_nt(tau::capture) );
		// transitional: the bare wrapper still counts as wff-kind
		// (removed again in the cleanup phase)
		CHECK( tau::is_wff_nt(tau::wff) );
		CHECK( !tau::is_wff_nt(tau::bf) );
	}

	TEST_CASE("capture kind split") {
		CHECK( tau::is_wff_nt(tau::wff_capture) );
		CHECK( !tau::is_wff_nt(tau::bf_capture) );
		CHECK( tau::is_term_nt(tau::bf_capture) );
		CHECK( !tau::is_term_nt(tau::wff_capture) );
		CHECK( tau::is_capture_nt(tau::capture) );
		CHECK( tau::is_capture_nt(tau::wff_capture) );
		CHECK( tau::is_capture_nt(tau::bf_capture) );
		CHECK( !tau::is_capture_nt(tau::variable) );
	}

	TEST_CASE("parsed captures carry their kind") {
		// bf-position capture
		tau::get_options bf_opts = {
			.parse = { .start = tau::bf },
			.reget_with_hooks = false
		};
		tref t = tau::get("$X & y", bf_opts);
		REQUIRE( t != nullptr );
		tref c = tau::get(t).find_top(is<node_t, tau::bf_capture>);
		CHECK( c != nullptr );
		// wff-position capture
		tau::get_options wff_opts = {
			.parse = { .start = tau::wff },
			.reget_with_hooks = false
		};
		tref f = tau::get("$X && T", wff_opts);
		REQUIRE( f != nullptr );
		tref w = tau::get(f).find_top(is<node_t, tau::wff_capture>);
		CHECK( w != nullptr );
	}

	TEST_CASE("is_wff member on parsed formula") {
		// trees still carry wrappers in this phase; both wrapper and
		// operator nodes must satisfy the predicates
		tref fm = tau::get("x = 0 && y != 0 .");
		REQUIRE( fm != nullptr );
		tref w = tau::get(fm).find_top(is<node_t, tau::wff_and>);
		REQUIRE( w != nullptr );
		CHECK( tau::get(w).is_wff() );
		CHECK( !tau::get(w).is_term() );
	}
}
