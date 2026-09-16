// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ex_subs_based_elimination"

#include <functional>
#include <map>
#include <optional>

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
	// Caveat: the capture-guard's `bound == var` check below is a raw tref
	// (pointer) compare, not the content compare above, so it is sibling-
	// sensitive in principle. It is inert today because `bound` (child 0 of
	// a 2-child quantifier) always carries its own scope as right sibling,
	// and a caller's `var` can never carry that same scope as its sibling --
	// a scope cannot contain itself.
	const std::pair<tref, tref> key { tau::trim_right_sibling(var),
		tau::trim_right_sibling(ex_clause) };
	// Every return, including both identity returns (raw ex_clause) and the
	// reget'd substitution result, must be trimmed before being stored: a
	// cache hit serves back whatever was stored under this trimmed key to
	// ANY call that hashes to it, not just the call that produced it. The
	// reget path is not exempt just because it looks freshly built --
	// replace_if's rebuild and reget's own `get(value, children,
	// right_sibling())` both carry the *current* ex_clause's right sibling
	// through unchanged, so an untrimmed return there would just as silently
	// hand a later call the first call's sibling attachment.
	//
	// Return identity matters too: callers test progress by comparing the
	// result against `ex_clause` by tref, so a content-unchanged result must
	// come back as `ex_clause` itself -- handing back the trimmed twin fakes
	// an elimination that never happened.
	auto identity_preserving = [ex_clause](tref r) {
		return tau::subtree_equals(r, ex_clause) ? ex_clause : r;
	};
	if (auto it = cache.find(key); it != cache.end())
		return identity_preserving(it->second);
	auto memo = [&key, &identity_preserving](tref r) {
		return identity_preserving(
			cache.emplace(key, tau::trim_right_sibling(r)).first->second);
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

// Forward declarations: normal_forms.h / normal_forms_transformations.h and
// the bv type predicate come later in the amalgamation than this file.
template <NodeType node> tref to_nnf(tref fm);
template <NodeType node> tref order_atoms_to_literals(tref fm);
template <NodeType node> bool is_bv_type_family(size_t ba_type_id);

// Block form (see the header): eliminate the bitvector variables of a run
// of same-kind existential binders that a total definition determines.
// Identities used, in order:
//   (1) clauses with the same witness merge:
//         (D1 || x = c) && (D2 || x = c)  ==  ((D1 && D2) || x = c)
//   (2) a clause set whose D-conjunction is unsatisfiable forces x in every
//       cell; with pairwise-valid D_i || D_j the cells are exclusive and
//         ex x (/\_i (D_i || x = c_i) && psi(x))  ==  \/_i (!D_i && psi[x := c_i])
//       (non-exclusive cells keep the consistency atoms D_j || c_i = c_j;
//       without readers the right-hand side is T);
//   (3) same-kind binders commute, so a definition found in an inner scope
//       of the block may be used as long as x occurs only inside that scope.
// The propositional checks are a bounded brute force over the atoms of the
// guards in canonical spelling (order atoms as `<`, `a <= b` as `!(b < a)`,
// then NNF), so `42 !< x` and `x !<= 42` are one atom in two polarities.
// Flattening of `D || (A && B)` into `(D || A), (D || B)` is capped, and a
// clause beyond the cap is a reader. Nothing else is touched.
template <NodeType node>
tref ex_subs_block_elimination(tref root) {
	using tau = tree<node>;
	auto is_wff_of = [](tref n, auto nt) { const tau& t = tau::get(n); return t.is(tau::wff) && t.child_is(nt); };
	auto lhs = [](tref n) { return tau::get(n)[0].first(); };
	auto rhs = [](tref n) { return tau::get(n)[0].second(); };
	std::function<void(tref, std::vector<tref>&)> conjuncts = [&](tref s, std::vector<tref>& out) {
		if (is_wff_of(s, tau::wff_and)) { conjuncts(lhs(s), out); conjuncts(rhs(s), out); } else out.push_back(s);
	};
	std::function<void(tref, std::vector<tref>&)> disjuncts = [&](tref s, std::vector<tref>& out) {
		if (is_wff_of(s, tau::wff_or)) { disjuncts(lhs(s), out); disjuncts(rhs(s), out); } else out.push_back(s);
	};
	auto is_eq_of = [&](tref d, tref var, tref& t_out, tref& bf_out) -> bool {
		const tau& td = tau::get(d);
		if (!(td.is(tau::wff) && td.child_is(tau::bf_eq))) return false;
		const tau& atom = td[0];
		tref l = atom[0][0].get(), r = atom[1][0].get();
		const bool lv = tau::get(l) == tau::get(var), rv = tau::get(r) == tau::get(var);
		if (lv == rv) return false;
		tref w = lv ? r : l;
		if (contains<node>(w, var)) return false;
		t_out = w; bf_out = lv ? atom[1].get() : atom[0].get(); return true;
	};
	subtree_unordered_map<node, tref> canon_cache;
	auto canon = [&](tref g) -> tref {
		if (auto it = canon_cache.find(g); it != canon_cache.end()) return it->second;
		tref h = order_atoms_to_literals<node>(g);
		auto le_to_lt = [&](tref m) -> tref {
			const tau& tm = tau::get(m);
			if (tm.is(tau::wff) && tm.child_is(tau::bf_lteq)) {
				const tau& atom = tm[0];
				return tau::build_wff_neg(tau::build_bf_lt(atom.second(), atom.first()));
			}
			return m;
		};
		h = pre_order<node>(h).apply_unique(le_to_lt);
		return canon_cache[g] = to_nnf<node>(h);
	};
	// Flatten a clause mentioning x: D || (A && B) -> (D || A), (D || B), recursively on
	// the x side only. Capped at 16 pieces: beyond it the clause stays a reader (the
	// distribution is exponential otherwise).
	std::function<bool(tref, tref, std::vector<std::vector<tref>>&)> flatten =
	[&](tref var, tref c, std::vector<std::vector<tref>>& out) -> bool {
		if (out.size() > 16) return false;
		std::vector<tref> ds; disjuncts(c, ds);
		for (size_t i = 0; i < ds.size(); ++i) {
			if (is_wff_of(ds[i], tau::wff_and) && contains<node>(ds[i], var)) {
				std::vector<tref> parts; conjuncts(ds[i], parts);
				for (tref p : parts) {
					std::vector<tref> rebuilt(ds.begin(), ds.end()); rebuilt[i] = p;
					if (!flatten(var, rebuilt.size() == 1 ? rebuilt[0] : tau::build_wff_or(rebuilt), out)) return false;
				}
				return true;
			}
		}
		out.push_back(ds);
		return out.size() <= 16;
	};
	struct dclause { tref D; tref c; tref cbf; tref orig; };   // D || x = c (D == nullptr: bare equation); cbf = c as a bf; orig = the flat clause
	auto classify = [&](tref var, const std::vector<tref>& ds, dclause& dc) -> bool {
		int eqpos = -1; tref w = nullptr, wbf = nullptr;
		for (size_t i = 0; i < ds.size(); ++i) {
			if (contains<node>(ds[i], var)) {
				tref w2, w2bf; if (eqpos >= 0 || !is_eq_of(ds[i], var, w2, w2bf)) return false;
				eqpos = static_cast<int>(i); w = w2; wbf = w2bf;
			}
		}
		if (eqpos < 0) return false;
		std::vector<tref> rest; for (size_t i = 0; i < ds.size(); ++i) if (static_cast<int>(i) != eqpos) rest.push_back(ds[i]);
		dc = {rest.empty() ? nullptr : rest.size() == 1 ? rest[0] : tau::build_wff_or(rest), w, wbf, ds.size() == 1 ? ds[0] : tau::build_wff_or(ds)};
		return true;
	};
	auto is_bv_var = [&](tref v) { const size_t vt = tau::get(v).get_ba_type(); return vt != 0 && is_bv_type_family<node>(vt); };
	struct binder { tref bnode; tref var; tref scope; };
	auto collect = [&](tref n0) {
		std::vector<binder> out; std::vector<tref> st{n0};
		while (!st.empty()) {
			tref n = st.back(); st.pop_back();
			if (!is_wff_of(n, tau::wff_ex)) continue;
			const tau& t = tau::get(n);
			out.push_back({n, t[0].first(), t[0].second()});
			std::vector<tref> cs; conjuncts(t[0].second(), cs);
			for (tref c : cs) if (is_wff_of(c, tau::wff_ex)) st.push_back(c);
		}
		return out;
	};
	// Propositional unsatisfiability of /\ D_i (canonical atoms; complementary atoms are one variable's two polarities).
	subtree_unordered_map<node, bool> unsat_memo;
	auto prop_unsat_raw = [&](const std::vector<tref>& Ds) -> bool {
		std::vector<tref> atoms; subtree_unordered_map<node, size_t> index;      // kanonisches positives Atom -> Nr
		struct lit { size_t v; bool neg; };
		auto atom_of = [&](tref a) -> std::optional<lit> {
			tref ca = canon(a);
			const tau& t = tau::get(ca);
			bool neg = false; tref pos = ca;
			if (t.is(tau::wff) && t.child_is(tau::wff_neg)) { neg = true; pos = t[0].first(); }
			// complementary spellings: match pos against canon(!pos')
			if (auto it = index.find(pos); it != index.end()) return lit{it->second, neg};
			tref npos = canon(tau::build_wff_neg(pos));
			if (auto it = index.find(npos); it != index.end()) return lit{it->second, !neg};
			if (atoms.size() >= 18) return std::nullopt;
			index.emplace(pos, atoms.size()); atoms.push_back(pos); return lit{atoms.size() - 1, neg};
		};
		// evaluate each D_i as a tree over &&/||/atoms under an assignment
		std::function<std::optional<std::function<bool(uint32_t)>>(tref)> compile = [&](tref f) -> std::optional<std::function<bool(uint32_t)>> {
			const tau& t = tau::get(f);
			if (t.is(tau::wff) && (t.child_is(tau::wff_and) || t.child_is(tau::wff_or))) {
				auto a = compile(lhs(f)), b = compile(rhs(f)); if (!a || !b) return std::nullopt;
				const bool is_and = t.child_is(tau::wff_and);
				return [a, b, is_and](uint32_t m) { return is_and ? ((*a)(m) && (*b)(m)) : ((*a)(m) || (*b)(m)); };
			}
			if (tau::get(f).equals_T()) return [](uint32_t) { return true; };
			if (tau::get(f).equals_F()) return [](uint32_t) { return false; };
			if (!is_atomic_fm<node>(f) && !(t.is(tau::wff) && t.child_is(tau::wff_neg))) return std::nullopt;
			auto l = atom_of(f); if (!l) return std::nullopt;
			const size_t v = l->v; const bool neg = l->neg;
			return [v, neg](uint32_t m) { const bool b = (m >> v) & 1u; return neg ? !b : b; };
		};
		std::vector<std::function<bool(uint32_t)>> fs;
		for (tref D : Ds) { auto c = compile(D); if (!c) return false; fs.push_back(*c); }
		const uint32_t n = static_cast<uint32_t>(atoms.size());
		for (uint32_t m = 0; m < (1u << n); ++m) {
			bool all = true; for (auto& f : fs) if (!f(m)) { all = false; break; }
			if (all) return false;   // satisfying assignment: not refuted
		}
		return true;
	};
	auto prop_unsat = [&](const std::vector<tref>& Ds) -> bool {
		for (tref D : Ds) if (!D) return true;
		tref key = Ds.size() == 1 ? Ds[0] : tau::build_wff_and(Ds);
		if (auto it = unsat_memo.find(key); it != unsat_memo.end()) return it->second;
		return unsat_memo[key] = prop_unsat_raw(Ds);
	};
	struct def { tref scope; std::vector<tref> consumed; std::vector<dclause> defs; std::vector<tref> readers; };
	// Cache per (var, scope): a scope changes its tref once something was substituted in it.
	std::map<std::pair<tref, tref>, std::optional<def>> def_cache;
	auto find_def_in_scope = [&](tref var, const binder& b, def& d) -> bool {
		const std::pair<tref, tref> key{var, b.scope};
		if (auto it = def_cache.find(key); it != def_cache.end()) { if (it->second) { d = *it->second; return true; } return false; }
		{
			std::vector<tref> cs; conjuncts(b.scope, cs);
			def cand{b.scope, {}, {}, {}};
			for (tref c : cs) {
				if (!contains<node>(c, var)) continue;
				std::vector<std::vector<tref>> flat;
				if (!flatten(var, c, flat)) { cand.readers.push_back(c); continue; }
				std::vector<dclause> cdefs; std::vector<tref> creaders;
				for (auto& ds : flat) {
					dclause dc;
					if (classify(var, ds, dc)) cdefs.push_back(dc);
					else creaders.push_back(ds.size() == 1 ? ds[0] : tau::build_wff_or(ds));
				}
				if (cdefs.empty()) { cand.readers.push_back(c); continue; }   // a pure reader clause stays as it is
				cand.consumed.push_back(c);
				cand.defs.insert(cand.defs.end(), cdefs.begin(), cdefs.end());
				cand.readers.insert(cand.readers.end(), creaders.begin(), creaders.end());
			}
			if (cand.defs.empty()) { def_cache[key] = std::nullopt; return false; }
			// (i) merge clauses with the same witness c
			struct merged { tref D; tref c; tref cbf; std::vector<tref> origs; bool dfalse; };
			std::vector<merged> ms; subtree_unordered_map<node, size_t> by_c;
			for (const dclause& dc : cand.defs) {
				auto it = by_c.find(dc.cbf);
				if (it == by_c.end()) { by_c.emplace(dc.cbf, ms.size()); ms.push_back({dc.D, dc.c, dc.cbf, {dc.orig}, dc.D == nullptr}); continue; }
				merged& m = ms[it->second]; m.origs.push_back(dc.orig);
				if (m.dfalse || dc.D == nullptr) { m.dfalse = true; m.D = nullptr; } else m.D = tau::build_wff_and(m.D, dc.D);
			}
			// (ii) the smallest subset whose D-conjunction is unsatisfiable is the total definition; all other clauses are readers
			std::vector<size_t> chosen;
			for (size_t i = 0; i < ms.size() && chosen.empty(); ++i) if (ms[i].dfalse) chosen = {i};
			const size_t n = ms.size();
			for (size_t s = 2; s <= std::min<size_t>(4, n) && chosen.empty(); ++s) {
				std::vector<size_t> idx(s); for (size_t i = 0; i < s; ++i) idx[i] = i;
				while (true) {
					std::vector<tref> Ds; for (size_t i : idx) Ds.push_back(ms[i].D);
					if (prop_unsat(Ds)) { chosen = idx; break; }
					// next combination
					int k = static_cast<int>(s) - 1;
					while (k >= 0 && idx[k] == n - s + k) --k;
					if (k < 0) break;
					++idx[k]; for (size_t j = k + 1; j < s; ++j) idx[j] = idx[j - 1] + 1;
				}
			}
			if (chosen.empty()) { def_cache[key] = std::nullopt; return false; }   // no total definition in this scope
			d = def{b.scope, cand.consumed, {}, cand.readers};
			std::vector<bool> is_chosen(n, false); for (size_t i : chosen) is_chosen[i] = true;
			for (size_t i = 0; i < n; ++i) {
				if (is_chosen[i]) d.defs.push_back({ms[i].D, ms[i].c, ms[i].cbf, nullptr});
				else for (tref o : ms[i].origs) d.readers.push_back(o);
			}
			def_cache[key] = d; return true;
		}
	};
	auto find_def = [&](tref var, const std::vector<binder>& chain, def& d) -> bool {
		for (const binder& b : chain) if (find_def_in_scope(var, b, d)) return true;
		return false;
	};
	subtree_set<node> skip;
	subtree_map<node, def> prev_defs;
	int round = 0;
	for (; round < 256; ++round) {
		auto chain = collect(root);
		// A definition found in the previous round stays valid while its clauses are still
		// top-level conjuncts of a block scope (by the leaf choice no definition mentions the
		// eliminated variable); its readers are recomputed from the current scope.
		std::vector<std::pair<binder, def>> defs;
		for (const binder& b : chain) {
			if (!is_bv_var(b.var) || skip.contains(b.var)) continue;
			bool reused = false;
			if (auto it = prev_defs.find(b.var); it != prev_defs.end() && !it->second.consumed.empty()) {
				for (const binder& s : chain) {
					std::vector<tref> cs; conjuncts(s.scope, cs);
					bool all_in = true;
					for (tref c : it->second.consumed) { bool f = false; for (tref x : cs) if (x == c) { f = true; break; } if (!f) { all_in = false; break; } }
					if (all_in) {
						def d = it->second; d.scope = s.scope; d.readers.clear();
						for (tref c : cs) {
							if (!contains<node>(c, b.var)) continue;
							bool consumed = false; for (tref k : d.consumed) if (k == c) { consumed = true; break; }
							if (!consumed) { d.readers.push_back(c); continue; }
							std::vector<std::vector<tref>> flat;
							if (!flatten(b.var, c, flat)) { d.readers.push_back(c); continue; }
							for (auto& ds : flat) { dclause dc; if (!classify(b.var, ds, dc)) d.readers.push_back(ds.size() == 1 ? ds[0] : tau::build_wff_or(ds)); }
						}
						defs.emplace_back(b, d); reused = true; break;
					}
				}
			}
			if (reused) continue;
			def d; if (find_def(b.var, chain, d)) defs.emplace_back(b, d);
		}
		if (defs.empty()) break;
		prev_defs.clear(); for (auto& pd : defs) prev_defs[pd.first.var] = pd.second;
		int pick = -1;
		for (size_t i = 0; i < defs.size() && pick < 0; ++i) {
			bool used = false;
			for (size_t j = 0; j < defs.size() && !used; ++j) {
				if (j == i) continue;
				for (const dclause& dc : defs[j].second.defs)
					if ((dc.D && contains<node>(dc.D, defs[i].first.var)) || contains<node>(dc.c, defs[i].first.var)) { used = true; break; }
			}
			if (!used) pick = static_cast<int>(i);
		}
		if (pick < 0) break;   // every remaining definition mentions another one: nothing to do
		const binder bx = defs[pick].first; const def d = defs[pick].second;
		auto decline = [&](const char*) { skip.insert(bx.var); };
		if (!(d.scope == bx.scope || contains<node>(bx.scope, d.scope))) { decline("Definition-ausserhalb-des-Scopes"); continue; }
		{	subtree_map<node, tref> m; m[d.scope] = tau::_T();
			tref outside = rewriter::replace<node>(bx.scope, m);
			if (contains<node>(outside, bx.var)) { decline("Vorkommen-ausserhalb-S"); continue; }
		}
		bool bad = false;
		for (const dclause& dc : d.defs) {
			auto tv = tau::get(dc.c).select_all(is_var_or_capture<node>());
			subtree_set<node> term_vars(tv.begin(), tv.end());
			if (tau::get(d.scope).find_top([&](tref n) {
				return is_logical_or_functional_quant<node>(n) && term_vars.contains(tau::get(n).child(0)); })) { bad = true; break; }
		}
		if (bad) { decline("Capture"); continue; }
		// rest = conjuncts of S free of x; psi = the readers (pieces mentioning x that are no definition)
		std::vector<tref> cs; conjuncts(d.scope, cs);
		std::vector<tref> rest;
		for (tref c : cs) {
			if (!contains<node>(c, bx.var)) { rest.push_back(c); continue; }
			bool consumed = false; for (tref k : d.consumed) if (k == c) { consumed = true; break; }
			(void)consumed;   // reader clauses are already in d.readers
		}
		const std::vector<tref>& psi = d.readers;
		auto subst = [&](tref body, tref w) -> tref {
			tref var = bx.var;
			auto query = [&var](tref n) -> bool {
				if (is<node>(n, tau::wff_all) || is<node>(n, tau::wff_ex)) { if (tau::get(n).child(0) == var) return false; }
				return true;
			};
			return contains<node>(body, var) ? tree<node>::reget(rewriter::replace_if<node>(body, var, w, query)) : body;
		};
		// Exclusive cells: two clauses never force x to different values at once when D_i || D_j is
		// propositionally valid; then the consistency atoms are not needed. The chosen subset is
		// exhaustive by construction, so the "no clause forces x" case never happens.
		bool exclusive = true;
		for (size_t i = 0; i < d.defs.size() && exclusive; ++i) for (size_t j = i + 1; j < d.defs.size() && exclusive; ++j) {
			if (tau::get(d.defs[i].c) == tau::get(d.defs[j].c)) continue;
			if (!d.defs[i].D || !d.defs[j].D) { exclusive = false; break; }   // D = F: that equation always forces
			std::vector<tref> both{to_nnf<node>(tau::build_wff_neg(d.defs[i].D)), to_nnf<node>(tau::build_wff_neg(d.defs[j].D))};
			if (!prop_unsat(both)) exclusive = false;
		}
		tref s2 = nullptr;
		if (psi.empty() && exclusive) s2 = tau::_T();
		else {
			std::vector<tref> alts;
			for (size_t i = 0; i < d.defs.size(); ++i) {
				std::vector<tref> parts;
				if (d.defs[i].D) parts.push_back(to_nnf<node>(tau::build_wff_neg(d.defs[i].D)));
				if (!exclusive) for (size_t j = 0; j < d.defs.size(); ++j) {
					if (j == i) continue;
					if (tau::get(d.defs[i].c) == tau::get(d.defs[j].c)) continue;
					tref eq = tau::build_bf_eq(d.defs[i].cbf, d.defs[j].cbf);
					parts.push_back(d.defs[j].D ? tau::build_wff_or(d.defs[j].D, eq) : eq);
				}
				for (tref q : psi) parts.push_back(subst(q, d.defs[i].c));
				alts.push_back(parts.empty() ? tau::_T() : parts.size() == 1 ? parts[0] : tau::build_wff_and(parts));
			}
			s2 = alts.size() == 1 ? alts[0] : tau::build_wff_or(alts);
		}
		if (!rest.empty()) { rest.push_back(s2); s2 = tau::build_wff_and(rest); }
		tref before = root;
		{	subtree_map<node, tref> m1; m1[d.scope] = s2; root = rewriter::replace<node>(root, m1); }
		if (root == before) { decline("Scope-nicht-ersetzt"); continue; }
		auto chain2 = collect(root);
		for (const binder& b : chain2) if (tau::get(b.var) == tau::get(bx.var)) {
			subtree_map<node, tref> m2; m2[b.bnode] = b.scope; root = rewriter::replace<node>(root, m2); break;
		}
	}
	return root;
}

template <NodeType node>
tref ex_subs_definitional_elimination(tref fm) {
	using tau = tree<node>;
	auto block = [](tref n) -> tref {
		if (!is_child<node>(n, tau::wff_ex)) return n;
		return ex_subs_block_elimination<node>(n);
	};
	return pre_order<node>(fm).apply_unique(block);
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