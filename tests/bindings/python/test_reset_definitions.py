#!/usr/bin/env python3
"""
Test script checking that reset_definitions() lets one process compile two
specs that type the same stream differently. Stream types are process-global,
so without the reset the second spec is rejected.
"""

import tau_loader as tau

def main():
	r = tau.get_interpreter("o7[t]:sbf = i7[t]:sbf.")
	assert r, f"Failed to create the first interpreter: {r.report.errors}"
	first = r.value

	clash = tau.get_interpreter("o7[t]:tau = o7[t]:tau.")
	assert not clash and clash.value is None, "o7 retyped without a reset"

	tau.reset_definitions()
	retyped = tau.get_interpreter("o7[t]:tau = o7[t]:tau.")
	assert retyped, f"Failed to retype o7 after the reset: {retyped.report.errors}"

	# The interpreter built before the reset keeps its own streams.
	r = tau.get_inputs_for_step(first)
	assert r, f"Failed to get inputs for step: {r.report.errors}"
	inputs = {at: "1" for at in r.value}
	assert len(inputs) == 1, "Expected the one input stream i7"
	stepped = tau.step(first, inputs)
	assert stepped, f"The first interpreter no longer steps: {stepped.report.errors}"

	print("Test passed!")

if __name__ == "__main__":
	main()
