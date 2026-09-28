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

// Selects each step's solution by matching the input guard against a
// baked codegen::strategy instead of re-solving. Flag outputs are read off
// the matched edge; witness/data outputs use edge_witnesses' precomputed
// value, or are solved per step from edge_witness_templates when the value
// depends on the step's own inputs.
template <NodeType node>
struct table_step_provider : step_provider<node> {
	// The Mealy view of a strategy of the data game (ltl_aba_solution::
	// data_game): every atom is read at the step played, from step 0 on,
	// `history` (a conjunction of atoms over the steps before 0) is solved
	// for the values the strategy starts from, and a step reads only the
	// inputs of the atoms its state's guards compare.
	struct from_start {
		trefs history;
		int_t lookback = 0;
	};

	// input_atoms: (name, atom template) per guard slot, evaluated each step.
	// flag_outputs: one output variable name per flag-output guard slot.
	// edge_witnesses[s][e]: precomputed (name, value) pairs for that edge.
	// edge_witness_templates[s][e]: atom conjuncts solved per step when the
	// output value depends on the step's own inputs.
	// edge_witness_template_is_counter[s][e]: parallel to
	// edge_witness_templates[s][e], true where that template atom is a
	// hoisted positional atom's step-counter relativization -- produce()
	// grounds it at the counter's own absolute step (time_point) instead of
	// the lookback-shifted formula_time_point every other template atom
	// uses. Empty (the default) means none are -- every atom grounds at
	// formula_time_point as before.
	// step_guard_ks: one threshold k per "__step_ge<k>" guard prop, matched
	// like an extra input since its value (time_point >= k) is a function
	// of the step, never a free choice.
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

	result<std::optional<solution<node>>> produce(
		const trefs& step_spec, const assignment<node>& memory,
		size_t time_point, size_t formula_time_point) override;

	std::optional<trefs> read_set(const trefs& vars) const override;
	std::optional<int> strategy_state() const override;
	int_t lookback() const override;
	void reset() override;

	// Every atom this table strategy may consult this run: input guards
	// (input_atoms_, evaluated unconditionally every step to route edges)
	// plus every edge's witness-template atoms across every state. Callers
	// building a table interpreter (emit_main's emitted code, make_table_
	// provider's in-process callers) pass this to make_table_interpreter's
	// live_probe_atoms parameter so step()'s input filter can tell, per
	// step, which declared inputs the strategy actually needs -- the same
	// substitute-and-simplify test appear_within_lookback runs against
	// ubt_ctn for the general solve path. A superset across all states is
	// fine: appear_within_lookback only ever grows its "appeared" set, so
	// including an atom from a state not currently active can only keep an
	// input requested longer than strictly necessary, never drop one that
	// is actually needed.
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
	int state_;
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

// Builds a table_step_provider from a solved LTL(ABA) strategy, the one
// playable_table_solution(sol) returns: carrier-typed output atoms keep a
// flag slot, data-typed ones become per-edge witness templates solved at
// runtime. Returns {provider, {lookback, highest_initial_pos}}; the report
// carries an error when playable_table_solution refuses the solution or a
// flag atom is not over a single variable.
template <NodeType node>
result<std::pair<std::shared_ptr<table_step_provider<node>>,
	std::pair<int, int>>>
make_table_provider(const ltl_aba_solution<node>& sol);

} // namespace idni::tau_lang

#include "table_step_provider.tmpl.h"

#endif // __IDNI__TAU__TABLE_STEP_PROVIDER_H__
