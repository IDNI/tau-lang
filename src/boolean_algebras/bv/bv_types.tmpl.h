// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_TYPES_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_TYPES_TMPL_H__

namespace idni::tau_lang {

template<NodeType node>
tref bv_type(unsigned short bitwidth) {
	using tau = tree<node>;

	tref subtype = tau::get(tau::subtype, tau::get_num(bitwidth));
	tref type = tau::get(node(tau::type, dict("bv")), subtype);
	return tau::get(tau::typed, type);
}

template<NodeType node>
size_t bv_type_id(unsigned short bitwidth) {
	return ba_types<node>::id(bv_type<node>(bitwidth));
}

template<NodeType node>
bool is_bv_type_family(tref t) {
	using tau = tree<node>;
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	if (auto it = cache.find(t); it != cache.end()) return it->second;
#endif // TAU_CACHE
	bool result = tau::get(t)[0].get_string() == "bv";
#ifdef TAU_CACHE
	cache.emplace(t, result);
#endif // TAU_CACHE
	return result;
}

template<NodeType node>
bool is_bv_type_family(size_t ba_type_id) {
	auto t = ba_types<node>::type_tree(ba_type_id);
	// Advisory drop: owns_type (ba_descriptor_complete) fixes this to bool,
	// so an out-of-range id reads the same as "not a bv type".
	if (!t.has_value()) return false;
	return is_bv_type_family<node>(t.value());
}

template<NodeType node>
bool is_tref_bv_type_family(tref t) {
	using tau = tree<node>;
	return is_bv_type_family<node>(tau::get(t).get_ba_type());
}

template <NodeType node>
result<size_t> get_bv_width(tref t) {
	using tau = tree<node>;
	using tt = tau::traverser;

	size_t num = tt(t) | tau::type | tau::subtype | tau::num | tt::num;
	if (!num) {
		result<size_t> r;
		return r.with_assert_check_error(code::type_error,
			"get_bv_width: bv type has no explicit bitwidth");
	}
	return result<size_t>{num};
}

template <NodeType node>
result<size_t> get_bv_width(size_t ba_type_id) {
	result<size_t> r;
	TAU_TRY(tref t, ba_types<node>::type_tree(ba_type_id));
	return get_bv_width<node>(t);
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_TYPES_TMPL_H__
