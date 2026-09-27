# Compile a Tau specification into a standalone executable with the `compile`
# verb of the tau binary. `tau gen <spec.tau>` writes a CMake project next to
# the spec (`spec.tau.build/`: one main.cpp driving the synthesized strategy
# through the same run loop `tau spec.tau` uses, plus a CMakeLists.txt that
# links the tau SDK, and one CMake preset per platform.
# `tau compile <spec.tau> -o <exe>` does that and then builds it, for this
# machine unless `--preset` names another platform. `-c <compiler>` or
# `TAU_CXX` picks the C++ compiler (default: cmake's, or the preset's). The
# produced program reads its inputs and prints its outputs exactly like the
# interpreter does, and exits when stdin closes.
TAU ?= tau

all: sim

sim: spec.tau
	$(TAU) compile $< -o $@

run: sim
	./sim

clean:
	rm -rf sim spec.tau.build

.PHONY: all run clean
