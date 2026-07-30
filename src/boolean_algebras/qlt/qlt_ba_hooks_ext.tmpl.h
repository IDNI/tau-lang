// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file qlt_ba_hooks_ext.tmpl.h
 * @brief qlt's comparison hooks, reached by core through ba_wff_hooks.
 *
 * qlt is a dense linear order, so a comparison of two ground constants decides
 * outright; anything else stays an atom for the QE and solver path, which core
 * does by passing the node through unchanged.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_BA_HOOKS_EXT_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_BA_HOOKS_EXT_TMPL_H__

#include "boolean_algebras/ba_descriptor.h"

namespace idni::tau_lang {

/**
 * @brief Compare two qlt singleton constants: -1/0/+1, or nullopt if either
 * side is not a finite qlt singleton.
 *
 * `bf_f` and `bf_t` act as the order's -inf and +inf sentinels.
 */
template<NodeType node>
static std::optional<int> qlt_singleton_cmp(
	const tree<node>& c1, const tree<node>& c2)
{
	using tau = tree<node>;
	const bool c1_neg_inf = c1.is(tau::bf_f);
	const bool c1_pos_inf = c1.is(tau::bf_t);
	const bool c2_neg_inf = c2.is(tau::bf_f);
	const bool c2_pos_inf = c2.is(tau::bf_t);
	const bool c1_known = c1_neg_inf || c1_pos_inf || c1.is_ba_constant();
	const bool c2_known = c2_neg_inf || c2_pos_inf || c2.is_ba_constant();
	if (!c1_known || !c2_known) return {}; // variable side: undetermined
	if (c1_neg_inf && c2_neg_inf) return 0;
	if (c1_pos_inf && c2_pos_inf) return 0;
	if (c1_neg_inf || c2_pos_inf) return -1; // -inf < anything, anything < +inf
	if (c1_pos_inf || c2_neg_inf) return +1; // +inf > anything, anything > -inf

	if (!c1.is_ba_constant() || !c2.is_ba_constant()) return {};
	auto v1 = c1.get_ba_constant();
	auto v2 = c2.get_ba_constant();
	if (!std::holds_alternative<qlt>(v1) || !std::holds_alternative<qlt>(v2))
		return {};
	const auto& q1 = std::get<qlt>(v1);
	const auto& q2 = std::get<qlt>(v2);
	if (q1.pieces.size() != 1 || q2.pieces.size() != 1) return {};
	const auto& p1 = q1.pieces[0];
	const auto& p2 = q2.pieces[0];
	if (p1.lo.val != p1.hi.val || p2.lo.val != p2.hi.val) return {};
	using bound_t = std::remove_cvref_t<decltype(p1.lo.bound)>;
	if (p1.lo.bound != bound_t::CLOSED || p1.hi.bound != bound_t::CLOSED) return {};
	if (p2.lo.bound != bound_t::CLOSED || p2.hi.bound != bound_t::CLOSED) return {};
	if (!p1.lo.val.is_finite() || !p2.lo.val.is_finite()) return {};
	if (p1.lo.val < p2.lo.val) return -1;
	if (p1.lo.val > p2.lo.val) return +1;
	return 0;
}

template <typename... PackBAs>
struct ba_wff_hooks<qlt, node<PackBAs...>> {
	using node_t = node<PackBAs...>;
	using tau = tree<node_t>;

	// ch[0] is the bf_<op> node; [i][0] is the i-th raw operand, matching
	// what core's own arg1/arg2 helpers read.
	static const tree<node_t>& arg1_hook(const tref* ch) {
		return tau::get(ch[0])[0][0];
	}
	static const tree<node_t>& arg2_hook(const tref* ch) {
		return tau::get(ch[0])[1][0];
	}

	/**
	 * @brief Wrap a decided comparison as T/F, or decline it.
	 *
	 * Declines when the operands carry different non-zero types, which is
	 * what core's _T/_F do by passing the node through instead.
	 */
	static tref decide(const tref* ch, tref r, bool value) {
		const size_t tl = arg1_hook(ch).get_ba_type();
		const size_t tr = arg2_hook(ch).get_ba_type();
		if (tl != tr && tl && tr) return nullptr;
		return tau::get(value ? tau::_T() : tau::_F(), r);
	}

	static tref eval(const tref* ch, tref r, auto holds) {
		auto cmp = qlt_singleton_cmp<node_t>(
			arg1_hook(ch), arg2_hook(ch));
		if (!cmp) return nullptr;
		return decide(ch, r, holds(*cmp));
	}

	static tref wff_lt(const tref* ch, tref r) {
		return eval(ch, r, [](int c) { return c < 0; });
	}
	static tref wff_nlt(const tref* ch, tref r) {
		return eval(ch, r, [](int c) { return c >= 0; });
	}
	static tref wff_lteq(const tref* ch, tref r) {
		return eval(ch, r, [](int c) { return c <= 0; });
	}
	static tref wff_nlteq(const tref* ch, tref r) {
		return eval(ch, r, [](int c) { return c > 0; });
	}
	static tref wff_gt(const tref* ch, tref r) {
		return eval(ch, r, [](int c) { return c > 0; });
	}
	static tref wff_ngt(const tref* ch, tref r) {
		return eval(ch, r, [](int c) { return c <= 0; });
	}
	static tref wff_gteq(const tref* ch, tref r) {
		return eval(ch, r, [](int c) { return c >= 0; });
	}
	static tref wff_ngteq(const tref* ch, tref r) {
		return eval(ch, r, [](int c) { return c < 0; });
	}
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_BA_HOOKS_EXT_TMPL_H__
