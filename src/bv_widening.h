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

} // namespace idni::tau_lang

#include "bv_widening.tmpl.h"

#endif // __IDNI__TAU__BV_WIDENING_H__
