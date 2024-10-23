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

#include <cassert>

#include "doctest.h"
#include "nso_rr.h"
#include "bdd_handle.h"
#include "normalizer.h"
#include "tau_ba.h"

#include "test_integration_helpers-tau.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

TEST_SUITE("Tau_splitter_tau_coeff") {
TEST_CASE("Tau_splitter_tau_coeff1") {
	const char *src = "{xy = 0.}v != 0.";
	auto fm = make_nso_rr_using_factory<tau_ba<bdd_test>, bdd_test>(src).value().main;
	auto s = tau_splitter(fm, splitter_type::upper);
	stringstream ss; ss << s;
	CHECK(ss.str() == "{ x = 0. } v != 0");
}

TEST_CASE("Tau_splitter_tau_coeff2") {
	const char *src = "{x|y = 0.}&v = 0.";
	auto fm = make_nso_rr_using_factory<tau_ba<bdd_test>, bdd_test>(src).value().main;
	auto s = tau_splitter(fm, splitter_type::upper);
	stringstream ss; ss << s;
	CHECK(ss.str() == "{ x|y = 0. } v|{ x|y = 0. } = 0");
}
}
