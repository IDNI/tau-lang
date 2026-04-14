// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <bit>

namespace idni::tau_lang {

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
static tref make_bit_rr_call(tref operand, tref bit) {
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

} // namespace idni::tau_lang