#!/usr/bin/env python3
"""
Test script checking the decision procedures the binding exposes over a full
spec: sat, unsat, valid, realizable, unrealizable and unsat_core, including
the verdict a spec unsatisfiable only for some inputs gets, and the error
results.
"""

import re

import tau_loader as tau

# Contradictory only when i1 is 0: unsat and unrealizable.
INPUT_DEPENDENT = "always o1[t]:bv[8] = i1[t]:bv[8] && o1[t] != {0}:bv[8]."
CONTRADICTION = "always o2[t] = 1 && o2[t] = 0."

def untyped(core):
	return [re.sub(r":[a-z]+(\[[0-9]+\])?", "", c) for c in core]

def verdict(r):
	assert r, f"no verdict: {r.report.errors}"
	assert isinstance(r.value, bool), f"not a bool: {r.value!r}"
	return r.value

def test_sat_valid_realizable():
	assert verdict(tau.sat("always o3[t] = i3[t].")) is True
	assert verdict(tau.realizable("always o3[t] = i3[t].")) is True
	assert verdict(tau.valid("always o3[t] = i3[t].")) is False
	assert verdict(tau.valid("always o3[t] = o3[t].")) is True

	assert verdict(tau.sat(CONTRADICTION)) is False
	assert verdict(tau.unsat(CONTRADICTION)) is True
	assert verdict(tau.unrealizable(CONTRADICTION)) is True

	# Tau satisfiability quantifies the inputs universally: some input
	# (i1 = 0) makes the spec contradictory, so it is unsat.
	assert verdict(tau.sat(INPUT_DEPENDENT)) is False
	assert verdict(tau.realizable(INPUT_DEPENDENT)) is False
	assert verdict(tau.unrealizable(INPUT_DEPENDENT)) is True
	assert verdict(tau.is_realizable(INPUT_DEPENDENT)) is False
	# ... which is exactly what get_interpreter rejects.
	assert not tau.get_interpreter(INPUT_DEPENDENT)

def test_definitions_are_applied():
	# A spec root is unwrapped to its main formula, definitions applied.
	assert verdict(tau.sat("f(x) := x'. always o4[t] = f(o4[t]).")) is False
	assert verdict(tau.sat("f(x) := x'. always o4[t] = f(i4[t]).")) is True

def test_errors_carry_no_verdict():
	for f in (tau.sat, tau.unsat, tau.valid, tau.realizable,
			tau.unrealizable, tau.unsat_core):
		r = f("not a spec ((")
		assert not r and r.value is None, f"{f.__name__}: {r!r}"
		assert r.report.has_error, f"{f.__name__}: no error reported"

def test_unsat_core():
	# Realizability core: o5 must follow i5 and be 1, so i5 = 0 breaks it;
	# the o6 conjuncts are irrelevant.
	spec = ("always (o5[t] = i5[t] && o6[t] = i6[t]) && "
		"always o5[t] = 1 && always o6[t] | i6[t] = o6[t] | i6[t].")
	r = tau.unsat_core(spec)
	assert r, f"no core: {r.report.errors}"
	assert len(r.value) == 2, f"core {r.value}"
	assert untyped(r.value) == ["always o5[t] = i5[t]",
		"always o5[t] = 1"], r.value
	# The sat core is the same: i5 = 0 contradicts it.
	r = tau.unsat_core(spec, realizability=False)
	assert r and len(r.value) == 2, f"sat core {r.value}"
	# A spec with no conflict has an empty core.
	r = tau.unsat_core("always o5[t] = i5[t] && always o6[t] = 1.")
	assert r and r.value == [], f"core {r.value}"

	r = tau.unsat_core("always o7[t] = 1 && always o8[t] = 0 && "
		"always o7[t] = 0.", realizability=False)
	assert r and len(r.value) == 2, f"core {r.value}"
	assert all("o7" in c for c in r.value), r.value

def test_reset_between_typings():
	# The decision procedures type streams like get_interpreter does, so a
	# host that retypes a stream resets in between.
	assert verdict(tau.sat("always o9[t]:bv[8] = {1}:bv[8].")) is True
	clash = tau.sat("always o9[t]:bv[16] = {1}:bv[16].")
	assert not clash, "o9 retyped without a reset"
	tau.reset_definitions()
	assert verdict(tau.sat("always o9[t]:bv[16] = {1}:bv[16].")) is True

def main():
	test_sat_valid_realizable()
	test_definitions_are_applied()
	test_errors_carry_no_verdict()
	test_unsat_core()
	test_reset_between_typings()
	print("Test passed!")

if __name__ == "__main__":
	main()
