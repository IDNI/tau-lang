// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "eliminability"

namespace idni::tau_lang {

template <NodeType node>
bool eliminability_comp(tref l, tref r) {
	return tree<node>::subtree_less(l, r);
}

template <NodeType node>
block_eliminability<node> analyse_block(const trefs& block_vars,
	const trefs& conjuncts, const analysis_context<node>& ctx)
{
	using tau = tree<node>;
	block_eliminability<node> res;
	if (block_vars.empty()) return res;

	// Gates only the `arith_tainted` PRECOMPUTE below -- never the seeds
	// themselves. Must be derived from the CONJUNCTS' own free variables,
	// not the block variables': a block can bind only non-bv variables
	// (e.g. `ex b ((b = 0) -> (x:bv[4] * y:bv[4] = 0:bv[4]))`) while its
	// body still carries bv content that must be seeded. Gating on
	// block_vars instead let such content fall through to `eliminable`
	// and reach generic Boole decomposition -- exactly the blow-up this
	// task exists to prevent.
	bool any_bv_content = false;
	for (tref conj : conjuncts) {
		for (tref v : get_free_vars<node>(conj))
			if (is_tref_bv_type_family<node>(v)) {
				any_bv_content = true; break;
			}
		if (any_bv_content) break;
	}
	subtree_unordered_set<node> arith_tainted;
	if (any_bv_content)
		for (tref conj : conjuncts)
			for (tref t : collect_bv_arithmetic_taint_uf<node>(conj))
				arith_tainted.insert(t);

	union_find_with_sets<decltype(eliminability_comp<node>), node>
		uf(eliminability_comp<node>);
	subtree_unordered_map<node, elim_verdict> by_root;

	auto verdict_of_root = [&](tref n) {
		tref root = uf.find(n);
		if (auto it = by_root.find(root); it != by_root.end())
			return it->second;
		return elim_verdict::eliminable;
	};
	auto assign = [&](tref n, elim_verdict v) {
		tref root = uf.find(n);
		by_root.insert_or_assign(root, join(verdict_of_root(root), v));
	};
	// Read both verdicts *before* merging: the merge picks a new root and
	// whichever old root loses would otherwise take its verdict with it.
	auto merge = [&](tref a, tref b) {
		elim_verdict va = verdict_of_root(a), vb = verdict_of_root(b);
		tref root = uf.merge(a, b);
		by_root.insert_or_assign(root, join(va, vb));
	};

	for (tref v : block_vars) uf.insert(v);

	// A wff wrapping an unresolved predicate reference. wff_ref is
	// deliberately excluded from is_atomic_fm (tau_tree_queries.tmpl.h:155),
	// so it needs its own branch.
	auto is_ref_fm = [](tref n) {
		const auto& t = tau::get(n);
		return t.is(tau::wff) && t.child_is(tau::wff_ref);
	};

	// Boolean connectives whose own shape carries no eliminability
	// information: their operands are wff nodes in their own right and get
	// visited (and classified) individually as the traversal descends into
	// them, so it is safe to fall through here without seeding anything.
	auto is_transparent_connective = [](tref n) {
		const auto& t = tau::get(n);
		if (!t.is(tau::wff)) return false;
		return t.child_is(tau::wff_and)   || t.child_is(tau::wff_or)
			|| t.child_is(tau::wff_xor)   || t.child_is(tau::wff_neg)
			|| t.child_is(tau::wff_imply) || t.child_is(tau::wff_rimply)
			|| t.child_is(tau::wff_equiv) || t.child_is(tau::wff_conditional)
			|| t.child_is(tau::wff_sometimes)
			|| t.child_is(tau::wff_always)
			|| t.child_is(tau::wff_parenthesis);
	};

	// Nodes seeded by visit's three branches, collected during traversal
	// and read off AFTER the conjunct loop finishes -- verdicts of merged
	// union-find components settle only once every conjunct has been
	// visited, so recording inside visit itself could read a verdict that
	// a later merge still changes.
	trefs analysed;

	for (tref conj : conjuncts) {
		// The conjunct itself joins its variables' component.
		// get_cnf_wff_clauses returns the leaves of the wff_and spine, and a
		// leaf is often not an atom (a negated equation is
		// wff(wff_neg(wff(bf_eq ...)))). Without this the traversal seeds
		// only the inner atom, the leaf node never enters the union-find,
		// and conjuncts_of() drops it -- which would let a caller re-wrap a
		// binder around a part not containing its own variable.
		//
		// Deliberately conservative: this unions ALL of a conjunct's
		// variables transitively (e.g. a disjunctive leaf unions its
		// disjuncts' variables together), which is coarser than atom-level
		// sharing. Over-freezing is sound; under-freezing is not.
		uf.insert(conj);
		for (tref v : get_free_vars<node>(conj)) merge(conj, v);

		auto visit = [&](tref m) -> bool {
			if (is_ref_fm(m) || is_child_quantifier<node>(m)) {
				// Two distinct reasons, one verdict. A reference
				// is unresolved, so anything reaching it must be
				// left alone. A kept binder means the equations
				// below it are not top-level conjuncts of this
				// clause, and squeezing them would drop the
				// binder and leak its variable free -- exactly
				// what the deleted `blocks_elimination` guard
				// prevented, now scoped to the component that
				// actually touches it rather than the whole
				// clause.
				if (is_ref_fm(m)) res.has_ref = true;
				assign(m, elim_verdict::frozen);
				for (tref v : get_free_vars<node>(m))
					merge(m, v);
				analysed.push_back(m);
				return false;
			}
			if (is_atomic_fm<node>(m)) {
				// Propagation channel: the atom's own verdict is
				// now seeded below (bv content only; otherwise
				// still the eliminable no-op), but its main job
				// remains inserting m into the union-find via
				// find() so the merge below has a root to work
				// from. Unioning m with its free variables makes
				// every variable sharing an atom share a root.
				// Order-independent -- the second merge joins the
				// sets and `join` lifts the result.
				//
				// Seed order matters only through `join`, which
				// is commutative -- but the *criterion* does not
				// commute with itself. `blasteable` keys on
				// bitvector TYPE, deliberately not on arithmetic
				// taint: blasting rewrites arithmetic into
				// per-bit atoms that are still bv-typed but no
				// longer tainted, and if those fell back to
				// `eliminable` they would reach generic Boole
				// decomposition -- hundreds of atoms per blasted
				// operation, each split copying the formula,
				// every BDD leaf allocating a cvc5 term. That is
				// a blow-up, not a wrong answer, so it must be
				// pinned by timing as well as by assertion.
				// An atom is a `wff` (a formula), not a term -- its
				// own `get_ba_type()` is the untyped/default id, so
				// `is_tref_bv_type_family<node>(m)` is silently
				// false no matter what it compares. bv-ness has to
				// be read off the atom's VARIABLES instead, the same
				// way collect_bv_arithmetic_taint_uf does it
				// (normalizer_uf_arithmetic.tmpl.h:98-99).
				const trefs& fvs = get_free_vars<node>(m);
				bool atom_is_bv = false;
				for (tref fv : fvs)
					if (is_tref_bv_type_family<node>(fv)) {
						atom_is_bv = true; break;
					}
				elim_verdict seed = elim_verdict::eliminable;
				if (atom_is_bv && ctx.bv_is_solver_owned)
					seed = elim_verdict::blasteable;
				if (arith_tainted.contains(m))
					seed = join(seed, elim_verdict::arithmetic);
				assign(m, seed);
				for (tref v : fvs) merge(m, v);
				analysed.push_back(m);
				return false;
			}
			if (tau::get(m).is(tau::wff) && !is_transparent_connective(m)) {
				// A wff-level shape none of the above recognise --
				// e.g. wff_t/wff_f, a bf_interval leaf (deliberately
				// excluded from is_atomic_fm,
				// tau_tree_queries.tmpl.h:155), or a constraint.
				// Fail closed: this analysis has no rule for it, so
				// freeze rather than silently leaving its variables
				// `eliminable`.
				assign(m, elim_verdict::frozen);
				for (tref v : get_free_vars<node>(m))
					merge(m, v);
				analysed.push_back(m);
				return false;
			}
			return true;
		};
		idni::pre_order<node>(conj).visit_unique(visit);
	}

	// Read every analysed node (vars AND atoms) into the flat maps. Verdicts
	// of merged union-find components settle only once every conjunct has
	// been visited, so this runs after the conjunct loop above, not inside
	// `visit`.
	auto record = [&](tref n) {
		const elim_verdict v = verdict_of_root(n);
		res.verdicts.insert_or_assign(n, v);
		res.members[v].insert(n);
	};
	for (tref v : block_vars) record(v);
	for (tref conj : conjuncts) record(conj);
	for (tref m : analysed) record(m);

	// Read components off, keyed by block variable.
	for (tref v : block_vars) {
		trefs comp;
		for (tref conj : conjuncts)
			if (uf.connected(v, conj))
				comp.push_back(conj);
		res.components.insert_or_assign(v, std::move(comp));
	}
	DBG(LOG_TRACE << "analyse_block: " << block_vars.size() << " var(s), "
		<< conjuncts.size() << " conjunct(s), has_ref=" << res.has_ref
		<< "\n";)
	return res;
}

} // namespace idni::tau_lang
