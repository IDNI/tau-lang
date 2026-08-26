// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "eliminability"

namespace idni::tau_lang {

template <NodeType node>
bool eliminability_comp(tref l, tref r) {
	return tree<node>::subtree_less(l, r);
}

/** @copydoc has_foreign_arith_constant */
template <NodeType node>
bool has_foreign_arith_constant(tref form) {
	using tau = tree<node>;
	auto foreign = [](tref n) {
		const tau& t = tau::get(n);
		if (!t.is_ba_constant() && !t.is(tau::bf_t) && !t.is(tau::bf_f))
			return false;
		const size_t ty = t.get_ba_type();
		return ty != 0 && !pack_type_has_arith_ops<node>(ty);
	};
	return tau::get(form).find_top(foreign) != nullptr;
}

template <NodeType node>
bool eliminability<node>::covers_atom(tref n) const {
	// Only an ATOM may stop the walk. A conjunct, a connective or a whole
	// clause must not: `analyse_block` records those too, and a conjunct's
	// component does not necessarily contain every atom below it (an atom with
	// no free variable at all -- `{3}:bv[2] * {2}:bv[2] = {0}:bv[2]` -- merges
	// with nothing and keeps its own root, so an `eliminable` conjunct can sit
	// above `arithmetic` content). An atom has no atom below it, so the same
	// hole cannot open here.
	if (!is_atomic_fm<node>(n)) return false;
	bool any_arith = false;
	for (tref v : get_free_vars<node>(n)) {
		if (!pack_type_has_arith_ops<node>(
			tree<node>::get(v).get_ba_type())) continue;
		any_arith = true;
		auto it = verdicts.find(v);
		if (it == verdicts.end()
			|| it->second != elim_verdict::eliminable) return false;
	}
	// `any_arith` is required, not incidental: without an arith-typed free
	// variable there is nothing the analysis could have vouched for, and a
	// variable-free arith-typed atom would be covered vacuously. Such an atom
	// keeps flooring, which is the conservative answer.
	return any_arith;
}

template <NodeType node>
bool eliminability<node>::has_skip_content(tref f) const {
	bool found = false;
	// Named, not a temporary: visit_unique takes `auto&`. Deduplicating on
	// unique subtrees is safe here -- both predicates below are pure functions
	// of the node they are handed.
	auto visit = [&](tref n) -> bool {
		if (found) return false;
		if (skip(n)) { found = true; return false; }
		// Covered atom: its own terms are the only floored nodes below it,
		// and the analysis has already vouched for them.
		return !covers_atom(n);
	};
	idni::pre_order<node>(f).visit_unique(visit);
	return found;
}

template <NodeType node>
bool eliminability<node>::has_frozen(tref f) const {
	using tau = tree<node>;
	if (auto it = frozen_memo.find(f); it != frozen_memo.end())
		return it->second;
	// No covers_atom-style prune needed here -- see the doc comment: frozen
	// is only ever an explicit entry, never the floor, so every node this
	// walk could stop early on carries no information the plain hit test
	// would miss.
	const bool found = tau::get(f).find_top([this](tref n) {
		return verdict_of(n) == elim_verdict::frozen;
	}) != nullptr;
	return frozen_memo.emplace(f, found).first->second;
}

namespace detail {

/**
 * @internal
 * @brief Seed verdict of ONE atomic formula, from its bitvector content alone.
 *
 * The single place the arithmetic/blasteable/eliminable split is decided;
 * `analyse_formula` and `analyse_block` both go through it, so the two can
 * never drift apart:
 *   - `arithmetic` -- the atom holds an arithmetic operator `atomic_blasting`
 *     cannot express (`bv_predicate_blasting.tmpl.h`: mul needs a constant
 *     factor, shl/shr/div/mod a constant second argument);
 *   - `blasteable` -- the atom holds a (supported) arithmetic operator;
 *   - `eliminable` -- otherwise, INCLUDING an arith-typed atom (its type's
 *     owning BA declares `arith_ops`) holding no arithmetic at all.
 *
 * That last case is the 2026-08-14 user directive ("bv variables that appear
 * only in atoms that are purely BA are also eliminable" -- bv being, today,
 * the only in-tree algebra declaring `arith_ops`). A purely Boolean
 * arith-typed atom -- `|`, `&`, `'`, constants, nothing else -- is decided by
 * the very Boole-expansion laws the elimination core implements, which hold
 * in ANY Boolean algebra, atomic or not (`f = 0` is solvable for `x` iff
 * `f[x<-0] f[x<-1] = 0`; `f != 0` iff `f[x<-0] | f[x<-1] != 0`). Shipping it to
 * cvc5 is work Tau can do itself. The verdict is returned for an ANALYSED atom,
 * so the caller records it EXPLICITLY in `verdicts`/`members` and it beats
 * `eliminability::arith_floor`; unanalysed arith-typed content still floors
 * to `blasteable`, which is what keeps blasting's per-bit residue
 * (arith-typed, arithmetic-free, and re-entered under
 * `eliminability::arith_only()` -- an analysis with no explicit entries at
 * all) out of generic Boole decomposition.
 *
 * The laws that do NOT survive an atomic BA are the ones distributing a block
 * over several atoms; those carry their own guards, at
 * `block_atom_profile::all_negated` and in `eliminate_block_over_clause`'s two
 * squeezes.
 *
 * An atom is a `wff` (a formula), not a term -- its own `get_ba_type()` is the
 * untyped/default id, so `pack_type_has_arith_ops<node>(m.get_ba_type())` is
 * silently false no matter what it compares. Arith-ness is therefore read
 * off the atom's VARIABLES.
 * @tparam node Tree node type.
 * @param m Atomic formula (`is_atomic_fm` must hold).
 * @param arith_is_solver_owned `analysis_context::arith_is_solver_owned`. Unused
 *        since the directive above: the two verdicts it used to scope are now
 *        decided without it (arithmetic is classified before it was ever read;
 *        a pure-BA atom is eliminable under both settings). Kept in the
 *        signature because it remains the analysis's contract with its context
 *        -- `process_quantifier_block` still demotes `blasteable` by it -- and
 *        Task 9 threads the full `analysis_context` through here.
 * @endinternal
 */
template <NodeType node>
elim_verdict atom_arith_verdict(tref m, bool arith_is_solver_owned) {
	using tau = tree<node>;
	// Arithmetic operators. Not is<node>({...}): that factory's returned
	// closure captures a std::initializer_list by value, whose backing array
	// is a temporary destroyed at the end of the factory call's full
	// expression -- silently dangling afterward.
	auto is_arith_op = [](tref n) {
		const auto& t = tau::get(n);
		return t.is(tau::bf_add) || t.is(tau::bf_sub)
			|| t.is(tau::bf_mul) || t.is(tau::bf_div)
			|| t.is(tau::bf_mod) || t.is(tau::bf_shl)
			|| t.is(tau::bf_shr) || t.is(tau::bf_cast);
	};
	// Arithmetic operators atomic_blasting cannot express
	// (bv_predicate_blasting.tmpl.h:206-290): mul needs a constant
	// factor; shl/shr/div/mod need a constant second argument. Only these
	// five operators are ever constrained -- add/sub/cast blast
	// unconditionally, matching `pack_term_is_blasteable`'s own default.
	// The constant-argument test itself is routed through the owning BA's
	// classification rather than a bare bv-specific argument test, so this
	// holds for whichever BA's descriptor owns the term's type.
	auto blasting_unsupported = [](tref n) {
		const auto& t = tau::get(n);
		if (t.is(tau::bf_mul) || t.is(tau::bf_shl) || t.is(tau::bf_shr)
			|| t.is(tau::bf_div) || t.is(tau::bf_mod))
			return !pack_term_is_blasteable<node>(t.get_ba_type(), n);
		return false;
	};
	if (tau::get(m).find_top(is_arith_op))
		return tau::get(m).find_top(blasting_unsupported)
			? elim_verdict::arithmetic : elim_verdict::blasteable;
	// No arithmetic operator anywhere below `m`: purely Boolean-algebra
	// content whatever its type, so this pass can eliminate it itself. The
	// arith-typed-free-variable scan that used to answer `blasteable` here, and
	// the `arith_is_solver_owned` early return that scoped it, both went with the
	// directive documented above -- a pure-BA atom is eliminable under either
	// setting, so neither branch had an answer left to give.
	(void) arith_is_solver_owned;
	return elim_verdict::eliminable;
}

/**
 * @internal
 * @brief `atom_arith_verdict` for every atomic formula of one conjunct.
 *
 * `analyse_block`'s precompute: the conjunct is walked once and every atom it
 * holds is classified, so the per-atom seeding below is a map lookup rather
 * than a second, differently-written classification. Atoms are not descended
 * into (`visit_unique` returns false at them), matching the seeding traversal
 * that consumes the result.
 * @tparam node Tree node type.
 * @endinternal
 */
template <NodeType node>
subtree_unordered_map<node, elim_verdict> collect_arith_verdicts(tref conj,
	bool arith_is_solver_owned)
{
	subtree_unordered_map<node, elim_verdict> res;
	// Named, not a temporary: visit_unique takes `auto&`.
	auto visit = [&](tref m) -> bool {
		if (!is_atomic_fm<node>(m)) return true;
		if (elim_verdict v = atom_arith_verdict<node>(m,
			arith_is_solver_owned); v != elim_verdict::eliminable)
				res.emplace(m, v);
		return false;
	};
	idni::pre_order<node>(conj).visit_unique(visit);
	return res;
}

} // namespace detail

template <NodeType node>
block_eliminability<node> analyse_block(const trefs& block_vars,
	const trefs& conjuncts, const analysis_context<node>& ctx)
{
	using tau = tree<node>;
	block_eliminability<node> res;
	if (block_vars.empty()) return res;

	// Gates only the arith-verdict PRECOMPUTE below -- never the seeds
	// themselves. Must be derived from the CONJUNCTS' own free variables,
	// not the block variables': a block can bind only non-arith-typed
	// variables (e.g. `ex b ((b = 0) -> (x:bv[4] * y:bv[4] = 0:bv[4]))`)
	// while its body still carries arith-typed content that must be seeded.
	// Gating on block_vars instead let such content fall through to
	// `eliminable` and reach generic Boole decomposition -- exactly the
	// blow-up this task exists to prevent.
	bool any_arith_content = false;
	for (tref conj : conjuncts) {
		for (tref v : get_free_vars<node>(conj))
			if (pack_type_has_arith_ops<node>(tau::get(v).get_ba_type())) {
				any_arith_content = true; break;
			}
		if (any_arith_content) break;
	}
	// One classification of the arith-typed content, shared with
	// `analyse_formula` through `detail::atom_arith_verdict` -- see there for
	// the split. Only non-eliminable entries are stored, so a map miss below
	// means "eliminable", which is also what a conjunct-free block gets.
	subtree_unordered_map<node, elim_verdict> arith_verdicts;
	if (any_arith_content)
		for (tref conj : conjuncts)
			for (const auto& [m, v] :
				detail::collect_arith_verdicts<node>(conj,
					ctx.arith_is_solver_owned))
						arith_verdicts.emplace(m, v);

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

	// Boolean connectives whose own shape carries no eliminability
	// information: their operands are wff nodes in their own right and get
	// visited (and classified) individually as the traversal descends into
	// them, so it is safe to fall through here without seeding anything.
	auto is_transparent_connective = [](tref n) {
		const auto& t = tau::get(n);
		if (!t.is(tau::wff)) return false;
		return t.child_is(tau::wff_and)   || t.child_is(tau::wff_or)
			|| t.child_is(tau::wff_xor)   || t.child_is(tau::wff_neg)
			|| t.child_is(tau::wff_imply) || t.child_is(tau::wff_rimply)
			|| t.child_is(tau::wff_equiv) || t.child_is(tau::wff_conditional)
			|| t.child_is(tau::wff_sometimes)
			|| t.child_is(tau::wff_always)
			|| t.child_is(tau::wff_parenthesis);
	};

	// Nodes seeded by visit's three branches, collected during traversal
	// and read off AFTER the conjunct loop finishes -- verdicts of merged
	// union-find components settle only once every conjunct has been
	// visited, so recording inside visit itself could read a verdict that
	// a later merge still changes.
	trefs analysed;

	for (tref conj : conjuncts) {
		// The conjunct itself joins its variables' component.
		// get_cnf_wff_clauses returns the leaves of the wff_and spine, and a
		// leaf is often not an atom (a negated equation is
		// wff(wff_neg(wff(bf_eq ...)))). Without this the traversal seeds
		// only the inner atom, the leaf node never enters the union-find,
		// and conjuncts_of() drops it -- which would let a caller re-wrap a
		// binder around a part not containing its own variable.
		//
		// Deliberately conservative: this unions ALL of a conjunct's
		// variables transitively (e.g. a disjunctive leaf unions its
		// disjuncts' variables together), which is coarser than atom-level
		// sharing. Over-freezing is sound; under-freezing is not.
		uf.insert(conj);
		for (tref v : get_free_vars<node>(conj)) merge(conj, v);

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
				analysed.push_back(m);
				return false;
			}
			if (is_atomic_fm<node>(m)) {
				// Propagation channel: the atom's own verdict is
				// now seeded below (arith-typed content only; otherwise
				// still the eliminable no-op), but its main job
				// remains inserting m into the union-find via
				// find() so the merge below has a root to work
				// from. Unioning m with its free variables makes
				// every variable sharing an atom share a root.
				// Order-independent -- the second merge joins the
				// sets and `join` lifts the result.
				//
				// Seed order matters only through `join`, which
				// is commutative. `blasteable` keys on a type whose
				// owning BA declares `arith_ops`, deliberately not on
				// arithmetic taint alone: blasting rewrites arithmetic
				// into per-bit atoms that are still arith-typed but no
				// longer tainted, and if those fell back to
				// `eliminable` they would reach generic Boole
				// decomposition -- hundreds of atoms per blasted
				// operation, each split copying the formula,
				// every BDD leaf allocating a cvc5 term. That is
				// a blow-up, not a wrong answer, so it must be
				// pinned by timing as well as by assertion.
				// The verdict itself comes from the precomputed
				// map above (a miss means `eliminable`), so this
				// analysis and `analyse_formula` classify arith-typed
				// content one way, in one place.
				const trefs& fvs = get_free_vars<node>(m);
				elim_verdict seed = elim_verdict::eliminable;
				if (auto it = arith_verdicts.find(m);
					it != arith_verdicts.end()) seed = it->second;
				assign(m, seed);
				for (tref v : fvs) merge(m, v);
				analysed.push_back(m);
				return false;
			}
			if (tau::get(m).is(tau::wff) && !is_transparent_connective(m)) {
				// A wff-level shape none of the above recognise --
				// e.g. wff_t/wff_f, a bf_interval leaf (deliberately
				// excluded from is_atomic_fm,
				// tau_tree_queries.tmpl.h:155), or a constraint.
				// Fail closed: this analysis has no rule for it, so
				// freeze rather than silently leaving its variables
				// `eliminable`.
				assign(m, elim_verdict::frozen);
				for (tref v : get_free_vars<node>(m))
					merge(m, v);
				analysed.push_back(m);
				return false;
			}
			return true;
		};
		idni::pre_order<node>(conj).visit_unique(visit);
	}

	// Read every analysed node (vars AND atoms) into the flat maps. Verdicts
	// of merged union-find components settle only once every conjunct has
	// been visited, so this runs after the conjunct loop above, not inside
	// `visit`.
	auto record = [&](tref n) {
		const elim_verdict v = verdict_of_root(n);
		res.verdicts.insert_or_assign(n, v);
		res.members[v].insert(n);
	};
	for (tref v : block_vars) record(v);
	for (tref conj : conjuncts) record(conj);
	for (tref m : analysed) record(m);

	// Read components off, keyed by block variable.
	for (tref v : block_vars) {
		trefs comp;
		for (tref conj : conjuncts)
			if (uf.connected(v, conj))
				comp.push_back(conj);
		res.components.insert_or_assign(v, std::move(comp));
	}
	DBG(LOG_TRACE << "analyse_block: " << block_vars.size() << " var(s), "
		<< conjuncts.size() << " conjunct(s), has_ref=" << res.has_ref
		<< "\n";)
	return res;
}

template<NodeType node>
void scoped_verdict_resolver<node>::open() {
	scoped.open();
}

template<NodeType node>
std::optional<typename scoped_verdict_resolver<node>::uf_t::scope_error>
scoped_verdict_resolver<node>::close() {
	return scoped.close();
}

template<NodeType node>
typename scoped_verdict_resolver<node>::element
scoped_verdict_resolver<node>::insert(tref n, elim_verdict k) {
	auto e = scoped.push(n);
	kinds.emplace(e, k);
	return e;
}

template<NodeType node>
elim_verdict scoped_verdict_resolver<node>::kind_of(tref n) {
	auto root = scoped.root(scoped.insert(n));
	if (auto it = kinds.find(root); it != kinds.end()) return it->second;
	return kinds.emplace(root, elim_verdict::eliminable).first->second;
}

template<NodeType node>
typename scoped_verdict_resolver<node>::element
scoped_verdict_resolver<node>::assign(tref n, elim_verdict k) {
	auto e = scoped.insert(n);
	auto root = scoped.root(e);
	if (auto it = kinds.find(root); it != kinds.end())
		kinds.insert_or_assign(root, join(it->second, k));
	else kinds.insert_or_assign(root, k);
	return root;
}

template<NodeType node>
typename scoped_verdict_resolver<node>::element
scoped_verdict_resolver<node>::merge(tref a, tref b) {
	auto ka = kind_of(a);
	auto kb = kind_of(b);
	auto new_root = scoped.merge(a, b);
	kinds.insert_or_assign(new_root, join(ka, kb));
	return new_root;
}

template <NodeType node>
eliminability<node> analyse_formula(tref form, const analysis_context<node>& ctx) {
	using tau = tree<node>;
	eliminability<node> res;

	// The two deleted collectors, generalised: reference usage and
	// arithmetic taint, each now over the full `elim_verdict` lattice.
	scoped_verdict_resolver<node> ref_res;
	scoped_verdict_resolver<node> arith_res;

	// A wff wrapping an unresolved predicate reference. wff_ref is
	// deliberately excluded from is_atomic_fm (tau_tree_queries.tmpl.h:155),
	// so it needs its own branch.
	auto is_ref_fm = [](tref n) {
		const auto& t = tau::get(n);
		return t.is(tau::wff) && t.child_is(tau::wff_ref);
	};

	// Read every node registered in scope s (in EITHER resolver -- the ref
	// resolver's scopes are a superset of the arith resolver's, since it alone
	// registers wff_ref nodes) into the result maps. Two passes: the first
	// only reads scoped_union_find's raw storage (never mutates it); the
	// second calls kind_of, which -- for a node one resolver never saw --
	// inserts a default `eliminable` entry as a side effect. Splitting the
	// passes keeps that mutation from invalidating the iteration it would
	// otherwise share.
	auto snapshot_scope = [&](typename scoped_verdict_resolver<node>::scope s) {
		subtree_unordered_set<node> seen;
		for (auto [elem, _] : ref_res.scoped.uf)
			if (elem.first == s) seen.insert(elem.second);
		for (auto [elem, _] : arith_res.scoped.uf)
			if (elem.first == s) seen.insert(elem.second);
		for (tref n : seen) {
			elim_verdict v = join(ref_res.kind_of(n), arith_res.kind_of(n));
			// A tref that is the SAME node under two unrelated binders
			// (structurally-identical variables hash-cons to one tref
			// regardless of which quantifier scope pushed them) is seen
			// here once per scope, each with its own scope-local verdict.
			// Both old collectors accumulate into a flat SET -- once
			// used/tainted in ANY scope, always so -- so recording must
			// join with whatever this node already resolved to, never
			// overwrite it; `members` is rebucketed alongside so it never
			// disagrees with `verdicts`.
			if (auto it = res.verdicts.find(n); it != res.verdicts.end()) {
				elim_verdict joined = join(it->second, v);
				if (joined != it->second) {
					res.members[it->second].erase(n);
					it->second = joined;
				}
				res.members[joined].insert(n);
			} else {
				res.verdicts.emplace(n, v);
				res.members[v].insert(n);
			}
		}
	};

	auto visit_subtree = [](tref) -> bool { return true; };

	auto visit = [&](tref m) -> bool {
		if (is_quantifier<node>(m)) {
			ref_res.open();
			arith_res.open();
			if (tref v = tau::get(m).find_top(
				(bool(*)(tref)) is_var_or_capture<node>); v) {
				ref_res.insert(v, elim_verdict::eliminable);
				arith_res.insert(v, elim_verdict::eliminable);
			}
			return true;
		}
		if (is_ref_fm(m)) {
			// Seed: every variable reachable from a predicate reference
			// is frozen. No BA-type filter -- predicate arguments carry
			// no restriction analogous to arithmetic's. The arith
			// resolver does not process wff_ref at all, matching the
			// deleted taint collector (which had no branch for it
			// either).
			ref_res.assign(m, elim_verdict::frozen);
			for (tref v : get_free_vars<node>(m)) ref_res.merge(m, v);
			return false;
		}
		if (is_atomic_fm<node>(m)) {
			// ref resolver: neutral seed, propagate through ALL free
			// variables -- mirrors the deleted reference-usage
			// collector exactly.
			ref_res.assign(m, elim_verdict::eliminable);
			for (tref v : get_free_vars<node>(m)) ref_res.merge(m, v);

			// arith resolver: seed per the arithmetic/blasting table
			// (detail::atom_arith_verdict, shared with
			// `analyse_block`), propagate only through free variables
			// whose type's owning BA declares `arith_ops`.
			const trefs& fvs = get_free_vars<node>(m);
			arith_res.assign(m, detail::atom_arith_verdict<node>(m,
				ctx.arith_is_solver_owned));
			for (tref fv : fvs)
				if (pack_type_has_arith_ops<node>(tau::get(fv).get_ba_type()))
					arith_res.merge(m, fv);
			return false;
		}
		return true;
	};

	auto up = [&](tref m) {
		if (!is_quantifier<node>(m)) return;
		auto s = ref_res.scoped.scopes.back();
		snapshot_scope(s);
		ref_res.close();
		arith_res.close();
	};

	idni::pre_order<node>(form).visit(visit, visit_subtree, up);
	snapshot_scope(ref_res.scoped.global);

	res.arith_floor = ctx.arith_is_solver_owned;
	return res;
}

} // namespace idni::tau_lang
