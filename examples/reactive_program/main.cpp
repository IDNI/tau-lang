// Minimal driver for a synthesized reactive program.
//
// Build:  make
// Run:    ./sim

#include "program.h"
#include <cstdio>

int main() {
	TauProgram p;
	TauProgram::Inputs in;

	std::printf("step  o_p0  ok\n");
	std::printf("----  ----  --\n");

	for (int t = 0; t < 10; ++t) {
		auto o = p.step(in);
		std::printf("%4d  %4d  %2d\n", t, (int)o.o_p0, (int)o.ok);
		if (!o.ok) {
			std::printf("!! program reached unreachable state — spec bug\n");
			return 1;
		}
	}

	std::puts("\nOK: 10 ticks completed, invariants preserved.");
	return 0;
}
