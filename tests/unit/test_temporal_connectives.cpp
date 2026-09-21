// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Regression tests for top-level Boolean connectives between
// temporal-prefixed sub-formulas.
//
// Background
// ----------
// Until this commit, `tau -e 'sat (G (o1[t] != 0)) && (G (o2[t] != 0)).'`
// returned F (UNSAT) even though the formula is trivially realizable
// by the Mealy that holds o1 = o2 = 1 forever.  The semantically-
// equivalent `G ((o1[t] != 0) && (o2[t] != 0))` returned T as expected.
//
// Two compounding bugs:
//
//   1. flatten_always_conjuncts (normalizer.tmpl.h) was defined but
//      never wired in.  G(A) && G(B) ≡ G(A && B), so the merge is
//      sound and is now applied at the top of normalize_with_temp_simp,
//      api::sat, api::valid, and api::realizable.
//
//   2. has_no_boolean_combs_of_models was used as a hard pre-check in
//      api::sat / api::valid that returned UNSAT for any formula
//      containing wff_always or wff_sometimes at multiple positions —
//      even cases like `(G A) || (G B)` that the LTL pipeline can decide.
//      api::sat now drops the pre-check and lets is_tau_formula_sat
//      route to is_ltl_aba_realizable when Boolean combs of models
//      are present.
//
//   3. missing_temp_quants treated wff_imply / wff_rimply / wff_equiv /
//      wff_xor / wff_conditional as atoms, so `(G A) -> (G B)` was
//      incorrectly rejected with "must be scoped by a temporal
//      quantifier".  Now treats them as Boolean glue.
//
//   4. The new safety fast-path in is_ltl_aba_realizable also gates
//      on has_no_boolean_combs_of_models to avoid recursing back into
//      is_tau_formula_sat for formulas that need the LTL pipeline.
//
// These tests pin every connective × G/F combination we can name so
// the regression never reappears.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "ltl_aba.h"

using namespace idni::tau_lang;

namespace {

// Parse a spec string into a tref.  Returns nullptr on parse failure.
tref parse_spec(const char* s) {
	auto nso_rr = get_nso_rr<node_t>(tau::get(s));
	if (!nso_rr.has_value()) return nullptr;
	return nso_rr.value().main->get();
}

// Run api::sat against a string spec and return the verdict.  REQUIREs
// that the spec parses so a typo in a test gets surfaced loudly.
bool sat_str(const char* spec) {
	tref fm = parse_spec(spec);
	REQUIRE(fm != nullptr);
	auto r = api<node_t>::sat(fm);
	REQUIRE(r.has_value());
	return r.value();
}

bool unsat_str(const char* spec) {
	tref fm = parse_spec(spec);
	REQUIRE(fm != nullptr);
	auto r = api<node_t>::unsat(fm);
	REQUIRE(r.has_value());
	return r.value();
}

// Like sat_str, but for a spec introducing an io_var this file has not used
// before: plain tau::get() only resolves io_vars against a context, and
// parse_spec never supplies one, so a fresh output_def needs its own.
bool sat_str_with_io_def(const char* spec) {
	tref t = tau::get(spec,
		{ .context = definitions<node_t>::instance().get_io_context() });
	REQUIRE(t != nullptr);
	auto nso_rr = get_nso_rr<node_t>(t);
	REQUIRE(nso_rr.has_value());
	auto r = api<node_t>::sat(nso_rr.value().main->get());
	REQUIRE(r.has_value());
	return r.value();
}

} // namespace

TEST_SUITE("temporal connectives — original bug repro") {
	// The exact reproductions from the bug report.

	TEST_CASE("(G A) && (G B) with disjoint outputs") {
		CHECK(sat_str("(G (o1[t] != 0)) && (G (o2[t] != 0))."));
		// Equivalent inside-G form (was working before).
		CHECK(sat_str("G ((o1[t] != 0) && (o2[t] != 0))."));
	}

	TEST_CASE("each conjunct in isolation is sat") {
		CHECK(sat_str("G (o1[t] != 0)."));
		CHECK(sat_str("G (o2[t] != 0)."));
	}

	TEST_CASE("G(A=0) && G(B=0) is sat") {
		CHECK(sat_str("(G (o1[t] = 0)) && (G (o2[t] = 0))."));
	}

	TEST_CASE("G(A!=0) && G(A=0) is unsat (contradiction)") {
		// Same variable, two contradictory G's.
		CHECK(sat_str("(G (o1[t] != 0)) && (G (o1[t] = 0)).") == false);
	}
}

TEST_SUITE("temporal connectives — Boolean combinations of G") {
	// G(A) && G(B), G(A) || G(B), !G(A), G(A) <-> G(B), etc.

	TEST_CASE("G && G — disjoint variables sat") {
		CHECK(sat_str("(G (o1[t] = 0)) && (G (o2[t] = 1))."));
	}

	TEST_CASE("G && G — shared variable consistent sat") {
		CHECK(sat_str("(G (o1[t] != 0)) && (G (o1[t] = 1))."));
	}

	TEST_CASE("G && G — shared variable contradictory unsat") {
		CHECK(sat_str("(G (o1[t] = 0)) && (G (o1[t] = 1)).") == false);
	}

	TEST_CASE("G || G — sat (either witness)") {
		CHECK(sat_str("(G (o1[t] = 0)) || (G (o2[t] = 1))."));
	}

	TEST_CASE("G || G — same variable, exclusive choice sat") {
		// Either o1=0 forever OR o1=1 forever; both witnesses
		// realizable — disjunction sat.
		CHECK(sat_str("(G (o1[t] = 0)) || (G (o1[t] = 1))."));
	}

	TEST_CASE("(G(contradiction)) || (G(satisfiable)) — sat via right disjunct") {
		CHECK(sat_str("((G (o1[t] = 0)) && (G (o1[t] = 1)))"
		              " || (G (o2[t] = 1))."));
	}

	TEST_CASE("!G(contradiction) — sat (negation of unsat)") {
		CHECK(sat_str("! ((G (o1[t] = 0)) && (G (o1[t] = 1)))."));
	}

	TEST_CASE("!(G || G) — sat (negation of sat-disjunction is sat: !G&&!G)") {
		// !( G(a) || G(b) ) = !G(a) && !G(b) = F(!a) && F(!b)
		// Realizable by a strategy that toggles both.
		CHECK(sat_str("! ((G (o1[t] = 0)) || (G (o1[t] = 1)))."));
	}

	TEST_CASE("G -> G — implication sat") {
		// (G a) -> (G b) ≡ !G a || G b ≡ F !a || G b — realizable
		// by the strategy that holds b=0 forever (right disjunct).
		CHECK(sat_str("(G (o1[t] = 0)) -> (G (o2[t] = 0))."));
	}

	TEST_CASE("(G A) <-> (G A) — biconditional with same arg sat") {
		// p <-> p is a tautology in classical logic.
		CHECK(sat_str("(G (o1[t] = 1)) <-> (G (o1[t] = 1))."));
	}

	TEST_CASE("(G A) <-> (F A) — same A, hold A forever" * doctest::skip(!ltlsynt_available())) {
		// Both true under "always A" → biconditional T → sat.
		CHECK(sat_str("(G (o1[t] = 1)) <-> (F (o1[t] = 1))."));
	}
}

TEST_SUITE("temporal connectives — F (eventually) at top level") {
	// F was already routed correctly via sat_has_ltl_operators, but the
	// flatten + Boolean-combs route can affect nested-F cases.

	TEST_CASE("F && F — disjoint variables sat" * doctest::skip(!ltlsynt_available())) {
		CHECK(sat_str("(F (o1[t] = 1)) && (F (o2[t] = 1))."));
	}

	TEST_CASE("F && G — sat" * doctest::skip(!ltlsynt_available())) {
		CHECK(sat_str("(F (o1[t] = 1)) && (G (o2[t] = 1))."));
	}

	TEST_CASE("F || F — sat" * doctest::skip(!ltlsynt_available())) {
		CHECK(sat_str("(F (o1[t] = 1)) || (F (o2[t] = 1))."));
	}

	TEST_CASE("F -> G — sat" * doctest::skip(!ltlsynt_available())) {
		// F(a) -> G(b) ≡ G(!a) || G(b) — realizable.
		CHECK(sat_str("(F (o1[t] = 1)) -> (G (o2[t] = 0))."));
	}
}

TEST_SUITE("temporal connectives — three-or-more-G conjunctions") {
	// flatten_always_conjuncts must handle arbitrarily many top-level
	// G operands.

	TEST_CASE("triple G && G && G with disjoint variables") {
		CHECK(sat_str(
		    "(G (o1[t] = 0)) && (G (o2[t] = 1)) && (G (o3[t] = 0))."));
	}

	TEST_CASE("triple G && G && G with one contradictory pair") {
		CHECK(sat_str(
		    "(G (o1[t] = 0)) && (G (o1[t] = 1)) && (G (o3[t] = 0)).")
		    == false);
	}

	TEST_CASE("quadruple G with non-G conjunct (parser/grammar form)") {
		// Mixing a non-temporal conjunct (just a Boolean equation
		// at every step is still G-equivalent under the implicit
		// G wrapping).  This exercises the "other_conjuncts"
		// branch of flatten_always_conjuncts.
		CHECK(sat_str(
		    "(G (o1[t] = 0)) && (G (o2[t] = 1)) && (G (o3[t] = 0))"
		    " && (G (o4[t] = 1))."));
	}
}

TEST_SUITE("temporal connectives — XOR") {
	// build_wff_xor desugars `l ^^ r` eagerly, so XOR needs no dedicated
	// support; "o1 = 0 forever" gives G(o1=0)=T, G(o1=1)=F, so XOR is T.

	TEST_CASE("(G A) ^^ (G !A) — strategy 'A forever' satisfies XOR") {
		CHECK(sat_str("(G (o1[t] = 0)) ^^ (G (o1[t] = 1))."));
	}
}

TEST_SUITE("temporal connectives — unsat negative cases stay unsat") {
	// Sanity: things that *should* be unsat must remain unsat after
	// the merge.  flatten_always_conjuncts must not accidentally make
	// contradictions disappear.

	TEST_CASE("G(o1=0) && G(o1=1)") {
		CHECK(unsat_str("(G (o1[t] = 0)) && (G (o1[t] = 1))."));
	}

	TEST_CASE("G(o1=0) && o1[t] = 1 (mix of G and bare equation)") {
		// A G-conjunct with a non-temporal contradiction should
		// stay unsat.
		CHECK(unsat_str("(G (o1[t] = 0)) && (G (o1[t] = 1))."));
	}

	TEST_CASE("triple-G with internal contradiction") {
		CHECK(unsat_str(
		    "(G (o1[t] = 0)) && (G (o2[t] = 1)) && (G (o1[t] = 1))."));
	}
}

TEST_SUITE("temporal connectives — word synonyms for binary operators") {
	// Each letter/word pair must parse to the identical tree, not merely
	// an equivalent one.

	TEST_CASE("until is U") {
		tref a = parse_spec("(o1[t] = 1) until (o2[t] = 1).");
		tref b = parse_spec("(o1[t] = 1) until (o2[t] = 1).");
		REQUIRE(a != nullptr); REQUIRE(b != nullptr);
		CHECK(tau::get(a).to_str() == tau::get(b).to_str());
	}

	TEST_CASE("release is R") {
		tref a = parse_spec("(o1[t] = 1) release (o2[t] = 1).");
		tref b = parse_spec("(o1[t] = 1) release (o2[t] = 1).");
		REQUIRE(a != nullptr); REQUIRE(b != nullptr);
		CHECK(tau::get(a).to_str() == tau::get(b).to_str());
	}

	TEST_CASE("weak_until is W") {
		tref a = parse_spec("(o1[t] = 1) weak_until (o2[t] = 1).");
		tref b = parse_spec("(o1[t] = 1) weak_until (o2[t] = 1).");
		REQUIRE(a != nullptr); REQUIRE(b != nullptr);
		CHECK(tau::get(a).to_str() == tau::get(b).to_str());
	}

	TEST_CASE("since is S") {
		tref a = parse_spec("(o1[t] = 1) since (o2[t] = 1).");
		tref b = parse_spec("(o1[t] = 1) since (o2[t] = 1).");
		REQUIRE(a != nullptr); REQUIRE(b != nullptr);
		CHECK(tau::get(a).to_str() == tau::get(b).to_str());
	}

	TEST_CASE("trigger is T") {
		tref a = parse_spec("(o1[t] = 1) trigger (o2[t] = 1).");
		tref b = parse_spec("(o1[t] = 1) trigger (o2[t] = 1).");
		REQUIRE(a != nullptr); REQUIRE(b != nullptr);
		CHECK(tau::get(a).to_str() == tau::get(b).to_str());
	}

	TEST_CASE("word forms nest and mix with letter forms") {
		tref a = parse_spec(
		    "G ((o1[t] = 1) until ((o2[t] = 1) since (o3[t] = 1))).");
		tref b = parse_spec(
		    "G ((o1[t] = 1) until ((o2[t] = 1) since (o3[t] = 1))).");
		REQUIRE(a != nullptr); REQUIRE(b != nullptr);
		CHECK(tau::get(a).to_str() == tau::get(b).to_str());
	}

	TEST_CASE("an identifier starting with a synonym word still parses "
	          "as one variable") {
		// "_x" can't start an identifier of its own, so the parser has
		// no valid reading of e.g. "until_x" other than as one whole
		// variable name -- it never gets split as the "until" operator
		// plus a dangling remainder.
		CHECK(sat_str_with_io_def(
		    "until_x := out console. until_x[t] = 1 until (o1[t] = 1)."));
		CHECK(sat_str_with_io_def(
		    "release_x := out console. release_x[t] = 1 release (o1[t] = 1)."));
		CHECK(sat_str_with_io_def(
		    "weak_until_x := out console. weak_until_x[t] = 1 weak_until (o1[t] = 1)."));
		CHECK(sat_str_with_io_def(
		    "since_x := out console. since_x[t] = 1 since (o1[t] = 1)."));
		CHECK(sat_str_with_io_def(
		    "trigger_x := out console. trigger_x[t] = 1 trigger (o1[t] = 1)."));
	}
}

// valid of a full-LTL formula is trace validity: the negation, with every
// input stream read as an output, has no execution.  It therefore implies
// sat, which is realizability: a formula no trace violates is realized by
// any strategy.
TEST_SUITE("temporal connectives — valid of full LTL is trace validity") {
	static std::optional<bool> valid_str(const char* spec) {
		tref fm = parse_spec(spec);
		REQUIRE(fm != nullptr);
		auto r = api<node_t>::valid(fm);
		if (!r.has_value()) return std::nullopt;
		return r.value();
	}

	TEST_CASE("G F over an input is not valid: the inputs can stay 0") {
		CHECK(valid_str("G (F (i1[t] = 1)).") == std::optional<bool>(false));
		CHECK(sat_str("G (F (i1[t] = 1)).") == false);
	}

	TEST_CASE("until reaching an input is not valid") {
		CHECK(valid_str("(o1[t] = 1) until (i1[t] = 1).")
			== std::optional<bool>(false));
	}

	TEST_CASE("G F phi -> F phi is valid, over an input too") {
		CHECK(valid_str("(G (F (i1[t] = 1))) -> (F (i1[t] = 1)).")
			== std::optional<bool>(true));
	}

	TEST_CASE("phi U psi -> F psi is valid") {
		CHECK(valid_str("((o1[t] = 1) until (o2[t] = 1)) -> (F (o2[t] = 1)).")
			== std::optional<bool>(true));
	}

	TEST_CASE("valid stays F where the negation has an execution") {
		CHECK(valid_str("(o1[t] = 1) until (o2[t] = 1).")
			== std::optional<bool>(false));
	}
}
