// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "hooks"

namespace idni::tau_lang {

template <NodeType node>
tref get_hook<node>::operator()(const node& v, const tref* ch, size_t len,
	tref r)
{
	HOOK_LOGGING(if (v.nt==tau::bf || v.nt==tau::wff || v.nt==tau::shift)
			log("- HOOK    -", v, ch, len, r, true);)
	tref ret = nullptr;
	if      (v.nt == tau::bf)    ret = term( v, ch, len, r);
	else if (v.nt == tau::wff)   ret = wff(  v, ch, len, r);
	else if (v.nt == tau::shift) ret = shift(v, ch, len, r);
	else return tau::get_raw(v, ch, len, r);

	HOOK_LOGGING(LOG_TRACE << "[- RESULT  -] " << LOG_FM_DUMP(ret);)
	DBG(typename node::type nt = tau::get(ret).get_type();)
	DBG(assert(nt == tau::bf || nt == tau::wff || nt == tau::shift);)
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
void applied(const std::string& rule) {
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
	return tau::get_raw(node(tau::bf), &x, 1, r);
}

template <NodeType node>
tref get_hook<node>::_1_typed(size_t ba_type, tref r) {
	HOOK_LOGGING(LOG_TRACE << "_1_typed " << LOG_BA_TYPE(ba_type);)
	tref x = tau::get_raw(node::ba_typed(tau::bf_t, ba_type), 0, 0);
	return tau::get_raw(node(tau::bf), &x, 1, r);
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
	case tau::bf_constant: return cte(v, ch, len, r);
	default: return tau::get_raw(v, ch, len, r);
	}
}

template <NodeType node>
tref get_hook<node>::term_or(const node& v, const tref* ch, size_t len, tref r){
	HOOK_LOGGING(log("term_or", v, ch, len, r);)
	DBG(assert(len == 1));

	if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

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
		return tau::get(tau::_1(), r);
	}
	//RULE(BF_SIMPLIFY_SELF_5, "$X' | $X := 1.")
	if (auto negated = arg1_fm(ch)() | tau::bf_neg | tau::bf;
			negated && negated.value_tree() == arg2_fm(ch))
	{
		HOOK_LOGGING(applied("$X' | $X := 1.");)
		return tau::get(tau::_1(), r);
	}
	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::term_and(const node& v, const tref* ch, size_t len, tref r)
{
	HOOK_LOGGING(log("term_and", v, ch, len, r);)
	DBG(assert(len == 1));

	if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

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
		return tau::get(tau::_0(), r);
	}
	//RULE(BF_SIMPLIFY_SELF_4, "$X' & $X := 0.")
	if (auto negated = arg1_fm(ch)() | tau::bf_neg | tau::bf;
			negated && negated.value_tree() == arg2_fm(ch))
	{
		HOOK_LOGGING(applied("$X' & $X := 0.");)
		return tau::get(tau::_0(), r);
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
		return type > 0 ? _0_typed(type, r)
			: tau::get(tau::_0(), r);
	}
	//RULE(BF_SIMPLIFY_ZERO_4, "0' := 1.")
	if (auto neg_zero = logic_operator(ch)() | tau::bf | tau::bf_f;
		neg_zero && logic_operator(ch).is(tau::bf_neg))
	{
		HOOK_LOGGING(applied("0' := 1.");)
		size_t type = neg_zero.value_tree().get_ba_type();
		return type > 0 ? _1_typed(type, r)
			: tau::get(tau::_1(), r);
	}
	//RULE(BF_ELIM_DOUBLE_NEGATION_0, "$X'' :=  $X.")
	if (auto double_neg = logic_operator(ch)() | tau::bf | tau::bf_neg;
		double_neg && logic_operator(ch).is(tau::bf_neg))
	{
		HOOK_LOGGING(applied("$X'' :=  $X.");)
		return tau::get(double_neg.value_tree().first(), r);
	}
	//RULE(BF_CALLBACK_NEG, "{ $X }' := bf_neg_cb $X.")
	if (arg1(ch).is_ba_constant() && arg1(ch).get_ba_type() > 0) {
		HOOK_LOGGING(applied("{ $X }' := bf_neg_cb $X.");)
		return cte_neg(v, ch, len, r);
	}
	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::term_xor(const node& v, const tref* ch, size_t len, tref r)
{
	HOOK_LOGGING(log("term_xor", v, ch, len, r);)
	DBG(assert(len == 1));

	if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

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
	return tau::get(tau::build_bf_xor(arg1_fm(ch).get(), arg2_fm(ch).get()), r);
}

// Simplify constants being syntactically true or false
template <NodeType node>
tref get_hook<node>::cte(const node& v, const tref* ch, size_t len, tref right){
	HOOK_LOGGING(log("cte", v, ch, len, right);)
	if (len == 1 && tau::get(ch[0]).is_ba_constant()) {
		const auto& l = tau::get(ch[0]);
		if (size_t typed = l.get_ba_type(); typed > 0) {
			HOOK_LOGGING(LOG_TRACE << "cte typed: " << LOG_BA_TYPE(typed);)
			if (is_syntactic_zero(l.get_ba_constant())) {
				HOOK_LOGGING(LOG_TRACE << LOG_FM_DUMP(l.get());)
				HOOK_LOGGING(applied("is_syntactic_zero");)
				return tau::get(typed
					? tau::get(tau::get(tau::bf, tau::get_raw(
						node::ba_typed(tau::bf_f, typed))), right)
					: tau::_0(), right);
			} else if (is_syntactic_one(l.get_ba_constant())) {
				HOOK_LOGGING(LOG_TRACE << LOG_FM_DUMP(l.get());)
				HOOK_LOGGING(applied("is_syntactic_one");)
				return tau::get(typed
					? tau::get(tau::get(tau::bf, tau::get_raw(
						node::ba_typed(tau::bf_t, typed))), right)
					: tau::_1(), right);
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
	//RULE(WFF_SIMPLIFY_ONE_2, "T && $X ::= $X.")
	if (arg1(ch).is(tau::wff_t)) {
		HOOK_LOGGING(applied("T && $X ::= $X.");)
		return tau::get(arg2_fm(ch).get(), r);
	}
	//RULE(WFF_SIMPLIFY_ONE_3, "$X && T ::= $X.")
	if (arg2(ch).is(tau::wff_t)) {
		HOOK_LOGGING(applied("$X && T ::= $X.");)
		return tau::get(arg1_fm(ch).get(), r);
	}
	//RULE(WFF_SIMPLIFY_ZERO_0, "F && $X ::= F.")
	if (arg1(ch).is(tau::wff_f)) {
		HOOK_LOGGING(applied("F && $X ::= F.");)
		return tau::get(arg1_fm(ch).get(), r);
	}
	//RULE(WFF_SIMPLIFY_ZERO_1, "$X && F ::= F.")
	if (arg2(ch).is(tau::wff_f)) {
		HOOK_LOGGING(applied("$X && F ::= F.");)
		return tau::get(arg2_fm(ch).get(), r);
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
		return tau::get(tau::_F(), r);
	}
	//RULE(WFF_SIMPLIFY_SELF_4, "!$X && $X ::= F.")
	if (auto negated = arg1_fm(ch)() | tau::wff_neg | tau::wff;
		negated && negated.value_tree() == arg2_fm(ch))
	{
		HOOK_LOGGING(applied("!$X && $X ::= F.");)
		return tau::get(tau::_F(), r);
	}
	//RULE(BF_EQ_AND_SIMPLIFY_0, "$X != 0 && $X = 0 ::= F.")
	//RULE(BF_EQ_AND_SIMPLIFY_1, "$X = 0 && $X != 0 ::= F.")
	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::wff_or(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_or", v, ch, len, r);)
	//RULE(WFF_SIMPLIFY_ONE_0, "T || $X ::= T.")
	if (arg1(ch).is(tau::wff_t)) {
		HOOK_LOGGING(applied("T || $X ::= T.");)
		return tau::get(arg1_fm(ch).get(), r);
	}
	//RULE(WFF_SIMPLIFY_ONE_1, "$X || T ::= T.")
	if (arg2(ch).is(tau::wff_t)) {
		HOOK_LOGGING(applied("$X || T ::= T.");)
		return tau::get(arg2_fm(ch).get(), r);
	}
	//RULE(WFF_SIMPLIFY_ZERO_2, "F || $X ::= $X.")
	if (arg1(ch).is(tau::wff_f)) {
		HOOK_LOGGING(applied("F || $X ::= $X.");)
		return tau::get(arg2_fm(ch).get(), r);
	}
	//RULE(WFF_SIMPLIFY_ZERO_3, "$X || F ::= $X.")
	if (arg2(ch).is(tau::wff_f)) {
		HOOK_LOGGING(applied("$X || F ::= $X.");)
		return tau::get(arg1_fm(ch).get(), r);
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
		return tau::get(tau::_T(), r);
	}
	//RULE(WFF_SIMPLIFY_SELF_5, "!$X || $X ::= T.")
	if (auto negated = arg1_fm(ch)() | tau::wff_neg | tau::wff;
			negated && negated.value_tree() == arg2_fm(ch))
	{
		HOOK_LOGGING(applied("!$X || $X ::= T.");)
		return tau::get(tau::_T(), r);
	}
	//RULE(BF_EQ_OR_SIMPLIFY_0, "$X != 0 || $X = 0 ::= T.")
	//RULE(BF_EQ_OR_SIMPLIFY_1, "$X = 0 || $X != 0 ::= T.")
	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::ctn_neg(tref n) {
	const auto& t = tau::get(n);
	auto num    = t.find_top(is<node, tau::num>);
	auto ctnvar = t.find_top(is<node, tau::ctnvar>);
	auto op = t[0].get_type();
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
	default: return n;
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
		return tau::get(tau::_F(), r);
	}
	//RULE(WFF_SIMPLIFY_ZERO_4, "! F ::= T.")
	if (arg1(ch).is(tau::wff_f)) {
		HOOK_LOGGING(applied("! F ::= T.");)
		return tau::get(tau::_T(), r);
	}
	//RULE(WFF_ELIM_DOUBLE_NEGATION_0, "! ! $X ::=  $X.")
	if (auto double_neg = arg1_fm(ch)() | tau::wff_neg | tau::wff;
		double_neg) {
		HOOK_LOGGING(applied("! ! $X ::=  $X.");)
		return tau::get(double_neg.value(), r);
	}
	if (arg1(ch).is(tau::constraint)) {
		HOOK_LOGGING(applied("! ! $X ::=  $X.");)
		return tau::get(ctn_neg(ch[0]), r);
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
		return tau::get(tau::_F(), r);
	}
	//RULE(BF_XOR_SIMPLIFY_3, "$X ^ ! $X ::= T.")
	if (auto negated = arg2_fm(ch)() | tau::wff_neg | tau::wff;
			negated && negated.value_tree() == arg1_fm(ch))
	{
		HOOK_LOGGING(applied("$X ^ ! $X ::= T.");)
		return tau::get(tau::_T(), r);
	}
	//RULE(BF_XOR_SIMPLIFY_4, "! $X ^ $X ::= T.")
	if (auto negated = arg1_fm(ch)() | tau::wff_neg | tau::wff;
			negated && negated.value_tree() == arg2_fm(ch))
	{
		HOOK_LOGGING(applied("! $X ^ $X ::= T.");)
		return tau::get(tau::_T(), r);
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

	if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

	//RULE(BF_EQ_SIMPLIFY_0, "1 = 0 ::=  F.")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("1 = 0 ::=  F.");)
		return tau::get(tau::_F(), r);
	}
	//RULE(BF_EQ_SIMPLIFY_1, "0 = 0 ::= T.")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("0 = 0 ::= T.");)
		return _T(v, ch, len, r);
	}
	//RULE(BF_DEF_SIMPLIFY_0, "0 = 1")
	if (arg1(ch).is(tau::bf_f) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("0 = 1 ::= F.");)
		return tau::get(tau::_F(), r);
	}
	//RULE(BF_DEF_SIMPLIFY_1, "1 = 1")
	if (arg1(ch).is(tau::bf_t) && arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("1 = 1 ::= T.");)
		return _T(v, ch, len, r);
	}
	//RULE(BF_DEF_SIMPLIFY_N, "{c} = 0 ::= ...."): this should never happen
	if (arg1(ch).is_ba_constant() && arg1(ch).get_ba_type() > 0
		&& arg2(ch).is(tau::bf_f))
	{
		HOOK_LOGGING(applied("{c} = 0 ::= ....");)
		return wff_eq_cte(v, ch, len, r);
	}
	//RULE(BF_DEF_EQ, "$X = $Y ::= $X & $Y' | $X' & $Y = 0.")
	if (!arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("$X = $Y ::= $X & $Y' | $X' & $Y = 0.");)
		return tau::get(tau::build_bf_eq(tau::build_bf_xor(
				arg1_fm(ch).get(), arg2_fm(ch).get())), r);
	}
	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::wff_eq_cte(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_eq_cte", v, ch, len, r);)
	auto l = tt(ch[0]) | tau::bf | tau::bf_constant;
	if (l && (l | tt::ba_constant) == false) return tau::get(tau::_T(), r);
	else if (l) return tau::get(tau::_F(), r);
	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::wff_neq(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_neq", v, ch, len, r);)

	if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

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
	//RULE(BF_DEF_SIMPLIFY_N, "{c} = 0 ::= ....")
	if (arg1(ch).is_ba_constant() && arg1(ch).get_ba_type() > 0
		&& arg2(ch).is(tau::bf_f))
	{
		HOOK_LOGGING(applied("{c} = 0 ::= ....");)
		return tau::get(wff_neq_cte(v, ch, len, r), r);
	}
	//RULE(BF_DEF_NEQ, "$X != $Y ::= $X & $Y' | $X' & $Y != 0.")
	if (!arg2(ch).is(tau::bf_f)) {
		HOOK_LOGGING(applied("$X != $Y ::= $X & $Y' | $X' & $Y != 0.");)
		return tau::get(tau::build_bf_neq(tau::build_bf_xor(
				arg1_fm(ch).get(), arg2_fm(ch).get())), r);
	}
	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::wff_neq_cte(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_neq_cte", v, ch, len, r);)
	auto l = tt(ch[0]) | tau::bf | tau::bf_constant;
	if (l.has_value() && (l | tt::ba_constant) == false)
		return tau::get(tau::_F(), r);
	else if (l.has_value()) return tau::get(tau::_T(), r);
	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::wff_sometimes(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_sometimes", v, ch, len, r);)
	//RULE(WFF_SIMPLIFY_ONE_6, " sometimes T ::= T.")
	if (arg1(ch).is(tau::wff_t)) {
		HOOK_LOGGING(applied("sometimes T ::= T.");)
		return tau::get(tau::_T(), r);
	}
	//RULE(WFF_SIMPLIFY_ZERO_6, "sometimes F ::= F.")
	if (arg1(ch).is(tau::wff_f)) {
		HOOK_LOGGING(applied("sometimes F ::= F.");)
		return tau::get(tau::_F(), r);
	}
	//RULE(WFF_SIMPLIFY_SOMETIMES_1,  "sometimes sometimes $X ::= sometimes $X.")
	if (auto double_quantifier = arg1_fm(ch)() | tau::wff_sometimes;
		double_quantifier) {
		HOOK_LOGGING(applied("sometimes sometimes $X ::= sometimes $X.");)
		return tau::get(arg1_fm(ch).get(), r);
	}
	//RULE(WFF_SIMPLIFY_SOMETIMES_2,  "sometimes always $X ::= always $X.")
	if (auto double_quantifier = arg1_fm(ch)() | tau::wff_always;
		double_quantifier) {
		HOOK_LOGGING(applied("sometimes always $X ::= always $X.");)
		return tau::get(arg1_fm(ch).get(), r);
	}
	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::wff_always(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_always", v, ch, len, r);)
	//RULE(WFF_SIMPLIFY_ONE_5, " always T ::= T.")
	if (arg1(ch).is(tau::wff_t)) {
		HOOK_LOGGING(applied("always T ::= T.");)
		return tau::get(tau::_T(), r);
	}
	//RULE(WFF_SIMPLIFY_ZERO_5, "always F ::= F.")
	if (arg1(ch).is(tau::wff_f)) {
		HOOK_LOGGING(applied("always F ::= F.");)
		return tau::get(tau::_F(), r);
	}
	//RULE(WFF_SIMPLIFY_ALWAYS_1,     "always always $X ::= always $X.")
	if (auto double_quantifier = arg1_fm(ch)() | tau::wff_always;
		double_quantifier && quantifier(ch).is(tau::wff_always))
	{
		HOOK_LOGGING(applied("always always $X ::= always $X.");)
		return tau::get(arg1_fm(ch).get(), r);
	}
	//RULE(WFF_SIMPLIFY_ALWAYS_2,     "always sometimes $X ::= sometimes $X.")
	if (auto double_quantifier = arg1_fm(ch)() | tau::wff_sometimes;
		double_quantifier && quantifier(ch).is(tau::wff_always))
	{
		HOOK_LOGGING(applied("always sometimes $X ::= sometimes $X.");)
		return tau::get(arg1_fm(ch).get(), r);
	}
	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::wff_conditional(const node& v, const tref* ch, size_t len, tref r) {
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
	return tau::get_raw(v, ch, len, r);
}

template <NodeType node>
tref get_hook<node>::wff_imply([[maybe_unused]] const node& v, const tref* ch,
	[[maybe_unused]] size_t len, [[maybe_unused]] tref r)
{
	HOOK_LOGGING(log("wff_imply", v, ch, len, r);)
	//RULE(WFF_IMPLY_SIMPLIFY_0, "F -> $X ::= T.")
	if (arg1(ch).is(tau::wff_f)) {
		HOOK_LOGGING(applied("F -> $X ::= T.");)
		return tau::get(tau::_T(), r);
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
		return tau::get(tau::_T(), r);
	}
	//RULE(WFF_IMPLY_SIMPLIFY_4, "$X -> $X ::= T.")
	if (arg1_fm(ch) == arg2_fm(ch)) {
		HOOK_LOGGING(applied("$X -> $X ::= T.");)
		return tau::get(tau::_T(), r);
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
		return tau::get(tau::_T(), r);
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
		return tau::get(tau::_T(), r);
	}
	//RULE(WFF_RIMPLY_SIMPLIFY_4, "$X <- $X ::= T.")
	if (arg1_fm(ch) == arg2_fm(ch)) {
		HOOK_LOGGING(applied("$X <- $X ::= T.");)
		return tau::get(tau::_T(), r);
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
		return tau::get(tau::_T(), r);
	}
	//RULE(WFF_EQUIV_SIMPLIFY_5, "$X <-> ! $X ::= F.")
	if (auto negated = tt(arg2_fm(ch)) | tau::wff_neg | tau::wff;
		negated && tau::get(negated.value()) == arg1_fm(ch)) {
		HOOK_LOGGING(applied("$X <-> ! $X ::= F.");)
		return tau::get(tau::_F(), r);
	}
	//RULE(WFF_EQUIV_SIMPLIFY_6, "! $X <-> $X ::= F.")
	if (auto negated = tt(arg1_fm(ch)) | tau::wff_neg | tau::wff;
		negated && tau::get(negated.value()) == arg2_fm(ch)) {
		HOOK_LOGGING(applied("! $X <-> $X ::= F.");)
		return tau::get(tau::_F(), r);
	}
	return tau::get(tau::build_wff_equiv(arg1_fm(ch).get(), arg2_fm(ch).get()), r);
}

template <NodeType node>
tref get_hook<node>::wff_lt(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_lt", v, ch, len, r);)

	if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

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
	if (arg2(ch).is(tau::bf_f)) return _F(v, ch, len, r);
	//RULE(BF_DEF_SIMPLIFY_N, "$X < 1 ::= $X' != 0.")
	if (arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("$X < 1 ::= $X' != 0.");)
		return tau::get(tau::build_bf_neq(
			tau::build_bf_neg(arg1_fm(ch).get())), r);
	}
	return tau::get(tau::build_bf_lt(arg1_fm(ch).get(),
					 arg2_fm(ch).get()), r);
}

template <NodeType node>
tref get_hook<node>::wff_nlt(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_nlt", v, ch, len, r);)

	if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

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
	if (arg2(ch).is(tau::bf_t)) {
		HOOK_LOGGING(applied("$X !< 1 ::= $X' = 0.");)
		return tau::get(tau::build_bf_eq(tau::build_bf_neg(arg1_fm(ch).get())), r);
	}
	return tau::get(tau::build_bf_nlt(arg1_fm(ch).get(), arg2_fm(ch).get()), r);
}

template <NodeType node>
tref get_hook<node>::wff_lteq(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_lteq", v, ch, len, r);)

	if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

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
	return tau::get(tau::build_bf_lteq(arg1_fm(ch).get(), arg2_fm(ch).get()), r);
}

template <NodeType node>
tref get_hook<node>::wff_nlteq(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_nlteq", v, ch, len, r);)

	if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

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
	return tau::get(tau::build_bf_nlteq(arg1_fm(ch).get(), arg2_fm(ch).get()), r);
}

template <NodeType node>
tref get_hook<node>::wff_gt(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_gt", v, ch, len, r);)

	if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

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
	return tau::get(tau::build_bf_gt(arg1_fm(ch).get(), arg2_fm(ch).get()), r);
}

template <NodeType node>
tref get_hook<node>::wff_ngt(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_ngt", v, ch, len, r);)

	if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

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
	return tau::get(tau::build_bf_ngt(arg1_fm(ch).get(), arg2_fm(ch).get()), r);
}

template <NodeType node>
tref get_hook<node>::wff_gteq(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_gteq", v, ch, len, r);)

	if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

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
	return tau::get(tau::build_bf_gteq(arg1_fm(ch).get(), arg2_fm(ch).get()), r);
}

template <NodeType node>
tref get_hook<node>::wff_ngteq(const node& v, const tref* ch, size_t len, tref r) {
	HOOK_LOGGING(log("wff_ngteq", v, ch, len, r);)

	if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

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
	return tau::get(tau::build_bf_ngteq(arg1_fm(ch).get(), arg2_fm(ch).get()), r);
}

template <NodeType node>
tref get_hook<node>::wff_interval([[maybe_unused]] const node& v, const tref* ch,
	[[maybe_unused]] size_t len, [[maybe_unused]] tref r)
{
	HOOK_LOGGING(log("wff_interval", v, ch, len, r);)

	if (check_type_mismatch(ch)) return tau::get_raw(v, ch, len, r);

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
