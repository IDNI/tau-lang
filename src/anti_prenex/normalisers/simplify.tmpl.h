// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file simplify.tmpl.h
 * @brief Template implementations for simplify.h (package S). Included by
 * simplify.h. simplify.h says what each function means; the comments here say
 * how it is built.
 *
 * The propagation pass is ONE `pre_order::apply` — the NON-unique variant, so
 * that `down` and `up` pair up exactly and the marker stack is sound. (The
 * memoising variant consults its memo on the node `down` returned and then
 * skips `up`, which a marker stack cannot survive; nnf.h documents that
 * asymmetry.) The pass carries a memo of its own, keyed by (node, environment
 * version), because a node's result depends on the pins in force on its path
 * (§10): a shared subtree met under two environments is two computations.
 *
 * The state — frames with an undo mark, an environment version, a marker
 * stack, the one-shot `no_descend` flag — is the path sweep's idiom
 * (heuristics/syntactic_path_simplification.tmpl.h), which is the same walk
 * one pass later.
 *
 * What is reused from layer 0: `term_of`, `simplify_term`, `simplify_atom`,
 * `mem_size` (terms.h); `fv_meets`, `is_literal`, `binder_var` (dag.h);
 * `tree::substitute` for every rewrite; `get_leaves` for the member view of
 * an open conjunction, with its spine.
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

// `syntactic_path_simplification_unchanged_negations` (heuristics/) is
// `SIMPLIFY`'s second pass; normal_forms.h is the header that assembles the
// heuristics in the right order, as terms.tmpl.h does for its own.
#include "normal_forms.h"

namespace idni::tau_lang::anti_prenexing {

namespace detail {

/// `y` — a trimmed variable node — free in `t`? `get_free_vars` is sorted by
/// `subtree_less`, so this is a binary search over a cached set (§1).
template <NodeType node>
bool free_in(tref t, tref y) {
	const trefs& fv = get_free_vars<node>(t);
	return std::binary_search(fv.begin(), fv.end(), y,
		tree<node>::subtree_less);
}

/// A POSITIVE equation: a `wff` whose child is `bf_eq`. An equation under `¬`
/// is a `wff_neg` wrapper and fails the same test (§3: it pins nothing).
template <NodeType node>
bool is_positive_equation(tref n) {
	return is_child<node>(n, tree<node>::bf_eq);
}

/// The substitution key of a variable: its `bf` TERM, not the bare
/// `variable` node `get_free_vars` hands out. `tree::substitute` matches a
/// key by content exactly as given, and every occurrence inside a term is the
/// wrapped form — a bare key would replace the node inside the wrapper and
/// nest one `bf` in another.
template <NodeType node>
tref term_key(tref var) {
	return tree<node>::get(tree<node>::bf, var);
}

} // namespace detail

// --- the pin match ---------------------------------------------------------------

template <NodeType node>
std::optional<pin<node>> find_pin(tref atom, const block& X,
	const var_order<node>& order)
{
	using tau = tree<node>;
	if (atom == nullptr || !detail::is_positive_equation<node>(atom))
		return {};
	// ORIENTATION (§3): only an `X`-free conjunct propagates — pushing a
	// block variable into a leaf would break the child-is-cofactor
	// identity, and `TRY_WITNESS` eliminates such a variable instead. It
	// also makes every free variable below `X`-free, so the scan needs no
	// second guard.
	if (fv_meets<node>(atom, X)) return {};
	tref f = term_of<node>(atom, order);
	if (f == nullptr) return {};
	// THE RING SUM IS SPELLED OUT FIRST. `TERM_OF` builds `l + r`, a
	// `bf_xor` node in the plain regime, and the plain `SIMPLIFY_TERM` —
	// the path simplifier — knows no xor algebra: it leaves `0 + a` and
	// `1 + a` as they are, so neither cofactor folds and no atom would
	// ever pin. `A + B ↦ A·B′ ∪ A′·B` puts the term in the ∧/∨/′ algebra
	// the cofactors, the union test and the witness all need. A BDD-backed
	// term holds no `bf_xor` node, so in the BDD regime this is a no-op
	// walk and the cofactors come from the BDD as before.
	f = apply_all_xor_def<node>(f);
	const size_t type = find_ba_type<node>(f);
	// By value: the loop builds nodes, and a reference into
	// `get_free_vars`' table does not survive a collection (dag.h).
	const trefs vars = get_free_vars<node>(f);
	std::optional<pin<node>> best;
	size_t best_size = 0;
	for (tref y : vars) {
		const tref key = detail::term_key<node>(y);
		const tref f0 = simplify_term<node>(
			tau::get(f).substitute(key, _0<node>(type), order),
			order);
		const tref f1 = simplify_term<node>(
			tau::get(f).substitute(key, _1<node>(type), order),
			order);
		// `usable`: `y` gone from both cofactors. Where it is not, `y`
		// still hides in a subterm the substitution did not reach and
		// the record must not be used (§1, the leaf hazard).
		if (detail::free_in<node>(f0, y) || detail::free_in<node>(f1, y))
			continue;
		// Boole's expansion puts the zeros of `f` at `f₀ ≤ y ≤ f₁′`,
		// an interval that `f₀ ∪ f₁ = 1` collapses to a point.
		if (!tau::get(simplify_term<node>(build_bf_or<node>(f0, f1),
			order)).equals_1()) continue;
		// The witness is the LOWER end `f₁′`: it carries the residual
		// into every sibling's terms, where `p = 0` then folds
		// syntactically (§3).
		const tref witness = simplify_term<node>(
			build_bf_neg<node>(f1), order);
		const tref p = simplify_term<node>(
			build_bf_and<node>(f0, f1), order);
		// A STRICT pin ends the scan: it is the cheapest of all, and
		// the spec takes one without comparing witnesses.
		if (tau::get(p).equals_0())
			return pin<node>{ y, witness, true };
		if (const size_t size = mem_size<node>(witness);
			!best || size < best_size)
				best = pin<node>{ y, witness, false },
				best_size = size;
	}
	return best;
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

	propagation(const var_order<node>& order, bool ref_args)
		: order(order), ref_args(ref_args)
	{
		// §1: the block in scope IS the live order's key set. Sorted,
		// so that the pass reads the same whatever order the map hands
		// its keys out in.
		X.reserve(order.size());
		for (const auto& [var, rank] : order) X.push_back(var);
		std::sort(X.begin(), X.end(), tau::subtree_less);
	}

	tref run(tref phi) {
		auto f = [this](tref n) { return down(n); };
		auto visit = [this](tref n) {
			// The sweep's one-shot flag: `down` sets it for a node
			// it finished, and the traversal asks right after, so
			// it is always consumed by the node that set it.
			if (no_descend) return no_descend = false, false;
			return while_is_formula<node>(n);
		};
		auto u = [this](tref r) { return up(r); };
		// NOT `apply_unique`: its memo hit skips `up` while `down` has
		// already run, which no marker stack survives. The pass
		// memoises on its own, per environment version.
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
	/// What stage 1 of a frame learned about one equation leaf: the form
	/// it was matched in, the pin it got (if any) and the size of `pins`
	/// right after its own step, which tells stage 2 whether anything
	/// joined the environment after it.
	struct matched_leaf {
		tref first = nullptr;
		std::optional<size_t> own;
		size_t pins_after = 0;
	};
	/// One open conjunction. `pending` holds the rewritten form of every
	/// equation member, by POSITION (a member carries its right sibling,
	/// so the key is the node as it occurs), and `spine` the inner chain
	/// wrappers, which must not open a frame of their own.
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

	const var_order<node>& order;
	const bool ref_args;
	block X;

	std::vector<pin_state> pins;
	/// The ACTIVE pins, in `substitute`'s own parameter form.
	subtree_map<node, tref> changes;
	size_t witness_sum = 0;         ///< `Σ‖witness‖` over `pins`
	size_t atom_sum = 0;            ///< `Σ‖TERM_OF(pinning atom)‖`
	std::vector<undo_record> undo;
	std::vector<std::vector<size_t>> suspensions;
	std::vector<marker> markers;
	std::vector<frame> frames;
	memo_t memo;
	subtree_unordered_map<node, std::optional<pin<node>>> pin_memo;
	size_t version = 0;
	size_t next_version = 0;
	bool no_descend = false;

	// --- rewriting ---------------------------------------------------------

	/// §3: an atom rewritten by the environment in force and re-emitted
	/// through `SIMPLIFY_ATOM` — which unwraps one `¬` itself — whenever
	/// the environment changed it, or always in `ref_args` mode, which is
	/// what establishes invariant 6 in phase 1.
	tref rewrite_atom(tref a) {
		const tref bare = tau::trim_right_sibling(a);
		const tref res = changes.empty() ? bare
			: tau::get(bare).substitute(changes, order);
		if (res != bare || ref_args)
			return simplify_atom<node>(res, order);
		return bare;
	}

	/// `rewrite_atom` with ONE pin taken out of the environment for this
	/// substitution: a PINNING conjunct is rewritten by every pin but its
	/// own (§3). Its own would fold it to `T` — `f[y ← f₁′]` IS the
	/// residual `p = 0`, which is `T` for a strict pin — and drop the
	/// constraint the spec keeps in place, weak pins' residual included.
	tref rewrite_atom_excluding(tref a, std::optional<size_t> own) {
		if (!own || !pins[*own].active) return rewrite_atom(a);
		const tref key = pins[*own].key;
		const tref witness = pins[*own].witness;
		changes.erase(key);
		const tref res = rewrite_atom(a);
		changes[key] = witness;
		return res;
	}

	/// `ref_args` mode: every `bf` argument of a reference through
	/// `SIMPLIFY_TERM`, never through a pin (§3). Post-order, so a
	/// reference nested inside an argument is finished first.
	tref simplify_reference_arguments(tref n) {
		auto f = [this](tref m) {
			const tau& t = tau::get(m);
			if (!t.is(tau::ref_arg)) return m;
			const tref a = tau::trim_right_sibling(t.first());
			if (!tau::get(a).is(tau::bf)) return m;
			const tref s = simplify_term<node>(a, order);
			return s == a ? m : tau::get(t.value, s);
		};
		return post_order<node>(n).apply_unique(f);
	}

	// --- the environment ---------------------------------------------------

	const std::optional<pin<node>>& matched(tref atom) {
		if (auto it = pin_memo.find(atom); it != pin_memo.end())
			return it->second;
		return pin_memo.emplace(atom,
			find_pin<node>(atom, X, order)).first->second;
	}

	/// §3: admit the pin of `atom`, if it has one and the cap allows it.
	/// @return the index of the pin it admitted in `pins`, or `nullopt`.
	/// Stage 2 needs it to leave a conjunct out of its OWN pin.
	std::optional<size_t> try_admit(tref atom) {
		const std::optional<pin<node>> p = matched(atom);
		if (!p) return {};
		const tref key = term_key<node>(p->var);
		// A variable already pinned IN FORCE cannot be pinned again:
		// its occurrences are rewritten before they are ever matched.
		// (One only SUSPENDED may be, and the two undo in order.)
		if (changes.contains(key)) return {};
		// The new range under the environment in force, so that the
		// environment stays idempotent.
		tref w = p->witness;
		if (!changes.empty()) w = simplify_term<node>(
			tau::get(w).substitute(changes, order), order);
		// and every range in force rewritten by the new pin.
		subtree_map<node, tref> one;
		one.emplace(key, w);
		std::vector<std::pair<size_t, tref>> rewritten;
		size_t new_witness_sum = mem_size<node>(w);
		for (size_t i = 0; i < pins.size(); ++i) {
			tref r = pins[i].witness;
			if (pins[i].active) {
				r = tau::get(r).substitute(one, order);
				if (r != pins[i].witness)
					r = simplify_term<node>(r, order);
			}
			if (r == pins[i].witness) {
				new_witness_sum += pins[i].size;
				continue;
			}
			new_witness_sum += mem_size<node>(r);
			rewritten.emplace_back(i, r);
		}
		// THE CAP (§3), measured after the insertion: once the ranges
		// outgrow the equations that licensed them, no further pin is
		// admitted in this pass. Precision, never soundness.
		const size_t new_atom_sum = atom_sum
			+ mem_size<node>(term_of<node>(atom, order));
		if (new_witness_sum > propagate_growth * new_atom_sum) return {};
		for (const auto& [i, r] : rewritten) {
			undo.push_back({ i, false, pins[i].witness,
				pins[i].size });
			pins[i].witness = r;
			pins[i].size = mem_size<node>(r);
			if (pins[i].active) changes[pins[i].key] = r;
		}
		undo.push_back({ pins.size(), true, nullptr, 0 });
		pins.push_back(pin_state{ p->var, key, w, true,
			mem_size<node>(w) });
		changes[key] = w;
		witness_sum = new_witness_sum;
		atom_sum = new_atom_sum;
		version = ++next_version;
		return pins.size() - 1;
	}

	void undo_to(size_t mark) {
		while (undo.size() > mark) {
			const undo_record u = undo.back();
			undo.pop_back();
			if (u.added) {
				if (pins.back().active)
					changes.erase(pins.back().key);
				pins.pop_back();
				continue;
			}
			pins[u.index].witness = u.witness;
			pins[u.index].size = u.size;
			if (pins[u.index].active)
				changes[pins[u.index].key] = u.witness;
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
		DBG(assert(m.version == version);)
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
		marker m{ n, plain, version };
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
	 * Each is rewritten by the environment in force so far and offered to
	 * `try_admit`, so a chained pin joins for the leaves after it and the
	 * environment stays normalised on insert. That first form exists for
	 * the pin test alone.
	 *
	 * STAGE 2, REWRITE: EVERY positive equation of the frame, X-touching
	 * ones included, rewritten from the ORIGINAL leaf under the FINAL
	 * environment — one substitution and one `SIMPLIFY_ATOM` per equation
	 * — with the leaf's OWN pin left out. Its own pin would fold it to
	 * `T` and drop the constraint the spec keeps in place; every OTHER
	 * pin applies, so `z = y` under a later `y ↦ a` becomes `z = a`, which
	 * is what the normalised environment already says. A leaf that saw no
	 * admission after its own keeps its stage-1 form: the environment
	 * minus its own pin is the one that form was computed under.
	 *
	 * The non-equation members are untouched here; the traversal rewrites
	 * them under the environment this leaves behind.
	 */
	tref open_frame(tref n, marker m) {
		frame fr;
		fr.undo_mark = undo.size();
		fr.version_before = version;
		fr.witness_sum_before = witness_sum;
		fr.atom_sum_before = atom_sum;
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
		std::unordered_map<tref, matched_leaf> stage1;
		for (tref e : candidates) {
			const tref first = rewrite_atom(e);
			// A leaf that only BECOMES a pin once rewritten joins
			// the environment like any other (§3).
			const std::optional<size_t> own = try_admit(first);
			stage1.emplace(e, matched_leaf{ first, own,
				pins.size() });
		}
		// --- stage 2 -----------------------------------------------
		for (tref e : eqs) {
			const auto it = stage1.find(e);
			if (it != stage1.end()
				&& it->second.pins_after == pins.size()) {
				frames.back().pending.emplace(e,
					it->second.first);
				continue;
			}
			frames.back().pending.emplace(e,
				rewrite_atom_excluding(e, it == stage1.end()
					? std::nullopt : it->second.own));
		}
		return n;
	}

	/// §4: `SIMPLIFY` descends into a unit's body, with a pin SUSPENDED
	/// under a binder over its variable or over a variable of its witness.
	/// `substitute` is applied per atom below this node and cannot see the
	/// binder, so the pass is what keeps the capture out.
	tref enter_binder(tref n, marker m) {
		const tref v = tau::trim_right_sibling(binder_var<node>(n));
		std::vector<size_t> suspended;
		for (size_t i = 0; i < pins.size(); ++i) {
			if (!pins[i].active) continue;
			if (!tau::subtree_equals(pins[i].var, v)
				&& !free_in<node>(pins[i].witness, v)) continue;
			pins[i].active = false;
			changes.erase(pins[i].key);
			suspended.push_back(i);
		}
		if (!suspended.empty()) version = ++next_version;
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
				pins[i].active = true;
				changes[pins[i].key] = pins[i].witness;
			}
			suspensions.pop_back();
			version = m.version;
			break;
		}
		case frame_top: {
			const frame fr = std::move(frames.back());
			frames.pop_back();
			undo_to(fr.undo_mark);
			version = fr.version_before;
			witness_sum = fr.witness_sum_before;
			atom_sum = fr.atom_sum_before;
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
tref simplify(tref phi, const var_order<node>& order, bool ref_args) {
	DBG(assert(phi != nullptr);)
	// A term input has nothing to propagate — the traversal's continuation
	// predicate stops on it and hands it back — and goes straight to the
	// sweep.
	const tref propagated = detail::propagation<node>(order, ref_args)
		.run(phi);
	return syntactic_path_simplification_unchanged_negations<node>(
		propagated);
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__NORMALISERS__SIMPLIFY_TMPL_H__
