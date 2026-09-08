// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file options.h
 * @brief Anti-prenexing foundations (layer 0): the six knobs of the §1 ctx
 * table, as process-wide defaults.
 *
 * Follows the house pattern of `heuristics/bv_simplify_options.h`: plain
 * `inline` globals in a dependency-free header, so both the algorithm and the
 * API layer can include it. `ctx::for_component` (ctx.h) copies these into
 * the component's ctx, as §5 `PUSH_EX_BLOCK` does with the spec's constants.
 * API setters (later) follow the codebase convention "0 = unlimited at the
 * surface, translated to the maximum internally"; the values here are the
 * EFFECTIVE values. Every constant is provisional pending benchmarks (§1).
 *
 * Every knob is a size or step counter (D5, fwd.h): exhausting one costs
 * precision, never soundness (inv. 3 — a graceful exit re-wraps).
 * Not thread-safe, like every other knob in the library.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__OPTIONS_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__OPTIONS_H__

#include <cstddef>

namespace idni::tau_lang::anti_prenexing {

/// §1 `K`: threshold on a clause's negative count above which
/// `SUBSUME_NEGATIVES` (finite method only) skips its O4 scan. Neither taint
/// nor flush attaches: subsumption costs no precision.
inline size_t subsume_max = 32;

/// §1 `K′`: node budget for `DECIDE_FINITE`'s BDD sweep — peak live nodes of
/// ONE sweep — past which it falls back to `ASK`. A sweep abandoned to an
/// `ASK` that ends `unknown` taints its computation.
inline size_t qbf_node_max = size_t(1) << 20;

/// §1 `K″`: threshold on a case pin's branch count above which the case
/// witness (`TRY_CASE_WITNESS`, `TRY_WITNESS_DEEP`) declines. Phase 2 reads
/// this constant bare — it predates any ctx.
inline size_t case_max = 16;

/// §1 `K‴`: budget on cases built by `EXPAND`, shared by `TRY_DECOMPOSE`'s
/// arms, counted per component (`ctx.expand_count`). A hit re-wraps the
/// still-unexpanded members as one pending block and taints.
inline size_t expand_max = size_t(1) << 14;

/// §1 `γ`: growth factor of the per-component size acceptance (§5): a
/// component push whose result exceeds `max(γ·|input|, accept_floor)` is
/// discarded for the re-wrapped input. Neither taint nor flush attaches.
inline size_t accept_growth = 16;

/// §1: absolute `|·|` under which acceptance never fires.
inline size_t accept_floor = size_t(1) << 20;

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__OPTIONS_H__
