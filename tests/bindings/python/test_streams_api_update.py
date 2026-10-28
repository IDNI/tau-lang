
#!/usr/bin/env python3
"""
Test script for remapped streams with an update stream: the value on u
revises the running spec, and the outputs after it follow the revision.
"""

import tau_loader as tau

class TauStreamUpdater:
	def __init__(self, spec: str):
		self.i_stream = tau.vector_input_stream()
		self.u_stream = tau.vector_output_stream()
		opts = tau.interpreter_options()
		opts.input_remaps["i"] = self.i_stream
		opts.output_remaps["u"] = self.u_stream

		r = tau.get_interpreter(spec, opts)
		assert r, f"Failed to create interpreter: {r.report.errors}"
		self.i = r.value

	def submit(self, i: str):
		self.i_stream.put(i)
		res = tau.step(self.i)
		assert res, f"Failed to step interpreter: {res.report.errors}"
		return self.u_stream.get()

# The inputs and the u they give: the fourth input revises u[t] = i[t]
# into u[t] = i[t] | i[t-1], which the last input tells apart (F | T).
# The revised spec restarts at the step after the update and its lookback
# does not reach the values played before (README, "Pointwise revision
# details"), so u at that step is free and takes the bottom value F,
# although i[3] is not 0.
STEPS = [("T.", "T"), ("F.", "F"), ("T.", "T"),
	("u[t] = i[t] | i[t-1].", "always i[t-1]:tau|i[t]:tau = u[t]:tau"),
	("F.", "F"), ("F.", "F"), ("T.", "T"), ("F.", "T")]

def main():
	spec = "u[t] = i[t]."
	updater = TauStreamUpdater(spec)
	assert updater.i.spec_revision == 0
	for step, (value, expected) in enumerate(STEPS):
		result = updater.submit(value)
		assert result == expected, \
			f"step {step}: u = {result!r}, expected {expected!r}"
		print(result)
	assert updater.i.spec_revision == 1, updater.i.spec_revision
	assert "i[t-1]" in updater.i.current_spec(), updater.i.current_spec()
	print("Test passed!")

if __name__ == "__main__":
	main()
