// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"
#include "parser_helper.h"

#include "boolean_algebras/bv_ba.h"

TEST_SUITE("configuration") {

	TEST_CASE("bdd_init") {
		bdd_init<Bool>();
	}

	TEST_CASE("logging") {
		logging::trace();
	}
}

tref parse_bf(const std::string& sample) {
	static tree<node_t>::get_options opts{ .parse = { .start = tree<node_t>::bf }};
	auto src = tree<node_t>::get(sample, opts);
	if (src == nullptr) {
		TAU_LOG_ERROR << "Parsing failed for: " << sample;
	}
	return src;
}

// Asserts the exact simplification result: `sample` must simplify to the tree
// that `expected` parses to. Both forms are printed so a mismatch is
// diagnosable straight from the test log rather than only as "not equal".
static void check_simplifies_to(const std::string& sample,
	const std::string& expected)
{
	tref src = parse_bf(sample);
	REQUIRE(src != nullptr);
	tref simplified = bv_ba_custom_simplification<node_t>(src);
	REQUIRE(simplified != nullptr);
	tref exp = parse_bf(expected);
	REQUIRE(exp != nullptr);
	std::cout << "sample:     " << sample
		<< "\n  simplified: " << tree<node_t>::get(simplified).to_str()
		<< "\n  expected:   " << tree<node_t>::get(exp).to_str() << "\n";
	CHECK(tree<node_t>::get(simplified) == tree<node_t>::get(exp));
}

// Asserts that no sound fold applies and the term is returned untouched.
static void check_unchanged(const std::string& sample) {
	tref src = parse_bf(sample);
	REQUIRE(src != nullptr);
	tref simplified = bv_ba_custom_simplification<node_t>(src);
	REQUIRE(simplified != nullptr);
	std::cout << "sample:     " << sample
		<< "\n  simplified: " << tree<node_t>::get(simplified).to_str()
		<< "\n  expected:   (unchanged)\n";
	CHECK(tree<node_t>::get(simplified) == tree<node_t>::get(src));
}

TEST_SUITE("simplify_blocks") {

	TEST_CASE("additive block is simplified at the root") {
		tref src = parse_bf("{1}:bv[8] + X:bv[8] + {2}:bv[8]");
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		REQUIRE(simplified != nullptr);
		std::cout << "Simplified result: " << tree<node_t>::get(simplified).to_str() << "\n";
		CHECK(matches_to_str_to_any_of(simplified, {
			"X+{ 3 }:bv[8]",  // Canonical form in debug
			"X:bv[8]+{ 3 }:bv[8]",
			"{ 3 }:bv[8]+X:bv[8]",
			"X:bv[8] + { 3 }:bv[8]",
			"{ 3 }:bv[8] + X:bv[8]",
			"(X+{ 3 }:bv[8])"
		}));
	}

}

TEST_SUITE("ba bv custom simplification") {

	// Chained constant addition (should fully flatten)
	TEST_CASE("chained addition flattening") {
		const char* sample = "{1}:bv[8] + {2}:bv[8] + {3}:bv[8]";
		tref src = tau::get(sample, parse_opts_bf);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		// Should be {6}:bv[8] after multi-pass
		const char* expected_str = "{6}:bv[8]";
		tref expected = tau::get(expected_str, parse_opts_bf);
		CHECK(simplified != nullptr);
		CHECK(tree<node_t>::get(simplified) == tree<node_t>::get(expected));
	}

	// Chained constant subtraction with cancellation
	TEST_CASE("chained subtraction cancellation") {
		const char* sample = "{5}:bv[8] - {2}:bv[8] + {2}:bv[8]";
		tref src = tau::get(sample, parse_opts_bf);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		const char* expected_str = "{5}:bv[8]";
		tref expected = tau::get(expected_str, parse_opts_bf);
		CHECK(simplified != nullptr);
		CHECK(tree<node_t>::get(simplified) == tree<node_t>::get(expected));
	}

	// Division by zero (is top)
	TEST_CASE("division by zero") {
		using node = node_t;

		const char* sample = "{5}:bv[8] / {0}:bv[8]";
		tref src = tau::get(sample, parse_opts_bf);
		const char* expected = "1:bv[8]";
		tref exp = tau::get(expected, parse_opts_bf);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		// Accept nullptr or a special error node, but must not crash
		DBG( LOG_TRACE << "simplified: " << (simplified == nullptr ? "nullptr" : tree<node>::get(simplified).tree_to_str()) << "\n" );
		CHECK(tree<node>::get(simplified) == tree<node>::get(exp));
	}

	// Division by zero (is top)
	TEST_CASE("division by zero with variables") {
		using node = node_t;

		const char* sample = "x:bv[8] / {0}:bv[8]";
		tref src = tau::get(sample, parse_opts_bf);
		const char* expected = "1:bv[8]";
		tref exp = tau::get(expected, parse_opts_bf);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		// Accept nullptr or a special error node, but must not crash
		DBG( LOG_TRACE << "simplified: " << (simplified == nullptr ? "nullptr" : tree<node>::get(simplified).tree_to_str()) << "\n" );
		CHECK(tree<node>::get(simplified) == tree<node>::get(exp));
	}

	// Division by zero (is top)
	TEST_CASE("division by zero with constants") {
		using node = node_t;

		const char* sample = "{5}:bv[8] / {0}:bv[8]";
		tref src = tau::get(sample, parse_opts_bf);
		const char* expected = "1:bv[8]";
		tref exp = tau::get(expected, parse_opts_bf);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		// Accept nullptr or a special error node, but must not crash
		DBG( LOG_TRACE << "simplified: " << (simplified == nullptr ? "nullptr" : tree<node>::get(simplified).tree_to_str()) << "\n" );
		CHECK(tree<node>::get(simplified) == tree<node>::get(exp));
	}

	// All variables (should not introduce neutral element)
	TEST_CASE("all variables no neutral element") {
		const char* sample = "x:bv[8] + y:bv[8]";
		tref src = tau::get(sample, parse_opts_bf);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		// Should not be 0 + x + y
		CHECK(simplified != nullptr);
		std::string str = tree<node_t>::get(simplified).tree_to_str();
		CHECK(str.find("0:bv[8]") == std::string::npos);
	}

	// Neutral element elimination (0 + x)
	TEST_CASE("neutral element elimination") {
		const char* sample = "0:bv[8] + x:bv[8]";
		tref src = tau::get(sample, parse_opts_bf);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		const char* expected = "x:bv[8]";
		tref exp = tau::get(expected, parse_opts_bf);
		CHECK(simplified != nullptr);
		CHECK(tree<node_t>::get(simplified) == tree<node_t>::get(exp));
	}

	// 1:bv[8] = 11111111
	TEST_CASE("top element multiplication") {
		const char* sample = "1:bv[8] * x:bv[8]";
		tref src = tau::get(sample, parse_opts_bf);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		CHECK(simplified != nullptr);
		CHECK(matches_to_str_to_any_of(simplified, { "x*1", "1*x", "(x*1)" }));
	}

	TEST_CASE("1 + 2") {
		const char* sample = "{1}:bv[64] + {2}:bv[64]";
		tref src = tau::get(sample, parse_opts_bf);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		tref expected = parse_bf("{3}:bv[64]");
		REQUIRE(simplified != nullptr);
		CHECK(tree<node_t>::get(simplified) == tree<node_t>::get(expected));
	}

	TEST_CASE("1 * 2") {
		const char* sample = "{1}:bv[64] * {2}:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		tref expected = parse_bf("{2}:bv[64]");
		REQUIRE(simplified != nullptr);
		CHECK(tree<node_t>::get(simplified) == tree<node_t>::get(expected));
	}

	TEST_CASE("1 + X") {
		const char* sample = "{1}:bv[64] + X:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		REQUIRE(simplified != nullptr);
		std::cout << "Simplified result: " << tree<node_t>::get(simplified).to_str() << "\n";
		CHECK(matches_to_str_to_any_of(simplified, { "X+{ 1 }:bv[64]", "{ 1 }:bv[64]+X", "X:bv[64]+{ 1 }:bv[64]", "(X+{ 1 }:bv[64])" }));
	}

	TEST_CASE("1 - X") {
		const char* sample = "{1}:bv[64] - X:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		REQUIRE(simplified != nullptr);
		std::cout << "Simplified result: " << tree<node_t>::get(simplified).to_str() << "\n";
		CHECK(matches_to_str_to_any_of(simplified, { "0-X+{ 1 }:bv[64]", "{ 1 }:bv[64]-X", "{ 1 }:bv[64]-X:bv[64]", "0:bv[64]-X:bv[64]+{ 1 }:bv[64]", "(0-X+{ 1 }:bv[64])" }));
	}

	// The multiplicative family is folded only when the block is a pure
	// product: `*` is associative and commutative in Z/2^n, so its operands
	// may be regrouped, but bvudiv is not the inverse of bvmul (`X*Y/Y != X`
	// as soon as `X*Y` wraps, and `X/X` is all ones at X = 0) and division
	// does not reassociate, so any block containing a `/` is left alone.

	TEST_CASE("1 * X") {
		check_simplifies_to("{1}:bv[64] * X:bv[64]",
			"X:bv[64] * {1}:bv[64]");
	}

	TEST_CASE("2 * X") {
		check_simplifies_to("{2}:bv[64] * X:bv[64]",
			"X:bv[64] * {2}:bv[64]");
	}

	// HE-2: `{1} / X` used to fold to `(0 / X) * {1}`, which is 0 for every X
	// while the source is 1 whenever X = 1.
	TEST_CASE("1 / X") {
		check_unchanged("{1}:bv[64] / X:bv[64]");
	}

	TEST_CASE("1 + X + 2") {
		check_simplifies_to("{1}:bv[64] + X:bv[64] + {2}:bv[64]",
			"X:bv[64] + {3}:bv[64]");
	}

	TEST_CASE("1 * X * 2") {
		check_simplifies_to("{1}:bv[64] * X:bv[64] * {2}:bv[64]",
			"X:bv[64] * {2}:bv[64]");
	}

	TEST_CASE("1 - X - 2") {
		check_simplifies_to("{1}:bv[64] - X:bv[64] - {2}:bv[64]",
			"0:bv[64] - X:bv[64] + 1:bv[64]");
	}

	// HE-2: the whole chain contains divisions, so nothing may be regrouped.
	TEST_CASE("1 / X / 2") {
		check_unchanged("{1}:bv[64] / X:bv[64] / {2}:bv[64]");
	}

	TEST_CASE("1 + X + 2 - 3") {
		check_simplifies_to("{1}:bv[64] + X:bv[64] + {2}:bv[64] - {3}:bv[64]",
			"X:bv[64]");
	}

	TEST_CASE("X * 2 + 3 -2") {
		check_simplifies_to("X:bv[64] * {2}:bv[64] + {3}:bv[64] - {2}:bv[64]",
			"X:bv[64] * {2}:bv[64] + {1}:bv[64]");
	}

	// The parentheses matter: `A * B / C` parses as `A * (B / C)` (bf_mul is
	// the earlier grammar alternative, so it wins the top), so the
	// unparenthesised `{6} * X * {2} / {3}` is already `{6} * (X * ({2} /
	// {3}))` = 0 at parse time and tests nothing. Parenthesised, the division
	// blocks the outer regrouping while the pure product below it is still a
	// block of its own: 6 * X * 2 folds to X * 12.
	TEST_CASE("(6 * X * 2) / 3") {
		check_simplifies_to("({6}:bv[64] * X:bv[64] * {2}:bv[64]) / {3}:bv[64]",
			"(X:bv[64] * {12}:bv[64]) / {3}:bv[64]");
	}

	// ... and the unparenthesised spelling really is a constant-folded zero.
	TEST_CASE("6 * X * 2 / 3 parses as 6 * (X * (2 / 3)) = 0") {
		check_unchanged("{6}:bv[64] * X:bv[64] * {2}:bv[64] / {3}:bv[64]");
	}
}

// HE-1/HE-2/HE-12: blocks that mix `+ - * /` with a non-block operator, a
// cast, or a division. Every case below was silently mis-folded (or crashed
// in release) while the suite above asserted only `simplified != nullptr`.
TEST_SUITE("bv custom simplification: unsound folds") {

	// HE-2: cancelling a term that appears on both sides of a division is
	// only valid in a group; Z/2^n under `*` is not one.
	TEST_CASE("X / X is not folded") {
		check_unchanged("x:bv[8] / x:bv[8]");
	}

	// bv[4]: X=5, Y=4 gives X*Y/Y = 4/4 = 1, not 5.
	TEST_CASE("X * Y / Y is not folded to X") {
		check_unchanged("x:bv[8] * y:bv[8] / y:bv[8]");
	}

	// HE-1(a): the `&` operand's frames used to survive on the block stack,
	// so the additive block folded `y + z` and deleted `x & y` entirely.
	TEST_CASE("(x & y) + z keeps the conjunction") {
		check_unchanged("(x:bv[8] & y:bv[8]) + z:bv[8]");
	}

	// HE-1(c): this shape popped the wrong frame and then called
	// apply_block_operation on a one-frame stack, reading past the end of an
	// empty vector.
	TEST_CASE("((a + b) & c) + d does not corrupt the block stack") {
		check_unchanged("((a:bv[8] + b:bv[8]) & c:bv[8]) + d:bv[8]");
	}

	// HE-1(b): round 2 of the fixpoint loop used to assign the right-hand
	// constant to the left-hand block, yielding `{5} & {5}`.
	TEST_CASE("(a + {1}) & ({2} + {3}) keeps both operands") {
		// the parser's hooks already fold {2}+{3} into {5}
		check_unchanged("(a:bv[8] + {1}:bv[8]) & {5}:bv[8]");
	}

	// HE-12: a cast terminates the block; folding {9} and {1} together across
	// the width change loses the truncation/zero-extension.
	TEST_CASE("a cast terminates the additive block") {
		check_unchanged("(bv[8]) (x:bv[4] + {9}:bv[4]) + {1}:bv[8]");
	}

	// BA1-31: the bv hook must not fold `0 / X` for a non-constant X either.
	TEST_CASE("{0} / X is not folded to zero") {
		check_unchanged("{0}:bv[8] / x:bv[8]");
	}
}

TEST_SUITE("bv custom simplification: block shapes") {

	// `X - X` never reaches the block algebra: term_sub folds it at parse
	// time (soundly -- Z/2^n is a group under +). Kept as the documented
	// boundary of the case below.
	TEST_CASE("X - X is folded by the hook") {
		check_simplifies_to("x:bv[8] - x:bv[8]", "0:bv[8]");
	}

	// This one does reach it: no hook matches any of the three subtractions,
	// so the additive block cancels x and y off both sides and both sides
	// come out empty -- the `_0_trimmed(type)` return path. It has to be the
	// *trimmed* zero: every other return of build_simplification is a bare
	// node, and returning the bf-wrapped `_0` would nest the replacement in a
	// second bf layer.
	TEST_CASE("a fully cancelling additive block folds to zero") {
		check_simplifies_to("x:bv[8] + y:bv[8] - x:bv[8] - y:bv[8]",
			"0:bv[8]");
	}

	// A parenthesised sub-block still belongs to the same associative run.
	// `collect` trims exactly one `bf` wrapper per child, which is enough:
	// the grammar's `@inline ... bf > bf_parenthesis > bf` (parser/tau.tgf)
	// is a *path* inlining, and the parser replaces the whole outer node with
	// the inner one (parser::result::inline_bintree_paths), so a parenthesis
	// leaves a single `bf` layer, not two. If that ever changes, this case is
	// what notices: the operand would be seen as opaque and the constants
	// would stop merging.
	TEST_CASE("a parenthesised additive sub-block joins the outer block") {
		check_simplifies_to("({1}:bv[8] + X:bv[8]) + {2}:bv[8]",
			"X:bv[8] + {3}:bv[8]");
	}
}

tref parse_bf_no_hooks(const std::string& sample) {
	static tree<node_t>::get_options opts{
		.parse = { .start = tree<node_t>::bf },
		.reget_with_hooks = false
	};
	return tree<node_t>::get(sample, opts);
}

TEST_SUITE("bv_ba_custom_simplification loops to a fixpoint (HE-6)") {

	// HE-6: bv_ba_custom_simplification inserted `current` into `visited`
	// every iteration right before checking the loop condition against
	// `visited`, so the condition was always false, the loop ran exactly
	// once regardless of whether simplify_blocks changed anything, and the
	// (possibly still-simplifiable) result of that one pass was returned
	// unconditionally.
	TEST_CASE("folds a constant-only chain when hooks are disabled at parse time") {
		// with hooks disabled the constants are not already folded by the
		// parser, so this only passes if bv_ba_custom_simplification itself
		// performs (and completes) the folding
		tref src = parse_bf_no_hooks("{1}:bv[8] + {2}:bv[8] + {3}:bv[8]");
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		REQUIRE(simplified != nullptr);
		tref expected = parse_bf_no_hooks("{6}:bv[8]");
		CHECK(tree<node_t>::get(simplified) == tree<node_t>::get(expected));
	}

	// The thirteen cases that used to sit here re-parsed the samples of the
	// suite above (with hooks enabled, i.e. identically) and asserted
	// `CHECK(true)`. They are replaced by no-hooks cases, which are the only
	// ones that actually exercise the fixpoint loop: with hooks disabled at
	// parse time the constants are not pre-folded, so reaching the folded
	// result takes more than one `simplify_blocks` pass.

	TEST_CASE("folds a mixed variable/constant chain over several rounds") {
		tref src = parse_bf_no_hooks(
			"{1}:bv[8] + X:bv[8] + {2}:bv[8] + {3}:bv[8]");
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		REQUIRE(simplified != nullptr);
		std::cout << "simplified: "
			<< tree<node_t>::get(simplified).to_str() << "\n";
		tref expected = parse_bf_no_hooks("X:bv[8] + {6}:bv[8]");
		CHECK(tree<node_t>::get(simplified) == tree<node_t>::get(expected));
	}

	TEST_CASE("folds a pure product over several rounds") {
		tref src = parse_bf_no_hooks(
			"{2}:bv[8] * X:bv[8] * {3}:bv[8]");
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		REQUIRE(simplified != nullptr);
		std::cout << "simplified: "
			<< tree<node_t>::get(simplified).to_str() << "\n";
		tref expected = parse_bf_no_hooks("X:bv[8] * {6}:bv[8]");
		CHECK(tree<node_t>::get(simplified) == tree<node_t>::get(expected));
	}
}

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
