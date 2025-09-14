// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_tau_helpers.h"

TEST_SUITE("configuration") {
	TEST_CASE("initialize bdd") {
		bdd_init<Bool>();
	}

	TEST_CASE("logging") {
		logging::debug();
	}
}

tref infer(const char* sample) {
	return tau::get(sample, tau::get_options{ .reget_with_hooks = false });
}

bool expect_infer_fail(const char* sample) {
	return infer(sample) == nullptr;
}

bool are_all_typed_as(tref n, const std::string& type) {
	using node = node_t;
	size_t tid = get_ba_type_id<node>(type);
	auto are_all_trefs_typed = [&](const trefs& ts) {
		for (tref t : ts)
			if (tau::get(t).get_ba_type() != tid) {
				LOG_ERROR << "Type mismatch: " << LOG_FM(t)
					<< " expected: " << LOG_BA_TYPE(tid)
					<< " got: " << LOG_BA_TYPE(tau::get(t)
								.get_ba_type());
				return false;
			}
		return true;
	};
	const auto& t = tau::get(n);
	return are_all_trefs_typed(t.select_all(is<node, tau::bf_constant>))
	    && are_all_trefs_typed(t.select_all(is<node, tau::variable>))
		&& are_all_trefs_typed(t.select_all(is<node, tau::bv_constant>));
}


TEST_SUITE("constant types") {
	TEST_CASE("all typed") {
		using node = node_t;
		auto n = infer("{ 0 } : sbf = { 1 } : sbf & { 0 } : sbf.");
		LOG_DEBUG << "inferred: " << LOG_FM(n);
		CHECK( are_all_typed_as(n, "sbf") );
	}

	TEST_CASE("all typed (y2)") {
		using node = node_t;
		auto n = infer("x : bv[16] =_ y : bv[16].");
		LOG_DEBUG << "inferred: " << LOG_FM(n);
		CHECK( are_all_typed_as(n, "bv") );
	}

	TEST_CASE("all typed (y3)") {
		using node = node_t;
		auto n = infer("#b1 : bv[16] =_ #b0 : bv[16] & #b1 : bv.");
		LOG_DEBUG << "inferred: " << LOG_FM(n);
		CHECK( are_all_typed_as(n, "bv") );
	}

	TEST_CASE("some typed") {
		using node = node_t;
		auto n = infer("{ 0 } : sbf = { 1 } { 0 } : sbf.");
		LOG_DEBUG << "inferred: " << LOG_FM(n);
		CHECK( are_all_typed_as(n, "sbf") );
	}

	TEST_CASE("some typed (y2)") {
		using node = node_t;
		auto n = infer("#b1 : bv =_ #b0 & #b1.");
		LOG_DEBUG << "inferred: " << LOG_FM(n);
		CHECK( are_all_typed_as(n, "bv") );
	}

	TEST_CASE("only 1 typed") {
		auto n = infer("{ 0 } = { 1 } & { 0 } : sbf.");
		CHECK( are_all_typed_as(n, "sbf") );
	}

	TEST_CASE("none typed") {
		auto n = infer("{ F } = { T } { F }.");
		CHECK( are_all_typed_as(n, "tau") );
	}

	TEST_CASE("type over many variables beyond atomic formulas") {
		auto n = infer("x = y && y = z && z = w && w = { x1 }:sbf.");
		CHECK( are_all_typed_as(n, "sbf") );
	}

	TEST_CASE("type over many variables beyond atomic formulas (y2)") {
		auto n = infer("x =_ y && y =_ z && z =_ w : bv.");
		CHECK( are_all_typed_as(n, "bv") );
	}

	TEST_CASE("type over many variables beyond atomic formulas (y3)") {
		auto n = infer("x =_ y && y =_ z && z =_ w : bv[16].");
		CHECK( are_all_typed_as(n, "bv") );
	}

	TEST_CASE("type mismatch head vs body") {
		auto n = expect_infer_fail("{ T }:tau = { 0 }:sbf { 1 }:sbf.");
		CHECK( n );
	}

	TEST_CASE("type mismatch in body") {
		auto n = expect_infer_fail("{ 0 }:sbf = { T }:tau { 1 }:sbf.");
		CHECK( n );
	}

	TEST_CASE("type mismatch of default type in head vs body") {
		auto n = expect_infer_fail("{ T }:tau = { 0 }:sbf { 1 }.");
		CHECK( n );
	}

	TEST_CASE("type mismatch of default type in body") {
		auto n = expect_infer_fail("{ 0 } = { T }:tau { 1 }:sbf { 1 }:sbf.");
		CHECK( n );
	}

}
