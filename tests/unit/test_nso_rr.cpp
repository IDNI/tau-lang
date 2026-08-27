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

		CHECK( applied != nullptr );
		// the result should be a wff (boolean formula) at its root
		CHECK( tau::get(applied).is(tau::wff) );
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
		CHECK( applied != nullptr );

		// The applied main should contain a bf (boolean formula) reference or body
		CHECK( ((tt(applied) | tau::bf).has_value()
			|| (tt(applied) | tau::bf_eq).has_value()) );
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
