// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "normalizer_uf_arithmetic"

namespace idni::tau_lang {

template<NodeType node>
void bv_arithmetic_resolver<node>::open() {
	scoped.open();
}

template<NodeType node>
std::optional<typename bv_arithmetic_resolver<node>::uf_t::scope_error>
bv_arithmetic_resolver<node>::close() {
	return scoped.close();
}

template<NodeType node>
typename bv_arithmetic_resolver<node>::element
bv_arithmetic_resolver<node>::insert(tref n, arith_kind k) {
	auto e = scoped.push(n);
	kinds.emplace(e, k);
	return e;
}

template<NodeType node>
arith_kind bv_arithmetic_resolver<node>::kind_of(tref n) {
	auto root = scoped.root(scoped.insert(n));
	if (auto it = kinds.find(root); it != kinds.end()) return it->second;
	return kinds.emplace(root, arith_kind::logical).first->second;
}

template<NodeType node>
typename bv_arithmetic_resolver<node>::element
bv_arithmetic_resolver<node>::assign(tref n, arith_kind k) {
	auto e = scoped.insert(n);
	auto root = scoped.root(e);
	if (auto it = kinds.find(root); it != kinds.end())
		kinds.insert_or_assign(root, unify(it->second, k));
	else kinds.insert_or_assign(root, k);
	return root;
}

template<NodeType node>
typename bv_arithmetic_resolver<node>::element
bv_arithmetic_resolver<node>::merge(tref a, tref b) {
	auto ka = kind_of(a);
	auto kb = kind_of(b);
	auto new_root = scoped.merge(a, b);
	kinds.insert_or_assign(new_root, unify(ka, kb));
	return new_root;
}

template <NodeType node>
subtree_unordered_set<node> collect_bv_arithmetic_taint_uf(tref formula) {
	using tau = tree<node>;
	subtree_unordered_set<node> tainted;
	bv_arithmetic_resolver<node> resolver;
	// Not is<node>({...}): that factory's returned closure captures a
	// std::initializer_list by value, whose backing array is a temporary
	// destroyed at the end of the factory call's full expression --
	// silently dangling afterward (undefined behavior masked by Debug's
	// stack layout, reliably wrong under Release's optimizations).
	auto is_arith_op = [](tref n) {
		return tau::get(n).is(tau::bf_add)
			|| tau::get(n).is(tau::bf_sub)
			|| tau::get(n).is(tau::bf_mul)
			|| tau::get(n).is(tau::bf_div)
			|| tau::get(n).is(tau::bf_mod)
			|| tau::get(n).is(tau::bf_shl)
			|| tau::get(n).is(tau::bf_shr)
			|| tau::get(n).is(tau::bf_cast);
	};

	auto snapshot_scope = [&](typename bv_arithmetic_resolver<node>::scope s) {
		for (auto [elem, _] : resolver.scoped.uf)
			if (elem.first == s && resolver.kind_of(elem.second)
				== arith_kind::arithmetic)
				tainted.insert(elem.second);
	};

	auto visit_subtree = [](tref) -> bool { return true; };

	auto visit = [&](tref m) -> bool {
		if (is_quantifier<node>(m)) {
			resolver.open();
			if (tref v = tau::get(m).find_top(
				(bool(*)(tref)) is_var_or_capture<node>); v)
				resolver.insert(v, arith_kind::logical);
			return true;
		}
		if (is_atomic_fm<node>(m)) {
			arith_kind k = tau::get(m).find_top(is_arith_op)
				? arith_kind::arithmetic : arith_kind::logical;
			resolver.assign(m, k);
			for (tref v : get_free_vars<node>(m))
				if (is_bv_type_family<node>(
					tau::get(v).get_ba_type()))
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
	return tainted;
}

template <NodeType node>
std::function<bool(tref)> make_bv_arithmetic_skip_uf(tref formula) {
	auto tainted = std::make_shared<subtree_unordered_set<node>>(
		collect_bv_arithmetic_taint_uf<node>(formula));
	return [tainted](tref n) { return tainted->contains(n); };
}

} // namespace idni::tau_lang
