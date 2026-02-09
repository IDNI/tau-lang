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

TEST_SUITE("ba bv cvc5 constant simplification") {

	TEST_CASE("addition of constants") {
		logging::trace();
		const char* sample = "{1}:bv[8] + {2}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( simplified != nullptr );
		CHECK( simplified != src) ;
		logging::info();
	}

	TEST_CASE("substraction of constants") {
		logging::trace();
		const char* sample = "{1}:bv[8] - {2}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( simplified != nullptr );
		CHECK( simplified != src) ;
		logging::info();
	}

	TEST_CASE("multiplication of constants") {
		logging::trace();
		const char* sample = "{3}:bv[8] * {4}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( simplified != nullptr );
		CHECK( simplified != src) ;
		logging::info();
	}

	TEST_CASE("division of constants") {
		logging::trace();
		const char* sample = "{10}:bv[8] / {2}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( simplified != nullptr );
		CHECK( simplified != src) ;
		logging::info();
	}

	TEST_CASE("mod of constants") {
		logging::trace();
		const char* sample = "{10}:bv[8] % {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( simplified != nullptr );
		CHECK( simplified != src) ;
		logging::info();
	}

	TEST_CASE("shift right of constants") {
		logging::trace();
		const char* sample = "{16}:bv[8] >> {2}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( simplified != nullptr );
		CHECK( simplified != src) ;
		logging::info();
	}

	TEST_CASE("shift left of constants") {
		logging::trace();
		const char* sample = "{1}:bv[8] << {2}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( simplified != nullptr );
		CHECK( simplified != src) ;
		logging::info();
	}

	TEST_CASE("not of constant") {
		logging::trace();
		const char* sample = "{0}:bv[8]'";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( simplified != nullptr );
		CHECK( simplified == src) ;
		logging::info();
	}

	TEST_CASE("and of constants") {
		logging::trace();
		const char* sample = "{5}:bv[8] & {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( simplified != nullptr );
		CHECK( simplified != src) ;
		logging::info();
	}

	TEST_CASE("nand of constants") {
		logging::trace();
		const char* sample = "{5}:bv[8] !& {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( simplified != nullptr );
		CHECK( simplified != src) ;
		logging::info();
	}

	TEST_CASE("or of constants") {
		logging::trace();
		const char* sample = "{5}:bv[8] | {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( simplified != nullptr );
		CHECK( simplified != src) ;
		logging::info();
	}

	TEST_CASE("nor of constants") {
		logging::trace();
		const char* sample = "{5}:bv[8] !| {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( simplified != nullptr );
		CHECK( simplified != src) ;
		logging::info();
	}

	TEST_CASE("xor of constants") {
		logging::trace();
		const char* sample = "{5}:bv[8] ^ {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( simplified != nullptr );
		CHECK( simplified != src) ;
		logging::info();
	}

	TEST_CASE("xnor of constants") {
		logging::trace();
		const char* sample = "{5}:bv[8] !^ {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( simplified != nullptr );
		CHECK( simplified != src) ;
		logging::info();
	}
}
