// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <bit>

namespace idni::tau_lang {

//
//
// _bvlt is a predicate that represents the less-than relation between two
// bitvectors of a given bitwidth. It is defined recursively based on the bitwise
// representation of the operands.
//
// The key idea is to use the following recurrence predicates:
// - _bvlt[0](x, y) = F; (base case: no bits means x is not less than y)
// - _bvlt[n](x, y) = ((bit[n-1](x) = 0) && (bit[n-1](y) = 1))
//		 || ((bit[n-1](x) = bit[n-1](y)) && _bvlt[n-1](x, y)); (general case: compare the most significant bit and recurse on the rest)
//
//

/**
 * @brief Creates a call to the bitvector less-than recurrence (by tref offset).
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @param offset Offset (tref)
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvlt_call_from_offset(tref left, tref right, tref offset) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_bvlt", { offset }, { left, right })));
}

/**
 * @brief Creates a call to the bitvector less-than recurrence (by index).
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @param index Bit index
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvlt_call_from_index(tref left, tref right, size_t index) {
	using tau = tree<node>;

	auto offset = tau::get_num(index);
	return make_bvlt_call_from_offset<node>(left, right, offset);
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

	static std::map<size_t, rewriter::rules> cache;
	if (auto it = cache.find(bitwidth); it != cache.end()) {
		return it->second;
	}

	auto left = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto right = tau::build_bf_variable(bv_type_id<node>(bitwidth));

	rewriter::rules rules;

	// base case: bvlt[0](x, y) = T;
	auto base_header = make_bvlt_call_from_index<node>(left, right, 0);
	auto base_body = tau::build_wff_and(
			make_is_bit_zero_call_from_index<node>(left, 0),
			make_is_bit_one_call_from_index<node>(right, 0));
	rules.push_back(make_rule<node>(base_header, base_body));
	// general case: bvlt[n](x, y) = ((bit[n](x) = 0) && (bit[n](y) = 1))
	//	 || ((bit[n](x) = bit[n](y)) && bvlt[n-1](x, y));
	auto n = tau::build_variable(untyped_type_id<node>());
	auto n_minus_1 = tau::build_shift(n, 1);
	auto general_header = make_bvlt_call_from_offset<node>(left, right, n);
	auto general_body = tau::build_wff_or(
		tau::build_wff_and(
			make_is_bit_zero_call_from_offset<node>(left, n),
			make_is_bit_one_call_from_offset<node>(right, n)),
		tau::build_wff_and(
			make_bvlt_call_from_offset<node>(left, right, n_minus_1),
			tau::build_bf_eq(
				make_bit_call_from_offset<node>(left, n),
				make_bit_call_from_offset<node>(right, n))));

	rules.push_back(make_rule<node>(general_header, general_body));

#ifdef DEBUG
	for (const auto& rule : rules) {
		LOG_TRACE << "bvlt_rules: " << LOG_RULE(rule) << "\n";
		LOG_TRACE << "bvlt_rules/header: " << LOG_FM(rule.first->get()) << "\n";
		LOG_TRACE << "bvlt_rules/body: " << LOG_FM(rule.second->get()) << "\n";
	}
#endif // DEBUG

	cache[bitwidth] = rules;
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

	static std::map<size_t, rewriter::rule> cache;
	if (auto it = cache.find(bitwidth); it != cache.end()) {
		return it->second;
	}

	auto left = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto right = tau::build_bf_variable(bv_type_id<node>(bitwidth));

	auto call = make_bvlt_call_from_index<node>(left, right, bitwidth - 1);
	auto rules = bvlt_rules<node>(bitwidth);
	auto bits = bit_rules<node>(bitwidth);
	auto bit_zeros = is_bit_zero_rules<node>(bitwidth);
	auto bit_ones = is_bit_one_rules<node>(bitwidth);
	rules.insert(rules.end(), bits.begin(), bits.end());
	rules.insert(rules.end(), bit_zeros.begin(), bit_zeros.end());
	rules.insert(rules.end(), bit_ones.begin(), bit_ones.end());
	auto rr = make_rr<node>(rules, call);
	auto body = apply_rr_to_formula(rr);
	auto rule = make_rule<node>(call, body);

#ifdef DEBUG
	LOG_TRACE << "bvlt_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvlt_rule/header: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvlt_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG

	cache[bitwidth] = rule;
	return rule;
}

template<NodeType node>
tref bvlt(tref left, tref right) {
	auto bitwidth = get_bv_type_bitwidth<node>(left);
	auto rule = bvlt_rule<node>(bitwidth);
	auto call = make_bvlt_call_from_index<node>(left, right, bitwidth-1);
	auto rr = make_rr<node>({ rule }, call);
	return apply_rr_to_formula(rr);
}

//
//
// _bvgt is a predicate that represents the greater-than relation between two
// bitvectors of a given bitwidth. It is defined recursively based on the bitwise
// representation of the operands.
// The key idea is to use the following recurrence predicates:
// - _bvgt[0](x, y) = F; (base case: no bits means x is not greater than y)
// - _bvgt[n](x, y) = ((bit[n-1](x) = 1) && (bit[n-1](y) = 0))
//		|| ((bit[n-1](x) = bit[n-1](y)) && _bvgt[n-1](x, y)); (general case: compare the most significant bit and recurse on the rest)
//
//

/**
 * @brief Creates a call to the bitvector greater-than recurrence (by tref offset).
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @param offset Offset (tref)
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvgt_call_from_offset(tref left, tref right, tref offset) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_bvgt", {offset}, { left, right })));
}

/**
 * @brief Creates a call to the bitvector greater-than recurrence (by index).
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @param index Index of the bit to compare
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvgt_call_from_index(tref left, tref right, int_t index) {
	using tau = tree<node>;

	auto offset = tau::get_num(index);
	return make_bvgt_call_from_offset<node>(left, right, offset);
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

	static std::map<size_t, rewriter::rules> cache;
	if (auto it = cache.find(bitwidth); it != cache.end()) {
		return it->second;
	}

	auto left = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto right = tau::build_bf_variable(bv_type_id<node>(bitwidth));

	rewriter::rules rules;

	// base case: bvgt[0](x, y) = T;
	auto base_header = make_bvgt_call_from_index<node>(left, right, bitwidth);
	auto base_body = tau::build_wff_and(
			make_is_bit_one_call_from_index<node>(left, 0),
			make_is_bit_zero_call_from_index<node>(right, 0));
	rules.push_back(make_rule<node>(base_header, base_body));
	// general case: bvgt[n](x, y) = ((bit[n-1](x) = 1) && (bit[n-1](y) = 0))
	//		|| ((bit[n-1](x) = bit[n-1](y)) && bvgt[n-1](x, y));
	auto n = tau::build_variable(untyped_type_id<node>());
	auto n_minus_1 = tau::build_shift(n, 1);
	auto general_header = make_bvgt_call_from_offset<node>(left, right, n);
	auto general_body = tau::build_wff_or(
		tau::build_wff_and(
			make_is_bit_one_call_from_offset<node>(left, n),
			make_is_bit_zero_call_from_offset<node>(right, n)),
		tau::build_wff_and(
			make_bvgt_call_from_offset<node>(left, right, n_minus_1),
			tau::build_bf_eq(
				make_bit_call_from_offset<node>(left, n),
				make_bit_call_from_offset<node>(right, n))));
	rules.push_back(make_rule<node>(general_header, general_body));

#ifdef DEBUG
	for (const auto& rule : rules) {
		LOG_TRACE << "bvgt_rules: " << LOG_RULE(rule) << "\n";
		LOG_TRACE << "bvgt_rules/header: " << LOG_FM(rule.first->get()) << "\n";
		LOG_TRACE << "bvgt_rules/body: " << LOG_FM(rule.second->get()) << "\n";
	}
#endif // DEBUG

	cache[bitwidth] = rules;
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

	static std::map<size_t, rewriter::rule> cache;
	if (auto it = cache.find(bitwidth); it != cache.end()) {
		return it->second;
	}

	auto left = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto right = tau::build_bf_variable(bv_type_id<node>(bitwidth));

	auto call = make_bvgt_call_from_index<node>(left, right, bitwidth);
	auto rules = bvgt_rules<node>(bitwidth);
	auto bits = bit_rules<node>(bitwidth);
	auto bit_zeros = is_bit_zero_rules<node>(bitwidth);
	auto bit_ones = is_bit_one_rules<node>(bitwidth);
	rules.insert(rules.end(), bits.begin(), bits.end());
	rules.insert(rules.end(), bit_zeros.begin(), bit_zeros.end());
	rules.insert(rules.end(), bit_ones.begin(), bit_ones.end());
	auto rr = make_rr<node>(rules, call);
	auto body = apply_rr_to_formula(rr);
	auto rule = make_rule<node>(call, body);

#ifdef DEBUG
	LOG_TRACE << "bvgt_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvgt_rule/header: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvgt_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG

	cache[bitwidth] = rule;
	return rule;
}

template<NodeType node>
tref bvgt(tref left, tref right) {
	auto bitwidth = get_bv_type_bitwidth<node>(left);
	auto rule = bvgt_rule<node>(bitwidth);
	auto call = make_bvgt_call_from_index<node>(left, right, bitwidth - 1);
	auto rr = make_rr<node>({ rule }, call);
	return apply_rr_to_formula(rr);
}

//
//
// _bvneq is a predicate that represents the not-equal relation between two
// bitvectors. Two bitvectors are unequal iff there exists a bit position where
// one is zero and the other is one.
//
// The key idea is to use the following recurrence predicates:
// - _bvneq[0](x, y) = (bit[0](x) = 0 && bit[0](y) = 1)
//                  || (bit[0](x) = 1 && bit[0](y) = 0);
// - _bvneq[n](x, y) = (bit[n](x) = 0 && bit[n](y) = 1)
//                  || (bit[n](x) = 1 && bit[n](y) = 0)
//                  || _bvneq[n-1](x, y);
//
//

template<NodeType node>
static tref make_bvneq_call_from_offset(tref left, tref right, tref offset) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_bvneq", { offset }, { left, right })));
}

template<NodeType node>
static tref make_bvneq_call_from_index(tref left, tref right, size_t index) {
	using tau = tree<node>;

	auto offset = tau::get_num(index);
	return make_bvneq_call_from_offset<node>(left, right, offset);
}

template<NodeType node>
static rewriter::rules bvneq_rules(size_t bitwidth) {
	using tau = tree<node>;

	static std::map<size_t, rewriter::rules> cache;
	if (auto it = cache.find(bitwidth); it != cache.end()) {
		return it->second;
	}

	auto left = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto right = tau::build_bf_variable(bv_type_id<node>(bitwidth));

	rewriter::rules rules;

	// base case: _bvneq[0](x, y) = (bit[0](x)=0 && bit[0](y)=1) || (bit[0](x)=1 && bit[0](y)=0)
	auto base_header = make_bvneq_call_from_index<node>(left, right, 0);
	auto base_body = tau::build_wff_or(
		tau::build_wff_and(
			make_is_bit_zero_call_from_index<node>(left, 0),
			make_is_bit_one_call_from_index<node>(right, 0)),
		tau::build_wff_and(
			make_is_bit_one_call_from_index<node>(left, 0),
			make_is_bit_zero_call_from_index<node>(right, 0)));
	rules.push_back(make_rule<node>(base_header, base_body));

	// general case: _bvneq[n](x, y) = (bit[n](x)=0 && bit[n](y)=1)
	//                               || (bit[n](x)=1 && bit[n](y)=0)
	//                               || _bvneq[n-1](x, y)
	auto n = tau::build_variable(untyped_type_id<node>());
	auto n_minus_1 = tau::build_shift(n, 1);
	auto general_header = make_bvneq_call_from_offset<node>(left, right, n);
	auto general_body = tau::build_wff_or(
		tau::build_wff_or(
			tau::build_wff_and(
				make_is_bit_zero_call_from_offset<node>(left, n),
				make_is_bit_one_call_from_offset<node>(right, n)),
			tau::build_wff_and(
				make_is_bit_one_call_from_offset<node>(left, n),
				make_is_bit_zero_call_from_offset<node>(right, n))),
		make_bvneq_call_from_offset<node>(left, right, n_minus_1));
	rules.push_back(make_rule<node>(general_header, general_body));

#ifdef DEBUG
	for (const auto& rule : rules) {
		LOG_TRACE << "bvneq_rules: " << LOG_RULE(rule) << "\n";
		LOG_TRACE << "bvneq_rules/header: " << LOG_FM(rule.first->get()) << "\n";
		LOG_TRACE << "bvneq_rules/body: " << LOG_FM(rule.second->get()) << "\n";
	}
#endif // DEBUG

	cache[bitwidth] = rules;
	return rules;
}

template<NodeType node>
static rewriter::rule bvneq_rule(size_t bitwidth) {
	using tau = tree<node>;

	static std::map<size_t, rewriter::rule> cache;
	if (auto it = cache.find(bitwidth); it != cache.end()) {
		return it->second;
	}

	auto left = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto right = tau::build_bf_variable(bv_type_id<node>(bitwidth));

	auto call = make_bvneq_call_from_index<node>(left, right, bitwidth - 1);
	auto rules = bvneq_rules<node>(bitwidth);
	auto bits = bit_rules<node>(bitwidth);
	auto bit_zeros = is_bit_zero_rules<node>(bitwidth);
	auto bit_ones = is_bit_one_rules<node>(bitwidth);
	rules.insert(rules.end(), bits.begin(), bits.end());
	rules.insert(rules.end(), bit_zeros.begin(), bit_zeros.end());
	rules.insert(rules.end(), bit_ones.begin(), bit_ones.end());
	auto rr = make_rr<node>(rules, call);
	auto body = apply_rr_to_formula(rr);
	auto rule = make_rule<node>(call, body);

#ifdef DEBUG
	LOG_TRACE << "bvneq_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvneq_rule/header: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvneq_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG

	cache[bitwidth] = rule;
	return rule;
}

template<NodeType node>
tref bvneq(tref left, tref right) {
	auto bitwidth = get_bv_type_bitwidth<node>(left);
	auto rule = bvneq_rule<node>(bitwidth);
	auto call = make_bvneq_call_from_index<node>(left, right, bitwidth - 1);
	auto rr = make_rr<node>({ rule }, call);
	return apply_rr_to_formula(rr);
}

} // namespace idni::tau_lang