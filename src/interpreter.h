// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file interpreter.h
 * @brief Step-by-step execution interpreter for normalized Tau specifications.
 *
 * `interpreter<node>` drives the I/O loop: it reads input streams, solves
 * the current specification step, and writes output streams. `run()` is the
 * convenience entry point for a bounded or unbounded execution.
 */

#ifndef __IDNI__TAU__INTERPRETER_H__
#define __IDNI__TAU__INTERPRETER_H__

#include "solver.h"

namespace idni::tau_lang {

/** @brief Map from variables to their assigned tree values (solution map). */
template <NodeType node>
using assignment = subtree_map<node, tref>;

/** @brief System of equations to solve, keyed by BA type identifier. */
using system = std::map<size_t, tref>;

template <NodeType node>
struct api;

/**
 * @brief Step-by-step interpreter for a normalized Tau specification.
 *
 * Manages an I/O context, solves the specification at each time point, and
 * reads/writes streams. Construct via `make_interpreter` rather than directly.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct interpreter {
	using tau = tree<node>;
	using tt = tau::traverser;
	friend struct api<node>;

	/**
	 * @brief Construct with the given components (prefer `make_interpreter`).
	 * @param ubt_ctn Unbounded continuation formulas.
	 * @param original_spec Specification partition (formula, representative pairs).
	 * @param output_partition Union-find structure over output stream groups.
	 * @param memory Current memory (variable-to-value map).
	 * @param ctx I/O context for reading/writing streams.
	 */
	interpreter(trefs& ubt_ctn, auto& original_spec, auto& output_partition,
		assignment<node>& memory, const io_context<node>& ctx);

	/**
	 * @brief Build an interpreter from a normalized Tau specification.
	 * @param spec Normalized Tau formula.
	 * @param ctx I/O context.
	 * @return Initialized interpreter, or `std::nullopt` if the spec is unsatisfiable.
	 */
	static std::optional<interpreter> make_interpreter(tref spec,
		const io_context<node>& ctx);

	/**
	 * @brief Execute one time step without providing explicit input values.
	 * @return Pair (output assignment if successful, whether execution should continue).
	 */
	std::pair<std::optional<assignment<node>>, bool> step();

	/**
	 * @brief Execute one time step with the given input @p values.
	 * @param values Input variable assignments for this step.
	 * @return Pair (output assignment if successful, whether execution should continue).
	 */
	std::pair<std::optional<assignment<node>>, bool> step(
						const assignment<node>& values);

	/**
	 * @brief Apply a pointwise revision to the running specification.
	 *
	 * Both the running spec and @p update must be normalized before calling.
	 * @param update Normalized update formula.
	 */
	void update(tref update);

	trefs ubt_ctn;
	/// Partition of spec each with representative for set of output streams.
	std::vector<std::pair<tref, tref>> original_spec;
	assignment<node> memory;
	size_t time_point = 0;
	input_streams<node>     inputs;
	output_streams<node>    outputs;
	io_context<node> ctx;

private:
	static bool stream_comp(tref s1, tref s2) {
		return tau::subtree_less(s1, s2);
	};
	union_find_with_sets<decltype(stream_comp), node> output_partition;
	trefs step_spec;
	bool final_system = false;
	size_t formula_time_point = 0;
	int_t highest_initial_pos = 0;
	int_t lookback = 0;

	/// @brief Partition @p spec by output stream representatives.
	static std::vector<std::pair<tref, tref>>
	create_spec_partition(tref spec, auto& output_partition);

	/// @brief Read input variables at the given @p time_step.
	std::pair<std::optional<assignment<node>>, bool> read(
		const trefs& in_vars, size_t time_step);
	/// @brief Write output assignments to the output context.
	bool write(const assignment<node>& outputs);
	/// @brief Serialize @p constant of @p type to @p ss.
	bool serialize_constant(std::stringstream& ss, tref constant,
		size_t type) const;
	/// @brief Rebuild the input stream map from @p current_inputs.
	void rebuild_inputs(const subtree_map<node, size_t>& current_inputs);
	/// @brief Rebuild the output stream map from @p current_outputs.
	void rebuild_outputs(const subtree_map<node, size_t>& current_outputs);

	/// @brief Collect all input stream variables from @p dnf into @p current_inputs.
	bool collect_input_streams(tref dnf, subtree_map<node, size_t>& current_inputs);
	/// @brief Return the set of input stream variables present in @p dnf.
	subtree_map<node, size_t> collect_input_streams(tref dnf);
	/// @brief Collect all output stream variables from @p dnf into @p current_outputs.
	bool collect_output_streams(tref dnf, subtree_map<node, size_t>& current_outputs);
	/// @brief Return the set of output stream variables present in @p dnf.
	subtree_map<node, size_t> collect_output_streams(tref dnf);

	/// @brief Return the unbounded continuation formulas at time @p t.
	trefs get_ubt_ctn_at(int_t t);

	/// @brief Compute and store the initial specification.
	bool calculate_initial_spec();

	/// @brief Build the input variable assignments required for step @p t.
	std::pair<trefs, bool> build_inputs_for_step(const size_t t);

	/// @brief Update formula @p f to reflect time point @p t.
	tref update_to_time_point(tref f, const int_t t);

	/// @brief Return `true` if all memory accesses in @p io_vars are valid.
	bool is_memory_access_valid(const auto& io_vars);

	/// @brief Compute and store the lookback and highest initial position.
	void compute_lookback_and_initial();

	/// @brief Find an executable specification clause from DNF.
	static tref get_executable_spec(tref& clause, const size_t start_time = 0);

	/// @brief Apply the pointwise revision algorithm to produce an updated specification.
	tref pointwise_revision(tref spec, tref update, const int_t start_time);

	/// @brief Find the maximal update-stream solution for @p spec.
	std::optional<assignment<node>> solution_with_max_update(tref spec);

	/// @brief Return `true` if @p var is excluded from output.
	static bool is_excluded_output(tref var);

	/// @brief Return those variables in @p vars that appear within the lookback.
	trefs appear_within_lookback(const trefs& vars);

	/// @brief Unsqueeze `always` statements without adjusting time points.
	static tref unsqueeze_always(tref cnf_expression);

	/// @brief Dump interpreter state to @p os.
	std::ostream& dump(std::ostream& os) const;
	/// @brief Dump interpreter state to a string.
	std::string dump_to_str() const;

	template <NodeType N>
	friend std::optional<interpreter<N>> run(tref,
		const io_context<N>&, const size_t);
};

/**
 * @brief Unpack a typed Tau constant node to its value tree.
 * @tparam node Tree node type.
 * @param constant Typed constant node.
 * @return The inner value tree.
 */
template <NodeType node>
tref unpack_tau_constant(tref constant);

/**
 * @brief Run a Tau specification for at most @p steps time steps.
 *
 * Builds an interpreter, then calls `step()` repeatedly until the spec is
 * exhausted, an error occurs, or @p steps steps have been executed.
 * @tparam node Tree node type.
 * @param form Normalized Tau formula.
 * @param ctx I/O context for stream I/O.
 * @param steps Maximum number of steps (0 = unlimited).
 * @return Interpreter after execution, or `std::nullopt` if initialization failed.
 */
template <NodeType node>
std::optional<interpreter<node>> run(tref form,
	const io_context<node>& ctx, const size_t steps = 0);

} // namespace idni::tau_lang

#include "interpreter.tmpl.h"

# endif //__IDNI__TAU__INTERPRETER_H__
