// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// The runtime shape of a compiled Mealy strategy, and how to walk it.
//
// Dependency-free on purpose: no NodeType, no BA awareness, nothing beyond
// <cstdint>/<string>/<vector>. This is the data a generated program's step() function
// carries at runtime (and, for a table-driven/PWR-capable program, what
// revise() swaps in) — the same shape whether the source spec was
// propositional or PWR-revised, which is what lets one emitter walk one
// representation instead of six.
//
// Guard convention: an edge's `guard` has one entry per input AP, in the
// order the emitting program_desc lists its Inputs, followed by one entry
// per FLAG-kind output field, in the order program_desc lists its Outputs
// (a witness-kind output has no slot here — its value is a per-edge C++
// expression baked into source, not table data; see cpp_codegen.h's
// program_desc/edge_desc). `strategy_step` matches only the first
// `num_inputs` entries against the caller-supplied `ap[]` — the remaining
// entries are read by the caller, from the returned edge, to assign flag
// outputs. -1 = must be false, 0 = don't care, +1 = must be true (matching)
// or is-assigned (output slots).

#ifndef __IDNI__TAU__CODEGEN_STRATEGY_H__
#define __IDNI__TAU__CODEGEN_STRATEGY_H__

#include <cstdint>
#include <string>
#include <vector>

namespace idni::tau_lang::codegen {

/// @brief One outgoing transition from some state.
struct edge {
	/// Input match entries, then flag-output slots (see the file comment).
	std::vector<std::int8_t> guard;  // see file comment for the layout
	/// Index of the state the transition leads to.
	size_t dst = 0;
};

/// @brief A full Mealy strategy: which state to start in, and each state's edges.
struct strategy {
	/// Number of states; `edges` has one entry per state.
	size_t num_states = 0;
	/// Index of the state a run starts in.
	size_t initial_state = 0;
	/// Number of leading `guard` entries that are matched against `ap[]`;
	/// the rest are flag-output assignment slots. Needed because `edge`
	/// carries both kinds of entry in one array and `strategy_step` cannot
	/// otherwise tell them apart without naming the source spec's I/O split.
	size_t num_inputs = 0;
	std::vector<std::vector<edge>> edges;  // edges[src] = outgoing edges
	/// Atomic-proposition names in guard order (inputs, then flag outputs).
	/// Empty means "unset"; revise() only compares this against the
	/// program's own aps when it is non-empty.
	std::vector<std::string> aps;
};

/**
 * @brief The first outgoing edge of @p src whose input guard entries are all
 * consistent with @p ap.
 *
 * Only the leading `min(s.num_inputs, guard.size())` entries are compared
 * (0 = don't care, so a shorter or all-zero guard always matches); edges are
 * tried in their stored order.
 * @param s The strategy to step.
 * @param src The current state index.
 * @param ap Input valuation in guard order; at least `s.num_inputs` valid
 * entries.
 * @return The matching edge, or nullptr when @p src is out of range or no
 * edge matches. A complete synthesized strategy always matches; a revised
 * (PWR) one is only checked by `revise()`'s asserts, not proven complete.
 */
inline const edge* strategy_step(const strategy& s, size_t src, const bool* ap) {
	if (src >= s.edges.size()) return nullptr;
	for (const auto& e : s.edges[src]) {
		bool match = true;
		const size_t n = s.num_inputs < e.guard.size()
			? s.num_inputs : e.guard.size();
		for (size_t i = 0; i < n; ++i) {
			if (e.guard[i] == 1 && !ap[i]) { match = false; break; }
			if (e.guard[i] == -1 && ap[i]) { match = false; break; }
		}
		if (match) return &e;
	}
	return nullptr;
}

} // namespace idni::tau_lang::codegen

#endif // __IDNI__TAU__CODEGEN_STRATEGY_H__
