// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for src/anti_prenex/eliminate/finite.h: §7 "Finite (pure
// Boolean)" — the method's `opaque?`, SUBSUME_NEGATIVES, the closing of
// FINITE_QUERY, FINITE_TIER2 and ELIMINATE_FINITE_CLAUSE through its tiers.
// Spec: anti_prenex.md §7, §1 (`subsume_max`, the negative tree, the term
// representation).
//
// Every case calls the method DIRECTLY on a parsed `∃P.ψ` over a bitvector
// type, split the way ELIMINATE_BLOCK hands a clause over: the block read
// off the binder chain, the component's ctx over it with the type read off
// the block, and the body prepared (`prepare_terms`) and taken as its
// conjunct list. Every variable is typed where it occurs. The ring sum is
// `^`: Tau parses `+` as arithmetic, which F1 uses on purpose. A negative
// literal is spelled `!(l = r)`, the one negative shape invariant 4 leaves.
//
// THE ORACLE is the solver: `bv_equivalent(a, b)` holds when the
// exclusive-or of the two FINISHED formulas is unsatisfiable, its free
// variables read existentially. A reference and a chain are beyond it:
// those claims are structural, and the keep-mode answer is checked through
// its plain re-run, the chains resolved by RESOLVE_FUNCTIONAL_PLAIN.
//
// The observables are the answers, `binder_count`, the delta of
// `taint_count` — global and never reset — and `same` on the shapes.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "anti_prenex/eliminate/finite.h"

#include <algorithm>
#include <utility>

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

using th = term_handle<node_t>;

tref parse(const char* sample) {
	auto rr = get_nso_rr(sample);
	REQUIRE(rr.has_value());
	return rr.value().main->get();
}

bool same(tref a, tref b) { return tau::subtree_equals(a, b); }

/// Is there a node of this kind anywhere in `n`?
bool holds(tref n, size_t nt) {
	return tau::get(n).find_top([nt](tref m) {
		return tree<node_t>::get(m).is(nt); }) != nullptr;
}

/// Every formula binder of `n`, nested ones included.
size_t binder_count(tref n) {
	return tau::get(n).select_all(is_child_quantifier<node_t>).size();
}

/// The formula with every stored BDD spelled out, which is what the oracle
/// reads.
tref finished(tref n) { return th::convert_to_tau_terms(n); }

/// NO FREE VARIABLE ESCAPED: `FV(out) ⊆ FV(in)`.
bool no_escape(tref out, tref in) {
	const trefs& before = get_free_vars<node_t>(in);
	for (tref v : get_free_vars<node_t>(out))
		if (!std::binary_search(before.begin(), before.end(), v,
			tau::subtree_less)) return false;
	return true;
}

/// INVARIANT 4 over a whole formula: no `bf_neq` and no negated or mirrored
/// order operator anywhere, and every `¬` sits directly over an ATOM.
bool invariant_4(tref n) {
	bool clean = true;
	auto check = [&clean](tref m) {
		const auto& t = tau::get(m);
		if (t.is(tau::bf_neq) || t.is(tau::bf_nlt) || t.is(tau::bf_nlteq)
			|| t.is(tau::bf_gt) || t.is(tau::bf_gteq)
			|| t.is(tau::bf_ngt) || t.is(tau::bf_ngteq))
				return clean = false;
		if (t.child_is(tau::wff_neg)
			&& !is_atomic_fm<node_t>(
				tau::trim_right_sibling(t[0].first())))
				return clean = false;
		return true;
	};
	auto all_of_it = [](tref) { return true; };
	auto up = [](tref) {};
	pre_order<node_t>(n).search_unique(check, all_of_it, up);
	return clean;
}

/// THE ORACLE: `a` and `b` agree on every assignment of their free
/// variables, asked as the unsatisfiability of their exclusive-or.
bool bv_equivalent(tref a, tref b) {
	const tref A = finished(a), B = finished(b);
	const tref differ = tau::build_wff_or(
		tau::build_wff_and(A, tau::build_wff_neg(B)),
		tau::build_wff_and(tau::build_wff_neg(A), B));
	return bv_formula_sat_status<node_t>(differ) == bv_sat_status::unsat;
}

/// A parsed `∃P.ψ` split the way ELIMINATE_BLOCK hands it over: the block
/// OUTERMOST FIRST, the component's ctx over it, and `ψ` prepared and taken
/// as its conjunct list — an ∨-node is one conjunct.
struct fixture {
	tref quantified = nullptr;   ///< `∃P.ψ` as parsed, the oracle's reference
	ap::block P;
	ap::ctx<node_t> c;
	trefs conjuncts;
};

fixture make(const char* src, bool keep = false) {
	fixture f;
	f.quantified = parse(src);
	tref n = f.quantified;
	while (is_child_quantifier<node_t>(n)) {
		f.P.push_back(tau::trim_right_sibling(
			ap::binder_var<node_t>(n)));
		n = tau::trim_right_sibling(ap::binder_body<node_t>(n));
	}
	REQUIRE(!f.P.empty());
	f.c = ap::ctx<node_t>::for_component(f.P,
		find_ba_type<node_t>(f.P[0]), keep);
	f.conjuncts = get_cnf_wff_clauses<node_t>(
		ap::prepare_terms<node_t>(n));
	return f;
}

/// What every eliminated clause the oracle can read claims: equivalent to
/// `∃P.ψ`, no free variable escaped, invariant 4.
void check_against_source(tref got, const fixture& f) {
	CHECK(bv_equivalent(got, f.quantified));
	CHECK(no_escape(finished(got), f.quantified));
	CHECK(invariant_4(got));
}

/// The binder member and the other member of a two-member conjunction or
/// disjunction.
std::pair<tref, tref> unit_and_rest(tref n) {
	const trefs ms = ap::members<node_t>(n);
	REQUIRE(ms.size() == 2);
	const bool first = is_child_quantifier<node_t>(ms[0]);
	return first ? std::pair{ ms[0], ms[1] } : std::pair{ ms[1], ms[0] };
}

/// `subsume_max` for the scope of one case, restored on the way out. A ctx
/// copies the knob at setup, so the guard precedes `make`.
struct subsume_max_guard {
	const size_t saved = ap::subsume_max;
	explicit subsume_max_guard(size_t v) { ap::subsume_max = v; }
	~subsume_max_guard() { ap::subsume_max = saved; }
};

/// `x·a = 0 ∧ x·b ≠ 0 ∧ x·a′·b ≠ 0`: modulo `f = x·a` the two negatives are
/// EQUIVALENT, since `f′·x·b = (x′ ∪ a′)·x·b = x·a′·b`.
const char* const singles =
	"ex x (x:bv[2] & a:bv[2] = { 0 }:bv[2] "
	"&& !(x:bv[2] & b:bv[2] = { 0 }:bv[2]) "
	"&& !(x:bv[2] & a:bv[2]' & b:bv[2] = { 0 }:bv[2])).";

/// `x·a = 0 ∧ x·b ≠ 0`.
const char* const tier1 =
	"ex x (x:bv[2] & a:bv[2] = { 0 }:bv[2] "
	"&& !(x:bv[2] & b:bv[2] = { 0 }:bv[2])).";

/// `x·a ≠ 0 ∧ ∀y (y·x = 0 ∨ y = 0)`: the unit forces `x = 0` (take
/// `y = 1`), so the clause is unsatisfiable.
const char* const swallowed =
	"ex x (!(x:bv[2] & a:bv[2] = { 0 }:bv[2]) "
	"&& all y (y:bv[2] & x:bv[2] = { 0 }:bv[2] "
	"|| y:bv[2] = { 0 }:bv[2])).";

} // namespace

TEST_SUITE("anti_prenex/eliminate/finite") {

// --- opaque? ----------------------------------------------------------------------

TEST_CASE("F1: what the sweep cannot read freezes its component") {
	SUBCASE("an order atom, beside a component decided F") {
		// `y ≤ b` freezes the `y` component; the `x` one is R13's
		// clause over bv[1], whose query is `F` — whatever is frozen.
		fixture f = make("ex x ex y (!(x:bv[1] = { 0 }:bv[1]) "
			"&& !(x:bv[1] = { 1 }:bv[1]) && y:bv[1] <= b:bv[1]).");
		const tref got = ap::eliminate_finite_clause<node_t>(
			f.conjuncts, f.P, f.c);
		CHECK(tau::get(got).equals_F());
		check_against_source(got, f);
	}
	SUBCASE("an arithmetic atom, beside a component decided at tier 0") {
		// `x + a = 0` freezes; `∀_y (y ∪ b) = b` leaves `b = 0` beside
		// it.
		fixture f = make("ex x ex y (x:bv[2] + a:bv[2] = { 0 }:bv[2] "
			"&& y:bv[2] | b:bv[2] = { 0 }:bv[2]).");
		const tref got = ap::eliminate_finite_clause<node_t>(
			f.conjuncts, f.P, f.c);
		check_against_source(got, f);
		REQUIRE(binder_count(got) == 1);
		const auto [unit, rest] = unit_and_rest(got);
		CHECK(same(ap::binder_var<node_t>(unit), f.P[0]));
		CHECK(!ap::fv_meets<node_t>(rest, f.P));
	}
	SUBCASE("a reference, structurally") {
		fixture f = make("ex x ex y (q(x:bv[2]) "
			"&& x:bv[2] & a:bv[2] = { 0 }:bv[2] "
			"&& y:bv[2] | b:bv[2] = { 0 }:bv[2]).");
		const tref got = ap::eliminate_finite_clause<node_t>(
			f.conjuncts, f.P, f.c);
		CHECK(invariant_4(got));
		CHECK(no_escape(finished(got), f.quantified));
		// ONE binder, over `x`, around the frozen component.
		REQUIRE(binder_count(got) == 1);
		const auto [unit, rest] = unit_and_rest(got);
		REQUIRE(is_child<node_t>(unit, tau::wff_ex));
		CHECK(same(ap::binder_var<node_t>(unit), f.P[0]));
		CHECK(ap::members<node_t>(ap::binder_body<node_t>(unit)).size()
			== 2);
		CHECK(!ap::fv_meets<node_t>(rest, f.P));
	}
	SUBCASE("a unit over a pure body is swallowed, not frozen") {
		// F10 runs this clause end to end.
		fixture f = make(swallowed);
		const auto unit = std::ranges::find_if(f.conjuncts,
			is_child_quantifier<node_t>);
		REQUIRE(unit != f.conjuncts.end());
		CHECK(!ap::finite_opaque<node_t>(*unit, f.P, f.c.type));
	}
}

// --- tiers 0 and 1 ----------------------------------------------------------------

TEST_CASE("F2: positives alone are tier 0: pos is the answer") {
	// `∀_x (x ∪ a) = a`, so `pos` is the one atom `a = 0`.
	fixture f = make("ex x (x:bv[2] | a:bv[2] = { 0 }:bv[2]).");
	const tref got = ap::eliminate_finite_clause<node_t>(f.conjuncts, f.P,
		f.c);
	check_against_source(got, f);
	CHECK(binder_count(got) == 0);
	CHECK(is_child<node_t>(got, tau::bf_eq));
	CHECK(bv_equivalent(got, parse("a:bv[2] = { 0 }:bv[2].")));
	// The oracle tells a different atom apart.
	CHECK(!bv_equivalent(got, parse("a:bv[2] = { 1 }:bv[2].")));
}

TEST_CASE("F3: one negative literal is tier 1, answered by its condition") {
	// `pos = ∀_x x·a = 0` is `T`; the condition `∃_x (x·a)′·x·b ≠ 0` is
	// `a′·b ≠ 0`.
	fixture f = make(tier1);
	const tref got = ap::eliminate_finite_clause<node_t>(f.conjuncts, f.P,
		f.c);
	check_against_source(got, f);
	CHECK(binder_count(got) == 0);
	CHECK(ap::is_negated_equation<node_t>(got));
	CHECK(bv_equivalent(got,
		parse("!(a:bv[2]' & b:bv[2] = { 0 }:bv[2]).")));
}

TEST_CASE("F4: one flat tree is tier 1, one condition per literal") {
	// `∃X` distributes over the tree's `∨`: one condition per literal.
	fixture f = make("ex x (x:bv[2] & a:bv[2] = { 0 }:bv[2] "
		"&& (!(x:bv[2] & b:bv[2] = { 0 }:bv[2]) "
		"|| !(x:bv[2] & c:bv[2] = { 0 }:bv[2]))).");
	const tref got = ap::eliminate_finite_clause<node_t>(f.conjuncts, f.P,
		f.c);
	check_against_source(got, f);
	CHECK(binder_count(got) == 0);
	CHECK(ap::is_negative_tree<node_t>(got));
	CHECK(bv_equivalent(got, parse("!(a:bv[2]' & b:bv[2] = { 0 }:bv[2]) "
		"|| !(a:bv[2]' & c:bv[2] = { 0 }:bv[2]).")));
}

// --- SUBSUME_NEGATIVES ------------------------------------------------------------

TEST_CASE("F5: subsumption, rule by rule") {
	SUBCASE("singles: the larger goes, of an equivalent pair the first") {
		// Modulo `f = x·a` the two terms are equivalent (see
		// `singles`): `x·a′·b ⊑ x·b` drops `x·b` when `x·a′·b` comes
		// first, and the reverse test drops `x·a′·b` when `x·b` does.
		fixture f = make(singles);
		const auto s = ap::squeeze<node_t>(f.conjuncts, f.P);
		REQUIRE(s.conjuncts.size() == 3);
		REQUIRE(s.negatives.size() == 2);
		const tref pos = *std::ranges::find_if(s.conjuncts, [](tref m) {
			return is_child<node_t>(m, tau::bf_eq); });
		// `x·a′·b` is the negative over three variables.
		const bool first_small =
			get_free_vars<node_t>(s.negatives[0]).size() == 3;
		const tref small = s.negatives[first_small ? 0 : 1];
		const tref large = s.negatives[first_small ? 1 : 0];
		for (const auto& [clause, survivor] : {
			std::pair{ trefs{ pos, small, large }, small },
			std::pair{ trefs{ pos, large, small }, large } })
		{
			const auto sb = ap::subsume_negatives<node_t>(s.f,
				clause, f.P, f.c);
			REQUIRE(sb.negatives.size() == 1);
			CHECK(same(sb.negatives[0], survivor));
			CHECK(sb.conjuncts.size() == 2);
		}
	}
	SUBCASE("a flat tree keeps its first member, which is a single then") {
		// The same two terms as members: equivalent modulo `f`, so
		// the second one goes, and the tree reduced to one literal
		// takes its place in the clause.
		fixture f = make("ex x (x:bv[2] & a:bv[2] = { 0 }:bv[2] "
			"&& (!(x:bv[2] & b:bv[2] = { 0 }:bv[2]) "
			"|| !(x:bv[2] & a:bv[2]' & b:bv[2] = { 0 }:bv[2]))).");
		const auto s = ap::squeeze<node_t>(f.conjuncts, f.P);
		REQUIRE(s.negatives.size() == 1);
		const tref t = s.negatives[0];
		REQUIRE(ap::is_flat_tree<node_t>(t));
		const trefs ms = ap::members<node_t>(t);
		REQUIRE(ms.size() == 2);
		const size_t at = std::ranges::find(s.conjuncts, t)
			- s.conjuncts.begin();
		const auto sb = ap::subsume_negatives<node_t>(s.f, s.conjuncts,
			f.P, f.c);
		REQUIRE(sb.negatives.size() == 1);
		CHECK(ap::is_negated_equation<node_t>(sb.negatives[0]));
		CHECK(same(sb.negatives[0], ms[0]));
		REQUIRE(sb.conjuncts.size() == 2);
		CHECK(same(sb.conjuncts[at], ms[0]));
	}
	SUBCASE("a single below a tree member drops the tree; X narrows") {
		// `x·b·c ≤ x·b`: the single implies the tree. `y` sat in the
		// dropped tree alone, so it leaves the block.
		fixture f = make("ex x ex y (!(x:bv[2] & b:bv[2] & c:bv[2] "
			"= { 0 }:bv[2]) && (!(x:bv[2] & b:bv[2] = { 0 }:bv[2]) "
			"|| !(y:bv[2] & d:bv[2] = { 0 }:bv[2]))).");
		const auto s = ap::squeeze<node_t>(f.conjuncts, f.P);
		REQUIRE(s.negatives.size() == 2);
		const tref single = *std::ranges::find_if(s.negatives,
			ap::is_negated_equation<node_t>);
		const auto sb = ap::subsume_negatives<node_t>(s.f, s.conjuncts,
			f.P, f.c);
		REQUIRE(sb.negatives.size() == 1);
		CHECK(same(sb.negatives[0], single));
		CHECK(sb.conjuncts.size() == 1);
		REQUIRE(sb.X.size() == 1);
		CHECK(same(sb.X[0], f.P[0]));
	}
	SUBCASE("a single above every tree member goes") {
		// `x·b·c ≤ x·b` and `x·b·d ≤ x·b`: the tree implies the single.
		fixture f = make("ex x (!(x:bv[2] & b:bv[2] = { 0 }:bv[2]) "
			"&& (!(x:bv[2] & b:bv[2] & c:bv[2] = { 0 }:bv[2]) "
			"|| !(x:bv[2] & b:bv[2] & d:bv[2] = { 0 }:bv[2]))).");
		const auto s = ap::squeeze<node_t>(f.conjuncts, f.P);
		REQUIRE(s.negatives.size() == 2);
		const tref t = *std::ranges::find_if(s.negatives,
			ap::is_negative_tree<node_t>);
		const auto sb = ap::subsume_negatives<node_t>(s.f, s.conjuncts,
			f.P, f.c);
		REQUIRE(sb.negatives.size() == 1);
		CHECK(same(sb.negatives[0], t));
		CHECK(sb.conjuncts.size() == 1);
	}
	SUBCASE("a tree below another tree drops the other") {
		// Every member of `G₁ = (x·b·c ∨ x·d·e)` is below a member of
		// `G₂ = (x·b ∨ x·d)`: `G₁` implies `G₂`, which goes. `G₁` comes
		// first, the survivor the rule tests against.
		fixture f = make("ex x ((!(x:bv[2] & b:bv[2] & c:bv[2] "
			"= { 0 }:bv[2]) || !(x:bv[2] & d:bv[2] & e:bv[2] "
			"= { 0 }:bv[2])) "
			"&& (!(x:bv[2] & b:bv[2] = { 0 }:bv[2]) "
			"|| !(x:bv[2] & d:bv[2] = { 0 }:bv[2]))).");
		const auto s = ap::squeeze<node_t>(f.conjuncts, f.P);
		REQUIRE(s.negatives.size() == 2);
		const bool first_g1 =
			get_free_vars<node_t>(s.negatives[0]).size() == 5;
		const tref g1 = s.negatives[first_g1 ? 0 : 1];
		const tref g2 = s.negatives[first_g1 ? 1 : 0];
		const auto sb = ap::subsume_negatives<node_t>(s.f,
			trefs{ g1, g2 }, f.P, f.c);
		REQUIRE(sb.negatives.size() == 1);
		CHECK(same(sb.negatives[0], g1));
	}
	SUBCASE("above the gate nothing is scanned") {
		subsume_max_guard g(0);
		fixture f = make(singles);
		const auto s = ap::squeeze<node_t>(f.conjuncts, f.P);
		const auto sb = ap::subsume_negatives<node_t>(s.f, s.conjuncts,
			f.P, f.c);
		CHECK(sb.negatives.size() == 2);
		REQUIRE(sb.conjuncts.size() == s.conjuncts.size());
		for (size_t i = 0; i < s.conjuncts.size(); ++i)
			CHECK(same(sb.conjuncts[i], s.conjuncts[i]));
	}
}

TEST_CASE("F5b: subsumption end to end") {
	SUBCASE("one negative is left: tier 1") {
		// Either survivor gives the same condition, `a′·b ≠ 0`.
		fixture f = make(singles);
		const tref got = ap::eliminate_finite_clause<node_t>(
			f.conjuncts, f.P, f.c);
		check_against_source(got, f);
		CHECK(binder_count(got) == 0);
		CHECK(ap::is_negated_equation<node_t>(got));
	}
	SUBCASE("under the gate both stay: tier 2, open and undecided") {
		// Not valid (`a = 1` forces `x = 0`), satisfiable (`a = 0`,
		// `b = x = 1`): the query re-wraps.
		subsume_max_guard g(0);
		fixture f = make(singles);
		const tref got = ap::eliminate_finite_clause<node_t>(
			f.conjuncts, f.P, f.c);
		check_against_source(got, f);
		CHECK(binder_count(got) == 1);
	}
}

// --- the middle tier --------------------------------------------------------------

TEST_CASE("F6: the middle tier") {
	SUBCASE("the root literal decided exactly, the rest one unit") {
		// `pos = ∀_x x·d = 0` is `T`, the root literal's condition is
		// `a·d′ ≠ 0`, and `∃x(x·d = 0 ∧ x·b ≠ 0 ∧ x·c ≠ 0)` is neither
		// valid (`b = 0`) nor unsatisfiable (`d = 0`, `b = c = x = 1`).
		fixture f = make("ex x (x:bv[2] & d:bv[2] = { 0 }:bv[2] "
			"&& (!(x:bv[2] & a:bv[2] = { 0 }:bv[2]) "
			"|| (!(x:bv[2] & b:bv[2] = { 0 }:bv[2]) "
			"&& !(x:bv[2] & c:bv[2] = { 0 }:bv[2])))).");
		const tref got = ap::eliminate_finite_clause<node_t>(
			f.conjuncts, f.P, f.c);
		check_against_source(got, f);
		REQUIRE(binder_count(got) == 1);
		REQUIRE(is_child<node_t>(got, tau::wff_or));
		const auto [unit, literal] = unit_and_rest(got);
		CHECK(is_child<node_t>(unit, tau::wff_ex));
		CHECK(bv_equivalent(literal,
			parse("!(a:bv[2] & d:bv[2]' = { 0 }:bv[2]).")));
	}
	SUBCASE("a root literal whose condition is T leaves no unit") {
		// `∃_x (x·d)′·x′ = ∃_x x′ = 1`: `x = 0` satisfies the clause.
		fixture f = make("ex x (x:bv[2] & d:bv[2] = { 0 }:bv[2] "
			"&& (!(x:bv[2]' = { 0 }:bv[2]) "
			"|| (!(x:bv[2] & b:bv[2] = { 0 }:bv[2]) "
			"&& !(x:bv[2] & c:bv[2] = { 0 }:bv[2])))).");
		const tref got = ap::eliminate_finite_clause<node_t>(
			f.conjuncts, f.P, f.c);
		CHECK(tau::get(got).equals_T());
		check_against_source(got, f);
	}
}

// --- tier 2 -----------------------------------------------------------------------

TEST_CASE("F7: tier 2, closed queries") {
	const size_t before = ap::taint_count<node_t>();
	SUBCASE("R13's clause over bv[1] is F") {
		fixture f = make("ex x (!(x:bv[1] = { 0 }:bv[1]) "
			"&& !(x:bv[1] = { 1 }:bv[1])).");
		const tref got = ap::eliminate_finite_clause<node_t>(
			f.conjuncts, f.P, f.c);
		CHECK(tau::get(got).equals_F());
		check_against_source(got, f);
	}
	SUBCASE("over bv[2] the elements 1 and 2 are left: T") {
		fixture f = make("ex x (!(x:bv[2] = { 0 }:bv[2]) "
			"&& !(x:bv[2] = { 3 }:bv[2])).");
		const tref got = ap::eliminate_finite_clause<node_t>(
			f.conjuncts, f.P, f.c);
		CHECK(tau::get(got).equals_T());
		check_against_source(got, f);
	}
	// The sweep decides both: no budget bound.
	CHECK(ap::taint_count<node_t>() == before);
}

TEST_CASE("F8: tier 2, open queries closed both ways") {
	const size_t before = ap::taint_count<node_t>();
	SUBCASE("valid: T") {
		// Four elements, at most two excluded.
		fixture f = make("ex x (!(x:bv[2] = { 0 }:bv[2]) "
			"&& !(x:bv[2] = a:bv[2])).");
		const tref got = ap::eliminate_finite_clause<node_t>(
			f.conjuncts, f.P, f.c);
		CHECK(tau::get(got).equals_T());
		check_against_source(got, f);
	}
	SUBCASE("unsatisfiable: F") {
		// Over bv[1], `a` or `a′` is `0`.
		fixture f = make("ex x (!(x:bv[1] & a:bv[1] = { 0 }:bv[1]) "
			"&& !(x:bv[1] & a:bv[1]' = { 0 }:bv[1])).");
		const tref got = ap::eliminate_finite_clause<node_t>(
			f.conjuncts, f.P, f.c);
		CHECK(tau::get(got).equals_F());
		check_against_source(got, f);
	}
	SUBCASE("neither: the exact re-wrap of the prepared clause") {
		// Not valid (`a = 0`), satisfiable (`a = b = x = 1`).
		fixture f = make("ex x (!(x:bv[2] & a:bv[2] = { 0 }:bv[2]) "
			"&& !(x:bv[2] & b:bv[2] = { 0 }:bv[2])).");
		const tref got = ap::eliminate_finite_clause<node_t>(
			f.conjuncts, f.P, f.c);
		CHECK(same(got, ap::rewrap<node_t>(
			ap::simplified_and_join<node_t>(f.conjuncts), f.P)));
		CHECK(binder_count(got) == 1);
		check_against_source(got, f);
	}
	CHECK(ap::taint_count<node_t>() == before);
}

TEST_CASE("F9: one query per part, then the joint closing") {
	SUBCASE("parts sharing no free variable stay two units") {
		// Each part needs its free variable mixed, neither `0` nor `1`;
		// `a` and `b` are independent, so no joint closing is asked.
		fixture f = make("ex x ex y ("
			"!(x:bv[2] & a:bv[2] = { 0 }:bv[2]) "
			"&& !(x:bv[2] & a:bv[2]' = { 0 }:bv[2]) "
			"&& !(y:bv[2] & b:bv[2] = { 0 }:bv[2]) "
			"&& !(y:bv[2] & b:bv[2]' = { 0 }:bv[2])).");
		const tref got = ap::eliminate_finite_clause<node_t>(
			f.conjuncts, f.P, f.c);
		CHECK(binder_count(got) == 2);
		check_against_source(got, f);
	}
	SUBCASE("parts sharing a free variable meet in the joint closing") {
		// The `x` part needs `a` mixed; the `y` part needs `y = 1` with
		// `y ≤ a`, so `a = 1`. Each is open on its own, and together
		// they are unsatisfiable.
		const size_t before = ap::taint_count<node_t>();
		fixture fx = make("ex x (!(x:bv[2] & a:bv[2] = { 0 }:bv[2]) "
			"&& !(x:bv[2] & a:bv[2]' = { 0 }:bv[2])).");
		CHECK(binder_count(ap::eliminate_finite_clause<node_t>(
			fx.conjuncts, fx.P, fx.c)) == 1);
		fixture fy = make("ex y (y:bv[2] & a:bv[2]' = { 0 }:bv[2] "
			"&& !(y:bv[2] & { 1 }:bv[2] = { 0 }:bv[2]) "
			"&& !(y:bv[2] & { 2 }:bv[2] = { 0 }:bv[2])).");
		CHECK(binder_count(ap::eliminate_finite_clause<node_t>(
			fy.conjuncts, fy.P, fy.c)) == 1);
		fixture f = make("ex x ex y ("
			"!(x:bv[2] & a:bv[2] = { 0 }:bv[2]) "
			"&& !(x:bv[2] & a:bv[2]' = { 0 }:bv[2]) "
			"&& y:bv[2] & a:bv[2]' = { 0 }:bv[2] "
			"&& !(y:bv[2] & { 1 }:bv[2] = { 0 }:bv[2]) "
			"&& !(y:bv[2] & { 2 }:bv[2] = { 0 }:bv[2])).");
		const tref got = ap::eliminate_finite_clause<node_t>(
			f.conjuncts, f.P, f.c);
		CHECK(tau::get(got).equals_F());
		check_against_source(got, f);
		CHECK(ap::taint_count<node_t>() == before);
	}
}

TEST_CASE("F10: a swallowed unit enters the query whole") {
	// See `swallowed`: the open query is unsatisfiable.
	const size_t before = ap::taint_count<node_t>();
	fixture f = make(swallowed);
	const tref got = ap::eliminate_finite_clause<node_t>(f.conjuncts, f.P,
		f.c);
	CHECK(tau::get(got).equals_F());
	check_against_source(got, f);
	CHECK(ap::taint_count<node_t>() == before);
}

TEST_CASE("F12: a negative the squeeze frees of the block is a part of its own") {
	// `x^b = x^c` squeezes to `b^c = 0`: a tier-2 part over no variable,
	// its query the conjunct itself, open and undecided, so it stays. The
	// `x` part needs `a` mixed and re-wraps; the two share no variable.
	fixture f = make("ex x (!(x:bv[2] ^ b:bv[2] = x:bv[2] ^ c:bv[2]) "
		"&& !(x:bv[2] & a:bv[2] = { 0 }:bv[2]) "
		"&& !(x:bv[2] & a:bv[2]' = { 0 }:bv[2])).");
	for (tref m : f.conjuncts) REQUIRE(ap::fv_meets<node_t>(m, f.P));
	const tref got = ap::eliminate_finite_clause<node_t>(f.conjuncts, f.P,
		f.c);
	check_against_source(got, f);
	REQUIRE(binder_count(got) == 1);
	const auto [unit, rest] = unit_and_rest(got);
	CHECK(same(ap::binder_var<node_t>(unit), f.P[0]));
	CHECK(ap::is_negated_equation<node_t>(rest));
	CHECK(!ap::fv_meets<node_t>(rest, f.P));
	CHECK(bv_equivalent(rest,
		parse("!(b:bv[2] ^ c:bv[2] = { 0 }:bv[2]).")));
}

// --- keep mode --------------------------------------------------------------------

TEST_CASE("F11: keep mode decides F3's conditions at their emission") {
	// `DISCHARGE` asks the callback once on `∀_x` of the positive and on
	// `∃_x` of the condition, as built. Each yes keeps its chain SLID to
	// x's levels, where every piece is closed and folds (§1): no chain and
	// no formula binder stand in the answer.
	fixture f = make(tier1, true);
	size_t chains = 0;
	const ap::keep_functional_fn<node_t> keep_all = [&chains](tref n) {
		if (!ap::strip_chain<node_t>(n).first.empty()) ++chains;
		return true;
	};
	f.c.kf = keep_all;
	const tref got = ap::eliminate_finite_clause<node_t>(f.conjuncts, f.P,
		f.c);
	CHECK(chains > 0);
	CHECK(!holds(got, tau::bf_fall));
	CHECK(!holds(got, tau::bf_fex));
	CHECK(binder_count(got) == 0);
	check_against_source(got, f);
}

} // TEST_SUITE
