// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file eliminate_block.tmpl.h
 * @brief Template implementations for eliminate_block.h. eliminate_block.h
 * says what each procedure means; the comments here say how it is built.
 *
 * The pieces come from the layers below: the type predicates of ba_types.h
 * for the table, `fv_intersect` / `fv_meets` / `members` (dag.h) for the
 * pre-steps, `try_witness` in its COF mode (witness/witness.h) for the
 * witness pass, `rewrap` (prims.h) for the key and for every graceful exit,
 * `memoised` (ctx.h) for the table, and `eliminate_atomless_clause`
 * (atomless.h) for the one method there is.
 *
 * ITS CALLERS come with the push: `PUSH_BLOCK`'s dispatch at a leaf, the
 * whole-formula fast paths, and `PUSH_OVER_CONJUNCTION`'s settle move and
 * consistency check. None of them exists yet, so nothing in the module calls
 * this file.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__ELIMINATE__ELIMINATE_BLOCK_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__ELIMINATE__ELIMINATE_BLOCK_TMPL_H__

#include <cassert>
#include <optional>
#include <utility>
#include <vector>

namespace idni::tau_lang::anti_prenexing {

// --- the type table ---------------------------------------------------------------

template <NodeType node>
method_kind method(ba_type_id type) {
	if (is_tau_type<node>(type) || is_sbf_type<node>(type))
		return method_kind::atomless;
	if (is_bv_type_family<node>(type)) return method_kind::bitvector;
	// Not a row: a block of this type must never have reached the push.
	DBG(assert(false && "method: this BA type is outside the algorithm");)
	return method_kind::none;
}

template <NodeType node>
bool ex_distributes_over_negatives(ba_type_id type) {
	// ONE row, read twice.
	return method<node>(type) == method_kind::atomless;
}

// --- the wrapper ------------------------------------------------------------------

template <NodeType node>
tref eliminate_block(tref clause, const block& X, ctx<node>& c) {
	using tau = tree<node>;
	DBG(assert(clause != nullptr);)
	if (tau::get(clause).equals_T() || tau::get(clause).equals_F())
		return clause;
	// Nothing enters the key that `∃X` does not scope.
	const block Xc = fv_intersect<node>(clause, X);
	if (Xc.empty()) return clause;
	// The wrap node carries the ordered `X`, so formula and block are one
	// key part; the flag joins it because the entry embeds `DISCHARGE`'s
	// output, which the flag switches (§1).
	const std::pair<tref, bool> key{ rewrap<node>(clause, Xc),
		c.keep_functional };
	// The wrapper carries §1's taint rule, and is a plain computation
	// wherever the table does not exist.
	return memoised<node, table::elim_memo>(key, [&] {
		return eliminate_block_uncached<node>(clause, Xc, c); });
}

// --- the pre-steps and the dispatch -------------------------------------------------

template <NodeType node>
tref eliminate_block_uncached(tref clause, const block& X, ctx<node>& c) {
	using tau = tree<node>;
	// 1. THE WITNESS PASS. The loop reads the block as it arrived and
	// writes a copy, so a variable witnessed away is gone from what the
	// steps below see while the pass itself walks every variable once.
	block left = X;
	for (tref x : X) {
		if (tau::get(clause).equals_T() || tau::get(clause).equals_F())
			break;
		// COF mode: the conjuncts' terms are BDD-backed under
		// `ctx.order`, and the pin is read off their cofactors (§3).
		const std::optional<tref> witnessed =
			try_witness<node>(x, clause, c);
		if (!witnessed) continue;   // `x` stays, the clause is untouched
		clause = *witnessed;
		std::erase_if(left, [x](tref v) {
			return tau::subtree_equals(v, x); });
	}
	if (tau::get(clause).equals_T() || tau::get(clause).equals_F())
		return clause;
	// 2. THE STRIP, one cached free-variable test per conjunct. The member
	// view of the clause is taken HERE, once, and the methods below work
	// on the list. ONLY AN ∧-NODE HAS ONE: `members` flattens either
	// connective, and a clause that is an ∨-node is a NEGATIVE TREE (§1),
	// which is ONE conjunct and goes whole to the method.
	const trefs ms = is_child<node>(clause, tau::wff_and)
		? members<node>(clause) : trefs{ clause };
	trefs indep, dep;
	for (tref m : ms) (fv_meets<node>(m, left) ? dep : indep).push_back(m);
	// `X` narrowed to what the dependent conjuncts still hold, in `X`'s
	// order: a variable whose last occurrence the witness substituted away
	// scopes nothing any more.
	subtree_unordered_set<node> held;
	for (tref m : dep)
		for (tref v : fv_intersect<node>(m, left)) held.insert(v);
	block Xd;
	for (tref v : left) if (held.contains(v)) Xd.push_back(v);
	if (Xd.empty()) return simplified_and_join<node>(ms);
	// 3. THE DISPATCH. `indep` rides outside the block either way, so the
	// method's answer — or the re-wrap that stands in for it — is one more
	// operand of the same join.
	switch (method<node>(c.type)) {
	case method_kind::atomless:
		indep.push_back(eliminate_atomless_clause<node>(dep, Xd, c));
		break;
	case method_kind::bitvector:   // §7's router, with no arm here yet
	case method_kind::none:
		// No method for this type: the block comes back around the
		// dependent conjuncts, undecided (invariant 3).
		indep.push_back(rewrap<node>(
			simplified_and_join<node>(dep), Xd));
		break;
	}
	return simplified_and_join<node>(indep);
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__ELIMINATE__ELIMINATE_BLOCK_TMPL_H__
