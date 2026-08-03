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
	(void) ctx; // bitvector seeds arrive in Task 3
	block_eliminability<node> res;
	if (block_vars.empty()) return res;

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

	for (tref conj : conjuncts) {
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
				return false;
			}
			if (is_atomic_fm<node>(m)) {
				// Propagation channel: the atom contributes no
				// verdict of its own, but unioning it with its
				// free variables makes every variable sharing an
				// atom share a root. Order-independent -- the
				// second merge joins the sets and `join` lifts
				// the result.
				assign(m, elim_verdict::eliminable);
				for (tref v : get_free_vars<node>(m))
					merge(m, v);
				return false;
			}
			return true;
		};
		idni::pre_order<node>(conj).visit_unique(visit);
	}

	// Read verdicts and components off, keyed by block variable.
	for (tref v : block_vars) {
		res.verdicts.insert_or_assign(v, verdict_of_root(v));
		trefs comp;
		for (tref conj : conjuncts)
			if (uf.contains(conj) && uf.connected(v, conj))
				comp.push_back(conj);
		res.components.insert_or_assign(v, std::move(comp));
	}
	DBG(LOG_TRACE << "analyse_block: " << block_vars.size() << " var(s), "
		<< conjuncts.size() << " conjunct(s), has_ref=" << res.has_ref
		<< "\n";)
	return res;
}

} // namespace idni::tau_lang
