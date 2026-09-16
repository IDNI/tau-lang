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

#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace idni::tau_lang {

/**
 * @brief Simplify contradictions and tautologies along every path.
 *
 * Converts @p fm to NNF, normalises the ordering atoms, then sweeps it once
 * with both kinds of assumption in force along every path: a conjunct is
 * true in its siblings, a disjunct false in its siblings. A syntactically
 * contradictory conjunction collapses to `F`/`0`, a tautological disjunction
 * to `T`/`1`. Every spelling of a literal is filed under its atom's key with
 * the right polarity, so `l != r` is read as the complement of `l = r`
 * without being rewritten, and the NNF spelling `l != r` is what the output
 * carries. Handles both wff (formulas) and bf (boolean terms).
 *
 * @tparam node Tree node type.
 * @param fm Formula or boolean term to simplify.
 * @return Simplified formula/term.
 *
 * @par Example
 * @code{.cpp}
 * // "x = 0 && !(x = 0 || y = 0)": NNF-normalizing first exposes the
 * // contradiction between "x = 0" and "x != 0", collapsing to F (see
 * // tests/integration/test_integration-heuristics-syntactic_path_simplification.cpp:209-214).
 * tref fm = get_nso_rr("x = 0 && !(x = 0 || y = 0).").value().main->get();
 * tref res = syntactic_path_simplification<node_t>(fm);
 * CHECK( tau::get(res).equals_F() );
 * @endcode
 */
template <NodeType node>
tref syntactic_path_simplification(tref fm);

/**
 * @brief Simplify contradictions along paths without manipulating existing negations.
 *
 * Like `syntactic_path_simplification` but with conjunction assumptions only
 * (no tautology folding), no NNF conversion and no atom normalisation: the
 * input's negations are left exactly where they are. A literal is still
 * recognised as the complement of its atom in every spelling (`!(x = 0)`,
 * `x != 0`, `a <= b` against `b < a`), so a contradiction between them is
 * found without rewriting either.
 *
 * @tparam node Tree node type.
 * @param fm Formula or boolean term to simplify.
 * @return Simplified formula/term with only contradictions removed.
 *
 * @par Example
 * @code{.cpp}
 * // Both spellings of the contradiction collapse, and neither is rewritten
 * // on the way (see
 * // tests/integration/test_integration-heuristics-syntactic_path_simplification.cpp).
 * tref fm1 = get_nso_rr("x = 0 && !(x = 0).").value().main->get();
 * CHECK( tau::get(syntactic_path_simplification_unsat_on_unchanged_negations<node_t>(fm1)).equals_F() );
 *
 * tref fm2 = get_nso_rr("x = 0 && x != 0.").value().main->get();
 * CHECK( tau::get(syntactic_path_simplification_unsat_on_unchanged_negations<node_t>(fm2)).equals_F() );
 * @endcode
 */
template <NodeType node>
tref syntactic_path_simplification_unsat_on_unchanged_negations(tref fm);

/**
 * @brief Simplify contradictions AND tautologies along every path without
 * manipulating existing negations.
 *
 * `syntactic_path_simplification`'s sweep — a conjunct assumed true in its
 * siblings, a disjunct assumed false in its siblings — on the formula AS
 * WRITTEN: no NNF conversion and no atom normalisation, so a `!(x = 0)` stays
 * spelled that way and no `!=` or negated ordering operator is introduced. A
 * literal is still recognised as the complement of its atom in every
 * spelling, so the folds fire as they do for the other entries.
 *
 * It is the second pass of the anti-prenexing module's `SIMPLIFY`
 * (anti_prenex.md §3), whose invariant 4 forbids the rewrites the plain entry
 * does on the way in; binder bodies are entered under the keys in force,
 * which is that spec's §4 rule for `SIMPLIFY`. A term input takes the same
 * route as in `syntactic_path_simplification`, having no negated atoms to
 * preserve.
 *
 * @tparam node Tree node type.
 * @param fm Formula or boolean term to simplify.
 * @return Simplified formula/term, with every negation where it was.
 *
 * @par Example
 * @code{.cpp}
 * // The contradiction folds, and `!(x = 0)` is not rewritten into `x != 0`.
 * tref fm = get_nso_rr("x = 0 && !(x = 0).").value().main->get();
 * CHECK( tau::get(syntactic_path_simplification_unchanged_negations<node_t>(fm)).equals_F() );
 * @endcode
 */
template <NodeType node>
tref syntactic_path_simplification_unchanged_negations(tref fm);

} // namespace idni::tau_lang

#include "syntactic_path_simplification.tmpl.h"

#endif // __IDNI__TAU__SYNTACTIC_PATH_SIMPLIFICATION_H__
