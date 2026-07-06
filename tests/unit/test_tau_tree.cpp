// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

// ── tree::build_shift(const std::string&, size_t) (TT-1) ────────────────────

TEST_SUITE("tree::build_shift") {

	TEST_CASE("string overload builds a shift over a fresh variable") {
		tref s = tau::build_shift(std::string("x"), 3);
		REQUIRE(s != nullptr);
		CHECK(tau::get(s).is(tau::shift));
		CHECK(tau::get(s)[1].get_num() == 3);
		CHECK(get_var_name<node_t>(tau::get(s)[0].get()) == "x");
	}
}

// ── tree::build_offsets(const std::string&/vector<string>&) (TT-10) ─────────

TEST_SUITE("tree::build_offsets") {

	TEST_CASE("string overload builds a single offset variable") {
		tref o = tau::build_offsets(std::string("n"));
		REQUIRE(o != nullptr);
		CHECK(tau::get(o).is(tau::offsets));
		CHECK(tau::get(o)[0].is(tau::offset));
		CHECK(tau::get(o)[0][0].is(tau::variable));
		CHECK(get_var_name<node_t>(tau::get(o)[0][0].get()) == "n");
	}

	TEST_CASE("vector overload builds an offset variable per name") {
		tref o = tau::build_offsets(strings{"n", "m"});
		REQUIRE(o != nullptr);
		CHECK(tau::get(o).is(tau::offsets));
		CHECK(get_var_name<node_t>(tau::get(o)[0][0].get()) == "n");
		CHECK(get_var_name<node_t>(tau::get(o)[1][0].get()) == "m");
	}
}

// ── node::extension(T) / extension() round-trip (TT-2) ───────────────────────

TEST_SUITE("node::extension") {

	TEST_CASE("round-trips nt, term and ext through raw storage, resets ba_type") {
		node_t n(tau::bf_and, 42, true, 0, 1);
		auto raw = n.extension();
		node_t back = node_t::extension(raw);
		CHECK(back.nt == n.nt);
		CHECK(back.term == n.term);
		CHECK(back.ext == n.ext);
		CHECK(back.data == n.data);
		CHECK(back.ba_type == 0);
	}
}

// ── get_var_name/get_var_name_sid null-node safety (TT-17) ──────────────────

TEST_SUITE("get_var_name null-node safety") {

	TEST_CASE("get_var_name returns empty string instead of crashing") {
		tref bogus = tau::get_num(5);
		CHECK(get_var_name<node_t>(bogus).empty());
	}

	TEST_CASE("get_var_name_sid returns 0 instead of crashing") {
		tref bogus = tau::get_num(5);
		CHECK(get_var_name_sid<node_t>(bogus) == 0);
	}
}
