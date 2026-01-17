#!/usr/bin/env python3
"""
Test script equivalent to test_api-string_api.cpp "using get_inputs_for_step"
"""

import tau_loader as tau

def main():
	# Make the interpreter for a given specification as a string
	maybe_i = tau.get_interpreter("o[t] = i[t].")
	assert maybe_i is not None, "Failed to create interpreter"
	i = maybe_i

	collected_inputs = []
	collected_outputs = []

	step = 0
	while step < 3:
		step += 1

		# Find out what inputs are expected for the next step
		inputs = tau.get_inputs_for_step(i)

		# Assign values to the inputs (spec has only `i` input stream)
		assigned_inputs = {}
		for input_at in inputs:
			value = "T." if step % 2 == 0 else "F."
			assigned_inputs[input_at] = value
			collected_inputs.append(value)

		# Step the interpreter with the assigned values
		# and retrieve outputs
		maybe_outputs = tau.step(i, assigned_inputs)
		assert maybe_outputs is not None, "Failed to step interpreter"
		outputs = maybe_outputs

		for _, value in outputs.items():
			collected_outputs.append(value)

	print(f"\nInput values: {collected_inputs}")
	print(f"Outputs values: {collected_outputs}\n")

	inputs_expected = ["F.", "T.", "F."]
	outputs_expected = ["F", "T", "F"]
	assert collected_inputs == inputs_expected, \
		f"Inputs expected {inputs_expected}, got {collected_inputs}"
	assert collected_outputs == outputs_expected, \
		f"Outputs expected {outputs_expected}, got {collected_outputs}"
	print("Test passed!")

if __name__ == "__main__":
	main()
