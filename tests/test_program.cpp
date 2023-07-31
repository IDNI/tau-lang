// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentiTd cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "../src/doctest.h"
#include "../src/program.h"
#include "../src/bool.h"
#include "../src/bdd_handle.h"

using namespace idni::rewriter;
using namespace idni::tau;
using namespace std;

namespace testing = doctest;

TEST_SUITE("testing order") {

	TEST_CASE("bool") {
		auto t = std::variant<bool>(true);
		auto f = std::variant<bool>(false);
		CHECK( (f <=> t) == std::strong_ordering::less );
	}

	struct Char {
		char c;
		Char(char c) : c(c) {}
		auto operator<=>(const Char& o) const = default;
	};

	TEST_CASE("Char") {
		auto b = std::variant<Char>(Char('b'));
		auto a = std::variant<Char>(Char('a'));
		CHECK( (a <=> b) == std::strong_ordering::less );
	}

	TEST_CASE("Bool") {
		auto t = std::variant<Bool>(Bool(true));
		auto f = std::variant<Bool>(Bool(false));
		CHECK( (f <=> t) == std::strong_ordering::less );
	}
}

/*TEST_SUITE("make_program") {

	static constexpr char* sample =	
		"bf_neg ( bf_neg ( $X ) ) = $X ."
		" { 0 } ";

	TEST_CASE("make_library") {
		auto src = make_tau_source(sample);
		bindings<Bool> bs;
		tau_source t(src);
		auto lib = make_program<Bool>(t, bs);
		CHECK( false );
	}

}*/

TEST_SUITE("make_library") {

	static constexpr char* sample =	"bf_neg ( bf_neg ( $X ) ) = $X .";

	TEST_CASE("make_library") {
		auto src = make_tau_source(sample);
		tau_source t(src);
		auto lib = make_library<Bool>(t);
		CHECK( true );
	}

}

TEST_SUITE("make_tau_source") {
	static constexpr char* sample =	"bf_neg ( bf_neg ( $X ) ) = $X .";

	TEST_CASE("make_tau_source") {
		auto src = make_tau_source(sample);
		CHECK( true );
	}
}

TEST_SUITE("apply") {

}

TEST_SUITE("bind") {

}

TEST_SUITE("builtin_applier") {

}

TEST_SUITE("get") {

}

TEST_SUITE("is_ignore") {

}

TEST_SUITE("is_capture") {

}

TEST_SUITE("non_terminal") {

}
