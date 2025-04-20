// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "boolean_algebras/bool_ba.h"
#include "defs.h"
#include "depreciating/nso_rr_depreciating.h"
#include "test_helpers_depreciating.h"

#include "depreciating/debug_helpers_depreciating.h"


namespace testing = doctest;

tau_<Bool> infer(const char* sample) {
	auto src = make_tau_source(sample);
	auto stmt = make_statement(src);
	return infer_constant_types<Bool>(stmt);
}

bool expect_infer_fail(const char* sample) {
	auto x = infer(sample);
	return x.get() == 0;
}

bool are_all_typed_as(const tau_<Bool>& n, const std::string& type) {
	for (const auto& c : select_all(n,
				is_non_terminal<tau_parser::bf_constant, Bool>))
	{
		auto tn = c | tau_parser::type;
		if ((!tn && type.size()) || ((tn && type != make_string<
			tau_node_terminal_extractor_t<Bool>, tau_<Bool>>(
				tau_node_terminal_extractor<Bool>, tn.value()))))
					return false;
	}
	return true;
}

TEST_SUITE("constant types") {
	TEST_CASE("all typed") {
		auto n = infer("{ 0 } : bool = { 1 } : bool & { 0 } : bool.");
		// ptree<Bool>(std::cout << "inferred: ", n) << "\n";
		CHECK( are_all_typed_as(n, "bool") );
	}

	TEST_CASE("some typed") {
		auto n = infer("{ 0 } : bool = { 1 } { 0 } : bool.");
		// ptree<Bool>(std::cout << "inferred: ", n) << "\n";
		CHECK( are_all_typed_as(n, "bool") );
	}

	TEST_CASE("only 1 typed") {
		auto n = infer("{ 0 } = { 1 } & { 0 } : bool.");
		// ptree<Bool>(std::cout << "inferred: ", n) << "\n";
		CHECK( are_all_typed_as(n, "bool") );
	}

	TEST_CASE("only 1 typed sbf") {
		auto n = expect_infer_fail("{ x } = { y } { 0 } : sbf.");
		// ptree<Bool>(std::cout << "inferred: ", n) << "\n";
		CHECK( n );
	}

	TEST_CASE("only 1 typed tau") {
		auto n = expect_infer_fail("{ F. } = { T. } { F. } : tau.");
		// ptree<Bool>(std::cout << "inferred: ", n) << "\n";
		CHECK( n );
	}

	TEST_CASE("none typed") {
		auto n = infer("{ F. } = { T. } { F. }.");
		// ptree<Bool>(std::cout << "inferred: ", n) << "\n";
		CHECK( are_all_typed_as(n, "bool") );
	}

	TEST_CASE("type mismatch") {
		auto n = expect_infer_fail("{ F. }:tau = { 0 }:sbf { 1 }:sbf.");
		// ptree<Bool>(std::cout << "inferred: ", n) << "\n";
		CHECK( n );
	}

}
