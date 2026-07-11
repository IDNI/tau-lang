/**
 * @file bv_arithmetic_scan.h
 * @brief Identify bitvector content that genuinely needs arithmetic (CVC5) reasoning.
 *
 * After bitvector predicate blasting, some atoms may survive unblasted
 * (e.g. multiplication/division by a non-constant) together with the
 * bitvector variables they operate on. This heuristic collects that
 * residue so the normalizer's block-antiprenexing pass can be told, via a
 * `skip` predicate, to leave it untouched for the solver rather than
 * attempting to Boole-decompose it.
 */

// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BV_ARITHMETIC_SCAN_H__
#define __IDNI__TAU__BV_ARITHMETIC_SCAN_H__

namespace idni::tau_lang {

/**
 * @brief Collect bitvector arithmetic residue in @p formula.
 *
 * Walks @p formula and collects (i) every bitvector-typed variable
 * appearing as an operand of an arithmetic bf operator (`bf_add`, `bf_sub`,
 * `bf_mul`, `bf_div`, `bf_mod`, `bf_shl`, `bf_shr`, `bf_cast`) anywhere in
 * the formula, and (ii) every atomic wff (comparison) node whose subtree
 * contains such an arithmetic operator anywhere below it.
 *
 * @tparam node Tree node type.
 * @param formula Formula to scan (typically the result of bv predicate blasting).
 * @return Set of variable/atom nodes that still require arithmetic reasoning.
 */
template <NodeType node>
subtree_unordered_set<node> collect_bv_arithmetic_taint(tref formula);

/**
 * @brief Build a `skip` predicate from `collect_bv_arithmetic_taint`'s result.
 *
 * The returned predicate answers `true` for a queried variable or atomic
 * wff node iff it is a member of the taint set collected from @p formula.
 * Must be (re-)built from the exact formula snapshot it will be used to
 * guard, immediately before the `anti_prenex_block` call it feeds.
 *
 * @tparam node Tree node type.
 * @param formula Formula to scan (typically the result of bv predicate blasting).
 * @return Predicate suitable as `anti_prenex_block`'s `skip` argument.
 */
template <NodeType node>
std::function<bool(tref)> make_bv_arithmetic_skip(tref formula);

} // namespace idni::tau_lang

#include "bv_arithmetic_scan.tmpl.h"

#endif // __IDNI__TAU__BV_ARITHMETIC_SCAN_H__
