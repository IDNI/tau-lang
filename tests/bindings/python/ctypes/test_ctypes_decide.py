#!/usr/bin/env python3
"""
The realizability verdict of the ctypes binding is the one the nanobind
module gives: a spec whose output must wait on an input the environment
never has to give is unrealizable, a spec with definitions reads with them
applied, and synthesize refuses what decide calls unrealizable.

Needs TAU_LTL_LIB naming libtau_ctypes and tau_lang.py on PYTHONPATH.
"""

import tau_lang as tau

# The environment may never set i1.
UNTIL_INPUT = "(o1[t] = 1) until (i1[t] = 1)."

def test_verdicts():
	assert tau.decide("always o2[t] = i2[t].") == tau.Verdict.REALIZABLE
	assert tau.decide("always o3[t] = 1 && o3[t] = 0.") \
		== tau.Verdict.UNREALIZABLE
	assert tau.decide(UNTIL_INPUT) == tau.Verdict.UNREALIZABLE, \
		tau.last_error()

def test_definitions_are_applied():
	assert tau.decide("f(x) := x'. always o4[t] = f(i4[t]).") \
		== tau.Verdict.REALIZABLE, tau.last_error()
	assert tau.decide("f(x) := x'. always o4[t] = f(o4[t]).") \
		== tau.Verdict.UNREALIZABLE, tau.last_error()

def test_parse_error():
	assert tau.decide("not a spec ((") == tau.Verdict.PARSE_ERROR
	assert tau.last_error().startswith("parse error")

def test_synthesize_refuses_unrealizable():
	try:
		tau.synthesize(UNTIL_INPUT)
	except tau.SynthesisError as e:
		assert e.code == tau.SYNTH_UNREALIZABLE, str(e)
	else:
		raise AssertionError("an unrealizable spec was synthesized")
	handle = tau.synthesize("always o5[t] = i5[t].")
	assert handle > 0
	out = tau.mealy_step(handle, {"i5": "T."})
	assert out["o5"] == "T", out
	tau.mealy_free(handle)

def main():
	test_verdicts()
	test_definitions_are_applied()
	test_parse_error()
	test_synthesize_refuses_unrealizable()
	print("Test passed!")

if __name__ == "__main__":
	main()
