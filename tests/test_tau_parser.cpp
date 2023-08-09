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
#include "../src/program.h"
#include "../src/bool.h"

using namespace idni::rewriter;
using namespace idni::tau;
using namespace std;

namespace testing = doctest;

// IMPORTANT: To check the parser (and the underlying grammar) we don't need 
// any special boolean algebra, so we use Bool to simplify our testing.

// The following test suites represent correct sentences of a tau formula or 
// library and as such must be parsed correctly and satisfy the structural checks
// we perform on the parsed formula (as the rest of the code assumes such structure).

TEST_SUITE("bf_main") {

	TEST_CASE("bf_neg_T") {
		static constexpr char* sample =	"$X =bf_neg({1}).";
		auto src = make_tau_source(sample);
		auto lib = make_library<Bool>(src);
	}

/*	TEST_CASE("bf_neg_F") {
		static constexpr char* sample =	"$X = bf_neg(F).";
		auto src = make_tau_source(sample);
		tau_source t(src);
		auto lib = make_library<Bool>(t);
	}

	TEST_CASE("T_bf_and_T") {
		static constexpr char* sample = "$X = (T bf_neg T).";
		auto src = make_tau_source(sample);
		tau_source t(src);
		auto lib = make_library<Bool>(t);
	}

	TEST_CASE("T_bf_or_T") {
		static constexpr char* sample = "$X = (T bf_or T).";
		auto src = make_tau_source(sample);
		tau_source t(src);
		auto lib = make_library<Bool>(t);
	}

	TEST_CASE("T_bf_xor_T") {
		static constexpr char* sample = "$X = (T bf_xor T).";
		auto src = make_tau_source(sample);
		tau_source t(src);
		auto lib = make_library<Bool>(t);
	}

	TEST_CASE("bf_all_X_(X)") {
		static constexpr char* sample = "$X = bf_all ?X (?X).";
		auto src = make_tau_source(sample);
		tau_source t(src);
		auto lib = make_library<Bool>(t);
	}	

	TEST_CASE("bf_ex_X_(X)") {
		static constexpr char* sample = "$X = bf_ex ?X (?X).";
		auto src = make_tau_source(sample);
		tau_source t(src);
		auto lib = make_library<Bool>(t);
	}*/
}

TEST_SUITE("cbf_main") { 
}

TEST_SUITE("wff_main") { 
}

TEST_SUITE("rules") { 
}

TEST_SUITE("rec_relations") { 
}

TEST_SUITE("builtins") { 
}

TEST_SUITE("programs") { 
}

TEST_SUITE("libraries") { 
}


