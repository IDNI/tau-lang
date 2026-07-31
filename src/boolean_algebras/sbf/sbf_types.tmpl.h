// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__SBF__SBF_TYPES_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__SBF__SBF_TYPES_TMPL_H__

namespace idni::tau_lang {

template<NodeType node>
tref sbf_type() {
	using tau = tree<node>;

	tref type = tau::get(tau::type, "sbf");
	return tau::get(tau::typed, type);
}

template<NodeType node>
inline size_t sbf_type_id() {
	static size_t id = ba_types<node>::id(sbf_type<node>());
	return id;
}

template<NodeType node>
bool is_sbf_type(tref t) {
	using tau = tree<node>;
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	if (auto it = cache.find(t); it != cache.end()) return it->second;
#endif // TAU_CACHE
	bool result = tau::get(t)[0].get_string() == "sbf";
#ifdef TAU_CACHE
	cache.emplace(t, result);
#endif // TAU_CACHE
	return result;
}

template <NodeType node>
bool is_sbf_type(size_t t) {
	return is_sbf_type<node>(ba_types<node>::type_tree(t));
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__SBF__SBF_TYPES_TMPL_H__
