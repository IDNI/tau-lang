// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_Bool_helpers.h"

tref infer(const char* sample) {
	tref code = tau::get(sample);
	DBG(assert(code != nullptr);)
	return infer_ba_types<node_t>(code);
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
				TAU_LOG_ERROR << "Type mismatch: " << TAU_LOG_FM(t)
					<< " expected: " << TAU_LOG_BA_TYPE(tid)
					<< " got: " << TAU_LOG_BA_TYPE(tau::get(t)
								.get_ba_type());
				return false;
			}
		return true;
	};
	const auto& t = tau::get(n);
	return are_all_trefs_typed(t.select_all(is<node, tau::bf_constant>))
	    && are_all_trefs_typed(t.select_all(is<node, tau::variable>));
}

TEST_SUITE("constant types") {
	TEST_CASE("all typed") {
		auto n = infer("{ 0 } : bool = { 1 } : bool & { 0 } : bool.");
		CHECK( are_all_typed_as(n, "bool") );
	}

	TEST_CASE("some typed") {
		using node = node_t;
		auto n = infer("{ 0 } : bool = { 1 } { 0 } : bool.");
		TAU_LOG_TRACE << "inferred: " << TAU_LOG_FM_DUMP(n);
		CHECK( are_all_typed_as(n, "bool") );
	}

	TEST_CASE("only 1 typed") {
		auto n = infer("{ 0 } = { 1 } & { 0 } : bool.");
		CHECK( are_all_typed_as(n, "bool") );
	}

	TEST_CASE("none typed") {
		auto n = infer("{ 0 } = { 1 } { 0 }.");
		CHECK( are_all_typed_as(n, "bool") );
	}

}
