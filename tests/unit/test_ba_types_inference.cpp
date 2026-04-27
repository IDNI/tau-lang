// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_Bool_helpers.h"

#include "ba_types_inference.h"

using namespace idni::tau_lang;

TEST_SUITE("Configuration") {

	TEST_CASE("bdd init") {
		bdd_init<Bool>();
	}
}

// Helper: parse a single expression without inference, using wff start
static tref parse_no_infer(const std::string& s) {
	tau::get_options opts{ .parse = { .start = tau::wff },
		.infer_ba_types = false, .reget_with_hooks = false };
	return tau::get(s, opts);
}

// Helper: parse as bf without inference
static tref parse_bf_no_infer(const std::string& s) {
	tau::get_options opts{ .parse = { .start = tau::bf },
		.infer_ba_types = false, .reget_with_hooks = false };
	return tau::get(s, opts);
}

// Helper: find first node of a given type in the tree (pre-order)
template<size_t NT>
static tref find_first(tref root) {
	tref found = nullptr;
	auto f = [&](tref n) {
		if (tau::get(n).get_type() == NT) { found = n; return false; }
		return true;
	};
	pre_order<node_t>(root).search_unique(f);
	return found;
}

TEST_SUITE("is_untyped_tref") {

	TEST_CASE("plain node with ba_type 0 and no typed child is untyped") {
		// x = 1 -- variable x has ba_type 0, no typed child
		tref parsed = parse_no_infer("x = 1");
		CHECK( parsed != nullptr );
		tref var = find_first<tau::variable>(parsed);
		CHECK( var != nullptr );
		CHECK( is_untyped_tref<node_t>(var) );
	}

	TEST_CASE("node with typed structural child is NOT untyped") {
		// x:sbf -- variable has a typed structural child before inference
		tref parsed = parse_no_infer("x:sbf = 1");
		CHECK( parsed != nullptr );
		tref var = find_first<tau::variable>(parsed);
		CHECK( var != nullptr );
		// The typed child should be present as a structural child
		bool has_typed_child = false;
		for (auto c : tau::get(var).get_children())
			if (tau::get(c).is(tau::typed)) { has_typed_child = true; break; }
		CHECK( has_typed_child );
		// Therefore is_untyped_tref should return false
		CHECK_FALSE( is_untyped_tref<node_t>(var) );
	}

	TEST_CASE("node with stamped ba_type and no typed child is NOT untyped") {
		// After inference, variable has ba_type stamped, no typed child
		tref parsed = parse_no_infer("x:sbf = 1");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		tref var = find_first<tau::variable>(inferred);
		CHECK( var != nullptr );
		// ba_type should be sbf
		CHECK( tau::get(var).get_ba_type() == sbf_type_id<node_t>() );
		// No typed structural child should remain
		bool has_typed_child = false;
		for (auto c : tau::get(var).get_children())
			if (tau::get(c).is(tau::typed)) { has_typed_child = true; break; }
		CHECK_FALSE( has_typed_child );
		CHECK_FALSE( is_untyped_tref<node_t>(var) );
	}
}

TEST_SUITE("get_effective_ba_type") {

	TEST_CASE("node with ba_type 0 and no typed child returns 0") {
		tref parsed = parse_no_infer("x = 1");
		CHECK( parsed != nullptr );
		tref var = find_first<tau::variable>(parsed);
		CHECK( var != nullptr );
		CHECK( get_effective_ba_type<node_t>(var) == 0 );
	}

	TEST_CASE("node with typed structural child returns that type id") {
		// x:sbf -- typed child present before inference
		tref parsed = parse_no_infer("x:sbf = 1");
		CHECK( parsed != nullptr );
		tref var = find_first<tau::variable>(parsed);
		CHECK( var != nullptr );
		CHECK( get_effective_ba_type<node_t>(var) == sbf_type_id<node_t>() );
	}

	TEST_CASE("node with stamped ba_type returns that type id") {
		// After inference, ba_type is stamped
		tref parsed = parse_no_infer("x:tau = 1");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		tref var = find_first<tau::variable>(inferred);
		CHECK( var != nullptr );
		CHECK( get_effective_ba_type<node_t>(var) == tau_type_id<node_t>() );
	}

	TEST_CASE("bf_t with typed structural child returns that type id") {
		// 1:sbf -- bf_t has a typed structural child before inference
		tref parsed = parse_bf_no_infer("1:sbf");
		CHECK( parsed != nullptr );
		tref bft = find_first<tau::bf_t>(parsed);
		CHECK( bft != nullptr );
		CHECK( get_effective_ba_type<node_t>(bft) == sbf_type_id<node_t>() );
	}
}

TEST_SUITE("typed child stripping after inference") {

	TEST_CASE("variable typed child is stripped after inference") {
		tref parsed = parse_no_infer("x:sbf = y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		// No variable should have a typed structural child after inference
		auto vars = tau::get(inferred).select_all_until(
			is<node_t, tau::variable>, is<node_t, tau::offset>);
		for (tref v : vars) {
			for (auto c : tau::get(v).get_children())
				CHECK_FALSE( tau::get(c).is(tau::typed) );
		}
	}

	TEST_CASE("bf_t typed child is stripped after inference") {
		// 1:sbf = x -- the 1:sbf should have its typed child stripped
		tref parsed = parse_no_infer("1:sbf = x");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		tref bft = find_first<tau::bf_t>(inferred);
		CHECK( bft != nullptr );
		CHECK( tau::get(bft).get_ba_type() == sbf_type_id<node_t>() );
		bool has_typed = false;
		for (auto c : tau::get(bft).get_children())
			if (tau::get(c).is(tau::typed)) { has_typed = true; break; }
		CHECK_FALSE( has_typed );
	}
}
