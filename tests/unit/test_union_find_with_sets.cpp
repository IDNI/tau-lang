// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// union_find_with_sets (src/union_find_with_sets.h) had zero test coverage
// anywhere in the repo, despite being the structure squeeze_absorb (in
// src/normal_forms_bf.tmpl.h) relies on to track variable-overlap
// equivalence classes via `uf.get_set(...)`. Unlike the simpler
// union_find/union_find_by_rank/union_find_by_less in src/union_find.h
// (covered by test_union_find.cpp), union_find_with_sets is keyed by tref
// tree nodes (compared/hashed structurally, not by pointer identity), takes
// its root-selection comparator by reference at construction, and -- beyond
// find/merge/connected -- maintains a circular next-pointer ring per set so
// that `get_set(x)` can enumerate every element sharing x's set in O(n).
//
// This file uses the lightweight node_t = node<bv, Bool> pack from
// test_Bool_helpers.h (the same one test_gc.cpp uses) rather than the full
// bas_pack, since these tests only need plain variable trefs and no BA
// simplification/solving machinery.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "union_find_with_sets.h"

using namespace idni::tau_lang;

namespace {

// Comparator matching the pattern used in squeeze_absorb/simplify_using_equality
// (src/normal_forms_bf.tmpl.h): `union_find_with_sets<decltype(uf_comp), node>(uf_comp)`.
inline auto make_subtree_comp() {
	return [](tref l, tref r) { return tau::subtree_less(l, r); };
}

// Returns true if `v` contains an element equal (by pointer, which is safe
// here since all trefs come from the hash-consed `build_bf_variable()` calls
// below) to `x`.
bool contains_tref(const trefs& v, tref x) {
	return std::find(v.begin(), v.end(), x) != v.end();
}

} // namespace

TEST_SUITE("union_find_with_sets basic insert/find/contains") {

	TEST_CASE("insert creates a singleton set") {
		auto comp = make_subtree_comp();
		union_find_with_sets<decltype(comp), node_t> uf(comp);
		tref a = tau::build_bf_variable("a");
		uf.insert(a);
		CHECK(uf.contains(a));
		CHECK(uf.find(a) == a);
		trefs set_a = uf.get_set(a);
		REQUIRE(set_a.size() == 1);
		CHECK(set_a[0] == a);
	}

	TEST_CASE("insert is a no-op if the element is already present") {
		auto comp = make_subtree_comp();
		union_find_with_sets<decltype(comp), node_t> uf(comp);
		tref a = tau::build_bf_variable("a");
		uf.insert(a);
		uf.insert(a);
		CHECK(uf.contains(a));
		CHECK(uf.get_set(a).size() == 1);
	}

	TEST_CASE("empty structure: find auto-inserts an absent element as its own root") {
		auto comp = make_subtree_comp();
		union_find_with_sets<decltype(comp), node_t> uf(comp);
		tref x = tau::build_bf_variable("x_fresh_empty_struct");
		CHECK(!uf.contains(x));
		CHECK(uf.find(x) == x);
		CHECK(uf.contains(x));
	}

	TEST_CASE("empty structure: contains/connected are false for anything not yet touched") {
		auto comp = make_subtree_comp();
		union_find_with_sets<decltype(comp), node_t> uf(comp);
		tref a = tau::build_bf_variable("a_empty2");
		tref b = tau::build_bf_variable("b_empty2");
		CHECK(!uf.contains(a));
		CHECK(!uf.contains(b));
		CHECK(!uf.connected(a, b));
	}
}

TEST_SUITE("union_find_with_sets merge/connected") {

	TEST_CASE("merging two elements connects them") {
		auto comp = make_subtree_comp();
		union_find_with_sets<decltype(comp), node_t> uf(comp);
		tref a = tau::build_bf_variable("a_m1");
		tref b = tau::build_bf_variable("b_m1");
		CHECK(!uf.connected(a, b));
		uf.merge(a, b);
		CHECK(uf.connected(a, b));
		CHECK(uf.find(a) == uf.find(b));
	}

	TEST_CASE("transitive merges: A-B, B-C implies A and C are connected") {
		auto comp = make_subtree_comp();
		union_find_with_sets<decltype(comp), node_t> uf(comp);
		tref a = tau::build_bf_variable("a_trans");
		tref b = tau::build_bf_variable("b_trans");
		tref c = tau::build_bf_variable("c_trans");
		uf.merge(a, b);
		uf.merge(b, c);
		CHECK(uf.connected(a, b));
		CHECK(uf.connected(b, c));
		CHECK(uf.connected(a, c));
		CHECK(uf.find(a) == uf.find(b));
		CHECK(uf.find(b) == uf.find(c));
	}

	TEST_CASE("independent/disjoint sets remain unconnected") {
		auto comp = make_subtree_comp();
		union_find_with_sets<decltype(comp), node_t> uf(comp);
		tref a = tau::build_bf_variable("a_disj");
		tref b = tau::build_bf_variable("b_disj");
		tref c = tau::build_bf_variable("c_disj");
		tref d = tau::build_bf_variable("d_disj");
		uf.merge(a, b);
		uf.merge(c, d);
		CHECK(uf.connected(a, b));
		CHECK(uf.connected(c, d));
		CHECK(!uf.connected(a, c));
		CHECK(!uf.connected(a, d));
		CHECK(!uf.connected(b, c));
		CHECK(!uf.connected(b, d));
	}

	TEST_CASE("edge case: merging an element with itself is a no-op") {
		auto comp = make_subtree_comp();
		union_find_with_sets<decltype(comp), node_t> uf(comp);
		tref a = tau::build_bf_variable("a_self");
		tref r = uf.merge(a, a);
		CHECK(r == a);
		CHECK(uf.connected(a, a));
		trefs set_a = uf.get_set(a);
		REQUIRE(set_a.size() == 1);
		CHECK(set_a[0] == a);
	}

	TEST_CASE("edge case: merging already-connected elements is idempotent") {
		auto comp = make_subtree_comp();
		union_find_with_sets<decltype(comp), node_t> uf(comp);
		tref a = tau::build_bf_variable("a_idem");
		tref b = tau::build_bf_variable("b_idem");
		tref first_root = uf.merge(a, b);
		tref second_root = uf.merge(a, b);
		CHECK(first_root == second_root);
		CHECK(uf.connected(a, b));
		// Re-merging an already-connected pair must not duplicate ring links.
		trefs set_a = uf.get_set(a);
		CHECK(set_a.size() == 2);
		CHECK(contains_tref(set_a, a));
		CHECK(contains_tref(set_a, b));
	}

	TEST_CASE("edge case: elements comparing equal under the comparator are not merged") {
		// The class documents that when neither element compares less than
		// the other, the merge is skipped entirely and the two sets remain
		// separate (src/union_find_with_sets.h merge(): "We do not merge
		// equal comparing elements"). A comparator that always returns
		// false makes every pair of distinct roots compare "equal".
		auto never_less = [](tref, tref) { return false; };
		union_find_with_sets<decltype(never_less), node_t> uf(never_less);
		tref x = tau::build_bf_variable("x_tie");
		tref y = tau::build_bf_variable("y_tie");
		tref result = uf.merge(x, y);
		CHECK(result == uf.find(x));
		CHECK(!uf.connected(x, y));
	}
}

TEST_SUITE("union_find_with_sets root selection via comparator") {

	TEST_CASE("the lesser element per comp becomes the new root, independent of argument order") {
		tref a = tau::build_bf_variable("a_rank");
		tref b = tau::build_bf_variable("b_rank");
		tref c = tau::build_bf_variable("c_rank");
		// Deterministic ranks: a < b < c, independent of tau::subtree_less's
		// (structural, not name-based) ordering.
		std::unordered_map<tref, int> rank { {a, 1}, {b, 2}, {c, 3} };
		auto rank_comp = [&rank](tref l, tref r) {
			return rank.at(l) < rank.at(r);
		};
		union_find_with_sets<decltype(rank_comp), node_t> uf(rank_comp);

		// merge(b, a): passing the higher-rank element first must still
		// leave the lower-rank element (a) as the representative, since
		// root selection is based on comparing roots, not argument order.
		tref root1 = uf.merge(b, a);
		CHECK(root1 == a);
		CHECK(uf.find(b) == a);

		// merge(c, root-of-{a,b}): c has the highest rank, so a remains root.
		tref root2 = uf.merge(c, a);
		CHECK(root2 == a);
		CHECK(uf.find(c) == a);
		CHECK(uf.find(b) == a);

		CHECK(uf.connected(a, b));
		CHECK(uf.connected(b, c));
		CHECK(uf.connected(a, c));
	}
}

TEST_SUITE("union_find_with_sets::get_set") {

	TEST_CASE("get_set on an untouched element returns just itself") {
		auto comp = make_subtree_comp();
		union_find_with_sets<decltype(comp), node_t> uf(comp);
		tref a = tau::build_bf_variable("a_getset_solo");
		trefs set_a = uf.get_set(a);
		REQUIRE(set_a.size() == 1);
		CHECK(set_a[0] == a);
	}

	TEST_CASE("get_set returns exactly the membership list of a merged group") {
		auto comp = make_subtree_comp();
		union_find_with_sets<decltype(comp), node_t> uf(comp);
		tref a = tau::build_bf_variable("a_getset");
		tref b = tau::build_bf_variable("b_getset");
		tref c = tau::build_bf_variable("c_getset");
		tref d = tau::build_bf_variable("d_getset"); // stays in its own set
		uf.merge(a, b);
		uf.merge(b, c);
		uf.insert(d);

		trefs group = uf.get_set(a);
		REQUIRE(group.size() == 3);
		CHECK(contains_tref(group, a));
		CHECK(contains_tref(group, b));
		CHECK(contains_tref(group, c));
		CHECK(!contains_tref(group, d));

		// get_set is well-defined from any member of the group, not just
		// the one used to start iteration.
		trefs group_from_c = uf.get_set(c);
		CHECK(group_from_c.size() == 3);
		CHECK(contains_tref(group_from_c, a));
		CHECK(contains_tref(group_from_c, b));
		CHECK(contains_tref(group_from_c, c));

		trefs group_d = uf.get_set(d);
		REQUIRE(group_d.size() == 1);
		CHECK(group_d[0] == d);
	}
}

TEST_SUITE("union_find_with_sets::merge(other) and clear") {

	TEST_CASE("merge(other) absorbs another union_find_with_sets's relationships") {
		// Both union-finds must share the exact same comparator type, so a
		// single lambda instance is used to construct both -- mirroring how
		// squeeze_absorb builds a fresh union_find_with_sets<decltype(uf_comp), node>
		// per scope while reusing the same comparator lambda.
		auto comp = make_subtree_comp();
		using uf_t = union_find_with_sets<decltype(comp), node_t>;

		tref a = tau::build_bf_variable("a_absorb");
		tref b = tau::build_bf_variable("b_absorb");
		tref c = tau::build_bf_variable("c_absorb");

		uf_t source(comp);
		source.merge(a, b);
		source.merge(b, c);

		uf_t target(comp);
		CHECK(!target.connected(a, c));
		target.merge(source);
		CHECK(target.connected(a, b));
		CHECK(target.connected(b, c));
		CHECK(target.connected(a, c));
	}

	TEST_CASE("clear resets the structure to empty") {
		auto comp = make_subtree_comp();
		union_find_with_sets<decltype(comp), node_t> uf(comp);
		tref a = tau::build_bf_variable("a_clear");
		tref b = tau::build_bf_variable("b_clear");
		uf.merge(a, b);
		REQUIRE(uf.connected(a, b));

		uf.clear();
		CHECK(!uf.contains(a));
		CHECK(!uf.contains(b));
		// After clear, elements are strangers again: find() re-inserts them
		// as fresh singletons rather than recalling the old merge.
		CHECK(!uf.connected(a, b));
		CHECK(uf.find(a) == a);
		CHECK(uf.find(b) == b);
	}
}

TEST_SUITE("union_find_with_sets copy semantics") {

	TEST_CASE("copy-constructing yields an independent structure sharing the comparator") {
		auto comp = make_subtree_comp();
		using uf_t = union_find_with_sets<decltype(comp), node_t>;
		uf_t uf(comp);
		tref a = tau::build_bf_variable("a_copy");
		tref b = tau::build_bf_variable("b_copy");
		tref c = tau::build_bf_variable("c_copy");
		tref d = tau::build_bf_variable("d_copy");
		uf.merge(a, b);

		uf_t copy(uf);
		CHECK(copy.connected(a, b));

		// Mutating the original after the copy must not affect the copy.
		uf.merge(c, d);
		CHECK(uf.connected(c, d));
		CHECK(!copy.connected(c, d));
	}
}

TEST_SUITE("union_find_with_sets move semantics") {

	TEST_CASE("move-constructing transfers the relationships to the new instance") {
		// _comp is held by reference (not owned), so the source lambda must
		// outlive both the moved-from and moved-to instances here.
		auto comp = make_subtree_comp();
		using uf_t = union_find_with_sets<decltype(comp), node_t>;
		uf_t uf(comp);
		tref a = tau::build_bf_variable("a_moveconstruct");
		tref b = tau::build_bf_variable("b_moveconstruct");
		uf.merge(a, b);

		uf_t moved(std::move(uf));
		CHECK(moved.connected(a, b));
		CHECK(moved.find(a) == moved.find(b));
	}

	TEST_CASE("move-assignment transfers the relationships to the target instance") {
		auto comp = make_subtree_comp();
		using uf_t = union_find_with_sets<decltype(comp), node_t>;
		uf_t src(comp);
		tref a = tau::build_bf_variable("a_moveassign");
		tref b = tau::build_bf_variable("b_moveassign");
		src.merge(a, b);

		uf_t dst(comp);
		tref c = tau::build_bf_variable("c_moveassign");
		dst.insert(c);

		dst = std::move(src);
		CHECK(dst.connected(a, b));
	}
}
