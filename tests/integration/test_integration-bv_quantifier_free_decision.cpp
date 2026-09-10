// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Quantifier-free decision of closed bitvector formulas whose binders are all
// of one kind (bv_ba_solver.tmpl.h, opt-in). Dropping positive existential
// binders keeps satisfiability, dropping positive universal binders and
// negating the matrix inverts it, so the verdicts with the switch on must
// equal the verdicts without it -- on the shapes it takes and on the shapes it
// must leave alone (both kinds, a binder under a negation).

#include "test_init.h"
#include "test_tau_helpers.h"

#include <cstdio>

namespace {

struct qf_config {
	qf_config(bool on) { bv_quantifier_free_decision = on; }
	~qf_config() { bv_quantifier_free_decision = false; }
};

tref parse_wff(const std::string& sample) {
	static tree<node_t>::get_options opts{ .parse = { .start = tree<node_t>::wff }};
	auto src = tree<node_t>::get(sample, opts);
	if (src == nullptr) TAU_LOG_ERROR << "Parsing failed for: " << sample;
	return src;
}

std::string norm(const std::string& sample, bool on) {
	qf_config c(on);
	auto wff = parse_wff(sample);
	return wff ? tau::get(normalizer<node_t>(wff)).to_str() : "parse_error";
}

// A subset-sum over k guarded contributions: bv[1] selectors s_i, bv[8]
// contributions c_i pinned to v_i or 0 by the selector, and a sum equal to
// `target`. The first selector is forced to 1, so the smallest reachable sum
// is 2 and the largest is 2 + 3 + ... + (k + 1).
std::string subset_sum(size_t k, const std::string& quantifier, unsigned target) {
	std::string vars, body;
	for (size_t i = 0; i < k; ++i) {
		std::string s = "s" + std::to_string(i) + ":bv[1]";
		std::string c = "c" + std::to_string(i) + ":bv[8]";
		vars += (i ? ", " : "") + s + ", " + c;
		char v[8]; std::snprintf(v, sizeof v, "%02x", (unsigned) (2 + i));
		body += "(" + s + " = { #x1 }:bv[1] -> " + c + " = { #x" + v + " }:bv[8]) && ("
			+ s + " = { #x0 }:bv[1] -> " + c + " = { #x00 }:bv[8]) && ";
	}
	std::string sum;
	for (size_t i = 0; i < k; ++i) sum += (i ? " + " : "") + ("c" + std::to_string(i) + ":bv[8]");
	char t[8]; std::snprintf(t, sizeof t, "%02x", target);
	return quantifier + " " + vars + " (" + body + "s0:bv[1] = { #x1 }:bv[1] && "
		+ sum + " = { #x" + t + " }:bv[8])";
}

} // namespace

TEST_SUITE("configuration") {
	TEST_CASE("bdd_init") { bdd_init<Bool>(); }
	TEST_CASE("logging") { logging::trace(); }
}

TEST_SUITE("quantifier-free bitvector decision") {

	TEST_CASE("existential closure: same verdicts as the quantified path") {
		// target 2: only the forced first contribution -- satisfiable.
		CHECK(norm(subset_sum(6, "ex", 2), true) == "T");
		CHECK(norm(subset_sum(6, "ex", 2), false) == "T");
		// target 255: above the largest reachable sum (2 + ... + 7 = 27).
		CHECK(norm(subset_sum(6, "ex", 255), true) == "F");
		CHECK(norm(subset_sum(6, "ex", 255), false) == "F");
		// target 27: everything on.
		CHECK(norm(subset_sum(6, "ex", 27), true) == "T");
		CHECK(norm(subset_sum(6, "ex", 27), false) == "T");
	}

	TEST_CASE("universal closure: inverted matrix, same verdicts") {
		// Not every assignment sums to 2, and none sums to 255, so both are F.
		CHECK(norm(subset_sum(6, "all", 2), true) == "F");
		CHECK(norm(subset_sum(6, "all", 2), false) == "F");
		// A universal tautology over the bitvector order.
		CHECK(norm("all x:bv[4] (x:bv[4] < { 3 }:bv[4] || x:bv[4] >= { 3 }:bv[4])", true) == "T");
		CHECK(norm("all x:bv[4] (x:bv[4] < { 3 }:bv[4] || x:bv[4] >= { 3 }:bv[4])", false) == "T");
		CHECK(norm("all x:bv[4] (x:bv[4] < { 3 }:bv[4])", true) == "F");
		CHECK(norm("all x:bv[4] (x:bv[4] < { 3 }:bv[4])", false) == "F");
	}

	TEST_CASE("the larger instance the quantified path does not finish in time") {
		// 14 contributions: the quantified path took over a minute per query
		// on the box this was measured on; here it decides in milliseconds.
		CHECK(norm(subset_sum(14, "ex", 2), true) == "T");
		CHECK(norm(subset_sum(14, "ex", 255), true) == "F");
		CHECK(norm(subset_sum(14, "all", 255), true) == "F");
	}

	TEST_CASE("left alone: both kinds, or a binder under a negation") {
		// Alternation: the quantified path, with and without the switch.
		CHECK(norm("all x:bv[2] ex y:bv[2] (y:bv[2] > x:bv[2])", true) == "F");
		CHECK(norm("all x:bv[2] ex y:bv[2] (y:bv[2] > x:bv[2])", false) == "F");
		CHECK(norm("ex y:bv[2] all x:bv[2] (y:bv[2] >= x:bv[2])", true) == "T");
		CHECK(norm("ex y:bv[2] all x:bv[2] (y:bv[2] >= x:bv[2])", false) == "T");
		// A negated existential is a universal in disguise: declined, same verdict.
		CHECK(norm("!(ex x:bv[4] (x:bv[4] > { 14 }:bv[4]))", true) == "F");
		CHECK(norm("!(ex x:bv[4] (x:bv[4] > { 14 }:bv[4]))", false) == "F");
		CHECK(norm("!(ex x:bv[4] (x:bv[4] > { 15 }:bv[4]))", true) == "T");
		CHECK(norm("!(ex x:bv[4] (x:bv[4] > { 15 }:bv[4]))", false) == "T");
	}
}
