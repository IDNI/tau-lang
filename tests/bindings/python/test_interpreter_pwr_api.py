#!/usr/bin/env python3
"""
Exercises the interpreter bindings added with the pointwise revision (PWR)
surface: reset, update, current_spec, can_extend, and the module-level
is_realizable oracle, including the error paths, which report through the
returned result.
"""

import tau_loader as tau

def make():
	r = tau.get_interpreter("o1[t] = i1[t].")
	assert r, f"Failed to create interpreter: {r.report.errors}"
	return r.value

def make_typed():
	r = tau.get_interpreter("o2[t]:sbf = i2[t]:sbf.")
	assert r, f"Failed to create interpreter: {r.report.errors}"
	return r.value

def run_steps(i, n):
	outputs = []
	for step in range(n):
		assigned = {}
		inputs_r = tau.get_inputs_for_step(i)
		assert inputs_r, f"Failed to get inputs for step: {inputs_r.report.errors}"
		for input_at in inputs_r.value:
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

def test_update_changes_current_spec_and_reports_on_garbage():
	# A revision handed to update() directly must carry its stream types
	# (the `u` update stream infers them from the running spec, a bare
	# formula cannot), hence the typed spec here. Stream types are
	# process-global, so this uses streams no other test touches.
	i = make_typed()
	before = i.current_spec()
	assert "o2" in before, before
	rev0 = i.spec_revision
	r = i.update("o2[t]:sbf = i2[t]:sbf | i2[t-1]:sbf")
	assert r.value is True, r.report.errors
	assert i.spec_revision == rev0 + 1, (rev0, i.spec_revision)
	after = i.current_spec()
	assert after != before, after
	# A rejected revision (the result is unsat) reports False and leaves
	# the running spec alone. The rejection reason travels through as a
	# warning rather than being dropped on the floor.
	r = i.update("o2[t]:sbf = i2[t]:sbf && o2[t]:sbf != i2[t]:sbf")
	assert r.value is False
	assert r.report.warnings, r.report.warnings
	assert i.spec_revision == rev0 + 1, i.spec_revision
	assert i.current_spec() == after
	bad = i.update("this is not a formula ((")
	assert not bad and bad.value is None
	assert bad.report.has_error and bad.report.errors

def test_can_extend_reports_on_garbage():
	i = make()
	r = i.can_extend("o1[t] = 1")
	assert isinstance(r.value, bool)
	bad = i.can_extend(")(")
	assert not bad and bad.value is None
	assert bad.report.has_error and bad.report.errors

def test_is_realizable_verdicts_and_errors():
	assert tau.is_realizable("always o1[t] = i1[t].").value is True
	assert tau.is_realizable("always o1[t] = 1 && o1[t] = 0.").value is False
	bad = tau.is_realizable("not a spec ((")
	assert not bad and bad.value is None
	assert bad.report.has_error and bad.report.errors

def main():
	test_reset_replays_from_time_zero()
	test_update_changes_current_spec_and_reports_on_garbage()
	test_can_extend_reports_on_garbage()
	test_is_realizable_verdicts_and_errors()
	print("test_interpreter_pwr_api: all checks passed")

if __name__ == "__main__":
	main()
