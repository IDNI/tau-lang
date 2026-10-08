// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_TYPES_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_TYPES_TMPL_H__

namespace idni::tau_lang {

/** @internal @copydoc bv_type @endinternal */
template<NodeType node>
tref bv_type(size_t bitwidth) {
	using tau = tree<node>;

	// A width outside the unsigned-short range is a caller bug; the node
	// payload stores the width at full width, and get_bv_width checks it
	// again when a spec supplies the number.
	DBG(assert(bitwidth >= 1 && bitwidth <= 0xffff);)
	tref subtype = tau::get(tau::subtype, tau::get_num(bitwidth));
	tref type = tau::get(node(tau::type, dict("bv")), subtype);
	return tau::get(tau::typed, type);
}

/** @internal @copydoc bv_type_id @endinternal */
template<NodeType node>
size_t bv_type_id(size_t bitwidth) {
	return ba_types<node>::id(bv_type<node>(bitwidth));
}

/** @internal @copydoc is_bv_type_family(tref) @endinternal */
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

/** @internal @copydoc is_bv_type_family(size_t) @endinternal */
template<NodeType node>
bool is_bv_type_family(size_t ba_type_id) {
	auto t = ba_types<node>::type_tree(ba_type_id);
	// An id the type table does not hold is not a bv type: owns_type is
	// asked about any id a fold is handed, an unknown one included.
	if (!t.has_value()) return false;
	return is_bv_type_family<node>(t.value());
}

/** @internal @copydoc is_tref_bv_type_family @endinternal */
template<NodeType node>
bool is_tref_bv_type_family(tref t) {
	using tau = tree<node>;
	return is_bv_type_family<node>(tau::get(t).get_ba_type());
}

/** @internal @copydoc get_bv_width(tref) @endinternal */
template <NodeType node>
result<size_t> get_bv_width(tref t) {
	using tau = tree<node>;
	using tt = tau::traverser;

	tref num = tt(t) | tau::type | tau::subtype | tau::num | tt::ref;
	if (!num) {
		result<size_t> r;
		return r.with_assert_check_error(code::type_error,
			"get_bv_width: bv type has no explicit bitwidth");
	}
	// Check after reading the full payload: narrowing first would turn an
	// out-of-range width into a plausible in-range one.
	const uint64_t width = tau::get(num).get_num();
	if (width < 1 || width > 0xffff) {
		result<size_t> r;
		return r.with_error(code::invalid_argument,
			"get_bv_width: bitvector width is out of range",
			{{label::size, static_cast<int_t>(width)}});
	}
	return result<size_t>{static_cast<size_t>(width)};
}

/** @internal @copydoc get_bv_width(size_t) @endinternal */
template <NodeType node>
result<size_t> get_bv_width(size_t ba_type_id) {
	result<size_t> r;
	TAU_TRY(tref t, ba_types<node>::type_tree(ba_type_id));
	return get_bv_width<node>(t);
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_TYPES_TMPL_H__
