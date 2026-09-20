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

#include <functional>
#include <string>
#include <vector>

#include "tau_diagnostics.h"

namespace idni::tau_lang {

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
/// disables the watchdog.
result<synthesis_verdict> synthesize(const std::string& formula,
	const std::vector<std::string>& ins,
	const std::vector<std::string>& outs, int timeout_sec);

/// Run `ltlsynt --print-game-hoa` and return the raw game-HOA text. A
/// caller that needs a parsed game (Algorithm D) parses this text itself --
/// the game structure is tau-lang's, not this backend's. Same
/// no-verdict-is-an-error contract as `synthesize`.
result<std::string> synthesize_game(const std::string& formula,
	const std::vector<std::string>& ins,
	const std::vector<std::string>& outs, int timeout_sec);

/// Run `autfilt --dot` on HOA text and return the rendered dot text.
result<std::string> to_dot(const std::string& hoa_text, int timeout_sec);

/// Run `ltlfilt -f` on a formula and report whether it printed "1" (the
/// formula is a propositional tautology). A caller using this as an
/// optimization fast path treats any error the same as "not proven".
result<bool> is_tautology(const std::string& formula, int timeout_sec);

/// True iff this build can spawn ltlsynt: always false under Emscripten (no
/// process model), a cached real PATH probe otherwise. Reuses
/// `spawn_capture`'s own not-found detection, so the two can never
/// disagree about whether ltlsynt is reachable.
bool available();

// ── Internal to this backend, named here only so its own unit tests can
// reach it ─────────────────────────────────────────────────────────────────

/// Spawn `argv` directly (no shell), capture its stdout, and decide the
/// outcome right here -- the raw exit code never leaves this function.
/// `exit_ok` names which exit codes count as success for the tool being
/// spawned (ltlsynt: 0 realizable or 1 unrealizable; autfilt/ltlfilt: 0).
/// `timeout_sec <= 0` disables the SIGTERM watchdog; a fired watchdog is
/// reported as a signal death with `label::timeout` attached.
result<std::string> spawn_capture(const std::vector<std::string>& argv,
	int timeout_sec = 0,
	std::function<bool(int)> exit_ok = [](int c) { return c == 0; });

} // namespace idni::tau_lang

#include "backends/spot/spot.tmpl.h"

#endif // __IDNI__TAU__BACKENDS__SPOT_H__
