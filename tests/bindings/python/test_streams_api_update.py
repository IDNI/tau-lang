
#!/usr/bin/env python3
"""
Test script equivalent to test_api-string_api.cpp "with remapped streams"
"""

import tau_loader as tau

class TauStreamUpdater:
	def __init__(self, spec: str):
		self.i_stream = tau.vector_input_stream()
		self.u_stream = tau.vector_output_stream()
		opts = tau.interpreter_options()
		opts.input_remaps["i"] = self.i_stream
		opts.output_remaps["u"] = self.u_stream

		self.i = tau.get_interpreter(spec, opts)
		assert self.i is not None, "Failed to create interpreter"

	def submit(self, i: str):
		self.i_stream.put(i)
		outputs = tau.step(self.i)
		assert outputs is not None, "Failed to step interpreter"
		return self.u_stream.get()

def main():
	spec = "u[t] = i[t]."
	updater = TauStreamUpdater(spec)
	for i in ["T.", "F.", "T.", "u[t] = i[t] | i[t-1].", "F.", "F.", "T."]:
		result = updater.submit(i)
		assert result is not None, "Failed to submit update"
		print(result)
	print("Test passed!")

if __name__ == "__main__":
	main()
