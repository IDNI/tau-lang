#!/usr/bin/env python3
# The Python binding: decisions, an interpreter stepped over a vector stream,
# options and preferences.
#
# Build the module first, from the repository root:
#   TAU_PYTHON=$HOME/.tau/py312/bin/python3 ./dev preset release-binding-python
# then point PYTHONPATH at its folder and run this file:
#   PYTHONPATH=build/release/bindings/python/nanobind python3 demos/demo_9.1-python.py
#
# Every call below comes from tests/bindings/python. This file has not been
# run as a demo.

import tau

# A decision returns a result: its value is the verdict, and a failed call
# carries its report instead.

r = tau.sat("always o3[t] = i3[t].")
print("sat:", r.value)
print("realizable:", tau.realizable("always o3[t] = i3[t].").value)
print("valid:", tau.valid("always o3[t] = o3[t].").value)

# A contradiction is unsat and unrealizable:

CONTRADICTION = "always o2[t] = 1 && o2[t] = 0."
print("unsat:", tau.unsat(CONTRADICTION).value)
print("unrealizable:", tau.unrealizable(CONTRADICTION).value)

# A spec that does not parse has no verdict, only an error:

bad = tau.sat("not a spec ((")
print("bad spec has an error:", bad.report.has_error)

# An interpreter steps a spec. get_inputs_for_step names the inputs the next
# step expects, and step takes their values and returns the outputs:

i = tau.get_interpreter("o[t] = i[t].").value
for n in range(3):
    inputs = tau.get_inputs_for_step(i).value
    assigned = {name: ("T." if n % 2 else "F.") for name in inputs}
    print("outputs:", tau.step(i, assigned).value)

# Streams can be remapped to vectors. The interpreter then reads its inputs
# from the vector and writes its outputs into another:

i_stream = tau.vector_input_stream(["T", "F", "T"])
o_stream = tau.vector_output_stream()
opts = tau.interpreter_options()
opts.input_remaps["i"] = i_stream
opts.output_remaps["o"] = o_stream
j = tau.get_interpreter("o[t] = i[t].", opts).value
for _ in range(3):
    tau.step(j)
print("o values:", o_stream.get_values())

# The options have the names of the command line. set_option answers with the
# text the option reads after the write, and a bad text is an error:

print(tau.set_option("max-fixpoint-steps", "1000").value)
print(tau.get_option("max-fixpoint-steps").value)
print(tau.set_option("max-fixpoint-steps", "x").report.has_error)

# apply_preferences strengthens a spec by a preference order, here preferring
# o7 = 0:

po = tau.PreferenceOrder()
entry = tau.PreferenceEntry()
entry.var_name = "o7"
entry.preferred_value = "0"
po.entries = [entry]
print(tau.apply_preferences("always o7[t] = i7[t] | o7[t].", po).value)
