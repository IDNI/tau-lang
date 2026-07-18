// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"
#include "parser_helper.h"

#include "ba_types_inference.h"

TEST_SUITE("Configuration") {

	TEST_CASE("bdd init") {
		bdd_init<Bool>();
	}
}


TEST_SUITE("type_scoped_resolver") {

	TEST_CASE("merging in the same scope") {
		type_scoped_resolver<node_t> r;
		tref a = tau::get("T", parse_opts_wff_no_infer); // just an existing tref for testing
		tref b = tau::get("F", parse_opts_wff_no_infer); // just an existing tref for testing
		r.insert(a);
		r.insert(b);
		size_t t = 1;
		CHECK(!std::holds_alternative<inference_error>(r.assign(a, t)));
		CHECK(!std::holds_alternative<inference_error>(r.merge(a, b)));
		CHECK(r.type_id_of(a) == r.type_id_of(b));
		CHECK(r.type_id_of(a) == t);
	}

	TEST_CASE("merging in different scopes") {
		type_scoped_resolver<node_t> r;
		tref a = tau::get("T", parse_opts_wff_no_infer); // just an existing tref for testing
		tref b = tau::get("F", parse_opts_wff_no_infer); // just an existing tref for testing
		size_t t = 1;
		r.insert(a);
		r.open({{b, t}});
		CHECK(!std::holds_alternative<inference_error>(r.merge(a, b)));
		CHECK(r.type_id_of(a) == r.type_id_of(b));
		CHECK(r.type_id_of(a) == t);
	}

	TEST_CASE("merging conflicting types in the same scope") {
		type_scoped_resolver<node_t> r;
		tref a = tau::get("T", parse_opts_wff_no_infer); // just an existing tref for testing
		tref b = tau::get("F", parse_opts_wff_no_infer); // just an existing tref for testing
		r.insert(a);
		r.insert(b);
		size_t t1 = 1;
		size_t t2 = 2;
		CHECK(!std::holds_alternative<inference_error>(r.assign(a, t1)));
		CHECK(!std::holds_alternative<inference_error>(r.assign(b, t2)));
		CHECK(std::holds_alternative<inference_error>(r.merge(a, b))); // conflicting types
	}

	TEST_CASE("merging conflicting types in different scopes") {
		type_scoped_resolver<node_t> r;
		tref a = tau::get("T", parse_opts_wff_no_infer); // just an existing tref for testing
		tref b = tau::get("F", parse_opts_wff_no_infer); // just an existing tref for testing
		size_t t1 = 1;
		size_t t2 = 2;
		r.insert(a);
		CHECK(!std::holds_alternative<inference_error>(r.assign(a, t1)));
		r.open({{b, t2}});
		CHECK(std::holds_alternative<inference_error>(r.merge(a, b))); // conflicting types
	}
}

// TY-14: direct tests for the free-function helpers declared in
// type_scoped_resolver.h (insert, the 4 open() overloads, the 3
// open_same_type() overloads, the 2 merge() overloads, and unify()).
// These were previously only exercised indirectly through
// infer_ba_types(), which let a real bug (TY-4, see below) survive.
TEST_SUITE("type_scoped_resolver free functions") {

	TEST_CASE("free insert (map variant): assigns types without opening a scope") {
		type_scoped_resolver<node_t> r;
		tref a = tau::get("T", parse_opts_wff_no_infer);
		tref b = tau::get("F", parse_opts_wff_no_infer);
		size_t bool_tid = get_ba_type_id<node_t>(bool_type<node_t>());
		size_t sbf_tid = get_ba_type_id<node_t>(sbf_type<node_t>());
		std::map<size_t, subtree_map<node_t, size_t>> types {
			{ 0, { { a, bool_tid } } },
			{ 1, { { b, sbf_tid } } } };
		auto scopes_before = r.scoped.scopes.size();
		auto err = insert<node_t>(r, types);
		CHECK(!err.has_value());
		CHECK(r.scoped.scopes.size() == scopes_before); // no scope opened
		CHECK(r.type_id_of(a) == bool_tid);
		CHECK(r.type_id_of(b) == sbf_tid);
	}

	TEST_CASE("free insert (map variant): reports inference_error on conflicting types") {
		type_scoped_resolver<node_t> r;
		tref a = tau::get("T", parse_opts_wff_no_infer);
		size_t nat_tid = get_ba_type_id<node_t>(nat_type<node_t>());
		size_t bool_tid = get_ba_type_id<node_t>(bool_type<node_t>());
		// Same tref `a` typed twice with incompatible types across two
		// groups; std::map iterates groups 0 then 1, so the conflict is
		// detected when the second assignment is attempted.
		std::map<size_t, subtree_map<node_t, size_t>> types {
			{ 0, { { a, nat_tid } } },
			{ 1, { { a, bool_tid } } } };
		auto err = insert<node_t>(r, types);
		REQUIRE(err.has_value());
		CHECK(err->element == a);
		CHECK(err->expected == nat_tid);
		CHECK(err->found == bool_tid);
	}

	TEST_CASE("free insert (initializer_list variant): assigns types without opening a scope") {
		type_scoped_resolver<node_t> r;
		tref a = tau::get("T", parse_opts_wff_no_infer);
		tref b = tau::get("F", parse_opts_wff_no_infer);
		size_t bool_tid = get_ba_type_id<node_t>(bool_type<node_t>());
		size_t sbf_tid = get_ba_type_id<node_t>(sbf_type<node_t>());
		auto scopes_before = r.scoped.scopes.size();
		auto err = insert<node_t>(r, {
			subtree_map<node_t, size_t>{ { a, bool_tid } },
			subtree_map<node_t, size_t>{ { b, sbf_tid } } });
		CHECK(!err.has_value());
		CHECK(r.scoped.scopes.size() == scopes_before); // no scope opened
		CHECK(r.type_id_of(a) == bool_tid);
		CHECK(r.type_id_of(b) == sbf_tid);
	}

	TEST_CASE("free insert (initializer_list variant): reports inference_error on conflicting types") {
		type_scoped_resolver<node_t> r;
		tref a = tau::get("T", parse_opts_wff_no_infer);
		size_t nat_tid = get_ba_type_id<node_t>(nat_type<node_t>());
		size_t bool_tid = get_ba_type_id<node_t>(bool_type<node_t>());
		auto err = insert<node_t>(r, {
			subtree_map<node_t, size_t>{ { a, nat_tid } },
			subtree_map<node_t, size_t>{ { a, bool_tid } } });
		REQUIRE(err.has_value());
		CHECK(err->element == a);
	}

	TEST_CASE("free open (initializer_list<trefs>, size_t type): opens a new scope, typing every ref with the given id") {
		type_scoped_resolver<node_t> r;
		tref a = tau::get("T", parse_opts_wff_no_infer);
		tref b = tau::get("F", parse_opts_wff_no_infer);
		tref c = tau::get("c = d", parse_opts_wff_no_infer);
		size_t sbf_tid = get_ba_type_id<node_t>(sbf_type<node_t>());
		auto scopes_before = r.scoped.scopes.size();
		open<node_t>(r, { {a, b}, {c} }, sbf_tid);
		CHECK(r.scoped.scopes.size() == scopes_before + 1);
		CHECK(r.type_id_of(a) == sbf_tid);
		CHECK(r.type_id_of(b) == sbf_tid);
		CHECK(r.type_id_of(c) == sbf_tid);
		CHECK(r.scope_of(a) == r.scoped.current);
		CHECK(!r.close().has_value());
		CHECK(r.scoped.scopes.size() == scopes_before);
	}

	TEST_CASE("free open (initializer_list<trefs>, tref type): resolves the type tree to its BA type id") {
		type_scoped_resolver<node_t> r;
		tref a = tau::get("T", parse_opts_wff_no_infer);
		tref bool_type_tree = bool_type<node_t>();
		size_t bool_tid = get_ba_type_id<node_t>(bool_type_tree);
		open<node_t>(r, { {a} }, bool_type_tree);
		CHECK(r.type_id_of(a) == bool_tid);
	}

	TEST_CASE("free open (map variant): opens a scope with per-node types from a grouped map") {
		type_scoped_resolver<node_t> r;
		tref a = tau::get("T", parse_opts_wff_no_infer);
		tref b = tau::get("F", parse_opts_wff_no_infer);
		size_t bool_tid = get_ba_type_id<node_t>(bool_type<node_t>());
		size_t sbf_tid = get_ba_type_id<node_t>(sbf_type<node_t>());
		std::map<size_t, subtree_map<node_t, size_t>> types {
			{ 0, { { a, bool_tid } } },
			{ 1, { { b, sbf_tid } } } };
		auto scopes_before = r.scoped.scopes.size();
		open<node_t>(r, types);
		CHECK(r.scoped.scopes.size() == scopes_before + 1);
		CHECK(r.type_id_of(a) == bool_tid);
		CHECK(r.type_id_of(b) == sbf_tid);
	}

	TEST_CASE("free open (initializer_list<subtree_map> variant): opens a scope with per-node types from a list of maps") {
		type_scoped_resolver<node_t> r;
		tref a = tau::get("T", parse_opts_wff_no_infer);
		tref b = tau::get("F", parse_opts_wff_no_infer);
		size_t bool_tid = get_ba_type_id<node_t>(bool_type<node_t>());
		size_t sbf_tid = get_ba_type_id<node_t>(sbf_type<node_t>());
		auto scopes_before = r.scoped.scopes.size();
		open<node_t>(r, {
			subtree_map<node_t, size_t>{ { a, bool_tid } },
			subtree_map<node_t, size_t>{ { b, sbf_tid } } });
		CHECK(r.scoped.scopes.size() == scopes_before + 1);
		CHECK(r.type_id_of(a) == bool_tid);
		CHECK(r.type_id_of(b) == sbf_tid);
	}

	TEST_CASE("free open_same_type (subtree_set variant): assigns default_type without opening a new scope") {
		type_scoped_resolver<node_t> r;
		tref a = tau::get("T", parse_opts_wff_no_infer);
		tref b = tau::get("F", parse_opts_wff_no_infer);
		size_t bool_tid = get_ba_type_id<node_t>(bool_type<node_t>());
		subtree_set<node_t> refs{ a, b };
		auto scopes_before = r.scoped.scopes.size();
		auto result = open_same_type<node_t>(r, refs, bool_tid);
		REQUIRE(!std::holds_alternative<inference_error>(result));
		CHECK(std::get<size_t>(result) == bool_tid);
		// Despite its name, this base overload does NOT open a new scope;
		// only the initializer_list overload below does (via resolver.open()).
		CHECK(r.scoped.scopes.size() == scopes_before);
		CHECK(r.type_id_of(a) == bool_tid);
		CHECK(r.type_id_of(b) == bool_tid);
	}

	TEST_CASE("free open_same_type (map variant): infers and correctly assigns the unified type") {
		type_scoped_resolver<node_t> r;
		tref a = tau::get("T", parse_opts_wff_no_infer);
		tref b = tau::get("F", parse_opts_wff_no_infer);
		size_t bool_tid = get_ba_type_id<node_t>(bool_type<node_t>());
		size_t untyped_tid = untyped_type_id<node_t>();
		std::map<size_t, subtree_map<node_t, size_t>> types {
			{ 0, { { a, bool_tid } } },
			{ 1, { { b, untyped_tid } } } };
		auto result = open_same_type<node_t>(r, types, untyped_tid);
		REQUIRE(!std::holds_alternative<inference_error>(result));
		// Unlike the initializer_list overload below (TY-4), this map
		// overload folds default_type with every provided type via unify()
		// *before* delegating to the subtree_set overload, so both the
		// return value and each node's type_id reflect the inferred type
		// (bool), not the original default_type (untyped).
		CHECK(std::get<size_t>(result) == bool_tid);
		CHECK(r.type_id_of(a) == bool_tid);
		CHECK(r.type_id_of(b) == bool_tid);
	}

	TEST_CASE("free open_same_type (map variant): reports inference_error on incompatible types") {
		type_scoped_resolver<node_t> r;
		tref a = tau::get("T", parse_opts_wff_no_infer);
		tref b = tau::get("F", parse_opts_wff_no_infer);
		size_t bool_tid = get_ba_type_id<node_t>(bool_type<node_t>());
		size_t sbf_tid = get_ba_type_id<node_t>(sbf_type<node_t>());
		std::map<size_t, subtree_map<node_t, size_t>> types {
			{ 0, { { a, bool_tid } } },
			{ 1, { { b, sbf_tid } } } };
		auto result = open_same_type<node_t>(r, types, untyped_type_id<node_t>());
		CHECK(std::holds_alternative<inference_error>(result));
	}

	// TY-4 (still present at HEAD): the initializer_list overload of
	// open_same_type() computes `inferred_type` by folding default_type
	// with every provided type via unify() -- exactly like the map overload
	// tested above -- but then stores `scoped[t] = default_type` (not
	// inferred_type) and calls resolver.open(scoped) directly, and finally
	// `return default_type` (not inferred_type). So both the returned type
	// and every node's stored type end up being the original default_type
	// even when a strictly more specific compatible type was inferred.
	// This test documents CURRENT (buggy) behavior; it intentionally does
	// NOT modify src/type_scoped_resolver.tmpl.h to fix it.
	TEST_CASE("free open_same_type (initializer_list variant): TY-4 bug -- stores/returns default_type instead of the inferred type") {
		type_scoped_resolver<node_t> r;
		tref a = tau::get("T", parse_opts_wff_no_infer);
		size_t bool_tid = get_ba_type_id<node_t>(bool_type<node_t>());
		size_t untyped_tid = untyped_type_id<node_t>();
		REQUIRE(bool_tid != untyped_tid);
		auto result = open_same_type<node_t>(r,
			{ subtree_map<node_t, size_t>{ { a, bool_tid } } }, untyped_tid);
		REQUIRE(!std::holds_alternative<inference_error>(result));
		// Buggy: the correctly-inferred type here is bool_tid, but the
		// function returns the original default_type (untyped) instead.
		CHECK(std::get<size_t>(result) == untyped_tid);
		// Buggy: `a` ends up typed as untyped in the resolver, not bool.
		CHECK(r.type_id_of(a) == untyped_tid);
	}

	TEST_CASE("free merge (initializer_list variant): merges nodes, ignoring the map's type values") {
		type_scoped_resolver<node_t> r;
		tref a = tau::get("T", parse_opts_wff_no_infer);
		tref b = tau::get("F", parse_opts_wff_no_infer);
		tref c = tau::get("c = d", parse_opts_wff_no_infer);
		size_t bool_tid = get_ba_type_id<node_t>(bool_type<node_t>());
		r.insert(a); r.insert(b); r.insert(c);
		CHECK(!std::holds_alternative<inference_error>(r.assign(a, bool_tid)));
		CHECK(!std::holds_alternative<inference_error>(r.assign(b, bool_tid)));
		CHECK(!std::holds_alternative<inference_error>(r.assign(c, bool_tid)));
		// The map's type-id values (999/888/777) are irrelevant: merge<node>
		// only extracts the keys (trefs) and merges their *already assigned*
		// types via resolver.merge(trefs).
		auto merged = merge<node_t>(r, {
			subtree_map<node_t, size_t>{ { a, 999 } },
			subtree_map<node_t, size_t>{ { b, 888 }, { c, 777 } } });
		REQUIRE(!std::holds_alternative<inference_error>(merged));
		CHECK(std::get<size_t>(merged) == bool_tid);
		CHECK(r.type_id_of(a) == bool_tid);
		CHECK(r.type_id_of(b) == bool_tid);
		CHECK(r.type_id_of(c) == bool_tid);
	}

	TEST_CASE("free merge (map variant): merges nodes, ignoring the map's type values") {
		type_scoped_resolver<node_t> r;
		tref a = tau::get("T", parse_opts_wff_no_infer);
		tref b = tau::get("F", parse_opts_wff_no_infer);
		size_t sbf_tid = get_ba_type_id<node_t>(sbf_type<node_t>());
		r.insert(a); r.insert(b);
		CHECK(!std::holds_alternative<inference_error>(r.assign(a, sbf_tid)));
		CHECK(!std::holds_alternative<inference_error>(r.assign(b, sbf_tid)));
		std::map<size_t, subtree_map<node_t, size_t>> types {
			{ 0, { { a, 111 } } },
			{ 1, { { b, 222 } } } };
		auto merged = merge<node_t>(r, types);
		REQUIRE(!std::holds_alternative<inference_error>(merged));
		CHECK(std::get<size_t>(merged) == sbf_tid);
		CHECK(r.type_id_of(a) == r.type_id_of(b));
	}

	TEST_CASE("free merge: reports inference_error when collected nodes have incompatible pre-assigned types") {
		type_scoped_resolver<node_t> r;
		tref a = tau::get("T", parse_opts_wff_no_infer);
		tref b = tau::get("F", parse_opts_wff_no_infer);
		size_t nat_tid = get_ba_type_id<node_t>(nat_type<node_t>());
		size_t bool_tid = get_ba_type_id<node_t>(bool_type<node_t>());
		r.insert(a); r.insert(b);
		CHECK(!std::holds_alternative<inference_error>(r.assign(a, nat_tid)));
		CHECK(!std::holds_alternative<inference_error>(r.assign(b, bool_tid)));
		auto merged = merge<node_t>(r, {
			subtree_map<node_t, size_t>{ { a, 0 } },
			subtree_map<node_t, size_t>{ { b, 0 } } });
		CHECK(std::holds_alternative<inference_error>(merged));
	}

	TEST_CASE("free unify (map, default_type): folds all provided types into one compatible id") {
		tref a = tau::get("T", parse_opts_wff_no_infer);
		tref b = tau::get("F", parse_opts_wff_no_infer);
		size_t untyped_tid = untyped_type_id<node_t>();
		size_t bool_tid = get_ba_type_id<node_t>(bool_type<node_t>());
		std::map<size_t, subtree_map<node_t, size_t>> types {
			{ 0, { { a, untyped_tid } } },
			{ 1, { { b, bool_tid } } } };
		auto result = unify<node_t>(types, untyped_tid);
		REQUIRE(!std::holds_alternative<inference_error>(result));
		CHECK(std::get<size_t>(result) == bool_tid);
	}

	TEST_CASE("free unify (map, default_type): reports inference_error on incompatible types") {
		tref a = tau::get("T", parse_opts_wff_no_infer);
		tref b = tau::get("F", parse_opts_wff_no_infer);
		size_t bool_tid = get_ba_type_id<node_t>(bool_type<node_t>());
		size_t sbf_tid = get_ba_type_id<node_t>(sbf_type<node_t>());
		std::map<size_t, subtree_map<node_t, size_t>> types {
			{ 0, { { a, bool_tid } } },
			{ 1, { { b, sbf_tid } } } };
		auto result = unify<node_t>(types, untyped_type_id<node_t>());
		REQUIRE(std::holds_alternative<inference_error>(result));
		CHECK(std::get<inference_error>(result).element == b);
	}

	// unify()'s "nat never unifies" contract (see TY-1): the nat type is
	// barred from BA unification even against itself.
	TEST_CASE("free unify (map, default_type): nat type never unifies, even with itself") {
		tref a = tau::get("T", parse_opts_wff_no_infer);
		size_t nat_tid = get_ba_type_id<node_t>(nat_type<node_t>());
		std::map<size_t, subtree_map<node_t, size_t>> types {
			{ 0, { { a, nat_tid } } } };
		auto result = unify<node_t>(types, nat_tid);
		CHECK(std::holds_alternative<inference_error>(result));
	}
}
