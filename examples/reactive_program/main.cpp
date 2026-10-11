// Reference shape of a host driving a synthesized reactive program through
// the emitted `tau_program` class (cpp_codegen's emit_program output). The
// standalone header this file includes is produced by the library API
// (`build_program_desc` + `emit_program`, src/cpp_codegen.h), not by a CLI
// flag: the `tau compile` verb (see the Makefile) emits and builds a complete
// executable instead. Keep this file as the documented consumer of the
// emitted class; `make` builds the `tau compile` executable.

#include "program.h"
#include <cstdio>

int main() {
	tau_program p;
	tau_program::inputs in;

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
