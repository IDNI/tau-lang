#!/usr/bin/env python3
"""
Test script checking the LTL verdicts of the binding that
test_ltl_decisions_api.py leaves out: valid over U, W, S and T, and the
UNKNOWN of a spec when no backend is found at all.
"""

import os
import shutil

import tau_loader as tau

def verdict(r):
	assert r, f"no verdict: {r.report.errors}"
	assert isinstance(r.value, bool), f"not a bool: {r.value!r}"
	return r.value

# (spec, valid)
VALID_CASES = [
	("(o1[t] != 0) weak_until (o1[t] = 0).", True),
	("(o1[t] != 0) until (o1[t] = 0).", False),
	("always ((i1[t] = 0) -> ((o1[t] = 0) since (i1[t] = 0))).", True),
	("always (((i1[t] = 0) trigger (o1[t] = 0)) -> (o1[t] = 0)).", True),
]

def test_valid():
	for spec, expected in VALID_CASES:
		got = verdict(tau.valid(spec))
		assert got is expected, f"valid {spec}: {got}"

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

def ltlsynt_found():
	if shutil.which("ltlsynt"):
		return True
	spot_bin = os.environ.get("TAU_SPOT_BIN", "")
	return bool(spot_bin) and os.access(
		os.path.join(spot_bin, "ltlsynt"), os.X_OK)

def main():
	test_no_backend_gives_no_verdict()
	if ltlsynt_found():
		test_valid()
	else:
		print("skipped valid: no ltlsynt")
	print("Test passed!")

if __name__ == "__main__":
	main()
