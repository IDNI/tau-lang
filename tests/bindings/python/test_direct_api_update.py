#!/usr/bin/env python3
"""
Test script  for direct Tau API calling with update stream
"""

import tau_loader as tau

class TauDirectUpdater:

	def __init__(self, spec: str):

		"Create a Tau interpreter for a given specification"
		self.i = tau.get_interpreter(spec)
		assert self.i is not None, "Failed to create interpreter"

	def submit(self, value: str):

		"Retrieve the inputs for the next step"
		inputs = tau.get_inputs_for_step(self.i)

		"Assign value to the input"
		input_values = {}
		for input in inputs:
			if input.name == "i":
				input_values[input] = value

		"Step the interpreter with the assigned value"
		outputs = tau.step(self.i, input_values)
		assert outputs is not None, "Failed to step interpreter"

		"Retrieve the output value from the update output"
		for output, output_value in outputs.items():
			if output.name == "u":
				return output_value

		return None

def main():

	spec = "u[t] = i[t]."
	updater = TauDirectUpdater(spec)

	"Submit the updates to the interpreter one by one and print results"
	for i in ["T.", "F.", "T.", "u[t] = i[t] | i[t-1].", "F.", "F.", "T."]:
		result = updater.submit(i)
		assert result is not None, "Failed to submit update"
		print(result)

if __name__ == "__main__":
	main()




