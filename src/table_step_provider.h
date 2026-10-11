// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// The interpreter-side step_provider that walks a baked codegen::strategy
// instead of re-solving -- the runtime counterpart of compile_spec's
// emitted artifact main, driven through the full interpreter.
// Codegen-side: core (interpreter.h) never includes this header.

#ifndef __IDNI__TAU__TABLE_STEP_PROVIDER_H__
#define __IDNI__TAU__TABLE_STEP_PROVIDER_H__

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "codegen_strategy.h"
#include "cpp_codegen.h"
#include "interpreter.h"

namespace idni::tau_lang {

/**
 * @brief Selects each step's solution by matching the input guard against a
 * baked codegen::strategy instead of re-solving.
 *
 * Flag outputs are read off the matched edge; witness/data outputs use
 * edge_witnesses' precomputed value, or are solved per step from
 * edge_witness_templates when the value depends on the step's own inputs.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct table_step_provider : step_provider<node> {
	/// @brief The Mealy view of a strategy of the data game (ltl_aba_solution::
	/// data_game): every atom is read at the step played, from step 0 on,
	/// `history` (a conjunction of atoms over the steps before 0) is solved
	/// for the values the strategy starts from, and a step reads only the
	/// inputs of the atoms its state's guards compare.
	struct from_start {
		/// Conjuncts over the steps before 0, solved at the first step.
		trefs history;
		/// Past steps the strategy reads (reported by `lookback()`).
		int_t lookback = 0;
	};

	/**
	 * @brief Build the provider; every tree is held GC-rooted.
	 * @param strat The baked strategy; play starts at its initial state.
	 * @param input_atoms (name, atom template) per guard slot, evaluated
	 *        each step.
	 * @param flag_outputs One output variable name per flag-output guard slot.
	 * @param edge_witnesses `[s][e]`: precomputed (name, value) pairs for
	 *        that edge.
	 * @param edge_witness_templates `[s][e]`: atom conjuncts solved per step
	 *        when the output value depends on the step's own inputs.
	 * @param edge_witness_template_is_counter `[s][e]`: parallel to
	 *        edge_witness_templates[s][e], true where that template atom is
	 *        a hoisted positional atom's step-counter relativization --
	 *        produce() grounds it at the counter's own absolute step
	 *        (time_point) instead of the lookback-shifted formula_time_point
	 *        every other template atom uses. Empty (the default) means none
	 *        are.
	 * @param step_guard_ks One threshold k per "__step_ge<k>" guard prop,
	 *        matched like an extra input since its value (time_point >= k)
	 *        is a function of the step, never a free choice.
	 * @param start Set for the from_start (data game) mode; empty otherwise.
	 */
	table_step_provider(
		codegen::strategy strat,
		std::vector<std::pair<std::string, tref>> input_atoms,
		std::vector<std::string> flag_outputs,
		std::vector<std::vector<std::vector<std::pair<std::string, tref>>>>
			edge_witnesses = {},
		std::vector<std::vector<trefs>> edge_witness_templates = {},
		std::vector<std::vector<std::vector<bool>>>
			edge_witness_template_is_counter = {},
		std::vector<int_t> step_guard_ks = {},
		std::optional<from_start> start = std::nullopt);

	/**
	 * @brief Play one step of the strategy.
	 *
	 * Evaluates the guard atoms against @p memory (in the from_start mode
	 * only those the current state's guards compare, at @p time_point, after
	 * solving the values before step 0 once), takes the matching edge, fills
	 * the flag outputs from its guard and the data outputs from its baked
	 * witnesses or by solving its witness templates, and moves to the edge's
	 * destination state.
	 * @param step_spec Unused: the strategy replaces the spec.
	 * @param memory Committed memory so far this step (inputs merged in).
	 * @param time_point Current execution time point.
	 * @param formula_time_point Time point the running formula is phrased at.
	 * @return The step's solution; nullopt when no edge of the current state
	 *         matches the inputs; an error when a guard atom is undecided or
	 *         no witness satisfies the matched edge.
	 */
	result<std::optional<solution<node>>> produce(
		const trefs& step_spec, const assignment<node>& memory,
		int_t time_point, int_t formula_time_point) override;

	/**
	 * @brief The inputs read at the current state.
	 * @param vars The inputs of the step, one io_var each.
	 * @return In the from_start mode, the members of @p vars whose stream an
	 *         atom of the current state's guards or witness templates reads
	 *         at the current step; nullopt otherwise.
	 */
	std::optional<trefs> read_set(const trefs& vars) const override;
	/// @brief The current strategy state in the from_start mode, nullopt
	/// otherwise.
	std::optional<size_t> strategy_state() const override;
	/// @brief The from_start lookback, 0 otherwise.
	int_t lookback() const override;
	/// @brief Return to the initial state and drop the values before step
	/// 0, the values found so far and the fresh-element ledger.
	void reset() override;

	/**
	 * @brief Every atom this table strategy may consult this run.
	 *
	 * The input guards (input_atoms_, evaluated every step to route edges;
	 * in the from_start mode only those the state's guards compare) plus
	 * every edge's witness-template atoms across every state. Callers
	 * building a table interpreter (emit_main's emitted code,
	 * make_table_provider's in-process callers) pass this to
	 * make_table_interpreter's live_probe_atoms parameter so step()'s input
	 * filter can tell, per step, which declared inputs the strategy actually
	 * needs -- the same substitute-and-simplify test appear_within_lookback
	 * runs against ubt_ctn for the general solve path. A superset across
	 * all states is fine: appear_within_lookback only ever grows its
	 * "appeared" set, so including an atom from a state not currently active
	 * can only keep an input requested longer than strictly necessary, never
	 * drop one that is actually needed.
	 * @return The atoms, input guards first; may repeat an atom.
	 */
	trefs live_probe_atoms() const;

private:
	// Held as htref, not tref: a provider can sit idle before its
	// interpreter exists, and bintree<node>::gc() sweeps the whole shared
	// tree store, so these must stay GC-rooted independently (tree<node>::geth).
	codegen::strategy strat_;
	std::vector<std::pair<std::string, htref>> input_atoms_;
	std::vector<std::string> flag_outputs_;
	std::vector<std::vector<std::vector<std::pair<std::string, htref>>>> edge_witnesses_;
	std::vector<std::vector<std::vector<htref>>> edge_witness_templates_;
	// Per (state, edge, template index): true when that template atom is a
	// hoisted positional atom's step-counter relativization (see the
	// constructor's doc comment).
	std::vector<std::vector<std::vector<bool>>> edge_witness_template_is_counter_;
	// Per (state, edge): true when every template atom is a bf_neq
	// disequality over an atomless-typed BA -- eligible for the direct
	// atomless decode instead of a full solve. Structural, so computed
	// once at construction and reused every step.
	std::vector<std::vector<bool>> edge_direct_decode_eligible_;
	// Parallel to the guard's step-guard slots (see the constructor's doc
	// comment): step_guard_ks_[j] is the k that slot's live truth value
	// (time_point >= k) is computed against, every produce() call.
	std::vector<int_t> step_guard_ks_;
	// Fresh-element ledger for this run, scoped to one table_step_provider
	// execution -- not reset between produce() calls, so a committed
	// witness from an earlier step keeps its ledger identity later.
	fresh_element_ledger ledger_;
	size_t state_;
	// Set in the from_start mode.
	bool from_start_ = false;
	std::vector<htref> history_;
	int_t lookback_ = 0;
	// The values before step 0, solved from history_ at the first step,
	// held as htrefs for the reason above.
	std::vector<std::pair<htref, htref>> before_;
	bool before_ready_ = false;
	// Values chosen at earlier steps (solve_equality_cube).
	std::vector<htref> found_;
};

/**
 * @brief Build a table_step_provider from a solved LTL(ABA) strategy, the
 * one playable_table_solution(sol) returns.
 *
 * Carrier-typed output atoms whose truth decides their variable keep a flag
 * slot; data-typed ones, and carrier ones that do not decide their
 * variable, become per-edge witness templates solved at runtime. A data
 * game solution yields a provider in the from_start mode.
 * @tparam node Tree node type.
 * @param sol The solved strategy.
 * @return {provider, {lookback, highest_initial_pos}}; an error when
 *         playable_table_solution refuses the solution or a carrier-typed
 *         flag atom is not over a single variable.
 */
template <NodeType node>
result<std::pair<std::shared_ptr<table_step_provider<node>>,
	std::pair<int, int>>>
make_table_provider(const ltl_aba_solution<node>& sol);

} // namespace idni::tau_lang

#include "table_step_provider.tmpl.h"

#endif // __IDNI__TAU__TABLE_STEP_PROVIDER_H__
