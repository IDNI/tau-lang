// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ltl_aba_result.h
 * @brief What a BA answers when asked to synthesise an LTL formula itself.
 *
 * Separate from ltl_aba.h so a BA plugin can include it: ltl_aba.h reaches
 * normalizer.h, which is not available where a BA header enters through the
 * generated pack.
 *
 * Include tau_tree.h before this file. It deliberately includes nothing of its
 * own beyond the standard library, because it is reachable from inside
 * tau_tree.h's own include chain, where an include guard it had already set
 * would leave its declarations missing.
 */

#ifndef __IDNI__TAU__LTL_ABA_RESULT_H__
#define __IDNI__TAU__LTL_ABA_RESULT_H__

#include <optional>
#include <string>
#include <vector>

namespace idni::tau_lang {

/** @brief One transition of a strategy automaton as Spot's HOA spells it. */
struct HoaEdge {
	std::string guard_label;  // Boolean formula over AP indices, e.g. "0&!1"
	int dst = 0;
	bool accepting = false;   // true if this edge carries an acceptance mark
};

/** @brief A strategy automaton parsed out of Spot's HOA output. */
struct HoaAutomaton {
	int num_states = 0;
	int initial_state = 0;
	std::vector<std::string> aps;       // atomic proposition names
	std::vector<std::vector<HoaEdge>> edges; // edges[src] = outgoing edges
	std::vector<bool> state_accepting;  // true if state has acceptance mark
};

HoaAutomaton parse_hoa(const std::string& hoa_text);

/** @brief Defined in ltl_aba_normalization.tmpl.h; incomplete is enough here. */
template <NodeType node>
struct LtlAbaSolution;

/**
 * @brief A BA's answer to try_propositional_synthesis.
 *
 * The outer level says whether a BA claimed the formula at all, the inner
 * whether it is realizable. Both are needed: declining sends core on to the
 * ABA-oracle path, while an unrealizable verdict is final, and one optional
 * would make those the same answer.
 */
template <NodeType node>
using propositional_synthesis =
	std::optional<std::optional<LtlAbaSolution<node>>>;

/** @brief The formula is not this BA's; core falls through to the ABA oracle. */
template <NodeType node>
propositional_synthesis<node> synthesis_declined() { return std::nullopt; }

/** @brief The formula is this BA's and it has no strategy. */
template <NodeType node>
propositional_synthesis<node> synthesis_unrealizable() {
	return propositional_synthesis<node>{
		std::optional<LtlAbaSolution<node>>{}};
}

/** @brief The formula is this BA's and @p sol is a strategy for it. */
template <NodeType node>
propositional_synthesis<node> synthesis_solved(const LtlAbaSolution<node>& sol) {
	return propositional_synthesis<node>{sol};
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__LTL_ABA_RESULT_H__
