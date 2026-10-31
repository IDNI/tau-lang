// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// LTL(ABA) realizability — extends tau-lang's safety fragment to full LTL
// and CTL*.
//
// Operators: F (finally/eventually), U (until), R (release), W (weak until),
//   the past S (since) and T (trigger), and the CTL* path quantifiers A and
//   E. G (globally) reuses wff_always. X (neXt) and Y (Yesterday) are
//   subsumed by io_var time indices.
//
// Pipeline (is_ltl_aba_realizable, solve_ltl_aba):
//   - A CTL* formula is reduced to LTL first (reduce_ctl_star_to_ltl).
//   - A BA of the pack that synthesises its own formulas propositionally
//     (pack_try_propositional_synthesis) may claim the formula.
//   - Otherwise the data atoms become the propositions of an LTL skeleton,
//     each classified as input or output, with the ABA consistency
//     constraints conjoined; ltlsynt decides the skeleton and gives a
//     strategy automaton, whose edges the ABA oracle checks.
//   - The data game (ltl_aba_data_game.tmpl.h) plays the skeleton's game on
//     the data and decides what the abstraction leaves open.

#ifndef __IDNI__TAU__LTL_ABA_H__
#define __IDNI__TAU__LTL_ABA_H__

#include "normalizer.h"
#include "ltl_aba_result.h"
#include "bounded_cache.h"
#include "bounded_call.h"
#include "ocltl_phi_delta.h"
#include "boolean_algebras/nso_ba.h"
#include "logging.h"
#include "ltl_aba_limits.h"
#include "hoa_label.h"
#include "utility/hashing.h"
#include <algorithm>
#include <array>
#include <cctype>
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace idni::tau_lang {

/**
 * @brief Cap on the number of feasibility checks the k-ary positive-subset
 * walk may spend per atom group (0 = unlimited).
 *
 * The walk (`extend_consistency_positive_k_ary_walk`) is Θ(2^n) when the
 * atoms are mostly jointly feasible (nothing to prune), and each check is a
 * full safety-synthesis fixpoint. Skipping the remaining subsets keeps a
 * REALIZABLE verdict sound: a strategy edge whose guard uses a
 * jointly-infeasible combination is still caught by the per-edge oracle.
 * Once the cap fires, an UNREALIZABLE verdict is reported as UNKNOWN
 * (ltl_verdict_incomplete). Runtime parameter by policy (`--max-consistency-subsets`,
 * REPL `set maxsubsets`, `api::set_max_consistency_subsets`); 0 = unlimited.
 * Default 4096. Read through @ref max_consistency_subsets.
 */
inline size_t max_consistency_subsets_param = 4096;

/**
 * @brief Cap on the literal products the ABA oracle's exact mixed-type
 * coverage check may expand (0 = unlimited).
 *
 * The check expands `I_k ∧ ⋀_j ¬I_j` into literal products. Beyond the cap
 * it keeps the weaker syntactic-subset verdict for that product and logs;
 * an UNREALIZABLE verdict is then reported as UNKNOWN
 * (ltl_verdict_incomplete). Runtime parameter by policy (`--max-cover-products`, REPL
 * `set maxcoverproducts`, `api::set_max_cover_products`); 0 = unlimited.
 * Default 256. Read through @ref max_cover_products.
 */
inline size_t max_cover_products_param = 256;

/**
 * @brief Effective cap on the k-ary consistency subset checks per atom
 * group (0 = unlimited).
 */
inline size_t max_consistency_subsets() {
	return max_consistency_subsets_param;
}

/**
 * @brief Effective cap on the literal products of the oracle's mixed-type
 * coverage check (0 = unlimited).
 */
inline size_t max_cover_products() { return max_cover_products_param; }

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
 * @param seed Initial hash value.
 * @return @p seed mixed with every verdict-relevant limit.
 */
inline uint64_t ltl_verdict_budget_fingerprint(uint64_t seed = 0) {
	auto mix = [&seed](uint64_t v) {
		seed ^= v + grcprime + (seed << 6) + (seed >> 2);
	};
	mix(max_consistency_subsets());
	mix(max_cover_products());
	mix((size_t) ltl_timeout_sec());
	mix(std::hash<std::string>{}(ltl_algorithm_choice()));
	mix(ltl_qe_max_vars());
	mix(ltl_hoa_max_states());
	mix(ltl_guard_max_cubes());
	mix(ltl_max_refinement_rounds());
	mix(ltl_data_fixpoint_rounds());
	mix(ltl_window_max_paths());
	mix(ltl_closed_regions_timeout());
	mix(ltl_data_game_max_nodes());
	mix(ltl_data_game_max_memo());
	mix(ltl_data_game_max_combinations());
	mix(ltl_max_observations());
	mix(std::hash<std::string>{}(ltl_synth_algo_param));
	mix(std::hash<std::string>{}(ltl_decompose_param));
	mix(std::hash<std::string>{}(ltl_simplification_param));
	mix(ltl_minimize_strategy_param ? 1 : 0);
	mix(ltl_strategy_max_states());
	// the routing, not the option: a check inside a decision over finite
	// traces reads infinite ones, and its memo entries must not answer
	// the decision
	mix(ltl_finite_routing() ? 1 : 0);
	mix(ltl_compositional_param);
	mix(ltl_ctl_star_tableau_max_states());
	// a revision's game without warm-up decides other formulas than the
	// same skeleton with it
	mix(ltl_skeleton_without_warm_up ? 1 : 0);
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
 * A "data atom" is an ABA comparison (`=`, `!=`, `<`, ...) that contains
 * at least one io_var, whether or not it sits under a temporal operator.
 * Each distinct atom is assigned a fresh proposition name "p0","p1"...
 *
 * Distinct atoms are told apart by structural equality (subtree_equals).
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
	/// Name of the fresh propositional state variable.
	std::string state_var;
	/// Informational only: always false ((φ S ψ)(−1) = false);
	/// the encoding hard-codes !state_var at t=0. Kept for the
	/// explain/debug output.
	bool        initial_value;
	/// LTL constraint (Spot syntax) encoding the tester's transition.
	std::string transition;
	/// Informational only: the negation is already inlined in the
	/// expression returned by skeleton_str_with_testers; this flag merely
	/// annotates the explain/debug output for T-testers.
	bool        negate_output;
	/// An operand holds a future operator, so the value at a step depends
	/// on inputs not given yet.
	bool        reads_future = false;
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
 * Reads `io_var_direction`: the resolved direction bit (data 1=input /
 * 2=output), and for an unresolved io_var its name (`i...` or `this` =
 * input, `o...` or `u` = output). An atom with no output io_var, including
 * one with no io_var at all, counts as pure input.
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
//
// The Spot tool calls, including their exit-code convention, live in
// backends/spot/spot.h over spawn_capture (spawn_capture.h); nothing outside them
// reads a raw exit code.

/// Prop name and the text of the atom it stands for.
using ltlsynt_legend = std::vector<std::pair<std::string, std::string>>;

/**
 * @brief Invoke ltlsynt as a subprocess and return {realizable,
 * hoa_strategy_text}.
 *
 * hoa_strategy_text is non-empty only when realizable == true.
 *
 * The result carries an error (code::solver_error) when the subprocess
 * produced no verdict, and when ltlsynt cannot be found (PATH, then
 * `spot-bin`, then the Spot folder of a package).
 * Every caller merges that error into its own result rather than reading it
 * as a definite UNREALIZABLE.
 * @param ltl_formula Propositional LTL formula in Spot syntax.
 * @param input_props Names of the environment-controlled propositions.
 * @param output_props Names of the system-controlled propositions.
 * @param legend The text of the atom each prop stands for, printed with the
 * environment's counter-strategy (`ltl-witness`) on UNREALIZABLE.
 * @return {realizable, hoa_strategy_text}, or an error result.
 */
result<std::pair<bool, std::string>> call_ltlsynt(
    const std::string& ltl_formula,
    const std::vector<std::string>& input_props,
    const std::vector<std::string>& output_props,
    const ltlsynt_legend& legend);

/// call_ltlsynt with no atom text for the props.
result<std::pair<bool, std::string>> call_ltlsynt(
    const std::string& ltl_formula,
    const std::vector<std::string>& input_props,
    const std::vector<std::string>& output_props);

/**
 * @brief Whether this build can actually invoke ltlsynt.
 *
 * Delegates to the Spot backend's own cached PATH probe
 * (backends/spot/spot.h), so this and `call_ltlsynt` can never disagree
 * about whether ltlsynt is reachable.
 * @return `true` iff `ltlsynt` can be spawned.
 */
bool ltlsynt_available();

// The HOA automaton and parse_hoa live in ltl_aba_result.h.

// ── ABA oracle ────────────────────────────────────────────────────────────────

/**
 * @brief The data atom each AP stands for, matched by name.
 *
 * The one mapping from a label literal to its atom: an AP that names no data
 * atom is a bookkeeping proposition of the synthesis encoding (Algorithm A's
 * R-bits, Algorithm B's P-bits, ...) and maps to nullptr.
 * @param aps Atomic proposition names, by AP index.
 * @param atoms Data atoms and their proposition names.
 * @return The atom of each AP index, or nullptr.
 */
inline std::vector<tref> ap_atoms(const std::vector<std::string>& aps,
	const std::vector<std::pair<tref, std::string>>& atoms)
{
	std::vector<tref> out(aps.size(), nullptr);
	for (size_t i = 0; i < aps.size(); ++i)
		for (const auto& [f, name] : atoms)
			if (name == aps[i]) { out[i] = f; break; }
	return out;
}

/**
 * @brief Build the ABA conjunction of data atoms / their negations denoted
 * by a HOA transition guard label.
 *
 * Given a HOA transition guard label and the proposition→data_atom mapping,
 * build the ABA conjunction of instantiated data atoms / their negations.
 * Returns T if guard is trivially true (no propositions), F if trivially false.
 * A proposition that is no data atom (a bookkeeping proposition of the
 * synthesis encoding) drops out as T in either polarity.
 * @tparam node Tree node type.
 * @param guard_label Guard label over AP indices, e.g. "0&!1".
 * @param aps Atomic proposition names indexed as in the label.
 * @param atoms Data atoms and their proposition names.
 * @return The instantiated formula, or T / F for a trivial guard; a
 * `code::parse_error` when the label is not a HOA label (`hoa_label::read`).
 */
template <NodeType node>
result<tref> guard_to_aba(const std::string& guard_label,
    const std::vector<std::string>& aps,
    const std::vector<std::pair<tref, std::string>>& atoms);

// ── CTL* → LTL reduction (Bloem/Schewe/Khalimov, arXiv:1711.10636) ──────────
//
// A restricted form of the reduction:
//   - positive E χ becomes a fresh witness output w with G(w → χ'); with
//     input streams, one direction output per input pins w's branch;
//   - positive A χ reachable only through ∧ / G / A becomes χ';
//   - A/E in negative polarity are rewritten through their NNF duals first;
//   - every other placement is refused.
// The result is a pure LTL formula over I ∪ O ∪ W (witnesses); see
// translate_ctl_star in ltl_aba_builders.tmpl.h for the soundness argument.

/**
 * @brief Result of reducing a CTL* formula to an LTL synthesis problem.
 *
 * Reduce a CTL* formula containing A/E quantifiers to an equivalent LTL
 * synthesis problem: the reduced formula (pure LTL) and its witness
 * outputs. The witness variables classify themselves -- they are built as
 * output variables (direction bit = output), so extract_data_atoms /
 * is_pure_input_atom treat them as outputs. Do NOT additionally add
 * `witnesses` to an output set. The names (`w_<n>`) have no `o` prefix, so
 * the classification rests on the direction bit.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct ctl_star_reduction {
    /// The reduced LTL formula.
    tref ltl_formula;                    // reduced LTL formula
    /// Witness output variable names.
    std::vector<std::string> witnesses;  // witness output variable names
    /// BA type id per witness (index-aligned with `witnesses`): the pack's
    /// Boolean carrier (pack_bool_carrier_type), or the input's type for a
    /// direction output. The interpreter registers each witness as an
    /// internal output stream.
    std::vector<size_t> witness_types;
    /// false when an E witness was encoded without direction outputs while
    /// the formula has inputs (a past operator inside χ), or with one set
    /// of directions for an E asked at several steps: the encoding is then
    /// stricter than E, so an unrealizable verdict is undecided.
    bool exact = true;
    /// true when an E asked at several steps got one set of directions:
    /// the reduction with `by_tableau` decides it exactly.
    bool shared_directions = false;
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
 * @param by_tableau Encode an E asked at several steps through the tableau
 * of its path formula, which is exact, instead of with one set of
 * directions, which is smaller and exact for a realizable verdict only.
 * @param first_witness Number of the first witness, `w_<first_witness>`:
 * a reduction for a running specification starts past its witnesses.
 * @return The reduced LTL formula plus witness names and types, or an
 * error.
 */
template <NodeType node>
result<ctl_star_reduction<node>> reduce_ctl_star_to_ltl(tref fm,
	bool by_tableau = false, int first_witness = 0);

/**
 * @brief Reduce a CTL* formula to LTL as reduce_ctl_star_to_ltl does, and
 * reduce it again by tableau when one set of directions per E loses.
 *
 * A reduction with shared directions is exact for a realizable verdict
 * only, so this decides its realizability first; an unrealizable one is a
 * rejected candidate and the tableau reduction is returned instead.
 * @tparam node Tree node type.
 * @param fm CTL* formula to reduce.
 * @param first_witness Number of the first witness, as for
 * reduce_ctl_star_to_ltl.
 * @param start_time Time point the realizability check starts at.
 * @param output Whether that check prints its verdict trace.
 * @param decided When not null, receives the realizability of the
 * returned reduction when this function decided it, and nullopt
 * otherwise; an undecided check is then an error. When null, an undecided
 * check keeps the reduction with shared directions.
 * @return The reduction, or an error.
 */
template <NodeType node>
result<ctl_star_reduction<node>> reduce_ctl_star_deciding(tref fm,
	int first_witness = 0, int_t start_time = 0, bool output = false,
	std::optional<bool>* decided = nullptr);

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

/**
 * @brief True iff @p atom contains an input io_var anywhere.
 * @tparam node Tree node type.
 * @param atom Atom or formula to inspect.
 * @return `true` iff an input stream variable occurs in @p atom.
 */
template <NodeType node>
bool atom_has_any_input(tref atom);

/**
 * @brief Decide realizability of a formula that may contain CTL* operators.
 *
 * Reduces A / E / `-` with reduce_ctl_star_to_ltl first when present, then
 * runs is_ltl_aba_realizable on the result.
 * @tparam node Tree node type.
 * @param fm Formula to decide.
 * @param start_time Time point the check starts at.
 * @param output Whether to print the verdict trace.
 * @return `true` iff @p fm is realizable, or an error when undecided.
 */
template <NodeType node>
result<bool> is_ctl_star_realizable(tref fm, int_t start_time, bool output);

// ── Semantic negation ─────────────────────────────────────────────────────────
//
// Semantic negation -φ means "φ is unrealizable by the system": a closed
// statement about φ's own game, decided by φ's realizability verdict
// (determinacy) before the CTL* translation runs. Under a temporal operator
// or a path quantifier it reads as "φ, started fresh here, is unrealizable".
// A `-φ` under a data quantifier is not folded and is refused.

/**
 * @brief True iff the formula contains a `wff_semantic_neg` node.
 * @tparam node Tree node type.
 * @param fm Formula to inspect.
 * @return `true` iff a `wff_semantic_neg` node occurs anywhere in @p fm.
 */
template <NodeType node>
bool has_semantic_negation(tref fm);

// ── Explain pipeline ─────────────────────────────────────────────────────────

/**
 * @brief Print a human-readable trace of the LTL(ABA) translation pipeline.
 *
 * The value is true if realizable, false if unrealizable; an error means
 * the verdict is undecided (backend failure, timeout, or a CTL* / semantic
 * negation placement with no sound reduction) -- the caller must check
 * has_value() and must not read an error as a decided false.
 * @tparam node Tree node type.
 * @param fm Formula to explain: a wff, or a spec whose main part is one;
 *        anything else is an invalid_argument error, not a verdict.
 * @param out Stream receiving the trace.
 * @param decide When given, the verdict printed and returned: the caller's
 *        realizability check of the spec @p fm was prepared from, so the
 *        trace never answers differently from it.
 * @return `true` iff @p fm is realizable, or an error when undecided.
 */
template <NodeType node>
result<bool> ltl_explain(tref fm, std::ostream& out,
	const std::function<result<bool>()>& decide = {});

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

/**
 * @brief The parts of a conjunction that share no output, for
 * ltl-compositional.
 *
 * A clause is a top-level conjunct, which the skeleton starts at its
 * deepest lookback. An always clause is split conjunct by conjunct, each
 * conjunct under an always of its own with the literal `x[t-k] = x[t-k]`
 * on one of its streams at the clause's deepest lookback k, so it starts
 * where the clause did. Every piece joins the part of its outputs; inputs
 * join no parts.
 * @tparam node Tree node type.
 * @param fm Formula to split.
 * @return The parts, or none when @p fm has fewer than two, calls a
 * definition, has CTL* operators or reads a relative step outside a
 * temporal operator.
 */
template <NodeType node>
trefs ltl_output_groups(tref fm);

// Strategy-automaton solution returned by `solve_ltl_aba` and (via
// `ltl_to_safety_formula_full`) cached on the interpreter for runtime
// introspection of the Mealy strategy.
//
// Forward-declared here; the full struct definition lives in
// `ltl_aba_normalization.tmpl.h`. Forward-decl is enough for
// `std::optional<ltl_aba_solution<node>>` in the signatures below — the type
// only needs to be complete at instantiation sites (interpreter.tmpl.h,
// cpp_codegen.tmpl.h), which include the tmpl chain that defines it.

// A strategy of the data game (ltl_aba_data_game.tmpl.h).
template <NodeType node>
struct data_game_strategy;

// Defined in satisfiability.tmpl.h, which includes this header before it.
template <NodeType node>
bool ctn_holds_at(tref constraint, int_t time_point);

// ── Interpreter-facing helpers ───────────────────────────────────────────────
//
// Defined in ltl_aba_builders.tmpl.h / ltl_aba_normalization.tmpl.h; declared
// here so the contracts are visible without reading the template bodies.
//
// ltl_aba_solution conventions (every algorithm honours these):
//   - `aut.num_states == 0` means "realizable with no strategy automaton";
//     is_ltl_aba_realizable reports such a solution REALIZABLE without
//     running the ABA oracle; parse_hoa never produces it (a strategy text
//     with fewer than one state is refused).  `num_states == 0`
//     with `executable == true` and non-empty `const_outputs` means
//     "constant strategy": `const_formula` is the executable
//     `always(⋀ o_k = c_k)` and `const_outputs` lists (stream, "p/q").
//   - `executable == false` marks a solution that cannot be compiled into a
//     program over the user's streams (Algorithm B bookkeeping bits);
//     ltl_to_safety_formula_full solves the formula again without
//     propositional synthesis to get a strategy over the data.
//   - `atoms` carry the AP names the automaton uses (p_i on the default
//     path, d_i on Algorithms A/D); the oracle and the safety encoding match
//     by NAME, so a mismatch makes both vacuous.

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
 * @return `true` iff the conjunction is feasible under the chosen check, or
 * an error when the check could not be decided.
 */
template <NodeType node>
static result<bool> aba_feasible_dispatch(tref fm, bool pure_input,
	bool has_input);

/**
 * @brief Multi-state Mealy strategy -> always(phi) with one-hot auxiliary
 * state bits `o__ltl_ms<i>__` (see the block comment at the definition).
 * @tparam node Tree node type.
 * @param sol Strategy solution to encode.
 * @return The safety formula `always(phi)`; an error for an edge to a state
 * the automaton does not have, or a body that does not normalize.
 */
template <NodeType node>
static result<tref> encode_mealy_as_safety(const ltl_aba_solution<node>& sol);

/**
 * @brief Fixed-time constraints for the warm-up steps of a multi-state Mealy
 * encoding.
 *
 * Steps 0 .. warmup-1 run before the interpreter enforces the G body of
 * encode_mealy_as_safety; these constraints make the machine start in its
 * initial state before step 0 and take one transition per step there.
 * @tparam node Tree node type.
 * @param sol Strategy solution being encoded.
 * @param sv Names of the one-hot state-bit streams.
 * @param warmup Number of steps before the G body is enforced.
 * @return The conjunction of the per-step constraints, or nullptr when the
 * initial state is out of range.
 */
template <NodeType node>
static result<tref> encode_mealy_warmup(const ltl_aba_solution<node>& sol,
                                const std::vector<std::string>& sv,
                                int_t warmup);

/**
 * @brief Convert a realizable LTL formula to a tau-lang safety formula
 * (always(phi)) that the interpreter pipeline can execute, together with
 * the ltl_aba_solution<node> (the strategy automaton + atoms map) when one
 * was synthesised.
 *
 * Strategy:
 *   - Pure past-LTL formulas: compiled away to safety + init invariants
 *     (no Mealy synthesis needed; the solution is nullopt).
 *   - Single-state Mealy strategies: extract the self-loop output constraint,
 *     wrap in always().
 *   - Multi-state Mealy strategies: encoded as a safety formula with auxiliary
 *     one-hot state bits `o__ltl_ms<i>__` per `encode_mealy_as_safety`.
 *
 * The interpreter caches the solution to introspect the Mealy state at
 * runtime.
 *
 * The optional is empty when:
 *   - the formula was unrealizable (returned nullptr),
 *   - the formula was pure past-LTL (no synthesis needed).
 *
 * On the multi-state encoding path, the caller can read sol.aut.num_states,
 * sol.aut.edges, sol.atoms etc. without re-running synthesis.
 *
 * The third element is the pure-past compile-away's list of UNANCHORED
 * auxiliary output names (`o__ltl_s<k>__` of every S):
 * the interpreter must seed each to bv-0 at t = formula_time_point - 1
 * (seed_since_aux_bits) to enforce S(-1) = false (LA-N3).  Empty on every
 * other path (the ppLTLTT tester encoding anchors inside the skeleton).
 * @tparam node Tree node type.
 * With @p data_strategy, a formula the data game decides gets that game's
 * strategy there instead, and the first element is nullptr.
 * @param fm Normalised LTL formula.
 * @param data_strategy Optional sink for the strategy of the data game.
 * @param synthesize Synthesize a strategy also for a formula without future
 * operators, which is otherwise returned as its own safety formula.
 * @param unrealizable Optional sink, set when a procedure deciding the
 * formula exactly (the data game, or an algebra's own synthesis) proves
 * that no strategy exists.
 * @return {safety formula or nullptr, optional solution, unanchored
 * auxiliary output names}; an error when the strategy cannot be
 * synthesised or encoded as a safety formula and the data game gives none.
 */
template <NodeType node>
result<std::tuple<tref, std::optional<ltl_aba_solution<node>>,
                  std::vector<std::string>>>
ltl_to_safety_formula_full(tref fm,
	std::shared_ptr<data_game_strategy<node>>* data_strategy = nullptr,
	bool synthesize = false, bool* unrealizable = nullptr);

} // namespace idni::tau_lang

#include "ltl_aba.tmpl.h"

#endif // __IDNI__TAU__LTL_ABA_H__
