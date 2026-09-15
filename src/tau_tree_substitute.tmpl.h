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

/** @internal @copydoc tree::substitute(tref, tref, const subtree_unordered_map<node, int_t>&, const argument_hook&) const @endinternal */
template<NodeType node>
tref tree<node>::substitute(tref that, tref with,
	const subtree_unordered_map<node, int_t>& o,
	const argument_hook& on_argument) const
{
	DBG(assert(that != nullptr && with != nullptr);)
	return substitute(subtree_map<node, tref>{ { that, with } },
		o, on_argument);
}

/** @internal @copydoc tree::substitute(const subtree_map<node, tref>&, const subtree_unordered_map<node, int_t>&, const argument_hook&) const @endinternal */
template<NodeType node>
tref tree<node>::substitute(const subtree_map<node, tref>& changes,
	const subtree_unordered_map<node, int_t>& o,
	const argument_hook& on_argument) const
{
	using tau = tree<node>;
	using handle = tau_term_bdd_handle<node>;
	const tref formula = this->get();
	DBG(assert(formula != nullptr);)
	if (changes.empty()) return formula;
	// The entry step, once per call: every replacement is spelled out and
	// renamed apart here, and the walk below — the leaves of a BDD and the
	// arguments of a reference included — reuses the result. Renaming is
	// not idempotent, so it happens in this one place.
	substitution s;
	subtree_set<node> vars;
	for (const auto& [key, value] : changes) {
		tref k = tau::trim_right_sibling(key);
		tref w = tau::trim_right_sibling(value);
		DBG(assert(k != nullptr && w != nullptr);)
		// A replacement carrying a `BDD_ID` is spelled out as a plain
		// term once, here, rather than at every leaf and reference
		// argument below.
		if (tau::get(w).find_top([](tref m) {
			return tau::get(m).is(tau::BDD_ID); }))
			w = handle::convert_to_tau_terms(w);
		// Then its bound variables are renamed apart, after the
		// spelling, which may have brought binders out of the BDD's
		// leaves.
		if (tau::get(w).find_top(is_logical_or_functional_quant<node>))
			w = rename_apart(formula, w);
		// A variable key is matched in its `bf` wrapper and used bare
		// for the occurrence guard and the compose; both spellings of
		// it name the same key.
		tref var = nullptr;
		if (const tau& t = tau::get(k); t.is(tau::variable)) {
			var = k;
			k = tau::get(tau::bf, k);
		} else if (t.is(tau::bf) && t.first() != nullptr
			&& tau::get(t.first()).is(tau::variable))
			var = tau::trim_right_sibling(t.first());
		if (var == nullptr) s.keys_are_variables = false;
		else {
			vars.insert(var);
			s.by_variable.emplace_back(var, w);
		}
		s.changes.emplace(k, w);
	}
	s.vars.assign(vars.begin(), vars.end());
#ifdef DEBUG
	subtree_set<node> free_in_with;
	for (const auto& [k, w] : s.changes) {
		const trefs& fv = tau_lang::get_free_vars<node>(w);
		free_in_with.insert(fv.begin(), fv.end());
	}
	s.free_in_with.assign(free_in_with.begin(), free_in_with.end());
#endif
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
	// The occurrence guard: one cached free-variable test per `wff` or
	// `bf` node. A node no key variable is free in is not entered, so it
	// comes back as the same tref, and a key rebound below it is left
	// alone. When some key is not a variable there is no such test and
	// every node is entered.
	auto free_in = [&s](tref n) {
		const trefs& fv = tau_lang::get_free_vars<node>(n);
		for (tref v : s.vars)
			if (std::binary_search(fv.begin(), fv.end(), v,
				tau::subtree_less)) return true;
		return false;
	};
	auto visit_subtree = [&](tref n) {
		if (!s.keys_are_variables) return true;
		const tau& t = tau::get(n);
		return (!t.is(tau::wff) && !t.is(tau::bf)) || free_in(n);
	};
	auto f = [&](tref n) -> tref {
		// An occurrence, compared by content. The walk stops here, so
		// nothing inside a replacement is rewritten again.
		if (const tref r = get_cached<node>(n, s.changes); r != n)
			return r;
		const tau& t = tau::get(n);
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
			typename tbdd::subs_t subs;
			for (const auto& [v, w] : s.by_variable)
				if (o.contains(v))
					subs.emplace_back(v,
						tbdd::build_bdd(w, o));
			if (!subs.empty())
				r = tbdd::bdd_compose(r, std::move(subs), o);
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
