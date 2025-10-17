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
std::function<bool(tref)> is_basic_atomic_fm() {
	using tau = tree<node>;

	return [](tref n) -> bool {
		auto fm = tau::get(n);
		return fm.is(node::type::wff)
			&& (fm.child_is(node::type::bf_eq)
				|| fm.child_is(node::type::bf_neq));
	};
}

template <NodeType node>
std::function<bool(tref)> is_atomic_fm() {
	using tau = tree<node>;

	return [](tref n) -> bool {
		auto fm = tau::get(n);
		return fm.is(node::type::wff)
			&& (fm.child_is(node::type::bf_eq)
				|| fm.child_is(node::type::bf_neq)
				|| fm.child_is(node::type::bf_lteq)
				|| fm.child_is(node::type::bf_nlteq)
				|| fm.child_is(node::type::bf_gt)
				|| fm.child_is(node::type::bf_ngt)
				|| fm.child_is(node::type::bf_gteq)
				|| fm.child_is(node::type::bf_ngteq)
				|| fm.child_is(node::type::bf_lt)
				|| fm.child_is(node::type::bf_nlt));
	};
}

template <NodeType node>
std::function<bool(tref)> is_atomic_bv_fm() {
	using tau = tree<node>;

	return [](tref n) -> bool {
		auto fm = tau::get(n);
		return fm.is(node::type::wff)
			&& (fm.child_is(node::type::bv_eq)
				|| fm.child_is(node::type::bv_neq)
				|| fm.child_is(node::type::bv_lteq)
				|| fm.child_is(node::type::bv_nlteq)
				|| fm.child_is(node::type::bv_gt)
				|| fm.child_is(node::type::bv_ngt)
				|| fm.child_is(node::type::bv_gteq)
				|| fm.child_is(node::type::bv_ngteq)
				|| fm.child_is(node::type::bv_lt)
				|| fm.child_is(node::type::bv_nlt));
	};
}

template <NodeType node>
int_t node_count (tref fm) {
	int_t c = 0;
	auto count = [&c](tref) {
		return ++c, true;
	};
	pre_order<node>(fm).visit(count);
	return c;
}

template <NodeType node>
auto visit_wff = [](tref n) static {
	if (tree<node>::get(n).is(node::type::bf)) return false;
	return true;
};

template <NodeType node>
size_t find_ba_type (tref term) {
	using tau = tree<node>;
	size_t type = 0;
	auto f = [&type](const tref n) {
		const auto& t = tau::get(n);
		if (t.is(tau::bf) || t.is(tau::bv)) type = t.get_ba_type();
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
