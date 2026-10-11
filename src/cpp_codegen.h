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
/**
 * @brief Kind of a generated program field.
 *
 * A field is a plain flag (bool-carrier/reserved type), a witness (a real
 * data BA's codegen_witness supplies a concrete value per edge), or a
 * witness template (value depends on step inputs, solved at runtime by
 * table_step_provider). An untyped io variable is rejected at emission.
 */
enum class field_kind { flag, witness, witness_template };

/// @brief One field of a generated program's inputs or outputs struct.
struct field_desc {
	std::string prop;      // the HOA/atom proposition name this field answers to
	std::string cpp_name;  // sanitized C++ identifier
	/// how the field's value is produced
	field_kind kind = field_kind::flag;
};

/**
 * @brief One outgoing transition, in program_desc terms rather than raw HOA
 * text.
 *
 * `guard` has one entry per input field, then one per program_desc::
 * step_guard_ks entry, then one per FLAG output field (codegen_strategy.h's
 * matching convention); a witness output field has no slot there -- its
 * value is `witness_ctors`' own tref-typed C++ expression, or the default
 * if absent from `witness_ctors` on this edge.
 */
struct edge_desc {
	/// One literal per slot: 1 true, -1 false, 0 either.
	std::vector<std::int8_t> guard;
	/// destination state
	size_t dst = 0;
	/// (output prop, C++ expression of type tref) per witness output.
	std::vector<std::pair<std::string, std::string>> witness_ctors;
	// Props of this edge's atoms whose value must be solved at runtime
	// (their trees are program_desc::atoms entries); consumed by the
	// table_step_provider path, unsupported by the standalone baked step().
	std::vector<std::string> witness_template_props;
	// Parallel to witness_template_props: true where the edge asks for the
	// atom to be false.
	std::vector<bool> witness_template_negated;
	// Parallel to witness_template_props: true where that prop is a hoisted
	// positional atom's step-counter relativization, grounded at the
	// counter's own absolute step rather than formula_time_point.
	std::vector<bool> witness_template_is_counter;
};

/// @brief One relative-time data atom (sol.atoms[i]): `ground_expr` is a
/// self-contained C++ expression of type tref rebuilding it, never
/// re-parsed from text.
struct atom_desc {
	/// the atom's proposition name
	std::string prop;
	/// C++ expression of type tref
	std::string ground_expr;
};

/**
 * @brief One entry of the emitting process's ba-type registry snapshot, in
 * id order.
 *
 * The emitted main replays every entry through its recipe and asserts
 * the resulting id, so each baked numeric type id in the artifact resolves
 * to the same type it named at emission -- regardless of what either
 * process's static initialization registered first.
 */
struct ba_type_entry {
	/// How the artifact rebuilds the type: a reserved core builder, a pack
	/// family (with its parameter), or a bare syntactic type tree.
	enum class recipe { reserved, family, syntactic };
	recipe kind = recipe::family;
	std::string name; // reserved builder / pack family / syntactic type name
	std::optional<unsigned short> param; // family instances only (bv[8])
	/// the id the type had at emission
	size_t id = 0;
};

/**
 * @brief One real io stream of the emitted artifact, with its ba-type id
 * (valid in the artifact via the ba_type_table replay).
 *
 * A stream not bound to a file in the spec (or built with no stream_ctx)
 * keeps the console default.
 */
struct stream_desc {
	/// Where the stream reads or writes.
	enum class binding { console, file };
	/// the stream variable's name
	std::string name;
	/// the stream's ba-type id
	size_t ba_type = 0;
	binding bind = binding::console;
	std::string filename; // set iff bind == binding::file
};

/**
 * @brief Everything emit_program() (the standalone step() class) and
 * emit_main() (compile_spec's one artifact driver) need, built once from a
 * solved LTL(ABA) strategy.
 */
struct program_desc {
	/// identifier of the generated class
	std::string class_name;
	size_t num_states = 0;
	size_t initial_state = 0;
	/// The fields of the inputs and outputs structs; flag outputs first.
	std::vector<field_desc> inputs, outputs;
	std::vector<std::vector<edge_desc>> edges;  // edges[state] = outgoing
	bool revisable = false;    // strategy table runtime-replaceable (PWR revise())
	bool needs_tau_link = false;   // emit_program(): false emits a self-contained class
	int lookback = 0;            // max relative shift across non-positional atoms
	int highest_initial_pos = 0; // highest constant position across positional atoms
	std::vector<atom_desc> atoms; // ground trefs the artifact needs: every input guard atom plus every witness_template output atom, whatever their BA type
	// One threshold k per "__step_ge<k>" guard prop (ltl_aba_helpers.tmpl.h's
	// step_guard_prop), matched like an extra input, never an output --
	// its value (step >= k) is the artifact's own to compute.
	std::vector<int_t> step_guard_ks;
	// Artifact surface: numeric ba-type ids are the emitting process's own,
	// made valid in the artifact by replaying ba_type_table (the full
	// registry snapshot) before anything else.
	std::vector<ba_type_entry> ba_type_table;
	std::vector<stream_desc> input_streams, output_streams;
	std::vector<std::string> flag_output_vars; // one var per flag guard slot
	std::string spec_src; // embedded verbatim for --print-spec; empty = none
	// The Mealy view of a strategy of the data game (ltl_aba_solution::
	// data_game): its atoms are read at the absolute step played, and
	// `history`, ground expressions of atoms over the steps before 0, gives
	// the values the strategy starts from.
	bool data_game = false;
	std::vector<std::string> history;
};

/**
 * @brief The solution a table plays without breaking its spec.
 *
 * A table (build_program_desc, make_table_provider) picks each step's
 * values from the edge that step takes and nothing else. That keeps the
 * spec only when every claim the strategy makes about a step's outputs can
 * be met at that step, whatever the inputs and the earlier values: an atom
 * over an earlier output (`o2[t] = o1[t-1]`) otherwise ties a later step to
 * a value the abstraction's strategy never sees. Nor does a table play the
 * steps before the deepest lookback, where a part of the spec with a
 * shallower one (a step guard) already holds. A solution with a claim that
 * cannot always be met, or with such a step guard, is refined the way `run`
 * refines it: the data game is solved on the solution's game skeleton, and
 * the Mealy view of its strategy is returned instead.
 *
 * Returns `sol` itself when it is a Mealy view of the data game, or when it
 * has no step guard below its deepest lookback and each of its claims can
 * always be met. The report carries an error when the data game then gives
 * no Mealy view (it does not decide the skeleton, finds it unrealizable,
 * the machine exceeds its bounds, or the solution came from a route without
 * a game skeleton), and when a solver call fails; `run`
 * executes such a spec by solving each step.
 * @tparam node Tree node type.
 * @param sol Solved LTL(ABA) strategy.
 * @return The solution to play, or an error report naming the claim.
 */
template <NodeType node>
result<ltl_aba_solution<node>> playable_table_solution(
    const ltl_aba_solution<node>& sol);

/**
 * @brief Builds a program_desc from a solved LTL(ABA) strategy.
 *
 * Builds a program_desc from a solved LTL(ABA) strategy via
 * classify_output_field(); witness values come from codegen_witness,
 * atom templates from codegen_constant_expr (real, non-carrier BAs only).
 * The strategy described is `playable_table_solution(sol)`.
 *
 * The report carries an error when `playable_table_solution` refuses the
 * solution, when `revisable` combines with a witness-kind
 * output, a witness owner declines codegen_witness for a feasible edge, an
 * atom's operand is an unsupported shape or declines codegen_constant_expr,
 * or an io variable is untyped.
 *
 * `stream_ctx`, when given, is the io_context the spec was parsed against
 * (definitions<node>::instance().get_io_context() while it still holds that
 * spec's bindings) -- each stream's console/file binding and filename are
 * read from it by variable name, the same lookup the interpreter itself
 * does (interpreter.tmpl.h's rebuild_inputs/rebuild_outputs). Null keeps
 * every stream console-bound.
 * @tparam node Tree node type.
 * @param sol Solved LTL(ABA) strategy.
 * @param class_name Identifier of the generated class.
 * @param revisable Whether the strategy table is runtime-replaceable.
 * @param stream_ctx io_context the spec was parsed against, or null.
 * @return The description, or an error report when none could be built.
 */
template <NodeType node>
result<program_desc> build_program_desc(
    const ltl_aba_solution<node>& sol,
    const std::string& class_name = "tau_program",
    bool revisable = false,
    const io_context<node>* stream_ctx = nullptr);

/**
 * @brief Convenience: same as `build_program_desc`, but for the
 * purely-propositional case (no data atoms).
 *
 * Every field is a flag, so this never fails and needs no NodeType. An AP
 * of @p aut named by neither list becomes an extra flag output, and an edge
 * whose guard does not parse is omitted.
 * @param aut Strategy automaton.
 * @param input_props Environment-controlled proposition names.
 * @param output_props System-controlled proposition names.
 * @param class_name Identifier of the generated class.
 * @param revisable Whether the strategy table is runtime-replaceable.
 * @return The description.
 */
program_desc build_program_desc_prop(
    const hoa_automaton& aut,
    const std::vector<std::string>& input_props,
    const std::vector<std::string>& output_props,
    const std::string& class_name = "tau_program",
    bool revisable = false);

/**
 * @brief Emit the C++ class program_desc describes.
 *
 * Non-templated: walks `d` only, names no BA and no NodeType. When
 * `d.needs_tau_link` is false the emitted text is self-contained (the
 * codegen::edge/strategy/strategy_step shape is inlined, not #included, so
 * the artifact has no path dependency on this tree at compile time). Covers
 * the PWR-capable (d.revisable) shape too.
 * @param d Program description to emit.
 * @param out Stream receiving the generated source.
 * @return true once the class is written; an unsupported_operation error,
 * with nothing written, when `d` is a data-game Mealy view that reads
 * steps before 0 (a lookback or a history) or has a witness_template
 * output, which only the interpreter's table step provider can drive.
 */
result<bool> emit_program(const program_desc& d, std::ostream& out);

} // namespace idni::tau_lang

#include "cpp_codegen.tmpl.h"

#endif // __IDNI__TAU__CPP_CODEGEN_H__
