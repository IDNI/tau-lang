// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file options.h
 * @brief Anti-prenexing foundations (layer 0): the eight knobs of the §1 ctx
 * table as process-wide defaults. The last two — `propagate_growth`, which
 * belongs to `SIMPLIFY`, and `absorb_occ_max`, which belongs to the result
 * joins — are process-wide ONLY: both run in every phase without a ctx, so
 * neither is copied into one and every use reads them bare.
 *
 * Plain `inline` globals in a dependency-free header — the pattern of
 * `heuristics/bv_simplify_options.h` — so both the algorithm and the API
 * layer can include it. `ctx::for_component` (ctx.h) copies these into the
 * component's ctx, as §5 `PUSH_EX_BLOCK` does with the spec's constants. The
 * values here are the EFFECTIVE ones; API setters follow the codebase
 * convention that `0` means "unlimited" at the surface and is translated to
 * the maximum internally. Every constant is provisional pending benchmarks
 * (§1).
 *
 * Every knob is a size limit or a step count. Exhausting one costs precision,
 * never soundness: by invariant 3 the algorithm can always exit gracefully,
 * re-wrapping the block it was pushing. Some budget hits also TAINT the
 * computation, which means its result is returned but not cached (ctx.h).
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
/// this constant directly: it runs before any component, so there is no ctx.
inline size_t case_max = 16;

/// §1 `K‴`: budget on the cases `EXPAND` builds, shared with
/// `TRY_DECOMPOSE`'s arms and counted per component (`ctx.expand_count`). A
/// hit re-wraps the still-unexpanded members as one pending block and
/// taints.
inline size_t expand_max = size_t(1) << 14;

/// §1 `γ`: growth factor of the per-component size acceptance (§5). A
/// component push whose result exceeds `max(γ·|input|, accept_floor)` is
/// discarded in favour of the re-wrapped input. Neither taint nor flush
/// attaches.
inline size_t accept_growth = 16;

/// §1: absolute `|·|` below which the size acceptance never fires.
inline size_t accept_floor = size_t(1) << 20;

/// §1 `propagate_growth`: cap on `SIMPLIFY`'s pin environment (§3). Once
/// `Σ‖witnesses‖` would exceed this factor times `Σ‖TERM_OF(pinning
/// conjunct)‖`, the pass admits no further pin. Not a component knob: read
/// bare, here. Precision, never soundness.
inline size_t propagate_growth = 4;

/// §1 `absorb_occ_max`: occurrence limit of the result joins' absorption pass
/// (§3). A part occurring in more members than this is no candidate key, and
/// a member all of whose parts exceed it stays unabsorbed. Not a component
/// knob: read bare, here. Precision, never soundness.
inline size_t absorb_occ_max = 32;

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__OPTIONS_H__
