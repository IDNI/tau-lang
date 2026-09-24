// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_bool_only_helpers.h"
#include "nso_rr.h"

TEST_SUITE("nso_rr unit tests") {

	TEST_CASE("nso_rr_apply: simple fixed-point loop unfolds") {
		const char* sample =
			"g[n](x) := !g[n-1](x)."
			"g[0](x) := T."
			"g(x).";

		auto maybe = get_nso_rr(sample);
		CHECK( maybe.has_value() );
		auto rr = maybe.value();

		// apply recurrence relations
		auto applied = nso_rr_apply<node_t>(rr);

		CHECK( applied.has_value() );
		// the result should be a wff (boolean formula) at its root
		CHECK( tau::get(applied.value()).is(tau::wff) );
	}

	TEST_CASE("nso_rr_apply: transform ref args to captures and substitute") {
		// A recurrence that uses a variable as a ref-arg should be transformed
		// so that variables in ref args become captures. This exercises the
		// transform_ref_args_to_captures / apply substitution path.
		const char* sample =
			"f(Y) := Y = 0."  // body uses variable Y
			"f(x).";

		auto maybe = get_nso_rr(sample);
		CHECK( maybe.has_value() );
		auto rr = maybe.value();

		auto applied = nso_rr_apply<node_t>(rr);
		CHECK( applied.has_value() );

		// The applied main should contain a bf (boolean formula) reference or body
		CHECK( ((tt(applied.value()) | tau::bf).has_value()
			|| (tt(applied.value()) | tau::bf_eq).has_value()) );
	}

}

// nso_rr_apply(rules, n) skips a definition whose head signature occurs
// nowhere in the formula. A definition reached only through another
// definition's body must still fire, whichever order the rules come in.
TEST_SUITE("definition expansion skips absent heads (#135)") {

	// Applies @p sample's definitions with rule counting on and returns
	// the expanded main plus the per-rule application counts.
	static std::pair<tref, std::unordered_map<std::string, size_t>>
		apply_counted(const char* sample)
	{
		auto maybe = get_nso_rr(sample);
		REQUIRE( maybe.has_value() );
		rule_apply_counts<node_t>().clear();
		rule_hit_counts<node_t>().clear();
		rule_counting = true;
		auto applied = nso_rr_apply<node_t>(maybe.value());
		rule_counting = false;
		auto counts = rule_apply_counts<node_t>();
		rule_apply_counts<node_t>().clear();
		rule_hit_counts<node_t>().clear();
		REQUIRE( applied.has_value() );
		return { applied.value(), counts };
	}

	static bool applied_rule_named(
		const std::unordered_map<std::string, size_t>& counts,
		const std::string& head)
	{
		for (const auto& [name, count] : counts)
			if (name.starts_with(head) && count) return true;
		return false;
	}

	static bool has_ref(tref n) {
		return tau::get(n).find_top(is<node_t, tau::ref>) != nullptr;
	}

	TEST_CASE("a definition whose head is absent is never attempted") {
		auto [fm, counts] = apply_counted(
			"k135a(x) := x = 0."
			"u135a(x) := x = 1."
			"k135a(y).");
		CHECK( !has_ref(fm) );
		CHECK( applied_rule_named(counts, "k135a(") );
		CHECK( !applied_rule_named(counts, "u135a(") );
	}

	TEST_CASE("a head introduced by an earlier rule still fires") {
		// g135b calls h135b; h135b comes later in rule order, so it
		// becomes eligible in the same round g135b is expanded.
		auto [fm, counts] = apply_counted(
			"g135b(x) := h135b(x)."
			"h135b(x) := x = 0."
			"g135b(y).");
		CHECK( !has_ref(fm) );
		CHECK( applied_rule_named(counts, "h135b(") );
	}

	TEST_CASE("a head introduced by a later rule fires next round") {
		auto [fm, counts] = apply_counted(
			"h135c(x) := x = 0."
			"m135c(x) := h135c(x)."
			"g135c(x) := m135c(x)."
			"g135c(y).");
		CHECK( !has_ref(fm) );
	}

	TEST_CASE("arities of one symbol are told apart") {
		auto [fm, counts] = apply_counted(
			"f135d(x) := x = 0."
			"f135d(x, y) := x = y."
			"f135d(z, w).");
		CHECK( !has_ref(fm) );
		// only the binary head occurs, so the unary one is skipped
		size_t unary = 0, binary = 0;
		for (const auto& [name, count] : counts) {
			if (!name.starts_with("f135d(")) continue;
			(name.find(',') < name.find(":=") ? binary : unary)
								+= count;
		}
		CHECK( binary > 0 );
		CHECK( unary == 0 );
	}

	TEST_CASE("recurrences with offsets still unfold") {
		auto [fm, counts] = apply_counted(
			"r135e[n](x) := !r135e[n-1](x)."
			"r135e[0](x) := x = 0."
			"r135e[2](y).");
		CHECK( !has_ref(fm) );
	}
}

TEST_SUITE("rule counting") {

	TEST_CASE("normalizer: rule_counting gates count nodes in the report") {
		// nso_rr_apply(rule, tref) caches on (rule, formula) identity, so
		// each case below needs a formula never rewritten anywhere else in
		// this binary -- otherwise a cache hit skips the counting code
		// entirely and the assertion below is testing a stale cache, not
		// the counting logic.
		auto with_flag = get_nso_rr(
			"q[m](y) := !q[m-1](y)."
			"q[0](y) := F."
			"q(y).");
		REQUIRE( with_flag.has_value() );
		rule_counting = true;
		auto counted = normalizer<node_t>(with_flag.value());
		rule_counting = false;
		CHECK( counted.has_value() );
		CHECK( report_has_code(counted.report(), code::info_count) );

		auto without_flag = get_nso_rr(
			"p[k](z) := !p[k-1](z)."
			"p[0](z) := T."
			"p(z).");
		REQUIRE( without_flag.has_value() );
		auto uncounted = normalizer<node_t>(without_flag.value());
		CHECK( uncounted.has_value() );
		CHECK( !report_has_code(uncounted.report(), code::info_count) );
	}

}

// GitHub #80: std::hash<rr<node>> used to hash the htref handles' addresses
// (via std::hash<shared_ptr>), while rr::operator== compares tree content. A
// handle is a weak-cached shared_ptr (bintree::geth), so once the last owner of
// a handle drops, the next geth of the same tree allocates a fresh htree at a
// new address and the same formula hashed differently run to run -- the source
// of the decomposition-effort/witness nondeterminism seen in #76/#89.
TEST_SUITE("rr hash agrees with equality (#80)") {

	static size_t hash_of(const rr<node_t>& r) {
		return std::hash<rr<node_t>>{}(r);
	}

	TEST_CASE("same content through a fresh handle hashes the same") {
		const char* sample = "g(x) := x = 0. g(y).";
		auto first = get_nso_rr(sample).value();
		size_t h1 = hash_of(first);
		tref main_ref = first.main->get();
		// Drop every handle so the weak cache entries expire ...
		first = rr<node_t>{};
		// ... and let a decoy handle take the freed allocation so the
		// re-parse below is forced onto a different address.
		htref decoy = tau::geth(_T<node_t>());
		auto second = get_nso_rr(sample).value();
		// Hash-consing gives the same tree back; only the handle is new.
		CHECK( second.main->get() == main_ref );
		CHECK( second == get_nso_rr(sample).value() );
		CHECK( hash_of(second) == h1 );
	}

	TEST_CASE("different content hashes differently") {
		auto a = get_nso_rr("x = 0.").value();
		auto b = get_nso_rr("x = 1.").value();
		CHECK( a != b );
		CHECK( hash_of(a) != hash_of(b) );
	}
}
