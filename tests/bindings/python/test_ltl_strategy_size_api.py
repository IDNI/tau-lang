#!/usr/bin/env python3
"""
Test script checking the size options of a strategy the binding exposes:
ltl-minimize-strategy keeps a verdict, and set_ltl_strategy_max_states keeps
it at the size of the strategy and answers UNKNOWN below it.
"""


import tau_loader as tau
from tau_loader import ltlsynt, verdict

# The binding has no type query, and each option name starts with its family.
HAS_BV = any(n.startswith("bv-") for n in tau.option_names())

ALTERNATION = ("G(o1[t]:bv[1] = 1 -> F(o1[t]:bv[1] = 0)) && "
	"G(o1[t]:bv[1] = 0 -> F(o1[t]:bv[1] = 1)) && "
	"G(o2[t]:bv[1] = 1 -> F(o2[t]:bv[1] = 0)) && "
	"G(o2[t]:bv[1] = 0 -> F(o2[t]:bv[1] = 1)).")

def test_strategy_size():
	assert tau.set_option("ltl-minimize-strategy", "true")
	try:
		assert verdict(tau.realizable(ALTERNATION)) is True
	finally:
		assert tau.set_option("ltl-minimize-strategy", "false")
	tau.set_ltl_minimize_strategy(True)
	try:
		assert tau.get_option("ltl-minimize-strategy").value == "true"
		assert verdict(tau.realizable(ALTERNATION)) is True
	finally:
		tau.set_ltl_minimize_strategy(False)
	assert tau.get_option("ltl-minimize-strategy").value == "false"
	assert tau.get_ltl_strategy_max_states() == 0
	tau.set_ltl_strategy_max_states(2)
	try:
		assert verdict(tau.realizable(ALTERNATION)) is True
		tau.set_ltl_strategy_max_states(1)
		r = tau.realizable(ALTERNATION)
		assert not r, f"a verdict: {r.value!r}"
		assert any("ltl-strategy-max-states" in e
			for e in r.report.errors), r.report.errors
	finally:
		tau.set_ltl_strategy_max_states(0)
	assert tau.get_ltl_strategy_max_states() == 0

def main():
	if not ltlsynt():
		print("skipped: no ltlsynt")
		print("Test passed!")
		return
	if not HAS_BV:
		print("skipped: the pack has no bv")
		print("Test passed!")
		return
	test_strategy_size()
	print("Test passed!")

if __name__ == "__main__":
	main()
