// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "tau_pack.h"
#include "test_tau_helpers.h"

namespace {
// tau_ba<bv, sbf_ba> is a different (smaller) node type than the ambient
// `node_t`/`tau` that test_tau_helpers.h defines for the rest of the suite
// (the 6-BA tau_ba<TAU_PACK_BASE_BAS> pack). `tref`s are
// only valid within the tree pool of the node type they were created in, so
// formulas fed to tau_ba<bv, sbf_ba> must be built via `small_tau`, not the
// ambient `tau` (see tests/unit/test_base_ba_dispatcher.cpp).
using small_node = idni::tau_lang::node<
	idni::tau_lang::tau_ba<idni::tau_lang::bv, idni::tau_lang::sbf_ba>,
	idni::tau_lang::bv, idni::tau_lang::sbf_ba>;
using small_tau = idni::tau_lang::tree<small_node>;

std::optional<rr<small_node>> small_get_nso_rr(const char* sample) {
	tref spec = small_tau::get(sample);
	if (spec == nullptr) return {};
	return idni::tau_lang::get_nso_rr<small_node>(spec);
}
} // namespace

TEST_SUITE("operators: negation") {

	TEST_CASE("{ F. }' = 0") {
		const char* sample = " { F. }' = 0 .";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}

	TEST_CASE("{ T. }' = 0") {
		const char* sample = " { T. }' = 0 .";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}
}

TEST_SUITE("tau_ba predicates") {

	TEST_CASE("is_zero detects contradiction") {
		tau_ba<bv, sbf_ba> fm(small_get_nso_rr("x = 0 && x != 0.")->main->get());
		CHECK(fm.is_zero());
		CHECK(!(fm == true));
	}

	TEST_CASE("is_one detects tautology") {
		tau_ba<bv, sbf_ba> fm(small_get_nso_rr("x = 0 || x != 0.")->main->get());
		CHECK(fm.is_one());
		CHECK(fm == true);
	}
}

TEST_SUITE("operators: conjunction") {

	TEST_CASE("{ T. } & { T. } = 0") {
		const char* sample = "{ T. } & { T. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}

	TEST_CASE("{ F. } & { T. } = 0") {
		const char* sample = "{ F. } & { T. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}

	TEST_CASE("{ T. } & { F. } = 0") {
		const char* sample = "{ T. } & { F. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}

	TEST_CASE("{ F. } & { F. } = 0") {
		const char* sample = "{ F. } & { F. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}
}

TEST_SUITE("operators: disjunction") {

	TEST_CASE("{ T. } | { T. } = 0") {
		const char* sample = "{ T. } | { T. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}

	TEST_CASE("{ F. } | { T. } = 0") {
		const char* sample = "{ F. } | { T. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}

	TEST_CASE("{ T. } | { F. } = 0") {
		const char* sample = "{ T. } | { F. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}

	TEST_CASE("{ F. } | { F. } = 0") {
		const char* sample = "{ F. } | { F. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}
}

TEST_SUITE("operators: exclusive or") {

	TEST_CASE("{ T. } ^ { T. } = 0") {
		const char* sample = "{ T. } ^ { T. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}

	TEST_CASE("{ F. } ^ { T. } = 0") {
		const char* sample = "{ F. } ^ { T. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}

	TEST_CASE("{ T. } ^ { F. } = 0") {
		const char* sample = "{ T. } ^ { F. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}

	TEST_CASE("{ F. } ^ { F. } = 0") {
		const char* sample = "{ F. } ^ { F. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}
}

TEST_SUITE("formulas: one level rec, no variables, no bindings and no quantifiers") {

	TEST_CASE("{ F. } = 0") {
		const char* sample = "{ F. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}

	TEST_CASE("{ T. } = 0") {
		const char* sample = "{ T. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}

	TEST_CASE("{ F. } != 0") {
		const char* sample = "{ F. } != 0.";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}

	TEST_CASE("{ T. } != 0") {
		const char* sample = "{ T. } != 0.";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}
}

TEST_SUITE("formulas: two level rec, no variables, no bindings and no quantifiers") {

	TEST_CASE("{ { F. } = 0. } = 0") {
		const char* sample = "{ { F. } = 0. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}

	TEST_CASE("{ { T. } = 0. } = 0") {
		const char* sample = "{ { T. } = 0. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}

	TEST_CASE("{ { F.} = 0. } != 0") {
		const char* sample = "{ { F. } = 0. } != 0.";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}

	TEST_CASE("{ { T. } = 0. } != 0") {
		const char* sample = "{ { T. } = 0. } != 0.";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}
}

TEST_SUITE("regression/bf_and not trimmed") {

	// The problem was related to not trimming the bf_and_sym in the grammar.
	// We could write the same formula in different ways and it would be
	// process differently

	TEST_CASE("ab = ba") {
		const char* sample = "ab = ba.";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}

	TEST_CASE("ab = (b & a)") {
		const char* sample = "ab = (b & a).";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}

	TEST_CASE("(a & b) = ba") {
		const char* sample = "(a & b) = ba.";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}

	TEST_CASE("(a & b) = (b & a)") {
		const char* sample = "(a & b) = (b & a).";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}

	TEST_CASE("((a & b) = (b & a))") {
		const char* sample = "((a & b) = (b & a)).";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}
}

// -----------------------------------------------------------------------------
// Regression: sugar connectives inside a Tau-BA constant (issue #69)
//
// A `{ ... }:tau` constant is parsed by `parse_tau` with reget_with_hooks off
// and, unlike every other parsed formula, never goes through `api::simplify`.
// Its main therefore used to reach the normalizer still carrying the sugar
// connectives (`->`, `<-`, `<->`, `^`, `? :`) that the construction hooks
// desugar everywhere else. Every pass below assumes NNF (only `&&`, `||` and
// negated atoms); `simplify_using_equality` in particular walked into an
// implication and registered its antecedent and consequent as *asserted*
// equalities of the enclosing conjunctive scope, which silently rewrote or
// dropped the surrounding conjuncts. The error went both ways: an
// unsatisfiable formula came back with a satisfiable residual, and a
// satisfiable one came back as `0`.
//
// Each case below is paired with the same formula written without sugar; the
// two must normalize to the same verdict.

TEST_SUITE("tau_ba constants: sugar connectives") {

	TEST_CASE("implication chain is unsatisfiable") {
		// o7=1 forces o9=1 forces o11=1, contradicting o11=0.
		const char* sample = "{ (o7[t]=1 -> o9[t]=1) "
			"&& (o9[t]=1 -> o11[t]=1) "
			"&& o7[t]=1 && o11[t]=0 } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}

	TEST_CASE("implication chain is unsatisfiable (desugared)") {
		const char* sample = "{ (!o7[t]=1 || o9[t]=1) "
			"&& (!o9[t]=1 || o11[t]=1) "
			"&& o7[t]=1 && o11[t]=0 } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}

	TEST_CASE("implication chain is satisfiable without the antecedent") {
		// Same chain, but o7=1 is not asserted, so o7=0 satisfies it.
		// Treating the antecedents as facts wrongly made this `0`.
		const char* sample = "{ (o7[t]=1 -> o9[t]=1) "
			"&& (o9[t]=1 -> o11[t]=1) && o11[t]=0 } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}

	TEST_CASE("implication chain is satisfiable without the antecedent"
		" (desugared)") {
		const char* sample = "{ (!o7[t]=1 || o9[t]=1) "
			"&& (!o9[t]=1 || o11[t]=1) && o11[t]=0 } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}

	TEST_CASE("reverse implication chain is satisfiable"
		" without the antecedent") {
		const char* sample = "{ (o9[t]=1 <- o7[t]=1) "
			"&& (o11[t]=1 <- o9[t]=1) && o11[t]=0 } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}

	TEST_CASE("equivalence chain is satisfiable") {
		// o7=o9=o11=0 satisfies it; o11=0 only forces o9 and o7 off.
		const char* sample = "{ (o7[t]=1 <-> o9[t]=1) "
			"&& (o9[t]=1 <-> o11[t]=1) && o11[t]=0 } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}

	TEST_CASE("equivalence chain is satisfiable (desugared)") {
		const char* sample = "{ (o7[t]=1 && o9[t]=1 "
			"|| !o7[t]=1 && !o9[t]=1) "
			"&& (o9[t]=1 && o11[t]=1 "
			"|| !o9[t]=1 && !o11[t]=1) && o11[t]=0 } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}
}

TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
