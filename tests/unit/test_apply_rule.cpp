// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Direct unit coverage for rewriter::apply_rule and rewriter::apply_rule_guarded
// (external/parser/src/utility/tree_rewriter.tmpl.h) as used from tau-lang's own
// code: see nso_rr_apply(const rewriter::rule&, const tref&) in
// src/nso_rr.tmpl.h, which calls rewriter::apply_rule<node, decltype(is_capture)>
// with the very same "a node is a capture iff its nonterminal is `capture`"
// predicate reproduced below. apply_rule_guarded is not currently called from
// tau-lang's own source, but is part of the same rewriting API surface and is
// exercised here directly with tau-lang's node/tree types and a capture
// predicate, per RR-5.
//
// The synthetic patterns below wrap capture/variable leaves in a `ref_args`
// node purely as a neutral, arity-N container to build multi-node trees for
// apply_rule to walk. `ref_args` is deliberately chosen because
// tree<node>::get() (src/tau_tree.tmpl.h) auto-propagates a compound node's
// BA type from its children for every nonterminal *except* wff, fp_fallback
// and ref_args -- so a `ref_args` wrapper's own BA type stays 0 regardless of
// what typed content (e.g. a real bf variable) is placed inside it. Using a
// nonterminal that *does* auto-propagate (e.g. offset/shift) would make a
// pattern's ref_args(capture) wrapper (BA type 0, since captures are
// untyped) silently fail to structurally match an input's ref_args(bf-typed
// variable) wrapper (a non-zero, propagated BA type), even though the rule
// itself is otherwise correct.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "nso_rr.h"

namespace {

// Mirrors the capture predicate tau-lang installs in nso_rr_apply
// (src/nso_rr.tmpl.h): a node is a capture iff its nonterminal is `capture`.
bool is_capture(const tref& n) {
	return tau::get(n).is(node_t::type::capture);
}

// Builds a leaf bf variable (mirrors the helper in tests/unit/test_execution.cpp).
tref bf_var(const char* name) {
	return build_bf_variable<node_t>(name, tau_type_id<node_t>());
}

// Builds a named capture leaf. Two calls with the same name intern to the
// very same tref (tau's hash-consing of leaf nodes), which is what lets a
// single named capture appear more than once in a pattern and require both
// occurrences to bind to structurally equal subtrees.
tref capture(const char* name) {
	return tau::get(tau::capture, name);
}

rewriter::rule make_rule(tref pattern, tref body) {
	return { tau::geth(pattern), tau::geth(body) };
}

} // namespace

TEST_SUITE("apply_rule") {

	TEST_CASE("matches a pattern with a single capture and substitutes it into the body") {
		// pattern: ref_args($X)   body: $X
		tref X = capture("X");
		tref x = bf_var("x");
		rewriter::rule unwrap = make_rule(tau::get(tau::ref_args, X), X);

		tref input = tau::get(tau::ref_args, x);
		tref result = rewriter::apply_rule<node_t>(unwrap, input, is_capture);

		CHECK( result == x );
	}

	TEST_CASE("repeated occurrences of the same named capture must bind to equal subtrees") {
		// pattern: ref_args($X, $X)   body: $X  -- only matches when both
		// children are the very same subtree (capture consistency across
		// repeated occurrences of one capture name).
		tref X = capture("X");
		rewriter::rule collapse = make_rule(tau::get(tau::ref_args, { X, X }), X);

		tref x = bf_var("x");
		tref matching_input = tau::get(tau::ref_args, { x, x });
		tref result = rewriter::apply_rule<node_t>(collapse, matching_input,
			is_capture);

		CHECK( result == x );
	}

	TEST_CASE("mismatching capture bindings leave the input unchanged") {
		tref X = capture("X");
		rewriter::rule collapse = make_rule(tau::get(tau::ref_args, { X, X }), X);

		tref x = bf_var("x");
		tref y = bf_var("y");
		// The two operands differ, so the repeated capture $X cannot
		// bind consistently and the rule must not fire anywhere.
		tref mismatching_input = tau::get(tau::ref_args, { x, y });
		tref result = rewriter::apply_rule<node_t>(collapse, mismatching_input,
			is_capture);

		CHECK( result == mismatching_input );
	}

	TEST_CASE("a pattern that matches nowhere in the tree leaves it unchanged") {
		tref X = capture("X");
		tref x = bf_var("x");
		rewriter::rule unwrap = make_rule(tau::get(tau::ref_args, X), X);

		// x is not wrapped in a ref_args node anywhere, so the pattern
		// never matches and apply_rule must return the input as-is.
		tref result = rewriter::apply_rule<node_t>(unwrap, x, is_capture);

		CHECK( result == x );
	}
}

TEST_SUITE("apply_rule_guarded") {

	TEST_CASE("an accepting guard sees the bound capture and lets the rewrite happen") {
		tref X = capture("X");
		tref x = bf_var("x");
		rewriter::rule unwrap = make_rule(tau::get(tau::ref_args, X), X);
		tref input = tau::get(tau::ref_args, x);

		// Accepts iff the capture $X was bound to `x`, which is what
		// the structural match above should produce.
		auto accept = [X, x](const rewriter::environment<node_t>& env, tref) {
			auto it = env.find(X);
			return it != env.end() && it->second == x;
		};

		tref result = rewriter::apply_rule_guarded<node_t>(unwrap, input,
			is_capture, accept);

		CHECK( result == x );
	}

	TEST_CASE("a rejecting guard blocks the rewrite even though the pattern matches") {
		tref X = capture("X");
		tref x = bf_var("x");
		tref y = bf_var("y");
		rewriter::rule unwrap = make_rule(tau::get(tau::ref_args, X), X);
		tref input = tau::get(tau::ref_args, x);

		// The guard only accepts if the captured subtree is `y`, but the
		// structural match actually captures `x`, so it must reject and
		// the rewrite must not happen.
		auto reject = [X, y](const rewriter::environment<node_t>& env, tref) {
			auto it = env.find(X);
			return it != env.end() && it->second == y;
		};

		tref result = rewriter::apply_rule_guarded<node_t>(unwrap, input,
			is_capture, reject);

		CHECK( result == input );
	}
}
