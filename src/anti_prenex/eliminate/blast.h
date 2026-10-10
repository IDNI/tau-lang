// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file blast.h
 * @brief §7 `BIT_BLAST` and `DECIDE_FINITE`: the truth of a closed pure
 * query over one bitvector type, decided by a propositional BDD sweep, with
 * the solver behind it.
 *
 * THE BLAST rides the isomorphism `bv[n] ≅ bool^n` (§7): every pure
 * operation acts per bit, so a term is `n` propositional functions, one per
 * SLICE, an equation the conjunction of its slices' equivalences, and a
 * binder the quantification of its variable's `n` bits. It is
 * compositional, so the blast and the sweep are ONE walk: each formula node
 * yields one BDD, each term node one BDD per slice, and a binder abstracts
 * its bits on its body's BDD as the walk returns. The quantified Boolean
 * formula itself is never built, and `BIT_BLAST` has no function of its own.
 *
 * THE UNIVERSE is the sweep's own, `bdd<Bool, sweep_options>`, never the
 * default one behind `sbf`. It is process-wide and append-only: its nodes
 * are never freed, and one node serves every query of its shape.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__ELIMINATE__BLAST_H__
#define __IDNI__TAU__ANTI_PRENEX__ELIMINATE__BLAST_H__

#include <optional>

#include "../foundations/fwd.h"
#include "../foundations/ctx.h"
#include "../foundations/dag.h"
#include "../foundations/terms.h"
#include "../foundations/prims.h"
#include "../../ba_types.h"
#include "../../boolean_algebras/bdds/bdd_handle.h"
#include "ask.h"

namespace idni::tau_lang::anti_prenexing {

/// The sweep's BDD universe: variable shifters on, since under slice-major
/// ids the slices of one pure term are shifted copies of one skeleton, which
/// the shifters store once; the node cap on, so that the node budget stops
/// an operation as it allocates; 2^40 nodes, so that the node budget is the
/// only limit; 2^22 variable ids.
inline constexpr auto sweep_options =
	bdd_options<INV_IN | INV_OUT | VARSHIFT | NODE_CAP>::create(40, 22);

/**
 * @brief §7 `DECIDE_FINITE(q, ctx)`: the truth of the CLOSED pure query
 * @p q, every variable of the bitvector type `ctx.type` — `true`, `false`,
 * or nothing, the spec's `unknown`.
 *
 * In order: `qbf_memo`; a `sat` / `unsat` entry of `solver_memo`, where
 * `unknown` is a miss; the BDD sweep over `BIT_BLAST(q)` within
 * `ctx.qbf_node_max` new nodes, whose verdict is recorded in `qbf_memo`;
 * then `ASK`. Nothing comes back only when the sweep did not finish and the
 * solver answered `unknown` — the one taint source of the elimination (§1
 * cache scope). A query whose bits exceed the universe's 2^22 variable ids
 * is not swept at all, and goes to `ASK`. The clock is read before the
 * sweep and again in `ASK` (§3 time limit); a cached verdict reads none.
 *
 * `BIT_BLAST`'s `FAIL`: a node the sweep has no rule for — an order atom, a
 * `bf_neq`, an arithmetic operator or a width cast, a reference, a temporal
 * operator, a node of another type, a symbolic constant — throws
 * `std::logic_error` in every build. Such a node is a routing bug, never a
 * verdict (§7). A free variable, which the contract excludes, throws the
 * same way.
 *
 * The caller canonicalises @p q (`canonicalise_binder_ids`): it is the memo
 * key as it stands.
 */
template <NodeType node>
std::optional<bool> decide_finite(tref q, ctx<node>& c);

} // namespace idni::tau_lang::anti_prenexing

#include "blast.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__ELIMINATE__BLAST_H__
