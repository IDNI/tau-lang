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
 * each round blocks one infeasible strategy edge and re-runs `ltlsynt`. On
 * the cap the verdict is UNKNOWN (an error), never a false answer.
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
 * @brief Hard bound on the atomic propositions of a synthesis game whose
 * assignments are enumerated as `1 << n`: a signed shift is undefined at
 * 31 and the enumeration is hopeless long before. Not tunable.
 */
inline constexpr int ltl_max_game_aps = 30;

/**
 * @brief Wall-clock cap, in seconds, on each external `ltlsynt` /
 * `ltl2tgba` call; 0 disables the watchdog.
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
 * warning (SY-R5).
 */
inline int ltl_timeout_sec() {
	if (ltl_timeout_sec_param >= 0)
		return (int) std::min(ltl_timeout_sec_param, ltl_timeout_sec_max);
	int timeout_sec = 60;
	if (const char* env_sec = std::getenv("TAU_LTL_TIMEOUT_SEC")) {
		char* end = nullptr;
		errno = 0;
		long v = std::strtol(env_sec, &end, 10);
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
 * `strtol`; garbage keeps the default), else 2.
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
 * @brief Set when a cap of the synthesis pipeline gave up on a check whose
 * skipped part could make an UNREALIZABLE verdict wrong.
 *
 * Per thread; is_ltl_aba_realizable clears it on entry and reports an
 * UNREALIZABLE result as undecided when it is set.
 */
inline thread_local bool ltl_verdict_incomplete = false;

} // namespace idni::tau_lang

#endif // __IDNI__TAU__LTL_ABA_LIMITS_H__
