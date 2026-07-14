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

} // namespace idni::tau_lang
