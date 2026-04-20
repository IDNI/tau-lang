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
 * @param augend Left operand
 * @param addend Right operand (constant)
 * @param sum Result variable
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvadd_call_from_offset(tref augend, tref addend, tref sum, tref offset) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_bvadd", { offset }, { augend, addend, sum })));
}

/**
 * @brief Creates a call to the indexed bitvector subtraction recurrence.
 * @tparam node Node type
 * @param minuend Left operand
 * @param subtrahend Right operand
 * @param difference Result variable
 * @param index Index for the recurrence
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvadd_call_from_index(tref minuend, tref subtrahend, tref difference, size_t index) {
	using tau = tree<node>;

	auto offset = tau::get_num(index);
	return make_bvadd_call_from_offset<node>(minuend, subtrahend, difference, offset);
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

	auto augend = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto addend = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto sum = tau::build_bf_variable(bv_type_id<node>(bitwidth));

	rewriter::rules rules;
	// base case: addition[0](x, y, z) = (z = x);
	auto base_header = make_bvadd_call_from_index<node>(augend, addend, sum, 0);
	auto base_body = tau::build_bf_eq(augend, sum);
	rules.push_back(make_rule<node>(base_header, base_body));
	// general case: addition[n](x, y, z) = ex w addition[n-1](x ^ y, w, z) && bv_shr_by_one(x & y, w);
	auto n = tau::build_variable(untyped_type_id<node>());
	auto n_minus_1 = tau::build_shift(n, 1);
	auto general_header = make_bvadd_call_from_offset<node>(augend, addend, sum, n);
	auto w = tau::build_variable(bv_type_id<node>(bitwidth));
	auto bf_w = tau::get(tau::bf, w);
	auto general_body = tau::build_wff_ex(w,
		tau::build_wff_and(
			make_bvadd_call_from_offset<node>(tau::build_bf_xor(augend, addend), bf_w, sum, n_minus_1),
			bvshl_by_one<node>(tau::build_bf_and(augend, addend), bf_w)
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

	auto augend = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto addend = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto sum = tau::build_bf_variable(bv_type_id<node>(bitwidth));

	auto head = make_bvadd_call_from_index<node>(augend, addend, sum, bitwidth);
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
tref bvadd(tref augend, tref addend, tref sum) {
	auto bitwidth = get_bv_type_bitwidth<node>(augend);
	auto rule = bvadd_rule<node>(bitwidth);
	auto call = make_bvadd_call_from_index<node>(augend, addend, sum, bitwidth);
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
 * @param minuend Left operand
 * @param subtrahend Right operand
 * @param difference Result variable
 * @param offset Offset for the recurrence
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvsub_call_from_offset(tref minuend, tref subtrahend, tref difference, tref offset) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_bvsub", { offset }, { minuend, subtrahend, difference })));
}

/**
 * @brief Creates a call to the indexed bitvector subtraction recurrence.
 * @tparam node Node type
 * @param minuend Left operand
 * @param subtrahend Right operand
 * @param difference Result variable
 * @param index Index for the recurrence
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvsub_call_from_index(tref minuend, tref subtrahend, tref difference, size_t index) {
	using tau = tree<node>;

	auto offset = tau::get_num(index);
	return make_bvsub_call_from_offset<node>(minuend, subtrahend, difference, offset);
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

	auto minuend = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto subtrahend = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto difference = tau::build_bf_variable(bv_type_id<node>(bitwidth));

	rewriter::rules rules;
	// base case: subtraction[0](x, y, z) = (z = x);
	auto base_header = make_bvsub_call_from_index<node>(minuend, subtrahend, difference, 0);
	auto base_body = tau::build_bf_eq(minuend, difference);
	rules.push_back(make_rule<node>(base_header, base_body));
	// general case: subtraction[n](x, y, z) = ex w subtraction[n-1](x ^ y, w, z) && bv_shr_by_one((~x & y), w);
	auto n = tau::build_variable(untyped_type_id<node>());
	auto n_minus_1 = tau::build_shift(n, 1);
	auto general_header = make_bvsub_call_from_offset<node>(minuend, subtrahend, difference, n);
	auto w = tau::build_variable(bv_type_id<node>(bitwidth));
	auto bf_w = tau::get(tau::bf, w);
	auto general_body = tau::build_wff_ex(w,
		tau::build_wff_and(
			make_bvsub_call_from_offset<node>(tau::build_bf_xor(minuend, subtrahend), bf_w, difference, n_minus_1),
			bvshl_by_one<node>(tau::build_bf_and(tau::build_bf_neg(minuend), subtrahend), bf_w)
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

	auto minuend = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto subtrahend = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto difference = tau::build_bf_variable(bv_type_id<node>(bitwidth));

	rewriter::rules rs;
	auto substractions = bvsub_rules<node>(bitwidth);
	rs.insert(rs.end(), substractions.begin(), substractions.end());
	auto head = make_bvsub_call_from_index<node>(minuend, subtrahend, difference, bitwidth);
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
tref bvsub(tref minuend, tref subtrahend, tref difference) {
	auto bitwidth = get_bv_type_bitwidth<node>(minuend);
	auto rule = bvsub_rule<node>(bitwidth);
	auto call = make_bvsub_call_from_index<node>(minuend, subtrahend, difference, bitwidth);
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
 * @param multiplicant Left operand
 * @param multiplier Right operand (constant)
 * @param product Result variable
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvmul_call(tref multiplicant, tref multiplier, tref product) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvmul", { multiplicant, multiplier, product })));
}

/**
 * @brief Returns the rule for bitvector multiplication recurrence.
 *
 * Handles multiplication by constant right operand.
 *
 * @tparam node Node type
 * @param multiplier Right operand (bv constant)
 * @param bitwidth Bitwidth of the operands
 * @return The constructed rule
 */
template<NodeType node>
static rewriter::rule bvmul_rec_rule(tref multiplier /* cvc5 constant */) {
	using tau = tree<node>;

	DBG( assert(tau::get(tau::trim(multiplier)).is(tau::bf_f) || is_bv_constant<node>(tau::trim(multiplier))); )

	static std::map<tref, rewriter::rule> cache;
	if (cache.find(multiplier) != cache.end()) {
		return cache[multiplier];
	}

	auto bitwidth = get_bv_type_bitwidth<node>(multiplier);
	auto multiplicand = tau::build_variable(bv_type_id<node>(bitwidth));
	auto bf_multiplicand = tau::get(tau::bf, multiplicand);
	auto product = tau::build_variable(bv_type_id<node>(bitwidth));
	auto bf_product = tau::get(tau::bf, product);
	auto head = make_bvmul_call<node>(bf_multiplicand, multiplier, bf_product);

	// multiplication[n](x, 0, z) = (z = 0) or multiplication[n](0, x, z) = (z = 0);
	if (tau::get(tau::trim(multiplier)).is(tau::bf_f)) { // || is_zero_bv_constant<node>(tau::trim(multiplier))) {
		auto zero_case = tau::build_bf_eq(tau::_0(bv_type_id<node>(bitwidth)), bf_product);
		auto rule = make_rule<node>(head, zero_case);

#ifdef DEBUG
		LOG_TRACE << "bvmul_rec_rule (zero case): " << LOG_RULE(rule) << "\n";
		LOG_TRACE << "bvmul_rec_rule/head: " << LOG_FM(rule.first->get()) << "\n";
		LOG_TRACE << "bvmul_rec_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG

		return rule;
	}

	auto shr_multiplier = tau::get(tau::bf, bv_shr_by_one<node>(tau::trim(multiplier)));
	auto bf_shr_multiplier = tau::get(tau::bf, shr_multiplier);
	auto subproduct = tau::build_variable(bv_type_id<node>(bitwidth));
	auto bf_subproduct = tau::get(tau::bf, subproduct);
	auto shl_multiplicant = tau::build_variable(bv_type_id<node>(bitwidth));
	auto bf_shl_multiplicant = tau::get(tau::bf, shl_multiplicant);

	// case y & 1 = 1: multiplication(x, y, w) = ex z ex v multiplication(x << 1, v) && addition(x, v, z);
	if (is_bv_lsb_one<node>(tau::trim(multiplier))) {
		auto odd_case = tau::build_wff_ex(shl_multiplicant,
			tau::build_wff_ex(subproduct,
				tau::build_wff_and(
					tau::build_wff_and(
						bvshl_by_one<node>(bf_multiplicand, bf_shl_multiplicant),
						make_bvmul_call<node>(bf_shl_multiplicant, bf_shr_multiplier, bf_subproduct)),
					bvadd<node>(bf_multiplicand, bf_subproduct, bf_product))));
		auto rule = make_rule<node>(head, odd_case);

#ifdef DEBUG
		LOG_TRACE << "bvmul_rec_rule (odd case): " << LOG_RULE(rule) << "\n";
		LOG_TRACE << "bvmul_rec_rule/head: " << LOG_FM(rule.first->get()) << "\n";
		LOG_TRACE << "bvmul_rec_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG

		return rule;
	}

	// case y & 1 = 0: multiplication(x, y, z) = ex v multiplication[y >> 1](x << 1, v) && (z = v);
	auto even_case = tau::build_wff_ex(shl_multiplicant,
		tau::build_wff_ex(subproduct,
			tau::build_wff_and(
				bvshl_by_one<node>(bf_multiplicand, bf_shl_multiplicant),
				tau::build_wff_and(
					make_bvmul_call<node>(bf_shl_multiplicant, bf_shr_multiplier, bf_subproduct),
					tau::build_bf_eq(bf_product, bf_subproduct)))));
	auto rule = make_rule<node>(head, even_case);

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
 * @param multiplier Right operand (constant)
 * @param bitwidth Bitwidth of the operands
 * @return The constructed rule
 */
template<NodeType node>
static rewriter::rule bvmul_rule(tref multiplier /* cvc5 constant */) {
	using tau = tree<node>;

	DBG( LOG_INFO << "Checking if tref " << LOG_FM_TREE(multiplier) << " is a zero bitvector constant.\n"; )

	static std::map<tref, rewriter::rule> cache;

	if (cache.find(multiplier) != cache.end()) return cache[multiplier];

	rewriter::rules rules;
	auto current = multiplier;
	auto bitwidth = get_bv_type_bitwidth<node>(multiplier);

	while (!tau::get(tau::trim(current)).is(tau::bf_f)) { //is_zero_bv_constant<node>(tau::trim(current))) {
		auto rule = bvmul_rec_rule<node>(current);
		rules.push_back(rule);
		current = tau::get(tau::bf, bv_shr_by_one<node>(tau::trim(current)));
	}
	// Then we build a main term to compute the actual predicate.
	auto multiplicant = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto product = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto call = make_bvmul_call<node>(multiplicant, multiplier, product);
	auto rr = make_rr<node>(rules, call);
	auto body = apply_rr_to_formula(rr);
	auto rule = make_rule<node>(call, body);

#ifdef DEBUG
	LOG_TRACE << "bvmul_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvmul_rule/head: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvmul_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG

	cache[multiplier] = rule;
	return cache[multiplier];
}

template<NodeType node>
tref bvmul(tref multiplicand, tref multiplier, tref product) {
	using tau = tree<node>;

	if (!tau::get(tau::trim(multiplier)).is(tau::bf_f) && !is_bv_constant<node>(tau::trim(multiplier))) {
		DBG( LOG_DEBUG << "Only multiplication by constant is supported in predicate blasting."; )
		return nullptr;
	}
	auto rule = bvmul_rule<node>(multiplier);
	auto call = make_bvmul_call<node>(multiplicand, multiplier, product);
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
static tref make_bvdiv_call(tref dividend, tref divisor, tref quotient) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvdiv", { dividend, divisor, quotient })));
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
static rewriter::rule bvdiv_rule(tref divisor /* bv constant */) {
	using tau = tree<node>;

	static std::map<tref, rewriter::rule> cache;
	if (auto it = cache.find(divisor); it != cache.end()) return it->second;

	auto bitwidth = get_bv_type_bitwidth<node>(divisor);

	// We build the main term to compute the actual predicate.
	auto quotient = tau::build_variable(bv_type_id<node>(bitwidth));
	auto bf_quotient = tau::get(tau::bf, quotient);
	auto remainder = tau::build_variable(bv_type_id<node>(bitwidth));
	auto bf_remainder = tau::get(tau::bf, remainder);
	auto dividend = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto bf_dividend = tau::get(tau::bf, dividend);
	auto exact = tau::build_variable(bv_type_id<node>(bitwidth));
	auto bf_exact = tau::get(tau::bf, exact);
	auto call = make_bvdiv_call<node>(dividend, divisor, bf_quotient);
	auto body = tau::build_wff_ex(exact,
		tau::build_wff_ex(quotient,
			tau::build_wff_ex(remainder,
				tau::build_wff_and(
					bvsub<node>(bf_dividend, bf_remainder, bf_exact),
					tau::build_wff_and(
						bvmul<node>(bf_quotient, divisor, bf_exact),
						bvlt<node>(bf_remainder, divisor))))));
	auto rule = make_rule<node>(call, body);

#ifdef DEBUG
	LOG_TRACE << "bvdiv_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvdiv_rule/head: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvdiv_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG

	return cache[divisor] = rule;
}

template<NodeType node>
tref bvdiv(tref dividend, tref divisor, tref quotient) {
	using tau = tree<node>;

	if (!tau::get(tau::trim(divisor)).is(tau::bf_f) && !is_bv_constant<node>(tau::trim(divisor))) {
		DBG( LOG_DEBUG << "Only division by constant is supported in predicate blasting."; )
		return nullptr;
	}
	auto rule = bvdiv_rule<node>(divisor);
	auto call = make_bvdiv_call<node>(dividend, divisor, quotient);
	auto rr = make_rr<node>({ rule }, call);
	return apply_rr_to_formula(rr);
}

/**
 * @brief Creates a call to the bitvector modulo recurrence.
 * @tparam node Node type
 * @param dividend Dividend
 * @param divisor Divisor
 * @param remainder Result variable
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvmod_call(tref dividend, tref divisor, tref remainder) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvmod", { dividend, divisor, remainder })));
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
static rewriter::rule bvmod_rule(tref divisor /* bv copnstant */) {
	using tau = tree<node>;

	static std::map<tref, rewriter::rule> cache;
	if (auto it = cache.find(divisor); it != cache.end()) return it->second;

	auto bitwidth = get_bv_type_bitwidth<node>(divisor);

	// We build the main term to compute the actual predicate.
	auto quotient_var = tau::build_variable(bv_type_id<node>(bitwidth));
	auto quotient = tau::get(tau::bf, quotient_var);
	auto remainder_var = tau::build_variable(bv_type_id<node>(bitwidth));
	auto remainder = tau::get(tau::bf, remainder_var);
	auto dividend_var = tau::build_variable(bv_type_id<node>(bitwidth));
	auto dividend = tau::get(tau::bf, dividend_var);
	auto exact_var = tau::build_variable(bv_type_id<node>(bitwidth));
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

	return cache[divisor] = rule;
}

template<NodeType node>
tref bvmod(tref dividend, tref divisor, tref remainder) {
	using tau = tree<node>;

	if (!tau::get(tau::trim(divisor)).is(tau::bf_f) && !is_bv_constant<node>(tau::trim(divisor))) {
		DBG( LOG_DEBUG << "Only modulo by constant is supported in predicate blasting."; )
		return nullptr;
	}
	auto rule = bvmod_rule<node>(divisor);
	auto call = make_bvmod_call<node>(dividend, divisor, remainder);
	auto rr = make_rr<node>({ rule }, call);
	return apply_rr_to_formula(rr);
}

/**
 * @brief Creates a call to the bitvector Euclidean division recurrence.
 * @tparam node Node type
 * @param dividend Dividend
 * @param divisor Divisor
 * @param quotient Quotient result variable
 * @param remainder Remainder result variable
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bved_call(tref dividend, tref divisor, tref quotient, tref remainder) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bved", { dividend, divisor, quotient, remainder })));
}

/**
 * @brief Returns the rule for bitvector Euclidean division (caching by divisor).
 *
 * Exposes both quotient and remainder as outputs. Only the intermediate
 * value `exact = dividend - remainder` is existentially quantified.
 *
 * @tparam node Node type
 * @param divisor Divisor (bv constant)
 * @return The constructed rule
 */
template<NodeType node>
static rewriter::rule bved_rule(tref divisor /* bv constant */) {
	using tau = tree<node>;

	static std::map<tref, rewriter::rule> cache;
	if (auto it = cache.find(divisor); it != cache.end()) return it->second;

	auto bitwidth = get_bv_type_bitwidth<node>(divisor);

	auto dividend_var  = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto quotient_var  = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto remainder_var = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto exact_var     = tau::build_variable(bv_type_id<node>(bitwidth));
	auto bf_exact      = tau::get(tau::bf, exact_var);

	// head: _bved(dividend, divisor, quotient, remainder)
	auto call = make_bved_call<node>(dividend_var, divisor, quotient_var, remainder_var);

	// body: ∃ exact:
	//   bvsub(dividend, remainder, exact)   -- exact = dividend - remainder
	//   && bvmul(quotient, divisor, exact)  -- quotient * divisor = exact
	//   && bvlt(remainder, divisor)         -- remainder < divisor
	auto body = tau::build_wff_ex(exact_var,
		tau::build_wff_and(
			bvsub<node>(dividend_var, remainder_var, bf_exact),
			tau::build_wff_and(
				bvmul<node>(quotient_var, divisor, bf_exact),
				bvlt<node>(remainder_var, divisor))));
	auto rule = make_rule<node>(call, body);

#ifdef DEBUG
	LOG_TRACE << "bved_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bved_rule/head: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bved_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG

	return cache[divisor] = rule;
}

template<NodeType node>
tref bved(tref dividend, tref divisor, tref quotient, tref remainder) {
	using tau = tree<node>;

	if (!tau::get(tau::trim(divisor)).is(tau::bf_f) && !is_bv_constant<node>(tau::trim(divisor))) {
		DBG( LOG_DEBUG << "Only Euclidean division by constant is supported in predicate blasting."; )
		return nullptr;
	}
	auto rule = bved_rule<node>(divisor);
	auto call = make_bved_call<node>(dividend, divisor, quotient, remainder);
	auto rr = make_rr<node>({ rule }, call);
	return apply_rr_to_formula(rr);
}

} // namespace idni::tau_lang