// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file tau_lang_api.h
 * @brief tau-lang LTL(ABA) stable public API surface.
 *
 * This header documents and re-exports the three functions that form the
 * stable external interface for LTL(ABA) synthesis and execution.  All
 * three are template functions parameterized by the node type, but in
 * practice callers should use the default `node_t` (the concrete BA pack
 * exported from tau.h).
 *
 * Semantic contract:
 *   - Formulas must be terminated with a '.' (spec terminator).
 *   - Output variables are named o1, o2, … (system-controlled).
 *   - Input variables are named i1, i2, … (environment-controlled).
 *   - Time indices: o1[t] = current output; o1[t-k] = k steps lookback.
 *   - REALIZABLE  = ∃strategy. ∀env. formula holds on every infinite run.
 *   - UNREALIZABLE = ∀strategy. ∃env. formula fails on some infinite run.
 *   - G (globally/always) uses the existing safety pipeline.
 *   - F, U, R, W, S, T use the full LTL(ABA) pipeline (Spot + ABA oracle).
 *
 * Environment variables that affect synthesis:
 *   TAU_LTL_ALG=A|B|D|auto Select the synthesis algorithm (case-insensitive;
 *                          `--ltl-alg` / `set ltlalg` win over it).  Unset
 *                          or `auto` selects the default routing: Algorithm
 *                          B is gated on and the default ABA-oracle path is
 *                          used when B does not apply.  Any other value is
 *                          reported once with a warning and read as `auto`.
 *                          Pure-output qlt formulas take Algorithm A
 *                          unconditionally, whatever this is set to.
 *                          Note (LS-20): an EXPLICIT `B` is not a no-op
 *                          relative to unset -- it additionally enables the
 *                          polarity-complete pairwise constraint pass in
 *                          normalization (unset only defaults the gate in
 *                          the builders).
 *   TAU_LTL_TIMEOUT_SEC=N   Synthesis wall-clock timeout in seconds (default 60,
 *                          0 = none, at most 86400)
 *   TAU_LTL_EXPORT_STRATEGY=hoa|dot  Print synthesized strategy to stderr
 *   `TAU_LTL_EXPORT_STRATEGY_FILE=<path>`  Write strategy HOA to file
 *   TAU_LTL_WITNESS=1       On UNREALIZABLE, print counterexample trace
 *   TAU_LTL_SIMPLIFICATION=bwoa|sat|bisim-sat|none  ltlsynt minimization
 *
 * The header declares nothing itself: `is_tau_formula_sat`, `get_nso_rr`
 * and `run` are declared in satisfiability.h, tau_tree.tmpl.h and
 * interpreter.h (all reached through the includes below) and described in
 * the section comments inside the namespace.
 *
 * Version: 1.0 (2026-04-21)
 */

#ifndef __IDNI__TAU__TAU_LANG_API_H__
#define __IDNI__TAU__TAU_LANG_API_H__

// ── Core synthesis API ────────────────────────────────────────────────────────

// Include the full tau header which provides `node_t`, `tau`, `get_nso_rr`,
// `is_tau_formula_sat`, and `run`.
#include "tau.h"
#include "interpreter.h"

namespace idni::tau_lang {

// ── is_tau_formula_sat ────────────────────────────────────────────────────────
//
// The top-level LTL realizability check.
//
//   tref fm          — a parsed and normalized formula tree (from get_nso_rr)
//   int_t start_time — time point the check starts at (default 0)
//   bool output      — print the verdict trace (default false)
//   Returns          — result<bool>: true iff fm is REALIZABLE; an error
//                      when no verdict could be obtained
//
// Usage:
//   auto nso = get_nso_rr<node_t>(tau::get("G (o1[t] = 0).").value());
//   if (nso.has_value()) {
//       auto r = is_tau_formula_sat<node_t>(nso.value().main->get());
//       if (r.has_value() && r.value()) { /* realizable */ }
//   }
//
// Declared in: satisfiability.h (included via tau.h)
// Template parameter: NodeType node — use node_t for the default BA pack.

// ── get_nso_rr ────────────────────────────────────────────────────────────────
//
// Extract the recurrence relation system of a parsed spec.
//
//   tref expr — the parsed spec, tau::get(str).value() (tau::get returns
//               a result<tref> carrying the parse report)
//   Returns   — result<rr<node>>, a failed report if extraction fails
//
// Usage:
//   auto parsed = tau::get("F (o1[t] = 0).");
//   if (!parsed.has_value()) { /* parse error */ }
//   auto result = get_nso_rr<node_t>(parsed.value());
//
// Declared in: tau_tree.tmpl.h (included via tau.h)
// Note: call classify_parse_error(formula_str) on failure for a user hint.

// ── run ──────────────────────────────────────────────────────────────────────
//
// Execute a realizable formula against an io_context for N steps.
//
//   tref fm               — a realizable formula (is_tau_formula_sat returned true)
//   const io_context& ctx — bound input/output streams
//   size_t steps          — maximum number of time steps (default 0 = unlimited)
//   Returns               — result<interpreter<node>>: the interpreter after
//                           execution, or an error if initialization failed
//
// Usage:
//   io_context<node_t> ctx;
//   ctx.add_output("o1", tau_type_id<node_t>(),
//                  std::make_shared<vector_output_stream>());
//   run<node_t>(fm, ctx, 10);
//
// Declared in: interpreter.h

} // namespace idni::tau_lang

#endif // __IDNI__TAU__TAU_LANG_API_H__
