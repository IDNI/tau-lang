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

// ── untype / canonize on a typed ba_constant ────────────────────────────────
//
// A ba_constant's `data` field means different things depending on whether the
// node is typed: for a typed constant it is an index into the BA constants
// pool, for an untyped one it is a string id for the constant's source text.
// node::hashit() picks which of the two to read from the node's ba_type.
//
// untype() used to clear the ba_type of every node it was given, ba_constants
// included, leaving the pool index in place. That produced a node claiming to
// be untyped while still holding a pool index, so hashing it looked the index
// up in the string dictionary. Once the pool had grown past the dictionary
// (reached by normalizing a handful of bitvector constants in one session) that
// read went out of bounds: an assert in debug, a segfault in release, and a
// silently wrong hash in between.
TEST_SUITE("untype of a ba_constant") {

	TEST_CASE("keeps the ba_type, so data stays a pool index") {
		tref c = tau::get_ba_constant(Bool(true), bool_type());
		REQUIRE( c != nullptr );
		const size_t type = tau::get(c).get_ba_type();
		const size_t id = tau::get(c).get_ba_constant_id();
		REQUIRE( type != 0 );  // it really is typed
		REQUIRE( id != 0 );

		tref u = untype<node_t>(c);
		REQUIRE( u != nullptr );
		CHECK( tau::get(u).get_ba_type() == type );
		CHECK( tau::get(u).get_ba_constant_id() == id );
	}

	// Hashing is where the inconsistent node was read, so exercise it. Two
	// constants of the same type must hash apart, and untyping must not
	// change a constant's hash.
	TEST_CASE("hashing an untyped ba_constant stays in range") {
		tref t = tau::get_ba_constant(Bool(true), bool_type());
		tref f = tau::get_ba_constant(Bool(false), bool_type());
		REQUIRE( t != nullptr );
		REQUIRE( f != nullptr );
		CHECK( tau::get(t).value.hashit()
					!= tau::get(f).value.hashit() );
		CHECK( tau::get(untype<node_t>(t)).value.hashit()
					== tau::get(t).value.hashit() );
		CHECK( tau::get(untype<node_t>(t)).value.hashit()
					!= tau::get(untype<node_t>(f)).value.hashit() );
	}

	// canonize() is untype()'s caller inside inference; it must keep
	// constants of the same value but different types apart.
	TEST_CASE("canonize keeps a typed constant distinct from another type") {
		tref c = tau::get_ba_constant(Bool(true), bool_type());
		REQUIRE( c != nullptr );
		CHECK( canonize<node_t>(c) == c );
	}

	// The fix must stay narrow: a typed *variable* is still untyped, since
	// its data is a name and does not depend on the ba_type.
	TEST_CASE("a typed variable is still untyped") {
		tref parsed = parse_bf_no_infer("x:sbf");
		REQUIRE( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		REQUIRE( inferred != nullptr );
		tref v = find_first<tau::variable>(inferred);
		REQUIRE( v != nullptr );
		REQUIRE( tau::get(v).get_ba_type() != 0 );
		CHECK( tau::get(untype<node_t>(v)).get_ba_type() == 0 );
	}
}

TEST_SUITE("regression/typed rec-relation head with a wff body") {

	// BA2-2: `p(x):sbf := x = 0.` is classified as a functional relation
	// (its head is typed) but its body is a formula, which
	// update_functional_rr rejects by returning nullptr. The rec_relation
	// on_leave handler only looked for parse_error/inference_error and
	// stored that nullptr as a child of the rec_relations node; the final
	// update pass then dereferenced it. The rejection must surface as an
	// inference failure (whole parse yields nullptr), never as a crash.
	TEST_CASE("a typed head with a wff body is rejected without crashing") {
		tref n = tau::get("p(x):sbf := x = 0. T.");
		CHECK( n == nullptr );
	}

	// The neighbouring accepted shapes from the same sample table must
	// keep parsing, so the rejection stays narrow.
	TEST_CASE("a typed head with a bf body still parses") {
		tref n = tau::get("p(x):sbf := x'. T.");
		CHECK( n != nullptr );
	}

	TEST_CASE("an untyped head with a wff body still parses") {
		tref n = tau::get("p(x:sbf) := x = 0. T.");
		CHECK( n != nullptr );
	}
}
