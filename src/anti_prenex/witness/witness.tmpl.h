// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file witness.tmpl.h
 * @brief Template implementations for witness.h (layer 2). witness.h says
 * what each function means; the comments here say how it is built.
 *
 * The pieces come from the layers below: `find_pin_for` and `simplify`
 * (normalisers/simplify.h), the result joins (normalisers/joins.h),
 * `members`, `fv_meets`, `formula_size`, `is_negated_equation`, `atom_of` and
 * the binder accessors (foundations/dag.h), `mem_size` and `simplify_term`
 * (foundations/terms.h), `case_max` (foundations/options.h).
 *
 * THE TWO REWRITES of the deep pass are the library's, never a hand-rolled
 * walk: `φ[x ← t]` is `tree<node>::substitute` (tau_tree.h) with the empty
 * order — the plain regime of phase 2 — and a re-simplifying argument hook,
 * which is the one thing that dirties a reference argument and is asked to
 * clean up after itself (§1, invariant 6); the spine's replacement is
 * `rewriter::replace`, a CONTENT match with a unique-cached, hooked rebuild
 * of the path above it. Replacing every occurrence of the spine is sound
 * exactly because the descent enforced confinement first (§3, condition (b)):
 * a second occurrence of that node elsewhere in `Φ` would put `x` free in two
 * members of some junction on the way down, which the descent refuses. That
 * is also how the "no private stack" rule is met — the descent is a loop
 * along one child, the rebuild is the library's.
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

/// §3 `φ[x ← t]`: the library's substitution under the EMPTY order — phase 2
/// is the plain regime — with the argument hook invariant 6 asks for.
/// Substitution is what dirties a reference argument from phase 2 on (§1), so
/// it re-simplifies the arguments it changed; `SIMPLIFY` after it does not
/// (only phase 1 passes `ref_args`). `key` is the variable's `bf` TERM, the
/// form every occurrence takes inside a term.
template <NodeType node>
tref subst_var(tref phi, tref key, tref t) {
	auto on_argument = [](tref a) { return simplify_term<node>(a); };
	return tree<node>::get(tree<node>::trim_right_sibling(phi))
		.substitute(key, t, {}, on_argument);
}

/// The result join of `ms` in the connective `conj` picks (§3): the ∧-join
/// for a `wff_and` spine, the ∨-join for a `wff_or` one. An empty list is the
/// connective's neutral constant — what a spine minus its only other member
/// leaves behind.
template <NodeType node>
tref join_of(const trefs& ms, bool conj) {
	return conj ? simplified_and_join<node>(ms) : simplified_or_join<node>(ms);
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
		return tau::build_bf_eq(tau::trim_right_sibling(t.first()),
			tau::trim_right_sibling(t.second()));
	}
	return nullptr;
}

/// The STRICT pin of `x` in one member, in `Q`'s form, or `nullopt` (§3). A
/// weak pin is NO pin for the deep pass: the member is dropped, so the
/// residual `p = 0` it keeps would be lost. `find_pin_for` (layer 1) is the
/// match itself; the empty order is phase 2's.
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
 * The match stays at `m`'s TOP branches deliberately (§3): expanding nested
 * structure into deeper cases would let guard disjunctions that merely
 * CONTAIN solving atoms qualify. A branch that is a unit is opaque (§4) — its
 * member view is the unit itself, which is no equation — so it never
 * qualifies, and nothing here descends into one.
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
	m = tau::trim_right_sibling(m);
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
			if (i != at) rest.push_back(
				tau::trim_right_sibling(ps[i]));
		out.push_back(case_branch<node>{ b,
			join_of<node>(rest, ex), hit->witness });
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
	using tau = tree<node>;
	using namespace witness_detail;
	DBG(assert(psi != nullptr && x != nullptr);)
	DBG(assert(!holds_bdd_term<node>(psi));)
	// The top-level conjuncts: the member view of a conjunction, else `ψ`
	// itself — a `wff_or` is ONE conjunct here, not a member list.
	const trefs cs = is_child<node>(psi, tau::wff_and)
		? members<node>(psi) : trefs{ psi };
	// Several pins on `x`: a STRICT one first, else the smallest `‖f₁′‖`
	// (§3) — the cost every later substitution of the witness pays.
	std::optional<pin<node>> best;
	size_t best_size = 0;
	for (tref c : cs) {
		std::optional<pin<node>> p = find_pin_for<node>(
			tau::trim_right_sibling(c), x);
		if (!p) continue;
		if (p->strict) { best = p; break; }
		if (const size_t size = mem_size<node>(p->witness);
			!best || size < best_size)
				best = p, best_size = size;
	}
	if (!best) return {};
	// EVERY conjunct, the pinning one included: its image is the residual
	// `p = 0`, `T` for a strict pin, so no site emits a residual on its own
	// (§3). One substitution over `ψ` reaches them all. The result is
	// SIMPLIFIED here (invariant 6).
	return simplify<node>(subst_var<node>(psi,
		detail::term_key<node>(x), best->witness));
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
	const tref key = detail::term_key<node>(x);
	block D;               // bound past the first kind flip, barred from FV(t)
	bool flipped = false;
	// The rewrite is a replacement inside this node, so a right sibling it
	// may carry as a chain member is not part of it.
	const tref root = tau::trim_right_sibling(phi);
	tref n = root;
	for (;;) {
		const bool is_spine = is_child<node>(n, spine_nt);
		const bool is_other = is_child<node>(n, other_nt);
		// EVERY node is a spine (§3: a flattened conjunction for
		// `Q = ∃`, a disjunction for `Q = ∀`). One of the spine's own
		// connective hands over its members; ANY OTHER NODE IS A
		// ONE-MEMBER SPINE, and the two tests below fire on it exactly
		// as they would among siblings — which is why the loop's last
		// arm reads "an atom WITHOUT a pin". What is left after the pin
		// is then the EMPTY join: `T` for `∃`, `F` for `∀`.
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
					ms[j], key, p->witness));
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
					b.residue, key, b.witness));
				for (size_t j = 0; j < ms.size(); ++j)
					if (j != i) parts.push_back(
						subst_var<node>(ms[j],
							key, b.witness));
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
			n = tau::trim_right_sibling(into);
			continue;
		}
		if (is_child_quantifier<node>(n)) {
			// (c) `Q`-kind variables bound BEFORE the first flip
			// commute above `Qx` and stay usable in `t`; everything
			// bound after it joins `D`.
			if (binder_kind<node>(n) != Q) flipped = true;
			if (flipped) D.push_back(binder_var<node>(n));
			n = tau::trim_right_sibling(binder_body<node>(n));
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
	using tau = tree<node>;
	using namespace witness_detail;
	DBG(assert(psi != nullptr && x != nullptr);)
	DBG(assert(!holds_bdd_term<node>(psi));)
	const trefs cs = is_child<node>(psi, tau::wff_and)
		? members<node>(psi) : trefs{ psi };
	// SMALLEST `|D|` FIRST (§3): one scan over the conjuncts, the smallest
	// case pin kept. `|·|` is `formula_size` (dag.h).
	std::vector<case_branch<node>> best;
	size_t at = 0, best_size = 0;
	for (size_t i = 0; i < cs.size(); ++i) {
		auto bs = case_pin_of<node>(cs[i], x, tau_term_bdd<node>::ex);
		if (!bs) continue;
		const size_t size = formula_size<node>(
			tau::trim_right_sibling(cs[i]));
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
		if (i != at) rest.push_back(tau::trim_right_sibling(cs[i]));
	r.rest = simplified_and_join<node>(rest);
	return r;
}

// --- ELIMINATE_BY_SUBSTITUTION ---------------------------------------------------

template <NodeType node>
tref eliminate_by_substitution(tref phi) {
	using tau = tree<node>;
	DBG(assert(phi != nullptr);)
	DBG(assert(!witness_detail::holds_bdd_term<node>(phi));)
	// The default memo slot is the traversal's own per-call one
	// (normal_forms_transformations.h): the result at a binder node is a
	// pure function of the node, so a shared binder is tried once. The
	// walk continues into what `down` returns, which is how a binder the
	// rewrite exposed is reached in the same pass.
	auto down = [](tref n) {
		// A rewrite can leave another binder AT THE TOP of what it
		// returns — `∃x.(x = t ∧ ∃w.ψ)` gives `∃w.ψ[x ← t]` — and the
		// traversal resumes below the node it just transformed, never
		// on it. So the attempt repeats here until nothing fires; the
		// traversal covers everything under that, which is what "the
		// walk continues into the replacement" means. It terminates:
		// every round deletes one binder.
		while (is_child_quantifier<node>(n)) {
			std::optional<tref> r = try_witness_deep<node>(
				binder_kind<node>(n), binder_var<node>(n),
				tau::trim_right_sibling(binder_body<node>(n)));
			if (!r) break;
			n = tau::trim_right_sibling(*r);
		}
		return n;
	};
	auto into_formulas = [](tref n) { return while_is_formula<node>(n); };
	return pre_order<node>(tree<node>::trim_right_sibling(phi))
		.apply_unique(down, into_formulas);
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__WITNESS__WITNESS_TMPL_H__
