// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_tree.h"

namespace idni::tau_lang {

template <NodeType node>
bool is_non_terminal(tref n) {
	return tree<node>::get(n).is_nt();
}

template <NodeType node>
bool is_terminal(tref n) {
	return tree<node>::get(n).is_t();
}

template <NodeType node>
bool is_non_terminal(const size_t nt, tref n) {
	return tree<node>::get(n).is(nt);
}

template <NodeType node>
bool is_child_non_terminal(const size_t nt, tref n) {
	return tree<node>::get(n).child_is(nt);
}

template <size_t nt, NodeType node>
bool is_non_terminal(tref n) { return is_non_terminal<node>(nt, n); }

template <size_t nt, NodeType node>
bool is_child_non_terminal(tref n) { return is_child_non_terminal<node>(nt, n); }

// factory method for is_non_terminal predicate
template <NodeType node>
inline std::function<bool(tref)> is_non_terminal(size_t nt) {
	return [nt](tref n) { return is_non_terminal<node>(nt, n); };
}

// factory method for is_non_terminal predicate
template <NodeType node>
inline std::function<bool(tref)> is_child_non_terminal(size_t nt) {
	return [nt](tref n) { return is_child_non_terminal<node>(nt, n); };
}

template <NodeType node>
auto visit_wff = [](tref n) static {
	if (tree<node>::get(n).is(node::type::bf)) return false;
	return true;
};

template <NodeType node>
auto visit_io_vars = [] (tref n) static {
	auto nt = tree<node>::get(n).get_type();
	if (nt == node::type::bf_constant
		|| nt == node::type::uninterpreted_constant) return false;
	return true;
};

} // idni::tau_lang namespace
