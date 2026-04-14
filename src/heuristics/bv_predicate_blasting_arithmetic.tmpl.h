// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <bit>

namespace idni::tau_lang {

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
static tref make_bvsub_rr_call(tref left, tref right, tref result, size_t index) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_bvsub", { index }, { left, right, result })));
}

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
static tref make_bvsub_call(tref left, tref right, tref result, tref offset) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_bvsub", { offset }, { left, right, result })));
}

/**
 * @brief Creates a call to the bitvector subtraction recurrence (no offset).
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @param result Result variable
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvsub_call(tref left, tref right, tref result) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvsub", { left, right, result })));
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
static tref make_bvmul_call(tref left, size_t right, tref result) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_bvmul", { right }, { left, result })));
}

/**
 * @brief Creates a call to the bitvector multiplication recurrence.
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @param result Result variable
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvmul_call(tref left, tref right, tref result) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvmul", { left, right, result })));
}

/**
 * @brief Creates a call to the euclidean division recurrence.
 * @tparam node Node type
 * @param dividend Dividend
 * @param divisor Divisor
 * @param quotient Quotient variable
 * @param remainder Remainder variable
 * @return The constructed call term
 */
template<NodeType node>
static tref make_euclidean_division_call(tref dividend, tref divisor, tref quotient, tref remainder) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_euclidean_division", { dividend, divisor, quotient, remainder })));
}

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
 * @brief Creates a call to the indexed bitvector addition recurrence.
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @param result Result variable
 * @param index Index for the recurrence
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvadd_rr_call(tref left, tref right, tref result, size_t index) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_bvadd", { index }, { left, right, result })));
}

/**
 * @brief Creates a call to the offset bitvector addition recurrence.
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @param result Result variable
 * @param offset Offset for the recurrence
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvadd_call(tref left, tref right, tref result, tref offset) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_bvadd", { offset }, { left, right, result })));
}

/**
 * @brief Creates a call to the bitvector addition recurrence (no offset).
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @param result Result variable
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvadd_call(tref left, tref right, tref result) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvadd", { left, right, result })));
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
	// addition(x, y) = addition(x ^ y, (x & y) << 1, m) mod m;
	using tau = tree<node>;

	rewriter::rules rules;
	// general case: addition[n](x, y, z) = ex w addition[n-1](x ^ y, w, z) && bv_shr_by_one(x & y, w);
	auto x = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto y = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto z = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto n = tau::build_variable(untyped_type_id<node>());
	auto n_minus_1 = tau::build_ref_shift_offset(n, 1);
	auto general_header = tau::build_rr_ref("_bvadd", {n}, { x, y, z });
	auto w = tau::build_variable(bv_type_id<node>(bitwidth));
	auto bf_w = tau::get(tau::bf, w);
	auto general_body = tau::build_wff_ex(w,
		tau::build_wff_and(
			make_bvadd_call<node>(tau::build_bf_xor(x, y), bf_w, z, n_minus_1),
			make_bvshl_by_one_call<node>(tau::build_bf_and(x, y), bf_w)
	));

	// base case: addition[0](x, y, z) = (z = x);
	auto base_header = make_bvadd_rr_call<node>(x, y, z, 0);
	auto base_body = tau::build_bf_eq(x, z);

	// create rules
	rules.push_back(make_rule<node>(base_header, base_body));
	rules.push_back(make_rule<node>(general_header, general_body));
#ifdef DEBUG
	for (const auto& rule : rules) {
		LOG_TRACE << "bvadd_rules/rule: " << LOG_RULE(rule) << "\n";
		LOG_TRACE << "bvadd_rules/header: " << LOG_FM(rule.first->get()) << "\n";
		LOG_TRACE << "bvadd_rules/body: " << LOG_FM(rule.second->get()) << "\n";
	}
#endif // DEBUG
	return rules;
}

// subtraction(x,y) = subtraction(x ^ y, (~x & y) << 1, m) mod m;
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

	rewriter::rules rules;
	// general case: subtraction[n](x, y, z) = ex w subtraction[n-1](x ^ y, w, z) && bv_shr_by_one((~x & y), w);
	auto x = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto y = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto z = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto n = tau::build_variable(untyped_type_id<node>());
	auto n_minus_1 = tau::build_ref_shift_offset(n, 1);
	auto general_header = tau::build_rr_ref("_bvsub", { n }, { x, y, z });
	auto w = tau::build_variable(bv_type_id<node>(bitwidth));
	auto bf_w = tau::get(tau::bf, w);
	auto general_body = tau::build_wff_ex(w,
		tau::build_wff_and(
			make_bvsub_call<node>(tau::build_bf_xor(x, y), bf_w, z, n_minus_1),
			make_bvshl_by_one_call<node>(tau::build_bf_and(tau::build_bf_neg(x), y), bf_w)
	));

	// base case: subtraction[0](x, y, z) = (z = x);
	auto base_header = make_bvsub_rr_call<node>(x, y, z, 0);
	auto base_body = tau::build_bf_eq(x, z);
	rules.push_back(make_rule<node>(base_header, base_body));
	rules.push_back(make_rule<node>(general_header, general_body));
#ifdef DEBUG
	for (const auto& rule : rules) {
		LOG_TRACE << "bvsub_rules: " << LOG_RULE(rule) << "\n";
		LOG_TRACE << "bvsub_rules/header: " << LOG_FM(rule.first->get()) << "\n";
		LOG_TRACE << "bvsub_rules/body: " << LOG_FM(rule.second->get()) << "\n";
	}
#endif // DEBUG
	return rules;
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
static rewriter::rule bvmul_rec_rule(tref y /* cvc5 constant */, size_t bitwidth) {
	using tau = tree<node>;

	DBG( assert(is_bv_constant<node>(y)); )

	auto x = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto z = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));

	// multiplication[n](x, 0, z) = (z = 0) or multiplication[n](0, x, z) = (z = 0);
	if (is_zero_bv_constant<node>(y)) {
		auto zero_right = tau::build_ref("_bvmul", { x, y, z });
		auto body = tau::build_bf_eq(tau::_0(bv_type_id<node>(bitwidth)), z);
		auto rule = make_rule<node>(zero_right, body);
#ifdef DEBUG
		LOG_TRACE << "bvmul_rec_rule (zero case): " << LOG_RULE(rule) << "\n";
		LOG_TRACE << "bvmul_rec_rule/header: " << LOG_FM(rule.first->get()) << "\n";
		LOG_TRACE << "bvmul_rec_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG
		return rule;
	}

	auto w = tau::build_variable(bv_type_id<node>(bitwidth));

	// case y & 1 = 1: multiplication(x, y, w) = ex z ex v multiplication(x << 1, v) && addition(x, v, z);
	if (is_bv_lsb_one<node>(y)) {
		auto v = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto odd_right = tau::build_ref("_bvmul", { x, y, z });
		auto odd_body = tau::build_wff_ex(w,
			tau::build_wff_ex(w,
				tau::build_wff_ex(v,
					tau::build_wff_and(
						tau::build_wff_and(
							make_bvshl_by_one_call<node>(x, v),
							make_bvmul_call<node>(v, bv_shr_by_one<node>(y), w)),
						make_bvadd_call<node>(x, w, z)))));
		auto rule = make_rule<node>(odd_right, odd_body);
#ifdef DEBUG
		LOG_TRACE << "bvmul_rec_rule (odd case): " << LOG_RULE(rule) << "\n";
		LOG_TRACE << "bvmul_rec_rule/header: " << LOG_FM(rule.first->get()) << "\n";
		LOG_TRACE << "bvmul_rec_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG
		return rule;
	}

	// case y & 1 = 0: multiplication(x, y, z) = ex v multiplication[y >> 1](x << 1, v) && (z = v);
	auto even_right = tau::build_ref("_bvmul", { x, y, z });
	auto shifted_x = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto even_body = tau::build_wff_ex(w,
		tau::build_wff_and(
			make_bvshl_by_one_call<node>(x, shifted_x),
			tau::build_wff_and(
				make_bvmul_call<node>(shifted_x, bv_shr_by_one<node>(y), w),
				tau::build_bf_eq(z, w))));
	auto rule = make_rule<node>(even_right, even_body);
#ifdef DEBUG
	LOG_TRACE << "bvmul_rec_rule (even case): " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvmul_rec_rule/header: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvmul_rec_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG
	return rule;
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
	// if the rule is already computed for the given bitwidth, we return it from the cache
	if (cache.find(bitwidth) != cache.end()) return cache[bitwidth];
	// Otherwise, we compute the rule, store it in the cache and return it.
	// First we collect all the rules.
	rewriter::rules rs;
	auto ars = bvadd_rules<node>(bitwidth);
	rs.insert(rs.end(), ars.begin(), ars.end());
	// Then we build a main term to compute the actual predicate.
	auto left = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto right = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto result = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto head = make_bvadd_rr_call<node>(left, right, result, bitwidth);
	auto temp = make_rr<node>(rs, head);
	auto body = apply_rr_to_formula(temp);
	auto rule = make_rule<node>(head, body);
	if (!body) {
		DBG( LOG_DEBUG << "Failed to compute bvadd rule. Returning empty rule."; )
		return rule;
	}
#ifdef DEBUG
	LOG_TRACE << "bvadd_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvadd_rule/header: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvadd_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG
	cache[bitwidth] = rule;
	return cache[bitwidth];
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
	// If the rule is already computed for the given bitwidth, we return it from the cache
	if (cache.find(bitwidth) != cache.end()) return cache[bitwidth];
	// Otherwise, we compute the rule, store it in the cache and return it.
	// First we collect all the rules.
	rewriter::rules rs;
	auto srs = bvsub_rules<node>(bitwidth);
	rs.insert(rs.end(), srs.begin(), srs.end());
	// Then we build a main term to compute the actual predicate.
	auto left = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto right = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto result = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto head = make_bvsub_rr_call<node>(left, right, result, bitwidth);
	auto temp = make_rr<node>(rs, head);
	auto body = apply_rr_to_formula(temp);
	auto rule = make_rule<node>(head, body);
	if (!body) {
		DBG( LOG_DEBUG << "Failed to compute bvsub rule. Returning empty rule."; )
		return rule;
	}
#ifdef DEBUG
	LOG_TRACE << "bvsub_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvsub_rule/header: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvsub_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG
	cache[bitwidth] = rule;
	return cache[bitwidth];
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
static rewriter::rule bvmul_rule(tref y, size_t bitwidth) {
	using tau = tree<node>;

	static std::map<size_t, std::map<tref, rewriter::rule>> cache;
	static std::map<size_t, std::map<tref, rewriter::rules>> partial_cache;
	// If the rule is already computed for the given bitwidth and right operand, we return it from the cache
	if (cache.find(bitwidth) != cache.end()) return cache[bitwidth][y];
	// Otherwise, we compute the rule, store it in the cache and return it.
	// First we collect all the rules.
	rewriter::rules rs;
	auto ny = y;
	while (!is_zero_bv_constant<node>(ny)) {
		if (partial_cache[bitwidth].find(ny) != partial_cache[bitwidth].end()) {
			rs.insert(rs.end(), partial_cache[bitwidth][ny].begin(), partial_cache[bitwidth][ny].end());
		} else {
			auto rule = bvmul_rec_rule<node>(ny, bitwidth);
			rs.push_back(rule);
			partial_cache[bitwidth][ny] = { rule };
		}
		ny = bv_shr_by_one<node>(ny);
	}
	// Then we build a main term to compute the actual predicate.
	auto left = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto result = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto main = make_bvmul_call<node>(left, y, result);
	auto temp = make_rr<node>(rs, main);
	auto body = apply_rr_to_formula(temp);
	auto rule = make_rule<node>(main, body);
	if (!body) {
		DBG( LOG_DEBUG << "Failed to compute bvmul rule. Returning empty rule."; )
		return rule;
	}
#ifdef DEBUG
	LOG_TRACE << "bvmul_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvmul_rule/header: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvmul_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG
	cache[bitwidth][y] = rule;
	return cache[bitwidth][y];
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
	// If the rule is already computed for the given bitwidth and right operand,
	// we return it from the cache
	if (cache.find(bitwidth) != cache.end()) return cache[bitwidth][divisor];
	// Otherwise, we compute the rule, store it in the cache and return it.
	// First we collect all the rules needed for computing the euclidean
	// division: addition, multiplication, less then,...
	rewriter::rules rs;
	auto add_rules = bvadd_rules<node>(bitwidth);
	rs.insert(rs.end(), add_rules.begin(), add_rules.end());
	//rs.push_back(bvmul_rule<node>(bitwidth).end());
	auto lt_rules = bvlt_rules<node>(bitwidth);
	rs.insert(rs.end(), lt_rules.begin(), lt_rules.end());
	// Then we build the main term to compute the actual predicate, i.e.
	// dividend = divisor * quotient + remainder, remainder < divisor.
	auto quotient_var = tau::tau::build_variable(bv_type_id<node>(bitwidth));
	auto quotient = tau::get(tau::bf, quotient_var);
	auto remainder_var = tau::tau::build_variable(bv_type_id<node>(bitwidth));
	auto remainder = tau::get(tau::bf, remainder_var);
	auto dividend = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto exact_var = tau::tau::build_variable(bv_type_id<node>(bitwidth));
	auto exact = tau::get(tau::bf, exact_var);
	auto main = tau::build_wff_ex(exact_var,
		tau::build_wff_ex(quotient_var,
			tau::build_wff_ex(remainder_var,
				tau::build_wff_and(
					make_bvsub_call<node>(dividend, remainder, exact),
					tau::build_wff_and(
						make_bvmul_call<node>(quotient, divisor, exact),
						make_bvlt_call<node>(remainder, divisor, bitwidth))))));
	auto temp = make_rr<node>(rs, main);
	auto head = make_bvdiv_call<node>(dividend, divisor, quotient);
	auto body = apply_rr_to_formula(temp);
	auto rule = make_rule<node>(head, body);
	if (!body) {
		DBG( LOG_DEBUG << "Failed to compute bvdiv rule. Returning empty rule."; )
		return rule;
	}
#ifdef DEBUG
	LOG_TRACE << "bvdiv_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvdiv_rule/header: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvdiv_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG
	cache[bitwidth][divisor] = rule;
	return cache[bitwidth][divisor];
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
	// If the rule is already computed for the given bitwidth and right operand, we return it from the cache
	if (cache.find(bitwidth) != cache.end()) return cache[bitwidth][divisor];
	// First we collect all the rules needed for computing the euclidean
	// division: addition, multiplication, less then,...
	rewriter::rules rs = bvadd_rules<node>(bitwidth);
	//rs.push_back(bvmul_rule<node>(bitwidth).end());
	auto lt_rules = bvlt_rules<node>(bitwidth);
	rs.insert(rs.end(), lt_rules.begin(), lt_rules.end());
	// Otherwise, we compute the rule, store it in the cache and return it..begin(), bvlt_rules<node>(bitwidth).end());
	// Then we build the main term to compute the actual predicate.
	auto quotient_var = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto quotient = tau::get(tau::bf, quotient_var);
	auto remainder_var = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto remainder = tau::get(tau::bf, remainder_var);
	auto dividend = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto exact_var = tau::tau::build_variable(bv_type_id<node>(bitwidth));
	auto exact = tau::get(tau::bf, exact_var);
	auto main =tau::build_wff_ex(exact_var,
		tau::build_wff_ex(quotient_var,
			tau::build_wff_ex(remainder_var,
				tau::build_wff_and(
					make_bvsub_call<node>(dividend, remainder, exact),
					tau::build_wff_and(
						make_bvmul_call<node>(quotient, divisor, exact),
						bvlt_call<node>(remainder, divisor, bitwidth))))));
	auto temp = make_rr<node>(rs, main);
	auto head = make_bvmod_call<node>(dividend, divisor, remainder);
	auto body = apply_rr_to_formula(temp);
	auto rule = make_rule<node>(head, body);
	if (!body) {
		DBG( LOG_DEBUG << "Failed to compute bvmod rule. Returning empty rule."; )
		return rule;
	}
#ifdef DEBUG
	LOG_TRACE << "bvmod_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvmod_rule/header: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvmod_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG
	cache[bitwidth][divisor] = rule;
	return cache[bitwidth][divisor];
}

/**
 * @brief Applies the bitvector addition rule to the given operands.
 *
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @param result Result variable
 * @return The resulting predicate term
 */
template<NodeType node>
tref bvadd(tref left, tref right, tref result) {
	auto bitwidth = get_bv_type_bitwidth<node>(left);
	auto rule = bvadd_rule<node>(bitwidth);
	auto call = make_bvadd_call<node>(left, right, result);
	return nso_rr_apply<node>(rule, call);
}

/**
 * @brief Applies the bitvector multiplication rule to the given operands.
 *
 * Only supports multiplication by a constant right operand.
 *
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand (must be constant)
 * @param result Result variable
 * @return The resulting predicate term, or nullptr if unsupported
 */
template<NodeType node>
tref bvmul(tref left, tref right, tref result) {
	auto bitwidth = get_bv_type_bitwidth<node>(left);
	if (!is_bv_constant<node>(right)) {
		DBG( LOG_DEBUG << "Currently only multiplication by constant is supported in predicate blasting."; )
		return nullptr;
	}
	auto rule = bvmul_rule<node>(right, bitwidth);
	auto call = make_bvmul_call<node>(left, right, result);
	return nso_rr_apply<node>(rule, call);
}

/**
 * @brief Applies the bitvector subtraction rule to the given operands.
 *
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @param result Result variable
 * @return The resulting predicate term
 */
template<NodeType node>
tref bvsub(tref left, tref right, tref result) {
	auto bitwidth = get_bv_type_bitwidth<node>(left);
	auto rule = bvsub_rule<node>(bitwidth);
	auto call = make_bvsub_call<node>(left, right, result);
	return nso_rr_apply<node>(rule, call);
}

/**
 * @brief Applies the bitvector division rule to the given operands.
 *
 * Only supports division by a constant divisor.
 *
 * @tparam node Node type
 * @param dividend Dividend
 * @param divisor Divisor (must be constant)
 * @param quotient Quotient variable
 * @return The resulting predicate term, or nullptr if unsupported
 */
template<NodeType node>
tref bvdiv(tref dividend, tref divisor, tref quotient) {
	auto bitwidth = get_bv_type_bitwidth<node>(divisor);
	if (!is_bv_constant<node>(divisor)) {
		DBG( LOG_DEBUG << "Currently only division by constant is supported in predicate blasting."; )
		return nullptr;
	}
	auto rule = bvdiv_rule<node>(divisor, bitwidth);
	auto call = make_bvdiv_call<node>(dividend, divisor, quotient);
	return nso_rr_apply<node>(rule, call);
}

/**
 * @brief Applies the bitvector modulo rule to the given operands.
 *
 * Only supports modulo by a constant divisor.
 *
 * @tparam node Node type
 * @param dividend Dividend
 * @param divisor Divisor (must be constant)
 * @param remainder Remainder variable
 * @return The resulting predicate term, or nullptr if unsupported
 */
template<NodeType node>
tref bvmod(tref dividend, tref divisor, tref remainder) {
	auto bitwidth = get_bv_type_bitwidth<node>(divisor);
	if (!is_bv_constant<node>(divisor)) {
		DBG( LOG_DEBUG << "Currently only division by constant is supported in predicate blasting."; )
		return nullptr;
	}
	auto rule = bvdiv_rule<node>(divisor, bitwidth);
	auto call = make_bvdiv_call<node>(dividend, divisor, remainder);
	return nso_rr_apply<node>(rule, call);
}

} // namespace idni::tau_lang