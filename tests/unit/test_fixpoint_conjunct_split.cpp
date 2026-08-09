// Regression test for the per-conjunct fixpoint check.
//
// `fixpoint_reached(prev, next)` splits `next` into its top-level conjuncts and
// checks `prev |= c` for each. That rests on an elementary equivalence:
//
//     phi |= psi_1 & ... & psi_n   <=>   for all i. phi |= psi_i
//
// so the split must never change the answer. These cases pin exactly that, and
// they are written so that a broken split FAILS:
//   * a conjunction whose LAST conjunct is not implied catches a collector that
//     only walks the first branch, and an early exit that returns too soon;
//   * a conjunction that is implied catches an inverted return.
//
// The reference is a DIRECT `is_nso_impl` call, not the helper with the flag
// off. Two reasons, and the second one bit us:
//   * `is_nso_impl` is what the helper has to agree with -- that is the contract,
//     while "helper with the flag off" is only a proxy for it;
//   * the flag is `fixpoint_conjunct_split || env_enabled`, so with
//     TAU_QE_FIXSPLIT set in the environment the "off" side is ON as well and the
//     comparison degenerates to on-vs-on. A suite run with the switch enabled
//     would then pass this file without testing anything.
//
// What this does NOT test: that the split is faster. It cannot -- the change is
// semantically neutral, so the only in-process observable is time, and time does
// not belong in an assertion. The effect is documented with measurements instead.
// Nor does the non-conjunction case prove the fallback is taken: a collector that
// returns a single-element list gives the same answer. It pins the answer, not
// the path.

#include "test_init.h"
#include "test_tau_helpers.h"

namespace {

// `x = 0` for a fresh atomless-typed variable.
tref atom(const char* name) {
	return tau::build_bf_eq_0(
		build_bf_variable<node_t>(name, tau_type_id<node_t>()));
}

// The undecomposed reference answer, and the split one. Both must agree.
// `ref` is a direct `is_nso_impl` so the test holds even when TAU_QE_FIXSPLIT is
// set in the environment (see the note at the top of this file).
std::pair<bool, bool> ref_and_split(tref prev, tref next) {
	const bool ref = is_nso_impl<node_t>(prev, next);
	api<node_t>::set_fixpoint_conjunct_split(true);
	const bool split = fixpoint_reached<node_t>(prev, next);
	api<node_t>::set_fixpoint_conjunct_split(false);
	return {ref, split};
}

} // namespace

TEST_SUITE("configuration") {
	TEST_CASE("bdd_init") { bdd_init<Bool>(); }
}

TEST_SUITE("fixpoint check per conjunct") {

	TEST_CASE("conjunction that is implied") {
		tref a = atom("fcs_a"), b = atom("fcs_b");
		tref prev = tau::build_wff_and(a, b);
		tref next = tau::build_wff_and(a, b);
		auto [ref, split] = ref_and_split(prev, next);
		CHECK( ref == split );
		CHECK( split );
	}

	TEST_CASE("first conjunct not implied") {
		tref a = atom("fcs_c"), b = atom("fcs_d");
		tref prev = b;
		tref next = tau::build_wff_and(a, b);
		auto [ref, split] = ref_and_split(prev, next);
		CHECK( ref == split );
		CHECK( !split );
	}

	// The one that catches a collector walking only one branch, and an early
	// exit that stops before the last conjunct.
	TEST_CASE("last conjunct not implied") {
		tref a = atom("fcs_e"), b = atom("fcs_f"), c = atom("fcs_g");
		tref prev = tau::build_wff_and(a, b);
		tref next = tau::build_wff_and(tau::build_wff_and(a, b), c);
		auto [ref, split] = ref_and_split(prev, next);
		CHECK( ref == split );
		CHECK( !split );
	}

	TEST_CASE("nested conjunction, middle conjunct not implied") {
		tref a = atom("fcs_h"), b = atom("fcs_i"), c = atom("fcs_j");
		tref prev = tau::build_wff_and(a, c);
		tref next = tau::build_wff_and(a, tau::build_wff_and(b, c));
		auto [ref, split] = ref_and_split(prev, next);
		CHECK( ref == split );
		CHECK( !split );
	}

	// Not a conjunction. This pins the ANSWER on that path; it cannot pin that
	// the fallback was taken (see the note at the top).
	TEST_CASE("single atom, not a conjunction") {
		tref a = atom("fcs_k");
		auto [ref, split] = ref_and_split(a, a);
		CHECK( ref == split );
		CHECK( split );
	}

	TEST_CASE("single atom not implied") {
		tref a = atom("fcs_l"), b = atom("fcs_m");
		auto [ref, split] = ref_and_split(a, b);
		CHECK( ref == split );
		CHECK( !split );
	}
}
