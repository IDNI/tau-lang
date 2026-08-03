// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

TEST_SUITE("eliminability") {

	TEST_CASE("join is idempotent") {
		CHECK(join(elim_verdict::eliminable, elim_verdict::eliminable)
			== elim_verdict::eliminable);
		CHECK(join(elim_verdict::frozen, elim_verdict::frozen)
			== elim_verdict::frozen);
	}

	TEST_CASE("eliminable is the identity") {
		CHECK(join(elim_verdict::eliminable, elim_verdict::solver_owned)
			== elim_verdict::solver_owned);
		CHECK(join(elim_verdict::arith_residue, elim_verdict::eliminable)
			== elim_verdict::arith_residue);
	}

	TEST_CASE("frozen is the top element") {
		// A reference makes content untouchable by *any* destination,
		// including the solver -- so nothing demotes frozen.
		CHECK(join(elim_verdict::frozen, elim_verdict::eliminable)
			== elim_verdict::frozen);
		CHECK(join(elim_verdict::frozen, elim_verdict::solver_owned)
			== elim_verdict::frozen);
		CHECK(join(elim_verdict::frozen, elim_verdict::arith_residue)
			== elim_verdict::frozen);
	}

	TEST_CASE("arith_residue outranks solver_owned") {
		// Arithmetic cvc5 cannot express must not be routed to it: the
		// narrower verdict wins where both apply.
		CHECK(join(elim_verdict::solver_owned, elim_verdict::arith_residue)
			== elim_verdict::arith_residue);
		CHECK(join(elim_verdict::arith_residue, elim_verdict::solver_owned)
			== elim_verdict::arith_residue);
	}

	TEST_CASE("join is commutative across every pair") {
		const elim_verdict all[] = { elim_verdict::eliminable,
			elim_verdict::solver_owned, elim_verdict::arith_residue,
			elim_verdict::frozen };
		for (elim_verdict a : all) for (elim_verdict b : all)
			CHECK(join(a, b) == join(b, a));
	}
}
