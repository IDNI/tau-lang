// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_Bool_helpers.h"

tref infer(const char* sample) {
	return infer_ba_types<node_t>(tau::get(sample));
}

bool expect_infer_fail(const char* sample) {
	return infer(sample) == nullptr;
}

bool are_all_typed_as(tref n, const std::string& type) {
	size_t tid = get_ba_type_id<node_t>(type);
	auto are_all_trefs_typed = [&](const trefs& ts) {
		for (tref t : ts)
			if (tau::get(t).get_ba_type() != tid) return false;
		return true;
	};
	const auto& t = tau::get(n);
	return are_all_trefs_typed(t.select_all(is<node_t, tau::bf_constant>))
	    && are_all_trefs_typed(t.select_all(is<node_t, tau::variable>));
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
