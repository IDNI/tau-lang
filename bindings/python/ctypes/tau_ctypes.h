// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// C API for Python/ctypes bindings (Q40-API2).
// Exposes LTL(ABA) realizability checking and Mealy machine synthesis.

#ifndef TAU_LTL_C_API_H
#define TAU_LTL_C_API_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Return codes for tau_lang_decide.
#define TAU_LTL_REALIZABLE    0
#define TAU_LTL_UNREALIZABLE  1
#define TAU_LTL_PARSE_ERROR   2
#define TAU_LTL_INTERNAL_ERR  3

// Error codes for tau_lang_synthesize (negative = error).
#define TAU_LTL_SYNTH_UNREALIZABLE -1
#define TAU_LTL_SYNTH_PARSE_ERROR  -2
#define TAU_LTL_SYNTH_INTERNAL_ERR -3

// Decide realizability of an LTL(ABA) formula.
// The input string must be a complete tau specification ending with '.'.
// Returns one of the TAU_LTL_* constants above.
int tau_lang_decide(const char* formula);

// Optional: get a short, human-readable hint for the last failure.
// Returns a statically-allocated C string; valid until the next call.
const char* tau_lang_last_error(void);

// ---------------------------------------------------------------------------
// Mealy machine synthesis and step-by-step execution
// ---------------------------------------------------------------------------

// Synthesize a Mealy machine from an LTL(ABA) formula.
// The formula must end with '.'.
// Returns an opaque handle (>0) on success, or a negative TAU_LTL_SYNTH_*
// error code on failure. Use tau_lang_last_error() for details.
int64_t tau_lang_synthesize(const char* formula);

// Advance the Mealy machine one step.
// input_json: JSON object with input variable assignments,
//   e.g. {"i1": "{1/2}:qlt"}.  Pass "{}" or NULL for no-input specs.
// Returns a JSON string with output assignments and current state,
//   e.g. {"o1": "{3/4}:qlt", "state": 1}.
// Returns NULL on error (check tau_lang_last_error()).
// The returned pointer is valid until the next call to this function.
const char* tau_lang_mealy_step(int64_t handle, const char* input_json);

// Query the output variable names for a synthesized interpreter.
// Returns a JSON array of output variable names, e.g. ["o1", "o2"].
// This is static metadata (same for all states); it does NOT return
// state-dependent admissible values — use mealy_step() to test whether
// a particular input assignment produces a valid output.
// Returns NULL on error. Valid until the next call to this function.
const char* tau_lang_mealy_output_vars(int64_t handle);

// Query the input variable names needed for the next step.
// Returns a JSON array of input variable names, e.g. ["i1"].
// Returns NULL on error. Valid until the next call to this function.
const char* tau_lang_mealy_input_vars(int64_t handle);

// Query the current time step index.
// Returns the time_point (>=0), or -1 on error.
// Note: this is the step counter, not the internal automaton state node.
int64_t tau_lang_mealy_state(int64_t handle);

// Release resources for a synthesized machine.
void tau_lang_mealy_free(int64_t handle);

// Return the engine to a fresh state: release every synthesized machine
// (their handles become invalid), drop the definitions, empty the caches and
// free every tree node nothing holds. Returns the number of nodes freed.
int64_t tau_lang_reset(void);

// ---------------------------------------------------------------------------
// Runtime limits and BA-declared options
// ---------------------------------------------------------------------------

// A numeric runtime limit, named as the C++ api setter and getter without
// their set_/get_ prefix (max_fixpoint_steps, ltl_hoa_max_states, ...);
// tau_lang_limit_names() lists them. The value read back is the one in
// force: the one set, else the limit's TAU_* environment variable, else its
// default; 0 means unlimited where the setter reads it so.
// Returns 0, or -1 when no limit has that name.
int tau_lang_set_limit(const char* name, uint64_t value);
// Writes the value in force to *value and returns 0, or returns -1 when no
// limit has that name.
int tau_lang_get_limit(const char* name, uint64_t* value);
// JSON array of the limit names. Valid until the next call.
const char* tau_lang_limit_names(void);

// The limits that are not a count.
void tau_lang_set_gc_growth_factor(double factor);
double tau_lang_get_gc_growth_factor(void);
// A negative value falls back to TAU_LTL_TIMEOUT_SEC or 60.
void tau_lang_set_ltl_timeout_sec(int64_t seconds);
int64_t tau_lang_get_ltl_timeout_sec(void);
// "A", "B", "D" or "auto"; "" falls back to TAU_LTL_ALG.
void tau_lang_set_ltl_algorithm(const char* algorithm);
// Valid until the next call.
const char* tau_lang_get_ltl_algorithm(void);

// An option an algebra of the build declares about itself, named
// <family>-<option> (bv-defelim-max-atoms, qlt-cells-budget): a flag takes
// 0 or 1, a count its number. Each writes the value now in force to *value
// and returns 0, or returns -1 when no algebra declares the name
// (tau_lang_last_error() says why). An unlimited cap may read as 2^64 - 1.
int tau_lang_set_ba_option(const char* name, uint64_t value,
	uint64_t* now);
int tau_lang_get_ba_option(const char* name, uint64_t* value);
// JSON array of the BA-declared option names. Valid until the next call.
const char* tau_lang_ba_option_names(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TAU_LTL_C_API_H
