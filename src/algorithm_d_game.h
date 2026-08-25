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

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdio>
#include <sys/wait.h>
#include <map>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace idni::tau_lang::alg_d {

// ── Synthesis game (from ltlsynt --print-game-hoa) ───────────────────────

struct SynthGame {
	int num_states = 0;
	int init       = 0;
	// player[q]: 0 = env (uncontrollable), 1 = sys (controller)
	std::vector<int> player;
	// state_color[q]: color for state-based acceptance (-1 if none)
	std::vector<int> state_color;
	// trans[q]: list of (guard_string, next_state, edge_color)
	// edge_color = -1 if no acceptance mark on this transition
	std::vector<std::vector<std::tuple<std::string,int,int>>> trans;
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
};

// ── HOA boolean formula evaluator ────────────────────────────────────────

// Parse and evaluate a HOA guard formula string against an AP bitmask.
// Grammar: t | f | N | !E | E&E | E|E | (E)
// Returns true iff the formula is satisfied by the assignment.

namespace hoa_guard {

static inline void skip_ws(const std::string& s, size_t& i) {
	while (i < s.size() && (s[i] == ' ' || s[i] == '\t')) ++i;
}

static bool eval(const std::string& s, size_t& i, int bitmask, int n_aps);

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
		int ap = std::stoi(s.substr(i, j - i));
		i = j;
		if (ap < 0 || ap >= n_aps) return false;
		return (bitmask >> ap) & 1;
	}
	return false;
}

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
// Consumers that must EMIT code for a label (tau_codegen) or reason about it
// symbolically (the ABA oracle) need its cubes instead, and both used to
// hand-lex it — identically wrongly.  The digit loop
// `for (char c : idx_str) if (isdigit(c)) idx = idx*10+(c-'0')` SKIPS '|', '('
// and ')' instead of rejecting them, so `0|1` was read as the single literal
// `1` (wrong AP, other disjunct lost) and `(0|1)` was dropped entirely,
// silently widening the guard to `true`.  Spot prints strategy edge labels as
// sums of products, so those shapes are the normal case, not a corner (LG-4).
//
// `to_dnf` returns nullopt when the label does not parse or the expansion
// exceeds `max_cubes`.  Callers must REFUSE the edge in that case: falling
// back to a partial reading is exactly the defect this replaces.

struct lit { int ap; bool pos; };
using cube = std::vector<lit>;   // conjunction; empty cube == true
// A DNF is a vector of cubes; an empty vector == false.

namespace dnf_detail {

// Normalise a cube: sort by AP, drop duplicates, reject if an AP appears with
// both polarities (the cube is then unsatisfiable).
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

struct parser {
	const std::string& s;
	size_t i = 0;
	size_t max_cubes;
	bool failed = false;

	explicit parser(const std::string& str, size_t cap) : s(str), max_cubes(cap) {}

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
			int ap = std::stoi(s.substr(i, j - i));
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

inline std::optional<std::vector<cube>> to_dnf(
	const std::string& label, size_t max_cubes = 512)
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

/// Evaluate a HOA guard label under an AP assignment. Bit `i` of
/// @p bitmask is the truth value of AP index `i` (the convention used by
/// build_product_game's 2^n_aps assignment loops and by the tests).
inline bool eval_guard(const std::string& guard, int bitmask, int n_aps) {
	size_t i = 0;
	return hoa_guard::eval(guard, i, bitmask, n_aps);
}

// ── HOA synthesis game parser ─────────────────────────────────────────────

inline SynthGame parse_synth_game_hoa(const std::string& hoa_text) {
	SynthGame g;

	// A decomposed specification prints one game per part. Parsing only the
	// first would drop the other parts' constraints, and their output APs
	// with them, which reads downstream as those APs being false.
	size_t n_games = 0;
	for (size_t p = hoa_text.find("HOA:"); p != std::string::npos;
		p = hoa_text.find("HOA:", p + 4))
		if (p == 0 || hoa_text[p - 1] == '\n') ++n_games;
	if (n_games > 1) return g;

	std::istringstream ss(hoa_text);
	std::string line;
	bool in_body = false;
	int cur_state = -1;
	bool is_buchi = false, is_cobuchi = false, is_all = false;
	bool is_parity = false, parity_min = false, parity_even = false;
	std::string acc_cond;   // the condition after the colour count

	auto parse_int_list = [](const std::string& s) {
		std::vector<int> out;
		std::istringstream is(s);
		int x;
		while (is >> x) out.push_back(x);
		return out;
	};

	while (std::getline(ss, line)) {
		if (line.empty()) continue;
		if (line == "--BODY--") { in_body = true; continue; }
		if (line == "--END--")  { break; }

		if (!in_body) {
			if (line.substr(0,7) == "States:") {
				g.num_states = std::stoi(line.substr(7));
				g.player.assign(g.num_states, 0);
				g.state_color.assign(g.num_states, -1);
				g.state_priority.assign(g.num_states, 0);
				g.trans.resize(g.num_states);
				g.edge_priority.resize(g.num_states);
			} else if (line.substr(0,6) == "Start:") {
				g.init = std::stoi(line.substr(6));
			} else if (line.substr(0,3) == "AP:") {
				std::istringstream apl(line.substr(3));
				int n; apl >> n;
				g.aps.resize(n);
				g.controllable.resize(n, false);
				for (int i = 0; i < n; ++i) {
					std::string ap; apl >> ap;
					if (!ap.empty() && ap.front() == '"') ap = ap.substr(1, ap.size()-2);
					g.aps[i] = ap;
				}
			} else if (line.substr(0,16) == "controllable-AP:") {
				auto idxs = parse_int_list(line.substr(16));
				for (int i : idxs) if (i < (int)g.controllable.size()) g.controllable[i] = true;
			} else if (line.substr(0,17) == "spot.state-player"
				|| line.substr(0,17) == "spot-state-player") {
				// "spot.state-player: 0 1 0 1 ..."
				// Spot (--print-game-hoa) spells the header name with a
				// dot; the dashed form is accepted too for hand-written
				// and legacy fixtures.  Getting this wrong silently
				// leaves every state env-owned.
				size_t colon = line.find(':');
				if (colon != std::string::npos) {
					auto players = parse_int_list(line.substr(colon+1));
					for (int q = 0; q < (int)players.size() && q < g.num_states; ++q)
						g.player[q] = players[q];
				}
			} else if (line.find("acc-name:") != std::string::npos) {
				if (line.find(" all") != std::string::npos) is_all = true;
				else if (line.find("Buchi") != std::string::npos &&
				         line.find("co-Buchi") == std::string::npos) is_buchi = true;
				else if (line.find("co-Buchi") != std::string::npos) is_cobuchi = true;
				else if (line.find("parity") != std::string::npos) {
					// LG-3: capture the flavor; normalized to
					// max-odd (the solver's convention) below.
					is_parity   = true;
					parity_min  = line.find(" min")  != std::string::npos;
					parity_even = line.find(" even") != std::string::npos;
				}
				// else: Streett acceptance (handled via n_colors)
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
			sl >> cur_state;
			// Check for color marks
			size_t lb = line.find('{');
			size_t rb = line.find('}');
			if (lb != std::string::npos && rb != std::string::npos) {
				std::istringstream cl(line.substr(lb+1, rb-lb-1));
				int c; cl >> c;
				if (cur_state < g.num_states) g.state_color[cur_state] = c;
			}
			continue;
		}
		if (cur_state < 0 || line.front() != '[') continue;

		// Parse transition: [guard] next_state {optional_color}
		size_t rb = line.find(']');
		if (rb == std::string::npos) continue;
		std::string guard = line.substr(1, rb - 1);
		std::istringstream tl(line.substr(rb+1));
		int next; tl >> next;
		// Check for edge acceptance mark
		int edge_color = -1;
		size_t elb = line.find('{', rb);
		size_t erb = line.find('}', rb);
		if (elb != std::string::npos && erb != std::string::npos) {
			std::istringstream cl(line.substr(elb+1, erb-elb-1));
			cl >> edge_color;
		}
		if (cur_state < g.num_states)
			g.trans[cur_state].emplace_back(guard, next, edge_color);
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
	for (int q = 0; q < g.num_states; ++q) {
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
	g.edge_priority.resize(g.num_states);
	for (int q = 0; q < g.num_states; ++q) {
		g.edge_priority[q].resize(g.trans[q].size(), -1);
		for (int j = 0; j < (int)g.trans[q].size(); ++j) {
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
	return g;
}

// ── Call ltlsynt and get parity game ──────────────────────────────────────

// Run ltlsynt on `phi_prop` and parse `--print-game-hoa` into a SynthGame.
//
// DEFINED IN ltl_aba_synthesis.tmpl.h, not here (LS-10).  It needs
// `write_tempfile` + `spawn_capture`, which live in that header and are
// included after this one; the callers below need only this declaration.
inline const SynthGame& call_ltlsynt_game(
	const std::string& phi_prop,
	const std::vector<std::string>& ins,
	const std::vector<std::string>& outs);

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

struct ProductGame {
	int  n_states = 0;
	int  init     = 0;
	std::vector<int> player;
	std::vector<int> priority;      // state-based (after intermediate conversion)
	std::vector<std::vector<int>> succs;
};

/// Parse the disjunct index N from a `d_N` atomic-proposition name.
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

/// Project an AP assignment (bit `i` = truth of AP index `i`) onto the
/// controllable `d_N` APs: bit `N` of the result is set iff `d_N` is true
/// in @p assignment. @p K bounds the accepted disjunct indices.
inline int d_pattern_from_assignment(const SynthGame& G, int assignment, int K) {
	int pat = 0;
	for (int ap = 0; ap < (int)G.aps.size(); ++ap) {
		if (ap >= (int)G.controllable.size() || !G.controllable[ap]) continue;
		if (((assignment >> ap) & 1) == 0) continue;
		int d = d_index_from_ap_name(G.aps[ap]);
		if (0 <= d && d < K) pat |= (1 << d);
	}
	return pat;
}

// The system's choices as (D_pattern, AP assignment) pairs. An output AP the
// game does not mention is unconstrained by the formula, so its D-bit still
// ranges over both values instead of silently reading as false — otherwise half
// the system's moves disappear and a realizable spec can report UNREALIZABLE.
inline std::vector<std::pair<int,int>> sys_choices(const SynthGame& G, int K) {
	const int n_aps = (int)G.aps.size();
	std::vector<int> ap_of_d(K, -1);
	std::vector<int> other_aps;
	for (int ap = 0; ap < n_aps; ++ap) {
		const bool ctrl = ap < (int)G.controllable.size() && G.controllable[ap];
		const int d = ctrl ? d_index_from_ap_name(G.aps[ap]) : -1;
		if (0 <= d && d < K) ap_of_d[d] = ap;
		else other_aps.push_back(ap);
	}
	const int n_other = (int)other_aps.size();
	std::vector<std::pair<int,int>> out;
	out.reserve((size_t)(1 << K) << n_other);
	for (int D_pat = 0; D_pat < (1 << K); ++D_pat)
		for (int o = 0; o < (1 << n_other); ++o) {
			int a = 0;
			for (int i = 0; i < K; ++i)
				if (((D_pat >> i) & 1) && ap_of_d[i] >= 0)
					a |= 1 << ap_of_d[i];
			for (int t = 0; t < n_other; ++t)
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
//
// `sorted_constants` must be the sorted, deduplicated constants list the
// T1/T3 positions were enumerated from (collect_qlt_constants returns it in
// exactly that form).
inline int initial_memory(const std::vector<omcat::Rat>& sorted_constants) {
	return omcat::qlt_type_of(omcat::Rat(0, 1), sorted_constants);
}

inline ProductGame build_product_game(
	const SynthGame& G,
	int T1_size,
	const std::vector<omcat::QltType3>& T3,
	const std::vector<int>& type_A,   // D-bitmask per T3 type
	int K,                             // number of D propositions
	int init_rho)                      // initial memory, from initial_memory()
{
	const int n_aps = (int)G.aps.size();

	// Fast feasibility lookup: given (pos_m, pos_y, D_pattern), does any T3 type match?
	// Index: rho * T1_size * (2^K) + rho_prime * (2^K) + D_pattern  → bool
	const int A_max = 1 << K;
	std::vector<std::vector<std::vector<bool>>> feasible(
		T1_size,
		std::vector<std::vector<bool>>(T1_size, std::vector<bool>(A_max, false)));
	for (int t = 0; t < (int)T3.size(); ++t) {
		int pm = T3[t].pos_m, py = T3[t].pos_y;
		if (pm < T1_size && py < T1_size && type_A[t] < A_max)
			feasible[pm][py][type_A[t]] = true;
	}

	// §14 / Batch O7: precise environment edges.  An env edge used to be
	// added whenever its guard was satisfiable by ANY propositional
	// assignment — no feasibility filter — so the product game let the
	// environment "move" into states the real environment cannot reach
	// (the guard's D-content is data, and data feasibility from ρ is the
	// same fact regardless of which player owns the state).  Those phantom
	// moves are why textbook dead-end semantics used to flip ALG-D-28 and
	// why the opponent attractor was refused in zielonka_win_player1.
	// Filter: an env edge from (q, ρ) exists iff its guard admits an
	// assignment whose D-pattern is feasible from ρ to SOME ρ' (the memory
	// still updates on sys moves only, so the env target keeps ρ).
	auto env_edge_reachable = [&](int rho, const std::string& guard) {
		for (int a = 0; a < (1 << n_aps); ++a) {
			if (!eval_guard(guard, a, n_aps)) continue;
			int D_pat = d_pattern_from_assignment(G, a, K);
			for (int rp = 0; rp < T1_size; ++rp)
				if (feasible[rho][rp][D_pat]) return true;
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
	for (int q = 0; q < G.num_states && !need_edge_layer; ++q)
		for (int p : G.edge_priority[q])
			if (p >= 0) { need_edge_layer = true; break; }

	// Count intermediate states: one per (q, rho, trans_idx) with edge priority
	// Intermediate state id = base + offset
	struct EdgeStub {
		int q, rho, trans_idx, next_q, next_rho;
		int priority;
		int player; // pass-through to next_q's player (doesn't matter, single succ)
	};
	std::vector<EdgeStub> stubs;
	// Map (q, rho, trans_idx) → stub_id
	std::map<std::tuple<int,int,int>, int> stub_map;

	const int base_n = G.num_states * T1_size;
	int stub_base = base_n;

	if (need_edge_layer) {
		for (int q = 0; q < G.num_states; ++q) {
			for (int rho = 0; rho < T1_size; ++rho) {
				for (int j = 0; j < (int)G.trans[q].size(); ++j) {
					int ep = G.edge_priority[q][j];
					if (ep < 0) continue; // no edge color, skip
					const auto& [guard, next_q, edge_col] = G.trans[q][j];
					// LG-11: env stubs are independent of the
					// assignment -- create them once from any
					// satisfying assignment instead of
					// re-testing the key 2^n_aps times.
					if (G.player[q] != 1) {
						// §14: same feasibility filter as
						// the transition site below.
						if (!env_edge_reachable(rho,
							guard)) continue;
						auto key = std::make_tuple(
							q * T1_size + rho, j,
							rho);
						if (stub_map.find(key)
							== stub_map.end()) {
							stub_map[key] = stub_base
								+ (int)stubs.size();
							stubs.push_back({q, rho,
								j, next_q, rho,
								ep, 0});
						}
						continue;
					}
					// Sys: picks D_pattern by AP name, picks rho'.
					for (const auto& [D_pat, a] : sys_choices(G, K)) {
						if (!eval_guard(guard, a, n_aps)) continue;
						for (int rp = 0; rp < T1_size; ++rp) {
							if (!feasible[rho][rp][D_pat]) continue;
							auto key = std::make_tuple(q * T1_size + rho, j, rp);
							if (stub_map.find(key) == stub_map.end()) {
								stub_map[key] = stub_base + (int)stubs.size();
								stubs.push_back({q, rho, j, next_q, rp, ep, 0});
							}
						}
					}
					// (env handled above, LG-11)
				}
			}
		}
	}

	ProductGame pg;
	pg.n_states = base_n + (int)stubs.size();
	// LG-12: the initial memory is the caller-supplied fixed convention
	// (see initial_memory above) — not position 0, not a solver choice.
	// Out of range is a caller bug (assert); in Release the bad index is
	// left as-is, which downstream membership tests read as UNREALIZABLE —
	// fail-safe, unlike a silent clamp back to the old position-0 phantom.
	assert(0 <= init_rho && init_rho < T1_size);
	pg.init = G.init * T1_size + init_rho;
	pg.player.assign(pg.n_states, 0);
	pg.priority.assign(pg.n_states, 0);
	pg.succs.resize(pg.n_states);

	// Fill base states
	for (int q = 0; q < G.num_states; ++q) {
		for (int rho = 0; rho < T1_size; ++rho) {
			int s = q * T1_size + rho;
			pg.player[s]   = G.player[q];
			pg.priority[s] = G.state_priority[q]; // overridden by edge stubs if needed
		}
	}

	// Fill intermediate (stub) states
	for (int i = 0; i < (int)stubs.size(); ++i) {
		int s = stub_base + i;
		pg.player[s]   = 0; // pass-through: single successor, player irrelevant
		pg.priority[s] = stubs[i].priority;
	}

	// Build transitions
	for (int q = 0; q < G.num_states; ++q) {
		for (int rho = 0; rho < T1_size; ++rho) {
			int s = q * T1_size + rho;

			if (G.player[q] == 1) {
				// Sys: enumerate D-patterns, not raw AP assignments
				for (const auto& [D_pat, a] : sys_choices(G, K)) {
					// Find matching transition in game
					for (int j = 0; j < (int)G.trans[q].size(); ++j) {
						const auto& [guard, nq, ec] = G.trans[q][j];
						if (!eval_guard(guard, a, n_aps)) continue;

						int ep = G.edge_priority[q][j];
						for (int rp = 0; rp < T1_size; ++rp) {
							if (!feasible[rho][rp][D_pat]) continue;
							int dest = nq * T1_size + rp;
							if (ep >= 0) {
								// Use stub for edge priority
								auto key = std::make_tuple(q * T1_size + rho, j, rp);
								auto it = stub_map.find(key);
								if (it != stub_map.end()) {
									int stub = it->second;
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
				// output APs; §14 filters its edges by the same T3
				// feasibility the sys edges use, since a guard's
				// data content is player-independent.
				for (int j = 0; j < (int)G.trans[q].size(); ++j) {
					const auto& [guard, nq, ec] = G.trans[q][j];
					// §14: only edges whose D-content is
					// feasible from rho exist for the real
					// environment.
					if (!env_edge_reachable(rho, guard))
						continue;
					int ep = G.edge_priority[q][j];
					int dest = nq * T1_size + rho;
					if (ep >= 0) {
						auto key = std::make_tuple(q * T1_size + rho, j, rho);
						auto it = stub_map.find(key);
						if (it != stub_map.end()) {
							int stub = it->second;
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
	return pg;
}

// ── Zielonka parity game solver ───────────────────────────────────────────
// Convention: ODD priorities favor player 1 (sys = controller = "wins").
// Returns: set of state indices where player 1 wins.

namespace zielonka_impl {

using StateSet = std::set<int>;

static StateSet attractor(
	int p,               // attracting player (0 or 1)
	const StateSet& T,
	int n,
	const std::vector<int>& plr,
	const std::vector<std::vector<int>>& succs)
{
	// Build predecessor lists
	std::vector<std::vector<int>> preds(n);
	for (int u = 0; u < n; ++u)
		for (int v : succs[u]) preds[v].push_back(u);

	// degree[u] = number of successors of u NOT yet in the attractor
	std::vector<int> deg(n);
	for (int u = 0; u < n; ++u) deg[u] = (int)succs[u].size();

	StateSet attr(T);
	std::vector<int> queue(T.begin(), T.end());
	while (!queue.empty()) {
		int v = queue.back(); queue.pop_back();
		for (int u : preds[v]) {
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

static std::pair<StateSet,StateSet> solve(
	const StateSet& V,
	int n,
	const std::vector<int>& plr,
	const std::vector<int>& pri,
	const std::vector<std::vector<int>>& succs)
{
	if (V.empty()) return {{},{}};

	// Restrict game to V
	std::vector<std::vector<int>> succs_V(n);
	for (int u : V)
		for (int v : succs[u])
			if (V.count(v)) succs_V[u].push_back(v);

	// NOTE on dead ends (LG-32 / Batch O7): they are decided ONCE, BEFORE
	// this recursion, in `zielonka_win_player1`'s textbook preprocessing —
	// deliberately NOT here.  Inside the recursion "no successor in V" is
	// not the same statement as "cannot move": the sub-games Zielonka
	// builds are traps for one player only, so the OTHER player may still
	// have moves that leave V, and declaring it stuck would be wrong.  The
	// game handed to the top-level solve call has no dead ends at all.

	int c_max = -1;
	for (int u : V) c_max = std::max(c_max, pri[u]);
	if (c_max < 0) return {{},{}};

	StateSet A;
	for (int u : V) if (pri[u] == c_max) A.insert(u);

	int beneficiary = c_max % 2; // 0=even→env wins, 1=odd→sys wins
	StateSet X = attractor(beneficiary, A, n, plr, succs_V);
	// Sub-game on V \ X
	StateSet V2;
	for (int u : V) if (!X.count(u)) V2.insert(u);
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
	for (int u : V) if (!Y.count(u)) V3.insert(u);
	auto [W0pp, W1pp] = solve(V3, n, plr, pri, succs);
	// Standard Zielonka: the opponent (player 1-b) wins on Y — the states from
	// which it can force the play into its sub-game winning set W'_{1-b} — plus
	// whatever it wins in the remaining sub-game V \ Y.  The beneficiary keeps
	// only its own share of that sub-game.
	StateSet W_1b_full = Y;
	for (int u : Wl) W_1b_full.insert(u);
	// W_1b_full is the opponent's region, so it collects the opponent's wins
	// in the residual subgame and is returned in the opponent's slot
	if (beneficiary == 1) {
		// opponent is player 0
		for (int u : W0pp) W_1b_full.insert(u);
		return {W_1b_full, W1pp};
	} else {
		// opponent is player 1
		for (int u : W1pp) W_1b_full.insert(u);
		return {W0pp, W_1b_full};
	}
}

} // namespace zielonka_impl

// Returns the set of states where player 1 (sys) wins.
//
// LG-32 / AL-R1 / §14 (Batch O7): TEXTBOOK dead-end semantics.  Parity-game
// semantics say the player who cannot move LOSES the finite play, while
// `solve` scores every state by its priority's parity — so dead ends are
// decided here, BEFORE the parity recursion, the standard way:
//
//   repeat until the subgame has no dead ends:
//     a dead end is lost for its owner; award it to the opponent TOGETHER
//     WITH the opponent's attractor of it, and remove that attractor from
//     the subgame (removal can create new dead ends, hence the loop);
//   then run Zielonka on the residual subgame, which has none.
//
// Every state the attractors removed carries exactly one of two facts: the
// winner can force the play into the dead-end set (∃-rule), or the loser
// cannot avoid it (∀-rule).  A state remaining in the residual keeps at
// least one in-subgame successor by the same rules, so `solve`'s internal
// restriction to V creates no fresh dead ends.
//
// History: this replaces the prune-own-suicidal-edges + one-shot-override
// patch, which deliberately REFUSED the opponent attractor because
// `build_product_game`'s environment edges were over-approximated (any
// satisfiable guard, no data feasibility) — a phantom env move could then
// "force" sys into a dead end no real environment can reach, flipping the
// realizable ALG-D-28.  §14 made the env edges precise (the same T3
// feasibility filter the sys edges use), so the refusal's reason is gone
// and the textbook rule is exactly right.
inline std::set<int> zielonka_win_player1(const ProductGame& pg) {
	std::set<int> V;
	for (int s = 0; s < pg.n_states; ++s) V.insert(s);

	// Attractor of `target` for player `p` within the CURRENT V.
	auto attractor = [&](int p, std::set<int> target) {
		for (bool changed = true; changed; ) {
			changed = false;
			for (int u : V) {
				if (target.count(u)) continue;
				bool add = false;
				if (pg.player[u] == p) {
					for (int v : pg.succs[u])
						if (V.count(v)
							&& target.count(v)) {
							add = true;
							break;
						}
				} else {
					bool any = false, all = true;
					for (int v : pg.succs[u]) {
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

	std::set<int> W1acc;   // sys wins (env dead ends + sys attractor)
	for (;;) {
		std::set<int> d_sys, d_env;
		for (int u : V) {
			bool any = false;
			for (int v : pg.succs[u])
				if (V.count(v)) { any = true; break; }
			if (!any)
				(pg.player[u] == 1 ? d_sys : d_env).insert(u);
		}
		if (d_sys.empty() && d_env.empty()) break;
		if (!d_sys.empty()) {
			// Sys stuck: env wins the attractor.  (Not accumulated:
			// only W1 is returned.)
			for (int u : attractor(0, d_sys)) V.erase(u);
			continue;   // removal may create new dead ends
		}
		for (int u : attractor(1, d_env)) {
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

// PRECONDITION (LG-30): output-only qlt atoms. Nothing below guards this --
// input atoms would silently produce garbage (the env branch never models
// input choice). Callers must check atom_has_any_input first, as both
// current callers (solve_ltl_aba, semantic_pwr_optimal) do.
// Returns true if the formula is REALIZABLE via Algorithm D.
// phi_star: propositional LTL with D_0,...,D_{K-1} as output propositions.
// T1_size: |T_1|.
// T3: enumerated 3-types.
// type_A: D_pattern bitmask for each T3 type.
// K: number of D propositions.
// init_rho: the fixed initial memory type — pass initial_memory(constants)
//   (LG-12: the ∃ρ₀ loop this replaces let the system win by asserting a
//   phantom previous output; see the convention block at initial_memory).
inline bool solve_algorithm_d(
	const std::string& phi_star,
	int T1_size,
	const std::vector<omcat::QltType3>& T3,
	const std::vector<int>& type_A,
	int K,
	int init_rho)
{
	if (phi_star.empty() || T1_size <= 0) return false;

	// Build list of D propositions as output
	std::vector<std::string> D_outs;
	for (int i = 0; i < K; ++i) D_outs.push_back("d_" + std::to_string(i));

	// Get synthesis parity game for φ*(D_i)
	SynthGame G = call_ltlsynt_game(phi_star, {}, D_outs);
	if (G.num_states == 0) return false;

	// Build product game (G × T_1)
	ProductGame pg = build_product_game(G, T1_size, T3, type_A, K, init_rho);
	if (pg.n_states == 0) return false;

	// Solve parity game with Zielonka
	auto W1 = zielonka_win_player1(pg);

	// REALIZABLE iff player 1 wins from the ONE initial state
	// (G.init, init_rho) — convention (F), see initial_memory.
	return W1.count(pg.init) != 0;
}

// ── Extended Algorithm D: returns winning region for semantic PWR ──────────

struct AlgDResult {
	bool realizable = false;
	std::set<int> winning_region;     // W1 state indices in product game
	ProductGame product_game;
	SynthGame synth_game;
	int T1_size = 0;
	int K = 0;                        // number of D propositions
	// The FIXED initial memory type (LG-12 convention (F), equal to the
	// initial_memory() argument) when realizable; -1 if unrealizable.
	// Consistent with product_game.init by construction:
	// product_game.init == synth_game.init * T1_size + init_rho.
	int init_rho = -1;
};

// PRECONDITION (LG-30): output-only qlt atoms; see solve_algorithm_d above.
// init_rho: the fixed initial memory type — pass initial_memory(constants).
inline AlgDResult solve_algorithm_d_full(
	const std::string& phi_star,
	int T1_size,
	const std::vector<omcat::QltType3>& T3,
	const std::vector<int>& type_A,
	int K,
	int init_rho)
{
	AlgDResult result;
	result.T1_size = T1_size;
	result.K = K;

	if (phi_star.empty() || T1_size <= 0) return result;

	std::vector<std::string> D_outs;
	for (int i = 0; i < K; ++i) D_outs.push_back("d_" + std::to_string(i));

	result.synth_game = call_ltlsynt_game(phi_star, {}, D_outs);
	if (result.synth_game.num_states == 0) return result;

	result.product_game = build_product_game(
		result.synth_game, T1_size, T3, type_A, K, init_rho);
	if (result.product_game.n_states == 0) return result;

	result.winning_region = zielonka_win_player1(result.product_game);

	// LG-12: one initial state — (synth_game.init, init_rho) — not ∃ρ₀.
	// init_rho stays -1 when unrealizable (pinned by SPWR-A-02).
	if (result.winning_region.count(result.product_game.init)) {
		result.realizable = true;
		result.init_rho = init_rho;
	}
	return result;
}

} // namespace idni::tau_lang::alg_d

#endif // __IDNI__TAU__ALGORITHM_D_GAME_H__
