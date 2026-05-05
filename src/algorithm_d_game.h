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

} // namespace hoa_guard

inline bool eval_guard(const std::string& guard, int bitmask, int n_aps) {
	size_t i = 0;
	return hoa_guard::eval(guard, i, bitmask, n_aps);
}

// ── HOA synthesis game parser ─────────────────────────────────────────────

inline SynthGame parse_synth_game_hoa(const std::string& hoa_text) {
	SynthGame g;
	std::istringstream ss(hoa_text);
	std::string line;
	bool in_body = false;
	int cur_state = -1;
	bool is_buchi = false, is_cobuchi = false, is_all = false;

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
			} else if (line.substr(0,17) == "controllable-AP:") {
				auto idxs = parse_int_list(line.substr(17));
				for (int i : idxs) if (i < (int)g.controllable.size()) g.controllable[i] = true;
			} else if (line.substr(0,17) == "spot-state-player") {
				// "spot-state-player: 0 1 0 1 ..."
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
				// else: Streett or parity acceptance (handled via n_colors)
			} else if (line.substr(0,11) == "Acceptance:") {
				std::istringstream al(line.substr(11));
				al >> g.n_colors;
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
	//   is_cobuchi Fin(0): color 0 → priority 0, else priority 1
	//   parity/Streett: color → priority directly
	for (int q = 0; q < g.num_states; ++q) {
		int c = g.state_color[q];
		if (is_all)
			g.state_priority[q] = 1;
		else if (is_buchi)
			g.state_priority[q] = (c == 0) ? 1 : 0;
		else if (is_cobuchi)
			g.state_priority[q] = (c == 0) ? 0 : 1;
		else
			g.state_priority[q] = (c >= 0) ? c : 0;  // general parity
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
				g.edge_priority[q][j] = (ec == 0) ? 0 : 1;
			} else {
				g.edge_priority[q][j] = ec;
			}
		}
	}
	return g;
}

// ── Call ltlsynt and get parity game ──────────────────────────────────────

inline SynthGame call_ltlsynt_game(
	const std::string& phi_prop,
	const std::vector<std::string>& ins,
	const std::vector<std::string>& outs)
{
	// Cache: avoid re-running ltlsynt on identical (formula, ins, outs).
	struct game_cache_key {
		std::string formula;
		std::vector<std::string> ins, outs;
		bool operator==(const game_cache_key& o) const {
			return formula == o.formula && ins == o.ins && outs == o.outs;
		}
	};
	struct game_cache_hash {
		size_t operator()(const game_cache_key& k) const {
			size_t h = std::hash<std::string>{}(k.formula);
			for (auto& s : k.ins)  h ^= std::hash<std::string>{}(s) + 0x9e3779b9 + (h << 6) + (h >> 2);
			for (auto& s : k.outs) h ^= std::hash<std::string>{}(s) + 0x517cc1b7 + (h << 6) + (h >> 2);
			return h;
		}
	};
	static std::unordered_map<game_cache_key, SynthGame, game_cache_hash> cache;
	game_cache_key key{phi_prop, ins, outs};
	if (auto it = cache.find(key); it != cache.end()) return it->second;

	// Shell-escape
	std::string esc;
	for (char c : phi_prop) {
		if (c == '"' || c == '\\' || c == '$' || c == '`') esc += '\\';
		esc += c;
	}

	// Configurable timeout (same env var as call_ltlsynt).
	std::string prefix;
	{
		const char* env_sec = std::getenv("TAU_LTL_TIMEOUT_SEC");
		int sec = env_sec ? std::atoi(env_sec) : 60;
		if (sec > 0) prefix = "timeout " + std::to_string(sec) + "s ";
	}

	std::string cmd = prefix + "ltlsynt --formula=\"" + esc + "\"";
	if (!ins.empty()) {
		cmd += " --ins=\"";
		for (int i = 0; i < (int)ins.size(); ++i) {
			if (i) cmd += ",";
			cmd += ins[i];
		}
		cmd += "\"";
	}
	if (!outs.empty()) {
		cmd += " --outs=\"";
		for (int i = 0; i < (int)outs.size(); ++i) {
			if (i) cmd += ",";
			cmd += outs[i];
		}
		cmd += "\"";
	}
	cmd += " --print-game-hoa 2>/dev/null";

	FILE* fp = popen(cmd.c_str(), "r");
	if (!fp) return {}; // transient error — don't cache
	std::string hoa;
	char buf[512];
	while (fgets(buf, sizeof(buf), fp)) hoa += buf;
	int status = pclose(fp);
	int exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
	// Don't cache timeout (124), not-found (127), or other transient errors —
	// output may be partial/truncated even when non-empty.
	// ltlsynt exits 0 for REALIZABLE, 1 for UNREALIZABLE — both are definitive.
	if (exit_code != 0 && exit_code != 1) return {};
	if (hoa.empty()) { cache[key] = {}; return {}; }
	auto result = parse_synth_game_hoa(hoa);
	cache[key] = result;
	return result;
}

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

inline int d_index_from_ap_name(const std::string& ap) {
	if (ap.size() <= 2 || ap[0] != 'd' || ap[1] != '_') return -1;
	int idx = 0;
	for (size_t i = 2; i < ap.size(); ++i) {
		if (!std::isdigit(static_cast<unsigned char>(ap[i]))) return -1;
		idx = idx * 10 + (ap[i] - '0');
	}
	return idx;
}

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

inline ProductGame build_product_game(
	const SynthGame& G,
	int T1_size,
	const std::vector<omcat::QltType3>& T3,
	const std::vector<int>& type_A,   // D-bitmask per T3 type
	int K)                             // number of D propositions
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
					// For each AP assignment satisfying guard → stub
					for (int a = 0; a < (1 << n_aps); ++a) {
						if (!eval_guard(guard, a, n_aps)) continue;
						if (G.player[q] == 1) {
							// Sys: picks D_pattern by AP name, picks rho'.
							int D_pat = d_pattern_from_assignment(G, a, K);
							for (int rp = 0; rp < T1_size; ++rp) {
								if (!feasible[rho][rp][D_pat]) continue;
								auto key = std::make_tuple(q * T1_size + rho, j, rp);
								if (stub_map.find(key) == stub_map.end()) {
									stub_map[key] = stub_base + (int)stubs.size();
									stubs.push_back({q, rho, j, next_q, rp, ep, 0});
								}
							}
						} else {
							// Env: rho unchanged
							auto key = std::make_tuple(q * T1_size + rho, j, rho);
							if (stub_map.find(key) == stub_map.end()) {
								stub_map[key] = stub_base + (int)stubs.size();
								stubs.push_back({q, rho, j, next_q, rho, ep, 0});
							}
						}
					}
				}
			}
		}
	}

	ProductGame pg;
	pg.n_states = base_n + (int)stubs.size();
	pg.init = G.init * T1_size + 0; // initial memory = T1 type 0 (below all constants)
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
				// Sys: enumerate all AP assignments, filter controllable ones
				for (int a = 0; a < (1 << n_aps); ++a) {
					// Build D_pattern from controllable d_i APs by name.
					int D_pat = d_pattern_from_assignment(G, a, K);

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
				// Env (player 0): rho unchanged.  Env observes (but
				// does not control) output APs, so a transition is
				// reachable if its guard is satisfiable under ANY
				// AP assignment.
				for (int j = 0; j < (int)G.trans[q].size(); ++j) {
					const auto& [guard, nq, ec] = G.trans[q][j];
					bool reachable = false;
					for (int a = 0; a < (1 << n_aps) && !reachable; ++a)
						reachable = eval_guard(guard, a, n_aps);
					if (!reachable) continue;
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

	// States with no successors in V: player p loses (stuck)
	// Add self-loops only for analysis (Zielonka's algorithm handles dead ends)

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
	StateSet W_1b_full = Y;
	for (int u : Wl) W_1b_full.insert(u);
	if (beneficiary == 1) {
		for (int u : W1pp) W_1b_full.insert(u);
		return {W0pp, W_1b_full};
	} else {
		for (int u : W0pp) W_1b_full.insert(u);
		return {W_1b_full, W1pp};
	}
}

} // namespace zielonka_impl

// Returns the set of states where player 1 (sys) wins.
inline std::set<int> zielonka_win_player1(const ProductGame& pg) {
	std::set<int> V;
	for (int s = 0; s < pg.n_states; ++s) V.insert(s);
	auto [W0, W1] = zielonka_impl::solve(V, pg.n_states, pg.player, pg.priority, pg.succs);
	return W1;
}

// ── Main Algorithm D entry point ──────────────────────────────────────────

// Returns true if the formula is REALIZABLE via Algorithm D.
// phi_star: propositional LTL with D_0,...,D_{K-1} as output propositions.
// T1_size: |T_1|.
// T3: enumerated 3-types.
// type_A: D_pattern bitmask for each T3 type.
// K: number of D propositions.
inline bool solve_algorithm_d(
	const std::string& phi_star,
	int T1_size,
	const std::vector<omcat::QltType3>& T3,
	const std::vector<int>& type_A,
	int K)
{
	if (phi_star.empty() || T1_size <= 0) return false;

	// Build list of D propositions as output
	std::vector<std::string> D_outs;
	for (int i = 0; i < K; ++i) D_outs.push_back("d_" + std::to_string(i));

	// Get synthesis parity game for φ*(D_i)
	SynthGame G = call_ltlsynt_game(phi_star, {}, D_outs);
	if (G.num_states == 0) return false;

	// Build product game (G × T_1)
	ProductGame pg = build_product_game(G, T1_size, T3, type_A, K);
	if (pg.n_states == 0) return false;

	// Solve parity game with Zielonka
	auto W1 = zielonka_win_player1(pg);

	// REALIZABLE iff ∃ initial T_1 memory type ρ_0 such that player 1 wins
	// from (G.init, ρ_0).  Since sys chooses its initial memory, take ∃.
	for (int rho0 = 0; rho0 < T1_size; ++rho0) {
		int s0 = G.init * T1_size + rho0;
		if (W1.count(s0)) return true;
	}
	return false;
}

// ── Extended Algorithm D: returns winning region for semantic PWR ──────────

struct AlgDResult {
	bool realizable = false;
	std::set<int> winning_region;     // W1 state indices in product game
	ProductGame product_game;
	SynthGame synth_game;
	int T1_size = 0;
	int K = 0;                        // number of D propositions
	int init_rho = -1;                // winning initial ρ₀ (-1 if unrealizable)
};

inline AlgDResult solve_algorithm_d_full(
	const std::string& phi_star,
	int T1_size,
	const std::vector<omcat::QltType3>& T3,
	const std::vector<int>& type_A,
	int K)
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
		result.synth_game, T1_size, T3, type_A, K);
	if (result.product_game.n_states == 0) return result;

	result.winning_region = zielonka_win_player1(result.product_game);

	for (int rho0 = 0; rho0 < T1_size; ++rho0) {
		int s0 = result.synth_game.init * T1_size + rho0;
		if (result.winning_region.count(s0)) {
			result.realizable = true;
			result.init_rho = rho0;
			break;
		}
	}
	return result;
}

} // namespace idni::tau_lang::alg_d

#endif // __IDNI__TAU__ALGORITHM_D_GAME_H__
