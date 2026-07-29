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
void profile_block_atoms_rec(tref n, block_atom_profile<node>& p) {
	using tau = tree<node>;
	const tau& t = tau::get(n);
	if (t.child_is(tau::wff_and) || t.child_is(tau::wff_or)) {
		profile_block_atoms_rec<node>(t[0].first(), p);
		profile_block_atoms_rec<node>(t[0].second(), p);
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
	const std::function<bool(tref)>& skip)
{
	using tau = tree<node>;
	block_atom_profile<node> p;
	p.skip_content = tau::get(formula).find_top(skip) != nullptr;
	detail::profile_block_atoms_rec<node>(formula, p);
	return p;
}

} // namespace idni::tau_lang
