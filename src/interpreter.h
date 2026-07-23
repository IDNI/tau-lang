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
#include "ltl_aba.h"

#include <functional>
#include <map>
#include <optional>
#include <vector>

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
	interpreter(htrefs& ubt_ctn, auto& original_spec, auto& output_partition,
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

	// Step with optional pointwise-revision payload `u`. When `u` is
	// non-null, the interpreter steps as usual on `values`, then runs
	// pointwise revision with `u` as the update (between turns, after the
	// commit — F6-compliant: never mid-token). Equivalent to calling
	// `step(values)` then `update(u)` in sequence.
	//
	// PWR mode is interpreter-internal; today only syntactic fast mode is
	// wired (pwr-ltl.tex §3 algorithm via `pointwise_revision_temporal`).
	// Semantic / Zielonka winning-region mode (pwr-ltl.tex §11 "optimal
	// mode") is a deferred design — the machinery (algorithm_d_game.h,
	// mealy_extract.h) exists but is not yet wired into PWR.
	std::pair<std::optional<assignment<node>>, bool> step(
						const assignment<node>& values,
						std::optional<tref> u);

	/**
	 * @brief Apply a pointwise revision to the running specification.
	 *
	 * Both the running spec and @p update must be normalized before calling.
	 * @param update Normalized update formula.
	 */
	void update(tref update);

	// ── Inspection / introspection (added for tau-neuro runtime) ─────────

	// Return the current running spec as a tau-syntax string. Reflects
	// whatever the interpreter holds in `original_spec` after any PWR
	// updates that have been applied — this is the "this[t]" view.
	std::string current_spec() const;

	// Reset the interpreter back to time t=0. Clears `memory`,
	// `time_point`, `formula_time_point`; recomputes lookback. The spec
	// (`original_spec`, `ubt_ctn`, `cached_solution`, IO streams) is
	// preserved — only the execution snapshot is reset.
	void reset();

	// Opaque identifier for the current Mealy state (or interpreter
	// snapshot if the spec has no Mealy strategy). Two states with the
	// same identifier are guaranteed equivalent for value-function lookup
	// and audit-log purposes.
	//
	// When `cached_solution` is present and the spec was multi-state
	// encoded, this returns the unique index `i` such that the auxiliary
	// one-hot bit `o__ltl_ms<i>__` is set in `memory` at the most-recently
	// committed time step. For single-state Mealy or pure-safety specs
	// (no auxiliary bits), this returns 0.
	int current_state() const;

	// Per-revision realisability pre-check: would PWR-merging `psi` with
	// the current spec keep the result realisable?
	//
	// Implementation: dry-run the same merge that `update(psi)` performs
	// (per-clause `pointwise_revision` followed by `get_executable_spec`
	// non-null gate), but don't mutate any state. Returns true iff the
	// resulting merged spec would be executable.
	//
	// Today this uses syntactic fast-mode PWR (`pointwise_revision_temporal`).
	// Semantic mode is deferred upstream (see pwr-ltl.tex §11).
	//
	// Non-const because the dry-run needs to copy the output_partition
	// union-find structure, which lacks a usable copy constructor;
	// implementation defers to the same machinery as `update()`.
	bool can_extend(tref psi);

	// Enumerate output assignments admissible at the current step without
	// advancing time. Implements plan v10 §7.1 step (b)
	// "K_t = M_Φ.admissible_outputs(q_t)" via blocking-clause solver
	// enumeration above `solve()`:
	//   1. solve(step_spec) → assignment s_1; record.
	//   2. block_1 = ∨_i ((s_1[var_i] ⊕ var_i) ≠ 0); conjunct.
	//   3. solve(step_spec ∧ block_1) → s_2; record.
	//   4. ... loop until UNSAT or max_results.
	//
	// For Mealy-synthesised specs, the strategy is already encoded into
	// `step_spec` via `o__ltl_ms<i>__` aux bits (encode_mealy_as_safety),
	// so a single uniform path covers both safety and Mealy cases.
	//
	// Caller: tau-neuro's per-token loop calls this ONCE per token to get
	// the admissibility set, then constrained-argmax over it. The returned
	// assignments are over OUTPUT stream variables; auxiliary state bits
	// (`o__ltl_ms*`, `o__ltl_s*`) are filtered via `is_excluded_output`.
	//
	// Non-const because lazy initialization of step_spec via
	// `calculate_initial_spec()` may be required.
	std::vector<assignment<node>>
	admissible_outputs(size_t max_results = 1024);

	// Read-only observability of a Def 5.7 accumulator.
	//
	// Per canonical paper §5.4 Definition 5.7, accumulators are
	// bounded-type spec-language state variables (Bool, Int[l..h],
	// Real([l,h], q), Phase, Set[T,M], List[T,M,ttl], Enum). Their update
	// function f_a runs INSIDE the synthesised Mealy (paper Def 5.7
	// "expressed in the spec language's primitives"); Python's role is
	// read-only observability.
	//
	// Implementation: look up `name` (or `acc_<name>`) in `memory` at the
	// most-recent committed time step, format the BA-element value via
	// `serialize_constant`. Returns the serialised string, or empty if
	// no accumulator with this name is found.
	std::string accumulator_state(const std::string& name) const;

	// ── Mealy-strategy introspection (cached_solution-dependent) ─────────
	//
	// All four methods below return meaningful results only when the spec
	// needed Mealy synthesis (general LTL with future operators). For
	// pure-safety / pure-past-LTL specs, they return empty / no-op
	// equivalents that document the absence of a strategy.

	// Emit the cached Mealy strategy as a Graphviz DOT graph. Useful for
	// operator audit (Approach A3 per claude-code-addendum.tex §5).
	// Returns "" when no Mealy was synthesised.
	std::string visualise_mealy_dot() const;

	// Return the cached deterministic Mealy strategy automaton produced by
	// ltlsynt at synthesis time (Approach A3). The HOA is already
	// deterministic-strategy by construction. Returns an empty automaton
	// (num_states == 0) when no Mealy was synthesised.
	HoaAutomaton determinise() const;

	// Extract up to `n` "boundary" traces — simple paths from the initial
	// Mealy state, sorted by length (longest first). For first iteration
	// this approximates the canonical paper §13.2 / addendum §13.2 notion
	// of "extremal behaviour" (longest delay before eventually fires,
	// minimum sequence between until antecedent and consequent) by
	// returning the longest distinct paths through the strategy graph.
	//
	// Each trace is a sequence of state indices (length ≥ 1, starting
	// at initial_state). Returns empty when no Mealy was synthesised.
	std::vector<std::vector<int>>
	boundary_traces(int n, int max_length = 100) const;

	// Cryptographic-approval commit for Approach A3 (operator-approved
	// committed Mealy). Records the operator's approval hash on the
	// interpreter; subsequent PWR / re-synthesis paths can check
	// `committed_approval_hash` and refuse / fork as policy dictates.
	//
	// Caller is responsible for the approval hash's structure (per
	// addendum §13.2: SHA-256 of `(timestamp, operator_id, NL_spec,
	// formula_with_atoms, boundary_trace_set, approach_choice,
	// prev_hash)`). This method only persists the string.
	void commit_realiser(const std::string& approval_hash);

	// The committed approval hash (empty if no commit_realiser call has
	// occurred). Public so downstream policy checks can read it.
	std::string committed_approval_hash;

	// ── Oracle-resolved output streams (declare_open) ────────────────────
	//
	//
	// An OracleHandler is invoked at runtime when step() encounters an
	// open output stream. It receives a serialized tau data formula F
	// characterizing the admissible values for that stream at the
	// current state, and must return a satisfying assignment serialized
	// as "var := value" (one per free variable in F). The engine
	// validates the returned assignment against F and commits it.
	//
	// F is guaranteed satisfiable by W-invariance (the engine only
	// dispatches from a winning sys-state in the parity game's winning
	// region). The handler need not check satisfiability of F itself.
	using OracleHandler = std::function<std::string(const std::string& formula)>;

	// Declare `stream_name` as an open output stream filled by `handler`.
	// Subsequent step() calls dispatch to `handler` when the value of this
	// stream needs to be chosen. Replaces any prior registration for the
	// same stream. Throws if `stream_name` is not an output stream of the
	// current spec, or if called from inside a handler invocation
	// (re-entrance violation).
	void declare_open(const std::string& stream_name, OracleHandler handler);

	// Remove a prior declaration for `stream_name`. Subsequent step()
	// calls use the committed strategy for this stream. No-op if the
	// stream was not declared open.
	void undeclare_open(const std::string& stream_name);

	// Inspectable: list currently-declared open streams (declaration
	// order preserved).
	std::vector<std::string> open_streams() const;

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

	htrefs ubt_ctn;
	/// Partition of spec each with representative for set of output streams.
	std::vector<std::pair<htref, htref>> original_spec;
	assignment<node> memory;
	size_t time_point = 0;
	input_streams<node>     inputs;
	output_streams<node>    outputs;
	io_context<node> ctx;

	// Cached LTL synthesis solution from `ltl_to_safety_formula_full` if the
	// spec needed Mealy synthesis (general LTL with future operators). Empty
	// for pure-safety / pure-past-LTL specs that bypass `solve_ltl_aba`.
	//
	// When present, downstream code can:
	//   - read sol.aut to introspect the Mealy strategy (states, edges),
	//   - correlate the runtime Mealy state with the auxiliary one-hot bits
	//     `o__ltl_ms<i>__` in `memory` (per `encode_mealy_as_safety`),
	//   - emit DOT visualisations, extract boundary traces, etc. — without
	//     re-running synthesis.
	std::optional<LtlAbaSolution<node>> cached_solution;

	// Open-stream handlers (declared via declare_open). Iteration order
	// matches insertion order via std::vector<std::string> open_streams_order_;
	// std::map gives us name-keyed lookup but loses order, so we keep a
	// parallel vector for declaration order (per design doc §4.5).
	std::map<std::string, OracleHandler> open_handlers_;
	std::vector<std::string> open_streams_order_;

	// Re-entrance guard: set true while inside an OracleHandler invocation;
	// mutating methods (step, update, declare_open, ...) check and refuse.
	bool in_oracle_handler_ = false;

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
	int_t announced_step_ = -1;

	/// Adaptive tree-node gc trigger: a sweep fires when bintree<node>::M()
	/// has both crossed the gc_min_size floor AND grown by at least
	/// gc_growth_factor since the last sweep. Set gc_growth_factor <= 0 to
	/// disable. Self-tunes across workloads — fast-growing M triggers
	/// frequent sweeps at small peak; slow-growing M sweeps rarely.
	static constexpr size_t gc_min_size      = 256;
	static constexpr double gc_growth_factor = 1.5;
	size_t m_at_last_gc = 0;
	/// @brief Run bintree<node>::gc(keep) if the trigger condition is met.
	void maybe_gc();

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

	/// @brief Return the unbounded continuation formulas at time @p t.
	trefs get_ubt_ctn_at(int_t t);

	/// @brief Compute and store the initial specification.
	bool calculate_initial_spec();

	/// @brief Build the input variable assignments required for step @p t.
	std::pair<trefs, bool> build_inputs_for_step(const size_t t);

	/// @brief Update formula @p f to reflect time point @p t.
	tref update_to_time_point(tref f, const int_t t);

	/// @brief Return `true` if all memory accesses in @p io_vars are valid.
	bool is_memory_access_valid(const auto& io_vars) const;

	/// @brief Compute and store the lookback and highest initial position.
	void compute_lookback_and_initial();

	/// @brief Evict memory entries that no future step can read.
	/// @param completed_time_point Value of `time_point` for the step
	///        that was just completed, before it was advanced.
	void prune_memory(size_t completed_time_point);

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
