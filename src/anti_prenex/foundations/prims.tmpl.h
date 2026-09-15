// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file prims.tmpl.h
 * @brief Template implementations for prims.h (package D). Included by prims.h.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__PRIMS_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__PRIMS_TMPL_H__


namespace idni::tau_lang::anti_prenexing {

// Reuse: `canonize_quantifier_ids` is this primitive, extended in place
// (ruling, Sep 9 2026) -- a second numbering would break every memo key.
template <NodeType node>
tref canonicalise_binder_ids(tref phi) {
	return canonize_quantifier_ids<node>(phi);
}

// New: no existing helper wraps an ordered list without renaming -- the
// `_many` builders always rename, which ground rule 4 forbids here.
template <NodeType node>
tref rewrap(tref phi, const block& X, quantifier<node> kind) {
	using tau = tree<node>;
	DBG(assert(phi != nullptr);)
#ifdef DEBUG
	// The contract, checked in Debug only: every element a variable, no
	// repeats (a repeat would build a vacuous inner binder and a
	// non-canonical D3 key). `X` is a block, so this stays small. NOT
	// checked: `X` ⊆ FV(phi) -- a graceful exit of invariant 3 may re-wrap
	// a variable the body has since lost, which is legal and is what
	// `fold_degenerate_binders` cleans up.
	for (size_t i = 0; i < X.size(); ++i) {
		assert(X[i] != nullptr && tau::get(X[i]).is(tau::variable));
		for (size_t j = i + 1; j < X.size(); ++j)
			assert(!tau::subtree_equals(X[i], X[j]));
	}
#endif // DEBUG
	// `X[0]` is the outermost binder, so attach from the back.
	for (auto it = X.rbegin(); it != X.rend(); ++it)
		phi = kind == tau_term_bdd<node>::ex
			? tau::build_wff_ex(*it, phi, false)
			: tau::build_wff_all(*it, phi, false);
	return phi;
}

// New, after the vacuous drop in normalizer.tmpl.h's
// `scope_out_independent_conjuncts`: that one is welded into a push pass this
// module must not depend on, and the hooks fold no binder. Post-order, so a
// cascade collapses in one go; `apply_unique` memoises per node (§10).
template <NodeType node>
tref fold_degenerate_binders(tref phi) {
	using tau = tree<node>;
	auto fold = [](tref n) -> tref {
		if (!is_child_quantifier<node>(n)
			&& !is_child<node>(n, tau::bf_fall)
			&& !is_child<node>(n, tau::bf_fex)) return n;
		tref var  = tau::trim2(n);
		tref body = tau::get(n)[0].second();
		// A constant scope needs no case of its own: it has no free
		// variables, so the test below already drops the binder. Dropping
		// it at all assumes a non-empty domain, as the normalizer does.
		// `var` comes from trim2 and still carries its right sibling,
		// while `get_free_vars` stores trimmed nodes; comparing the two is safe
		// because subtree_less ignores a node's right sibling.
		const trefs& vars = get_free_vars<node>(body);
		if (std::binary_search(vars.begin(), vars.end(), var,
			tau::subtree_less)) return n;
		return body;
	};
	return post_order<node>(phi).apply_unique(fold);
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__PRIMS_TMPL_H__
