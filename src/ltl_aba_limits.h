// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ltl_aba_limits.h
 * @brief Runtime limits of the LTL(ABA) synthesis game machinery that the
 * game headers (algorithm_d_game.h) need before ltl_aba.h is complete.
 *
 * Every limit here is a runtime parameter by policy (CLI flag, REPL `set`,
 * `api::set_*`), never a header constant; ltl_aba.h documents the surface.
 * Each one is read through its accessor.
 */

#ifndef __IDNI__TAU__LTL_ABA_LIMITS_H__
#define __IDNI__TAU__LTL_ABA_LIMITS_H__

#include "logging.h"
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <optional>
#include <set>
#include <string>

namespace idni::tau_lang {

/**
 * @brief Largest state count accepted from an `ltlsynt` HOA strategy; a
 * larger count is treated as a garbled header (SY-R3), not an automaton.
 *
 * Runtime parameter by policy (`--ltl-hoa-max-states`, REPL
 * `set ltlhoamaxstates`, `api::set_ltl_hoa_max_states`); 0 = unlimited.
 * Default 2^22. Read through @ref ltl_hoa_max_states.
 */
inline size_t ltl_hoa_max_states_param = size_t(1) << 22;

/**
 * @brief Cap on the DNF cubes a HOA guard label may expand into in the
 * Algorithm D product game; a guard beyond it is refused (no verdict).
 *
 * Runtime parameter by policy (`--ltl-guard-max-cubes`, REPL
 * `set ltlguardmaxcubes`, `api::set_ltl_guard_max_cubes`); 0 = unlimited.
 * Default 512. Read through @ref ltl_guard_max_cubes.
 */
inline size_t ltl_guard_max_cubes_param = 512;

/**
 * @brief Cap on the ABA-oracle refinement rounds of `is_ltl_aba_realizable`:
 * each round blocks one infeasible strategy edge and re-runs `ltlsynt`. The
 * same cap bounds the fixpoint rounds of `strategy_wins_on_data`. On the cap
 * the verdict is UNKNOWN (an error), never a false answer.
 *
 * Runtime parameter by policy (`--ltl-refinement-rounds`, REPL
 * `set ltlrefinementrounds`, `api::set_ltl_max_refinement_rounds`);
 * 0 = unlimited. Default 64. Read through @ref ltl_max_refinement_rounds.
 */
inline size_t ltl_max_refinement_rounds_param = 64;

/**
 * @brief Cap on the strategy paths the window oracle examines per check
 * (`window_infeasible_paths`); a hit cap yields UNKNOWN, never a verdict.
 *
 * Runtime parameter by policy (`--ltl-window-max-paths`, REPL
 * `set ltlwindowmaxpaths`, `api::set_ltl_window_max_paths`); 0 = unlimited.
 * Default 4096. Read through @ref ltl_window_max_paths.
 */
inline size_t ltl_window_max_paths_param = 4096;

/**
 * @brief Seconds the data game may spend on regions that keep their
 * quantifiers (formula_regions::closed_type), all their questions together,
 * each taking at most a quarter of it; past either that attempt is
 * undecided, and the game with it.
 *
 * Runtime parameter by policy (`--ltl-closed-regions-timeout`, REPL
 * `set ltlclosedregionstimeout`, `api::set_ltl_closed_regions_timeout`);
 * 0 skips the attempt. Default 20. Read through
 * @ref ltl_closed_regions_timeout.
 */
inline size_t ltl_closed_regions_timeout_param = 20;

/**
 * @brief Most nodes live at once in the BDD of a data game over codes
 * (`code_regions`); a full table makes the game undecided, never a verdict.
 * A product of two bitvector streams wider than 4 bits needs millions of
 * nodes; 2^23 nodes and their tables take about 1 GB.
 *
 * Runtime parameter by policy (`--ltl-data-game-max-nodes`, REPL
 * `set ltldatagamemaxnodes`, `api::set_ltl_data_game_max_nodes`);
 * 0 = unlimited. Default 2^23. Read through @ref ltl_data_game_max_nodes.
 */
inline size_t ltl_data_game_max_nodes_param = size_t{1} << 23;

/**
 * @brief Most entries of the operation memo of a data game's BDD; the memo
 * is emptied when it reaches the cap, which costs recomputation only.
 *
 * Runtime parameter by policy (`--ltl-data-game-max-memo`, REPL
 * `set ltldatagamemaxmemo`, `api::set_ltl_data_game_max_memo`);
 * 0 = unlimited. Default 2^25. Read through @ref ltl_data_game_max_memo.
 */
inline size_t ltl_data_game_max_memo_param = size_t{1} << 25;

/**
 * @brief Most value combinations the data game tabulates for one comparison
 * its circuits do not encode (`tabulate`); past it the comparison has no
 * code, and the game falls back to formula regions or is undecided.
 *
 * Runtime parameter by policy (`--ltl-data-game-max-combinations`, REPL
 * `set ltldatagamemaxcombinations`,
 * `api::set_ltl_data_game_max_combinations`); 0 = unlimited. Default 4096.
 * Read through @ref ltl_data_game_max_combinations.
 */
inline size_t ltl_data_game_max_combinations_param = 4096;

/**
 * @brief Most observation props whose impossible joint values the skeleton
 * assumes away (`assume_observation_consistency`, 3^n feasibility checks);
 * beyond it nothing is assumed, which leaves the environment moves no data
 * produces.
 *
 * Runtime parameter by policy (`--ltl-max-observations`, REPL
 * `set ltlmaxobservations`, `api::set_ltl_max_observations`). At most
 * @ref ltl_max_observations_hard, and 0 means that bound. Default 8. Read
 * through @ref ltl_max_observations.
 */
inline size_t ltl_max_observations_param = 8;

/// Hard bound on @ref ltl_max_observations_param: the masks of the
/// observations are `size_t` bit sets, and 3^30 checks is hopeless anyway.
inline constexpr size_t ltl_max_observations_hard = 30;

/// The bounds of the Mealy view a data-game strategy is played through
/// (`code_strategy::build_mealy`); past either the moves are played directly,
/// and 0 builds no view. Set via `--ltl-mealy-max-states` /
/// `--ltl-mealy-max-edges`, REPL `ltlmealymaxstates` / `ltlmealymaxedges`,
/// `api::set_ltl_mealy_max_states` / `api::set_ltl_mealy_max_edges`
/// (defaults 4096 and 65536).
inline size_t data_game_mealy_max_states = 4096;
/// @copydoc data_game_mealy_max_states
inline size_t data_game_mealy_max_edges = size_t{1} << 16;

/// The most edges of a Mealy view `tau gen` / `tau compile` carries as a
/// table: past a few hundred edges the C++ compiler takes longer over the
/// table than over the program that solves the spec as it runs. 0 carries
/// none. Set via `--compile-max-table-edges`, REPL `compilemaxtableedges`
/// or `api::set_compile_max_table_edges` (default 400).
inline size_t compile_max_table_edges = 400;

/// Seconds the cmake build `tau compile` runs may take before it is stopped
/// and the compile fails. 0 waits for it without bound. Set via
/// `--compile-build-timeout`, REPL `compilebuildtimeout` or
/// `api::set_compile_build_timeout` (default 3600).
inline size_t compile_build_timeout = 3600;

/**
 * @brief Hard bound on the atomic propositions of a synthesis game whose
 * assignments are enumerated as `1 << n`: a signed shift is undefined at
 * 31 and the enumeration is hopeless long before. Not tunable.
 */
inline constexpr int ltl_max_game_aps = 30;

/**
 * @brief Wall-clock cap, in seconds, on each external Spot call
 * (`ltlsynt`, and the `ltlfilt` tautology check); 0 disables the watchdog.
 *
 * Runtime parameter by policy (`--ltl-timeout`, REPL `set ltltimeout`,
 * `api::set_ltl_timeout_sec`). Default @ref ltl_timeout_sec_default, at most
 * @ref ltl_timeout_sec_max: the setter and the option clamp a larger value.
 * Read through @ref ltl_timeout_sec.
 */
inline constexpr size_t ltl_timeout_sec_default = 60;
/// @copydoc ltl_timeout_sec_default
inline size_t ltl_timeout_sec_param = ltl_timeout_sec_default;

/// Largest accepted `ltl_timeout_sec_param`: one day.
inline constexpr size_t ltl_timeout_sec_max = 86400;

/**
 * @brief Synthesis algorithm override for the omcat (qlt) encodings:
 * `"A"`, `"B"` or `"D"`, or `"auto"` for the default routing.
 *
 * Runtime parameter by policy (`--ltl-alg`, REPL `set ltlalg`,
 * `api::set_ltl_algorithm`). Default `auto`; the empty string reads as
 * `auto` too. Read through @ref ltl_algorithm_choice.
 */
inline std::string ltl_algorithm_param = "auto";

/**
 * @brief Free-variable cap of the per-variable omcat quantifier-elimination
 * fast path in `aba_existential_feasible`; above it the joint solver decides.
 *
 * With at most two free variables the per-variable check is exact; values
 * above 2 re-enable a fast path that is NOT sound and exist only for
 * measurement, and 0 turns the fast path off. Runtime parameter by policy
 * (`--ltl-qe-max-vars`, REPL `set ltlqemaxvars`, `api::set_ltl_qe_max_vars`).
 * Default 2. Read through @ref ltl_qe_max_vars.
 */
inline size_t ltl_qe_max_vars_param = 2;

/// `ltl-witness`: on UNREALIZABLE, print the environment's winning
/// strategy (HOA) to stderr.
inline bool ltl_witness_param = false;

/// `ltl-export-strategy`: `hoa` or `dot` prints the winning strategy to
/// stderr. Empty prints nothing.
inline std::string ltl_export_strategy_param;

/// `ltl-export-strategy-file`: a path that also receives the HOA strategy.
/// Empty writes no file.
inline std::string ltl_export_strategy_file_param;

/// `phi-delta-crosscheck`: compare the closed form of Φ_Δ against the
/// solver on matching shapes. It changes logging only, never a verdict.
inline bool phi_delta_crosscheck_param = false;


/**
 * @brief Effective `ltlsynt` watchdog in seconds (0 = disabled).
 * @return The timeout in seconds, in [0, @ref ltl_timeout_sec_max].
 */
inline int ltl_timeout_sec() {
	return (int) std::min(ltl_timeout_sec_param, ltl_timeout_sec_max);
}

/**
 * @brief The synthesis algorithm @p v names, upper-cased: `"A"`, `"B"`,
 * `"D"`, or `""` for `auto` and the empty string; nullopt for any other
 * word.
 *
 * Case-insensitive. The one list of accepted words: the option reads it to
 * refuse a typo, and @ref ltl_algorithm_choice to route.
 */
inline std::optional<std::string> ltl_algorithm_name(std::string v) {
	for (auto& c : v) c = (char) std::toupper((unsigned char) c);
	if (v.empty() || v == "AUTO") return std::string();
	if (v == "A" || v == "B" || v == "D") return v;
	return std::nullopt;
}

/**
 * @brief Effective synthesis algorithm choice: `"A"`, `"B"`, `"D"` or `""`
 * for the default routing.
 *
 * Anything other than A, B, D or auto (case-insensitive) is reported once
 * per value and thread and read as the default: a typo must not silently
 * disable every gate.
 * @return The upper-cased choice, or `""` for `auto`, empty or invalid.
 */
inline std::string ltl_algorithm_choice() {
	if (auto v = ltl_algorithm_name(ltl_algorithm_param)) return *v;
	static thread_local std::set<std::string> warned;
	if (warned.insert(ltl_algorithm_param).second) {
		TAU_LOG_WARNING << "[ltl_aba] synthesis algorithm \""
			<< ltl_algorithm_param << "\" is not recognised (only A, "
			"B, D and auto are); using the default routing";
	}
	return "";
}

/// Effective free-variable cap of the omcat QE fast path; 0 turns it off.
inline size_t ltl_qe_max_vars() { return ltl_qe_max_vars_param; }

/**
 * @brief Effective largest state count accepted from an `ltlsynt` HOA
 * strategy (0 = unlimited).
 */
inline size_t ltl_hoa_max_states() { return ltl_hoa_max_states_param; }

/**
 * @brief Effective cap on the DNF cubes a HOA guard may expand into
 * (0 = unlimited).
 */
inline size_t ltl_guard_max_cubes() { return ltl_guard_max_cubes_param; }

/**
 * @brief Effective cap on the ABA-oracle refinement rounds of one
 * realizability check (0 = unlimited).
 */
inline size_t ltl_max_refinement_rounds() {
	return ltl_max_refinement_rounds_param;
}

/**
 * @brief Effective cap on the strategy paths the window oracle examines per
 * check (0 = unlimited).
 */
inline size_t ltl_window_max_paths() { return ltl_window_max_paths_param; }

/**
 * @brief Effective budget, in seconds, of the data game's regions that keep
 * their quantifiers (0 = no such attempt).
 */
inline size_t ltl_closed_regions_timeout() {
	return ltl_closed_regions_timeout_param;
}

/**
 * @brief Effective cap on the live nodes of a data game's BDD
 * (0 = unlimited).
 */
inline size_t ltl_data_game_max_nodes() {
	return ltl_data_game_max_nodes_param;
}

/**
 * @brief Effective cap on the operation memo entries of a data game's BDD
 * (0 = unlimited).
 */
inline size_t ltl_data_game_max_memo() {
	return ltl_data_game_max_memo_param;
}

/**
 * @brief Effective cap on the value combinations the data game tabulates per
 * comparison (0 = unlimited).
 */
inline size_t ltl_data_game_max_combinations() {
	return ltl_data_game_max_combinations_param;
}

/**
 * @brief Effective cap on the observation props whose joint values are
 * assumed consistent, within [1, @ref ltl_max_observations_hard].
 *
 * 0 and values above the hard bound read as the hard bound.
 */
inline size_t ltl_max_observations() {
	const size_t n = ltl_max_observations_param;
	return n == 0 || n > ltl_max_observations_hard
		? ltl_max_observations_hard : n;
}

/**
 * @brief Set when a cap of the synthesis pipeline gave up on a check whose
 * skipped part could make an UNREALIZABLE verdict wrong.
 *
 * Per thread; is_ltl_aba_realizable clears it before its game pipeline
 * (restoring the caller's flag on exit) and reports an UNREALIZABLE result
 * as undecided when it is set.
 */
inline thread_local bool ltl_verdict_incomplete = false;

/**
 * @brief Set while the observed abstraction is built (a second solve after
 * a strategy lost against the data): the consistency constraints then
 * forbid only combinations no data satisfies, since observations tell a
 * strategy when a claim depending on the inputs can be kept.
 */
inline thread_local bool ltl_observed_abstraction = false;

/**
 * @brief Set, with ltl_observed_abstraction, while the abstraction that
 * settles an UNREALIZABLE verdict is built: an input atom reading a past
 * step then gets a present-time twin (add_input_twins).
 */
inline thread_local bool ltl_input_twins = false;

/**
 * @brief Cleared while execution solves again a formula whose algebra
 * synthesised it propositionally with a strategy that names no data (a
 * strategy over bookkeeping bits): the default path then builds the
 * abstraction and the data game, whose strategies can be played.
 */
inline thread_local bool ltl_propositional_synthesis = true;

} // namespace idni::tau_lang

#endif // __IDNI__TAU__LTL_ABA_LIMITS_H__
