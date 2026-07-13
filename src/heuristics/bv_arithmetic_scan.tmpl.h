// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_arithmetic_scan"

namespace idni::tau_lang {

/** @internal @copydoc collect_bv_arithmetic_taint @endinternal */
template <NodeType node>
subtree_unordered_set<node> collect_bv_arithmetic_taint(tref formula) {
	using tau = tree<node>;
	subtree_unordered_set<node> tainted;
	// Not `is<node>({...})` (the std::function-returning factory overload):
	// that overload's lambda captures the std::initializer_list by value,
	// which only copies a pointer to the backing array of the braced-init
	// temporary: the array's lifetime ends with this statement, so every
	// later call through the stored std::function reads a dangling array
	// (UB; silently "worked" under -O0, read garbage and matched nothing
	// under -O1+). Wrapping the plain (n, initializer_list) overload in our
	// own lambda instead re-materializes a fresh, live initializer_list on
	// every call.
	auto is_arith_op = [](tref n) {
		return is<node>(n, { tau::bf_add, tau::bf_sub, tau::bf_mul,
			tau::bf_div, tau::bf_mod, tau::bf_shl, tau::bf_shr,
			tau::bf_cast });
	};
	// Pass 1: bv-typed variables occurring under an arithmetic operator.
	for (tref op : tau::get(formula).select_all(is_arith_op))
		for (tref v : get_free_vars<node>(op))
			if (is_bv_type_family<node>(tau::get(v).get_ba_type()))
				tainted.insert(v);
	// Pass 2: atomic wffs (comparisons) whose subtree still contains an
	// arithmetic operator (blasting could not eliminate it).
	for (tref atom : tau::get(formula).select_top(is_atomic_fm<node>))
		if (tau::get(atom).find_top(is_arith_op))
			tainted.insert(atom);
	return tainted;
}

/** @internal @copydoc make_bv_arithmetic_skip @endinternal */
template <NodeType node>
std::function<bool(tref)> make_bv_arithmetic_skip(tref formula) {
	auto tainted = std::make_shared<subtree_unordered_set<node>>(
		collect_bv_arithmetic_taint<node>(formula));
	return [tainted](tref n) { return tainted->contains(n); };
}

} // namespace idni::tau_lang
