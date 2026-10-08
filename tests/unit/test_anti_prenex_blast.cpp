// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for src/anti_prenex/eliminate/ask.h and blast.h: §7 ASK, and §7
// DECIDE_FINITE with BIT_BLAST fused into its BDD sweep. Spec: anti_prenex.md
// §7 ("Bitvector — the router" for ASK, "Finite (pure Boolean)" for the blast
// and the sweep), §1 (`qbf_node_max`, `solver_memo`, `qbf_memo`, cache scope).
//
// Every query is CLOSED and PARSED with `get_nso_rr`, so inference types its
// variables, and its terms are plain; the chain queries alone are built with
// the library's chain constructor. The ring sum is `^`: Tau parses `+` as
// arithmetic. A negative literal is spelled `!(l = r)`, the one negative
// shape the blast reads (invariant 4); a `bf_neq` is FAIL. Each case's ctx
// is `for_component` over the query's outermost block, with its type. R13
// names the case of test_anti_prenex_regression.cpp whose query B2 and B7
// decide.
//
// The observables are the answers, the delta of `taint_count` — global and
// never reset — and in Release (`TAU_CACHE`) the two tables through `lookup`.
//
// THE BUDGET CASES COME FIRST. The sweep's universe is process-wide and
// append-only, and with variable shifters one node serves every query of its
// shape, so a budget of `0` abandons a sweep only while that sweep still
// allocates: B7 and B8 run before any other sweep of this binary.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "anti_prenex/eliminate/blast.h"

#include <optional>
#include <stdexcept>

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

using tb = tau_term_bdd<node_t>;

tref parse(const char* sample) {
	auto rr = get_nso_rr(sample);
	REQUIRE(rr.has_value());
	return rr.value().main->get();
}

/// The ctx of the query's outermost block — its leading run of binders of
/// one kind — with the type read off the block.
ap::ctx<node_t> ctx_of(tref q) {
	ap::block P;
	const bool ex = is_child<node_t>(q, tau::wff_ex);
	tref n = q;
	while (is_child_quantifier<node_t>(n)
		&& is_child<node_t>(n, tau::wff_ex) == ex)
	{
		P.push_back(tau::trim_right_sibling(ap::binder_var<node_t>(n)));
		n = tau::trim_right_sibling(ap::binder_body<node_t>(n));
	}
	REQUIRE(!P.empty());
	return ap::ctx<node_t>::for_component(P, find_ba_type<node_t>(P[0]),
		false);
}

std::optional<bool> decide(tref q) {
	auto c = ctx_of(q);
	return ap::decide_finite<node_t>(q, c);
}

/// `qbf_node_max` for the scope of one case, restored on the way out. A ctx
/// copies the knob at setup, so a ctx made under the guard carries it.
struct qbf_node_max_guard {
	const size_t saved = ap::qbf_node_max;
	explicit qbf_node_max_guard(size_t v) { ap::qbf_node_max = v; }
	~qbf_node_max_guard() { ap::qbf_node_max = saved; }
};

/// `∃x (∀_y (x·y) = 0)` over bv[2], built with the library's chain
/// constructor: the solver has no case for a chain, the sweep reads it.
tref chain_query() {
	const size_t bv2 = bv_type_id<node_t>(2);
	const tref x = tau::build_variable("x", bv2);
	const tref y = tau::build_variable("y", bv2);
	const tref xy = tau::build_bf_and(tau::build_bf_variable("x", bv2),
		tau::build_bf_variable("y", bv2));
	const tref chain = tb::build_functional_quantifiers(
		{ { y, tb::all } }, xy);
	REQUIRE(tau::get(chain).child_is(tau::bf_fall));
	return tau::build_wff_ex(x, build_bf_eq_0<node_t>(chain), false);
}

/// R13's query, `∃x (x ≠ 0 ∧ x ≠ 1)` over bv[1], each `≠` spelled `¬(=)`.
const char* const r13 =
	"ex x (!(x:bv[1] = { 0 }:bv[1]) && !(x:bv[1] = { 1 }:bv[1])).";

} // namespace

TEST_SUITE("anti_prenex/eliminate/blast") {

// --- ASK ------------------------------------------------------------------------

TEST_CASE("A1: ASK answers sat and unsat, and unknown on a query it cannot read") {
	CHECK(ap::ask<node_t>(parse("ex x (x:bv[2] = { 0 }:bv[2]).")
		) == ap::answer::sat);
	// R13 as the solver reads it, `≠` and all.
	CHECK(ap::ask<node_t>(parse(
		"ex x (x:bv[1] != { 0 }:bv[1] && x:bv[1] != { 1 }:bv[1]).")
		) == ap::answer::unsat);
	// A chain does not translate, which is `unknown`, never an error. A
	// free variable in the query trips ask's Debug assertion.
	CHECK(ap::ask<node_t>(chain_query()) == ap::answer::unknown);
}

// --- the budget (first: see the header) ------------------------------------------

TEST_CASE("B7: a spent budget hands the query to the solver, which decides it") {
	const tref q = parse(r13);
	[[maybe_unused]] const tref k = ap::canonicalise_binder_ids<node_t>(q);
	qbf_node_max_guard g(0);
	auto c = ctx_of(q);
#ifdef TAU_CACHE
	REQUIRE(!ap::lookup<node_t, ap::table::qbf_memo>(k).has_value());
	REQUIRE(!ap::lookup<node_t, ap::table::solver_memo>(k).has_value());
#endif
	const size_t before = ap::taint_count<node_t>();
	CHECK(ap::decide_finite<node_t>(q, c) == false);
	CHECK(ap::taint_count<node_t>() == before);
#ifdef TAU_CACHE
	// The solver's verdict, not the sweep's.
	CHECK(ap::lookup<node_t, ap::table::solver_memo>(k) == ap::answer::unsat);
	CHECK(!ap::lookup<node_t, ap::table::qbf_memo>(k).has_value());
#endif
	// B2 asks the same query and has to reach the sweep.
	ap::flush_solver_dependent<node_t>();
}

TEST_CASE("B8: a spent budget and a solver that cannot read the query taint") {
	const tref q = chain_query();
	qbf_node_max_guard g(0);
	auto c = ctx_of(q);
	const size_t before = ap::taint_count<node_t>();
	CHECK(!ap::decide_finite<node_t>(q, c).has_value());
	CHECK(ap::taint_count<node_t>() == before + 1);
}

// --- the sweep ------------------------------------------------------------------

TEST_CASE("B1: a closed existential is true") {
	CHECK(decide(parse("ex x (x:bv[2] = { 0 }:bv[2]).")) == true);
}

TEST_CASE("B2: two negatives compete for the atoms of a small algebra") {
	// Over bv[1] both are nonzero only if x is neither 0 nor 1.
	const tref q = parse(r13);
	CHECK(decide(q) == false);
#ifdef TAU_CACHE
	CHECK(ap::lookup<node_t, ap::table::qbf_memo>(
		ap::canonicalise_binder_ids<node_t>(q)) == false);
#endif
	// Over bv[2] the elements 1 and 2 are left.
	CHECK(decide(parse("ex x (!(x:bv[2] = { 0 }:bv[2]) "
		"&& !(x:bv[2] = { 3 }:bv[2])).")) == true);
}

TEST_CASE("B3: an alternation is decided whole") {
	// y = x′ meets both equations for every x.
	CHECK(decide(parse("all x ex y (x:bv[2] & y:bv[2] = { 0 }:bv[2] "
		"&& x:bv[2] | y:bv[2] = { 3 }:bv[2]).")) == true);
}

TEST_CASE("B4: the sweep reads a constant's bits and the ring sum") {
	// x·10 = 10 holds for x ∈ {10, 11}, not for x = 00.
	CHECK(decide(parse("ex x (x:bv[2] & { 2 }:bv[2] = { 2 }:bv[2]).")) == true);
	CHECK(decide(parse("all x (x:bv[2] & { 2 }:bv[2] = { 2 }:bv[2]).")) == false);
	// x ⊕ 01 = 0 at x = 01.
	CHECK(decide(parse("ex x (x:bv[2] ^ { 1 }:bv[2] = { 0 }:bv[2]).")) == true);
}

TEST_CASE("B5: the sweep decides a unit nested inside the matrix") {
	// By hand: y = 0 meets `y·x = 0` and every other y meets `y ≠ 0`, so the
	// inner ∀y holds for every x and the query is ∃x (x ≠ 0): true.
	CHECK(decide(parse("ex x (!(x:bv[2] = { 0 }:bv[2]) && all y ("
		"y:bv[2] & x:bv[2] = { 0 }:bv[2] || !(y:bv[2] = { 0 }:bv[2]))).")
		) == true);
}

TEST_CASE("B6: the sweep reads a functional quantifier in the query") {
	// ∀_y (x·y) = (x·0)·(x·1) = 0, so the equation holds for every x.
	CHECK(decide(chain_query()) == true);
}

TEST_CASE("B9: an order atom is FAIL, never a verdict") {
	const tref q = parse("ex x (x:bv[2] <= { 1 }:bv[2]).");
	auto c = ctx_of(q);
	CHECK_THROWS_AS(ap::decide_finite<node_t>(q, c), std::logic_error);
}

// An equation's slices are shifted copies of one node, conjoined by the
// engine's and_many; B10 and B11 guard verdicts that hinge on it.

TEST_CASE("B10: six nonempty pairwise disjoint elements do not fit in bv[5]") {
	const std::string v = "abcdef";
	auto var = [](char x) { return std::string(1, x) + ":bv[5]"; };
	std::string q = "ex a, b, c, d, e, f (";
	for (char x : v) q += "!(" + var(x) + " = { 0 }:bv[5]) && ";
	for (size_t i = 0; i < v.size(); ++i)
		for (size_t j = i + 1; j < v.size(); ++j)
			q += var(v[i]) + " & " + var(v[j]) + " = { 0 }:bv[5] && ";
	q.replace(q.size() - 4, 4, ").");
	CHECK(decide(parse(q.c_str())) == false);
}

TEST_CASE("B11: two atoms of bv[8], each distinct from its predecessor") {
	// `x ≤ v → x = 0 ∨ x = v` makes v an atom; bv[8] has eight.
	CHECK(decide(parse("ex u, v, w (!(v:bv[8] = { 0 }:bv[8]) "
		"&& all x (!(x:bv[8] & v:bv[8]' = { 0 }:bv[8]) "
		"|| x:bv[8] = { 0 }:bv[8] || x:bv[8] = v:bv[8]) "
		"&& !(v:bv[8] = u:bv[8]) && !(w:bv[8] = { 0 }:bv[8]) "
		"&& all y (!(y:bv[8] & w:bv[8]' = { 0 }:bv[8]) "
		"|| y:bv[8] = { 0 }:bv[8] || y:bv[8] = w:bv[8]) "
		"&& !(w:bv[8] = v:bv[8])).")) == true);
}

} // TEST_SUITE
