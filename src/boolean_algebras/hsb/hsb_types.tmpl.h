// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB__HSB_TYPES_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB__HSB_TYPES_TMPL_H__

namespace idni::tau_lang {

// hsb type definitions
template<NodeType node>
tref hsb_type() {
	using tau = tree<node>;
	tref type = tau::get(tau::type, "hsb");
	return tau::get(tau::typed, type);
}

template<NodeType node>
inline size_t hsb_type_id() {
	static size_t id = ba_types<node>::id(hsb_type<node>());
	return id;
}

template<NodeType node>
bool is_hsb_type(tref t) {
	using tau = tree<node>;
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	if (auto it = cache.find(t); it != cache.end()) return it->second;
#endif // TAU_CACHE
	bool result = tau::get(t)[0].get_string() == "hsb";
#ifdef TAU_CACHE
	cache.emplace(t, result);
#endif // TAU_CACHE
	return result;
}

template <NodeType node>
bool is_hsb_type(size_t t) {
	return is_hsb_type<node>(ba_types<node>::type_tree(t));
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB__HSB_TYPES_TMPL_H__
