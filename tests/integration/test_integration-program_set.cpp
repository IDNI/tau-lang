// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Verdicts of program-set expressions (src/program_set.h) through the API:
// the Boolean operations on sets of programs, the time boundaries under
// tau's warm-up convention, memory, lookahead and the abstraction. One
// input i1, one visible output o1, type sbf.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "api.h"
#include <sstream>

using namespace idni::tau_lang;

namespace {

// The nonemptiness verdict of the program-set expression @p term spells,
// parsed with the hooks off (on, two tau constants fold into one).
std::optional<bool> nonempty(const char* term) {
	tref t = nullptr;
	{
		// the decision itself runs with the hooks on
		use_hooks_guard<node_t> hooks_off(false);
		t = tau::get(term, parse_bf()).value_or(nullptr);
	}
	REQUIRE( t );
	auto r = api<node_t>::program_set_realizable(t);
	if (!r.has_value()) {
		// the report tells an undecided verdict from a refused expression
		std::ostringstream why;
		r.report().print(idni::diagnostics::sinks{
			.error = &why, .warning = &why, .info = {} },
			term::colors(false));
		MESSAGE( why.str() );
		return std::nullopt;
	}
	return r.value();
}

const std::optional<bool> T = true, F = false;

} // namespace

#define PS_CASE(name) TEST_CASE(name * doctest::skip(!ltlsynt_available()))

TEST_SUITE("program_set Boolean operations") {

	PS_CASE("T1 the complement of the copier is nonempty") {
		CHECK( nonempty("{ always o1[t]:sbf = i1[t]:sbf }:tau'") == T );
	}
	PS_CASE("T2 a set and its complement are disjoint") {
		CHECK( nonempty("{ always o1[t]:sbf = i1[t]:sbf }:tau"
			" & { always o1[t]:sbf = i1[t]:sbf }:tau'") == F );
	}
	PS_CASE("T3 the copier is outside 'eventually 1': the zero input") {
		CHECK( nonempty("{ always o1[t]:sbf = i1[t]:sbf }:tau"
			" & { sometimes o1[t]:sbf = 1 }:tau'") == T );
	}
	PS_CASE("T4 copying entails the implication") {
		CHECK( nonempty("{ always o1[t]:sbf = i1[t]:sbf }:tau"
			" & { always (i1[t]:sbf = 1 -> o1[t]:sbf = 1) }:tau'") == F );
	}
	PS_CASE("T5 one branch of a union cannot fit both inputs") {
		CHECK( nonempty("({ always o1[t]:sbf = 1 }:tau | { always o1[t]:sbf = 0 }:tau)"
			" & { always ((i1[t]:sbf = 1) <-> (o1[t]:sbf = 1)) }:tau") == F );
	}
	PS_CASE("T6 no program forces the first input: the complement is everything") {
		CHECK( nonempty("{ always (i1[t]:sbf = 1 || [t > 0]) }:tau'") == T );
	}
	PS_CASE("T7 no program fails a tautology") {
		CHECK( nonempty("{ always o1[t]:sbf = o1[t]:sbf }:tau'") == F );
	}
	PS_CASE("T8 two distinct values share the atomless type") {
		CHECK( nonempty(
			"{ always ((i1[t]:sbf != 0 && i1[t]:sbf != 1) -> o1[t]:sbf != i1[t]:sbf) }:tau'"
			" & { always ((i1[t]:sbf != 0 && i1[t]:sbf != 1) -> o1[t]:sbf != i1[t]:sbf') }:tau'")
			== T );
	}
	// The first output may vary with the first input, which needs the
	// prediction chosen before step 0: both complements hold of the program
	// `o1[0] = i1[0]'`, each on its own first input. The third literal
	// brings the input into the expression: tau's programs read the
	// streams the expression names.
	PS_CASE("T9 the first output varies with the first input") {
		CHECK( nonempty("{ always (o1[t]:sbf = 1 || [t > 0]) }:tau'"
			" & { always (o1[t]:sbf != 1 || [t > 0]) }:tau'"
			" & { always o2[t]:sbf = i1[t]:sbf }:tau") == T );
	}
	// Without an input the expression is input-free: a program has one
	// trace, whose first output is one value.
	PS_CASE("T9' with no input the two complements are disjoint") {
		CHECK( nonempty("{ always (o1[t]:sbf = 1 || [t > 0]) }:tau'"
			" & { always (o1[t]:sbf != 1 || [t > 0]) }:tau'") == F );
	}
	PS_CASE("T10 the zero input path cannot have both first outputs") {
		CHECK( nonempty(
			"{ (always i1[t]:sbf = 0) -> always (o1[t]:sbf != 1 || [t > 0]) }:tau'"
			" & { (always i1[t]:sbf = 0) -> always (o1[t]:sbf = 1 || [t > 0]) }:tau'")
			== F );
	}
	PS_CASE("T11 eventual but not recurring") {
		CHECK( nonempty("{ sometimes o1[t]:sbf = 1 }:tau"
			" & { always sometimes o1[t]:sbf = 1 }:tau'") == T );
	}
	PS_CASE("T12 emit 1 iff the input is nonzero") {
		CHECK( nonempty("{ always (i1[t]:sbf != 0 -> sometimes o1[t]:sbf = 1) }:tau"
			" & { sometimes o1[t]:sbf = 1 }:tau'") == T );
	}
	PS_CASE("T13 recurring entails eventual") {
		CHECK( nonempty("{ always sometimes o1[t]:sbf = 1 }:tau"
			" & { sometimes o1[t]:sbf = 1 }:tau'") == F );
	}
	PS_CASE("T15 the complement of everything is empty") {
		CHECK( nonempty("({ always o1[t]:sbf = 1 }:tau | { always o1[t]:sbf = 1 }:tau')'")
			== F );
	}
}

TEST_SUITE("program_set Tau BA regressions") {

	PS_CASE("the copier is not included in constant zero") {
		CHECK( nonempty("{ always o1[t]:sbf = i1[t]:sbf }:tau"
			" & { always o1[t]:sbf = 0 }:tau'") == T );
	}
	PS_CASE("constant zero is not included in the copier") {
		CHECK( nonempty("{ always o1[t]:sbf = 0 }:tau"
			" & { always o1[t]:sbf = i1[t]:sbf }:tau'") == T );
	}
	PS_CASE("constant one is not included in constant zero") {
		CHECK( nonempty("{ always o1[t]:sbf = 1 }:tau"
			" & { always o1[t]:sbf = 0 }:tau'") == T );
	}
	PS_CASE("the copier is included in itself") {
		CHECK( nonempty("{ always o1[t]:sbf = i1[t]:sbf }:tau"
			" & { always o1[t]:sbf = i1[t]:sbf }:tau'") == F );
	}
}

TEST_SUITE("program_set time boundaries") {

	PS_CASE("B2 a lookback clause is enforced from step 1") {
		CHECK( nonempty("{ always o1[t]:sbf = o1[t-1]:sbf }:tau") == T );
	}
	// Each literal keeps its own warm-up: constant 1 keeps its value from
	// step 1 on, so it is in the second set.
	PS_CASE("B3 constant one is in 'keep the last value'") {
		CHECK( nonempty("{ always o1[t]:sbf = 1 }:tau"
			" & { always o1[t]:sbf = o1[t-1]:sbf }:tau'") == F );
	}
	PS_CASE("B4 one at step 0 and a change later is outside 'keep the last value'") {
		CHECK( nonempty("{ always (o1[t]:sbf = 1 || [t > 0]) }:tau"
			" & { always o1[t]:sbf = o1[t-1]:sbf }:tau'") == T );
	}
	PS_CASE("W1 an intermediate lag is kept") {
		CHECK( nonempty("{ always o1[t]:sbf = i1[t-2]:sbf }:tau") == T );
	}
	PS_CASE("L1 lookahead is reindexed before the window") {
		CHECK( nonempty("{ always i1[t+2]:sbf = i1[t+2]:sbf }:tau") == T );
	}
	PS_CASE("L2 reindexing gives no future input") {
		CHECK( nonempty("{ always o1[t]:sbf = i1[t+1]:sbf }:tau") == F );
	}
	PS_CASE("S1 an output before the input it must equal") {
		CHECK( nonempty("{ always o1[t-1]:sbf = i1[t]:sbf }:tau") == F );
	}
}

TEST_SUITE("program_set abstraction") {

	PS_CASE("R1 globally feasible letters are not enough") {
		CHECK( nonempty("{ (always o1[t]:sbf = o1[t-1]:sbf)"
			" && (always (i1[t]:sbf = 1 -> o1[t]:sbf = 1))"
			" && (always (i1[t]:sbf = 0 -> o1[t]:sbf = 0)) }:tau") == F );
	}
	PS_CASE("R1' without the last conjunct, constant one") {
		CHECK( nonempty("{ (always o1[t]:sbf = o1[t-1]:sbf)"
			" && (always (i1[t]:sbf = 1 -> o1[t]:sbf = 1)) }:tau") == T );
	}
}

TEST_SUITE("program_set CTL* leaves") {

	PS_CASE("C1 a positive E leaf is conjoined: a path with a 1 exists") {
		CHECK( nonempty("{ E (sometimes i1[t]:sbf = 1) }:tau"
			" & { always o1[t]:sbf = i1[t]:sbf }:tau") == T );
	}

	PS_CASE("C2 a positive E leaf keeps its own verdict: the first input may be 0") {
		CHECK( nonempty("{ E (always i1[t]:sbf = 1) }:tau"
			" & { always o1[t]:sbf = 0 }:tau") == F );
	}

	PS_CASE("C3 an E leaf in a union sits under the selector guard") {
		CHECK( nonempty("{ E (always i1[t]:sbf = 1) }:tau"
			" | { always o1[t]:sbf = 0 }:tau") == T );
	}

	PS_CASE("C4 a complemented E leaf is refused, not decided") {
		CHECK( nonempty("{ E (sometimes i1[t]:sbf = 1) }:tau'") == std::nullopt );
	}

	PS_CASE("C5 a complement over an intersection with an E leaf is refused") {
		CHECK( nonempty("({ E (sometimes i1[t]:sbf = 1) }:tau"
			" & { always o1[t]:sbf = 0 }:tau)'") == std::nullopt );
	}
}
