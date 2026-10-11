// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file qlt_ba_hooks_ext.tmpl.h
 * @brief qlt's comparison hooks, reached by core through ba_wff_hooks.
 *
 * qlt is a dense linear order, so a comparison of two ground constants decides
 * outright (every point of one side below every point of the other); anything
 * else stays an atom for the QE and solver path, which core does by passing
 * the node through unchanged. The typed 0 and 1 are the order's
 * ends, not points, so a variable's equality with either decides as well.
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

/**
 * @brief One side of an order comparison: the typed 0 (`end` -1, below every
 * point), the typed 1 (`end` 1, above every point), or a set of points
 * (`end` 0), a variable being the set holding its point.
 */
struct qlt_order_side {
	/// -1 for the typed 0, 1 for the typed 1, 0 for a set of points.
	int end = 0;
	/// The points when `end` is 0; unused otherwise.
	qlt points;
};

/**
 * @brief `a < b` (@p strict) or `a <= b` between two sides: every point of
 * @p a is below (at most) every point of @p b, so an empty side makes it
 * true. nullopt when an end needed is a named endpoint or the two ends
 * do not compare.
 */
inline std::optional<bool> qlt_order_holds(const qlt_order_side& a,
	const qlt_order_side& b, bool strict)
{
	if (a.end == -1 || b.end == 1)
		return !(strict && a.end == b.end);
	if (a.end == 1) return b.end == 0 && b.points.is_empty();
	if (b.end == -1) return a.points.is_empty();
	if (a.points.is_empty() || b.points.is_empty()) return true;
	const qlt_endpoint& hi = a.points.pieces.back().hi;
	const qlt_endpoint& lo = b.points.pieces.front().lo;
	if (hi.val.is_sym() || lo.val.is_sym()) return std::nullopt;
	const auto c = qlt_sem_cmp(hi.val, lo.val);
	if (c == std::partial_ordering::less) return true;
	if (c == std::partial_ordering::greater) return false;
	if (c != std::partial_ordering::equivalent) return std::nullopt;
	return !strict || hi.bound == qlt_bound::OPEN
		|| lo.bound == qlt_bound::OPEN;
}

/**
 * @brief The side a constant, the typed 0 or the typed 1 gives a comparison;
 * nullopt for anything else or an inexact constant.
 */
template<NodeType node>
static std::optional<qlt_order_side> qlt_constant_side(const tree<node>& c) {
	using tau = tree<node>;
	if (c.is(tau::bf_f)) return qlt_order_side{ -1, {} };
	if (c.is(tau::bf_t)) return qlt_order_side{ 1, {} };
	if (!c.is_ba_constant()) return std::nullopt;
	auto v = c.get_ba_constant();
	if (!std::holds_alternative<qlt>(v)) return std::nullopt;
	const qlt& q = std::get<qlt>(v);
	if (q.inexact) return std::nullopt;
	return qlt_order_side{ 0, q };
}

// Defined in qlt_qe.tmpl.h, which the descriptor includes after this file.
/// The truth of comparison @p op between ground @p lhs and @p rhs over named
/// endpoints when it holds wherever the names lie; nullopt otherwise.
template<NodeType node>
static std::optional<bool> qlt_named_ground_truth(size_t op, tref lhs,
	tref rhs);

/**
 * @brief qlt's comparison hooks. Each `wff_*` member takes the children @p ch
 * of the comparison node @p r and returns its T/F replacement, or nullptr to
 * decline and leave the atom to the generic path.
 */
template <typename... PackBAs>
struct ba_wff_hooks<qlt, node<PackBAs...>> {
	using node_t = node<PackBAs...>;
	using tau = tree<node_t>;

	// ch[0] is the bf_<op> node; [i][0] is the i-th raw operand, matching
	// what core's own arg1/arg2 helpers read. arg1_hook returns the left
	// operand, arg2_hook the right one.
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

	/**
	 * @brief A comparison of terms without variables that hold named
	 * endpoints, decided when it has the same truth wherever the names lie.
	 */
	static tref named(const tref* ch, tref r) {
		const auto& c = tau::get(ch[0]);
		auto h = qlt_named_ground_truth<node_t>(
			static_cast<size_t>(c.value.nt), c.first(), c.second());
		if (!h) return nullptr;
		return decide(ch, r, *h);
	}

	// `lhs < rhs` (strict) or `lhs <= rhs`, with the sides swapped for
	// > and >= and the verdict negated for the n-forms. Tries `named` first;
	// nullptr when a side is not a constant or an end, or the order of the
	// two sides is undetermined.
	static tref eval(const tref* ch, tref r, bool swap, bool strict,
		bool negate)
	{
		if (tref d = named(ch, r)) return d;
		auto a = qlt_constant_side<node_t>(arg1_hook(ch));
		auto b = qlt_constant_side<node_t>(arg2_hook(ch));
		if (!a || !b) return nullptr;
		auto h = swap ? qlt_order_holds(*b, *a, strict)
			: qlt_order_holds(*a, *b, strict);
		if (!h) return nullptr;
		return decide(ch, r, *h != negate);
	}

	/// Decides `a < b`; see eval.
	static tref wff_lt(const tref* ch, tref r) {
		return eval(ch, r, false, true, false);
	}
	/// Decides `a !< b`; see eval.
	static tref wff_nlt(const tref* ch, tref r) {
		return eval(ch, r, false, true, true);
	}
	/// Decides `a <= b`; see eval.
	static tref wff_lteq(const tref* ch, tref r) {
		return eval(ch, r, false, false, false);
	}
	/// Decides `a !<= b`; see eval.
	static tref wff_nlteq(const tref* ch, tref r) {
		return eval(ch, r, false, false, true);
	}
	/// Decides `a > b`; see eval.
	static tref wff_gt(const tref* ch, tref r) {
		return eval(ch, r, true, true, false);
	}
	/// Decides `a !> b`; see eval.
	static tref wff_ngt(const tref* ch, tref r) {
		return eval(ch, r, true, true, true);
	}
	/// Decides `a >= b`; see eval.
	static tref wff_gteq(const tref* ch, tref r) {
		return eval(ch, r, true, false, false);
	}
	/// Decides `a !>= b`; see eval.
	static tref wff_ngteq(const tref* ch, tref r) {
		return eval(ch, r, true, false, true);
	}

	/**
	 * @brief `v = 0`, `v = 1` and their disequalities for a variable v.
	 *
	 * No point sits at an end of the order, so the equality is F and the
	 * disequality T. A compound term declines: `x & {3} = 0` is a meet, not
	 * a point.
	 */
	static tref end_eq(const tref* ch, tref r, bool is_eq) {
		if (tref d = named(ch, r)) return d;
		const auto& a = arg1_hook(ch);
		const auto& b = arg2_hook(ch);
		auto is_end = [](const tree<node_t>& t) {
			return t.is(tau::bf_f) || t.is(tau::bf_t);
		};
		if (!(a.is(tau::variable) && is_end(b))
			&& !(b.is(tau::variable) && is_end(a))) return nullptr;
		return decide(ch, r, !is_eq);
	}
	/// Decides `a = b` when one side is a variable and the other an end.
	static tref wff_eq(const tref* ch, tref r) {
		return end_eq(ch, r, true);
	}
	/// Decides `a != b` when one side is a variable and the other an end.
	static tref wff_neq(const tref* ch, tref r) {
		return end_eq(ch, r, false);
	}
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_BA_HOOKS_EXT_TMPL_H__
