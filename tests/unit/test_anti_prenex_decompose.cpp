// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for src/anti_prenex/push/decompose.h: §6's TRY_DECOMPOSE and
// DECOMPOSE_ARMS, the licensed Boole decomposition above the conjunction's
// floor. Spec: anti_prenex.md §6 (TRY_DECOMPOSE, DECOMPOSE_ARMS,
// FOLD_DECIDED), §1 (`prio`), §3 (TRY_WITNESS's pin test), invariants 5 and
// 8.
//
// The harness is the expansion suite's: parse `∃P.ψ`, normalise `ψ` the way
// phases 1 and 3 leave it — no SIMPLIFY, so a shape phase 1 would fold still
// reaches the step — put it into §1's term representation and call the step
// DIRECTLY on the prepared clause. What is claimed of a result: EQUIVALENT to
// `∃P.ψ` (`are_nso_equivalent` on the finished formula), no free variable
// ESCAPED, INVARIANT 4, and per case the block RESOLVED or the binders left.
//
// SIZES are §1's `|φ|`: an atom is 1, `|¬ψ| = 1 + |ψ|`, and a chain of `k`
// members costs `k − 1` plus its members. The licence passes iff
// `4·lhs ≤ 5·rhs`, `rhs` the touched conjuncts' sizes and `lhs` their two
// cofactors'.
//
// The constant-false atom runs over `sbf`, whose constants parse as
// `{A}:sbf` — which is why this suite is built over the `tau` pack of BAs,
// the one that carries `sbf`.
//
// Parsing note: a parsed quantifier's body runs to the RIGHT END and
// juxtaposition is conjunction, so every input keeps its parentheses.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "normalizer.h"
#include "anti_prenex/push/push_block.h"
#include "anti_prenex/push/decompose.h"
#include "anti_prenex/normalisers/nnf.h"

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

/// Every formula binder of `n`, nested ones included.
size_t binder_count(tref n) {
	return tau::get(n).select_all(is_child_quantifier<node_t>).size();
}

/// A member of `n` content-equal to `m`?
bool has_member(tref n, tref m) {
	for (tref c : ap::members<node_t>(n)) if (same(c, m)) return true;
	return false;
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

/// A parsed `∃P.ψ` split the way the push's callers hand it over: the block
/// OUTERMOST FIRST, the component's ctx over it — `type` read off the block,
/// as invariant 2 allows — and the body as phase 4 meets it, put into §1's
/// term representation.
struct fixture {
	tref quantified = nullptr;   ///< `∃P.ψ` as parsed, the oracle's reference
	ap::block P;
	ap::ctx<node_t> c;
	tref clause = nullptr;       ///< `ψ`, normalised and prepared over `P`
};

/// `ψ` as PHASE 4 meets it: in NNF, with `f ≠ 0` spelled `¬(f = 0)` (phases 1
/// and 3, §3).
tref normalised(tref body) {
	return ap::normalize_operators<node_t>(
		ap::to_canonically_factored_nnf<node_t>(body));
}

fixture make(const char* src) {
	// The engine behind `sbf`'s constants is set up before one is parsed;
	// the call is idempotent.
	bdd_init<Bool>();
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
		find_ba_type<node_t>(f.P[0]), false);
	f.clause = ap::prepare_terms<node_t>(normalised(n), f.P, f.c.order);
	return f;
}

/// TRY_DECOMPOSE on the prepared clause.
std::optional<tref> decomposed(fixture& f) {
	return ap::try_decompose<node_t>(f.clause, f.P, f.c);
}

/// What every pushed block claims: equivalent to `∃P.ψ`, no free variable
/// escaped, invariant 4.
void check_against_source(tref got, const fixture& f) {
	CHECK(are_nso_equivalent<node_t>(finished(got), f.quantified));
	CHECK(no_escape(finished(got), f.quantified));
	CHECK(invariant_4(got));
}

/// THE BLOCK IS RESOLVED: no binder came back and no block variable is free
/// in the result.
void check_resolved(tref got, const fixture& f) {
	CHECK(binder_count(got) == 0);
	CHECK(!ap::fv_meets<node_t>(got, f.P));
}

/// The clause's disjunctive conjuncts, in member order.
trefs disjunctive(const fixture& f) {
	trefs out;
	for (tref m : ap::members<node_t>(f.clause))
		if (is_child<node_t>(m, tau::wff_or)) out.push_back(m);
	return out;
}

/// The atom under the one negated literal at a member top of the clause's
/// disjunctive conjuncts: the atom each case splits on.
tref negated_top(const fixture& f) {
	tref a = nullptr;
	size_t found = 0;
	for (tref s : disjunctive(f))
		for (tref m : ap::members<node_t>(s))
			if (ap::is_negated_atom<node_t>(m))
				a = ap::atom_of<node_t>(m), ++found;
	REQUIRE(found == 1);
	return a;
}

/// `a`'s occurrences at member tops of the disjunctive conjuncts, positive
/// and negative: what the census counts.
std::pair<size_t, size_t> member_tops(const fixture& f, tref a) {
	std::pair<size_t, size_t> k{ 0, 0 };
	for (tref s : disjunctive(f))
		for (tref m : ap::members<node_t>(s)) {
			if (same(m, a)) ++k.first;
			else if (ap::is_negated_atom<node_t>(m)
				&& same(ap::atom_of<node_t>(m), a)) ++k.second;
		}
	return k;
}

/// The X-free member of `n`, which a case reads back as a node.
tref free_member(tref n, const fixture& f) {
	for (tref m : ap::members<node_t>(n))
		if (!ap::fv_meets<node_t>(m, f.P))
			return tau::trim_right_sibling(m);
	FAIL("no X-free member");
	return nullptr;
}

constexpr const char* spanning =
	"ex x ((a = 0 || xy = 0) && (a != 0 || xz = 0)).";

} // namespace


TEST_SUITE("anti_prenex/decompose") {

TEST_CASE("C1: the spanning atom is taken") {
	// `a = 0` spans the two disjuncts, and its cofactors weigh 4 against 7.
	// The arms `a = 0 ∧ x·z = 0` and `¬(a = 0) ∧ x·y = 0` push to `a = 0`
	// and `¬(a = 0)`, whose ∨-join is `T`: both arms pushed.
	fixture f = make(spanning);
	const tref a = negated_top(f);
	REQUIRE(member_tops(f, a) == std::pair<size_t, size_t>{ 1, 1 });
	// No cheaper step applies: 2a and 2b are refuted, no positive equation
	// is a conjunct, `x` does not settle, and neither pin matches.
	const trefs cs = ap::members<node_t>(f.clause);
	REQUIRE(std::ranges::none_of(cs, [](tref m) {
		return is_child<node_t>(m, tau::bf_eq); }));
	REQUIRE(!ap::try_fast_paths<node_t>(f.clause, f.P, f.c).has_value());
	const ap::incidence_result<node_t> inc =
		ap::incidence<node_t>(cs, f.P);
	REQUIRE(inc.parts.size() == 1);
	REQUIRE(inc.settled.empty());
	REQUIRE(!ap::try_witness<node_t>(f.P[0], f.clause, f.c).has_value());
	REQUIRE(!ap::try_case_witness<node_t>(f.P[0], f.clause, f.c)
		.has_value());
	const std::optional<tref> got = decomposed(f);
	REQUIRE(got.has_value());
	CHECK(tau::get(*got).equals_T());
	check_against_source(*got, f);
}

TEST_CASE("C2: a one-signed atom is no candidate") {
	// L17's clause: `x·y = 0` and `x·w = 0` are positive member tops, and
	// the spine's `¬(x·z = 0)` does not count.
	fixture f = make("ex x ((xy = 0 || xw = 0) && xz != 0).");
	REQUIRE(disjunctive(f).size() == 1);
	CHECK(!decomposed(f).has_value());
}

TEST_CASE("C3: an ∧-member's conjuncts are no member tops") {
	// `a = 0` is negated at a member top of the second disjunct, but its
	// positive occurrence sits inside the ∧-member `a = 0 ∧ x·y = 0`,
	// which the census does not open: one sign only.
	fixture f = make("ex x (((a = 0 && xy = 0) || xw = 0) "
		"&& (a != 0 || xz = 0)).");
	const tref a = negated_top(f);
	REQUIRE(member_tops(f, a) == std::pair<size_t, size_t>{ 0, 1 });
	REQUIRE(std::ranges::any_of(disjunctive(f), [a](tref s) {
		return std::ranges::any_of(ap::members<node_t>(s), [a](tref m) {
			return is_child<node_t>(m, tau::wff_and)
				&& has_member(m, a); }); }));
	CHECK(!decomposed(f).has_value());
}

TEST_CASE("C4: a constant-false candidate is folded, not decomposed") {
	// FD2's atom `f = 0`, `f = x′·A ∪ x·B`: its cofactors' product `A·B` is a
	// nonzero constant, so the probe proves it `F`. `ψ[a ↦ F]` is `x·y = 0`,
	// `T` at `x := 0`, with no arm.
	fixture f = make("ex x ((((x' & {A}:sbf) | (x & {B}:sbf)) = 0 "
		"|| x & y = 0) && (((x' & {A}:sbf) | (x & {B}:sbf)) != 0 "
		"|| x & z = 0)).");
	REQUIRE(f.c.type == sbf_type_id<node_t>());
	const tref a = negated_top(f);
	REQUIRE(member_tops(f, a) == std::pair<size_t, size_t>{ 1, 1 });
	REQUIRE(!ap::fold_decided<node_t>({ a }, f.P, f.c).has_value());
	const std::optional<tref> got = decomposed(f);
	REQUIRE(got.has_value());
	CHECK(tau::get(*got).equals_T());
	check_against_source(*got, f);
}

TEST_CASE("C5: the licence refuses an atom that copies bulk") {
	// `|B| = 17` (nine atoms); the third conjunct is 26, so rhs = 3 + 4 + 26
	// = 33. Its cofactors `B ∨ x·u = 0` and `B ∨ x·v = 0` are 19 each and the
	// others 1 each: lhs = 42, and 4·42 = 168 > 165 = 5·33.
	fixture f = make("ex x ((a = 0 || xy = 0) && (a != 0 || xz = 0) "
		"&& ((xb = 0 && xc = 0 && xd = 0 && xe = 0 && xg = 0 && xh = 0 "
		"&& xk = 0 && xm = 0 && xn = 0) || (a = 0 && xu = 0) "
		"|| (a != 0 && xv = 0))).");
	const tref a = negated_top(f);
	REQUIRE(member_tops(f, a) == std::pair<size_t, size_t>{ 1, 1 });
	const trefs S = disjunctive(f);
	REQUIRE(S.size() == 3);
	std::vector<size_t> sizes;
	for (tref s : S) sizes.push_back(ap::formula_size<node_t>(s));
	std::ranges::sort(sizes);
	REQUIRE(sizes == std::vector<size_t>{ 3, 4, 26 });
	for (tref s : S) REQUIRE(ap::has_atom<node_t>(s, a));
	CHECK(!decomposed(f).has_value());
}

TEST_CASE("C6: the pin arm deletes its binder") {
	// `x = a` pins `x` strictly (cofactors `a`, `a′`), witness `a`: the T-arm
	// is `z = 0` under the empty block, the F-arm `¬(x = a) ∧ y = 0` pushes
	// to `y = 0` (`x := a′`). Both arms pushed; `y = 0 ∨ z = 0`.
	fixture f = make("ex x ((x = a || y = 0) && (x != a || z = 0)).");
	const tref a = negated_top(f);
	REQUIRE(member_tops(f, a) == std::pair<size_t, size_t>{ 1, 1 });
	REQUIRE(ap::cof_pin<node_t>(a, f.P[0], f.c).has_value());
	const std::optional<tref> got = decomposed(f);
	REQUIRE(got.has_value());
	check_against_source(*got, f);
	check_resolved(*got, f);
	CHECK(are_nso_equivalent<node_t>(finished(*got),
		parse("y = 0 || z = 0.")));
	// The arms alone, on the clause's own `z = 0` and `y = 0`, build the
	// same node.
	fixture g = make("ex x ((x = a || y = 0) && (x != a || z = 0)).");
	const trefs S = disjunctive(g);
	REQUIRE(S.size() == 2);
	const tref a_g = negated_top(g);
	const tref y0 = free_member(has_member(S[0], a_g) ? S[0] : S[1], g);
	const tref z0 = free_member(has_member(S[0], a_g) ? S[1] : S[0], g);
	const tref arms = ap::decompose_arms<node_t>(a_g, z0, y0, g.P, g.c);
	CHECK(same(arms, *got));
}

TEST_CASE("C9: an arm that folds to a constant is inserted as it stands") {
	// C1's clause plus `a = 0`, added raw since the joins would absorb the
	// first disjunct. Licensed 6 against 8; `C_F` holds `a = 0 ↦ F`, so the
	// F-arm is `F`, never pushed, and the T-arm pushes to `a = 0`.
	fixture f = make(spanning);
	const tref a = negated_top(f);
	trefs cs = ap::members<node_t>(f.clause);
	cs.push_back(a);
	f.clause = ap::canonical_and<node_t>(cs);
	f.quantified = parse("ex x ((a = 0 || xy = 0) && (a != 0 || xz = 0) "
		"&& a = 0).");
	REQUIRE(member_tops(f, a) == std::pair<size_t, size_t>{ 1, 1 });
	REQUIRE(ap::members<node_t>(f.clause).size() == 3);
	REQUIRE(has_member(f.clause, a));
	const std::optional<tref> got = decomposed(f);
	REQUIRE(got.has_value());
	CHECK(same(*got, a));
	check_against_source(*got, f);
	check_resolved(*got, f);
}

TEST_CASE("C10: each arm pushed reads the clock first") {
	// C1's clause under a limit of zero: the probe folds nothing, and the
	// first arm is past it (§3).
	fixture f = make(spanning);
	const ap::time_limit_scope limit(
		std::chrono::steady_clock::duration::zero());
	CHECK_THROWS_AS(decomposed(f), ap::time_limit_exceeded);
}

} // TEST_SUITE
