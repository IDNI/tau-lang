# Compile a Tau specification into a standalone executable with the `compile`
# verb of the tau binary. `tau compile <spec.tau> -o <exe>` emits a small
# CMake project next to the spec (`spec.tau.build/`: one main.cpp driving the
# synthesized strategy through the same run loop `tau spec.tau` uses, linked
# against the emitting build's libTAU) and builds it; `-c <compiler>` picks
# the C++ compiler (default: $TAU_CXX, else clang++ when on PATH, else
# cmake's default). The produced program reads its inputs and prints its
# outputs exactly like the interpreter does, and exits when stdin closes.
TAU ?= tau

all: sim

sim: spec.tau
	$(TAU) compile $< -o $@

run: sim
	./sim

clean:
	rm -rf sim spec.tau.build

.PHONY: all run clean
