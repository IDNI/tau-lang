// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "boolean_algebras/bv/bv_ba.h"


TEST_SUITE("Configuration") {

	TEST_CASE("logging") {
		logging::trace();
	}

	TEST_CASE("bdd init") {
		bdd_init<Bool>();
	}
}

tref parse_wff(const std::string& sample) {
	auto opts = tau::get_options{
		.parse = { .start = tau::wff },
		.reget_with_hooks = true
	};
	tref src = tree<node_t>::get(sample, opts).value_or(nullptr);
	if (src == nullptr) {
		TAU_LOG_ERROR << "Parsing failed for: " << sample;
	}
	return src;
}

TEST_SUITE("bv cast - zero extension") {

	// ((bv[16]) X:bv[8]) widens X from 8 to 16 bits (zero-fills high bits)
	TEST_CASE("zero-extend sat: ((bv[16]) X:bv[8]) = { 5 }:bv[16]") {
		auto src = parse_wff("((bv[16]) X:bv[8]) = { 5 }:bv[16]");
		REQUIRE( src != nullptr );
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// zero-extend produces a wider value; the narrow variable can't equal a
	// value that requires more than 8 bits (>255), so this is unsat
	TEST_CASE("zero-extend unsat: ((bv[16]) X:bv[8]) = { 256 }:bv[16]") {
		auto src = parse_wff("((bv[16]) X:bv[8]) = { 256 }:bv[16]");
		REQUIRE( src != nullptr );
		CHECK( is_bv_formula_unsat<node_t>(src) );
	}

	// valid: zero-extending anything keeps value in [0, 255]
	TEST_CASE("zero-extend valid: ((bv[16]) X:bv[8]) <= { 255 }:bv[16]") {
		auto src = parse_wff("((bv[16]) X:bv[8]) <= { 255 }:bv[16]");
		REQUIRE( src != nullptr );
		CHECK( is_bv_formula_valid<node_t>(src) );
	}

	// cast of constant: ((bv[16]) {5}:bv[8]) = {5}:bv[16]
	TEST_CASE("zero-extend constant: ((bv[16]) {5}:bv[8]) = { 5 }:bv[16]") {
		auto src = parse_wff("((bv[16]) { 5 }:bv[8]) = { 5 }:bv[16]");
		REQUIRE( src != nullptr );
		CHECK( is_bv_formula_valid<node_t>(src) );
	}
}

TEST_SUITE("bv cast - truncation") {

	// truncating to fewer bits discards high bits
	TEST_CASE("truncate sat: ((bv[8]) X:bv[16]) = { 5 }:bv[8]") {
		auto src = parse_wff("((bv[8]) X:bv[16]) = { 5 }:bv[8]");
		REQUIRE( src != nullptr );
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// truncation is always satisfiable (multiple inputs map to same output)
	TEST_CASE("truncate sat: ((bv[4]) X:bv[8]) = { 15 }:bv[4]") {
		auto src = parse_wff("((bv[4]) X:bv[8]) = { 15 }:bv[4]");
		REQUIRE( src != nullptr );
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// truncation of constant: low 8 bits of 0x1FF (511) = 0xFF (255)
	TEST_CASE("truncate constant: ((bv[8]) {255}:bv[16]) = { 255 }:bv[8]") {
		auto src = parse_wff("((bv[8]) { 255 }:bv[16]) = { 255 }:bv[8]");
		REQUIRE( src != nullptr );
		CHECK( is_bv_formula_valid<node_t>(src) );
	}
}

TEST_SUITE("bv cast - no-op (same width)") {

	TEST_CASE("same-width cast sat: ((bv[8]) X:bv[8]) = { 42 }:bv[8]") {
		auto src = parse_wff("((bv[8]) X:bv[8]) = { 42 }:bv[8]");
		REQUIRE( src != nullptr );
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}
}

TEST_SUITE("bv cast - widthless cast") {

	// Regression: `(bv)` names the family but carries no bitwidth of its
	// own. Inference completes it from the operand's own bv[8] annotation;
	// before that write-back existed, the incomplete cast reached the
	// solver and crashed in get_bv_size on a type tree with no
	// explicit bitwidth.
	TEST_CASE("widthless cast solves: (bv) x:bv[8] = 0") {
		auto src = parse_wff("(bv) x:bv[8] = 0");
		REQUIRE( src != nullptr );
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// Regression: the width lives on the OTHER side of `=` from the cast.
	// A group-level fix must complete the cast from whichever member
	// carries the width, not only from its own operand, so this solves
	// whatever order the atom's members are visited in.
	TEST_CASE("widthless cast solves, width on the other side: x:bv = (bv) y:bv[8]") {
		auto src = parse_wff("x:bv = (bv) y:bv[8]");
		REQUIRE( src != nullptr );
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// The cast's operand is itself a bare, widthless annotation and
	// nothing anywhere supplies a bitwidth: a family-only annotation is
	// incomplete, not wrong, but with no width to borrow it is a type
	// error, the same as `x:bv = { 5 }:bv` does outside any cast.
	TEST_CASE("widthless cast, widthless operand, no width anywhere: (bv) x:bv = 0") {
		auto src = parse_wff("(bv) x:bv = 0");
		CHECK( src == nullptr );
	}

	// Regression: the cast sits under an arithmetic operator, not directly
	// under `=`. The width must still be found and applied to every
	// sibling in the same atom (here, y).
	TEST_CASE("widthless cast under an arithmetic operand: ((bv) x:bv[8]) + y = 0") {
		auto src = parse_wff("((bv) x:bv[8]) + y = 0");
		REQUIRE( src != nullptr );
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// Regression: the operand is a bare-annotated CONSTANT literal, not a
	// variable. The width lives in x:bv[8] on the other side of `=`; a
	// bare constant could not even be parsed under its own family-only
	// type (no bitwidth to size the value with), so it stayed
	// unevaluated until this width completes it.
	TEST_CASE("widthless cast, bare constant operand, width elsewhere: (bv) { 5 }:bv = x:bv[8]") {
		auto src = parse_wff("(bv) { 5 }:bv = x:bv[8]");
		REQUIRE( src != nullptr );
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// A cast that already carries its own width is a deliberate width
	// change, not an incomplete annotation, and must stay untouched.
	TEST_CASE("cast with its own width is untouched: (bv[16]) x:bv[8] = 0") {
		auto src = parse_wff("(bv[16]) x:bv[8] = 0");
		REQUIRE( src != nullptr );
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// The cast's operand carries no annotation at all (bare `x`, not
	// `x:bv`), so the operand's own untyped-leaf check rejects it before
	// this pass ever runs: a clean type error, unaffected by whether the
	// pack has a default width for the cast's own family.
	TEST_CASE("widthless cast, fully untyped operand, no width anywhere: (bv) x = 0") {
		auto src = parse_wff("(bv) x = 0");
		CHECK( src == nullptr );
	}

	// No width appears anywhere for this cast, and none of its operand's
	// own annotations or an enclosing binder supplies one either: a clean
	// type error, same as a fully untyped element.
	TEST_CASE("cast with no width anywhere is a clean type error: (bv) x = y") {
		auto src = parse_wff("(bv) x = y");
		CHECK( src == nullptr );
	}
}

//
// REVIEW (nested casting): confirm the CVC5 solver path supports nested
// casts.
//
TEST_SUITE("bv cast - nested") {

	// widen twice: bv[4] -> bv[8] -> bv[16]
	TEST_CASE("nested widen sat: ((bv[16]) ((bv[8]) X:bv[4])) = { 5 }:bv[16]") {
		auto src = parse_wff("((bv[16]) ((bv[8]) X:bv[4])) = { 5 }:bv[16]");
		REQUIRE( src != nullptr );
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// X:bv[4] <= 15, so the doubly-widened value can never be 16
	TEST_CASE("nested widen unsat: ((bv[16]) ((bv[8]) X:bv[4])) = { 16 }:bv[16]") {
		auto src = parse_wff("((bv[16]) ((bv[8]) X:bv[4])) = { 16 }:bv[16]");
		REQUIRE( src != nullptr );
		CHECK( is_bv_formula_unsat<node_t>(src) );
	}

	// widen then truncate: bv[8] -> bv[16] -> bv[4]
	TEST_CASE("nested widen-trunc sat: ((bv[4]) ((bv[16]) X:bv[8])) = { 5 }:bv[4]") {
		auto src = parse_wff("((bv[4]) ((bv[16]) X:bv[8])) = { 5 }:bv[4]");
		REQUIRE( src != nullptr );
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// truncate then widen: after bv[4] the value is <= 15, can never be 16
	TEST_CASE("nested trunc-widen unsat: ((bv[16]) ((bv[4]) X:bv[8])) = { 16 }:bv[16]") {
		auto src = parse_wff("((bv[16]) ((bv[4]) X:bv[8])) = { 16 }:bv[16]");
		REQUIRE( src != nullptr );
		CHECK( is_bv_formula_unsat<node_t>(src) );
	}

	// nested cast of a constant should fold to a constant (hook path)
	TEST_CASE("nested constant valid: ((bv[16]) ((bv[8]) { 5 }:bv[4])) = { 5 }:bv[16]") {
		auto src = parse_wff("((bv[16]) ((bv[8]) { 5 }:bv[4])) = { 5 }:bv[16]");
		REQUIRE( src != nullptr );
		CHECK( is_bv_formula_valid<node_t>(src) );
	}

	// nested cast without inner parentheses (grammar check)
	TEST_CASE("nested no parens sat: ((bv[16]) (bv[8]) X:bv[4]) = { 5 }:bv[16]") {
		auto src = parse_wff("((bv[16]) (bv[8]) X:bv[4]) = { 5 }:bv[16]");
		REQUIRE( src != nullptr );
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}
}

//
// REVIEW (cast of bounded quantifiers): bf_fall and bf_fex were accidentally
// omitted from the bf_cast_oprnd alternatives when the right-lazy grammar
// change was made. This suite documents and tests the regression.
//
TEST_SUITE("bv cast - quantifier operand") {

	// (bv[N]) fall x x — cast of a bounded universal quantifier.
	// In the old grammar any bf was accepted as cast operand; the right-lazy
	// rewrite restricted the list but forgot bf_fall and bf_fex.
	// The bound variable is annotated: a cast operand is typed from its
	// own annotations (or an enclosing binder), and `(bv[8]) fall x x`
	// with x untyped is rejected by inference since the 2026-08-27 grammar
	// pass (before it, `(bv[8])` "parsed" as the juxtaposition `b v[8]`).
	TEST_CASE("parse: (bv[8]) fall x x = { 1 }:bv[8]") {
		auto src = parse_wff("(bv[8]) fall x:bv[4] x = { 1 }:bv[8]");
		CHECK( src != nullptr );
	}

	TEST_CASE("parse: (bv[8]) fex x x = { 1 }:bv[8]") {
		auto src = parse_wff("(bv[8]) fex x:bv[4] x = { 1 }:bv[8]");
		CHECK( src != nullptr );
	}
}

TEST_SUITE("bv cast - negated constant") {

	// Helper: returns true when the tree still contains a bf_cast node.
	// After constant-folding hooks run the cast should be gone.
	auto has_cast = [](tref t) -> bool {
		return tau::get(t).find_top(is<node_t, tau::bf_cast>) != nullptr;
	};

	// {5}:bv[4] = 0101, bitwise NOT in 4 bits = 1010 = 10
	// zero-extend to bv[8]: {10}:bv[8]
	TEST_CASE("zext of negated const: ((bv[8]) ({5}:bv[4])') folds to {10}:bv[8]") {
		auto src = parse_wff("((bv[8]) ({ 5 }:bv[4])') = { 10 }:bv[8]");
		REQUIRE( src != nullptr );
		// Hooks should have constant-folded the cast: no bf_cast should remain
		CHECK( !has_cast(src) );
		CHECK( is_bv_formula_valid<node_t>(src) );
	}

	// {0}:bv[4] complement = {15}:bv[4], zero-extended = {15}:bv[8]
	TEST_CASE("zext of negated zero: ((bv[8]) ({0}:bv[4])') folds to {15}:bv[8]") {
		auto src = parse_wff("((bv[8]) ({ 0 }:bv[4])') = { 15 }:bv[8]");
		REQUIRE( src != nullptr );
		CHECK( !has_cast(src) );
		CHECK( is_bv_formula_valid<node_t>(src) );
	}

	// truncate of negated const: {245}:bv[8] = 11110101, complement = 00001010 = 10
	// truncate to 4 bits: {10}:bv[4]
	TEST_CASE("trunc of negated const: ((bv[4]) ({245}:bv[8])') folds to {10}:bv[4]") {
		auto src = parse_wff("((bv[4]) ({ 245 }:bv[8])') = { 10 }:bv[4]");
		REQUIRE( src != nullptr );
		CHECK( !has_cast(src) );
		CHECK( is_bv_formula_valid<node_t>(src) );
	}

	// same-width negated constant: ((bv[8]) ({5}:bv[8])') = {250}:bv[8]
	// {5} = 00000101, complement = 11111010 = 250
	TEST_CASE("same-width cast of negated const: ((bv[8]) ({5}:bv[8])') folds to {250}:bv[8]") {
		auto src = parse_wff("((bv[8]) ({ 5 }:bv[8])') = { 250 }:bv[8]");
		REQUIRE( src != nullptr );
		CHECK( !has_cast(src) );
		CHECK( is_bv_formula_valid<node_t>(src) );
	}
}

TEST_SUITE("bv cast - quantifier operand - unambiguous") {

	// Use parentheses around the entire cast expression to force it to be
	// parsed as bf_parenthesis(bf_cast(fall x x)), not as any other construct.
	// This MUST fail before adding bf_fall/bf_fex to bf_cast_oprnd.
	TEST_CASE("parse forced: ((bv[8]) fall x x) = { 1 }:bv[8]") {
		auto src = parse_wff("((bv[8]) fall x:bv[4] x) = { 1 }:bv[8]");
		CHECK( src != nullptr );
	}
}

// TY-20: the content of a `{...}:bv[N]` ba_constant is parsed by a
// dedicated sub-grammar (parser/bitvector.tgf -> bitvector_parser.generated.h,
// used from bv_constant_from_parse_tree() in src/boolean_algebras/bv_ba.tmpl.h)
// that accepts three literal forms: plain decimal digits (already covered by
// every other test in this file), "#b" + binary digits, and "#x" + hex
// digits. This is tau's own literal syntax (not borrowed from cvc5's
// SMT-LIB "#x..." notation, even though it happens to look the same) --
// confirmed by reading parser/bitvector.tgf:
//   bitvector => "#b" binary | decimal | "#x" hexadecimal.
// This suite exercises the "#x" hexadecimal form and checks it is numerically
// equivalent to the matching decimal literal.
TEST_SUITE("bv literal - hexadecimal form") {

	TEST_CASE("hex literal: { #xF }:bv[8] equals decimal { 15 }:bv[8]") {
		auto src = parse_wff("{ #xF }:bv[8] = { 15 }:bv[8]");
		REQUIRE( src != nullptr );
		CHECK( is_bv_formula_valid<node_t>(src) );
	}

	TEST_CASE("hex literal: { #xFF }:bv[8] equals decimal { 255 }:bv[8]") {
		auto src = parse_wff("{ #xFF }:bv[8] = { 255 }:bv[8]");
		REQUIRE( src != nullptr );
		CHECK( is_bv_formula_valid<node_t>(src) );
	}

	TEST_CASE("hex literal: { #x0A }:bv[4] equals decimal { 10 }:bv[4]") {
		auto src = parse_wff("{ #x0A }:bv[4] = { 10 }:bv[4]");
		REQUIRE( src != nullptr );
		CHECK( is_bv_formula_valid<node_t>(src) );
	}

	// Lower-case hex digits must also be accepted (xdigit char class).
	TEST_CASE("hex literal: { #xff }:bv[8] equals decimal { 255 }:bv[8]") {
		auto src = parse_wff("{ #xff }:bv[8] = { 255 }:bv[8]");
		REQUIRE( src != nullptr );
		CHECK( is_bv_formula_valid<node_t>(src) );
	}
}
