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
#include "tau_memory_budget.h"

#include <functional>
#include <map>
#include <memory>
#include <set>
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
 * @brief Pluggable source of the solution `interpreter::step` commits each step.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct step_provider {
	virtual ~step_provider() = default;

	/**
	 * @brief Produce the (unfiltered) solution for the current step.
	 * @param step_spec Spec parts to satisfy this step.
	 * @param memory Committed memory so far this step (inputs already merged in).
	 * @param time_point Current execution time point.
	 * @param formula_time_point Time point the running formula is phrased at.
	 * @return The solution, or `std::nullopt` when the step has none (for
	 * the default provider: some part of @p step_spec has no solvable
	 * path); the report carries the probes tried along the way.
	 */
	virtual result<std::optional<solution<node>>> produce(
		const trefs& step_spec, const assignment<node>& memory,
		int_t time_point, int_t formula_time_point) = 0;

	/**
	 * @brief The inputs of a step the provider reads.
	 * @param vars The inputs of the step, one io_var each.
	 * @return The ones to read, or nullopt to leave the choice to the
	 * spec's lookback filter (appear_within_lookback).
	 */
	virtual std::optional<trefs> read_set(const trefs& vars) const {
		(void)vars;
		return std::nullopt;
	}

	/// @brief The state of the strategy the provider plays, when it has one.
	virtual std::optional<size_t> strategy_state() const { return std::nullopt; }

	/// @brief Past steps the provider reads beyond the spec's own lookback.
	virtual int_t lookback() const { return 0; }

	/// @brief The last fixed step the spec the provider plays reads.
	virtual int_t highest_fixed_step() const { return 0; }

	/// @brief Starts the provider's own memory afresh (interpreter::reset).
	virtual void reset() {}

	/// @brief False when the provider cannot follow a revised spec.
	virtual bool revisable() const { return true; }
};

/// @brief Default step_provider: re-runs the general solver every step. Full
/// definition in interpreter.tmpl.h; forward-declared for make_interpreter's use.
template <NodeType node>
struct solve_step_provider;

/// @brief step_provider playing a strategy of the data game; defined in
/// interpreter.tmpl.h.
template <NodeType node>
struct data_game_step_provider;

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
	 * @brief Runtime size guard for updated specifications.
	 *
	 * When nonzero, update() logs a WARNING whenever the stored
	 * specification exceeds this many printed characters. 0 disables the
	 * check (the default). Set via `--spec-size-warn`, REPL
	 * `specsizewarn` or `api::set_spec_size_warn`; a runtime parameter by
	 * policy, never a header constant. Environment fallback
	 * `TAU_SPEC_SIZE_WARN`.
	 */
	static inline env_limit<size_t> spec_size_warn_threshold{
		"TAU_SPEC_SIZE_WARN", 0 };

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
	 * dropped with a WARNING. 0 disables the cap (the default). Set via
	 * `--max-revision-alts`, REPL `revisionalts` or
	 * `api::set_max_revision_alts`; a runtime parameter by policy, never a
	 * header constant. Environment fallback `TAU_MAX_REVISION_ALTS`.
	 */
	static inline env_limit<size_t> max_revision_alts{
		"TAU_MAX_REVISION_ALTS", 0 };

	/**
	 * @brief Definitional propagation before a step's paths are enumerated.
	 *
	 * The step formula keeps the clauses of its conditionals, and a guard
	 * reading a value the same step's definitions determine is not folded
	 * by the syntactic simplification, so `expression_paths` enumerates one
	 * path per open guard -- 2^k for k of them -- and each is normalized
	 * and solved until the first satisfiable one (GitHub #126). When on,
	 * the step normalizes the formula once, substitutes every top-level
	 * `o = c` with c a constant and repeats until no new constant appears,
	 * carrying the values into the solution; an identity on the solution
	 * set. On by default; disabled via `--step-definitional-propagation=false`,
	 * the REPL option `stepprop` or `api::set_step_definitional_propagation`.
	 */
	static inline bool definitional_propagation = true;

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
	 * or `api::set_gc_min_size`/`api::set_gc_growth_factor`. A floor of 0
	 * leaves the growth factor as the only trigger. Environment fallbacks
	 * `TAU_GC_MIN_SIZE` and `TAU_GC_GROWTH_FACTOR`. A store past its soft
	 * tref budget sweeps regardless of both.
	 */
	static inline env_limit<size_t> gc_min_size{ "TAU_GC_MIN_SIZE", 256 };
	/// Growth of bintree<node>::M() since the last sweep that triggers the
	/// next one (default 1.5); <= 0 disables the growth-triggered sweeps
	/// (see gc_min_size). Environment fallback `TAU_GC_GROWTH_FACTOR`.
	static inline env_limit<double> gc_growth_factor{
		"TAU_GC_GROWTH_FACTOR", 1.5 };

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
	 * @brief Build an interpreter from a Tau specification.
	 *
	 * Classifies the io vars against @p ctx, reduces CTL* operators,
	 * normalizes the spec (or, for game operators, synthesizes a strategy
	 * and encodes it as a safety formula), partitions it by output streams
	 * and opens its streams.
	 * @param spec Tau formula as parsed; must not be null.
	 * @param ctx I/O context.
	 * @return Initialized interpreter, or an error when @p spec is null, its
	 * CTL* reduction fails, it does not normalize, it is unsat or
	 * unrealizable, its realizability cannot be decided, or a stream cannot
	 * be typed or opened.
	 */
	static result<interpreter> make_interpreter(tref spec,
		const io_context<node>& ctx);

	/**
	 * @brief Build a table-driven interpreter with no spec-derived state.
	 *
	 * Bypasses `make_interpreter`'s normalizer/safety-encoding/partitioning
	 * pipeline: constructs directly with empty spec-side state and takes
	 * @p lookback / @p highest_initial_pos as given (they would otherwise be
	 * derived from `ubt_ctn`, which is empty here). The C++ program
	 * `tau compile` emits runs its strategy through this.
	 * @param ctx I/O context; also the source of the input/output streams.
	 * @param provider Solution source `step()` will consult each step.
	 * @param lookback Baked lookback (max relative shift across the table's atoms).
	 * @param highest_initial_pos Baked highest initial position.
	 * @param live_probe_atoms Atoms (input guards + witness templates) the
	 *        table strategy may consult, e.g. `table_step_provider::
	 *        live_probe_atoms()`; seeds `live_probe_atoms` so step()'s
	 *        input filter can tell which declared inputs a given step
	 *        actually needs, the same way the general solve path uses
	 *        `ubt_ctn` (left empty here -- see the member's doc comment).
	 * @return Initialized interpreter, or a report error if a stream in
	 *         @p ctx could not be opened.
	 */
	static result<interpreter> make_table_interpreter(
		const io_context<node>& ctx,
		std::shared_ptr<step_provider<node>> provider,
		int_t lookback, int_t highest_initial_pos = 0,
		const trefs& live_probe_atoms = {});

	/// The outputs of a step, and whether the step read an input (the
	/// caller then continues without prompting).
	using step_result = std::pair<std::optional<assignment<node>>, bool>;

	/**
	 * @brief Execute one time step, reading its inputs from the input streams.
	 *
	 * Only the inputs the step provider or the spec's lookback needs are read.
	 * @return As step(const assignment<node>&); an `invalid_state` error when
	 * an input stream has ended or a value could not be read or parsed.
	 */
	result<step_result> step();

	/**
	 * @brief Execute one time step with the given input @p values.
	 * @param values Input variable assignments for this step; stored in
	 *        `memory`.
	 * @return The step's outputs (always present on success) and whether
	 * @p values was non-empty; an `unsat` error when a spec part or the step
	 * provider finds no solution. Advances the time point on success and
	 * may run the tree-node gc (see gc_min_size).
	 *
	 * Lifetime of the returned map (IN-M1): its nodes are owned by the
	 * tree store and kept alive by the interpreter until the SECOND
	 * following step() call -- the sweep at the start of step N+1 pins
	 * step N's map, the sweep at the start of step N+2 does not. A host
	 * that needs a step's outputs for longer must serialize them (or copy
	 * the trees into its own htref-held storage) before then.
	 */
	result<step_result> step(const assignment<node>& values);

	/**
	 * @brief Apply a pointwise revision to the running specification.
	 *
	 * Both the running spec and @p update must be normalized before calling.
	 * @param update Normalized update formula.
	 * @return The verdict: true iff the update was accepted and committed;
	 *         false leaves the interpreter exactly as it was (spec,
	 *         streams, memory). The report carries the rejection reason;
	 *         no value means an internal invariant broke.
	 */
	result<bool> update(tref update);

	/**
	 * @brief The interactive stepping loop run() drives after construction.
	 *
	 * Steps until the spec is exhausted, input ends, the user quits, or
	 * @p steps steps have executed (0 = unlimited); applies `u`-stream
	 * pointwise revisions between steps. Public so a table-mode caller
	 * (make_table_interpreter) reuses the same prompting/reading/writing.
	 *
	 * A step producing no output (an input stream was asked for and given
	 * an empty line) always ends the loop immediately, for every caller,
	 * regardless of @p steps or @p quit_on_idle.
	 *
	 * When @p steps == 0 and a step needed no input (auto_continue is
	 * false), the loop is idle: with @p quit_on_idle false (the default)
	 * it logs a prompt and waits on stdin for ENTER (continue) or q/quit
	 * (stop), invoking @p idle_hook(true) before and @p idle_hook(false)
	 * after the wait so a caller (e.g. a measuring wrapper) can pause and
	 * resume its own timing around the human wait; with @p quit_on_idle
	 * true it stops immediately instead of prompting.
	 *
	 * @param quit_on_idle Stop instead of prompting when idle (steps == 0
	 *   and no input was needed this step).
	 * @param idle_hook Optional callback invoked with true when the loop
	 *   starts waiting on stdin and with false when the wait ends.
	 * @param steps Number of steps to run; 0 runs until input ends or the
	 *   user quits.
	 * @return true on a clean stop, including a user- or input-driven one;
	 *   no value, with the error in the report, when a step fails for a
	 *   reason other than awaiting input, an output fails to write, or an
	 *   update breaks an invariant. The report also carries any
	 *   pointwise-revision rejection reasons seen along the way.
	 */
	result<bool> run_loop(const size_t steps = 0, bool quit_on_idle = false,
		const std::function<void(bool)>& idle_hook = {});

	// ── Inspection / introspection ───────────────────────────────────────

	/// @brief The current running spec as a tau-syntax string.
	///
	/// Reflects whatever the interpreter holds in `original_spec` after any
	/// pointwise-revision updates -- the "this[t]" view.
	///
	/// IN-M2: a part with several revision alternatives is executed by
	/// step() as its FIRST solvable alternative, not as their disjunction.
	/// Once a step has run, this reports the alternatives that step chose;
	/// before the first step (or after an update, until the next step) it
	/// reports the disjunction, which over-approximates.
	std::string current_spec() const;

	/// @brief True once a pointwise-revision update has been committed after
	/// the Mealy strategy in `cached_solution` was synthesised (IN-N3).
	///
	/// The automaton then no longer describes the running spec, and the
	/// strategy introspection below (visualise_mealy_dot, determinise,
	/// boundary_traces) reports nothing rather than a stale machine.
	bool strategy_stale() const { return cached_solution_stale_; }

	/// @brief Reset the interpreter back to time t=0.
	///
	/// Clears `memory`, `time_point`, `formula_time_point` and the step
	/// state, resets the step provider, recomputes lookback and re-seeds the
	/// inner-S auxiliary anchors (see seed_since_aux_bits). The spec
	/// (`original_spec`, `ubt_ctn`, `cached_solution`, IO streams) is
	/// preserved -- only the execution snapshot is reset.
	/// @return The re-seeding result.
	result<void> reset();

	/// @brief Pre-populate `memory` with the strong-past anchors of the inner
	/// Since/Trigger auxiliaries (LA-N3).
	///
	/// Sets the Boolean carrier's false (pack_bool_carrier_type) for every
	/// INNER (off-spine) S/T auxiliary `o__ltl_s<k>__`
	/// in `since_aux_anchor_`, at t = formula_time_point - 1 -- the anchor
	/// S(-1) = false (equivalently T(-1) = true) that the compile-away pass
	/// cannot state in the formula without a cross-BA-type or negative-time
	/// shape (see compile_since_trigger_rec). Without it the first enforced
	/// step's `φ ∧ prev` arm lets the strategy claim a Since through phantom
	/// memory. Called by make_interpreter and by reset(); no-op when the
	/// list is empty or lookback is 0.
	/// @return An error when an anchor's io var cannot be built.
	result<void> seed_since_aux_bits();

	/// @brief Opaque identifier for the current Mealy state (or interpreter
	/// snapshot if the spec has no Mealy strategy).
	///
	/// Two states with the same identifier are equivalent for value-function
	/// lookup and audit-log purposes. A step provider that plays a strategy
	/// reports its own state. Otherwise, when `cached_solution` holds a
	/// multi-state strategy, this is the index `i` whose one-hot bit
	/// `o__ltl_ms<i>__` is set in `memory` at the last committed step (the
	/// automaton's initial state before the first step or when no bit is
	/// set). For single-state Mealy or pure-safety specs it is 0.
	/// @return The state; an error when a bit's value cannot be decided.
	result<size_t> current_state() const;

	/// @brief Whether pointwise-revising the running spec by @p psi would
	/// commit, without changing the interpreter.
	///
	/// Runs the same plan_update() as update(), so can_extend(psi) is true
	/// exactly when update(psi) would commit (IN-M7); plan_update() itself
	/// routes a run of the data game's strategy to plan_data_game_update(),
	/// for both. The one side
	/// effect both share is that unknown console streams named by @p psi get
	/// registered in the io_context during stream collection. Non-const
	/// because the dry run copies the output_partition union-find.
	/// @param psi Normalized update formula; null is trivially accepted.
	/// @return The verdict; the report carries the rejection reason, and
	/// no value means an internal invariant broke.
	result<bool> can_extend(tref psi);

	/// @brief Enumerate output assignments admissible at the current step
	/// without advancing time.
	///
	/// Blocking-clause enumeration over `solve()`:
	///   1. solve(step_spec) → assignment s_1; record.
	///   2. block_1 = ∨_i ((s_1[var_i] ⊕ var_i) ≠ 0); conjunct.
	///   3. solve(step_spec ∧ block_1) → s_2; record.
	///   4. ... loop until UNSAT or max_results.
	///
	/// For Mealy-synthesised specs, the strategy is already encoded into
	/// `step_spec` via `o__ltl_ms<i>__` aux bits (encode_mealy_as_safety),
	/// so a single uniform path covers both safety and Mealy cases. The
	/// returned assignments drop the outputs `is_excluded_output` names
	/// (auxiliary state bits and internal outputs).
	///
	/// IN-M2: for a part with several revision alternatives the constraint
	/// is the FIRST alternative that is solvable under the current memory
	/// -- the one step() would execute -- not the disjunction of all of
	/// them (which admitted outputs step() never emits).
	///
	/// Non-const because it may compute step_spec lazily
	/// (`calculate_initial_spec()`).
	/// @param max_results Maximum number of assignments to return.
	/// @return The admissible assignments. The report carries every solve()
	/// call's diagnostics; an error means the solver failed, not that the
	/// admissibility set is exhausted.
	result<std::vector<assignment<node>>>
	admissible_outputs(size_t max_results = 1024);

	/// @brief Read-only observability of an accumulator: a spec output
	/// stream holding state the strategy updates.
	///
	/// Looks up `name` (then `acc_<name>`) in `memory` at the most recent
	/// committed time step and formats its value via `serialize_constant`.
	/// @param name Name of the output stream, with or without `acc_`.
	/// @return The serialised value, or empty if no entry of that name is
	/// found; an error when entries match but none serialises.
	result<std::string> accumulator_state(const std::string& name) const;

	/// @brief Whether the data game's strategy chooses this run's outputs.
	/// Its Mealy view, when it has one, is then `cached_solution`.
	bool plays_data_game() const;

	/// @brief Return `true` if @p var is an internal output a host never
	/// sees: an auxiliary LTL state bit, a CTL* witness, a clause warm-up or
	/// an `_e`/`_f` output. Input variables are never excluded.
	static bool is_excluded_output(tref var);

	// ── Mealy-strategy introspection (cached_solution-dependent) ─────────
	//
	// All four methods below return meaningful results only when the spec
	// needed Mealy synthesis (general LTL with future operators) and the
	// strategy is not stale (strategy_stale). Otherwise they return empty /
	// no-op equivalents that document the absence of a strategy.

	/// @brief The cached Mealy strategy as a Graphviz DOT graph, for an
	/// operator audit; "" when no current strategy was synthesised.
	std::string visualise_mealy_dot() const;

	/// @brief The cached deterministic Mealy strategy automaton produced by
	/// ltlsynt at synthesis time (already deterministic by construction).
	/// @return The automaton, or an empty one (num_states == 0) when no
	/// current strategy was synthesised.
	hoa_automaton determinise() const;

	/// @brief Extract up to @p n "boundary" traces: simple paths from the
	/// initial Mealy state, longest first.
	///
	/// This approximates "extremal behaviour" (longest delay before
	/// eventually fires, minimum sequence between until antecedent and
	/// consequent) by returning the longest distinct paths through the
	/// strategy graph. Every prefix of a path is a path too; ties are
	/// ordered lexicographically by state.
	/// @param n Maximum number of traces; <= 0 returns none.
	/// @param max_length Maximum number of states per trace; <= 0 returns none.
	/// @return Sequences of state indices (length ≥ 1, starting at
	/// initial_state); empty when no current strategy was synthesised.
	std::vector<std::vector<size_t>>
	boundary_traces(int n, int max_length = 100) const;

	/// @brief Record an operator's approval hash of the committed Mealy
	/// strategy in `committed_approval_hash`.
	///
	/// Policy checks can read it and refuse or fork a later revision or
	/// re-synthesis. The caller owns the hash's structure; this method only
	/// stores the string.
	/// @param approval_hash The approval hash to store.
	void commit_realiser(const std::string& approval_hash);

	/// The committed approval hash (empty if no commit_realiser call has
	/// occurred). Public so downstream policy checks can read it.
	std::string committed_approval_hash;

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
	/// Parallel to `original_spec` (same size, same part order) -- the
	/// multi-state Mealy initial-output part pushed by make_interpreter
	/// has a representative-less entry in `original_spec` too (IN-N11).
	std::vector<htrefs> ubt_ctn;
	/// Table mode only: atoms (input guards + witness templates) a table
	/// strategy may consult, seeded by make_table_interpreter and consulted
	/// by appear_within_lookback ALONGSIDE ubt_ctn (which table mode leaves
	/// empty on purpose -- see make_table_interpreter's doc comment). Kept
	/// separate from ubt_ctn so it never reaches calculate_initial_spec /
	/// get_ubt_ctn_at, whose QE machinery table mode is built to bypass.
	htrefs live_probe_atoms;
	/// Partition of spec: per part the ordered alternative formulas with a
	/// representative for its set of output streams.
	std::vector<std::pair<htrefs, htref>> original_spec;
	/// Committed stream values (inputs read and outputs chosen), keyed by
	/// io var at a time point; pruned to what a future step can read.
	assignment<node> memory;
	/// The next step to execute.
	int_t time_point = 0;
	/// The input streams, per io var.
	input_streams<node>     inputs;
	/// The output streams, per io var.
	output_streams<node>    outputs;
	/// The I/O context the streams and the stream types come from.
	io_context<node> ctx;

	/// Cached LTL synthesis solution from `ltl_to_safety_formula_full` if the
	/// spec needed Mealy synthesis (general LTL with future operators). Empty
	/// for pure-safety / pure-past-LTL specs that bypass `solve_ltl_aba`.
	///
	/// When present, downstream code can:
	///   - read sol.aut to introspect the Mealy strategy (states, edges),
	///   - correlate the runtime Mealy state with the auxiliary one-hot bits
	///     `o__ltl_ms<i>__` in `memory` (per `encode_mealy_as_safety`),
	///   - emit DOT visualisations, extract boundary traces, etc. — without
	///     re-running synthesis.
	std::optional<ltl_aba_solution<node>> cached_solution;
	/// Backs strategy_stale().
	bool cached_solution_stale_ = false;

	/// LA-N3: auxiliary output names (`o__ltl_s<k>__`) of the inner /
	/// off-spine S operators from the pure-past compile-away, whose t=0
	/// anchor is not expressible in the safety formula itself. Set by
	/// make_interpreter from ltl_to_safety_formula_full's third result;
	/// consumed by seed_since_aux_bits() (make_interpreter and reset()).
	std::vector<std::string> since_aux_anchor_;

private:
	/// Counts applied updates; see spec_revision().
	size_t spec_revision_ = 0;

	/// Per io var, the file stream id its current stream object was opened
	/// from (entries exist for file-backed streams only). Lets
	/// `rebuild_inputs`/`rebuild_outputs` keep a file stream's object --
	/// and with it its read position / already-written content -- across
	/// the rebuilds `interpreter::update` performs after an accepted
	/// update, instead of reopening (inputs) or truncating (outputs) the
	/// file.
	subtree_map<node, size_t> input_stream_sources;
	subtree_map<node, size_t> output_stream_sources;

	/// Order of the output streams in output_partition.
	static bool stream_comp(tref s1, tref s2) {
		return tau::subtree_less(s1, s2);
	};
	/// Groups the output streams that share a spec part.
	union_find_with_sets<decltype(stream_comp), node> output_partition;
	/// Per spec part, the alternatives' continuations at the current step.
	std::vector<trefs> step_spec;
	/// True once step_spec no longer changes with the time point.
	bool final_system = false;
	/// Time point step_spec was last (re)computed for; -1 means stale.
	int_t step_spec_time_point_ = -1;
	/// Time point the running formula is phrased at (time_point + lookback).
	int_t formula_time_point = 0;
	/// Highest fixed position (o[3]) the spec reads.
	int_t highest_initial_pos = 0;
	/// Largest backward shift (o[t-k]) the spec reads.
	int_t lookback = 0;
	/// Inputs the spec names at a fixed time position, by name and time.
	std::set<std::pair<std::string, int_t>> fixed_inputs_;
	/// Last step whose "Execution step" line was logged; -1 for none.
	int_t announced_step_ = -1;

	// Freshness ledger for step()'s warm-up direct-decode fallback;
	// self-pins against GC like table_step_provider's own ledger_.
	fresh_element_ledger ledger_;

	/// Solution source step() consults; set by make_interpreter/make_table_interpreter.
	std::shared_ptr<step_provider<node>> provider_;
	/// bintree<node>::M() after the last gc sweep.
	size_t m_at_last_gc = 0;
	/// The output map returned by the previous step(); pinned through the
	/// next sweep so a host may still read it while feeding the next step
	/// (IN-M1, see step()'s lifetime note).
	assignment<node> last_outputs_;
	/// Per part, the index (into the part's alternatives) step() executed
	/// last; empty until the first step and after every update (IN-M2).
	std::vector<size_t> chosen_alt_;
	/// @brief Run bintree<node>::gc(keep) if the trigger condition is met.
	/// @param pin Optional caller-held map whose nodes must survive the
	/// sweep (collect_live_refs cannot see locals).
	void maybe_gc(const assignment<node>* pin = nullptr);

	/// Shared memory pre-population for auxiliary state bits (the Mealy
	/// one-hot bits and the LA-N3 inner-S anchors): for each (name → bit)
	/// entry whose `name[t-1]` lookback occurs in `ubt_ctn`, emplace
	/// memory[name[t = formula_time_point - 1]] := the Boolean carrier's
	/// true (bit 1) or false (bit 0). No-op when `formula_time_point` is 0
	/// (no lookback, nothing to seed) or @p bits is empty.
	result<void> seed_aux_lookback_bits(const std::map<std::string, int>& bits);

	/// @brief Everything update() needs to commit, computed without
	/// mutating the interpreter.
	struct update_plan {
		std::vector<htrefs> ubt_ctn;
		std::vector<std::pair<htrefs, htref>> spec;
		union_find_with_sets<decltype(stream_comp), node> partition;
		input_streams<node>  inputs;
		output_streams<node> outputs;
		subtree_map<node, size_t> input_sources;
		subtree_map<node, size_t> output_sources;
		std::string spec_str;
		// Set by a revision of a run of the data game's strategy: the
		// provider playing the strategy for the revised spec and its
		// Mealy view.
		std::shared_ptr<step_provider<node>> provider;
		std::optional<ltl_aba_solution<node>> solution;
		// The union-find's move constructor is explicit, so the members
		// are direct-initialized here rather than brace-aggregated.
		update_plan(std::vector<htrefs>&& c,
			std::vector<std::pair<htrefs, htref>>&& s,
			union_find_with_sets<decltype(stream_comp), node>&& p,
			input_streams<node>&& i, output_streams<node>&& o,
			subtree_map<node, size_t>&& is,
			subtree_map<node, size_t>&& os,
			std::string&& str)
			: ubt_ctn(std::move(c)), spec(std::move(s)),
			  partition(std::move(p)), inputs(std::move(i)),
			  outputs(std::move(o)), input_sources(std::move(is)),
			  output_sources(std::move(os)), spec_str(std::move(str)) {}
	};
	/// @brief Dry-run the pointwise revision of the running spec by
	/// @p update: the first update clause that yields an entirely
	/// executable revised spec (with its streams resolvable) wins.
	/// @return The plan, or a structured error/warning report when no
	///         clause does.
	result<update_plan> plan_update(tref update);

	/// @brief plan_update for a run of the data game's strategy: the
	/// running spec is revised by @p update as in plan_update, and the
	/// data game is solved again for the revised spec from the values of
	/// the steps already played.
	result<update_plan> plan_data_game_update(tref update);

	/// @brief The index of the first alternative of part @p part whose
	/// continuation is solvable at the current time point under the
	/// current memory -- the one step() would execute (IN-M2). The value
	/// is `std::nullopt` when no alternative solves; the report carries
	/// the probes tried along the way.
	result<std::optional<size_t>> first_solvable_alternative(size_t part);

	/// @brief Thin wrapper over the free solution_with_max_update,
	/// supplying this interpreter's own time_point.
	/// @param spec The formula to solve.
	/// @return As the free solution_with_max_update.
	result<assignment<node>> solution_with_max_update(tref spec);

	/// @brief The running spec as step() executes it: per part its chosen
	/// alternative when known (@p use_memory picks by solvability under
	/// the current memory, else the last step's choice), the
	/// disjunction otherwise. The report carries the per-part
	/// solvability probes.
	result<tref> executed_spec_fm(bool use_memory);
	/// The spec restricted to the alternatives the last step chose
	/// (executed_spec_fm(false)); const, reads chosen_alt_ only.
	tref chosen_spec_fm() const;

	/// @brief Drop dead and duplicate alternatives (keeping the earliest,
	/// i.e. strongest, position) and apply the max_revision_alts cap.
	/// The report carries the cap warning; the value is always present.
	static result<htrefs> finalize_alternatives(const trefs& alts);

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
	/// @return On success, the assignment (always present) and whether
	/// the stream signalled quit. On a hard read/parse failure the
	/// result carries no value; the report says why, including any
	/// child diagnostic (e.g. from ba_constants<node>::get).
	result<std::pair<std::optional<assignment<node>>, bool>> read(
		const trefs& in_vars, int_t time_step);
	/// @brief Write output assignments to the output context.
	/// @return An error and nothing written when a bdd node table filled
	/// while the outputs were computed (`bdd_node_table_exhausted`), or
	/// when a value cannot be serialized or written.
	result<bool> write(const assignment<node>& outputs);
	/// @brief Rebuild the input stream map from @p current_inputs.
	/// The report says which stream could not be opened; interpretation
	/// should stop on a valueless result.
	result<bool> rebuild_inputs(const subtree_map<node, size_t>& current_inputs);
	/// @brief Rebuild the output stream map from @p current_outputs.
	/// The report says which stream could not be opened; interpretation
	/// should stop on a valueless result.
	result<bool> rebuild_outputs(const subtree_map<node, size_t>& current_outputs);
	/// @brief Build the input stream map for @p current_inputs into
	/// @p out_inputs/@p out_sources, reusing a stream from
	/// @p previous_inputs when @p previous_sources says the same file
	/// backs the variable. Touches no member state -- callers (including
	/// a dry run such as can_extend) decide whether to keep the result.
	/// The report says which stream could not be opened or found.
	result<bool> build_inputs(const subtree_map<node, size_t>& current_inputs,
		const input_streams<node>& previous_inputs,
		const subtree_map<node, size_t>& previous_sources,
		input_streams<node>& out_inputs,
		subtree_map<node, size_t>& out_sources) const;
	/// @brief Build the output stream map for @p current_outputs; see
	/// build_inputs for the continuity/side-effect contract.
	result<bool> build_outputs(const subtree_map<node, size_t>& current_outputs,
		const output_streams<node>& previous_outputs,
		const subtree_map<node, size_t>& previous_sources,
		output_streams<node>& out_outputs,
		subtree_map<node, size_t>& out_sources) const;

	/// @brief Collect all input stream variables from @p dnf into @p current_inputs.
	/// The report names an input stream that must be typed.
	result<bool> collect_input_streams(tref dnf,
		subtree_map<node, size_t>& current_inputs);
	/// @brief Return the set of input stream variables present in @p dnf.
	result<subtree_map<node, size_t>> collect_input_streams(tref dnf);
	/// @brief Collect all output stream variables from @p dnf into @p current_outputs.
	/// The report names an output stream that must be typed.
	result<bool> collect_output_streams(tref dnf,
		subtree_map<node, size_t>& current_outputs);
	/// @brief Return the set of output stream variables present in @p dnf.
	result<subtree_map<node, size_t>> collect_output_streams(tref dnf);

	/// @brief Return the unbounded continuation formulas at time @p t,
	/// per spec part in alternative order. The report carries any
	/// alternative dropped because it did not normalize.
	result<std::vector<trefs>> get_ubt_ctn_at(int_t t);

	/// @brief Compute and store the initial specification. The report
	/// carries get_ubt_ctn_at's report. The value is whether the initial
	/// spec was calculated.
	result<bool> calculate_initial_spec();

	/// @brief The input variables step @p t reads, and whether a tau-typed
	/// `this` input stream is registered.
	std::pair<trefs, bool> build_inputs_for_step(const int_t t);

	/** @brief Return `true` if a tau-typed `this` input stream is registered. */
	bool has_this_input_stream() const;

	/// @brief Update formula @p f to reflect time point @p t (memoized;
	/// see the free-function counterpart below for callers with no
	/// interpreter instance, e.g. a step_provider).
	result<tref> update_to_time_point(tref f, const int_t t);


	/// @brief Return `true` if every io var of @p io_vars at a fixed time
	/// point below the current one has a value in `memory`.
	bool is_memory_access_valid(const auto& io_vars) const;

	/// @brief Compute and store the lookback and highest initial position.
	void compute_lookback_and_initial();

	/// @brief Evict memory entries that no future step can read.
	/// @param completed_time_point Value of `time_point` for the step
	///        that was just completed, before it was advanced.
	void prune_memory(int_t completed_time_point);

	/// @brief The executable form (unbounded continuation) of a spec clause.
	/// Solves the clause's uninterpreted constants and substitutes their
	/// model into both the clause and the result.
	/// @param clause The clause; rewritten in place with that model.
	/// @param start_time Time point the clause starts at.
	/// @return The executable formula; an error when @p clause is null,
	/// reduces to false, reads a negative fixed position, or its
	/// uninterpreted constants have no model.
	static result<tref> get_executable_spec(tref& clause, const int_t start_time = 0);

	/// @brief Recompute the executable continuations of a part's ordered
	/// alternatives. Alternatives that are not executable are dropped from
	/// @p alts (they could never fire in step()).
	/// @return An error when no alternative survives; the report carries
	/// why each dropped alternative was rejected either way.
	static result<bool> compute_part_continuations(htrefs& alts, htrefs& ctns,
		const int_t start_time);

	/// @brief Apply the pointwise revision algorithm to a part's ordered
	/// alternatives (I1). Conjoins @p update into every alternative; when
	/// the plain conjunction is unsat, appends the update clause as a
	/// last-resort alternative instead of embedding the guarded
	/// ¬∃outs.(S∧U) disjunction into a stored formula.
	/// @param check_goals Keep an alternative's sometimes clauses only
	/// when they stay satisfiable with it; false keeps them unchecked, for
	/// a caller that decides each alternative itself and falls back to
	/// the one without them.
	/// @return The revised ordered alternatives, or a value of
	/// `std::nullopt` when no update clause yields a satisfiable
	/// revision; the report carries the probes tried along the way.
	result<std::optional<htrefs>> pointwise_revision(const htrefs& alts,
		tref update, const int_t start_time, bool check_goals = true);

	/// @brief Return those variables in @p vars that still occur, after
	/// substituting `memory` and simplifying, in a continuation at some time
	/// point from the current one to `lookback` steps ahead (or in
	/// live_probe_atoms in table mode). Computes step_spec if needed.
	result<trefs> appear_within_lookback(const trefs& vars);

	/// @brief Re-fold the per-clause `always` wrappers of one partition
	/// part into a single `always`, conjoining the bodies verbatim.
	///
	/// The clauses were split from one `always` body by
	/// create_spec_partition, so they share a time frame and must not be
	/// re-aligned to a common lookback (that shift asserts the shorter
	/// clause one step before the start and makes guarded latches with an
	/// initial condition read as unsat, GitHub #100).
	static tref unsqueeze_always(tref cnf_expression);

	/// @brief Combine a spec partition into the single formula it denotes.
	static tref spec_partition_fm(
		const std::vector<std::pair<htrefs, htref>>& parts);

	/// @brief This interpreter's current specification, as a formula.
	///
	/// Recomputed from `original_spec`, so it follows every `update()`
	/// rather than being a snapshot taken at construction. This is the
	/// value the "Updated specification" log line stringifies -- both go
	/// through spec_partition_fm, so the two cannot drift apart.
	///
	/// Not the `u` output stream: `u` carries the incoming revision that
	/// `update()` merges in, this is the merged result.
	tref current_spec_fm() const;

	/// @brief Number of updates this interpreter has applied.
	///
	/// Maintained by `update()`: bumped once per applied update, never on
	/// a rejected one. Lets a caller detect that `current_spec_fm()`
	/// changed without diffing it.
	size_t spec_revision() const { return spec_revision_; }

	/// @brief Dump interpreter state (spec, step spec, memory) to @p os.
	std::ostream& dump(std::ostream& os) const;
	/// @brief Dump interpreter state to a string.
	std::string dump_to_str() const;

	template <NodeType N>
	friend result<interpreter<N>> run(tref,
		const io_context<N>&, const size_t);
};

/**
 * @brief Unpack a typed Tau constant node to its value tree.
 * @tparam node Tree node type.
 * @param constant Typed constant node, possibly wrapped.
 * @return The formula the tau constant holds; nullptr when @p constant is
 * not a BA constant.
 */
template <NodeType node>
tref unpack_tau_constant(tref constant);

/**
 * @brief Return `true` if @p fm contains a variable that must be quantified
 * but appears free (io_var streams must be declared inputs/outputs;
 * uninterpreted-constant names are allowed free).
 *
 * @tparam node Tree node type.
 * @param fm Formula to check.
 * @return `true` if a disallowed free variable (or undeclared stream) exists;
 * the report names each one in an info message.
 */
template <NodeType node>
result<bool> has_free_vars(tref fm);

/**
 * @brief Update formula @p f to reflect time point @p t.
 *
 * Free function (no interpreter state read, hence unmemoized): shared by
 * any `step_provider` (e.g. table_step_provider) that needs to phrase a
 * spec part at a time point without an interpreter instance to hand.
 * `interpreter::step` and other members use the memoized member of the
 * same name instead.
 * @tparam node Tree node type.
 * @param f Formula whose io vars are relative to the time variable.
 * @param t Time point to phrase @p f at.
 * @return @p f at @p t; an error when an io var cannot be shifted.
 */
template <NodeType node>
result<tref> update_to_time_point(tref f, const int_t t);

/**
 * @brief Ground @p atom_ref at @p formula_time_point against @p memory and
 * return its truth.
 *
 * update_to_time_point, then rewriter::replace with @p memory, then
 * normalize_non_temp: a step_provider's guard-evaluation counterpart to
 * update_to_time_point.
 * @return Whether the grounded atom normalizes to T; an error when it does
 * not normalize, so "unknown" is never read as false.
 */
template <NodeType node>
result<bool> evaluate_atom(tref atom_ref, const assignment<node>& memory,
	int_t formula_time_point);

/**
 * @brief Find the maximal update-stream solution for @p spec.
 *
 * Free function: the only interpreter state it reads is the current
 * @p time_point, passed explicitly so a `step_provider` can call it too.
 * @tparam node Tree node type.
 * @param spec The formula to solve.
 * @param time_point The step whose update stream `u` is maximized.
 * @return A solution of @p spec; an `unsat` error when it has none.
 */
template <NodeType node>
result<assignment<node>> solution_with_max_update(tref spec, int_t time_point);

/**
 * @brief Run a Tau specification for at most @p steps time steps.
 *
 * Clears the global definitions<node>, builds an interpreter with
 * make_interpreter, then drives it with run_loop(@p steps).
 * @tparam node Tree node type.
 * @param form Tau formula, as make_interpreter takes it.
 * @param ctx I/O context for stream I/O.
 * @param steps Maximum number of steps (0 = until input ends or the user
 *        quits).
 * @return Interpreter after execution, or an error if initialization or a
 * step failed.
 */
template <NodeType node>
result<interpreter<node>> run(tref form,
	const io_context<node>& ctx, const size_t steps = 0);

/**
 * @brief Find the `repl_pending_input_stream` at the bottom of @p stream's
 * wrapper chain, if any.
 *
 * Sees through the ADT grouping wrappers `interpreter<node>::rebuild_inputs`
 * can put between an io var's entry in `interpreter::inputs` and its
 * physical stream -- `adt_member_input_stream`'s shared `adt_tuple_reader`,
 * and this library's own ownership-bridging physical-stream wrapper used
 * when a stream's source (a caller's remap, or `io_context::
 * console_input_factory`) can only hand back a `shared_ptr` -- so a caller
 * that needs to introspect the actual physical stream (the REPL's
 * `continue_running`, scanning for an awaiting console stream to prompt
 * for) doesn't need to know those wrapper types exist.
 * @param stream Any input stream, as stored in `interpreter::inputs`.
 * @return The underlying `repl_pending_input_stream`, or `nullptr` if
 * @p stream isn't (transitively) one.
 */
template <NodeType node>
std::shared_ptr<repl_pending_input_stream> find_repl_pending_input(
	const std::shared_ptr<serialized_constant_input_stream>& stream);

} // namespace idni::tau_lang

#include "interpreter.tmpl.h"

#endif //__IDNI__TAU__INTERPRETER_H__
