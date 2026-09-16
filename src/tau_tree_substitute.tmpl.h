// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file tau_tree_substitute.tmpl.h
 * @brief `tree<node>::substitute` and its two helpers, `bound_vars` and
 * `rename_apart`. Included by tau_tree.tmpl.h; tau_tree.h says what each one
 * means, the comments here say how it is built.
 *
 * Substitution is a tree operation, and a `BDD_ID` is one node kind it knows
 * about: a BDD-backed term keeps its variables in the BDD store rather than
 * as tree nodes, so the walk reaches them through
 * `tau_term_bdd_handle<node>`, as `get_free_vars`
 * (tau_tree_extractors.tmpl.h) and `find_biggest_var_id`
 * (tau_tree_builders.tmpl.h) do.
 */

#ifndef __IDNI__TAU__TAU_TREE_SUBSTITUTE_TMPL_H__
#define __IDNI__TAU__TAU_TREE_SUBSTITUTE_TMPL_H__

#include <algorithm>
#include <cctype>
#include <ranges>
#include <stdexcept>
#include <string>

namespace idni::tau_lang {

/** @internal @copydoc tree::bound_vars(tref) @endinternal */
template<NodeType node>
trefs tree<node>::bound_vars(tref t) {
	using tau = tree<node>;
	subtree_set<node> vs;
	auto f = [&vs](tref m) {
		if (is_logical_or_functional_quant<node>(m))
			vs.insert(tau::trim_right_sibling(tau::get(m).first()));
		return true;
	};
	pre_order<node>(t).visit_unique(f);
	return trefs(vs.begin(), vs.end());
}

/** @internal @copydoc tree::rename_apart(tref, tref) @endinternal */
template<NodeType node>
tref tree<node>::rename_apart(tref n, tref t) {
	using tau = tree<node>;
	const trefs bound = bound_vars(t);
	if (bound.empty()) return t;
	const int_t base = std::max(find_biggest_var_id<node>(n),
		find_biggest_var_id<node>(t));
	subtree_map<node, tref> changes;
	for (tref v : bound) {
		const std::string name = get_var_name<node>(v);
		if (name.empty() || !std::ranges::all_of(name,
			[](unsigned char c) { return std::isdigit(c) != 0; }))
			continue;
		int_t id;
		try { id = static_cast<int_t>(std::stoll(name)); }
		catch (const std::out_of_range&) { continue; }
		changes.emplace(v, tau::build_variable(std::to_string(id + base),
			tau::get(v).get_ba_type()));
	}
	if (changes.empty()) return t;
	return rewriter::replace<node>(t, changes);
}

/** @internal @copydoc tree::prepare(substitution&, tref, tref, tref) @endinternal */
template<NodeType node>
void tree<node>::prepare(substitution& s, tref formula, tref key, tref value)
{
	using tau = tree<node>;
	using handle = tau_term_bdd_handle<node>;
	// Trimming interns a node, so it is asked for only when there is a
	// sibling to drop.
	tref k = tau::get(key).has_right_sibling()
		? tau::trim_right_sibling(key) : key;
	tref w = tau::get(value).has_right_sibling()
		? tau::trim_right_sibling(value) : value;
	DBG(assert(k != nullptr && w != nullptr);)
	// One scan of the replacement answers both questions it is asked: is
	// a `BDD_ID` in it, and is a binder. The walk stops as soon as both
	// are known.
	bool has_bdd_id = false, has_binder = false;
	tau::get(w).find_top([&](tref m) {
		if (tau::get(m).is(tau::BDD_ID)) has_bdd_id = true;
		else if (is_logical_or_functional_quant<node>(m))
			has_binder = true;
		return has_bdd_id && has_binder;
	});
	// A replacement carrying a `BDD_ID` is spelled out as a plain term
	// once, here, rather than at every leaf and reference argument below.
	if (has_bdd_id) {
		w = handle::convert_to_tau_terms(w);
		// The spelling may have brought binders out of the BDD's
		// leaves, which are not tree children, so the question is put
		// again to the spelled-out term.
		has_binder = tau::get(w).find_top(
			is_logical_or_functional_quant<node>) != nullptr;
	}
	// Bound variables are renamed apart after the spelling, and renaming
	// is not idempotent, so it happens in this one place.
	if (has_binder) w = rename_apart(formula, w);
	// The key is matched exactly as given. Either spelling of a variable
	// key, bare or in its `bf` wrapper, names the variable the occurrence
	// guard and the compose need.
	tref var = nullptr;
	if (const tau& t = tau::get(k); t.is(tau::variable)) var = k;
	else if (t.is(tau::bf) && t.first() != nullptr
		&& tau::get(t.first()).is(tau::variable))
		var = tau::trim_right_sibling(t.first());
	if (var == nullptr) s.keys_are_variables = false;
	else s.by_variable.emplace_back(var, w);
	s.key_types.set(static_cast<size_t>(tau::get(k).get_type()));
	s.changes.emplace(k, w);
}

/** @internal @copydoc tree::finish(substitution&) @endinternal */
template<NodeType node>
void tree<node>::finish(substitution& s) {
	using tau = tree<node>;
	// The guard searches the key variables, so they are put in order once
	// here; one key is already in it. Duplicates are left as they are,
	// two keys naming one variable being two pairs the compose is entitled
	// to see.
	if (s.by_variable.size() > 1)
		std::ranges::sort(s.by_variable,
			[](tref a, tref b) { return tau::subtree_less(a, b); },
			&std::pair<tref, tref>::first);
#ifdef DEBUG
	subtree_set<node> free_in_with;
	for (const auto& [k, w] : s.changes) {
		const trefs& fv = tau_lang::get_free_vars<node>(w);
		free_in_with.insert(fv.begin(), fv.end());
	}
	s.free_in_with.assign(free_in_with.begin(), free_in_with.end());
#endif
}

/** @internal @copydoc tree::substitute(tref, tref, const subtree_unordered_map<node, int_t>&, const argument_hook&) const @endinternal */
template<NodeType node>
tref tree<node>::substitute(tref that, tref with,
	const subtree_unordered_map<node, int_t>& o,
	const argument_hook& on_argument) const
{
	using tau = tree<node>;
	const tref formula = this->get();
	DBG(assert(formula != nullptr && that != nullptr && with != nullptr);)
	// One pair, prepared straight into the substitution: the map the other
	// overload takes would be built only to be read back.
	substitution s;
	prepare(s, formula, that, with);
	finish(s);
	return tau::substitute(formula, s, o, on_argument);
}

/** @internal @copydoc tree::substitute(const subtree_map<node, tref>&, const subtree_unordered_map<node, int_t>&, const argument_hook&) const @endinternal */
template<NodeType node>
tref tree<node>::substitute(const subtree_map<node, tref>& changes,
	const subtree_unordered_map<node, int_t>& o,
	const argument_hook& on_argument) const
{
	using tau = tree<node>;
	const tref formula = this->get();
	DBG(assert(formula != nullptr);)
	if (changes.empty()) return formula;
	// The entry step, once per call: every replacement is spelled out and
	// renamed apart here, and the walk below — the leaves of a BDD and the
	// arguments of a reference included — reuses the result.
	substitution s;
	for (const auto& [key, value] : changes)
		prepare(s, formula, key, value);
	finish(s);
	return tau::substitute(formula, s, o, on_argument);
}

/** @internal @copydoc tree::substitute(tref, const substitution&, const subtree_unordered_map<node, int_t>&, const argument_hook&) @endinternal */
template<NodeType node>
tref tree<node>::substitute(tref formula, const substitution& s,
	const subtree_unordered_map<node, int_t>& o,
	const argument_hook& on_argument)
{
	using tau   = tree<node>;
	using tbdd  = tau_term_bdd<node>;
	using handle = tau_term_bdd_handle<node>;
	// The occurrence guard: a `wff` or `bf` node is entered only if a key
	// variable may be free in it, so a node none of them reaches comes back
	// as the same tref and a key rebound below it is left alone. When some
	// key is not a variable there is no such test and every node is
	// entered.
	// Both sides are sorted and deduplicated, so the intersection is found
	// by searching each element of the shorter one in the longer: a clause
	// whose row holds two variables costs two searches against sixty-four
	// keys, not sixty-four. The comparator is a lambda because
	// `tau::subtree_less` passed by name reaches the search as a function
	// pointer and never inlines.
	auto holds_key = [&s](const trefs& fv) {
		if (fv.empty()) return false;
		auto less = [](tref a, tref b) {
			return tau::subtree_less(a, b); };
		// The key variables are the `.first` of the pairs, in the same
		// order the row is in, so either side can be the one searched.
		if (s.by_variable.size() < fv.size()) {
			for (const auto& [v, w] : s.by_variable)
				if (std::binary_search(fv.begin(), fv.end(), v,
					less)) return true;
			return false;
		}
		for (tref v : fv)
			if (std::ranges::binary_search(s.by_variable, v, less,
				&std::pair<tref, tref>::first)) return true;
		return false;
	};
	// A wrapper around one chain connective. `get_free_vars` answers such a
	// chain from its top: the top and every clause under it get an answer
	// of their own, the links between them are taken apart and get none.
	auto wraps_connective = [](const tau& t) {
		const tref c = t.first();
		if (c == nullptr || tau::get(c).has_right_sibling()) return false;
		const tau& ct = tau::get(c);
		return ct.is(tau::wff_and) || ct.is(tau::wff_or)
			|| ct.is(tau::bf_and) || ct.is(tau::bf_or);
	};
	auto visit_subtree = [&](tref n) {
		if (!s.keys_are_variables) return true;
		const tau& t = tau::get(n);
		if (!t.is(tau::wff) && !t.is(tau::bf)) return true;
		// The root of this walk, and every node that is not a chain
		// link, are asked outright. Each of them owns its answer, and
		// the root's is what fills in the answers below it: from any
		// node it is asked at, the walk answers every owner it reaches
		// and stops only where one is answered already.
		if (n == formula || !wraps_connective(t))
			return holds_key(tau_lang::get_free_vars<node>(n));
		// A chain link, which after that has no answer of its own. It
		// is ENTERED rather than asked: asking would walk the whole
		// subtree below the link and keep an answer for it, at every
		// link of the chain, which costs a k-clause chain k walks and k
		// answers of k variables each. Its clauses do own answers, so
		// nothing below is entered that asking would have pruned.
		const trefs* fv = cached_free_vars<node>(n);
		return fv == nullptr || holds_key(*fv);
	};
	// The compose list, built at the first BDD-backed node this call meets
	// and reused by every later one. `build_bdd(w, o)` turns a replacement
	// into a BDD under the live order, and both are constants of the call,
	// so the answer is one per call rather than one per backed node. It is
	// built here rather than in the entry step because a formula with no
	// backed node in it must not pay for a BDD it never composes.
	auto compose_list = [&s, &o]() -> const typename tbdd::subs_t& {
		if (!s.compose_prepared) {
			for (const auto& [v, w] : s.by_variable)
				if (o.contains(v)) s.compose_subs.emplace_back(
					v, tbdd::build_bdd(w, o));
			// The order a compose descends in, sorted once here
			// instead of at every node.
			std::sort(s.compose_subs.begin(), s.compose_subs.end(),
				[&o](const auto& a, const auto& b) {
					return tbdd::less_then(a.first,
						b.first, o); });
			s.compose_prepared = true;
		}
		return s.compose_subs;
	};
	auto f = [&](tref n) -> tref {
		const tau& t = tau::get(n);
		// An occurrence, compared by content. The walk stops here, so
		// nothing inside a replacement is rewritten again. Only a node
		// whose type some key has is looked up at all: the comparison
		// is by content and content includes the type, so no other node
		// can match, and most of what a walk meets -- wrappers,
		// connectives, names, constants -- is of no key's type.
		if (s.key_types.test(static_cast<size_t>(t.get_type())))
			if (const tref r = get_cached<node>(n, s.changes);
				r != n) return r;
		// A BDD-backed term holds its variables in the BDD store, not
		// as tree nodes. Its leaves are rewritten first, on the
		// original leaves, so a key inside a replacement is not
		// substituted again; then one compose removes the keys that
		// are decision variables.
		if (handle::is_bdd_backed(n)) {
			DBG(assert(!o.empty());)
			const typename tbdd::ref x =
				handle::convert_to_handle(n).get();
			DBG(assert(tbdd::is_ordered(x, o));)
			auto leaf = [&](tref l) {
				return tau::substitute(l, s, o, on_argument);
			};
			typename tbdd::ref r = tbdd::map_leaves(x, leaf, o);
			const typename tbdd::subs_t& subs = compose_list();
			// One key takes the single-pair compose, which is the
			// shape the module's calls have and the one that routes
			// a constant replacement through the cofactor.
			if (subs.size() == 1)
				r = tbdd::bdd_compose(r, subs.front().first,
					subs.front().second, o);
			else if (!subs.empty())
				r = tbdd::bdd_compose(r, subs, o);
			if (r == x) return n;
			return handle::convert_to_tau_node_or_term(handle(r),
				find_ba_type<node>(n));
		}
		// A reference argument is rewritten whole and passed through
		// the hook once per call: the memo holds its result, so a
		// shared argument is not rewritten again. A reference nested
		// inside the argument is finished by the recursive call
		// first.
		if (t.is(tau::ref_arg)) {
			const tref a = tau::trim_right_sibling(t.first());
			auto it = s.argument_memo.find(a);
			if (it == s.argument_memo.end()) {
				tref r = tau::substitute(a, s, o, on_argument);
				if (r != a) r = on_argument(r);
				it = s.argument_memo.emplace(a, r).first;
			}
			if (it->second == a) return n;
			return tau::get(t.value, it->second);
		}
		// A binder is where capture would happen. The replacements are
		// renamed apart and their free variables are free at the site,
		// so it cannot; Debug checks it, and only under the occurrence
		// guard, since without it subtrees holding no occurrence are
		// entered too.
		DBG(if (s.keys_are_variables
			&& is_logical_or_functional_quant<node>(n))
			assert(!std::binary_search(s.free_in_with.begin(),
				s.free_in_with.end(),
				tau::trim_right_sibling(t.first()),
				tau::subtree_less));)
		return n;
	};
	return pre_order<node>(formula).apply_unique_until_change(f,
		visit_subtree);
}

// --- the free functions -----------------------------------------------------------

template <NodeType node>
tref substitute(tref formula, tref that, tref with,
	const subtree_unordered_map<node, int_t>& o,
	const typename tree<node>::argument_hook& on_argument)
{
	return tree<node>::get(formula).substitute(that, with, o, on_argument);
}

template <NodeType node>
tref substitute(tref formula, const subtree_map<node, tref>& changes,
	const subtree_unordered_map<node, int_t>& o,
	const typename tree<node>::argument_hook& on_argument)
{
	return tree<node>::get(formula).substitute(changes, o, on_argument);
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__TAU_TREE_SUBSTITUTE_TMPL_H__
