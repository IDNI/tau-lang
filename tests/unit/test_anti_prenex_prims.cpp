// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Layer 0 unit tests for src/anti_prenex/foundations/prims.h.
// Spec: anti_prenex.md.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "anti_prenex/foundations/prims.h"

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

// A bound-variable node (the shape a binder's child 0 has).
tref qvar(const char* n) {
	return build_variable<node_t>(n, tau_type_id<node_t>());
}

// The same variable as a term.
tref tvar(const char* n) {
	return build_bf_variable<node_t>(n, tau_type_id<node_t>());
}

// The atom `n = 0`, a wff.
tref eq0(const char* n) {
	return tau::build_bf_eq_0(tvar(n));
}

// The bound variable of a binder given by its wrapper node.
std::string bound_name(tref n) {
	return get_var_name<node_t>(tau::trim2(n));
}

} // namespace

// ── CANONICALISE_BINDER_IDS ─────────────────────────────────────────────────
//
// Ids are depth-derived over ONE space shared by formula binders
// (wff_all/wff_ex) and functional quantifiers (bf_fall/bf_fex): id = maximal
// quantifier depth below + 1, counting both kinds. Raw inputs are built with
// calculate_quant_id = false so the builders do no numbering of their own.

TEST_SUITE("anti_prenex/foundations/prims: canonicalise_binder_ids") {

	TEST_CASE("alpha-variants of a functional quantifier become one node") {
		tref a = ap::canonicalise_binder_ids<node_t>(tau::build_bf_eq_0(
			tau::build_bf_fex(qvar("x"), tvar("x"), false)));
		tref b = ap::canonicalise_binder_ids<node_t>(tau::build_bf_eq_0(
			tau::build_bf_fex(qvar("z"), tvar("z"), false)));
		CHECK(a == b);
	}

	TEST_CASE("alpha-variants of a formula binder become one node") {
		tref a = ap::canonicalise_binder_ids<node_t>(
			tau::build_wff_ex(qvar("x"), eq0("x"), false));
		tref b = ap::canonicalise_binder_ids<node_t>(
			tau::build_wff_ex(qvar("z"), eq0("z"), false));
		CHECK(a == b);
	}

	TEST_CASE("the two kinds share one depth count") {
		// ex x. ((fex y. y) = 0) -- the functional quantifier sits below
		// the formula binder, so it lowers the formula binder's id: the
		// outer binder is 2, not the 1 a formula-only count would give.
		tref raw = tau::build_wff_ex(qvar("x"), tau::build_bf_eq_0(
			tau::build_bf_fex(qvar("y"), tvar("y"), false)), false);
		tref expected = tau::build_wff_ex(qvar("2"), tau::build_bf_eq_0(
			tau::build_bf_fex(qvar("1"), tvar("1"), false)), false);
		CHECK(ap::canonicalise_binder_ids<node_t>(raw) == expected);
	}

	TEST_CASE("nested functional quantifiers strictly decrease") {
		tref raw = tau::build_bf_eq_0(tau::build_bf_fex(qvar("x"),
			tau::build_bf_fex(qvar("y"), tvar("y"), false), false));
		tref expected = tau::build_bf_eq_0(tau::build_bf_fex(qvar("2"),
			tau::build_bf_fex(qvar("1"), tvar("1"), false), false));
		CHECK(ap::canonicalise_binder_ids<node_t>(raw) == expected);
	}

	TEST_CASE("a term binder shadowing a formula binder does not capture") {
		// ex x. ((fex x. x) = 0): the occurrence belongs to the INNER
		// functional quantifier. Renaming only formula binders rewrote it
		// to the outer binder's id and captured it -- the bug this pass
		// closes. The inner occurrence must carry the inner id.
		tref raw = tau::build_wff_ex(qvar("x"), tau::build_bf_eq_0(
			tau::build_bf_fex(qvar("x"), tvar("x"), false)), false);
		tref expected = tau::build_wff_ex(qvar("2"), tau::build_bf_eq_0(
			tau::build_bf_fex(qvar("1"), tvar("1"), false)), false);
		CHECK(ap::canonicalise_binder_ids<node_t>(raw) == expected);
	}

	TEST_CASE("free variables are never renamed") {
		// ex x. (x = 0 & y = 0) -- y is free and keeps its source name,
		// which is also why an id can never collide with one: a source
		// name must start with a letter.
		tref raw = tau::build_wff_ex(qvar("x"),
			tau::build_wff_and(eq0("x"), eq0("y")), false);
		tref expected = tau::build_wff_ex(qvar("1"),
			tau::build_wff_and(eq0("1"), eq0("y")), false);
		CHECK(ap::canonicalise_binder_ids<node_t>(raw) == expected);
	}

	TEST_CASE("is idempotent") {
		tref raw = tau::build_wff_ex(qvar("x"), tau::build_bf_eq_0(
			tau::build_bf_fex(qvar("y"), tvar("x"), false)), false);
		tref once = ap::canonicalise_binder_ids<node_t>(raw);
		CHECK(ap::canonicalise_binder_ids<node_t>(once) == once);
	}

	TEST_CASE("is the identity on a parsed formula") {
		const char* sample = "ex x x = 0";
		tref parsed = tau::get(sample, parse_wff());
		REQUIRE(parsed != nullptr);
		CHECK(ap::canonicalise_binder_ids<node_t>(parsed) == parsed);
	}

	TEST_CASE("siblings may share an id; a path may not") {
		// (ex a. a = 0 & y = 0) & (ex b. b = 0 & z = 0): both binders are
		// at depth 1, so both become 1. Sharing across siblings is fine --
		// only strict decrease ALONG A PATH is load-bearing. The bodies
		// differ so the conjunction survives the `$X && $X ::= $X` hook
		// and the two binders are really both there to look at.
		tref raw = tau::build_wff_and(
			tau::build_wff_ex(qvar("a"),
				tau::build_wff_and(eq0("a"), eq0("y")), false),
			tau::build_wff_ex(qvar("b"),
				tau::build_wff_and(eq0("b"), eq0("z")), false));
		tref expected = tau::build_wff_and(
			tau::build_wff_ex(qvar("1"),
				tau::build_wff_and(eq0("1"), eq0("y")), false),
			tau::build_wff_ex(qvar("1"),
				tau::build_wff_and(eq0("1"), eq0("z")), false));
		CHECK(ap::canonicalise_binder_ids<node_t>(raw) == expected);
	}
}

// ── REWRAP ──────────────────────────────────────────────────────────────────

TEST_SUITE("anti_prenex/foundations/prims: rewrap") {

	TEST_CASE("wraps in X's order, X[0] outermost, without renaming") {
		tref phi = eq0("a");
		tref got = ap::rewrap<node_t>(phi, { qvar("x"), qvar("y") });
		tref expected = tau::build_wff_ex(qvar("x"),
			tau::build_wff_ex(qvar("y"), phi, false), false);
		CHECK(got == expected);
	}

	TEST_CASE("permutations of X give different nodes (the D3 key)") {
		tref phi = eq0("a");
		CHECK(ap::rewrap<node_t>(phi, { qvar("x"), qvar("y") })
			!= ap::rewrap<node_t>(phi, { qvar("y"), qvar("x") }));
	}

	TEST_CASE("an empty block returns the formula itself") {
		tref phi = eq0("a");
		CHECK(ap::rewrap<node_t>(phi, {}) == phi);
	}

	TEST_CASE("the kind parameter selects the binder") {
		tref phi = eq0("a");
		CHECK(ap::rewrap<node_t>(phi, { qvar("x") }, tau_term_bdd<node_t>::all)
			== tau::build_wff_all(qvar("x"), phi, false));
		CHECK(ap::rewrap<node_t>(phi, { qvar("x") }, tau_term_bdd<node_t>::all)
			!= ap::rewrap<node_t>(phi, { qvar("x") }));
	}

	TEST_CASE("keeps ids, so a re-wrap is not a fresh numbering") {
		// The body already binds "1"; build_wff_ex(..., true) would mint
		// "2" for the outer binder. rewrap must not: ground rule 4.
		tref inner = tau::build_wff_ex(qvar("1"), eq0("1"), false);
		tref got = ap::rewrap<node_t>(inner, { qvar("1") });
		CHECK(bound_name(got) == "1");
	}
}

// ── FOLD_DEGENERATE_BINDERS ─────────────────────────────────────────────────

TEST_SUITE("anti_prenex/foundations/prims: fold_degenerate_binders") {

	TEST_CASE("drops a binder over an absent variable") {
		tref phi = eq0("a");
		tref raw = tau::build_wff_ex(qvar("x"), phi, false);
		CHECK(ap::fold_degenerate_binders<node_t>(raw) == phi);
	}

	TEST_CASE("drops a whole cascade in one pass") {
		tref phi = eq0("a");
		tref raw = tau::build_wff_ex(qvar("x"),
			tau::build_wff_all(qvar("y"), phi, false), false);
		CHECK(ap::fold_degenerate_binders<node_t>(raw) == phi);
	}

	TEST_CASE("drops a binder over a constant scope") {
		tref raw = tau::build_wff_ex(qvar("x"), tau::_T(), false);
		CHECK(ap::fold_degenerate_binders<node_t>(raw) == tau::_T());
	}

	TEST_CASE("leaves a used binder pointer-identical") {
		tref raw = tau::build_wff_ex(qvar("x"), eq0("x"), false);
		CHECK(ap::fold_degenerate_binders<node_t>(raw) == raw);
	}

	TEST_CASE("drops a vacuous functional quantifier too") {
		tref phi = tau::build_bf_eq_0(tvar("a"));
		tref raw = tau::build_bf_eq_0(
			tau::build_bf_fex(qvar("y"), tvar("a"), false));
		CHECK(ap::fold_degenerate_binders<node_t>(raw) == phi);
	}

	TEST_CASE("leaves a used functional quantifier pointer-identical") {
		tref raw = tau::build_bf_eq_0(
			tau::build_bf_fex(qvar("y"), tvar("y"), false));
		CHECK(ap::fold_degenerate_binders<node_t>(raw) == raw);
	}

	TEST_CASE("folds a binder nested under a live one") {
		// ex x. (x = 0 & ex y. a = 0) -- only the inner binder goes.
		tref inner = tau::build_wff_ex(qvar("y"), eq0("a"), false);
		tref raw = tau::build_wff_ex(qvar("x"),
			tau::build_wff_and(eq0("x"), inner), false);
		tref expected = tau::build_wff_ex(qvar("x"),
			tau::build_wff_and(eq0("x"), eq0("a")), false);
		CHECK(ap::fold_degenerate_binders<node_t>(raw) == expected);
	}
}
