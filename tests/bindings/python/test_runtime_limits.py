#!/usr/bin/env python3
"""
Test script checking the runtime budgets and engine switches the binding
exposes: every setter accepts a value, the tree-node budget refuses a call,
and every option reads back by name what was set.
"""

import tau_loader as tau

COUNT_SETTERS = (
	"set_block_max_splits", "set_block_max_rounds", "set_cqe_max_clauses",
	"set_lgrs_max_vars", "set_block_squeeze_cap", "set_max_fixpoint_steps",
	"set_max_flag_search_steps", "set_max_def_passes", "set_max_enum_steps",
	"set_max_probe_steps", "set_max_rewrite_rounds",
	"set_max_simplify_rounds", "set_gc_min_size", "set_tref_budget",
	"set_tref_budget_soft_percent", "set_spec_size_warn",
	"set_max_revision_alts", "set_max_consistency_subsets",
	"set_cache_bound", "set_max_cover_products", "set_max_constant_size",
	"set_ltl_qe_max_vars",
	"set_ltl_hoa_max_states", "set_ltl_guard_max_cubes",
	"set_ltl_max_refinement_rounds", "set_ltl_window_max_paths",
	"set_ltl_closed_regions_timeout", "set_ba_decision_pins",
	"set_ltl_data_game_max_nodes", "set_ltl_data_game_max_memo",
	"set_ltl_data_game_max_combinations", "set_ltl_max_observations",
	"set_ltl_mealy_max_states", "set_ltl_mealy_max_edges",
	"set_compile_max_table_edges", "set_compile_build_timeout",
	"set_bf_dependence_max_nodes", "set_max_blast_reentry_depth",
)

# The shipped default of each count setter, restored after the round trip.
COUNT_DEFAULTS = {
	"set_lgrs_max_vars": 8, "set_max_fixpoint_steps": 500,
	"set_max_flag_search_steps": 500, "set_max_probe_steps": 10000,
	"set_gc_min_size": 256, "set_tref_budget_soft_percent": 75,
	"set_max_consistency_subsets": 4096, "set_cache_bound": 4096,
	"set_max_cover_products": 256, "set_max_constant_size": 2000,
	"set_ltl_hoa_max_states": 4194304,
	"set_ltl_guard_max_cubes": 512, "set_ltl_max_refinement_rounds": 64,
	"set_ltl_window_max_paths": 4096, "set_ba_decision_pins": 4096,
	"set_ltl_closed_regions_timeout": 20,
	"set_ltl_data_game_max_nodes": 8388608,
	"set_ltl_data_game_max_memo": 33554432,
	"set_ltl_data_game_max_combinations": 4096,
	"set_ltl_max_observations": 8, "set_ltl_mealy_max_states": 4096,
	"set_ltl_mealy_max_edges": 65536, "set_compile_max_table_edges": 400,
	"set_compile_build_timeout": 3600,
	"set_bf_dependence_max_nodes": 65536,
}

# What a getter reads after its setter took the restore value, where that
# differs: the QE cap's 0 falls back to its default 2.
READ_BACK = { "set_ltl_qe_max_vars": 2 }

FLAG_SETTERS = {
	"set_preprocessing": True, "set_ba_component_factoring": True,
	"set_pwr_semantic_fallback": False,
	"set_step_definitional_propagation": True,
}

SPEC = "always o1[t] = i1[t]."

def verdict(r):
	assert r, f"no verdict: {r.report.errors}"
	return r.value

def test_every_setter_takes_a_value():
	for name in COUNT_SETTERS:
		setter = getattr(tau, name)
		getter = getattr(tau, "get_" + name[4:])
		assert setter(7) is None, name
		assert getter() == 7, name
		restore = COUNT_DEFAULTS.get(name, 0)
		setter(restore)
		assert getter() == READ_BACK.get(name, restore), name
	for name, default in FLAG_SETTERS.items():
		getattr(tau, name)(not default)
		getattr(tau, name)(default)
	tau.set_gc_growth_factor(2.0)
	assert tau.get_gc_growth_factor() == 2.0
	tau.set_gc_growth_factor(1.5)
	tau.set_ltl_timeout_sec(30)
	assert tau.get_ltl_timeout_sec() == 30
	tau.set_ltl_timeout_sec(-1)
	assert tau.get_ltl_timeout_sec() == 60
	tau.set_ltl_algorithm("B")
	assert tau.get_ltl_algorithm() == "B"
	tau.set_ltl_algorithm("")
	assert tau.get_ltl_algorithm() == "auto"
	assert verdict(tau.sat(SPEC)) is True

def test_tref_budget_refuses_a_call():
	assert tau.tref_count() > 0
	tau.set_tref_budget(1)
	refused = tau.sat(SPEC)
	tau.set_tref_budget(0)
	assert not refused and refused.report.has_error, repr(refused)
	assert verdict(tau.sat(SPEC)) is True

def test_nlang_text_options(names):
	if "nlang-model" not in names:
		return
	assert verdict(tau.set_option("nlang-model", "my-model-1")) \
		== "my-model-1"
	assert verdict(tau.get_option("nlang-model")) == "my-model-1"
	assert verdict(tau.set_option("nlang-endpoint",
		"http://localhost:8080/v1")) == "http://localhost:8080/v1"
	assert verdict(tau.set_option("nlang-provider", "anthropic")) \
		== "anthropic"
	refused = tau.set_option("nlang-provider", "nobody")
	assert not refused and refused.report.has_error, repr(refused)
	assert verdict(tau.get_option("nlang-provider")) == "anthropic"
	# the key is never read back
	assert verdict(tau.set_option("nlang-api-key", "sk-secret")) == "set"
	assert verdict(tau.get_option("nlang-api-key")) == "set"
	assert not tau.set_option("nlang-max-tokens", "x")
	assert verdict(tau.set_option("nlang-max-tokens", "512")) == "512"
	for name in ("nlang-model", "nlang-endpoint", "nlang-provider",
			"nlang-api-key"):
		assert tau.set_option(name, "")

def test_options():
	names = tau.option_names()
	assert "max-fixpoint-steps" in names, names
	for name in names:
		t = tau.get_option(name)
		assert isinstance(verdict(t), str), f"{name}: {t!r}"
	unknown = tau.set_option("nope-nothing", "x")
	assert not unknown and unknown.report.has_error, repr(unknown)
	assert not tau.get_option("nope-nothing")
	test_nlang_text_options(names)
	if "bv-definitional-elimination" not in names:
		return
	assert verdict(tau.set_option("bv-definitional-elimination", "off")) \
		== "false"
	assert verdict(tau.set_option("bv-definitional-elimination", "on")) \
		== "true"
	saved = verdict(tau.get_option("bv-defelim-max-atoms"))
	assert verdict(tau.set_option("bv-defelim-max-atoms", "5")) == "5"
	assert verdict(tau.set_option("bv-defelim-max-atoms", saved)) == saved

def main():
	test_every_setter_takes_a_value()
	test_tref_budget_refuses_a_call()
	test_options()
	print("Test passed!")

if __name__ == "__main__":
	main()
