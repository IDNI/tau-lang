// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

namespace idni::tau_lang {

template<NodeType node>
tref distribute_block_over_atoms(tref formula, const trefs& block) {
	using tau = tree<node>;
	const tau& t = tau::get(formula);
	if (t.child_is(tau::wff_and))
		return tau::build_wff_and(
			distribute_block_over_atoms<node>(t[0].first(), block),
			distribute_block_over_atoms<node>(t[0].second(), block));
	if (t.child_is(tau::wff_or))
		return tau::build_wff_or(
			distribute_block_over_atoms<node>(t[0].first(), block),
			distribute_block_over_atoms<node>(t[0].second(), block));
	if (t.child_is(tau::wff_t) || t.child_is(tau::wff_f)) return formula;
	// An atom. Only give it a binder if a block variable actually occurs
	// in it -- a vacuous binder is sound but pure noise downstream.
	const trefs& vars = get_free_vars<node>(formula);
	bool dependent = false;
	for (tref v : block)
		if (hasbc(vars, v, tau::subtree_less)) { dependent = true; break; }
	if (!dependent) return formula;
	for (auto v = block.rbegin(); v != block.rend(); ++v)
		formula = build_wff_ex<node>(*v, formula, false);
	return formula;
}

} // namespace idni::tau_lang
