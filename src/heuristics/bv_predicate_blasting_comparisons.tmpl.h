// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <bit>

namespace idni::tau_lang {

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
				make_bit_rr_call<node>(x, n),
				make_bit_rr_call<node>(y, n))),
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
				make_bit_rr_call<node>(x, n),
				make_bit_rr_call<node>(y, n))),
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

} // namespace idni::tau_lang