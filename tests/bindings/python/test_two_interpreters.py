#!/usr/bin/env python3
"""
Two interpreters of one spec in one process, each revised by the same rule
and stepped after it: the node store is shared, so a sweep one of them
starts must keep the nodes the other holds.
"""

import tau_loader as tau

SPEC = ("((!(i0[t] = 0)) ? ( u[t] = i0[t] && o0[t] = 0 ) : o0[t] = 1)"
	" && (o5[t]:bv[24] = o5[t]:bv[24]).")
RULE = ("always ( ((i1[t]:bv[24] >  i2[t]:bv[24]) && o2[t] = { #x000001 }:bv[24])"
	" || ((i1[t]:bv[24] <= i2[t]:bv[24]) && o2[t] = { #x000000 }:bv[24]) ).")


def step(interpreter, values):
	r = tau.get_inputs_for_step(interpreter)
	assert r, f"Failed to get inputs for step: {r.report.errors}"
	res = tau.step(interpreter, {i: values[i.name] for i in r.value})
	assert res, f"Failed to step interpreter: {res.report.errors}"
	return {o.name: v for o, v in res.value.items()}


def main():
	if not any(n.startswith("bv-") for n in tau.option_names()):
		tau.skip("the spec and the rule need bv in the pack")
	# a sweep at every step
	assert tau.set_option("gc-min-size", "0")
	assert tau.set_option("gc-growth-factor", "1")
	interpreters = []
	for _ in range(2):
		r = tau.get_interpreter(SPEC)
		assert r, f"Failed to create interpreter: {r.report.errors}"
		interpreters.append(r.value)
	for i in interpreters:
		step(i, {"i0": RULE, "i1": "100", "i2": "50"})
	for i in interpreters:
		outputs = step(i, {"i0": "F", "i1": "100", "i2": "50"})
		assert outputs.get("o2") == "1", outputs
	print("two interpreters: ok")


if __name__ == "__main__":
	main()
