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
#include "hoa_scan.h"
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
	/// Number of states.
	size_t num_states = 0;
	/// Initial state.
	size_t init = 0;
	/// player[q]: 0 = env (uncontrollable), 1 = sys (controller)
	std::vector<int> player;
	/// state_color[q]: color for state-based acceptance (-1 if none)
	std::vector<int> state_color;
	/// trans[q]: list of (guard_string, next_state, edge_color)
	/// edge_color = -1 if no acceptance mark on this transition
	std::vector<std::vector<std::tuple<std::string,size_t,int>>> trans;
	/// Atomic proposition names, by AP index.
	std::vector<std::string> aps;
	/// controllable[i]: AP i is a system output.
	std::vector<bool> controllable;
	// Acceptance info
	int  n_colors  = 0;    // number of acceptance sets
	bool trans_acc = false; // true = transition-based acceptance
	/// Parity priority for each state (computed from color + acceptance type)
	/// Even priority = good for env (player 0); odd = good for sys (player 1).
	/// For trivial (all): priority 1 everywhere.
	/// For Büchi  Inf(0): color 0 → priority 1 elsewhere priority 0.
	/// For co-Büchi Fin(0): color 0 → priority 2 elsewhere priority 1.
	/// For parity: the color, normalized to the max-odd convention.
	std::vector<int> state_priority;
	/// Edge priorities: edge_priority[q][j] = priority of j-th trans from q
	std::vector<std::vector<int>> edge_priority;
	/// The acceptance is one of all, Buchi, co-Buchi or parity.
	bool acc_known = false;
	/// A run that sees no colour infinitely often is accepted.
	bool acc_accepts_uncolored = false;
	/// Some state or edge carries more than one colour; the priorities
	/// above read the first one only.
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

// ── HOA synthesis game parser ─────────────────────────────────────────────

/**
 * @brief Parse the HOA text of `ltlsynt --print-game-hoa` into a
 * `synth_game`.
 *
 * The text is read by `hoa_scan::scan`; this reads the header items of a
 * game (controllable APs, state players, acceptance) from the lines it
 * hands back, then derives state and edge priorities in the solver's
 * max-odd convention as described in the body comments.
 * @param hoa_text HOA text to parse.
 * @return The parsed game, or an error result: a decomposed multi-game
 * text, the `code::parse_error` of `hoa_scan::scan`, or a malformed game
 * header item.
 */
inline result<synth_game> parse_synth_game_hoa(const std::string& hoa_text) {
	result<synth_game> r;

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

	TAU_TRY(auto scanned, hoa_scan::scan(hoa_text, "the synthesis game HOA"));
	synth_game g;
	g.num_states = scanned.num_states;
	g.init = scanned.start;
	g.aps = std::move(scanned.aps);
	g.controllable.assign(g.aps.size(), false);
	g.player.assign(g.num_states, 0);
	g.state_color.assign(g.num_states, -1);
	g.state_priority.assign(g.num_states, 0);
	g.trans.resize(g.num_states);
	g.edge_priority.resize(g.num_states);

	bool is_buchi = false, is_cobuchi = false, is_all = false;
	bool is_parity = false, parity_min = false, parity_even = false;
	std::string acc_cond;   // the condition after the colour count
	auto malformed = [&r](const std::string& ln) -> result<synth_game> {
		return r.with_error(code::parse_error,
			"the synthesis game HOA has a malformed header item",
			{{label::value, truncate_for_message(ln)}});
	};
	// The words of the rest of a header line.
	auto words = [](hoa_scan::line& l) {
		std::vector<std::string_view> out;
		for (l.blanks(); !l.done(); l.blanks()) {
			size_t b = l.i;
			while (!l.done() && !hoa_scan::blank(l.s[l.i])) ++l.i;
			out.push_back(l.s.substr(b, l.i - b));
		}
		return out;
	};
	for (const auto& ln : scanned.other) {
		hoa_scan::line l{ ln };
		if (l.eat("controllable-AP:")) {
			auto idxs = l.nums();
			if (!idxs) return malformed(ln);
			for (long i : *idxs) {
				if (static_cast<size_t>(i) >= g.controllable.size())
					return r.with_error(code::parse_error,
						"the synthesis game HOA has a "
						"controllable AP index outside the "
						"AP table",
						{{label::value, truncate_for_message(ln)}});
				g.controllable[static_cast<size_t>(i)] = true;
			}
		} else if (l.eat("spot-state-player:")
			|| l.eat("spot.state-player:"))
		{
			// Spot 2.16 (--print-game-hoa) spells the header name with a
			// dash; the dotted form is accepted too for hand-written
			// fixtures. Getting this wrong silently leaves every state
			// env-owned.
			auto players = l.nums();
			if (!players) return malformed(ln);
			// A player list shorter than the state table is ordinary;
			// fill only the states it covers.
			for (size_t q = 0; q < players->size() && q < g.player.size(); ++q)
				g.player[q] = (*players)[q] != 0;
		} else if (l.eat("acc-name:")) {
			// by the exact name: generalized-Buchi and generalized-co-Buchi
			// contain the plain names, and any other condition (Streett,
			// Rabin, ...) is left unknown, so the game is refused
			auto ws = words(l);
			if (ws.empty()) return malformed(ln);
			if (ws[0] == "all") is_all = true;
			else if (ws[0] == "Buchi") is_buchi = true;
			else if (ws[0] == "co-Buchi") is_cobuchi = true;
			else if (ws[0] == "parity") {
				// LG-3: capture the flavor; normalized to max-odd (the
				// solver's convention) below.
				is_parity = true;
				parity_min = std::ranges::find(ws, "min") != ws.end();
				parity_even = std::ranges::find(ws, "even") != ws.end();
			}
		} else if (l.eat("Acceptance:")) {
			l.blanks();
			auto n = l.num();
			if (!n || *n < 0 || *n > INT_MAX) return malformed(ln);
			g.n_colors = static_cast<int>(*n);
			acc_cond = std::string(l.s.substr(l.i));
		} else if (l.eat("properties:")) {
			auto ws = words(l);
			if (std::ranges::find(ws, "trans-acc") != ws.end())
				g.trans_acc = true;
		}
	}

	// The first colour of a mark, -1 for none; a mark of several colours
	// marks the game multi-coloured.
	bool too_wide = false;
	auto colour = [&](const std::optional<std::vector<long>>& acc) -> int {
		if (!acc || acc->empty()) return -1;
		if (acc->size() > 1) g.multi_colored = true;
		if (acc->front() > INT_MAX) { too_wide = true; return -1; }
		return static_cast<int>(acc->front());
	};
	for (auto& st : scanned.states) {
		if (int c = colour(st.acc); c >= 0) g.state_color[st.n] = c;
		for (auto& ed : st.edges)
			g.trans[st.n].emplace_back(std::move(ed.guard), ed.dst,
				colour(ed.acc));
	}
	if (too_wide)
		return r.with_error(code::parse_error, "the synthesis game HOA "
			"has a colour beyond the integer range");

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
// For each env state (q, rho): env keeps an edge only when its guard admits
//   a D-pattern feasible from rho (the same T3 filter as sys edges).
//   New state: (q', rho) [rho unchanged].
//
// For trans-based acceptance: insert intermediate "color" state per edge.
// State i in intermediate layer: (q * T1_size + rho) + offset.

/// @brief Product of the synthesis game with the T_1 memory types (state
/// index q * T1_size + rho, plus edge stubs for transition acceptance).
struct product_game {
	/// Number of product states, edge stubs included.
	size_t n_states = 0;
	/// The initial product state, (G.init, init_rho).
	size_t init = 0;
	/// player[s]: 0 = env, 1 = sys.
	std::vector<int> player;
	std::vector<int> priority;      // state-based (after intermediate conversion)
	/// succs[s]: the successors of state s.
	std::vector<std::vector<size_t>> succs;
};

/// @brief Parse the disjunct index N from a `d_N` atomic-proposition name.
/// Returns -1 if @p ap is not a `d_` AP or N has more than nine digits.
inline int d_index_from_ap_name(const std::string& ap) {
	if (ap.size() <= 2 || ap[0] != 'd' || ap[1] != '_') return -1;
	// At most nine digits, so the index fits an int.
	if (ap.size() - 2 > 9) return -1;
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
// Neither alternative is right: ∃ρ₀ ("the system chooses its initial
// memory") is unsound — ρ is not a free bookkeeping state like
// a Mealy initial state but the type of an actual prior output, so choosing
// it asserts a phantom value (starkest for a point type {c_j}) that no
// first move can implement, and the interpreter then fails at its first
// enforced step; ∀ρ₀ is needlessly incomplete — it pessimises against
// initial memories that can never occur.
//
// This is one of the three t = 0 conventions in the codebase; the other two
// are the safety pipeline's "initial lookback values are 0" (see
// `atom_has_lookback` in ltl_aba_normalization.tmpl.h) and the LA-N3
// S auxiliary anchor `S(-1) = false` (see `compile_since_trigger_rec`
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
 * which downstream reads as UNREALIZABLE). A game whose acceptance is not
 * all, Buchi, co-Buchi or parity, or that carries several colours on one
 * state or edge, gives an empty product game (no state is won).
 * @param G Synthesis game.
 * @param T1_size |T_1|.
 * @param T3 Enumerated 3-types.
 * @param type_A D-bitmask per T3 type.
 * @param K Number of D propositions.
 * @param init_rho Initial memory, from `initial_memory()`.
 * @return The product game (empty for an unsupported acceptance), or an
 * error result when the game has more atomic propositions than the
 * assignment enumeration supports.
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
	// The assignment loops below shift `1 << n_aps`: a signed shift is
	// undefined at 31 and the enumeration is hopeless well before.
	if (n_aps > ltl_max_game_aps) {
		return r.with_error(code::unsupported_operation,
			"the product game has more atomic propositions than "
			"the assignment enumeration supports; refusing",
			{{label::limit, ltl_max_game_aps},
			 {label::value, std::to_string(n_aps)}});
	}
	// Each edge label, read once; bit `ap` of an assignment is AP `ap`.
	std::vector<std::vector<hoa_label::expr>> labels(G.trans.size());
	for (size_t q = 0; q < G.trans.size(); ++q)
		for (const auto& t : G.trans[q]) {
			auto lab = hoa_label::read(std::get<0>(t));
			if (!lab) return r.with_error(code::parse_error,
				"the synthesis game has an edge label that is not "
				"a label",
				{{label::value,
					truncate_for_message(std::get<0>(t))}});
			labels[q].push_back(std::move(*lab));
		}
	auto holds = [&](const hoa_label::expr& lab, int a) {
		return hoa_label::eval(lab, [&](size_t ap) {
			return ap < static_cast<size_t>(n_aps) && ((a >> ap) & 1);
		});
	};
	// Colours of a condition other than all, Buchi, co-Buchi or parity
	// (Streett, generalized Buchi, ...) are not priorities.
	if (G.multi_colored || (!G.acc_known && G.n_colors > 0))
		return r.with_value(product_game{});
	// A parsed game's colours sit two above the priority of a run that
	// sees none, as in build_data_arena. A game built by hand, with no
	// acceptance declared, keeps the priorities it was given.
	const bool lift = G.acc_known;
	const int uncolored = G.acc_accepts_uncolored ? 1 : 0;
	auto state_prio = [&](size_t q) {
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
	auto env_edge_reachable = [&](size_t rho, const hoa_label::expr& lab) {
		for (int a = 0; a < (1 << n_aps); ++a) {
			if (!holds(lab, a)) continue;
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
					// Env stubs are independent of the
					// assignment -- create them once from any
					// satisfying assignment instead of
					// re-testing the key 2^n_aps times.
					if (G.player[q] != 1) {
						// Same feasibility filter as
						// the transition site below.
						if (!env_edge_reachable(rho,
							labels[q][j])) continue;
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
						if (!holds(labels[q][j], a)) continue;
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
					// (env handled above)
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
						if (!holds(labels[q][j], a)) continue;

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
					if (!env_edge_reachable(rho, labels[q][j]))
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
 * PRECONDITION: output-only qlt atoms. Nothing below guards this --
 * input atoms would silently produce garbage (the env branch never models
 * input choice). Callers must check atom_has_any_input first, as both
 * current callers (qlt_try_propositional_synthesis and, through
 * solve_algorithm_d_full, qlt_semantic_pwr_optimal) do.
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
	/// Player 1 wins from the initial product state.
	bool realizable = false;
	std::set<size_t> winning_region; // W1 state indices in product game
	/// The product game the region is computed on.
	struct product_game product_game;
	/// The synthesis game from ltlsynt.
	struct synth_game synth_game;
	/// |T_1|, as passed in.
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
 * PRECONDITION: output-only qlt atoms; see solve_algorithm_d above.
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
