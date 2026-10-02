// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Algorithm D: parity game product solver for LTL(ABA).
//
// Paper sketch (Conclusion): replace each data subformula δ_i with propositional
// variable D_i, build the synthesis parity game for φ*(D_i), then solve the
// data-enriched product game (game × T_1 memory types) using Zielonka's
// algorithm with per-step T_3 feasibility checks.
//
// This avoids putting T_2/T_3 structural constraints into the LTL formula:
// instead, Φ_δ / Ψ_I / Φ_I are enforced lazily via feasibility pruning of
// the product-game transitions.
//
// Scope: currently handles output-only qlt formulas (all D_i are system
// outputs, no uncontrollable input atoms).  Input atoms need extension.

#ifndef __IDNI__TAU__ALGORITHM_D_GAME_H__
#define __IDNI__TAU__ALGORITHM_D_GAME_H__

#include "omcat_types.h"
#include "tau_diagnostics.h"
#include "ltl_aba_limits.h"
#include <cerrno>
#include <climits>
#include <cstdint>
#include <cstdlib>

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdio>
#if !defined(__EMSCRIPTEN__) && !defined(_WIN32)
#include <sys/wait.h>
#endif
#include <functional>
#include <map>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace idni::tau_lang::alg_d {

// ── Synthesis game (from ltlsynt --print-game-hoa) ───────────────────────

/// @brief Synthesis parity game parsed from `ltlsynt --print-game-hoa`.
struct synth_game {
	size_t num_states = 0;
	size_t init = 0;
	// player[q]: 0 = env (uncontrollable), 1 = sys (controller)
	std::vector<int> player;
	// state_color[q]: color for state-based acceptance (-1 if none)
	std::vector<int> state_color;
	// trans[q]: list of (guard_string, next_state, edge_color)
	// edge_color = -1 if no acceptance mark on this transition
	std::vector<std::vector<std::tuple<std::string,size_t,int>>> trans;
	std::vector<std::string> aps;
	std::vector<bool> controllable;
	// Acceptance info
	int  n_colors  = 0;    // number of acceptance sets
	bool trans_acc = false; // true = transition-based acceptance
	// Parity priority for each state (computed from color + acceptance type)
	// Even priority = good for env (player 0); odd = good for sys (player 1).
	// For trivial (all): priority 1 everywhere.
	// For Büchi  Inf(0): color 0 → priority 1 elsewhere priority 0.
	// For general parity: priority = color.
	std::vector<int> state_priority;
	// Edge priorities: edge_priority[q][j] = priority of j-th trans from q
	std::vector<std::vector<int>> edge_priority;
	// The acceptance is one of all, Buchi, co-Buchi or parity, and whether
	// a run that sees no colour infinitely often is accepted.
	bool acc_known = false;
	bool acc_accepts_uncolored = false;
	// Some state or edge carries more than one colour; the priorities
	// above read the first one only.
	bool multi_colored = false;
};

/// @brief Whether an acceptance condition (`Inf(n)`, `Fin(n)`, `t`, `f`,
/// `!`, `&`, `|`, parentheses) holds for a run that sees no colour
/// infinitely often; nullopt when the text does not parse.
inline std::optional<bool> acceptance_without_colors(const std::string& cond) {
	size_t i = 0;
	bool failed = false;
	auto ws = [&] { while (i < cond.size() && std::isspace((unsigned char)cond[i])) ++i; };
	std::function<bool()> disj, conj, prim;
	prim = [&]() -> bool {
		ws();
		if (i >= cond.size()) { failed = true; return false; }
		if (cond[i] == '(') {
			++i;
			bool v = disj();
			ws();
			if (i < cond.size() && cond[i] == ')') ++i; else failed = true;
			return v;
		}
		if (cond[i] == '!') { ++i; return !prim(); }
		if (cond[i] == 't') { ++i; return true; }
		if (cond[i] == 'f') { ++i; return false; }
		for (const char* kw : { "Inf", "Fin" })
			if (cond.compare(i, 3, kw) == 0) {
				i += 3;
				size_t close = cond.find(')', i);
				if (close == std::string::npos) { failed = true; return false; }
				i = close + 1;
				return kw[0] == 'F';
			}
		failed = true;
		return false;
	};
	conj = [&]() -> bool {
		bool v = prim();
		for (ws(); !failed && i < cond.size() && cond[i] == '&'; ws())
			{ ++i; v = prim() && v; }
		return v;
	};
	disj = [&]() -> bool {
		bool v = conj();
		for (ws(); !failed && i < cond.size() && cond[i] == '|'; ws())
			{ ++i; v = conj() || v; }
		return v;
	};
	bool v = disj();
	ws();
	if (failed || i != cond.size()) return std::nullopt;
	return v;
}

// ── HOA boolean formula evaluator ────────────────────────────────────────

// Parse and evaluate a HOA guard formula string against an AP bitmask.
// Grammar: t | f | N | !E | E&E | E|E | (E)
// Returns true iff the formula is satisfied by the assignment.

namespace hoa_guard {

/// @brief Advance @p i past spaces and tabs in @p s.
static inline void skip_ws(const std::string& s, size_t& i) {
	while (i < s.size() && (s[i] == ' ' || s[i] == '\t')) ++i;
}

/// @brief Evaluate a disjunction (E|E) of the guard grammar from position
/// @p i.
static bool eval(const std::string& s, size_t& i, int bitmask, int n_aps);

/// @brief Evaluate one atom (t, f, N, !E or (E)) of the guard grammar.
static bool eval_atom(const std::string& s, size_t& i, int bitmask, int n_aps) {
	skip_ws(s, i);
	if (i >= s.size()) return false;
	if (s[i] == 't') { ++i; return true; }
	if (s[i] == 'f') { ++i; return false; }
	if (s[i] == '(') {
		++i;
		bool v = eval(s, i, bitmask, n_aps);
		skip_ws(s, i);
		if (i < s.size() && s[i] == ')') ++i;
		return v;
	}
	if (s[i] == '!') {
		++i;
		return !eval_atom(s, i, bitmask, n_aps);
	}
	if (std::isdigit((unsigned char)s[i])) {
		size_t j = i;
		while (j < s.size() && std::isdigit((unsigned char)s[j])) ++j;
		// Bounded digit run: an AP index beyond int range is a garbled
		// guard, read as an out-of-range AP (false), never an exception.
		int ap = -1;
		if (j - i <= 9) ap = std::stoi(s.substr(i, j - i));
		i = j;
		if (ap < 0 || ap >= n_aps) return false;
		return (bitmask >> ap) & 1;
	}
	return false;
}

/// @brief Evaluate a conjunction (E&E) of the guard grammar.
static bool eval_and(const std::string& s, size_t& i, int bitmask, int n_aps) {
	bool v = eval_atom(s, i, bitmask, n_aps);
	while (true) {
		skip_ws(s, i);
		if (i >= s.size() || s[i] != '&') break;
		++i;
		v &= eval_atom(s, i, bitmask, n_aps);
	}
	return v;
}

static bool eval(const std::string& s, size_t& i, int bitmask, int n_aps) {
	bool v = eval_and(s, i, bitmask, n_aps);
	while (true) {
		skip_ws(s, i);
		if (i >= s.size() || s[i] != '|') break;
		++i;
		v |= eval_and(s, i, bitmask, n_aps);
	}
	return v;
}


// ── HOA guard → DNF (sum of products) ────────────────────────────────────
//
// The evaluator above answers "does this label hold under this assignment".
// Consumers that must EMIT code for a label (cpp_codegen.tmpl.h) or reason about it
// symbolically (the ABA oracle) need its cubes instead, and both must call
// `to_dnf` rather than hand-lex the label.  The digit loop
// `for (char c : idx_str) if (isdigit(c)) idx = idx*10+(c-'0')` SKIPS '|', '('
// and ')' instead of rejecting them, so it reads `0|1` as the single literal
// `1` (wrong AP, other disjunct lost) and drops `(0|1)` entirely, which
// silently widens the guard to `true`.  Spot prints strategy edge labels as
// sums of products, so those shapes are the normal case, not a corner (LG-4).
//
// `to_dnf` returns nullopt when the label does not parse or the expansion
// exceeds `max_cubes`.  Callers must REFUSE the edge in that case: falling
// back to a partial reading silently widens the guard to `true`.

/// @brief One literal of a guard cube: AP index and polarity.
struct lit { size_t ap; bool pos; };
/// @brief A cube is a conjunction of literals; the empty cube is true. A DNF
/// is a vector of cubes; an empty vector == false.
using cube = std::vector<lit>;   // conjunction; empty cube == true

namespace dnf_detail {

/// @brief Normalise a cube: sort by AP, drop duplicates, reject if an AP
/// appears with both polarities (the cube is then unsatisfiable).
inline bool normalise_cube(cube& c) {
	std::sort(c.begin(), c.end(), [](const lit& a, const lit& b) {
		return a.ap != b.ap ? a.ap < b.ap : (int)a.pos < (int)b.pos;
	});
	cube out;
	for (const auto& l : c) {
		if (!out.empty() && out.back().ap == l.ap) {
			if (out.back().pos != l.pos) return false;  // p & !p
			continue;                                   // duplicate
		}
		out.push_back(l);
	}
	c.swap(out);
	return true;
}

/// @brief Recursive-descent parser turning a guard label into DNF cubes,
/// failing when the expansion exceeds `max_cubes` (0 = unlimited).
struct parser {
	const std::string& s;
	size_t i = 0;
	size_t max_cubes;
	bool failed = false;

	explicit parser(const std::string& str, size_t cap)
		: s(str), max_cubes(cap ? cap : SIZE_MAX) {}

	static std::vector<cube> dnf_true()  { return { cube{} }; }
	static std::vector<cube> dnf_false() { return {}; }

	std::vector<cube> conj(const std::vector<cube>& a, const std::vector<cube>& b) {
		std::vector<cube> r;
		for (const auto& ca : a)
			for (const auto& cb : b) {
				if (r.size() >= max_cubes) { failed = true; return {}; }
				cube m = ca;
				m.insert(m.end(), cb.begin(), cb.end());
				if (normalise_cube(m)) r.push_back(std::move(m));
			}
		return r;
	}

	std::vector<cube> disj(std::vector<cube> a, const std::vector<cube>& b) {
		for (const auto& cb : b) {
			if (a.size() >= max_cubes) { failed = true; return {}; }
			a.push_back(cb);
		}
		return a;
	}

	// ¬(c1 ∨ … ∨ cn) = ∧_i (∨_l ¬l)
	std::vector<cube> negate(const std::vector<cube>& a) {
		std::vector<cube> r = dnf_true();
		for (const auto& c : a) {
			if (c.empty()) return dnf_false();   // ¬true
			std::vector<cube> d;
			for (const auto& l : c) d.push_back(cube{ lit{ l.ap, !l.pos } });
			r = conj(r, d);
			if (failed) return {};
		}
		return r;
	}

	std::vector<cube> parse_atom() {
		skip_ws(s, i);
		if (i >= s.size()) { failed = true; return {}; }
		if (s[i] == 't') { ++i; return dnf_true(); }
		if (s[i] == 'f') { ++i; return dnf_false(); }
		if (s[i] == '(') {
			++i;
			auto v = parse_or();
			skip_ws(s, i);
			if (i < s.size() && s[i] == ')') ++i;
			else failed = true;
			return v;
		}
		if (s[i] == '!') { ++i; return negate(parse_atom()); }
		if (std::isdigit((unsigned char)s[i])) {
			size_t j = i;
			while (j < s.size() && std::isdigit((unsigned char)s[j])) ++j;
			// Bounded digit run (see eval_atom): a garbled index fails
			// the parse instead of throwing out of it.
			if (j - i > 9) { failed = true; return {}; }
			size_t ap = 0;
			for (size_t k = i; k < j; ++k)
				ap = ap * 10 + static_cast<size_t>(s[k] - '0');
			i = j;
			return { cube{ lit{ ap, true } } };
		}
		failed = true;
		return {};
	}

	std::vector<cube> parse_and() {
		auto v = parse_atom();
		while (!failed) {
			skip_ws(s, i);
			if (i >= s.size() || s[i] != '&') break;
			++i;
			v = conj(v, parse_atom());
		}
		return v;
	}

	std::vector<cube> parse_or() {
		auto v = parse_and();
		while (!failed) {
			skip_ws(s, i);
			if (i >= s.size() || s[i] != '|') break;
			++i;
			v = disj(std::move(v), parse_and());
		}
		return v;
	}
};

} // namespace dnf_detail

/**
 * @brief Expand a HOA guard label into its DNF cubes.
 *
 * Returns nullopt when the label does not parse or the expansion exceeds
 * `max_cubes` (see the section comment above: callers must REFUSE the edge
 * in that case).  An empty label is the unconditional guard.
 * @param label Guard label over AP indices.
 * @param max_cubes Cap on the number of cubes produced (0 = unlimited).
 * @return The cubes, or `std::nullopt`.
 */
inline std::optional<std::vector<cube>> to_dnf(
	const std::string& label, size_t max_cubes = ltl_guard_max_cubes())
{
	// An empty label is the unconditional guard, same convention the
	// evaluator and the ABA guard parser use.
	std::string s = label;
	dnf_detail::parser p(s, max_cubes);
	if (s.empty()) return dnf_detail::parser::dnf_true();
	auto r = p.parse_or();
	skip_ws(s, p.i);
	if (p.failed || p.i != s.size()) return std::nullopt;
	return r;
}

} // namespace hoa_guard

/// @brief Evaluate a HOA guard label under an AP assignment. Bit `i` of
/// @p bitmask is the truth value of AP index `i` (the convention used by
/// build_product_game's 2^n_aps assignment loops and by the tests).
inline bool eval_guard(const std::string& guard, int bitmask, int n_aps) {
	size_t i = 0;
	return hoa_guard::eval(guard, i, bitmask, n_aps);
}

// ── HOA synthesis game parser ─────────────────────────────────────────────

/**
 * @brief Parse the HOA text of `ltlsynt --print-game-hoa` into a
 * `synth_game`.
 *
 * Reads the header (states, start, APs, controllable APs, state players,
 * acceptance) and the body transitions, then derives state and edge
 * priorities in the solver's max-odd convention as described in the body
 * comments.
 * @param hoa_text HOA text to parse.
 * @return The parsed game, or an error result for a text that yields no
 * game: a decomposed multi-game text, a malformed header integer, more
 * atomic propositions than the product game can enumerate, or a text
 * without a state count.
 */
inline result<synth_game> parse_synth_game_hoa(const std::string& hoa_text) {
	result<synth_game> r;
	synth_game g;

	// A decomposed specification prints one game per part. Parsing only the
	// first would drop the other parts' constraints, and their output APs
	// with them, which reads downstream as those APs being false.
	size_t n_games = 0;
	for (size_t p = hoa_text.find("HOA:"); p != std::string::npos;
		p = hoa_text.find("HOA:", p + 4))
		if (p == 0 || hoa_text[p - 1] == '\n') ++n_games;
	if (n_games > 1) {
		return r.with_error(code::unsupported_operation,
			"the synthesis game HOA holds several games (a decomposed "
			"specification); only a single game can be parsed -- run "
			"ltlsynt with --decompose=no",
			{{label::value, std::to_string(n_games)}});
	}

	// Header integers come from an external process: parse them with a
	// range check instead of std::stoi, which throws on garbage and
	// accepts counts the vectors below could never allocate.
	auto header_int = [](const std::string& s, long max_value) -> long {
		char* end = nullptr;
		errno = 0;
		long v = std::strtol(s.c_str(), &end, 10);
		if (end == s.c_str() || errno == ERANGE || v < 0 || v > max_value)
			return -1;
		while (*end == ' ' || *end == '\t') ++end;
		return *end == '\0' ? v : -1;
	};
	const size_t state_cap = ltl_hoa_max_states();
	const long max_states = state_cap
		? (long) std::min<size_t>(state_cap, (size_t) LONG_MAX)
		: LONG_MAX;

	// Spot wraps a long header item (the state players of a large game)
	// onto indented continuation lines; they are joined to their item.
	std::string joined;
	{
		std::istringstream raw(hoa_text);
		std::string l;
		bool body = false;
		while (std::getline(raw, l)) {
			if (l == "--BODY--") body = true;
			if (!body && !l.empty() && (l[0] == ' ' || l[0] == '\t')
				&& !joined.empty())
			{
				joined.back() = ' ';
				joined += l + "\n";
				continue;
			}
			joined += l + "\n";
		}
	}
	std::istringstream ss(joined);
	std::string line;
	bool in_body = false;
	// The state whose body block is being read; empty before the first
	// `State:` line and after a malformed one.
	std::optional<size_t> cur_state;
	bool is_buchi = false, is_cobuchi = false, is_all = false;
	bool is_parity = false, parity_min = false, parity_even = false;
	std::string acc_cond;   // the condition after the colour count

	auto parse_int_list = [](const std::string& s) -> result<std::vector<int>> {
		result<std::vector<int>> pr;
		std::vector<int> out;
		std::istringstream is(s);
		std::string tok;
		while (is >> tok) {
			char* end = nullptr;
			errno = 0;
			long v = std::strtol(tok.c_str(), &end, 10);
			// One token is one list entry; a trailing suffix or an
			// overflow is not an integer.
			if (end == tok.c_str() || *end != '\0' || errno == ERANGE
				|| v < INT_MIN || v > INT_MAX)
				return pr.with_error(code::parse_error,
					"the synthesis game HOA has a non-integer "
					"or out-of-range value in a header list",
					{{label::value, truncate_for_message(tok)}});
			out.push_back(static_cast<int>(v));
		}
		return pr.with_value(std::move(out));
	};

	while (std::getline(ss, line)) {
		if (!line.empty() && line.back() == '\r') line.pop_back();
		if (line.empty()) continue;
		if (line == "--BODY--") { in_body = true; continue; }
		if (line == "--END--")  { break; }

		if (!in_body) {
			if (line.substr(0,7) == "States:") {
				const long n = header_int(line.substr(7), max_states);
				if (n < 1) {
					return r.with_error(code::parse_error,
						"malformed synthesis game HOA: bad state "
						"count",
						{{label::value, truncate_for_message(
							line.substr(7))}});
				}
				// The range check above makes the count fit the id type.
				g.num_states = static_cast<size_t>(n);
				g.player.assign(g.num_states, 0);
				g.state_color.assign(g.num_states, -1);
				g.state_priority.assign(g.num_states, 0);
				g.trans.resize(g.num_states);
				g.edge_priority.resize(g.num_states);
			} else if (line.substr(0,6) == "Start:") {
				const long n = header_int(line.substr(6), INT_MAX);
				if (n < 0) {
					return r.with_error(code::parse_error,
						"malformed synthesis game HOA: bad start "
						"state",
						{{label::value, truncate_for_message(
							line.substr(6))}});
				}
				g.init = static_cast<size_t>(n);
			} else if (line.substr(0,3) == "AP:") {
				std::istringstream apl(line.substr(3));
				int n = -1; apl >> n;
				// The product game enumerates 1 << n_aps assignments:
				// a signed shift is undefined at 31 and the loop is
				// hopeless well before (ltl_max_game_aps).
				if (n < 0 || n > ltl_max_game_aps) {
					return r.with_error(
						code::unsupported_operation,
						"the synthesis game has more atomic "
						"propositions than the product game can "
						"enumerate; refusing",
						{{label::limit, ltl_max_game_aps},
						 {label::value, std::to_string(n)}});
				}
				// n passed the range check above; it is the AP count.
				const size_t n_aps = static_cast<size_t>(n);
				g.aps.resize(n_aps);
				g.controllable.resize(n_aps, false);
				for (size_t i = 0; i < n_aps; ++i) {
					std::string ap; apl >> ap;
					if (!ap.empty() && ap.front() == '"') ap = ap.substr(1, ap.size()-2);
					g.aps[i] = ap;
				}
			} else if (line.substr(0,16) == "controllable-AP:") {
				TAU_TRY(auto idxs, parse_int_list(line.substr(16)));
				for (int i : idxs) {
					if (i < 0
						|| static_cast<size_t>(i) >= g.controllable.size())
						return r.with_error(code::parse_error,
							"the synthesis game HOA has a "
							"controllable AP index outside the "
							"AP table",
							{{label::value, truncate_for_message(
								line)}});
					g.controllable[static_cast<size_t>(i)] = true;
				}
			} else if (line.substr(0,17) == "spot.state-player"
				|| line.substr(0,17) == "spot-state-player") {
				// "spot.state-player: 0 1 0 1 ..."
				// Spot (--print-game-hoa) spells the header name with a
				// dot; the dashed form is accepted too for hand-written
				// and legacy fixtures.  Getting this wrong silently
				// leaves every state env-owned.
				size_t colon = line.find(':');
				if (colon != std::string::npos) {
					TAU_TRY(auto players,
						parse_int_list(line.substr(colon+1)));
					// A player list shorter than the state table is
					// ordinary; fill only the states it covers.
					for (size_t q = 0; q < players.size() && q < g.player.size(); ++q)
						g.player[q] = players[q];
				}
			} else if (size_t at = line.find("acc-name:");
				at != std::string::npos)
			{
				// by the exact name: generalized-Buchi and
				// generalized-co-Buchi contain the plain names, and
				// any other condition (Streett, Rabin, ...) is left
				// unknown, so the game is refused
				std::istringstream nl(line.substr(at + 9));
				std::string name;
				nl >> name;
				if (name == "all") is_all = true;
				else if (name == "Buchi") is_buchi = true;
				else if (name == "co-Buchi") is_cobuchi = true;
				else if (name == "parity") {
					// LG-3: capture the flavor; normalized to
					// max-odd (the solver's convention) below.
					is_parity   = true;
					parity_min  = line.find(" min")  != std::string::npos;
					parity_even = line.find(" even") != std::string::npos;
				}
			} else if (line.substr(0,11) == "Acceptance:") {
				std::istringstream al(line.substr(11));
				al >> g.n_colors;
				std::getline(al, acc_cond);
			} else if (line.find("trans-acc") != std::string::npos) {
				g.trans_acc = true;
			}
			continue;
		}

		// In body
		if (line.substr(0,6) == "State:") {
			// "State: N" or "State: N {k}" or "State: N {k1 k2}"
			std::istringstream sl(line.substr(6));
			long n = -1;
			sl >> n;
			const size_t state = static_cast<size_t>(n);
			if (n < 0 || state >= g.num_states)
				return r.with_error(code::parse_error,
					"the synthesis game HOA has a state number "
					"outside the state table",
					{{label::value, truncate_for_message(line)}});
			cur_state = state;
			// Check for color marks
			size_t lb = line.find('{');
			size_t rb = line.find('}');
			if (lb != std::string::npos && rb != std::string::npos) {
				std::istringstream cl(line.substr(lb+1, rb-lb-1));
				int c; cl >> c;
				g.state_color[*cur_state] = c;
				if (int more; cl >> more) g.multi_colored = true;
			}
			continue;
		}
		if (!cur_state || line.front() != '[') continue;

		// Parse transition: [guard] next_state {optional_color}
		size_t rb = line.find(']');
		if (rb == std::string::npos) continue;
		std::string guard = line.substr(1, rb - 1);
		std::istringstream tl(line.substr(rb+1));
		long next_raw = -1;
		tl >> next_raw;
		const size_t next = static_cast<size_t>(next_raw);
		if (next_raw < 0 || next >= g.num_states)
			return r.with_error(code::parse_error,
				"the synthesis game HOA has a transition to a "
				"state outside the state table",
				{{label::value, truncate_for_message(line)}});
		// Check for edge acceptance mark
		int edge_color = -1;
		size_t elb = line.find('{', rb);
		size_t erb = line.find('}', rb);
		if (elb != std::string::npos && erb != std::string::npos) {
			std::istringstream cl(line.substr(elb+1, erb-elb-1));
			cl >> edge_color;
			if (int more; cl >> more) g.multi_colored = true;
		}
		g.trans[*cur_state].emplace_back(guard, next, edge_color);
	}

	// Compute state_priority from acceptance type and colors.
	// Convention: odd priority = good for player 1 (sys = controller).
	//   is_all: priority 1 everywhere (player 1 always "wins" structurally)
	//   is_buchi   Inf(0): color 0 → priority 1, else priority 0
	//   is_cobuchi Fin(0): color 0 → priority 2, else priority 1
	//     Color 0 marks the rejecting states: a run visiting them
	//     infinitely often must lose for sys, so color 0 needs an EVEN
	//     priority that DOMINATES the uncolored (odd) one.  Mapping it to
	//     0 instead would let any run that also revisits an uncolored
	//     state have max recurring priority 1 (odd) and be scored sys-win.
	//   parity/Streett: color → priority directly
	// LG-3: normalize any parity flavor to the solver's max-odd
	// convention. min flavors reflect (c' = k-1-c, so the min color
	// becomes the max priority); a flavor whose winning parity lands on
	// even after that reflection shifts by one (parity of k-1 decides
	// whether reflection flips it).
	auto parity_prio = [&](int c) {
		if (!is_parity) return c;             // no acc-name: raw color
		const int k = g.n_colors;
		int p = parity_min ? (k - 1 - c) : c;
		const bool win_even_after = parity_min
			? (((k - 1) % 2 == 0) ? parity_even : !parity_even)
			: parity_even;
		return win_even_after ? p + 1 : p;
	};
	// AL-11: a header without `acc-name:` but with the trivially-all
	// condition `Acceptance: 0 t` would otherwise give every state
	// priority 0 (env-good) and the all-even game is won by env everywhere
	// — a blanket UNREALIZABLE for hand-fed HOA.  (Spot always emits
	// acc-name; this only matters for fixtures and foreign tools.)
	if (!is_all && !is_buchi && !is_cobuchi && !is_parity
	    && g.n_colors == 0) {
		auto first = acc_cond.find_first_not_of(" \t\r");
		if (first != std::string::npos && acc_cond[first] == 't')
			is_all = true;
	}
	if (is_all || is_buchi || is_cobuchi || is_parity) {
		auto empty = acceptance_without_colors(acc_cond);
		g.acc_known = empty.has_value();
		g.acc_accepts_uncolored = empty.value_or(false);
	}
	// The state count is non-negative (checked at parse), so it sizes the
	// priority tables and bounds the loops.
	const size_t n_states = g.num_states;
	for (size_t q = 0; q < n_states; ++q) {
		int c = g.state_color[q];
		if (is_all)
			g.state_priority[q] = 1;
		else if (is_buchi)
			g.state_priority[q] = (c == 0) ? 1 : 0;
		else if (is_cobuchi)
			g.state_priority[q] = (c == 0) ? 2 : 1;
		else
			g.state_priority[q] = (c >= 0) ? parity_prio(c) : 0;
	}
	// Compute edge priorities similarly
	g.edge_priority.resize(n_states);
	for (size_t q = 0; q < n_states; ++q) {
		g.edge_priority[q].resize(g.trans[q].size(), -1);
		for (size_t j = 0; j < g.trans[q].size(); ++j) {
			int ec = std::get<2>(g.trans[q][j]);
			if (ec < 0) {
				g.edge_priority[q][j] = -1;
			} else if (is_all) {
				g.edge_priority[q][j] = 1;
			} else if (is_buchi) {
				g.edge_priority[q][j] = (ec == 0) ? 1 : 0;
			} else if (is_cobuchi) {
				// see the state_priority comment: Fin(0)'s color 0
				// must map to a dominant EVEN priority
				g.edge_priority[q][j] = (ec == 0) ? 2 : 1;
			} else {
				g.edge_priority[q][j] = parity_prio(ec);
			}
		}
	}
	if (g.num_states == 0) {
		return r.with_error(code::parse_error,
			"the synthesis game HOA carries no state count; "
			"no game was parsed");
	}
	return r.with_value(std::move(g));
}

// ── Call ltlsynt and get parity game ──────────────────────────────────────

/**
 * @brief Run ltlsynt on `phi_prop` and parse `--print-game-hoa` into a
 * synth_game.
 *
 * DEFINED IN ltl_aba_synthesis.tmpl.h, not here (LS-10).  It needs the Spot
 * backend (backends/spot/spot.h), included after this one; the callers
 * below need only this declaration. Not spelled inline here: a unit that
 * includes this header without the definition (the qlt plugin's own) would
 * declare an inline function it never defines, which gcc rejects; the
 * definition is inline and is emitted by every unit that includes it.
 *
 * An error result means the subprocess produced no verdict; the caller
 * merges it into its own result rather than reading it as an empty,
 * definitively unrealizable game.
 * @param phi_prop Propositional LTL formula in Spot syntax.
 * @param ins Input proposition names.
 * @param outs Output proposition names.
 * @param algo ltlsynt's `--algo=` value; empty asks for a parity game
 * (`acd`, then `sd`), since the default construction may give a Streett one.
 * @return The parsed synthesis game, or an error result.
 */
result<synth_game> call_ltlsynt_game(
	const std::string& phi_prop,
	const std::vector<std::string>& ins,
	const std::vector<std::string>& outs,
	const std::string& algo = {});

// ── Product game (game × T_1) ─────────────────────────────────────────────
//
// Product state index: q * T1_size + rho.
// For each sys state (q, rho): sys picks D_pattern AND rho'.
//   Transition is valid iff T3_feasible(pos_m=rho, pos_y=rho', D_pattern).
//   New state: (q', rho') where q' = game_next(q, D_pattern).
// For each env state (q, rho): env has unconditional transitions (output-only).
//   New state: (q', rho) [rho unchanged].
//
// For trans-based acceptance: insert intermediate "color" state per edge.
// State i in intermediate layer: (q * T1_size + rho) + offset.

/// @brief Product of the synthesis game with the T_1 memory types (state
/// index q * T1_size + rho, plus edge stubs for transition acceptance).
struct product_game {
	size_t n_states = 0;
	size_t init = 0;
	std::vector<int> player;
	std::vector<int> priority;      // state-based (after intermediate conversion)
	std::vector<std::vector<size_t>> succs;
};

/// @brief Parse the disjunct index N from a `d_N` atomic-proposition name.
/// Returns -1 if @p ap is not a `d_` AP.
inline int d_index_from_ap_name(const std::string& ap) {
	if (ap.size() <= 2 || ap[0] != 'd' || ap[1] != '_') return -1;
	int idx = 0;
	for (size_t i = 2; i < ap.size(); ++i) {
		if (!std::isdigit(static_cast<unsigned char>(ap[i]))) return -1;
		idx = idx * 10 + (ap[i] - '0');
	}
	return idx;
}

/// @brief Project an AP assignment (bit `i` = truth of AP index `i`) onto the
/// controllable `d_N` APs: bit `N` of the result is set iff `d_N` is true
/// in @p assignment. @p K bounds the accepted disjunct indices.
inline size_t d_pattern_from_assignment(const synth_game& G, int assignment, int K) {
	size_t pat = 0;
	for (size_t ap = 0; ap < G.aps.size(); ++ap) {
		if (ap >= G.controllable.size() || !G.controllable[ap]) continue;
		if (((assignment >> ap) & 1) == 0) continue;
		int d = d_index_from_ap_name(G.aps[ap]);
		if (0 <= d && d < K) pat |= (size_t{1} << d);
	}
	return pat;
}

/**
 * @brief The system's choices as (D_pattern, AP assignment) pairs.
 *
 * An output AP the game does not mention is unconstrained by the formula,
 * so its D-bit still ranges over both values instead of silently reading as
 * false -- otherwise half the system's moves disappear and a realizable spec
 * can report UNREALIZABLE.
 * @param G Synthesis game.
 * @param K Number of D propositions.
 * @return Every (D_pattern, assignment) pair the system may pick.
 */
inline std::vector<std::pair<size_t,int>> sys_choices(const synth_game& G, int K) {
	// K is a non-negative proposition count, so it sizes the table.
	const size_t K_sz = static_cast<size_t>(K);
	std::vector<std::optional<size_t>> ap_of_d(K_sz);
	std::vector<size_t> other_aps;
	for (size_t ap = 0; ap < G.aps.size(); ++ap) {
		const bool ctrl = ap < G.controllable.size()
			&& G.controllable[ap];
		const int d = ctrl ? d_index_from_ap_name(G.aps[ap]) : -1;
		// d is a D-pattern bit position; the table takes its index form.
		if (0 <= d && d < K) ap_of_d[static_cast<size_t>(d)] = ap;
		else other_aps.push_back(ap);
	}
	const int n_other = (int)other_aps.size();
	std::vector<std::pair<size_t,int>> out;
	out.reserve((size_t{1} << K) << n_other);
	for (size_t D_pat = 0; D_pat < (size_t{1} << K); ++D_pat)
		for (int o = 0; o < (1 << n_other); ++o) {
			int a = 0;
			for (size_t i = 0; i < K_sz; ++i)
				if (((D_pat >> i) & 1) && ap_of_d[i])
					a |= 1 << *ap_of_d[i];
			for (size_t t = 0; t < other_aps.size(); ++t)
				if ((o >> t) & 1) a |= 1 << other_aps[t];
			out.emplace_back(D_pat, a);
		}
	return out;
}

// ── LG-12 / AL-N4: the t = 0 initial-memory convention ───────────────────
//
// At t = 0 the memory ρ ("1-type of the previous output") has no referent:
// no output has ever been emitted.  The convention here is (F), FIXED:
//
//     ρ₀ = qlt_type_of(0, constants)
//
// because it is the 1-type of the previous output the INTERPRETER actually
// supplies at its first enforced step: steps before `formula_time_point`
// are auto-continued with defaulted outputs (zero), so a lookback atom
// `o[t-1] ⋈ …` there is evaluated against the constant 0.  Fixing ρ₀ to the
// same type makes the Algorithm-D verdict and the execution agree.
//
// The two rejected alternatives, for the record: ∃ρ₀ ("the system chooses
// its initial memory") is unsound — ρ is not a free bookkeeping state like
// a Mealy initial state but the type of an actual prior output, so choosing
// it asserts a phantom value (starkest for a point type {c_j}) that no
// first move can implement, and the interpreter then fails at its first
// enforced step; ∀ρ₀ is needlessly incomplete — it pessimises against
// initial memories that can never occur.
//
// This is one of the three t = 0 conventions in the codebase; the other two
// are the safety pipeline's "initial lookback values are 0" (see
// `atom_has_lookback` in ltl_aba_normalization.tmpl.h) and the LA-N3
// inner-S auxiliary anchor `S(-1) = false` (see `compile_since_trigger_rec`
// there, and `seed_since_aux_bits` in interpreter.tmpl.h).  All three say
// the same thing: history before the first enforced step is the defaulted
// zero stream.

/**
 * @brief The fixed t = 0 initial memory type, rho_0 = qlt_type_of(0,
 * constants) (convention (F), see the section comment above).
 *
 * `sorted_constants` must be the sorted, deduplicated constants list the
 * T1/T3 positions were enumerated from (collect_qlt_constants returns it in
 * exactly that form).
 * @param sorted_constants Sorted, deduplicated qlt constants.
 * @return The T_1 index of the value 0.
 */
inline int initial_memory(const std::vector<omcat::rational>& sorted_constants) {
	return omcat::qlt_type_of(omcat::rational(0, 1), sorted_constants);
}

/**
 * @brief Build the product game (game x T_1) described in the section
 * comment above.
 *
 * Sys edges enumerate `sys_choices` and keep only D-patterns with a T3
 * type feasible from the current memory; env edges keep the memory and are
 * filtered by the same feasibility (sec. 14); edge stubs carry transition-based
 * priorities.  The initial product state is (G.init, init_rho); an
 * out-of-range @p init_rho is a caller bug (asserted; left as-is in Release,
 * which downstream reads as UNREALIZABLE).
 * @param G Synthesis game.
 * @param T1_size |T_1|.
 * @param T3 Enumerated 3-types.
 * @param type_A D-bitmask per T3 type.
 * @param K Number of D propositions.
 * @param init_rho Initial memory, from `initial_memory()`.
 * @return The product game, or an error result when the game has more
 * atomic propositions than the assignment enumeration supports.
 */
inline result<product_game> build_product_game(
	const synth_game& G,
	size_t T1_size,
	const std::vector<omcat::qlt_type3>& T3,
	const std::vector<int>& type_A,   // D-bitmask per T3 type
	int K,                             // number of D propositions
	int_t init_rho)                    // initial memory, from initial_memory()
{
	result<product_game> r;
	// The product state id is q * T1_size + rho, so |T_1| and the memory
	// position share the state id's size_t. A D-pattern is a bitmask and
	// stays int.
	const int n_aps = (int)G.aps.size();
	// The assignment loops below shift `1 << n_aps`; the parser already
	// refuses such a game, this guards games built by hand (tests, the
	// semantic PWR).
	if (n_aps > ltl_max_game_aps) {
		return r.with_error(code::unsupported_operation,
			"the product game has more atomic propositions than "
			"the assignment enumeration supports; refusing",
			{{label::limit, ltl_max_game_aps},
			 {label::value, std::to_string(n_aps)}});
	}
	// Colours of a condition other than all, Buchi, co-Buchi or parity
	// (Streett, generalized Buchi, ...) are not priorities.
	if (G.multi_colored || (!G.acc_known && G.n_colors > 0))
		return r.with_value(product_game{});
	// A parsed game's colours sit two above the priority of a run that
	// sees none, as in build_data_arena. A game built by hand, with no
	// acceptance declared, keeps the priorities it was given.
	const bool lift = G.acc_known;
	const int uncolored = G.acc_accepts_uncolored ? 1 : 0;
	auto state_prio = [&](int q) {
		if (!lift) return G.state_priority[q];
		return G.state_color[q] < 0 ? uncolored : G.state_priority[q] + 2;
	};

	// Fast feasibility lookup: given (pos_m, pos_y, D_pattern), does any T3 type match?
	// Index: rho * T1_size * (2^K) + rho_prime * (2^K) + D_pattern  → bool
	const int A_max = 1 << K;
	const size_t a_max = static_cast<size_t>(A_max);
	std::vector<std::vector<std::vector<bool>>> feasible(
		T1_size,
		std::vector<std::vector<bool>>(T1_size,
			std::vector<bool>(a_max, false)));
	// A qlt position and a D-pattern are non-negative by construction; the
	// table index takes that form here once.
	auto feasible_at = [&](size_t pos_m, size_t pos_y, size_t d_pat) {
		return feasible[pos_m][pos_y][d_pat];
	};
	for (size_t t = 0; t < T3.size(); ++t) {
		// A qlt position and a D-pattern are non-negative by
		// construction; reject a negative one before the conversion.
		const int_t pm = T3[t].pos_m;
		const int_t py = T3[t].pos_y;
		if (pm < 0 || py < 0
			|| !std::cmp_less(pm, T1_size) || !std::cmp_less(py, T1_size)
			|| type_A[t] < 0 || type_A[t] >= A_max)
				continue;
		feasible_at(static_cast<size_t>(pm), static_cast<size_t>(py),
			static_cast<size_t>(type_A[t])) = true;
	}

	// Precise environment edges.  An env edge exists only when its
	// guard's D-pattern is feasible from ρ, not merely when the guard is
	// satisfiable by any propositional assignment (the guard's D-content is
	// data, and data feasibility from ρ is the same fact regardless of which
	// player owns the state).
	// Filter: an env edge from (q, ρ) exists iff its guard admits an
	// assignment whose D-pattern is feasible from ρ to SOME ρ' (the memory
	// still updates on sys moves only, so the env target keeps ρ).
	auto env_edge_reachable = [&](size_t rho, const std::string& guard) {
		for (int a = 0; a < (1 << n_aps); ++a) {
			if (!eval_guard(guard, a, n_aps)) continue;
			const size_t D_pat = d_pattern_from_assignment(G, a, K);
			for (size_t rp = 0; rp < T1_size; ++rp)
				if (feasible_at(rho, rp, D_pat))
					return true;
		}
		return false;
	};

	// Base layer: G.num_states * T1_size states
	// Intermediate layer for trans-based acceptance: one extra state per edge
	// Layer 0: base (q, rho) with state priority
	// Layer 1: intermediate edge states (for trans-based priorities)
	//
	// For simplicity: if all trans have edge_priority = -1 (state-based acceptance),
	// we skip the intermediate layer entirely.
	bool need_edge_layer = false;
	for (size_t q = 0; q < G.num_states && !need_edge_layer; ++q)
		for (int p : G.edge_priority[q])
			if (p >= 0) { need_edge_layer = true; break; }

	// Count intermediate states: one per (q, rho, trans_idx) with edge priority
	// Intermediate state id = base + offset
	struct edge_stub {
		size_t q;
		size_t rho;
		size_t trans_idx;
		size_t next_q;
		size_t next_rho;
		int priority;
		int player; // pass-through to next_q's player (doesn't matter, single succ)
	};
	std::vector<edge_stub> stubs;
	// Map (q, rho, trans_idx) → stub_id
	std::map<std::tuple<size_t,size_t,size_t>, size_t> stub_map;

	const size_t base_n = G.num_states * T1_size;
	size_t stub_base = base_n;

	if (need_edge_layer) {
		for (size_t q = 0; q < G.num_states; ++q) {
			for (size_t rho = 0; rho < T1_size; ++rho) {
				for (size_t j = 0; j < G.trans[q].size(); ++j) {
					int ep = G.edge_priority[q][j];
					if (ep < 0) continue; // no edge color, skip
					const auto& [guard, next_q, edge_col] = G.trans[q][j];
					// LG-11: env stubs are independent of the
					// assignment -- create them once from any
					// satisfying assignment instead of
					// re-testing the key 2^n_aps times.
					if (G.player[q] != 1) {
						// Same feasibility filter as
						// the transition site below.
						if (!env_edge_reachable(rho,
							guard)) continue;
						auto key = std::make_tuple(
							q * T1_size + rho, j,
							rho);
						if (stub_map.find(key)
							== stub_map.end()) {
							stub_map[key] = stub_base
								+ stubs.size();
							stubs.push_back({q, rho,
								j, next_q, rho,
								ep, 0});
						}
						continue;
					}
					// Sys: picks D_pattern by AP name, picks rho'.
					for (const auto& [D_pat, a] : sys_choices(G, K)) {
						if (!eval_guard(guard, a, n_aps)) continue;
						for (size_t rp = 0; rp < T1_size; ++rp) {
							if (!feasible_at(rho, rp, D_pat))
								continue;
							auto key = std::make_tuple(q * T1_size + rho, j, rp);
							if (stub_map.find(key) == stub_map.end()) {
								stub_map[key] = stub_base + stubs.size();
								stubs.push_back({q, rho, j, next_q, rp, ep, 0});
							}
						}
					}
					// (env handled above, LG-11)
				}
			}
		}
	}

	product_game pg;
	pg.n_states = base_n + stubs.size();
	// LG-12: the initial memory is the caller-supplied fixed convention
	// (see initial_memory above) — not position 0, not a solver choice.
	// Out of range is a caller bug (assert); in Release the bad index is
	// left as-is, which downstream membership tests read as UNREALIZABLE —
	// fail-safe, unlike a silent clamp to position 0.
	assert(0 <= init_rho && static_cast<size_t>(init_rho) < T1_size);
	pg.init = G.init * T1_size + static_cast<size_t>(init_rho);
	pg.player.assign(pg.n_states, 0);
	pg.priority.assign(pg.n_states, 0);
	pg.succs.resize(pg.n_states);

	// Fill base states
	for (size_t q = 0; q < G.num_states; ++q) {
		for (size_t rho = 0; rho < T1_size; ++rho) {
			const size_t s = q * T1_size + rho;
			pg.player[s]   = G.player[q];
			pg.priority[s] = state_prio(q); // overridden by edge stubs if needed
		}
	}

	// Fill intermediate (stub) states
	for (size_t i = 0; i < stubs.size(); ++i) {
		const size_t s = stub_base + i;
		pg.player[s]   = 0; // pass-through: single successor, player irrelevant
		pg.priority[s] = lift ? stubs[i].priority + 2 : stubs[i].priority;
	}

	// Build transitions
	for (size_t q = 0; q < G.num_states; ++q) {
		for (size_t rho = 0; rho < T1_size; ++rho) {
			const size_t s = q * T1_size + rho;

			if (G.player[q] == 1) {
				// Sys: enumerate D-patterns, not raw AP assignments
				for (const auto& [D_pat, a] : sys_choices(G, K)) {
					// Find matching transition in game
					for (size_t j = 0; j < G.trans[q].size(); ++j) {
						const auto& [guard, nq, ec] = G.trans[q][j];
						if (!eval_guard(guard, a, n_aps)) continue;

						int ep = G.edge_priority[q][j];
						for (size_t rp = 0; rp < T1_size; ++rp) {
							if (!feasible_at(rho, rp, D_pat))
								continue;
							size_t dest = nq * T1_size + rp;
							if (ep >= 0) {
								// Use stub for edge priority
								auto key = std::make_tuple(q * T1_size + rho, j, rp);
								auto it = stub_map.find(key);
								if (it != stub_map.end()) {
									size_t stub = it->second;
									// s → stub → dest (if not already added)
									auto& succs_s = pg.succs[s];
									if (std::find(succs_s.begin(), succs_s.end(), stub) == succs_s.end())
										succs_s.push_back(stub);
									auto& succs_stub = pg.succs[stub];
									if (std::find(succs_stub.begin(), succs_stub.end(), dest) == succs_stub.end())
										succs_stub.push_back(dest);
									continue;
								}
							}
							// Direct transition
							auto& sv = pg.succs[s];
							if (std::find(sv.begin(), sv.end(), dest) == sv.end())
								sv.push_back(dest);
						}
					}
				}
			} else {
				// Env (player 0): rho unchanged (memory updates on
				// sys moves).  Env observes (but does not control)
				// output APs, and its edges are filtered by the
				// same T3 feasibility the sys edges use, since a
				// guard's data content is player-independent.
				for (size_t j = 0; j < G.trans[q].size(); ++j) {
					const auto& [guard, nq, ec] = G.trans[q][j];
					// Only edges whose D-content is
					// feasible from rho exist for the real
					// environment.
					if (!env_edge_reachable(rho, guard))
						continue;
					int ep = G.edge_priority[q][j];
					size_t dest = nq * T1_size + rho;
					if (ep >= 0) {
						auto key = std::make_tuple(q * T1_size + rho, j, rho);
						auto it = stub_map.find(key);
						if (it != stub_map.end()) {
							size_t stub = it->second;
							auto& ss = pg.succs[s];
							if (std::find(ss.begin(), ss.end(), stub) == ss.end())
								ss.push_back(stub);
							auto& st = pg.succs[stub];
							if (std::find(st.begin(), st.end(), dest) == st.end())
								st.push_back(dest);
							continue;
						}
					}
					auto& sv = pg.succs[s];
					if (std::find(sv.begin(), sv.end(), dest) == sv.end())
						sv.push_back(dest);
				}
			}
		}
	}
	return r.with_value(std::move(pg));
}

// ── Zielonka parity game solver ───────────────────────────────────────────
// Convention: ODD priorities favor player 1 (sys = controller = "wins").
// Returns: set of state indices where player 1 wins.

namespace zielonka_impl {

/// @brief Set of product-game state indices.
using StateSet = std::set<size_t>;

/// @brief Attractor of @p T for player @p p over the given successor
/// relation (standard backward closure).
static StateSet attractor(
	int p,               // attracting player (0 or 1)
	const StateSet& T,
	size_t n,
	const std::vector<int>& plr,
	const std::vector<std::vector<size_t>>& succs)
{
	// Build predecessor lists
	std::vector<std::vector<size_t>> preds(n);
	for (size_t u = 0; u < n; ++u)
		for (size_t v : succs[u]) preds[v].push_back(u);

	// degree[u] = number of successors of u NOT yet in the attractor
	std::vector<size_t> deg(n);
	for (size_t u = 0; u < n; ++u) deg[u] = succs[u].size();

	StateSet attr(T);
	std::vector<size_t> queue(T.begin(), T.end());
	while (!queue.empty()) {
		size_t v = queue.back(); queue.pop_back();
		for (size_t u : preds[v]) {
			if (attr.count(u)) continue;
			if (plr[u] == p) {
				// p can choose to go to v ∈ attr
				attr.insert(u);
				queue.push_back(u);
			} else {
				// other player: u ∈ attr iff ALL successors in attr
				--deg[u];
				if (deg[u] == 0) {
					attr.insert(u);
					queue.push_back(u);
				}
			}
		}
	}
	return attr;
}

/// @brief Zielonka recursion on the subgame @p V; returns {W0, W1}. Dead ends
/// are decided by the caller, not here (see the NOTE in the body).
static std::pair<StateSet,StateSet> solve(
	const StateSet& V,
	size_t n,
	const std::vector<int>& plr,
	const std::vector<int>& pri,
	const std::vector<std::vector<size_t>>& succs)
{
	if (V.empty()) return {{},{}};

	// Restrict game to V
	std::vector<std::vector<size_t>> succs_V(n);
	for (size_t u : V)
		for (size_t v : succs[u])
			if (V.count(v)) succs_V[u].push_back(v);

	// NOTE on dead ends: they are decided ONCE, BEFORE
	// this recursion, in `zielonka_win_player1`'s textbook preprocessing —
	// deliberately NOT here.  Inside the recursion "no successor in V" is
	// not the same statement as "cannot move": the sub-games Zielonka
	// builds are traps for one player only, so the OTHER player may still
	// have moves that leave V, and declaring it stuck would be wrong.  The
	// game handed to the top-level solve call has no dead ends at all.

	int c_max = -1;
	for (size_t u : V) c_max = std::max(c_max, pri[u]);
	if (c_max < 0) return {{},{}};

	StateSet A;
	for (size_t u : V) if (pri[u] == c_max) A.insert(u);

	int beneficiary = c_max % 2; // 0=even→env wins, 1=odd→sys wins
	StateSet X = attractor(beneficiary, A, n, plr, succs_V);
	// Sub-game on V \ X
	StateSet V2;
	for (size_t u : V) if (!X.count(u)) V2.insert(u);
	auto [W0p, W1p] = solve(V2, n, plr, pri, succs);

	// W_{1-b} in the sub-game
	StateSet& Wl = (beneficiary == 0) ? W1p : W0p; // "loser in sub-game"
	if (Wl.empty()) {
		// beneficiary wins everywhere in V
		if (beneficiary == 1) return {W0p, V};
		else                  return {V, W1p};
	}
	StateSet Y = attractor(1 - beneficiary, Wl, n, plr, succs_V);
	StateSet V3;
	for (size_t u : V) if (!Y.count(u)) V3.insert(u);
	auto [W0pp, W1pp] = solve(V3, n, plr, pri, succs);
	// Standard Zielonka: the opponent (player 1-b) wins on Y — the states from
	// which it can force the play into its sub-game winning set W'_{1-b} — plus
	// whatever it wins in the remaining sub-game V \ Y.  The beneficiary keeps
	// only its own share of that sub-game.
	StateSet W_1b_full = Y;
	for (size_t u : Wl) W_1b_full.insert(u);
	// W_1b_full is the opponent's region, so it collects the opponent's wins
	// in the residual subgame and is returned in the opponent's slot
	if (beneficiary == 1) {
		// opponent is player 0
		for (size_t u : W0pp) W_1b_full.insert(u);
		return {W_1b_full, W1pp};
	} else {
		// opponent is player 1
		for (size_t u : W1pp) W_1b_full.insert(u);
		return {W0pp, W_1b_full};
	}
}

} // namespace zielonka_impl

/**
 * @brief Returns the set of states where player 1 (sys) wins.
 *
 * TEXTBOOK dead-end semantics.  Parity-game
 * semantics say the player who cannot move LOSES the finite play, while
 * `solve` scores every state by its priority's parity — so dead ends are
 * decided here, BEFORE the parity recursion, the standard way:
 *
 *   repeat until the subgame has no dead ends:
 *     a dead end is lost for its owner; award it to the opponent TOGETHER
 *     WITH the opponent's attractor of it, and remove that attractor from
 *     the subgame (removal can create new dead ends, hence the loop);
 *   then run Zielonka on the residual subgame, which has none.
 *
 * Every state the attractors removed carries exactly one of two facts: the
 * winner can force the play into the dead-end set (∃-rule), or the loser
 * cannot avoid it (∀-rule).  A state remaining in the residual keeps at
 * least one in-subgame successor by the same rules, so `solve`'s internal
 * restriction to V creates no fresh dead ends.
 *
 * @param pg Product game to solve.
 * @return Indices of the product states won by player 1.
 */
inline std::set<size_t> zielonka_win_player1(const product_game& pg) {
	std::set<size_t> V;
	for (size_t s = 0; s < pg.n_states; ++s) V.insert(s);

	// Attractor of `target` for player `p` within the CURRENT V.
	auto attractor = [&](int p, std::set<size_t> target) {
		for (bool changed = true; changed; ) {
			changed = false;
			for (size_t u : V) {
				if (target.count(u)) continue;
				bool add = false;
				if (pg.player[u] == p) {
					for (size_t v : pg.succs[u])
						if (V.count(v)
							&& target.count(v)) {
							add = true;
							break;
						}
				} else {
					bool any = false, all = true;
					for (size_t v : pg.succs[u]) {
						if (!V.count(v)) continue;
						any = true;
						if (!target.count(v)) {
							all = false;
							break;
						}
					}
					add = any && all;
				}
				if (add) {
					target.insert(u);
					changed = true;
				}
			}
		}
		return target;
	};

	std::set<size_t> W1acc;   // sys wins (env dead ends + sys attractor)
	for (;;) {
		std::set<size_t> d_sys, d_env;
		for (size_t u : V) {
			bool any = false;
			for (size_t v : pg.succs[u])
				if (V.count(v)) { any = true; break; }
			if (!any)
				(pg.player[u] == 1 ? d_sys : d_env).insert(u);
		}
		if (d_sys.empty() && d_env.empty()) break;
		if (!d_sys.empty()) {
			// Sys stuck: env wins the attractor.  (Not accumulated:
			// only W1 is returned.)
			for (size_t u : attractor(0, d_sys)) V.erase(u);
			continue;   // removal may create new dead ends
		}
		for (size_t u : attractor(1, d_env)) {
			W1acc.insert(u);
			V.erase(u);
		}
	}

	auto [W0, W1] = zielonka_impl::solve(V, pg.n_states, pg.player,
		pg.priority, pg.succs);
	(void) W0;
	W1.insert(W1acc.begin(), W1acc.end());
	return W1;
}

// ── Main Algorithm D entry point ──────────────────────────────────────────

/**
 * @brief Decide realizability via Algorithm D: synthesis game, product with
 * T_1, Zielonka from the one initial state.
 *
 * PRECONDITION (LG-30): output-only qlt atoms. Nothing below guards this --
 * input atoms would silently produce garbage (the env branch never models
 * input choice). Callers must check atom_has_any_input first, as both
 * current callers (solve_ltl_aba, semantic_pwr_optimal) do.
 * Returns REALIZABLE/UNREALIZABLE via Algorithm D, or an error result when
 * the ltlsynt subprocess gave no verdict -- that case is undecided, not
 * UNREALIZABLE, and the caller must not read it as one.
 * @param phi_star propositional LTL with D_0,...,D_{K-1} as output
 * propositions.
 * @param T1_size |T_1|.
 * @param T3 enumerated 3-types.
 * @param type_A D_pattern bitmask for each T3 type.
 * @param K number of D propositions.
 * @param init_rho the fixed initial memory type -- pass
 *   initial_memory(constants), which returns the fixed t = 0 memory type
 *   (LG-12: see the convention block at initial_memory).
 * @return `true` iff player 1 wins from (G.init, init_rho); `false` for an
 * empty formula, an empty T_1 or an empty game; an error result when
 * ltlsynt gave no verdict.
 */
inline result<bool> solve_algorithm_d(
	const std::string& phi_star,
	size_t T1_size,
	const std::vector<omcat::qlt_type3>& T3,
	const std::vector<int>& type_A,
	int K,
	int_t init_rho)
{
	result<bool> r;
	if (phi_star.empty() || T1_size == 0) { return r.with_value(false); }

	// Build list of D propositions as output
	std::vector<std::string> D_outs;
	for (int i = 0; i < K; ++i) D_outs.push_back("d_" + std::to_string(i));

	// Get synthesis parity game for φ*(D_i)
	TAU_TRY(auto G, call_ltlsynt_game(phi_star, {}, D_outs));
	if (G.num_states == 0) { return r.with_value(false); }

	// Build product game (G × T_1). A refused construction (too many
	// APs) is not an UNREALIZABLE verdict.
	TAU_TRY(auto pg, build_product_game(G, T1_size, T3, type_A, K, init_rho));

	// Solve parity game with Zielonka
	auto W1 = zielonka_win_player1(pg);

	// REALIZABLE iff player 1 wins from the ONE initial state
	// (G.init, init_rho) — convention (F), see initial_memory.
	return r.with_value(W1.count(pg.init) != 0);
}

// ── Extended Algorithm D: returns winning region for semantic PWR ──────────

/// @brief Extended Algorithm D result: verdict plus the winning region and
/// the games it was computed on (consumed by semantic PWR).
struct alg_d_result {
	bool realizable = false;
	std::set<size_t> winning_region; // W1 state indices in product game
	struct product_game product_game;
	struct synth_game synth_game;
	size_t T1_size = 0;
	int K = 0;                        // number of D propositions
	// The FIXED initial memory type (LG-12 convention (F), equal to the
	// initial_memory() argument) when realizable; -1 if unrealizable.
	// Consistent with product_game.init by construction:
	// product_game.init == synth_game.init * T1_size + init_rho.
	int_t init_rho = -1;
};

/**
 * @brief Same as `solve_algorithm_d`, but returns the winning region and
 * the games for semantic PWR.
 *
 * PRECONDITION (LG-30): output-only qlt atoms; see solve_algorithm_d above.
 * An error result means the ltlsynt subprocess gave no verdict (undecided,
 * not unrealizable); see solve_algorithm_d above.
 * @param phi_star propositional LTL with D_0,...,D_{K-1} as output
 * propositions.
 * @param T1_size |T_1|.
 * @param T3 enumerated 3-types.
 * @param type_A D_pattern bitmask for each T3 type.
 * @param K number of D propositions.
 * @param init_rho the fixed initial memory type -- pass
 * initial_memory(constants).
 * @return The result; `init_rho` stays -1 when unrealizable.
 */
inline result<alg_d_result> solve_algorithm_d_full(
	const std::string& phi_star,
	size_t T1_size,
	const std::vector<omcat::qlt_type3>& T3,
	const std::vector<int>& type_A,
	int K,
	int_t init_rho)
{
	result<alg_d_result> r;
	alg_d_result result;
	result.T1_size = T1_size;
	result.K = K;

	if (phi_star.empty() || T1_size == 0) { return r.with_value(std::move(result)); }

	std::vector<std::string> D_outs;
	for (int i = 0; i < K; ++i) D_outs.push_back("d_" + std::to_string(i));

	TAU_TRY(result.synth_game, call_ltlsynt_game(phi_star, {}, D_outs));
	if (result.synth_game.num_states == 0) { return r.with_value(std::move(result)); }

	TAU_TRY(result.product_game, build_product_game(
		result.synth_game, T1_size, T3, type_A, K, init_rho));

	result.winning_region = zielonka_win_player1(result.product_game);

	// LG-12: one initial state — (synth_game.init, init_rho) — not ∃ρ₀.
	// init_rho stays -1 when unrealizable (pinned by SPWR-A-02).
	if (result.winning_region.count(result.product_game.init)) {
		result.realizable = true;
		result.init_rho = init_rho;
	}
	return r.with_value(std::move(result));
}

} // namespace idni::tau_lang::alg_d

#endif // __IDNI__TAU__ALGORITHM_D_GAME_H__
