// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// AP-22: get_hook<node> (src/hooks.h/.tmpl.h) is the construction-time
// rewrite dispatcher invoked by tree<node> every time a node is built; no
// test file anywhere exercised it before. get_hook itself is private
// (`friend tree<node>`), so it cannot be called directly -- it is exercised
// here the way it is exercised in production: by parsing source text through
// the public API and checking that the resulting tree is the
// already-reduced/desugared form the corresponding hook rule produces,
// rather than the literal node the surface syntax would naively suggest.
//
// tau_tree<node> hash-conses nodes, so two independently-parsed formulas
// that denote the same reduced tree compare as tref-equal; that equality is
// used throughout below instead of comparing to_str() output, mirroring how
// the hooks themselves compare subtrees (e.g. `arg1_fm(ch) == arg2_fm(ch)`).

#include "test_init.h"
#include "test_tau_helpers.h"

using tau_api = api<node_t>;

namespace {

tref fm(const char* src) {
	tref f = tau_api::get_formula(src);
	REQUIRE(f);
	return f;
}

} // namespace

TEST_SUITE("hooks: wff_imply") {

	TEST_CASE("F -> $X ::= T") {
		CHECK(fm("F -> x = 0") == fm("T"));
	}
	TEST_CASE("T -> $X ::= $X") {
		CHECK(fm("T -> x = 0") == fm("x = 0"));
	}
	TEST_CASE("$X -> F ::= ! $X") {
		CHECK(fm("(x = 0) -> F") == fm("!(x = 0)"));
	}
	TEST_CASE("$X -> T ::= T") {
		CHECK(fm("(x = 0) -> T") == fm("T"));
	}
	TEST_CASE("$X -> $X ::= T") {
		CHECK(fm("(x = 0) -> (x = 0)") == fm("T"));
	}
	TEST_CASE("a non-degenerate implication is left as-is") {
		CHECK(fm("(x = 0) -> (y = 0)") != fm("T"));
		CHECK(fm("(x = 0) -> (y = 0)") != fm("F"));
	}
}

TEST_SUITE("hooks: wff_rimply") {

	// $X <- $Y is built as $Y -> $X (see get_hook<node>::wff_rimply's
	// general-case fallthrough), so a non-degenerate reverse implication
	// must be tref-identical to the equivalent forward implication with
	// swapped operands.
	TEST_CASE("$X <- $Y ::= $Y -> $X") {
		CHECK(fm("(x = 0) <- (y = 0)") == fm("(y = 0) -> (x = 0)"));
	}
	// NOTE: get_hook<node>::wff_rimply's own RULE(...) comments in
	// hooks.tmpl.h have their left/right operands swapped relative to
	// what the code actually checks (e.g. the comment for the
	// `arg1(ch).is(tau::wff_f)` branch reads "F <- $X ::= T." but arg1 is
	// the *left* operand of `<-`, so that branch actually fires for
	// "F <- $X" and returns "! $X", not T). The cases below name the
	// pattern after the code's real behavior, verified by construction:
	// `$X <- $Y` builds `$Y -> $X` (see the "::= $Y -> $X" case above),
	// and imply's own T/F rules are separately covered above.
	TEST_CASE("F <- $X ::= ! $X") {
		CHECK(fm("F <- (x = 0)") == fm("!(x = 0)"));
	}
	TEST_CASE("$X <- T ::= $X") {
		CHECK(fm("(x = 0) <- T") == fm("x = 0"));
	}
	TEST_CASE("T <- $X ::= T") {
		CHECK(fm("T <- (x = 0)") == fm("T"));
	}
	TEST_CASE("$X <- F ::= T") {
		CHECK(fm("(x = 0) <- F") == fm("T"));
	}
}

TEST_SUITE("hooks: wff_equiv") {

	TEST_CASE("F <-> $X ::= ! $X") {
		CHECK(fm("F <-> (x = 0)") == fm("!(x = 0)"));
	}
	TEST_CASE("T <-> $X ::= $X") {
		CHECK(fm("T <-> (x = 0)") == fm("x = 0"));
	}
	TEST_CASE("$X <-> F ::= ! $X") {
		CHECK(fm("(x = 0) <-> F") == fm("!(x = 0)"));
	}
	TEST_CASE("$X <-> T ::= $X") {
		CHECK(fm("(x = 0) <-> T") == fm("x = 0"));
	}
	TEST_CASE("$X <-> $X ::= T") {
		CHECK(fm("(x = 0) <-> (x = 0)") == fm("T"));
	}
	TEST_CASE("$X <-> ! $X ::= F") {
		CHECK(fm("(x = 0) <-> !(x = 0)") == fm("F"));
	}
	TEST_CASE("! $X <-> $X ::= F") {
		CHECK(fm("!(x = 0) <-> (x = 0)") == fm("F"));
	}
}

TEST_SUITE("hooks: wff_conditional") {

	TEST_CASE("F ? $X : $Y ::= $Y") {
		CHECK(fm("F ? (x = 0) : (y = 0)") == fm("y = 0"));
	}
	TEST_CASE("T ? $X : $Y ::= $X") {
		CHECK(fm("T ? (x = 0) : (y = 0)") == fm("x = 0"));
	}
	TEST_CASE("$X ? $Y : $Y ::= $Y") {
		CHECK(fm("(z = 0) ? (x = 0) : (x = 0)") == fm("x = 0"));
	}
	TEST_CASE("a non-degenerate conditional is left as-is") {
		tref c = fm("(z = 0) ? (x = 0) : (y = 0)");
		CHECK(c != fm("x = 0"));
		CHECK(c != fm("y = 0"));
	}
}

// NF-1's explanation (src/hooks.tmpl.h:1145 for wff_lt, :1264 for wff_lteq)
// documents that `<`/`<=` have a bitvector-specific definition, but expand
// into a generic Boolean-algebra formula for every other (non-bv) type:
//   $X < $Y  ::= ($X & !$Y) = 0 && $X != $Y
//   $X <= $Y ::= ($X & !$Y) = 0
// Plain, untyped variables in this node pack default to a non-bv type, so
// `x < y` never even reaches the bitvector fast path.
TEST_SUITE("hooks: wff_lt / wff_lteq (non-bv expansion)") {

	TEST_CASE("$X < $Y ::= ($X & !$Y) = 0 && $X != $Y for non-bv types") {
		CHECK(fm("x < y") == fm("(x & y') = 0 && x != y"));
	}
	TEST_CASE("$X <= $Y ::= ($X & !$Y) = 0 for non-bv types") {
		CHECK(fm("x <= y") == fm("(x & y') = 0"));
	}
	TEST_CASE("$X < $X ::= F") {
		CHECK(fm("x < x") == fm("F"));
	}
	TEST_CASE("$X <= $X ::= T") {
		CHECK(fm("x <= x") == fm("T"));
	}
}
