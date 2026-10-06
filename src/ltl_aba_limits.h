// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ltl_aba_limits.h
 * @brief Runtime limits of the LTL(ABA) synthesis game machinery that the
 * game headers (algorithm_d_game.h) need before ltl_aba.h is complete.
 *
 * Every limit here is a runtime parameter by policy (CLI flag, REPL `set`,
 * `api::set_*`, and a `TAU_*` environment variable as the last fallback),
 * never a header constant; ltl_aba.h documents the surface. Each one is read
 * through its accessor, which resolves parameter > environment > default; the
 * parameter is what the three option surfaces write, so a flag always wins
 * over the environment.
 */

#ifndef __IDNI__TAU__LTL_ABA_LIMITS_H__
#define __IDNI__TAU__LTL_ABA_LIMITS_H__

#include "env_limits.h"
#include "logging.h"
#include <algorithm>
#include <cctype>
#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <string>

namespace idni::tau_lang {

/**
 * @brief Largest state count accepted from an `ltlsynt` HOA strategy; a
 * larger count is treated as a garbled header (SY-R3), not an automaton.
 *
 * Runtime parameter by policy (`--ltl-hoa-max-states`, REPL
 * `set ltlhoamaxstates`, `api::set_ltl_hoa_max_states`); 0 = unlimited.
 * The sentinel -1 means "not set", in which case `TAU_LTL_HOA_MAX_STATES`
 * is consulted and 2^22 applies when that is absent too. Read through
 * @ref ltl_hoa_max_states.
 */
inline long ltl_hoa_max_states_param = -1;

/**
 * @brief Cap on the DNF cubes a HOA guard label may expand into in the
 * Algorithm D product game; a guard beyond it is refused (no verdict).
 *
 * Runtime parameter by policy (`--ltl-guard-max-cubes`, REPL
 * `set ltlguardmaxcubes`, `api::set_ltl_guard_max_cubes`); 0 = unlimited.
 * The sentinel -1 means "not set", in which case `TAU_LTL_GUARD_MAX_CUBES`
 * is consulted and 512 applies when that is absent too. Read through
 * @ref ltl_guard_max_cubes.
 */
inline long ltl_guard_max_cubes_param = -1;

/**
 * @brief Cap on the ABA-oracle refinement rounds of `is_ltl_aba_realizable`:
 * each round blocks one infeasible strategy edge and re-runs `ltlsynt`. The
 * same cap bounds the fixpoint rounds of `strategy_wins_on_data`. On the cap
 * the verdict is UNKNOWN (an error), never a false answer.
 *
 * Runtime parameter by policy (`--ltl-refinement-rounds`, REPL
 * `set ltlrefinementrounds`, `api::set_ltl_max_refinement_rounds`);
 * 0 = unlimited. The sentinel -1 means "not set", in which case
 * `TAU_LTL_REFINEMENT_ROUNDS` is consulted and 64 applies when that is
 * absent too. Read through @ref ltl_max_refinement_rounds.
 */
inline long ltl_max_refinement_rounds_param = -1;

/**
 * @brief Cap on the strategy paths the window oracle examines per check
 * (`window_infeasible_paths`); a hit cap yields UNKNOWN, never a verdict.
 *
 * Runtime parameter by policy (`--ltl-window-max-paths`, REPL
 * `set ltlwindowmaxpaths`, `api::set_ltl_window_max_paths`); 0 = unlimited.
 * The sentinel -1 means "not set", in which case `TAU_LTL_WINDOW_MAX_PATHS`
 * is consulted and 4096 applies when that is absent too. Read through
 * @ref ltl_window_max_paths.
 */
inline long ltl_window_max_paths_param = -1;

/**
 * @brief Seconds the data game may spend on regions that keep their
 * quantifiers (formula_regions::closed_type), all their questions together,
 * each taking at most a quarter of it; past either that attempt is
 * undecided, and the game with it.
 *
 * Runtime parameter by policy (`--ltl-closed-regions-timeout`, REPL
 * `set ltlclosedregionstimeout`, `api::set_ltl_closed_regions_timeout`);
 * 0 skips the attempt. The sentinel -1 means "not set", in which case
 * `TAU_LTL_CLOSED_REGIONS_TIMEOUT` is consulted and 20 applies when that is
 * absent too. Read through @ref ltl_closed_regions_timeout.
 */
inline long ltl_closed_regions_timeout_param = -1;

/**
 * @brief Most nodes live at once in the BDD of a data game over codes
 * (`code_regions`); a full table makes the game undecided, never a verdict.
 * A product of two bitvector streams wider than 4 bits needs millions of
 * nodes; 2^23 nodes and their tables take about 1 GB.
 *
 * Runtime parameter by policy (`--ltl-data-game-max-nodes`, REPL
 * `set ltldatagamemaxnodes`, `api::set_ltl_data_game_max_nodes`);
 * 0 = unlimited. The sentinel -1 means "not set", in which case
 * `TAU_LTL_DATA_GAME_MAX_NODES` is consulted and 2^23 applies when that is
 * absent too. Read through @ref ltl_data_game_max_nodes.
 */
inline long ltl_data_game_max_nodes_param = -1;

/**
 * @brief Most entries of the operation memo of a data game's BDD; the memo
 * is emptied when it reaches the cap, which costs recomputation only.
 *
 * Runtime parameter by policy (`--ltl-data-game-max-memo`, REPL
 * `set ltldatagamemaxmemo`, `api::set_ltl_data_game_max_memo`);
 * 0 = unlimited. The sentinel -1 means "not set", in which case
 * `TAU_LTL_DATA_GAME_MAX_MEMO` is consulted and 2^25 applies when that is
 * absent too. Read through @ref ltl_data_game_max_memo.
 */
inline long ltl_data_game_max_memo_param = -1;

/**
 * @brief Most value combinations the data game tabulates for one comparison
 * its circuits do not encode (`tabulate`); past it the comparison has no
 * code, and the game falls back to formula regions or is undecided.
 *
 * Runtime parameter by policy (`--ltl-data-game-max-combinations`, REPL
 * `set ltldatagamemaxcombinations`,
 * `api::set_ltl_data_game_max_combinations`); 0 = unlimited. The sentinel
 * -1 means "not set", in which case `TAU_LTL_DATA_GAME_MAX_COMBINATIONS` is
 * consulted and 4096 applies when that is absent too. Read through
 * @ref ltl_data_game_max_combinations.
 */
inline long ltl_data_game_max_combinations_param = -1;

/**
 * @brief Most observation props whose impossible joint values the skeleton
 * assumes away (`assume_observation_consistency`, 3^n feasibility checks);
 * beyond it nothing is assumed, which leaves the environment moves no data
 * produces.
 *
 * Runtime parameter by policy (`--ltl-max-observations`, REPL
 * `set ltlmaxobservations`, `api::set_ltl_max_observations`). At most
 * @ref ltl_max_observations_hard, and 0 means that bound. The sentinel -1
 * means "not set", in which case `TAU_LTL_MAX_OBSERVATIONS` is consulted and
 * 8 applies when that is absent too. Read through
 * @ref ltl_max_observations.
 */
inline long ltl_max_observations_param = -1;

/// Hard bound on @ref ltl_max_observations_param: the masks of the
/// observations are `size_t` bit sets, and 3^30 checks is hopeless anyway.
inline constexpr size_t ltl_max_observations_hard = 30;

/// The bounds of the Mealy view a data-game strategy is played through
/// (`code_strategy::build_mealy`); past either the moves are played directly,
/// and 0 builds no view. Set via `--ltl-mealy-max-states` /
/// `--ltl-mealy-max-edges`, REPL `ltlmealymaxstates` / `ltlmealymaxedges`,
/// `api::set_ltl_mealy_max_states` / `api::set_ltl_mealy_max_edges`, or the
/// environment variables `TAU_LTL_MEALY_MAX_STATES` (default 4096) and
/// `TAU_LTL_MEALY_MAX_EDGES` (default 65536).
inline env_limit<size_t> data_game_mealy_max_states{
	"TAU_LTL_MEALY_MAX_STATES", 4096 };
/// @copydoc data_game_mealy_max_states
inline env_limit<size_t> data_game_mealy_max_edges{
	"TAU_LTL_MEALY_MAX_EDGES", size_t{1} << 16 };

/// The most edges of a Mealy view `tau gen` / `tau compile` carries as a
/// table: past a few hundred edges the C++ compiler takes longer over the
/// table than over the program that solves the spec as it runs. 0 carries
/// none. Set via `--compile-max-table-edges`, REPL `compilemaxtableedges`,
/// `api::set_compile_max_table_edges`, or the environment variable
/// `TAU_COMPILE_MAX_TABLE_EDGES` (default 400).
inline env_limit<size_t> compile_max_table_edges{
	"TAU_COMPILE_MAX_TABLE_EDGES", 400 };

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
 * `api::set_ltl_timeout_sec`). The sentinel -1 means "not set", in which
 * case the environment variable `TAU_LTL_TIMEOUT_SEC` is consulted and the
 * default of 60 s applies when that is absent too; an explicit parameter
 * always wins over the environment. Read through @ref ltl_timeout_sec.
 */
inline long ltl_timeout_sec_param = -1;

/// Largest accepted `ltl_timeout_sec_param` / `TAU_LTL_TIMEOUT_SEC`: one day.
inline constexpr long ltl_timeout_sec_max = 86400;

/**
 * @brief Synthesis algorithm override for the omcat (qlt) encodings:
 * `"A"`, `"B"` or `"D"`, or `"auto"` for the default routing.
 *
 * Runtime parameter by policy (`--ltl-alg`, REPL `set ltlalg`,
 * `api::set_ltl_algorithm`). Empty means "not set": the environment
 * variable `TAU_LTL_ALG` is consulted and `auto` applies when it is absent.
 * Read through @ref ltl_algorithm_choice, which validates the value once.
 */
inline std::string ltl_algorithm_param;

/**
 * @brief Free-variable cap of the per-variable omcat quantifier-elimination
 * fast path in `aba_existential_feasible`; above it the joint solver decides.
 *
 * With at most two free variables the per-variable check is exact; values
 * above 2 re-enable a fast path that is NOT sound and exist only for
 * measurement. Runtime parameter by policy (`--ltl-qe-max-vars`, REPL
 * `set ltlqemaxvars`, `api::set_ltl_qe_max_vars`). 0 means "not set": the
 * environment variable `TAU_LTL_OMCAT_QE_MAX_VARS` is consulted and 2 applies
 * when it is absent. Read through @ref ltl_qe_max_vars.
 */
inline size_t ltl_qe_max_vars_param = 0;


/**
 * @brief Effective `ltlsynt` watchdog in seconds (0 = disabled).
 *
 * Precedence: @ref ltl_timeout_sec_param when set (>= 0), else the
 * `TAU_LTL_TIMEOUT_SEC` environment variable, else 60. Garbage in the
 * variable keeps the default instead of `atoi`'s 0 silently removing the
 * cap (LS-9); out-of-range values clamp to @ref ltl_timeout_sec_max with a
 * warning (SY-R5). A parameter above the maximum clamps silently.
 * @return The timeout in seconds, in [0, @ref ltl_timeout_sec_max].
 */
inline int ltl_timeout_sec() {
	if (ltl_timeout_sec_param >= 0)
		return (int) std::min(ltl_timeout_sec_param, ltl_timeout_sec_max);
	int timeout_sec = 60;
	if (const char* env_sec = std::getenv("TAU_LTL_TIMEOUT_SEC")) {
		char* end = nullptr;
		errno = 0;
		// 64 bits on every target: a 32-bit long (wasm32) would read
		// 2^32 as out of range instead of clamping it
		long long v = std::strtoll(env_sec, &end, 10);
		if (end == env_sec || *end != '\0' || v < 0 || errno == ERANGE) {
			TAU_LOG_WARNING << "TAU_LTL_TIMEOUT_SEC='" << env_sec
				<< "' is not a non-negative number; keeping the default "
				<< timeout_sec << "s";
		} else if (v > ltl_timeout_sec_max) {
			TAU_LOG_WARNING << "TAU_LTL_TIMEOUT_SEC=" << v
				<< " exceeds the maximum; clamping to "
				<< ltl_timeout_sec_max << "s";
			timeout_sec = (int) ltl_timeout_sec_max;
		} else timeout_sec = (int) v;
	}
	return timeout_sec;
}

/**
 * @brief Effective synthesis algorithm choice: `"A"`, `"B"`, `"D"` or `""`
 * for the default routing.
 *
 * Precedence: @ref ltl_algorithm_param when non-empty, else the
 * `TAU_LTL_ALG` environment variable, else the default. Anything other
 * than A, B, D or auto (case-insensitive) is reported once and read as the
 * default (LS-8): a typo must not silently disable every gate.
 * @return The upper-cased choice, or `""` for `auto`, unset or invalid.
 */
inline std::string ltl_algorithm_choice() {
	std::string v = ltl_algorithm_param;
	if (v.empty())
		if (const char* env = std::getenv("TAU_LTL_ALG"); env) v = env;
	for (auto& c : v) c = (char) std::toupper((unsigned char) c);
	if (v.empty() || v == "AUTO") return "";
	if (v == "A" || v == "B" || v == "D") return v;
	static std::string warned_for;
	if (warned_for != v) {
		warned_for = v;
		TAU_LOG_WARNING << "[ltl_aba] synthesis algorithm \"" << v
			<< "\" is not recognised (only A, B, D and auto are); "
			"using the default routing";
	}
	return "";
}

/**
 * @brief Effective free-variable cap of the omcat QE fast path.
 *
 * Precedence: @ref ltl_qe_max_vars_param when non-zero, else the
 * `TAU_LTL_OMCAT_QE_MAX_VARS` environment variable (validated with
 * `strtol`; garbage or a non-positive value keeps the default with a
 * warning), else 2.
 * @return The cap, always >= 1.
 */
inline size_t ltl_qe_max_vars() {
	if (ltl_qe_max_vars_param > 0) return ltl_qe_max_vars_param;
	size_t cap = 2;
	if (const char* env_cap = std::getenv("TAU_LTL_OMCAT_QE_MAX_VARS")) {
		char* end = nullptr;
		errno = 0;
		long v = std::strtol(env_cap, &end, 10);
		if (end == env_cap || *end != '\0' || v <= 0 || errno == ERANGE) {
			TAU_LOG_WARNING << "TAU_LTL_OMCAT_QE_MAX_VARS='" << env_cap
				<< "' is not a positive number; keeping the default "
				<< cap;
		} else cap = (size_t) v;
	}
	return cap;
}

/**
 * @brief Effective largest state count accepted from an `ltlsynt` HOA
 * strategy (0 = unlimited).
 *
 * Precedence: @ref ltl_hoa_max_states_param when set (>= 0), else
 * `TAU_LTL_HOA_MAX_STATES`, else 2^22.
 */
inline size_t ltl_hoa_max_states() {
	if (ltl_hoa_max_states_param >= 0)
		return (size_t) ltl_hoa_max_states_param;
	return env_limit_count("TAU_LTL_HOA_MAX_STATES", size_t(1) << 22);
}

/**
 * @brief Effective cap on the DNF cubes a HOA guard may expand into
 * (0 = unlimited).
 *
 * Precedence: @ref ltl_guard_max_cubes_param when set (>= 0), else
 * `TAU_LTL_GUARD_MAX_CUBES`, else 512.
 */
inline size_t ltl_guard_max_cubes() {
	if (ltl_guard_max_cubes_param >= 0)
		return (size_t) ltl_guard_max_cubes_param;
	return env_limit_count("TAU_LTL_GUARD_MAX_CUBES", 512);
}

/**
 * @brief Effective cap on the ABA-oracle refinement rounds of one
 * realizability check (0 = unlimited).
 *
 * Precedence: @ref ltl_max_refinement_rounds_param when set (>= 0), else
 * `TAU_LTL_REFINEMENT_ROUNDS`, else 64.
 */
inline size_t ltl_max_refinement_rounds() {
	if (ltl_max_refinement_rounds_param >= 0)
		return (size_t) ltl_max_refinement_rounds_param;
	return env_limit_count("TAU_LTL_REFINEMENT_ROUNDS", 64);
}

/**
 * @brief Effective cap on the strategy paths the window oracle examines per
 * check (0 = unlimited).
 *
 * Precedence: @ref ltl_window_max_paths_param when set (>= 0), else
 * `TAU_LTL_WINDOW_MAX_PATHS`, else 4096.
 */
inline size_t ltl_window_max_paths() {
	if (ltl_window_max_paths_param >= 0)
		return (size_t) ltl_window_max_paths_param;
	return env_limit_count("TAU_LTL_WINDOW_MAX_PATHS", 4096);
}

/**
 * @brief Effective budget, in seconds, of the data game's regions that keep
 * their quantifiers (0 = no such attempt).
 *
 * Precedence: @ref ltl_closed_regions_timeout_param when set (>= 0), else
 * `TAU_LTL_CLOSED_REGIONS_TIMEOUT`, else 20.
 */
inline size_t ltl_closed_regions_timeout() {
	if (ltl_closed_regions_timeout_param >= 0)
		return (size_t) ltl_closed_regions_timeout_param;
	return env_limit_count("TAU_LTL_CLOSED_REGIONS_TIMEOUT", 20);
}

/**
 * @brief Effective cap on the live nodes of a data game's BDD
 * (0 = unlimited).
 *
 * Precedence: @ref ltl_data_game_max_nodes_param when set (>= 0), else
 * `TAU_LTL_DATA_GAME_MAX_NODES`, else 2^23.
 */
inline size_t ltl_data_game_max_nodes() {
	if (ltl_data_game_max_nodes_param >= 0)
		return (size_t) ltl_data_game_max_nodes_param;
	return env_limit_count("TAU_LTL_DATA_GAME_MAX_NODES", size_t{1} << 23);
}

/**
 * @brief Effective cap on the operation memo entries of a data game's BDD
 * (0 = unlimited).
 *
 * Precedence: @ref ltl_data_game_max_memo_param when set (>= 0), else
 * `TAU_LTL_DATA_GAME_MAX_MEMO`, else 2^25.
 */
inline size_t ltl_data_game_max_memo() {
	if (ltl_data_game_max_memo_param >= 0)
		return (size_t) ltl_data_game_max_memo_param;
	return env_limit_count("TAU_LTL_DATA_GAME_MAX_MEMO", size_t{1} << 25);
}

/**
 * @brief Effective cap on the value combinations the data game tabulates per
 * comparison (0 = unlimited).
 *
 * Precedence: @ref ltl_data_game_max_combinations_param when set (>= 0),
 * else `TAU_LTL_DATA_GAME_MAX_COMBINATIONS`, else 4096.
 */
inline size_t ltl_data_game_max_combinations() {
	if (ltl_data_game_max_combinations_param >= 0)
		return (size_t) ltl_data_game_max_combinations_param;
	return env_limit_count("TAU_LTL_DATA_GAME_MAX_COMBINATIONS", 4096);
}

/**
 * @brief Effective cap on the observation props whose joint values are
 * assumed consistent, within [1, @ref ltl_max_observations_hard].
 *
 * Precedence: @ref ltl_max_observations_param when set (>= 0), else
 * `TAU_LTL_MAX_OBSERVATIONS`, else 8; 0 and values above the hard bound
 * read as the hard bound.
 */
inline size_t ltl_max_observations() {
	const size_t n = ltl_max_observations_param >= 0
		? (size_t) ltl_max_observations_param
		: env_limit_count("TAU_LTL_MAX_OBSERVATIONS", 8);
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
