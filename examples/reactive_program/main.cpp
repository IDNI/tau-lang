// Reference shape of a host driving a synthesized reactive program through
// the class emit_program writes (src/cpp_codegen.h). The header it includes
// comes from the library API, `build_program_desc` + `emit_program`, over
// program.tau, not from a CLI flag. tests/unit/test_cpp_codegen_program_desc.cpp
// emits it, builds this file and runs it. `make` builds spec.tau with
// `tau compile`, a complete executable with no host of its own.

#include "program.h"
#include <cstdio>

int main() {
	tau_program p;
	tau_program::inputs in;

	std::printf("step    p0  ok\n");
	std::printf("----  ----  --\n");

	for (int t = 0; t < 10; ++t) {
		auto o = p.step(in);
		std::printf("%4d  %4d  %2d\n", t, (int)o.p0, (int)o.ok);
		if (!o.ok) {
			std::printf("!! program reached unreachable state — spec bug\n");
			return 1;
		}
	}

	std::puts("\nOK: 10 ticks completed, invariants preserved.");
	return 0;
}
