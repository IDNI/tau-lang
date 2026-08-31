// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Every (solver_placement x blast_placement x blast_method) combination must
// normalize a bv formula to an equivalent result. The default combination
// (eager/per_leaf/anti_prenex_result) is the baseline; the others were
// measured fully untested (antiprenexing.tmpl.h:1271-1369, :176-207,
// normalizer.tmpl.h:261-265).

#include "test_init.h"
#include "test_tau_helpers.h"

namespace {

struct placement_guard {
	solver_site  sp = solver_placement;
	blast_site   bp = blast_placement;
	blast_mode   bm = blast_method;
	bool         bb = bv_blasting;
	size_t       rd = max_blast_reentry_depth;
	~placement_guard() {
		solver_placement = sp; blast_placement = bp;
		blast_method = bm; bv_blasting = bb;
		max_blast_reentry_depth = rd;
	}
};

tref parse(const char* sample) {
	return get_nso_rr(sample).value().main->get();
}

tref norm_under(const char* sample, solver_site sp, blast_site bp,
	blast_mode bm, bool blasting)
{
	placement_guard g;
	solver_placement = sp; blast_placement = bp;
	blast_method = bm; bv_blasting = blasting;
	return normalizer<node_t>(parse(sample));
}

} // namespace

TEST_SUITE("placement matrix") {

	// Closed bv formulas with a definite verdict: T and F must come out
	// identically under every placement.
	static constexpr const char* closed_sat =
		"ex x : bv[8] x = { 1 }:bv[8].";
	static constexpr const char* closed_unsat =
		"ex x : bv[8] (x = { 0 }:bv[8] && x = { 1 }:bv[8]).";
	// An alternation the solver sites treat differently from the leaves.
	static constexpr const char* alternating =
		"all x : bv[8] ex y : bv[8] x + y = { 0 }:bv[8].";

	TEST_CASE("solver placements agree, blasting off") {
		for (const char* s : { closed_sat, closed_unsat, alternating }) {
			tref base = norm_under(s, solver_site::eager,
				blast_site::per_leaf,
				blast_mode::anti_prenex_result, false);
			for (solver_site sp : { solver_site::per_closed_block,
						solver_site::per_formula }) {
				tref r = norm_under(s, sp, blast_site::per_leaf,
					blast_mode::anti_prenex_result, false);
				REQUIRE( r != nullptr );
				CHECK( are_nso_equivalent<node_t>(r, base) );
			}
		}
	}

	TEST_CASE("blast placements and modes agree, blasting on") {
		for (const char* s : { closed_sat, closed_unsat, alternating }) {
			tref base = norm_under(s, solver_site::eager,
				blast_site::per_leaf,
				blast_mode::anti_prenex_result, true);
			for (blast_site bp : { blast_site::per_block,
						blast_site::per_formula })
				for (blast_mode bm : {
					blast_mode::anti_prenex_result,
					blast_mode::defer }) {
				tref r = norm_under(s, solver_site::eager,
					bp, bm, true);
				REQUIRE( r != nullptr );
				CHECK( are_nso_equivalent<node_t>(r, base) );
			}
			// defer at the default site too (per_leaf x defer).
			tref r = norm_under(s, solver_site::eager,
				blast_site::per_leaf, blast_mode::defer, true);
			REQUIRE( r != nullptr );
			CHECK( are_nso_equivalent<node_t>(r, base) );
		}
	}

	// Nonlinear bv blocks (x*x is not invertible by trivial Skolem) cross
	// the placements on formulas the resolve passes cannot finish alone.
	// NOTE: even these leave `blk.displaced` empty -- the trailing
	// blasteable-segment handling of resolve_ex_block
	// (antiprenexing.tmpl.h ~1494-1512) stayed unreached by every probe
	// tried (2026-08-31 coverage pass) and remains untested.
	static constexpr const char* nonlinear =
		"ex x : bv[8] x * x = { 4 }:bv[8].";
	static constexpr const char* nonlinear_pair =
		"ex x : bv[8] ex y : bv[8] (x * y = { 6 }:bv[8]"
		" && x > { 1 }:bv[8]).";

	TEST_CASE("nonlinear blocks agree across solver placements") {
		for (const char* s : { nonlinear, nonlinear_pair }) {
			tref base = norm_under(s, solver_site::eager,
				blast_site::per_leaf,
				blast_mode::anti_prenex_result, false);
			for (solver_site sp : { solver_site::per_closed_block,
						solver_site::per_formula }) {
				tref r = norm_under(s, sp, blast_site::per_leaf,
					blast_mode::anti_prenex_result, false);
				REQUIRE( r != nullptr );
				CHECK( are_nso_equivalent<node_t>(r, base) );
			}
			// per_closed_block with blasting on per_block
			tref r = norm_under(s, solver_site::per_closed_block,
				blast_site::per_block,
				blast_mode::anti_prenex_result, true);
			REQUIRE( r != nullptr );
			CHECK( are_nso_equivalent<node_t>(r, base) );
		}
	}

	TEST_CASE("blast re-entry depth cap keeps the result sound") {
		placement_guard g;
		bv_blasting = true;
		blast_placement = blast_site::per_leaf;
		blast_method = blast_mode::anti_prenex_result;
		max_blast_reentry_depth = 1;
		tref capped = normalizer<node_t>(parse(alternating));
		max_blast_reentry_depth = 0;
		tref uncapped = normalizer<node_t>(parse(alternating));
		REQUIRE( capped != nullptr );
		CHECK( are_nso_equivalent<node_t>(capped, uncapped) );
	}
}
