// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_arithmetic_scan"

namespace idni::tau_lang {

/** @internal @copydoc collect_bv_arithmetic_taint @endinternal */
template <NodeType node>
subtree_unordered_set<node> collect_bv_arithmetic_taint(tref formula) {
	using tau = tree<node>;
	subtree_unordered_set<node> tainted;
	auto is_arith_op = is<node>({ tau::bf_add, tau::bf_sub, tau::bf_mul,
		tau::bf_div, tau::bf_mod, tau::bf_shl, tau::bf_shr,
		tau::bf_cast });
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
