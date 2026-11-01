#!/usr/bin/env python3
"""
Test script checking that reset() returns the engine to a fresh state: it
frees tree nodes, drops the stream types, and leaves the engine usable.
"""

import gc

import tau_loader as tau

def test_interpreter_built_before_a_reset():
	r = tau.get_interpreter(
		"i1 : sbf := in console. o1 : sbf := out console. "
		"always o1[t] = i1[t].", tau.interpreter_options())
	assert r, f"Failed to create an interpreter: {r.report.errors}"
	i = r.value
	assert not i.released
	assert tau.reset() > 0
	assert i.released

	for name, answer in (("step", tau.step(i)),
			("step with inputs", tau.step(i, {})),
			("get_inputs_for_step", tau.get_inputs_for_step(i)),
			("reset", i.reset()), ("update", i.update("o1[t] = 0.")),
			("can_extend", i.can_extend("o1[t] = 0.")),
			("admissible_outputs", i.admissible_outputs()),
			("current_state", i.current_state())):
		assert not answer, f"{name} answered after the reset"
		assert "reset()" in answer.report.errors[0], answer.report.errors
	for call in (i.current_spec, i.visualise_mealy_dot):
		try:
			call()
		except RuntimeError as e:
			assert "reset()" in str(e), str(e)
		else:
			raise AssertionError(f"{call} answered after the reset")

	# destroying it stays safe, and so does the engine
	del i, r
	gc.collect()
	fresh = tau.get_interpreter("o9[t]:sbf = 0.")
	assert fresh and not fresh.value.released

def main():
	test_interpreter_built_before_a_reset()

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
