// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Issue #129: cnf and dnf of a Boolean function containing `^` returned a
// term that was not equivalent to the input (`cnf (x ^ y)` gave `1`). Each
// case converts a term and asks `valid` whether the result equals the input,
// so the check does not depend on the printed shape of the normal form.

#include "test_init.h"
#include "test_tau_helpers.h"

using tau_api = api<node_t>;

namespace {

const std::vector<std::string> xor_terms = {
	"x ^ y",
	"(x ^ y) & z",
	"x ^ (x & y)",
	"(x ^ y)'",
	"(x ^ y)' & z",
	"x ^ y ^ z",
	"(x ^ y) | (z ^ w)",
	"(x ^ y') & (y ^ z)",
	"((x ^ y) & z)'",
	"x ^ (y | z')",
	"(x | y) ^ (x & z)",
	"x ^ x",
	"x ^ x'",
	"(x ^ y) ^ (x ^ z)",
	"x'y ^ z",
	"(x ^ (y ^ (z ^ w)))'",
	"x ^ 1",
	"x ^ 0",
	"(x ^ y) & (x ^ y')",
};

// `valid (t) = (nf)`, where nf is the normal form of t.
void check_equivalent(const std::string& t, bool cnf) {
	CAPTURE(t);
	CAPTURE(cnf);
	auto term_r = tau_api::get_term(t);
	REQUIRE(term_r.has_value());
	auto nf_r = cnf ? tau_api::cnf(term_r.value())
			: tau_api::dnf(term_r.value());
	REQUIRE(nf_r.has_value());
	std::string nf = tau_api::to_str(nf_r.value());
	CAPTURE(nf);
	auto eq_r = tau_api::valid("(" + t + ") = (" + nf + ")");
	REQUIRE(eq_r.has_value());
	CHECK(eq_r.value());
}

} // namespace

TEST_SUITE("normal forms of terms with xor (issue #129)") {

	TEST_CASE("cnf is equivalent to its input") {
		for (const auto& t : xor_terms) check_equivalent(t, true);
	}

	TEST_CASE("dnf is equivalent to its input") {
		for (const auto& t : xor_terms) check_equivalent(t, false);
	}

	TEST_CASE("cnf of xor has no xor and no constant answer") {
		auto term_r = tau_api::get_term("x ^ y");
		REQUIRE(term_r.has_value());
		auto nf_r = tau_api::cnf(term_r.value());
		REQUIRE(nf_r.has_value());
		CHECK(!tau_api::contains(nf_r.value(), tau::bf_xor));
		CHECK(!tau::get(nf_r.value()).equals_1());
	}
}
