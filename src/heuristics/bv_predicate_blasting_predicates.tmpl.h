// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <bit>

namespace idni::tau_lang {

// Custom factory methods for creating rules and rr's.
/**
 * @brief Creates a rewriter rule from a head and body term.
 * @tparam node Node type
 * @param head The head of the rule
 * @param body The body of the rule
 * @return The constructed rewriter rule
 */
template<NodeType node>
inline static rewriter::rule make_rule(tref head, tref body) {
	using tau = tree<node>;

	return rewriter::rule(tau::geth(head), tau::geth(body));
}

/**
 * @brief Creates a recurrence relation (rr) from rules and a main term.
 * @tparam node Node type
 * @param rules The set of rules
 * @param main The main term
 * @return The constructed recurrence relation
 */
template<NodeType node>
static rr<node> make_rr(const rewriter::rules& rules, tref main) {
	using tau = tree<node>;

	return rr<node>{rules, tau::geth(main)};
}

// Helper functions to create calls to the bitblasting recurrence functions and relations.

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
static tref make_bvadd_call_with_index(tref left, tref right, tref result, size_t index) {
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
 * @brief Creates a call to the indexed bitvector subtraction recurrence.
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @param result Result variable
 * @param index Index for the recurrence
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvsub_call_with_index(tref left, tref right, tref result, size_t index) {
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
 * @brief Creates a call to the bitvector shift-left-by-one recurrence.
 * @tparam node Node type
 * @param operand Operand to shift
 * @param shifted Result variable
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvshl_by_one_call(tref operand,  tref shifted) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvshl_by_one", { operand, shifted })));
}

/**
 * @brief Creates a call to the bitvector right-shift-by-one recurrence.
 * @tparam node Node type
 * @param operand Operand to shift
 * @param shifted Result variable
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvrhl_by_one_call(tref operand, tref shifted) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvrhl_by_one", { operand, shifted })));
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
 * @brief Creates a call to the bitvector shift-left recurrence (constant shift).
 * @tparam node Node type
 * @param left Operand to shift
 * @param right Shift amount (constant)
 * @param result Result variable
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvshl_call(tref left, tref right /* bv constant */, tref result) {
	using tau = tree<node>;

	DBG( assert(is_bv_constant<node>(right)); )

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvshl", { left, right, result })));
}

/**
 * @brief Creates a call to the bitvector right-shift recurrence (constant shift).
 * @tparam node Node type
 * @param left Operand to shift
 * @param right Shift amount (constant)
 * @param result Result variable
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvrhl_call(tref left, tref right /* bv copnstant */, tref result) {
	using tau = tree<node>;

	DBG( assert(is_bv_constant<node>(right)); )

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvrhl", { left, right, result })));
}

/**
 * @brief Creates a call to extract a specific bit from an operand (by index).
 * @tparam node Node type
 * @param operand Operand
 * @param bit Bit index
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bit_call(tref operand, size_t bit) {
	using tau = tree<node>;

	return tau::get(tau::bf, tau::get(tau::bf_ref, tau::build_rr_ref("_bit", bit, { operand })));
}

/**
 * @brief Creates a call to extract a specific bit from an operand (by tref).
 * @tparam node Node type
 * @param operand Operand
 * @param bit Bit tref
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bit_call(tref operand, tref bit) {
	using tau = tree<node>;

	return tau::get(tau::bf, tau::get(tau::bf_ref, tau::build_rr_ref("_bit", { bit }, { operand })));
}

/**
 * @brief Creates a call to check if a specific bit is zero (by index).
 * @tparam node Node type
 * @param operand Operand
 * @param bit Bit index
 * @return The constructed call term
 */
template<NodeType node>
static tref make_is_bit_zero_call(tref operand, size_t bit) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_is_bit_zero", bit, { operand })));
}

/**
 * @brief Creates a call to check if a specific bit is zero (by tref).
 * @tparam node Node type
 * @param operand Operand
 * @param bit Bit tref
 * @return The constructed call term
 */
template<NodeType node>
static tref make_is_bit_zero_call(tref operand, tref bit) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_is_bit_zero", { bit }, { operand })));
}

/**
 * @brief Creates a call to check if a specific bit is one (by index).
 * @tparam node Node type
 * @param operand Operand
 * @param bit Bit index
 * @return The constructed call term
 */
template<NodeType node>
static tref make_is_bit_one_call(tref operand, size_t bit) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_is_bit_one", bit, { operand })));
}

/**
 * @brief Creates a call to check if a specific bit is one (by tref).
 * @tparam node Node type
 * @param operand Operand
 * @param bit Bit tref
 * @return The constructed call term
 */
template<NodeType node>
static tref make_is_bit_one_call(tref operand, tref bit) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_is_bit_one", { bit }, { operand })));
}

/**
 * @brief Creates a call to the bitvector less-than recurrence (by index).
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @param offset Offset (index)
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvlt_call(tref left, tref right, size_t offset) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_bvlt", offset, { left, right })));
}

/**
 * @brief Creates a call to the bitvector less-than recurrence (by tref offset).
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @param offset Offset (tref)
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvlt_call(tref left, tref right, tref offset) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_bvlt", {offset}, { left, right })));
}

/**
 * @brief Creates a call to the bitvector greater-than recurrence (by index).
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @param offset Offset (index)
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvgt_call(tref left, tref right, size_t offset) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_bvgt", offset, { left, right })));
}

/**
 * @brief Creates a call to the bitvector greater-than recurrence (by tref offset).
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @param offset Offset (tref)
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvgt_call(tref left, tref right, tref offset) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_bvgt", {offset}, { left, right })));
}

// Factory methods returning the rule(s) needed in certain predicate blasting
// cases (bvadd,...).

// addition(x, y) = addition(x ^ y, (x & y) << 1, m) mod m;
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
	auto base_header = make_bvadd_call_with_index<node>(x, y, z, 0);
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
	auto base_header = make_bvsub_call_with_index<node>(x, y, z, 0);
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
 * @brief Returns the rule for bitvector shift-left-by-one recurrence.
 *
 * The rightmost bit is zero, the rest are shifted.
 *
 * @tparam node Node type
 * @param bitwidth Bitwidth of the operand
 * @return The constructed rule
 */
template<NodeType node>
static rewriter::rule bvshl_by_one_rule(size_t bitwidth) {
	using tau = tree<node>;

	auto var = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto result = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto header = tau::build_rr_ref("_bvshl_by_one", {}, { var, result });
	// the rightest bit is zero, the rest of the bits are the same as the original variable shifted by one
	auto body = tau::build_bf_eq(
		make_bit_call<node>(result, 0),
		tau::_0(bv_type_id<node>(bitwidth)));
	for (size_t i = 0; i < bitwidth - 1; ++i) {
		body = tau::build_wff_and(
			body,
			tau::build_bf_eq(
				make_bit_call<node>(var, i),
				make_bit_call<node>(result, i + 1)));
	}
	auto rule = make_rule<node>(header, body);
#ifdef DEBUG
	LOG_TRACE << "bvshl_by_one_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvshl_by_one_rule/header: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvshl_by_one_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG
	return rule;
}

/**
 * @brief Returns the rule for bitvector right-shift-by-one recurrence.
 *
 * The leftmost bit is zero, the rest are shifted.
 *
 * @tparam node Node type
 * @param bitwidth Bitwidth of the operand
 * @return The constructed rule
 */
template<NodeType node>
static rewriter::rule bvrhl_by_one_rule(size_t bitwidth) {
	using tau = tree<node>;

	auto var = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto result = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto header = tau::build_rr_ref("_bvrhl_by_one", {}, { var, result });
	// the left most bit is zero, the rest of the bits are the same as the original variable shifted by one
	auto body = tau::build_bf_eq(
		make_bit_call<node>(result, bitwidth - 1),
		tau::_0(bv_type_id<node>(bitwidth)));
	for (size_t i = 1; i < bitwidth; ++i) {
		body = tau::build_wff_and(
			body,
			tau::build_bf_eq(
				make_bit_call<node>(var, i),
				make_bit_call<node>(result, i - 1)));
	}
	auto rule = make_rule<node>(header, body);
#ifdef DEBUG
	LOG_TRACE << "bvrhl_by_one_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvrhl_by_one_rule/header: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvrhl_by_one_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG
	return rule;
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
 * @brief Returns the rules for bitvector less-than recurrence.
 *
 * Generates the base and general case rules for bitvector less-than.
 *
 * @tparam node Node type
 * @param bitwidth Bitwidth of the operands
 * @return The set of rules for less-than
 */
template<NodeType node>
static rewriter::rules bvlt_rules(size_t bitwidth) {
	using tau = tree<node>;

	auto x = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto y = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));

	rewriter::rules rules;

	// base case: bvlt[0](x, y) = F;
	auto base_header = tau::build_rr_ref("_bvlt", { 0 }, { x, y });
	auto base_body = tau::_F();
	rules.push_back(make_rule<node>(base_header, base_body));
	// general case: bvlt[n](x, y) = (bit[n](x) = 0) && (bit[n](y) = 1) || ((bit[n](x) = bit[n](y)) && bvlt[n-1](x, y));
	auto n = tau::build_variable(untyped_type_id<node>());
	auto n_minus_1 = tau::build_ref_shift_offset(n, 1);
	auto general_header = make_bvlt_call<node>(x, y, bitwidth);
	auto general_body = tau::build_wff_or(
		tau::build_wff_and(
			make_bvlt_call<node>(x, y, n_minus_1),
			tau::build_bf_eq(
				make_bit_call<node>(x, n),
				make_bit_call<node>(y, n))),
		tau::build_wff_and(
			make_is_bit_zero_call<node>(x, n),
			make_is_bit_one_call<node>(y, n)));
	rules.push_back(make_rule<node>(general_header, general_body));
#ifdef DEBUG
	for (const auto& rule : rules) {
		LOG_TRACE << "bvlt_rules: " << LOG_RULE(rule) << "\n";
		LOG_TRACE << "bvlt_rules/header: " << LOG_FM(rule.first->get()) << "\n";
		LOG_TRACE << "bvlt_rules/body: " << LOG_FM(rule.second->get()) << "\n";
	}
#endif // DEBUG
	return rules;
}

/**
 * @brief Returns the rule for bitvector less-than recurrence.
 *
 * Applies the rules to construct the main predicate.
 *
 * @tparam node Node type
 * @param bitwidth Bitwidth of the operands
 * @return The constructed rule
 */
template<NodeType node>
static rewriter::rule bvlt_rule(size_t bitwidth) {
	using tau = tree<node>;

	auto x = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto y = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));

	auto main = make_bvlt_call<node>(x, y, bitwidth);
	auto rules = bvlt_rules<node>(bitwidth);
	auto temp = make_rr<node>(rules, main);
	auto body = apply_rr_to_formula(temp);
	auto rule = make_rule<node>(main, body);
	if (!body) {
		LOG_ERROR << "Failed to apply rules for bvlt predicate.";
		return rule;
	}
#ifdef DEBUG
	LOG_TRACE << "bvlt_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvlt_rule/header: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvlt_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG
	return rule;
}

/**
 * @brief Returns the rules for bitvector greater-than recurrence.
 *
 * Generates the base and general case rules for bitvector greater-than.
 *
 * @tparam node Node type
 * @param bitwidth Bitwidth of the operands
 * @return The set of rules for greater-than
 */
template<NodeType node>
static rewriter::rules bvgt_rules(size_t bitwidth) {
	using tau = tree<node>;

	auto x = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto y = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));

	rewriter::rules rules;

	// base case: bvgt[0](x, y) = F;
	auto base_header = make_bvgt_call<node>(x, y, bitwidth);
	auto base_body = tau::_F();
	rules.push_back(make_rule<node>(base_header, base_body));
	// general case: bvgt[n](x, y) = (bit[n-1](x) = 1) && (bit[n-1](y) = 0) || ((bit[n-1](x) = bit[n-1](y)) && bvgt[n-1](x, y));
	auto n = tau::build_variable(untyped_type_id<node>());
	auto n_minus_1 = tau::build_ref_shift_offset(n, 1);
	auto general_header = make_bvgt_call<node>(x, y, n);
	auto general_body = tau::build_wff_or(
		tau::build_wff_and(
			make_bvgt_call<node>(x, y, n_minus_1),
			tau::build_bf_eq(
				make_bit_call<node>(x, n),
				make_bit_call<node>(y, n))),
		tau::build_wff_and(
			make_is_bit_one_call<node>(x, n),
			make_is_bit_zero_call<node>(y, n)));
	rules.push_back(make_rule<node>(general_header, general_body));
#ifdef DEBUG
	for (const auto& rule : rules) {
		LOG_TRACE << "bvgt_rules: " << LOG_RULE(rule) << "\n";
		LOG_TRACE << "bvgt_rules/header: " << LOG_FM(rule.first->get()) << "\n";
		LOG_TRACE << "bvgt_rules/body: " << LOG_FM(rule.second->get()) << "\n";
	}
#endif // DEBUG
	return rules;
}

/**
 * @brief Returns the rule for bitvector greater-than recurrence.
 *
 * Applies the rules to construct the main predicate.
 *
 * @tparam node Node type
 * @param bitwidth Bitwidth of the operands
 * @return The constructed rule
 */
template<NodeType node>
static rewriter::rule bvgt_rule(size_t bitwidth) {
	using tau = tree<node>;

	auto x = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto y = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));

	auto main = make_bvgt_call<node>(x, y, bitwidth);
	auto rules = bvgt_rules<node>(bitwidth);
	auto temp = make_rr<node>(rules, main);
	auto body = apply_rr_to_formula(temp);
	auto rule = make_rule<node>(main, body);
	if (!body) {
		LOG_ERROR << "Failed to apply rules for bvgt predicate.";
		return rule;
	}
#ifdef DEBUG
	LOG_TRACE << "bvgt_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvgt_rule/header: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvgt_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG
	return rule;
}

/**
 * @brief Returns the rule for extracting a bit from a bitvector.
 *
 * @tparam node Node type
 * @param bit Bit index
 * @param bitwidth Bitwidth of the operand
 * @return The constructed rule
 */
template<NodeType node>
static rewriter::rule bit_rule(size_t bit, size_t bitwidth) {
	using tau = tree<node>;

	auto bit_offset = tau::get_num(bit);
	auto bit_cte =
		tau::get(tau::bf,
			tau::get_ba_constant(
				make_bitvector_value(1 <<bit, bitwidth), bv_type_id<node>(bitwidth)));
	auto var = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto head = tau::build_rr_ref("_bit", { bit_offset }, { var });
	auto body = tau::build_bf_and( var, bit_cte);
	auto rule = make_rule<node>(head, body);
#ifdef DEBUG
	LOG_TRACE << "bit_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bit_rule/header: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bit_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG
	return rule;
}

/**
 * @brief Returns the rule for checking if a bit is zero in a bitvector.
 *
 * @tparam node Node type
 * @param bit Bit index
 * @param bitwidth Bitwidth of the operand
 * @return The constructed rule
 */
template<NodeType node>
static rewriter::rule is_bit_zero_rule(size_t bit, size_t bitwidth) {
	using tau = tree<node>;

	auto bit_offset = tau::get_num(bit);
	auto bit_cte =
		tau::get(tau::bf,
			tau::get_ba_constant(
				make_bitvector_value(1 << bit, bitwidth), bv_type_id<node>(bitwidth)));
	auto var = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto head = tau::build_rr_ref("_is_bit_zero", { bit_offset }, { var });
	auto body =	tau::build_bf_eq_0(tau::build_bf_and(var, bit_cte));
	auto rule = make_rule<node>(head, body);
#ifdef DEBUG
	LOG_TRACE << "is_bit_zero_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "is_bit_zero_rule/header: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "is_bit_zero_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG
	return rule;
}

/**
 * @brief Returns the rule for checking if a bit is one in a bitvector.
 *
 * @tparam node Node type
 * @param bit Bit index
 * @param bitwidth Bitwidth of the operand
 * @return The constructed rule
 */
template<NodeType node>
static rewriter::rule is_bit_one_rule(size_t bit, size_t bitwidth) {
	using tau = tree<node>;

	auto bit_offset = tau::get_num(bit);
	auto bit_cte =
		tau::get(tau::bf,
			tau::get_ba_constant(
				make_bitvector_value(1 << bit, bitwidth), bv_type_id<node>(bitwidth)));
	auto var = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto head = tau::build_rr_ref("_is_bit_one", { bit_offset }, { var });
	auto body =	tau::build_bf_eq(tau::build_bf_and(var, bit_cte), bit_cte);
	auto rule = make_rule<node>(head, body);
#ifdef DEBUG
	LOG_TRACE << "is_bit_one_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "is_bit_one_rule/header: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "is_bit_one_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG
	return rule;
}

// Factory methods to create predicates from their arguments and the rules defined above.
// The rules are cached so that they are only computed once per bitwidth (or bv constants
// argument).

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
	auto head = make_bvadd_call_with_index<node>(left, right, result, bitwidth);
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
	auto head = make_bvsub_call_with_index<node>(left, right, result, bitwidth);
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
	// If the rule is already computed for the given bitwidth and right operand, we return it from the cache
	if (cache.find(bitwidth) != cache.end()) return cache[bitwidth][divisor];
	// Otherwise, we compute the rule, store it in the cache and return it.
	// First we collect all the rules needed for computing the euclidean
	// division: addition, multiplication, less then,...
	rewriter::rules rs;
	rs.insert(rs.end(), bvadd_rules<node>(bitwidth).begin(), bvadd_rules<node>(bitwidth).end());
	//rs.push_back(bvmul_rule<node>(bitwidth).end());
	rs.insert(rs.end(), bvlt_rules<node>(bitwidth).begin(), bvlt_rules<node>(bitwidth).end());
	// Then we build the main term to compute the actual predicate.
	auto quotient = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto remainder = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto dividend = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto exact = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto main = tau::build_wff_ex(remainder,
		tau::build_wff_ex(exact,
			tau::build_wff_and(
				make_bvsub_call<node>(dividend, remainder, exact),
				tau::build_wff_and(
					make_bvadd_call<node>(quotient, divisor, exact),
					make_bvlt_call<node>(remainder, divisor, bitwidth)))));
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
	rewriter::rules rs;
	rs.insert(rs.end(), bvadd_rules<node>(bitwidth).begin(), bvadd_rules<node>(bitwidth).end());
	//rs.push_back(bvmul_rule<node>(bitwidth).end());
	rs.insert(rs.end(), bvlt_rules<node>(bitwidth));
	// Otherwise, we compute the rule, store it in the cache and return it..begin(), bvlt_rules<node>(bitwidth).end());
	// Then we build the main term to compute the actual predicate.
	auto quotient = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto remainder = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto dividend = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto exact = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto main = tau::build_wff_ex(quotient,
		tau::build_wff_ex(exact,
			tau::build_wff_and(
				make_bvsub_call<node>(dividend, remainder, exact),
				tau::build_wff_and(
					make_bvadd_call<node>(quotient, divisor, exact),
					make_bvlt_call<node>(remainder, divisor, bitwidth)))));
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
 * @brief Returns the rule for bitvector shift-left (caching by bitwidth and shift amount).
 *
 * @tparam node Node type
 * @param shift Shift amount (constant)
 * @param bitwidth Bitwidth of the operands
 * @return The constructed rule
 */
template<NodeType node>
static rewriter::rule bvshl_rule(tref shift /* bv constant */, size_t bitwidth) {
	using tau = tree<node>;

	static std::map<size_t, std::map<tref, rewriter::rule>> cache;
	// If the rule is already computed for the given bitwidth and right operand, we return it from the cache
	if (cache.find(bitwidth) != cache.end()) return cache[bitwidth][shift];
	// Get bv constant value
	auto shift_value = get_bv_constant_value<node>(shift);
	if (!shift_value) return make_rule<node>(nullptr, nullptr);
	// If the shift is greater or equal to the bitwidth, the result is always zero
	auto var = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto shifted = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto head = make_bvshl_call<node>(var, shift, shifted);
	if (shift_value.value() >= bitwidth) {
		auto zero = tau::get(tau::bf, build_bv_zero<node>(bitwidth));
		auto body = tau::build_bf_eq(shifted, zero);
		auto rule = make_rule<node>(head, body);
#ifdef DEBUG
		LOG_TRACE << "bvshl_rule (zero case): " << LOG_RULE(rule) << "\n";
		LOG_TRACE << "bvshl_rule/header: " << LOG_FM(rule.first->get()) << "\n";
		LOG_TRACE << "bvshl_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG
		cache[bitwidth][shift] = rule;
		return cache[bitwidth][shift];
	}
	// Otherwise, we compute the rule, store it in the cache and return it.
	tref body = nullptr;
	for (size_t i = 0; i < bitwidth; ++i) {
		if (i + shift_value.value() > bitwidth) {
			auto shifted_bit = tau::get(tau::bf, tau::build_rr_ref("_bit", { i }, { shifted }));
			auto zero = tau::get(tau::bf, build_bv_zero<node>(bitwidth));
			auto shift_eq = tau::build_bf_eq(shifted_bit, zero);
			body = body ? tau::build_wff_and(body, shift_eq) : shift_eq;
			continue;
		}
		auto bit = tau::get(tau::bf, tau::build_rr_ref("_bit", i, { var }));
		auto shifted_bit = tau::get(tau::bf, tau::build_rr_ref("_bit", i + shift_value.value(), { shifted }));
		auto shift_eq = tau::build_bf_eq(shifted_bit, bit);
		body = body ? tau::build_wff_and(body, shift_eq) : shift_eq;
	}
	auto rule = make_rule<node>(head, body);
#ifdef DEBUG
	LOG_TRACE << "bvshl_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvshl_rule/header: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvshl_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG
	cache[bitwidth][shift] = rule;
	return cache[bitwidth][shift];
}

/**
 * @brief Returns the rule for bitvector right-shift (caching by bitwidth and shift amount).
 *
 * @tparam node Node type
 * @param shift Shift amount (constant)
 * @param bitwidth Bitwidth of the operands
 * @return The constructed rule
 */
template<NodeType node>
static rewriter::rule bvrhl_rule(tref shift /* bv constant */, size_t bitwidth) {
	using tau = tree<node>;

	static std::map<size_t, std::map<tref, rewriter::rule>> cache;
	// If the rule is already computed for the given bitwidth and right operand, we return it from the cache
	if (cache.find(bitwidth) != cache.end()) return cache[bitwidth][shift];
	// Unsupported operation for now
	// Get bv constant value
	auto shift_value = get_bv_constant_value<node>(shift);
	if (!shift_value) return make_rule<node>(nullptr, nullptr);
	// If the shift is greater or equal to the bitwidth, the result is always zero
	auto var = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto shifted = tau::tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto head = make_bvshl_call<node>(var, shift, shifted);
	if (shift_value.value() >= bitwidth) {
		auto zero = tau::get(tau::bf, build_bv_zero<node>(bitwidth));
		auto body = tau::build_bf_eq(shifted, zero);
		auto rule = make_rule<node>(head, body);
#ifdef DEBUG
		LOG_TRACE << "bvrhl_rule (zero case): " << LOG_RULE(rule) << "\n";
		LOG_TRACE << "bvrhl_rule/header: " << LOG_FM(rule.first->get()) << "\n";
		LOG_TRACE << "bvrhl_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG
		cache[bitwidth][shift] = rule;
		return cache[bitwidth][shift];
	}
	// Otherwise, we compute the rule, store it in the cache and return it.
	tref body = nullptr;
	for (size_t i = 0; i < bitwidth; ++i) {
		if (i - shift_value.value() < 0) {
			auto shifted_bit = tau::get(tau::bf, tau::build_rr_ref("_bit", i, { shifted }));
			auto zero = tau::get(tau::bf, build_bv_zero<node>(bitwidth));
			auto shift_eq = tau::build_bf_eq(shifted_bit, zero);
			body = body ? tau::build_wff_and(body, shift_eq) : shift_eq;
			continue;
		}
		auto bit = tau::get(tau::bf, tau::build_rr_ref("_bit", i, { var }));
		auto shifted_bit = tau::get(tau::bf, tau::build_rr_ref("_bit", i - shift_value.value(), { shifted }));
		auto bit_eq = tau::build_bf_eq(bit, shifted_bit);
		body = body ? tau::build_wff_and(body, bit_eq) : bit_eq;
	}
	auto rule = make_rule<node>(head, body);
#ifdef DEBUG
	LOG_TRACE << "bvrhl_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvrhl_rule/header: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvrhl_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG
	cache[bitwidth][shift] = rule;
	return cache[bitwidth][shift];
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

/**
 * @brief Applies the bitvector right-shift-by-one rule to the given operands.
 *
 * @tparam node Node type
 * @param left Operand to shift
 * @param shifted Result variable
 * @return The resulting predicate term
 */
template<NodeType node>
tref bvrhl_one(tref left, tref shifted) {
	auto bitwidth = get_bv_type_bitwidth<node>(left);
	auto predicate = bvrhl_by_one_rule<node>(bitwidth);
	auto call = make_bvrhl_by_one_call<node>(left, shifted);
	return nso_rr_apply<node>(predicate, call);
}

/**
 * @brief Applies the bitvector shift-left-by-one rule to the given operands.
 *
 * @tparam node Node type
 * @param left Operand to shift
 * @param shifted Result variable
 * @return The resulting predicate term
 */
template<NodeType node>
tref bvshl_one(tref left, tref shifted) {
	auto bitwidth = get_bv_type_bitwidth<node>(left);
	auto predicate = bvshl_by_one_rule<node>(bitwidth);
	auto call = make_bvshl_by_one_call<node>(left, shifted);
	return nso_rr_apply<node>(predicate, call);
}

/**
 * @brief Applies the bitvector right-shift rule to the given operands.
 *
 * @tparam node Node type
 * @param left Operand to shift
 * @param shift Shift amount (constant)
 * @param shifted Result variable
 * @return The resulting predicate term
 */
template<NodeType node>
tref bvrhl(tref left, tref shift, tref shifted) {
	auto bitwidth = get_bv_type_bitwidth<node>(shift);
	auto predicate = bvrhl_rule<node>(shift, bitwidth);
	auto call = make_bvrhl_call<node>(left, shift, shifted);
	return nso_rr_apply<node>(predicate, call);
}

/**
 * @brief Applies the bitvector shift-left rule to the given operands.
 *
 * @tparam node Node type
 * @param left Operand to shift
 * @param shift Shift amount (constant)
 * @param shifted Result variable
 * @return The resulting predicate term
 */
template<NodeType node>
tref bvshl(tref left, tref shift, tref shifted) {
	auto bitwidth = get_bv_type_bitwidth<node>(shift);
	auto predicate = bvshl_rule<node>(shift, bitwidth);
	auto call = make_bvshl_call<node>(left, shift, shifted);
	return nso_rr_apply<node>(predicate, call);
}

/**
 * @brief Blasts an equality predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic equality predicate
 * @return The resulting predicate term
 */
template<NodeType node>
tref eq_predicate(tref atomic) {
	// TODO (HIGH) add simplifications to avoid the top level variable if possible
	subtree_map<node, tref> changes;
	trefs vars;

	return bf_predicate_blasting<node>(atomic, changes, vars);
}

/**
 * @brief Blasts an inequality predicate over bitvectors.
 *
 * @tparam node Node type
 * @param n The atomic inequality predicate
 * @return The resulting predicate term
 */
template<NodeType node>
tref neq_predicate(tref n) {
	using tau = tree<node>;

	return tau::build_wff_neg(eq_predicate<node>(n));
}

/**
 * @brief Blasts a less-than predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic less-than predicate
 * @return The resulting predicate term, or nullptr on error
 */
template<NodeType node>
tref lt_predicate(tref atomic) {
	using tau = tree<node>;

	subtree_map<node, tref> changes;
	trefs vars;

	auto bitwidth = get_bv_type_bitwidth<node>(atomic);
	auto predicate = bvlt_rule<node>(bitwidth);
	auto blasted = bf_predicate_blasting<node>(atomic, changes, vars);
	if (!blasted) return nullptr;
	auto left = tau::get(blasted).child(0);
	auto right = tau::get(blasted).child(1);
	auto call = make_bvlt_call<node>(left, right, bitwidth);
	return nso_rr_apply<node>(predicate, call);
}

/**
 * @brief Blasts a greater-than predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic greater-than predicate
 * @return The resulting predicate term, or nullptr on error
 */
template<NodeType node>
tref gt_predicate(tref atomic) {
	using tau = tree<node>;

	subtree_map<node, tref> changes;
	trefs vars;

	auto bitwidth = get_bv_type_bitwidth<node>(atomic);
	auto predicate = bvgt_rule<node>(bitwidth);
	auto blasted = bf_predicate_blasting<node>(atomic, changes, vars);
	if (!blasted) return nullptr;
	auto left = tau::get(blasted).child(0);
	auto right = tau::get(blasted).child(1);
	auto call = make_bvgt_call<node>(left, right, bitwidth);
	return nso_rr_apply<node>(predicate, call);
}

/**
 * @brief Blasts a less-than-or-equal predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic less-than-or-equal predicate
 * @return The resulting predicate term
 */
template<NodeType node>
tref lteq_predicate(tref atomic) {
	using tau = tree<node>;

	return tau::build_wff_neg(gt_predicate<node>(atomic));
}

/**
 * @brief Blasts a greater-than-or-equal predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic greater-than-or-equal predicate
 * @return The resulting predicate term
 */
template<NodeType node>
tref gteq_predicate(tref atomic) {
	using tau = tree<node>;

	return tau::build_wff_neg(lt_predicate<node>(atomic));
}

/**
 * @brief Blasts a not-less-than predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic not-less-than predicate
 * @return The resulting predicate term
 */
template<NodeType node>
tref nlt_predicate(tref atomic) {
	return gteq_predicate<node>(atomic);
}

/**
 * @brief Blasts a not-greater-than predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic not-greater-than predicate
 * @return The resulting predicate term
 */
template<NodeType node>
tref ngt_predicate(tref atomic) {
	return lteq_predicate<node>(atomic);
}

/**
 * @brief Blasts a not-less-than-or-equal predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic not-less-than-or-equal predicate
 * @return The resulting predicate term
 */
template<NodeType node>
tref nlteq_predicate(tref atomic) {
	return gt_predicate<node>(atomic);
}

/**
 * @brief Blasts a not-greater-than-or-equal predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic not-greater-than-or-equal predicate
 * @return The resulting predicate term
 */
template<NodeType node>
tref ngteq_predicate(tref atomic) {
	return lt_predicate<node>(atomic);
}

} // namespace idni::tau_lang