// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file bv_widening.h
 * @brief Bottom-up bitwidth computation for opt-in exact bitvector arithmetic.
 *
 * Part of the `bv_widening` elaboration pass (see `bv_widening_options.h`):
 * when enabled, arithmetic atoms are elaborated to compute at a width wide
 * enough to be overflow-free, then truncated back at the point where the
 * result is actually assigned/compared. `needed_width` computes that width
 * bottom-up over a single `bf` (bitvector term) subtree.
 *
 * Width rules (`W` = the operand width(s) as computed by recursion, `base_w`
 * = the width contributed by an untyped/typed leaf):
 *  - `variable`, `ba_constant`, `bf_t`, `bf_f` (leaves): `base_w`.
 *  - `bf_parenthesis`: transparent -- the width of its single inner `bf`.
 *  - `bf_cast`: a boundary -- its own target type's width, regardless of
 *    what is inside (the operand is re-elaborated independently, later, by
 *    the caller).
 *  - `bf_add`: `max(l, r) + 1` (a carry-out bit may be needed).
 *  - `bf_sub`: `max(l, r)` (two's-complement subtraction does not grow).
 *  - `bf_mul`: `l + r` (the product of an l-bit and an r-bit value fits in
 *    `l + r` bits).
 *  - `bf_div`, `bf_mod`: `l` (the left/dividend operand's width; the result
 *    magnitude is bounded by the dividend).
 *  - `bf_min`, `bf_max`, `bf_and`, `bf_or`, `bf_xor`, `bf_nand`, `bf_nor`,
 *    `bf_xnor`: `max(l, r)` (bitwise/selection ops never grow beyond the
 *    wider operand).
 *  - `bf_neg`: `l` (bitwise complement does not change the width).
 *  - `bf_shr`: `l` (a right shift never grows the needed width).
 *  - `bf_shl`: `l + k` where `k` is the shift amount when it is a literal
 *    bitvector constant, else `0` (a variable shift amount executes at
 *    width `l` and may wrap -- the elaboration pass cannot know `k` ahead
 *    of time).
 *  - anything else (e.g. `bf_ref`, `capture`): opaque -- returns `0`.
 *
 * Any `0` returned by a child call propagates: an operator with an opaque
 * child is itself opaque, and returns `0`. Callers are expected to skip
 * (leave untouched) any atom for which the top-level `needed_width` call
 * returns `0`.
 */

#ifndef __IDNI__TAU__BV_WIDENING_H__
#define __IDNI__TAU__BV_WIDENING_H__

#include "tau_tree.h"
#include "ba_types.h"
#include "boolean_algebras/bv_ba.h" // Only for IDE resolution, not really needed.
#include "bv_widening_options.h"

namespace idni::tau_lang {

/**
 * @brief Compute the exact-result bitwidth needed to evaluate a `bf` subtree
 * without silently wrapping/overflowing, updating the running maximum.
 *
 * @tparam node Tree node type.
 * @param bf A `bf`-nonterminal node ref (the tree wraps every operator/leaf
 *   in a `bf` node -- see the doc comment above for the per-operator rules).
 * @param base_w The width contributed by a leaf (`variable`, `ba_constant`,
 *   `bf_t`, `bf_f`) that is untyped or whose declared width should not be
 *   consulted here (the caller decides `base_w`, typically the atom's own
 *   declared/inferred width).
 * @param maxW [in,out] Running maximum width seen while descending the
 *   subtree; the caller initializes it (typically to `0`) before the
 *   top-level call.
 * @return The subtree's exact-result width, or `0` if `bf` (or any
 *   descendant) is an opaque subterm (e.g. `bf_ref`, `capture`) that the
 *   elaboration pass cannot reason about -- the caller should then skip the
 *   enclosing atom entirely.
 *
 * @par Example
 * @code{.cpp}
 * // "x * y", both operands untyped (base width 8): needed width is 16.
 * auto opts = tau::get_options{ .parse = { .start = tau::bf },
 *                                .infer_ba_types = false,
 *                                .reget_with_hooks = false };
 * tref src = tree<node_t>::get("x * y", opts);
 * size_t maxW = 0;
 * CHECK( needed_width<node_t>(src, 8, maxW) == 16 );
 * CHECK( maxW == 16 );
 * @endcode
 */
template <NodeType node>
size_t needed_width(tref bf, size_t base_w, size_t& maxW);

/**
 * @brief Rebuild a `bf` subtree at width `W`, upcasting leaves and
 * retyping every operator node to `bv[W]` -- the term-level half of the
 * `bv_widening` atom-elaboration pass (see `widen_atom` for the atom-level
 * rules that call this).
 *
 * Every leaf (`variable`, `ba_constant`, `bf_t`, `bf_f`) is wrapped in a
 * single zero-extending `(bv[W])` cast (`build_bf_cast`). An existing
 * user-written `bf_cast` is treated the same way -- it is a boundary (per
 * the design: "the pass treats a cast operand as an independent
 * sub-computation whose result is the cast's declared width"), so its own
 * operand is left completely untouched and the whole existing cast is
 * simply wrapped in one more, outer, `(bv[W])` cast.
 *
 * Every other operator node (`bf_add`, `bf_sub`, `bf_mul`, `bf_div`,
 * `bf_mod`, `bf_min`, `bf_max`, `bf_and`, `bf_or`, `bf_xor`, `bf_nand`,
 * `bf_nor`, `bf_xnor`, `bf_neg`, `bf_shl`, `bf_shr` -- the bitwise ops and
 * both shift operands included, per the amended D2/D3 rule that ALL
 * operators run at `W`) is rebuilt with its children widened first, then
 * retyped `bv[W]` via `tree<node>::get_typed`. There are no interior
 * truncations anywhere.
 *
 * `bf_parenthesis` needs no special case: like `bf_neg` it wraps a single
 * child, so it falls into the same generic rebuild and is simply retyped
 * `bv[W]` -- it is a purely transparent grouping wrapper, so widening it
 * this way does not change what it means.
 *
 * @tparam node Tree node type.
 * @param bf_node A `bf`-nonterminal node ref (see `needed_width`'s doc
 *   comment above for the `bf -> op -> ...` shape every `bf` subtree has).
 * @param base_w The already-declared/unified width of the atom this term
 *   belongs to (used only to decide whether any work is needed at all).
 * @param W The target width to elaborate at (`>= base_w`, computed by the
 *   caller via `needed_width`).
 * @return `bf_node` unchanged (same tref) when `W == base_w` -- the no-op
 *   fast path; otherwise a newly built (or hash-consed-identical) `bf`
 *   node, always typed `bv[W]`.
 */
template <NodeType node>
tref widen_term(tref bf_node, size_t base_w, size_t W);

/**
 * @brief Elaborate one bv-family atomic formula per the `bv_widening`
 * mode's rules (design spec section 2): compute the atom's overflow-free
 * width `W` (via `needed_width` on every side), then either extend every
 * side exactly or extend one side and wrap it in a truncating cast:
 *
 *  - **Comparisons** (`bf_lt`, `bf_nlt`, `bf_lteq`, `bf_nlteq`, `bf_gt`,
 *    `bf_ngt`, `bf_gteq`, `bf_ngteq`), **`bf_interval`**, and an
 *    **equality/inequality between two compound sides**: every side is
 *    rebuilt with `widen_term(side, base_w, W)` and the atom is
 *    reassembled with the same nt -- both/all sides end up typed `bv[W]`,
 *    exactly, with no truncation.
 *  - **`bf_eq`/`bf_neq` with exactly one bare-storage side** (a
 *    `variable` -- which covers io_vars and uninterpreted constants
 *    alike, they all parse into `tau::variable`, see
 *    `parser/tau.tgf:149`, possibly under transparent `bf_parenthesis`
 *    wrappers): the bare side is left completely untouched, the other
 *    side is elaborated at `W` via `widen_term`, and the result is
 *    wrapped in a truncating `build_bf_cast<node>(..., bv_type_id<node>(
 *    base_w))` -- "assignment" semantics.
 *
 * @tparam node Tree node type.
 * @param atom A node ref whose own nt is one of `bf_eq`, `bf_neq`,
 *   `bf_lt`, `bf_nlt`, `bf_lteq`, `bf_nlteq`, `bf_gt`, `bf_ngt`,
 *   `bf_gteq`, `bf_ngteq`, or `bf_interval` -- the comparison/equality
 *   node itself, *not* the enclosing `wff` wrapper that `build_bf_eq`
 *   (etc.) puts around it. Its two (three for `bf_interval`) children are
 *   `bf`-nonterminal sides.
 * @return `atom` unchanged (same tref) when: its own BA type is not
 *   bv-family (nothing to elaborate); every side is ALREADY uniformly
 *   expressed at the atom's own current width (`is_side_saturated_at` --
 *   the idempotency guard for the extend-all-sides shapes: re-running
 *   `needed_width` on an already-widened comparison/interval/
 *   both-compound-equality atom would otherwise inflate `W` without bound,
 *   since a `bf_cast` boundary's declared width is always trusted at face
 *   value); any side is opaque to `needed_width` (a `bf_ref`/`capture`/...
 *   subterm the pass cannot reason about); or the computed `W` equals the
 *   atom's already-declared width (this is what makes repeated application
 *   idempotent for the truncating-assignment shape specifically, since the
 *   outer truncating cast resets the rebuilt atom's own auto-propagated
 *   type back down to `base_w` -- the saturation guard above never matches
 *   that shape, because its untouched bare side is never "saturated").
 *   Returns `nullptr`, after `LOG_ERROR`-ing the cap violation, when the
 *   computed `W` exceeds `bv_max_width` (the D4 width cap).
 */
template <NodeType node>
tref widen_atom(tref atom);

} // namespace idni::tau_lang

#include "bv_widening.tmpl.h"

#endif // __IDNI__TAU__BV_WIDENING_H__
