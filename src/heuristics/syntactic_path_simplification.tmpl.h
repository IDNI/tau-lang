// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "syntactic_path_simplification"

namespace idni::tau_lang {

// Forward declaration: defined in normal_forms.tmpl.h, included after this
// file. This is the first declaration the compiler sees, so the default of
// `rewrite_neq` lives here. The main entry passes `false`: the sweep files a
// negated equality under its equality's key in every spelling, so `!=`
// needs no rewrite on the way in and nothing to restore on the way out.
template <NodeType node, bool rewrite_neq = true>
tref normalize_atomic_formula_operators(tref fm);

// ── Internal helpers ─────────────────────────────────────────────────────────

/**
 * @internal
 * @brief Strict ordering used to sort conjuncts: equality atoms (`bf_eq`)
 * sort before non-equality atoms; among same-kind atoms, `subtree_less`
 * breaks ties.
 * @tparam node Tree node type.
 * @param l Left atom.
 * @param r Right atom.
 * @return `true` if @p l strictly precedes @p r.
 *
 * @par Example
 * @code{.cpp}
 * // "x = 0" (equality) precedes "y != 0" (non-equality) (see
 * // tests/integration/test_integration-heuristics-syntactic_path_simplification.cpp:12-17).
 * tref eq_atm  = get_nso_rr("x = 0.").value().main->get();
 * tref neq_atm = get_nso_rr("y != 0.").value().main->get();
 * CHECK( syntactic_path_simplification_wff_comp<node_t>(eq_atm, neq_atm) == true );
 * CHECK( syntactic_path_simplification_wff_comp<node_t>(neq_atm, eq_atm) == false );
 * @endcode
 * @endinternal
 */
template <NodeType node>
bool syntactic_path_simplification_wff_comp(tref l, tref r) {
	using tau = tree<node>;
	if (tau::get(l).child_is(tau::bf_eq))
		if (tau::get(r).child_is(tau::bf_eq))
			return tau::subtree_less(l,r);
		else return true;
	else {
		if (tau::get(r).child_is(tau::bf_eq))
			return false;
		else return tau::subtree_less(l, r);
	}
}

// ── The path sweep ───────────────────────────────────────────────────────────
//
// One pre-order traversal carrying a scoped environment of literal keys. At a
// conjunction top the literal conjuncts are pushed as keys (l -> T, !m -> m:F)
// before the descent; every node then costs one environment lookup, a hit
// returns the key's value without descending. Keys are popped in `up`. A
// literal conjunct's position returns T from `up` so the rest of the spine
// folds through the hooks, and its processed form is re-emitted in a sorted
// block conjoined with the rest -- the same output shape the eager
// substitution produced, at one visit per node.
//
// The traversal is pre_order::apply (side effects allowed, `up` fires for
// every node `f` was applied to). apply_unique cannot be used: its memo key
// cannot carry the environment and a memo hit skips `up`. `f` refuses the
// descent through a one-shot flag consumed by the immediately following
// `visit` call -- the convention the former `skip` used, without its
// structural matching.
//
// Results are memoised on (node, the keys in force that can occur in it),
// the occurrence test being a 256-bit atom fingerprint cached per node
// (`path_bits_of`). A node no key can reach has an empty signature, and its
// result lives in a cache shared across calls: the root of every call, and
// every subtree a near-repeat call left untouched, cost one lookup.

/**
 * @brief Modes of the path sweep.
 */
struct path_sweep_options {
	/// Open disjunction frames too: a disjunct is assumed false in its
	/// siblings and a tautological disjunction folds to T (stage 2).
	bool tautologies = false;
	/// Treat every binder body as a leaf (the spec's §4 unit rule for
	/// layer 1). By default binder bodies are entered under the keys in
	/// force, with the capture guard.
	bool units_opaque = false;
};

/**
 * @internal
 * @brief Level traits of the sweep: the formula level (`wff`) and the term
 * level (`bf`) differ only in node kinds, constants, builders and order.
 */
/// A literal's canonical positive key and whether the literal is its
/// complement.
struct canon_t { tref key; bool flipped; };

/// A 256-bit Bloom fingerprint of the atoms under a node: one bit per
/// canonical atom key. A key can occur in a subtree only if its bits are a
/// subset of the subtree's; the converse may fail (a false positive costs a
/// descent, never a wrong result).
struct path_bits {
	std::array<uint64_t, 4> w{};
	void set(size_t h) { w[(h >> 6) & 3] |= uint64_t(1) << (h & 63); }
	void merge(const path_bits& o) { for (size_t i = 0; i < 4; ++i) w[i] |= o.w[i]; }
	bool covers(const path_bits& k) const {
		for (size_t i = 0; i < 4; ++i) if (k.w[i] & ~w[i]) return false;
		return true;
	}
	bool any() const { return w[0] | w[1] | w[2] | w[3]; }
};

template <NodeType node, bool is_wff> struct path_sweep_level;

template <NodeType node>
struct path_sweep_level<node, true> {
	using tau = tree<node>;
	static constexpr auto top  = tau::wff;
	static constexpr auto land = tau::wff_and;
	static constexpr auto lor  = tau::wff_or;
	static constexpr auto lneg = tau::wff_neg;
	static bool is_or_like(const tau& c) { return c.is(tau::wff_or); }
	static bool is_binder(const tau& c) {
		return c.is(tau::wff_ex) || c.is(tau::wff_all);
	}
	/// Connectives the sweep descends through; everything else is a leaf
	/// (atoms, references, temporal operators, constants -- rulings 1, 2).
	static bool is_connective(const tau& c) {
		return c.is(tau::wff_and) || c.is(tau::wff_or)
			|| c.is(tau::wff_neg) || is_binder(c)
			|| c.is(tau::wff_imply) || c.is(tau::wff_rimply)
			|| c.is(tau::wff_equiv) || c.is(tau::wff_xor)
			|| c.is(tau::wff_conditional);
	}
	static bool is_true(tref n)  { return tau::get(n).equals_T(); }
	static bool is_false(tref n) { return tau::get(n).equals_F(); }
	static tref constant(bool v, tref) { return v ? _T<node>() : _F<node>(); }
	static tref join(bool conj, tref l, tref r) {
		return conj ? tau::build_wff_and(l, r) : tau::build_wff_or(l, r);
	}
	static tref join_all(bool conj, const trefs& v, tref) {
		return conj ? tau::build_wff_and(v) : tau::build_wff_or(v);
	}
	static bool visit(tref n) { return is_formula<node>(n); }
	static bool less(tref l, tref r) {
		return syntactic_path_simplification_wff_comp<node>(l, r);
	}
	/// A wrapper the fingerprint counts as one atom: any leaf the sweep
	/// does not descend through.
	static bool is_atom_shaped(const tau& t) {
		return t.is(top) && t.has_child() && !is_connective(t[0]);
	}
	/// The key a literal is filed under and whether the literal is that
	/// key's complement: `!(l)` toggles; `l != r` is the complement of
	/// `l = r`; every ordering spelling is a `<` atom or its complement,
	/// by the total-order identities the atom normalisation already
	/// relies on (`a <= b` is `!(b < a)`, `a > b` is `b < a`, ...).
	static canon_t canon(tref n) {
		const tau& t = tau::get(n);
		if (!t.is(tau::wff)) return {n, false};
		const tau& c = t[0];
		switch (c.value.nt) {
		case tau::wff_neg: {
			const canon_t in = canon(c.first());
			return {in.key, !in.flipped};
		}
		case tau::bf_neq:
			return {tau::build_bf_eq(c.first(), c.second()), true};
		case tau::bf_lteq:
			return {tau::build_bf_lt(c.second(), c.first()), true};
		case tau::bf_nlt:
			return {tau::build_bf_lt(c.first(), c.second()), true};
		case tau::bf_nlteq:
			return {tau::build_bf_lt(c.second(), c.first()), false};
		case tau::bf_gt:
			return {tau::build_bf_lt(c.second(), c.first()), false};
		case tau::bf_gteq:
			return {tau::build_bf_lt(c.first(), c.second()), true};
		case tau::bf_ngt:
			return {tau::build_bf_lt(c.second(), c.first()), true};
		case tau::bf_ngteq:
			return {tau::build_bf_lt(c.first(), c.second()), false};
		default: return {n, false};
		}
	}
};

template <NodeType node>
struct path_sweep_level<node, false> {
	using tau = tree<node>;
	static constexpr auto top  = tau::bf;
	static constexpr auto land = tau::bf_and;
	static constexpr auto lor  = tau::bf_or;
	static constexpr auto lneg = tau::bf_neg;
	static bool is_or_like(const tau& c) {
		return c.is(tau::bf_or) || c.is(tau::bf_xor);
	}
	static bool is_binder(const tau& c) {
		return c.is(tau::bf_fex) || c.is(tau::bf_fall);
	}
	static bool is_connective(const tau& c) {
		return c.is(tau::bf_and) || c.is(tau::bf_or) || c.is(tau::bf_xor)
			|| c.is(tau::bf_neg) || is_binder(c);
	}
	static bool is_true(tref n)  { return tau::get(n).equals_1(); }
	static bool is_false(tref n) { return tau::get(n).equals_0(); }
	static tref constant(bool v, tref like) {
		const size_t t = find_ba_type<node>(like);
		return v ? _1<node>(t) : _0<node>(t);
	}
	static tref join(bool conj, tref l, tref r) {
		return conj ? tau::build_bf_and(l, r) : tau::build_bf_or(l, r);
	}
	static tref join_all(bool conj, const trefs& v, tref like) {
		const size_t t = find_ba_type<node>(like);
		return conj ? tau::build_bf_and(v, t) : tau::build_bf_or(v, t);
	}
	static bool visit(tref n) {
		return is_boolean_operation<node>(n) || is<node, tau::bf>(n);
	}
	static bool less(tref l, tref r) { return tau::subtree_less(l, r); }
	static bool is_atom_shaped(const tau& t) {
		return t.is(top) && t.has_child() && !is_connective(t[0]);
	}
	/// A term literal has one negative spelling, `t'`.
	static canon_t canon(tref n) {
		const tau& t = tau::get(n);
		if (!t.is(tau::bf) || !t[0].is(tau::bf_neg)) return {n, false};
		const canon_t in = canon(t[0].first());
		return {in.key, !in.flipped};
	}
};

/**
 * @internal
 * @brief The fingerprint of a subtree, cached per node across calls (GC-swept).
 * Computed bottom-up: an atom-shaped wrapper contributes the bit of its
 * canonical key, an inner node the union of its children. The walk prunes at
 * every node already cached, so a formula is fingerprinted once and each new
 * node built later costs its own subtree once.
 * @endinternal
 */
template <NodeType node, bool is_wff>
const path_bits& path_bits_of(tref n) {
	using tau = tree<node>;
	using L = path_sweep_level<node, is_wff>;
	using cache_t = subtree_unordered_map<node, path_bits>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	if (auto it = cache.find(n); it != cache.end()) return it->second;
	auto visit_subtree = [](tref m) {
		if (cache.contains(m)) return false;           // prune: known
		const tau& t = tau::get(m);
		return t.is(L::top) || is_boolean_operation<node>(m)
			|| !t.is_term() || L::is_atom_shaped(t);
	};
	auto visit = [](tref) { return true; };
	auto up = [](tref m) {
		const tau& t = tau::get(m);
		path_bits b;
		// A constant is never a key, so it contributes no bit.
		if (L::is_atom_shaped(t) && !L::is_true(m) && !L::is_false(m))
			b.set(hash_lcrs_tref<node>{}(L::canon(m).key));
		else for (tref c : t.children())
			if (auto it = cache.find(c); it != cache.end()) b.merge(it->second);
		cache.emplace(m, b);
	};
	pre_order<node>(n).visit(visit, visit_subtree, up);
	return cache.find(n)->second;
}

/**
 * @internal
 * @brief The sweep's state: environment, frames, per-node markers, memo.
 */
template <NodeType node, bool is_wff>
struct path_sweep {
	using tau = tree<node>;
	using L = path_sweep_level<node, is_wff>;

	explicit path_sweep(path_sweep_options o) : opts(o) {}

	tref run(tref root) {
		DBG(assert(tau::use_hooks);) // every fold below is a construction hook
		tref res = sweep(root);
		DBG(assert(markers.empty() && frames.empty() && undo.empty());)
		return res;
	}

private:
	// ── environment ───────────────────────────────────────────────────
	struct entry { bool value; unsigned suspended; path_bits bits; };
	struct undo_record { tref key; bool had_previous; entry previous; };
	subtree_unordered_map<node, entry> keys;
	std::vector<undo_record> undo;
	uint64_t var_bits = 0;      // variables mentioned by keys in force

	const entry* find_active(tref n) const {
		auto it = keys.find(n);
		if (it == keys.end() || it->second.suspended) return nullptr;
		return &it->second;
	}
	void push_key(tref key, bool value) {
		const entry e{value, 0, path_bits_of<node, is_wff>(key)};
		auto [it, fresh] = keys.try_emplace(key, e);
		undo.push_back({key, !fresh, fresh ? entry{} : it->second});
		if (!fresh) it->second = e;
		for (tref v : get_free_vars<node>(key)) var_bits |= var_bit(v);
	}
	void pop_keys_to(size_t mark) {
		while (undo.size() > mark) {
			const undo_record& u = undo.back();
			if (u.had_previous) keys[u.key] = u.previous;
			else keys.erase(u.key);
			undo.pop_back();
		}
	}
	static uint64_t var_bit(tref v) {
		return uint64_t(1) << (hash_lcrs_tref<node>{}(v) & 63);
	}
	static bool mentions(tref key, tref var) {
		const trefs& fv = get_free_vars<node>(key);
		return std::binary_search(fv.begin(), fv.end(), var,
						tau::subtree_less);
	}

	// ── frames ────────────────────────────────────────────────────────
	struct frame {
		enum kind_t : uint8_t { join, scope } kind;
		explicit frame(kind_t k) : kind(k) {}
		bool conj = true;               // join: conjunction or disjunction
		size_t undo_mark = 0;
		uint64_t var_bits_before = 0;
		std::unordered_set<tref> literal_positions; // join
		std::unordered_set<tref> spine;             // join: inner wrappers
		subtree_unordered_map<node, bool> pushed;   // join: its keys
		trefs literal_results;                      // join
		trefs suspended;                            // scope
	};
	std::vector<frame> frames;

	frame* innermost_join() {
		if (frames.empty() || frames.back().kind != frame::join) return nullptr;
		return &frames.back();
	}
	void open_scope(trefs suspended) {
		frame fr(frame::scope);
		fr.suspended = std::move(suspended);
		for (tref k : fr.suspended) ++keys.find(k)->second.suspended;
		frames.push_back(std::move(fr));
	}
	void close_scope() {
		frame& fr = frames.back();
		DBG(assert(fr.kind == frame::scope);)
		for (tref k : fr.suspended) --keys.find(k)->second.suspended;
		frames.pop_back();
	}
	/// Keys in force whose free variables contain `var` (a binder's
	/// variable): they must not reach into the binder's body.
	trefs captured_by(tref var) {
		trefs out;
		if (!(var_bits & var_bit(var))) return out;
		for (const auto& [k, e] : keys)
			if (!e.suspended && mentions(k, var)) out.push_back(k);
		return out;
	}

	// ── per-node markers ──────────────────────────────────────────────
	enum kind_t : uint8_t {
		plain,       // descend; rebuild; post-check; memo
		final_,      // no descent, result is final
		spine,       // inner spine wrapper of the innermost join
		join_top,    // opened a join frame
		binder_,     // opened a scope frame (capture guard)
		literal_pos, // a literal conjunct's position; may have opened a scope
	};
	/// The keys in force that can occur in a node: those whose fingerprint
	/// the node's covers, sorted so equal sets compare equal. Empty means
	/// the node's result is a property of the node alone.
	using relevant_t = std::vector<std::pair<tref, bool>>;
	struct marker {
		tref orig; kind_t kind; bool scoped;
		relevant_t relevant;   // set for kinds that memoise
	};
	std::vector<marker> markers;
	bool no_descend = false;

	relevant_t relevant_keys(tref n) const {
		relevant_t out;
		if (undo.empty()) return out;
		const path_bits& nb = path_bits_of<node, is_wff>(n);
		if (!nb.any()) return out;
		for (const auto& [k, e] : keys)
			if (!e.suspended && nb.covers(e.bits)) out.emplace_back(k, e.value);
		std::sort(out.begin(), out.end(), [](const auto& a, const auto& b) {
			return tau::subtree_less(a.first, b.first);
		});
		return out;
	}

	tref finish(marker m, tref result) {
		m.kind = final_;
		markers.push_back(m);
		no_descend = true;
		return result;
	}

	// ── memo: (node, relevant keys) -> result ─────────────────────────
	// Per call for a non-empty relevant set; across calls (GC-swept,
	// TAU_CACHE-gated) for the empty one, where the result depends on the
	// node alone -- at the root of every call this is the entry cache, and
	// at any subtree none of the keys in force can reach it is what makes
	// a near-repeat call cost only its touched paths.
	struct memo_key { tref orig; relevant_t relevant; };
	struct memo_hash {
		size_t operator()(const memo_key& k) const {
			size_t h = hash_lcrs_tref<node>{}(k.orig);
			for (const auto& [key, v] : k.relevant)
				h ^= (hash_lcrs_tref<node>{}(key) + (v ? 0x9e3779b97f4a7c15ULL : 0))
					+ 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
			return h;
		}
	};
	struct memo_equal {
		bool operator()(const memo_key& a, const memo_key& b) const {
			if (!tau::subtree_equals(a.orig, b.orig)) return false;
			if (a.relevant.size() != b.relevant.size()) return false;
			for (size_t i = 0; i < a.relevant.size(); ++i)
				if (a.relevant[i].second != b.relevant[i].second
					|| !tau::subtree_equals(a.relevant[i].first,
								b.relevant[i].first))
					return false;
			return true;
		}
	};
	std::unordered_map<memo_key, tref, memo_hash, memo_equal> memo;

	/// The cross-call cache of env-independent results, one per mode.
	static subtree_unordered_map<node, tref>* global_memo(path_sweep_options o) {
#ifdef TAU_CACHE
		using cache_t = subtree_unordered_map<node, tref>;
		static cache_t& c00 = tau::template create_cache<cache_t>();
		static cache_t& c01 = tau::template create_cache<cache_t>();
		static cache_t& c10 = tau::template create_cache<cache_t>();
		static cache_t& c11 = tau::template create_cache<cache_t>();
		return o.tautologies ? (o.units_opaque ? &c11 : &c10)
				     : (o.units_opaque ? &c01 : &c00);
#else
		(void)o;
		return nullptr;
#endif // TAU_CACHE
	}

	const tref* memo_find(const marker& m) const {
		if (m.relevant.empty()) {
			if (auto* g = global_memo(opts)) if (auto it = g->find(m.orig); it != g->end())
				return &it->second;
			return nullptr;
		}
		if (auto it = memo.find(memo_key{m.orig, m.relevant}); it != memo.end())
			return &it->second;
		return nullptr;
	}
	void memo_store(const marker& m, tref res) {
		if (m.relevant.empty()) {
			// Stored trimmed: a value shared across calls must not carry
			// the sibling of the position that first produced it.
			if (auto* g = global_memo(opts))
				g->emplace(m.orig, tau::trim_right_sibling(res));
			return;
		}
		memo.emplace(memo_key{m.orig, m.relevant}, res);
	}

	const path_sweep_options opts;

	/// One traversal of `n` under the keys currently in force. Re-entrant:
	/// a join whose rebuilt form exposes a literal that was not a key
	/// re-sweeps itself from `up` (see `exposes_new_literal`), on top of
	/// the frames and markers of the traversal in progress.
	tref sweep(tref n) {
		auto f = [this](tref m) { return down(m); };
		auto visit = [this](tref m) {
			if (no_descend) return no_descend = false, false;
			return L::visit(m);
		};
		auto up = [this](tref r) { return this->up(r); };
		return pre_order<node>(n).apply(f, visit, up);
	}

	/// A binder over a constant body is that constant: ∃x.T ≡ T, ∃x.F ≡ F,
	/// and the same for ∀ and the term-level `fex`/`fall`. The hooks do
	/// not fold this shape, and the sweep produces it whenever a body
	/// collapses under the keys in force.
	static tref fold_binder(tref r) {
		const tau& t = tau::get(r);
		if (!t.is(L::top) || !L::is_binder(t[0])) return r;
		tref body = t[0].second();
		if (L::is_true(body) || L::is_false(body)) return body;
		return r;
	}

	/// Whether the rebuilt join `res` has a top-level literal that was not
	/// a key of the frame that produced it: a disjunctive member that
	/// folded to a literal under the keys, or a compound literal whose
	/// processed form is a new literal. Such a literal is a key for its
	/// siblings, which were processed without it -- the eager substitution
	/// found it by re-descending into the substituted rest; the sweep
	/// re-sweeps the join, which pushes it and terminates since keys
	/// only grow along the chain.
	bool exposes_new_literal(tref res, const frame& fr) const {
		trefs leaves;
		get_leaves<node>(res, fr.conj ? L::land : L::lor, leaves);
		for (tref l : leaves) {
			const tau& lt = tau::get(l);
			if (!lt.is(L::top)) continue;
			const tau& lc = lt[0];
			if (lc.is(L::land) || L::is_or_like(lc)) continue;
			if (L::is_true(l) || L::is_false(l)) continue;
			const canon_t k = L::canon(l);
			const tref key = k.key;
			const bool value = fr.conj != k.flipped;
			if (auto it = fr.pushed.find(key);
				it != fr.pushed.end() && it->second == value) continue;
			if (const entry* e = find_active(key); e && e->value == value)
				continue;
			return true;
		}
		return false;
	}

	// ── down ──────────────────────────────────────────────────────────
	tref down(tref n) {
		DBG(assert(!no_descend);)
		const tau& t = tau::get(n);
		marker m{n, plain, false, {}};
		if (!t.is(L::top)) return markers.push_back(m), n;
		if (frame* fr = innermost_join()) {
			if (fr->literal_positions.contains(n)) return enter_literal(n, m);
			if (fr->spine.contains(n)) {
				m.kind = spine;
				return markers.push_back(m), n;
			}
		}
		if (const canon_t k = L::canon(n); const entry* e = find_active(k.key))
			return finish(m, L::constant(e->value != k.flipped, n));
		m.relevant = relevant_keys(n);
		if (const tref* hit = memo_find(m)) return finish(m, *hit);
		const tau& c = t[0];
		if (c.is(L::land)) return open_join(n, m, true);
		if (opts.tautologies && c.is(L::lor)) return open_join(n, m, false);
		if (L::is_binder(c)) return enter_binder(n, m);
		if (!L::is_connective(c)) return finish(m, n);
		return markers.push_back(m), n;
	}

	tref open_join(tref n, marker m, bool conj) {
		frame fr(frame::join);
		fr.conj = conj;
		fr.undo_mark = undo.size();
		fr.var_bits_before = var_bits;
		trefs leaves, inner;
		get_leaves<node>(n, conj ? L::land : L::lor, leaves, &inner);
		for (tref l : leaves) {
			const tau& lt = tau::get(l);
			if (!lt.is(L::top)) continue;
			const tau& lc = lt[0];
			if (lc.is(L::land) || L::is_or_like(lc)) continue;
			if (L::is_true(l) || L::is_false(l)) continue;
			const canon_t k = L::canon(l);
			const tref key = k.key;
			const bool value = conj != k.flipped;
			if (const entry* e = find_active(key)) {
				if (e->value == value) continue; // redundant
				// A contradiction (conjunction) or a tautology
				// (disjunction): the join is decided here.
				pop_keys_to(fr.undo_mark);
				var_bits = fr.var_bits_before;
				return finish(m, L::constant(!conj, n));
			}
			push_key(key, value);
			fr.pushed.emplace(key, value);
			fr.literal_positions.insert(l);
		}
		for (tref s : inner)
			if (tau::get(s).is(L::top)) fr.spine.insert(s);
		frames.push_back(std::move(fr));
		m.kind = join_top;
		return markers.push_back(m), n;
	}

	tref enter_binder(tref n, marker m) {
		if (opts.units_opaque) return finish(m, n);
		trefs captured = captured_by(tau::get(n)[0].first());
		if (captured.empty()) return markers.push_back(m), n;
		open_scope(std::move(captured));
		m.kind = binder_; m.scoped = true;
		return markers.push_back(m), n;
	}

	tref enter_literal(tref n, marker m) {
		m.kind = literal_pos;
		const tau& c = tau::get(n)[0];
		const bool negated = c.is(L::lneg);
		// The node under the literal's own wrapper: the atom, binder or
		// connective that the (possibly negated) literal is made of.
		const tau& inner = negated ? c[0][0] : c;
		// An atom-shaped literal is re-emitted verbatim: nothing inside it
		// can hold a key, and its own key must not fire on itself.
		if (!L::is_connective(inner)) return finish_literal(m, n);
		if (L::is_binder(inner) && opts.units_opaque) return finish_literal(m, n);
		// Compound literal: enter under the keys in force, its own key
		// suspended so it cannot fire on its own subtree; a binder also
		// gets the capture guard.
		trefs suspended{L::canon(n).key};
		if (L::is_binder(inner))
			for (tref k : captured_by(inner.first()))
				if (!tau::subtree_equals(k, suspended[0]))
					suspended.push_back(k);
		open_scope(std::move(suspended));
		m.scoped = true;
		return markers.push_back(m), n;
	}

	/// A literal position whose subtree is not entered still reports its
	/// (verbatim) form through the literal path of `up`.
	tref finish_literal(marker m, tref n) {
		m.kind = literal_pos;
		m.scoped = false;
		markers.push_back(m);
		no_descend = true;
		return n;
	}

	// ── up ────────────────────────────────────────────────────────────
	tref up(tref r) {
		marker m = markers.back();
		markers.pop_back();
		switch (m.kind) {
		case final_:
		case spine: return r;
		case plain: {
			tref res = post_check(fold_binder(r), m);
			if (tau::get(m.orig).is(L::top)) memo_store(m, res);
			return res;
		}
		case binder_: {
			close_scope();
			tref res = post_check(fold_binder(r), m);
			memo_store(m, res);
			return res;
		}
		case literal_pos: {
			if (m.scoped) close_scope();
			r = fold_binder(r);
			frame& fr = frames.back();
			DBG(assert(fr.kind == frame::join);)
			const bool conj = fr.conj;
			if (conj ? L::is_false(r) : L::is_true(r)) return r; // decides the join
			if (conj ? L::is_true(r) : L::is_false(r)) return r; // vanished
			fr.literal_results.push_back(r);
			return L::constant(conj, r); // the spine folds it away
		}
		case join_top: {
			frame fr = std::move(frames.back());
			frames.pop_back();
			pop_keys_to(fr.undo_mark);
			var_bits = fr.var_bits_before;
			const bool conj = fr.conj;
			tref res = r;
			if (!(conj ? L::is_false(r) : L::is_true(r))) {
				trefs& lits = fr.literal_results;
				std::ranges::stable_sort(lits, L::less);
				lits.erase(std::unique(lits.begin(), lits.end(),
					tau::subtree_equals), lits.end());
				tref block = L::join_all(conj, lits, m.orig);
				res = L::join(conj, block, r);
				if (exposes_new_literal(res, fr)) res = sweep(res);
			}
			res = post_check(res, m);
			memo_store(m, res);
			return res;
		}
		}
		return r;
	}

	/// A node that became equal to a key in force only after its subtree
	/// was rewritten still takes the key's value.
	tref post_check(tref r, const marker& m) const {
		if (r == m.orig) return r;
		if (const canon_t k = L::canon(r); const entry* e = find_active(k.key))
			return L::constant(e->value != k.flipped, r);
		return r;
	}
};

/**
 * @internal
 * @brief The formula-level path sweep: within every conjunction, each literal
 * conjunct is assumed (a bare conjunct true, a negated one false) in the rest
 * of the conjunction, and a conjunction whose assumptions contradict collapses
 * to `F`. One traversal, one environment lookup per node; see `path_sweep`.
 * @tparam node Tree node type.
 * @param root Formula to sweep.
 * @param opts Sweep modes (`path_sweep_options`).
 * @return @p root with contradictory conjunctions collapsed to `F`.
 *
 * @par Example
 * @code{.cpp}
 * // "x = 0 && !(x = 0)" collapses to F (see
 * // tests/integration/test_integration-heuristics-syntactic_path_simplification.cpp).
 * tref fm = get_nso_rr("x = 0 && !(x = 0).").value().main->get();
 * tref res = syntactic_path_simplification_simplify_wff<node_t>(fm);
 * CHECK( tau::get(res).equals_F() );
 * @endcode
 * @endinternal
 */
template <NodeType node>
tref syntactic_path_simplification_simplify_wff(tref root,
	path_sweep_options opts = {})
{
	return path_sweep<node, true>(opts).run(root);
}

/**
 * @internal
 * @brief The term-level twin of `syntactic_path_simplification_simplify_wff`:
 * within every `bf_and`, each factor is assumed (a bare factor `1`, a negated
 * one `0`) in the rest, and a contradictory conjunction collapses to `0`.
 * @tparam node Tree node type.
 * @param root Boolean term to sweep.
 * @param opts Sweep modes (`path_sweep_options`).
 * @return @p root with contradictory conjunctions collapsed to `0`.
 *
 * @par Example
 * @code{.cpp}
 * // "x & x'" collapses to 0.
 * tref fm = get_bf_nso_rr("", "x & x'").value().main->get();
 * tref res = syntactic_path_simplification_simplify_bf<node_t>(fm);
 * CHECK( tau::get(res).equals_0() );
 * @endcode
 * @endinternal
 */
template <NodeType node>
tref syntactic_path_simplification_simplify_bf(tref root,
	path_sweep_options opts = {})
{
	return path_sweep<node, false>(opts).run(root);
}

// ── Public functions ──────────────────────────────────────────────────────────

template <NodeType node>
tref syntactic_path_simplification(tref fm) {
	using tau = tree<node>;
	// No entry cache: the sweep's cross-call memo answers at the root
	// (empty key signature) with one lookup, and the passes before it
	// carry their own caches.
	DBG(LOG_DEBUG << "Syntactic_path_simplification on " << LOG_FM(fm) << "\n";)
	// One sweep with both kinds of assumption in force along every path: a
	// conjunct is true in its siblings, a disjunct false in its siblings. A
	// contradictory conjunction folds to F/0, a tautological disjunction to
	// T/1. It replaces the former contradiction sweep, negation, second
	// sweep and negation, and simplifies at least as much: every path saw
	// only one kind of assumption before.
	path_sweep_options opts;
	opts.tautologies = true;
	tref res = nullptr;
	if (tau::get(fm).is_term()) {
		if (tau::get(fm).equals_0() || tau::get(fm).equals_1()) return fm;
		res = syntactic_path_simplification_simplify_bf<node>(
			push_negation_in<node, false>(fm), opts);
	} else {
		if (tau::get(fm).equals_F() || tau::get(fm).equals_T()) return fm;
		res = syntactic_path_simplification_simplify_wff<node>(
			normalize_atomic_formula_operators<node, false>(to_nnf<node>(fm)),
			opts);
	}
	DBG(LOG_DEBUG << "Syntactic_path_simplification result: " << LOG_FM(res) << "\n";)
	return res;
}

template <NodeType node>
tref syntactic_path_simplification_unsat_on_unchanged_negations(tref fm) {
	using tau = tree<node>;
	// The entry cache measured on 2026-08-15 (satisfiability2's
	// mixed_lookback medians 11.23/12.88 s with it vs 11.51/16.09 s
	// without) is now the sweep's cross-call memo at the root: same one
	// lookup on a repeat, and every untouched subtree of a near-repeat
	// answers the same way.
	if (tau::get(fm).is_term()) {
		if (tau::get(fm).equals_0() || tau::get(fm).equals_1()) return fm;
		return syntactic_path_simplification_simplify_bf<node>(fm);
	}
	return syntactic_path_simplification_simplify_wff<node>(fm);
}

} // namespace idni::tau_lang
