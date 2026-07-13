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
 *
 * @par Example
 * @code{.cpp}
 * // "x * y = z" (non-constant multiplier, unblastable by bvmul): the
 * // top-level atomic wff is tainted (pass 2). No variables get tainted
 * // directly here (get_free_vars finds none on this bf-level subtree), so
 * // the taint set has exactly one member: the whole atom.
 * tref fm = get_nso_rr("x:bv[4] * y:bv[4] = z:bv[4].").value().main->get();
 * auto tainted = collect_bv_arithmetic_taint<node_t>(fm);
 * CHECK( tainted.size() == 1 );
 * CHECK( tainted.contains(fm) );
 *
 * // No arithmetic operator anywhere -> nothing tainted.
 * tref plain = get_nso_rr("x:bv[4] = y:bv[4].").value().main->get();
 * CHECK( collect_bv_arithmetic_taint<node_t>(plain).empty() );
 * @endcode
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
 *
 * @par Example
 * @code{.cpp}
 * tref fm = get_nso_rr("x:bv[4] * y:bv[4] = z:bv[4].").value().main->get();
 * auto skip = make_bv_arithmetic_skip<node_t>(fm);
 * CHECK( skip(fm) );  // fm itself is the tainted atom (see collect_bv_arithmetic_taint)
 *
 * tref other = get_nso_rr("x:bv[4] = z:bv[4].").value().main->get();
 * CHECK( !skip(other) );
 * @endcode
 */
template <NodeType node>
std::function<bool(tref)> make_bv_arithmetic_skip(tref formula);

} // namespace idni::tau_lang

#include "bv_arithmetic_scan.tmpl.h"

#endif // __IDNI__TAU__BV_ARITHMETIC_SCAN_H__
