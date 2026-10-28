#!/usr/bin/env python3
"""
Test script for direct Tau API calling with update stream: the value on u
revises the running spec, and the outputs after it follow the revision.
"""

import tau_loader as tau

class TauDirectUpdater:

	def __init__(self, spec: str):

		"Create a Tau interpreter for a given specification"
		r = tau.get_interpreter(spec)
		assert r, f"Failed to create interpreter: {r.report.errors}"
		self.i = r.value

	def submit(self, value: str):

		"Retrieve the inputs for the next step"
		r = tau.get_inputs_for_step(self.i)
		assert r, f"Failed to get inputs for step: {r.report.errors}"
		inputs = r.value

		"Assign value to the input"
		input_values = {}
		for input in inputs:
			if input.name == "i":
				input_values[input] = value

		"Step the interpreter with the assigned value"
		res = tau.step(self.i, input_values)
		assert res, f"Failed to step interpreter: {res.report.errors}"
		outputs = res.value

		"Retrieve the output value from the update output"
		for output, output_value in outputs.items():
			if output.name == "u":
				return output_value

		return None

# The inputs and the u they give: the fourth input revises u[t] = i[t]
# into u[t] = i[t] | i[t-1], which the last input tells apart (F | T).
STEPS = [("T.", "T"), ("F.", "F"), ("T.", "T"),
	("u[t] = i[t] | i[t-1].", "always i[t-1]:tau|i[t]:tau = u[t]:tau"),
	("F.", "F"), ("F.", "F"), ("T.", "T"), ("F.", "T")]

def main():

	spec = "u[t] = i[t]."
	updater = TauDirectUpdater(spec)
	assert updater.i.spec_revision == 0

	"Submit the updates to the interpreter one by one and check the results"
	for step, (value, expected) in enumerate(STEPS):
		result = updater.submit(value)
		assert result == expected, \
			f"step {step}: u = {result!r}, expected {expected!r}"
		print(result)
	assert updater.i.spec_revision == 1, updater.i.spec_revision
	assert "i[t-1]" in updater.i.current_spec(), updater.i.current_spec()

if __name__ == "__main__":
	main()
