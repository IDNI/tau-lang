#!/usr/bin/env python3
"""
The spec files of tests/codegen_specs through the Python step loop: the
outputs are the ones the spec fixes from its inputs, as `tau <spec file>` and
`run` print them (test_repl-entry_parity.cmake).
"""

import os
import tau_loader as tau

HAS_BV = any(n.startswith("bv-") for n in tau.option_names())

SPECS = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "codegen_specs")

def run_file(name):
	with open(os.path.join(SPECS, name + ".tau")) as f:
		i = tau.get_interpreter(f.read().strip())
	assert i, i.report.errors
	i = i.value
	with open(os.path.join(SPECS, name + ".in")) as f:
		lines = iter(f.read().split("\n"))
	out = []
	while True:
		needed = tau.get_inputs_for_step(i)
		assert needed, needed.report.errors
		values = {}
		try:
			for at in needed.value:
				values[at] = next(lines)
				if not values[at]:
					return out
		except StopIteration:
			return out
		res = tau.step(i, values)
		assert res, res.report.errors
		out.append(sorted(res.value.values()))

def test_echo_prints_its_inputs():
	assert run_file("echo") == [["2"], ["7"], ["4"]]

def main():
	if not HAS_BV:
		tau.skip("the echo spec needs bv in the pack")
	test_echo_prints_its_inputs()
	print("test_entry_parity: all checks passed")

if __name__ == "__main__":
	main()
