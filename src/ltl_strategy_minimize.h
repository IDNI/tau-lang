// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ltl_strategy_minimize.h
 * @brief The minimisation of a strategy automaton by bisimulation.
 *
 * Two states are merged when, for every valuation of the propositions, the
 * classes they can move to are the same. The quotient plays exactly the
 * labelled runs of the automaton, so it is a strategy for every formula
 * the automaton is a strategy for.
 *
 * Include ltl_aba_result.h before this file.
 */

#ifndef __IDNI__TAU__LTL_STRATEGY_MINIMIZE_H__
#define __IDNI__TAU__LTL_STRATEGY_MINIMIZE_H__

#include "hoa_label.h"

#include <cstdint>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace idni::tau_lang {

namespace strategy_minimize_detail {

/// Up to this many propositions a guard is compared by its truth table,
/// above it by its text.
inline constexpr size_t truth_table_max_aps = 16;

/// The truth table of a guard over n propositions: bit v holds the value at
/// the valuation whose bit i is proposition i.
using table = std::vector<uint64_t>;

inline size_t table_words(size_t n) {
	return n >= 6 ? (size_t{ 1 } << (n - 6)) : 1;
}

inline uint64_t last_word_mask(size_t n) {
	return n >= 6 ? ~uint64_t{ 0 } : ((uint64_t{ 1 } << (1u << n)) - 1);
}

inline table constant_table(size_t n, bool v) {
	table t(table_words(n), v ? ~uint64_t{ 0 } : 0);
	t.back() &= last_word_mask(n);
	return t;
}

inline table variable_table(size_t n, size_t k) {
	// within a word, bit b of the valuation is the pattern of k < 6;
	// above it, whole words are set or clear
	static constexpr uint64_t low[6] = { 0xAAAAAAAAAAAAAAAAull,
		0xCCCCCCCCCCCCCCCCull, 0xF0F0F0F0F0F0F0F0ull,
		0xFF00FF00FF00FF00ull, 0xFFFF0000FFFF0000ull,
		0xFFFFFFFF00000000ull };
	table t(table_words(n), 0);
	for (size_t w = 0; w < t.size(); ++w)
		t[w] = k < 6 ? low[k]
			: (((w >> (k - 6)) & 1) ? ~uint64_t{ 0 } : 0);
	t.back() &= last_word_mask(n);
	return t;
}

/// The truth table of a HOA label over @p n propositions; nullopt when the
/// text is not a label or names a proposition the automaton does not have.
inline std::optional<table> guard_table(const std::string& text, size_t n) {
	auto e = hoa_label::read(text);
	if (!e || hoa_label::ap_bound(*e) > n) return std::nullopt;
	using hoa_label::kind;
	return hoa_label::fold<table>(*e,
		[&](const hoa_label::node& nd, std::vector<table>& v) -> table
	{
		switch (nd.k) {
		case kind::tt: return constant_table(n, true);
		case kind::ff: return constant_table(n, false);
		case kind::ap: return variable_table(n, nd.ap);
		case kind::neg: {
			table t = std::move(v[nd.lhs]);
			for (auto& w : t) w = ~w;
			t.back() &= last_word_mask(n);
			return t;
		}
		case kind::conj: case kind::disj: {
			table t = std::move(v[nd.lhs]);
			const table& u = v[nd.rhs];
			for (size_t w = 0; w < t.size(); ++w)
				t[w] = nd.k == kind::conj ? t[w] & u[w] : t[w] | u[w];
			return t;
		}
		}
		return constant_table(n, false);
	});
}

} // namespace strategy_minimize_detail

/**
 * @brief The quotient of a strategy automaton by its coarsest bisimulation.
 *
 * Only the states reachable from the initial one are kept, and the initial
 * state becomes state 0. With at most 16 propositions two guards are equal
 * when their truth tables are; above that, when their texts are, which can
 * only keep more states. An automaton with acceptance marks, or a guard the
 * reader does not know, is returned unchanged.
 * @param aut the strategy automaton
 * @return the minimised automaton
 */
inline hoa_automaton minimize_strategy(const hoa_automaton& aut) {
	using namespace strategy_minimize_detail;
	if (aut.num_states == 0 || aut.initial_state >= aut.num_states)
		return aut;
	for (size_t s = 0; s < aut.num_states; ++s) {
		if (aut.state_accepting[s]) return aut;
		for (const auto& e : aut.edges[s]) if (e.accepting) return aut;
	}

	// the reachable states, in breadth-first order from the initial one
	std::vector<size_t> order{ aut.initial_state };
	std::vector<bool> seen(aut.num_states, false);
	seen[aut.initial_state] = true;
	for (size_t k = 0; k < order.size(); ++k)
		for (const auto& e : aut.edges[order[k]])
			if (!seen[e.dst]) seen[e.dst] = true, order.push_back(e.dst);

	// each guard as a key: its truth table, or its text past the cap
	const bool by_table = aut.aps.size() <= truth_table_max_aps;
	std::vector<std::vector<table>> tables(aut.num_states);
	if (by_table) for (size_t s : order) {
		for (const auto& e : aut.edges[s]) {
			auto t = guard_table(e.guard_label, aut.aps.size());
			if (!t) return aut;
			tables[s].push_back(std::move(*t));
		}
	}

	// Moore refinement from a single class: a state's signature is, for each
	// class it moves to, the union of its guards into that class
	std::vector<size_t> cls(aut.num_states, 0);
	size_t classes = 1;
	for (;;) {
		using signature = std::map<size_t, std::pair<table,
			std::set<std::string>>>;
		std::map<std::pair<size_t, signature>, size_t> ids;
		std::vector<size_t> next(aut.num_states, 0);
		for (size_t s : order) {
			signature sig;
			for (size_t k = 0; k < aut.edges[s].size(); ++k) {
				const auto& e = aut.edges[s][k];
				auto& [t, texts] = sig[cls[e.dst]];
				if (!by_table) { texts.insert(e.guard_label); continue; }
				if (t.empty()) t = tables[s][k];
				else for (size_t w = 0; w < t.size(); ++w)
					t[w] |= tables[s][k][w];
			}
			auto key = std::make_pair(cls[s], std::move(sig));
			auto it = ids.try_emplace(std::move(key), ids.size()).first;
			next[s] = it->second;
		}
		const size_t count = ids.size();
		cls = std::move(next);
		if (count == classes) break;
		classes = count;
	}

	// number the classes by their first state in breadth-first order
	std::vector<size_t> id(classes, SIZE_MAX), rep;
	for (size_t s : order) if (id[cls[s]] == SIZE_MAX)
		id[cls[s]] = rep.size(), rep.push_back(s);

	hoa_automaton q;
	q.num_states = rep.size();
	q.initial_state = 0;
	q.aps = aut.aps;
	q.edges.assign(q.num_states, {});
	q.state_accepting.assign(q.num_states, false);
	for (size_t c = 0; c < rep.size(); ++c) {
		std::set<std::pair<std::string, size_t>> kept;
		for (const auto& e : aut.edges[rep[c]]) {
			size_t dst = id[cls[e.dst]];
			if (kept.emplace(e.guard_label, dst).second)
				q.edges[c].push_back(hoa_edge{ e.guard_label, dst, false });
		}
	}
	return q;
}

/**
 * @brief The HOA text of @p aut, with the header lines of @p original.
 *
 * The `States:` and `Start:` lines are rewritten; every other header line
 * (the propositions, the controllable ones, the acceptance) is kept.
 * @param aut the automaton, over the propositions of @p original
 * @param original the HOA text @p aut was read from
 * @return the HOA text, or nullopt when @p original has no `--BODY--`
 */
inline std::optional<std::string> hoa_text_of(const hoa_automaton& aut,
	const std::string& original)
{
	const auto body = original.find("--BODY--");
	if (body == std::string::npos) return std::nullopt;
	std::string out;
	for (size_t at = 0; at < body;) {
		size_t eol = original.find('\n', at);
		if (eol == std::string::npos || eol > body) eol = body;
		std::string line = original.substr(at, eol - at);
		at = eol + 1;
		if (line.rfind("States:", 0) == 0)
			line = "States: " + std::to_string(aut.num_states);
		else if (line.rfind("Start:", 0) == 0)
			line = "Start: " + std::to_string(aut.initial_state);
		if (!line.empty()) out += line + "\n";
	}
	out += "--BODY--\n";
	for (size_t s = 0; s < aut.num_states; ++s) {
		out += "State: " + std::to_string(s) + "\n";
		for (const auto& e : aut.edges[s])
			out += "[" + e.guard_label + "] " + std::to_string(e.dst)
				+ "\n";
	}
	out += "--END--\n";
	return out;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__LTL_STRATEGY_MINIMIZE_H__
