// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Regression tests for suspected defects of the LTL(ABA) layer.
//
// Each case asserts the correct behaviour. A case that reproduces a defect
// still open carries `doctest::should_fail()` with a one-line statement of
// the defect, so the suite stays green and the decorator must be dropped
// once the defect is fixed.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "ltl_aba.h"
#include "algorithm_d_game.h"
#include "cpp_codegen.h"
#include "interpreter.h"
#include "preferences.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace idni::tau_lang;

// The cases spawn processes and put stub tools on PATH (mkdtemp, chmod,
// setenv), which neither Windows nor Emscripten offers.
#if !defined(_WIN32) && !defined(__EMSCRIPTEN__)

#include <sys/stat.h>
#include <unistd.h>

namespace {

// The memos of the satisfiability checks exist only with TAU_CACHE, which
// the Coverage build type turns off.
#ifdef TAU_CACHE
constexpr bool memo_enabled = true;
#else
constexpr bool memo_enabled = false;
#endif

tref op_wff(const char* s) {
	tau::get_options opts;
	opts.parse.start = tau::wff;
	return tau::get(s, opts).value_or(nullptr);
}

tref op_spec(const char* s) {
	auto nso_rr = get_nso_rr<node_t>(tau::get(s).value_or(nullptr));
	if (!nso_rr.has_value()) return nullptr;
	return nso_rr.value().main->get();
}

result<bool> op_realizable(const char* s) {
	tref fm = op_spec(s);
	REQUIRE(fm != nullptr);
	return is_ltl_aba_realizable<node_t>(
		flatten_always_conjuncts<node_t>(fm), 0, false);
}

std::string report_text(const report& rep) {
	std::ostringstream os;
	rep.print(os);
	return os.str();
}

double seconds_since(std::chrono::steady_clock::time_point t0) {
	return std::chrono::duration<double>(
		std::chrono::steady_clock::now() - t0).count();
}

// The path of `tool` where tau finds it: on the current PATH, else in
// TAU_SPOT_BIN, the bin folder of the Spot store package. Empty when it is in
// neither.
std::string tool_path(const std::string& tool) {
	std::string out;
	if (FILE* p = popen(("command -v " + tool).c_str(), "r")) {
		char buf[4096];
		if (fgets(buf, sizeof(buf), p)) out = buf;
		pclose(p);
	}
	while (!out.empty() && (out.back() == '\n' || out.back() == '\r'))
		out.pop_back();
	if (!out.empty()) return out;
	if (const char* bin = std::getenv("TAU_SPOT_BIN"); bin && *bin) {
		const std::string full = std::string(bin) + "/" + tool;
		if (access(full.c_str(), X_OK) == 0) return full;
	}
	return out;
}

size_t count_lines(const std::string& path) {
	std::ifstream in(path);
	size_t n = 0;
	for (std::string l; std::getline(in, l); ) ++n;
	return n;
}

std::vector<std::string> read_lines(const std::string& path) {
	std::ifstream in(path);
	std::vector<std::string> out;
	for (std::string l; std::getline(in, l); ) out.push_back(l);
	return out;
}

// Scripts put first on PATH for the lifetime of the object. `@REAL_x@` in a
// script is replaced by the path `x` had before, `@DIR@` by the directory.
struct path_stubs {
	std::string dir, saved_path;
	explicit path_stubs(const std::map<std::string, std::string>& scripts) {
		char tmpl[] = "/tmp/tau_ltl_op_XXXXXX";
		char* d = mkdtemp(tmpl);
		REQUIRE(d != nullptr);
		dir = d;
		const char* old = std::getenv("PATH");
		saved_path = old ? old : "";
		for (const auto& [name, body] : scripts) {
			std::string text = body;
			for (const char* tool : {"ltlsynt", "ltlfilt"}) {
				const std::string key = std::string("@REAL_") + tool + "@";
				for (size_t at; (at = text.find(key)) != std::string::npos; )
					text.replace(at, key.size(), tool_path(tool));
			}
			for (size_t at; (at = text.find("@DIR@")) != std::string::npos; )
				text.replace(at, 5, dir);
			const std::string f = dir + "/" + name;
			std::ofstream(f) << text;
			chmod(f.c_str(), 0755);
		}
		setenv("PATH", (dir + ":" + saved_path).c_str(), 1);
	}
	~path_stubs() {
		setenv("PATH", saved_path.c_str(), 1);
		std::error_code ec;
		std::filesystem::remove_all(dir, ec);
	}
	std::string file(const std::string& name) const { return dir + "/" + name; }
};

// Every ltlsynt call is logged, one line of arguments per call, to @DIR@/log.
const char* logging_ltlsynt =
	"#!/bin/sh\necho \"$*\" >> @DIR@/log\nexec @REAL_ltlsynt@ \"$@\"\n";

// ltlsynt that synthesizes as usual but prints no game, so no data game
// is ever built; Algorithm D's own game (outputs d_0, d_1, ...) is kept.
const char* no_data_game_ltlsynt =
	"#!/bin/sh\n"
	"game=0; dprops=0\n"
	"for a in \"$@\"; do\n"
	"  [ \"$a\" = \"--print-game-hoa\" ] && game=1\n"
	"  case \"$a\" in --outs=d_*) dprops=1;; esac\n"
	"done\n"
	"[ $game = 1 ] && [ $dprops = 0 ] && exit 2\n"
	"exec @REAL_ltlsynt@ \"$@\"\n";

// no_data_game_ltlsynt that also logs each call's arguments, temporary
// file paths removed, one line per call to @DIR@/log.
const char* logging_no_data_game_ltlsynt =
	"#!/bin/sh\n"
	"echo \"$*\" | sed 's#/tmp/[^ ]*##' >> @DIR@/log\n"
	"game=0; dprops=0\n"
	"for a in \"$@\"; do\n"
	"  [ \"$a\" = \"--print-game-hoa\" ] && game=1\n"
	"  case \"$a\" in --outs=d_*) dprops=1;; esac\n"
	"done\n"
	"[ $game = 1 ] && [ $dprops = 0 ] && exit 2\n"
	"exec @REAL_ltlsynt@ \"$@\"\n";

// The --ins= propositions of one logged ltlsynt call.
std::set<std::string> ins_of(const std::string& call) {
	std::set<std::string> out;
	auto at = call.find("--ins=");
	if (at == std::string::npos) return out;
	std::istringstream is(call.substr(at + 6, call.find(' ', at) - at - 6));
	for (std::string p; std::getline(is, p, ','); ) out.insert(p);
	return out;
}

// ltlsynt that never gives a verdict.
const char* mute_ltlsynt = "#!/bin/sh\nexit 2\n";

// ltlsynt that answers UNREALIZABLE and prints no game, so no data game
// can decide the abstraction's verdict.
const char* refuting_ltlsynt =
	"#!/bin/sh\n"
	"for a in \"$@\"; do [ \"$a\" = \"--print-game-hoa\" ] && exit 2; done\n"
	"echo UNREALIZABLE\nexit 1\n";

struct ltl_alg_scope {
	explicit ltl_alg_scope(const std::string& alg) {
		api<node_t>::set_ltl_algorithm(alg);
	}
	~ltl_alg_scope() { api<node_t>::set_ltl_algorithm(""); }
};

// Algorithm D's product over one memory type in which exactly the
// D-patterns of `allowed` are feasible; returns whether the system wins.
bool alg_d_product_wins(const alg_d::synth_game& g, int K,
	const std::vector<int>& allowed)
{
	std::vector<omcat::qlt_type3> T3(allowed.size());
	auto pg_r = alg_d::build_product_game(g, 1, T3, allowed, K, 0);
	REQUIRE(pg_r.has_value());
	const auto& pg = pg_r.value();
	if (pg.n_states == 0) return false;
	return alg_d::zielonka_win_player1(pg).count(pg.init) != 0;
}

} // namespace

TEST_SUITE("LTL(ABA) open points: guard and HOA parsers") {

	TEST_CASE("guard DNF with cap 0 is unlimited") {
		const long saved = ltl_guard_max_cubes_param;
		api<node_t>::set_ltl_guard_max_cubes(0);
		auto conj = alg_d::hoa_guard::to_dnf("0 & 1");
		auto disj = alg_d::hoa_guard::to_dnf("0 | 1");
		auto neg  = alg_d::hoa_guard::to_dnf("!0");
		ltl_guard_max_cubes_param = saved;
		CHECK(conj.has_value());
		CHECK(disj.has_value());
		CHECK(neg.has_value());
	}

	TEST_CASE("HOA strategy with a state count beyond int is refused")
	{
		const long saved = ltl_hoa_max_states_param;
		api<node_t>::set_ltl_hoa_max_states(0);
		auto r = parse_hoa(
			"HOA: v1\nStates: 4294967297\nStart: 0\nAP: 0\n"
			"acc-name: all\nAcceptance: 0 t\n--BODY--\n"
			"State: 0\n[t] 0\n--END--\n");
		ltl_hoa_max_states_param = saved;
		CHECK_FALSE(r.has_value());
	}

	TEST_CASE("HOA strategy with an out-of-range Start is refused")
	{
		auto r = parse_hoa(
			"HOA: v1\nStates: 1\nStart: 5\nAP: 0\n"
			"acc-name: all\nAcceptance: 0 t\n--BODY--\n"
			"State: 0\n[t] 0\n--END--\n");
		CHECK_FALSE(r.has_value());
	}

	TEST_CASE("HOA strategy with an unbounded AP count is refused")
	{
		auto r = parse_hoa(
			"HOA: v1\nStates: 1\nStart: 0\nAP: 1000000 \"p0\"\n"
			"acc-name: all\nAcceptance: 0 t\n--BODY--\n"
			"State: 0\n[t] 0\n--END--\n");
		CHECK_FALSE(r.has_value());
	}
}

TEST_SUITE("LTL(ABA) open points: pairwise consistency") {

	TEST_CASE("equal-meaning ground equalities are not declared infeasible") {
		tref a = op_wff("o1[t] = 0");
		tref b = op_wff("o1[t]' = 1");
		tref c = op_wff("0 = o1[t]");
		tref d = op_wff("1 = o1[t]");
		REQUIRE(a != nullptr);
		REQUIRE(b != nullptr);
		REQUIRE(c != nullptr);
		REQUIRE(d != nullptr);
		CHECK_FALSE(ground_eq_pair_syntactically_infeasible<node_t>(a, b));
		CHECK_FALSE(ground_eq_pair_syntactically_infeasible<node_t>(a, c));
		// two distinct constants are still caught, in either orientation
		CHECK(ground_eq_pair_syntactically_infeasible<node_t>(a, d));
		CHECK(ground_eq_pair_syntactically_infeasible<node_t>(d, c));
	}
}

TEST_SUITE("LTL(ABA) open points: Algorithm D acceptance") {

	// ltlsynt's default algorithm prints the game of `GF d_0 -> GF d_1`
	// with Streett acceptance `Fin(0) | Inf(1)`. With `d_1` infeasible on
	// data, the system wins by never raising `d_0`, a play whose edges
	// carry no colour.
	TEST_CASE("Algorithm D wins a Streett game through its uncoloured edges"
		* doctest::skip(!ltlsynt_available()))
	{
		auto g = alg_d::call_ltlsynt_game("G F d_0 -> G F d_1", {},
			{"d_0", "d_1"});
		REQUIRE(g.has_value());
		const auto& G = g.value();
		REQUIRE(G.num_states > 0);
		INFO("acc_known=" << G.acc_known << " n_colors=" << G.n_colors);
		// patterns 0b00 and 0b01 (d_0 only) are the feasible ones
		CHECK(alg_d_product_wins(G, 2, {0, 1}));
	}

	TEST_CASE("Algorithm D realizes a spec won by never raising a premise"
		* doctest::skip(!ltlsynt_available()))
	{
		// o14 <= 0 forever falsifies the premise; the conclusion's atoms
		// can never hold together
		const char* s = "(G F (o14[t]:qlt > {0}:qlt)) -> "
			"(G F (o14[t]:qlt > {1}:qlt && o14[t]:qlt < {0}:qlt)).";
		auto control = op_realizable(s);
		REQUIRE(control.has_value());
		REQUIRE(control.value());
		ltl_alg_scope alg("D");
		auto r = op_realizable(s);
		REQUIRE(r.has_value());
		CHECK(r.value());
	}

	TEST_CASE("Algorithm D refuses a generalized Buchi game") {
		// Only colour 0 is ever seen, so Inf(1) fails and the system
		// cannot win; reading the condition as plain Buchi says it can.
		auto parsed = alg_d::parse_synth_game_hoa(
			"HOA: v1\nStates: 2\nStart: 0\nAP: 1 \"d_0\"\n"
			"acc-name: generalized-Buchi 2\n"
			"Acceptance: 2 Inf(0)&Inf(1)\n"
			"properties: trans-labels explicit-labels trans-acc\n"
			"spot-state-player: 0 1\ncontrollable-AP: 0\n--BODY--\n"
			"State: 0\n[t] 1\n"
			"State: 1\n[0] 0 {0}\n[!0] 0 {0}\n--END--\n");
		REQUIRE(parsed.has_value());
		const auto& G = parsed.value();
		REQUIRE(G.num_states == 2);
		CHECK_FALSE(G.acc_known);
		CHECK_FALSE(alg_d_product_wins(G, 1, {0, 1}));
	}
}

TEST_SUITE("LTL(ABA) open points: Algorithm D gates") {

	// D's single output slot would read o11 > 0 && o12 < 0 as
	// Y > 0 && Y < 0.
	TEST_CASE("Algorithm D keeps two output streams apart"
		* doctest::skip(!ltlsynt_available()))
	{
		for (const char* s : {
			"F (o10[t]:qlt > {0}:qlt && o12[t]:qlt < {0}:qlt).",
			"G F (o11[t]:qlt > {0}:qlt && o12[t]:qlt < {0}:qlt).",
		}) {
			const std::string text = s;
			CAPTURE(text);
			auto control = op_realizable(s);
			REQUIRE(control.has_value());
			REQUIRE(control.value());
			ltl_alg_scope alg("D");
			auto r = op_realizable(s);
			REQUIRE(r.has_value());
			CHECK(r.value());
		}
	}

	TEST_CASE("Algorithm D does not realize a contradiction on top"
		* doctest::skip(!ltlsynt_available()))
	{
		ltl_alg_scope alg("D");
		auto r = op_realizable(
			"F (o13[t]:qlt = {top}:qlt) && G (o13[t]:qlt != {top}:qlt).");
		REQUIRE(r.has_value());
		CHECK_FALSE(r.value());
	}
}

TEST_SUITE("LTL(ABA) open points: Spot processes") {

	TEST_CASE("constant-output fast path honours the LTL timeout"
		* doctest::skip(!ltlsynt_available()))
	{
		tref fm = op_spec("F (o15[t]:qlt > {0}:qlt).");
		REQUIRE(fm != nullptr);
		auto atoms = extract_data_atoms<node_t>(fm);
		REQUIRE(!atoms.empty());
		path_stubs stubs({{"ltlfilt", "#!/bin/sh\nexec sleep 5\n"}});
		api<node_t>::set_ltl_timeout_sec(1);
		auto t0 = std::chrono::steady_clock::now();
		(void) constant_output_realizable<node_t>(fm, atoms);
		const double elapsed = seconds_since(t0);
		api<node_t>::set_ltl_timeout_sec(-1);
		// three constant outputs, each one ltlfilt call: 15 s unbounded
		INFO("elapsed " << elapsed << " s");
		CHECK(elapsed < 10.0);
	}

	TEST_CASE("tautology check takes a formula beyond the argument limit"
		* doctest::skip(!ltlsynt_available()))
	{
		std::string phi = "1";
		while (phi.size() < 200000) phi += " & 1";
		auto r = is_tautology(phi, 60);
		INFO(report_text(r.report()));
		REQUIRE(r.has_value());
		CHECK(r.value());
	}

	TEST_CASE("a child ignoring SIGTERM does not outlive the timeout")
	{
		auto t0 = std::chrono::steady_clock::now();
		auto r = spawn_capture({"sh", "-c", "trap '' TERM; sleep 12"}, 1,
			[](int) { return true; });
		const double elapsed = seconds_since(t0);
		INFO("elapsed " << elapsed << " s");
		CHECK_FALSE(r.has_value());
		CHECK(elapsed < 8.0);
	}

	TEST_CASE("a SIGTERM the watchdog did not send is not a timeout")
	{
		auto r = spawn_capture({"sh", "-c", "kill -TERM $$"}, 30,
			[](int) { return true; });
		REQUIRE_FALSE(r.has_value());
		INFO(report_text(r.report()));
		CHECK_FALSE(report_has_attr(r.report(),
			tau_lang::label::timeout));
	}
}

TEST_SUITE("LTL(ABA) open points: budgets and memos") {

	TEST_CASE("the constant size budget is part of the options fingerprint")
	{
		const size_t saved = max_constant_size;
		const size_t before = api_detail::semantic_options_fingerprint<node_t>();
		api<node_t>::set_max_constant_size(saved + 17);
		const size_t after = api_detail::semantic_options_fingerprint<node_t>();
		api<node_t>::set_max_constant_size(saved);
		CHECK(before != after);
	}

	TEST_CASE("a repeated full-LTL satisfiability query runs no ltlsynt"
		* doctest::skip(!ltlsynt_available() || !memo_enabled))
	{
		// flatten_always_conjuncts merges the two always parts, so the
		// formula the memo stores differs from the one it is asked.
		tref fm = op_spec("always (o16[t] = 0) && always (o17[t] = 1) && "
			"((o18[t] = 0) until (o18[t] = 1)).");
		REQUIRE(fm != nullptr);
		REQUIRE(flatten_always_conjuncts<node_t>(fm) != fm);
		path_stubs stubs({{"ltlsynt", logging_ltlsynt}});
		auto first = is_tau_formula_sat<node_t>(fm, 0, false);
		REQUIRE(first.has_value());
		const size_t n1 = count_lines(stubs.file("log"));
		auto second = is_tau_formula_sat<node_t>(fm, 0, false);
		REQUIRE(second.has_value());
		const size_t n2 = count_lines(stubs.file("log")) - n1;
		CHECK(first.value() == second.value());
		INFO("first query: " << n1 << " calls, second: " << n2);
		REQUIRE(n1 > 0);
		CHECK(n2 == 0);
	}

	TEST_CASE("an undecided atom is feasible, not infeasible") {
		tref fm = op_spec("F ((fex x (x * o19[t]:bv[4])) = 1).");
		REQUIRE(fm != nullptr);
		auto atoms = extract_data_atoms<node_t>(fm);
		REQUIRE(atoms.size() == 1);
		tref atom = atoms[0].first;
		auto sat = is_non_temp_nso_satisfiable<node_t>(atom);
		REQUIRE_FALSE(sat.has_value());
		ltl_verdict_incomplete = false;
		auto existential = aba_existential_feasible<node_t>(atom);
		REQUIRE(existential.has_value());
		CHECK(existential.value());
		CHECK(ltl_verdict_incomplete);
		ltl_verdict_incomplete = false;
		auto synthesis = aba_synthesis_feasible<node_t>(atom);
		REQUIRE(synthesis.has_value());
		CHECK(synthesis.value());
		CHECK(ltl_verdict_incomplete);
		// an edge is accepted only on a proven answer
		ltl_verdict_incomplete = false;
		auto proven = aba_existential_proven_feasible<node_t>(atom);
		REQUIRE(proven.has_value());
		CHECK_FALSE(proven.value());
		CHECK(ltl_verdict_incomplete);
		ltl_verdict_incomplete = false;
	}
}

TEST_SUITE("LTL(ABA) open points: consistency constraints") {

	TEST_CASE("a recorded implication does not subsume a k-ary forbid")
	{
		tref fm = op_spec("G F (o21[t]:qlt > o22[t]:qlt && "
			"o22[t]:qlt > o23[t]:qlt && o23[t]:qlt > o21[t]:qlt).");
		REQUIRE(fm != nullptr);
		auto atoms = extract_data_atoms<node_t>(fm);
		REQUIRE(atoms.size() == 3);
		auto forbids_all = [&](const std::string& skel) {
			for (const auto& [_, p] : atoms)
				if (skel.find(p) == std::string::npos) return false;
			return skel.find("G(!(") != std::string::npos;
		};
		{
			std::string skel = "base";
			std::vector<std::string> out;
			extend_consistency_positive_k_ary<node_t>(atoms, skel, &out);
			INFO(skel);
			REQUIRE(forbids_all(skel));
		}
		std::string skel = "base";
		std::vector<std::string> out = {
			"G(" + atoms[0].second + " -> " + atoms[1].second + ")" };
		extend_consistency_positive_k_ary<node_t>(atoms, skel, &out);
		INFO(skel);
		CHECK(forbids_all(skel));
	}

	TEST_CASE("a stream-free residue in the skeleton is refused") {
		tref fm = op_spec("F (o24[t]:qlt = {0}:qlt) && "
			"G (o24[t]:qlt = {0}:qlt || {c}:qlt > {0}:qlt).");
		REQUIRE(fm != nullptr);
		auto atoms = extract_data_atoms<node_t>(fm);
		auto skel = ltl_skeleton_with_testers<node_t>(fm, atoms);
		INFO("skeleton: " << (skel.has_value() ? skel.value().first : ""));
		CHECK_FALSE(skel.has_value());
	}
}

TEST_SUITE("LTL(ABA) open points: validity") {

	// valid reads i29 as an output; its new name must not be o_in_i29
	TEST_CASE("valid keeps an input apart from an output named after it") {
		auto safety = api<node_t>::valid("always (o_in_i29[t] = i29[t])");
		REQUIRE(safety.has_value());
		CHECK_FALSE(safety.value());
		auto ltl = api<node_t>::valid("G F (o_in_i29[t] = i29[t])");
		REQUIRE(ltl.has_value());
		CHECK_FALSE(ltl.value());
	}
}

TEST_SUITE("LTL(ABA) open points: execution") {

	TEST_CASE("the counter route reports an undecided spec as unknown") {
		path_stubs stubs({{"ltlsynt", mute_ltlsynt}});
		io_context<node_t> ctx;
		auto nso = get_nso_rr<node_t>(ctx, tau::get("always o32[1] = 1 && "
			"o32[0] = 0 && o32[t] = o31[t-1] && o31[t-1] = 1.")
			.value_or(nullptr));
		REQUIRE(nso.has_value());
		auto i = interpreter<node_t>::make_interpreter(
			nso.value().main->get(), ctx);
		REQUIRE_FALSE(i.has_value());
		const std::string text = report_text(i.report());
		INFO(text);
		CHECK(text.find("Tau specification is unsat") == std::string::npos);
	}

	// Only the data game refutes on the default path: an abstraction that
	// is unrealizable while no data game decides is undecided.
	TEST_CASE("the counter route reports an undecided refutation as unknown") {
		path_stubs stubs({{"ltlsynt", refuting_ltlsynt}});
		io_context<node_t> ctx;
		auto nso = get_nso_rr<node_t>(ctx, tau::get("always o32[1] = 1 && "
			"o32[0] = 0 && o32[t] = o31[t-1] && o31[t-1] = 1.")
			.value_or(nullptr));
		REQUIRE(nso.has_value());
		auto i = interpreter<node_t>::make_interpreter(
			nso.value().main->get(), ctx);
		REQUIRE_FALSE(i.has_value());
		const std::string text = report_text(i.report());
		INFO(text);
		CHECK(text.find("Tau specification is unsat") == std::string::npos);
		CHECK(text.find("UNKNOWN") != std::string::npos);
	}

	TEST_CASE("execution stops once the data game refutes the spec"
		* doctest::skip(!ltlsynt_available()))
	{
		// o43 is o41 two steps back, which the abstraction's windows miss
		tref fm = op_spec("G (o42[t]:bv[1] = o41[t-1]:bv[1]) && "
			"G (o43[t]:bv[1] = o42[t-1]:bv[1]) && "
			"G (F (o43[t]:bv[1] != o41[t-2]:bv[1])).");
		REQUIRE(fm != nullptr);
		path_stubs stubs({{"ltlsynt", logging_ltlsynt}});
		std::shared_ptr<data_game_strategy<node_t>> ds;
		bool unrealizable = false;
		(void) ltl_to_safety_formula_full<node_t>(fm, &ds, false,
			&unrealizable);
		auto calls = read_lines(stubs.file("log"));
		size_t game = calls.size(), after = 0;
		for (size_t k = 0; k < calls.size(); ++k)
			if (calls[k].find("--print-game-hoa") != std::string::npos)
				{ game = k; break; }
		for (size_t k = game + 1; k < calls.size(); ++k) ++after;
		INFO("calls: " << calls.size() << ", after the game: " << after);
		REQUIRE(unrealizable);
		CHECK(after == 0);
	}

	TEST_CASE("run follows realizable for CTL* specifications"
		* doctest::skip(!ltlsynt_available()))
	{
		for (const char* spec_text : {
			"A G F (o51[t] = 1)",
			"A (G F o51[t] = 1) && E (F o52[t] = 1)",
			"!(E F o51[t] = 1)",
			"A ((o51[t] = 1) until (o52[t] = 1)) && "
				"A G (o51[t] = 1 || o51[t] = 0)",
			"!!(A F o51[t] = 1)",
			"A (F o51[t] = 1 && (o52[t] = 0 || o52[t] != 0))",
		}) {
			const std::string s = spec_text;
			CAPTURE(s);
			auto rz = api<node_t>::realizable(s);
			INFO(report_text(rz.report()));
			REQUIRE(rz.has_value());
			REQUIRE(rz.value());
			io_context<node_t> ctx;
			auto nso = get_nso_rr<node_t>(ctx,
				tau::get(s + ".").value_or(nullptr));
			REQUIRE(nso.has_value());
			auto i = interpreter<node_t>::make_interpreter(
				nso.value().main->get(), ctx);
			INFO(report_text(i.report()));
			CHECK(i.has_value());
		}
	}

	TEST_CASE("a spec decided by Algorithm B runs when the data game declines"
		* doctest::skip(!ltlsynt_available()))
	{
		path_stubs stubs({{"ltlsynt", no_data_game_ltlsynt}});
		for (const char* s : {
			"G F (o61[t]:qlt > i61[t]:qlt)",
			"G F (o61[t]:qlt > i61[t]:qlt) && G F (o61[t]:qlt < {0}:qlt)",
			"G (F (o61[t]:qlt > i61[t]:qlt) && F (o61[t]:qlt < i61[t]:qlt))",
			"F G (o61[t]:qlt >= i61[t]:qlt || o61[t]:qlt = {3}:qlt)",
		}) {
			const std::string text = s;
			CAPTURE(text);
			tref fm = op_spec((std::string(s) + ".").c_str());
			REQUIRE(fm != nullptr);
			auto fast = solve_ltl_aba<node_t>(fm);
			REQUIRE(fast.has_value());
			REQUIRE(fast.value().has_value());
			REQUIRE_FALSE(fast.value()->executable);
			auto rz = api<node_t>::realizable(std::string(s));
			INFO(report_text(rz.report()));
			REQUIRE(rz.has_value());
			REQUIRE(rz.value());
			io_context<node_t> ctx;
			auto nso = get_nso_rr<node_t>(ctx,
				tau::get(std::string(s) + ".").value_or(nullptr));
			REQUIRE(nso.has_value());
			auto i = interpreter<node_t>::make_interpreter(
				nso.value().main->get(), ctx);
			INFO(report_text(i.report()));
			CHECK(i.has_value());
		}
	}

	// A reset restarts the revised spec, so its fixed step stays at 6.
	TEST_CASE("a revised data-game run keeps its fixed steps after reset"
		* doctest::skip(!ltlsynt_available()))
	{
		io_context<node_t> ctx;
		auto nso = get_nso_rr<node_t>(ctx,
			tau::get("always F (o71[t]:bv[1] = 0).").value_or(nullptr));
		REQUIRE(nso.has_value());
		auto made = interpreter<node_t>::make_interpreter(
			nso.value().main->get(), ctx);
		REQUIRE(made.has_value());
		auto& i = made.value();
		REQUIRE(i.plays_data_game());
		std::map<std::string, std::string> values;
		auto run = [&](int steps) {
			values.clear();
			for (int k = 0; k < steps; ++k) {
				auto sr = i.step();
				REQUIRE(sr.has_value());
				if (const auto& out = sr.value().first)
					for (const auto& [var, val] : *out)
						values[tau::get(var).to_str()] =
							tau::get(val).to_str();
			}
		};
		run(2);
		// made at step 2, the update's fixed step 4 is step 6
		auto psi = api<node_t>::get_formula("o71[4]:bv[1] = 1");
		REQUIRE(psi.has_value());
		auto accepted = i.update(psi.value());
		REQUIRE(accepted.has_value());
		REQUIRE(accepted.value());
		REQUIRE(i.plays_data_game());
		run(5);
		REQUIRE(values["o71[6]:bv[1]"] == "1");
		i.reset();
		run(7);
		INFO("o71[4] = " << values["o71[4]:bv[1]"]);
		CHECK(values["o71[6]:bv[1]"] == "1");
	}

	// The update names a stream the running spec does not, with no type.
	TEST_CASE("a data-game run accepts an update on a new untyped stream"
		* doctest::skip(!ltlsynt_available()))
	{
		io_context<node_t> ctx;
		auto nso = get_nso_rr<node_t>(ctx,
			tau::get("always F (o1[t] = 0).").value_or(nullptr));
		REQUIRE(nso.has_value());
		auto made = interpreter<node_t>::make_interpreter(
			nso.value().main->get(), ctx);
		REQUIRE(made.has_value());
		auto& i = made.value();
		REQUIRE(i.plays_data_game());
		REQUIRE(i.step().has_value());
		auto psi = api<node_t>::get_formula("always F (o2[t] = 1)");
		REQUIRE(psi.has_value());
		auto accepted = i.update(psi.value());
		REQUIRE(accepted.has_value());
		CHECK(accepted.value());
		CHECK(i.step().has_value());
	}

	// An untyped stream of the update named like a running one takes its
	// type, so its values are the running stream's.
	TEST_CASE("an untyped update of a running stream takes its type"
		* doctest::skip(!ltlsynt_available()))
	{
		io_context<node_t> ctx;
		auto nso = get_nso_rr<node_t>(ctx,
			tau::get("always F (o1[t]:bv[8] = 0).").value_or(nullptr));
		REQUIRE(nso.has_value());
		auto made = interpreter<node_t>::make_interpreter(
			nso.value().main->get(), ctx);
		REQUIRE(made.has_value());
		auto& i = made.value();
		REQUIRE(i.plays_data_game());
		REQUIRE(i.step().has_value());
		auto psi = api<node_t>::get_formula("o1[3] = {7}");
		REQUIRE(psi.has_value());
		auto accepted = i.update(psi.value());
		REQUIRE(accepted.has_value());
		REQUIRE(accepted.value());
		std::map<std::string, std::string> values;
		for (int k = 0; k < 4; ++k) {
			auto sr = i.step();
			REQUIRE(sr.has_value());
			if (const auto& out = sr.value().first)
				for (const auto& [var, val] : *out)
					values[tau::get(var).to_str()] =
						tau::get(val).to_str();
		}
		// made at step 1, the update's fixed step 3 is step 4
		CHECK(values["o1[4]:bv[8]"] == "{ 7 }:bv[8]");
	}

	TEST_CASE("an update giving a running stream another type is refused"
		* doctest::skip(!ltlsynt_available()))
	{
		io_context<node_t> ctx;
		auto nso = get_nso_rr<node_t>(ctx,
			tau::get("always F (o1[t]:bv[8] = 0).").value_or(nullptr));
		REQUIRE(nso.has_value());
		auto made = interpreter<node_t>::make_interpreter(
			nso.value().main->get(), ctx);
		REQUIRE(made.has_value());
		auto& i = made.value();
		REQUIRE(i.plays_data_game());
		REQUIRE(i.step().has_value());
		auto psi = api<node_t>::get_formula("o1[3]:sbf = 1");
		REQUIRE(psi.has_value());
		auto accepted = i.update(psi.value());
		REQUIRE(accepted.has_value());
		CHECK_FALSE(accepted.value());
		CHECK(i.step().has_value());
	}

	TEST_CASE("codegen refuses a strategy edge whose guard does not parse"
		* doctest::skip(!ltlsynt_available()))
	{
		auto fm = api<node_t>::get_formula("G(o81[t]:qlt > {1/2}:qlt)");
		REQUIRE(fm.has_value());
		auto solved = solve_ltl_aba<node_t>(fm.value());
		REQUIRE(solved.has_value());
		REQUIRE(solved.value().has_value());
		auto sol = *solved.value();
		REQUIRE(sol.aut.num_states > 0);
		REQUIRE(!sol.aut.edges[0].empty());
		sol.aut.edges[0][0].guard_label = "0 &";
		auto d = build_program_desc<node_t>(sol, "garbled_guard");
		CHECK_FALSE(d.has_value());
	}

	TEST_CASE("preferences keep a realizable CTL* strengthening"
		* doctest::skip(!ltlsynt_available()))
	{
		auto both = api<node_t>::realizable(
			std::string("A (G F (o91[t] = 1)) && G (o92[t] = 0)"));
		REQUIRE(both.has_value());
		REQUIRE(both.value());
		auto spec = api<node_t>::get_formula("A (G F (o91[t] = 1))");
		REQUIRE(spec.has_value());
		preference_order po;
		po.entries.push_back({"o92", "0"});
		tref out = apply_preferences<node_t>(spec.value(), po);
		CHECK(out != spec.value());
	}
}

TEST_SUITE("LTL(ABA) open points: sound abstraction") {

	// Realizable: o101[1] = i101[0] meets the right-hand side at step 1. With
	// no data game, the refined abstraction has no strategy and the
	// answer is settled on the observed abstraction, whose input twin
	// stands for the past-reading input atom i101[t-1] = 1.
	const char* past_input_until = "(o101[t]:bv[1] = 0) until "
		"(i101[t-1]:bv[1] = 1 || i101[t]:bv[1] = 0 || "
		"o101[t]:bv[1] = i101[t-1]:bv[1]).";

	// The call of the sound abstraction: the first one with more inputs
	// than the default abstraction's.
	size_t sound_call(const std::vector<std::string>& calls) {
		REQUIRE(!calls.empty());
		const auto base = ins_of(calls[0]);
		for (size_t k = 1; k < calls.size(); ++k)
			if (ins_of(calls[k]).size() > base.size()) return k;
		return calls.size();
	}

	// Every ltlsynt call after the sound one keeps its input twins.
	TEST_CASE("the sound abstraction keeps its input twins when it observes"
		* doctest::skip(!ltlsynt_available()))
	{
		path_stubs stubs({{"ltlsynt", logging_no_data_game_ltlsynt}});
		tref fm = op_spec(past_input_until);
		REQUIRE(fm != nullptr);
		auto r = is_ltl_aba_realizable<node_t>(fm, 0, false);
		auto calls = read_lines(stubs.file("log"));
		const size_t k = sound_call(calls);
		REQUIRE(k < calls.size());
		const auto twins = ins_of(calls[k]);
		for (size_t j = k + 1; j < calls.size(); ++j) {
			CAPTURE(calls[j]);
			const auto ins = ins_of(calls[j]);
			CHECK(std::includes(ins.begin(), ins.end(),
				twins.begin(), twins.end()));
		}
		if (r.has_value()) CHECK(r.value());
	}

	// The sound abstraction's solution is an observed one, so its first
	// refinement adds observations.
	TEST_CASE("the sound abstraction's first refinement adds observations"
		* doctest::skip(!ltlsynt_available()))
	{
		path_stubs stubs({{"ltlsynt", logging_no_data_game_ltlsynt}});
		tref fm = op_spec(past_input_until);
		REQUIRE(fm != nullptr);
		(void) is_ltl_aba_realizable<node_t>(fm, 0, false);
		auto calls = read_lines(stubs.file("log"));
		const size_t k = sound_call(calls);
		REQUIRE(k < calls.size());
		if (k + 1 < calls.size()) {
			CAPTURE(calls[k]);
			CAPTURE(calls[k + 1]);
			CHECK(ins_of(calls[k + 1]).size() > ins_of(calls[k]).size());
		}
	}
}

TEST_SUITE("LTL(ABA) open points: initial memory") {

	// Algorithm A lets the first move follow any previous type, while
	// Algorithm D fixes it to the type of 0, the value the interpreter
	// supplies. On a step-0 read of o[t-1] the two must still agree, and a
	// realizable verdict must be one run can execute.
	TEST_CASE("Algorithms A and D agree on a first step reading the past"
		* doctest::skip(!ltlsynt_available()))
	{
		for (const char* spec_text : {
			"G (o111[t-1]:qlt = {1}:qlt) && F (o111[t]:qlt > {0}:qlt)",
			"G F (o112[t-1]:qlt = {1}:qlt && o112[t]:qlt > {0}:qlt)",
			"F (o113[t-1]:qlt = {1}:qlt)",
			"G (o114[t-1]:qlt = {1}:qlt || o114[t]:qlt = {5}:qlt) && "
				"F (o114[t]:qlt < {0}:qlt)",
		}) {
			const std::string s = spec_text;
			CAPTURE(s);
			std::map<std::string, std::optional<bool>> verdict;
			for (const char* alg : {"", "A", "D"}) {
				ltl_alg_scope scope(alg);
				auto r = api<node_t>::realizable(s);
				verdict[alg] = r.has_value()
					? std::optional<bool>(r.value()) : std::nullopt;
			}
			REQUIRE(verdict["A"].has_value());
			CHECK(verdict["A"] == verdict["D"]);
			CHECK(verdict["A"] == verdict[""]);
			io_context<node_t> ctx;
			auto nso = get_nso_rr<node_t>(ctx,
				tau::get(s + ".").value_or(nullptr));
			REQUIRE(nso.has_value());
			auto i = interpreter<node_t>::make_interpreter(
				nso.value().main->get(), ctx);
			CHECK(i.has_value() == *verdict["A"]);
			if (i.has_value()) CHECK(i.value().step().has_value());
		}
	}

	// A formula reading the past needs a step guard, and solve_ltl_aba
	// offers no step-guarded formula to the qlt fast paths: A never sees
	// a first step reading o[t-1], so its props stay p_i, not d_i.
	TEST_CASE("Algorithm A is not offered a formula reading the past"
		* doctest::skip(!ltlsynt_available()))
	{
		ltl_alg_scope alg("A");
		tref fm = op_spec("F (o115[t]:qlt > {1}:qlt) && "
			"G (o115[t]:qlt > o115[t-1]:qlt).");
		REQUIRE(fm != nullptr);
		REQUIRE_FALSE(collect_step_guards<node_t>(fm).empty());
		auto solved = solve_ltl_aba<node_t>(fm);
		REQUIRE(solved.has_value());
		REQUIRE(solved.value().has_value());
		for (const auto& [_, prop] : solved.value()->atoms) {
			CAPTURE(prop);
			CHECK(prop.rfind("d_", 0) != 0);
		}
	}
}



TEST_SUITE("LTL(ABA) open points: mixed algebras") {

	TEST_CASE("a quantifier prefix over two algebras is decided") {
		tref sat = op_spec("ex x:sbf ex y:tau "
			"((x = 0 || y = 0) && (x != 0 || y != 0)).");
		tref unsat = op_spec("ex x:sbf ex y:qlt (x != 0 && "
			"y > {0}:qlt && (x = 0 || y < {0}:qlt)).");
		REQUIRE(sat != nullptr);
		REQUIRE(unsat != nullptr);
		auto r1 = normalizer<node_t>(sat);
		auto r2 = normalizer<node_t>(unsat);
		REQUIRE(r1.has_value());
		REQUIRE(r2.has_value());
		CHECK(tau::get(r1.value()).equals_T());
		CHECK(tau::get(r2.value()).equals_F());
	}

	// Defect: one stream make_code_window cannot code makes the whole
	// window nullopt, so the game on codes declines the codable streams too.
	TEST_CASE("an uncodable stream leaves the other streams their codes"
		* doctest::skip(!ltlsynt_available()) * doctest::should_fail())
	{
		// o2 is read against the sbf constant X, which no code stands for
		tref fm = op_spec("G (F (o3[t]:bv[1] != o3[t-1]:bv[1])) && "
			"G (F (o2[t]:sbf = {X}:sbf)).");
		REQUIRE(fm != nullptr);
		ltl_aba_solution<node_t> partial;
		auto m = solve_ltl_aba<node_t>(fm, &partial);
		REQUIRE(m.has_value());
		const ltl_aba_solution<node_t> s = m.value() ? *m.value() : partial;
		REQUIRE_FALSE(s.game_skeleton.empty());
		auto on_formulas = solve_data_game<node_t>(s.game_skeleton, s.atoms,
			s.input_props, s.output_props, true);
		auto on_codes = solve_data_game<node_t>(s.game_skeleton, s.atoms,
			s.input_props, s.output_props, false);
		REQUIRE(on_formulas.has_value());
		REQUIRE(on_codes.has_value());
		CHECK(on_formulas.value() == data_game_verdict::realizable);
		CHECK(on_codes.value() == data_game_verdict::realizable);
	}
}

#endif // !_WIN32 && !__EMSCRIPTEN__
