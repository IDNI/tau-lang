// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "boolean_algebras/bv/bv_ba.h"
#include "bounded_child.h"

#include <thread>

namespace {

// jobs of this suite's own children
std::optional<uint8_t> sleep_job(const std::string&) {
	std::this_thread::sleep_for(std::chrono::seconds(30));
	return 0;
}
std::optional<uint8_t> exit_job(const std::string&) { std::_Exit(3); }
std::optional<uint8_t> echo_job(const std::string& request) {
	return static_cast<uint8_t>(request.size());
}
[[maybe_unused]] const bool jobs_registered = (register_bounded_child("test-sleep", &sleep_job),
	register_bounded_child("test-exit", &exit_job),
	register_bounded_child("test-echo", &echo_job), true);

cvc5::Sort bv_sort(uint32_t w) { return cvc5_term_manager.mkBitVectorSort(w); }

cvc5::Term bv_value(uint32_t w, uint64_t v) {
	return cvc5_term_manager.mkBitVector(w, v);
}

// the verdict of @p q in this process, as bv_check_sat asks it
bv_sat_status in_process(const cvc5::Term& q, bool alternating) {
	cvc5::Solver solver(cvc5_term_manager);
	if (alternating) config_cvc5_solver_alternating_quantifiers(solver);
	config_cvc5_solver(solver, true);
	solver.assertFormula(q);
	auto r = solver.checkSat();
	return r.isSat() ? bv_sat_status::sat
		: r.isUnknown() ? bv_sat_status::unknown : bv_sat_status::unsat;
}

// Questions over every shape the text carries: free constants, bound
// variables of both quantifiers, values, Booleans and indexed operators.
std::vector<std::pair<cvc5::Term, bool>> questions() {
	auto& tm = cvc5_term_manager;
	using K = cvc5::Kind;
	cvc5::Term c = tm.mkConst(bv_sort(8), "o1[t-1]");
	cvc5::Term x = tm.mkVar(bv_sort(8), "x");
	cvc5::Term y = tm.mkVar(bv_sort(8), "y");
	cvc5::Term z = tm.mkVar(bv_sort(16), "z");
	cvc5::Term nibble = tm.mkConst(bv_sort(4), "n");
	cvc5::Term product = tm.mkTerm(K::BITVECTOR_MULT, { x, y });
	cvc5::Term all_ex = tm.mkTerm(K::FORALL, {
		tm.mkTerm(K::VARIABLE_LIST, { x }),
		tm.mkTerm(K::EXISTS, { tm.mkTerm(K::VARIABLE_LIST, { y }),
			tm.mkTerm(K::EQUAL, { product, x }) }) });
	cvc5::Term low = tm.mkTerm(tm.mkOp(K::BITVECTOR_EXTRACT, { 7, 0 }), { z });
	cvc5::Term wide = tm.mkTerm(tm.mkOp(K::BITVECTOR_ZERO_EXTEND, { 8 }),
		{ c });
	cvc5::Term extended = tm.mkTerm(K::EXISTS, {
		tm.mkTerm(K::VARIABLE_LIST, { z }),
		tm.mkTerm(K::AND, {
			tm.mkTerm(K::EQUAL, { low, c }),
			tm.mkTerm(K::BITVECTOR_ULT, { wide, z }),
			tm.mkBoolean(true) }) });
	cvc5::Term unsat = tm.mkTerm(K::AND, {
		tm.mkTerm(K::BITVECTOR_ULT, { c, bv_value(8, 3) }),
		tm.mkTerm(K::BITVECTOR_UGT, { c, bv_value(8, 200) }),
		tm.mkTerm(K::DISTINCT, { nibble, bv_value(4, 9) }) });
	return { { all_ex, true }, { extended, false }, { unsat, false } };
}

} // namespace

TEST_SUITE("bounded child") {

	TEST_CASE("a cvc5 term built again from its text is the same term") {
		for (const auto& [q, alternating] : questions()) {
			auto text = cvc5_term_to_text(q);
			REQUIRE( text.has_value() );
			auto back = cvc5_term_from_text(*text);
			REQUIRE( back.has_value() );
			CHECK( back->toString() == q.toString() );
			CHECK( in_process(*back, alternating)
				== in_process(q, alternating) );
		}
	}

	TEST_CASE("broken text builds no term") {
		CHECK( !cvc5_term_from_text("").has_value() );
		CHECK( !cvc5_term_from_text("2\nx").has_value() );
		auto text = cvc5_term_to_text(questions()[0].first);
		REQUIRE( text.has_value() );
		CHECK( !cvc5_term_from_text(text->substr(0, text->size() / 2))
			.has_value() );
	}

	TEST_CASE("a term of another sort gives no text") {
		auto& tm = cvc5_term_manager;
		cvc5::Term i = tm.mkConst(tm.getIntegerSort(), "i");
		CHECK( !cvc5_term_to_text(tm.mkTerm(cvc5::Kind::EQUAL,
			{ i, tm.mkInteger(1) })).has_value() );
	}

	TEST_CASE("the child job decides a question as the process does") {
		for (const auto& [q, alternating] : questions()) {
			auto request = bv_check_sat_request(q, alternating);
			REQUIRE( request.has_value() );
			auto v = bv_check_sat_child(*request);
			REQUIRE( v.has_value() );
			CHECK( static_cast<bv_sat_status>(*v)
				== in_process(q, alternating) );
		}
		CHECK( !bv_check_sat_child("99 0\n1\n").has_value() );
	}

	TEST_CASE("a question is decided in a new process of this program") {
		REQUIRE( bounded_children_available() );
		for (const auto& [q, alternating] : questions()) {
			auto request = bv_check_sat_request(q, alternating);
			REQUIRE( request.has_value() );
			auto out = run_bounded_child("bv-check-sat", *request, 60'000);
			REQUIRE( out.status == bounded_outcome::done );
			CHECK( static_cast<bv_sat_status>(out.value)
				== in_process(q, alternating) );
		}
	}

	TEST_CASE("a child reads its request whole") {
		REQUIRE( bounded_children_available() );
		auto out = run_bounded_child("test-echo",
			std::string("a\r\n\x1A b\0c", 9), 60'000);
		REQUIRE( out.status == bounded_outcome::done );
		CHECK( out.value == 9 );
	}

	TEST_CASE("a child past its bound is timed out") {
		REQUIRE( bounded_children_available() );
		auto out = run_bounded_child("test-sleep", "", 1'000);
		CHECK( out.status == bounded_outcome::timed_out );
		CHECK( out.seconds < 20 );
		CHECK( out.rep.has_error() );
		CHECK( report_attr_value(out.rep, tau_lang::label::timeout) == 1 );
	}

	TEST_CASE("a child without an answer failed") {
		REQUIRE( bounded_children_available() );
		auto out = run_bounded_child("test-exit", "", 60'000);
		CHECK( out.status == bounded_outcome::failed );
		CHECK( out.rep.has_error() );
		CHECK( report_attr_value(out.rep, tau_lang::label::exit_code)
			== 3 );
		auto unknown = run_bounded_child("no-such-job", "", 60'000);
		CHECK( unknown.status == bounded_outcome::failed );
		CHECK( report_attr_value(unknown.rep,
			tau_lang::label::exit_code) == 2 );
	}

	TEST_CASE("the error of a dead child reaches the budget note") {
		REQUIRE( bounded_children_available() );
		take_time_budget_exhausted();
		take_time_budget_report();
		auto out = run_bounded_child("test-exit", "", 60'000);
		note_time_budget_exhausted("UNKNOWN", std::move(out.rep));
		CHECK( take_time_budget_exhausted() == "UNKNOWN" );
		auto rep = take_time_budget_report();
		CHECK( rep.has_error() );
		CHECK( report_attr_value(rep, tau_lang::label::exit_code) == 3 );
	}
}
