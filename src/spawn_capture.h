// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Process spawning: run a program directly (no shell), capture its stdout
// and decide its outcome, under an optional watchdog. The Spot backend and
// every other caller that runs a program use it.

#ifndef __IDNI__TAU__SPAWN_CAPTURE_H__
#define __IDNI__TAU__SPAWN_CAPTURE_H__

#include <functional>
#include <string>
#include <vector>

#include "tau_diagnostics.h"

namespace idni::tau_lang {

/// `spot-bin`: a directory searched for the Spot tools after PATH. Empty
/// skips it.
inline std::string spot_bin_param;

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
/// A bare `argv[0]` is looked up on PATH, then in `spot-bin`, then in
/// the Spot folder of an installed package (`libexec/tau/spot`); a tool
/// found nowhere is a `code::not_found` error. An empty `argv` is a
/// `code::invalid_argument` error. Under Emscripten every call is a
/// `code::not_found` error. On success the value is the captured stdout.
result<std::string> spawn_capture(const std::vector<std::string>& argv,
	int timeout_sec = 0,
	std::function<bool(int)> exit_ok = [](int c) { return c == 0; },
	const spawn_options& opts = spawn_options{});

} // namespace idni::tau_lang

#include "spawn_capture.tmpl.h"

#endif // __IDNI__TAU__SPAWN_CAPTURE_H__
