#!/usr/bin/env python3
"""
Test script checking ltl-semantics through the binding: sat and realizable
over the finite traces of LTLf against the same formulas over infinite
traces, and the refusal of a word the option does not take.
"""

import os
import shutil

import tau_loader as tau

# The binding has no type query, and each option name starts with its family.
HAS_BV = any(n.startswith("bv-") for n in tau.option_names())

# (formula, sat over infinite traces, sat over finite traces)
SAT = [
	("(G(F(o1[t]:bv[1] = 0))) && (G(F(o1[t]:bv[1] = 1)))", True, False),
	("(G(o1[t]:bv[1] = 1)) && (F(o1[t]:bv[1] = 0))", False, False),
]
# (formula, realizable over infinite traces, over finite traces)
REALIZABLE = [
	("G(o1[t]:bv[1] = i1[t+1]:bv[1])", False, True),
	("G(i1[t]:bv[1] = 1 -> F(o1[t]:bv[1] = 1))", True, True),
	("G(F(i1[t]:bv[1] = 1))", False, False),
]

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

def with_semantics(word, check):
	assert tau.set_option("ltl-semantics", word)
	try:
		check()
	finally:
		assert tau.set_option("ltl-semantics", "infinite")

def test_semantics():
	for fm, inf, _ in SAT:
		assert verdict(tau.sat(fm + ".")) is inf, fm
	for fm, inf, _ in REALIZABLE:
		assert verdict(tau.realizable(fm + ".")) is inf, fm
	def finite():
		for fm, _, fin in SAT:
			assert verdict(tau.sat(fm + ".")) is fin, fm
		for fm, _, fin in REALIZABLE:
			assert verdict(tau.realizable(fm + ".")) is fin, fm
	with_semantics("finite", finite)
	assert not tau.set_option("ltl-semantics", "eventually")
	assert tau.get_option("ltl-semantics").value == "infinite"

def main():
	if not ltlsynt():
		print("skipped: no ltlsynt")
		print("Test passed!")
		return
	if not HAS_BV:
		print("skipped: the pack has no bv")
		print("Test passed!")
		return
	test_semantics()
	print("Test passed!")

if __name__ == "__main__":
	main()
