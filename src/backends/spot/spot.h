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
/// disables the watchdog. The formula goes to ltlsynt through a temporary
/// file; a non-empty `TAU_LTL_SIMPLIFICATION` environment variable is passed
/// as `--simplification=`.
///
/// @param formula LTL formula in Spot syntax.
/// @param ins Input proposition names (`--ins=`, omitted when empty).
/// @param outs Output proposition names (`--outs=`, omitted when empty).
/// @param timeout_sec Watchdog limit in seconds; `<= 0` disables it.
/// @return The verdict, with the strategy HOA when realizable, or an error
/// (`code::solver_error`) when no verdict was produced.
result<synthesis_verdict> synthesize(const std::string& formula,
	const std::vector<std::string>& ins,
	const std::vector<std::string>& outs, int timeout_sec);

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

/// True iff this build can spawn ltlsynt: always false under Emscripten (no
/// process model), a cached real PATH probe otherwise. Reuses
/// `spawn_capture`'s own not-found detection, so the two can never
/// disagree about whether ltlsynt is reachable.
bool available();

// ── Internal to this backend, named here only so its own unit tests can
// reach it ─────────────────────────────────────────────────────────────────

/// Extras for `spawn_capture`. The defaults keep the tool-facing contract:
/// the child reads this process's stdin, and its stderr is dropped so a
/// noisy tool cannot corrupt the parsed stdout.
struct spawn_options {
	/// Send the child's stderr to the stdout pipe, a shell's `2>&1`.
	/// `stderr_path` takes precedence when it is set.
	bool merge_stderr = false;
	/// When not empty, the child reads its stdin from this file. When
	/// empty, the child inherits this process's stdin.
	std::string stdin_path;
	/// When not empty, the child writes its stderr to this file, created
	/// or truncated. `merge_stderr` then has no effect.
	std::string stderr_path;
};

/// Spawn `argv` directly (no shell), capture its stdout, and decide the
/// outcome right here -- the raw exit code never leaves this function.
/// `exit_ok` names which exit codes count as success for the tool being
/// spawned (ltlsynt: 0 realizable or 1 unrealizable; autfilt/ltlfilt: 0).
/// `timeout_sec <= 0` disables the SIGTERM watchdog; a fired watchdog is
/// reported as a signal death with `label::timeout` attached. A failed
/// report carries the captured output as `label::value`.
///
/// A bare `argv[0]` is looked up on PATH, then in `TAU_SPOT_BIN`, then in
/// the Spot folder of an installed package (`libexec/tau/spot`); a tool
/// found nowhere is a `code::not_found` error. An empty `argv` is a
/// `code::invalid_argument` error. Under Emscripten every call is a
/// `code::not_found` error. On success the value is the captured stdout.
result<std::string> spawn_capture(const std::vector<std::string>& argv,
	int timeout_sec = 0,
	std::function<bool(int)> exit_ok = [](int c) { return c == 0; },
	const spawn_options& opts = spawn_options{});

} // namespace idni::tau_lang

#include "backends/spot/spot.tmpl.h"

#endif // __IDNI__TAU__BACKENDS__SPOT_H__
