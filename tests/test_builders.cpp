// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentioned cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "../src/doctest.h"
#include "../src/formula.h"
#include "../src/bool.h"
#include "../src/bdd_handle.h"
#include "../src/normalizer2.h"
#include "test_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

TEST_SUITE("builders parsing") {

	TEST_CASE("BLDR_WFF_EQ") {
		auto src = make_builder<Bool>(BLDR_WFF_EQ);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(src.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(src.second) );
	}

	TEST_CASE("BLDR_WFF_NEQ") {
		auto src = make_builder<Bool>(BLDR_WFF_NEQ);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(src.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(src.second) );
	}

	TEST_CASE("BLDR_WFF_AND") {
		auto src = make_builder<Bool>(BLDR_WFF_AND);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(src.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(src.second) );
	}

	TEST_CASE("BLDR_WFF_OR") {
		auto src = make_builder<Bool>(BLDR_WFF_OR);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(src.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(src.second) );
	}

	TEST_CASE("BLDR_WFF_XOR") {
		auto src = make_builder<Bool>(BLDR_WFF_XOR);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(src.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(src.second) );
	}

	TEST_CASE("BLDR_WFF_NEG") {
		auto src = make_builder<Bool>(BLDR_WFF_NEG);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(src.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(src.second) );
	}

	TEST_CASE("BLDR_WFF_IMPLY") {
		auto src = make_builder<Bool>(BLDR_WFF_IMPLY);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(src.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(src.second) );
	}

	TEST_CASE("BLDR_WFF_EQUIV") {
		auto src = make_builder<Bool>(BLDR_WFF_EQUIV);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(src.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(src.second) );
	}

	TEST_CASE("BLDR_WFF_COIMPLY") {
		auto src = make_builder<Bool>(BLDR_WFF_COIMPLY);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(src.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(src.second) );
	}

	TEST_CASE("BLDR_WFF_ALL") {
		auto src = make_builder<Bool>(BLDR_WFF_ALL);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(src.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(src.second) );
	}

	TEST_CASE("BLDR_WFF_EX") {
		auto src = make_builder<Bool>(BLDR_WFF_EX);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(src.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(src.second) );
	}
}