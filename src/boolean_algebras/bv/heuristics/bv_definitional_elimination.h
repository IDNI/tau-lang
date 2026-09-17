/**
 * @file bv_definitional_elimination.h
 * @brief Elimination of existentially quantified bitvector variables that a
 * total definition determines.
 *
 * Provides `bv_eliminate_definitional_existentials`, which substitutes a
 * bitvector variable bound by `ex` where a definition in its scope forces
 * its value in every case, and drops the binder.
 */

// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_DEFINITIONAL_ELIMINATION_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_DEFINITIONAL_ELIMINATION_H__

#include <algorithm>
#include <cstdint>
#include <functional>
#include <limits>
#include <map>
#include <optional>
#include <utility>
#include <vector>

#include "nso_rr.h"

namespace idni::tau_lang {

/// Elimination of definitional existentials before the case split (see
/// `bv_eliminate_definitional_existentials` in
/// boolean_algebras/bv/heuristics/bv_definitional_elimination.tmpl.h). Off
/// by default; an identity on the formula. Enabled via
/// `--bv-definitional-elimination` or the REPL option
/// `bv-definitional-elimination` (GitHub #124).
inline bool bv_definitional_elimination = false;

/// Cap on the clauses a conjunct mentioning the variable may be flattened
/// into (`D || (A && B)` distributes to `(D || A), (D || B)`, exponential in
/// the nesting); a conjunct beyond it stays a reader. SIZE_MAX = unlimited
/// (0 through the option setter); the REPL/CLI option
/// `bv-defelim-max-clauses`.
inline size_t bv_defelim_max_clauses = 16;

/// Cap on the distinct atoms the propositional check of a guard set may
/// brute-force over (2^atoms assignments); above it the set is not refuted
/// and the definition is not used. Never more than 30 whatever the setting.
/// The option `bv-defelim-max-atoms`.
inline size_t bv_defelim_max_atoms = 18;

/// Cap on the size of the clause subsets searched for a total definition
/// (all subsets up to this size are tried). The option
/// `bv-defelim-max-subset`.
inline size_t bv_defelim_max_subset = 4;

/// Cap on the elimination rounds per existential block (one variable is
/// eliminated per round). SIZE_MAX = unlimited (0 through the setter); the
/// option `bv-defelim-max-rounds`.
inline size_t bv_defelim_max_rounds = 256;

// Eliminate the bitvector variables of a block of same-kind existential
// binders that a total definition determines.
//
// The block is the run of `ex` binders in conjunct position hanging off
// `root`. For a bound bitvector x, the conjuncts of a block scope that
// mention x are flattened into clauses `D_i || x = c_i` with x-free D_i (a
// bare equation has D_i = F; clauses with the same c merge). The smallest
// subset whose D-conjunction is propositionally unsatisfiable is a total
// definition: it forces x in every cell. If every pair `D_i || D_j` is valid
// the cells are exclusive and
//
//     ex x (definition && psi(x))  ==  \/_i (!D_i && psi[x := c_i])
//
// which is T when nothing reads x. Non-exclusive cells keep the consistency
// atoms `D_j || c_i = c_j`. Variables are taken in reverse dependency order
// (one whose witnesses no other definition mentions first), a witness that
// a binder inside the scope would capture declines the substitution, and a
// variable without a total definition keeps its binder. An identity on the
// formula.
//
// Why it is here: a run's closure quantifies the outputs existentially, and
// an output defined by arithmetic and read by k guarded comparisons reaches
// the case split and the solver as an `ex` chain nested by occurrence; the
// case split then carries the chain into every instance (GitHub #124). The
// substitution pass `ex_subs_based_elimination` runs inside anti-prenexing,
// after both, and knows no conditional definitions.
//
// `root` is a `wff` whose child is the outermost `wff_ex` of the block; the
// rewritten block is returned, or `root` when nothing applies.
template <NodeType node>
tref bv_definitional_block_elimination(tref root);

// Apply `bv_definitional_block_elimination` to every existential block of
// `fm`, outermost first (pre-order continues into the rewritten node).
template <NodeType node>
tref bv_eliminate_definitional_existentials(tref fm);

} // namespace idni::tau_lang

#include "bv_definitional_elimination.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_DEFINITIONAL_ELIMINATION_H__
