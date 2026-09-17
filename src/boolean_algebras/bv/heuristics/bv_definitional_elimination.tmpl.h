// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_definitional_elimination"

namespace idni::tau_lang {

// Declared here as well: the plugin may be compiled ahead of the normal-form
// headers that define them.
template <NodeType node> tref to_nnf(tref fm);
template <NodeType node> tref order_atoms_to_literals(tref fm);

namespace bv_defelim_detail {

// The brute-force check below evaluates every assignment of up to this many
// atoms; the shift below would overflow past it.
inline constexpr size_t hard_atom_cap = 30;

inline size_t clause_cap() { return bv_defelim_max_clauses; }
inline size_t atom_cap() {
	return std::min<size_t>(bv_defelim_max_atoms, hard_atom_cap);
}
inline size_t subset_cap() { return bv_defelim_max_subset; }
inline size_t round_cap() { return bv_defelim_max_rounds; }

} // namespace bv_defelim_detail

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
// guards in canonical spelling (order atoms as `<`, a bitvector `a <= b` as
// `!(b < a)`, then NNF), so `42 !< x` and `x !<= 42` are one atom in two
// polarities. Flattening of `D || (A && B)` into `(D || A), (D || B)` is
// capped, and a clause beyond the cap is a reader. Nothing else is touched.
template <NodeType node>
tref bv_definitional_block_elimination(tref root) {
	using tau = tree<node>;
	using namespace bv_defelim_detail;
	auto is_wff_of = [](tref n, auto nt) {
		const tau& t = tau::get(n);
		return t.is(tau::wff) && t.child_is(nt);
	};
	auto lhs = [](tref n) { return tau::get(n)[0].first(); };
	auto rhs = [](tref n) { return tau::get(n)[0].second(); };
	std::function<void(tref, std::vector<tref>&)> conjuncts =
		[&](tref s, std::vector<tref>& out) {
			if (is_wff_of(s, tau::wff_and)) {
				conjuncts(lhs(s), out); conjuncts(rhs(s), out);
			} else out.push_back(s);
		};
	std::function<void(tref, std::vector<tref>&)> disjuncts =
		[&](tref s, std::vector<tref>& out) {
			if (is_wff_of(s, tau::wff_or)) {
				disjuncts(lhs(s), out); disjuncts(rhs(s), out);
			} else out.push_back(s);
		};
	auto or_of = [&](const std::vector<tref>& ds) -> tref {
		return ds.size() == 1 ? ds[0] : tau::build_wff_or(ds);
	};
	auto and_of = [&](const std::vector<tref>& cs) -> tref {
		return cs.size() == 1 ? cs[0] : tau::build_wff_and(cs);
	};
	// `d` is `var = w` or `w = var` with w free of var: w as a term and as
	// the bf node holding it.
	auto is_eq_of = [&](tref d, tref var, tref& t_out, tref& bf_out) -> bool {
		const tau& td = tau::get(d);
		if (!(td.is(tau::wff) && td.child_is(tau::bf_eq))) return false;
		const tau& atom = td[0];
		tref l = atom[0][0].get(), r = atom[1][0].get();
		const bool lv = tau::get(l) == tau::get(var);
		const bool rv = tau::get(r) == tau::get(var);
		if (lv == rv) return false;
		tref w = lv ? r : l;
		if (contains<node>(w, var)) return false;
		t_out = w; bf_out = lv ? atom[1].get() : atom[0].get();
		return true;
	};
	// Canonical spelling of a guard for the propositional checks. `a <= b`
	// becomes `!(b < a)` only over a bitvector type: the two atoms are
	// complementary in a total order and not in general.
	subtree_unordered_map<node, tref> canon_cache;
	auto canon = [&](tref g) -> tref {
		if (auto it = canon_cache.find(g); it != canon_cache.end())
			return it->second;
		tref h = order_atoms_to_literals<node>(g);
		auto le_to_lt = [&](tref m) -> tref {
			const tau& tm = tau::get(m);
			if (tm.is(tau::wff) && tm.child_is(tau::bf_lteq)) {
				const size_t type = find_ba_type<node>(m);
				if (type == 0 || !is_bv_type_family<node>(type)) return m;
				const tau& atom = tm[0];
				return tau::build_wff_neg(
					tau::build_bf_lt(atom.second(), atom.first()));
			}
			return m;
		};
		h = pre_order<node>(h).apply_unique(le_to_lt);
		return canon_cache[g] = to_nnf<node>(h);
	};
	// Flatten a clause mentioning x: D || (A && B) -> (D || A), (D || B),
	// recursively on the x side only, up to the clause cap.
	std::function<bool(tref, tref, std::vector<std::vector<tref>>&)> flatten =
		[&](tref var, tref c, std::vector<std::vector<tref>>& out) -> bool {
			if (out.size() > clause_cap()) return false;
			std::vector<tref> ds; disjuncts(c, ds);
			for (size_t i = 0; i < ds.size(); ++i) {
				if (is_wff_of(ds[i], tau::wff_and) && contains<node>(ds[i], var)) {
					std::vector<tref> parts; conjuncts(ds[i], parts);
					for (tref p : parts) {
						std::vector<tref> rebuilt(ds.begin(), ds.end());
						rebuilt[i] = p;
						if (!flatten(var, or_of(rebuilt), out)) return false;
					}
					return true;
				}
			}
			out.push_back(ds);
			return out.size() <= clause_cap();
		};
	// D || x = c; D == nullptr is a bare equation; cbf is c as a bf; orig
	// is the flat clause.
	struct dclause { tref D; tref c; tref cbf; tref orig; };
	auto classify = [&](tref var, const std::vector<tref>& ds, dclause& dc) -> bool {
		int eqpos = -1; tref w = nullptr, wbf = nullptr;
		for (size_t i = 0; i < ds.size(); ++i) {
			if (!contains<node>(ds[i], var)) continue;
			tref w2 = nullptr, w2bf = nullptr;
			if (eqpos >= 0 || !is_eq_of(ds[i], var, w2, w2bf)) return false;
			eqpos = static_cast<int>(i); w = w2; wbf = w2bf;
		}
		if (eqpos < 0) return false;
		std::vector<tref> rest;
		for (size_t i = 0; i < ds.size(); ++i)
			if (static_cast<int>(i) != eqpos) rest.push_back(ds[i]);
		dc = { rest.empty() ? nullptr : or_of(rest), w, wbf, or_of(ds) };
		return true;
	};
	auto is_bv_var = [&](tref v) {
		const size_t vt = tau::get(v).get_ba_type();
		return vt != 0 && is_bv_type_family<node>(vt);
	};
	struct binder { tref bnode; tref var; tref scope; };
	// The `ex` binders in conjunct position under n0, outermost first.
	auto collect = [&](tref n0) {
		std::vector<binder> out; std::vector<tref> st{n0};
		while (!st.empty()) {
			tref n = st.back(); st.pop_back();
			if (!is_wff_of(n, tau::wff_ex)) continue;
			const tau& t = tau::get(n);
			out.push_back({ n, t[0].first(), t[0].second() });
			std::vector<tref> cs; conjuncts(t[0].second(), cs);
			for (tref c : cs) if (is_wff_of(c, tau::wff_ex)) st.push_back(c);
		}
		return out;
	};
	// Rebuild the block along its binder chain only -- wff_and children and
	// binder scopes -- replacing each binder node b by fn(b) where fn
	// returns something else. Positions off the chain (a copy of a scope
	// under a negation, say) are never touched.
	std::function<tref(tref, const std::function<tref(tref)>&)> along =
		[&](tref n, const std::function<tref(tref)>& fn) -> tref {
			if (is_wff_of(n, tau::wff_and)) {
				tref l = along(lhs(n), fn), r = along(rhs(n), fn);
				return (l == lhs(n) && r == rhs(n)) ? n : tau::build_wff_and(l, r);
			}
			if (is_wff_of(n, tau::wff_ex)) {
				tref m = fn(n);
				if (m != n) return m;
				tref sc = along(rhs(n), fn);
				return sc == rhs(n) ? n : tau::build_wff_ex(lhs(n), sc, false);
			}
			return n;
		};
	// Propositional unsatisfiability of /\ D_i over canonical atoms
	// (complementary spellings are one variable's two polarities).
	subtree_unordered_map<node, bool> unsat_memo;
	auto prop_unsat_raw = [&](const std::vector<tref>& Ds) -> bool {
		std::vector<tref> atoms;
		subtree_unordered_map<node, size_t> index;   // canonical positive atom -> index
		struct lit { size_t v; bool neg; };
		auto atom_of = [&](tref a) -> std::optional<lit> {
			tref ca = canon(a);
			const tau& t = tau::get(ca);
			bool neg = false; tref pos = ca;
			if (t.is(tau::wff) && t.child_is(tau::wff_neg)) { neg = true; pos = t[0].first(); }
			if (auto it = index.find(pos); it != index.end()) return lit{ it->second, neg };
			tref npos = canon(tau::build_wff_neg(pos));
			if (auto it = index.find(npos); it != index.end()) return lit{ it->second, !neg };
			if (atoms.size() >= atom_cap()) return std::nullopt;
			index.emplace(pos, atoms.size()); atoms.push_back(pos);
			return lit{ atoms.size() - 1, neg };
		};
		using eval_t = std::function<bool(uint32_t)>;
		std::function<std::optional<eval_t>(tref)> compile =
			[&](tref f) -> std::optional<eval_t> {
				const tau& t = tau::get(f);
				if (t.is(tau::wff) && (t.child_is(tau::wff_and) || t.child_is(tau::wff_or))) {
					auto a = compile(lhs(f)), b = compile(rhs(f));
					if (!a || !b) return std::nullopt;
					const bool is_and = t.child_is(tau::wff_and);
					return eval_t([a, b, is_and](uint32_t m) {
						return is_and ? ((*a)(m) && (*b)(m)) : ((*a)(m) || (*b)(m)); });
				}
				if (t.equals_T()) return eval_t([](uint32_t) { return true; });
				if (t.equals_F()) return eval_t([](uint32_t) { return false; });
				if (!is_atomic_fm<node>(f)
					&& !(t.is(tau::wff) && t.child_is(tau::wff_neg))) return std::nullopt;
				auto l = atom_of(f);
				if (!l) return std::nullopt;
				const size_t v = l->v; const bool neg = l->neg;
				return eval_t([v, neg](uint32_t m) {
					const bool b = (m >> v) & 1u; return neg ? !b : b; });
			};
		std::vector<eval_t> fs;
		for (tref D : Ds) {
			auto c = compile(D);
			if (!c) return false;   // not decided: not refuted
			fs.push_back(*c);
		}
		const uint32_t n = static_cast<uint32_t>(atoms.size());
		for (uint32_t m = 0; m < (1u << n); ++m) {
			bool all = true;
			for (auto& f : fs) if (!f(m)) { all = false; break; }
			if (all) return false;   // a satisfying assignment: not refuted
		}
		return true;
	};
	auto prop_unsat = [&](const std::vector<tref>& Ds) -> bool {
		for (tref D : Ds) if (!D) return true;   // a bare equation always forces
		tref key = and_of(Ds);
		if (auto it = unsat_memo.find(key); it != unsat_memo.end()) return it->second;
		return unsat_memo[key] = prop_unsat_raw(Ds);
	};
	struct def {
		tref scope;                   // the block scope the definition sits in
		std::vector<tref> consumed;   // its conjuncts that hold the defining clauses
		std::vector<dclause> defs;    // the chosen clauses D_i || x = c_i
		std::vector<tref> readers;    // every other piece mentioning x
	};
	// Cache per (var, scope): a scope changes its tref once something was
	// substituted in it.
	std::map<std::pair<tref, tref>, std::optional<def>> def_cache;
	auto find_def_in_scope = [&](tref var, const binder& b, def& d) -> bool {
		const std::pair<tref, tref> key{ var, b.scope };
		if (auto it = def_cache.find(key); it != def_cache.end()) {
			if (it->second) { d = *it->second; return true; }
			return false;
		}
		std::vector<tref> cs; conjuncts(b.scope, cs);
		def cand{ b.scope, {}, {}, {} };
		for (tref c : cs) {
			if (!contains<node>(c, var)) continue;
			std::vector<std::vector<tref>> flat;
			if (!flatten(var, c, flat)) { cand.readers.push_back(c); continue; }
			std::vector<dclause> cdefs; std::vector<tref> creaders;
			for (auto& ds : flat) {
				dclause dc;
				if (classify(var, ds, dc)) cdefs.push_back(dc);
				else creaders.push_back(or_of(ds));
			}
			// A pure reader clause stays as it is.
			if (cdefs.empty()) { cand.readers.push_back(c); continue; }
			cand.consumed.push_back(c);
			cand.defs.insert(cand.defs.end(), cdefs.begin(), cdefs.end());
			cand.readers.insert(cand.readers.end(), creaders.begin(), creaders.end());
		}
		if (cand.defs.empty()) { def_cache[key] = std::nullopt; return false; }
		// (1) merge clauses with the same witness c
		struct merged { tref D; tref c; tref cbf; std::vector<tref> origs; bool dfalse; };
		std::vector<merged> ms; subtree_unordered_map<node, size_t> by_c;
		for (const dclause& dc : cand.defs) {
			auto it = by_c.find(dc.cbf);
			if (it == by_c.end()) {
				by_c.emplace(dc.cbf, ms.size());
				ms.push_back({ dc.D, dc.c, dc.cbf, { dc.orig }, dc.D == nullptr });
				continue;
			}
			merged& m = ms[it->second]; m.origs.push_back(dc.orig);
			if (m.dfalse || dc.D == nullptr) { m.dfalse = true; m.D = nullptr; }
			else m.D = tau::build_wff_and(m.D, dc.D);
		}
		// (2) the smallest subset whose D-conjunction is unsatisfiable is the
		// total definition; every other clause is a reader
		std::vector<size_t> chosen;
		for (size_t i = 0; i < ms.size() && chosen.empty(); ++i)
			if (ms[i].dfalse) chosen = { i };
		const size_t n = ms.size();
		for (size_t s = 2; s <= std::min<size_t>(subset_cap(), n) && chosen.empty(); ++s) {
			std::vector<size_t> idx(s);
			for (size_t i = 0; i < s; ++i) idx[i] = i;
			while (true) {
				std::vector<tref> Ds;
				for (size_t i : idx) Ds.push_back(ms[i].D);
				if (prop_unsat(Ds)) { chosen = idx; break; }
				// next combination
				int k = static_cast<int>(s) - 1;
				while (k >= 0 && idx[k] == n - s + k) --k;
				if (k < 0) break;
				++idx[k];
				for (size_t j = k + 1; j < s; ++j) idx[j] = idx[j - 1] + 1;
			}
		}
		// no total definition in this scope
		if (chosen.empty()) { def_cache[key] = std::nullopt; return false; }
		d = def{ b.scope, cand.consumed, {}, cand.readers };
		std::vector<bool> is_chosen(n, false);
		for (size_t i : chosen) is_chosen[i] = true;
		for (size_t i = 0; i < n; ++i) {
			if (is_chosen[i]) d.defs.push_back({ ms[i].D, ms[i].c, ms[i].cbf, nullptr });
			else for (tref o : ms[i].origs) d.readers.push_back(o);
		}
		def_cache[key] = d;
		return true;
	};
	auto find_def = [&](tref var, const std::vector<binder>& chain, def& d) -> bool {
		for (const binder& b : chain) if (find_def_in_scope(var, b, d)) return true;
		return false;
	};
	// Declined binders, by node: a binder is retried only once the block
	// around it changed.
	subtree_set<node> skip;
	subtree_map<node, def> prev_defs;
	for (size_t round = 0; round < round_cap(); ++round) {
		auto chain = collect(root);
		// A definition found in the previous round stays valid while its
		// clauses are still top-level conjuncts of a block scope (by the
		// leaf choice no definition mentions the eliminated variable); its
		// readers are recomputed from the current scope.
		std::vector<std::pair<binder, def>> defs;
		for (const binder& b : chain) {
			if (!is_bv_var(b.var) || skip.contains(b.bnode)) continue;
			bool reused = false;
			if (auto it = prev_defs.find(b.var);
				it != prev_defs.end() && !it->second.consumed.empty())
			{
				for (const binder& s : chain) {
					std::vector<tref> cs; conjuncts(s.scope, cs);
					bool all_in = true;
					for (tref c : it->second.consumed) {
						bool f = false;
						for (tref x : cs) if (x == c) { f = true; break; }
						if (!f) { all_in = false; break; }
					}
					if (!all_in) continue;
					def d = it->second; d.scope = s.scope; d.readers.clear();
					for (tref c : cs) {
						if (!contains<node>(c, b.var)) continue;
						bool consumed = false;
						for (tref k : d.consumed) if (k == c) { consumed = true; break; }
						if (!consumed) { d.readers.push_back(c); continue; }
						std::vector<std::vector<tref>> flat;
						if (!flatten(b.var, c, flat)) { d.readers.push_back(c); continue; }
						for (auto& ds : flat) {
							dclause dc;
							if (!classify(b.var, ds, dc)) d.readers.push_back(or_of(ds));
						}
					}
					defs.emplace_back(b, d); reused = true; break;
				}
			}
			if (reused) continue;
			def d;
			if (find_def(b.var, chain, d)) defs.emplace_back(b, d);
		}
		if (defs.empty()) break;
		prev_defs.clear();
		for (auto& pd : defs) prev_defs[pd.first.var] = pd.second;
		// Leaf first: a variable no other definition mentions.
		int pick = -1;
		for (size_t i = 0; i < defs.size() && pick < 0; ++i) {
			bool used = false;
			for (size_t j = 0; j < defs.size() && !used; ++j) {
				if (j == i) continue;
				for (const dclause& dc : defs[j].second.defs)
					if ((dc.D && contains<node>(dc.D, defs[i].first.var))
						|| contains<node>(dc.c, defs[i].first.var)) { used = true; break; }
			}
			if (!used) pick = static_cast<int>(i);
		}
		// every remaining definition mentions another one: nothing to do
		if (pick < 0) break;
		const binder bx = defs[pick].first;
		const def d = defs[pick].second;
		auto decline = [&](const char* why) {
			LOG_DEBUG << "[bv_definitional_elimination] " << why << ", keeping "
				<< LOG_FM(bx.var) << "\n";
			skip.insert(bx.bnode);
		};
		if (!(d.scope == bx.scope || contains<node>(bx.scope, d.scope))) {
			decline("the definition sits outside the variable's scope");
			continue;
		}
		if (d.scope != bx.scope) {
			// x must occur only inside the inner scope holding its definition.
			tref outside = along(bx.scope, [&](tref b) -> tref {
				return rhs(b) == d.scope ? tau::build_wff_ex(lhs(b), tau::_T(), false) : b; });
			if (contains<node>(outside, bx.var)) {
				decline("the variable occurs outside the scope of its definition");
				continue;
			}
		}
		bool captured = false;
		for (const dclause& dc : d.defs) {
			auto tv = tau::get(dc.c).select_all(is_var_or_capture<node>());
			subtree_set<node> term_vars(tv.begin(), tv.end());
			if (tau::get(d.scope).find_top([&](tref n) {
				return is_logical_or_functional_quant<node>(n)
					&& term_vars.contains(tau::get(n).child(0)); })) { captured = true; break; }
		}
		if (captured) { decline("a witness would be captured by a binder in the scope"); continue; }
		// rest: the conjuncts of the scope free of x; psi: the readers
		std::vector<tref> cs; conjuncts(d.scope, cs);
		std::vector<tref> rest;
		for (tref c : cs) if (!contains<node>(c, bx.var)) rest.push_back(c);
		const std::vector<tref>& psi = d.readers;
		auto subst = [&](tref body, tref w) -> tref {
			tref var = bx.var;
			auto query = [&var](tref n) -> bool {
				if (is<node>(n, tau::wff_all) || is<node>(n, tau::wff_ex))
					if (tau::get(n).child(0) == var) return false;
				return true;
			};
			return contains<node>(body, var)
				? tau::reget(rewriter::replace_if<node>(body, var, w, query)) : body;
		};
		// Exclusive cells: two clauses never force x to different values at
		// once when D_i || D_j is propositionally valid; the consistency
		// atoms are then not needed. The chosen subset is exhaustive by
		// construction, so "no clause forces x" never happens.
		bool exclusive = true;
		for (size_t i = 0; i < d.defs.size() && exclusive; ++i)
			for (size_t j = i + 1; j < d.defs.size() && exclusive; ++j) {
				if (tau::get(d.defs[i].c) == tau::get(d.defs[j].c)) continue;
				// D = F: that equation always forces
				if (!d.defs[i].D || !d.defs[j].D) { exclusive = false; break; }
				std::vector<tref> both{
					to_nnf<node>(tau::build_wff_neg(d.defs[i].D)),
					to_nnf<node>(tau::build_wff_neg(d.defs[j].D)) };
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
				alts.push_back(parts.empty() ? tau::_T() : and_of(parts));
			}
			s2 = or_of(alts);
		}
		if (!rest.empty()) { rest.push_back(s2); s2 = tau::build_wff_and(rest); }
		// One pass along the chain: the scope holding the definition becomes
		// s2, and x's own binder is dropped (x occurs nowhere any more).
		tref before = root;
		std::function<tref(tref)> rewrite = [&](tref b) -> tref {
			if (b == bx.bnode)
				return rhs(b) == d.scope ? s2 : along(rhs(b), rewrite);
			if (rhs(b) == d.scope) return tau::build_wff_ex(lhs(b), s2, false);
			return b;
		};
		root = along(root, rewrite);
		if (root == before) { decline("the scope was not rewritten"); continue; }
	}
	return root;
}

template <NodeType node>
tref bv_eliminate_definitional_existentials(tref fm) {
	using tau = tree<node>;
	auto block = [](tref n) -> tref {
		if (!is_child<node>(n, tau::wff_ex)) return n;
		return bv_definitional_block_elimination<node>(n);
	};
	return pre_order<node>(fm).apply_unique(block);
}

} // namespace idni::tau_lang
