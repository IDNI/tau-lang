#!/usr/bin/env python3
"""
Test script equivalent to test_api-string_api.cpp "with remapped streams"
"""

import tau_loader as tau

def main():
	# Remap input and output streams from default console to custom streams
	i_values = ["T", "F", "T"]
	i_stream = tau.vector_input_stream(i_values)
	o_stream = tau.vector_output_stream()

	opts = tau.interpreter_options()
	opts.input_remaps["i"] = i_stream
	opts.output_remaps["o"] = o_stream

	# Make the interpreter for a given specification as string
	maybe_i = tau.get_interpreter("o[t] = i[t].", opts)
	assert maybe_i is not None, "Failed to create interpreter"
	i = maybe_i

	step = 0
	while step < 3:
		# print(f"Step {step}")
		step += 1

		# Step the interpreter and retrieve outputs
		maybe_outputs = tau.step(i)
		assert maybe_outputs is not None, "Failed to step interpreter"
		# outputs = maybe_outputs

	o_values = o_stream.get_values()

	print(f"\nInput values: {i_values}")
	print(f"Output values: {o_values}\n")

	assert len(o_values) == len(i_values), \
		f"Expected {len(i_values)} outputs, got {len(o_values)}"
	assert o_values == i_values, \
		f"Expected {i_values}, got {o_values}"
	print("Test passed!")

if __name__ == "__main__":
	main()
