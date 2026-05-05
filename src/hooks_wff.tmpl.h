// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// hooks_wff.tmpl.h - WFF hooks: logical ops, equality, containment
// Split from hooks.tmpl.h for readability.

namespace idni::tau_lang {


template <NodeType node>
tref get_hook<node>::wff(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff", v, ch, len, r);)
	switch (logic_operator(ch).get_type()) {
	case tau::wff_and:          return wff_and(v, ch, len, r);
	case tau::wff_or:           return wff_or(v, ch, len, r);
	case tau::wff_neg:          return wff_neg(v, ch, len, r);
	case tau::wff_xor:          return wff_xor(v, ch, len, r);
	case tau::constraint:       return wff_ctn(v, ch, len, r);
	case tau::bf_eq:            return wff_eq(v, ch, len, r);
	case tau::bf_neq:           return wff_neq(v, ch, len, r);
	case tau::wff_sometimes:    return wff_sometimes(v, ch, len, r);
	case tau::wff_always:       return wff_always(v, ch, len, r);
	case tau::wff_A:            return wff_A(v, ch, len, r);
	case tau::wff_E:            return wff_E(v, ch, len, r);
	case tau::wff_semantic_neg: return wff_semantic_neg(v, ch, len, r);
	case tau::wff_conditional:  return wff_conditional(v, ch, len, r);
	case tau::wff_imply:        return wff_imply(v, ch, len, r);
	case tau::wff_rimply:       return wff_rimply(v, ch, len, r);
	case tau::wff_equiv:        return wff_equiv(v, ch, len, r);
	case tau::bf_lt:            return wff_lt(v, ch, len, r);
	case tau::bf_nlt:           return wff_nlt(v, ch, len, r);
	case tau::bf_lteq:          return wff_lteq(v, ch, len, r);
	case tau::bf_nlteq:         return wff_nlteq(v, ch, len, r);
	case tau::bf_gt:            return wff_gt(v, ch, len, r);
	case tau::bf_ngt:           return wff_ngt(v, ch, len, r);
	case tau::bf_gteq:          return wff_gteq(v, ch, len, r);
	case tau::bf_ngteq:         return wff_ngteq(v, ch, len, r);
	case tau::bf_interval:      return wff_interval(v, ch, len, r);
	default: return tau::get_raw(v, ch, len, r);
	}
	return nullptr;
}

template <NodeType node>
tref get_hook<node>::wff_and(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_and", v, ch, len, r);)
	// Guard: arg1/arg2 navigate ch[0][0][0] which crashes on
	// hash-wrapped T/F nodes (childless). Detect via address
	// comparison OR structural child_is check. Return bare wff
	// nodes for T/F to satisfy the nt assertion.
	auto lhs = arg1_fm(ch).get();
	auto rhs = arg2_fm(ch).get();
	bool lhs_is_T = (lhs == tau::_T()) || arg1_fm(ch).equals_T();
	bool rhs_is_T = (rhs == tau::_T()) || arg2_fm(ch).equals_T();
	bool lhs_is_F = (lhs == tau::_F()) || arg1_fm(ch).equals_F();
	bool rhs_is_F = (rhs == tau::_F()) || arg2_fm(ch).equals_F();
	//RULE(WFF_SIMPLIFY_ONE_2, "T && $X ::= $X.")
	if (lhs_is_T) {
		HOOK_LOGGING(applied("T && $X ::= $X.");)
		if (rhs_is_T) return tau::get(tau::get(tau::wff, tau::_T_trimmed()), r);
		if (rhs_is_F) return tau::get(tau::get(tau::wff, tau::_F_trimmed()), r);
		return tau::get(rhs, r);
	}
	//RULE(WFF_SIMPLIFY_ONE_3, "$X && T ::= $X.")
	if (rhs_is_T) {
		HOOK_LOGGING(applied("$X && T ::= $X.");)
		if (lhs_is_F) return tau::get(tau::get(tau::wff, tau::_F_trimmed()), r);
		return tau::get(lhs, r);
	}
	//RULE(WFF_SIMPLIFY_ZERO_0, "F && $X ::= F.")
	if (lhs_is_F) {
		HOOK_LOGGING(applied("F && $X ::= F.");)
		return tau::get(tau::get(tau::wff, tau::_F_trimmed()), r);
	}
	//RULE(WFF_SIMPLIFY_ZERO_1, "$X && F ::= F.")
	if (rhs_is_F) {
		HOOK_LOGGING(applied("$X && F ::= F.");)
		return tau::get(tau::get(tau::wff, tau::_F_trimmed()), r);
	}
	//RULE(WFF_SIMPLIFY_SELF_0, "$X && $X ::= $X.")
	if (arg1_fm(ch) == arg2_fm(ch)) {
		HOOK_LOGGING(applied("$X && $X ::= $X.");)
		return tau::get(arg1_fm(ch).get(), r);
	}
	//RULE(WFF_SIMPLIFY_SELF_2, "$X && ! $X ::= F.")
	if (auto negated = arg2_fm(ch)() | tau::wff_neg | tau::wff;
		negated && negated.value_tree() == arg1_fm(ch))
	{
		HOOK_LOGGING(applied("$X && ! $X ::= F.");)
		return tau::get(tau::get(tau::wff, tau::_F_trimmed()), r);
	}
	//RULE(WFF_SIMPLIFY_SELF_4, "!$X && $X ::= F.")
	if (auto negated = arg1_fm(ch)() | tau::wff_neg | tau::wff;
		negated && negated.value_tree() == arg2_fm(ch))
	{
		HOOK_LOGGING(applied("!$X && $X ::= F.");)
		return tau::get(tau::get(tau::wff, tau::_F_trimmed()), r);
	}
	//RULE(BF_EQ_AND_SIMPLIFY_0, "$X != 0 && $X = 0 ::= F.")
	//RULE(BF_EQ_AND_SIMPLIFY_1, "$X = 0 && $X != 0 ::= F.")
	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::wff_or(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_or", v, ch, len, r);)
	// Guard: same as wff_and — detect hash-wrapped T/F via address.
	auto lhs = arg1_fm(ch).get();
	auto rhs = arg2_fm(ch).get();
	bool lhs_is_T = (lhs == tau::_T()) || arg1_fm(ch).equals_T();
	bool rhs_is_T = (rhs == tau::_T()) || arg2_fm(ch).equals_T();
	bool lhs_is_F = (lhs == tau::_F()) || arg1_fm(ch).equals_F();
	bool rhs_is_F = (rhs == tau::_F()) || arg2_fm(ch).equals_F();
	//RULE(WFF_SIMPLIFY_ONE_0, "T || $X ::= T.")
	if (lhs_is_T) {
		HOOK_LOGGING(applied("T || $X ::= T.");)
		return tau::get(tau::get(tau::wff, tau::_T_trimmed()), r);
	}
	//RULE(WFF_SIMPLIFY_ONE_1, "$X || T ::= T.")
	if (rhs_is_T) {
		HOOK_LOGGING(applied("$X || T ::= T.");)
		return tau::get(tau::get(tau::wff, tau::_T_trimmed()), r);
	}
	//RULE(WFF_SIMPLIFY_ZERO_2, "F || $X ::= $X.")
	if (lhs_is_F) {
		HOOK_LOGGING(applied("F || $X ::= $X.");)
		if (rhs_is_T) return tau::get(tau::get(tau::wff, tau::_T_trimmed()), r);
		if (rhs_is_F) return tau::get(tau::get(tau::wff, tau::_F_trimmed()), r);
		return tau::get(rhs, r);
	}
	//RULE(WFF_SIMPLIFY_ZERO_3, "$X || F ::= $X.")
	if (rhs_is_F) {
		HOOK_LOGGING(applied("$X || F ::= $X.");)
		if (lhs_is_T) return tau::get(tau::get(tau::wff, tau::_T_trimmed()), r);
		return tau::get(lhs, r);
	}
	//RULE(WFF_SIMPLIFY_SELF_1, "$X || $X ::= $X.")
	if (arg1_fm(ch) == arg2_fm(ch)) {
		HOOK_LOGGING(applied("$X || $X ::= $X.");)
		return tau::get(arg1_fm(ch).get(), r);
	}
	//RULE(WFF_SIMPLIFY_SELF_3, "$X || ! $X ::= T.")
	if (auto negated = arg2_fm(ch)() | tau::wff_neg | tau::wff;
			negated && negated.value_tree() == arg1_fm(ch))
	{
		HOOK_LOGGING(applied("$X || ! $X ::= T.");)
		return tau::get(tau::get(tau::wff, tau::_T_trimmed()), r);
	}
	//RULE(WFF_SIMPLIFY_SELF_5, "!$X || $X ::= T.")
	if (auto negated = arg1_fm(ch)() | tau::wff_neg | tau::wff;
			negated && negated.value_tree() == arg2_fm(ch))
	{
		HOOK_LOGGING(applied("!$X || $X ::= T.");)
		return tau::get(tau::get(tau::wff, tau::_T_trimmed()), r);
	}
	//RULE(BF_EQ_OR_SIMPLIFY_0, "$X != 0 || $X = 0 ::= T.")
	//RULE(BF_EQ_OR_SIMPLIFY_1, "$X = 0 || $X != 0 ::= T.")
	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::ctn_neg(const tree<node>& n) {
	auto num    = n.find_top(is<node, tau::num>);
	auto ctnvar = n.find_top(is<node, tau::ctnvar>);
	auto op = n[0][0].get_type();
	switch (op) {
		//RULE(BF_PUSH_NEGATION_INWARDS_2, "($X != $Y)' := $X = $Y.")
	case tau::ctn_neq:
		HOOK_LOGGING(applied("($X != $Y)' := $X = $Y.");)
		return tau::build_wff_ctn_eq(ctnvar, num);
		//RULE(BF_PUSH_NEGATION_INWARDS_3, "($X = $Y)' := $X != $Y.")
	case tau::ctn_eq:
		HOOK_LOGGING(applied("($X = $Y)' := $X != $Y.");)
		return tau::build_wff_ctn_neq(ctnvar, num);
		//RULE(BF_PUSH_NEGATION_INWARDS_4, "($X >= $Y)' := $X < $Y.")
	case tau::ctn_gteq:
		HOOK_LOGGING(applied("($X >= $Y)' := $X < $Y.");)
		return tau::build_wff_ctn_lt(ctnvar, num);
		//RULE(BF_PUSH_NEGATION_INWARDS_5, "($X > $Y)' := $X <= $Y.")
	case tau::ctn_gt:
		HOOK_LOGGING(applied("($X > $Y)' := $X <= $Y.");)
		return tau::build_wff_ctn_lteq(ctnvar, num);
		//RULE(BF_PUSH_NEGATION_INWARDS_6, "($X <= $Y)' := $X > $Y.")
	case tau::ctn_lteq:
		HOOK_LOGGING(applied("($X <= $Y)' := $X > $Y.");)
		return tau::build_wff_ctn_gt(ctnvar, num);
		//RULE(BF_PUSH_NEGATION_INWARDS_7, "($X < $Y)' := $X >= $Y.")
	case tau::ctn_lt:
		HOOK_LOGGING(applied("($X < $Y)' := $X >= $Y.");)
		return tau::build_wff_ctn_gteq(ctnvar, num);
	default: return build_wff_neg<node>(n.get());
	}
	return nullptr;
}

template <NodeType node>
tref get_hook<node>::wff_neg(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_neg", v, ch, len, r);)
	DBG(assert(len == 1));
	//RULE(WFF_SIMPLIFY_ONE_4, " ! T ::= F.")
	if (arg1(ch).is(tau::wff_t)) {
		HOOK_LOGGING(applied("! T ::= F.");)
		return bare_wff_F<node>(r);
	}
	//RULE(WFF_SIMPLIFY_ZERO_4, "! F ::= T.")
	if (arg1(ch).is(tau::wff_f)) {
		HOOK_LOGGING(applied("! F ::= T.");)
		return bare_wff_T<node>(r);
	}
	//RULE(WFF_ELIM_DOUBLE_NEGATION_0, "! ! $X ::=  $X.")
	if (auto double_neg = arg1_fm(ch)() | tau::wff_neg | tau::wff;
		double_neg) {
		HOOK_LOGGING(applied("! ! $X ::=  $X.");)
		return tau::get(double_neg.value(), r);
	}
	if (arg1(ch).is(tau::constraint)) {
		HOOK_LOGGING(applied("! [a op b] ::=  [a op' b].");)
		return tau::get(ctn_neg(arg1_fm(ch)), r);
	}
	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::wff_xor([[maybe_unused]] const node& v, const tref* ch,
	[[maybe_unused]] size_t len, [[maybe_unused]] tref r)
{
	HOOK_LOGGING(log("wff_xor", v, ch, len, r);)
	//RULE(BF_XOR_SIMPLIFY_0, "$X ^ F ::= $X.")
	if (arg2(ch).is(tau::wff_f)) {
		HOOK_LOGGING(applied("$X ^ F ::= $X.");)
		return tau::get(arg1_fm(ch).get(), r);
	}
	//RULE(BF_XOR_SIMPLIFY_1, "F ^ $X ::= $X.")
	if (arg1(ch).is(tau::wff_f)) {
		HOOK_LOGGING(applied("F ^ $X ::= $X.");)
		return tau::get(arg2_fm(ch).get(), r);
	}
	//RULE(BF_XOR_SIMPLIFY_2, "$X ^ $X ::= F.")
	if (arg1_fm(ch) == arg2_fm(ch)) {
		HOOK_LOGGING(applied("$X ^ $X ::= F.");)
		return bare_wff_F<node>(r);
	}
	//RULE(BF_XOR_SIMPLIFY_3, "$X ^ ! $X ::= T.")
	if (auto negated = arg2_fm(ch)() | tau::wff_neg | tau::wff;
			negated && negated.value_tree() == arg1_fm(ch))
	{
		HOOK_LOGGING(applied("$X ^ ! $X ::= T.");)
		return bare_wff_T<node>(r);
	}
	//RULE(BF_XOR_SIMPLIFY_4, "! $X ^ $X ::= T.")
	if (auto negated = arg1_fm(ch)() | tau::wff_neg | tau::wff;
			negated && negated.value_tree() == arg2_fm(ch))
	{
		HOOK_LOGGING(applied("! $X ^ $X ::= T.");)
		return bare_wff_T<node>(r);
	}
	//RULE(BF_XOR_SIMPLIFY_5, "$X ^ T ::= ! $X.")
	if (arg2(ch).is(tau::wff_t)) {
		HOOK_LOGGING(applied("$X ^ T ::= ! $X.");)
		return tau::get(tau::build_wff_neg(arg1_fm(ch).get()), r);
	}
	//RULE(BF_XOR_SIMPLIFY_6, "T ^ $X ::= ! $X.")
	if (arg1(ch).is(tau::wff_t)) {
		HOOK_LOGGING(applied("T ^ $X ::= ! $X.");)
		return tau::get(tau::build_wff_neg(arg2_fm(ch).get()), r);
	}
	return tau::get(tau::build_wff_xor(arg1_fm(ch).get(), arg2_fm(ch).get()), r);
}

template <NodeType node>
tref get_hook<node>::wff_ctn(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_ctn", v, ch, len, r);)
	tref n = tau::get_raw(v, ch, len, r);
	const auto& t = tau::get(n);
	tref num    = t.find_top(is<node, tau::num>);
	tref ctnvar = t.find_top(is<node, tau::ctnvar>);
	size_t op = t[0][0].get_type();
	switch (op) {
	case tau::ctn_eq: return tau::get(tau::build_wff_and(
				tau::build_wff_ctn_lteq(ctnvar, num),
				tau::build_wff_ctn_gteq(ctnvar, num)), r);
	case tau::ctn_neq: return tau::get(tau::build_wff_or(
				tau::build_wff_ctn_lt(ctnvar, num),
				tau::build_wff_ctn_gt(ctnvar, num)), r);
	}
	return n;
}

template <NodeType node>
tref get_hook<node>::wff_eq(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_eq", v, ch, len, r);)

	// if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

	//RULE(BF_EQ_SIMPLIFY_0, "1 = 0 ::=  F.")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("1 = 0 ::=  F.");)
		return bare_wff_F<node>(r);
	}
	//RULE(BF_EQ_SIMPLIFY_1, "0 = 0 ::= T.")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("0 = 0 ::= T.");)
		return _T(v, ch, len, r);
	}
	//RULE(BF_DEF_SIMPLIFY_0, "0 = 1")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("0 = 1 ::= F.");)
		return bare_wff_F<node>(r);
	}
	//RULE(BF_DEF_SIMPLIFY_1, "1 = 1")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("1 = 1 ::= T.");)
		return _T(v, ch, len, r);
	}
	//RULE(BF_DEF_SIMPLIFY_N, "{c} = 0 ::= T or F"): this should never happen
	if (arg1(ch).is_ba_constant() && arg1(ch).get_ba_type() > 0
		&& arg2(ch).is(tau::bf_f))
	{
		HOOK_LOGGING(applied("{c} = 0 ::= T or F");)
		return wff_eq_cte(v, ch, len, r);
	}
	//RULE(BF_DEF_SIMPLIFY_N, "{c} = 1 ::= T or F")
	if (arg1(ch).is_ba_constant() && arg1(ch).get_ba_type() > 0
		&& arg2(ch).is(tau::bf_t))
	{
		HOOK_LOGGING(applied("{c} = 1 ::= T or F");)
		return tau::build_bf_eq_0(tau::build_bf_neg(
			arg1_fm(ch).get()));
	}

	// Rule {c} = {d} ::= {c} + {d} = 0
	if (arg1(ch).is_ba_constant() && arg2(ch).is_ba_constant())
		if (size_t t = arg1(ch).get_ba_type();
			t > 0 && t == arg2(ch).get_ba_type()) {
			HOOK_LOGGING(applied("{c} = {d} ::= {c} + {d} = 0");)
			return tau::build_bf_eq_0(
				build_bf_xor<node>(
					arg1_fm(ch).get(), arg2_fm(ch).get()));
			}

	// Rule X = X ::= T
	if (arg1_fm(ch) == arg2_fm(ch)) {
		HOOK_LOGGING(applied("X = X ::= T");)
		return bare_wff_T<node>(r);
	}

	//Rule X = X' ::= F
	if (arg1(ch).is(tau::bf_neg)) {
		if (arg1(ch)[0] == arg2_fm(ch)) {
			HOOK_LOGGING(applied("X' = X ::= F");)
			return bare_wff_F<node>(r);
		}
	} else if (arg2(ch).is(tau::bf_neg)) {
		if (arg2(ch)[0] == arg1_fm(ch)) {
			HOOK_LOGGING(applied("X = X' ::= F");)
			return bare_wff_F<node>(r);
		}
	}

	//1/0 = X ::= X = 1/0
	if (arg1(ch).is(tau::bf_t) || arg1(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("1/0 = X ::= X = 1/0");)
		return tau::build_bf_eq(arg2_fm(ch).get(),
			arg1_fm(ch).get());
	}

	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::wff_eq_cte(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_eq_cte", v, ch, len, r);)
	auto l = tt(ch[0]) | tau::bf | tau::ba_constant;
	if (l && (l | tt::ba_constant) == false) return bare_wff_T<node>(r);
	else if (l) return bare_wff_F<node>(r);
	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::wff_neq(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_neq", v, ch, len, r);)

	// if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

	//RULE(BF_NEQ_SIMPLIFY_0, "0 != 0 ::= F.")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("0 != 0 ::= F.");)
		return _F(v, ch, len, r);
	}
	//RULE(BF_NEQ_SIMPLIFY_1, "1 != 0 ::= T.")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("1 != 0 ::= T.");)
		return _T(v, ch, len, r);
	}
	//RULE(BF_NEQ_SIMPLIFY_2, "0 != 1 ::= T.")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("0 != 1 ::= T.");)
		return _T(v, ch, len, r);
	}
	//RULE(BF_NEQ_SIMPLIFY_3, "1 != 1 ::= F.")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("1 != 1 ::= F.");)
		return _F(v, ch, len, r);
	}
	//RULE(BF_DEF_SIMPLIFY_N, "{c} != 0 ::= T or F")
	if (arg1(ch).is_ba_constant() && arg1(ch).get_ba_type() > 0
		&& arg2(ch).is(tau::bf_f))
	{
		HOOK_LOGGING(applied("{c} != 0 ::= T or F");)
		return tau::get(wff_neq_cte(v, ch, len, r), r);
	}
	//RULE(BF_DEF_SIMPLIFY_N, "{c} != 1 ::= T or F")
	if (arg1(ch).is_ba_constant() && arg1(ch).get_ba_type() > 0
		&& arg2(ch).is(tau::bf_t))
	{
		HOOK_LOGGING(applied("{c} != 1 ::= T or F");)
		return tau::build_bf_neq_0(tau::build_bf_neg(
			arg1_fm(ch).get()));
	}

	// Rule {c} != {d} ::= {c} + {d} != 0
	if (arg1(ch).is_ba_constant() && arg2(ch).is_ba_constant())
		if (size_t t = arg1(ch).get_ba_type();
			t > 0 && t == arg2(ch).get_ba_type()) {
			HOOK_LOGGING(applied("{c} != {d} ::= {c} + {d} != 0");)
			return tau::build_bf_neq_0(
				build_bf_xor<node>(
					arg1_fm(ch).get(), arg2_fm(ch).get()));
		}

	//Rule X != X ::= F
	if (arg1_fm(ch) == arg2_fm(ch)) {
		HOOK_LOGGING(applied("X != X ::= F");)
		return bare_wff_F<node>(r);
	}

	//Rule X != X' ::= T
	if (arg1(ch).is(tau::bf_neg)) {
		if (arg1(ch)[0] == arg2_fm(ch)) {
			HOOK_LOGGING(applied("X' != X ::= T");)
			return bare_wff_T<node>(r);
		}
	} else if (arg2(ch).is(tau::bf_neg)) {
		if (arg2(ch)[0] == arg1_fm(ch)) {
			HOOK_LOGGING(applied("X != X' ::= T");)
			return bare_wff_T<node>(r);
		}
	}

	//1/0 != X ::= X != 1/0
	if (arg1(ch).is(tau::bf_t) || arg1(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("1/0 != X ::= X != 1/0");)
		return tau::build_bf_neq(arg2_fm(ch).get(),
			arg1_fm(ch).get());
	}

	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::wff_neq_cte(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_neq_cte", v, ch, len, r);)
	auto l = tt(ch[0]) | tau::bf | tau::ba_constant;
	if (l.has_value() && (l | tt::ba_constant) == false)
		return bare_wff_F<node>(r);
	else if (l.has_value()) return bare_wff_T<node>(r);
	return tau::get_raw(v, ch, len, r);
}

} // namespace idni::tau_lang
