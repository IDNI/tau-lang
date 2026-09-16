#!/usr/bin/env python3
"""
Exercises the interpreter bindings added with the pointwise revision (PWR)
surface: reset, update, current_spec, can_extend, declare_open /
undeclare_open / open_streams, and the module-level is_realizable oracle,
including the error paths, which raise instead of answering False.
"""

import tau_loader as tau

def make():
	r = tau.get_interpreter("o1[t] = i1[t].")
	assert r, f"Failed to create interpreter: {r.report.errors}"
	return r.value

def run_steps(i, n):
	outputs = []
	for step in range(n):
		assigned = {}
		for input_at in tau.get_inputs_for_step(i):
			assigned[input_at] = "T." if step % 2 == 0 else "F."
		res = tau.step(i, assigned)
		assert res, f"Failed to step interpreter: {res.report.errors}"
		outputs.append(list(res.value.values()))
	return outputs

def test_reset_replays_from_time_zero():
	i = make()
	first = run_steps(i, 2)
	assert i.time_point == 2, i.time_point
	i.reset()
	assert i.time_point == 0, i.time_point
	# The memo of the step spec must not survive the reset: the replay
	# has to produce the same outputs as the first run, not the
	# default-zero fallback.
	again = run_steps(i, 2)
	assert again == first, (first, again)

def test_update_changes_current_spec_and_raises_on_garbage():
	# A revision handed to update() directly must carry its stream types
	# (the `u` update stream infers them from the running spec, a bare
	# formula cannot), hence the typed spec here. Stream types are
	# process-global, so this uses streams no other test touches.
	r = tau.get_interpreter("o2[t]:sbf = i2[t]:sbf.")
	assert r, f"Failed to create interpreter: {r.report.errors}"
	i = r.value
	before = i.current_spec()
	assert "o2" in before, before
	rev0 = i.spec_revision
	assert i.update("o2[t]:sbf = i2[t]:sbf | i2[t-1]:sbf") is True
	assert i.spec_revision == rev0 + 1, (rev0, i.spec_revision)
	after = i.current_spec()
	assert after != before, after
	# A rejected revision (the result is unsat) reports False and leaves
	# the running spec alone.
	assert i.update("o2[t]:sbf = i2[t]:sbf && o2[t]:sbf != i2[t]:sbf") is False
	assert i.spec_revision == rev0 + 1, i.spec_revision
	assert i.current_spec() == after
	try:
		i.update("this is not a formula ((")
	except ValueError:
		pass
	else:
		raise AssertionError("update must raise on a parse failure")

def test_can_extend_raises_on_garbage():
	i = make()
	assert isinstance(i.can_extend("o1[t] = 1"), bool)
	try:
		i.can_extend(")(")
	except ValueError:
		pass
	else:
		raise AssertionError("can_extend must raise on a parse failure")

def test_declare_open_registers_and_lists():
	i = make()
	assert i.open_streams() == [], i.open_streams()
	i.declare_open("o1", lambda formula: "0")
	assert i.open_streams() == ["o1"], i.open_streams()
	i.undeclare_open("o1")
	assert i.open_streams() == [], i.open_streams()

def test_is_realizable_verdicts_and_errors():
	assert tau.is_realizable("always o1[t] = i1[t].") is True
	assert tau.is_realizable("always o1[t] = 1 && o1[t] = 0.") is False
	try:
		tau.is_realizable("not a spec ((")
	except ValueError:
		pass
	else:
		raise AssertionError("is_realizable must raise on a parse failure")

def main():
	test_reset_replays_from_time_zero()
	test_update_changes_current_spec_and_raises_on_garbage()
	test_can_extend_raises_on_garbage()
	test_declare_open_registers_and_lists()
	test_is_realizable_verdicts_and_errors()
	print("test_interpreter_pwr_api: all checks passed")

if __name__ == "__main__":
	main()
