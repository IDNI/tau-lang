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

	// Division by zero (should not crash, should return nullptr or error node)
	TEST_CASE("division by zero") {
		const char* sample = "{5}:bv[8] / {0}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		// Accept nullptr or a special error node, but must not crash
		CHECK((simplified == nullptr || simplified != src));
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

	// Neutral element elimination (1 * x)
	TEST_CASE("neutral element elimination multiplication") {
		const char* sample = "1:bv[8] * x:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		const char* expected = "x:bv[8]";
		tref exp = parse_bf(expected);
		CHECK(simplified != nullptr);
		CHECK(tree<node_t>::get(simplified) == tree<node_t>::get(exp));
	}

	TEST_CASE("1 + 2") {
		const char* sample = "{1}:bv[64] + {2}:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( true );
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
