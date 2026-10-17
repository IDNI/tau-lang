// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Fuzz / property-based tests for LTL(ABA) synthesis.
//
// Generates random propositional LTL formulas over two variables (o1, i1)
// and checks these properties:
//
//   CRASH   : tau does not crash or throw for any generated formula.
//   DETERM  : the same formula produces the same verdict when evaluated 3×.
//   CROSS   : tau's verdict matches Spot's ltlsynt verdict (gold-standard
//             oracle for propositional LTL realizability), and the run of
//             a realizable bv[1] formula on random inputs violates none of
//             its clauses.
//
// Atom mapping (tau ↔ Spot), untyped: each atom is a proposition of its
// own, and no step has both atoms of a variable (see ATOMS):
//   (o1[t] = 1)  ↔  o1a      (o1[t] = 0)  ↔  o1b
//   (i1[t] = 1)  ↔  i1a      (i1[t] = 0)  ↔  i1b
// over bv[1], `(x = 0)` is `!(x = 1)` (see BV1_ATOMS).
//
// Run count is controlled by the TAU_FUZZ_COUNT environment variable
// (default 300 for CI; set to 10000 for overnight runs).
// Seed is controlled by TAU_FUZZ_SEED (default 42).

// GCC fires -Wstrict-overflow on loop-with-break patterns at -O2; all counters here
// are clearly bounded so this is a false positive.
#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wstrict-overflow"
#endif

#include "test_init.h"
#include "test_tau_helpers.h"

#include <random>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <limits>
#include <unordered_set>

using namespace idni::tau_lang;

static void do_gc() {
	std::unordered_set<tref> keep;
	// The Tau-BDD store is never swept and holds raw Tau trefs.
	tau_term_bdd<node_t>::collect_live_refs(keep);
	tau::gc(keep);
}
using std::string;
using std::mt19937;
using std::uint32_t;

// ── formula pair (tau syntax + Spot syntax) ────────────────────────────────────

struct formula {
	string tau; // tau-lang spec string (needs trailing "." to be a full spec)
	string spot; // Spot/ltlsynt LTL string
	bool future = false; // has G, F, U, W or R
	// Spot reading of the formula's top-level conjuncts, each without a
	// future operator read as G, as tau reads it beside a temporal one
	string scoped = spot;
};

// The Spot formula of what tau decides for f as a whole specification: a
// top-level conjunct without a future operator, and a specification
// without one, hold at every step.
static string top_spot(const formula& f) {
	if (f.spot.empty()) return f.spot;
	return f.future ? f.scoped : "G(" + f.spot + ")";
}

// Untyped atoms live in the tau algebra, which is atomless: a variable can
// be 0, 1 or neither, so `(i1=0) || (i1=1)` is no tautology. Each atom is
// therefore a proposition of its own, and the only relation between the two
// atoms of a variable is that they never hold together (UNTYPED_ASSUME,
// UNTYPED_GUARANTEE), which makes the Spot reading exact.
static const formula ATOMS[] = {
	{"(o1[t] = 1)", "o1a"},
	{"(o1[t] = 0)", "o1b"},
	{"(i1[t] = 1)", "i1a"},
	{"(i1[t] = 0)", "i1b"},
};
static const string UNTYPED_ASSUME = "G(!(i1a & i1b))";
static const string UNTYPED_GUARANTEE = "G(!(o1a & o1b))";
static constexpr int N_ATOMS = 4;

// Two-element atoms: bv[1] has exactly the elements {0, 1}, so `(x = 0) ||
// (x = 1)` IS a tautology and every verdict must agree with Spot's
// propositional reading. Mismatches on this atom set are genuine
// disagreements, which is what lets the CROSS-bv1 suite run with zero
// tolerance where the untyped suite can only be informational.
// Stream types are process-global once inferred, and the suites above fix
// o1/i1 as :tau, so these atoms use their own streams (o2/i2); the Spot
// side keeps o1/i1, which spot_decide declares as output/input.
static const formula BV1_ATOMS[] = {
	{"(o2[t]:bv[1] = 1)", "o1"},
	{"(o2[t]:bv[1] = 0)", "(!o1)"},
	{"(i2[t]:bv[1] = 1)", "i1"},
	{"(i2[t]:bv[1] = 0)", "(!i1)"},
};

// Two bv[1] inputs and two bv[1] outputs, on streams of their own: a
// Boolean combination of clauses over two inputs is where a strategy has to
// wait for the inputs before it picks the disjunct it fulfils.
static const formula BV1_2X2_ATOMS[] = {
	{"(o3[t]:bv[1] = 1)", "o1"},
	{"(o3[t]:bv[1] = 0)", "(!o1)"},
	{"(o4[t]:bv[1] = 1)", "o2"},
	{"(o4[t]:bv[1] = 0)", "(!o2)"},
	{"(i3[t]:bv[1] = 1)", "i1"},
	{"(i3[t]:bv[1] = 0)", "(!i1)"},
	{"(i4[t]:bv[1] = 1)", "i2"},
	{"(i4[t]:bv[1] = 0)", "(!i2)"},
};

// Recursive random formula generator over the first n_atoms of atoms.
// depth 0 → atom; higher depth → operator applied to sub-formulas.
static formula make_formula(mt19937& rng, int depth,
	const formula* atoms = ATOMS, int n_atoms = N_ATOMS)
{
	// Force atom at depth 0 or with 1/3 probability at shallow depths
	if (depth == 0 || (depth <= 2 && rng() % 3 == 0)) {
		formula a = atoms[rng() % static_cast<uint32_t>(n_atoms)];
		a.scoped = "G(" + a.spot + ")";
		return a;
	}

	// 0-2: unary, 3-10: binary (S since, -> and <-> included)
	int op = static_cast<int>(rng() % 11);
	auto unary = [](string tau, string spot, bool future) {
		formula f{ std::move(tau), std::move(spot), future };
		f.scoped = f.future ? f.spot : "G(" + f.spot + ")";
		return f;
	};
	if (op < 3) {
		auto child = make_formula(rng, depth - 1, atoms, n_atoms);
		switch (op) {
			case 0: return unary("G (" + child.tau + ")",
					"G(" + child.spot + ")", true);
			case 1: return unary("F (" + child.tau + ")",
					"F(" + child.spot + ")", true);
			case 2: return unary("!(" + child.tau + ")",
					"!(" + child.spot + ")", child.future);
		}
	}
	auto left  = make_formula(rng, depth - 1, atoms, n_atoms);
	auto right = make_formula(rng, depth - 1, atoms, n_atoms);
	// Wrap operands in extra parens for tau to avoid precedence issues.
	std::string lt = "(" + left.tau + ")";
	std::string rt = "(" + right.tau + ")";
	const bool future = left.future || right.future;
	auto binary = [&](string tau, string spot, bool fut) {
		return unary(std::move(tau), std::move(spot), fut);
	};
	switch (op) {
		case 3: {
			formula f{ "(" + left.tau + " && " + right.tau + ")",
				"(" + left.spot + " & " + right.spot + ")", future };
			f.scoped = "(" + left.scoped + " & " + right.scoped + ")";
			// a part without a future operator is read at every step
			// only beside one that has it
			if (left.spot.empty() || right.spot.empty())
				f.spot = f.scoped = "";
			return f;
		}
		case 4: return binary("(" + left.tau + " || " + right.tau + ")",
		                "(" + left.spot + " | " + right.spot + ")", future);
		case 5: return binary("(" + lt + " U "  + rt + ")",
		                "(" + left.spot + " U "  + right.spot + ")", true);
		case 6: return binary("(" + lt + " W "  + rt + ")",
		                "(" + left.spot + " W "  + right.spot + ")", true);
		case 7: return binary("(" + lt + " R "  + rt + ")",
		                "(" + left.spot + " R "  + right.spot + ")", true);
		// S (Since): tau has S; Spot uses S (past LTL).
		// Spot's ltlsynt does not support past operators, so S formulas are
		// skipped in the CROSS test (spot_decide returns -1 for past LTL).
		case 8: return { "(" + lt + " S "  + rt + ")", "", future, "" };
		case 9: return binary("(" + lt + " -> " + rt + ")",
		                "(" + left.spot + " -> " + right.spot + ")", future);
		case 10: return binary("(" + lt + " <-> " + rt + ")",
		                 "(" + left.spot + " <-> " + right.spot + ")", future);
		default: return atoms[0];
	}
}

// ── tau oracle ─────────────────────────────────────────────────────────────────

// Returns 1 (satisfiable), 0 (unsatisfiable), -1 (parse error / skip /
// undecided). Used by the CRASH and DETERM suites.
static int tau_decide(const string& spec_str) {
	auto nso = get_nso_rr<node_t>(tau::get(spec_str.c_str()).value_or(nullptr));
	if (!nso.has_value()) return -1;
	tref fm = nso.value().main->get();
	if (!fm) return -1;
	auto r = is_tau_formula_sat<node_t>(fm);
	if (!r.has_value()) return -1;
	return r.value() ? 1 : 0;
}

// Returns 1 (REALIZABLE), 0 (UNREALIZABLE), -1 (parse error), -2 (no
// verdict: UNKNOWN). ltlsynt answers realizability, so the cross-check asks
// tau the same question through api::realizable.
// Takes the bare formula (no trailing "."): api::get_formula runs the type
// inference the typed bv[1] atoms need, which the raw parse does not.
static int tau_decide_realizable(const string& formula_str) {
	auto fm = api<node_t>::get_formula(formula_str);
	if (!fm.has_value()) return -1;
	auto r = api<node_t>::realizable(fm.value());
	if (!r.has_value()) return -2;
	return r.value() ? 1 : 0;
}

// ── Spot oracle ────────────────────────────────────────────────────────────────

// Returns 1 (REALIZABLE), 0 (UNREALIZABLE), -1 (timeout/error/skip).
// @p ins and @p outs are the comma-separated input and output propositions.
static int spot_decide(const string& ltl_formula, const string& ins,
	const string& outs)
{
	// Use timeout to guard against hard instances; suppress stderr noise
	string cmd = "timeout 30 ltlsynt --formula '" + ltl_formula + "' --ins '"
		+ ins + "' --outs '" + outs + "' 2>/dev/null";
	FILE* p = popen(cmd.c_str(), "r");
	if (!p) return -1;
	char buf[8192] = {};
	size_t off = 0;
	while (off < sizeof(buf) - 1) {
		size_t n = fread(buf + off, 1, sizeof(buf) - 1 - off, p);
		if (n == 0) break;
		off += n;
	}
	pclose(p);
	string out(buf, off);
	if (out.find("UNREALIZABLE") != string::npos) return 0;
	if (out.find("REALIZABLE")   != string::npos) return 1;
	return -1;
}

// ── helpers ────────────────────────────────────────────────────────────────────

// mt19937 seeds from 32 bits only, so a seed that does not fit fails the test
// instead of losing its high bits.
static uint32_t get_env_seed(const char* name, uint32_t def) {
	auto v = test_env(name);
	v.print_pending();
	REQUIRE(v.has_value());
	if (!v.value()) return def;
	const unsigned long long seed = std::stoull(*v.value());
	if (seed > std::numeric_limits<uint32_t>::max())
		FAIL(name << "=" << seed << " does not fit the 32-bit mt19937 seed");
	return static_cast<uint32_t>(seed);
}

static int get_env_int(const char* name, int def) {
	auto v = test_env(name);
	v.print_pending();
	REQUIRE(v.has_value());
	return v.value() ? std::stoi(*v.value()) : def;
}

// The run of a realizable formula on random inputs, read clause by clause;
// the text of a violated clause, or empty.
static string run_violation(const string& formula_str) {
	auto fm = api<node_t>::get_formula(formula_str);
	if (!fm.has_value()) return {};
	auto v = ltl_self_check<node_t>(fm.value(), 32);
	return v.has_value() && v.value() ? *v.value() : string{};
}

// The cross-check over bv[1] atoms, where {0, 1} are the only elements
// and tau's semantics coincide with Spot's propositional one: every
// disagreement is a real one, so none is tolerated and each is a failure
// that names the formula. A formula without an output atom (`out` names its
// streams) is skipped; an UNKNOWN is counted and reported, not compared.
// The verdict is taken without the self-check of `realizable`, and the run
// of each realizable formula is checked here instead, so a violation fails
// rather than turning into an UNKNOWN.
static void cross_bv1(uint32_t default_seed, const formula* atoms,
	int n_atoms, const char* out, bool two, const char* name)
{
	uint32_t seed  = get_env_seed("TAU_FUZZ_SEED",  default_seed);
	int      count = get_env_int ("TAU_FUZZ_COUNT", 300) / 3;
	int      depth = get_env_int ("TAU_FUZZ_DEPTH", 3);
	if (count < 1) count = 1;
	if (depth < 1) depth = 1;
	if (depth > 5) depth = 5;
	mt19937  rng(seed);
	const size_t saved_steps = api<node_t>::get_ltl_self_check_steps();
	api<node_t>::set_ltl_self_check_steps(0);

	int checked = 0, skipped = 0, unknown = 0, runs = 0;
	for (int i = 0; i < count; ++i) {
		do_gc();
		auto f = make_formula(rng, depth, atoms, n_atoms);
		string spec = f.tau + ".";
		if (f.tau.find(out) == string::npos || f.spot.empty()) {
			++skipped; continue;
		}
		int tau_r = tau_decide_realizable(f.tau);
		if (tau_r == -2) { ++unknown; continue; }
		if (tau_r < 0) { ++skipped; continue; }
		int spot_r = spot_decide(top_spot(f), two ? "i1,i2" : "i1",
			two ? "o1,o2" : "o1");
		if (spot_r < 0) { ++skipped; continue; }
		++checked;
		std::ostringstream msg;
		msg << "formula #" << i << ": tau=" << tau_r
		    << " spot=" << spot_r
		    << "\n  tau  formula: " << spec
		    << "\n  spot formula: " << top_spot(f);
		CHECK_MESSAGE(tau_r == spot_r, msg.str());
		if (tau_r != 1) continue;
		++runs;
		const string violated = run_violation(f.tau);
		std::ostringstream run_msg;
		run_msg << "formula #" << i << ": " << violated
			<< "\n  tau  formula: " << spec;
		CHECK_MESSAGE(violated.empty(), run_msg.str());
	}
	api<node_t>::set_ltl_self_check_steps(saved_steps);
	std::ostringstream summary;
	summary << name << ": checked " << checked << " formulas, skipped "
		<< skipped << ", unknown " << unknown << ", runs checked " << runs;
	MESSAGE(summary.str());
	CHECK(checked > 0);
}

// ── tests ──────────────────────────────────────────────────────────────────────

TEST_SUITE("LTL fuzz (property-based)") {

	TEST_CASE("CRASH: no exception on random formulas"
		* doctest::skip(!ltlsynt_available())) {
		// Any formula tau can parse must not throw an exception.
		// Parse failures (nullptr) are acceptable — crashes are not.
		uint32_t seed  = get_env_seed("TAU_FUZZ_SEED",  42);
		int      count = get_env_int ("TAU_FUZZ_COUNT", 300);
		int      depth = get_env_int ("TAU_FUZZ_DEPTH", 3);
		if (depth < 1) depth = 1;
		if (depth > 5) depth = 5;
		mt19937  rng(seed);

		int parsed = 0;
		for (int i = 0; i < count; ++i) {
			do_gc();
			auto f = make_formula(rng, depth);
			int r = tau_decide(f.tau + ".");
			if (r >= 0) ++parsed;
		}
		// At least half the generated formulas must parse successfully
		const int min_parsed = count / 2;
		CHECK(parsed >= min_parsed);
	}

	TEST_CASE("DETERM: same formula gives the same verdict 3 times") {
		uint32_t seed  = get_env_seed("TAU_FUZZ_SEED",  43);
		int      count = get_env_int ("TAU_FUZZ_COUNT", 300) / 5;
		if (count < 1) count = 1;
		int      depth = get_env_int ("TAU_FUZZ_DEPTH", 3);
		if (depth < 1) depth = 1;
		if (depth > 5) depth = 5;
		mt19937  rng(seed);

		int failures = 0;
		for (int i = 0; i < count; ++i) {
			do_gc();
			auto f = make_formula(rng, depth);
			string spec = f.tau + ".";
			int r1 = tau_decide(spec);
			if (r1 < 0) continue; // skip unparseable
			int r2 = tau_decide(spec);
			int r3 = tau_decide(spec);
			if (r1 != r2 || r1 != r3) {
				FAIL_CHECK("Non-determinism for formula: " << spec
				           << " — results: " << r1 << " " << r2 << " " << r3);
				++failures;
			}
		}
		CHECK(failures == 0);
	}

	// spot_decide() popens ltlsynt directly instead of going through tau's
	// synthesis path, so the precondition is this process being able to spawn:
	// true under Node, false in a browser. Deliberately not ltlsynt_available(),
	// which answers whether tau's own path can invoke it and is false under all
	// of Emscripten -- using it here would skip the case under Node as well.
	TEST_CASE("CROSS: tau verdict matches Spot (ltlsynt) verdict"
		* doctest::skip(::system("which ltlsynt > /dev/null 2>&1") != 0)) {
		uint32_t seed  = get_env_seed("TAU_FUZZ_SEED",  44);
		int      count = get_env_int ("TAU_FUZZ_COUNT", 300);
		int      depth = get_env_int ("TAU_FUZZ_DEPTH", 3);
		if (depth < 1) depth = 1;
		if (depth > 5) depth = 5;
		mt19937  rng(seed);

		int checked = 0, skipped = 0, mismatches = 0;
		for (int i = 0; i < count; ++i) {
			do_gc();
			auto f = make_formula(rng, depth);
			string spec = f.tau + ".";

			// a formula without an output atom has nothing to synthesize
			if (f.tau.find("o1") == string::npos || f.spot.empty()) {
				++skipped; continue;
			}

			int tau_r  = tau_decide_realizable(f.tau);
			if (tau_r < 0) { ++skipped; continue; }

			int spot_r = spot_decide("(" + UNTYPED_ASSUME + ") -> (("
				+ UNTYPED_GUARANTEE + ") & (" + top_spot(f) + "))",
				"i1a,i1b", "o1a,o1b");
			if (spot_r < 0) { ++skipped; continue; }

			++checked;
			if (tau_r != spot_r) {
				// One pre-built string: under
				// DOCTEST_CONFIG_ASSERTION_PARAMETERS_BY_VALUE a
				// `<<` chain of literals is logged as pointer values.
				std::ostringstream msg;
				msg << "Mismatch #" << i << ": tau=" << tau_r
				    << " spot=" << spot_r
				    << "\n  tau  formula: " << spec
				    << "\n  spot formula: " << top_spot(f);
				MESSAGE(msg.str());
				++mismatches;
			}
		}
		std::ostringstream summary;
		summary << "Checked " << checked << " formulas, skipped " << skipped
		        << ", mismatches " << mismatches;
		MESSAGE(summary.str());
		CHECK(mismatches == 0);
	}

	// cross_bv1 over one input and one output. The atoms are typed bv[1],
	// so a pack without bv parses none of them and the case would check
	// nothing; skip it there rather than fail on the `checked > 0` guard.
	TEST_CASE("CROSS-bv1: tau verdict matches Spot over two-element atoms"
		* doctest::skip(::system("which ltlsynt > /dev/null 2>&1") != 0
			|| !pack_owns_ba_type_name<node_t>("bv"))) {
		cross_bv1(45, BV1_ATOMS, 4, "o2", false, "CROSS-bv1");
	}

	// Two inputs and two outputs, where a Boolean combination of clauses
	// over inputs needs the game rather than one disjunct at a time.
	TEST_CASE("CROSS-bv1-2x2: tau verdict matches Spot over two inputs and "
		"two outputs"
		* doctest::skip(::system("which ltlsynt > /dev/null 2>&1") != 0
			|| !pack_owns_ba_type_name<node_t>("bv"))) {
		cross_bv1(46, BV1_2X2_ATOMS, 8, "(o", true, "CROSS-bv1-2x2");
	}
}
