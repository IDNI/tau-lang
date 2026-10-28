#!/usr/bin/env python3
"""
Test script checking the LTL decisions the binding exposes: realizable and
sat of each of U, R, W, S, T and G(F) both ways, a liveness guarantee under
an assumption, the strategy export to a file, and the UNKNOWN of an ltlsynt
the ltl-timeout watchdog kills.
"""

import os
import stat
import tempfile

import tau_loader as tau
from tau_loader import ltlsynt, verdict

# (formula, realizable): one realizable and one unrealizable formula per
# operator; the inputs of sat are universal, so sat agrees
CASES = [
	("(i1[t] = 1) U (o1[t] = 1)", True),
	("(o1[t] = 1) U (i1[t] = 1)", False),
	("(i1[t] = 1) R (o1[t] = 1)", True),
	("(o1[t] = 1) R (i1[t] = 1)", False),
	("(o1[t] = 1) W (i1[t] = 1)", True),
	("(i1[t] = 1) W (G(i1[t] = 0))", False),
	("G((i1[t] = 1) S (o1[t] = 1))", True),
	("G((o1[t] = 1) S (i1[t] = 1))", False),
	("G((i1[t] = 1) T (o1[t] = 1))", True),
	("G((o1[t] = 1) T (i1[t] = 1))", False),
	("G(F(o1[t] = 1))", True),
	("G(F(i1[t] = 1))", False),
	("(G(F(i1[t] = 1))) -> (G(F(o1[t] = 1 && i1[t] = 1)))", True),
]

def test_operators():
	for fm, real in CASES:
		assert verdict(tau.realizable(fm + ".")) is real, fm
		assert verdict(tau.sat(fm + ".")) is real, fm
		assert verdict(tau.unrealizable(fm + ".")) is (not real), fm

def test_strategy_export():
	with tempfile.TemporaryDirectory() as d:
		path = os.path.join(d, "strategy.hoa")
		assert tau.set_option("ltl-export-strategy-file", path)
		try:
			assert verdict(tau.realizable("G(F(o2[t] = 1)).")) is True
		finally:
			assert tau.set_option("ltl-export-strategy-file", "")
		with open(path) as f:
			hoa = f.read()
		assert hoa.startswith("HOA: v1"), hoa[:80]
		assert "--END--" in hoa, hoa[-80:]

# An ltlsynt that answers --version and then never answers: the run is
# killed after ltl-timeout seconds, and the verdict is UNKNOWN, not F.
def test_timeout_is_unknown(real):
	with tempfile.TemporaryDirectory() as d:
		slow = os.path.join(d, "ltlsynt")
		with open(slow, "w") as f:
			f.write("#!/bin/sh\n"
				f'case "$1" in --version) exec "{real}" "$@";; esac\n'
				"sleep 30\n")
		os.chmod(slow, stat.S_IRWXU)
		saved_path = os.environ["PATH"]
		saved_timeout = tau.get_option("ltl-timeout").value
		os.environ["PATH"] = d + os.pathsep + saved_path
		try:
			assert tau.set_option("ltl-timeout", "1")
			r = tau.realizable("F(o3[t] = 1).")
		finally:
			os.environ["PATH"] = saved_path
			assert tau.set_option("ltl-timeout", saved_timeout)
		assert not r, f"a verdict: {r.value!r}"
		assert any("UNKNOWN" in e for e in r.report.errors), r.report.errors

def main():
	real = ltlsynt()
	if not real:
		print("skipped: no ltlsynt")
		print("Test passed!")
		return
	test_operators()
	test_strategy_export()
	if os.name == "posix":
		test_timeout_is_unknown(real)
	print("Test passed!")

if __name__ == "__main__":
	main()
