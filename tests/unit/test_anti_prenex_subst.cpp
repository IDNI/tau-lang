// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Layer 0 unit tests for src/anti_prenex/foundations/subst.h (package C).
// Spec: anti_prenex.md §1 (atoms_memo), §3 (`[atm ↦ T/F]`), §4 (what may
// touch a unit), §10 (occurrence guards).
//
// `φ[x ← t]` is the library's `tree<node>::substitute` and is tested in
// tests/unit/test_tau_tree.cpp; the module's own call of it is in
// test_anti_prenex_layer0.cpp.
//
// Conventions. Every substitution takes the `wff` WRAPPER. A member of a chain
// that is not the last child of its operator node carries a right sibling, so
// it is a different `tref` from the same formula standing alone: pointer
// identity of an UNTOUCHED member is checked on the trimmed member, content
// on anything rebuilt (`tau::subtree_equals`). The parser canonicalises binder
// ids (`ex x ex y … all z` → 3, 2, 1).

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "anti_prenex/foundations/subst.h"

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

tref bf(const char* s) {
	tref t = tau::get(s, parse_bf());
	REQUIRE(t != nullptr);
	return t;
}
tref wff(const char* s) {
	tref t = tau::get(s, parse_wff());
	REQUIRE(t != nullptr);
	return t;
}
/// The trimmed `variable` node of a one-variable term — the shape `x`, order
/// keys and free-variable sets hold.
tref vr(const char* s) { return tau::trim(bf(s)); }

bool same(tref a, tref b) { return tau::subtree_equals(a, b); }

/// The first member of `chain` satisfying `pred`, TRIMMED; nullptr if none.
template <typename P>
tref member_where(tref chain, P pred) {
	for (tref m : ap::members<node_t>(chain))
		if (pred(m)) return tau::trim_right_sibling(m);
	return nullptr;
}

bool in_fv(tref n, tref v) {
	const trefs& vars = get_free_vars<node_t>(n);
	return std::binary_search(vars.begin(), vars.end(), v, tau::subtree_less);
}

} // namespace

TEST_SUITE("anti_prenex/foundations/subst") {

// --- the occurrence guard: untouched means the same tref ---------------------------

TEST_CASE("subst_atom: an atom that is not reachable leaves the formula untouched") {
	tref phi = wff("a = 0 && b = 0");
	CHECK(ap::subst_atom<node_t>(phi, wff("c = 0"), true) == phi);
	// Only inside a unit: not reachable (§4).
	tref only = wff("ex z (a = 0 && z = 0)");
	CHECK(ap::subst_atom<node_t>(only, wff("a = 0"), false) == only);
	// An x-free member of a rewritten chain keeps its tref too.
	tref psi = wff("a = 0 && (b = 0 || c = 0)");
	tref d = member_where(psi, is_child<node_t, tau::wff_or>);
	tref r = ap::subst_atom<node_t>(psi, wff("a = 0"), true);
	CHECK(r == d); // T ∧ d → d: the whole result IS the untouched member
}

// --- what may touch a unit (§4) ------------------------------------------------------

TEST_CASE("[atm ↦ T/F] treats a unit as an opaque leaf") {
	tref a = wff("a = 0");
	tref phi = wff("a = 0 && ex z (a = 0 && z = 0)");
	tref u = member_where(phi, is_child_quantifier<node_t>);
	REQUIRE(u != nullptr);
	tref s = ap::subst_atom<node_t>(phi, a, true);
	CHECK(s == u);                    // T ∧ unit → the unit, the SAME tref
	CHECK(!ap::has_atom<node_t>(u, a));
	CHECK(ap::atoms<node_t>(u).empty());
	// The same unit under `[x ← t]` IS entered — the one licensed rewrite,
	// and the library's business (test_tau_tree.cpp).
}

// --- the chain shape ------------------------------------------------------------------

TEST_CASE("[atm ↦ T/F] erases in place, so a canonical chain stays canonical") {
	// Members only vanish — the left fold of the remaining members,
	// whichever member goes. (A non-canonical input is not re-shaped.)
	tref abc = ap::canonical_and<node_t>({ wff("a = 0"), wff("b = 0"), wff("c = 0") });
	for (const char* gone : { "a = 0", "b = 0", "c = 0" }) {
		trefs rest;
		for (tref m : ap::members<node_t>(abc))
			if (!same(m, wff(gone))) rest.push_back(tau::trim_right_sibling(m));
		CHECK(same(ap::subst_atom<node_t>(abc, wff(gone), true),
			ap::canonical_and<node_t>(rest)));
	}
}

TEST_CASE("a temporal operator is opaque to [atm ↦ T/F]") {
	tref a = wff("x = 0");
	tref alw = tau::build_wff_always(a);
	REQUIRE(is_child_temporal_quantifier<node_t>(alw));
	REQUIRE(in_fv(alw, vr("x")));
	CHECK(ap::atoms<node_t>(alw).empty());
	tref psi = tau::build_wff_and(alw, wff("c = 0"));
	CHECK(ap::subst_atom<node_t>(psi, a, true) == psi);
	CHECK(ap::subst_atom<node_t>(psi, wff("c = 0"), true) == alw);
}

// --- [atm ↦ T/F] ----------------------------------------------------------------------

TEST_CASE("subst_atom: every reachable occurrence, the complement under ¬, the hooks fold") {
	tref a = wff("a = 0"), b = wff("b = 0"), c = wff("c = 0");
	CHECK(same(ap::subst_atom<node_t>(wff("a = 0 && b = 0"), a, true), b));
	CHECK(tau::get(ap::subst_atom<node_t>(wff("a = 0 && b = 0"), a, false)).equals_F());
	CHECK(tau::get(ap::subst_atom<node_t>(wff("a = 0 || b = 0"), a, true)).equals_T());
	CHECK(same(ap::subst_atom<node_t>(wff("a = 0 || b = 0"), a, false), b));
	CHECK(tau::get(ap::subst_atom<node_t>(wff("!(a = 0) && b = 0"), a, true)).equals_F());
	CHECK(same(ap::subst_atom<node_t>(wff("!(a = 0) && b = 0"), a, false), b));
	CHECK(tau::get(ap::subst_atom<node_t>(wff("!(a = 0) || b = 0"), a, false)).equals_T());
	// Both polarities at once, shared across two disjuncts.
	tref phi = wff("(a = 0 && b = 0) || (!(a = 0) && c = 0)");
	CHECK(same(ap::subst_atom<node_t>(phi, a, true), b));
	CHECK(same(ap::subst_atom<node_t>(phi, a, false), c));
	// The root itself, plain and negated.
	CHECK(tau::get(ap::subst_atom<node_t>(a, a, true)).equals_T());
	CHECK(tau::get(ap::subst_atom<node_t>(wff("!(a = 0)"), a, true)).equals_F());
	// The atom as it occurs: a semantically equal spelling is another atom.
	tref spelled = wff("a' = 1 && b = 0");
	CHECK(ap::subst_atom<node_t>(spelled, a, true) == spelled);
}

// --- atoms_memo ---------------------------------------------------------------------

TEST_CASE("atoms: the vocabulary with subst_atom's reach, sorted, one row per node") {
	tref a = wff("a = 0"), b = wff("b = 0"), c = wff("c = 0");
	tref phi = wff("a = 0 && !(b = 0) || c = 0");
	const trefs& as = ap::atoms<node_t>(phi);
	CHECK(as.size() == 3);
	CHECK(std::is_sorted(as.begin(), as.end(), tau::subtree_less));
	for (tref t : { a, b, c }) CHECK(ap::has_atom<node_t>(phi, t));
	CHECK(!ap::has_atom<node_t>(phi, wff("d = 0")));
	CHECK(&ap::atoms<node_t>(phi) == &as);          // one row, never recomputed
	// The reach: ¬ is passed; a unit, a reference, a temporal operator and a
	// constant are opaque.
	REQUIRE(ap::atoms<node_t>(wff("!(a = 0)")).size() == 1);
	CHECK(same(ap::atoms<node_t>(wff("!(a = 0)"))[0], a));
	CHECK(ap::atoms<node_t>(wff("ex z (a = 0 && z = 0)")).empty());
	CHECK(ap::atoms<node_t>(wff("f(x)")).empty());
	CHECK(ap::atoms<node_t>(tau::build_wff_sometimes(a)).empty());
	CHECK(ap::atoms<node_t>(tau::_T()).empty());
	CHECK(ap::atoms<node_t>(tau::_F()).empty());
	CHECK(ap::atoms<node_t>(wff("a = 0 && ex z (b = 0)")).size() == 1);
	// Duplicates collapse; a member carrying a right sibling is found.
	tref dup = wff("a = 0 && (a = 0 || b = 0)");
	CHECK(ap::atoms<node_t>(dup).size() == 2);
	CHECK(ap::has_atom<node_t>(dup, ap::members<node_t>(dup)[0]));
	// atoms vs fv on a unit: its variables are free, its atoms unreachable.
	tref u = wff("a = 0 && ex z (x & z = 0)");
	CHECK(ap::atoms<node_t>(u).size() == 1);
	CHECK(in_fv(u, vr("x")));
}

TEST_CASE("atoms: no row on a spine node, a row on every other wrapper") {
	// Fresh names: the table is process-wide and hash-consing would hand a
	// row from another case to the same node.
	tref s1 = wff("s1 = 0"), s2 = wff("s2 = 0"), s3 = wff("s3 = 0");
	tref inner = tau::build_wff_and(s2, s3);
	tref phi = tau::build_wff_and(s1, inner);     // right-nested: `inner` is a spine node
	REQUIRE(ap::find<node_t, ap::table::atoms_memo>(inner) == nullptr);
	CHECK(ap::atoms<node_t>(phi).size() == 3);
	for (tref m : ap::members<node_t>(phi))
		CHECK(ap::find<node_t, ap::table::atoms_memo>(m) != nullptr);
	CHECK(ap::find<node_t, ap::table::atoms_memo>(phi) != nullptr);
	CHECK(ap::find<node_t, ap::table::atoms_memo>(inner) == nullptr);
	// Asked about directly it is a root and gets its row.
	CHECK(ap::atoms<node_t>(inner).size() == 2);
	CHECK(ap::find<node_t, ap::table::atoms_memo>(inner) != nullptr);
	// A spine node that has a row is not entered again, and its members are
	// still what a chain top above it reads.
	tref s0 = wff("s0 = 0");
	CHECK(ap::atoms<node_t>(tau::build_wff_and(s0, inner)).size() == 3);
}

// --- GC (last: a sweep rebuilds every table in the process) ----------------------------

TEST_CASE("gc: an atoms row survives a sweep with a live key, its trimmed atoms pinned") {
	tref phi = wff("g1 = 0 && g2 = 0");
	const trefs& as = ap::atoms<node_t>(phi);
	REQUIRE(as.size() == 2);
	// The row holds TRIMMED atoms; the first member carries its sibling in
	// `phi`, so its trimmed twin is NOT a subtree of the key and lives only
	// through the row's for_each_tref.
	tref a0 = as[0], a1 = as[1];
	htref keep_phi = tau::geth(phi);
	std::unordered_set<tref> keep;
	tau::gc(keep);
	const auto* row = ap::find<node_t, ap::table::atoms_memo>(phi);
	REQUIRE(row != nullptr);
	REQUIRE(row->items.size() == 2);
	CHECK(row->items[0] == a0);
	CHECK(row->items[1] == a1);
	CHECK(ap::has_atom<node_t>(phi, wff("g1 = 0")));
	CHECK(ap::has_atom<node_t>(phi, wff("g2 = 0")));
}

} // TEST_SUITE
