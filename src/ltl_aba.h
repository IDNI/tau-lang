// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// LTL(ABA) Realizability — extends tau-lang's safety fragment to full LTL.
//
// Operators: F (finally/eventually), U (until), R (release), W (weak until).
//   G (globally) reuses wff_always — the existing safety pipeline handles it.
//   X (neXt) and Y (Yesterday) are subsumed by io_var time indices and are
//   therefore not needed as explicit grammar operators.
//
// Algorithm (oracle-assisted synthesis, see book.tex §6.7):
//   1. Extract data atoms (maximal temporal-operator-free subformulas).
//   2. Build the propositional LTL skeleton over abstract propositions p0,p1,...
//   3. Classify each proposition as input (only i* io_vars) or output (has o* io_var).
//   4. Call Spot's ltlsynt for propositional realizability + strategy extraction.
//   5. For each strategy automaton transition, verify ABA feasibility via tau-lang's
//      existing ABA engine (the oracle).
//   6. Return REALIZABLE iff there is an ABA-feasible accepting strategy.

#ifndef __IDNI__TAU__LTL_ABA_H__
#define __IDNI__TAU__LTL_ABA_H__

#include "normalizer.h"
#include "ltl_aba_result.h"
#include "bounded_cache.h"
#include "ocltl_phi_delta.h"
#include "boolean_algebras/nso_ba.h"
#include "logging.h"
#include "ltl_aba_limits.h"
#include <algorithm>
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <functional>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

namespace idni::tau_lang {

/**
 * @brief Cap on the number of feasibility checks the k-ary positive-subset
 * walk may spend per atom group (0 = unlimited).
 *
 * LT-17: cap on the number of ∀∃-synthesis feasibility checks the k-ary
 * positive-subset walk (`extend_consistency_positive_k_ary`) may spend per
 * atom group. The walk is Θ(2^n) when the atoms are mostly jointly feasible
 * (nothing to prune), and each check is a full safety-synthesis fixpoint.
 * Skipping the remaining subsets is sound: a strategy edge whose guard uses
 * a jointly-infeasible combination is still caught by the per-edge oracle,
 * so a fired cap can only cost completeness (a false UNREALIZABLE when
 * ltlsynt happened to pick such an edge — D3 = skip + log), never a false
 * REALIZABLE. Runtime parameter by policy (`--max-consistency-subsets`,
 * REPL `set maxsubsets`, `api::set_max_consistency_subsets`); 0 = unlimited.
 */
inline size_t max_consistency_subsets = 4096;

/**
 * @brief Cap on the literal products the ABA oracle's exact mixed-type
 * coverage check may expand (0 = unlimited).
 *
 * §13 / Batch O8: cap on the literal products the ABA oracle's exact
 * mixed-type coverage check may expand `I_k ∧ ⋀_j ¬I_j` into. Beyond the
 * cap the check keeps the (weaker, syntactic-subset) pre-O8 verdict for
 * that product and logs — a possible false UNREALIZABLE, never a false
 * REALIZABLE. Runtime parameter by policy (`--max-cover-products`, REPL
 * `set maxcoverproducts`, `api::set_max_cover_products`); 0 = unlimited.
 */
inline size_t max_cover_products = 256;

// The ltlsynt watchdog, the algorithm choice and the QE cap
// (`ltl_timeout_sec_param`, `ltl_algorithm_param`, `ltl_qe_max_vars_param`
// and their accessors) live in ltl_aba_limits.h so the qlt plugin headers,
// which are compiled before this header is complete, can read them.

/**
 * @brief Fingerprint of every runtime parameter that can change a
 * satisfiability or realizability verdict.
 *
 * The verdict memos (`is_tau_formula_sat`, `transform_to_execution`, the
 * oracle feasibility caches) are keyed on the formula only; a budget change
 * between two queries would otherwise return the first query's bounded
 * give-up as the second's answer. Each memo compares this fingerprint with
 * the one it was filled under and drops its entries when they differ.
 */
inline size_t ltl_verdict_budget_fingerprint(size_t seed = 0) {
	auto mix = [&seed](size_t v) {
		seed ^= v + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
	};
	mix(max_consistency_subsets);
	mix(max_cover_products);
	mix((size_t) ltl_timeout_sec());
	mix(std::hash<std::string>{}(ltl_algorithm_choice()));
	mix(ltl_qe_max_vars());
	mix(ltl_hoa_max_states);
	mix(ltl_guard_max_cubes);
	mix(ltl_max_refinement_rounds);
	mix(ltl_window_max_paths);
	return seed;
}

// ── Detection ────────────────────────────────────────────────────────────────

/**
 * @brief True iff the formula has an operator the safety pipeline cannot
 * decide satisfiability for.
 *
 * True iff the formula has an operator the safety pipeline cannot decide
 * satisfiability for: wff_until, wff_release, wff_weak_until, and the past operators wff_since,
 * wff_trigger (past formulas must route to the LTL pipeline's temporal testers).
 * A nested sometimes, or an always whose body contains another temporal
 * operator, is routed the same way.
 * @tparam node Tree node type.
 * @param fm Formula to inspect.
 * @return `true` iff @p fm must be routed to the LTL(ABA) pipeline.
 */
template <NodeType node>
bool sat_has_ltl_operators(tref fm);

/**
 * @brief True iff the formula has an operator whose realizability needs the
 * game.
 *
 * True iff the formula has an operator whose realizability needs the game:
 * sat_has_ltl_operators's set, plus wff_sometimes -- the safety pipeline
 * answers satisfiability, and that is a different question once the
 * environment can control an eventuality.
 * @tparam node Tree node type.
 * @param fm Formula to inspect.
 * @return `true` iff realizability of @p fm needs the synthesis game.
 */
template <NodeType node>
bool realizability_has_game_operators(tref fm);

// ── Data-atom extraction ──────────────────────────────────────────────────────

/**
 * @brief Extract the data atoms of a formula and name them "p0", "p1", ...
 *
 * A "data atom" is a maximal subtree that contains no temporal operators
 * (wff_always, wff_sometimes, wff_until, wff_release, wff_weak_until) but does contain at least
 * one io_var.  Each distinct atom is assigned a fresh proposition name "p0","p1"...
 *
 * Returns a vector of {tref, proposition_name} in discovery order.
 * The map from tref → name is built using structural equality (subtree_equals).
 * @tparam node Tree node type.
 * @param fm Formula to scan.
 * @return Pairs {atom, proposition name} in discovery order.
 */
template <NodeType node>
std::vector<std::pair<tref, std::string>> extract_data_atoms(tref fm);

// ── Propositional LTL skeleton ────────────────────────────────────────────────

/**
 * @brief Produce a Spot-parseable LTL formula string by replacing every data
 * atom with its assigned proposition name.
 *
 * Non-atom Boolean subformulas are kept.
 *
 * A CTL* node (A / E / semantic negation) with no sound propositional
 * encoding reaching the skeleton walk is a `result<T>` error; the caller
 * must check has_value() before using the skeleton.
 * @tparam node Tree node type.
 * @param fm Formula to abstract.
 * @param atoms Data atoms and their proposition names, as returned by
 * `extract_data_atoms`.
 * @return The propositional LTL skeleton in Spot syntax, or an error.
 */
template <NodeType node>
result<std::string> ltl_skeleton(tref fm,
    const std::vector<std::pair<tref, std::string>>& atoms);

// ── DFA temporal testers (ppLTLTT approach) ──────────────────────────────────

/**
 * @brief DFA temporal tester for one past-operator (S/T) subformula.
 *
 * Past-operator S/T → DFA temporal tester following Azzopardi et al. (ATVA'23).
 * Each S/T subformula is replaced by a fresh propositional state variable
 * in the skeleton.  The DFA transition + initial condition are encoded as
 * LTL constraints (G, X, propositional) that ltlsynt handles natively.
 */
struct past_temporal_tester {
	std::string state_var;
	/// Informational only (LT-15): always false ((φ S ψ)(−1) = false);
	/// the encoding hard-codes !state_var at t=0. Kept for the
	/// explain/debug output.
	bool        initial_value;
	std::string transition;
	/// Informational only (LT-15): the negation is already inlined in the
	/// expression returned by skeleton_str_with_testers; this flag merely
	/// annotates the explain/debug output for T-testers.
	bool        negate_output;
};

/**
 * @brief Build the propositional skeleton with past-operator temporal
 * testers.
 *
 * Build skeleton with temporal testers.  Returns {skeleton, testers}.
 * Caller must call append_tester_constraints() and add state vars to outputs.
 * A CTL* node with no sound propositional encoding reaching the skeleton
 * walk is a `result<T>` error; the caller must check has_value() before
 * using the returned skeleton.
 * @tparam node Tree node type.
 * @param fm Formula to abstract.
 * @param atoms Data atoms and their proposition names.
 * @return The skeleton string and the testers it introduced, or an error.
 */
template <NodeType node>
result<std::pair<std::string, std::vector<past_temporal_tester>>>
ltl_skeleton_with_testers(
    tref fm,
    const std::vector<std::pair<tref, std::string>>& atoms);

/**
 * @brief Append DFA tester constraints to the skeleton string.
 * @param skeleton Skeleton string to extend in place.
 * @param testers Testers returned by `ltl_skeleton_with_testers`.
 */
void append_tester_constraints(
    std::string& skeleton,
    const std::vector<past_temporal_tester>& testers);

// ── Input / output classification ────────────────────────────────────────────

/**
 * @brief True iff every io_var appearing in the atom is an INPUT variable.
 *
 * Primarily reads the resolved direction bit (data 1=input / 2=output);
 * only unresolved io_vars fall back to the name prefix ('o' = output).
 * Note the resolver additionally classifies `this` as input and `u` as
 * output (see tau_tree_extractors.tmpl.h); the prefix fallback here does
 * not replicate that.
 * @tparam node Tree node type.
 * @param atom Data atom to classify.
 * @return `true` iff the atom mentions no output variable.
 */
template <NodeType node>
bool is_pure_input_atom(tref atom);

// ── Spot interface ────────────────────────────────────────────────────────────

// A failed or killed ltlsynt is NOT an UNREALIZABLE answer (LT-7): every
// backend failure is a `result<T>` error, merged into the caller's own `r`.
// Every skeleton/translation walker reports the same way, via its own
// `result<T>` return.

/**
 * @brief How to read a (exit_code, stdout) pair from a Spot subprocess.
 *
 *   ok         a verdict was produced (REALIZABLE / UNREALIZABLE).
 *   not_found  the binary is not on PATH — spawn_capture maps ENOENT to 127.
 *   failed     no verdict: the TAU_LTL_TIMEOUT_SEC watchdog killed the child
 *              (exit 128 + SIGTERM = 143), the spawn itself failed (-1), the
 *              tool reported a usage/internal error, or it exited non-zero
 *              with nothing on stdout.
 */
enum class spot_exit_kind { ok, not_found, failed };

/**
 * @brief Classify the exit code and stdout of a Spot subprocess as a
 * `spot_exit_kind`.
 * @param exit_code Exit code as returned by `spawn_capture`.
 * @param out Captured stdout of the subprocess.
 * @return `not_found` for 127, `failed` for a negative code, a code >= 128,
 * any code other than 0/1, or empty output; otherwise `ok`.
 */
inline spot_exit_kind classify_spot_exit(int exit_code, const std::string& out) {
	if (exit_code == 127) return spot_exit_kind::not_found;
	// Negative: spawn_capture could not create the pipe or the process.
	// >= 128: killed by signal 128 + N — in particular 143 = SIGTERM, which
	// is exactly what the timeout watchdog sends.
	if (exit_code < 0 || exit_code >= 128) return spot_exit_kind::failed;
	// ltlsynt exits 0 on REALIZABLE and 1 on UNREALIZABLE; every other code
	// is a usage or internal error.
	if (exit_code != 0 && exit_code != 1) return spot_exit_kind::failed;
	// SY-R4: ltlsynt always prints a verdict line, so no output at all is
	// no verdict -- whatever the exit code (a 0 with empty stdout used to be
	// read as UNREALIZABLE by call_ltlsynt and as an empty game by
	// call_ltlsynt_game).
	if (out.empty()) return spot_exit_kind::failed;
	return spot_exit_kind::ok;
}

/**
 * @brief Invoke ltlsynt as a subprocess and return {realizable,
 * hoa_strategy_text}.
 *
 * hoa_strategy_text is non-empty only when realizable == true.
 *
 * The result carries an error (code::solver_error) when the subprocess
 * produced no verdict (see classify_spot_exit) -- including when ltlsynt is
 * not on PATH (IN-N1: the old {false, ""} degradation made a missing Spot
 * install print "UNREALIZABLE (propositional)" for every specification).
 * Every caller merges that error into its own result rather than reading it
 * as a definite UNREALIZABLE.
 * @param ltl_formula Propositional LTL formula in Spot syntax.
 * @param input_props Names of the environment-controlled propositions.
 * @param output_props Names of the system-controlled propositions.
 * @return {realizable, hoa_strategy_text}, or an error result.
 */
result<std::pair<bool, std::string>> call_ltlsynt(
    const std::string& ltl_formula,
    const std::vector<std::string>& input_props,
    const std::vector<std::string>& output_props);

/**
 * @brief Whether this build can actually invoke ltlsynt.
 *
 * Whether this build can actually invoke ltlsynt: always false under
 * Emscripten (no process model to spawn it with), a real PATH probe
 * otherwise. Reuses call_ltlsynt's own spawn/not-found detection, so the
 * two can never disagree about whether ltlsynt is reachable.  The probe
 * runs once per process and its answer is cached.
 * @return `true` iff `ltlsynt` can be spawned.
 */
bool ltlsynt_available();

// The HOA automaton and parse_hoa live in ltl_aba_result.h.

// ── ABA oracle ────────────────────────────────────────────────────────────────

/**
 * @brief Build the ABA conjunction of data atoms / their negations denoted
 * by a HOA transition guard label.
 *
 * Given a HOA transition guard label and the proposition→data_atom mapping,
 * build the ABA conjunction of instantiated data atoms / their negations.
 * Returns T if guard is trivially true (no propositions), F if trivially false.
 * @tparam node Tree node type.
 * @param guard_label Guard label over AP indices, e.g. "0&!1".
 * @param aps Atomic proposition names indexed as in the label.
 * @param atoms Data atoms and their proposition names.
 * @return The instantiated conjunction, or T / F for a trivial guard.
 */
template <NodeType node>
tref guard_to_aba(const std::string& guard_label,
    const std::vector<std::string>& aps,
    const std::vector<std::pair<tref, std::string>>& atoms);

// ── CTL* → LTL reduction (Bloem/Schewe/Khalimov, arXiv:1711.10636) ──────────
//
// The reduction works bottom-up on state subformulas:
//   - E χ introduces a fresh witness output variable w and adds G(w → χ_LTL)
//   - A χ is rewritten to ¬E¬χ first
// The result is a pure LTL formula over I ∪ O ∪ W (witnesses).

/// @brief State subformula context for CTL* reduction.
struct ctl_star_witness {
    tref original;       // the E χ subformula
    std::string name;    // fresh witness output name, e.g. "w_0"
    tref path_formula;   // the inner path formula χ
};

/**
 * @brief Result of reducing a CTL* formula to an LTL synthesis problem.
 *
 * Reduce a CTL* formula containing A/E quantifiers to an equivalent LTL
 * synthesis problem. Returns the reduced formula (pure LTL) and the set of
 * LT-23: the witness variables are SELF-CLASSIFYING -- they are built with
 * node::output_variable() (direction bit = output), so extract_data_atoms /
 * is_pure_input_atom already treat them as outputs and the only caller
 * (is_tau_formula_sat) rightly uses ltl_formula alone. Do NOT additionally
 * add `witnesses` to an output set; the vector is informational. Note the
 * names (`w_<n>`) have no `o` prefix, so classification rests entirely on
 * the direction bit.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct ctl_star_reduction {
    tref ltl_formula;                    // reduced LTL formula
    std::vector<std::string> witnesses;  // witness output variable names
    // IN-R6: BA type id per witness (index-aligned with `witnesses`);
    // currently always the default bv type. The interpreter uses these to
    // register each witness as an internal output stream.
    std::vector<size_t> witness_types;
};

/**
 * @brief Reduce a CTL* formula containing A/E quantifiers to an equivalent
 * LTL synthesis problem.
 *
 * Translates the formula (fresh witness output `w_<n>` per E-subformula,
 * with a `G(w -> chi)` constraint each), conjoins the collected constraints
 * onto the translated root and records one Boolean-carrier type id per
 * witness.
 * An error result means the CTL* formula has no sound LTL reduction; see
 * translate_ctl_star's refusal cases.
 * @tparam node Tree node type.
 * @param fm CTL* formula to reduce.
 * @return The reduced LTL formula plus witness names and types, or an
 * error.
 */
template <NodeType node>
result<ctl_star_reduction<node>> reduce_ctl_star_to_ltl(tref fm);

/**
 * @brief Check if a formula contains CTL* path quantifiers (A or E).
 *
 * Also true for a `wff_semantic_neg` node, which the reduction handles
 * alongside A/E.
 * @tparam node Tree node type.
 * @param fm Formula to inspect.
 * @return `true` iff a `wff_A`, `wff_E` or `wff_semantic_neg` node occurs.
 */
template <NodeType node>
bool has_ctl_star_operators(tref fm);

// ── Semantic negation ─────────────────────────────────────────────────────────
//
// Semantic negation -φ means "φ is unrealizable by the system", i.e. the
// environment can force ¬φ.  Deciding it means swapping the input/output roles
// for the subformula and checking realizability of ¬φ.
//
// THAT ROLE SWAP IS NOT IMPLEMENTED (LT-5).  `apply_semantic_negation` only
// re-wraps its argument in a `wff_semantic_neg` node; the header used to claim
// the swap happened "at the synthesis call site", but no such site exists.
// The surviving node then reached `skeleton_wff`'s default case which, because
// the subformula contains io_vars, emitted the propositional constant "1" — so
// every non-constant `-φ` was silently decided REALIZABLE regardless of φ.
//
// Until the swap exists, a `wff_semantic_neg` that survives constant folding
// is REJECTED (a `result<T>` error) rather than answered wrongly.  The
// hook-level folding of `-T` → F and `-F` → T is unaffected and keeps working.

/**
 * @brief True iff the formula contains a `wff_semantic_neg` node.
 * @tparam node Tree node type.
 * @param fm Formula to inspect.
 * @return `true` iff a `wff_semantic_neg` node occurs anywhere in @p fm.
 */
template <NodeType node>
bool has_semantic_negation(tref fm);

/**
 * @brief Wrap a formula in a `wff_semantic_neg` node.
 *
 * THE ROLE SWAP IS NOT IMPLEMENTED (LT-5): this function is an AST
 * constructor and nothing more (see the section comment above).
 * @tparam node Tree node type.
 * @param fm Formula to wrap.
 * @return `-fm` as a `wff_semantic_neg` node.
 */
template <NodeType node>
tref apply_semantic_negation(tref fm);

// ── Explain pipeline ─────────────────────────────────────────────────────────

/**
 * @brief Print a human-readable trace of the LTL(ABA) translation pipeline.
 *
 * The value is true if realizable, false if unrealizable; an error means
 * the verdict is undecided (backend failure, timeout, or a CTL* / semantic
 * negation placement with no sound reduction) -- the caller must check
 * has_value() and must not read an error as a decided false.
 * @tparam node Tree node type.
 * @param fm Formula to explain.
 * @param out Stream receiving the trace.
 * @return `true` iff @p fm is realizable, or an error when undecided.
 */
template <NodeType node>
result<bool> ltl_explain(tref fm, std::ostream& out);

// ── Main entry point ──────────────────────────────────────────────────────────

/**
 * @brief Check whether `fm` is LTL(ABA)-realizable.
 *
 * `fm` must already be normalised (i.e. have passed through the standard
 * tau-lang normaliser).  `start_time` and `output` follow the same convention
 * as `is_tau_formula_sat`.
 * @tparam node Tree node type.
 * @param fm Normalised formula to decide.
 * @param start_time Start time, as for `is_tau_formula_sat`.
 * @param output Diagnostic-output flag, as for `is_tau_formula_sat`.
 * @return `true` iff @p fm is REALIZABLE, or an error when no verdict
 * could be obtained.
 */
template <NodeType node>
result<bool> is_ltl_aba_realizable(tref fm, int_t start_time, bool output);

// Strategy-automaton solution returned by `solve_ltl_aba` and (via
// `ltl_to_safety_formula_full`) cached on the interpreter for runtime
// introspection of the Mealy strategy.
//
// Forward-declared here; the full struct definition lives in
// `ltl_aba_normalization.tmpl.h` (per upstream PR #90 god-file split).
// Forward-decl is enough for `std::optional<ltl_aba_solution<node>>` in the
// signature below — the type only needs to be complete at instantiation
// sites (interpreter.impl.h, cpp_codegen.tmpl.h), which include the tmpl
// chain that defines it.

// ── Interpreter-facing helpers (LT-29) ───────────────────────────────────────
//
// Defined in ltl_aba_builders.tmpl.h / ltl_aba_normalization.tmpl.h; declared
// here so the contracts are visible without reading the template bodies.
//
// ltl_aba_solution conventions (every algorithm honours these):
//   - `aut.num_states == 0` means "realizable with no strategy automaton";
//     is_ltl_aba_realizable reports such a solution REALIZABLE without
//     running the ABA oracle; parse_hoa never produces it (a strategy text
//     with fewer than one state is refused).  LA-10: `num_states == 0`
//     with `executable == true` and non-empty `const_outputs` means
//     "constant strategy": `const_formula` is the executable
//     `always(⋀ o_k = c_k)` and `const_outputs` lists (stream, "p/q").
//   - `executable == false` marks a solution that cannot be compiled into a
//     program over the user's streams (Algorithm B bookkeeping bits);
//     ltl_to_safety_formula_full and tau_codegen refuse it.
//   - `atoms` carry the AP names the automaton uses (p_i on the default
//     path, d_i on Algorithms A/D); the oracle and the safety encoding match
//     by NAME, so a mismatch makes both vacuous (LT-8).

/**
 * @brief Run the whole LTL(ABA) pipeline on a (normalised) formula and
 * return the strategy solution, or nullopt when the formula is
 * UNREALIZABLE.
 *
 * An error on the returned result means no verdict could be obtained (e.g.
 * a malformed HOA strategy out of ltlsynt) -- distinct from, and never
 * collapsed into, the UNREALIZABLE nullopt.
 * partial_out, when non-null, stays populated even when the return value
 * ends up std::nullopt. The default lives here: a function template cannot
 * gain a default argument in a later declaration, so the definition in
 * ltl_aba_builders.tmpl.h declares the parameter without one.
 * @tparam node Tree node type.
 * @param fm Normalised formula to solve.
 * @param partial_out Optional sink for the partial solution.
 * @return The strategy solution, `std::nullopt` when UNREALIZABLE, or an
 * error when no verdict could be obtained.
 */
template <NodeType node>
static result<std::optional<ltl_aba_solution<node>>> solve_ltl_aba(tref fm,
	ltl_aba_solution<node>* partial_out = nullptr);

/**
 * @brief Existential / synthesis feasibility dispatch for a data
 * conjunction.
 *
 * Pure-input and pure-output omcat/nlang formulas use existential
 * satisfiability, everything else the safety-synthesis fixpoint.
 * @tparam node Tree node type.
 * @param fm Data conjunction to check.
 * @param pure_input Whether @p fm mentions only input variables.
 * @param has_input Whether @p fm mentions any input variable.
 * @return `true` iff the conjunction is feasible under the chosen check.
 */
template <NodeType node>
static bool aba_feasible_dispatch(tref fm, bool pure_input, bool has_input);

/**
 * @brief Multi-state Mealy strategy -> always(phi) with one-hot auxiliary
 * state bits `o__ltl_ms<i>__` (see the block comment at the definition).
 * @tparam node Tree node type.
 * @param sol Strategy solution to encode.
 * @return The safety formula `always(phi)`.
 */
template <NodeType node>
static tref encode_mealy_as_safety(const ltl_aba_solution<node>& sol);

/**
 * @brief Initial-state conditions for the encoding above: {init bits,
 * first-step output constraint}.
 *
 * The second element is nullptr when the initial state has no outgoing
 * edge.
 * @tparam node Tree node type.
 * @param sol Strategy solution being encoded.
 * @param sv Names of the one-hot state-bit streams.
 * @return {init bits, first-step output constraint}.
 */
template <NodeType node>
static std::pair<tref,tref>
encode_mealy_initial_conditions(const ltl_aba_solution<node>& sol,
                                const std::vector<std::string>& sv);

/**
 * @brief Convert a realizable LTL formula to a tau-lang safety formula
 * (always(phi)) that the existing interpreter pipeline can execute.
 *
 * Strategy:
 *   - Pure past-LTL formulas: compiled away to safety + init invariants
 *     (no Mealy synthesis needed; `_full` returns nullopt for the solution).
 *   - Single-state Mealy strategies: extract the self-loop output constraint,
 *     wrap in always().
 *   - Multi-state Mealy strategies: encoded as a safety formula with auxiliary
 *     one-hot state bits `o__ltl_ms<i>__` per `encode_mealy_as_safety`.
 *
 * Returns nullptr if not realizable.
 * @tparam node Tree node type.
 * @param fm Normalised LTL formula.
 * @return The safety formula, or nullptr if not realizable.
 */
template <NodeType node>
tref ltl_to_safety_formula(tref fm);

/**
 * @brief Variant of `ltl_to_safety_formula` that ALSO returns the
 * ltl_aba_solution<node> (the strategy automaton + atoms map) when one was
 * synthesised.
 *
 * The interpreter caches it so that downstream code can introspect the
 * Mealy state at runtime, visualise the strategy, extract boundary traces,
 * etc. — the strategy's information is otherwise discarded after the safety
 * encoding.
 *
 * The optional is empty when:
 *   - the formula was unrealizable (returned nullptr),
 *   - the formula was pure past-LTL (no synthesis needed).
 *
 * On the multi-state encoding path, the caller can read sol.aut.num_states,
 * sol.aut.edges, sol.atoms etc. without re-running synthesis.
 *
 * The third element is the pure-past compile-away's list of UNANCHORED
 * auxiliary output names (`o__ltl_s<k>__` of every inner / off-spine S):
 * the interpreter must seed each to bv-0 at t = formula_time_point - 1
 * (seed_since_aux_bits) to enforce S(-1) = false (LA-N3).  Empty on every
 * other path (the ppLTLTT tester encoding anchors inside the skeleton).
 * @tparam node Tree node type.
 * @param fm Normalised LTL formula.
 * @return {safety formula or nullptr, optional solution, unanchored
 * auxiliary output names}.
 */
template <NodeType node>
std::tuple<tref, std::optional<ltl_aba_solution<node>>, std::vector<std::string>>
ltl_to_safety_formula_full(tref fm);

} // namespace idni::tau_lang

#include "ltl_aba.tmpl.h"

#endif // __IDNI__TAU__LTL_ABA_H__
