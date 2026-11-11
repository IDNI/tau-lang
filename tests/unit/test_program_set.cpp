// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// The program-set compiler (src/program_set.h): the shape of the compiled
// specification of each kind of expression, the normal form, the refused
// literals, the term reader, and the warm-up the hidden streams give no
// clause.

#include "test_init.h"
#include "test_tau_helpers.h"

using namespace idni::tau_lang;

namespace {

using expr = program_set_expr<node_t>;

expr set_of(const char* spec) { return expr::literal(spec_or_fail(spec)); }

program_set_compiled<node_t> compiled_or_fail(const expr& e) {
	auto r = compile_program_set<node_t>(e);
	REQUIRE( r.has_value() );
	return r.value();
}

std::string text(tref fm) { return tau::get(fm).to_str(); }

bool contains(const std::string& s, const std::string& part) {
	return s.find(part) != std::string::npos;
}

size_t count(const std::string& s, const std::string& part) {
	size_t n = 0;
	for (size_t p = s.find(part); p != std::string::npos;
		p = s.find(part, p + part.size())) ++n;
	return n;
}

// The skeleton of @p fm, as ltl_skeleton_with_testers builds it.
std::string skeleton_of(tref fm) {
	auto atoms = extract_data_atoms<node_t>(fm);
	auto r = ltl_skeleton_with_testers<node_t>(fm, atoms);
	REQUIRE( r.has_value() );
	return r.value().first;
}

} // namespace

TEST_SUITE("program_set compile") {

	TEST_CASE("an input-free expression is the Boolean combination of its literals") {
		auto c = compiled_or_fail(expr::intersection(
			set_of("always o1[t]:sbf = 1."),
			expr::complement(set_of("always o1[t]:sbf = 0."))));
		CHECK( c.input_free );
		CHECK( c.hidden.empty() );
		CHECK( c.negatives.empty() );
		CHECK( c.inputs.empty() );
		CHECK_FALSE( has_program_set_stream<node_t>(c.formula) );
	}

	TEST_CASE("a complement over an input gets one prediction stream per input") {
		auto c = compiled_or_fail(expr::complement(
			set_of("always o1[t]:sbf = i1[t]:sbf.")));
		CHECK_FALSE( c.input_free );
		REQUIRE( c.inputs.size() == 1 );
		CHECK( c.inputs[0].first == "i1" );
		REQUIRE( c.hidden.size() == 1 );
		CHECK( c.hidden[0].first == "w_ps0_d_i1" );
		CHECK( c.hidden[0].second == c.inputs[0].second );
		REQUIRE( c.negatives.size() == 1 );
		CHECK( c.negatives[0].lookback == 0 );
		CHECK( c.negatives[0].predictions
			== std::vector<std::string>{ "w_ps0_d_i1" } );
		CHECK( c.selectors.empty() );
		CHECK( has_program_set_stream<node_t>(c.formula) );
		// the prediction is followed from step 0 on
		CHECK( contains(text(c.formula), "w_ps0_d_i1[t-1]") );
		CHECK_FALSE( contains(text(c.formula), "w_ps0_d_i1[t-2]") );
	}

	TEST_CASE("a complemented literal of lookback l pins the l inputs before the step") {
		auto c = compiled_or_fail(expr::complement(
			set_of("always o1[t]:sbf = i1[t-2]:sbf.")));
		REQUIRE( c.negatives.size() == 1 );
		CHECK( c.negatives[0].lookback == 2 );
		const std::string s = text(c.formula);
		CHECK( contains(s, "w_ps0_d_i1[t-1]") );
		CHECK( contains(s, "w_ps0_d_i1[t-2]") );
		CHECK( contains(s, "w_ps0_d_i1[t-3]") );
		CHECK_FALSE( contains(s, "w_ps0_d_i1[t-4]") );
	}

	TEST_CASE("every input of the expression gets a prediction in each complement") {
		auto c = compiled_or_fail(expr::intersection(
			set_of("always o1[t]:sbf = i2[t]:sbf."),
			expr::complement(set_of("always o1[t]:sbf = i1[t]:sbf."))));
		REQUIRE( c.inputs.size() == 2 );
		REQUIRE( c.negatives.size() == 1 );
		CHECK( c.negatives[0].predictions.size() == 2 );
		CHECK( c.hidden.size() == 2 );
	}

	TEST_CASE("a union gets one constant selector read before the step") {
		auto c = compiled_or_fail(expr::set_union(
			set_of("always o1[t]:sbf = 1."),
			set_of("always o1[t]:sbf = i1[t]:sbf.")));
		REQUIRE( c.selectors.size() == 1 );
		CHECK( c.selectors[0] == "w_ps0_b" );
		REQUIRE( c.hidden.size() == 1 );
		CHECK( c.hidden[0].first == "w_ps0_b" );
		CHECK( c.negatives.empty() );
		const std::string s = text(c.formula);
		CHECK( contains(s, "w_ps0_b[t-1]") );
		CHECK( contains(s, "w_ps0_b[t]") );
	}

	TEST_CASE("the complement of an intersection is a union of complements") {
		auto c = compiled_or_fail(expr::complement(expr::intersection(
			set_of("always o1[t]:sbf = i1[t]:sbf."),
			set_of("always o1[t]:sbf = 1."))));
		CHECK( c.negatives.size() == 2 );
		CHECK( c.selectors.size() == 1 );
		CHECK( c.hidden.size() == 3 );
	}

	TEST_CASE("a double complement is the literal") {
		auto c = compiled_or_fail(expr::complement(expr::complement(
			set_of("always o1[t]:sbf = i1[t]:sbf."))));
		CHECK( c.negatives.empty() );
		CHECK( c.hidden.empty() );
	}

	TEST_CASE("a bare conjunct beside a future operator is scoped by always") {
		auto c = compiled_or_fail(expr::complement(
			set_of("o1[t]:sbf = i1[t]:sbf && sometimes o1[t]:sbf = 1.")));
		CHECK( contains(text(c.formula), "always") );
	}
}

TEST_SUITE("program_set refusals") {

	TEST_CASE("a complemented literal with a CTL* operator is refused") {
		auto r = compile_program_set<node_t>(expr::complement(
			set_of("E (always o1[t]:sbf = i1[t]:sbf).")));
		CHECK_FALSE( r.has_value() );
	}

	TEST_CASE("a CTL* literal under a complemented intersection is refused") {
		auto r = compile_program_set<node_t>(expr::complement(
			expr::intersection(
				set_of("E (always o1[t]:sbf = i1[t]:sbf)."),
				set_of("always o1[t]:sbf = 0."))));
		CHECK_FALSE( r.has_value() );
	}

	TEST_CASE("a positive literal with a CTL* operator is conjoined") {
		auto c = compiled_or_fail(expr::intersection(
			set_of("E (sometimes i1[t]:sbf = 1)."),
			expr::complement(set_of("always o1[t]:sbf = 0."))));
		CHECK( contains(text(c.formula), "E ") );
		CHECK( c.hidden.size() == 1 );
	}

	TEST_CASE("a CTL* literal in a union stays under the selector guard") {
		auto c = compiled_or_fail(expr::set_union(
			set_of("E (sometimes i1[t]:sbf = 1)."),
			set_of("always o1[t]:sbf = 0.")));
		CHECK( contains(text(c.formula), "E ") );
		CHECK( c.selectors.size() == 1 );
	}

	TEST_CASE("a literal holding a tau constant is refused") {
		auto r = compile_program_set<node_t>(
			set_of("always o2[t]:tau = { always o1[t]:sbf = 1 }:tau."));
		CHECK_FALSE( r.has_value() );
	}

	TEST_CASE("a literal reading a hidden stream is refused") {
		auto c = compiled_or_fail(expr::complement(
			set_of("always o1[t]:sbf = i1[t]:sbf.")));
		auto r = compile_program_set<node_t>(expr::literal(c.formula));
		CHECK_FALSE( r.has_value() );
	}
}

TEST_SUITE("program_set of a term") {

	// The term is parsed with the hooks off: on, two tau constants fold
	// into one.
	tref term_or_fail(const char* s) {
		use_hooks_guard<node_t> hooks_off(false);
		tref t = tau::get(s, parse_bf()).value_or(nullptr);
		REQUIRE( t );
		return t;
	}

	TEST_CASE("& is the intersection, ' the complement") {
		auto r = program_set_of_term<node_t>(term_or_fail(
			"{ always o1[t]:sbf = i1[t]:sbf }:tau & { always o1[t]:sbf = 0 }:tau'"));
		REQUIRE( r.has_value() );
		const auto& e = r.value();
		REQUIRE( e.op == expr::kind::intersection );
		REQUIRE( e.args.size() == 2 );
		CHECK( e.args[0].op == expr::kind::literal );
		CHECK( e.args[1].op == expr::kind::complement );
		auto c = compiled_or_fail(e);
		CHECK( c.negatives.size() == 1 );
	}

	TEST_CASE("| is the union and ^ the symmetric difference") {
		auto u = program_set_of_term<node_t>(term_or_fail(
			"{ always o1[t]:sbf = 1 }:tau | { always o1[t]:sbf = 0 }:tau"));
		REQUIRE( u.has_value() );
		CHECK( u.value().op == expr::kind::set_union );
		auto x = program_set_of_term<node_t>(term_or_fail(
			"{ always o1[t]:sbf = 1 }:tau ^ { always o1[t]:sbf = 0 }:tau"));
		REQUIRE( x.has_value() );
		CHECK( x.value().op == expr::kind::set_union );
		REQUIRE( x.value().args.size() == 2 );
		CHECK( x.value().args[0].op == expr::kind::intersection );
	}

	TEST_CASE("a term of another type is refused") {
		auto r = program_set_of_term<node_t>(term_or_fail("{ x }:sbf & { y }:sbf"));
		CHECK_FALSE( r.has_value() );
	}
}

TEST_SUITE("program_set warm-up") {

	// A lag on a prediction stream is a value the system chose before the
	// step: the clause that follows the prediction starts at step 0.
	TEST_CASE("a lag on a hidden stream gives no warm-up") {
		auto c = compiled_or_fail(expr::complement(
			set_of("always o1[t]:sbf = i1[t]:sbf.")));
		CHECK( body_max_lookback<node_t>(c.formula) == 0 );
		const std::string skel = skeleton_of(c.formula);
		CHECK_FALSE( contains(skel, "X(") );
		CHECK_FALSE( contains(skel, "__step_ge") );
	}

	TEST_CASE("the warm-up of a complemented literal is the literal's own") {
		auto c = compiled_or_fail(expr::complement(
			set_of("always o1[t]:sbf = i1[t-1]:sbf.")));
		CHECK( body_max_lookback<node_t>(c.formula) == 1 );
		const std::string skel = skeleton_of(c.formula);
		CHECK( count(skel, "X(") == 1 );
		CHECK_FALSE( contains(skel, "__step_ge2") );
	}

	TEST_CASE("the selector of a union gives no warm-up") {
		auto c = compiled_or_fail(expr::set_union(
			set_of("always o1[t]:sbf = 1."),
			set_of("always o1[t]:sbf = i1[t]:sbf.")));
		CHECK( body_max_lookback<node_t>(c.formula) == 0 );
		CHECK_FALSE( contains(skeleton_of(c.formula), "X(") );
	}
}
