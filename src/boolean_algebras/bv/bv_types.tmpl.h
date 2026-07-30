// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_TYPES_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_TYPES_TMPL_H__

namespace idni::tau_lang {

template<NodeType node>
tref bv_type(unsigned short bitwidth) {
	using tau = tree<node>;

	tref type = tau::get(tau::type, "bv");
	tref subtype = tau::get(tau::subtype, tau::get_num(bitwidth));
	return tau::get(tau::typed, type, subtype);
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
	return is_bv_type_family<node>(ba_types<node>::type_tree(ba_type_id));
}

template<NodeType node>
bool is_tref_bv_type_family(tref t) {
	using tau = tree<node>;
	return is_bv_type_family<node>(tau::get(t).get_ba_type());
}

template <NodeType node>
size_t get_bv_width(tref t) {
	using tau = tree<node>;
	using tt = tau::traverser;

	DBG(assert(is_bv_type_family<node>(t)));
	size_t num = tt(t) | tau::subtype | tau::num | tt::num;
	assert(num && "bv type must have explicit bitwidth");
	return num;
}

template <NodeType node>
size_t get_bv_width(size_t ba_type_id) {
	tref t = ba_types<node>::type_tree(ba_type_id);
	DBG(assert(is_bv_type_family<node>(t)));
	return get_bv_width<node>(t);
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_TYPES_TMPL_H__
