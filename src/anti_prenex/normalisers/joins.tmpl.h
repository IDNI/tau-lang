// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file joins.tmpl.h
 * @brief Template implementations for joins.h. joins.h says what each
 * function means; the comments here say how it is built.
 *
 * The pieces come from the foundations: `members` for the flattening and for
 * the parts of a member, `is_literal` and `complement_of` for the unit
 * elimination, `canonical_and`/`canonical_or` for the emission, and
 * `subtree_unordered_set`/`subtree_unordered_map` for every set and index —
 * all keyed by CONTENT, which is what a member carrying a right sibling
 * needs.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__NORMALISERS__JOINS_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__NORMALISERS__JOINS_TMPL_H__

#include <cassert>
#include <utility>
#include <vector>

namespace idni::tau_lang::anti_prenexing {

namespace detail {

/**
 * @brief §3 absorption: the PARTS of a member. The members of the DUAL
 * connective — an ∨-node inside an ∧-join — and `{m}` for anything else.
 *
 * Only a dual-connective member can be absorbed (it has more than one part);
 * any member can absorb. `members` (dag.h) is asked for the dual connective
 * explicitly: a member of the join's OWN connective cannot occur, the insert
 * rules having flattened it away.
 */
template <NodeType node, bool conj>
trefs parts_of(tref m) {
	using tau = tree<node>;
	constexpr auto dual = conj ? tau::wff_or : tau::wff_and;
	if (is_child<node>(m, dual)) return members<node>(m);
	return trefs{ m };
}

/**
 * @brief §3 the absorption pass of the result joins, `d ∧ (d ∨ e) = d`,
 * indexed the way SAT subsumption is.
 *
 * An occurrence index maps a part to the members holding it, built once per
 * pass. A member `d` looks for what it absorbs among the members of its
 * RAREST part alone, and absorbs `m′` — a member with strictly MORE parts —
 * iff `parts(d) ⊆ parts(m′)`, tested by marking: the parts of every member
 * are held as a set, once per member, and `d`'s parts are looked up in it.
 *
 * `absorb_occ_max` (options.h) is the cost guard: a part occurring in more
 * members than the limit is no candidate key, so a member all of whose parts
 * exceed it stays unabsorbed. That costs precision, never soundness.
 *
 * Strictly more parts keeps the relation acyclic: two members with the same
 * part set never absorb each other. An already absorbed absorber is skipped —
 * by transitivity of ⊆ its own absorber absorbs whatever it would, and that
 * absorber's key passed the occurrence test too.
 */
template <NodeType node, bool conj>
trefs absorb(const trefs& ms) {
	if (ms.size() < 2) return ms;
	// The parts of every member, deduplicated, as a list (to iterate, for
	// an absorber) and as a set (to mark, for an absorbed candidate).
	std::vector<trefs> parts(ms.size());
	std::vector<subtree_unordered_set<node>> marked(ms.size());
	subtree_unordered_map<node, std::vector<size_t>> occ;
	for (size_t i = 0; i < ms.size(); ++i)
		for (tref p : parts_of<node, conj>(ms[i]))
			if (marked[i].insert(p).second) {
				parts[i].push_back(p);
				occ[p].push_back(i);
			}
	std::vector<bool> absorbed(ms.size(), false);
	for (size_t d = 0; d < ms.size(); ++d) {
		if (absorbed[d]) continue;
		const std::vector<size_t>* key = nullptr;
		for (tref p : parts[d]) {
			const std::vector<size_t>& o = occ.find(p)->second;
			if (!key || o.size() < key->size()) key = &o;
		}
		if (key == nullptr || key->size() > absorb_occ_max) continue;
		for (size_t m : *key) {
			if (m == d || absorbed[m]) continue;
			if (parts[m].size() <= parts[d].size()) continue;
			bool subset = true;
			for (tref p : parts[d])
				if (!marked[m].contains(p)) {
					subset = false;
					break;
				}
			if (subset) absorbed[m] = true;
		}
	}
	trefs survivors;
	survivors.reserve(ms.size());
	for (size_t i = 0; i < ms.size(); ++i)
		if (!absorbed[i]) survivors.push_back(ms[i]);
	return survivors;
}

} // namespace detail

// --- the builder ----------------------------------------------------------------

template <NodeType node, bool conj>
bool join_builder<node, conj>::insert(tref r) {
	using tau = tree<node>;
	DBG(assert(r != nullptr && tau::get(r).is(tau::wff));)
	if (is_decided) return true;   // a decided join takes nothing further
	constexpr auto own = conj ? tau::wff_and : tau::wff_or;
	// Rule 3: a same-connective operand contributes its members. ONE level
	// is all there is to flatten — `members` applies associativity in both
	// nesting directions, so no member of the view is that connective
	// again — and the loop stops as soon as one of them decides.
	if (is_child<node>(r, own)) {
		for (tref m : members<node>(r)) if (insert_member(m)) break;
		return is_decided;
	}
	return insert_member(r);
}

template <NodeType node, bool conj>
bool join_builder<node, conj>::insert_member(tref m) {
	using tau = tree<node>;
	if (is_decided) return true;
	const tree<node>& t = tau::get(m);
	// Rules 1 and 2: the neutral constant is skipped, the deciding one
	// decides. The hooks fold both as well, one construction later.
	if (conj ? t.equals_T() : t.equals_F()) return false;
	if (conj ? t.equals_F() : t.equals_T()) return is_decided = true;
	// Rule 4: already a member, in O(1) against the content-keyed set.
	if (present.contains(m)) return false;
	// Rule 5: unit elimination. Only a LITERAL decides — `complement_of`
	// strips or adds one `¬` and is not `NEG`, so nothing deeper is
	// complemented here.
	if (is_literal<node>(m) && present.contains(complement_of<node>(m)))
		return is_decided = true;
	// Rule 6.
	ms.push_back(m);
	present.insert(m);
	return false;
}

template <NodeType node, bool conj>
bool join_builder<node, conj>::decided() const { return is_decided; }

template <NodeType node, bool conj>
tref join_builder<node, conj>::result() {
	if (is_decided) return conj ? _F<node>() : _T<node>();
	if (ms.empty())  return conj ? _T<node>() : _F<node>();
	trefs survivors = detail::absorb<node, conj>(ms);
	// The constructor sorts into content order, deduplicates and folds
	// left-nested through the hooked binary builder; a single survivor
	// comes back trimmed. That emission is what makes the join a function
	// of the member SET (§3, AC-canonicity).
	return conj ? canonical_and<node>(std::move(survivors))
		    : canonical_or<node>(std::move(survivors));
}

// --- the n-ary forms ------------------------------------------------------------

template <NodeType node>
tref simplified_and_join(const trefs& rs) {
	join_builder<node, true> b;
	for (tref r : rs) if (b.insert(r)) break;
	return b.result();
}

template <NodeType node>
tref simplified_or_join(const trefs& rs) {
	join_builder<node, false> b;
	for (tref r : rs) if (b.insert(r)) break;
	return b.result();
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__NORMALISERS__JOINS_TMPL_H__
