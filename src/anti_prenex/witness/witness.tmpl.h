// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file witness.tmpl.h
 * @brief Template implementations for witness.h. witness.h says what each
 * function means; the comments here say how it is built.
 *
 * WHAT THE TWO MODES SHARE: the tail of `TRY_WITNESS` — the choice among the
 * pins a conjunct list offers, the substitution of the winner's witness and
 * the simplification — is one helper over the mode's own pin test, and the
 * COF test itself is one helper over a conjunct list, asked of `ψ`'s
 * conjuncts by `TRY_WITNESS` and of a branch's by `TRY_CASE_WITNESS`.
 *
 * THE TWO REWRITES of the deep pass are the library's: `φ[x ← t]` is
 * `subst_var` (foundations/subst.h) under the empty order, phase 2 holding
 * no BDD-backed term, and the spine's replacement is `rewriter::replace`, a
 * CONTENT match with a hooked rebuild of the path above it. Replacing every
 * occurrence of the spine is sound because the descent enforces confinement
 * first (§3): a second occurrence of that node elsewhere in `Φ` would put
 * `x` free in two members of some junction on the way down, which the descent
 * refuses.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__WITNESS__WITNESS_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__WITNESS__WITNESS_TMPL_H__

#include <algorithm>
#include <cassert>

namespace idni::tau_lang::anti_prenexing {

namespace witness_detail {

/// Does `n` carry a BDD-backed term anywhere? The `BDD_ID` search of
/// tau_bdd.tmpl.h, on the whole subtree. DEBUG ONLY: it is the entry guard of
/// the spelled mode (§3, "nothing is BDD-backed there"), where a backed term
/// is a caller bug.
template <NodeType node>
bool holds_bdd_term(tref n) {
	using tau = tree<node>;
	return tau::get(n).find_top([](tref m) {
		return tau::get(m).is(tau::BDD_ID); }) != nullptr;
}

/// The result join of `ms` in the connective `conj` picks (§3): the ∧-join
/// for a `wff_and` spine, the ∨-join for a `wff_or` one. An empty list is the
/// connective's neutral constant — what a spine minus its only other member
/// leaves behind.
template <NodeType node>
tref join_of(const trefs& ms, bool conj) {
	return conj ? simplified_and_join<node>(ms) : simplified_or_join<node>(ms);
}

/// §3 The choice among the pins a conjunct list offers, `match` being the
/// mode's pin test on ONE conjunct: a STRICT pin first — taken without
/// comparing witnesses — else the smallest `‖f₁′‖` (`mem_size`), the cost every
/// later substitution of the witness pays.
template <NodeType node, typename Match>
std::optional<pin<node>> best_pin(const trefs& cs, Match&& match) {
	std::optional<pin<node>> best;
	size_t best_size = 0;
	for (tref c : cs) {
		std::optional<pin<node>> p = match(c);
		if (!p) continue;
		if (p->strict) return p;
		if (const size_t size = mem_size<node>(p->witness);
			!best || size < best_size)
				best = p, best_size = size;
	}
	return best;
}

/// §3 `TRY_WITNESS`'s TAIL, the one both modes run once their own `search`
/// has picked a pin among `ψ`'s conjuncts: `x` is replaced by that pin's
/// witness in EVERY conjunct, THE PINNING ONE INCLUDED — its image is the
/// residual `p = 0`, `T` for a strict pin, so no site emits a residual of its
/// own — and the result is SIMPLIFIED (invariant 6). One substitution over
/// `ψ` reaches every conjunct. `order` is the mode's: empty for the spelled
/// one, `ctx.order` for the COF one.
template <NodeType node, typename Search>
std::optional<tref> witness_with(tref x, tref psi,
	const var_order<node>& order, Search&& search)
{
	const std::optional<pin<node>> best = search(get_cnf_wff_clauses<node>(psi));
	if (!best) return {};
	return simplify<node>(
		subst_var<node>(psi, x, best->witness, order), order);
}

/// The POSITIVE EQUATION the pin match reads off a spine member, in `Q`'s
/// form (§3). For `Q = ∃` the member itself, when it is an equation. For
/// `Q = ∀`, under `¬∃x¬`, the equation under a NEGATED one — `¬(l = r)` after
/// `TO_NNF`, or a `bf_neq` atom, which is still a spelling at phase 2, since
/// `NORMALIZE_OPERATORS` is phase 3. Reading a `bf_neq` is allowed; only
/// BUILDING one is not (invariant 4), and the equation built here is a
/// `bf_eq`. `nullptr` for anything else — an atom that cannot pin, a
/// reference, a unit, a temporal operator.
template <NodeType node>
tref pin_equation(tref m, quantifier<node> Q) {
	using tau = tree<node>;
	m = tau::trim_right_sibling(m);
	if (Q == tau_term_bdd<node>::ex)
		return is_child<node>(m, tau::bf_eq) ? m : nullptr;
	if (is_negated_equation<node>(m)) return atom_of<node>(m);
	if (is_child<node>(m, tau::bf_neq)) {
		const auto& t = tau::get(m)[0];
		return tau::build_bf_eq(t.first(), t.second());
	}
	return nullptr;
}

/// The STRICT pin of `x` in one member, in `Q`'s form, or `nullopt` (§3). A
/// weak pin is NO pin for the deep pass: the member is dropped, so the
/// residual `p = 0` it keeps would be lost. `find_pin_for` (simplify.h) is
/// the match itself; the empty order is phase 2's.
template <NodeType node>
std::optional<pin<node>> strict_pin_of(tref m, tref x, quantifier<node> Q) {
	const tref eq = pin_equation<node>(m, Q);
	if (eq == nullptr) return {};
	std::optional<pin<node>> p = find_pin_for<node>(eq, x);
	if (!p || !p->strict) return {};
	return p;
}

/// One branch of a case pin: the branch as it stands, the branch MINUS its
/// pinning member — the neutral constant when the pin was all of it — and the
/// witness that pin gives.
template <NodeType node>
struct case_branch {
	tref branch  = nullptr;
	tref residue = nullptr;
	tref witness = nullptr;
};

/**
 * @brief §3 A CASE PIN for `x` in the member `m`, in `Q`'s form: one record
 * per branch, or `nullopt`.
 *
 * For `Q = ∃`: `m` is an ∨-node, at most `case_max` branches, and EVERY
 * branch has a conjunct pinning `x` strictly with `x ∉ FV(tᵢ)` — all or
 * nothing, since a branch without a pin would keep `∃x` alive inside its copy
 * (§3). For `Q = ∀`, under `¬∃x¬`, the dual: an ∧-node whose branches hold a
 * negated-equation DISJUNCT.
 *
 * The match stays at `m`'s TOP branches (§3): expanding nested structure
 * into deeper cases would let guard disjunctions that merely CONTAIN solving
 * atoms qualify. A branch that is a unit is opaque (§4) — its member view is
 * the unit itself, which is no equation — so it never qualifies, and nothing
 * here descends into one.
 *
 * `case_max` is read BARE from options.h: phase 2 runs before any component,
 * so there is no ctx (§1, `K″`).
 */
template <NodeType node>
std::optional<std::vector<case_branch<node>>> case_pin_of(tref m, tref x,
	quantifier<node> Q)
{
	using tau = tree<node>;
	const bool ex = Q == tau_term_bdd<node>::ex;
	if (!is_child<node>(m, ex ? tau::wff_or : tau::wff_and)) return {};
	const trefs bs = members<node>(m);
	if (bs.size() > case_max) return {};
	std::vector<case_branch<node>> out;
	out.reserve(bs.size());
	for (tref b : bs) {
		b = tau::trim_right_sibling(b);
		// The branch's own members: its conjuncts for `Q = ∃`, its
		// disjuncts for `Q = ∀`. A branch that is not that connective
		// is its own single member, so a lone `x = t` branch qualifies
		// and a unit branch does not.
		const trefs ps = members<node>(b);
		std::optional<pin<node>> hit;
		size_t at = 0;
		for (size_t i = 0; i < ps.size() && !hit; ++i)
			if ((hit = strict_pin_of<node>(ps[i], x, Q))) at = i;
		if (!hit) return {};      // ALL-OR-NOTHING (§3)
		DBG(assert(!fv_meets<node>(hit->witness, block{ x }));)
		trefs rest;
		rest.reserve(ps.size() - 1);
		for (size_t i = 0; i < ps.size(); ++i)
			if (i != at) rest.push_back(ps[i]);
		out.push_back(case_branch<node>{ b,
			join_of<node>(rest, ex), hit->witness });
	}
	return out;
}

/**
 * @brief §3 The COF-MODE pin test on ONE conjunct: the pin `x` has in a
 * BDD-backed conjunct, or `nullopt`.
 *
 * Only a POSITIVE equation is asked — an equation under `¬` pins nothing in
 * the ∃ sense, and an order atom is no equation and never pins. The term is
 * `TERM_OF(c)` as it arrives, which IS `cof_memo`'s key (§1), so this site
 * forms it itself. A term not mentioning `x` is skipped rather than asked:
 * it can pin nothing, and `COF` Debug-asserts against such a vacuous entry.
 * The pin is `COF`'s (§6); the witness `f₁′` and the STRICT verdict `p = 0`
 * are read off the record, which carries neither.
 */
template <NodeType node>
std::optional<pin<node>> cof_pin_of(tref m, tref x, ctx<node>& c) {
	using tau = tree<node>;
	m = tau::trim_right_sibling(m);
	if (!is_child<node>(m, tau::bf_eq)) return {};
	const tref f = term_of<node>(m, c.order);
	if (f == nullptr) return {};
	// The binary search over the cached free-variable set (simplify.tmpl.h).
	if (!detail::free_in<node>(f, x)) return {};
	const cof_entry e = cof<node>(f, x, c);
	if (!e.pin) return {};
	// THE WITNESS IS THE LOWER END `f₁′` (§3): it carries the residual into
	// every sibling's terms, where `p = 0` then folds.
	const tref witness =
		simplify_term<node>(build_bf_neg<node>(e.f1), c.order);
	return pin<node>{ x, witness, tau::get(e.p).equals_0() };
}

/// §3 The COF-mode pin search over ONE conjunct list: the scan
/// `TRY_WITNESS` runs over `ψ`'s conjuncts, and the one `TRY_CASE_WITNESS`
/// runs over a branch's.
template <NodeType node>
std::optional<pin<node>> cof_pin_in(const trefs& cs, tref x, ctx<node>& c) {
	return best_pin<node>(cs,
		[x, &c](tref m) { return cof_pin_of<node>(m, x, c); });
}

/**
 * @brief §3 A CASE PIN for `x` in the conjunct `m`, COF mode: the branches
 * with their witnesses, or `nullopt`.
 *
 * `m` is an ∨-node of at most `ctx.case_max` branches — the component's knob,
 * where phase 2 reads the bare constant — and EVERY branch has a conjunct
 * whose term pins `x` by `COF`: all or nothing, since a branch without a pin
 * would keep `∃x` alive inside its copy (§3). A branch that is not a
 * conjunction is its own one-member view, so a lone `x = t` branch qualifies
 * and a unit branch, being no equation, does not (§4). The match stays at
 * `m`'s TOP branches (§3). Each branch comes back AS IT STANDS, its pinning
 * conjunct included: the pin becomes the branch's residual under `[x ← tᵢ]`.
 */
template <NodeType node>
std::optional<std::vector<std::pair<tref, tref>>> cof_case_pin_of(tref m,
	tref x, ctx<node>& c)
{
	using tau = tree<node>;
	m = tau::trim_right_sibling(m);
	if (!is_child<node>(m, tau::wff_or)) return {};
	const trefs bs = members<node>(m);
	if (bs.size() > c.case_max) return {};
	std::vector<std::pair<tref, tref>> out;
	out.reserve(bs.size());
	for (tref b : bs) {
		b = tau::trim_right_sibling(b);
		const std::optional<pin<node>> hit =
			cof_pin_in<node>(members<node>(b), x, c);
		if (!hit) return {};      // ALL-OR-NOTHING (§3)
		// `x ∉ FV(tᵢ)` comes with `usable` (§1, the leaf hazard).
		DBG(assert(!fv_meets<node>(hit->witness, block{ x }));)
		out.emplace_back(b, hit->witness);
	}
	return out;
}

/// `x` free in this node? The block form of the cached test (dag.h).
template <NodeType node>
bool holds(tref n, tref x) {
	return fv_meets<node>(n, block{ x });
}

} // namespace witness_detail

// --- TRY_WITNESS, spelled mode ---------------------------------------------------

template <NodeType node>
std::optional<tref> try_witness(tref x, tref psi) {
	using namespace witness_detail;
	DBG(assert(psi != nullptr && x != nullptr);)
	DBG(assert(!holds_bdd_term<node>(psi));)
	// The SPELLED match on every conjunct, under the empty order: phase 2 is
	// the plain regime, where nothing is BDD-backed.
	return witness_with<node>(x, psi, {}, [x](const trefs& cs) {
		return best_pin<node>(cs,
			[x](tref m) { return find_pin_for<node>(m, x); });
	});
}

// --- TRY_WITNESS, COF mode -------------------------------------------------------

template <NodeType node>
std::optional<tref> try_witness(tref x, tref psi, ctx<node>& c) {
	using namespace witness_detail;
	DBG(assert(psi != nullptr && x != nullptr);)
	// The same tail over the COF match (§6), under the component's order:
	// the substitution rewrites a BDD-backed conjunct through the library's
	// compose, and the pinning conjunct's image is the residual `p = 0`.
	return witness_with<node>(x, psi, c.order, [x, &c](const trefs& cs) {
		return cof_pin_in<node>(cs, x, c);
	});
}

// --- TRY_WITNESS_DEEP, phase 2's deep pass ---------------------------------------

template <NodeType node>
std::optional<tref> try_witness_deep(quantifier<node> Q, tref x, tref phi) {
	using tau = tree<node>;
	using namespace witness_detail;
	DBG(assert(phi != nullptr && x != nullptr);)
	DBG(assert(!holds_bdd_term<node>(phi));)
	const bool ex = Q == tau_term_bdd<node>::ex;
	const size_t spine_nt = ex ? tau::wff_and : tau::wff_or;
	const size_t other_nt = ex ? tau::wff_or : tau::wff_and;
	block D;               // bound past the first kind flip, barred from FV(t)
	bool flipped = false;
	// The rewrite is a replacement inside this node, so a right sibling it
	// may carry as a chain member is not part of it.
	const tref root = tau::trim_right_sibling(phi);
	tref n = root;
	for (;;) {
		const bool is_spine = is_child<node>(n, spine_nt);
		const bool is_other = is_child<node>(n, other_nt);
		// EVERY node is a spine (§3): the spine's own connective hands
		// over its members, ANY OTHER NODE IS A ONE-MEMBER SPINE, and
		// the two tests below fire on it exactly as they would among
		// siblings. What is left after the pin is then the EMPTY join:
		// `T` for `∃`, `F` for `∀`.
		const trefs ms = is_spine ? members<node>(n) : trefs{ n };
		// A PLAIN PIN: the first member that pins `x` strictly with a
		// witness clear of `D`. The spine becomes its other members,
		// each `[x ← t]`, joined in its own connective; the pin itself
		// is dropped.
		for (size_t i = 0; i < ms.size(); ++i) {
			std::optional<pin<node>> p =
				strict_pin_of<node>(ms[i], x, Q);
			if (!p || fv_meets<node>(p->witness, D)) continue;
			// (a) a strict witness is `x`-free by `usable`.
			DBG(assert(!holds<node>(p->witness, x));)
			trefs rest;
			rest.reserve(ms.size() - 1);
			for (size_t j = 0; j < ms.size(); ++j)
				if (j != i) rest.push_back(subst_var<node>(
					ms[j], x, p->witness));
			return rewriter::replace<node>(root, n,
				join_of<node>(rest, ex));
		}
		// A CASE PIN: the WHOLE member must avoid `D`, not only the
		// witnesses — past a kind flip the branch CHOICE may not depend
		// on the inner variable (§3). The spine becomes
		// `⋁ᵢ ((dᵢ minus its pin) ∧ (spine minus M))[x ← tᵢ]`, dualised
		// for `∀`; on a one-member spine there is nothing to carry and
		// it is `⋁ᵢ (dᵢ minus its pin)[x ← tᵢ]`.
		for (size_t i = 0; i < ms.size(); ++i) {
			if (fv_meets<node>(ms[i], D)) continue;
			auto bs = case_pin_of<node>(ms[i], x, Q);
			if (!bs) continue;
			trefs cases;
			cases.reserve(bs->size());
			for (const case_branch<node>& b : *bs) {
				trefs parts;
				parts.reserve(ms.size());
				parts.push_back(subst_var<node>(
					b.residue, x, b.witness));
				for (size_t j = 0; j < ms.size(); ++j)
					if (j != i) parts.push_back(
						subst_var<node>(ms[j],
							x, b.witness));
				cases.push_back(join_of<node>(parts, ex));
			}
			return rewriter::replace<node>(root, n,
				join_of<node>(cases, !ex));
		}
		// THE DESCENT, one rule for both connectives: CONFINEMENT (b)
		// refuses a junction with `x` free in two members — a rewrite
		// below it would leave an occurrence outside the node it
		// touches — and the `x`-free siblings of the one member that
		// holds `x` ride along outside `Qx`.
		if (is_spine || is_other) {
			const trefs kids = is_spine ? ms : members<node>(n);
			tref into = nullptr;
			for (tref m : kids) if (holds<node>(m, x)) {
				if (into) return {};
				into = m;
			}
			if (!into) return {};   // nothing left to descend to
			n = into;
			continue;
		}
		if (is_child_quantifier<node>(n)) {
			// (c) `Q`-kind variables bound BEFORE the first flip
			// commute above `Qx` and stay usable in `t`; everything
			// bound after it joins `D`.
			if (binder_kind<node>(n) != Q) flipped = true;
			if (flipped) D.push_back(binder_var<node>(n));
			n = binder_body<node>(n);
			continue;
		}
		// An atom without a pin, a reference, a temporal operator: not
		// descended (§3). A `¬` over a non-atom cannot occur post-NNF.
		return {};
	}
}

// --- TRY_CASE_WITNESS, the matcher -----------------------------------------------

template <NodeType node>
std::optional<case_witness<node>> try_case_witness(tref x, tref psi) {
	using namespace witness_detail;
	DBG(assert(psi != nullptr && x != nullptr);)
	DBG(assert(!holds_bdd_term<node>(psi));)
	const trefs cs = get_cnf_wff_clauses<node>(psi);
	// SMALLEST `|D|` FIRST (§3): one scan over the conjuncts, the smallest
	// case pin kept. `|·|` is `formula_size` (dag.h).
	std::vector<case_branch<node>> best;
	size_t at = 0, best_size = 0;
	for (size_t i = 0; i < cs.size(); ++i) {
		auto bs = case_pin_of<node>(cs[i], x, tau_term_bdd<node>::ex);
		if (!bs) continue;
		const size_t size = formula_size<node>(cs[i]);
		if (best.empty() || size < best_size)
			best = std::move(*bs), at = i, best_size = size;
	}
	if (best.empty()) return {};
	case_witness<node> r;
	r.branches.reserve(best.size());
	// The branch AS IT STANDS: at phase 4 its pin stays and becomes the
	// residual under `[x ← tᵢ]` (§3 `TRY_WITNESS`). No copy is built here.
	for (const case_branch<node>& b : best)
		r.branches.emplace_back(b.branch, b.witness);
	trefs rest;
	rest.reserve(cs.size() - 1);
	for (size_t i = 0; i < cs.size(); ++i)
		if (i != at) rest.push_back(cs[i]);
	r.rest = simplified_and_join<node>(rest);
	return r;
}

// --- TRY_CASE_WITNESS, COF mode --------------------------------------------------

template <NodeType node>
std::optional<case_witness<node>> try_case_witness(tref x, tref psi,
	ctx<node>& c)
{
	using namespace witness_detail;
	DBG(assert(psi != nullptr && x != nullptr);)
	const trefs cs = get_cnf_wff_clauses<node>(psi);
	// SMALLEST `|D|` FIRST (§3): one scan over the conjuncts, the smallest
	// case pin kept. `|·|` is `formula_size` (dag.h).
	std::optional<std::vector<std::pair<tref, tref>>> best;
	size_t at = 0, best_size = 0;
	for (size_t i = 0; i < cs.size(); ++i) {
		auto bs = cof_case_pin_of<node>(cs[i], x, c);
		if (!bs) continue;
		const size_t size = formula_size<node>(cs[i]);
		if (!best || size < best_size)
			best = std::move(bs), at = i, best_size = size;
	}
	if (!best) return {};
	case_witness<node> r;
	// The branches AS THEY STAND, with the witnesses their pins give; no
	// copy of the rewrite is built here (§3).
	r.branches = std::move(*best);
	trefs rest;
	rest.reserve(cs.size() - 1);
	for (size_t i = 0; i < cs.size(); ++i)
		if (i != at) rest.push_back(cs[i]);
	r.rest = simplified_and_join<node>(rest);
	return r;
}

// --- ELIMINATE_BY_SUBSTITUTION ---------------------------------------------------

template <NodeType node>
tref eliminate_by_substitution(tref phi) {
	DBG(assert(phi != nullptr);)
	DBG(assert(!witness_detail::holds_bdd_term<node>(phi));)
	// The default memo slot is the traversal's own per-call one
	// (normal_forms_transformations.h): the result at a binder node is a
	// pure function of the node, so a shared binder is tried once.
	auto down = [](tref n) {
		// A rewrite can leave another binder AT THE TOP of what it
		// returns — `∃x.(x = t ∧ ∃w.ψ)` gives `∃w.ψ[x ← t]` — while
		// the traversal resumes BELOW the node it just transformed,
		// never on it. So the attempt repeats here until nothing fires.
		// It terminates: every round deletes one binder.
		while (is_child_quantifier<node>(n)) {
			std::optional<tref> r = try_witness_deep<node>(
				binder_kind<node>(n), binder_var<node>(n),
				binder_body<node>(n));
			if (!r) break;
			n = *r;
		}
		return n;
	};
	auto into_formulas = [](tref n) { return while_is_formula<node>(n); };
	return pre_order<node>(tree<node>::trim_right_sibling(phi))
		.apply_unique(down, into_formulas);
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__WITNESS__WITNESS_TMPL_H__
