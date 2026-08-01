// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

namespace idni::tau_lang {

template<NodeType node>
boole_atom_analysis<node> analyze_boole_atom(tref atm, tref var) {
	using tau = tree<node>;
	boole_atom_analysis<node> r;
	if (!tau::get(atm)[0].is(tau::bf_eq)) return r;

	// Same shape as legacy ex_quantified_boole_decomposition -- the variable
	// is wrapped in a bf node -- but with while_is_boolean_operation rather
	// than the legacy is_boolean_operation, which does not accept the
	// tau::bf substitution root and therefore makes the whole replace_if a
	// no-op (see the note on the declaration).
	//
	// trim_right_sibling first: the variable taken off a quantifier node
	// still carries the scope as its right sibling. tau::get(tau::bf, ...)
	// re-parents it and so would drop the sibling anyway, but trimming here
	// keeps the intent explicit and independent of get_raw's behaviour.
	const tref bf_var = tau::get(tau::bf, tau::trim_right_sibling(var));
	const size_t vt = find_ba_type<node>(bf_var);
	tref func = tau::trim2(norm_equation<node>(atm));
	func = apply_xor_def<node>(func);
	tref f0 = syntactic_path_simplification<node>(
		rewriter::replace_if<node>(func, bf_var, tau::_0(vt),
			while_is_boolean_operation<node>));
	tref f1 = syntactic_path_simplification<node>(
		rewriter::replace_if<node>(func, bf_var, tau::_1(vt),
			while_is_boolean_operation<node>));
	r.cofactor_0 = f0;
	r.cofactor_1 = f1;

	// γ₂: f is identically 0, so `f = 0` is T whatever x is.
	if (tau::get(f0).equals_0() && tau::get(f1).equals_0()) {
		r.kind = boole_atom_case::identically_zero;
		return r;
	}
	// γ₃: f is identically 1, so `f = 0` is F whatever x is.
	if (tau::get(f0).equals_1() && tau::get(f1).equals_1()) {
		r.kind = boole_atom_case::identically_one;
		return r;
	}
	// γ₄: both cofactors agree and x is gone, so f does not depend on x.
	if (tau::get(f0) == tau::get(f1) && !contains<node>(f0, bf_var)) {
		r.kind = boole_atom_case::independent;
		return r;
	}
	// γ₁: f[x<-0] == f[x<-1]' means f has exactly one zero, at
	// x := f[x<-0].
	tref not_f1 = push_negation_in<node, false>(tau::build_bf_neg(f1));
	if (tau::get(f0) == tau::get(not_f1) && !contains<node>(f0, bf_var)) {
		r.kind = boole_atom_case::unique_zero;
		return r;
	}
	return r;
}

} // namespace idni::tau_lang
