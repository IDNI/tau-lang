// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ref_variables_resolver"

namespace idni::tau_lang {

template<NodeType node>
void ref_variables_resolver<node>::open() {
	scoped.open();
}

template<NodeType node>
std::optional<typename ref_variables_resolver<node>::uf_t::scope_error>
ref_variables_resolver<node>::close() {
	return scoped.close();
}

template<NodeType node>
typename ref_variables_resolver<node>::element
ref_variables_resolver<node>::insert(tref n, ref_variable_kind k) {
	auto e = scoped.push(n);
	kinds.emplace(e, k);
	return e;
}

template<NodeType node>
ref_variable_kind ref_variables_resolver<node>::kind_of(tref n) {
	auto root = scoped.root(scoped.insert(n));
	if (auto it = kinds.find(root); it != kinds.end()) return it->second;
	return kinds.emplace(root, ref_variable_kind::unused).first->second;
}

template<NodeType node>
typename ref_variables_resolver<node>::element
ref_variables_resolver<node>::assign(tref n, ref_variable_kind k) {
	auto e = scoped.insert(n);
	auto root = scoped.root(e);
	if (auto it = kinds.find(root); it != kinds.end())
		kinds.insert_or_assign(root, unify(it->second, k));
	else kinds.insert_or_assign(root, k);
	return root;
}

template<NodeType node>
typename ref_variables_resolver<node>::element
ref_variables_resolver<node>::merge(tref a, tref b) {
	auto ka = kind_of(a);
	auto kb = kind_of(b);
	auto new_root = scoped.merge(a, b);
	kinds.insert_or_assign(new_root, unify(ka, kb));
	return new_root;
}

template <NodeType node>
subtree_unordered_set<node> collect_used_ref_variables(tref formula) {
	using tau = tree<node>;
	subtree_unordered_set<node> used;
	ref_variables_resolver<node> resolver;

	// A wff wrapping an unresolved predicate reference. wff_ref is
	// deliberately excluded from is_atomic_fm (see
	// tau_tree_queries.tmpl.h:155), so it needs its own branch rather
	// than a predicate handed to find_top.
	auto is_ref_fm = [](tref n) {
		const auto& t = tau::get(n);
		return t.is(tau::wff) && t.child_is(tau::wff_ref);
	};

	auto snapshot_scope = [&](typename ref_variables_resolver<node>::scope s) {
		for (auto [elem, _] : resolver.scoped.uf)
			if (elem.first == s && resolver.kind_of(elem.second)
				== ref_variable_kind::used)
				used.insert(elem.second);
	};

	auto visit_subtree = [](tref) -> bool { return true; };

	auto visit = [&](tref m) -> bool {
		if (is_quantifier<node>(m)) {
			resolver.open();
			if (tref v = tau::get(m).find_top(
				(bool(*)(tref)) is_var_or_capture<node>); v)
				resolver.insert(v, ref_variable_kind::unused);
			return true;
		}
		if (is_ref_fm(m)) {
			// Seed: every variable reachable from a predicate
			// reference is used. No BA-type filter -- predicate
			// arguments carry no restriction analogous to
			// bitvector arithmetic's.
			resolver.assign(m, ref_variable_kind::used);
			for (tref v : get_free_vars<node>(m))
				resolver.merge(m, v);
			return false;
		}
		if (is_atomic_fm<node>(m)) {
			// Propagation channel: the atom contributes no use of
			// its own, but unioning it with its free variables
			// makes every variable sharing an atom share a root,
			// so `used` reaches variables that are not themselves
			// predicate arguments. Order-independent: whichever of
			// atom/ref is visited first, the second merge joins
			// the two sets and unify() lifts the result to `used`.
			resolver.assign(m, ref_variable_kind::unused);
			for (tref v : get_free_vars<node>(m))
				resolver.merge(m, v);
			return false;
		}
		return true;
	};

	auto up = [&](tref m) {
		if (!is_quantifier<node>(m)) return;
		auto s = resolver.scoped.scopes.back();
		snapshot_scope(s);
		resolver.close();
	};

	idni::pre_order<node>(formula).visit(visit, visit_subtree, up);
	snapshot_scope(resolver.scoped.global);
	return used;
}

template <NodeType node>
std::function<bool(tref)> make_ref_variables_skip(tref formula) {
	auto used = std::make_shared<subtree_unordered_set<node>>(
		collect_used_ref_variables<node>(formula));
	return [used](tref n) { return used->contains(n); };
}

} // namespace idni::tau_lang
