// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// The T_k zero-mask encoding, its restriction, and the witness construction.

#include "test_init.h"
#include "test_tau_helpers.h"

#include "ocltl_types.h"

using ba_t = sbf_ba;

namespace {

// A concrete sbf variable, parsed through the descriptor.
ba_t sbf_var(const std::string& name) {
	using desc = ba_descriptor<ba_t, node_t>;
	auto c = desc::parse(name, desc::type_tree());
	REQUIRE(c.has_value());
	REQUIRE(std::holds_alternative<ba_t>(c->first));
	return std::get<ba_t>(c->first);
}

} // namespace

TEST_SUITE("ocltl_types: T_k cardinality") {

	size_t count_valid(size_t k) {
		size_t n = 0;
		ocltl_type_mask limit = ocltl_minterm_count(k) >= 64
			? 0 // handled separately; not enumerated for k this large
			: (ocltl_type_mask{1} << ocltl_minterm_count(k));
		for (ocltl_type_mask z = 0; z < limit; ++z)
			if (ocltl_is_valid_type(z, k)) ++n;
		return n;
	}

	TEST_CASE("|T_1| = 3") { CHECK(count_valid(1) == 3); }
	TEST_CASE("|T_2| = 15") { CHECK(count_valid(2) == 15); }
	TEST_CASE("|T_3| = 255") { CHECK(count_valid(3) == 255); }

	TEST_CASE("ocltl_type_count matches enumeration") {
		CHECK(ocltl_type_count(1) == 3);
		CHECK(ocltl_type_count(2) == 15);
		CHECK(ocltl_type_count(3) == 255);
	}

	TEST_CASE("k=6 fits the 64-bit word without overflow") {
		CHECK(ocltl_minterm_count(ocltl_max_k) == 64);
		CHECK(ocltl_full_mask(ocltl_max_k) == ~ocltl_type_mask{0});
	}
}

TEST_SUITE("ocltl_types: validity") {

	TEST_CASE("the all-ones mask is invalid, every other mask is valid (k=3)") {
		ocltl_type_mask all_ones = ocltl_full_mask(3);
		CHECK_FALSE(ocltl_is_valid_type(all_ones, 3));
		for (ocltl_type_mask z = 0; z < all_ones; ++z)
			CHECK(ocltl_is_valid_type(z, 3));
	}
}

TEST_SUITE("ocltl_types: restriction") {

	TEST_CASE("drop-one agrees with the general index-set form (k=3)") {
		for (ocltl_type_mask z = 0; z < ocltl_full_mask(3); ++z) {
			for (size_t i = 0; i < 3; ++i) {
				std::vector<size_t> keep;
				for (size_t c = 0; c < 3; ++c) if (c != i) keep.push_back(c);
				CHECK(ocltl_drop_coordinate(z, 3, i) == ocltl_restrict(z, 3, keep));
			}
		}
	}

	TEST_CASE("dropping i then j equals dropping {i, j} (k=3)") {
		// Drops the larger index first, leaving the smaller one's position fixed.
		for (ocltl_type_mask z = 0; z < ocltl_full_mask(3); ++z) {
			ocltl_type_mask step = ocltl_drop_coordinate(z, 3, 2);   // drop coord 2
			ocltl_type_mask composed = ocltl_drop_coordinate(step, 2, 1); // drop old coord 1
			ocltl_type_mask direct = ocltl_restrict(z, 3, { 0 });
			CHECK(composed == direct);
		}
	}

	TEST_CASE("restriction of a valid mask is valid (k=3 -> k=2 and k=1)") {
		for (ocltl_type_mask z = 0; z < ocltl_full_mask(3); ++z) {
			CHECK(ocltl_is_valid_type(z, 3));
			for (size_t i = 0; i < 3; ++i) {
				ocltl_type_mask r2 = ocltl_drop_coordinate(z, 3, i);
				CHECK(ocltl_is_valid_type(r2, 2));
			}
			ocltl_type_mask r1 = ocltl_restrict(z, 3, { 0 });
			CHECK(ocltl_is_valid_type(r1, 1));
		}
	}

	TEST_CASE("sh(tau)|_m is restriction to the third coordinate") {
		for (ocltl_type_mask z = 0; z < ocltl_full_mask(3); ++z)
			CHECK(ocltl_sh_restrict(z) == ocltl_restrict(z, 3, { 2 }));
	}
}

TEST_SUITE("ocltl_types: witness") {

	TEST_CASE("extension then re-computing the type returns the requested mask") {
		ba_t x = sbf_var("x");
		std::vector<ba_t> a{ x };

		// tp(x) over a single free variable is Z = ∅: neither ¬x nor x is zero.
		REQUIRE(ocltl_type_of<ba_t, node_t>(a) == 0);

		// b covers ¬x entirely and splits x, exercising both witness branches.
		ocltl_type_mask tau = ocltl_type_mask{1} << 0;
		REQUIRE(ocltl_is_valid_type(tau, 2));
		REQUIRE(ocltl_restrict(tau, 2, { 0 }) == ocltl_type_of<ba_t, node_t>(a));

		ba_t b = ocltl_witness<ba_t, node_t>(a, tau);
		std::vector<ba_t> ab{ x, b };
		CHECK(ocltl_type_of<ba_t, node_t>(ab) == tau);
	}

	TEST_CASE("witness round-trips for several requested masks (k=1 -> k=2)") {
		ba_t x = sbf_var("y");
		std::vector<ba_t> a{ x };
		ocltl_type_mask base = ocltl_type_of<ba_t, node_t>(a);

		for (ocltl_type_mask tau = 0; tau < ocltl_full_mask(2); ++tau) {
			if (ocltl_restrict(tau, 2, { 0 }) != base) continue; // must extend tp(a)
			ba_t b = ocltl_witness<ba_t, node_t>(a, tau);
			std::vector<ba_t> ab{ x, b };
			CHECK(ocltl_type_of<ba_t, node_t>(ab) == tau);
		}
	}
}

TEST_SUITE("ocltl_types: wide-K agreement with the capped (k <= 6) path") {

	TEST_CASE("ocltl_restrict_wide agrees with ocltl_restrict (k=3)") {
		for (ocltl_type_mask z = 0; z < ocltl_full_mask(3); ++z) {
			ocltl_type_mask_wide zw = ocltl_to_wide_mask(z, 3);
			for (size_t i = 0; i < 3; ++i) {
				std::vector<size_t> keep;
				for (size_t c = 0; c < 3; ++c) if (c != i) keep.push_back(c);
				ocltl_type_mask capped = ocltl_restrict(z, 3, keep);
				ocltl_type_mask_wide wide = ocltl_restrict_wide(zw, 3, keep);
				CHECK(ocltl_from_wide_mask(wide) == capped);
			}
		}
	}

	TEST_CASE("ocltl_drop_coordinate_wide agrees with ocltl_drop_coordinate (k=3)") {
		for (ocltl_type_mask z = 0; z < ocltl_full_mask(3); ++z) {
			ocltl_type_mask_wide zw = ocltl_to_wide_mask(z, 3);
			for (size_t i = 0; i < 3; ++i) {
				ocltl_type_mask capped = ocltl_drop_coordinate(z, 3, i);
				ocltl_type_mask_wide wide = ocltl_drop_coordinate_wide(zw, 3, i);
				CHECK(ocltl_from_wide_mask(wide) == capped);
			}
		}
	}

	TEST_CASE("ocltl_wide_is_valid_type agrees with ocltl_is_valid_type (k=3)") {
		for (ocltl_type_mask z = 0; z <= ocltl_full_mask(3); ++z) {
			ocltl_type_mask_wide zw = ocltl_to_wide_mask(z, 3);
			CHECK(ocltl_wide_is_valid_type(zw) == ocltl_is_valid_type(z, 3));
		}
	}

	TEST_CASE("ocltl_sh_restrict_wide agrees with ocltl_sh_restrict (k=3)") {
		for (ocltl_type_mask z = 0; z < ocltl_full_mask(3); ++z) {
			ocltl_type_mask_wide zw = ocltl_to_wide_mask(z, 3);
			CHECK(ocltl_from_wide_mask(ocltl_sh_restrict_wide(zw)) == ocltl_sh_restrict(z));
		}
	}

	TEST_CASE("ocltl_witness_wide agrees with ocltl_witness, every requested mask (k=1 -> k=2)") {
		ba_t x = sbf_var("z");
		std::vector<ba_t> a{ x };
		ocltl_type_mask base = ocltl_type_of<ba_t, node_t>(a);

		for (ocltl_type_mask tau = 0; tau < ocltl_full_mask(2); ++tau) {
			if (ocltl_restrict(tau, 2, { 0 }) != base) continue; // must extend tp(a)
			ba_t b_capped = ocltl_witness<ba_t, node_t>(a, tau);
			ba_t b_wide = ocltl_witness_wide<ba_t, node_t>(a, ocltl_to_wide_mask(tau, 2));

			std::vector<ba_t> ab_capped{ x, b_capped };
			std::vector<ba_t> ab_wide{ x, b_wide };
			// Both witnesses must realize the same requested type -- the two
			// constructions need not pick identical splitter symbols, only
			// agree on the type they decode to.
			CHECK(ocltl_type_of<ba_t, node_t>(ab_capped) == tau);
			CHECK(ocltl_type_of<ba_t, node_t>(ab_wide) == tau);
		}
	}

	TEST_CASE("ocltl_witness_wide agrees with ocltl_witness on the empty tuple (k=0 -> k=1)") {
		std::vector<ba_t> empty;
		for (ocltl_type_mask tau : { ocltl_type_mask{0b01}, ocltl_type_mask{0b10}, ocltl_type_mask{0b00} }) {
			ba_t b_capped = ocltl_witness<ba_t, node_t>(empty, tau);
			ba_t b_wide = ocltl_witness_wide<ba_t, node_t>(empty, ocltl_to_wide_mask(tau, 1));
			std::vector<ba_t> ab_capped{ b_capped }, ab_wide{ b_wide };
			CHECK(ocltl_type_of<ba_t, node_t>(ab_capped) == tau);
			CHECK(ocltl_type_of<ba_t, node_t>(ab_wide) == tau);
		}
	}

	TEST_CASE("ocltl_witness_wide runs past ocltl_max_k (k=10 -> k=11)") {
		std::vector<ba_t> a;
		for (size_t i = 0; i < 10; ++i) a.push_back(sbf_var("v" + std::to_string(i)));

		// Every coordinate independent: the all-nonzero-minterm mask (all-false)
		// requests a fresh coordinate with no forced relation to any of the ten.
		ocltl_type_mask_wide tau(ocltl_wide_minterm_count(11), false);
		ba_t b = ocltl_witness_wide<ba_t, node_t>(a, tau);

		// Spot-check pairwise independence against the first coordinate at a
		// safe k=2 (ocltl_type_of is capped at ocltl_max_k): no minterm among
		// the four (a[0], b) combinations should be forced zero.
		std::vector<ba_t> pair{ a[0], b };
		CHECK(ocltl_type_of<ba_t, node_t>(pair) == 0);
	}
}

TEST_SUITE("ocltl_types: empty tuple (k=0)") {

	size_t count_valid_empty() {
		size_t n = 0;
		for (ocltl_type_mask z = 0; z < (ocltl_type_mask{1} << ocltl_minterm_count(0)); ++z)
			if (ocltl_is_valid_type(z, 0)) ++n;
		return n;
	}

	TEST_CASE("|T_0| = 1") {
		CHECK(count_valid_empty() == 1);
		CHECK(ocltl_type_count(0) == 1);
	}

	TEST_CASE("ocltl_type_of of the empty tuple is the unique T_0 element") {
		std::vector<ba_t> empty;
		CHECK(ocltl_type_of<ba_t, node_t>(empty) == 0);
	}

	TEST_CASE("ocltl_witness on an empty tuple decodes the three T_1 masks") {
		std::vector<ba_t> empty;
		ba_t unit = ocltl_unit<ba_t, node_t>();

		ba_t b01 = ocltl_witness<ba_t, node_t>(empty, ocltl_type_mask{0b01});
		CHECK(b01 == unit);

		ba_t b10 = ocltl_witness<ba_t, node_t>(empty, ocltl_type_mask{0b10});
		CHECK(is_sbf_zero(b10));

		ba_t b00 = ocltl_witness<ba_t, node_t>(empty, ocltl_type_mask{0b00});
		CHECK_FALSE(is_sbf_zero(b00));
		CHECK_FALSE(b00 == unit);
	}
}
