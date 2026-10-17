// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Spot subprocess backend: everything that shells out to ltlsynt, autfilt
// or ltlfilt lives here. A generic LTL API over strings -- no tau tree
// type crosses into this file, so it needs nothing from normalizer.h or
// the pack, and an out-of-tree consumer can use it standalone.
//
// The exit-code convention of each tool is decided once, inside
// `spawn_capture`, at the moment the exit code exists: no caller of the
// public surface below ever branches on a raw process exit code.

#ifndef __IDNI__TAU__BACKENDS__SPOT_H__
#define __IDNI__TAU__BACKENDS__SPOT_H__

#include <string>
#include <vector>

#include "spawn_capture.h"
#include "tau_diagnostics.h"

namespace idni::tau_lang {

/// How ltlsynt builds and simplifies a strategy. An empty field keeps the
/// default of ltlsynt.
struct synthesis_options {
	/// `--algo=`: sd, ds, ps, lar, lar.old or acd.
	std::string algo;
	/// `--decompose=`: yes or no.
	std::string decompose;
	/// `--simplify=`: no, bisim, bwoa, sat, bisim-sat or bwoa-sat.
	std::string simplify;
	/// `--semantics=Moore`: the outputs of a step are chosen before its
	/// inputs are seen. Mealy otherwise.
	bool moore = false;
	/// `--aiger --verify`: the strategy as an AIGER circuit, which Spot
	/// checks against the formula and follows with `Circuit was verified`.
	bool aiger = false;
	/// `--verify`: Spot checks the strategy against the formula and follows
	/// it with `Strategy was verified`.
	bool verify = false;
};

/// Verdict of an `ltlsynt` synthesis call. `hoa` is non-empty only when
/// `realizable` is true.
struct synthesis_verdict {
	bool realizable;
	std::string hoa;
};

/// Run ltlsynt on a propositional LTL formula in Spot syntax over the given
/// input/output proposition names and return its verdict.
///
/// An error result means no verdict was produced at all -- ltlsynt not on
/// PATH, killed by the `timeout_sec` watchdog, a usage/internal error, or
/// output carrying neither a REALIZABLE nor an UNREALIZABLE line -- and
/// must never be read as a definitive UNREALIZABLE. `timeout_sec <= 0`
/// disables the watchdog. The formula goes to ltlsynt through a temporary
/// file.
///
/// @param formula LTL formula in Spot syntax.
/// @param ins Input proposition names (`--ins=`, omitted when empty).
/// @param outs Output proposition names (`--outs=`, omitted when empty).
/// @param timeout_sec Watchdog limit in seconds; `<= 0` disables it.
/// @param opts The non-empty fields go to ltlsynt as `--algo=`,
/// `--decompose=` and `--simplify=`, `moore` as `--semantics=Moore` and
/// `aiger` as `--aiger --verify`.
/// @return The verdict, with the strategy HOA when realizable, or an error
/// (`code::solver_error`) when no verdict was produced.
result<synthesis_verdict> synthesize(const std::string& formula,
	const std::vector<std::string>& ins,
	const std::vector<std::string>& outs, int timeout_sec,
	const synthesis_options& opts = {});

/// Run `ltlsynt --print-game-hoa` and return the raw game-HOA text. A
/// caller that needs a parsed game (Algorithm D) parses this text itself --
/// the game structure is tau-lang's, not this backend's. Same
/// no-verdict-is-an-error contract as `synthesize`. A non-empty `algo` is
/// passed as `--algo=`. The game is printed with `--polarity=no` and
/// `--decompose=no`, so it is the single, unsimplified arena over every
/// proposition.
result<std::string> synthesize_game(const std::string& formula,
	const std::vector<std::string>& ins,
	const std::vector<std::string>& outs, int timeout_sec,
	const std::string& algo = {});

/// Run `autfilt --dot` on HOA text (written to a temporary file) and return
/// the rendered dot text; an error when autfilt is missing, times out
/// (`timeout_sec <= 0` disables the watchdog) or exits non-zero.
result<std::string> to_dot(const std::string& hoa_text, int timeout_sec);

/// Run `ltlfilt -F` on a formula (written to a temporary file) and report whether it printed "1" (the
/// formula is a propositional tautology). A caller using this as an
/// optimization fast path treats any error the same as "not proven".
result<bool> is_tautology(const std::string& formula, int timeout_sec);

/// Run `ltlfilt --from-ltlf=<alive>` on an LTLf formula (written to a
/// temporary file) and return the LTL formula it prints: @p alive holds on
/// a prefix of the trace, the finite trace the formula is read over, and
/// never again after it. An error when ltlfilt is missing, times out
/// (`timeout_sec <= 0` disables the watchdog), exits non-zero or prints
/// nothing.
result<std::string> from_ltlf(const std::string& formula,
	const std::string& alive, int timeout_sec);

/// is_tautology over several formulas in one `ltlfilt -F` run, one formula
/// per line: element i is true iff formula i printed "1". An error when
/// ltlfilt is missing, times out or prints a line count other than the
/// number of formulas.
result<std::vector<bool>> are_tautologies(
	const std::vector<std::string>& formulas, int timeout_sec);

/// True iff this build can spawn ltlsynt: always false under Emscripten (no
/// process model), a cached real PATH probe otherwise. Reuses
/// `spawn_capture`'s own not-found detection, so the two can never
/// disagree about whether ltlsynt is reachable.
bool available();

} // namespace idni::tau_lang

#include "backends/spot/spot.tmpl.h"

#endif // __IDNI__TAU__BACKENDS__SPOT_H__
