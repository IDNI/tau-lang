// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for src/anti_prenex/eliminate/bitvector.h: §7 "Bitvector — the
// router" — its `opaque?`, SPLIT_ARITHMETIC, and ELIMINATE_BITVECTOR_CLAUSE
// with SOLVE_ARITHMETIC and the joint ask. Spec: anti_prenex.md §7.
//
// Every case calls the router DIRECTLY on a `∃P.ψ` over a bitvector type,
// split the way ELIMINATE_BLOCK hands a clause over: the block read off the
// binder chain, the component's ctx over it with the type read off the
// block, and the body prepared (`prepare_terms`) and taken as its conjunct
// list. Every variable is typed where it occurs. The ring sum is `^`; `+` is
// arithmetic, which the solver's cases use on purpose. A negative literal is
// spelled `!(l = r)`. The fixtures holding a chain are built with the
// library's chain constructor, the rest are parsed.
//
// THE ORACLE is the solver: `bv_equivalent(a, b)` holds when the
// exclusive-or of the two FINISHED formulas is unsatisfiable, its free
// variables read existentially. A reference and a chain are beyond it: those
// claims are structural, and the keep-mode answer is checked through its
// plain re-run, the chains resolved by RESOLVE_FUNCTIONAL_PLAIN.
//
// The observables are the answers, `binder_count`, and `same` on the shapes.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "anti_prenex/eliminate/bitvector.h"

#include <algorithm>

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

using tb = tau_term_bdd<node_t>;
using th = term_handle<node_t>;

tref parse(const char* sample) {
	auto rr = get_nso_rr(sample);
	REQUIRE(rr.has_value());
	return rr.value().main->get();
}

bool same(tref a, tref b) { return tau::subtree_equals(a, b); }

/// Two lists hold the same nodes in the same order.
bool same_list(const trefs& a, const trefs& b) {
	return std::ranges::equal(a, b, same);
}

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

/// A `∃P.ψ` split the way ELIMINATE_BLOCK hands it over: the block
/// OUTERMOST FIRST, the component's ctx over it, and `ψ` prepared and taken
/// as its conjunct list — an ∨-node is one conjunct.
struct fixture {
	tref quantified = nullptr;   ///< `∃P.ψ` as given, the oracle's reference
	ap::block P;
	ap::ctx<node_t> c;
	trefs conjuncts;
};

fixture make(tref quantified, bool keep = false) {
	fixture f;
	f.quantified = quantified;
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
		ap::prepare_terms<node_t>(n, f.P, f.c.order));
	return f;
}

fixture make(const char* src, bool keep = false) {
	return make(parse(src), keep);
}

/// The whole clause re-wrapped under the whole block: the answer when
/// nothing is decided.
tref rewrapped(const fixture& f) {
	return ap::rewrap<node_t>(ap::simplified_and_join<node_t>(f.conjuncts),
		f.P);
}

/// What every answer the oracle can read claims: equivalent to `∃P.ψ`, no
/// free variable escaped, invariant 4.
void check_against_source(tref got, const fixture& f) {
	CHECK(bv_equivalent(got, f.quantified));
	CHECK(no_escape(finished(got), f.quantified));
	CHECK(invariant_4(got));
}

/// The claims left when the answer is out of the oracle's reach.
void check_structure(tref got, const fixture& f) {
	CHECK(no_escape(finished(got), f.quantified));
	CHECK(invariant_4(got));
}

/// `∀_y body` over bv[2], built with the library's chain constructor.
tref fall(const char* y, tref body) {
	const tref chain = tb::build_functional_quantifiers(
		{ { tau::build_variable(y, bv_type_id<node_t>(2)), tb::all } },
		body);
	REQUIRE(tau::get(chain).child_is(tau::bf_fall));
	return chain;
}

/// V7's clause: the solver side `(x·1) + a = 1` needs `a ∈ {0, 1}`, the
/// finite side `y ≤ a ∧ y·1 ≠ 0 ∧ y·2 ≠ 0` needs `y = 3`, hence `a = 3`.
const char* const joint =
	"ex x ex y ((x:bv[2] & { 1 }:bv[2]) + a:bv[2] = { 1 }:bv[2] "
	"&& y:bv[2] & a:bv[2]' = { 0 }:bv[2] "
	"&& !(y:bv[2] & { 1 }:bv[2] = { 0 }:bv[2]) "
	"&& !(y:bv[2] & { 2 }:bv[2] = { 0 }:bv[2])).";

} // namespace

TEST_SUITE("anti_prenex/eliminate/bitvector") {

// --- opaque? ----------------------------------------------------------------------

TEST_CASE("V1: what neither engine reads freezes its component") {
	SUBCASE("a reference") {
		// `x·a = 0` shares `x` with `q(x)`: one component, the whole
		// clause, re-wrapped exactly.
		fixture f = make("ex x (x:bv[2] & a:bv[2] = { 0 }:bv[2] "
			"&& q(x:bv[2])).");
		const auto ref = std::ranges::find_if(f.conjuncts, [](tref m) {
			return is_child<node_t>(m, tau::wff_ref); });
		REQUIRE(ref != f.conjuncts.end());
		CHECK(ap::bitvector_opaque<node_t>(*ref, f.c.type));
		const tref got = ap::eliminate_bitvector_clause<node_t>(
			f.conjuncts, f.P, f.c);
		CHECK(same(got, rewrapped(f)));
		check_structure(got, f);
	}
	SUBCASE("a unit with a chain in its body") {
		// `∃x (x·a = 0 ∧ ∀y (∀_z (x·y·z) = 0))`: the solver cannot
		// translate the unit, and `x` binds the other conjunct to it.
		const size_t bv2 = bv_type_id<node_t>(2);
		const tref x = tau::build_bf_variable("x", bv2);
		const tref xyz = tau::build_bf_and(tau::build_bf_and(x,
			tau::build_bf_variable("y", bv2)),
			tau::build_bf_variable("z", bv2));
		const tref unit = tau::build_wff_all(
			tau::build_variable("y", bv2),
			build_bf_eq_0<node_t>(fall("z", xyz)), false);
		const tref xa = build_bf_eq_0<node_t>(tau::build_bf_and(x,
			tau::build_bf_variable("a", bv2)));
		fixture f = make(tau::build_wff_ex(tau::build_variable("x", bv2),
			tau::build_wff_and(xa, unit), false));
		const auto u = std::ranges::find_if(f.conjuncts,
			is_child_quantifier<node_t>);
		REQUIRE(u != f.conjuncts.end());
		CHECK(ap::bitvector_opaque<node_t>(*u, f.c.type));
		const tref got = ap::eliminate_bitvector_clause<node_t>(
			f.conjuncts, f.P, f.c);
		CHECK(same(got, rewrapped(f)));
		CHECK(binder_count(got) == 2);
		check_structure(got, f);
	}
	SUBCASE("an impure conjunct carrying a chain") {
		// `∃x (x + ∀_y (y·a) = 0)`: arithmetic keeps it from the finite
		// side, the chain from the solver.
		const size_t bv2 = bv_type_id<node_t>(2);
		const tref chain = fall("y", tau::build_bf_and(
			tau::build_bf_variable("y", bv2),
			tau::build_bf_variable("a", bv2)));
		fixture f = make(tau::build_wff_ex(tau::build_variable("x", bv2),
			build_bf_eq_0<node_t>(tau::build_bf_add(
				tau::build_bf_variable("x", bv2), chain)), false));
		REQUIRE(f.conjuncts.size() == 1);
		CHECK(ap::bitvector_opaque<node_t>(f.conjuncts[0], f.c.type));
		const tref got = ap::eliminate_bitvector_clause<node_t>(
			f.conjuncts, f.P, f.c);
		CHECK(same(got, rewrapped(f)));
		CHECK(binder_count(got) == 1);
		check_structure(got, f);
	}
	SUBCASE("an order atom is the solver's, not frozen") {
		// `≤` compares arithmetically, which the solver reads natively.
		fixture f = make("ex x (x:bv[2] <= a:bv[2]).");
		REQUIRE(f.conjuncts.size() == 1);
		CHECK(!ap::bitvector_opaque<node_t>(f.conjuncts[0], f.c.type));
	}
}

// --- SPLIT_ARITHMETIC -------------------------------------------------------------

TEST_CASE("V2: the split") {
	SUBCASE("a pure conjunct rides with the impure one it shares x with") {
		// `x + b = 0` is impure and shares `x` with `x·a = 0`; the `y`
		// conjunct shares nothing and is pure.
		fixture f = make("ex x ex y (x:bv[2] & a:bv[2] = { 0 }:bv[2] "
			"&& x:bv[2] + b:bv[2] = { 0 }:bv[2] "
			"&& !(y:bv[2] & c:bv[2] = { 0 }:bv[2])).");
		REQUIRE(f.conjuncts.size() == 3);
		const ap::block Xx{ f.P[0] }, Xy{ f.P[1] };
		trefs A, B;
		for (tref m : f.conjuncts)
			(ap::fv_meets<node_t>(m, Xx) ? A : B).push_back(m);
		REQUIRE(A.size() == 2);
		const auto sp = ap::split_arithmetic<node_t>(f.conjuncts, f.P,
			f.c.type);
		CHECK(same_list(sp.A, A));
		CHECK(same_list(sp.X_A, Xx));
		CHECK(same_list(sp.B, B));
		CHECK(same_list(sp.X_B, Xy));
	}
	SUBCASE("an all-pure clause leaves A empty") {
		// Two pure literals: nothing for the solver.
		fixture f = make("ex x (x:bv[2] & a:bv[2] = { 0 }:bv[2] "
			"&& !(x:bv[2] & b:bv[2] = { 0 }:bv[2])).");
		const auto sp = ap::split_arithmetic<node_t>(f.conjuncts, f.P,
			f.c.type);
		CHECK(sp.A.empty());
		CHECK(sp.X_A.empty());
		CHECK(same_list(sp.B, f.conjuncts));
		CHECK(same_list(sp.X_B, f.P));
	}
	SUBCASE("an all-impure clause leaves B empty") {
		// An arithmetic atom and an order atom: nothing for the finite
		// side.
		fixture f = make("ex x (x:bv[2] + a:bv[2] = { 0 }:bv[2] "
			"&& x:bv[2] <= b:bv[2]).");
		const auto sp = ap::split_arithmetic<node_t>(f.conjuncts, f.P,
			f.c.type);
		CHECK(same_list(sp.A, f.conjuncts));
		CHECK(same_list(sp.X_A, f.P));
		CHECK(sp.B.empty());
		CHECK(sp.X_B.empty());
	}
}

// --- the solver side --------------------------------------------------------------

TEST_CASE("V3: the solver side, closed") {
	SUBCASE("satisfiable: T") {
		// `x = 3` is `−1` modulo 4.
		fixture f = make("ex x (x:bv[2] + { 1 }:bv[2] = { 0 }:bv[2]).");
		const tref got = ap::eliminate_bitvector_clause<node_t>(
			f.conjuncts, f.P, f.c);
		CHECK(tau::get(got).equals_T());
		check_against_source(got, f);
	}
	SUBCASE("unsatisfiable: F") {
		// Adding `1` changes every value.
		fixture f = make("ex x (x:bv[2] + { 1 }:bv[2] = x:bv[2]).");
		const tref got = ap::eliminate_bitvector_clause<node_t>(
			f.conjuncts, f.P, f.c);
		CHECK(tau::get(got).equals_F());
		check_against_source(got, f);
	}
}

TEST_CASE("V4: the solver side, open") {
	SUBCASE("valid: T") {
		// `x = −a` for every `a`.
		fixture f = make("ex x (x:bv[2] + a:bv[2] = { 0 }:bv[2]).");
		const tref got = ap::eliminate_bitvector_clause<node_t>(
			f.conjuncts, f.P, f.c);
		CHECK(tau::get(got).equals_T());
		check_against_source(got, f);
	}
	SUBCASE("neither: the exact re-wrap") {
		// `x·1 ∈ {0, 1}`, so `a ∈ {0, 1}` alone satisfies it: not
		// valid, not unsatisfiable.
		fixture f = make("ex x ((x:bv[2] & { 1 }:bv[2]) + a:bv[2] "
			"= { 1 }:bv[2]).");
		const tref got = ap::eliminate_bitvector_clause<node_t>(
			f.conjuncts, f.P, f.c);
		CHECK(same(got, rewrapped(f)));
		CHECK(binder_count(got) == 1);
		check_against_source(got, f);
	}
}

TEST_CASE("V5: an order atom reaches the solver") {
	SUBCASE("valid: T") {
		// `x = 0` is below every `a`.
		fixture f = make("ex x (x:bv[2] <= a:bv[2]).");
		const tref got = ap::eliminate_bitvector_clause<node_t>(
			f.conjuncts, f.P, f.c);
		CHECK(tau::get(got).equals_T());
		check_against_source(got, f);
	}
	SUBCASE("neither: the exact re-wrap") {
		// Unsigned order: `a = 0` leaves `x = 0` alone, `a = 1` admits
		// `x = 1`.
		fixture f = make("ex x (x:bv[2] <= a:bv[2] "
			"&& !(x:bv[2] = { 0 }:bv[2])).");
		const tref got = ap::eliminate_bitvector_clause<node_t>(
			f.conjuncts, f.P, f.c);
		CHECK(same(got, rewrapped(f)));
		CHECK(binder_count(got) == 1);
		check_against_source(got, f);
	}
}

// --- both sides -------------------------------------------------------------------

TEST_CASE("V6: both sides decided") {
	SUBCASE("the solver's T beside the finite T") {
		// `x = −a`; the elements 1 and 2 are left for `y`.
		fixture f = make("ex x ex y (x:bv[2] + a:bv[2] = { 0 }:bv[2] "
			"&& !(y:bv[2] = { 0 }:bv[2]) "
			"&& !(y:bv[2] = { 3 }:bv[2])).");
		const tref got = ap::eliminate_bitvector_clause<node_t>(
			f.conjuncts, f.P, f.c);
		CHECK(tau::get(got).equals_T());
		check_against_source(got, f);
	}
	SUBCASE("the finite side's F decides over bv[1]") {
		// R13's clause for `y`: bv[1] has no third element.
		fixture f = make("ex x ex y (x:bv[1] + a:bv[1] = { 0 }:bv[1] "
			"&& !(y:bv[1] = { 0 }:bv[1]) "
			"&& !(y:bv[1] = { 1 }:bv[1])).");
		const tref got = ap::eliminate_bitvector_clause<node_t>(
			f.conjuncts, f.P, f.c);
		CHECK(tau::get(got).equals_F());
		check_against_source(got, f);
	}
}

TEST_CASE("V7: the joint ask") {
	SUBCASE("each side open alone") {
		// See `joint`: the two sides as clauses of their own.
		fixture fx = make("ex x ((x:bv[2] & { 1 }:bv[2]) + a:bv[2] "
			"= { 1 }:bv[2]).");
		CHECK(binder_count(ap::eliminate_bitvector_clause<node_t>(
			fx.conjuncts, fx.P, fx.c)) == 1);
		fixture fy = make("ex y (y:bv[2] & a:bv[2]' = { 0 }:bv[2] "
			"&& !(y:bv[2] & { 1 }:bv[2] = { 0 }:bv[2]) "
			"&& !(y:bv[2] & { 2 }:bv[2] = { 0 }:bv[2])).");
		CHECK(binder_count(ap::eliminate_bitvector_clause<node_t>(
			fy.conjuncts, fy.P, fy.c)) == 1);
	}
	SUBCASE("sharing a, together unsatisfiable: F") {
		fixture f = make(joint);
		const tref got = ap::eliminate_bitvector_clause<node_t>(
			f.conjuncts, f.P, f.c);
		CHECK(tau::get(got).equals_F());
		check_against_source(got, f);
	}
	SUBCASE("V7b: no shared variable, two units") {
		// `b` for `a` on the finite side: the sides are independent and
		// no joint ask is made.
		fixture f = make("ex x ex y ((x:bv[2] & { 1 }:bv[2]) + a:bv[2] "
			"= { 1 }:bv[2] && y:bv[2] & b:bv[2]' = { 0 }:bv[2] "
			"&& !(y:bv[2] & { 1 }:bv[2] = { 0 }:bv[2]) "
			"&& !(y:bv[2] & { 2 }:bv[2] = { 0 }:bv[2])).");
		const tref got = ap::eliminate_bitvector_clause<node_t>(
			f.conjuncts, f.P, f.c);
		CHECK(binder_count(got) == 2);
		check_against_source(got, f);
	}
}

// --- keep mode --------------------------------------------------------------------

TEST_CASE("V8: a chain on the pure side skips the joint ask") {
	// Keep mode spells the pure side's `pos` as the chain `∀_y (y·a′) = 0`,
	// which the solver cannot read: the two units stay.
	fixture f = make(joint, true);
	const tref got = ap::eliminate_bitvector_clause<node_t>(
		f.conjuncts, f.P, f.c);
	CHECK(!tau::get(got).equals_F());
	CHECK(holds(got, tau::bf_fall));
	CHECK(binder_count(got) == 2);
	check_structure(got, f);
	// The plain re-run resolves the chain, and the oracle reads the rest.
	const tref plain = ap::resolve_functional_quantifiers_plain<node_t>(
		finished(got));
	CHECK(!holds(plain, tau::bf_fall));
	CHECK(!holds(plain, tau::bf_fex));
	CHECK(bv_equivalent(plain, f.quantified));
}

TEST_CASE("V9: a chain the split's closure puts on the solver side") {
	// `∃x (x + a = 0 ∧ ∀_y (x·y) = 0)`: the pure chain equation shares `x`
	// with the arithmetic atom, so both re-wrap as one unit over `x`.
	const size_t bv2 = bv_type_id<node_t>(2);
	const tref x = tau::build_bf_variable("x", bv2);
	const tref vx = tau::build_variable("x", bv2);
	const tref xa = build_bf_eq_0<node_t>(tau::build_bf_add(x,
		tau::build_bf_variable("a", bv2)));
	const tref xy = build_bf_eq_0<node_t>(fall("y", tau::build_bf_and(x,
		tau::build_bf_variable("y", bv2))));
	// Preparation folds this chain to `0`, so its equation joins the
	// prepared rest as written.
	fixture f = make(tau::build_wff_ex(vx, xa, false), true);
	f.quantified = tau::build_wff_ex(vx, tau::build_wff_and(xa, xy), false);
	f.conjuncts.push_back(xy);
	const auto sp = ap::split_arithmetic<node_t>(f.conjuncts, f.P,
		f.c.type);
	REQUIRE(sp.A.size() == 2);
	REQUIRE(sp.B.empty());
	const size_t before = ap::taint_count<node_t>();
	const tref got = ap::eliminate_bitvector_clause<node_t>(
		f.conjuncts, f.P, f.c);
	CHECK(ap::taint_count<node_t>() == before);
	CHECK(same(got, rewrapped(f)));
	REQUIRE(binder_count(got) == 1);
	CHECK(same(tau::trim_right_sibling(ap::binder_var<node_t>(got)),
		f.P[0]));
	const tref body = finished(ap::binder_body<node_t>(got));
	CHECK((holds(body, tau::bf_fall) || holds(body, tau::bf_fex)));
	CHECK(holds(body, tau::bf_add));
	check_structure(got, f);
#ifdef TAU_CACHE
	// Neither the side's query nor its two closings reached the solver.
	const tref q = ap::finish_terms<node_t>(ap::rewrap<node_t>(
		ap::simplified_and_join<node_t>(sp.A), sp.X_A));
	const trefs Z = get_free_vars<node_t>(q);
	for (tref k : { q, ap::rewrap<node_t>(q, Z, tb::all),
		ap::rewrap<node_t>(q, Z, tb::ex) })
		CHECK(!ap::lookup<node_t, ap::table::solver_memo>(
			ap::canonicalise_binder_ids<node_t>(k)).has_value());
#endif
}

} // TEST_SUITE
