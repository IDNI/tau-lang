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
	 * @brief Runtime size guard for updated specifications (I7).
	 *
	 * When nonzero, update() logs a WARNING whenever the stored
	 * specification exceeds this many printed characters. 0 disables the
	 * check. Set from the `--spec-size-warn` CLI option; a runtime
	 * parameter by policy, never a header constant.
	 */
	static inline size_t spec_size_warn_threshold = 0;

	/**
	 * @brief Runtime cap on the revision alternatives kept per spec part.
	 *
	 * Every per-update cost of the factored pointwise revision (I1) is
	 * proportional to the number of alternatives a part carries, and a
	 * conflicting update can add one each time. When nonzero and a
	 * revision produces more than this many alternatives, the first
	 * `max_revision_alts - 1` (the strongest accumulated behavior) and
	 * the last one (the newest update clause, the part's universally
	 * executable anchor) are kept and the middle preference tiers are
	 * dropped with a WARNING. 0 disables the cap. Set from the
	 * `--max-revision-alts` CLI option; a runtime parameter by policy,
	 * never a header constant.
	 */
	static inline size_t max_revision_alts = 0;

	/**
	 * @brief Adaptive tree-node gc trigger knobs.
	 *
	 * A sweep fires when bintree<node>::M() has both crossed the
	 * gc_min_size floor AND grown by at least gc_growth_factor since the
	 * last sweep. Set gc_growth_factor <= 0 to disable. Self-tunes across
	 * workloads — fast-growing M triggers frequent sweeps at small peak;
	 * slow-growing M sweeps rarely. Runtime parameters (defaults kept at
	 * the tuned 256 / 1.5), public like the two limits above so the REPL
	 * `get` printers can read them back: set via
	 * `--gc-min-size`/`--gc-growth-factor`, REPL `gcminsize`/`gcgrowth`,
	 * or `api::set_gc_min_size`/`api::set_gc_growth_factor`.
	 */
	static inline size_t gc_min_size      = 256;
	static inline double gc_growth_factor = 1.5;

	/**
	 * @brief Construct with the given components (prefer `make_interpreter`).
	 * @param ubt_ctn Per spec part, the ordered unbounded continuation
	 *        formulas of its alternatives (parallel to @p original_spec).
	 * @param original_spec Specification partition; per part an ordered
	 *        list of alternative formulas plus its representative.
	 * @param output_partition Union-find structure over output stream groups.
	 * @param memory Current memory (variable-to-value map).
	 * @param ctx I/O context for reading/writing streams.
	 */
	interpreter(std::vector<htrefs>& ubt_ctn, auto& original_spec,
		auto& output_partition,
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

	/**
	 * @brief Insert every raw tref reachable from this interpreter into @p keep.
	 *
	 * This is the walk-collect half of the gc strategy: every container
	 * that holds raw trefs (`memory`, `step_spec`, `inputs`/`outputs`
	 * keys, `output_partition`) must contribute here so that
	 * `bintree<node>::gc(keep)` does not free live nodes. htref-held
	 * state (`ubt_ctn`, `original_spec`, `ctx`) needs no walk.
	 * @param keep Set of tree nodes to preserve across gc.
	 */
	void collect_live_refs(std::unordered_set<tref>& keep) const;

	// I1 (factored spec storage): each spec part holds an ORDERED list of
	// alternative formulas, strongest first. Semantically the part is the
	// disjunction of its alternatives; operationally step() tries them in
	// order and the first solvable one wins, which implements the
	// pointwise-revision preference "follow the accumulated spec when the
	// current inputs allow it, else fall back to the newer update" without
	// ever embedding the ¬∃outs.(S∧U) guard (and thus a second copy of the
	// whole spec) into a stored formula. Stored size grows additively per
	// update instead of doubling.
	/// Per spec part, the executable continuations of its alternatives.
	std::vector<htrefs> ubt_ctn;
	/// Partition of spec: per part the ordered alternative formulas with a
	/// representative for its set of output streams.
	std::vector<std::pair<htrefs, htref>> original_spec;
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
	/// Per spec part, the alternatives' continuations at the current step.
	std::vector<trefs> step_spec;
	bool final_system = false;
	size_t formula_time_point = 0;
	int_t highest_initial_pos = 0;
	int_t lookback = 0;
	int_t announced_step_ = -1;

	size_t m_at_last_gc = 0;
	/// @brief Run bintree<node>::gc(keep) if the trigger condition is met.
	void maybe_gc();

	/// Memo for update_to_time_point, valid for a single time point:
	/// identical formulas (duplicate alternatives, repeated
	/// get_ubt_ctn_at calls in one step) are rewritten once. Cleared when
	/// t changes and before gc -- it is a pure cache whose raw trefs are
	/// deliberately NOT walked by collect_live_refs.
	std::unordered_map<tref, tref> tp_rewrite_memo_;
	int_t tp_rewrite_memo_t_ = std::numeric_limits<int_t>::min();

	/// @brief Partition @p spec by output stream representatives.
	static std::vector<std::pair<htref, htref>>
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
	/// @return false if a stream could not be found (interpretation should stop).
	bool rebuild_inputs(const subtree_map<node, size_t>& current_inputs);
	/// @brief Rebuild the output stream map from @p current_outputs.
	/// @return false if a stream could not be found (interpretation should stop).
	bool rebuild_outputs(const subtree_map<node, size_t>& current_outputs);

	/// @brief Collect all input stream variables from @p dnf into @p current_inputs.
	bool collect_input_streams(tref dnf, subtree_map<node, size_t>& current_inputs);
	/// @brief Return the set of input stream variables present in @p dnf.
	subtree_map<node, size_t> collect_input_streams(tref dnf);
	/// @brief Collect all output stream variables from @p dnf into @p current_outputs.
	bool collect_output_streams(tref dnf, subtree_map<node, size_t>& current_outputs);
	/// @brief Return the set of output stream variables present in @p dnf.
	subtree_map<node, size_t> collect_output_streams(tref dnf);

	/// @brief Return the unbounded continuation formulas at time @p t,
	/// per spec part in alternative order.
	std::vector<trefs> get_ubt_ctn_at(int_t t);

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

	/// @brief Evict memory entries that no future step can read.
	/// @param completed_time_point Value of `time_point` for the step
	///        that was just completed, before it was advanced.
	void prune_memory(size_t completed_time_point);

	/// @brief Find an executable specification clause from DNF.
	static tref get_executable_spec(tref& clause, const size_t start_time = 0);

	/// @brief Recompute the executable continuations of a part's ordered
	/// alternatives. Alternatives that are not executable are dropped from
	/// @p alts (they could never fire in step()).
	/// @return false when no alternative survives.
	static bool compute_part_continuations(htrefs& alts, htrefs& ctns,
		const size_t start_time);

	/// @brief Apply the pointwise revision algorithm to a part's ordered
	/// alternatives (I1). Conjoins @p update into every alternative; when
	/// the plain conjunction is unsat, appends the update clause as a
	/// last-resort alternative instead of embedding the guarded
	/// ¬∃outs.(S∧U) disjunction into a stored formula.
	/// @return The revised ordered alternatives, or `std::nullopt` when no
	/// update clause yields a satisfiable revision.
	std::optional<htrefs> pointwise_revision(const htrefs& alts,
		tref update, const int_t start_time);

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
