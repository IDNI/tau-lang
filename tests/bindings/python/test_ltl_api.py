#!/usr/bin/env python3
"""
Test script checking the LTL verdicts the binding gives: each temporal
operator (U, R, W, S, T and G F) through sat, valid, realizable and
unrealizable, a spec no backend decides, and the export of the winning
strategy.
"""

import os
import tempfile

import tau_loader as tau

def verdict(r):
	assert r, f"no verdict: {r.report.errors}"
	assert isinstance(r.value, bool), f"not a bool: {r.value!r}"
	return r.value

# (command, spec, verdict); the inputs are quantified universally
CASES = [
	(tau.unrealizable, "(o1[t] = 0) until (i1[t] = 0).", True),
	(tau.unrealizable, "(o1[t] = 0) weak_until (i1[t] = 0).", False),
	(tau.unrealizable, "(i1[t] = 0) release (o1[t] = 0).", False),
	(tau.unrealizable, "(o1[t] = 0) release (i1[t] = 0).", True),
	(tau.realizable, "always (sometimes (o1[t] = 0)).", True),
	(tau.realizable, "always (sometimes (i1[t] = 0)).", False),
	(tau.realizable, "always ((o1[t] = 0) since (i1[t] = 0)).", False),
	(tau.realizable, "always ((i1[t] = 0) trigger (o1[t] = 0)).", True),
	(tau.realizable, "(always (sometimes (i1[t] = 0))) -> "
		"(always (sometimes (o1[t] = 0 && i1[t] = 0))).", True),
	(tau.valid, "(o1[t] != 0) weak_until (o1[t] = 0).", True),
	(tau.valid, "(o1[t] != 0) until (o1[t] = 0).", False),
	(tau.valid, "always ((i1[t] = 0) -> "
		"((o1[t] = 0) since (i1[t] = 0))).", True),
	(tau.valid, "always (((i1[t] = 0) trigger (o1[t] = 0)) -> "
		"(o1[t] = 0)).", True),
	(tau.sat, "(always (sometimes (o1[t] = 0))) && "
		"(always (sometimes (o1[t] != 0))).", True),
	(tau.sat, "(always (sometimes (o1[t] = 0))) && "
		"(sometimes (always (o1[t] != 0))).", False),
]

def test_operators():
	for command, spec, expected in CASES:
		got = verdict(command(spec))
		assert got is expected, f"{command.__name__} {spec}: {got}"

def test_no_backend_gives_no_verdict():
	# Without ltlsynt the realizability of a full-LTL spec is not decided:
	# the result is an error, never the verdict False.
	path, spot_bin = os.environ.get("PATH", ""), tau.get_option("spot-bin")
	assert spot_bin, "no spot-bin option"
	os.environ["PATH"] = ""
	assert tau.set_option("spot-bin", ""), "spot-bin refused"
	try:
		for command in (tau.realizable, tau.unrealizable):
			r = command("(o1[t] = 0) until (i1[t] = 0).")
			assert not r and r.value is None, \
				f"{command.__name__}: {r.value!r}"
			errors = [str(e) for e in r.report.errors]
			assert any("UNKNOWN" in e for e in errors), \
				f"{command.__name__}: {errors}"
	finally:
		os.environ["PATH"] = path
		tau.set_option("spot-bin", spot_bin.value)

def test_strategy_export():
	with tempfile.TemporaryDirectory() as scratch:
		out = os.path.join(scratch, "strategy.hoa")
		assert tau.set_option("ltl-export-strategy-file", out), \
			"ltl-export-strategy-file refused"
		try:
			assert verdict(tau.realizable(
				"always (sometimes (o1[t] = 0)).")) is True
		finally:
			tau.set_option("ltl-export-strategy-file", "")
		with open(out) as f:
			text = f.read()
		assert text.startswith("HOA: v1"), text[:80]
		assert "--BODY--" in text and "--END--" in text, text

def main():
	test_operators()
	test_no_backend_gives_no_verdict()
	test_strategy_export()
	print("Test passed!")

if __name__ == "__main__":
	main()
