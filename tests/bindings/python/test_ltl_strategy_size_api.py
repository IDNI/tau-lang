#!/usr/bin/env python3
"""
Test script checking the size options of a strategy the binding exposes:
ltl-minimize-strategy keeps a verdict, and set_ltl_strategy_max_states keeps
it at the size of the strategy and answers UNKNOWN below it.
"""

import os
import shutil

import tau_loader as tau

ALTERNATION = ("G(o1[t]:bv[1] = 1 -> F(o1[t]:bv[1] = 0)) && "
	"G(o1[t]:bv[1] = 0 -> F(o1[t]:bv[1] = 1)) && "
	"G(o2[t]:bv[1] = 1 -> F(o2[t]:bv[1] = 0)) && "
	"G(o2[t]:bv[1] = 0 -> F(o2[t]:bv[1] = 1)).")

def ltlsynt():
	found = shutil.which("ltlsynt")
	if found:
		return found
	spot_bin = os.environ.get("TAU_SPOT_BIN", "")
	candidate = os.path.join(spot_bin, "ltlsynt") if spot_bin else ""
	return candidate if candidate and os.access(candidate, os.X_OK) else None

def verdict(r):
	assert r, f"no verdict: {r.report.errors}"
	assert isinstance(r.value, bool), f"not a bool: {r.value!r}"
	return r.value

def test_strategy_size():
	assert tau.set_option("ltl-minimize-strategy", "true")
	try:
		assert verdict(tau.realizable(ALTERNATION)) is True
	finally:
		assert tau.set_option("ltl-minimize-strategy", "false")
	assert tau.get_ltl_strategy_max_states() == 0
	tau.set_ltl_strategy_max_states(3)
	try:
		assert verdict(tau.realizable(ALTERNATION)) is True
		tau.set_ltl_strategy_max_states(2)
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
	test_strategy_size()
	print("Test passed!")

if __name__ == "__main__":
	main()
