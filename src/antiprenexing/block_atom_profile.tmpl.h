// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

namespace idni::tau_lang {

namespace detail {

/**
 * @internal
 * @brief Recursive worker for `profile_block_atoms`.
 * @tparam node Tree node type.
 * @param n Sub-formula to census.
 * @param p Accumulator, updated in place.
 * @endinternal
 */
template<NodeType node>
void profile_block_atoms_rec(tref n, block_atom_profile<node>& p,
	bool guards_only = false)
{
	using tau = tree<node>;
	// Neither guard can fire once a non-fast-path atom was seen, or once
	// both signs are present. Exact for the guards; the counts are then
	// lower bounds, which is why this is opt-in.
	if (guards_only && (p.others > 0
		|| (p.positives > 0 && p.negatives > 0))) return;
	const tau& t = tau::get(n);
	if (t.child_is(tau::wff_and) || t.child_is(tau::wff_or)) {
		profile_block_atoms_rec<node>(t[0].first(), p, guards_only);
		profile_block_atoms_rec<node>(t[0].second(), p, guards_only);
		return;
	}
	// Constants are not atoms.
	if (t.child_is(tau::wff_t) || t.child_is(tau::wff_f)) return;
	if (t.child_is(tau::bf_eq)) { ++p.positives; return; }
	if (t.child_is(tau::wff_neg) && t[0][0].child_is(tau::bf_eq)) {
		++p.negatives;
		return;
	}
	// bf_lt, bf_lteq, wff_ref, a nested quantifier, a negation of any of
	// those: neither fast path can handle it, and it is not descended into.
	++p.others;
}

} // namespace detail

template<NodeType node>
block_atom_profile<node> profile_block_atoms(tref formula,
	const eliminability<node>& el, bool guards_only)
{
	using tau = tree<node>;
	block_atom_profile<node> p;
	// `el.has_skip_content`, not `find_top(el.skip)`: the latter reports the
	// bv-typed TERMS inside an atom the analysis explicitly classified
	// eliminable, which would keep every pure-BA bv matrix out of both fast
	// paths. See `eliminability::has_skip_content`.
	p.skip_content = el.has_skip_content(formula);
	detail::profile_block_atoms_rec<node>(formula, p, guards_only);
	// Costs an extra traversal, so it is only paid when the sign census leaves
	// step 2a's guard otherwise satisfied -- `all_positive()` does not need it
	// (squeezing `f1 = 0 && f2 = 0` into `f1|f2 = 0` and distributing `ex` over
	// a disjunction are valid in any Boolean algebra).
	if (!p.skip_content && p.others == 0 && p.positives == 0 && p.negatives > 0)
		p.finite_ba_content = tau::get(formula).find_top(
			[](tref n) {
				const size_t t = tau::get(n).get_ba_type();
				return t > 0 && (pack_type_has_arith_ops<node>(t)
					|| is_bool_type<node>(t));
			}) != nullptr;
	return p;
}

} // namespace idni::tau_lang
