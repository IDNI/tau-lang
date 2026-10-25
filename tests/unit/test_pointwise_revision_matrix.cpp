// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Pointwise revision over every pair of temporal shapes: each spec shape is
// revised by each update shape. The spec shapes speak about o1 = 0 and the
// update shapes about o1 = 1, so every spec shape needs o1 = 0 at some step
// and every update shape is met by o1 = 1 at every step.
//
// [PWR-MX-REV] pins the case revise() takes for each pair of root
// operators, [PWR-MX-AGM] checks the AGM postulates of the whole revision
// for each pair, and [PWR-MX-KEEP] checks that a clause the update does not
// touch survives the revision.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "pointwise_revision.h"

using namespace idni::tau_lang;

namespace {

tref spec(const std::string& s) {
	auto nso_rr = get_nso_rr<node_t>(tau::get(s).value_or(nullptr));
	if (!nso_rr.has_value()) return nullptr;
	return nso_rr.value().main->get();
}

bool is_realizable(tref fm) {
	REQUIRE(fm != nullptr);
	auto r = is_tau_formula_sat<node_t>(fm);
	REQUIRE(r.has_value());
	return r.value();
}

// `a` entails `b` under synthesis semantics: a && !b is unrealizable.
bool entails(tref a, tref b) {
	REQUIRE(a != nullptr);
	REQUIRE(b != nullptr);
	return !is_realizable(tau::build_wff_and(a, tau::build_wff_neg(b)));
}

// A shape is a formula over the operands `A` (one atom) and `B` (a
// conjunction of two atoms); `op` is the temporal operator at its root as
// get_temporal_op reads it. O desugars to `T S _` and H to `!(T S !_)`, so
// O has S at its root and H a negation.
struct shape {
	const char* name;
	const char* fm;
	temporal_op op;
};

const shape shapes[] = {
	{ "step",   "A",                           temporal_op::NONE },
	{ "G",      "G (A)",                       temporal_op::ALWAYS },
	{ "F",      "F (A)",                       temporal_op::SOMETIMES },
	{ "O",      "O (A)",                       temporal_op::S },
	{ "H",      "H (A)",                       temporal_op::NONE },
	{ "U",      "(A) U (B)",                   temporal_op::U },
	{ "R",      "(A) R (B)",                   temporal_op::R },
	{ "W",      "(A) W (B)",                   temporal_op::W },
	{ "S",      "(A) S (B)",                   temporal_op::S },
	{ "T",      "(A) T (B)",                   temporal_op::T },
	{ "GF",     "G (F (A))",                   temporal_op::ALWAYS },
	{ "FG",     "F (G (A))",                   temporal_op::SOMETIMES },
	{ "G_U",    "G ((A) U (B))",               temporal_op::ALWAYS },
	{ "G_R",    "G ((A) R (B))",               temporal_op::ALWAYS },
	{ "G_W",    "G ((A) W (B))",               temporal_op::ALWAYS },
	{ "U_U",    "((A) U (B)) U (B)",           temporal_op::U },
	{ "U_or_G", "((A) U (B)) || (G (A))",      temporal_op::NONE },
	{ "G_and_R","(G (A)) && ((A) R (B))",      temporal_op::NONE },
};

std::string instantiate(const char* fm, const char* a, const char* b) {
	std::string s;
	for (const char* p = fm; *p; ++p)
		if (*p == 'A' && (p[1] == ')' || p[1] == '\0')) s += a;
		else if (*p == 'B' && p[1] == ')') s += b;
		else s += *p;
	return s + ".";
}

tref spec_of(const shape& sh) {
	return spec(instantiate(sh.fm, "o1[t] = 0", "o1[t] = 0 && o2[t] = 0"));
}

tref update_of(const shape& sh) {
	return spec(instantiate(sh.fm, "o1[t] = 1", "o1[t] = 1 && o2[t] = 1"));
}

bool is_binary(temporal_op op) {
	return op == temporal_op::U || op == temporal_op::R
		|| op == temporal_op::W || op == temporal_op::S
		|| op == temporal_op::T;
}

// The root operator of revise(phi, psi, _) for the root operators of phi
// and psi, case by case as revise() documents them. `alt` is the second
// answer of the cases that keep the spec's operator only when the full
// update accepts it.
struct expected_root { temporal_op op; temporal_op alt; };

expected_root expected_revise_root(const shape& phi, const shape& psi,
	bool phi_step, bool psi_step)
{
	temporal_op p = phi.op, q = psi.op;
	// Case 1: both step formulas
	if (phi_step && psi_step) return { temporal_op::NONE, temporal_op::NONE };
	// Case 2 and 2b: one operator on both sides
	if (p == q && p != temporal_op::NONE) return { p, p };
	// Case 3: a step formula lifted to the binary operator of the other side
	if (phi_step && is_binary(q)) return { q, q };
	if (is_binary(p) && psi_step) return { p, p };
	// Case 4: G against R or T, and R or T against G
	if (p == temporal_op::ALWAYS
		&& (q == temporal_op::R || q == temporal_op::T)) return { q, q };
	if ((p == temporal_op::R || p == temporal_op::T)
		&& q == temporal_op::ALWAYS) return { p, temporal_op::ALWAYS };
	// Case 5: F against U or W
	if (p == temporal_op::SOMETIMES
		&& (q == temporal_op::U || q == temporal_op::W)) return { q, q };
	// Case 6: the update wins
	return { q, q };
}

bool has_temporal(tref fm) {
	return !is_non_temporal_fm<node_t>(fm);
}

} // namespace

TEST_SUITE("[PWR-MX: pointwise revision shape matrix]") {

	// Every pair goes past the early exit of revise(): the full update
	// conjoins G(o1 = 1), which no spec shape meets.
	TEST_CASE("[PWR-MX-REV] revise() takes the documented case for every "
		"pair of shapes" * doctest::skip(!ltlsynt_available()))
	{
		tref pin = spec("G (o1[t] = 1).");
		REQUIRE(pin != nullptr);
		for (const auto& sp : shapes) for (const auto& up : shapes) {
			CAPTURE(std::string(sp.name));
			CAPTURE(std::string(up.name));
			tref phi = spec_of(sp);
			tref psi = update_of(up);
			REQUIRE(phi != nullptr);
			REQUIRE(psi != nullptr);
			REQUIRE(get_temporal_op<node_t>(phi) == sp.op);
			REQUIRE(get_temporal_op<node_t>(psi) == up.op);
			tref psi_f = build_wff_and<node_t>(psi, pin);
			auto rr = revise<node_t>(phi, psi, psi_f, 0);
			REQUIRE(rr.has_value());
			tref res = rr.value();
			REQUIRE(res != nullptr);
			auto [op, alt] = expected_revise_root(sp, up,
				!has_temporal(phi), !has_temporal(psi));
			temporal_op got = get_temporal_op<node_t>(res);
			CHECK((got == op || got == alt));
			// a step-formula case answers a step formula
			if (!has_temporal(phi) && !has_temporal(psi))
				CHECK_FALSE(has_temporal(res));
		}
	}

	// Success (the revision entails the update), consistency (a realizable
	// update gives a realizable revision) and vacuity (a spec compatible
	// with the update is conjoined with it, unchanged).
	TEST_CASE("[PWR-MX-AGM] the revision of every pair of shapes meets "
		"success, consistency and vacuity"
		* doctest::skip(!ltlsynt_available()))
	{
		size_t revised = 0, vacuous = 0;
		for (const auto& sp : shapes) for (const auto& up : shapes) {
			CAPTURE(std::string(sp.name));
			CAPTURE(std::string(up.name));
			tref s = spec_of(sp);
			tref u = update_of(up);
			REQUIRE(s != nullptr);
			REQUIRE(u != nullptr);
			REQUIRE(is_realizable(u));
			auto rr = pointwise_revision_temporal<node_t>(s, u, 0);
			REQUIRE(rr.has_value());
			tref res = rr.value();
			REQUIRE(res != nullptr);
			CHECK(is_realizable(res));
			CHECK(entails(res, u));
			if (is_realizable(build_wff_and<node_t>(s, u))) {
				++vacuous;
				CHECK(res == build_wff_and<node_t>(s, u));
			} else ++revised;
		}
		// both branches are exercised, the revising one by most pairs
		CHECK(vacuous > 0);
		CHECK(revised > vacuous);
	}

	// A spec clause over o3 alone is compatible with every update, so the
	// revision keeps it next to whatever it makes of the conflicting
	// clause.
	TEST_CASE("[PWR-MX-KEEP] a clause the update does not touch survives "
		"the revision of every pair of shapes"
		* doctest::skip(!ltlsynt_available()))
	{
		tref side = spec("G (o3[t] = 0).");
		REQUIRE(side != nullptr);
		for (const auto& sp : shapes) for (const auto& up : shapes) {
			CAPTURE(std::string(sp.name));
			CAPTURE(std::string(up.name));
			tref s = build_wff_and<node_t>(spec_of(sp), side);
			tref u = update_of(up);
			auto rr = pointwise_revision_temporal<node_t>(s, u, 0);
			REQUIRE(rr.has_value());
			tref res = rr.value();
			REQUIRE(res != nullptr);
			CAPTURE(tau::get(res).to_str());
			CHECK(entails(res, u));
			CHECK(entails(res, side));
		}
	}
}

TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
