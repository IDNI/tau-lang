// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// tau-lang LTL(ABA) stable public API surface — Q40-API1.
//
// This header documents and re-exports the three functions that form the
// stable external interface for LTL(ABA) synthesis and execution.  All
// three are template functions parameterized by the node type, but in
// practice callers should use the default `node_t` (the concrete BA pack
// exported from tau.h).
//
// Semantic contract:
//   - Formulas must be terminated with a '.' (spec terminator).
//   - Output variables are named o1, o2, … (system-controlled).
//   - Input variables are named i1, i2, … (environment-controlled).
//   - Time indices: o1[t] = current output; o1[t-k] = k steps lookback.
//   - REALIZABLE  = ∃strategy. ∀env. formula holds on every infinite run.
//   - UNREALIZABLE = ∀strategy. ∃env. formula fails on some infinite run.
//   - G (globally/always) uses the existing safety pipeline.
//   - F, U, R, W, S use the full LTL(ABA) pipeline (Spot + ABA oracle).
//
// Environment variables that affect synthesis:
//   TAU_LTL_ALG=A|B|D      Select synthesis algorithm (default: heuristic)
//   TAU_LTL_TIMEOUT_SEC=N   Synthesis wall-clock timeout in seconds (default 60)
//   TAU_LTL_EXPORT_STRATEGY=hoa|dot  Print synthesized strategy to stderr
//   TAU_LTL_EXPORT_STRATEGY_FILE=<path>  Write strategy HOA to file
//   TAU_LTL_WITNESS=1       On UNREALIZABLE, print counterexample trace
//   TAU_LTL_SIMPLIFICATION=bwoa|sat|bisim-sat|none  ltlsynt minimization
//
// Version: 1.0 (2026-04-21)

#ifndef __IDNI__TAU__TAU_LTL_API_H__
#define __IDNI__TAU__TAU_LTL_API_H__

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
//   tref fm  — a parsed and normalized formula tree (from get_nso_rr)
//   Returns  — true iff fm is REALIZABLE
//
// Usage:
//   auto nso = get_nso_rr<node_t>(tau::get("G (o1[t] = 0)."));
//   if (nso) {
//       bool r = is_tau_formula_sat<node_t>(nso.value().main->get());
//   }
//
// Declared in: normalizer.h (included via tau.h)
// Template parameter: NodeType node — use node_t for the default BA pack.

// ── get_nso_rr ────────────────────────────────────────────────────────────────
//
// Parse a tau-lang formula string and return a normalized rr<node>.
//
//   tref expr — a formula string wrapped with tau::get(str)
//   Returns   — std::optional<rr<node>>, empty if parse fails
//
// Usage:
//   auto result = get_nso_rr<node_t>(tau::get("F (o1[t] = 0)."));
//   if (!result.has_value()) { /* parse error */ }
//
// Declared in: tau.h
// Note: call classify_parse_error(formula_str) on failure for a user hint.

// ── run ──────────────────────────────────────────────────────────────────────
//
// Execute a realizable formula against an io_context for N steps.
//
//   tref fm         — a realizable formula (is_tau_formula_sat returned true)
//   io_context& ctx — bound input/output streams
//   size_t steps    — number of time steps to execute
//
// Usage:
//   io_context<node_t> ctx;
//   ctx.add_output("o1", tau_type_id<node_t>(),
//                  std::make_shared<vector_output_stream>());
//   run<node_t>(fm, ctx, 10);
//
// Declared in: interpreter.h

} // namespace idni::tau_lang

#endif // __IDNI__TAU__TAU_LTL_API_H__
