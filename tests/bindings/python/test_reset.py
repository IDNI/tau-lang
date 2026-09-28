#!/usr/bin/env python3
"""
Test script checking that reset() returns the engine to a fresh state: it
frees tree nodes, drops the stream types, and leaves the engine usable.
"""

import gc

import tau_loader as tau

def main():
	r = tau.get_interpreter("o7[t]:sbf = i7[t]:sbf.")
	assert r, f"Failed to create the first interpreter: {r.report.errors}"
	del r
	gc.collect()

	freed = tau.reset()
	assert isinstance(freed, int) and freed > 0, f"reset freed {freed} nodes"

	retyped = tau.get_interpreter("o7[t]:tau = o7[t]:tau.")
	assert retyped, f"Failed to retype o7 after the reset: {retyped.report.errors}"

	r = tau.get_interpreter("o8[t]:sbf = i8[t]:sbf.")
	assert r, f"Failed to create an interpreter after the reset: {r.report.errors}"
	i = r.value
	r = tau.get_inputs_for_step(i)
	assert r, f"Failed to get inputs for step: {r.report.errors}"
	stepped = tau.step(i, {at: "1" for at in r.value})
	assert stepped, f"The interpreter does not step: {stepped.report.errors}"

	print("Test passed!")

if __name__ == "__main__":
	main()
