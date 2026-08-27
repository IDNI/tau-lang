// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// tau→C++ program compiler.
//
// Given a realizable LTL(ABA) specification, the synthesis pipeline produces
// an hoa_automaton strategy (states + labelled edges). build_program_desc()
// turns that into a program_desc; emit_program() walks it into a standalone
// C++ class implementing the strategy. A flag-only artifact has no runtime
// dependency on this tree; a witness-bearing one links tau for its factory
// expressions (program_desc::needs_tau_link).
//
// Output format (single-file header):
//
//   class tau_program {
//   public:
//     struct inputs { bool in_0; bool in_1; ... };
//     struct outputs { bool out_0; bool out_1; ...; bool ok; };
//     outputs step(const inputs& in);
//   };
//
// The `ok` flag is false when the input combination matches no outgoing edge
// from the current state — which indicates either a bug in the customer's
// environment model or a synthesis bug; never silently proceed on `!ok`.

#ifndef __IDNI__TAU__CPP_CODEGEN_H__
#define __IDNI__TAU__CPP_CODEGEN_H__

#include <cstdint>
#include <optional>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "ltl_aba.h"

namespace idni::tau_lang {

// ── Data-driven emit path ─────────────────────────────────────────────────────
//
// One non-templated emit_program() walks one description of the generated
// program (program_desc); variations are data, not separate emitters.
//
// A field is a plain flag (bool-carrier/reserved type), a witness (a real
// data BA's codegen_witness supplies a concrete value per edge), or a
// witness template (value depends on step inputs, solved at runtime by
// table_step_provider). An untyped io variable is rejected at emission.
enum class field_kind { flag, witness, witness_template };

// One field of a generated program's inputs or outputs struct.
struct field_desc {
	std::string prop;      // the HOA/atom proposition name this field answers to
	std::string cpp_name;  // sanitized C++ identifier
	field_kind kind = field_kind::flag;
};

// One outgoing transition, in program_desc terms rather than raw HOA text.
// `guard` has one entry per input field, then one per FLAG output field
// (codegen_strategy.h's matching convention); a witness output field has
// no slot there -- its value is `witness_ctors`' own tref-typed C++
// expression, or the default if absent from `witness_ctors` on this edge.
struct edge_desc {
	std::vector<std::int8_t> guard;
	int dst = 0;
	std::vector<std::pair<std::string, std::string>> witness_ctors;
	// Props of this edge's positive atoms whose value must be solved at
	// runtime (their trees are program_desc::atoms entries); consumed by the
	// table_step_provider path, unsupported by the standalone baked step().
	std::vector<std::string> witness_template_props;
	// Parallel to witness_template_props: true where that prop is a hoisted
	// positional atom's step-counter relativization, grounded at the
	// counter's own absolute step rather than formula_time_point.
	std::vector<bool> witness_template_is_counter;
};

// One relative-time data atom (sol.atoms[i]): `ground_expr` is a self-contained C++ expression of type tref rebuilding it, never re-parsed from text.
struct atom_desc {
	std::string prop;
	std::string ground_expr;
};

// One entry of the emitting process's ba-type registry snapshot, in id
// order. The emitted main replays every entry through its recipe and asserts
// the resulting id, so each baked numeric type id in the artifact resolves
// to the same type it named at emission -- regardless of what either
// process's static initialization registered first.
struct ba_type_entry {
	enum class recipe { reserved, family, syntactic };
	recipe kind = recipe::family;
	std::string name; // reserved builder / pack family / syntactic type name
	std::optional<unsigned short> param; // family instances only (bv[8])
	size_t id = 0;
};

// One real io stream of the emitted artifact, with its ba-type id (valid in
// the artifact via the ba_type_table replay). A stream not bound to a file in
// the spec (or built with no stream_ctx) keeps the console default.
struct stream_desc {
	enum class binding { console, file };
	std::string name;
	size_t ba_type = 0;
	binding bind = binding::console;
	std::string filename; // set iff bind == binding::file
};

// Everything emit_program() (the standalone step() class) and emit_main()
// (compile_spec's one artifact driver) need, built once from a solved
// LTL(ABA) strategy — the target of what used to be six separate emitters
// (prop/data/pwr/open x2).
struct program_desc {
	std::string class_name;
	int num_states = 0;
	int initial_state = 0;
	std::vector<field_desc> inputs, outputs;
	std::vector<std::vector<edge_desc>> edges;  // edges[state] = outgoing
	bool revisable = false;    // strategy table runtime-replaceable (PWR revise())
	std::vector<std::string> open_streams;       // declare_open surface
	bool needs_tau_link = false;   // emit_program(): false emits a self-contained class
	int lookback = 0;            // max relative shift across non-positional atoms
	int highest_initial_pos = 0; // highest constant position across positional atoms
	std::vector<atom_desc> atoms; // ground trefs the artifact needs: every input guard atom plus every witness_template output atom, whatever their BA type
	// Artifact surface: numeric ba-type ids are the emitting process's own,
	// made valid in the artifact by replaying ba_type_table (the full
	// registry snapshot) before anything else.
	std::vector<ba_type_entry> ba_type_table;
	std::vector<stream_desc> input_streams, output_streams;
	std::vector<std::string> flag_output_vars; // one var per flag guard slot
	std::string spec_src; // embedded verbatim for --print-spec; empty = none
};

// Builds a program_desc from a solved LTL(ABA) strategy via
// classify_output_field(); witness values come from codegen_witness,
// atom templates from codegen_constant_expr (real, non-carrier BAs only).
//
// Throws std::runtime_error when `revisable` combines with a witness-kind
// output, a witness owner declines codegen_witness for a feasible edge, an
// atom's operand is an unsupported shape or declines codegen_constant_expr,
// or an io variable is untyped.
//
// `stream_ctx`, when given, is the io_context the spec was parsed against
// (definitions<node>::instance().get_io_context() while it still holds that
// spec's bindings) -- each stream's console/file binding and filename are
// read from it by variable name, the same lookup the interpreter itself
// does (interpreter.tmpl.h's rebuild_inputs/rebuild_outputs). Null keeps
// every stream console-bound, as before.
template <NodeType node>
std::optional<program_desc> build_program_desc(
    const ltl_aba_solution<node>& sol,
    const std::string& class_name = "tau_program",
    bool revisable = false,
    const std::vector<std::string>& open_streams = {},
    const io_context<node>* stream_ctx = nullptr);

// Convenience: same, but for the purely-propositional case (no data atoms) --
// every field is a flag, so this never fails and needs no NodeType.
program_desc build_program_desc_prop(
    const hoa_automaton& aut,
    const std::vector<std::string>& input_props,
    const std::vector<std::string>& output_props,
    const std::string& class_name = "tau_program",
    bool revisable = false,
    const std::vector<std::string>& open_streams = {});

// Emit the C++ class program_desc describes. Non-templated: walks `d` only,
// names no BA and no NodeType. When `d.needs_tau_link` is false the emitted
// text is self-contained (the codegen::edge/strategy/strategy_step shape is
// inlined, not #included, so the artifact has no path dependency on this
// tree at compile time).
void emit_program(const program_desc& d, std::ostream& out);

} // namespace idni::tau_lang

#include "cpp_codegen.tmpl.h"

#endif // __IDNI__TAU__CPP_CODEGEN_H__
