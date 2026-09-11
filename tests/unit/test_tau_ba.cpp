// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// BA-3: dedicated unit tests for tau_ba<BAs...> (boolean_algebras/tau_ba.h) --
// operator==, is_zero/is_one and is_tau_closed, which per the review were
// only exercised indirectly via integration tests.

#include "test_init.h"
#include "test_tau_helpers.h"

TEST_SUITE("configuration") {

	TEST_CASE("bdd_init") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("tau_ba equality") {

	TEST_CASE("default-constructed tau_ba compares equal to itself") {
		tau_ba<bv, sbf_ba> a, b;
		CHECK( a == b );
	}

	TEST_CASE("operator== compares main and rec_relations structurally") {
		tau_ba<bv, sbf_ba> a(tau::_T());
		tau_ba<bv, sbf_ba> b(tau::_T());
		tau_ba<bv, sbf_ba> c(tau::_F());

		CHECK( a == b );
		CHECK_FALSE( a == c );
		CHECK( a != c );
		CHECK_FALSE( a != b );
	}
}

TEST_SUITE("tau_ba zero/one predicates") {

	TEST_CASE("T is one and not zero") {
		tau_ba<bv, sbf_ba> t(tau::_T());
		CHECK( t.is_one() );
		CHECK_FALSE( t.is_zero() );
		CHECK( t == true );
		CHECK( true == t );
		CHECK( t != false );
	}

	TEST_CASE("F is zero and not one") {
		tau_ba<bv, sbf_ba> f(tau::_F());
		CHECK( f.is_zero() );
		CHECK_FALSE( f.is_one() );
		CHECK( f == false );
		CHECK( false == f );
		CHECK( f != true );
	}

	TEST_CASE("bitwise operators reflect boolean semantics on T/F") {
		tau_ba<bv, sbf_ba> t(tau::_T());
		tau_ba<bv, sbf_ba> f(tau::_F());

		CHECK( (~t).is_zero() );
		CHECK( (~f).is_one() );
		CHECK( (t & f).is_zero() );
		CHECK( (t | f).is_one() );
		CHECK( (t ^ f).is_one() );
		CHECK( (t ^ t).is_zero() );
		CHECK( (t + f).is_one() );
	}
}

TEST_SUITE("is_tau_closed") {

	TEST_CASE("the constants T and F are closed") {
		CHECK( is_tau_closed(tau_ba<bv, sbf_ba>(tau::_T())) );
		CHECK( is_tau_closed(tau_ba<bv, sbf_ba>(tau::_F())) );
	}

	TEST_CASE("a spec whose free variables are all i/o variables is closed") {
		auto nso_rr = get_nso_rr("(always o1[t] = i1[t]).");
		REQUIRE(nso_rr.has_value());
		tau_ba<bv, sbf_ba> spec(nso_rr.value().rec_relations,
					 nso_rr.value().main);
		CHECK( is_tau_closed(spec) );
	}

	TEST_CASE("a spec with a plain free variable is not closed") {
		auto nso_rr = get_nso_rr("xyz = 0.");
		REQUIRE(nso_rr.has_value());
		tau_ba<bv, sbf_ba> spec(nso_rr.value().rec_relations,
					 nso_rr.value().main);
		CHECK_FALSE( is_tau_closed(spec) );
	}
}

// GitHub #92 (residue b): the decision caches of is_zero/is_one are keyed on
// tree identity and filtered after every sweep, so a decided row whose key
// tree nothing held was dropped and the constant re-decided at the next
// step. The keys of decided rows are now pinned with a handle, bounded by
// ba_decision_pins; with the cap at 0 the old behaviour is back.
TEST_SUITE("tau_ba decision rows survive the sweep") {

	tref parse_wff(const char* sample) {
		static tree<node_t>::get_options opts{
			.parse = { .start = tree<node_t>::wff } };
		tref r = tree<node_t>::get(sample, opts);
		REQUIRE( r != nullptr );
		return r;
	}

	// Plain (non-stream) variables: the mains go through the same predicate
	// cache, and stream variables would need an io context to classify them.
	// The interpreter's per-step sweep (interpreter::maybe_gc): everything
	// not reachable from the definitions, the term-BDD store or a handle
	// goes; GC-registered caches drop the rows whose key did not survive.
	void sweep() {
		std::unordered_set<tref> keep;
		definitions<node_t>::instance().collect_live_refs(keep);
		tau_term_bdd<node_t>::collect_live_refs(keep);
		tree<node_t>::gc(keep);
	}

	TEST_CASE("a pinned key is found again after a sweep") {
		const size_t saved = ba_decision_pins;
		ba_decision_pins = 4096;
		const size_t m0 = tau_ba_predicate_misses;
		{
			tau_ba<bv, sbf_ba> a(parse_wff("(x:sbf = 0) && (y:sbf = 1)"));
			CHECK_FALSE( a.is_zero() );
		}
		CHECK( tau_ba_predicate_misses == m0 + 1 );
		sweep();
		{
			tau_ba<bv, sbf_ba> b(parse_wff("(x:sbf = 0) && (y:sbf = 1)"));
			CHECK_FALSE( b.is_zero() );
		}
		CHECK( tau_ba_predicate_misses == m0 + 1 );
		ba_decision_pins = saved;
	}

	TEST_CASE("with the cap at 0 the row is lost at the sweep") {
		const size_t saved = ba_decision_pins;
		ba_decision_pins = 0;
		const size_t m0 = tau_ba_predicate_misses;
		{
			tau_ba<bv, sbf_ba> a(parse_wff("(z:sbf = 0) && (w:sbf = 1)"));
			CHECK_FALSE( a.is_zero() );
		}
		CHECK( tau_ba_predicate_misses == m0 + 1 );
		sweep();
		{
			tau_ba<bv, sbf_ba> b(parse_wff("(z:sbf = 0) && (w:sbf = 1)"));
			CHECK_FALSE( b.is_zero() );
		}
		CHECK( tau_ba_predicate_misses == m0 + 2 );
		ba_decision_pins = saved;
	}
}
