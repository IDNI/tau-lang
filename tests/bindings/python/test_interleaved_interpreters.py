#!/usr/bin/env python3
"""
Interleaved interpreters of one spec in one process, driven the way the
tau-testnet determinism suite drove them before it moved each case into a
process of its own: a long-lived interpreter revised by a transfer rule, then
two fresh interpreters per case, each revised by the case's rule and stepped
in turn, the three dropped at the end of the case.

On GitHub runners that suite died with SIGSEGV inside the native step of the
second or third case. Each round here replays its four cases in pytest's
order. TAU_INTERLEAVED_ROUNDS sets the number of rounds (default 3), and
TAU_INTERLEAVED_SWEEP=1 forces a sweep at every step.
"""

import gc
import os
import time

import tau_loader as tau

GENESIS = ("((!(i0[t] = 0)) ? ( u[t] = i0[t] && o0[t] = 0 ) : o0[t] = 1)"
	" && (o5[t]:bv[24] = o5[t]:bv[24]).")
TRANSFER_RULE = ("always (o1[t]:bv[24] = ((i1[t]:bv[24] + { #x000000 }:bv[24])"
	" & ((i1[t]:bv[24] + { #x000000 }:bv[24]) | { #x000000 }:bv[24]))).")
DET_RULE = ("always ( ((i1[t]:bv[24] >  i2[t]:bv[24]) && o2[t] = { #x000001 }:bv[24])"
	" || ((i1[t]:bv[24] <= i2[t]:bv[24]) && o2[t] = { #x000000 }:bv[24]) ).")
O5_RULE = ("always ((i3[t] = {#x0011}:bv[16] && i1[t] > {1000}:bv[24])"
	" ? o5[t] = {0}:bv[16] : o5[t] = {1}:bv[16]).")
O9_RULE = "always (o9[t]:bv[24] = { #x00000a }:bv[24])."


def interpreter():
	r = tau.get_interpreter(GENESIS)
	assert r, f"Failed to create interpreter: {r.report.errors}"
	return r.value


def communicate(interp, rule=None, inputs=None):
	"""One step as tau-testnet takes it: prompt,
	answer from the queued inputs, the rule on i0 or the fallback ("F" on i0,
	"0" elsewhere), and prompt again while the step asks for more input
	without moving time. Returns the outputs by name."""
	queues = {k: list(v) for k, v in (inputs or {}).items()}
	rule_delivered = False
	for _ in range(100):
		r = tau.get_inputs_for_step(interp)
		assert r, f"Failed to get inputs for step: {r.report.errors}"
		assignments, taken, offered = {}, {}, False
		for i in r.value:
			queue, n = queues.get(i.name), taken.get(i.name, 0)
			if queue and n < len(queue):
				assignments[i] = queue[n]
				taken[i.name] = n + 1
			elif i.name == "i0" and rule is not None and not rule_delivered:
				assignments[i] = rule
				offered = True
			else:
				assignments[i] = "F" if i.name == "i0" else "0"
		time_before = interp.time_point
		res = tau.step(interp, assignments)
		time_moved = interp.time_point != time_before
		if res.value is not None or time_moved:
			for name, n in taken.items():
				del queues[name][:n]
			rule_delivered = rule_delivered or offered
		if res.value is not None:
			assert not res.report.has_error, res.report.errors
			return {o.name: str(v) for o, v in res.value.items()}
		if res.report.awaiting_input and not time_moved:
			continue
		raise Refused(res.report.errors)
	raise AssertionError("the step asked for input 100 times")


class Refused(Exception):
	pass


def fresh_pair(rule, inputs):
	a, b = interpreter(), interpreter()
	communicate(a, rule)
	communicate(b, rule)
	return communicate(a, None, inputs), communicate(b, None, inputs)


def case_fresh_instances():
	x, y = fresh_pair(DET_RULE, {"i1": ["100"], "i2": ["50"]})
	assert x == y, (x, y)
	assert x.get("o2") == "1", x


def case_o5_user_policy():
	# o5 is bv[24] in the genesis spec, so the first interpreter refuses
	# the rule and the case ends with the second one never stepped
	try:
		fresh_pair(O5_RULE, {"i1": ["2000"], "i3": ["#x0011"]})
	except Refused as e:
		assert any("Incompatible type information in o5" in m
			for m in e.args[0]), e
		return
	raise AssertionError("the o5 rule was not refused")


def case_o9_consensus_fee():
	x, y = fresh_pair(O9_RULE, {"i1": ["100"], "i5": ["1700000000"]})
	assert x.get("o9") == y.get("o9"), (x, y)


def case_same_instance(live):
	communicate(live, DET_RULE)
	inputs = {"i1": ["100"], "i2": ["50"]}
	x = communicate(live, None, inputs)
	y = communicate(live, None, inputs)
	assert x.get("o2") == y.get("o2"), (x, y)


def main():
	if not any(n.startswith("bv-") for n in tau.option_names()):
		tau.skip("the spec and the rules need bv in the pack")
	if os.environ.get("TAU_INTERLEAVED_SWEEP") == "1":
		assert tau.set_option("gc-min-size", "0")
		assert tau.set_option("gc-growth-factor", "1")
	rounds = int(os.environ.get("TAU_INTERLEAVED_ROUNDS", "3"))
	cases = [lambda live: case_fresh_instances(),
		lambda live: case_o5_user_policy(),
		lambda live: case_o9_consensus_fee(),
		case_same_instance]
	for n in range(rounds):
		start = time.monotonic()
		for case in cases:
			# the setUp of the suite: a live interpreter with the
			# transfer rule, kept while the case runs
			live = interpreter()
			communicate(live, TRANSFER_RULE)
			case(live)
			del live
			gc.collect()
		print(f"round {n + 1}/{rounds}: ok in "
			f"{time.monotonic() - start:.3f} s", flush=True)
	print("interleaved interpreters: ok")


if __name__ == "__main__":
	main()
