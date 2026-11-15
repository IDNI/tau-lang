#!/usr/bin/env python3
"""
The runtime limits and the options of the ctypes binding: every limit the
library names reads back what was set, an unknown name is refused, the
limits that are not a count round-trip too, and every option is set and read
by its command-line name, a bad name or text refused.

Needs TAU_LTL_LIB naming libtau_ctypes and tau_lang.py on PYTHONPATH.
"""

import tau_lang as tau

def test_count_limits_round_trip():
	names = tau.limit_names()
	for expected in ("max_fixpoint_steps", "ltl_hoa_max_states",
			"ltl_mealy_max_states", "compile_max_table_edges",
			"bf_dependence_max_nodes", "ltl_max_observations",
			"ltl_strategy_max_states"):
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

def test_options_round_trip():
	names = tau.option_names()
	for expected in ("max-fixpoint-steps", "ltl-parameters", "ba-complement"):
		assert expected in names, f"{expected} missing from {names}"
	assert tau.set_option("max-fixpoint-steps", "9") == "9"
	assert tau.get_limit("max_fixpoint_steps") == 9
	assert tau.set_option("max-fixpoint-steps", "500") == "500"
	for name, text in (("ltl-parameters", "universal"),
			("ba-complement", "trace")):
		saved = tau.get_option(name)
		assert tau.set_option(name, text) == text, name
		assert tau.set_option(name, saved) == saved, name
	if "bv-bitblast-qf-max-nodes" in names:
		saved = tau.get_option("bv-bitblast-qf-max-nodes")
		assert tau.set_option("bv-bitblast-qf-max-nodes", "0") == "0"
		assert tau.set_option("bv-bitblast-qf-max-nodes", saved) == saved

def test_bad_options_are_refused():
	try:
		tau.get_option("no-such-option")
	except KeyError:
		pass
	else:
		raise AssertionError("an unknown option name was read")
	for name, text in (("no-such-option", "1"),
			("ba-complement", "no-such-reading")):
		try:
			tau.set_option(name, text)
		except ValueError as e:
			assert str(e), name
			continue
		raise AssertionError(f"{name} = {text} was accepted")
	assert tau.get_option("ba-complement") == "program"

def main():
	test_count_limits_round_trip()
	test_unknown_limit_is_refused()
	test_other_limits_round_trip()
	test_options_round_trip()
	test_bad_options_are_refused()
	print("Test passed!")

if __name__ == "__main__":
	main()
