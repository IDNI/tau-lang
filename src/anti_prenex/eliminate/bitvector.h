// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file bitvector.h
 * @brief §7 "Bitvector — the router": `ELIMINATE_BITVECTOR_CLAUSE`, the
 * method of the bitvector family, with its `opaque?` and `SPLIT_ARITHMETIC`.
 *
 * TWO ENGINES SERVE `bv[n]`, split by translatability. The PURE fragment —
 * (¬)equations over `∪ · ′` and the ring sum, functional quantifiers
 * included — is the finite method's (finite.h), decided without the solver.
 * The rest is the solver's: a block variable under an arithmetic operator
 * admits no Shannon expansion, so no BDD-engine move applies to it (§1 leaf
 * hazard), and an order atom is arithmetic. Tau writes the ring sum `^`; its
 * `+` is arithmetic.
 *
 * EVERY ARITHMETIC CONJUNCT IS THE SOLVER'S: the linear-arithmetic
 * conversion that would move one to the pure side (`IS_LINEAR_ARITHMETIC`,
 * `LINEAR_ARITHMETIC_TO_BA`) is §9's open item.
 *
 * WHAT NEITHER ENGINE READS FREEZES (invariant 3): a reference, a unit whose
 * body the solver cannot translate, and an impure conjunct carrying a
 * functional quantifier — the solver has no term-level binder, and the
 * finite side reads pure content alone.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__ELIMINATE__BITVECTOR_H__
#define __IDNI__TAU__ANTI_PRENEX__ELIMINATE__BITVECTOR_H__

#include "../foundations/fwd.h"
#include "../foundations/ctx.h"
#include "../foundations/dag.h"
#include "../foundations/terms.h"
#include "../foundations/prims.h"
#include "../shared/parts.h"
#include "../normalisers/joins.h"
#include "conditions.h"
#include "finite.h"
#include "ask.h"

namespace idni::tau_lang::anti_prenexing {

/**
 * @brief §7 the router's `opaque?`: can NEITHER engine read @p conjunct?
 *
 * Opaque: a conjunct with a REFERENCE anywhere in it; a TEMPORAL OPERATOR,
 * which no engine opens (§4); a BINDER UNIT whose body the solver cannot
 * translate — a reference, a functional quantifier, or a variable or
 * constant of a type other than @p type inside; and an IMPURE conjunct
 * carrying a FUNCTIONAL QUANTIFIER. A BDD-backed term is read leaf by leaf.
 *
 * Everything else one engine reads: a pure conjunct the finite side, an
 * impure one without a chain the solver.
 *
 * @param type the block's type, which purity is read under
 */
template <NodeType node>
bool bitvector_opaque(tref conjunct, ba_type_id type);

/// §7 `SPLIT_ARITHMETIC`'s result: the arithmetic side `A` over `X_A` and
/// the pure side `B` over `X_B`, variable-disjoint, each in input order.
template <NodeType node>
struct split_result {
	trefs A;
	block X_A;
	trefs B;
	block X_B;
};

/**
 * @brief §7 `SPLIT_ARITHMETIC(clause, X)`: the impure conjuncts, with every
 * conjunct they reach through shared block variables, on the solver's side
 * `A`; the rest, all pure, on the finite side `B` — so that
 * `∃X(A ∧ B) ≡ ∃X_A.A ∧ ∃X_B.B`.
 *
 * PURE is `is_pure_formula`: a pure (¬)equation, functional-quantifier
 * terms included, a negative tree of pure literals, or a binder unit with a
 * recursively pure body. The closure is §6 `PARTS`: a part holding an
 * impure conjunct goes to `A` whole, and parts share no block variable.
 *
 * @param type the block's type, which purity is read under
 */
template <NodeType node>
split_result<node> split_arithmetic(const trefs& conjuncts, const block& X,
	ba_type_id type);

/**
 * @brief §7 `ELIMINATE_BITVECTOR_CLAUSE(clause, X, ctx)`: a formula
 * equivalent to `∃X.⋀conjuncts` over a bitvector type.
 *
 * The method contract (§7) holds on entry. FREEZE what neither engine
 * reads, SPLIT the rest; the pure side is `ELIMINATE_FINITE_CLAUSE`'s, and
 * its `F` decides; the arithmetic side is `SOLVE_ARITHMETIC`'s — one solver
 * query with its binder units swallowed as written, decided closed as it is
 * or open by closing its free variables both ways. A pure conjunct carrying
 * a functional quantifier lands there when the split's closure ties it to an
 * impure one; the side is then re-wrapped whole, never asked.
 *
 * The per-side closings lose no validity, but UNSATISFIABILITY does not
 * distribute: the two sides may share a free variable satisfiable only
 * under incompatible assignments, and ONE JOINT ASK over both catches that.
 * It is skipped when either side carries a functional quantifier, which
 * the solver cannot read.
 *
 * `F` comes out only where the clause is unsatisfiable; what no engine
 * decides is re-wrapped, never answered `F` (invariant 3).
 */
template <NodeType node>
tref eliminate_bitvector_clause(const trefs& conjuncts, const block& X,
	ctx<node>& c);

} // namespace idni::tau_lang::anti_prenexing

#include "bitvector.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__ELIMINATE__BITVECTOR_H__
