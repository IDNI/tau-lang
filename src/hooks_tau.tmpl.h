// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// hooks_tau.tmpl.h - Tau-specific hooks: temporal, quantifier, equiv, comparison ops
// Split from hooks.tmpl.h for readability.

namespace idni::tau_lang {


template <NodeType node>
tref get_hook<node>::wff_sometimes(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_sometimes", v, ch, len, r);)
	//RULE(WFF_SIMPLIFY_ONE_6, " sometimes T ::= T.")
	if (arg1(ch).is(tau::wff_t)) {
		HOOK_LOGGING(applied("sometimes T ::= T.");)
		return bare_wff_T<node>(r);
	}
	//RULE(WFF_SIMPLIFY_ZERO_6, "sometimes F ::= F.")
	if (arg1(ch).is(tau::wff_f)) {
		HOOK_LOGGING(applied("sometimes F ::= F.");)
		return bare_wff_F<node>(r);
	}
	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::wff_always(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_always", v, ch, len, r);)
	//RULE(WFF_SIMPLIFY_ONE_5, " always T ::= T.")
	if (arg1(ch).is(tau::wff_t)) {
		HOOK_LOGGING(applied("always T ::= T.");)
		return bare_wff_T<node>(r);
	}
	//RULE(WFF_SIMPLIFY_ZERO_5, "always F ::= F.")
	if (arg1(ch).is(tau::wff_f)) {
		HOOK_LOGGING(applied("always F ::= F.");)
		return bare_wff_F<node>(r);
	}
	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::wff_A(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_A", v, ch, len, r);)
	if (arg1(ch).is(tau::wff_t)) {
		HOOK_LOGGING(applied("A T ::= T.");)
		return bare_wff_T<node>(r);
	}
	if (arg1(ch).is(tau::wff_f)) {
		HOOK_LOGGING(applied("A F ::= F.");)
		return bare_wff_F<node>(r);
	}
	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::wff_E(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_E", v, ch, len, r);)
	if (arg1(ch).is(tau::wff_t)) {
		HOOK_LOGGING(applied("E T ::= T.");)
		return bare_wff_T<node>(r);
	}
	if (arg1(ch).is(tau::wff_f)) {
		HOOK_LOGGING(applied("E F ::= F.");)
		return bare_wff_F<node>(r);
	}
	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::wff_semantic_neg(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_semantic_neg", v, ch, len, r);)
	if (arg1(ch).is(tau::wff_t)) {
		HOOK_LOGGING(applied("-T ::= F.");)
		return bare_wff_F<node>(r);
	}
	if (arg1(ch).is(tau::wff_f)) {
		HOOK_LOGGING(applied("-F ::= T.");)
		return bare_wff_T<node>(r);
	}
	return tau::get_raw(v, ch, len, r);
}

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

template <NodeType node>
tref get_hook<node>::wff_imply([[maybe_unused]] const node& v, const tref* ch,
	[[maybe_unused]] size_t len, [[maybe_unused]] tref r)
{
	HOOK_LOGGING(log("wff_imply", v, ch, len, r);)
	//RULE(WFF_IMPLY_SIMPLIFY_0, "F -> $X ::= T.")
	if (arg1(ch).is(tau::wff_f)) {
		HOOK_LOGGING(applied("F -> $X ::= T.");)
		return bare_wff_T<node>(r);
	}
	//RULE(WFF_IMPLY_SIMPLIFY_1, "T -> $X ::= $X.")
	if (arg1(ch).is(tau::wff_t)) {
		HOOK_LOGGING(applied("T -> $X ::= $X.");)
		return tau::get(arg2_fm(ch).get(), r);
	}
	//RULE(WFF_IMPLY_SIMPLIFY_2, "$X -> F ::= ! $X.")
	if (arg2(ch).is(tau::wff_f)) {
		HOOK_LOGGING(applied("$X -> F ::= ! $X.");)
		return tau::get(tau::build_wff_neg(arg1_fm(ch).get()), r);
	}
	//RULE(WFF_IMPLY_SIMPLIFY_3, "$X -> T ::= T.")
	if (arg2(ch).is(tau::wff_t)) {
		HOOK_LOGGING(applied("$X -> T ::= T.");)
		return bare_wff_T<node>(r);
	}
	//RULE(WFF_IMPLY_SIMPLIFY_4, "$X -> $X ::= T.")
	if (arg1_fm(ch) == arg2_fm(ch)) {
		HOOK_LOGGING(applied("$X -> $X ::= T.");)
		return bare_wff_T<node>(r);
	}
	return tau::get(tau::build_wff_imply(arg1_fm(ch).get(), arg2_fm(ch).get()), r);
}

template <NodeType node>
tref get_hook<node>::wff_rimply([[maybe_unused]] const node& v, const tref* ch,
	[[maybe_unused]] size_t len, [[maybe_unused]] tref r)
{
	HOOK_LOGGING(log("wff_rimply", v, ch, len, r);)
	//RULE(WFF_RIMPLY_SIMPLIFY_0, "F <- $X ::= T.")
	if (arg2(ch).is(tau::wff_f)) {
		HOOK_LOGGING(applied("F <- $X ::= T.");)
		return bare_wff_T<node>(r);
	}
	//RULE(WFF_RIMPLY_SIMPLIFY_1, "T <- $X ::= $X.")
	if (arg2(ch).is(tau::wff_t)) {
		HOOK_LOGGING(applied("T <- $X ::= $X.");)
		return tau::get(arg1_fm(ch).get(), r);
	}
	//RULE(WFF_RIMPLY_SIMPLIFY_2, "$X <- F ::= ! $X.")
	if (arg1(ch).is(tau::wff_f)) {
		HOOK_LOGGING(applied("$X <- F ::= ! $X.");)
		return tau::get(tau::build_wff_neg(arg2_fm(ch).get()), r);
	}
	//RULE(WFF_RIMPLY_SIMPLIFY_3, "$X <- T ::= T.")
	if (arg1(ch).is(tau::wff_t)) {
		HOOK_LOGGING(applied("$X <- T ::= T.");)
		return bare_wff_T<node>(r);
	}
	//RULE(WFF_RIMPLY_SIMPLIFY_4, "$X <- $X ::= T.")
	if (arg1_fm(ch) == arg2_fm(ch)) {
		HOOK_LOGGING(applied("$X <- $X ::= T.");)
		return bare_wff_T<node>(r);
	}
	return tau::get(tau::build_wff_imply(arg2_fm(ch).get(), arg1_fm(ch).get()), r);
}

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

// Unit elimination for (Q,<): compare two qlt singleton constants.
// Returns -1/0/+1 if c1 </=/> c2, or nullopt if either side is not a finite
// qlt singleton constant.
template<NodeType node>
static std::optional<int> qlt_singleton_cmp(
    const tree<node>& c1, const tree<node>& c2)
{
    if constexpr (ba_variant_includes_v<qlt, typename tree<node>::constant>) {
        using tau = tree<node>;
        // bf_f = -∞, bf_t = +∞ as DLO sentinels; only compare if both sides are known
        const bool c1_neg_inf = c1.is(tau::bf_f);
        const bool c1_pos_inf = c1.is(tau::bf_t);
        const bool c2_neg_inf = c2.is(tau::bf_f);
        const bool c2_pos_inf = c2.is(tau::bf_t);
        const bool c1_known = c1_neg_inf || c1_pos_inf || c1.is_ba_constant();
        const bool c2_known = c2_neg_inf || c2_pos_inf || c2.is_ba_constant();
        if (!c1_known || !c2_known) return {}; // variable side: undetermined
        if (c1_neg_inf && c2_neg_inf) return 0;
        if (c1_pos_inf && c2_pos_inf) return 0;
        if (c1_neg_inf || c2_pos_inf) return -1; // -∞ < anything, anything < +∞
        if (c1_pos_inf || c2_neg_inf) return +1; // +∞ > anything, anything > -∞

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
    return {};
}

template <NodeType node>
tref get_hook<node>::wff_lt(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_lt", v, ch, len, r);)

	// if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

	//RULE(BF_LESS_SIMPLIFY_20, "0 < 1 ::= T.")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("0 < 1 ::= T.");)
		return _T(v, ch, len, r);
	}
	//RULE(BF_LESS_SIMPLIFY_21, "1 < 0 ::= F.")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("1 < 0 ::= F.");)
		return _F(v, ch, len, r);
	}
	//RULE(BF_LESS_SIMPLIFY_0, "0 < 0 ::= F.")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("0 < 0 ::= F.");)
		return _F(v, ch, len, r);
	}
	//RULE(BF_DEF_SIMPLIFY_N, "1 < 1 ::= F.")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("1 < 1 ::= F.");)
		return _F(v, ch, len, r);
	}
	//RULE(BF_LESS_SIMPLIFY_0, "$X < 0 ::= F.") @CP
	if (arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("$X < 0 ::= F.");)
		return _F(v, ch, len, r);
	}
	//RULE(BF_DEF_SIMPLIFY_N, "1 < $X ::= F.")
	if (arg1(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("1 < $X ::= F.");)
		return _F(v, ch, len, r);
	}

	if (arg1_fm(ch) == arg2_fm(ch)) {
		HOOK_LOGGING(applied("$X < $X ::= F.");)
		return _F(v, ch, len, r);
	}

	// The definition for the operator for bitvectors is different
	if (is_bv_type_family<node>(arg1_fm(ch).get_ba_type())) {
		if (tref bv_r = wff_bv_lt<node>(ch, r)) {
			HOOK_LOGGING(applied("Using bitvector-specific definition for <.");)
			return bv_r;
		}
		return tau::get_raw(v, ch, len, r);
	}
	// omcat types (e.g., qlt): unit-eliminate ground constant comparisons; pass through for cvc5
	{
		auto ba = arg1_fm(ch).get_ba_type();
		if (!is_omcat_type_family<node>(ba)) ba = arg2_fm(ch).get_ba_type();
		if (is_omcat_type_family<node>(ba)) {
			if (auto cmp = qlt_singleton_cmp<node>(arg1(ch), arg2(ch)); cmp)
				return (*cmp < 0) ? _T(v, ch, len, r) : _F(v, ch, len, r);
			return tau::get_raw(v, ch, len, r);
		}
	}

	return tau::get(build_wff_and<node>(
		build_bf_eq_0<node>(build_bf_and<node>(arg1_fm(ch).get(),
				build_bf_neg<node>(arg2_fm(ch).get()))),
		build_bf_neq<node>(arg1_fm(ch).get(), arg2_fm(ch).get())), r);
}

template <NodeType node>
tref get_hook<node>::wff_nlt(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_nlt", v, ch, len, r);)

	// if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

	//RULE(BF_NLESS_SIMPLIFY_20, "0 !< 1 ::= F.")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("0 !< 1 ::= F.");)
		return _F(v, ch, len, r);
	}
	//RULE(BF_NLESS_SIMPLIFY_21, "1 !< 0 ::= T.")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("1 !< 0 ::= T.");)
		return _T(v, ch, len, r);
	}
	//RULE(BF_NLESS_SIMPLIFY_0, "0 !< 0 ::= T.")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("0 !< 0 ::= T.");)
		return _T(v, ch, len, r);
	}
	//RULE(BF_DEF_SIMPLIFY_N, "1 !< 1 ::= T.")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("1 !< 1 ::= T.");)
		return _T(v, ch, len, r);
	}
	//RULE(BF_NLESS_SIMPLIFY_0, "$X !< 0 ::= T.") @CP
	if (arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("$X !< 0 ::= T.");)
		return _T(v, ch, len, r);
	}
	//RULE(BF_DEF_SIMPLIFY_N, "$X !< 1 ::= $X' = 0.")
	// Skip for qlt (omcat): BF-specific rewrite, not valid in DLO.
	if (arg2(ch).is(tau::bf_t)
		&& !is_omcat_type_family<node>(arg1_fm(ch).get_ba_type())
		&& !is_omcat_type_family<node>(arg2_fm(ch).get_ba_type())) {
		HOOK_LOGGING(applied("$X !< 1 ::= $X' = 0.");)
		return tau::get(tau::build_bf_eq_0(tau::build_bf_neg(arg1_fm(ch).get())), r);
	}
	//RULE(BF_DEF_SIMPLIFY_N, "0 !< $X ::= $X = 0.")
	// Skip for qlt (omcat): BF-specific rewrite, not valid in DLO.
	if (arg1(ch).is(tau::bf_f)
		&& !is_omcat_type_family<node>(arg1_fm(ch).get_ba_type())
		&& !is_omcat_type_family<node>(arg2_fm(ch).get_ba_type())) {
		HOOK_LOGGING(applied("0 !< $X ::= $X = 0.");)
		return tau::get(tau::build_bf_eq_0(arg2_fm(ch).get()), r);
	}
	//RULE(BF_NLESS_SIMPLIFY_0, "1 !< $X ::= T.") @CP
	if (arg1(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("1 !< $X ::= T.");)
		return _T(v, ch, len, r);
	}
	if (arg1_fm(ch) == arg2_fm(ch)) {
		HOOK_LOGGING(applied("$X !< $X ::= T.");)
		return _T(v, ch, len, r);
	}

	// The definition for the operator for bitvectors is different
	if (is_bv_type_family<node>(arg1_fm(ch).get_ba_type())) {
		if (tref bv_r = wff_bv_nlt<node>(ch, r)) {
			HOOK_LOGGING(applied("Using bitvector-specific definition for !<.");)
			return bv_r;
		}
		return tau::get_raw(v, ch, len, r);
	}
	// omcat types (e.g., qlt): unit-eliminate ground constant comparisons; pass through for cvc5
	{
		auto ba = arg1_fm(ch).get_ba_type();
		if (!is_omcat_type_family<node>(ba)) ba = arg2_fm(ch).get_ba_type();
		if (is_omcat_type_family<node>(ba)) {
			if (auto cmp = qlt_singleton_cmp<node>(arg1(ch), arg2(ch)); cmp)
				return (*cmp >= 0) ? _T(v, ch, len, r) : _F(v, ch, len, r); // !< means >=
			return tau::get_raw(v, ch, len, r);
		}
	}

	return tau::get(build_wff_or<node>(
		build_bf_neq_0<node>(build_bf_and<node>(arg1_fm(ch).get(),
				build_bf_neg<node>(arg2_fm(ch).get()))),
		build_bf_eq<node>(arg1_fm(ch).get(), arg2_fm(ch).get())), r);
}

template <NodeType node>
tref get_hook<node>::wff_lteq(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_lteq", v, ch, len, r);)

	// if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

	//RULE(BF_LESS_EQUAL_SIMPLIFY_2, "0 <= 1 ::= T.")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("0 <= 1 ::= T.");)
		return _T(v, ch, len, r);
	}
	//RULE(BF_LESS_EQUAL_SIMPLIFY_0, "0 <= 0 ::= T.")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("0 <= 0 ::= T.");)
		return _T(v, ch, len, r);
	}
	//RULE(BF_LESS_EQUAL_SIMPLIFY_1, "1 <= 0 ::= F.")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("1 <= 0 ::= F.");)
		return _F(v, ch, len, r);
	}
	//RULE(BF_DEF_SIMPLIFY_N, "1 <= 1 ::= T.")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("1 <= 1 ::= T.");)
		return _T(v, ch, len, r);
	}
	//RULE(BF_LESS_EQUAL_SIMPLIFY_0, "$X <= 1 ::= T.") @CP
	if (arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("$X <= 1 ::= T.");)
		return _T(v, ch, len, r);
	}
	//RULE(BF_LESS_EQUAL_SIMPLIFY_0, "1 <= $X ::= $X' = 0.") @CP
	// Skip for qlt (omcat): BF-specific rewrite, not valid in DLO.
	if (arg1(ch).is(tau::bf_t)
		&& !is_omcat_type_family<node>(arg1_fm(ch).get_ba_type())
		&& !is_omcat_type_family<node>(arg2_fm(ch).get_ba_type())) {
		HOOK_LOGGING(applied("1 <= $X ::= $X' = 0.");)
		return tau::get(tau::build_bf_eq_0(
			tau::build_bf_neg(arg2_fm(ch).get())), r);
	}
	//RULE(BF_LESS_EQUAL_SIMPLIFY_0, "$X <= 0 ::= X = 0.") @CP
	// Skip for qlt (omcat): BF-specific rewrite, not valid in DLO.
	if (arg2(ch).is(tau::bf_f)
		&& !is_omcat_type_family<node>(arg1_fm(ch).get_ba_type())
		&& !is_omcat_type_family<node>(arg2_fm(ch).get_ba_type())) {
		HOOK_LOGGING(applied("$X <= 0 ::= X = 0.");)
		return tau::get(tau::build_bf_eq_0(arg1_fm(ch).get()), r);
	}
	//RULE(BF_LESS_EQUAL_SIMPLIFY_0, "0 <= $X ::= T.") @CP
	if (arg1(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("0 <= $X ::= T.");)
		return _T(v, ch, len, r);
	}
	if (arg1_fm(ch) == arg2_fm(ch)) {
		HOOK_LOGGING(applied("$X <= $X ::= T.");)
		return _T(v, ch, len, r);
	}

	// The definition for the operator for bitvectors is different
	if (is_bv_type_family<node>(arg1_fm(ch).get_ba_type())) {
		if (tref bv_r = wff_bv_lteq<node>(ch, r)) {
			HOOK_LOGGING(applied("Using bitvector-specific definition for <=.");)
			return bv_r;
		}
		return tau::get_raw(v, ch, len, r);
	}
	// omcat types (e.g., qlt): unit-eliminate ground constant comparisons; pass through for cvc5
	{
		auto ba = arg1_fm(ch).get_ba_type();
		if (!is_omcat_type_family<node>(ba)) ba = arg2_fm(ch).get_ba_type();
		if (is_omcat_type_family<node>(ba)) {
			if (auto cmp = qlt_singleton_cmp<node>(arg1(ch), arg2(ch)); cmp)
				return (*cmp <= 0) ? _T(v, ch, len, r) : _F(v, ch, len, r); // <=
			return tau::get_raw(v, ch, len, r);
		}
	}

	return tau::get(build_bf_eq_0<node>(build_bf_and<node>(arg1_fm(ch).get(),
		build_bf_neg<node>(arg2_fm(ch).get()))), r);
}

template <NodeType node>
tref get_hook<node>::wff_nlteq(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_nlteq", v, ch, len, r);)

	// if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

	//RULE(BF_NLEQ_SIMPLIFY_2, "0 !<= 1 ::= F.")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("0 !<= 1 ::= F.");)
		return _F(v, ch, len, r);
	}
	//RULE(BF_NLEQ_SIMPLIFY_0, "0 !<= 0 ::= F.")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("0 !<= 0 ::= F.");)
		return _F(v, ch, len, r);
	}
	//RULE(BF_NLEQ_SIMPLIFY_1, "1 !<= 0 ::= T.")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("1 !<= 0 ::= T.");)
		return _T(v, ch, len, r);
	}
	//RULE(BF_NLEQ_SIMPLIFY_0, "1 !<= 1 ::= F.")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("1 !<= 1 ::= F.");)
		return _F(v, ch, len, r);
	}
	//RULE(BF_NLEQ_SIMPLIFY_0, "$X !<= 1 ::= F.") @CP
	if (arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("$X !<= 1 ::= F.");)
		return _F(v, ch, len, r);
	}
	//RULE(BF_NLEQ_SIMPLIFY_0, "0 !<= $X ::= F.") @CP
	if (arg1(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("0 !<= $X ::= F.");)
		return _F(v, ch, len, r);
	}
	if (arg1_fm(ch) == arg2_fm(ch)) {
		HOOK_LOGGING(applied("$X !<= $X ::= F.");)
		return _F(v, ch, len, r);
	}

	// The definition for the operator for bitvectors is different
	if (is_bv_type_family<node>(arg1_fm(ch).get_ba_type())) {
		if (tref bv_r = wff_bv_nlteq<node>(ch, r)) {
			HOOK_LOGGING(applied("Using bitvector-specific definition for !<=.");)
			return bv_r;
		}
		return tau::get_raw(v, ch, len, r);
	}
	// omcat types (e.g., qlt): unit-eliminate ground constant comparisons; pass through for cvc5
	{
		auto ba = arg1_fm(ch).get_ba_type();
		if (!is_omcat_type_family<node>(ba)) ba = arg2_fm(ch).get_ba_type();
		if (is_omcat_type_family<node>(ba)) {
			if (auto cmp = qlt_singleton_cmp<node>(arg1(ch), arg2(ch)); cmp)
				return (*cmp > 0) ? _T(v, ch, len, r) : _F(v, ch, len, r); // !<= means >
			return tau::get_raw(v, ch, len, r);
		}
	}

	return tau::get(build_bf_neq_0<node>(build_bf_and<node>(arg1_fm(ch).get(),
		build_bf_neg<node>(arg2_fm(ch).get()))), r);
}

template <NodeType node>
tref get_hook<node>::wff_gt(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_gt", v, ch, len, r);)

	// if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

	//RULE(BF_GREATER_SIMPLIFY_2, "1 > 0 ::= T.")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("1 > 0 ::= T.");)
		return _T(v, ch, len, r);
	}
	//RULE(BF_GREATER_SIMPLIFY_0, "0 > 0 ::= F.")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("0 > 0 ::= F.");)
		return _F(v, ch, len, r);
	}
	//RULE(BF_GREATER_SIMPLIFY_1, "0 > 1 ::= F.")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("0 > 1 ::= F.");)
		return _F(v, ch, len, r);
	}
	//RULE(BF_GREATER_SIMPLIFY_3, "1 > 1 ::= F.")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("1 > 1 ::= F.");)
		return _F(v, ch, len, r);
	}
	//RULE(BF_GREATER_SIMPLIFY_0, "$X > 1 ::= F.") @CP
	if (arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("$X > 1 ::= F.");)
		return _F(v, ch, len, r);
	}
	//RULE(BF_GREATER_SIMPLIFY_3, "0 > $X ::= F.") @CP
	if (arg1(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("0 > $X ::= F.");)
		return _F(v, ch, len, r);
	}
	if (arg1_fm(ch) == arg2_fm(ch)) {
		HOOK_LOGGING(applied("$X > $X ::= F.");)
		return _F(v, ch, len, r);
	}

	// The definition for the operator for bitvectors is different
	if (is_bv_type_family<node>(arg1_fm(ch).get_ba_type())) {
		if (tref bv_r = wff_bv_gt<node>(ch, r)) {
			HOOK_LOGGING(applied("Using bitvector-specific definition for >.");)
			return bv_r;
		}
		return tau::get_raw(v, ch, len, r);
	}
	// omcat types (e.g., qlt): unit-eliminate ground constant comparisons; pass through for cvc5
	{
		auto ba = arg1_fm(ch).get_ba_type();
		if (!is_omcat_type_family<node>(ba)) ba = arg2_fm(ch).get_ba_type();
		if (is_omcat_type_family<node>(ba)) {
			if (auto cmp = qlt_singleton_cmp<node>(arg1(ch), arg2(ch)); cmp)
				return (*cmp > 0) ? _T(v, ch, len, r) : _F(v, ch, len, r); // >
			return tau::get_raw(v, ch, len, r);
		}
	}

	return tau::get(build_wff_and<node>(
		build_bf_eq_0<node>(build_bf_and<node>(arg2_fm(ch).get(),
				build_bf_neg<node>(arg1_fm(ch).get()))),
		build_bf_neq<node>(arg2_fm(ch).get(), arg1_fm(ch).get())), r);
}

template <NodeType node>
tref get_hook<node>::wff_ngt(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_ngt", v, ch, len, r);)

	// if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

	//RULE(BF_NGREATER_SIMPLIFY_2, "1 !> 0 ::= F.")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("1 !> 0 ::= F.");)
		return _F(v, ch, len, r);
	}
	//RULE(BF_NGREATER_SIMPLIFY_1, "0 !> 0 ::= T.")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("0 !> 0 ::= T.");)
		return _T(v, ch, len, r);
	}
	//RULE(BF_NGREATER_SIMPLIFY_0, "0 !> 1 ::= T.")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("0 !> 1 ::= T.");)
		return _T(v, ch, len, r);
	}
	//RULE(BF_NGREATER_SIMPLIFY_3, "1 !> 1 ::= T.")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("1 !> 1 ::= T.");)
		return _T(v, ch, len, r);
	}
	//RULE(BF_NGREATER_SIMPLIFY_0, "$X !> 1 ::= T.") @CP
	if (arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("$X !> 1 ::= T.");)
		return _T(v, ch, len, r);
	}
	//RULE(BF_NGREATER_SIMPLIFY_3, "0 !> $X ::= T.") @CP
	if (arg1(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("0 !> $X ::= T.");)
		return _T(v, ch, len, r);
	}
	if (arg1_fm(ch) == arg2_fm(ch)) {
		HOOK_LOGGING(applied("$X !> $X ::= T.");)
		return _T(v, ch, len, r);
	}

	// The definition for the operator for bitvectors is different
	if (is_bv_type_family<node>(arg1_fm(ch).get_ba_type())) {
		if (tref bv_r = wff_bv_ngt<node>(ch, r)) {
			HOOK_LOGGING(applied("Using bitvector-specific definition for !>.");)
			return bv_r;
		}
		return tau::get_raw(v, ch, len, r);
	}
	// omcat types (e.g., qlt): unit-eliminate ground constant comparisons; pass through for cvc5
	{
		auto ba = arg1_fm(ch).get_ba_type();
		if (!is_omcat_type_family<node>(ba)) ba = arg2_fm(ch).get_ba_type();
		if (is_omcat_type_family<node>(ba)) {
			if (auto cmp = qlt_singleton_cmp<node>(arg1(ch), arg2(ch)); cmp)
				return (*cmp <= 0) ? _T(v, ch, len, r) : _F(v, ch, len, r); // !> means <=
			return tau::get_raw(v, ch, len, r);
		}
	}

	return tau::get(build_wff_or<node>(
		build_bf_neq_0<node>(build_bf_and<node>(arg2_fm(ch).get(),
				build_bf_neg<node>(arg1_fm(ch).get()))),
		build_bf_eq<node>(arg2_fm(ch).get(), arg1_fm(ch).get())), r);
}

template <NodeType node>
tref get_hook<node>::wff_gteq(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_gteq", v, ch, len, r);)

	// if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

	//RULE(BF_GREATER_EQUAL_SIMPLIFY_2, "1 >= 0 ::= T.")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("1 >= 0 ::= T.");)
		return _T(v, ch, len, r);
	}
	//RULE(BF_GREATER_EQUAL_SIMPLIFY_1, "0 >= 0 ::= T.")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("0 >= 0 ::= T.");)
		return _T(v, ch, len, r);
	}
	//RULE(BF_GREATER_EQUAL_SIMPLIFY_0, "0 >= 1 ::= F.")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("0 >= 1 ::= F.");)
		return _F(v, ch, len, r);
	}
	//RULE(BF_GREATER_EQUAL_SIMPLIFY_3, "1 >= 1 ::= T.")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("1 >= 1 ::= T.");)
		return _T(v, ch, len, r);
	}
	//RULE(BF_GREATER_EQUAL_SIMPLIFY_0, "$X >= 0 ::= T.") @CP
	if (arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("$X >= 0 ::= T.");)
		return _T(v, ch, len, r);
	}
	//RULE(BF_GREATER_EQUAL_SIMPLIFY_2, "1 >= X ::= T.")
	if (arg1(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("1 >= $X ::= T.");)
		return _T(v, ch, len, r);
	}
	if (arg1_fm(ch) == arg2_fm(ch)) {
		HOOK_LOGGING(applied("$X >= $X ::= T.");)
		return _T(v, ch, len, r);
	}

	// The definition for the operator for bitvectors is different
	if (is_bv_type_family<node>(arg1_fm(ch).get_ba_type())) {
		if (tref bv_r = wff_bv_gteq<node>(ch, r)) {
			HOOK_LOGGING(applied("Using bitvector-specific definition for >=.");)
			return bv_r;
		}
		return tau::get_raw(v, ch, len, r);
	}
	// omcat types (e.g., qlt): unit-eliminate ground constant comparisons; pass through for cvc5
	{
		auto ba = arg1_fm(ch).get_ba_type();
		if (!is_omcat_type_family<node>(ba)) ba = arg2_fm(ch).get_ba_type();
		if (is_omcat_type_family<node>(ba)) {
			if (auto cmp = qlt_singleton_cmp<node>(arg1(ch), arg2(ch)); cmp)
				return (*cmp >= 0) ? _T(v, ch, len, r) : _F(v, ch, len, r); // >=
			return tau::get_raw(v, ch, len, r);
		}
	}

	return tau::get(build_bf_eq_0<node>(build_bf_and<node>(arg2_fm(ch).get(),
				build_bf_neg<node>(arg1_fm(ch).get()))), r);
}

template <NodeType node>
tref get_hook<node>::wff_ngteq(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_ngteq", v, ch, len, r);)

	// if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

	//RULE(BF_NGEQ_SIMPLIFY_2, "1 !>= 0 ::= F.")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("1 !>= 0 ::= F.");)
		return _F(v, ch, len, r);
	}
	//RULE(BF_NGEQ_SIMPLIFY_1, "0 !>= 0 ::= F.")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("0 !>= 0 ::= F.");)
		return _F(v, ch, len, r);
	}
	//RULE(BF_NGEQ_SIMPLIFY_0, "0 !>= 1 ::= T.")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("0 !>= 1 ::= T.");)
		return _T(v, ch, len, r);
	}
	//RULE(BF_NGEQ_SIMPLIFY_3, "1 !>= 1 ::= F.")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("1 !>= 1 ::= F.");)
		return _F(v, ch, len, r);
	}
	//RULE(BF_NGEQ_SIMPLIFY_0, "$X !>= 0 ::= F.") @CP
	if (arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("$X !>= 0 ::= F.");)
		return _F(v, ch, len, r);
	}
	//RULE(BF_NGEQ_SIMPLIFY_1, "1 !>= $X ::= F.") @CP
	if (arg1(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("1 !>= $X ::= F.");)
		return _F(v, ch, len, r);
	}
	if (arg1_fm(ch) == arg2_fm(ch)) {
		HOOK_LOGGING(applied("$X !>= $X ::= F.");)
		return _F(v, ch, len, r);
	}

	// The definition for the operator for bitvectors is different
	if (is_bv_type_family<node>(arg1_fm(ch).get_ba_type())) {
		if (tref bv_r = wff_bv_ngteq<node>(ch, r)) {
			HOOK_LOGGING(applied("Using bitvector-specific definition for !>=.");)
			return bv_r;
		}
		return tau::get_raw(v, ch, len, r);
	}
	// omcat types (e.g., qlt): unit-eliminate ground constant comparisons; pass through for cvc5
	{
		auto ba = arg1_fm(ch).get_ba_type();
		if (!is_omcat_type_family<node>(ba)) ba = arg2_fm(ch).get_ba_type();
		if (is_omcat_type_family<node>(ba)) {
			if (auto cmp = qlt_singleton_cmp<node>(arg1(ch), arg2(ch)); cmp)
				return (*cmp < 0) ? _T(v, ch, len, r) : _F(v, ch, len, r); // !>= means <
			return tau::get_raw(v, ch, len, r);
		}
	}

	return tau::get(build_bf_neq_0<node>(build_bf_and<node>(arg2_fm(ch).get(),
				build_bf_neg<node>(arg1_fm(ch).get()))), r);
}

template <NodeType node>
tref get_hook<node>::wff_interval([[maybe_unused]] const node& v, const tref* ch,
	[[maybe_unused]] size_t len, [[maybe_unused]] tref r)
{
	HOOK_LOGGING(log("wff_interval", v, ch, len, r);)

	// The definition for the operator for bitvectors is different
	if (is_bv_type_family<node>(arg1_fm(ch).get_ba_type())) {
		return tau::build_wff_and(
			tau::build_bf_lteq(arg1_fm(ch).get(),
				arg2_fm(ch).get()),
			tau::build_bf_lteq(arg2_fm(ch).get(),
				arg3_fm(ch).get()));
	}
	return tau::get(tau::build_bf_interval(
		arg1_fm(ch).get(), arg2_fm(ch).get(), arg3_fm(ch).get()), r);
}

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
	else if (c0.is(tau::num)) left = (int_t)(c0.get_num());
	if (left < 0) {
		DBG(assert(c0.is(tau::variable) || c0.is(tau::capture));)
		return tau::get_raw(v, ch, len, r);
	}
	int_t right = -1;
	if (tau::get(ch[1]).is(tau::num))
		right = (int_t)(tau::get(ch[1]).get_num());
	DBG(assert(right >= 0);)
	if (left >= right) return tau::get(tau::get_integer(left - right), r);
	return nullptr; // Return error
}

} // namespace idni::tau_lang
