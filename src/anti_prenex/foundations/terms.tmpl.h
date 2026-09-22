// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file terms.tmpl.h
 * @brief Template implementations for terms.h (package B). Included by
 * terms.h. terms.h says what each function means; the comments here say how
 * it is built.
 *
 * Conventions of the BDD library this file rides on (tau_bdd.h):
 *  - the library's universe map `U`, which links a `BDD_ID` node to its BDD,
 *    is keyed by the `BDD_ID` node under the `bf` wrapper (`key_of`). That is
 *    the same tref in every spelling of the wrapper, so a lookup steps to the
 *    child and never trims. `convert_to_tau_node` interns (tau_bdd.tmpl.h),
 *    so one BDD per type is one Tau node.
 *  - a BDD leaf's variable (`get_var`, or `get_var_term` as a `bf` term) is
 *    the TRIMMED Tau node under `bf`, a `bf_neg` node for an inverted leaf.
 *    `add(leaf)` takes the trimmed node and maps `bf_t` / `bf_f` to the
 *    terminals.
 *  - the memoised BDD walks are the library's own (`visit_nodes`,
 *    `map_leaves`, `bdd_cofactor`): recursive over BDD references with a
 *    per-call memo, the shape of its own workers (`bdd_ex`,
 *    `bdd_compose_impl`). Walks over the TAU tree go through `pre_order`
 *    (dag.h, tree.h) instead.
 *  - `build_bdd` owns the functional-quantifier SLIDE: it collects a chain
 *    whole, builds the body under the order and wraps the chain onto every
 *    leaf. A chain BINDING a key of the order does not slide -- it is one
 *    leaf, hiding what its body carries (§1) -- and neither does one that
 *    meets no key. Nothing here prepares a term for any of that, and every
 *    caller hands it the term as written.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_TMPL_H__

// `syntactic_path_simplification` (heuristics/) and
// `syntactic_atomic_formula_simplification` (normal_forms.tmpl.h, declared
// nowhere else) are the simplification building blocks the two normalisers
// below reuse. The heuristics header must be included through
// normal_forms.h, which declares what its body uses before the body.
#include "normal_forms.h"

#include <optional>

namespace idni::tau_lang::anti_prenexing {

namespace terms_detail {

template <NodeType node> using tbdd  = tau_term_bdd<node>;
template <NodeType node> using bref  = typename tau_term_bdd<node>::ref;
template <NodeType node> using thandle = term_handle<node>;

/// The order a chain quantifies under: its INNERMOST subscript ranks 1,
/// outward from there, over the subscripts alone. That is what `bdd_quant`
/// asserts of the reversed prefix.
template <NodeType node>
var_order<node> chain_order(const typename tbdd<node>::quants& q) {
	var_order<node> o;
	for (size_t i = 0; i < q.size(); ++i)
		o.emplace(q[i].first, int_t(q.size() - i));
	return o;
}

/// Every binder of the prefix has the same kind.
template <NodeType node>
bool one_kind(const typename tbdd<node>::quants& q) {
	for (size_t i = 1; i < q.size(); ++i)
		if (q[i].second != q[0].second) return false;
	return true;
}

/// ONE quantification of @p x by the WHOLE prefix @p q under @p o. A prefix of
/// one kind is a set quantification whose members commute, so it goes through
/// `bdd_ex` / `bdd_all`, which sort the set themselves and share the library's
/// set memo; a mixed prefix goes through `bdd_quant`, whose own precondition is
/// that @p o NESTS it — each subscript ranking strictly below the one outside
/// it, which the caller's order does by construction.
template <NodeType node>
bref<node> quantify_prefix(bref<node> x, const typename tbdd<node>::quants& q,
	const var_order<node>& o)
{
	if (!one_kind<node>(q)) {
#ifdef DEBUG
		for (size_t i = 1; i < q.size(); ++i)
			assert(tbdd<node>::less_then(q[i].first,
				q[i - 1].first, o));
#endif
		return tbdd<node>::bdd_quant(x, q, o);
	}
	trefs ys;
	ys.reserve(q.size());
	for (size_t i = 0; i < q.size(); ++i) ys.push_back(q[i].first);
	return q[0].second == tbdd<node>::all
		? tbdd<node>::bdd_all(x, std::move(ys), o)
		: tbdd<node>::bdd_ex(x, ys, o);
}

/// Does @p t carry a `BDD_ID` anywhere -- a stored BDD at the top, or one
/// held under a functional-quantifier chain (§7 `DISCHARGE`'s keep-mode
/// emission) or inside a leaf? The BDD regime of the two normalisers below is
/// keyed on it: nothing outside this file knows a `BDD_ID`.
template <NodeType node>
bool holds_bdd_id(tref t) {
	using tau = tree<node>;
	return tau::get(t).find_top([](tref m) {
		return tau::get(m).is(tau::BDD_ID); }) != nullptr;
}

#ifdef DEBUG
/// Every `BDD_ID` of @p t is a BDD under @p order: the contract both resolvers
/// have on their input (terms.h), which an EMPTY @p order turns into "no
/// `BDD_ID` at all", a stored BDD always branching. The LEAVES of a stored BDD
/// are not entered — they are terms of the BDD universe, not nodes of this
/// tree, and a `BDD_ID` inside one is what `convert_to_tau_terms` is total for.
template <NodeType node>
bool stored_bdds_under_order(tref t, const var_order<node>& order) {
	using tau = tree<node>;
	bool ok = true;
	auto check = [&](tref m) {
		if (!tau::get(m).is(tau::BDD_ID)) return true;
		if (!tbdd<node>::is_ordered(
			thandle<node>::convert_to_handle(m).get(), order))
			ok = false;
		return false;   // nothing below it is a node of this tree
	};
	pre_order<node>(t).visit_unique(check);
	return ok;
}
#endif

/// What the chain step hands back: the stored BDD quantified over the
/// subscripts it took, and the prefix that STAYS over it, outermost first. An
/// empty `kept` is a chain that resolved whole.
template <NodeType node>
struct chain_result {
	bref<node> b;
	typename tbdd<node>::quants kept;
};

/**
 * @brief THE CHAIN STEP (§3): ONE quantification of the STORED BDD @p s by
 * those subscripts of the canonical prefix @p q — outermost first — that the
 * §1 LEAF HAZARD leaves reachable. @p order is the order @p s is built over,
 * and every subscript is a key of it.
 *
 * A subscript occurring in a LEAF of @p s is reached by no quantification and
 * stays. THE STUCK RULE decides the rest, INNERMOST FIRST: ahead of the first
 * hidden subscript every subscript is quantified; the first hidden one opens a
 * stuck run of ITS KIND, into which an outer subscript of that kind still
 * commutes — quantified when it is not hidden, joining the run when it is —
 * and the first subscript of the OTHER kind blocks, staying along with
 * everything outward of it. What is taken is quantified in ONE pass, and
 * `std::nullopt` says nothing was, which leaves the chain as it is.
 *
 * A choice of size, not of soundness: crossing an alternation by cofactors is
 * legal and duplicates the stuck chain.
 */
template <NodeType node>
std::optional<chain_result<node>> resolve_chain(
	const typename tbdd<node>::quants& q, bref<node> s,
	const var_order<node>& order)
{
	using tau = tree<node>;
#ifdef DEBUG
	assert(!q.empty());
	assert(tbdd<node>::is_ordered(s, order));
	for (const auto& qi : q) assert(order.contains(qi.first));
#endif
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
	return chain_result<node>{ quantify_prefix<node>(s, e, order),
		std::move(kept) };
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
		// The body over the CHAIN'S OWN order, innermost subscript
		// lowest: nothing is BDD-backed here, so there is nothing to
		// spell out first, and a chain of the body that survived is slid
		// onto the leaves by the build (§1 transparency), where the
		// quantification leaves it alone. A build that does not BRANCH
		// has every subscript in a leaf and settles the chain as it is.
		const var_order<node> o = chain_order<node>(q);
		bref<node> b = tbdd<node>::build_bdd(body, o);
		if (tbdd<node>::leaf(b)) return done.insert(c), c;
		auto r = resolve_chain<node>(q, b, o);
		if (!r) return done.insert(c), c;
		// PLAIN again before the pass moves on: a `BDD_ID` minted under
		// a chain's order would be a node of no live order.
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
/// A leaf that holds no `BDD_ID` is the path sweep's, exactly as before. A
/// leaf that holds one -- §7 `DISCHARGE`'s keep-mode emission
/// `Q_P (bf(BDD_ID))`, which `build_bdd` keeps whole because §1 forbids
/// spelling a stored BDD out before the component's close -- is swept over
/// its PLAIN structure alone: to the sweep a `bf(BDD_ID)` is an opaque leaf,
/// never descended into (`path_sweep::down` stops at a node that is no
/// connective), never negated into (`push_negation_in` leaves a negated one
/// as it is) and never substituted into. The stored BDDs the sweep left
/// standing are then simplified from the inside, one `map_leaves` each with
/// this same simplifier, so a nest of them settles innermost leaf first.
///
/// Results are memoised per leaf node: a leaf's simplified form is a function
/// of the leaf and the live order, and one leaf is reached from several BDDs.
template <NodeType node>
struct leaf_simplifier {
	explicit leaf_simplifier(const var_order<node>& o) : order(o) {}

	tref operator()(tref leaf) {
		if (auto it = memo.find(leaf); it != memo.end())
			return it->second;
		tref r = simplify(leaf);
		memo.emplace(leaf, r);
		return r;
	}

private:
	const var_order<node>& order;
	subtree_unordered_map<node, tref> memo;

	tref simplify(tref leaf) {
		tref swept = syntactic_path_simplification<node>(leaf);
		if (!holds_bdd_id<node>(swept)) return swept;
		auto inside = [this](tref m) -> tref {
			if (!thandle<node>::is_bdd_backed(m)) return m;
			bref<node> b = thandle<node>::convert_to_handle(m).get();
			DBG(assert(tbdd<node>::is_ordered(b, order));)
			return thandle<node>::convert_to_tau_node_or_term(
				thandle<node>(tbdd<node>::map_leaves(
					b, *this, order)),
				find_ba_type<node>(m));
		};
		// A rebuilt stored BDD is not re-entered: its own leaves went
		// through this simplifier already.
		return pre_order<node>(swept).apply_unique_until_change(inside);
	}
};

/// §3 `SIMPLIFY_TERM`'s fourth law, the COFACTOR CHECK: one-level Shannon
/// reduction of the WHOLE term over every free variable of @p t that is no
/// variable of @p order. An order variable is one the BDD has decided
/// already, and one still free in a leaf sits where the expansion is not
/// licensed (§1's leaf hazard).
///
/// `f₀ = t[y←0]` and `f₁ = t[y←1]` reach BOOLEAN POSITIONS only, which is
/// what the substitution's guard says: no Shannon expansion holds under an
/// arithmetic operator, a reference or a foreign-typed subterm. Leaving a
/// variable inside one of those is sound — it is an opaque element the
/// expansion is taken around. Equal cofactors, a constant included, make
/// `t ← f₀`, which covers being `0` under both bits, being `1` under both,
/// and not depending on `y` at all.
///
/// The variables are read off the term handed in and then tried ON THE
/// RUNNING term, so the ones after a replacement are tried on the smaller
/// term. A cofactor is what the substitution hands back, folded by the
/// construction hooks and simplified no further.
template <NodeType node>
tref cofactor_check(tref t, const var_order<node>& order) {
	using tau = tree<node>;
	// Copied rather than held: the walks below write to the cache this
	// list comes from.
	const trefs vars = get_free_vars<node>(t);
	for (tref y : vars) {
		if (order.contains(y)) continue;
		const tref key = tau::get(tau::bf, y);
		const size_t ba = find_ba_type<node>(y);
		const tref f0 = tau::get(t).substitute(key, _0<node>(ba),
			order, identity, while_is_boolean_operation<node>);
		const tref f1 = tau::get(t).substitute(key, _1<node>(ba),
			order, identity, while_is_boolean_operation<node>);
		if (f0 != f1) continue;
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
tref prepare_terms(tref body, [[maybe_unused]] const block& P,
	const var_order<node>& order)
{
	using tau = tree<node>;
	using namespace terms_detail;
#ifdef DEBUG
	assert(P.size() == order.size());
	for (tref p : P) assert(order.contains(p));
#endif
	// Memoised per term, within this call only.
	subtree_unordered_map<node, tref> term_memo;
	auto prep = [&](tref t) -> tref {
		if (thandle<node>::is_bdd_backed(t)) { // already prepared: idempotent
			DBG(assert(tbdd<node>::is_ordered(
				thandle<node>::convert_to_handle(t).get(), order));)
			return t;
		}
		if (!tbdd<node>::has_bdd_var(t, order)) return t;
		if (auto it = term_memo.find(t); it != term_memo.end())
			return it->second;
		tref r = thandle<node>::convert_to_tau_node_or_term(
			thandle<node>(tbdd<node>::build_bdd(t, order)),
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

// --- cofactors and quantification -----------------------------------------------

template <NodeType node>
tref cofactor(tref f, tref x, bool bit, const var_order<node>& order) {
	using namespace terms_detail;
	if (!thandle<node>::is_bdd_backed(f)) return f; // no decision variables
	DBG(assert(tree<node>::get(x).is(tree<node>::variable));)
	if (!order.contains(x)) return f;      // not a decision variable
	bref<node> r = thandle<node>::convert_to_handle(f).get();
	DBG(assert(tbdd<node>::is_ordered(r, order));)
	return thandle<node>::convert_to_tau_node_or_term(
		thandle<node>(tbdd<node>::bdd_cofactor(r, x, bit, order)),
		find_ba_type<node>(f));
}

template <NodeType node>
tref quantify_over(quantifier<node> kind, tref f, const block& X,
	const var_order<node>& order)
{
	using namespace terms_detail;
	if (!thandle<node>::is_bdd_backed(f) || X.empty()) return f;
	bref<node> r = thandle<node>::convert_to_handle(f).get();
	DBG(assert(tbdd<node>::is_ordered(r, order));)
	trefs v(X.begin(), X.end());
	bref<node> q = kind == tbdd<node>::all
		? tbdd<node>::bdd_all(r, std::move(v), order)
		: tbdd<node>::bdd_ex(r, v, order);
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
		// The subscript keys the cross-call memos of `bdd_ex` and
		// `bdd_quant`, which order their key vectors by tref, so it is
		// trimmed to the spelling those tables already hold.
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
	// The EMPTY order is the contract: no term of `n` is BDD-backed.
	DBG(assert(stored_bdds_under_order<node>(n, var_order<node>{}));)
	// One memo for the whole call, shared with every recursive resolution:
	// a body reached from two chains is resolved once.
	subtree_unordered_map<node, tref> memo;
	return resolve_plain_chains<node>(n, keep, memo);
}

template <NodeType node>
tref resolve_functional_quantifiers_bdd(tref n, const var_order<node>& order,
	const keep_functional_fn<node>& keep)
{
	using tau = tree<node>;
	using namespace terms_detail;
	n = tau::trim_right_sibling(n);
	DBG(assert(stored_bdds_under_order<node>(n, order));)
	// What this pass produced: a chain `keep` accepted, a chain the stuck
	// rule left standing, and a quantified result, plus every BDD-backed
	// term met on the way. All of them are decided — the traversal refuses
	// to enter one.
	subtree_unordered_set<node> done;
	auto f = [&](tref m) -> tref {
		const tau& tm = tau::get(m);
		if (!tm.is(tau::bf)) return m;
		// A BDD-backed term keeps its chains in the LEAVES, and a leaf
		// is never entered: a chain there is an inner block's kept
		// emission or a stuck one, and both are decided (§3).
		if (thandle<node>::is_bdd_backed(m)) return done.insert(m), m;
		if (!tm.child_is(tau::bf_fall) && !tm.child_is(tau::bf_fex))
			return m;    // a chain may still sit deeper: descend
		// THE WHOLE NEST at once, canonicalised through the
		// constructor. Its body is not resolved first: at the close a
		// body is either BDD-backed or the chain stays whole.
		auto [q0, b0] = strip_chain<node>(m);
		tref c = tbdd<node>::build_functional_quantifiers(q0, b0);
		// `keep` sees that canonical chain NODE, ONCE — prefix and body
		// come off it with `strip_chain` (§3) — and its yes keeps the
		// whole chain.
		auto [q, body] = strip_chain<node>(c);
		if (q.empty()) return done.insert(c), c;
		if (keep(c)) return done.insert(c), c;
		// A PLAIN body is one whose BDD did not branch: every subscript
		// sits in a leaf, so nothing of the chain resolves.
		if (!thandle<node>::is_bdd_backed(body))
			return done.insert(c), c;
		// The subscripts are decision variables of the stored BDD, so
		// ONE quantification under the LIVE order settles the chain
		// where it lies, and the result is emitted like every other
		// term: plain when nothing branches, which is the usual
		// whole-block outcome.
		bref<node> s = thandle<node>::convert_to_handle(body).get();
		auto r = resolve_chain<node>(q, s, order);
		if (!r) return done.insert(c), c;
		tref e = thandle<node>::convert_to_tau_node_or_term(
			thandle<node>(r->b), find_ba_type<node>(body));
		// The prefix the stuck rule left, re-attached in its order.
		tref res = tbdd<node>::build_functional_quantifiers(r->kept, e);
		return done.insert(res), res;
	};
	auto visit = [&done](tref m) { return !done.contains(m); };
	return pre_order<node>(n).apply_unique(f, visit);
}

// --- the two aggressive normalisers of invariant 6 --------------------------------

template <NodeType node>
tref simplify_term(tref t, const var_order<node>& order) {
	using tau = tree<node>;
	using namespace terms_detail;
	t = tau::trim_right_sibling(t);
	if (tau::get(t).equals_0() || tau::get(t).equals_1()) return t;
	// Phases 1, 2 and 5 run without an order, and no term holds a stored
	// BDD there (terms.h), so the plain regime is reached at once -- the
	// search for a `BDD_ID` is a Debug check of that contract, never a
	// cost of those phases.
	if (order.empty()) {
		DBG(assert(!holds_bdd_id<node>(t));)
		return cofactor_check<node>(
			syntactic_path_simplification<node>(t), order);
	}
	// Plain regime under a live order: a term that neither touches `P` nor
	// carries a stored BDD anywhere -- the last test is what keeps a chain
	// over a `BDD_ID` out of the plain simplifier, which knows no `BDD_ID`
	// (§7 `DISCHARGE`'s keep emission is `P`-free and not backed itself).
	if (!thandle<node>::is_bdd_backed(t) && !tbdd<node>::has_bdd_var(t, order)
		&& !holds_bdd_id<node>(t))
		return cofactor_check<node>(
			syntactic_path_simplification<node>(t), order);
	// BDD regime: the representation re-established (a BDD-backed term is
	// canonical already; a plain combination of BDD-backed subterms is
	// built over `P`; a chain over a stored BDD is ONE LEAF of its own
	// BDD, §1), then every leaf through the leaf simplifier and the
	// rebuild that merges leaves that became equal. A whole-block keep
	// emission is that single leaf and comes back as it stands, its inner
	// leaves simplified. The sweep is per leaf; the check that follows it
	// is per term.
	bref<node> r = tbdd<node>::build_bdd(t, order);
	DBG(assert(tbdd<node>::is_ordered(r, order));)
	leaf_simplifier<node> simp{order};
	return cofactor_check<node>(
		thandle<node>::convert_to_tau_node_or_term(
			thandle<node>(tbdd<node>::map_leaves(r, simp, order)),
			find_ba_type<node>(t)), order);
}

template <NodeType node>
tref simplify_atom(tref a, const var_order<node>& order) {
	using tau = tree<node>;
	using namespace terms_detail;
	auto [atom, negated] = unwrap_neg<node>(a);
	const tau& t = tau::get(atom);
	if (t.equals_T() || t.equals_F())
		return negated ? build_wff_neg<node>(atom) : a;
	DBG(assert(is_atomic_fm<node>(atom));)
	tref l = tau::trim_right_sibling(t[0].first());
	tref r = tau::trim_right_sibling(t[0].second());
	tref res;
	// Phases 1, 2 and 5 run without an order, and no side holds a stored
	// BDD there (terms.h), so they take the plain branch at once -- the
	// search for a `BDD_ID` is a Debug check of that contract, never a
	// cost of those phases.
	DBG(assert(!order.empty()
		|| (!holds_bdd_id<node>(l) && !holds_bdd_id<node>(r)));)
	if (!order.empty()
		&& (holds_bdd_id<node>(l) || holds_bdd_id<node>(r))) {
		// BDD regime: side-wise, the atom never reshaped; a constant-only
		// atom folds through the construction hooks. A BDD-backed side
		// is the top-level case of the test; the deeper one is §7
		// `DISCHARGE`'s keep emission `Q_P (bf(BDD_ID))`, a side that
		// holds a stored BDD without being backed itself.
		tref l2 = simplify_term<node>(l, order);
		tref r2 = simplify_term<node>(r, order);
		res = (l2 == l && r2 == r) ? atom
			: tau::get(tau::wff, tau::get(t[0].value.nt, l2, r2));
	} else {
		// Plain regime: the full existing atom simplifier — the
		// norm/denorm round trip on the joint `l + r` and the per-variable
		// pass — idempotent, so "as written" is "as simplified".
		res = syntactic_atomic_formula_simplification<node>(atom);
	}
	return negated ? build_wff_neg<node>(res) : res;
}

// --- reading and rewriting equations ---------------------------------------------

template <NodeType node>
tref term_of(tref atom, const var_order<node>& order) {
	using namespace terms_detail;
	using tau = tree<node>;
	using bdd = tbdd<node>;
	auto [eq, _] = unwrap_neg<node>(atom);
	const tau& t = tau::get(eq);
	if (!t.is(tau::wff) || !t.child_is(tau::bf_eq)) {
		DBG(assert(false && "term_of: not an equation");)
		return nullptr;
	}
	tref l = t[0].first();
	tref r = t[0].second();
	if (!thandle<node>::is_bdd_backed(l) && !thandle<node>::is_bdd_backed(r))
		return build_bf_xor<node>(l, r);
	// The ring sum of two BDD-backed sides is a BDD operation; a plain side
	// (a constant, a P-free term) is a leaf under the same order.
	bref<node> a = tbdd<node>::build_bdd(l, order),
		b = tbdd<node>::build_bdd(r, order);
	DBG(assert(bdd::is_ordered(a, order) && bdd::is_ordered(b, order));)
	return thandle<node>::convert_to_tau_node_or_term(
		thandle<node>(bdd::bdd_xor(a, b, order)),
		find_ba_type<node>(thandle<node>::is_bdd_backed(l) ? l : r));
}

template <NodeType node>
tref norm_equation(tref atom, const var_order<node>& order) {
	using tau = tree<node>;
	using namespace terms_detail;
	auto [eq, negated] = unwrap_neg<node>(atom);
	if (!tau::get(eq).child_is(tau::bf_eq)) return atom;
	tref res = build_bf_eq_0<node>(term_of<node>(eq, order));
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

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_TMPL_H__
