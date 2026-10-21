// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// minimize_strategy: the bisimulation quotient behind ltl-minimize-strategy.
// Each case checks the state count and that the quotient plays the same
// labelled runs as the automaton it came from.

#include "test_init.h"
#include "test_tau_helpers.h"

#include <random>

using namespace idni::tau_lang;

namespace {

hoa_automaton automaton(size_t aps, size_t initial,
	std::vector<std::vector<std::pair<std::string, size_t>>> edges)
{
	hoa_automaton a;
	a.num_states = edges.size();
	a.initial_state = initial;
	for (size_t k = 0; k < aps; ++k) a.aps.push_back("p" + std::to_string(k));
	a.edges.resize(edges.size());
	for (size_t s = 0; s < edges.size(); ++s)
		for (auto& [g, d] : edges[s]) a.edges[s].push_back({ g, d, false });
	a.state_accepting.assign(edges.size(), false);
	return a;
}

// The value of guard @p g at the valuation @p v (bit i is proposition i).
bool holds(const std::string& g, size_t aps, size_t v) {
	auto t = strategy_minimize_detail::guard_reader(g, aps).read();
	REQUIRE(t.has_value());
	return ((*t)[v / 64] >> (v % 64)) & 1;
}

// The states reachable from @p from at valuation @p v.
std::set<size_t> step(const hoa_automaton& a, const std::set<size_t>& from,
	size_t v)
{
	std::set<size_t> to;
	for (size_t s : from)
		for (const auto& e : a.edges[s])
			if (holds(e.guard_label, a.aps.size(), v)) to.insert(e.dst);
	return to;
}

// Both automata accept the same valuation at each step of @p runs random
// runs of @p length steps: a run that one of them can no longer follow, the
// other cannot either.
void same_runs(const hoa_automaton& a, const hoa_automaton& b,
	size_t runs = 200, size_t length = 12)
{
	std::mt19937 gen(7);
	const size_t vals = size_t{ 1 } << a.aps.size();
	for (size_t k = 0; k < runs; ++k) {
		std::set<size_t> x{ a.initial_state }, y{ b.initial_state };
		for (size_t i = 0; i < length; ++i) {
			size_t v = gen() % vals;
			x = step(a, x, v);
			y = step(b, y, v);
			CHECK(x.empty() == y.empty());
			if (x.empty() || y.empty()) break;
		}
	}
}

} // namespace

TEST_SUITE("minimize_strategy") {

	TEST_CASE("[MIN-01] two states with the same moves become one") {
		auto a = automaton(2, 0, {
			{ { "0", 1 }, { "!0", 2 } },
			{ { "1", 0 } },
			{ { "1", 0 } } });
		auto q = minimize_strategy(a);
		CHECK(q.num_states == 2);
		same_runs(a, q);
	}

	TEST_CASE("[MIN-02] guards are compared by their truth table") {
		// `0&1 | 0&!1` and `0` are the same guard
		auto a = automaton(2, 0, {
			{ { "0", 1 }, { "!0", 2 } },
			{ { "0&1 | 0&!1", 0 } },
			{ { "0", 0 } } });
		auto q = minimize_strategy(a);
		CHECK(q.num_states == 2);
		same_runs(a, q);
	}

	TEST_CASE("[MIN-03] a minimal cycle keeps its states") {
		auto a = automaton(4, 0, {
			{ { "1&!2&!3", 1 } },
			{ { "1&!2&!3", 2 } },
			{ { "!1&3", 0 } } });
		CHECK(minimize_strategy(a).num_states == 3);
	}

	TEST_CASE("[MIN-04] unreachable states are dropped, the start becomes 0") {
		auto a = automaton(1, 2, {
			{ { "t", 0 } },
			{ { "0", 1 } },
			{ { "0", 2 }, { "!0", 2 } } });
		auto q = minimize_strategy(a);
		CHECK(q.num_states == 1);
		CHECK(q.initial_state == 0);
		same_runs(a, q);
	}

	TEST_CASE("[MIN-05] the edges to one class are united") {
		// states 1 and 2 move alike, so state 0's two edges reach one
		// class and, with `0 | !0` = `t`, state 0 moves alike too
		auto a = automaton(1, 0, {
			{ { "0", 1 }, { "!0", 2 } },
			{ { "t", 1 } },
			{ { "t", 2 } } });
		auto q = minimize_strategy(a);
		CHECK(q.num_states == 1);
		same_runs(a, q);
	}

	TEST_CASE("[MIN-06] an automaton with acceptance marks is kept") {
		auto a = automaton(1, 0, { { { "t", 1 } }, { { "t", 1 } } });
		a.edges[1][0].accepting = true;
		CHECK(minimize_strategy(a).num_states == 2);
	}

	TEST_CASE("[MIN-07] a guard the reader does not know keeps the automaton") {
		auto a = automaton(1, 0, { { { "@alias", 1 } }, { { "t", 1 } } });
		CHECK(minimize_strategy(a).num_states == 2);
		auto b = automaton(1, 0, { { { "3", 1 } }, { { "t", 1 } } });
		CHECK(minimize_strategy(b).num_states == 2);
	}

	TEST_CASE("[MIN-08] the HOA text of a quotient parses back to it") {
		const std::string hoa =
			"HOA: v1\n"
			"States: 3\n"
			"Start: 0\n"
			"AP: 2 \"p0\" \"p1\"\n"
			"acc-name: all\n"
			"Acceptance: 0 t\n"
			"controllable-AP: 1\n"
			"--BODY--\n"
			"State: 0\n"
			"[0&1] 1\n"
			"[!0&!1] 2\n"
			"State: 1\n"
			"[1] 0\n"
			"State: 2\n"
			"[1] 0\n"
			"--END--\n";
		auto a = parse_hoa(hoa);
		REQUIRE(a.has_value());
		auto q = minimize_strategy(a.value());
		CHECK(q.num_states == 2);
		auto text = hoa_text_of(q, hoa);
		REQUIRE(text.has_value());
		CHECK(text->find("States: 2\n") != std::string::npos);
		CHECK(text->find("controllable-AP: 1\n") != std::string::npos);
		auto back = parse_hoa(*text);
		REQUIRE(back.has_value());
		CHECK(back.value().num_states == 2);
		same_runs(a.value(), back.value());
	}

	TEST_CASE("[MIN-09] above 16 propositions guards are compared by text"
		) {
		auto a = automaton(17, 0, {
			{ { "16", 1 }, { "!16", 2 } },
			{ { "0", 0 } },
			{ { "0", 0 } } });
		CHECK(minimize_strategy(a).num_states == 2);
		// equal truth tables, different texts: not merged past the cap
		auto b = automaton(17, 0, {
			{ { "16", 1 }, { "!16", 2 } },
			{ { "0", 0 } },
			{ { "0&1 | 0&!1", 0 } } });
		CHECK(minimize_strategy(b).num_states == 3);
	}

	TEST_CASE("[MIN-10] an ltlsynt strategy keeps its runs"
		* doctest::skip(!ltlsynt_available()))
	{
		synthesis_options opts;
		opts.simplify = "no";
		auto v = synthesize("G(i <-> X X X o)", { "i" }, { "o" }, 60, opts);
		REQUIRE(v.has_value());
		REQUIRE(v.value().realizable);
		auto a = parse_hoa(v.value().hoa);
		REQUIRE(a.has_value());
		auto q = minimize_strategy(a.value());
		CHECK(q.num_states <= a.value().num_states);
		same_runs(a.value(), q);
	}

	TEST_CASE("[MIN-11] the reader takes parentheses and propositions past 6") {
		// valuation bit k is proposition k: 64 sets proposition 6 alone
		CHECK(holds("(6 | 7) & !0", 8, 64));
		CHECK(holds("(6 | 7) & !0", 8, 128));
		CHECK_FALSE(holds("(6 | 7) & !0", 8, 65));
		CHECK_FALSE(holds("(6 | 7) & !0", 8, 0));
		CHECK(holds("!(6 & 7)", 8, 64));
		CHECK_FALSE(holds("!(6 & 7)", 8, 192));
		using strategy_minimize_detail::guard_reader;
		CHECK_FALSE(guard_reader("(0 & 1", 2).read().has_value());
		CHECK_FALSE(guard_reader("(0) 1", 2).read().has_value());
	}

	TEST_CASE("[MIN-12] an automaton without a valid start is kept") {
		hoa_automaton empty;
		CHECK(minimize_strategy(empty).num_states == 0);
		auto a = automaton(1, 0, { { { "t", 1 } }, { { "t", 1 } } });
		a.initial_state = 5;
		auto q = minimize_strategy(a);
		CHECK(q.num_states == 2);
		CHECK(q.initial_state == 5);
	}

	TEST_CASE("[MIN-13] ltl-minimize-strategy rewrites the strategy text") {
		std::string hoa =
			"HOA: v1\n"
			"States: 3\n"
			"Start: 0\n"
			"AP: 2 \"p0\" \"p1\"\n"
			"acc-name: all\n"
			"Acceptance: 0 t\n"
			"controllable-AP: 1\n"
			"--BODY--\n"
			"State: 0\n"
			"[0&1] 1\n"
			"[!0&!1] 2\n"
			"State: 1\n"
			"[1] 0\n"
			"State: 2\n"
			"[1] 0\n"
			"--END--\n";
		const bool saved = ltl_minimize_strategy_param;
		ltl_minimize_strategy_param = false;
		std::string kept = hoa;
		auto unchanged = minimized_strategy(kept);
		ltl_minimize_strategy_param = true;
		auto states = minimized_strategy(hoa);
		std::string garbage = "not a strategy";
		auto refused = minimized_strategy(garbage);
		ltl_minimize_strategy_param = saved;
		REQUIRE(unchanged.has_value());
		CHECK(*unchanged == 3);
		CHECK(kept.find("States: 3\n") != std::string::npos);
		REQUIRE(states.has_value());
		CHECK(*states == 2);
		CHECK(hoa.find("States: 2\n") != std::string::npos);
		auto back = parse_hoa(hoa);
		REQUIRE(back.has_value());
		CHECK(back.value().num_states == 2);
		CHECK_FALSE(refused.has_value());
	}

	TEST_CASE("[MIN-14] a strategy above ltl-strategy-max-states is asked again"
		* doctest::skip(!ltlsynt_available()))
	{
		// three states for `G o`, which ltlsynt answers with one
		std::string hoa =
			"HOA: v1\n"
			"States: 3\n"
			"Start: 0\n"
			"AP: 1 \"o\"\n"
			"acc-name: all\n"
			"Acceptance: 0 t\n"
			"controllable-AP: 0\n"
			"--BODY--\n"
			"State: 0\n"
			"[0] 1\n"
			"State: 1\n"
			"[0] 2\n"
			"State: 2\n"
			"[0] 0\n"
			"--END--\n";
		const size_t saved = ltl_strategy_max_states_param;
		ltl_strategy_max_states_param = 1;
		auto r = bound_strategy(hoa, "G(o)", {}, { "o" }, 60);
		ltl_strategy_max_states_param = saved;
		CHECK(r.has_value());
		auto a = parse_hoa(hoa);
		REQUIRE(a.has_value());
		CHECK(a.value().num_states == 1);
	}
}
