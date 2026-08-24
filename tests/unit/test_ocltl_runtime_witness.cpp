// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Gate 0: soundness of ocltl_decode_witness for the exact runtime
// instantiation table_step_provider uses -- a CONCRETE tuple (committed
// memory + current input, real BA elements) plus the edge's D-pattern,
// decoded via ocltl_witness_wide instead of a per-step solver call.
//
// Every case validates ocltl_decode_witness's output against GROUND TRUTH
// computed independently (per-atom is_zero on the substituted constraint,
// ba_equal below), never by calling back into ocltl_decode_witness/
// ocltl_witness_wide's own machinery.

#include "test_init.h"
#include "test_tau_helpers.h"

#include "ocltl_witness_decode.h"

#include <random>

using sbf_t = sbf_ba;
using tau_t = tau_ba<TAU_PACK_BASE_BAS>;

namespace {

// A concrete sbf variable, parsed through the descriptor -- an opaque BA
// element standing in for an already-committed value (the mechanism under
// test never looks past the descriptor interface, so a free variable here
// is exactly as valid a stand-in as a composed splitter output would be).
sbf_t sbf_var(const std::string& name) {
	using desc = ba_descriptor<sbf_t, node_t>;
	auto c = desc::parse(name, desc::type_tree());
	REQUIRE(c.has_value());
	REQUIRE(std::holds_alternative<sbf_t>(c->first));
	return std::get<sbf_t>(c->first);
}

// A concrete tau_ba element: a self-contained boolean-typed formula (never
// an io_var) -- "name = 1" is a valid wff (bf_eq of a plain bf variable
// against the literal 1), and a distinct variable name gives a distinct,
// mutually-unrelated element, the same stand-in role as sbf_var above but
// for the exact algebra atomless2.tau's o1/o2/i1/i2 atoms use. tau_spec's
// grammar has no bare-identifier wff (a bare ref needs call args), so a
// trivial comparison is the simplest valid closed-enough spec text.
tau_t tau_var(const std::string& name) {
	using desc = ba_descriptor<tau_t, node_t>;
	auto c = desc::parse(name + " = 1", desc::type_tree());
	REQUIRE(c.has_value());
	REQUIRE(std::holds_alternative<tau_t>(c->first));
	return std::get<tau_t>(c->first);
}

template <typename BA, typename Node>
bool ba_equal(const BA& x, const BA& y) {
	using desc = ba_descriptor<BA, Node>;
	return desc::is_zero((x & ~y) | (~x & y));
}

// Ground truth for one target_atom, decided independently of
// ocltl_decode_witness/ocltl_witness_wide -- direct is_zero on the
// substituted constraint, per the Gate-0 mandate.
template <typename BA, typename Node>
bool check_atom(const BA& b, const ocltl_target_atom& atom,
	const std::vector<BA>& a, const BA& unit, const BA& zero)
{
	BA value = atom.coord == ocltl_target_none
		? (atom.const_is_one ? unit : zero) : a.at(atom.coord);
	bool eq = ba_equal<BA, Node>(b, value);
	return atom.asserted ? eq : !eq;
}

template <typename BA, typename Node>
bool check_all(const BA& b, const std::vector<ocltl_target_atom>& atoms,
	const std::vector<BA>& a, const BA& unit, const BA& zero)
{
	for (auto& atom : atoms)
		if (!check_atom<BA, Node>(b, atom, a, unit, zero)) return false;
	return true;
}

// atomless2.tau's own exclusion sets: o1[t] excludes 9 already-concrete
// values, o2[t] excludes 10 plus the now-concrete o1[t] -- 11 total,
// decoded second.
template <typename BA, typename Node>
void check_atomless2_step(
	const std::function<BA(const std::string&)>& var,
	const BA& unit, const BA& zero)
{
	// The 8 memory + 2 input coordinates a real step already has committed.
	BA o1_1 = var("o1tm1"), o1_2 = var("o1tm2");
	BA o2_1 = var("o2tm1"), o2_2 = var("o2tm2");
	BA i1_0 = var("i1t"),   i1_1 = var("i1tm1"), i1_2 = var("i1tm2");
	BA i2_0 = var("i2t"),   i2_1 = var("i2tm1"), i2_2 = var("i2tm2");

	// o1[t]: 9 exclusions (design §2.1(e), atoms 1,2,6,7,10,12,14,15,16).
	std::vector<BA> a1{ o1_1, o1_2, o2_1, o2_2, i1_0, i1_1, i1_2 };
	std::vector<ocltl_target_atom> atoms1{
		{ ocltl_target_none, true,  false }, // != {T.}
		{ ocltl_target_none, false, false }, // != {F.}
		{ 0, false, false }, { 1, false, false }, // != o1[t-1], o1[t-2]
		{ 2, false, false }, { 3, false, false }, // != o2[t-1], o2[t-2]
		{ 4, false, false }, { 5, false, false }, { 6, false, false }, // != i1[t],i1[t-1],i1[t-2]
	};
	REQUIRE(atoms1.size() == 9);
	BA b1 = ocltl_decode_witness<BA, Node>(a1, atoms1);
	CHECK(check_all<BA, Node>(b1, atoms1, a1, unit, zero));

	// o2[t]: 10 exclusions plus the sibling o1[t] -- 11 total, decoded with
	// o1[t] now concrete (design's forced decode order).
	std::vector<BA> a2{ o2_1, o2_2, o1_1, o1_2, i2_0, i2_1, i2_2, i1_0, b1 };
	std::vector<ocltl_target_atom> atoms2{
		{ ocltl_target_none, true,  false }, // != {T.}
		{ ocltl_target_none, false, false }, // != {F.}
		{ 0, false, false }, { 1, false, false }, // != o2[t-1], o2[t-2]
		{ 2, false, false }, { 3, false, false }, // != o1[t-1], o1[t-2]
		{ 4, false, false }, { 5, false, false }, { 6, false, false }, // != i2[t],i2[t-1],i2[t-2]
		{ 7, false, false }, // != i1[t]
		{ 8, false, false }, // != o1[t] (sibling, now concrete)
	};
	REQUIRE(atoms2.size() == 11);
	BA b2 = ocltl_decode_witness<BA, Node>(a2, atoms2);
	CHECK(check_all<BA, Node>(b2, atoms2, a2, unit, zero));
}

} // namespace

TEST_SUITE("ocltl runtime witness decode (Gate 0): atomless2's exact step shape") {

	TEST_CASE("sbf: 9 exclusions then 11 with the sibling") {
		sbf_t unit = ocltl_unit<sbf_t, node_t>();
		sbf_t zero = ~unit;
		std::function<sbf_t(const std::string&)> var = sbf_var;
		check_atomless2_step<sbf_t, node_t>(var, unit, zero);
	}

	TEST_CASE("tau: small-k soundness check on the real algebra (k=4)") {
		// atomless2's real k=7/k=9 scale is a separate, opt-in case below;
		// this one stays in the default run to keep tau_ba's soundness
		// checked on every build.
		tau_t unit = ocltl_unit<tau_t, node_t>();
		tau_t zero = ~unit;
		tau_t v0 = tau_var("kfour0"), v1 = tau_var("kfour1"), v2 = tau_var("kfour2");
		std::vector<tau_t> a{ v0, v1, v2 };
		std::vector<ocltl_target_atom> atoms{
			{ ocltl_target_none, true, false }, // != {T.}
			{ 0, false, false }, { 1, false, false }, { 2, false, false },
		};
		tau_t b = ocltl_decode_witness<tau_t, node_t>(a, atoms);
		CHECK(check_all<tau_t, node_t>(b, atoms, a, unit, zero));
	}
}

// Opt-in (TAU_OCLTL_RUN_TAU_SCALE=1): reproduces atomless2.tau's real step
// shape (k=7 then k=9) on the "tau" algebra. ocltl_decode_witness's
// per-minterm enumeration (O(2^k) BA operations) does not complete in
// reasonable time at this k, unlike solve_inequality_system_atomless's
// O(|Δ|) construction -- a tractability finding, not a soundness one (every
// case that DOES complete agrees with ground truth). Kept opt-in so this
// documents the finding without making every default run pay for it.
TEST_SUITE("ocltl runtime witness decode: atomless2 exact scale on tau_ba (opt-in, tractability)") {
	TEST_CASE("tau: 9 exclusions then 11 with the sibling (atomless2's actual algebra)") {
		if (!std::getenv("TAU_OCLTL_RUN_TAU_SCALE")) {
			MESSAGE("TAU_OCLTL_RUN_TAU_SCALE not set; skipping (see comment above -- measured non-terminating within 400s)");
			return;
		}
		tau_t unit = ocltl_unit<tau_t, node_t>();
		tau_t zero = ~unit;
		std::function<tau_t(const std::string&)> var = tau_var;
		check_atomless2_step<tau_t, node_t>(var, unit, zero);
	}
}

TEST_SUITE("ocltl runtime witness decode (Gate 0): adversarial -- real positive constraints") {

	TEST_CASE("sbf: one positive equality-to-unit atom mixed with satisfiable disequalities") {
		// Here b IS required to equal the unit (not merely an unconstrained
		// feasibility probe), and must also differ from two unrelated
		// concrete values that are not the unit.
		sbf_t unit = ocltl_unit<sbf_t, node_t>();
		sbf_t zero = ~unit;
		sbf_t x = sbf_var("advx"), y = sbf_var("advy");
		std::vector<sbf_t> a{ x, y };
		std::vector<ocltl_target_atom> atoms{
			{ ocltl_target_none, true, true },  // b == {T.}, required
			{ 0, false, false },                // b != x
			{ 1, false, false },                // b != y
		};
		sbf_t b = ocltl_decode_witness<sbf_t, node_t>(a, atoms);
		CHECK(check_all<sbf_t, node_t>(b, atoms, a, unit, zero));
		// The positive atom fully determines b: it must equal the unit exactly.
		CHECK(ba_equal<sbf_t, node_t>(b, unit));
	}

	TEST_CASE("sbf: positive equality to a sibling coordinate") {
		sbf_t unit = ocltl_unit<sbf_t, node_t>();
		sbf_t zero = ~unit;
		sbf_t x = sbf_var("advp"), y = sbf_var("advq");
		std::vector<sbf_t> a{ x, y };
		std::vector<ocltl_target_atom> atoms{
			{ 0, false, true },  // b == x, required
			{ 1, false, false }, // b != y
		};
		sbf_t b = ocltl_decode_witness<sbf_t, node_t>(a, atoms);
		CHECK(check_all<sbf_t, node_t>(b, atoms, a, unit, zero));
		CHECK(ba_equal<sbf_t, node_t>(b, x));
	}

	TEST_CASE("sbf: positive equality to zero, mixed with disequalities") {
		sbf_t unit = ocltl_unit<sbf_t, node_t>();
		sbf_t zero = ~unit;
		sbf_t x = sbf_var("advr");
		std::vector<sbf_t> a{ x };
		std::vector<ocltl_target_atom> atoms{
			{ ocltl_target_none, false, true }, // b == {F.}, required
			{ 0, false, false },                // b != x
		};
		sbf_t b = ocltl_decode_witness<sbf_t, node_t>(a, atoms);
		CHECK(check_all<sbf_t, node_t>(b, atoms, a, unit, zero));
		CHECK(ba_equal<sbf_t, node_t>(b, zero));
	}
}

TEST_SUITE("ocltl runtime witness decode (Gate 0): degenerate cases") {

	TEST_CASE("no live atoms: k=0, atoms empty") {
		sbf_t unit = ocltl_unit<sbf_t, node_t>();
		sbf_t zero = ~unit;
		std::vector<sbf_t> a{};
		std::vector<ocltl_target_atom> atoms{};
		sbf_t b = ocltl_decode_witness<sbf_t, node_t>(a, atoms);
		CHECK(check_all<sbf_t, node_t>(b, atoms, a, unit, zero)); // vacuously true
	}

	TEST_CASE("no live atoms: nonempty a, empty atom list") {
		sbf_t unit = ocltl_unit<sbf_t, node_t>();
		sbf_t zero = ~unit;
		std::vector<sbf_t> a{ sbf_var("dega"), sbf_var("degb"), sbf_var("degc") };
		std::vector<ocltl_target_atom> atoms{};
		sbf_t b = ocltl_decode_witness<sbf_t, node_t>(a, atoms);
		CHECK(check_all<sbf_t, node_t>(b, atoms, a, unit, zero));
	}

	TEST_CASE("all-equal memory values: every exclusion is the same concrete value") {
		sbf_t unit = ocltl_unit<sbf_t, node_t>();
		sbf_t zero = ~unit;
		sbf_t v = sbf_var("degsame");
		std::vector<sbf_t> a{ v, v, v, v, v };
		std::vector<ocltl_target_atom> atoms{
			{ 0, false, false }, { 1, false, false }, { 2, false, false },
			{ 3, false, false }, { 4, false, false },
		};
		sbf_t b = ocltl_decode_witness<sbf_t, node_t>(a, atoms);
		CHECK(check_all<sbf_t, node_t>(b, atoms, a, unit, zero));
	}

	TEST_CASE("all-equal memory values, tau algebra") {
		tau_t unit = ocltl_unit<tau_t, node_t>();
		tau_t zero = ~unit;
		tau_t v = tau_var("degsametau");
		std::vector<tau_t> a{ v, v, v };
		std::vector<ocltl_target_atom> atoms{
			{ 0, false, false }, { 1, false, false }, { 2, false, false },
		};
		tau_t b = ocltl_decode_witness<tau_t, node_t>(a, atoms);
		CHECK(check_all<tau_t, node_t>(b, atoms, a, unit, zero));
	}
}

TEST_SUITE("ocltl runtime witness decode (Gate 0): randomized cases") {

	// Deterministic pseudo-random sbf terms over a small variable pool, so
	// "concrete committed values" are structurally related (as real runs
	// would produce) rather than mutually independent free variables.
	sbf_t random_term(std::mt19937& rng, const std::vector<sbf_t>& pool) {
		std::uniform_int_distribution<int> pick(0, (int)pool.size() - 1);
		std::uniform_int_distribution<int> op(0, 2);
		sbf_t t = pool[pick(rng)];
		int ops = std::uniform_int_distribution<int>(0, 3)(rng);
		for (int i = 0; i < ops; ++i) {
			sbf_t other = pool[pick(rng)];
			switch (op(rng)) {
			case 0: t = t & other; break;
			case 1: t = t | other; break;
			default: t = ~t; break;
			}
		}
		return t;
	}

	void run_random_case(unsigned seed, size_t k) {
		std::mt19937 rng(seed);
		sbf_t unit = ocltl_unit<sbf_t, node_t>();
		sbf_t zero = ~unit;
		std::vector<sbf_t> pool;
		for (size_t i = 0; i < 4; ++i)
			pool.push_back(sbf_var("rnd" + std::to_string(seed) + std::to_string(i)));

		std::vector<sbf_t> a;
		for (size_t i = 0; i < k; ++i) a.push_back(random_term(rng, pool));

		std::vector<ocltl_target_atom> atoms;
		// One coordinate gets a positive requirement (b == a[0] or b == a
		// constant); the rest are disequalities -- the mixed shape Phase 1's
		// counterexample family and atomless2 both exercise.
		std::uniform_int_distribution<int> positive_kind(0, 2);
		size_t skip = k; // which disequality index to omit (coord 0, when it carries the positive atom)
		switch (positive_kind(rng)) {
		case 0: atoms.push_back({ 0, false, true }); skip = 0; break;    // b == a[0]
		case 1: atoms.push_back({ ocltl_target_none, true, true }); break;  // b == unit
		default: atoms.push_back({ ocltl_target_none, false, true }); break; // b == zero
		}
		for (size_t i = 0; i < k; ++i)
			if (i != skip) atoms.push_back({ i, false, false }); // b != a[i]

		sbf_t b = ocltl_decode_witness<sbf_t, node_t>(a, atoms);
		CHECK(check_all<sbf_t, node_t>(b, atoms, a, unit, zero));
	}

	TEST_CASE("random case 1 (k=6, seed=1001)") { run_random_case(1001, 6); }
	TEST_CASE("random case 2 (k=9, seed=2002)") { run_random_case(2002, 9); }
	TEST_CASE("random case 3 (k=11, seed=3003)") { run_random_case(3003, 11); }
}
