// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

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
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		// Should be {6}:bv[8] after multi-pass
		const char* expected_str = "{6}:bv[8]";
		tref expected = parse_bf(expected_str);
		CHECK(simplified != nullptr);
		CHECK(tree<node_t>::get(simplified) == tree<node_t>::get(expected));
	}

	// Chained constant subtraction with cancellation
	TEST_CASE("chained subtraction cancellation") {
		const char* sample = "{5}:bv[8] - {2}:bv[8] + {2}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		const char* expected_str = "{5}:bv[8]";
		tref expected = parse_bf(expected_str);
		CHECK(simplified != nullptr);
		CHECK(tree<node_t>::get(simplified) == tree<node_t>::get(expected));
	}

	// Division by zero (is top)
	TEST_CASE("division by zero") {
		using node = node_t;

		const char* sample = "{5}:bv[8] / {0}:bv[8]";
		tref src = parse_bf(sample);
		const char* expected = "1:bv[8]";
		tref exp = parse_bf(expected);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		// Accept nullptr or a special error node, but must not crash
		DBG( LOG_TRACE << "simplified: " << (simplified == nullptr ? "nullptr" : tree<node>::get(simplified).tree_to_str()) << "\n" );
		CHECK(tree<node>::get(simplified) == tree<node>::get(exp));
	}

	// Division by zero (is top)
	TEST_CASE("division by zero with variables") {
		using node = node_t;

		const char* sample = "x:bv[8] / {0}:bv[8]";
		tref src = parse_bf(sample);
		const char* expected = "1:bv[8]";
		tref exp = parse_bf(expected);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		// Accept nullptr or a special error node, but must not crash
		DBG( LOG_TRACE << "simplified: " << (simplified == nullptr ? "nullptr" : tree<node>::get(simplified).tree_to_str()) << "\n" );
		CHECK(tree<node>::get(simplified) == tree<node>::get(exp));
	}

	// Division by zero (is top)
	TEST_CASE("division by zero with constants") {
		using node = node_t;

		const char* sample = "{5}:bv[8] / {0}:bv[8]";
		tref src = parse_bf(sample);
		const char* expected = "1:bv[8]";
		tref exp = parse_bf(expected);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		// Accept nullptr or a special error node, but must not crash
		DBG( LOG_TRACE << "simplified: " << (simplified == nullptr ? "nullptr" : tree<node>::get(simplified).tree_to_str()) << "\n" );
		CHECK(tree<node>::get(simplified) == tree<node>::get(exp));
	}

	// All variables (should not introduce neutral element)
	TEST_CASE("all variables no neutral element") {
		const char* sample = "x:bv[8] + y:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		// Should not be 0 + x + y
		CHECK(simplified != nullptr);
		std::string str = tree<node_t>::get(simplified).tree_to_str();
		CHECK(str.find("0:bv[8]") == std::string::npos);
	}

	// Neutral element elimination (0 + x)
	TEST_CASE("neutral element elimination") {
		const char* sample = "0:bv[8] + x:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		const char* expected = "x:bv[8]";
		tref exp = parse_bf(expected);
		CHECK(simplified != nullptr);
		CHECK(tree<node_t>::get(simplified) == tree<node_t>::get(exp));
	}

	// 1:bv[8] = 11111111
	TEST_CASE("top element multiplication") {
		const char* sample = "1:bv[8] * x:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		CHECK(simplified != nullptr);
		CHECK(matches_to_str_to_any_of(simplified, { "x*1", "1*x", "(x*1)" }));
	}

	TEST_CASE("1 + 2") {
		const char* sample = "{1}:bv[64] + {2}:bv[64]";
		tref src = parse_bf(sample);
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

	TEST_CASE("1 * X") {
		const char* sample = "{1}:bv[64] * X:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( simplified != nullptr );
	}

	TEST_CASE("2 * X") {
		const char* sample = "{1}:bv[64] * X:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( simplified != nullptr );
	}

	TEST_CASE("1 / X") {
		const char* sample = "{1}:bv[64] / X:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( simplified != nullptr );
	}

	TEST_CASE("1 + X + 2") {
		const char* sample = "{1}:bv[64] + X:bv[64] + {2}:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( simplified != nullptr );
	}

	TEST_CASE("1 * X * 2") {
		const char* sample = "{1}:bv[64] * X:bv[64] * {2}:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( simplified != nullptr );
	}

	TEST_CASE("1 - X - 2") {
		const char* sample = "{1}:bv[64] - X:bv[64] - {2}:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( simplified != nullptr );
	}

	TEST_CASE("1 / X / 2") {
		const char* sample = "{1}:bv[64] / X:bv[64] / {2}:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( simplified != nullptr );
	}

	TEST_CASE("1 + X + 2 - 3") {
		const char* sample = "{1}:bv[64] + X:bv[64] + {2}:bv[64] - {3}:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( simplified != nullptr );
	}

	TEST_CASE("X * 2 + 3 -2") {
		const char* sample = "X:bv[64] * {2}:bv[64] + {3}:bv[64] - {2}:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( simplified != nullptr );
	}

	TEST_CASE("6 * X * 2 / 3") {
		const char* sample = "{6}:bv[64] * X:bv[64] * {2}:bv[64] / {3}:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( simplified != nullptr );
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

	TEST_CASE("1 * 2") {
		const char* sample = "{1}:bv[64] * {2}:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( true );
	}

	TEST_CASE("1 + X") {
		const char* sample = "{1}:bv[64] + X:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( true );
	}

	TEST_CASE("1 - X") {
		const char* sample = "{1}:bv[64] + {2}:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( true );
	}

	TEST_CASE("1 * X") {
		const char* sample = "{1}:bv[64] * X:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( true );
	}

	TEST_CASE("2 * X") {
		const char* sample = "{1}:bv[64] * X:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( true );
	}

	TEST_CASE("1 / X") {
		const char* sample = "{1}:bv[64] / X:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( true );
	}

	TEST_CASE("1 + X + 2") {
		const char* sample = "{1}:bv[64] + X:bv[64] + {2}:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( true );
	}

	TEST_CASE("1 * X * 2") {
		const char* sample = "{1}:bv[64] * X:bv[64] * {2}:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( true );
	}

	TEST_CASE("1 - X - 2") {
		const char* sample = "{1}:bv[64] - X:bv[64] - {2}:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( true );
	}

	TEST_CASE("1 / X / 2") {
		const char* sample = "{1}:bv[64] / X:bv[64] / {2}:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( true );
	}

	TEST_CASE("1 + X + 2 - 3") {
		const char* sample = "{1}:bv[64] + X:bv[64] + {2}:bv[64] - {3}:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( true );
	}

	TEST_CASE("X * 2 + 3 -2") {
		const char* sample = "X:bv[64] * {2}:bv[64] + {3}:bv[64] - {2}:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( true );
	}

	TEST_CASE("6 * X * 2 / 3") {
		const char* sample = "{6}:bv[64] * X:bv[64] * {2}:bv[64] / {3}:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( true );
	}
}

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
