// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file simplify.tmpl.h
 * @brief Template implementations for simplify.h. simplify.h says what each
 * function means; the comments here say how it is built.
 *
 * The propagation pass is ONE `pre_order::apply` — the NON-memoising variant,
 * so that `down` and `up` pair up exactly and the marker stack is sound. The
 * memoising variant consults its memo on the node `down` returned and skips
 * `up` on a hit, which no marker stack survives. The pass carries a memo of
 * its own, keyed by (node, environment version), because a node's result
 * depends on the pins in force on its path (§10): a shared subtree met under
 * two environments is two computations.
 *
 * Its state — frames with an undo mark, an environment version, a marker
 * stack and the one-shot `no_descend` flag — follows the path sweep
 * (heuristics/syntactic_path_simplification.tmpl.h), which is the same walk
 * one pass later.
 *
 * The pieces come from the foundations: `term_of`, `simplify_term`,
 * `simplify_atom`, `mem_size` (terms.h); `fv_meets`, `is_literal`,
 * `binder_var` (dag.h); `tree::substitute` for every rewrite; `get_leaves`
 * for the member view of an open conjunction, with its spine.
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

namespace detail {

/// The term the pin match reads off an atom: `TERM_OF`, with the ring sum
/// SPELLED OUT first. `TERM_OF` builds `l + r`, a `bf_xor` node in the plain
/// regime, and the plain `SIMPLIFY_TERM` knows no xor algebra: it leaves
/// `0 + a` and `1 + a` as they are, so no cofactor folds and no atom pins.
/// `A + B ↦ A·B′ ∪ A′·B` puts the term in the ∧/∨/′ algebra the cofactors,
/// the union test and the witness need. A BDD-backed term holds no `bf_xor`,
/// so in the BDD regime this walk changes nothing. `nullptr` when the atom
/// cannot pin at all: it is no positive equation, or it touches `X`.
template <NodeType node>
tref pin_term(tref atom, const block& X, const var_order<node>& order) {
	if (atom == nullptr || !is_positive_equation<node>(atom)) return nullptr;
	// ORIENTATION (§3): only an `X`-free conjunct propagates — pushing a
	// block variable into a leaf would break the child-is-cofactor
	// identity, and `TRY_WITNESS` eliminates such a variable instead. It
	// also makes every free variable below `X`-free, so the scan needs no
	// second guard.
	if (fv_meets<node>(atom, X)) return nullptr;
	const tref f = term_of<node>(atom, order);
	if (f == nullptr) return nullptr;
	return apply_all_xor_def<node>(f);
}

/// The pin a FREE variable `y` has in the prepared term `f` of BA type
/// `type`, or `nullopt` (§3): `usable ∧ f₀ ∪ f₁ = 1` on the two cofactors,
/// witness `f₁′`, STRICT when the residual `p = f₀f₁` folds to `0`. The
/// per-variable half of the match, which `find_pin` asks of every free
/// variable and `find_pin_for` of one.
template <NodeType node>
std::optional<pin<node>> pin_of_var(tref f, tref y, size_t type,
	const var_order<node>& order)
{
	using tau = tree<node>;
	const tref key = term_key<node>(y);
	const tref f0 = simplify_term<node>(
		tau::get(f).substitute(key, _0<node>(type), order), order);
	const tref f1 = simplify_term<node>(
		tau::get(f).substitute(key, _1<node>(type), order), order);
	// `usable`: `y` gone from both cofactors. Where it is not, `y` still
	// hides in a subterm the substitution did not reach and the record
	// must not be used (§1, the leaf hazard).
	if (free_in<node>(f0, y) || free_in<node>(f1, y)) return {};
	// Boole's expansion puts the zeros of `f` at `f₀ ≤ y ≤ f₁′`, an
	// interval that `f₀ ∪ f₁ = 1` collapses to a point.
	if (!tau::get(simplify_term<node>(build_bf_or<node>(f0, f1), order))
		.equals_1()) return {};
	// The witness is the LOWER end `f₁′`: it carries the residual into
	// every sibling's terms, where `p = 0` then folds syntactically (§3).
	const tref witness = simplify_term<node>(build_bf_neg<node>(f1), order);
	const tref p = simplify_term<node>(build_bf_and<node>(f0, f1), order);
	return pin<node>{ y, witness, tau::get(p).equals_0() };
}

} // namespace detail

template <NodeType node>
std::optional<pin<node>> find_pin(tref atom, const block& X,
	const var_order<node>& order)
{
	const tref f = detail::pin_term<node>(atom, X, order);
	if (f == nullptr) return {};
	const size_t type = find_ba_type<node>(f);
	// By value: the loop builds nodes, and a reference into
	// `get_free_vars`' table does not survive a collection (dag.h).
	const trefs vars = get_free_vars<node>(f);
	std::optional<pin<node>> best;
	size_t best_size = 0;
	for (tref y : vars) {
		std::optional<pin<node>> p =
			detail::pin_of_var<node>(f, y, type, order);
		if (!p) continue;
		// A STRICT pin ends the scan: §3 takes one without comparing
		// witnesses.
		if (p->strict) return p;
		if (const size_t size = mem_size<node>(p->witness);
			!best || size < best_size)
				best = p, best_size = size;
	}
	return best;
}

template <NodeType node>
std::optional<pin<node>> find_pin_for(tref atom, tref x,
	const var_order<node>& order)
{
	// The block is empty here (simplify.h): phase 2 has none, and a later
	// caller hands in an atom its own guard already found `X`-free.
	const tref f = detail::pin_term<node>(atom, {}, order);
	if (f == nullptr) return {};
	// `find_pin` asks only about free variables, so the per-variable half
	// may assume it; a variable that does not occur is no pin of the atom.
	if (!detail::free_in<node>(f, x)) return {};
	return detail::pin_of_var<node>(f, x, find_ba_type<node>(f), order);
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
	/// it also REWROTE a range in force. An admission that did env.changes the
	/// environment for the leaves before it too, so its own leaf's first
	/// form was computed under an environment that no longer exists —
	/// which is what stage 2's shortcut has to watch for.
	struct admission {
		size_t index = 0;
		bool rewrote_ranges = false;
	};
	/// What stage 1 of a frame learned about one equation leaf: the form
	/// it was matched in, the admission it produced (if any) and the size
	/// of `env.pins` right after its own step, which tells stage 2 whether
	/// anything joined the environment after it.
	struct matched_leaf {
		tref first = nullptr;
		std::optional<admission> own;
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

	/// THE PIN ENVIRONMENT: the admitted pins, the active ones in
	/// `substitute`'s own parameter form, and the two sums the cap
	/// compares (§3). The pass holds one LIVE environment, `env`, and
	/// stage 2 re-derives a SCRATCH copy of it per pinning leaf
	/// (`open_frame`), which is why this is a value type and not four
	/// members: an exclusion is a second derivation, not a mask.
	struct environment {
		std::vector<pin_state> pins;
		subtree_map<node, tref> changes;
		size_t witness_sum = 0;   ///< `Σ‖witness‖` over `pins`
		size_t atom_sum = 0;      ///< `Σ‖TERM_OF(pinning atom)‖`
	};

	environment env;
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

	/// §3: an atom rewritten by the environment `e` and re-emitted through
	/// `SIMPLIFY_ATOM` — which unwraps one `¬` itself — whenever the
	/// environment changed it, or always in `ref_args` mode, which is what
	/// establishes invariant 6 in phase 1.
	tref rewrite_under(const environment& e, tref a) {
		const tref bare = tau::trim_right_sibling(a);
		const tref res = e.changes.empty() ? bare
			: tau::get(bare).substitute(e.changes, order);
		if (res != bare || ref_args)
			return simplify_atom<node>(res, order);
		return bare;
	}

	/// The same under the LIVE environment, which is what every caller
	/// outside stage 2's exclusion wants.
	tref rewrite_atom(tref a) { return rewrite_under(env, a); }

	/**
	 * @brief §3: a PINNING conjunct rewritten by every pin but its OWN.
	 * Its own would erase it — `f[y ← f₁′]` IS the residual `p = 0`, so a
	 * strict pin folds the conjunct to `T`, taking the constraint on `y`
	 * with it, a weak pin's residual included.
	 *
	 * The exclusion is a RE-DERIVATION, not a mask: stage 1 is run again
	 * over `candidates`, in the same content order and from the same
	 * starting environment `before`, with `leaf` LEFT OUT and its own
	 * VARIABLE barred from being pinned at all, and the ORIGINAL leaf is
	 * rewritten under what that leaves.
	 *
	 * Masking the leaf's own index in the final environment was unsound:
	 * admitting a pin NORMALISES the ranges in force, so a later admission
	 * can fold this leaf's own pin INTO another pin's range, where masking
	 * does not reach it — the conjunct was then rewritten by its own
	 * equation and folded to `T` (`p = q ∧ r = p` came back as `q = p`,
	 * with `r` unconstrained).
	 *
	 * BARRING THE KEY is the other half. Without the leaf, a sibling may
	 * pin the SAME variable with a different witness, and rewriting the
	 * leaf by that pin erases the leaf's own constraint just as its own
	 * pin would — in `y = a·b ∧ y ∪ z = 0` it would leave neither conjunct
	 * mentioning `y`. In the live environment a variable is pinned once,
	 * so "every pin but its own" is exactly "every pin on another
	 * variable", and that is what the bar reproduces.
	 *
	 * Cost: one re-derivation per pinning leaf the shortcut does not
	 * cover. `find_pin` is memoised per atom for the whole call, so a
	 * candidate whose rewritten form is unchanged is a lookup.
	 */
	tref rewrite_atom_excluding(const environment& before,
		const trefs& candidates, tref leaf, tref own_key)
	{
		environment e = before;
		for (tref c : candidates) {
			if (c == leaf) continue;
			admit_into(e, rewrite_under(e, c), nullptr, own_key);
		}
		return rewrite_under(e, leaf);
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

	/**
	 * @brief §3: admit the pin of `atom` into the environment `e`, if it
	 * has one and the cap allows it.
	 *
	 * It works on an EXPLICIT environment because stage 2's exclusion
	 * re-derives one from scratch: `e` is the live `env` when `rec` is
	 * the pass's `undo` list, and a throw-away copy when `rec` is null —
	 * a scratch run records nothing, since nothing has to be undone.
	 * `blocked`, when given, is a substitution key no pin may take: the
	 * variable of the leaf an exclusion is re-deriving without
	 * (`rewrite_atom_excluding` says why).
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
		const tref key = term_key<node>(p->var);
		if (blocked && tau::subtree_equals(key, blocked)) return {};
		// A variable already pinned IN FORCE cannot be pinned again:
		// its occurrences are rewritten before they are ever matched.
		// (One only SUSPENDED may be, and the two undo in order.)
		if (e.changes.contains(key)) return {};
		// The new range under the environment in force, so that the
		// environment stays idempotent.
		tref w = p->witness;
		if (!e.changes.empty()) w = simplify_term<node>(
			tau::get(w).substitute(e.changes, order), order);
		// and every range in force rewritten by the new pin.
		subtree_map<node, tref> one;
		one.emplace(key, w);
		std::vector<std::pair<size_t, tref>> rewritten;
		size_t new_witness_sum = mem_size<node>(w);
		for (size_t i = 0; i < e.pins.size(); ++i) {
			tref r = e.pins[i].witness;
			if (e.pins[i].active) {
				r = tau::get(r).substitute(one, order);
				if (r != e.pins[i].witness)
					r = simplify_term<node>(r, order);
			}
			if (r == e.pins[i].witness) {
				new_witness_sum += e.pins[i].size;
				continue;
			}
			new_witness_sum += mem_size<node>(r);
			rewritten.emplace_back(i, r);
		}
		// THE CAP (§3), measured after the insertion: once the ranges
		// outgrow the equations that licensed them, no further pin is
		// admitted in this pass. Precision, never soundness.
		const size_t new_atom_sum = e.atom_sum
			+ mem_size<node>(term_of<node>(atom, order));
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
		return admission{ e.pins.size() - 1, !rewritten.empty() };
	}

	/// The admission into the LIVE environment: it records its undo and
	/// bumps the version the pass's memo is keyed by.
	std::optional<admission> try_admit(tref atom) {
		const std::optional<admission> a =
			admit_into(env, atom, &undo);
		if (a) version = ++next_version;
		return a;
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
	 * @brief §3: the conjunction is where env.pins are matched and admitted,
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
	 * ones included, rewritten from the ORIGINAL leaf — one substitution
	 * and one `SIMPLIFY_ATOM` per equation — with the leaf's OWN pin left
	 * out. Every OTHER pin applies, so `z = y` under a later `y ↦ a`
	 * becomes `z = a`, which is what the normalised environment already
	 * says. A leaf that admitted nothing takes the final environment as it
	 * stands; one that admitted a pin takes the environment RE-DERIVED
	 * without it (`rewrite_atom_excluding`) — the environment that would
	 * exist had that leaf never been a candidate, which is NOT the final
	 * one with its index masked: a later admission normalises the ranges
	 * in force, so the leaf's own pin can already sit inside another pin's
	 * range, and masking one key would let the conjunct be rewritten by
	 * its own equation and fold to `T`, dropping what it constrained.
	 * A leaf keeps its stage-1 form, and needs no re-derivation at all,
	 * when nothing joined after its own step AND its own admission rewrote
	 * no range — exactly when the environment minus its own pin is still
	 * the one that form was computed under.
	 *
	 * The non-equation members are untouched here; the traversal rewrites
	 * them under the environment this leaves behind.
	 */
	tref open_frame(tref n, marker m) {
		frame fr;
		fr.undo_mark = undo.size();
		fr.version_before = version;
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
		std::unordered_map<tref, matched_leaf> stage1;
		for (tref e : candidates) {
			const tref first = rewrite_atom(e);
			// A leaf that only BECOMES a pin once rewritten joins
			// the environment like any other (§3).
			const std::optional<admission> own = try_admit(first);
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
			// A leaf that admitted nothing has no own pin to leave
			// out and takes the final environment; one that did is
			// rewritten under the environment RE-DERIVED without
			// it.
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
		const tref v = tau::trim_right_sibling(binder_var<node>(n));
		std::vector<size_t> suspended;
		for (size_t i = 0; i < env.pins.size(); ++i) {
			if (!env.pins[i].active) continue;
			if (!tau::subtree_equals(env.pins[i].var, v)
				&& !free_in<node>(env.pins[i].witness, v)) continue;
			env.pins[i].active = false;
			env.changes.erase(env.pins[i].key);
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
				env.pins[i].active = true;
				env.changes[env.pins[i].key] = env.pins[i].witness;
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
