# Build the reactive-program example.
#
# Dependencies:
#   - `tau_codegen` on PATH (built from this repo).
#   - g++ with C++17 support.
#
# The generated header is pure C++17; no runtime link to libTAU is required
# for propositional specs.  Specs with data atoms (qlt, bv arithmetic, etc.)
# need `-lTAU` at link time — see the tau-lang manual for details.

TAU_CODEGEN ?= tau_codegen
CXX         ?= g++
CXXFLAGS    ?= -O3 -flto -std=c++17 -Wall

all: sim

program.h: spec.tau
	$(TAU_CODEGEN) $< -o $@

sim: main.cpp program.h
	$(CXX) $(CXXFLAGS) main.cpp -o $@

clean:
	rm -f program.h sim

.PHONY: all clean
