// Reference example for the codegen-side declare_open API
//
// Build:  make
// Run:    make run
//
// What this demonstrates:
//   1. Synthesise spec.tau via `tau_codegen --open o_p0` to emit
//      program.h. The emitted class gains:
//        - OracleCallback typedef
//        - register_open_oracle / unregister_open_oracle methods
//        - open_streams() / open_streams_count() static metadata
//        - admissible_values_mask(State, stream) static method
//          (per-state admissibility for declared streams)
//   2. The host (this driver) registers a Python-style "oracle"
//      callback for the declared stream o_p0. The oracle picks a
//      value from the admissibility set the runtime computes via
//      the static mask method.
//   3. Per step, the host:
//        a. Queries admissible_values_mask(state, "o_p0") to learn
//           which boolean values keep the synthesised invariant
//           satisfied at the current state.
//        b. Builds a tiny "F formula" (here: the set of admissible
//           bool values) and dispatches the oracle.
//        c. Validates the oracle's response is in the admissible set.
//        d. Calls step() to advance the runtime state machine.
//
// Note: in v1+v2-incremental the runtime's step() does NOT yet
// dispatch the oracle automatically — that's A4 v2 operational
// (deferred). The host orchestrates the dispatch externally; the
// codegen-emitted ABI gives the host all the metadata it needs.

#include "program.h"
#include <cstdio>
#include <cstring>
#include <string>

// A trivial oracle: prefer `true` when both values admissible.
// Real hosts would call into an LLM, an operator UI, an RNG, etc.
static const char* my_oracle(const char* formula, void* /*user_data*/) {
	// `formula` is a serialized tau formula in v1+ (e.g.,
	// "(o_p0 = 1) || (o_p0 = 0)"). The reference v2-incremental
	// runtime doesn't yet pass this in (registration only); when
	// the operational v2 lands, parse it here. For this demo we
	// just print and return a default value.
	std::printf("    [oracle] formula='%s'\n", formula ? formula : "(null)");
	return "o_p0 := 1";
}

int main() {
	TauProgram prog;

	// ── Step 1: register oracle ─────────────────────────────────
	int rc = prog.register_open_oracle("o_p0", my_oracle, nullptr);
	if (rc != 0) {
		std::printf("register_open_oracle failed: rc=%d\n", rc);
		return 1;
	}

	std::printf("Declared open streams: %zu\n",
	            TauProgram::open_streams_count());
	for (const char* const* p = TauProgram::open_streams(); *p; ++p) {
		std::printf("  - %s\n", *p);
	}
	std::printf("\n");

	// ── Step 2: per-step loop with externally-orchestrated dispatch ──
	std::printf("step  state  mask  chosen  ok\n");
	std::printf("----  -----  ----  ------  --\n");
	TauProgram::Inputs in;
	for (int t = 0; t < 5; ++t) {
		// Query the admissibility mask for the declared stream at
		// the current state. Bit 0 = false admissible; bit 1 =
		// true admissible. 0 = unreachable for o_p0 from this state
		// (synthesis bug or PWR-narrowed strategy); 3 = both ok.
		auto mask = TauProgram::admissible_values_mask(prog.state(),
		                                                "o_p0");

		// In a real host: build F as a tau-syntax disjunction over
		// the values the mask says are admissible, then dispatch
		// the oracle. Here we pass a synthetic formula string for
		// the demo's purposes.
		std::string F;
		if (mask & 0x1) F += "(o_p0 = 0)";
		if ((mask & 0x3) == 0x3) F += " || ";
		if (mask & 0x2) F += "(o_p0 = 1)";

		const char* chosen = my_oracle(F.c_str(), nullptr);
		// Parse "o_p0 := V" → V (very loose for demo).
		bool chose_true = chosen && std::strstr(chosen, ":= 1");
		// Validate: chosen value must be admissible per mask.
		std::uint8_t needed = chose_true ? 0x2 : 0x1;
		bool valid = (mask & needed) != 0;
		if (!valid) {
			std::printf("    [host] !! oracle returned %s but"
			            " mask=0x%x didn't admit it; aborting\n",
			            chosen, mask);
			return 2;
		}

		// Advance the runtime step machine (today the chosen value
		// is NOT injected into step — A4 v2 operational will wire
		// that. The state still advances per the synthesised
		// strategy; the oracle dispatch is observational here).
		auto out = prog.step(in);
		std::printf("%4d  %5d  0x%2x  %6s  %2d\n", t,
		            (int)prog.state(), mask,
		            chose_true ? "1" : "0", (int)out.ok);
		if (!out.ok) {
			std::printf("!! program reached unreachable state\n");
			return 1;
		}
	}

	std::puts("\nOK: 5 ticks completed; oracle dispatched per step.");
	return 0;
}
