// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ex_subs_based_elimination"

namespace idni::tau_lang {

/**
 * @internal
 * @brief Searches @p ex_clause for a substitution witness for @p var.
 *
 * Pre-order-visits @p ex_clause looking for a `bf_eq` node with @p var on
 * one side and a term not containing @p var (occurs-check) on the other,
 * returning that other term as the witness. Descent stops at `wff_or`
 * (only a single conjunctive obligation is considered) and at the first
 * `bf_eq` found along a branch.
 * @tparam node Tree node type.
 * @param var The variable to find a substitution witness for.
 * @param ex_clause The clause to search.
 * @return The witness term, or `nullptr` if none is found.
 *
 * @par Example
 * @code{.cpp}
 * // "x = a && y = b": the "x = a" equality offers "a" as a witness for x.
 * tref var = build_variable<node_t>("x", tau_type_id<node_t>());
 * tref ex_clause = get_nso_rr("x = a && y = b.").value().main->get();
 * tref witness = preorder<node_t>(var, ex_clause);
 * // tau::get(witness).to_str() == "a"
 * @endcode
 * @endinternal
 */
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
		// Deliberately stop at wff_or: this pass only eliminates from a single
		// conjunctive obligation x = t inside ex_clause. Substituting through an
		// arbitrary disjunction would require branch-sensitive reasoning about
		// which equality actually defines the witness.
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

#ifdef TAU_CACHE
	using cache_t = std::unordered_map<std::pair<tref, tref>, tref>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	// Neither parameter is reassigned below, but both can arrive carrying a
	// live right sibling: the 1-arg overload's tau::trim2(n) hands in a var
	// whose right sibling is the scope it was split from (trim2 returns
	// child(0), still linked to child(1) via first()'s raw left-child
	// pointer), and ex_clause is not guaranteed sibling-free at every call
	// site either. Neither the occurs-/capture-check logic below nor tau's
	// own structural equality (lcrs_tree::operator==, which compares value
	// and left-child only, never right_sibling) look past the operand's own
	// content, so trimming is safe -- and it is what lets leaf_clause.tmpl.h's
	// witness loop, which re-asks for the same logical (var, ex_clause) pair
	// across anti_prenex_block's branch recursion, actually land on the same
	// entry instead of missing on an incidental right-sibling difference.
	const std::pair<tref, tref> key { tau::trim_right_sibling(var),
		tau::trim_right_sibling(ex_clause) };
	if (auto it = cache.find(key); it != cache.end()) return it->second;
	// Every return, including both identity returns (raw ex_clause) and the
	// reget'd substitution result, must be trimmed before being stored: a
	// cache hit serves back whatever was stored under this trimmed key to
	// ANY call that hashes to it, not just the call that produced it. The
	// reget path is not exempt just because it looks freshly built --
	// replace_if's rebuild and reget's own `get(value, children,
	// right_sibling())` both carry the *current* ex_clause's right sibling
	// through unchanged, so an untrimmed return there would just as silently
	// hand a later call the first call's sibling attachment. Trimming here
	// (rather than only at the two identity-return call sites) closes the
	// echo risk uniformly and is a no-op today, since both current callers
	// already pass a sibling-free ex_clause (leaf_clause.tmpl.h's freshly
	// built `scoped`; the 1-arg overload's `scope`, always the last child of
	// a 2-child wff_ex).
	auto memo = [&key](tref r) {
		return cache.emplace(key, tau::trim_right_sibling(r)).first->second;
	};
#else
	auto memo = [](tref r) { return r; };
#endif // TAU_CACHE

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
			return memo(ex_clause);
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
		return memo(tree<node>::reget(replaced));
	}
	else return memo(ex_clause);
}

template <NodeType node>
tref ex_subs_based_elimination(tref fm) {
	using tau = tree<node>;
	
	auto subs_elim = [](tref n) -> tref {
		if (!is_child<node>(n, tau::wff_ex)) return n;
		tref var = tau::trim2(n);
		tref scope = tau::get(n)[0].second();
		// No "scope contains a wff_or -> decline" guard here. It used to
		// bail out whenever a disjunction appeared *anywhere* in the scope,
		// which is far stronger than what soundness needs and is what made
		// `run` hang on specs built from nested conditionals: those compile
		// to a conjunction of disjunctions, so a scope like
		// `ex x (x = c && (p || q) && (r || s))` was left untouched even
		// though `x = c` is a plain top-level conjunct. The quantifier then
		// survived into the Boole-decomposition stage, which is exponential
		// in the number of atoms and has no total budget once the block
		// algorithm's own `block_boole_max_splits` is spent.
		//
		// `ex x (x = t && phi)` == `phi[x := t]` needs three things, all
		// checked where they belong and none of them a property of `phi`'s
		// connectives: the witness must come from a conjunctive obligation
		// (`preorder`'s visit_subtree descends only through wff/wff_and/bf_eq,
		// so it never takes one from under a wff_or or a wff_neg), `x` must
		// not occur in `t` (occurs-check in `preorder`), and `t` must not be
		// captured by a binder inside the scope (capture-check in the
		// two-argument overload, which declines the substitution outright).
		tref elim = ex_subs_based_elimination<node>(var, scope);
		return elim != scope ? elim : n;
	};
	return post_order<node>(fm).apply_unique(subs_elim);
}

} // namespace idni::tau_lang