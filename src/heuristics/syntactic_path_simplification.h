// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file syntactic_path_simplification.h
 * @brief Path-based syntactic simplification heuristic.
 *
 * Detects syntactically contradictory or tautological sub-formulas along every
 * conjunction path and replaces them by `F`/`T` (or `0`/`1` for bf). Works
 * purely syntactically (no semantic normalization) and runs in near-linear time
 * in the formula size and the number of paths.
 */

#ifndef __IDNI__TAU__SYNTACTIC_PATH_SIMPLIFICATION_H__
#define __IDNI__TAU__SYNTACTIC_PATH_SIMPLIFICATION_H__

namespace idni::tau_lang {

/**
 * @brief Simplify contradictions along paths, then resolve tautologies.
 *
 * Converts @p fm to NNF, traverses every conjunction path, and replaces
 * syntactically contradictory sub-formulas with `F`/`0` or tautological ones
 * with `T`/`1`. Handles both wff (formulas) and bf (boolean terms).
 *
 * @tparam node Tree node type.
 * @param fm Formula or boolean term to simplify.
 * @return Simplified formula/term.
 */
template <NodeType node>
tref syntactic_path_simplification(tref fm);

/**
 * @brief Simplify contradictions along paths without manipulating existing negations.
 *
 * Like `syntactic_path_simplification` but skips the tautology-resolution pass
 * and does not push negations inward. Useful when negations must be preserved.
 *
 * @tparam node Tree node type.
 * @param fm Formula or boolean term to simplify.
 * @return Simplified formula/term with only contradictions removed.
 */
template <NodeType node>
tref syntactic_path_simplification_unsat_on_unchanged_negations(tref fm);

} // namespace idni::tau_lang

#include "syntactic_path_simplification.tmpl.h"

#endif // __IDNI__TAU__SYNTACTIC_PATH_SIMPLIFICATION_H__
