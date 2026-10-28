// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// hooks_tau.tmpl.h - Tau-specific hooks: temporal and path operators,
// conditional, implication, equivalence, order comparisons, interval, shift.

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "hooks"

namespace idni::tau_lang {


/** @internal @copydoc get_hook::unary_cte @endinternal */
template <NodeType node>
tref get_hook<node>::unary_cte(const tref* ch, tref r, bool keeps) {
	if (arg1(ch).is(tau::wff_t)) {
		HOOK_LOGGING(applied(keeps ? "op T ::= T." : "op T ::= F.");)
		return keeps ? bare_wff_T<node>(r) : bare_wff_F<node>(r);
	}
	if (arg1(ch).is(tau::wff_f)) {
		HOOK_LOGGING(applied(keeps ? "op F ::= F." : "op F ::= T.");)
		return keeps ? bare_wff_F<node>(r) : bare_wff_T<node>(r);
	}
	return nullptr;
}

/** @internal @copydoc get_hook::wff_unary_cte @endinternal */
template <NodeType node>
tref get_hook<node>::wff_unary_cte(const node& v, const tref* ch, size_t len,
	tref r)
{
	HOOK_LOGGING(log("wff_unary_cte", v, ch, len, r);)
	//RULE(WFF_SIMPLIFY_ONE_5, "always T ::= T.")
	//RULE(WFF_SIMPLIFY_ZERO_5, "always F ::= F.")
	//RULE(WFF_SIMPLIFY_ONE_6, "sometimes T ::= T.")
	//RULE(WFF_SIMPLIFY_ZERO_6, "sometimes F ::= F.")
	// `A`, `E` alike; `-T ::= F` and `-F ::= T`.
	if (tref c = unary_cte(ch, r,
		!logic_operator(ch).is(tau::wff_semantic_neg))) return c;
	return tau::get_raw(v, ch, len, r);
}

/** @internal @copydoc get_hook::wff_binary_temporal @endinternal */
// Constant-operand laws that need no other temporal operator on the result.
// `F U ψ`, `T R ψ` and `F W ψ` are ψ at the current step only: at the top
// level that is the first step, while a bare ψ there holds at every step,
// so they stay. `F S ψ` and `T T ψ` fold to ψ, since a top-level past
// formula holds at every step as well (scope_bare_clauses).
template <NodeType node>
tref get_hook<node>::wff_binary_temporal(const node& v, const tref* ch,
	size_t len, tref r)
{
	HOOK_LOGGING(log("wff_binary_temporal", v, ch, len, r);)
	const auto nt = logic_operator(ch).get_type();
	const bool l_t = arg1(ch).is(tau::wff_t), l_f = arg1(ch).is(tau::wff_f);
	const bool r_t = arg2(ch).is(tau::wff_t), r_f = arg2(ch).is(tau::wff_f);
	auto rhs = [&]() { return tau::get(arg2_fm(ch).get(), r); };
	switch (nt) {
	case tau::wff_until:      // φ U T = T, φ U F = F
	case tau::wff_release:    // φ R T = T, φ R F = F
	case tau::wff_since:      // φ S T = T, φ S F = F
	case tau::wff_trigger:    // φ T T = T, φ T F = F
		if (r_t) return bare_wff_T<node>(r);
		if (r_f) return bare_wff_F<node>(r);
		if (nt == tau::wff_since && l_f) return rhs();   // F S ψ = ψ
		if (nt == tau::wff_trigger && l_t) return rhs(); // T T ψ = ψ
		break;
	case tau::wff_weak_until: // φ W T = T, T W ψ = T
		if (r_t || l_t) return bare_wff_T<node>(r);
		break;
	default: break;
	}
	return tau::get_raw(v, ch, len, r);
}

/** @internal @copydoc get_hook::wff_conditional @endinternal */
template <NodeType node>
tref get_hook<node>::wff_conditional(
		[[maybe_unused]] const node& v,
		[[maybe_unused]] const tref* ch,
		[[maybe_unused]] size_t len,
		[[maybe_unused]] tref r) {
	HOOK_LOGGING(log("wff_conditional", v, ch, len, r);)
	//RULE(WFF_CONDITIONAL_SIMPLIFY_0, "F ? $X : $Y ::= $Y.")
	if (arg1(ch).is(tau::wff_f)) {
		HOOK_LOGGING(applied("F ? $X : $Y ::= $Y.");)
		return tau::get(arg3_fm(ch).get(), r);
	}
	//RULE(WFF_CONDITIONAL_SIMPLIFY_1, "T ? $X : $Y ::= $X.")
	if (arg1(ch).is(tau::wff_t)) {
		HOOK_LOGGING(applied("T ? $X : $Y ::= $X.");)
		return tau::get(arg2_fm(ch).get(), r);
	}
	//RULE(WFF_CONDITIONAL_SIMPLIFY_4, "$X ? $Y : $Y ::= $Y.")
	if (arg2_fm(ch) == arg3_fm(ch)) {
		HOOK_LOGGING(applied("$X ? $Y : $Y ::= $Y.");)
		return tau::get(arg3_fm(ch).get(), r);
	}
	return tau::get(tau::build_wff_conditional(
			arg1_fm(ch).get(), arg2_fm(ch).get(), arg3_fm(ch).get()), r);
}

/** @internal @copydoc get_hook::imply @endinternal */
template <NodeType node>
tref get_hook<node>::imply(const tree<node>& ante, const tree<node>& cons,
	tref r)
{
	//RULE(WFF_IMPLY_SIMPLIFY_0, "F -> $X ::= T.")
	if (ante[0].is(tau::wff_f)) {
		HOOK_LOGGING(applied("F -> $X ::= T.");)
		return bare_wff_T<node>(r);
	}
	//RULE(WFF_IMPLY_SIMPLIFY_1, "T -> $X ::= $X.")
	if (ante[0].is(tau::wff_t)) {
		HOOK_LOGGING(applied("T -> $X ::= $X.");)
		return tau::get(cons.get(), r);
	}
	//RULE(WFF_IMPLY_SIMPLIFY_2, "$X -> F ::= ! $X.")
	if (cons[0].is(tau::wff_f)) {
		HOOK_LOGGING(applied("$X -> F ::= ! $X.");)
		return tau::get(tau::build_wff_neg(ante.get()), r);
	}
	//RULE(WFF_IMPLY_SIMPLIFY_3, "$X -> T ::= T.")
	if (cons[0].is(tau::wff_t)) {
		HOOK_LOGGING(applied("$X -> T ::= T.");)
		return bare_wff_T<node>(r);
	}
	//RULE(WFF_IMPLY_SIMPLIFY_4, "$X -> $X ::= T.")
	if (ante == cons) {
		HOOK_LOGGING(applied("$X -> $X ::= T.");)
		return bare_wff_T<node>(r);
	}
	return tau::get(tau::build_wff_imply(ante.get(), cons.get()), r);
}

/** @internal @copydoc get_hook::wff_imply @endinternal */
template <NodeType node>
tref get_hook<node>::wff_imply([[maybe_unused]] const node& v, const tref* ch,
	[[maybe_unused]] size_t len, tref r)
{
	HOOK_LOGGING(log("wff_imply", v, ch, len, r);)
	return imply(arg1_fm(ch), arg2_fm(ch), r);
}

/** @internal @copydoc get_hook::wff_rimply @endinternal */
template <NodeType node>
tref get_hook<node>::wff_rimply([[maybe_unused]] const node& v, const tref* ch,
	[[maybe_unused]] size_t len, tref r)
{
	HOOK_LOGGING(log("wff_rimply", v, ch, len, r);)
	// `$X <- $Y` is `$Y -> $X`.
	return imply(arg2_fm(ch), arg1_fm(ch), r);
}

/** @internal @copydoc get_hook::wff_equiv @endinternal */
template <NodeType node>
tref get_hook<node>::wff_equiv([[maybe_unused]] const node& v, const tref* ch,
	[[maybe_unused]] size_t len, [[maybe_unused]] tref r)
{
	HOOK_LOGGING(log("wff_equiv", v, ch, len, r);)
	//RULE(WFF_EQUIV_SIMPLIFY_0, "F <-> $X ::= ! $X.")
	if (arg1(ch).is(tau::wff_f)) {
		HOOK_LOGGING(applied("F <-> $X ::= ! $X.");)
		return tau::get(tau::build_wff_neg(arg2_fm(ch).get()), r);
	}
	//RULE(WFF_EQUIV_SIMPLIFY_1, "T <-> $X ::= $X.")
	if (arg1(ch).is(tau::wff_t)) {
		HOOK_LOGGING(applied("T <-> $X ::= $X.");)
		return tau::get(arg2_fm(ch).get(), r);
	}
	//RULE(WFF_EQUIV_SIMPLIFY_2, "$X <-> F ::= ! $X.")
	if (arg2(ch).is(tau::wff_f)) {
		HOOK_LOGGING(applied("$X <-> F ::= ! $X.");)
		return tau::get(tau::build_wff_neg(arg1_fm(ch).get()), r);
	}
	//RULE(WFF_EQUIV_SIMPLIFY_3, "$X <-> T ::= $X.")
	if (arg2(ch).is(tau::wff_t)) {
		HOOK_LOGGING(applied("$X <-> T ::= $X.");)
		return tau::get(arg1_fm(ch).get(), r);
	}
	//RULE(WFF_EQUIV_SIMPLIFY_4, "$X <-> $X ::= T.")
	if (arg1_fm(ch) == arg2_fm(ch)) {
		HOOK_LOGGING(applied("$X <-> $X ::= T.");)
		return bare_wff_T<node>(r);
	}
	//RULE(WFF_EQUIV_SIMPLIFY_5, "$X <-> ! $X ::= F.")
	if (auto negated = tt(arg2_fm(ch)) | tau::wff_neg | tau::wff;
		negated && tau::get(negated.value()) == arg1_fm(ch)) {
		HOOK_LOGGING(applied("$X <-> ! $X ::= F.");)
		return bare_wff_F<node>(r);
	}
	//RULE(WFF_EQUIV_SIMPLIFY_6, "! $X <-> $X ::= F.")
	if (auto negated = tt(arg1_fm(ch)) | tau::wff_neg | tau::wff;
		negated && tau::get(negated.value()) == arg2_fm(ch)) {
		HOOK_LOGGING(applied("! $X <-> $X ::= F.");)
		return bare_wff_F<node>(r);
	}
	return tau::get(tau::build_wff_equiv(arg1_fm(ch).get(), arg2_fm(ch).get()), r);
}

namespace hooks_detail {

/// The rules of one order comparison `a op b`.
struct order_rules {
	const char* hook;           ///< the hook name, for the hook log
	const char* sym;            ///< the operator, for the hook log
	order_fold pair[2][2];      ///< both operands 0 or 1: [a is 1][b is 1]
	order_fold lhs[2];          ///< only a is 0 or 1: [a is 1]
	order_fold rhs[2];          ///< only b is 0 or 1: [b is 1]
	order_fold self;            ///< `x op x`
	bool self_needs_total;      ///< `self` holds only where the order is total
	// The Boolean-lattice definition of the operator: `a <= b` is
	// `a & b' = 0`, `a < b` is that and `a != b`; `negated` takes the
	// complement, `swapped` reads `b op a`.
	bool strict, negated, swapped;
};

constexpr order_rules order_rules_of(order_op op) {
	using enum order_fold;
	switch (op) {
	case order_op::lt: return { "wff_lt", "<",
		{ { F, T }, { F, F } }, { none, F }, { F, none }, F, false,
		true, false, false };
	case order_op::nlt: return { "wff_nlt", "!<",
		{ { T, F }, { T, T } }, { eq0, T }, { T, neg_eq0 }, T, false,
		true, true, false };
	case order_op::lteq: return { "wff_lteq", "<=",
		{ { T, T }, { F, T } }, { T, neg_eq0 }, { eq0, T }, T, true,
		false, false, false };
	case order_op::nlteq: return { "wff_nlteq", "!<=",
		{ { F, F }, { T, F } }, { F, none }, { none, F }, F, true,
		false, true, false };
	case order_op::gt: return { "wff_gt", ">",
		{ { F, F }, { T, F } }, { F, none }, { none, F }, F, false,
		true, false, true };
	case order_op::ngt: return { "wff_ngt", "!>",
		{ { T, T }, { F, T } }, { T, none }, { none, T }, T, false,
		true, true, true };
	case order_op::gteq: return { "wff_gteq", ">=",
		{ { T, F }, { T, T } }, { none, T }, { T, none }, T, true,
		false, false, true };
	case order_op::ngteq: return { "wff_ngteq", "!>=",
		{ { F, T }, { F, F } }, { none, F }, { F, none }, F, true,
		false, true, true };
	}
	return {};
}

#define TAU_HOOKS_ORDER_OPS(X) \
	X(lt) X(nlt) X(lteq) X(nlteq) X(gt) X(ngt) X(gteq) X(ngteq)

/// `try_wff_<op>` of the order comparison @p op: the owner's fold, nullopt
/// when none folds.
template <order_op op, typename node_t>
std::optional<tref> try_wff_order(const tref* ch, tref r, size_t ba_type) {
#define TAU_HOOKS_ORDER_TRY(name) if constexpr (op == order_op::name) \
		return try_wff_##name<node_t>(ch, r, ba_type); else
	TAU_HOOKS_ORDER_OPS(TAU_HOOKS_ORDER_TRY)
	return std::nullopt;
#undef TAU_HOOKS_ORDER_TRY
}

/// `pack_ba_type_has_wff_<op>_hook` of the order comparison @p op: whether
/// the owner of @p ba_type defines the operator at all.
template <order_op op, typename node_t>
bool has_wff_order_hook(size_t ba_type) {
#define TAU_HOOKS_ORDER_HAS(name) if constexpr (op == order_op::name) \
		return pack_ba_type_has_wff_##name##_hook<node_t>(ba_type); else
	TAU_HOOKS_ORDER_OPS(TAU_HOOKS_ORDER_HAS)
	return false;
#undef TAU_HOOKS_ORDER_HAS
}

#undef TAU_HOOKS_ORDER_OPS

#ifdef HOOK_LOGGING_ENABLED
// The rule an order hook applied, as `0 < 1 ::= T.`; k1 / k2 are 0 or 1 for a
// constant operand, -1 for `$X`.
inline std::string order_rule_text(const order_rules& R, int k1, int k2,
	order_fold f)
{
	auto side = [](int k) { return k < 0 ? std::string("$X")
		: std::string(k ? "1" : "0"); };
	static const char* folds[] = { "", "T", "F", "$X = 0", "$X != 0",
		"$X' = 0", "$X' != 0" };
	return side(k1) + " " + R.sym + " " + (k1 < 0 && k2 < 0 ? "$X"
		: side(k2)) + " ::= " + folds[static_cast<int>(f)] + ".";
}
#endif // HOOK_LOGGING_ENABLED

} // namespace hooks_detail

/** @internal @copydoc get_hook::order_fold_to @endinternal */
template <NodeType node>
tref get_hook<node>::order_fold_to(hooks_detail::order_fold f, const tau& x,
	const node& v, const tref* ch, size_t len, tref r)
{
	using enum hooks_detail::order_fold;
	switch (f) {
	case none: return nullptr;
	case T: return _T(v, ch, len, r);
	case F: return _F(v, ch, len, r);
	default: break;
	}
	// Rewriting an order comparison into a complement equality holds only
	// where < is a Boolean lattice order, not where it is a linear one.
	if (pack_type_is_non_aba_omcat<node>(arg1_fm(ch).get_ba_type())
		|| pack_type_is_non_aba_omcat<node>(arg2_fm(ch).get_ba_type()))
		return nullptr;
	const tref t = f == neg_eq0 || f == neg_neq0
		? tau::build_bf_neg(x.get()) : x.get();
	return tau::get(f == eq0 || f == neg_eq0 ? tau::build_bf_eq_0(t)
		: tau::build_bf_neq_0(t), r);
}

/** @internal @copydoc get_hook::wff_order @endinternal */
template <NodeType node>
template <hooks_detail::order_op op>
tref get_hook<node>::wff_order(const node& v, const tref* ch, size_t len, tref r) {
	using namespace hooks_detail;
	static constexpr order_rules R = order_rules_of(op);
	HOOK_LOGGING(log(R.hook, v, ch, len, r);)
	const tau& a1 = arg1(ch);
	const tau& a2 = arg2(ch);
	const int k1 = a1.is(tau::bf_f) ? 0 : a1.is(tau::bf_t) ? 1 : -1;
	const int k2 = a2.is(tau::bf_f) ? 0 : a2.is(tau::bf_t) ? 1 : -1;
	if (k1 >= 0 && k2 >= 0) {
		HOOK_LOGGING(applied(order_rule_text(R, k1, k2, R.pair[k1][k2]));)
		return R.pair[k1][k2] == order_fold::T ? _T(v, ch, len, r)
			: _F(v, ch, len, r);
	}
	if (k1 >= 0 || k2 >= 0) {
		const order_fold f = k1 >= 0 ? R.lhs[k1] : R.rhs[k2];
		const tau& x = k1 >= 0 ? arg2_fm(ch) : arg1_fm(ch);
		if (tref res = order_fold_to(f, x, v, ch, len, r)) {
			HOOK_LOGGING(applied(order_rule_text(R, k1, k2, f));)
			return res;
		}
	}
	if (R.self != order_fold::none && arg1_fm(ch) == arg2_fm(ch)
		&& (!R.self_needs_total
			|| order_is_total<node>(arg1_fm(ch).get(), arg2_fm(ch).get())))
	{
		HOOK_LOGGING(applied(order_rule_text(R, -1, -1, R.self));)
		return R.self == order_fold::T ? _T(v, ch, len, r)
			: _F(v, ch, len, r);
	}
	// The owning BA may define the operator differently
	const size_t cmp_ba = comparison_ba_type<node>(arg1_fm(ch), arg2_fm(ch));
	if (auto hook_r = try_wff_order<op, node>(ch, r, cmp_ba)) {
		HOOK_LOGGING(applied("Using the BA's own definition for "
			+ std::string(R.sym) + ".");)
		return *hook_r;
	}
	if (has_wff_order_hook<op, node>(cmp_ba))
		return tau::get_raw(v, ch, len, r);
	// The Boolean-lattice definition: `a <= b` is `a & b' = 0`.
	tref a = arg1_fm(ch).get(), b = arg2_fm(ch).get();
	if (R.swapped) std::swap(a, b);
	tref diff = build_bf_and<node>(a, build_bf_neg<node>(b));
	tref res;
	if (!R.strict) res = R.negated ? build_bf_neq_0<node>(diff)
		: build_bf_eq_0<node>(diff);
	else res = R.negated
		? build_wff_or<node>(build_bf_neq_0<node>(diff),
			build_bf_eq<node>(a, b))
		: build_wff_and<node>(build_bf_eq_0<node>(diff),
			build_bf_neq<node>(a, b));
	return tau::get(res, r);
}

/** @internal @copydoc get_hook::wff_interval @endinternal */
template <NodeType node>
tref get_hook<node>::wff_interval([[maybe_unused]] const node& v, const tref* ch,
	[[maybe_unused]] size_t len, [[maybe_unused]] tref r)
{
	HOOK_LOGGING(log("wff_interval", v, ch, len, r);)

	// Every type reads `x <= y <= z` as `x <= y && y <= z`
	return tau::get(tau::build_bf_interval(
		arg1_fm(ch).get(), arg2_fm(ch).get(), arg3_fm(ch).get()), r);
}

/** @internal @copydoc get_hook::shift @endinternal */
// Folds `n - k` over constants to the integer n - k; keeps a variable or
// capture offset raw, and returns nullptr when n < k: the rewriter reads a
// null instantiated body as "this rule does not apply", so a recurrence
// step that would reach a negative index is not taken.
template <NodeType node>
tref get_hook<node>::shift(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("shift", v, ch, len, r);)
	DBG(assert(len == 2);)
	// apply numerical simplifications
	// This node must have two children
	// The first node is either p::variable, p::capture, p::num or p::integer
	// The second node must be p::num
	int_t left = -1;
	const auto& c0 = tau::get(ch[0]);
	if (c0.is(tau::integer)) left = c0.get_integer();
	else if (c0.is(tau::num)) left = get_payload_int<node>(c0);
	if (left < 0) {
		DBG(assert(c0.is(tau::variable) || c0.is(tau::capture));)
		return tau::get_raw(v, ch, len, r);
	}
	int_t right = -1;
	if (tau::get(ch[1]).is(tau::num))
		right = get_payload_int<node>(tau::get(ch[1]));
	DBG(assert(right >= 0);)
	if (left >= right) return tau::get(tau::get_integer(left - right), r);
	return nullptr; // a negative time point: the rule does not apply
}

} // namespace idni::tau_lang
