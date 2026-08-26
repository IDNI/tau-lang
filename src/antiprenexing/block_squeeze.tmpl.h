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

template<NodeType node>
std::optional<trefs> squeeze_positive_disjuncts(tref formula, size_t ba_type) {
	using tau = tree<node>;
	const tau& t = tau::get(formula);
	if (t.child_is(tau::wff_or)) {
		auto l = squeeze_positive_disjuncts<node>(t[0].first(), ba_type);
		if (!l) return std::nullopt;
		auto r = squeeze_positive_disjuncts<node>(t[0].second(), ba_type);
		if (!r) return std::nullopt;
		if (block_squeeze_cap
			&& l->size() + r->size() > block_squeeze_cap)
			return std::nullopt;
		l->insert(l->end(), r->begin(), r->end());
		return l;
	}
	if (t.child_is(tau::wff_and)) {
		auto l = squeeze_positive_disjuncts<node>(t[0].first(), ba_type);
		if (!l) return std::nullopt;
		auto r = squeeze_positive_disjuncts<node>(t[0].second(), ba_type);
		if (!r) return std::nullopt;
		if (block_squeeze_cap
			&& l->size() * r->size() > block_squeeze_cap)
			return std::nullopt;
		trefs out;
		out.reserve(l->size() * r->size());
		for (tref a : *l) for (tref b : *r)
			out.push_back(tau::build_bf_or(a, b));
		return out;
	}
	// T is `0 = 0`; F is the empty disjunction.
	if (t.child_is(tau::wff_t)) return trefs{ tau::_0(ba_type) };
	if (t.child_is(tau::wff_f)) return trefs{};
	if (t.child_is(tau::bf_eq))
		return trefs{ tau::trim2(norm_equation<node>(formula)) };
	return std::nullopt;
}

} // namespace idni::tau_lang
