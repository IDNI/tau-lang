// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Layer 0 unit tests for src/anti_prenex/foundations/ctx.h (package E).
// Spec: anti_prenex.md §1 (ctx table, "Cache scope"), §5 (PUSH_EX_BLOCK's
// setup), §6/§7 (the wrapper's consumers).
//
// The structural tables are UNCONDITIONAL, so their behaviour — key identity,
// lazy fill, GC survival — is checked in BOTH build types. Everything that
// needs a gated table to exist is under `#ifdef TAU_CACHE`, i.e. Release:
// `TAU_CACHE` is OFF in Debug and there is deliberately no fallback storage.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "anti_prenex/foundations/ctx.h"

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

/// A distinct formula per name: `x_<name> = 0`, the shape §1 calls an atom.
tref mk_atom(const std::string& name) {
	return tau::build_bf_eq_0(tau::build_bf_variable("x_" + name, 0));
}

/// A distinct block variable per name.
tref mk_var(const std::string& name) {
	return tau::build_bf_variable(name, 0);
}

/// A SECOND SPELLING of `n`: the same content carrying a right sibling, the
/// shape any child collected from a traversal has. Structurally equal to `n`
/// under `subtree_equality`, but a different `tref` — so a pointer hash and
/// a content hash disagree about it.
tref untrimmed(tref n, tref sibling) {
	return tau::get(tau::get(tau::build_wff_and(n, sibling)).l).l;
}

} // namespace

TEST_SUITE("anti_prenex/foundations/ctx") {

// --- ctx (§5 PUSH_EX_BLOCK's setup) ---------------------------------------------

TEST_CASE("for_component: the two rank maps run opposite over 1..|P|") {
	// §5: P[0] is the OUTERMOST variable; order ← {P[i] ↦ |P|-i} (inner →
	// LOWER), prio ← {P[i] ↦ i+1} (inner → HIGHER).
	ap::block P{ mk_var("o1"), mk_var("o2"), mk_var("o3") };
	auto c = ap::ctx<node_t>::for_component(P, 1, false);

	CHECK(c.order.at(P[0]) == 3);
	CHECK(c.order.at(P[1]) == 2);
	CHECK(c.order.at(P[2]) == 1);

	CHECK(c.prio.at(P[0]) == 1);
	CHECK(c.prio.at(P[1]) == 2);
	CHECK(c.prio.at(P[2]) == 3);

	// The conventions are opposite, and each covers exactly 1..|P|.
	for (size_t i = 0; i < P.size(); ++i)
		CHECK(c.order.at(P[i]) + c.prio.at(P[i])
			== static_cast<int_t>(P.size()) + 1);
	CHECK(c.order.size() == P.size());
	CHECK(c.prio.size() == P.size());
}

TEST_CASE("for_component: type, keep_functional, expand_count, empty block") {
	ap::block P{ mk_var("t1") };
	auto c = ap::ctx<node_t>::for_component(P, 7, true);
	CHECK(c.type == 7);
	CHECK(c.keep_functional);
	CHECK(c.expand_count == 0);          // the one component-scoped counter

	// A default ctx is "not set up": type 0 is the untyped id.
	ap::ctx<node_t> fresh;
	CHECK(fresh.type == 0);

	auto empty = ap::ctx<node_t>::for_component(ap::block{}, 0, false);
	CHECK(empty.order.empty());
	CHECK(empty.prio.empty());
}

TEST_CASE("for_component: knobs are read at the call, not at start-up") {
	ap::block P{ mk_var("k1") };
	auto before = ap::ctx<node_t>::for_component(P, 1, false);
	CHECK(before.expand_max == ap::expand_max);
	CHECK(before.subsume_max == ap::subsume_max);

	const size_t saved = ap::expand_max;
	ap::expand_max = saved + 5;
	auto after = ap::ctx<node_t>::for_component(P, 1, false);
	ap::expand_max = saved;

	CHECK(after.expand_max == saved + 5);   // the NEXT component sees it
	CHECK(before.expand_max == saved);      // the one already set up does not
}

// --- taint (§1 cache scope) ------------------------------------------------------

TEST_CASE("taint: a global counter, never reset, incremented one at a time") {
	const size_t before = ap::taint_count<node_t>();
	ap::taint<node_t>();
	ap::taint<node_t>();
	CHECK(ap::taint_count<node_t>() == before + 2);
}

// --- the unconditional structural tables (both build types) -----------------------

TEST_CASE("find/store on an unconditional table: miss, hit, rewrite") {
	tref k = mk_atom("s1");
	CHECK(ap::find<node_t, ap::table::size_memo>(k) == nullptr);

	const size_t& r = ap::store<node_t, ap::table::size_memo>(k, 7);
	CHECK(r == 7);

	const size_t* p = ap::find<node_t, ap::table::size_memo>(k);
	REQUIRE(p != nullptr);
	CHECK(*p == 7);
	CHECK(p == &r);                       // one entry, not a copy

	// insert_or_assign: the entry keeps its place, so `r` still names it.
	ap::store<node_t, ap::table::size_memo>(k, 9);
	CHECK(*ap::find<node_t, ap::table::size_memo>(k) == 9);
	CHECK(r == 9);
}

TEST_CASE("memoised on an unconditional table: computed once, by reference") {
	tref k = mk_atom("m1");
	size_t calls = 0;
	auto& first = ap::memoised<node_t, ap::table::size_memo>(k,
		[&] { ++calls; return size_t(42); });
	CHECK(first == 42);
	CHECK(calls == 1);

	auto& second = ap::memoised<node_t, ap::table::size_memo>(k,
		[&] { ++calls; return size_t(43); });
	CHECK(second == 42);                  // the stored value wins
	CHECK(calls == 1);                    // §10: never recomputed
	CHECK(&first == &second);             // and it is the table's own entry
}

TEST_CASE("memoised on an unconditional table: a recursive fill is safe") {
	// The reference a parent holds survives the children's insertions
	// (node-based unordered_map). This is the shape of `formula_size` and
	// `atoms`.
	tref leaf = mk_atom("r_leaf"), mid = mk_atom("r_mid"), top = mk_atom("r_top");
	auto size_of = [&](tref n, auto&& self) -> const size_t& {
		return ap::memoised<node_t, ap::table::size_memo>(n, [&] {
			if (n == leaf) return size_t(1);
			if (n == mid)  return self(leaf, self) + 1;
			return self(mid, self) + 1;
		});
	};
	const size_t& top_size = size_of(top, size_of);
	CHECK(top_size == 3);
	CHECK(*ap::find<node_t, ap::table::size_memo>(mid) == 2);
	CHECK(*ap::find<node_t, ap::table::size_memo>(leaf) == 1);
	CHECK(top_size == 3);                 // still the live entry
}

TEST_CASE("tref_set values: what an atoms row looks like") {
	tref k = mk_atom("v1"), a = mk_atom("v2"), b = mk_atom("v3");
	const auto& s = ap::store<node_t, ap::table::atoms_memo>(k,
		ap::tref_set{ trefs{ a, b } });
	CHECK(s.items.size() == 2);
	// Package C's `atoms` hands out `const trefs&` from here.
	const trefs& items = ap::find<node_t, ap::table::atoms_memo>(k)->items;
	CHECK(items[0] == a);
	CHECK(items[1] == b);
}

// --- key identity (both build types, through a structural table) ------------------

TEST_CASE("keys are structural: a trimmed and an untrimmed spelling are one") {
	tref a = mk_atom("ka"), b = mk_atom("kb");
	tref f = tau::build_wff_and(a, b);
	tref conj = tau::get(f).l;                  // the wff_and node
	REQUIRE(conj != nullptr);
	tref first = tau::get(conj).l;              // `a`, carrying `b` as sibling
	REQUIRE(first != nullptr);

	// Same content, different node: the sibling link is part of the tref.
	CHECK(tau::trim_right_sibling(first) == a);
	CHECK(first != a);
	CHECK(subtree_equality<node_t>{}(first, a));
	CHECK(hash_lcrs_tref<node_t>{}(first) == hash_lcrs_tref<node_t>{}(a));

	// So both spellings land on one entry, whichever wrote it.
	ap::store<node_t, ap::table::size_memo>(a, 11);
	const size_t* via_untrimmed = ap::find<node_t, ap::table::size_memo>(first);
	REQUIRE(via_untrimmed != nullptr);
	CHECK(*via_untrimmed == 11);
}

TEST_CASE("pair keys: the hashers hash CONTENT, and agree with their equality") {
	tref k = mk_atom("p1"), j = mk_atom("p2");
	tref k2 = untrimmed(k, j);            // same content as k, other tref
	REQUIRE(k2 != k);
	REQUIRE(subtree_equality<node_t>{}(k2, k));

	ap::tref_bool_hash<node_t> hb;
	subtree_pair_equal<node_t, bool> eqb;
	ap::tref_pair_hash<node_t> hp;
	subtree_pair_equal<node_t, tref> eqp;

	// THE property ctx.h warns about: equal keys must hash equal. A hasher
	// that took a `tref` by pointer — via std::hash or by handing the tref
	// itself to hash_combine — passes every check below that uses one tref
	// twice, and fails these.
	CHECK(eqb({ k2, true }, { k, true }));
	CHECK(hb({ k2, true }) == hb({ k, true }));
	CHECK(eqp({ k2, j }, { k, j }));
	CHECK(hp({ k2, j }) == hp({ k, j }));
	CHECK(eqp({ j, k2 }, { j, k }));      // the second half too
	CHECK(hp({ j, k2 }) == hp({ j, k }));

	// And they still discriminate. keep_functional joins push_memo's key
	// alone (§1); cof_memo's (settled term, x) is ordered.
	CHECK(!eqb({ k, true }, { k, false }));
	CHECK(hb({ k, true }) != hb({ k, false }));
	CHECK(!eqp({ k, j }, { j, k }));
	CHECK(hp({ k, j }) != hp({ j, k }));
}

// --- the gated result tables ------------------------------------------------------

TEST_CASE("memoised on a gated table: one call with the table, two without") {
	tref k = mk_atom("g1");
	size_t calls = 0;
	auto compute = [&] { ++calls; return true; };

	CHECK(ap::memoised<node_t, ap::table::qbf_memo>(k, compute) == true);
	CHECK(ap::memoised<node_t, ap::table::qbf_memo>(k, compute) == true);
	// The result is the same either way — no result may depend on a hit.
#ifdef TAU_CACHE
	CHECK(calls == 1);
#else
	CHECK(calls == 2);
#endif
}

TEST_CASE("a pair-keyed gated table: the worker's result in both build types") {
	// Without this, memoised<push_memo>/<elim_memo> — the (tref, bool) keyed
	// tables layers 3 and 4 live on — are not even instantiated in Debug.
	tref k = mk_atom("pk1"), v = mk_atom("pk2");
	size_t calls = 0;
	auto compute = [&] { ++calls; return v; };
	CHECK(ap::memoised<node_t, ap::table::push_memo>({ k, false }, compute) == v);
	CHECK(ap::memoised<node_t, ap::table::push_memo>({ k, false }, compute) == v);
	CHECK(ap::memoised<node_t, ap::table::elim_memo>({ k, true }, compute) == v);
#ifdef TAU_CACHE
	CHECK(calls == 2);   // push_memo hit; elim_memo is a separate table
#else
	CHECK(calls == 3);
#endif
}

TEST_CASE("store/lookup on a gated table follow the build type") {
	tref k = mk_atom("g3");
	CHECK(!ap::lookup<node_t, ap::table::solver_memo>(k).has_value());
	ap::store<node_t, ap::table::solver_memo>(k, ap::answer::sat);
	auto got = ap::lookup<node_t, ap::table::solver_memo>(k);
#ifdef TAU_CACHE
	REQUIRE(got.has_value());
	CHECK(*got == ap::answer::sat);
#else
	CHECK(!got.has_value());              // no storage, hence no entry
	CHECK(ap::find<node_t, ap::table::solver_memo>(k) == nullptr);
#endif
}

TEST_CASE("flush leaves the structural tables alone, gated ones or not") {
	ap::flush_solver_dependent<node_t>();          // safe with no gated tables
	tref k = mk_atom("f0");
	ap::store<node_t, ap::table::size_memo>(k, 3); // structural, untouched
	ap::flush_solver_dependent<node_t>();
	CHECK(*ap::find<node_t, ap::table::size_memo>(k) == 3);
}

#ifdef TAU_CACHE

TEST_CASE("taint: a computation that hit a budget is returned, never cached") {
	tref k = mk_atom("t1"), v = mk_atom("t2");
	size_t calls = 0;
	auto tainting = [&] { ++calls; ap::taint<node_t>(); return v; };

	CHECK(ap::memoised<node_t, ap::table::push_memo>({ k, false }, tainting) == v);
	CHECK(ap::memoised<node_t, ap::table::push_memo>({ k, false }, tainting) == v);
	CHECK(calls == 2);                    // no entry was written
	CHECK(!ap::lookup<node_t, ap::table::push_memo>({ k, false }).has_value());

	tref clean_key = mk_atom("t3");
	size_t clean = 0;
	auto untainted = [&] { ++clean; return v; };
	ap::memoised<node_t, ap::table::push_memo>({ clean_key, false }, untainted);
	ap::memoised<node_t, ap::table::push_memo>({ clean_key, false }, untainted);
	CHECK(clean == 1);
}

TEST_CASE("taint is transitive through nesting") {
	// A hit inside the computation is a hit inside every enclosing one, so
	// neither the inner nor the outer entry is written.
	tref outer_key = mk_atom("t4"), inner_key = mk_atom("t5"), v = mk_atom("t6");
	size_t outer = 0, inner = 0;
	auto run = [&] {
		return ap::memoised<node_t, ap::table::push_memo>(
			{ outer_key, false }, [&] {
				++outer;
				return ap::memoised<node_t, ap::table::push_memo>(
					{ inner_key, false }, [&] {
						++inner;
						ap::taint<node_t>();
						return v;
					});
			});
	};
	run();
	run();
	CHECK(inner == 2);
	CHECK(outer == 2);
	CHECK(!ap::lookup<node_t, ap::table::push_memo>({ outer_key, false }).has_value());

	// The positive case, so this cannot pass by never caching anything:
	// nothing taints, so both the inner and the outer entry are written.
	tref clean_outer = mk_atom("t9"), clean_inner = mk_atom("t10");
	size_t co = 0, ci = 0;
	auto clean_run = [&] {
		return ap::memoised<node_t, ap::table::push_memo>(
			{ clean_outer, false }, [&] {
				++co;
				return ap::memoised<node_t, ap::table::push_memo>(
					{ clean_inner, false }, [&] { ++ci; return v; });
			});
	};
	clean_run();
	clean_run();
	CHECK(ci == 1);
	CHECK(co == 1);
}

TEST_CASE("a table that is not taint_aware caches across a taint") {
	// qbf_memo's entries are mathematical truths (§1), so a budget hit
	// elsewhere in the computation cannot invalidate them.
	tref k = mk_atom("t7");
	size_t calls = 0;
	auto tainting = [&] { ++calls; ap::taint<node_t>(); return true; };
	ap::memoised<node_t, ap::table::qbf_memo>(k, tainting);
	ap::memoised<node_t, ap::table::qbf_memo>(k, tainting);
	CHECK(calls == 1);
}

TEST_CASE("keep_functional discriminates two entries of one formula") {
	tref k = mk_atom("kf1"), yes = mk_atom("kf2"), no = mk_atom("kf3");
	ap::store<node_t, ap::table::push_memo>({ k, true }, yes);
	ap::store<node_t, ap::table::push_memo>({ k, false }, no);
	CHECK(*ap::lookup<node_t, ap::table::push_memo>({ k, true }) == yes);
	CHECK(*ap::lookup<node_t, ap::table::push_memo>({ k, false }) == no);
}

TEST_CASE("flush clears the solver-dependent tables and nothing else") {
	tref k = mk_atom("fl"), v = mk_atom("fl_v");
	ap::store<node_t, ap::table::solver_memo>(k, ap::answer::unsat);
	ap::store<node_t, ap::table::push_memo>({ k, false }, v);
	ap::store<node_t, ap::table::elim_memo>({ k, true }, v);
	ap::store<node_t, ap::table::qbf_memo>(k, true);
	ap::store<node_t, ap::table::cof_memo>({ k, v }, ap::cof_entry{ v, v, v, true, true });
	ap::store<node_t, ap::table::size_memo>(k, 5);

	ap::flush_solver_dependent<node_t>();

	// §1 FLUSH: solver_memo and the two block memos that may embed its
	// verdicts.
	CHECK(!ap::lookup<node_t, ap::table::solver_memo>(k).has_value());
	CHECK(!ap::lookup<node_t, ap::table::push_memo>({ k, false }).has_value());
	CHECK(!ap::lookup<node_t, ap::table::elim_memo>({ k, true }).has_value());
	// Exempt: mathematical truths, pure functions of the key, and the
	// structural facets.
	CHECK(*ap::lookup<node_t, ap::table::qbf_memo>(k) == true);
	CHECK(ap::find<node_t, ap::table::cof_memo>({ k, v }) != nullptr);
	CHECK(*ap::find<node_t, ap::table::size_memo>(k) == 5);
}

TEST_CASE("cof_memo pins terms that are not subtrees of its key") {
	// The three terms are built by COF, so GC introspection reaches them
	// only through cof_entry::for_each_tref (fwd.h).
	tref f = mk_atom("cof_f"), x = mk_atom("cof_x");
	tref f0 = mk_atom("cof_f0"), f1 = mk_atom("cof_f1"), p = mk_atom("cof_p");
	ap::store<node_t, ap::table::cof_memo>({ f, x },
		ap::cof_entry{ f0, f1, p, true, false });
	const auto* e = ap::find<node_t, ap::table::cof_memo>({ f, x });
	REQUIRE(e != nullptr);
	CHECK(e->f0 == f0);
	CHECK(e->usable);
	CHECK(!e->pin);
	size_t walked = 0;
	e->for_each_tref([&](tref) { ++walked; });
	CHECK(walked == 3);
}

#endif // TAU_CACHE

TEST_CASE("a table reference survives the insertions that rehash the table") {
	// ctx.h's LIFETIME claim: only a gc() sweep invalidates, never a later
	// insertion. Three entries cannot rehash; four thousand do.
	tref k = mk_atom("rh_key");
	const size_t& r = ap::store<node_t, ap::table::size_memo>(k, 123);
	const size_t* const before = &r;
	for (size_t i = 0; i < 4096; ++i)
		ap::store<node_t, ap::table::size_memo>(
			mk_atom("rh_" + std::to_string(i)), i);
	CHECK(&r == before);                  // same entry, however many rehashes
	CHECK(r == 123);
	CHECK(ap::find<node_t, ap::table::size_memo>(k) == before);
}

// --- GC (last: a sweep rebuilds every table in the process) -----------------------

TEST_CASE("gc: a live key keeps its row and pins the row's trefs; a dead key goes") {
	tref live = mk_atom("gc_live");
	tref dead = mk_atom("gc_dead");
	tref value = mk_atom("gc_value");        // not a subtree of either key

	ap::store<node_t, ap::table::atoms_memo>(live, ap::tref_set{ trefs{ value } });
	ap::store<node_t, ap::table::atoms_memo>(dead, ap::tref_set{ trefs{ value } });

	// A handle is what keeps a node reachable; `dead` and `value` have none,
	// so `value` survives only if the table's pre-sweep promotion runs.
	htref keep_live = tau::geth(live);
	std::unordered_set<tref> keep;
	tau::gc(keep);

	const auto* row = ap::find<node_t, ap::table::atoms_memo>(live);
	REQUIRE(row != nullptr);              // the key lived, so the row did
	CHECK(row->items.size() == 1);
	CHECK(row->items[0] == mk_atom("gc_value"));  // and the value tref is the same

	// The dead key's row is gone. Re-create the node before asking: the old
	// tref was freed by the sweep.
	CHECK(ap::find<node_t, ap::table::atoms_memo>(mk_atom("gc_dead")) == nullptr);
}

}
