// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// rr<node>'s comparison operators, operator<=>, and its std::hash
// specialization (src/rr.tmpl.h) had zero test coverage anywhere in the
// repo: rr<node> itself is constructed and field-accessed constantly by
// existing tests (via get_bf_nso_rr/get_nso_rr), but nothing ever compares
// two rr instances, hashes one, or stores one in an unordered/ordered
// container -- so the lexicographic rec_relations-then-main ordering and
// the hash_combine-based hash were never actually exercised.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "rr.h"
#include <unordered_set>

using namespace idni::tau_lang;

TEST_SUITE("rr<node> comparison, equality and hashing") {

	TEST_CASE("structurally identical rr instances built independently compare equal and hash equal") {
		auto a = get_bf_nso_rr("", "x | y");
		auto b = get_bf_nso_rr("", "x | y");
		REQUIRE(a.has_value());
		REQUIRE(b.has_value());
		CHECK(a.value() == b.value());
		CHECK_FALSE(a.value() != b.value());
		CHECK(std::hash<rr<node_t>>{}(a.value())
		   == std::hash<rr<node_t>>{}(b.value()));
	}

	TEST_CASE("rr instances with different main formulas compare unequal with a consistent strict order") {
		auto a = get_bf_nso_rr("", "x | y");
		auto b = get_bf_nso_rr("", "x & y");
		REQUIRE(a.has_value());
		REQUIRE(b.has_value());
		CHECK(a.value() != b.value());
		bool a_lt_b = a.value() < b.value();
		bool b_lt_a = b.value() < a.value();
		// Exactly one strict direction holds for two distinct values.
		CHECK(a_lt_b != b_lt_a);
		CHECK((a.value() <= b.value()) == a_lt_b);
		CHECK((a.value() >= b.value()) == b_lt_a);
	}

	TEST_CASE("differing rec_relations count dominates ordering before main is compared") {
		auto no_rules = get_bf_nso_rr("", "x | y");
		auto one_rule = get_bf_nso_rr("h(X):tau := 1.", "x | y"); // same main source
		REQUIRE(no_rules.has_value());
		REQUIRE(one_rule.has_value());
		CHECK(no_rules.value().rec_relations.size() == 0);
		CHECK(one_rule.value().rec_relations.size() == 1);
		// Same main formula, but rec_relations sizes differ (0 vs 1):
		// operator<=> compares sizes first, so fewer rules sorts first.
		CHECK(no_rules.value() < one_rule.value());
		CHECK(no_rules.value() != one_rule.value());
	}

	TEST_CASE("rr is usable as an unordered_set key: structural duplicates collapse, distinct ones don't") {
		std::unordered_set<rr<node_t>> set;
		set.insert(get_bf_nso_rr("", "x | y").value());
		set.insert(get_bf_nso_rr("", "x | y").value()); // structural duplicate, independently parsed
		CHECK(set.size() == 1);
		set.insert(get_bf_nso_rr("", "x & y").value()); // distinct main
		CHECK(set.size() == 2);
	}

	TEST_CASE("default-constructed rr instances (no rules, null main) compare equal") {
		rr<node_t> a;
		rr<node_t> b;
		CHECK(a.rec_relations.empty());
		CHECK(a.main == nullptr);
		CHECK(a == b);
		CHECK(std::hash<rr<node_t>>{}(a) == std::hash<rr<node_t>>{}(b));
	}
}
