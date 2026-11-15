// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Verdicts of program-set expressions (src/program_set.h) through the API:
// the Boolean operations on sets of programs, the time boundaries under
// tau's warm-up convention, memory, lookahead and the abstraction. One
// input i1, one visible output o1, type sbf.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "api.h"
#include "program_set.h"
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

// The reading of the parameters (ltl-parameters) for a scope, the one in
// force restored at its end.
struct parameters_reading {
	std::string before;
	explicit parameters_reading(const char* reading)
		: before(api<node_t>::get_ltl_parameters())
	{
		REQUIRE( api<node_t>::set_ltl_parameters(reading).has_value() );
	}
	~parameters_reading() {
		(void) api<node_t>::set_ltl_parameters(before);
	}
};

// The term @p term spells, parsed with the hooks off.
tref term_of(const char* term) {
	use_hooks_guard<node_t> hooks_off(false);
	tref t = tau::get(term, parse_bf()).value_or(nullptr);
	REQUIRE( t );
	return t;
}

// The nonemptiness verdict of @p term with its first parameter fixed to
// 1 or to 0 of its type: the fixed reading through the API.
std::optional<bool> nonempty_with_first(const char* term, bool one) {
	tref t = term_of(term);
	auto compiled = api<node_t>::program_set_compile(t);
	REQUIRE( compiled.has_value() );
	REQUIRE( !compiled.value().parameters.empty() );
	const auto& p = compiled.value().parameters.front();
	subtree_map<node_t, tref> values;
	values[p.constant] = one ? tau::_1(p.type) : tau::_0(p.type);
	auto r = api<node_t>::program_set_realizable(t, values);
	if (!r.has_value()) return std::nullopt;
	return r.value();
}

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

TEST_SUITE("program_set lookback of one literal") {

	// The complement of a literal with no lookback is broken from step 0
	// whatever the other literals read: no program forces the first
	// input to be 1, so the complement is every program, and the
	// intersection is the programs that keep their output.
	PS_CASE("K1 a lookback of 1 beside a complement broken at step 0") {
		CHECK( nonempty("{ always o1[t]:sbf = o1[t-1]:sbf }:tau"
			" & { always (i1[t]:sbf = 1 || [t > 0]) }:tau'") == T );
	}
	PS_CASE("K2 a lookback of 1 beside a complement broken at step 0 or 1") {
		CHECK( nonempty("{ always o1[t]:sbf = o1[t-1]:sbf }:tau"
			" & { always (i1[t]:sbf = 1 || [t > 1]) }:tau'") == T );
	}
	PS_CASE("K3 the complement alone is every program") {
		CHECK( nonempty("{ always (i1[t]:sbf = 1 || [t > 0]) }:tau'") == T );
	}
	PS_CASE("K4 no lookback beside the complement") {
		CHECK( nonempty("{ always o1[t]:sbf = 0 }:tau"
			" & { always (i1[t]:sbf = 1 || [t > 0]) }:tau'") == T );
	}
	PS_CASE("K5 a lookback of 2 beside a complement broken at step 0 or 1") {
		CHECK( nonempty("{ always o1[t]:sbf = o1[t-2]:sbf }:tau"
			" & { always (i1[t]:sbf = 1 || [t > 1]) }:tau'") == T );
	}
	PS_CASE("K6 a lookback of 2 beside a complement broken up to step 2") {
		CHECK( nonempty("{ always o1[t]:sbf = o1[t-2]:sbf }:tau"
			" & { always (i1[t]:sbf = 1 || [t > 2]) }:tau'") == T );
	}
	// A program with a constant value that is neither 0 nor 1 satisfies
	// the first literal, and the zero input breaks the last one at step 0.
	PS_CASE("K7 two positive literals, one with a lookback, beside a complement") {
		CHECK( nonempty("({ always (o1[t]:sbf & i1[t]:sbf' = 0"
			" -> i1[t]:sbf != 0) }:tau"
			" & { always (o1[t]:sbf = o1[t-1]:sbf' || [t > 0]) }:tau)"
			" & { always (o1[t]:sbf | i1[t]:sbf = 1 || [t > 0]) }:tau'")
			== T );
	}
	// A positive literal keeps its own start too: no program forces the
	// first input to be 1, so the second set is empty and so is the
	// intersection.
	PS_CASE("K8 a lookback of 1 beside a positive literal asked at step 0") {
		CHECK( nonempty("{ always o1[t]:sbf = o1[t-1]:sbf }:tau"
			" & { always (i1[t]:sbf = 1 || [t > 0]) }:tau") == F );
	}
	PS_CASE("K8' the positive literal alone is empty") {
		CHECK( nonempty("{ always (i1[t]:sbf = 1 || [t > 0]) }:tau") == F );
	}
	// One specification has one always part, started at its deepest
	// lookback: the same statements in one literal are not asked at step 0.
	PS_CASE("K9 the one-literal spelling keeps one warm-up") {
		CHECK( nonempty("{ (always o1[t]:sbf = o1[t-1]:sbf)"
			" && (always (i1[t]:sbf = 1 || [t > 0])) }:tau") == T );
	}
	PS_CASE("K9' one always with both statements keeps one warm-up") {
		CHECK( nonempty("{ always (o1[t]:sbf = o1[t-1]:sbf"
			" && (i1[t]:sbf = 1 || [t > 0])) }:tau") == T );
	}
	// The input-free case merges the always parts the same way.
	PS_CASE("K10 input-free literals asked at step 0 beside a lookback") {
		CHECK( nonempty("{ always o1[t]:sbf = o1[t-1]:sbf }:tau"
			" & { always (o1[t]:sbf = 1 || [t > 0]) }:tau"
			" & { always (o1[t]:sbf = 0 || [t > 0]) }:tau") == F );
	}
	PS_CASE("K10' the same statements in one literal") {
		CHECK( nonempty("{ (always o1[t]:sbf = o1[t-1]:sbf)"
			" && (always (o1[t]:sbf = 1 || [t > 0]))"
			" && (always (o1[t]:sbf = 0 || [t > 0])) }:tau") == T );
	}
	// The selector of a union is constant from before step 0 beside a
	// literal with a lookback as well: a program whose constant value
	// follows the first input is on neither side.
	PS_CASE("K11 a union beside a lookback") {
		CHECK( nonempty("({ always o1[t]:sbf = 1 }:tau"
			" | { always o1[t]:sbf = 0 }:tau)"
			" & { always o1[t]:sbf = o1[t-1]:sbf }:tau"
			" & { always (o1[t]:sbf = i1[t]:sbf || [t > 0]) }:tau") == F );
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

TEST_SUITE("program_set bare literals") {

	// The compiled expression of @p term: the formula and the hidden
	// streams. The verdict cases above decide both spellings; these pin
	// that the bare one compiles to the formula of the `always` one.
	program_set_compiled<node_t> compiled(const char* term) {
		tref t = nullptr;
		{
			use_hooks_guard<node_t> hooks_off(false);
			t = tau::get(term, parse_bf()).value_or(nullptr);
		}
		REQUIRE( t );
		auto e = program_set_of_term<node_t>(t);
		REQUIRE( e.has_value() );
		auto c = compile_program_set<node_t>(e.value());
		REQUIRE( c.has_value() );
		return c.value();
	}

	void check_same_compilation(const char* bare, const char* scoped) {
		auto b = compiled(bare), s = compiled(scoped);
		CAPTURE( tau::get(b.formula).to_str() );
		CAPTURE( tau::get(s.formula).to_str() );
		CHECK( matches_tree_mod_and_or(b.formula, s.formula) );
		CHECK( b.hidden == s.hidden );
		CHECK( b.inputs == s.inputs );
		CHECK( b.input_free == s.input_free );
	}

	TEST_CASE("a bare literal compiles as its always") {
		check_same_compilation("{ o1[t]:sbf = i1[t]:sbf }:tau",
			"{ always o1[t]:sbf = i1[t]:sbf }:tau");
	}
	TEST_CASE("a bare complement compiles as the complement of its always") {
		check_same_compilation("{ o1[t]:sbf = i1[t]:sbf }:tau'",
			"{ always o1[t]:sbf = i1[t]:sbf }:tau'");
	}
	TEST_CASE("a bare intersection with a complement") {
		check_same_compilation(
			"{ o1[t]:sbf = i1[t]:sbf }:tau & { o1[t]:sbf = 0 }:tau'",
			"{ always o1[t]:sbf = i1[t]:sbf }:tau"
			" & { always o1[t]:sbf = 0 }:tau'");
	}
	TEST_CASE("a bare union") {
		check_same_compilation(
			"{ o1[t]:sbf = i1[t]:sbf }:tau | { o1[t]:sbf = 0 }:tau",
			"{ always o1[t]:sbf = i1[t]:sbf }:tau"
			" | { always o1[t]:sbf = 0 }:tau");
	}
	TEST_CASE("one bare conjunct beside an always one") {
		check_same_compilation(
			"{ (o1[t]:sbf = i1[t]:sbf) && always (o2[t]:sbf = 0) }:tau'",
			"{ always (o1[t]:sbf = i1[t]:sbf) && always (o2[t]:sbf = 0) }:tau'");
	}
	TEST_CASE("a past operator with no future one is an always too") {
		check_same_compilation(
			"{ (o1[t]:sbf = 1) S (i1[t]:sbf = 1) }:tau'",
			"{ always ((o1[t]:sbf = 1) S (i1[t]:sbf = 1)) }:tau'");
	}
	TEST_CASE("an input-free bare complement is its sometimes") {
		auto c = compiled("{ o1[t]:sbf = 1 }:tau'");
		CHECK( c.input_free );
		CHECK( matches_wff_mod_and_or(c.formula,
			"!(always o1[t]:sbf = 1)") );
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

// An uninterpreted constant of a literal is a parameter of the expression:
// a value the program chooses once (existential, the default), a value the
// expression must admit every one of (universal), or a given value (fixed,
// through the API).
TEST_SUITE("program_set parameters") {

	PS_CASE("T14 some value keeps the input masked, not every value") {
		const char* e = "{ always (i1[t]:sbf & <:c> = 0) }:tau";
		CHECK( nonempty(e) == T );
		{
			parameters_reading universal("universal");
			CHECK( nonempty(e) == F );
		}
		CHECK( nonempty_with_first(e, false) == T );
		CHECK( nonempty_with_first(e, true) == F );
	}
	PS_CASE("P1 a parameter shared by two leaves is one value in both") {
		const char* e = "{ always o1[t]:sbf = <:c> }:tau"
			" & { always o1[t]:sbf != <:c> }:tau";
		CHECK( nonempty(e) == F );
		parameters_reading universal("universal");
		CHECK( nonempty(e) == F );
	}
	PS_CASE("P2 the program may read the parameter") {
		const char* e = "{ always o1[t]:sbf = <:c> }:tau";
		CHECK( nonempty(e) == T );
		parameters_reading universal("universal");
		CHECK( nonempty(e) == T );
	}
	PS_CASE("P3 not every value admits a constant program equal to 1") {
		const char* e = "{ always (o1[t]:sbf = <:c> && o1[t]:sbf = 1) }:tau";
		CHECK( nonempty(e) == T );
		parameters_reading universal("universal");
		CHECK( nonempty(e) == F );
	}
	// The complement predicts the visible input only: for every c, the
	// program that writes 0 and keeps o2 at c is broken on the path where
	// i1 is c.
	PS_CASE("P4 the witnesses of a complement stay existential") {
		const char* e = "{ always (i1[t]:sbf = <:c> -> o1[t]:sbf = 1) }:tau'"
			" & { always o2[t]:sbf = <:c> }:tau";
		CHECK( nonempty(e) == T );
		parameters_reading universal("universal");
		CHECK( nonempty(e) == T );
	}
	PS_CASE("P5 a program that always writes 1 breaks the implication on no path") {
		const char* e = "{ always (i1[t]:sbf = <:c> -> o1[t]:sbf = 1) }:tau'"
			" & { always o1[t]:sbf = 1 }:tau";
		CHECK( nonempty(e) == F );
		parameters_reading universal("universal");
		CHECK( nonempty(e) == F );
	}
	PS_CASE("P6 a complement with a parameter the universal reading refutes") {
		const char* e = "{ always (i1[t]:sbf & <:c> = 0) }:tau"
			" & { always o1[t]:sbf = 0 }:tau'";
		CHECK( nonempty(e) == T );
		parameters_reading universal("universal");
		CHECK( nonempty(e) == F );
	}
}

// The compiled specification under each reading of the parameters.
TEST_SUITE("program_set parameter compilation") {

	TEST_CASE("the existential reading leaves the constant in the literal") {
		auto c = api<node_t>::program_set_compile(
			term_of("{ always (i1[t]:sbf & <:c> = 0) }:tau"));
		REQUIRE( c.has_value() );
		REQUIRE( c.value().parameters.size() == 1 );
		CHECK( c.value().parameters[0].name == ":c" );
		CHECK( c.value().parameter_inputs.empty() );
		CHECK( c.value().hidden.empty() );
		REQUIRE( c.value().inputs.size() == 1 );
		CHECK( c.value().inputs[0].first == "i1" );
		const std::string s = tau::get(c.value().formula).to_str();
		CHECK( s.find("<:c>") != std::string::npos );
		CHECK( s.find("w_ps") == std::string::npos );
	}

	TEST_CASE("the universal reading reads a hidden input kept from before step 0") {
		parameters_reading universal("universal");
		auto c = api<node_t>::program_set_compile(
			term_of("{ always (i1[t]:sbf & <:c> = 0) }:tau"));
		REQUIRE( c.has_value() );
		REQUIRE( c.value().parameters.size() == 1 );
		CHECK( c.value().parameters[0].name == ":c" );
		CHECK( c.value().parameters[0].type == c.value().inputs[0].second );
		CHECK( c.value().parameter_inputs
			== std::vector<std::string>{ "w_ps0_p_c" } );
		// a parameter is no input of the literals and no hidden output
		CHECK( c.value().hidden.empty() );
		REQUIRE( c.value().inputs.size() == 1 );
		CHECK( c.value().inputs[0].first == "i1" );
		const std::string s = tau::get(c.value().formula).to_str();
		CHECK( s.find("<:c>") == std::string::npos );
		CHECK( s.find("w_ps0_p_c[t-1]") != std::string::npos );
		CHECK( s.find("w_ps0_p_c[t-2]") == std::string::npos );
		CHECK( has_program_set_stream<node_t>(c.value().formula) );
	}

	TEST_CASE("one name is one stream in every literal") {
		parameters_reading universal("universal");
		auto c = api<node_t>::program_set_compile(term_of(
			"{ always o1[t]:sbf = <:c> }:tau"
			" & { always o1[t]:sbf != <:c> }:tau"));
		REQUIRE( c.has_value() );
		CHECK( c.value().parameters.size() == 1 );
		CHECK( c.value().parameter_inputs.size() == 1 );
		CHECK( c.value().input_free );
		const std::string s = tau::get(c.value().formula).to_str();
		size_t reads = 0;
		for (size_t p = s.find("w_ps0_p_c[t]"); p != std::string::npos;
			p = s.find("w_ps0_p_c[t]", p + 1)) ++reads;
		// the kept-value clause and the two literals
		CHECK( reads == 3 );
	}

	TEST_CASE("the complement over the visible input keeps its prediction, the parameter gets none") {
		parameters_reading universal("universal");
		auto c = api<node_t>::program_set_compile(term_of(
			"{ always (i1[t]:sbf = <:c> -> o1[t]:sbf = 1) }:tau'"));
		REQUIRE( c.has_value() );
		CHECK( c.value().parameter_inputs
			== std::vector<std::string>{ "w_ps0_p_c" } );
		REQUIRE( c.value().hidden.size() == 1 );
		CHECK( c.value().hidden[0].first == "w_ps1_d_i1" );
		REQUIRE( c.value().negatives.size() == 1 );
		CHECK( c.value().negatives[0].predictions
			== std::vector<std::string>{ "w_ps1_d_i1" } );
		CHECK( c.value().negatives[0].lookback == 0 );
	}

	TEST_CASE("a fixed value leaves no parameter") {
		tref t = term_of("{ always (i1[t]:sbf & <:c> = 0) }:tau");
		auto expr = program_set_of_term<node_t>(t);
		REQUIRE( expr.has_value() );
		auto params = program_set_parameters<node_t>(expr.value());
		REQUIRE( params.size() == 1 );
		subtree_map<node_t, tref> values;
		values[params[0].constant] = tau::_1(params[0].type);
		auto fixed = program_set_with_values<node_t>(expr.value(), values);
		CHECK( program_set_parameters<node_t>(fixed).empty() );
		// the key built for the name and type is the same key
		subtree_map<node_t, tref> built;
		built[tau::build_bf_uconst("", "c", params[0].type)]
			= tau::_1(params[0].type);
		CHECK( program_set_parameters<node_t>(
			program_set_with_values<node_t>(expr.value(), built)).empty() );
	}

	TEST_CASE("the option is read and refuses any other word") {
		CHECK( api<node_t>::get_ltl_parameters() == "existential" );
		CHECK_FALSE( api<node_t>::set_ltl_parameters("sometimes").has_value() );
		CHECK( api<node_t>::get_ltl_parameters() == "existential" );
		{
			parameters_reading universal("universal");
			CHECK( api<node_t>::get_ltl_parameters() == "universal" );
		}
		CHECK( api<node_t>::get_ltl_parameters() == "existential" );
	}
}
