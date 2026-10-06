#!/usr/bin/env python3
"""
The runtime limits and BA-declared options of the ctypes binding: every
limit the library names reads back what was set, an unknown name is
refused, and the limits that are not a count round-trip too.

Needs TAU_LTL_LIB naming libtau_ctypes and tau_lang.py on PYTHONPATH.
"""

import tau_lang as tau

def test_count_limits_round_trip():
	names = tau.limit_names()
	for expected in ("max_fixpoint_steps", "ltl_hoa_max_states",
			"ltl_mealy_max_states", "compile_max_table_edges",
			"bf_dependence_max_nodes", "ltl_max_observations"):
		assert expected in names, f"{expected} missing from {names}"
	for name in names:
		saved = tau.get_limit(name)
		tau.set_limit(name, 7)
		assert tau.get_limit(name) == 7, name
		tau.set_limit(name, saved)
		assert tau.get_limit(name) == saved, name
	# the per-name spelling of the nanobind module
	tau.set_max_fixpoint_steps(9)
	assert tau.get_max_fixpoint_steps() == 9
	tau.set_max_fixpoint_steps(500)
	assert tau.get_max_fixpoint_steps() == 500

def test_unknown_limit_is_refused():
	for call in (lambda: tau.set_limit("no_such_limit", 1),
			lambda: tau.get_limit("no_such_limit")):
		try:
			call()
		except KeyError:
			continue
		raise AssertionError("an unknown limit name was accepted")

def test_other_limits_round_trip():
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

def test_ba_options():
	names = tau.ba_option_names()
	assert all("-" in n for n in names), names
	for name in names:
		v = tau.get_ba_option(name)
		assert tau.set_ba_option(name, v) == v, name
	try:
		tau.get_ba_option("nope-nothing")
	except KeyError:
		pass
	else:
		raise AssertionError("an undeclared BA option was accepted")

def main():
	test_count_limits_round_trip()
	test_unknown_limit_is_refused()
	test_other_limits_round_trip()
	test_ba_options()
	print("Test passed!")

if __name__ == "__main__":
	main()
