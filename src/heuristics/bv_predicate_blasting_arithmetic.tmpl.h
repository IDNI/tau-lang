// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <bit>

namespace idni::tau_lang {

//
//
// _bvadd is a predicate recurrence relation that computes the addition of two
// bitvectors. The idea is to define the addition ugin:
//
// 	addition(x, y) = addition(x ^ y, (x & y) << 1, m) mod m;
//
// The above translates to the following recurrence rules:
// - addition[0](x, y, z) = (z = x); (base case: adding zero bits means the result is just x)
// - addition[n](x, y, z) = ex w addition[n-1](x ^ y, w, z)
//		&& bv_shl_by_one(x & y, w); (general case: the result of adding n bits is the result of adding n-1 bits of the xor of x and y, and the carry (x & y) shifted left by one)
//
//

/**
 * @brief Creates a call to the offset bitvector addition recurrence.
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand (constant)
 * @param result Result variable
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvadd_call_from_offset(tref left, tref right, tref result, tref offset) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_bvadd", { offset }, { left, right, result })));
}

/**
 * @brief Creates a call to the indexed bitvector subtraction recurrence.
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @param result Result variable
 * @param index Index for the recurrence
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvadd_call_from_index(tref left, tref right, tref result, size_t index) {
	using tau = tree<node>;

	auto offset = tau::get_integer(index);
	return make_bvadd_call_from_offset<node>(left, right, result, offset);
}

/**
 * @brief Returns the rules for bitvector addition recurrence.
 *
 * Generates the base and general case rules for bitvector addition.
 *
 * @tparam node Node type
 * @param bitwidth Bitwidth of the operands
 * @return The set of rules for addition
 */
template<NodeType node>
static rewriter::rules bvadd_rules(size_t bitwidth) {
	using tau = tree<node>;

	static std::map<size_t, rewriter::rules> cache;
	if (auto it = cache.find(bitwidth); it != cache.end()) {
		return it->second;
	}

	auto left = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto right = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto result = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));

	rewriter::rules rules;
	// base case: addition[0](x, y, z) = (z = x);
	auto base_header = make_bvadd_call_from_index<node>(left, right, result, 0);
	auto base_body = tau::build_bf_eq(left, result);
	rules.push_back(make_rule<node>(base_header, base_body));
	// general case: addition[n](x, y, z) = ex w addition[n-1](x ^ y, w, z) && bv_shr_by_one(x & y, w);
	auto n = tau::build_variable(untyped_type_id<node>());
	auto n_minus_1 = tau::build_ref_shift_offset(n, 1);
	auto general_header = make_bvadd_call_from_offset<node>(left, right, result, n);
	auto w = tau::build_variable(bv_type_id<node>(bitwidth));
	auto bf_w = tau::get(tau::bf, w);
	auto general_body = tau::build_wff_ex(w,
		tau::build_wff_and(
			make_bvadd_call_from_offset<node>(tau::build_bf_xor(left, right), bf_w, result, n_minus_1),
			bvshl_by_one<node>(tau::build_bf_and(left, right), bf_w)
	));
	rules.push_back(make_rule<node>(general_header, general_body));

#ifdef DEBUG
	for (const auto& rule : rules) {
		LOG_TRACE << "bvadd_rules/rule: " << LOG_RULE(rule) << "\n";
		LOG_TRACE << "bvadd_rules/head: " << LOG_FM(rule.first->get()) << "\n";
		LOG_TRACE << "bvadd_rules/body: " << LOG_FM(rule.second->get()) << "\n";
	}
#endif // DEBUG

	cache[bitwidth] = rules;
	return rules;
}

/**
 * @brief Returns the rule for bitvector addition (caching by bitwidth).
 *
 * @tparam node Node type
 * @param bitwidth Bitwidth of the operands
 * @return The constructed rule
 */
template<NodeType node>
static rewriter::rule bvadd_rule(size_t bitwidth) {
	using tau = tree<node>;

	static std::map<size_t, rewriter::rule> cache;
	if (auto it = cache.find(bitwidth); it != cache.end()) {
		return it->second;
	}

	rewriter::rules rs;
	auto additions = bvadd_rules<node>(bitwidth);
	rs.insert(rs.end(), additions.begin(), additions.end());

	auto left = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto right = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto result = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));

	auto head = make_bvadd_call_from_index<node>(left, right, result, bitwidth);
	auto rr = make_rr<node>(rs, head);
	auto body = apply_rr_to_formula(rr);
	auto rule = make_rule<node>(head, body);

#ifdef DEBUG
	LOG_TRACE << "bvadd_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvadd_rule/head: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvadd_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG

	cache[bitwidth] = rule;
	return cache[bitwidth];
}

template<NodeType node>
tref bvadd(tref left, tref right, tref result) {
	auto bitwidth = get_bv_type_bitwidth<node>(left);
	auto rule = bvadd_rule<node>(bitwidth);
	auto call = make_bvadd_call_from_index<node>(left, right, result, bitwidth);
	auto rr = make_rr<node>({ rule } , call);
	return apply_rr_to_formula(rr);
}

//
//
// _bvsub is a predicate recurrence relation that computes the subtraction of two
// bitvectors. The idea is to define the subtraction using:
//
// subtraction(x, y) = subtraction(x ^ y, (~x & y) << 1, m) mod m;
//
// The above translates to the following recurrence rules:
// - subtraction[0](x, y, z) = (z = x); (base case: subtracting zero bits means the result is just x)
// - subtraction[n](x, y, z) = ex w subtraction[n-1](x ^ y, w, z)
// 		&& bv_shr_by_one((~x & y), w); (general case: the result of subtracting n bits is the result of subtracting n-
//
//

/**
 * @brief Creates a call to the offset bitvector subtraction recurrence.
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @param result Result variable
 * @param offset Offset for the recurrence
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvsub_call_from_offset(tref left, tref right, tref result, tref offset) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_bvsub", { offset }, { left, right, result })));
}

/**
 * @brief Creates a call to the indexed bitvector subtraction recurrence.
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @param result Result variable
 * @param index Index for the recurrence
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvsub_call_from_index(tref left, tref right, tref result, size_t index) {
	using tau = tree<node>;

	auto offset = tau::get_integer(index);
	return make_bvsub_call_from_offset<node>(left, right, result, offset);
}

/**
 * @brief Returns the rules for bitvector subtraction recurrence.
 *
 * Generates the base and general case rules for bitvector subtraction.
 *
 * @tparam node Node type
 * @param bitwidth Bitwidth of the operands
 * @return The set of rules for subtraction
 */
template<NodeType node>
static rewriter::rules bvsub_rules(size_t bitwidth) {
	using tau = tree<node>;

	static std::map<size_t, rewriter::rules> cache;
	if (auto it = cache.find(bitwidth); it != cache.end()) {
		return it->second;
	}

	auto left = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto right = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto result = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));

	rewriter::rules rules;
	// base case: subtraction[0](x, y, z) = (z = x);
	auto base_header = make_bvsub_call_from_index<node>(left, right, result, 0);
	auto base_body = tau::build_bf_eq(left, result);
	rules.push_back(make_rule<node>(base_header, base_body));
	// general case: subtraction[n](x, y, z) = ex w subtraction[n-1](x ^ y, w, z) && bv_shr_by_one((~x & y), w);
	auto n = tau::build_variable(untyped_type_id<node>());
	auto n_minus_1 = tau::build_ref_shift_offset(n, 1);
	auto general_header = make_bvsub_call_from_offset<node>(left, right, result, n);
	auto w = tau::build_variable(bv_type_id<node>(bitwidth));
	auto bf_w = tau::get(tau::bf, w);
	auto general_body = tau::build_wff_ex(w,
		tau::build_wff_and(
			make_bvsub_call_from_offset<node>(tau::build_bf_xor(left, right), bf_w, result, n_minus_1),
			bvshl_by_one<node>(tau::build_bf_and(tau::build_bf_neg(left), right), bf_w)
		));
	rules.push_back(make_rule<node>(general_header, general_body));

#ifdef DEBUG
	for (const auto& rule : rules) {
	LOG_TRACE << "bvsub_rules: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvsub_rules/head: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvsub_rules/body: " << LOG_FM(rule.second->get()) << "\n";
	}
#endif // DEBUG

	cache[bitwidth] = rules;
	return rules;
}

/**
 * @brief Returns the rule for bitvector subtraction (caching by bitwidth).
 *
 * @tparam node Node type
 * @param bitwidth Bitwidth of the operands
 * @return The constructed rule
 */
template<NodeType node>
static rewriter::rule bvsub_rule(size_t bitwidth) {
	using tau = tree<node>;

	static std::map<size_t, rewriter::rule> cache;
	if (auto it = cache.find(bitwidth); it != cache.end()) {
		return it->second;
	}

	auto left = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto right = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto result = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));

	rewriter::rules rs;
	auto substractions = bvsub_rules<node>(bitwidth);
	rs.insert(rs.end(), substractions.begin(), substractions.end());
	auto head = make_bvsub_call_from_index<node>(left, right, result, bitwidth);
	auto rr = make_rr<node>(rs, head);
	auto body = apply_rr_to_formula(rr);
	auto rule = make_rule<node>(head, body);

#ifdef DEBUG
	LOG_TRACE << "bvsub_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvsub_rule/head: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvsub_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG

	cache[bitwidth] = rule;
	return cache[bitwidth];
}

template<NodeType node>
tref bvsub(tref left, tref right, tref result) {
	auto bitwidth = get_bv_type_bitwidth<node>(left);
	auto rule = bvsub_rule<node>(bitwidth);
	auto call = make_bvsub_call_from_index<node>(left, right, result, bitwidth);
	auto rr = make_rr<node>({ rule }, call);
	return apply_rr_to_formula(rr);
}

//
//
// _bvmul is a predicate recurrence relation that computes the multiplication of two
// bitvectors. The idea is to define the multiplication using:
//
// multiplication(x, y) = if (y & 1) then (x + multiplication(x << 1, y >> 1)) else multiplication(x << 1, y >> 1);
//
// The above translates to the following recurrence rules:
// - multiplication[0](x, y, z) = (z = 0); (base case: multiplying zero bits means the result is zero)
// - multiplication[n](x, y, z) = if (y & 1) then (ex v multiplication[n-1](x << 1, y >> 1, v) && (z = x + v)) else (ex v multiplication[n-1](x << 1, y >> 1, v) && (z = v)); (general case: the result of multiplying n bits is the result of multiplying n-1 bits of x shifted left by one and
//
//

/**
 * @brief Creates a call to the bitvector multiplication recurrence (constant right operand).
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand (constant)
 * @param result Result variable
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvmul_call_from_offset(tref left, tref result, tref offset) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_bvmul", { offset }, { left, result })));
}

/**
 * @brief Creates a call to the bitvector multiplication recurrence (constant right operand).
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand (constant)
 * @param result Result variable
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvmul_call_from_index(tref left, tref result, int_t index) {
	using tau = tree<node>;

	auto offset = tau::get_integer(index);
	return make_bvmul_call_from_offset<node>(left, result, offset);
}

/**
 * @brief Returns the rule for bitvector multiplication recurrence.
 *
 * Handles multiplication by constant right operand.
 *
 * @tparam node Node type
 * @param y Right operand (constant)
 * @param bitwidth Bitwidth of the operands
 * @return The constructed rule
 */
template<NodeType node>
static rewriter::rule bvmul_rec_rule(tref right /* cvc5 constant */, size_t bitwidth) {
	using tau = tree<node>;

	DBG( assert(is_bv_constant<node>(right)); )

	static std::map<size_t, std::map<tref, rewriter::rule>> cache;
	if (cache.find(bitwidth) != cache.end() && cache[bitwidth].find(right) != cache[bitwidth].end()) {
		return cache[bitwidth][right];
	}

	auto left = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto result = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));

	// multiplication[n](x, 0, z) = (z = 0) or multiplication[n](0, x, z) = (z = 0);
	if (is_zero_bv_constant<node>(right)) {
		auto zero_right = tau::build_ref("_bvmul", { left, right, result });
		auto body = tau::build_bf_eq(tau::_0(bv_type_id<node>(bitwidth)), result);
		auto rule = make_rule<node>(zero_right, body);

#ifdef DEBUG
		LOG_TRACE << "bvmul_rec_rule (zero case): " << LOG_RULE(rule) << "\n";
		LOG_TRACE << "bvmul_rec_rule/head: " << LOG_FM(rule.first->get()) << "\n";
		LOG_TRACE << "bvmul_rec_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG

		return rule;
	}

	auto w = tau::build_variable(bv_type_id<node>(bitwidth));

	// case y & 1 = 1: multiplication(x, y, w) = ex z ex v multiplication(x << 1, v) && addition(x, v, z);
	if (is_bv_lsb_one<node>(right)) {
		auto v = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto odd_right = tau::build_ref("_bvmul", { left, right, result });
		auto odd_body = tau::build_wff_ex(w,
			tau::build_wff_ex(w,
				tau::build_wff_ex(v,
					tau::build_wff_and(
						tau::build_wff_and(
							bvshl_by_one<node>(left, v),
							make_bvmul_call_from_offset<node>(v, w, bv_shr_by_one<node>(right))),
						bvadd<node>(left, w, result)))));
		auto rule = make_rule<node>(odd_right, odd_body);

#ifdef DEBUG
		LOG_TRACE << "bvmul_rec_rule (odd case): " << LOG_RULE(rule) << "\n";
		LOG_TRACE << "bvmul_rec_rule/head: " << LOG_FM(rule.first->get()) << "\n";
		LOG_TRACE << "bvmul_rec_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG

		return rule;
	}

	// case y & 1 = 0: multiplication(x, y, z) = ex v multiplication[y >> 1](x << 1, v) && (z = v);
	auto even_right = tau::build_ref("_bvmul", { left, right, result });
	auto shifted_x = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto even_body = tau::build_wff_ex(w,
		tau::build_wff_and(
			bvshl_by_one<node>(left, shifted_x),
			tau::build_wff_and(
				make_bvmul_call_from_offset<node>(shifted_x, w, bv_shr_by_one<node>(right)),
				tau::build_bf_eq(result, w))));
	auto rule = make_rule<node>(even_right, even_body);

#ifdef DEBUG
	LOG_TRACE << "bvmul_rec_rule (even case): " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvmul_rec_rule/head: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvmul_rec_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG

	return rule;
}

/**
 * @brief Returns the rule for bitvector multiplication (caching by bitwidth and right operand).
 *
 * @tparam node Node type
 * @param y Right operand (constant)
 * @param bitwidth Bitwidth of the operands
 * @return The constructed rule
 */
template<NodeType node>
static rewriter::rule bvmul_rule(tref left, size_t bitwidth) {
	using tau = tree<node>;

	static std::map<size_t, std::map<tref, rewriter::rule>> cache;
	static std::map<size_t, std::map<tref, rewriter::rules>> partial_cache;
	// If the rule is already computed for the given bitwidth and right operand, we return it from the cache
	if (cache.find(bitwidth) != cache.end()) return cache[bitwidth][left];
	// Otherwise, we compute the rule, store it in the cache and return it.
	// First we collect all the rules.
	rewriter::rules rules;
	auto n_left = left;
	while (!is_zero_bv_constant<node>(n_left)) {
		if (partial_cache[bitwidth].find(n_left) != partial_cache[bitwidth].end()) {
			rules.insert(rules.end(), partial_cache[bitwidth][n_left].begin(), partial_cache[bitwidth][n_left].end());
		} else {
			auto rule = bvmul_rec_rule<node>(n_left, bitwidth);
			rules.push_back(rule);
			partial_cache[bitwidth][n_left] = { rule };
		}
		n_left = bv_shr_by_one<node>(n_left);
	}
	// Then we build a main term to compute the actual predicate.
	auto right = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto result = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto main = make_bvmul_call_from_offset<node>(right, result, left);
	auto rr = make_rr<node>(rules, main);
	auto body = apply_rr_to_formula(rr);
	auto rule = make_rule<node>(main, body);

#ifdef DEBUG
	LOG_TRACE << "bvmul_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvmul_rule/head: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvmul_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG

	cache[bitwidth][right] = rule;
	return cache[bitwidth][right];
}

template<NodeType node>
tref bvmul(tref left, tref right, tref result) {
	auto bitwidth = get_bv_type_bitwidth<node>(left);
	if (!is_bv_constant<node>(right)) {
		DBG( LOG_DEBUG << "Only multiplication by constant is supported in predicate blasting."; )
		return nullptr;
	}
	auto rule = bvmul_rule<node>(right, bitwidth);
	auto call = make_bvmul_call_from_offset<node>(left, result, right);
	return nso_rr_apply<node>({ rule }, call);
}

//
//
// _bvdiv and _bvmod are predicate recurrence relations that compute the quotient
// and modulo of two bitvectors (being the right operand a constant). The idea is
// to define the division and modulo as usual:
//
// dividend = divisor * quotient + remainder && remainder < divisor
//
//

/**
 * @brief Creates a call to the bitvector division recurrence.
 * @tparam node Node type
 * @param dividend Dividend
 * @param divisor Divisor
 * @param result Result variable
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvdiv_call(tref dividend, tref divisor, tref result) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvdiv", { dividend, divisor, result })));
}

/**
 * @brief Returns the rule for bitvector division (caching by bitwidth and divisor).
 *
 * @tparam node Node type
 * @param divisor Divisor (constant)
 * @param bitwidth Bitwidth of the operands
 * @return The constructed rule
 */
template<NodeType node>
static rewriter::rule bvdiv_rule(tref divisor /* bv copnstant */, size_t bitwidth) {
	using tau = tree<node>;

	static std::map<size_t, std::map<tref, rewriter::rule>> cache;
	if (cache.find(bitwidth) != cache.end()) return cache[bitwidth][divisor];

	// We build the main term to compute the actual predicate.
	auto quotient_var = tau::tau::build_variable(bv_type_id<node>(bitwidth));
	auto quotient = tau::get(tau::bf, quotient_var);
	auto remainder_var = tau::tau::build_variable(bv_type_id<node>(bitwidth));
	auto remainder = tau::get(tau::bf, remainder_var);
	auto dividend = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto exact_var = tau::tau::build_variable(bv_type_id<node>(bitwidth));
	auto exact = tau::get(tau::bf, exact_var);
	auto call = make_bvdiv_call<node>(dividend, divisor, quotient);
	auto body = tau::build_wff_ex(exact_var,
		tau::build_wff_ex(quotient_var,
			tau::build_wff_ex(remainder_var,
				tau::build_wff_and(
					bvsub<node>(dividend, remainder, exact),
					tau::build_wff_and(
						bvmul<node>(quotient, divisor, exact),
						bvlt<node>(remainder, divisor))))));
	auto rule = make_rule<node>(call, body);

#ifdef DEBUG
	LOG_TRACE << "bvdiv_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvdiv_rule/head: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvdiv_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG

	cache[bitwidth][divisor] = rule;
	return cache[bitwidth][divisor];
}

template<NodeType node>
tref bvdiv(tref dividend, tref divisor, tref quotient) {
	auto bitwidth = get_bv_type_bitwidth<node>(divisor);
	if (!is_bv_constant<node>(divisor)) {
		DBG( LOG_DEBUG << "Only division by constant is supported in predicate blasting."; )
		return nullptr;
	}
	auto rule = bvdiv_rule<node>(divisor, bitwidth);
	auto call = make_bvdiv_call<node>(dividend, divisor, quotient);
	auto rr = make_rr<node>({ rule }, call);
	return apply_rr_to_formula(rr);
}

/**
 * @brief Creates a call to the bitvector modulo recurrence.
 * @tparam node Node type
 * @param dividend Dividend
 * @param divisor Divisor
 * @param result Result variable
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvmod_call(tref dividend, tref divisor, tref result) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvmod", { dividend, divisor, result })));
}

/**
 * @brief Returns the rule for bitvector modulo (caching by bitwidth and divisor).
 *
 * @tparam node Node type
 * @param divisor Divisor (constant)
 * @param bitwidth Bitwidth of the operands
 * @return The constructed rule
 */
template<NodeType node>
static rewriter::rule bvmod_rule(tref divisor /* bv copnstant */, size_t bitwidth) {
	using tau = tree<node>;

	static std::map<size_t, std::map<tref, rewriter::rule>> cache;
	if (cache.find(bitwidth) != cache.end()) return cache[bitwidth][divisor];

	// We build the main term to compute the actual predicate.
	auto quotient_var = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto quotient = tau::get(tau::bf, quotient_var);
	auto remainder_var = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto remainder = tau::get(tau::bf, remainder_var);
	auto dividend_var = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto dividend = tau::get(tau::bf, dividend_var);
	auto exact_var = tau::tau::build_variable(bv_type_id<node>(bitwidth));
	auto exact = tau::get(tau::bf, exact_var);
	auto call = make_bvmod_call<node>(dividend, divisor, remainder);
	auto body = tau::build_wff_ex(exact_var,
		tau::build_wff_ex(quotient_var,
			tau::build_wff_ex(remainder_var,
				tau::build_wff_and(
					bvsub<node>(dividend, remainder, exact),
					tau::build_wff_and(
						bvmul<node>(quotient, divisor, exact),
						bvlt<node>(remainder, divisor))))));
	auto rule = make_rule<node>(call, body);

#ifdef DEBUG
	LOG_TRACE << "bvmod_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvmod_rule/head: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvmod_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG

	cache[bitwidth][divisor] = rule;
	return cache[bitwidth][divisor];
}

template<NodeType node>
tref bvmod(tref dividend, tref divisor, tref remainder) {
	auto bitwidth = get_bv_type_bitwidth<node>(divisor);
	if (!is_bv_constant<node>(divisor)) {
		DBG( LOG_DEBUG << "Only modulo by constant is supported in predicate blasting."; )
		return nullptr;
	}
	auto rule = bvmod_rule<node>(divisor, bitwidth);
	auto call = make_bvmod_call<node>(dividend, divisor, remainder);
	auto rr = make_rr<node>({ rule }, call);
	return apply_rr_to_formula(rr);
}

} // namespace idni::tau_lang