// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_tree.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "tau_tree_queries"

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
bool is_io_var(tref n) {
	return tree<node>::get(n).is_input_variable()
		|| tree<node>::get(n).is_output_variable();
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
};

template <NodeType node>
bool contains(tref fm, tref sub_fm) {
	bool is_contained = false;
	auto has_sub_fm = [&sub_fm, &is_contained](tref n) {
		if (tree<node>::subtree_equals(n, sub_fm))
			return is_contained = true, false;
		return true;
	};
	pre_order<node>(fm).search_unique(has_sub_fm);
	return is_contained;
}

template <NodeType node>
auto visit_wff = [](tref n) static {
	if (tree<node>::get(n).is(node::type::bf)) return false;
	return true;
};

// template <NodeType node>
// auto visit_io_vars = [] (tref n) static {
// 	auto nt = tree<node>::get(n).get_type();
// 	if (nt == node::type::bf_constant
// 		|| nt == node::type::uconst_name) return false;
// 	return true;
// };

} // idni::tau_lang namespace
