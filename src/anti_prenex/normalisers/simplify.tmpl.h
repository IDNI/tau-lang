// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file simplify.tmpl.h
 * @brief Template implementations for simplify.h. simplify.h says what each
 * function means; the comments here say how it is built.
 *
 * The propagation pass is ONE `pre_order::apply` — the NON-memoising variant,
 * because a node's result depends on the pins in force on its path (§10): a
 * shared subtree met under two environments is two computations, which no
 * memo keyed by the node can hold. The pass carries a memo of its own, keyed
 * by (node, environment version).
 *
 * Its state — frames with an undo mark, an environment version, a marker
 * stack and the one-shot `no_descend` flag — follows the path sweep
 * (heuristics/syntactic_path_simplification.tmpl.h), which is the same walk
 * one pass later.
 *
 * The pieces come from the foundations: `term_of`, `simplify_term`,
 * `simplify_atom`, `mem_size` (terms.h); `fv_meets`, `free_in`, `is_literal`,
 * `binder_var` (dag.h); `cof` (cofactors.h) for the backed pin match, and
 * `memoised` over `plain_cof_memo` (ctx.h) for the plain one's cofactor
 * records; `tree::substitute` with the argument hook
 * `resimplify_argument` (subst.h) for every rewrite;
 * `get_leaves` for the member view of an open conjunction, with its spine.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__NORMALISERS__SIMPLIFY_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__NORMALISERS__SIMPLIFY_TMPL_H__

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// `SIMPLIFY`'s second pass is
// `syntactic_path_simplification_unchanged_negations` (heuristics/), reached
// through the one header that assembles the heuristics in the right order.
#include "normal_forms.h"

namespace idni::tau_lang::anti_prenexing {

namespace detail {

/// A POSITIVE equation: a `wff` whose child is `bf_eq`. An equation under `¬`
/// is a `wff_neg` wrapper and fails the same test (§3: it pins nothing).
template <NodeType node>
bool is_positive_equation(tref n) {
	return is_child<node>(n, tree<node>::bf_eq);
}

} // namespace detail

// --- the pin match ---------------------------------------------------------------

namespace detail {

/// The term the pin match reads off an atom: `TERM_OF` in the regime @p rg.
/// In the plain regime its ring sum is SPELLED OUT first: `TERM_OF` builds
/// `l + r` as a `bf_xor` node there, and the plain `SIMPLIFY_TERM` knows no
/// xor algebra: it leaves `0 + a` and `1 + a` as they are, so no cofactor
/// folds and no atom pins. `A + B ↦ A·B′ ∪ A′·B` puts the term in the ∧/∨/′
/// algebra the cofactors, the union test and the witness need. `nullptr`
/// when the atom cannot pin at all: it is no positive equation, or it touches
/// `X`.
template <NodeType node>
tref pin_term(tref atom, const block& X, regime rg) {
	if (atom == nullptr || !is_positive_equation<node>(atom)) return nullptr;
	// ORIENTATION (§3): only an `X`-free conjunct propagates — pushing a
	// block variable into a leaf would break the child-is-cofactor
	// identity, and `TRY_WITNESS` eliminates such a variable instead. It
	// also makes every free variable below `X`-free, so the scan needs no
	// second guard.
	if (fv_meets<node>(atom, X)) return nullptr;
	const tref f = term_of<node>(atom, rg);
	if (f == nullptr || rg == regime::backed) return f;
	return apply_all_xor_def<node>(f);
}

/// The pin a FREE variable `y` has in the term `f`, or `nullopt` (§3): the
/// per-variable half of the match, which `find_strict_pin` asks of every
/// free variable and `find_pin_for` of one.
///
/// ONE MATCH, two recipes by regime. On backed terms it is `COF`'s
/// (cofactors.h): restricts, the record holding the witness and the
/// strictness. In the plain regime the cofactors are taken by SUBSTITUTION
/// and tested by `pin_from_cofactors`, the record MEMOISED in
/// `plain_cof_memo` (§1) under `(f, y)`. The table is a cache, absent in
/// Debug builds, so nothing here depends on a hit.
template <NodeType node>
std::optional<pin<node>> pin_of_var(tref f, tref y, regime rg) {
	using tau = tree<node>;
	cof_entry e;
	if (rg == regime::backed) e = cof<node>(f, y);
	else e = memoised<node, table::plain_cof_memo>({ f, y }, [&] {
		const size_t type = find_ba_type<node>(f);
		const tref key = tau::get(tau::bf, y);
		const tref f0 = simplify_term<node>(tau::get(f).substitute(key,
			_0<node>(type), resimplify_argument<node>()));
		const tref f1 = simplify_term<node>(tau::get(f).substitute(key,
			_1<node>(type), resimplify_argument<node>()));
		return pin_from_cofactors<node>(f0, f1, y);
	});
	if (!e.pin) return {};
	return pin<node>{ y, e.witness, e.strict };
}

} // namespace detail

// --- the plain pin recipe --------------------------------------------------------

template <NodeType node>
cof_entry pin_from_cofactors(tref f0, tref f1, tref y) {
	using tau = tree<node>;
	cof_entry e{ f0, f1 };
	// `usable`: `y` gone from both cofactors. Where it is not, `y` still
	// hides in a subterm no cofactor reached and the record must not be
	// used (§1, the leaf hazard).
	if (free_in<node>(f0, y) || free_in<node>(f1, y)) return e;
	e.usable = true;
	// Boole's expansion puts the zeros of the term at `f₀ ≤ y ≤ f₁′`, an
	// interval that `f₀ ∪ f₁ = 1` collapses to a point.
	e.pin = tau::get(simplify_term<node>(build_bf_or<node>(f0, f1)))
		.equals_1();
	// `p` is the pin's own residual equation; a non-pin reads none of it.
	if (!e.pin) return e;
	e.p = simplify_term<node>(build_bf_and<node>(f0, f1));
	// The witness is the LOWER end `f₁′`: it carries the residual into
	// every sibling's terms, where `p = 0` then folds syntactically (§3).
	e.witness = simplify_term<node>(build_bf_neg<node>(f1));
	e.strict = tau::get(e.p).equals_0();
	return e;
}

template <NodeType node>
std::optional<pin<node>> find_strict_pin(tref atom, const block& X,
	regime rg)
{
	const tref f = detail::pin_term<node>(atom, X, rg);
	if (f == nullptr) return {};
	// By value: the loop builds nodes, and a reference into
	// `get_free_vars`' table does not survive a collection (dag.h).
	const trefs vars = get_free_vars<node>(f);
	// A strict pin makes the cofactors complements, a Boolean identity that
	// holds under every assignment of the leaves: one assignment breaking it
	// rules `y` out before its plain record costs a sweep per cofactor.
	std::optional<terms_detail::cofactor_refuter<node>> refuter;
	if (rg == regime::plain) refuter.emplace(f);
	for (tref y : vars) {
		if (refuter && !refuter->complementary(
			tree<node>::get(tree<node>::bf, y))) continue;
		std::optional<pin<node>> p = detail::pin_of_var<node>(f, y, rg);
		if (p && p->strict) return p;
	}
	return {};
}

template <NodeType node>
std::optional<pin<node>> find_pin_for(tref atom, tref x, regime rg) {
	// A term built from the atom holds no variable the atom lacks.
	if (!free_in<node>(atom, x)) return {};
	// The block is empty here (simplify.h): phase 2 has none, and a later
	// caller hands in an atom its own guard already found `X`-free.
	const tref f = detail::pin_term<node>(atom, {}, rg);
	if (f == nullptr) return {};
	// `find_strict_pin` asks only about free variables, so the per-variable
	// half may assume it; a variable that does not occur is no pin of the
	// atom.
	if (!free_in<node>(f, x)) return {};
	return detail::pin_of_var<node>(f, x, rg);
}

namespace detail {

/**
 * @brief §3 `SIMPLIFY`'s first pass: equality propagation, one `pre_order`
 * traversal per call.
 *
 * DEEP, ONE PASS, CHAINED (§3). A conjunction is handled in TWO STAGES before
 * the traversal descends into it: its X-free equation members are MATCHED in
 * content order, each under the pins admitted before it, so chained pins
 * propagate within the pass; then every equation member is REWRITTEN under the
 * environment those matches leave behind, minus its own pin. So the order of
 * the matches decides which pins exist, and no member's final form depends on
 * where it sorted. The environment is kept idempotent — a new witness is
 * rewritten by the pins in force, and the ranges in force are rewritten by the
 * new pin — and CAPPED by `propagate_growth`. It is still not a fixpoint: a
 * pin that only surfaces once the sweep has folded something waits for the
 * next construction site.
 */
template <NodeType node>
struct propagation {
	using tau = tree<node>;

	propagation(regime rg, const block& X, bool ref_args)
		: rg(rg), X(X), ref_args(ref_args) {}

	tref run(tref phi) {
		root = phi;
		auto f = [this](tref n) { return down(n); };
		auto visit = [this](tref n) {
			// The sweep's one-shot flag: `down` sets it for a node
			// it finished, and the traversal asks right after, so
			// it is always consumed by the node that set it.
			if (no_descend) return no_descend = false, false;
			return while_is_formula<node>(n);
		};
		auto u = [this](tref r) { return up(r); };
		// `apply`, not `apply_unique`: the file header says why.
		tref res = pre_order<node>(phi).apply(f, visit, u);
		DBG(assert(markers.empty() && frames.empty());)
		return res;
	}

private:
	// --- the environment ---------------------------------------------------
	/// One admitted pin. `witness` is its range, rewritten whenever a
	/// later pin joins; `active` is false while a binder over its variable
	/// or over a variable of its witness is open.
	struct pin_state {
		tref var = nullptr;         ///< the bare `variable` node
		tref key = nullptr;         ///< its `bf` term, the substitution key
		tref witness = nullptr;
		bool active = true;
		size_t size = 0;            ///< `‖witness‖`, for the cap
	};
	/// Undoing one admission: either the pin added last (`added`) or the
	/// previous range of the pin at `index`.
	struct undo_record {
		size_t index = 0;
		bool added = false;
		tref witness = nullptr;
		size_t size = 0;
	};
	enum kind_t : std::uint8_t { plain, spine, leaf_, binder_, frame_top };
	struct marker {
		tref orig = nullptr;
		kind_t kind = plain;
		size_t version = 0;
	};
	/// What one admission did: the index of the pin it added, and whether
	/// it also REWROTE a range in force. An admission that did changes the
	/// environment for the leaves before it too, so its own leaf's first
	/// form was computed under an environment that no longer exists —
	/// which is what stage 2's shortcut has to watch for.
	struct admission {
		size_t index = 0;
		bool rewrote_ranges = false;
	};
	/// What stage 1 of a frame learned about one equation leaf: the form
	/// it was matched in, the admission it produced (if any) and the size
	/// of the pin list right after its own step, which tells stage 2
	/// whether anything joined the environment after it.
	struct matched_leaf {
		tref first = nullptr;
		std::optional<admission> own;
		size_t pins_after = 0;
	};
	/// One open conjunction. `pending` holds the rewritten form of every
	/// equation member, by POSITION (a member carries its right sibling,
	/// so the key is the node as it occurs), and `spine` the inner chain
	/// wrappers, which must not open a frame of their own.
	///
	/// BY POSITION, NOT BY CONTENT, in both. The input is not
	/// deduplicated, so one equation may stand at two positions of the
	/// chain, and each is matched and rewritten on its own — the second
	/// under the pin the first admits. And a node content-equal to a
	/// member but standing somewhere else below the frame, inside a
	/// disjunct say, is no member of it: the traversal rewrites that one
	/// under the environment in force where it stands.
	struct frame {
		size_t undo_mark = 0;
		size_t version_before = 0;
		size_t witness_sum_before = 0;
		size_t atom_sum_before = 0;
		std::unordered_set<tref> spine;
		std::unordered_map<tref, tref> pending;
	};
	/// The pass's own memo: (node, environment version) → result (§10).
	using memo_t = std::unordered_map<std::pair<tref, size_t>, tref,
		subtree_pair_hash<node, size_t>,
		subtree_pair_equal<node, size_t>>;

	const regime rg;
	const block& X;
	const bool ref_args;

	/// THE PIN ENVIRONMENT: the admitted pins, the active ones in
	/// `substitute`'s own parameter form, and the two sums the cap
	/// compares (§3). A value type, because the pass holds one LIVE
	/// environment, `env`, while stage 2 re-derives a SCRATCH copy of it
	/// per pinning leaf (`open_frame`).
	struct environment {
		std::vector<pin_state> pins;
		subtree_map<node, tref> changes;
		size_t witness_sum = 0;   ///< `Σ‖witness‖` over `pins`
		size_t atom_sum = 0;      ///< `Σ‖TERM_OF(pinning atom)‖`
		/// A fresh number at every change, live or scratch, so one
		/// version is one content: the key of the memo and of the
		/// prepared substitutions.
		size_t version = 0;
	};

	/// The pass's input. Every atom the pass rewrites is a node of it, so
	/// its largest id bounds theirs: the environment is prepared against it.
	tref root = nullptr;
	environment env;
	std::vector<undo_record> undo;
	std::vector<std::vector<size_t>> suspensions;
	std::vector<marker> markers;
	std::vector<frame> frames;
	memo_t memo;
	subtree_unordered_map<node, std::optional<pin<node>>> pin_memo;
	/// Environment version → its changes prepared as one substitution,
	/// whose spellings, compose list and argument memo every atom
	/// rewritten at that version shares.
	std::unordered_map<size_t, typename tau::substitution> prepared;
#ifdef DEBUG
	/// The changes each version was prepared from.
	std::unordered_map<size_t, subtree_map<node, tref>> prepared_from;
#endif
	size_t next_version = 0;
	bool no_descend = false;

	// --- rewriting ---------------------------------------------------------

	/// §3: an atom rewritten by the environment `e` and re-emitted through
	/// `SIMPLIFY_ATOM` — which unwraps one `¬` itself — whenever the
	/// environment changed it, or always in `ref_args` mode. A pin reaches
	/// a reference argument like any other occurrence, and the substitution
	/// re-simplifies an argument it changed (`resimplify_argument`). In
	/// `ref_args` mode EVERY reference argument inside the atom's terms
	/// goes through `SIMPLIFY_TERM` besides: a reference is a leaf to the
	/// atom simplifier, so nothing else reaches them.
	tref rewrite_under(const environment& e, tref a) {
		const tref bare = tau::trim_right_sibling(a);
		tref res = e.changes.empty() ? bare
			: tau::substitute(bare, prepared_for(e),
				resimplify_argument<node>());
		if (ref_args) res = simplify_reference_arguments(res);
		if (res != bare || ref_args)
			return simplify_atom<node>(res, rg);
		return bare;
	}

	/// The same under the LIVE environment, which is what every caller
	/// outside stage 2's exclusion wants.
	tref rewrite_atom(tref a) { return rewrite_under(env, a); }

	/**
	 * @brief §3: a PINNING conjunct rewritten by every pin but its OWN.
	 * Its own would erase it — `f[y ← f₁′]` IS the residual `p = 0`, which
	 * a strict pin folds to `T`, taking the constraint on `y` with it.
	 *
	 * The exclusion is a RE-DERIVATION, not a mask: stage 1 runs again
	 * over `candidates`, in the same content order and from the same
	 * starting environment `before`, with `leaf` LEFT OUT and its own
	 * VARIABLE barred from being pinned at all, and the ORIGINAL leaf is
	 * rewritten under what that leaves. Masking the leaf's own index in
	 * the final environment is not enough: admitting a pin NORMALISES the
	 * ranges in force, so a later admission can fold this leaf's own pin
	 * INTO another pin's range, where a mask does not reach it, and the
	 * conjunct is then rewritten by its own equation and folds to `T`.
	 *
	 * BARRING THE KEY is the other half. Without the leaf, a sibling may
	 * pin the SAME variable with a different witness, and rewriting the
	 * leaf by that pin erases the leaf's own constraint just as its own
	 * pin would — in `y = a·b ∧ y = c·d` it would leave neither conjunct
	 * mentioning `y`. In the live environment a variable is pinned once,
	 * so "every pin but its own" is exactly "every pin on another
	 * variable", which is what the bar reproduces.
	 *
	 * Cost: one re-derivation per pinning leaf the shortcut does not
	 * cover. `find_strict_pin` is memoised per atom for the whole call, so
	 * a candidate whose rewritten form is unchanged is a lookup.
	 */
	tref rewrite_atom_excluding(const environment& before,
		const trefs& candidates, tref leaf, tref own_key)
	{
		const size_t scratch_from = next_version;
		environment e = before;
		for (tref c : candidates) {
			// One conjunct is left out, not every copy of it: a
			// content-equal candidate at another position is still
			// a conjunct of the frame.
			if (c == leaf) continue;
			admit_into(e, rewrite_under(e, c), nullptr, own_key);
		}
		const tref res = rewrite_under(e, leaf);
		// The scratch versions are never met again.
		for (size_t v = scratch_from + 1; v <= next_version; ++v) {
			prepared.erase(v);
			DBG(prepared_from.erase(v);)
		}
		return res;
	}

	/// The environment `e` prepared as one substitution, once per version
	/// (§3), against the pass's root.
	const typename tau::substitution& prepared_for(const environment& e) {
		auto it = prepared.find(e.version);
		if (it == prepared.end()) {
			it = prepared.emplace(e.version,
				tau::prepare(root, e.changes)).first;
			DBG(prepared_from.emplace(e.version, e.changes);)
		}
		// A prepared environment serves the version it was prepared at.
		DBG(assert(prepared_from.at(e.version) == e.changes);)
		return it->second;
	}

	/// `ref_args` mode: every `bf` argument of a reference below `n`
	/// through `SIMPLIFY_TERM`, changed by a pin or not (§3). `n` is a
	/// `wff_ref` node, or an atom whose terms may hold a `bf_ref`
	/// (`rewrite_under`). Post-order, so a reference nested inside an
	/// argument is finished first.
	tref simplify_reference_arguments(tref n) {
		auto f = [this](tref m) {
			const tau& t = tau::get(m);
			if (!t.is(tau::ref_arg)) return m;
			const tref a = t.first();
			if (!tau::get(a).is(tau::bf)) return m;
			const tref s = simplify_term<node>(a, rg);
			return s == a ? m : tau::get(t.value, s);
		};
		return post_order<node>(n).apply_unique(f);
	}

	// --- the environment ---------------------------------------------------

	const std::optional<pin<node>>& matched(tref atom) {
		if (auto it = pin_memo.find(atom); it != pin_memo.end())
			return it->second;
		return pin_memo.emplace(atom,
			find_strict_pin<node>(atom, X, rg)).first->second;
	}

	/**
	 * @brief §3: admit the pin of `atom` into the environment `e`, if it
	 * has one and the cap allows it.
	 *
	 * The environment is EXPLICIT because stage 2's exclusion re-derives
	 * one from scratch: `e` is the live `env` when `rec` is the pass's
	 * `undo` list, and a throw-away copy when `rec` is null — a scratch
	 * run records nothing, since nothing has to be undone. `blocked`, when
	 * given, is a substitution key no pin may take: the variable of the
	 * leaf an exclusion is re-deriving without (`rewrite_atom_excluding`
	 * says why).
	 *
	 * @return what the admission did — the pin's index, and whether it
	 *         also REWROTE a range in force — or `nullopt` when nothing
	 *         was admitted.
	 */
	std::optional<admission> admit_into(environment& e, tref atom,
		std::vector<undo_record>* rec, tref blocked = nullptr)
	{
		const std::optional<pin<node>> p = matched(atom);
		if (!p) return {};
		const tref key = tau::get(tau::bf, p->var);
		if (blocked && tau::subtree_equals(key, blocked)) return {};
		// A variable already pinned IN FORCE cannot be pinned again:
		// its occurrences are rewritten before they are ever matched.
		// (One only SUSPENDED may be, and the two undo in order.)
		if (e.changes.contains(key)) return {};
		// The new range under the environment in force, so that the
		// environment stays idempotent.
		tref w = p->witness;
		if (!e.changes.empty()) w = simplify_term<node>(
			tau::substitute(w, prepared_for(e),
				resimplify_argument<node>()), rg);
		// and every range in force rewritten by the new pin, prepared
		// once for all of them.
		subtree_map<node, tref> one;
		one.emplace(key, w);
		std::optional<typename tau::substitution> by_new;
		std::vector<std::pair<size_t, tref>> rewritten;
		size_t new_witness_sum = mem_size<node>(w);
		for (size_t i = 0; i < e.pins.size(); ++i) {
			tref r = e.pins[i].witness;
			if (e.pins[i].active) {
				if (!by_new) by_new = tau::prepare(root, one);
				r = tau::substitute(r, *by_new,
					resimplify_argument<node>());
				if (r != e.pins[i].witness)
					r = simplify_term<node>(r, rg);
			}
			if (r == e.pins[i].witness) {
				new_witness_sum += e.pins[i].size;
				continue;
			}
			new_witness_sum += mem_size<node>(r);
			rewritten.emplace_back(i, r);
		}
		// THE CAP (§3), measured with this pin in and the ranges
		// rewritten by it: a pin that would push the ranges past the
		// factor times the equations that licensed them is refused, and
		// the match goes on to the next candidate. Precision, never
		// soundness.
		const size_t new_atom_sum = e.atom_sum
			+ mem_size<node>(term_of<node>(atom, rg));
		if (new_witness_sum > propagate_growth * new_atom_sum) return {};
		for (const auto& [i, r] : rewritten) {
			if (rec) rec->push_back({ i, false, e.pins[i].witness,
				e.pins[i].size });
			e.pins[i].witness = r;
			e.pins[i].size = mem_size<node>(r);
			if (e.pins[i].active) e.changes[e.pins[i].key] = r;
		}
		if (rec) rec->push_back({ e.pins.size(), true, nullptr, 0 });
		e.pins.push_back(pin_state{ p->var, key, w, true,
			mem_size<node>(w) });
		e.changes[key] = w;
		e.witness_sum = new_witness_sum;
		e.atom_sum = new_atom_sum;
		e.version = ++next_version;
		return admission{ e.pins.size() - 1, !rewritten.empty() };
	}

	void undo_to(size_t mark) {
		while (undo.size() > mark) {
			const undo_record u = undo.back();
			undo.pop_back();
			if (u.added) {
				if (env.pins.back().active)
					env.changes.erase(env.pins.back().key);
				env.pins.pop_back();
				continue;
			}
			env.pins[u.index].witness = u.witness;
			env.pins[u.index].size = u.size;
			if (env.pins[u.index].active)
				env.changes[env.pins[u.index].key] = u.witness;
		}
	}

	// --- the traversal ------------------------------------------------------

	const tref* memo_find(const marker& m) const {
		const auto it = memo.find(std::make_pair(m.orig, m.version));
		return it == memo.end() ? nullptr : &it->second;
	}

	void memo_store(const marker& m, tref res) {
		// `up` has restored whatever the node's own descent changed, so
		// the environment is the one `down` saw.
		DBG(assert(m.version == env.version);)
		memo.emplace(std::make_pair(m.orig, m.version), res);
	}

	/// A node whose subtree is not entered: its result is final here.
	tref finish(marker m, tref res) {
		m.kind = leaf_;
		markers.push_back(m);
		no_descend = true;
		return res;
	}

	tref down(tref n) {
		DBG(assert(!no_descend);)
		const tau& t = tau::get(n);
		marker m{ n, plain, env.version };
		// An operator node (`wff_and`, `wff_neg`, …) carries no state
		// of its own; the traversal rebuilds it from its children.
		if (!t.is(tau::wff)) return markers.push_back(m), n;
		if (!frames.empty()) {
			const frame& fr = frames.back();
			if (const auto it = fr.pending.find(n);
				it != fr.pending.end())
					return finish(m, it->second);
			if (fr.spine.contains(n))
				return m.kind = spine, markers.push_back(m), n;
		}
		if (const tref* hit = memo_find(m)) return finish(m, *hit);
		if (is_child<node>(n, tau::wff_and)) return open_frame(n, m);
		if (is_child_quantifier<node>(n)) return enter_binder(n, m);
		// Opaque to both passes (§3, §4): a temporal operator and a
		// reference are never entered — the latter except for its
		// arguments in `ref_args` mode.
		if (is_child_temporal_quantifier<node>(n)) return finish(m, n);
		if (is_child<node>(n, tau::wff_ref)) return finish(m,
			ref_args ? simplify_reference_arguments(n) : n);
		if (t.equals_T() || t.equals_F()) return finish(m, n);
		if (is_literal<node>(n)) return finish(m, rewrite_atom(n));
		// `wff_or`, `wff_neg` over a non-atom, a sugar connective: the
		// environment holds throughout, so the traversal descends.
		return markers.push_back(m), n;
	}

	/**
	 * @brief §3: the conjunction is where pins are matched and admitted,
	 * in TWO STAGES, both here, before the traversal descends.
	 *
	 * STAGE 1, MATCH: the X-FREE positive equations, in content order —
	 * an X-touching one can never pin (§3's orientation rule), since a
	 * witness is X-free and a substitution cannot remove a block variable.
	 * Each is rewritten by the environment in force so far and admitted
	 * into the live one, so a chained pin joins for the leaves after it
	 * and the environment stays normalised on insert. That first form
	 * exists for the pin test alone.
	 *
	 * STAGE 2, REWRITE: EVERY positive equation of the frame, X-touching
	 * ones included, rewritten from the ORIGINAL leaf — one substitution
	 * and one `SIMPLIFY_ATOM` per equation — with the leaf's OWN pin left
	 * out. Every OTHER pin applies, so `z = y` under a later `y ↦ a`
	 * becomes `z = a`, which is what the normalised environment already
	 * says. A leaf that admitted nothing takes the final environment as it
	 * stands; one that admitted a pin takes the environment RE-DERIVED
	 * without it (`rewrite_atom_excluding`), the environment that would
	 * exist had that leaf never been a candidate. It keeps its stage-1
	 * form, and needs no re-derivation at all, when nothing joined after
	 * its own step AND its own admission rewrote no range — exactly when
	 * the environment minus its own pin is still the one that form was
	 * computed under.
	 *
	 * The non-equation members are untouched here; the traversal rewrites
	 * them under the environment this leaves behind.
	 */
	tref open_frame(tref n, marker m) {
		frame fr;
		fr.undo_mark = undo.size();
		fr.version_before = env.version;
		fr.witness_sum_before = env.witness_sum;
		fr.atom_sum_before = env.atom_sum;
		trefs leaves, inner;
		get_leaves<node>(n, tau::wff_and, leaves, &inner);
		// The inner chain wrappers: `down` must not open a second frame
		// on one, and must not answer it from the memo either.
		for (tref s : inner)
			if (tau::get(s).is(tau::wff)) fr.spine.insert(s);
		frames.push_back(std::move(fr));
		m.kind = frame_top;
		markers.push_back(m);
		trefs eqs, candidates;
		for (tref l : leaves) {
			if (!is_positive_equation<node>(l)) continue;
			eqs.push_back(l);
			if (!fv_meets<node>(l, X)) candidates.push_back(l);
		}
		// --- stage 1 -----------------------------------------------
		std::sort(candidates.begin(), candidates.end(),
			tau::subtree_less);
		// The environment stage 1 starts from: what an exclusion has to
		// re-derive from (the outer pins, this frame's admissions not
		// yet among them). Copied only when there is a candidate.
		const environment before = candidates.empty() ? environment{}
							     : env;
		// What each candidate learns, by POSITION like `pending`: an
		// equation spelled at two positions is matched twice, the
		// second time under the pin the first admitted.
		std::unordered_map<tref, matched_leaf> stage1;
		for (tref e : candidates) {
			const tref first = rewrite_atom(e);
			// A leaf that only BECOMES a pin once rewritten joins
			// the environment like any other (§3).
			const std::optional<admission> own =
				admit_into(env, first, &undo);
			stage1.emplace(e, matched_leaf{ first, own,
				env.pins.size() });
		}
		// --- stage 2 -----------------------------------------------
		for (tref e : eqs) {
			const auto it = stage1.find(e);
			const std::optional<admission> own =
				it == stage1.end() ? std::nullopt
						   : it->second.own;
			// The shortcut: nothing joined the environment after
			// this leaf's own step, and its own admission left
			// every range in force alone, so the environment minus
			// its own pin IS the one its first form was computed
			// under.
			if (it != stage1.end()
				&& it->second.pins_after == env.pins.size()
				&& !(own && own->rewrote_ranges)) {
				frames.back().pending.emplace(e,
					it->second.first);
				continue;
			}
			frames.back().pending.emplace(e, own
				? rewrite_atom_excluding(before, candidates, e,
					env.pins[own->index].key)
				: rewrite_atom(e));
		}
		return n;
	}

	/// §4: `SIMPLIFY` descends into a unit's body, with a pin SUSPENDED
	/// under a binder over its variable or over a variable of its witness.
	/// `substitute` is applied per atom below this node and cannot see the
	/// binder, so the pass is what keeps the capture out.
	tref enter_binder(tref n, marker m) {
		const tref v = binder_var<node>(n);
		std::vector<size_t> suspended;
		for (size_t i = 0; i < env.pins.size(); ++i) {
			if (!env.pins[i].active) continue;
			if (!tau::subtree_equals(env.pins[i].var, v)
				&& !free_in<node>(env.pins[i].witness, v)) continue;
			env.pins[i].active = false;
			env.changes.erase(env.pins[i].key);
			suspended.push_back(i);
		}
		if (!suspended.empty()) env.version = ++next_version;
		suspensions.push_back(std::move(suspended));
		m.kind = binder_;
		return markers.push_back(m), n;
	}

	tref up(tref r) {
		const marker m = markers.back();
		markers.pop_back();
		switch (m.kind) {
		case spine: return r;   // a position of the open chain, no state
		case binder_: {
			for (size_t i : suspensions.back()) {
				env.pins[i].active = true;
				env.changes[env.pins[i].key] = env.pins[i].witness;
			}
			suspensions.pop_back();
			env.version = m.version;
			break;
		}
		case frame_top: {
			const frame fr = std::move(frames.back());
			frames.pop_back();
			undo_to(fr.undo_mark);
			env.version = fr.version_before;
			env.witness_sum = fr.witness_sum_before;
			env.atom_sum = fr.atom_sum_before;
			break;
		}
		case plain:
		case leaf_: break;
		}
		// No sorting and no canonical rebuild (§10): `apply` has
		// rebuilt a changed parent through the hooks, keeping its
		// nesting.
		if (tau::get(m.orig).is(tau::wff)) memo_store(m, r);
		return r;
	}
};

} // namespace detail

template <NodeType node>
tref simplify(tref phi, regime rg, const block& X, bool ref_args) {
	DBG(assert(phi != nullptr);)
	// A term input has nothing to propagate — the traversal's continuation
	// predicate stops on it and hands it back — and goes straight to the
	// sweep.
	const tref propagated = detail::propagation<node>(rg, X, ref_args)
		.run(phi);
	// A compound guard is a sweep key with its `neg`, like a literal.
	return syntactic_path_simplification_unchanged_negations<node>(
		propagated, [](tref m) { return guard_neg<node>(m); });
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__NORMALISERS__SIMPLIFY_TMPL_H__
