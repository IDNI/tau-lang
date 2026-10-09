// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file terms.tmpl.h
 * @brief Template implementations for terms.h. Included by terms.h. terms.h
 * says what each function means; the comments here say how it is built.
 *
 * Conventions of the BDD library this file rides on (tau_bdd.h):
 *  - the library's universe map `U`, which links a `BDD_ID` node to its BDD,
 *    is keyed by the id that node carries (`key_of`), the same in every
 *    spelling of the `bf` wrapper. `convert_to_tau_node` interns
 *    (tau_bdd.tmpl.h), so one BDD per type is one Tau node.
 *  - a BDD leaf's variable (`get_var`, or `get_var_term` as a `bf` term) is
 *    the TRIMMED Tau node under `bf`, a `bf_neg` node for an inverted leaf.
 *    `add(leaf)` takes the trimmed node and maps `bf_t` / `bf_f` to the
 *    terminals.
 *  - the memoised BDD walks are the library's own (`visit_nodes`,
 *    `map_leaves`, `bdd_cofactor`): recursive over BDD references with a
 *    per-call memo, the shape of its own workers (`bdd_ex`,
 *    `bdd_compose_impl`). Walks over the TAU tree go through `pre_order`
 *    (dag.h, tree.h) instead.
 *  - `build_bdd` collects a functional-quantifier chain whole, puts it
 *    through the chain constructor and slides it to its subscripts' levels
 *    (§1). Nothing here prepares a term for that, and every caller hands it
 *    the term as written.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_TMPL_H__

// `syntactic_path_simplification` (heuristics/) is the sweep the two
// normalisers below reuse. The heuristics header must be included through
// normal_forms.h, which declares what its body uses before the body.
#include "normal_forms.h"

#include <cstdint>
#include <optional>

namespace idni::tau_lang::anti_prenexing {

namespace terms_detail {

template <NodeType node> using tbdd  = tau_term_bdd<node>;
template <NodeType node> using bref  = typename tau_term_bdd<node>::ref;
template <NodeType node> using thandle = term_handle<node>;

/// Does @p t carry a `BDD_ID` anywhere -- a stored BDD at the top, or one
/// held under an operator, a functional-quantifier chain or inside a leaf?
/// The BDD regime of the two normalisers below is keyed on it: nothing outside
/// this file knows a `BDD_ID`.
template <NodeType node>
bool holds_bdd_id(tref t) {
	using tau = tree<node>;
	return tau::get(t).find_top([](tref m) {
		return tau::get(m).is(tau::BDD_ID); }) != nullptr;
}

/// What the chain step hands back: the body's BDD quantified over the
/// subscripts it took, and the prefix that STAYS over it, outermost first. An
/// empty `kept` is a chain that resolved whole.
template <NodeType node>
struct chain_result {
	bref<node> b;
	typename tbdd<node>::quants kept;
};

/**
 * @brief THE CHAIN STEP: ONE quantification of the body's BDD @p s by those
 * subscripts of the canonical prefix @p q — outermost first — that the stuck
 * rule (terms.h) takes, one same-kind run at a time (`bdd_quant`).
 * `std::nullopt` says nothing was taken, which leaves the chain as it is.
 */
template <NodeType node>
std::optional<chain_result<node>> resolve_chain(
	const typename tbdd<node>::quants& q, bref<node> s)
{
	using tau = tree<node>;
	DBG(assert(!q.empty() && tbdd<node>::is_ordered(s));)
	// The subscripts no quantification reaches, read off the leaves.
	trefs lfv = thandle<node>::get_free_leaf_vars(s.b);
	std::vector<bool> hidden(q.size());
	for (size_t i = 0; i < q.size(); ++i)
		hidden[i] = std::binary_search(lfv.begin(), lfv.end(),
			q[i].first, tau::subtree_less);
	// THE STUCK RULE, innermost first: a hidden subscript opens a stuck run
	// of its kind or joins the open one, a subscript of the open run's kind
	// commutes into it and is taken, and one of the other kind ends the walk
	// with everything outward of it left standing.
	std::vector<bool> taken(q.size(), false);
	bool stuck = false;                        // is a stuck run open,
	quantifier<node> run = tbdd<node>::ex;     // and of which kind
	for (size_t i = q.size(); i-- > 0; ) {
		if (stuck && q[i].second != run) break;
		if (hidden[i]) stuck = true, run = q[i].second;
		else taken[i] = true;
	}
	// Both keep the prefix's own order, outermost first.
	typename tbdd<node>::quants e, kept;
	for (size_t i = 0; i < q.size(); ++i)
		(taken[i] ? e : kept).push_back(q[i]);
	if (e.empty()) return std::nullopt;
	return chain_result<node>{ tbdd<node>::bdd_quant(s, e), std::move(kept) };
}

/**
 * @brief `RESOLVE_FUNCTIONAL_PLAIN`'s worker: @p memo is shared with every
 * recursive call, so a body reached from two chains is resolved once per
 * public call.
 */
template <NodeType node>
tref resolve_plain_chains(tref n, const keep_functional_fn<node>& keep,
	subtree_unordered_map<node, tref>& memo)
{
	using tau = tree<node>;
	n = tau::trim_right_sibling(n);
	if (auto it = memo.find(n); it != memo.end()) return it->second;
	// What this pass produced: a chain `keep` accepted, a chain the stuck
	// rule left standing, and a quantified result. All of them are resolved
	// already — the traversal refuses to enter one, so a kept chain's inner
	// binders are never looked at.
	subtree_unordered_set<node> done;
	auto f = [&](tref m) -> tref {
		const tau& tm = tau::get(m);
		if (!tm.is(tau::bf)) return m;
		if (!tm.child_is(tau::bf_fall) && !tm.child_is(tau::bf_fex))
			return m;    // a chain may still sit deeper: descend
		// THE WHOLE NEST at once, its BODY resolved first — what comes
		// before this chain is what is NESTED inside it, never an
		// adjacent binder.
		auto [q0, b0] = strip_chain<node>(m);
		tref c = tbdd<node>::build_functional_quantifiers(q0,
			resolve_plain_chains<node>(b0, keep, memo));
		// Canonical now: a degenerate or shadowed subscript is gone,
		// each run is in content order, an adjoining run of the body is
		// merged in, and a closed plain chain is already folded. `keep`
		// sees that chain NODE, ONCE — prefix and body come off it with
		// `strip_chain` (§3) — and its yes keeps the whole chain.
		auto [q, body] = strip_chain<node>(c);
		if (q.empty()) return done.insert(c), c;
		if (keep(c)) return done.insert(c), c;
		// The body under the global order: nothing is BDD-backed here,
		// so there is nothing to spell out first. A chain of the body
		// that survived is SLID by the build below the outer
		// subscripts' levels, where their quantification crosses it. A
		// build that does not BRANCH has every subscript in a leaf and
		// settles the chain as it is.
		bref<node> b = tbdd<node>::build_bdd(body);
		if (tbdd<node>::leaf(b)) return done.insert(c), c;
		auto r = resolve_chain<node>(q, b);
		if (!r) return done.insert(c), c;
		// PLAIN again before the pass moves on: phase 1 is the plain
		// regime.
		tref e = tbdd<node>::to_tau_term(r->b, find_ba_type<node>(body));
		tref res = tbdd<node>::build_functional_quantifiers(r->kept, e);
		return done.insert(res), res;
	};
	auto visit = [&done](tref m) { return !done.contains(m); };
	tref r = pre_order<node>(n).apply_unique(f, visit);
	memo.emplace(n, r);
	return r;
}

/// §3 `SIMPLIFY_TERM`'s LEAF SIMPLIFIER, the function its BDD regime maps
/// over the leaves.
///
/// A leaf that holds no `BDD_ID` goes through the path sweep. A leaf that
/// holds one is swept over its PLAIN structure alone: to the sweep a
/// `bf(BDD_ID)` is an opaque leaf, never descended into, negated into or
/// substituted into. The stored BDDs the sweep left standing are
/// then simplified from the inside, one `map_leaves` each with this same
/// simplifier, so a nest of them settles innermost leaf first.
///
/// Results are memoised per leaf node: a leaf's simplified form is a function
/// of the leaf, and one leaf is reached from several BDDs.
template <NodeType node>
struct leaf_simplifier {
	tref operator()(tref leaf) {
		if (auto it = memo.find(leaf); it != memo.end())
			return it->second;
		tref r = simplify(leaf);
		memo.emplace(leaf, r);
		return r;
	}

private:
	subtree_unordered_map<node, tref> memo;

	tref simplify(tref leaf) {
		tref swept = syntactic_path_simplification<node>(leaf);
		if (!holds_bdd_id<node>(swept)) return swept;
		auto inside = [this](tref m) -> tref {
			if (!thandle<node>::is_bdd_backed(m)) return m;
			bref<node> b = thandle<node>::convert_to_handle(m).get();
			DBG(assert(tbdd<node>::is_ordered(b));)
			return thandle<node>::convert_to_tau_node_or_term(
				thandle<node>(tbdd<node>::map_leaves(b, *this)),
				find_ba_type<node>(m));
		};
		// A rebuilt stored BDD is not re-entered: its own leaves went
		// through this simplifier already.
		return pre_order<node>(swept).apply_unique_until_change(inside);
	}
};

/// Refutes the cofactor check on 64 content-derived assignments at once: the
/// hooks folding a cofactor are Boolean identities, so cofactors whose words
/// differ are never one node. A skeleton node the hooks interpret is refused.
template <NodeType node>
struct cofactor_refuter {
	explicit cofactor_refuter(tref t) {
		index_t at;
		root = walk(t, at);
		if (ok) w0.resize(steps.size()), w1.resize(steps.size());
	}

	/// Do `t[key←0]` and `t[key←1]` differ under one of the 64 assignments?
	/// `false` decides nothing.
	bool differ(tref key) {
		using tau = tree<node>;
		if (!ok) return false;
		const tau& k = tau::get(key);
		for (size_t i = 0; i < steps.size(); ++i) {
			const step& s = steps[i];
			switch (s.o) {
			case op::leaf:
				// The substitution's own occurrence test.
				if (s.n != nullptr && tau::get(s.n) == k)
					w0[i] = 0, w1[i] = ~uint64_t{0};
				else w0[i] = w1[i] = s.w;
				break;
			case op::conj: w0[i] = w0[s.a] & w0[s.b],
				w1[i] = w1[s.a] & w1[s.b]; break;
			case op::disj: w0[i] = w0[s.a] | w0[s.b],
				w1[i] = w1[s.a] | w1[s.b]; break;
			case op::sum: w0[i] = w0[s.a] ^ w0[s.b],
				w1[i] = w1[s.a] ^ w1[s.b]; break;
			case op::neg: w0[i] = ~w0[s.a], w1[i] = ~w1[s.a]; break;
			}
		}
		return w0[root] != w1[root];
	}

private:
	enum class op : unsigned char { leaf, conj, disj, sum, neg };
	// A leaf's `n` is set on a variable alone, the only leaf a key can be.
	struct step { op o; uint32_t a, b; uint64_t w; tref n; };
	// The step of each `bf` node met so far.
	using index_t = subtree_unordered_map<node, uint32_t>;
	std::vector<step> steps;
	std::vector<uint64_t> w0, w1;
	uint32_t root = 0;
	bool ok = true;

	// splitmix64's finaliser: the node hash alone spreads too few of its bits
	// to make 64 independent assignments. Taken from the atom under its `bf`.
	static uint64_t word(tref atom) {
		uint64_t z = hash_lcrs_tref<node>{}(atom);
		z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull;
		z = (z ^ (z >> 27)) * 0x94d049bb133111ebull;
		return z ^ (z >> 31);
	}

	/// Appends the steps of the term @p t and returns the step of `t`.
	uint32_t walk(tref t, index_t& at) {
		using tau = tree<node>;
		auto index = [&at](tref n) { return at.find(n)->second; };
		auto admit = [](tref n) {
			const tau& tn = tau::get(n);
			return tn.is(tau::bf) || tn.is(tau::bf_and)
				|| tn.is(tau::bf_or) || tn.is(tau::bf_xor)
				|| tn.is(tau::bf_neg);
		};
		auto visit = [this](tref n) {
			if (!ok) return false;
			const tau& tn = tau::get(n);
			if (!tn.is(tau::bf)) return true;
			switch (tau::get(tn.first()).get_type()) {
			case tau::bf_and: case tau::bf_or: case tau::bf_xor:
			case tau::bf_neg: case tau::variable: case tau::bf_ref:
			case tau::bf_t: case tau::bf_f:
				return true;
			default: return ok = false;
			}
		};
		auto up = [&](tref n) {
			const tau& tn = tau::get(n);
			if (!ok || !tn.is(tau::bf)) return;
			const tref ch = tn.first();
			const tau& c = tau::get(ch);
			step s{ op::leaf, 0, 0, 0, nullptr };
			switch (c.get_type()) {
			case tau::bf_and: s = { op::conj, index(c.first()),
				index(c.second()), 0, nullptr }; break;
			case tau::bf_or: s = { op::disj, index(c.first()),
				index(c.second()), 0, nullptr }; break;
			case tau::bf_xor: s = { op::sum, index(c.first()),
				index(c.second()), 0, nullptr }; break;
			case tau::bf_neg: s = { op::neg, index(c.first()), 0, 0,
				nullptr }; break;
			case tau::bf_t: s.w = ~uint64_t{0}; break;
			case tau::bf_f: break;
			case tau::variable: s.n = n, s.w = word(ch); break;
			default: s.w = word(ch);                  // a reference
			}
			if (at.try_emplace(n, static_cast<uint32_t>(
				steps.size())).second) steps.push_back(s);
		};
		pre_order<node>(t).search_unique(visit, admit, up);
		return ok ? index(t) : 0;
	}
};

/// §3 `SIMPLIFY_TERM`'s fourth law, the COFACTOR CHECK of the plain regime
/// (terms.h says what it means). The variables are read off the term handed
/// in and then tried ON THE RUNNING term, so the ones after a replacement are
/// tried on the smaller term. Equal cofactors, a constant included, make
/// `t ← f₀`, which covers being `0` under both bits, being `1` under both,
/// and not depending on `y` at all.
template <NodeType node>
tref cofactor_check(tref t) {
	using tau = tree<node>;
	// Copied rather than held: the walks below write to the cache this
	// list comes from.
	const trefs vars = get_free_vars<node>(t);
	// Built for the running term at its first candidate variable.
	std::optional<cofactor_refuter<node>> refuter;
	for (tref y : vars) {
		const tref key = tau::get(tau::bf, y);
		if (!refuter) refuter.emplace(t);
		if (refuter->differ(key)) continue;
		const size_t ba = find_ba_type<node>(y);
		const tref f0 = tau::get(t).substitute(key, _0<node>(ba),
			identity, while_is_boolean_operation<node>);
		const tref f1 = tau::get(t).substitute(key, _1<node>(ba),
			identity, while_is_boolean_operation<node>);
		if (f0 != f1) continue;
		if (f0 != t) refuter.reset();
		t = f0;
		if (tau::get(t).equals_0() || tau::get(t).equals_1()) break;
	}
	return t;
}

/// The atom under one optional `¬`, and whether there was one.
template <NodeType node>
std::pair<tref, bool> unwrap_neg(tref a) {
	using tau = tree<node>;
	if (tau::get(a).child_is(tau::wff_neg))
		return { tau::trim_right_sibling(tau::get(a)[0].first()), true };
	return { a, false };
}

} // namespace terms_detail

// --- the representation boundary (PREPARE_TERMS) ---------------------------------

template <NodeType node>
tref prepare_terms(tref body) {
	using tau = tree<node>;
	using namespace terms_detail;
	// Memoised per term, within this call only.
	subtree_unordered_map<node, tref> term_memo;
	auto prep = [&](tref t) -> tref {
		if (thandle<node>::is_bdd_backed(t)) { // already prepared: idempotent
			DBG(assert(tbdd<node>::is_ordered(
				thandle<node>::convert_to_handle(t).get()));)
			return t;
		}
		if (get_free_vars<node>(t).empty()) return t;
		if (auto it = term_memo.find(t); it != term_memo.end())
			return it->second;
		tref r = thandle<node>::convert_to_tau_node_or_term(
			thandle<node>(tbdd<node>::build_bdd(t)),
			find_ba_type<node>(t));
		return term_memo.emplace(t, r).first->second;
	};
	// Equations only: an order atom is never cofactored and is consumed
	// plain by the solver path, so it is left as written.
	auto f = [&](tref n) -> tref {
		const tau& tn = tau::get(n);
		if (!tn.is(tau::wff) || !tn.child_is(tau::bf_eq)) return n;
		tref l = tn[0].first();
		tref r = tn[0].second();
		tref l2 = prep(l), r2 = prep(r);
		if (l2 == l && r2 == r) return n;
		return build_bf_eq<node>(l2, r2);
	};
	// Descend through connectives and negation only: a binder, a
	// reference and a temporal operator are opaque.
	auto visit = [](tref n) {
		const tau& tn = tau::get(n);
		return tn.is(tau::wff) || tn.is(tau::wff_and) || tn.is(tau::wff_or)
			|| tn.is(tau::wff_neg);
	};
	return pre_order<node>(body).apply_unique(f, visit);
}

template <NodeType node>
tref finish_terms(tref phi) {
	using tau = tree<node>;
	using namespace terms_detail;
	DBG(assert(phi != nullptr);)
	// Nothing prepared, nothing to spell: the formula comes back as the
	// SAME node, its junctions untouched. One cached walk pays for it.
	if (!holds_bdd_id<node>(phi)) return phi;
	// An atom holding a stored BDD anywhere is spelled out by the library
	// and then normalised in the PLAIN regime, the spelling phase 1 gives
	// every atom. Everything else is handed back unlooked at.
	auto down = [](tref n) -> tref {
		if (!is_atomic_fm<node>(n) || !holds_bdd_id<node>(n)) return n;
		return simplify_atom<node>(
			thandle<node>::convert_to_tau_terms(n), regime::plain);
	};
	// A spelled atom may sort elsewhere among its siblings, and one that
	// folded to a constant decides the junction it stood in, so every ∧/∨
	// the walk closes is re-emitted through the canonical constructor over
	// its member view: content order, deduplicated, the hooks folding `T`,
	// `F`, a repeated member and a complement pair.
	//
	// THE RAW CONSTRUCTOR AND NOT THE RESULT JOINS, which are a normaliser
	// and may not be included from a foundation. The joins add one thing
	// to it, the absorption pass, and that finds nothing new here: the
	// members were already absorbed against each other, and spelling is
	// injective on atoms, so it cannot bring two of them into a
	// containment they were not already in.
	auto up = [](tref r) -> tref {
		if (is_child<node>(r, tau::wff_and))
			return canonical_and<node>(members<node>(r));
		if (is_child<node>(r, tau::wff_or))
			return canonical_or<node>(members<node>(r));
		return r;
	};
	// `while_is_formula` enters ∧, ∨, ¬, a re-wrapped binder and a temporal
	// operator, and stops at a term — which is where an atom's own `down`
	// has already done the work.
	return pre_order<node>(phi).apply_unique_pure(down,
		while_is_formula<node>, up);
}

// --- cofactors and quantification -----------------------------------------------

template <NodeType node>
tref cofactor(tref f, tref x, bool bit) {
	using namespace terms_detail;
	if (!thandle<node>::is_bdd_backed(f)) return f; // no decision variables
	DBG(assert(tree<node>::get(x).is(tree<node>::variable));)
	bref<node> r = thandle<node>::convert_to_handle(f).get();
	DBG(assert(tbdd<node>::is_ordered(r));)
	return thandle<node>::convert_to_tau_node_or_term(
		thandle<node>(tbdd<node>::bdd_cofactor(r, x, bit)),
		find_ba_type<node>(f));
}

template <NodeType node>
tref quantify_over(quantifier<node> kind, tref f, const block& X) {
	using namespace terms_detail;
	if (!thandle<node>::is_bdd_backed(f) || X.empty()) return f;
	bref<node> r = thandle<node>::convert_to_handle(f).get();
	DBG(assert(tbdd<node>::is_ordered(r));)
	trefs v(X.begin(), X.end());
	bref<node> q = kind == tbdd<node>::all
		? tbdd<node>::bdd_all(r, std::move(v))
		: tbdd<node>::bdd_ex(r, v);
	return thandle<node>::convert_to_tau_node_or_term(thandle<node>(q),
		find_ba_type<node>(f));
}

template <NodeType node>
bool carries_functional_quantifier(tref f) {
	using tau = tree<node>;
	using namespace terms_detail;
	auto has_fq = [](tref t) {
		return tau::get(t).find_top(
			(bool(*)(tref)) is_functional_quantifier<node>) != nullptr;
	};
	if (!thandle<node>::is_bdd_backed(f)) return has_fq(f);
	auto at_node = [&](bref<node> x, bool leaf) {
		return !(leaf && has_fq(tbdd<node>::get_var_term(x)));
	};
	return !tbdd<node>::visit_nodes(
		thandle<node>::convert_to_handle(f).get(), at_node);
}

// Down the `bf_fall` / `bf_fex` spine, one binder per step: its subscript and
// its kind onto the prefix, then on into the body, until what hangs below is
// no binder any more. Nothing else in the tree is entered.
template <NodeType node>
std::pair<typename tau_term_bdd<node>::quants, tref> strip_chain(tref n) {
	using tau = tree<node>;
	using tbdd = tau_term_bdd<node>;
	typename tbdd::quants q;
	for (;;) {
		const tau& tn = tau::get(n);
		if (!tn.child_is(tau::bf_fall) && !tn.child_is(tau::bf_fex))
			break;
		const tau& c = tau::get(tn.first());
		// The subscript keys the cross-call memo of `bdd_ex`, which
		// orders its key vectors by tref, so it is trimmed to the
		// spelling that table already holds.
		q.emplace_back(tau::trim_right_sibling(c.first()),
			c.is(tau::bf_fall) ? tbdd::all : tbdd::ex);
		n = c.second();
	}
	return { std::move(q), n };
}

template <NodeType node>
tref resolve_functional_quantifiers_plain(tref n,
	const keep_functional_fn<node>& keep)
{
	using namespace terms_detail;
	// The contract: no term of `n` is BDD-backed.
	DBG(assert(!holds_bdd_id<node>(n));)
	// One memo for the whole call, shared with every recursive resolution:
	// a body reached from two chains is resolved once.
	subtree_unordered_map<node, tref> memo;
	return resolve_plain_chains<node>(n, keep, memo);
}

// --- the two aggressive normalisers of invariant 6 --------------------------------

template <NodeType node>
tref simplify_term(tref t, regime rg) {
	using tau = tree<node>;
	using namespace terms_detail;
	t = tau::trim_right_sibling(t);
	if (tau::get(t).equals_0() || tau::get(t).equals_1()) return t;
	// Phases 1, 2 and 5 run in the plain regime, and no term holds a stored
	// BDD there (terms.h) -- the search for a `BDD_ID` is a Debug check of
	// that contract, never a cost of those phases.
	if (rg == regime::plain) {
		DBG(assert(!holds_bdd_id<node>(t));)
		return cofactor_check<node>(syntactic_path_simplification<node>(t));
	}
	// A term with no variable and no stored BDD has nothing to build; the
	// last test is what keeps a term holding a `BDD_ID` out of the sweep,
	// which knows no `BDD_ID`.
	if (!thandle<node>::is_bdd_backed(t) && get_free_vars<node>(t).empty()
		&& !holds_bdd_id<node>(t))
		return syntactic_path_simplification<node>(t);
	// The representation re-established (a BDD-backed term is canonical
	// already; a plain combination of BDD-backed subterms is built; a chain
	// is slid, §1), then every leaf through the leaf simplifier and the
	// rebuild that merges leaves that became equal.
	bref<node> r = tbdd<node>::build_bdd(t);
	DBG(assert(tbdd<node>::is_ordered(r));)
	leaf_simplifier<node> simp;
	return thandle<node>::convert_to_tau_node_or_term(
		thandle<node>(tbdd<node>::map_leaves(r, simp)),
		find_ba_type<node>(t));
}

template <NodeType node>
tref simplify_atom(tref a, regime rg) {
	using tau = tree<node>;
	using namespace terms_detail;
	auto [atom, negated] = unwrap_neg<node>(a);
	const tau& t = tau::get(atom);
	if (t.equals_T() || t.equals_F())
		return negated ? build_wff_neg<node>(atom) : a;
	DBG(assert(is_atomic_fm<node>(atom));)
	tref l = tau::trim_right_sibling(t[0].first());
	tref r = tau::trim_right_sibling(t[0].second());
	// Phases 1, 2 and 5 run in the plain regime, and no side holds a stored
	// BDD there (terms.h): a Debug check of that contract.
	DBG(assert(rg == regime::backed
		|| (!holds_bdd_id<node>(l) && !holds_bdd_id<node>(r)));)
	// ONE recipe in both regimes: the sides simplified, the atom rebuilt
	// through the construction hooks, which fold a constant-only atom,
	// equal sides and a side against its complement. The atom is never
	// reshaped.
	tref l2 = simplify_term<node>(l, rg);
	tref r2 = simplify_term<node>(r, rg);
	DBG(assert(tau::get(l2).is(tau::bf) && tau::get(r2).is(tau::bf));)
	tref res = (l2 == l && r2 == r) ? atom
		: tau::get(tau::wff, tau::get(t[0].value.nt, l2, r2));
	// The joint `l + r` then DECIDES a (¬)equation and does nothing else:
	// it is what sees two sides the hooks cannot compare, two `BDD_ID`
	// nodes above all. `0` decides `=` as `T` and a nonzero constant as
	// `F`, dually for `≠`, which phase 1 meets before NORMALIZE_OPERATORS;
	// anything else leaves the atom as its sides stand.
	const bool eq = t.child_is(tau::bf_eq), neq = t.child_is(tau::bf_neq);
	// A side of `0` makes `term_of` the other side, simplified two lines
	// above already, its own constant (if any) folded by the hooks.
	if (const tau& rt = tau::get(res);
		(eq || neq) && !rt.equals_T() && !rt.equals_F()
		&& !tau::get(l2).equals_0() && !tau::get(r2).equals_0())
	{
		tref f = simplify_term<node>(term_of<node>(res), rg);
		const tau& z = tau::get(build_bf_eq_0<node>(f));
		if (z.equals_T()) res = eq ? tau::_T() : tau::_F();
		else if (z.equals_F()) res = eq ? tau::_F() : tau::_T();
	}
	return negated ? build_wff_neg<node>(res) : res;
}

// --- reading and rewriting equations ---------------------------------------------

template <NodeType node>
tref term_of(tref atom) {
	using namespace terms_detail;
	using tau = tree<node>;
	using bdd = tbdd<node>;
	auto [eq, _] = unwrap_neg<node>(atom);
	const tau& t = tau::get(eq);
	if (!t.is(tau::wff)
		|| (!t.child_is(tau::bf_eq) && !t.child_is(tau::bf_neq))) {
		DBG(assert(false && "term_of: not an equation");)
		return nullptr;
	}
	tref l = t[0].first();
	tref r = t[0].second();
	if (!thandle<node>::is_bdd_backed(l) && !thandle<node>::is_bdd_backed(r))
		return build_bf_xor<node>(l, r);
	// The ring sum of two BDD-backed sides is a BDD operation; a plain side
	// is built alongside, a constant or a single leaf.
	bref<node> a = tbdd<node>::build_bdd(l), b = tbdd<node>::build_bdd(r);
	DBG(assert(bdd::is_ordered(a) && bdd::is_ordered(b));)
	return thandle<node>::convert_to_tau_node_or_term(
		thandle<node>(bdd::bdd_xor(a, b)),
		find_ba_type<node>(thandle<node>::is_bdd_backed(l) ? l : r));
}

template <NodeType node>
tref norm_equation(tref atom) {
	using tau = tree<node>;
	using namespace terms_detail;
	auto [eq, negated] = unwrap_neg<node>(atom);
	if (!tau::get(eq).child_is(tau::bf_eq)) return atom;
	tref res = build_bf_eq_0<node>(term_of<node>(eq));
	return negated ? build_wff_neg<node>(res) : res;
}

// --- in-memory size and the leaf hazard -------------------------------------------

template <NodeType node>
size_t mem_size(tref t) {
	using namespace terms_detail;
	if (!thandle<node>::is_bdd_backed(t)) {
		// Term-structure nodes only, each distinct one once: every child of
		// a `bf` wrapper — an operator, a functional quantifier, a leaf
		// (variable, constant or reference) — counts 1; the wrappers are not
		// counted, and the walk does not enter a leaf's own nodes (a
		// variable's name, a reference's arguments) or a quantifier's
		// subscript. The traversal's `visit_subtree` predicate gates a node's
		// own visit, so a leaf is admitted by its parent and its children are
		// not.
		using tau = tree<node>;
		size_t n = 0;
		auto count = [&n](tref m) {
			if (!tau::get(m).is(tau::bf)) ++n;
			return true;
		};
		// The traversal asks about the root with a null parent.
		auto admit = [](tref m, tref parent) {
			return while_is_boolean_operation<node>(m)
				|| (parent && tau::get(parent).is(tau::bf));
		};
		auto up = [](tref) {};
		pre_order<node>(t).visit_unique(count, admit, up);
		return n;
	}
	return tbdd<node>::node_count(thandle<node>::convert_to_handle(t).get());
}

template <NodeType node>
trefs leaf_fv(tref f) {
	using namespace terms_detail;
	if (!thandle<node>::is_bdd_backed(f)) return get_free_vars<node>(f);
	// The library's `get_free_leaf_vars`, by value and never stored: the
	// per-leaf sets are `get_free_vars`' own cached entries.
	return thandle<node>::get_free_leaf_vars(
		thandle<node>::convert_to_handle(f).get().b);
}

// --- the PURE shapes --------------------------------------------------------------

template <NodeType node>
bool is_pure_term(tref t, ba_type_id type) {
	using tau = tree<node>;
	using namespace terms_detail;
	// ONE scan per plain term, stopping at the first node that breaks the
	// shape: an arithmetic operator or a width cast
	// (`is_non_boolean_term`), a term-level reference, or a node carrying a
	// BA type of its own that is not `type` — a foreign-typed subterm. The
	// untyped id `0` is no claim of its own and passes.
	auto plain_pure = [type](tref n) {
		return tau::get(n).find_top([type](tref m) {
			const tau& tm = tau::get(m);
			if (is_non_boolean_term<node>(m)
				|| tm.is(tau::bf_ref)) return true;
			const ba_type_id bt = tm.get_ba_type();
			return bt != 0 && bt != type;
		}) == nullptr;
	};
	if (!thandle<node>::is_bdd_backed(t)) return plain_pure(t);
	// A stored BDD branches on variables alone, so the same scan on every
	// leaf answers for the whole term. The walk stops at the first impure
	// leaf and then reports that it was stopped.
	auto at_node = [&plain_pure](bref<node> x, bool leaf) {
		return !leaf || plain_pure(tbdd<node>::get_var_term(x));
	};
	return tbdd<node>::visit_nodes(
		thandle<node>::convert_to_handle(t).get(), at_node);
}

template <NodeType node>
bool is_pure_equation(tref literal, ba_type_id type) {
	using tau = tree<node>;
	const tref a = is_negated_equation<node>(literal)
		? atom_of<node>(literal) : literal;
	if (!is_child<node>(a, tau::bf_eq)) return false;
	// The sides are read off the atom as `TERM_OF` reads them, each trimmed
	// of the sibling chain it carries inside the atom.
	const tau& eq = tau::get(a)[0];
	return is_pure_term<node>(tau::trim_right_sibling(eq.first()), type)
		&& is_pure_term<node>(tau::trim_right_sibling(eq.second()), type);
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_TMPL_H__
