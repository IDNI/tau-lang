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
	table_step_provider(
		codegen::strategy strat,
		std::vector<std::pair<std::string, tref>> input_atoms,
		std::vector<std::string> flag_outputs,
		std::vector<std::vector<std::vector<std::pair<std::string, tref>>>>
			edge_witnesses = {},
		std::vector<std::vector<trefs>> edge_witness_templates = {},
		std::vector<std::vector<std::vector<bool>>>
			edge_witness_template_is_counter = {});

	std::optional<solution<node>> produce(
		const trefs& step_spec, const assignment<node>& memory,
		size_t time_point, size_t formula_time_point) override;

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
	// Fresh-element ledger for this run, scoped to one table_step_provider
	// execution -- not reset between produce() calls, so a committed
	// witness from an earlier step keeps its ledger identity later.
	fresh_element_ledger ledger_;
	int state_;
};

// Builds a table_step_provider from a solved LTL(ABA) strategy: carrier-
// typed output atoms keep a flag slot, data-typed ones become per-edge
// witness templates solved at runtime. Returns {provider, {lookback,
// highest_initial_pos}}; provider is nullptr when a flag atom isn't
// single-variable.
template <NodeType node>
std::pair<std::shared_ptr<table_step_provider<node>>, std::pair<int, int>>
make_table_provider(const ltl_aba_solution<node>& sol);

} // namespace idni::tau_lang

#include "table_step_provider.tmpl.h"

#endif // __IDNI__TAU__TABLE_STEP_PROVIDER_H__
