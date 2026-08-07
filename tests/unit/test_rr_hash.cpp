// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// `std::hash<rr<node>>` must agree with `rr::operator==`, which compares tree
// content through `compare_trees`. Both members are built from `htref` =
// `std::shared_ptr<htree>`, and with no `std::hash<htref>` in sight the
// `shared_ptr` specialization applies and hashes the stored address.
//
// The address is not a property of the tree. `bintree<T>::geth` caches the
// handle in a `weak_ptr`, so asking twice returns the same `htree` only while
// someone still holds it; once the last owner is gone, the next call builds a
// fresh `htree` for the same `tref` — same tree, new address. The cases below
// reconstruct exactly that.
//
// Two notes for anyone editing this file:
//   - the probe tree must be one nothing else retains. `tau::_T()` is held
//     permanently elsewhere (`use_count` 3 in a bare test binary), so its
//     handle never expires and the defect cannot show on it.
//   - the ballast has to stay allocated. Releasing it lets the allocator hand
//     the very same address back, and the test then passes with the defect
//     present.

#include "test_init.h"
#include "test_tau_helpers.h"

using rr_t = rr<node_t>;
using rr_hash = std::hash<rr_t>;

namespace {

// A tree nobody else holds a handle to.
tref probe_tree(const char* name) {
	return tau::build_bf_eq_0(
		build_bf_variable<node_t>(name, tau_type_id<node_t>()));
}

// Occupy the block the released `htree` left behind, and keep it occupied.
std::vector<std::unique_ptr<char[]>> g_ballast;

void occupy_freed_blocks() {
	for (size_t sz = 8; sz <= 128; sz += 8)
		for (int i = 0; i < 32; ++i)
			g_ballast.push_back(std::make_unique<char[]>(sz));
}

size_t hash_of_fresh_handle(tref x) {
	return rr_hash{}(rr_t(tau::geth(x)));
}

} // namespace

TEST_SUITE("configuration") {
	TEST_CASE("bdd_init") { bdd_init<Bool>(); }
}

TEST_SUITE("rr hash agrees with rr equality") {

	TEST_CASE("equal content hashes equally across handle lifetimes") {
		tref x = probe_tree("rr_hash_probe_a");
		size_t first = hash_of_fresh_handle(x);   // handle dies here
		occupy_freed_blocks();
		size_t second = hash_of_fresh_handle(x);  // fresh htree, same tree
		CHECK( first == second );
	}

	TEST_CASE("the same holds with non-empty rec_relations") {
		tref head = probe_tree("rr_hash_probe_head");
		tref body = probe_tree("rr_hash_probe_body");
		tref main = probe_tree("rr_hash_probe_main");

		size_t first;
		{
			rewriter::rules rules;
			rules.emplace_back(tau::geth(head), tau::geth(body));
			first = rr_hash{}(rr_t(rules, tau::geth(main)));
		}
		occupy_freed_blocks();

		rewriter::rules rules;
		rules.emplace_back(tau::geth(head), tau::geth(body));
		rr_t b(rules, tau::geth(main));

		CHECK( first == rr_hash{}(b) );
	}

	TEST_CASE("different content still hashes differently") {
		rr_t a(tau::geth(probe_tree("rr_hash_probe_x")));
		rr_t b(tau::geth(probe_tree("rr_hash_probe_y")));
		CHECK_FALSE( a == b );
		CHECK( rr_hash{}(a) != rr_hash{}(b) );
	}

	TEST_CASE("rec_relations take part in the hash") {
		tref main = probe_tree("rr_hash_probe_main2");
		rewriter::rules rules;
		rules.emplace_back(tau::geth(probe_tree("rr_hash_probe_h2")),
				   tau::geth(probe_tree("rr_hash_probe_b2")));
		rr_t with(rules, tau::geth(main));
		rr_t without(tau::geth(main));
		CHECK_FALSE( with == without );
		CHECK( rr_hash{}(with) != rr_hash{}(without) );
	}
}
