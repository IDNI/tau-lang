// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// hooks_bf.tmpl.h - BF hooks: helpers, primitives, term ops, CTE
// Split from hooks.tmpl.h for readability.

namespace idni::tau_lang {

// Return a bare wff(wff_t) / wff(wff_f) node with the given right
// sibling.  Unlike tau::_T()/tau::_F(), these have nonterminal wff(15)
// so the rewriter and hook assertions accept them.
template <NodeType node>
static tref bare_wff_T(tref r) {
	using tau = tree<node>;
	return tau::get(tau::get(tau::wff, tau::_T_trimmed()), r);
}
template <NodeType node>
static tref bare_wff_F(tref r) {
	using tau = tree<node>;
	return tau::get(tau::get(tau::wff, tau::_F_trimmed()), r);
}

template <NodeType node>
tref get_hook<node>::operator()(const node& v, const tref* ch, size_t len,
	tref r)
{
	HOOK_LOGGING(if (v.nt==tau::bf || v.nt==tau::wff || v.nt==tau::shift)
		log("- HOOK    -", v, ch, len, r, true);)
	tref ret = nullptr;
	if      (v.nt == tau::bf)          ret = term( v, ch, len, r);
	else if (v.nt == tau::wff)         ret = wff(  v, ch, len, r);
	else if (v.nt == tau::shift)       ret = shift(v, ch, len, r);
	else return tau::get_raw(v, ch, len, r);

	if (ret) {
		HOOK_LOGGING(LOG_TRACE << "[- RESULT  -] " << LOG_FM_DUMP(ret);)
		// Note: hash-wrapped T/F nodes (_T()/_F()) may have
		// non-standard nonterminals (e.g. 450 instead of wff=15).
		// These are semantically correct singleton constants.
	} else  { HOOK_LOGGING(LOG_TRACE << "[- RESULT  -] error";) }
	return ret;
}

#ifdef HOOK_LOGGING_ENABLED
template <NodeType node>
void get_hook<node>::log(const char* msg, const node& v, const tref* ch,
	size_t len, [[maybe_unused]] tref r, bool track_each_call)
{
	if (!track_each_call) return;
	std::stringstream ss;
	ss << "[" << msg << "] ";
	ss << LOG_NT(v.nt);
	if (len) {
		ss << " \t[";
		for (size_t i = 0; i < len; ++i) {
			DBG(assert(ch[i] != nullptr);)
			// ss << "<" << ch[i] << ">";
			ss <<(i ? ", " : "")<<LOG_NT(tau::get(ch[i]).get_type())
				<< "(" << LOG_FM(ch[i]) << ")";
		}
		ss << "]";
	}
	LOG_TRACE << ss.str();
}
inline void applied(const std::string& rule) {
	LOG_TRACE << "[- " << LOG_BRIGHT("APPLIED") << " -] "
				<< LOG_RULE_COLOR << rule << TC.CLEAR();
}
#endif // HOOK_LOGGING_ENABLED

template <NodeType node>
const tree<node>& get_hook<node>::arg1(const tref* ch) {
	return tau::get(ch[0])[0][0];
}

template <NodeType node>
const tree<node>& get_hook<node>::arg2(const tref* ch) {
	return tau::get(ch[0])[1][0];
}

template <NodeType node>
const tree<node>& get_hook<node>::arg3(const tref* ch) {
	return tau::get(ch[0])[2][0];
}

template <NodeType node>
const tree<node>& get_hook<node>::arg1_fm(const tref* ch) {
	return tau::get(ch[0])[0];
}

template <NodeType node>
const tree<node>& get_hook<node>::arg2_fm(const tref* ch) {
	return tau::get(ch[0])[1];
}

template <NodeType node>
const tree<node>& get_hook<node>::arg3_fm(const tref* ch) {
	return tau::get(ch[0])[2];
}

template <NodeType node>
const tree<node>& get_hook<node>::logic_operator(const tref* ch) {
	return tau::get(ch[0]);
}

template <NodeType node>
const tree<node>& get_hook<node>::quantifier(const tref* ch) {
	return tau::get(ch[0]);
}

template <NodeType node>
const tree<node>& get_hook<node>::quantified_formula(const tref* ch) {
	return tau::get(ch[0])[0];
}

template <NodeType node>
bool get_hook<node>::check_type_mismatch(const tref* ch) {
	std::set<size_t> types;
	for (tref c : tau::get(ch[0]).children()) {
		const auto& t = tau::get(c)[0];
		if (t.get_ba_type() > 0 || !(t.is(tau::bf_t) || t.is(tau::bf_f)))
			types.insert(t.get_ba_type());
		if (types.size() > 1) {
			HOOK_LOGGING(applied("type mismatch or unresolved yet. skipping hook");)
			return true;
		}
	}
	return false;
}

template <NodeType node>
tref get_hook<node>::_0_typed(size_t ba_type, tref r) {
	HOOK_LOGGING(LOG_TRACE << "_0_typed " << LOG_BA_TYPE(ba_type);)
	tref x = tau::get_raw(node::ba_typed(tau::bf_f, ba_type), 0, 0);
	return tau::get_raw(node::ba_typed(tau::bf, ba_type), &x, 1, r);
}

template <NodeType node>
tref get_hook<node>::_1_typed(size_t ba_type, tref r) {
	HOOK_LOGGING(LOG_TRACE << "_1_typed " << LOG_BA_TYPE(ba_type);)
	tref x = tau::get_raw(node::ba_typed(tau::bf_t, ba_type), 0, 0);
	return tau::get_raw(node::ba_typed(tau::bf, ba_type), &x, 1, r);
}

template <NodeType node>
tref get_hook<node>::_0(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("_0", v, ch, len, r);)
	size_t type_l = arg1(ch).get_ba_type(), type_r = arg2(ch).get_ba_type();
	// if (type_l == type_r && type_l >  0) return _0_typed(type_l, r);
	if (type_l == type_r)                return _0_typed(type_l, r);
	if (type_l >  0      && type_r == 0) return _0_typed(type_l, r);
	if (type_l == 0      && type_r >  0) return _0_typed(type_r, r);
	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::_1(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("_1", v, ch, len, r);)
	size_t type_l = arg1(ch).get_ba_type(), type_r = arg2(ch).get_ba_type();
	// if (type_l == type_r && type_l >  0) return _1_typed(type_l, r);
	if (type_l == type_r)                return _1_typed(type_l, r);
	if (type_l >  0      && type_r == 0) return _1_typed(type_l, r);
	if (type_l == 0      && type_r >  0) return _1_typed(type_r, r);
	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::_F(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("_F", v, ch, len, r);)
	auto type_l = arg1(ch).get_ba_type(), type_r = arg2(ch).get_ba_type();
	if ((type_l == type_r && type_l > 0)
		|| (type_l == type_r)
		|| (type_l >  0   && type_r == 0)
		|| (type_l == 0   && type_r > 0)) return tau::get(tau::_F(), r);
	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::_T(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("_T", v, ch, len, r);)
	auto type_l = arg1(ch).get_ba_type(), type_r = arg2(ch).get_ba_type();
	if ((type_l == type_r && type_l > 0)
		|| (type_l == type_r)
		|| (type_l >  0   && type_r == 0)
		|| (type_l == 0   && type_r > 0)) return tau::get(tau::_T(), r);
	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::term(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("term", v, ch, len, r);)
#ifdef DEBUG
	if (len != 1) {
		LOG_TRACE << "term: " << LOG_NT(v.nt) << " len: " << len;
		for (size_t i = 0; i < len; ++i) {
			LOG_TRACE << "ch[" << i << "]: " << LOG_FM(ch[i]);
		}
	}
#endif
	DBG(assert(len == 1);)
	switch (tau::get(ch[0]).get_type()) {
	case tau::bf_or:       return term_or(v, ch, len, r);
	case tau::bf_and:      return term_and(v, ch, len, r);
	case tau::bf_neg:      return term_neg(v, ch, len, r);
	case tau::bf_xor:      return term_xor(v, ch, len, r);
	case tau::ba_constant: return cte(v, ch, len, r);
	default: break;
	}
	// Term is currently untyped
	if (v.ba_type == 0) return tau::get_raw(v, ch, len, r);
	// Simplifications for bitvector symbols
	return node::ba::simplify_symbol(tau::get_raw(v, ch, len, r));
}

template <NodeType node>
tref get_hook<node>::term_or(const node& v, const tref* ch, size_t len, tref r){
	HOOK_LOGGING(log("term_or", v, ch, len, r);)
	DBG(assert(len == 1));

	// if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

	// RULE(UNBINDED, UNBINDED_SUBEXPRESSIONS, NODE)
	// if (unbound_subexpressions(ch)) return tau::get_raw(v, ch, len, r);
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("1 | 1 := 1.");)
		return _1(v, ch, len, r); }
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("0 | 0 := 0.");)
		return _0(v, ch, len, r);
	}
	//RULE(BF_SIMPLIFY_ONE_02, "1 | 0 := 1.")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("1 | 0 := 1.");)
		return _1(v, ch, len, r);
	}
	//RULE(BF_SIMPLIFY_ONE_03, "0 | 1 := 1.")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("0 | 1 := 1.");)
		return _1(v, ch, len, r);
	}
	//RULE(BF_SIMPLIFY_ONE_0, "1 | $X := 1.")
	if (arg1(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("1 | $X := 1.");)
		return tau::get(arg1_fm(ch).get(), r);
	}
	//RULE(BF_SIMPLIFY_ONE_1, "$X | 1 := 1.")
	if (arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("$X | 1 := 1.");)
		return tau::get(arg2_fm(ch).get(), r);
	}
	//RULE(BF_SIMPLIFY_ZERO_2, "0 | $X := $X.")
	if (arg1(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("0 | $X := $X.");)
		return tau::get(arg2_fm(ch).get(), r);
	}
	//RULE(BF_SIMPLIFY_ZERO_3, "$X | 0 := $X.")
	if (arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("$X | 0 := $X.");)
		return tau::get(arg1_fm(ch).get(), r);
	}
	//RULE(BF_SIMPLIFY_SELF_1, "$X | $X := $X.")
	if (arg1_fm(ch) == arg2_fm(ch)) {
		HOOK_LOGGING(applied("$X | $X := $X.");)
		return tau::get(arg1_fm(ch).get(), r);
	}
	//RULE(BF_CALLBACK_OR, "{ $X } | { $Y } := bf_or_cb $X $Y.")
	if (arg1(ch).is_ba_constant() && arg2(ch).is_ba_constant()
		&& arg1(ch).get_ba_type() > 0 && arg2(ch).get_ba_type() > 0)
	{
		HOOK_LOGGING(applied("{ $X } | { $Y } := bf_or_cb $X $Y.");)
		return cte_or(v, ch, len, r);
	}
	//RULE(BF_SIMPLIFY_SELF_3, "$X | $X' := 1.")
	if (auto negated = arg2_fm(ch)() | tau::bf_neg | tau::bf;
			negated && negated.value_tree() == arg1_fm(ch))
	{
		HOOK_LOGGING(applied("$X | $X' := 1.");)
		return _1_typed(negated.value_tree().get_ba_type(), r);
	}
	//RULE(BF_SIMPLIFY_SELF_5, "$X' | $X := 1.")
	if (auto negated = arg1_fm(ch)() | tau::bf_neg | tau::bf;
			negated && negated.value_tree() == arg2_fm(ch))
	{
		HOOK_LOGGING(applied("$X' | $X := 1.");)
		return _1_typed(negated.value_tree().get_ba_type(), r);
	}
	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::term_and(const node& v, const tref* ch, size_t len, tref r)
{
	HOOK_LOGGING(log("term_and", v, ch, len, r);)
	DBG(assert(len == 1));

	// if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

	// RULE(UNBINDED, UNBINDED_SUBEXPRESSIONS, NODE)
	// if (unbound_subexpressions(ch)) return tau::get_raw(v, ch, len, r);
	//RULE(BF_SIMPLIFY_ONE_00, "1 & 1 := 1.")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("1 & 1 := 1.");)
		return _1(v, ch, len, r);
	}
	//RULE(BF_SIMPLIFY_ONE_01, "0 & 0 := 0.")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("0 & 0 := 0.");)
		return _0(v, ch, len, r);
	}
	//RULE(BF_SIMPLIFY_ONE_02, "1 & 0 := 0.")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("1 & 0 := 0.");)
		return _0(v, ch, len, r);
	}
	//RULE(BF_SIMPLIFY_ONE_03, "0 & 1 := 0.")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("0 & 1 := 0.");)
		return _0(v, ch, len, r);
	}
	//RULE(BF_SIMPLIFY_ONE_2, "1 & $X := $X.")
	if (arg1(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("1 & $X := $X.");)
		return tau::get(arg2_fm(ch).get(), r);
	}
	//RULE(BF_SIMPLIFY_ONE_3, "$X & 1 := $X.")
	if (arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("$X & 1 := $X.");)
		return tau::get(arg1_fm(ch).get(), r);
	}
	//RULE(BF_SIMPLIFY_ZERO_0, "0 & $X := 0.")
	if (arg1(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("0 & $X := 0.");)
		return tau::get(arg1_fm(ch).get(), r);
	}
	//RULE(BF_SIMPLIFY_ZERO_1, "$X & 0 := 0.")
	if (arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("$X & 0 := 0.");)
		return tau::get(arg2_fm(ch).get(), r);
	}
	//RULE(BF_SIMPLIFY_SELF_0, "$X & $X := $X.")
	if (arg1_fm(ch) == arg2_fm(ch)) {
		HOOK_LOGGING(applied("$X & $X := $X.");)
		return tau::get(arg1_fm(ch).get(), r);
	}
	//RULE(BF_CALLBACK_AND, "{ $X } & { $Y } := bf_and_cb $X $Y.")
	if (arg1(ch).is_ba_constant() && arg2(ch).is_ba_constant()
		&& arg1(ch).get_ba_type() > 0 && arg2(ch).get_ba_type() > 0)
	{
		HOOK_LOGGING(applied("{ $X } & { $Y } := bf_and_cb $X $Y.");)
		return cte_and(v, ch, len, r);
	}
	//RULE(BF_SIMPLIFY_SELF_2, "$X & $X' := 0.")
	if (auto negated = arg2_fm(ch)() | tau::bf_neg | tau::bf;
			negated && negated.value_tree() == arg1_fm(ch))
	{
		HOOK_LOGGING(applied("$X & $X' := 0.");)
		return _0_typed(negated.value_tree().get_ba_type(), r);
	}
	//RULE(BF_SIMPLIFY_SELF_4, "$X' & $X := 0.")
	if (auto negated = arg1_fm(ch)() | tau::bf_neg | tau::bf;
			negated && negated.value_tree() == arg2_fm(ch))
	{
		HOOK_LOGGING(applied("$X' & $X := 0.");)
		return _0_typed(negated.value_tree().get_ba_type(), r);
	}
	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::term_neg(const node& v, const tref* ch, size_t len, tref r)
{
	HOOK_LOGGING(log("term_neg", v, ch, len, r);)
	DBG(assert(len == 1));
	//RULE(BF_SIMPLIFY_ONE_4, "1' := 0.")
	if (auto neg_one = logic_operator(ch)() | tau::bf | tau::bf_t;
		neg_one && logic_operator(ch).is(tau::bf_neg))
	{
		HOOK_LOGGING(applied("1' := 0.");)
		size_t type = neg_one.value_tree().get_ba_type();
		return _0_typed(type, r);
	}
	//RULE(BF_SIMPLIFY_ZERO_4, "0' := 1.")
	if (auto neg_zero = logic_operator(ch)() | tau::bf | tau::bf_f;
		neg_zero && logic_operator(ch).is(tau::bf_neg))
	{
		HOOK_LOGGING(applied("0' := 1.");)
		size_t type = neg_zero.value_tree().get_ba_type();
		return _1_typed(type, r);
	}
	//RULE(BF_ELIM_DOUBLE_NEGATION_0, "$X'' :=  $X.")
	if (auto double_neg = logic_operator(ch)() | tau::bf | tau::bf_neg;
		double_neg && logic_operator(ch).is(tau::bf_neg))
	{
		HOOK_LOGGING(applied("$X'' :=  $X.");)
		return tau::get(double_neg.value_tree().first(), r);
	}
	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::term_xor(const node& v, const tref* ch, size_t len, tref r)
{
	HOOK_LOGGING(log("term_xor", v, ch, len, r);)
	DBG(assert(len == 1));

	// if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

	// RULE(UNBINDED, UNBINDED_SUBEXPRESSIONS, NODE)
	// if (unbound_subexpressions(ch)) return tau::get_raw(v, ch, len, r);
	//RULE(BF_SIMPLIFY_ONE_00, "1 ^ 1 := 0.")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("1 ^ 1 := 0.");)
		return _0(v, ch, len, r);
	}
	//RULE(BF_SIMPLIFY_ONE_01, "0 ^ 0 := 0.")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("0 ^ 0 := 0.");)
		return _0(v, ch, len, r);
	}
	//RULE(BF_SIMPLIFY_ONE_02, "1 ^ 0 := 1.")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("1 ^ 0 := 1.");)
		return _1(v, ch, len, r);
	}
	//RULE(BF_SIMPLIFY_ONE_03, "0 ^ 1 := 1.")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("0 ^ 1 := 1.");)
		return _1(v, ch, len, r);
	}
	//RULE(BF_SIMPLIFY_ONE_N, "1 ^ $X := $X'.")
	if (arg1(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("1 ^ $X := $X'.");)
		return tau::get(tau::build_bf_neg(arg2_fm(ch).get()), r);
	}
	//RULE(BF_SIMPLIFY_ONE_N, "$X ^ 1 := $X'.")
	if (arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("$X ^ 1 := $X'.");)
		return tau::get(tau::build_bf_neg(arg1_fm(ch).get()), r);
	}
	//RULE(BF_SIMPLIFY_ZERO_N, "0 ^ $X := $X.")
	if (arg1(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("0 ^ $X := $X.");)
		return tau::get(arg2_fm(ch).get(), r);
	}
	//RULE(BF_SIMPLIFY_ZERO_N, "$X ^ 0 := $X.")
	if (arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("$X ^ 0 := $X.");)
		return tau::get(arg1_fm(ch).get(), r);
	}
	//RULE(BF_SIMPLIFY_SELF_N, "$X ^ $X := 0.")
	if (arg1_fm(ch) == arg2_fm(ch)) {
		HOOK_LOGGING(applied("$X ^ $X := 0.");)
		return _0(v, ch, len, r);
	}
	//RULE(BF_SIMPLIFY_SELF_2, "$X ^ $X' := 1.")
	if (auto negated = arg2_fm(ch)() | tau::bf_neg | tau::bf;
			negated && negated.value_tree() == arg1_fm(ch))
	{
		HOOK_LOGGING(applied("$X ^ $X' := 1.");)
		return _1(v, ch, len, r);
	}
	//RULE(BF_SIMPLIFY_SELF_2, "$X' ^ $X := 1.")
	if (auto negated = arg1_fm(ch)() | tau::bf_neg | tau::bf;
			negated && negated.value_tree() == arg2_fm(ch))
	{
		HOOK_LOGGING(applied("$X' ^ $X := 1.");)
		return _1(v, ch, len, r);
	}
	//RULE(BF_CALLBACK_XOR, "{ $X } ^ { $Y } := bf_xor_cb $X $Y.")
	if (arg1(ch).is_ba_constant() && arg2(ch).is_ba_constant()
		&& arg1(ch).get_ba_type() > 0 && arg2(ch).get_ba_type() > 0)
	{
		HOOK_LOGGING(applied("{ $X } ^ { $Y } := bf_xor_cb $X $Y.");)
		return cte_xor(v, ch, len, r);
	}
	return tau::get_raw(v, ch, len, r);
}

// Simplify constants being syntactically true or false
template <NodeType node>
tref get_hook<node>::cte(const node& v, const tref* ch, size_t len, tref right){
	HOOK_LOGGING(log("cte", v, ch, len, right);)
	if (len == 1 && tau::get(ch[0]).is_ba_constant()) {
		const auto& l = tau::get(ch[0]);
		if (size_t typed = l.get_ba_type(); (!l.has_child()) && typed > 0) {
			HOOK_LOGGING(LOG_TRACE << "cte typed: " << LOG_BA_TYPE(typed);)
			if (node::ba::is_syntactic_zero(l.get_ba_constant())) {
				HOOK_LOGGING(LOG_TRACE << LOG_FM_DUMP(l.get());)
				HOOK_LOGGING(applied("is_syntactic_zero");)
				return tau::get(tau::get(tau::bf, tau::get_raw(
						node::ba_typed(tau::bf_f, typed))), right);
			} else if (node::ba::is_syntactic_one(l.get_ba_constant())) {
				HOOK_LOGGING(LOG_TRACE << LOG_FM_DUMP(l.get());)
				HOOK_LOGGING(applied("is_syntactic_one");)
				return tau::get(tau::get(tau::bf, tau::get_raw(
						node::ba_typed(tau::bf_t, typed))), right);
			}
		}
	}
	return tau::get_raw(v, ch, len, right);
}

template <NodeType node>
tref get_hook<node>::cte_or([[maybe_unused]] const node& v, const tref* ch,
	[[maybe_unused]] size_t len, tref right)
{
	HOOK_LOGGING(log("cte_or", v, ch, len, right);)
	auto l = arg1(ch).get_ba_constant();
	auto r = arg2(ch).get_ba_constant();
	auto type_l = arg1(ch).get_ba_type();
	auto type_r = arg2(ch).get_ba_type();
	auto type = type_l ? type_l : type_r;
	return build_bf_ba_constant<node>(l | r, type, right);
}

template <NodeType node>
tref get_hook<node>::cte_and([[maybe_unused]] const node& v, const tref* ch,
	[[maybe_unused]] size_t len, tref right)
{
	HOOK_LOGGING(log("cte_and", v, ch, len, right);)
	auto l = arg1(ch).get_ba_constant();
	auto r = arg2(ch).get_ba_constant();
	auto type_l = arg1(ch).get_ba_type();
	auto type_r = arg2(ch).get_ba_type();
	auto type = type_l ? type_l : type_r;
	return build_bf_ba_constant<node>(l & r, type, right);
}

template <NodeType node>
tref get_hook<node>::cte_xor([[maybe_unused]] const node& v, const tref* ch,
	[[maybe_unused]] size_t len, tref right)
{
	HOOK_LOGGING(log("cte_xor", v, ch, len, right);)
	auto l = arg1(ch).get_ba_constant();
	auto r = arg2(ch).get_ba_constant();
	auto type_l = arg1(ch).get_ba_type();
	auto type_r = arg2(ch).get_ba_type();
	auto type = type_l ? type_l : type_r;
	return build_bf_ba_constant<node>(l ^ r, type, right);
}

template <NodeType node>
tref get_hook<node>::cte_neg([[maybe_unused]] const node& v, const tref* ch,
	[[maybe_unused]] size_t len, tref right)
{
	HOOK_LOGGING(log("cte_neg", v, ch, len, right);)
	auto l = arg1(ch).get_ba_constant();
	size_t type = arg1(ch).get_ba_type();
	return build_bf_ba_constant<node>(~l, type, right);
}

} // namespace idni::tau_lang
