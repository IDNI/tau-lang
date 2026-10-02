// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// The synthesis game of ltlsynt (`--print-game-hoa`): its HOA guard
// evaluator, its parser and the call that produces it, which the data game
// and the generated programs read.

#ifndef __IDNI__TAU__ALGORITHM_D_GAME_H__
#define __IDNI__TAU__ALGORITHM_D_GAME_H__

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
 * need only this declaration. Not spelled inline here: a unit that includes
 * this header without the definition would declare an inline function it
 * never defines, which gcc rejects; the definition is inline and is emitted
 * by every unit that includes it.
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

} // namespace idni::tau_lang::alg_d

#endif // __IDNI__TAU__ALGORITHM_D_GAME_H__
