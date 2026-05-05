// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Mealy-machine extraction from an Algorithm D parity-game winning set (#7).
//
// Given the winning region W ⊆ T_1 (or T_1 × Q for product games) and a
// per-vertex witness map recording which τ witnessed the CPre inclusion at
// each step, build the concrete Mealy machine:
//   - states Q (from the DPA; or just {*} for GR(1) without DPA).
//   - input alphabet T_2.
//   - output alphabet T_3.
//   - transition δ : Q × T_2 → T_3 × Q mapping
//     (q, σ) ↦ (τ, δ_A(q, {i: δ_i ∈ τ})) where τ is the stored witness.
//
// This is the skeleton signature for Algorithm D's Phase 4.  The concrete
// witness-storage scheme needs to be supplied by the caller's fixpoint
// iteration (stash τ at each (b, q) when it first enters the winning set).

#ifndef __IDNI__TAU__MEALY_EXTRACT_H__
#define __IDNI__TAU__MEALY_EXTRACT_H__

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace idni::tau_lang::mealy {

// Edge of the extracted Mealy machine.
struct Edge {
	int from_state;       // vertex index (b, q) in the product game
	int input_sigma;      // T_2 index
	int output_tau;       // T_3 index
	int to_state;         // next vertex index
};

// Mealy machine: Q states, |T_2| inputs per state, |T_3| outputs per
// edge.  The initial_state corresponds to (b_0, q_0).
struct Mealy {
	int num_states = 0;
	int initial_state = 0;
	std::vector<Edge> edges;
};

// Extract a Mealy machine from a winning-strategy witness map.
// `winning_vertices` is the winning region (vertex indices); `witness`
// maps (vertex, σ) → τ for each σ ∈ T_2 at each winning vertex.
// `successor` computes the next vertex from (vertex, τ) using the game's
// transition function (sh(τ)|_m combined with δ_A(q, J(τ))).
template <class Successor>
inline Mealy extract_mealy(
    const std::vector<int>& winning_vertices,
    int initial_state,
    const std::map<std::pair<int,int>, int>& witness,
    int num_T2,
    Successor successor)
{
	Mealy m;
	m.initial_state = initial_state;
	// Assign dense state indices to winning vertices.
	std::unordered_map<int,int> vertex_to_state;
	for (size_t i = 0; i < winning_vertices.size(); ++i)
		vertex_to_state[winning_vertices[i]] = (int)i;
	m.num_states = (int)winning_vertices.size();
	// Emit edges.
	for (int v : winning_vertices) {
		int qs = vertex_to_state[v];
		for (int sigma = 0; sigma < num_T2; ++sigma) {
			auto it = witness.find({v, sigma});
			if (it == witness.end()) continue;
			int tau = it->second;
			int next_v = successor(v, tau);
			auto nit = vertex_to_state.find(next_v);
			if (nit == vertex_to_state.end()) continue;
			Edge e{qs, sigma, tau, nit->second};
			m.edges.push_back(e);
		}
	}
	return m;
}

} // namespace idni::tau_lang::mealy

#endif // __IDNI__TAU__MEALY_EXTRACT_H__
