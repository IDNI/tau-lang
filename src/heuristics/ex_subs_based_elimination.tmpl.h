// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ex_subs_based_elimination"

namespace idni::tau_lang {

template <NodeType node>
tref preorder(tref var, tref ex_clause) {
	using tau = tree<node>;
	tref found = nullptr;

	auto visit = [&](tref n) {
		if (is<node>(n, tau::bf_eq)) {
			const tau& t = tau::get(n);
			tref left = t[0][0].get();
			tref right = t[1][0].get();
			// Occurs-check: ex x (x = t && phi(x)) ≡ phi(t) only holds
			// when x does not occur in t; otherwise substituting t for x
			// and dropping the quantifier leaks the bound variable free
			if (tau::get(left) == tau::get(var)
					&& !contains<node>(right, var))
				found = right;
			else if (tau::get(right) == tau::get(var)
					&& !contains<node>(left, var))
				found = left;
		}
	};

	// We visit the formula until reaching atomic formulas (eq)
	auto visit_subtree = [&](tref n) -> bool {
		if (found) return false;
		return is<node>(n, tau::wff) || is<node>(n, tau::wff_and) || is<node>(n, tau::bf_eq);
	};

	auto up = [&](tref) -> void { return; };

	pre_order<node>(ex_clause).visit_unique(visit, visit_subtree, up);
	return found;
}

template <NodeType node>
tref ex_subs_based_elimination(tref var, tref ex_clause)
{
	using tau = tree<node>;
	if (auto res = preorder<node>(var, ex_clause); res) {
		// Capture-check: if the substituted term contains a variable that
		// is re-bound by a quantifier inside the clause, substituting under
		// that binder would capture it. Reject the substitution entirely:
		// skipping such scopes instead is not an option because the caller
		// drops the quantifier on var, which would leave the occurrences of
		// var inside the skipped scope free
		auto res_vars = tau::get(res).select_all(is_var_or_capture<node>());
		subtree_set<node> term_vars(res_vars.begin(), res_vars.end());
		auto binds_term_var = [&term_vars](tref n) -> bool {
			return is_logical_or_functional_quant<node>(n)
				&& term_vars.contains(tau::get(n).child(0));
		};
		if (tau::get(ex_clause).find_top(binds_term_var))
			return ex_clause;
		// Scope-aware replacement: skip quantifiers that bind var to avoid
		// replacing their bound variable (variable capture prevention)
		auto query = [&var](tref n) -> bool {
			if (is<node>(n, tau::wff_all) || is<node>(n, tau::wff_ex)) {
				tref bound = tau::get(n).child(0);
				if (bound == var) return false;
			}
			return true;
		};
		auto replaced = rewriter::replace_if<node>(ex_clause, var, res, query);
		// replace_if rebuilds nodes without invoking the construction
		// hooks, so trivially foldable subformulas (constant equations,
		// T/F connectives...) would survive; rebuild with hooks.
		return tree<node>::reget(replaced);
	}
	else return ex_clause;
}

} // namespace idni::tau_lang