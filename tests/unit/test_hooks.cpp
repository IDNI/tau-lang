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
	auto f = tau_api::get_formula(src);
	REQUIRE(f.has_value());
	return f.value();
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

	// $X <- $Y is built as $Y -> $X, so a non-degenerate reverse implication
	// must be tref-identical to the equivalent forward implication with
	// swapped operands.
	TEST_CASE("$X <- $Y ::= $Y -> $X") {
		CHECK(fm("(x = 0) <- (y = 0)") == fm("(y = 0) -> (x = 0)"));
	}
	// `<-` shares the rules of `->` with its operands swapped.
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
	TEST_CASE("$X <- $X ::= T") {
		CHECK(fm("(x = 0) <- (x = 0)") == fm("T"));
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

// A constant pair of two distinct concrete types is a type clash: every
// constant rule of `=` and `!=` keeps the atom for type inference to report,
// instead of deciding it.
TEST_SUITE("hooks: wff_eq / wff_neq constant pairs") {

	using tau = tree<node_t>;

	TEST_CASE("constants of one type are decided") {
		const size_t s = sbf_type_id<node_t>();
		tref one = tau::build_bf_t_type(s), zero = tau::build_bf_f_type(s);
		CHECK(tau::build_bf_eq(one, zero) == tau::_F());
		CHECK(tau::build_bf_eq(zero, one) == tau::_F());
		CHECK(tau::build_bf_eq(zero, zero) == tau::_T());
		CHECK(tau::build_bf_neq(one, zero) == tau::_T());
	}
	TEST_CASE("constants of two types keep the atom") {
		const size_t s = sbf_type_id<node_t>(), t = tau_type_id<node_t>();
		tref one = tau::build_bf_t_type(s), zero = tau::build_bf_f_type(t);
		for (tref atom : { tau::build_bf_eq(one, zero),
			tau::build_bf_eq(zero, one), tau::build_bf_neq(one, zero),
			tau::build_bf_neq(zero, one) })
		{
			CHECK(atom != tau::_F());
			CHECK(atom != tau::_T());
			const bool kept = tau::get(atom).child_is(tau::bf_eq)
				|| tau::get(atom).child_is(tau::bf_neq);
			CHECK(kept);
		}
	}
}

// With 0 the bottom and 1 the top of a Boolean algebra, an order comparison
// with one constant operand is an equation; `a op b` and its mirror
// `b op' a` give the same one.
TEST_SUITE("hooks: order comparisons with a constant operand") {

	TEST_CASE("< and its mirror >") {
		CHECK(fm("0 < x") == fm("x != 0"));
		CHECK(fm("x > 0") == fm("x != 0"));
		CHECK(fm("x < 1") == fm("x' != 0"));
		CHECK(fm("1 > x") == fm("x' != 0"));
	}
	TEST_CASE("!< and its mirror !>") {
		CHECK(fm("0 !< x") == fm("x = 0"));
		CHECK(fm("x !> 0") == fm("x = 0"));
		CHECK(fm("x !< 1") == fm("x' = 0"));
		CHECK(fm("1 !> x") == fm("x' = 0"));
	}
	TEST_CASE("<= and its mirror >=") {
		CHECK(fm("x <= 0") == fm("x = 0"));
		CHECK(fm("0 >= x") == fm("x = 0"));
		CHECK(fm("1 <= x") == fm("x' = 0"));
		CHECK(fm("x >= 1") == fm("x' = 0"));
	}
	TEST_CASE("!<= and its mirror !>=") {
		CHECK(fm("x !<= 0") == fm("x != 0"));
		CHECK(fm("0 !>= x") == fm("x != 0"));
		CHECK(fm("1 !<= x") == fm("x' != 0"));
		CHECK(fm("x !>= 1") == fm("x' != 0"));
	}
	TEST_CASE("the trivial sides are T or F") {
		CHECK(fm("x < 0") == fm("F"));
		CHECK(fm("0 > x") == fm("F"));
		CHECK(fm("x <= 1") == fm("T"));
		CHECK(fm("1 >= x") == fm("T"));
		CHECK(fm("0 !<= x") == fm("F"));
		CHECK(fm("x !>= 0") == fm("F"));
	}
}
