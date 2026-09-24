#!/usr/bin/env python3
"""
Test script checking the runtime budgets and engine switches the binding
exposes: every setter accepts a value, the tree-node budget refuses a call,
and the options an algebra declares about itself read back what was set.
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
	"set_cache_bound", "set_max_cover_products", "set_ltl_qe_max_vars",
	"set_ltl_hoa_max_states", "set_ltl_guard_max_cubes",
	"set_ltl_max_refinement_rounds", "set_ltl_window_max_paths",
	"set_ba_decision_pins",
)

# The shipped default of each count setter, restored after the round trip.
COUNT_DEFAULTS = {
	"set_lgrs_max_vars": 8, "set_max_fixpoint_steps": 500,
	"set_max_flag_search_steps": 500, "set_max_probe_steps": 10000,
	"set_gc_min_size": 256, "set_tref_budget_soft_percent": 75,
	"set_max_consistency_subsets": 4096, "set_cache_bound": 4096,
	"set_max_cover_products": 256, "set_ltl_hoa_max_states": 4194304,
	"set_ltl_guard_max_cubes": 512, "set_ltl_max_refinement_rounds": 64,
	"set_ltl_window_max_paths": 4096, "set_ba_decision_pins": 4096,
}

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
		assert setter(7) is None, name
		setter(COUNT_DEFAULTS.get(name, 0))
	for name, default in FLAG_SETTERS.items():
		getattr(tau, name)(not default)
		getattr(tau, name)(default)
	tau.set_gc_growth_factor(2.0)
	tau.set_gc_growth_factor(1.5)
	tau.set_ltl_timeout_sec(30)
	tau.set_ltl_timeout_sec(-1)
	tau.set_ltl_algorithm("B")
	tau.set_ltl_algorithm("")
	assert verdict(tau.sat(SPEC)) is True

def test_tref_budget_refuses_a_call():
	assert tau.tref_count() > 0
	tau.set_tref_budget(1)
	refused = tau.sat(SPEC)
	tau.set_tref_budget(0)
	assert not refused and refused.report.has_error, repr(refused)
	assert verdict(tau.sat(SPEC)) is True

def test_ba_options():
	names = tau.ba_option_names()
	assert all("-" in n for n in names), names
	for name in names:
		r = tau.get_ba_option(name)
		assert r and isinstance(r.value, int), f"{name}: {r!r}"
	unknown = tau.set_ba_option("nope-nothing", 1)
	assert not unknown and unknown.report.has_error, repr(unknown)
	assert not tau.get_ba_option("nope-nothing")
	if "bv-definitional-elimination" not in names:
		return
	assert verdict(tau.set_ba_option("bv-definitional-elimination", 0)) == 0
	assert verdict(tau.get_ba_option("bv-definitional-elimination")) == 0
	assert verdict(tau.set_ba_option("bv-definitional-elimination", 1)) == 1
	saved = verdict(tau.get_ba_option("bv-defelim-max-atoms"))
	assert verdict(tau.set_ba_option("bv-defelim-max-atoms", 5)) == 5
	assert verdict(tau.set_ba_option("bv-defelim-max-atoms", saved)) == saved

def main():
	test_every_setter_takes_a_value()
	test_tref_budget_refuses_a_call()
	test_ba_options()
	print("Test passed!")

if __name__ == "__main__":
	main()
