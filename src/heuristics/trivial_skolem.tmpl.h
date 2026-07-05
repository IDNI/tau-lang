// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "trivial_skolem"

// Overview
// --------
// This heuristic eliminates existentially quantified variables by finding,
// anywhere in the formula, an atomic equation that *syntactically isolates*
// the variable using only invertible operators, and substituting `T` for
// that equation in place.
//
// High-level flow:
//  1) `trivial_skolem_ex(vars, phi)` collects every atomic formula in `phi`
//     via `select_all`, stopping descent at negations, wff-level xor,
//     implication-style connectives, and (temporal or plain) quantifiers: an
//     atom under any of those is not in a monotone position (or, for
//     quantifiers, out of scope for this pass), so substituting `T` there
//     would not preserve equivalence. Deliberately, descent does *not* stop at
//     `wff_or`: replacing a defining atom by `T` under disjunction is still
//     equivalence-preserving as long as the atom is not behind one of the
//     non-monotone boundaries above. The implication-style nodes usually
//     desugar into and/or/neg, but the explicit boundary check below keeps the
//     pass safe on raw or partially normalized inputs too.
//  2) For each existential variable `v` in `vars`, it first counts every
//     syntactic occurrence of `v` anywhere in `phi` (expanding shared
//     subtrees, not deduplicating them) and requires exactly one: `v`
//     could otherwise occur once in an eligible atom and again behind a
//     boundary (or via a subtree shared by a desugared connective above),
//     and eliminating it would leave that other occurrence dangling free.
//     Only then does it look up which eligible atom contains that
//     occurrence.
//  3) In that unique atom, `trivial_skolem_invertible_clause` checks whether
//     it is an equality `lhs = rhs` where exactly one side contains `v`.
//  4) `trivial_skolem_invert_term` then recursively inverts operators on the
//     side containing `v` until the base form `v = witness` is obtained.
//  5) The witness is accepted only if it does not mention any variable from
//     `vars` (so it is independent from eliminated existentials in this pass).
//  6) Every accepted atom is replaced by `T` in one pass and the formula is
//     rebuilt (`tree<node>::reget`) so the normal simplification hooks fold
//     away the resulting `T && x`/`T || x` redundancies; `v` is removed from
//     the existential binder list. Because a substitution is sound wherever
//     the atom sits (AND, OR, or otherwise) as long as it isn't negated,
//     this works even when the defining equation is nested inside a
//     disjunction, not just a top-level conjunction.
//
// Inversion details (`trivial_skolem_invert_term`):
//  - Base case: if current side is exactly `var`, the accumulated `other`
//    expression is the witness.
//  - `neg(x) = y`      -> `x = neg(y)`
//  - `x xor c = y`     -> `x = y xor c`   (and symmetrically if `var` is on rhs)
//  - `x + c = y`       -> `x = y - c`
//  - `x - c = y`       -> `x = y + c`
//  - `c - x = y`       -> `x = c - y`
//  If both children contain `var` or neither does, inversion fails for that
//  operator, because isolation is not unique in this simple syntactic scheme.
//
// Why this is written for existential quantifiers (`∃`) and not universally
// quantified elimination (`∀`) in the same way:
//  - For `∃x`, replacing/removing `x` via a witness term is local to
//    satisfiability/definability and does not require deciding an ordering of
//    external free variables in this pass.
//  - For `∀x`, an analogous transformation would require knowing the quantifier
//    status/order of currently free variables (are they truly free, or intended
//    to become `∀`/`∃`, and in what prefix position?). That quantifier context
//    changes semantics, so a local clause-level rewrite is not sound in
//    general without that global information.

namespace idni::tau_lang {

// Attempts to invert `side = other` for `var`: recursively peels invertible
// term operators off `side` until `var` is isolated, accumulating the
// witness expression in `result`. Returns false if `var` can't be isolated
// this way.
template <NodeType node>
bool trivial_skolem_invert_term(tref side, tref other, tref var, tref& result) {
	using tau = tree<node>;
	// side/other/result are "bf"-wrapped terms; var is a raw variable node,
	// so it must be compared against side's unwrapped content.
	if (tau::get(side)[0].get() == var) { result = other; return true; }
	const tau& t = tau::get(side)[0];
	if (t.is(tau::bf_neg)) {
		tref inner = t.first();
		return trivial_skolem_invert_term<node>(inner,
			build_bf_neg<node>(other), var, result);
	}
	if (t.is(tau::bf_xor)) {
		tref l = t.first(), r = t.second();
		bool l_has = contains<node>(l, var), r_has = contains<node>(r, var);
		if (l_has == r_has) return false;
		return l_has
			? trivial_skolem_invert_term<node>(l, build_bf_xor<node>(other, r), var, result)
			: trivial_skolem_invert_term<node>(r, build_bf_xor<node>(other, l), var, result);
	}
	if (t.is(tau::bf_add)) {
		tref l = t.first(), r = t.second();
		bool l_has = contains<node>(l, var), r_has = contains<node>(r, var);
		if (l_has == r_has) return false;
		return l_has
			? trivial_skolem_invert_term<node>(l, build_bf_sub<node>(other, r), var, result)
			: trivial_skolem_invert_term<node>(r, build_bf_sub<node>(other, l), var, result);
	}
	if (t.is(tau::bf_sub)) {
		tref l = t.first(), r = t.second();
		bool l_has = contains<node>(l, var), r_has = contains<node>(r, var);
		if (l_has == r_has) return false;
		return l_has
			? trivial_skolem_invert_term<node>(l, build_bf_add<node>(other, r), var, result)
			: trivial_skolem_invert_term<node>(r, build_bf_sub<node>(l, other), var, result);
	}
	return false;
}

// Checks whether `clause` (a wff-wrapped formula) is a bf_eq that isolates
// `var` via invertible operators, with a witness free of every variable in
// `vars`. On success, `witness` holds the witness expression.
template <NodeType node>
bool trivial_skolem_invertible_clause(tref clause, tref var,
	const trefs& vars, tref& witness)
{
	using tau = tree<node>;
	const tau& c0 = tau::get(clause)[0];
	if (!c0.is(tau::bf_eq)) return false;
	tref l = c0.first(), r = c0.second();
	bool l_has = contains<node>(l, var), r_has = contains<node>(r, var);
	if (l_has == r_has) return false;
	tref candidate;
	bool ok = l_has
		? trivial_skolem_invert_term<node>(l, r, var, candidate)
		: trivial_skolem_invert_term<node>(r, l, var, candidate);
	if (!ok) return false;
	for (tref v : vars) if (contains<node>(candidate, v)) return false;
	witness = candidate;
	return true;
}

// Atomic formulas under negation, wff-level xor, or implication-style
// connectives are not in a monotone position (see file overview). We also stop
// at nested (possibly temporal) quantifiers because those atoms are out of
// scope for this pass. We intentionally do *not* stop at `wff_or`: once a
// defining atom is proven equivalent to `T`, replacing it by `T` remains sound
// under disjunction, and doing so lets the simplifier remove the now-trivial
// branch.
template <NodeType node>
bool trivial_skolem_atom_boundary(tref n) {
	using tau = tree<node>;
	return is<node>(n, tau::wff_neg)
		|| is<node>(n, tau::wff_xor)
		|| is<node>(n, tau::wff_imply)
		|| is<node>(n, tau::wff_rimply)
		|| is<node>(n, tau::wff_equiv)
		|| is<node>(n, tau::wff_conditional)
		|| is_quantifier<node>(n)
		|| is_functional_quantifier<node>(n)
		|| is_temporal_quantifier<node>(n);
}

// Counts every syntactic occurrence of `var` in `fm`, expanding shared
// subtrees rather than deduplicating them: `var` could be reachable through
// more than one path (e.g. an operand shared by a desugared `wff_equiv`,
// which references it once negated and once positively), and a variable
// that occurs more than once anywhere in `fm` must never be eliminated,
// even if exactly one of its occurrences sits inside an eligible atom.
template <NodeType node>
size_t trivial_skolem_count_occurrences(tref fm, tref var) {
	using cache_t = std::unordered_map<tref, size_t>;
	cache_t memo;
	std::function<size_t(tref)> count = [&](tref n) -> size_t {
		if (auto it = memo.find(n); it != memo.end()) return it->second;
		size_t total = (n == var) ? 1 : 0;
		for (tref child : tree<node>::get(n).children()) total += count(child);
		memo.emplace(n, total);
		return total;
	};
	return count(fm);
}

template <NodeType node>
tref trivial_skolem_ex(const trefs& vars, tref phi) {
	using tau = tree<node>;
	trefs atoms = rewriter::select_all_until<node>(phi, is_atomic_fm<node>,
		trivial_skolem_atom_boundary<node>);
	trefs kept_vars;
	subtree_map<node, tref> changes;
	for (tref v : vars) {
		// A variable occurring more than once anywhere in phi (whether in
		// another eligible atom or hidden behind a boundary) can't be
		// eliminated: dropping its quantifier would leave the other
		// occurrence(s) dangling free.
		if (trivial_skolem_count_occurrences<node>(phi, v) != 1) {
			kept_vars.push_back(v);
			continue;
		}
		size_t occurrence = atoms.size();
		for (size_t i = 0; i < atoms.size(); ++i)
			if (contains<node>(atoms[i], v)) { occurrence = i; break; }
		tref witness;
		if (occurrence != atoms.size()
				&& trivial_skolem_invertible_clause<node>(
					atoms[occurrence], v, vars, witness))
			changes.emplace(atoms[occurrence], tau::_T());
		else kept_vars.push_back(v);
	}
	tref matrix = changes.empty()
		? phi : tau::reget(rewriter::replace(phi, changes));
	if (kept_vars.empty()) return matrix;
	return tau::build_wff_ex_many(kept_vars, matrix);
}

} // namespace idni::tau_lang
