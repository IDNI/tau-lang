// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// tau→C++ program compiler (#57).
//
// Given a realizable LTL(ABA) specification, the synthesis pipeline produces
// an HoaAutomaton strategy (states + labelled edges).  This header emits a
// standalone C++17 class implementing that strategy as a switch/case state
// machine.  The emitted code is g++ -O3 compilable and has no dependency on
// libtau *at runtime* for the core dispatch — it only needs libtau if the
// customer wants witness extraction for complex atoms (qlt comparisons,
// Boolean-algebra containment, etc.).  The minimal form treats atoms as
// boolean inputs supplied by the customer at each tick.
//
// Output format (single-file header):
//
//   class TauProgram {
//     enum State { q0, q1, ... } state_ = q0;
//   public:
//     struct Inputs { bool in_0; bool in_1; ... };
//     struct Outputs { bool out_0; bool out_1; ...; bool ok; };
//     Outputs step(const Inputs& in);
//   };
//
// The `ok` flag is false when the input combination matches no outgoing edge
// from the current state — which indicates either a bug in the customer's
// environment model or a synthesis bug; never silently proceed on `!ok`.

#ifndef __IDNI__TAU__CPP_CODEGEN_H__
#define __IDNI__TAU__CPP_CODEGEN_H__

#include <ostream>
#include <string>

#include "ltl_aba.h"

namespace idni::tau_lang {

// Emit a self-contained C++17 program class from a solved strategy.
// `class_name` is the generated class identifier; defaults to "TauProgram".
// The emitted code goes to `out`; the function does no I/O beyond that.
template <NodeType node>
void emit_cpp_program(
    const LtlAbaSolution<node>& sol,
    std::ostream& out,
    const std::string& class_name = "TauProgram");

// Convenience: same, but for the purely-propositional case (no data atoms).
void emit_cpp_program_prop(
    const HoaAutomaton& aut,
    const std::vector<std::string>& input_props,
    const std::vector<std::string>& output_props,
    std::ostream& out,
    const std::string& class_name = "TauProgram");

// Emit a PWR-capable C++17 program class.  The generated class uses a
// table-driven strategy (not hard-coded switch/case) so that a new strategy
// can be swapped in at runtime via revise().  This matches the interpreter's
// pointwise_revision() capability but in the codegen world.
//
// Output format (single-file header):
//
//   class TauProgramPwr {
//   public:
//     struct Inputs { bool i_...; };
//     struct Outputs { bool o_...; bool ok; };
//     struct Edge { std::vector<int8_t> guard; int dst; ... };
//     struct Strategy { int num_states; int initial; vector<vector<Edge>>; ... };
//     Outputs step(const Inputs& in);
//     void revise(Strategy new_strategy);
//     int state() const;
//     int revision_count() const;
//   };
//
// The `revise()` method accepts a new Strategy, validates it, and swaps
// the strategy table.  The state machine resets to the new initial state
// (matching the interpreter's behaviour where a revised spec restarts
// the unbound continuation from the current time point).
void emit_cpp_program_pwr(
    const HoaAutomaton& aut,
    const std::vector<std::string>& input_props,
    const std::vector<std::string>& output_props,
    std::ostream& out,
    const std::string& class_name = "TauProgramPwr");

// Build a Strategy initializer list (C++ source fragment) from an HoaAutomaton.
// This can be used to construct a Strategy object for revise().
void emit_strategy_initializer(
    const HoaAutomaton& aut,
    const std::vector<std::string>& input_props,
    const std::vector<std::string>& output_props,
    std::ostream& out);

// declare_open codegen support.
//
// Emit a C++17 program with C-ABI hooks for runtime oracle callbacks per
// declared-open output stream. Same shape as emit_cpp_program but with
// additional OracleCallback typedef, register_open_oracle method, and
// per-step admissibility metadata for the named open streams.
//
// V1 (this commit): emits the registration API + open_streams metadata.
// Per-step admissibility-formula construction + handler dispatch in step()
// is V2 (deferred — needs W-region table baked into the emit, see design
// doc §14.4 step 4).
//
// `open_streams`: list of output stream names (without "[t]" suffix) to
// expose as oracle-resolved.
template <NodeType node>
void emit_cpp_program_open(
    const LtlAbaSolution<node>& sol,
    const std::vector<std::string>& open_streams,
    std::ostream& out,
    const std::string& class_name = "TauProgram");

// Convenience: same for purely-propositional case.
void emit_cpp_program_open_prop(
    const HoaAutomaton& aut,
    const std::vector<std::string>& input_props,
    const std::vector<std::string>& output_props,
    const std::vector<std::string>& open_streams,
    std::ostream& out,
    const std::string& class_name = "TauProgram");

} // namespace idni::tau_lang

#include "cpp_codegen.tmpl.h"

#endif // __IDNI__TAU__CPP_CODEGEN_H__
