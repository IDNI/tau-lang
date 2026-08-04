// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// The two opposite atom-spelling preconditions that meet in this module:
//
//   squeeze_positives (normal_forms_transformations.tmpl.h:382-385) DBG-asserts
//   its input holds no wff_neg(bf_eq). Feeding it `!(g = 0)` makes its
//   select_top descend through the wff_neg and fold the equation into the
//   POSITIVE squeeze, while the disequation scan never re-adds the negation --
//   so `ex x (x a = 0 && !(x b = 0))` collapses to T, the disequation inverted
//   and dropped.
//
//   The block squeeze requires the OPPOSITE: a bf_neq-free clause, which
//   normalize_atomic_formula_operators establishes by rewriting `!= 0` into
//   `!(= 0)`.
//
// Neither spelling can be made global -- squeeze_positives has a second caller
// at interpreter.tmpl.h:1156. Each precondition is established locally at its
// point of use. These cases pin both paths against the collapse.

#include "test_init.h"
#include "test_tau_helpers.h"

// Both public entries are pinned in every case. Neither fails today: the
// collapse is latent, reachable only once the two paths share one leaf
// handler. These are the guard rails for that merge, not red tests -- they
// pass before it and must keep passing after.
namespace {

// The two public entry points into quantifier elimination. `anti_prenex` is
// the legacy step-based algorithm, `anti_prenex_block` the block one; the
// merged leaf handler is reached through both.
tref by_legacy(tref fm) { return anti_prenex<node_t>(fm); }
tref by_block(tref fm)  { return anti_prenex_block<node_t>(fm); }

using entry = tref (*)(tref);
const std::pair<const char*, entry> entries[] = {
	{ "anti_prenex", by_legacy }, { "anti_prenex_block", by_block } };

tref parse(const char* sample) {
	return get_nso_rr(sample).value().main->get();
}

} // namespace

TEST_SUITE("leaf_clause") {

	// The hazard reached through the single-variable path.
	TEST_CASE("a disequation is not silently dropped, single variable") {
		for (const auto& [name, run] : entries) {
			CAPTURE(name);
			CHECK( !tau::get(run(parse(
				"ex x (x a = 0 && !(x b = 0))."))).equals_T() );
		}
	}

	// The same hazard reached through the block path, which has the
	// opposite precondition.
	TEST_CASE("a disequation is not silently dropped, block") {
		for (const auto& [name, run] : entries) {
			CAPTURE(name);
			CHECK( !tau::get(run(parse(
				"ex x, y (x y a = 0 && !(x y b = 0))."))).equals_T() );
		}
	}

	// Both spellings of the same disequation must reach the same answer.
	TEST_CASE("the bf_neq spelling gives the same answer as !(= 0)") {
		for (const auto& [name, run] : entries) {
			CAPTURE(name);
			tref a = run(parse("ex x (x a = 0 && x b != 0)."));
			tref b = run(parse("ex x (x a = 0 && !(x b = 0))."));
			CHECK( tau::get(a) == tau::get(b) );
		}
	}

	// A conjunct free of every block variable does not belong under the
	// binder, and the binder itself is fully eliminable here.
	TEST_CASE("an independent conjunct is lifted out of the binder") {
		for (const auto& [name, run] : entries) {
			CAPTURE(name);
			CHECK( !tau::get(run(parse("ex x (x a = 0 && w = 0).")))
				.find_top(is<node_t, tau::wff_ex>) );
		}
	}
}
