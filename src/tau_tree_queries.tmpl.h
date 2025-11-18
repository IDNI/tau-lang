// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_tree.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "queries"

namespace idni::tau_lang {

template <NodeType node>
bool is(tref n, size_t nt) {
	return tree<node>::get(n).is(nt);
}

template <NodeType node, typename node::type nt>
bool is(tref n) { return is<node>(n, nt); }

// factory method for is predicate
template <NodeType node>
inline std::function<bool(tref)> is(size_t nt) {
	return [nt](tref n) { return is<node>(n, nt); };
}

template <NodeType node>
bool is_child(tref n, size_t nt) {
	return tree<node>::get(n).child_is(nt);
}

template <NodeType node, size_t nt>
bool is_child(tref n) { return is_child<node>(n, nt); }

// factory method for is predicate
template <NodeType node>
inline std::function<bool(tref)> is_child(size_t nt) {
	return [nt](tref n) { return is_child<node>(n, nt); };
}

template <NodeType node>
bool is_child_quantifier(tref n) {
	return tree<node>::get(n).child_is(node::type::wff_all)
		|| tree<node>::get(n).child_is(node::type::wff_ex);
}

template <NodeType node>
bool is_temporal_quantifier(tref n) {
	return tree<node>::get(n).is(node::type::wff_always)
		|| tree<node>::get(n).is(node::type::wff_sometimes);

}

template <NodeType node>
bool is_ba_element(tref n) {
	return tree<node>::get(n).is(node::type::ba_constant)
		|| tree<node>::get(n).is(node::type::variable)
		|| tree<node>::get(n).is(node::type::bf_t)
		|| tree<node>::get(n).is(node::type::bf_f);
}

template <NodeType node>
bool is_uconst(tref n) {
	return tree<node>::get(n).is(node::type::uconst_name);
}

template <NodeType node>
bool is_io_var(tref n) {
	return tree<node>::get(n).is(node::type::io_var)
		|| (tree<node>::get(n).is(node::type::variable)
			&& tree<node>::get(n).child_is(node::type::io_var));
}

template <NodeType node>
bool is_input_var(tref n) {
	return tree<node>::get(n).is_input_variable();
}

template <NodeType node>
bool is_output_var(tref n) {
	return tree<node>::get(n).is_output_variable();
}

template <NodeType node>
bool is_var_or_capture(tref n) {
	return tree<node>::get(n).is(node::type::variable)
		|| tree<node>::get(n).is(node::type::capture);
}

template <NodeType node>
inline std::function<bool(tref)> is_var_or_capture() {
	return [](tref n) { return is_var_or_capture<node>(n); };
}

template <NodeType node>
bool is_quantifier(tref n) {
	return tree<node>::get(n).is(node::type::wff_all)
		|| tree<node>::get(n).is(node::type::wff_ex);
}

template<NodeType node>
bool is_functional_quantifier(tref n) {
	using tau = tree<node>;
	return tau::get(n).is(tau::bf_fall) || tau::get(n).is(tau::bf_fex);
}

template <NodeType node>
bool is_logical_or_functional_quant(tref n) {
	return is_quantifier<node>(n) || is_functional_quantifier<node>(n);
}

template <NodeType node>
bool contains(tref fm, tref sub_fm) {
#ifdef TAU_CACHE
	using tau = tree<node>;
	using cache_t = std::unordered_map<std::pair<tref, tref>, bool>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(std::make_pair(tau::trim_right_sibling(fm),
		tau::trim_right_sibling(sub_fm))); it != cache.end())
		return it->second;
#endif // TAU_CACHE
	bool is_contained = false;
	auto has_sub_fm = [&sub_fm, &is_contained](tref n) {
		if (tree<node>::subtree_equals(n, sub_fm))
			return is_contained = true, false;
		return true;
	};
	pre_order<node>(fm).search_unique(has_sub_fm);
#ifdef TAU_CACHE
	return cache.emplace(std::make_pair(tau::trim_right_sibling(fm),
		tau::trim_right_sibling(sub_fm)), is_contained).first->second;
#endif // TAU_CACHE
	return is_contained;
}

template <NodeType node>
std::function<bool(tref)> is_atomic_fm() {
	using tau = tree<node>;

	return [](tref n) -> bool {
		auto fm = tau::get(n);
		if (fm.children_size() != 1) return false;
		const tau& child = fm[0];
		return fm.is(tau::wff)
			&& (child.is(tau::bf_eq)
				|| child.is(tau::bf_neq)
				|| child.is(tau::bf_lteq)
				|| child.is(tau::bf_nlteq)
				|| child.is(tau::bf_gt)
				|| child.is(tau::bf_ngt)
				|| child.is(tau::bf_gteq)
				|| child.is(tau::bf_ngteq)
				|| child.is(tau::bf_lt)
				|| child.is(tau::bf_nlt));
	};
}

template <NodeType node>
int_t node_count (tref fm) {
#ifdef TAU_CACHE
	using tau = tree<node>;
	using cache_t = std::unordered_map<tref, int_t>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(tau::trim_right_sibling(fm)); it != cache.end())
		return it->second;
#endif // TAU_CACHE
	int_t c = 0;
	auto count = [&c](tref) {
		return ++c, true;
	};
	pre_order<node>(fm).visit(count);
#ifdef TAU_CACHE
	return cache.emplace(tau::trim_right_sibling(fm), c).first->second;
#endif // TAU_CACHE
	return c;
}

template <NodeType node>
auto visit_wff = [](tref n) static {
	return !tree<node>::get(n).is_term();
};

template <NodeType node>
auto is_boolean_operation = [](tref n) static {
	using tau = tree<node>;
	const tau& t = tau::get(n);
	if (t.is(tau::bf) || t.is(tau::bf_and) || t.is(tau::bf_or)
		|| t.is(tau::bf_xor) || t.is(tau::bf_neg)
		|| t.is(tau::bf_fex) || t.is(tau::bf_fall)) return true;
	return false;
};

template <NodeType node>
auto is_non_boolean_term = [](tref n) static {
	using tau = tree<node>;
	const tau& t = tau::get(n);
	if (t.is(tau::bf_add) || t.is(tau::bf_sub) || t.is(tau::bf_mul)
		|| t.is(tau::bf_div) || t.is(tau::bf_mod) || t.is(tau::bf_shr)
			|| t.is(tau::bf_shl) || t.is(tau::bf_nand)
			|| t.is(tau::bf_nor) || t.is(tau::bf_xnor))
		return true;
	return false;
};

template <NodeType node>
size_t find_ba_type (tref term) {
	using tau = tree<node>;
	size_t type = tau::get(term).get_ba_type();
	if (type != 0) return type;
	auto f = [&type](const tref n) {
		const auto& t = tau::get(n);
		if (t.is(tau::bf)) type = t.get_ba_type();
		if (type > 0) return false;
		return true;
	};
	pre_order<node>(term).search_unique(f);
	return type;
}

template <NodeType node>
tref find_ba_type_tree (tref term) {
	const size_t t = find_ba_type<node>(term);
	return get_ba_type_tree<node>(t);
}

// template <NodeType node>
// auto visit_io_vars = [] (tref n) static {
// 	auto nt = tree<node>::get(n).get_type();
// 	if (nt == node::type::ba_constant
// 		|| nt == node::type::uconst_name) return false;
// 	return true;
// };

} // namespace idni::tau_lang
