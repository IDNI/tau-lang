// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <bit>

namespace idni::tau_lang {

//
//
// _bit is a functional recurrence relation that computes the value of a specific
// bit in a bitvector. The idea is really simple, we just compute a bitvector
// value of 1 << bit and conjunct it with the operand.
//
//

/**
 * @brief Creates a call to extract a specific bit from an operand (by index).
 * @tparam node Node type
 * @param operand Operand
 * @param bit Bit index
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bit_call_from_offset(tref operand, tref offset) {
	using tau = tree<node>;

	return tau::get(tau::bf, tau::get(tau::bf_ref, tau::build_rr_ref("_bit", { offset }, { operand })));
}

/**
 * @brief Creates a call to extract a specific bit from an operand (by index).
 * @tparam node Node type
 * @param operand Operand
 * @param bit Bit index
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bit_call_from_index(tref operand, size_t bit) {
	using tau = tree<node>;

	auto offset = tau::get_num(bit);
	return make_bit_call_from_offset<node>(operand, offset);
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
static rewriter::rule bit_rule(int_t bit, size_t bitwidth) {
	using tau = tree<node>;

	static std::map<std::tuple<int_t, size_t>, rewriter::rule> cache;
	if (auto it = cache.find({ bit, bitwidth }); it != cache.end()) {
		return it->second;
	}

	auto cte =
		tau::get(tau::bf,
			tau::get_ba_constant(
				make_bitvector_value(1 << bit, bitwidth), bv_type_id<node>(bitwidth)));
	auto base = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto head = make_bit_call_from_index<node>(base, bit);
	auto body = tau::build_bf_and( base, cte);
	auto rule = make_rule<node>(head, body);

#ifdef DEBUG
	LOG_TRACE << "bit_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bit_rule/head: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bit_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG

	cache[{ bit, bitwidth }] = rule;
	return rule;
}

/**
 * @brief Returns the rules for extracting bits for a given bitwidth.
 *
 * @tparam node Node type
 * @param bitwidth Bitwidth of the operand
 * @return The constructed rule
 */
template<NodeType node>
static rewriter::rules bit_rules(size_t bitwidth) {
	static std::map<size_t, rewriter::rules> cache;

	if (auto it = cache.find(bitwidth); it != cache.end()) {
		return it->second;
	}

	rewriter::rules rules;
	for (size_t bit = 0; bit < bitwidth; ++bit) {
		rules.push_back(bit_rule<node>(bit, bitwidth));
	}

	cache[bitwidth] = rules;
	return rules;
}

template<NodeType node>
tref bit(tref operand, int_t bit) {
	auto bitwidth = get_bv_type_bitwidth<node>(operand);
	auto rules = bit_rules<node>(bitwidth);
	auto call = make_bit_call_from_index<node>(operand, bit);
	auto rr = make_rr<node>(rules, call);
	return apply_rr_to_formula(rr);
}

//
//
// _bvshl_by_one is a predicate that holds between two bitvectors if the second
// is the result of left-shifting the first by one. Similarly, _bvrhl_by_one is
// a predicate that holds between two bitvectors if the second is the result of
// right-shifting the first by one.
//
//

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

	static std::map<size_t, rewriter::rule> cache;
	if (auto it = cache.find(bitwidth); it != cache.end()) {
		return it->second;
	}

	auto base = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto shifted = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto header = make_bvshl_by_one_call<node>(base, shifted);
	// the rightest bit is zero, the rest of the bits are the same as the
	// original variable shifted by one
	auto body = tau::build_bf_eq_0(bit<node>(shifted, 0));
	for (size_t i = 0; i < bitwidth - 1; ++i) {
		body = tau::build_wff_and( body,
			tau::build_bf_eq(bit<node>(base, i),	bit<node>(shifted, i + 1)));
	}
	auto rule = make_rule<node>(header, body);

#ifdef DEBUG
	LOG_TRACE << "bvshl_by_one_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvshl_by_one_rule/head: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvshl_by_one_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG

	cache[bitwidth] = rule;
	return rule;
}

template<NodeType node>
tref bvshl_by_one(tref base, tref shifted) {
	auto bitwidth = get_bv_type_bitwidth<node>(base);
	rewriter::rules rules;
	auto rule = bvshl_by_one_rule<node>(bitwidth);
	auto call = make_bvshl_by_one_call<node>(base, shifted);
	auto rr = make_rr<node>({ rule }, call);
	return apply_rr_to_formula(rr);
}

/**
 * @brief Creates a call to the bitvector right-shift-by-one recurrence.
 * @tparam node Node type
 * @param base Operand to shift
 * @param shifted Result variable
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvrhl_by_one_call(tref base, tref shifted) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvrhl_by_one", { base, shifted })));
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

	static std::map<size_t, rewriter::rule> cache;
	if (auto it = cache.find(bitwidth); it != cache.end()) {
		return it->second;
	}

	auto base = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto shifted = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto header = make_bvrhl_by_one_call<node>(base, shifted);
	// the leftest bit is zero, the rest of the bits are the same as the
	// original variable shifted by one
	auto body = tau::build_bf_eq_0(bit<node>(shifted, bitwidth - 1));
	for (size_t i = 1; i < bitwidth; ++i) {
		body = tau::build_wff_and(body,
			tau::build_bf_eq(
				bit<node>(base, i),
				bit<node>(shifted, i - 1)));
	}
	auto rule = make_rule<node>(header, body);

#ifdef DEBUG
	LOG_TRACE << "bvrhl_by_one_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvrhl_by_one_rule/head: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvrhl_by_one_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG

	cache[bitwidth] = rule;
	return rule;
}

template<NodeType node>
tref bvrhl_by_one(tref base, tref shifted) {
	auto bitwidth = get_bv_type_bitwidth<node>(base);
	auto rule = bvrhl_by_one_rule<node>(bitwidth);
	auto call = make_bvrhl_by_one_call<node>(base, shifted);
	auto rr = make_rr<node>({ rule }, call);
	return apply_rr_to_formula(rr);
}

//
//
// _is_bit_zero is a predicate recurrence relation that holds if a specific bit
// in a bitvector is zero. We can define it using the _bit predicate and checking
// if the result is zero. Or directly by checking if the bit is not one.
//
//

/**
 * @brief Creates a call to check if a specific bit is zero (by tref).
 * @tparam node Node type
 * @param operand Operand
 * @param bit Bit tref
 * @return The constructed call term
 */
template<NodeType node>
static tref make_is_bit_zero_call_from_offset(tref operand, tref bit) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_is_bit_zero", { bit }, { operand })));
}

/**
 * @brief Creates a call to check if a specific bit is zero (by index).
 * @tparam node Node type
 * @param operand Operand
 * @param bit Bit index
 * @return The constructed call term
 */
template<NodeType node>
static tref make_is_bit_zero_call_from_index(tref operand, size_t bit) {
	using tau = tree<node>;

	auto offset = tau::get_num(bit);
	return make_is_bit_zero_call_from_offset<node>(operand, offset);
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

	static std::map<std::pair<size_t, size_t>, rewriter::rule> cache;
	if (auto it = cache.find({ bit, bitwidth }); it != cache.end()) {
		return it->second;
	}

	auto cte = tau::get(tau::bf,
		tau::get_ba_constant(
			make_bitvector_value(1 << bit, bitwidth), bv_type_id<node>(bitwidth)));
	auto var = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto head = make_bit_call_from_index<node>(var, bit);
	auto body =	tau::build_bf_eq_0(tau::build_bf_and(var, cte));
	auto rule = make_rule<node>(head, body);

#ifdef DEBUG
	LOG_TRACE << "is_bit_zero_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "is_bit_zero_rule/head: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "is_bit_zero_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG

	cache[{ bit, bitwidth }] = rule;
	return rule;
}

/**
 * @brief Returns the rules for checking if a bit is zero in a bitvector.
 *
 * @tparam node Node type
 * @param bitwidth Bitwidth of the operand
 * @return The constructed rule
 */
template<NodeType node>
static rewriter::rules is_bit_zero_rules(size_t bitwidth) {
	static std::map<size_t, rewriter::rules> cache;
	if (auto it = cache.find(bitwidth); it != cache.end()) {
		return it->second;
	}

	rewriter::rules rules;
	for (size_t bit = 0; bit < bitwidth; ++bit) {
		rules.push_back(is_bit_zero_rule<node>(bit, bitwidth));
	}

	cache[bitwidth] = rules;
	return rules;
}

template<NodeType node>
tref is_bit_zero(tref operand, int_t bit) {
	auto bitwidth = get_bv_type_bitwidth<node>(operand);
	auto rule = is_bit_zero_rule<node>(bit, bitwidth);
	auto call = make_is_bit_zero_call<node>(operand, bit);
	auto rr = make_rr<node>({ rule }, call);
	return apply_rr_to_formula(rr);
}

//
//
// _is_bit_one is a predicate recurrence relation that holds if a specific bit
// in a bitvector is one. We can define it using the _bit predicate and checking
// if the result is one. Or directly by checking if the bit is not zero.
//
//

/**
 * @brief Creates a call to check if a specific bit is one (by tref).
 * @tparam node Node type
 * @param operand Operand
 * @param bit Bit tref
 * @return The constructed call term
 */
template<NodeType node>
static tref make_is_bit_one_call_from_offset(tref operand, tref bit) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_is_bit_one", { bit }, { operand })));
}

/**
 * @brief Creates a call to check if a specific bit is one (by index).
 * @tparam node Node type
 * @param operand Operand
 * @param bit Bit index
 * @return The constructed call term
 */
template<NodeType node>
static tref make_is_bit_one_call_from_index(tref operand, size_t bit) {
	using tau = tree<node>;

	auto offset = tau::get_num(bit);
	return make_is_bit_one_call_from_offset<node>(operand, offset);
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

	static std::map<std::pair<size_t, size_t>, rewriter::rule> cache;
	if (auto it = cache.find({ bit, bitwidth }); it != cache.end()) {
		return it->second;
	}

	auto cte =
		tau::get(tau::bf,
			tau::get_ba_constant(
				make_bitvector_value(1 << bit, bitwidth), bv_type_id<node>(bitwidth)));
	auto var = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto head = make_bit_call_from_index<node>(var, bit);
	auto body =	tau::build_bf_eq(tau::build_bf_and(var, cte), cte);
	auto rule = make_rule<node>(head, body);

#ifdef DEBUG
	LOG_TRACE << "is_bit_one_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "is_bit_one_rule/head: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "is_bit_one_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG

	cache[{ bit, bitwidth }] = rule;
	return rule;
}

/**
 * @brief Returns the rules for checking if a bit is one in a bitvector.
 *
 * @tparam node Node type
 * @param bitwidth Bitwidth of the operand
 * @return The constructed rule
 */
template<NodeType node>
static rewriter::rules is_bit_one_rules(size_t bitwidth) {
	static std::map<size_t, rewriter::rules> cache;
	if (auto it = cache.find(bitwidth); it != cache.end()) {
		return it->second;
	}

	rewriter::rules rules;
	for (size_t bit = 0; bit < bitwidth; ++bit) {
		rules.push_back(is_bit_one_rule<node>(bit, bitwidth));
	}

	cache[bitwidth] = rules;
	return rules;
}

template<NodeType node>
tref is_bit_one(tref operand, int_t bit) {
	auto bitwidth = get_bv_type_bitwidth<node>(operand);
	auto rule = is_bit_one_rule<node>(bit, bitwidth);
	auto call = make_is_bit_one_call<node>(operand, bit);
	auto rr = make_rr<node>({ rule }, call);
	return apply_rr_to_formula(rr);
}

//
//
// _bvshl and _bvrhl are more complex recurrences that can be defined using
// the _bit predicate and checking the relationships between the bits of the
// operands and the result. We can also define them directly by checking the
// relationships between the bits of the operands and the result without using
// the _bit predicate, which is what we do in the rules below. We also provide
// optimized versions for constant shifts.
//
//

/**
 * @brief Creates a call to the bitvector shift-left recurrence (constant shift).
 * @tparam node Node type
 * @param base Operand to shift
 * @param count Shift count (constant)
 * @param shifted Result variable
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvshl_call(tref base, tref count /* bv constant */, tref shifted) {
	using tau = tree<node>;

	DBG( assert(is_bv_constant<node>(tau::trim(count))); )

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvshl", { base, count, shifted })));
}

/**
 * @brief Returns the rule for bitvector shift-left (caching by bitwidth and shift amount).
 *
 * @tparam node Node type
 * @param count Shift amount (constant)
 * @param bitwidth Bitwidth of the operands
 * @return The constructed rule
 */
template<NodeType node>
static rewriter::rule bvshl_rule(tref count /* bv constant */) {
	using tau = tree<node>;

	static std::map<tref, rewriter::rule> cache;
	if (cache.find(count) != cache.end()) return cache[count];

	auto bitwidth = get_bv_type_bitwidth<node>(count);
	auto offset = get_bv_constant_value<node>(tau::trim(count)).value();
	// If the shift is greater or equal to the bitwidth, the result is always zero
	auto base = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto shifted = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto head = make_bvshl_call<node>(base, count, shifted);
	if (offset >= bitwidth) {
		auto body = tau::build_bf_eq_0(shifted);
		auto rule = make_rule<node>(head, body);

#ifdef DEBUG
		LOG_TRACE << "bvshl_rule (zero case): " << LOG_RULE(rule) << "\n";
		LOG_TRACE << "bvshl_rule/head: " << LOG_FM(rule.first->get()) << "\n";
		LOG_TRACE << "bvshl_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG

		cache[count] = rule;
		return cache[count];
	}
	// Otherwise, we compute the rule, store it in the cache and return it.
	tref body = nullptr;
	for (size_t i = 0; i < bitwidth; ++i) {
		if (i + offset > bitwidth) {
			auto shifted_bit = bit<node>(shifted, i);
			auto shift_eq = tau::build_bf_eq_0(shifted_bit);
			body = body ? tau::build_wff_and(body, shift_eq) : shift_eq;
			continue;
		}
		auto base_bit = bit<node>(base, i);
		auto shifted_bit = bit<node>(shifted, i + offset);
		auto shift_eq = tau::build_bf_eq(shifted_bit, base_bit);
		body = body ? tau::build_wff_and(body, shift_eq) : shift_eq;
	}
	auto rule = make_rule<node>(head, body);

#ifdef DEBUG
	LOG_TRACE << "bvshl_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvshl_rule/head: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvshl_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG

	cache[count] = rule;
	return cache[count];
}

template<NodeType node>
tref bvshl(tref base, tref count, tref shifted) {
	// TODO (MEDIUM) check inputs
	auto rule = bvshl_rule<node>(count);
	auto call = make_bvshl_call<node>(base, count, shifted);
	auto rr = make_rr<node>({ rule }, call);
	return apply_rr_to_formula(rr);
}

/**
 * @brief Creates a call to the bitvector right-shift recurrence (constant shift).
 * @tparam node Node type
 * @param base Operand to shift
 * @param count Shift amount (constant)
 * @param shifted Result variable
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvrhl_call(tref base, tref count /* bv constant */, tref shifted) {
	using tau = tree<node>;

	DBG( assert(is_bv_constant<node>(count)); )

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvrhl", { base, count, shifted })));
}

/**
 * @brief Returns the rule for bitvector right-shift (caching by bitwidth and shift amount).
 *
 * @tparam node Node type
 * @param count Shift count (bv constant)
 * @return The constructed rule
 */
template<NodeType node>
static rewriter::rule bvrhl_rule(tref count /* bv constant */) {
	using tau = tree<node>;

	static std::map<tref, rewriter::rule> cache;
	if (cache.find(count) != cache.end()) return cache[count];

	auto bitwidth = get_bv_type_bitwidth<node>(count);
	auto offset = get_bv_constant_value<node>(tau::trim(count)).value();
	auto base = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto shifted = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto head = make_bvshl_call<node>(base, count, shifted);
	// If the shift is greater or equal to the bitwidth, the result is always zero
	if (offset >= bitwidth) {
		auto body = tau::build_bf_eq_0(shifted);
		auto rule = make_rule<node>(head, body);

#ifdef DEBUG
		LOG_TRACE << "bvrhl_rule (zero case): " << LOG_RULE(rule) << "\n";
		LOG_TRACE << "bvrhl_rule/head: " << LOG_FM(rule.first->get()) << "\n";
		LOG_TRACE << "bvrhl_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG

		cache[count] = rule;
		return cache[count];
	}
	// Otherwise, we compute the rule, store it in the cache and return it.
	tref body = nullptr;
	for (size_t i = 0; i < bitwidth; ++i) {
		if (i < offset) {
			auto shifted_bit = bit<node>(shifted, i);
			auto shift_eq = tau::build_bf_eq_0(shifted_bit);
			body = body ? tau::build_wff_and(body, shift_eq) : shift_eq;
			continue;
		}
		auto base_bit = bit<node>(base, i);
		auto shifted_bit = bit<node>(shifted, i - offset);
		auto bit_eq = tau::build_bf_eq(base_bit, shifted_bit);
		body = body ? tau::build_wff_and(body, bit_eq) : bit_eq;
	}
	auto rule = make_rule<node>(head, body);

#ifdef DEBUG
	LOG_TRACE << "bvrhl_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvrhl_rule/head: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvrhl_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG

	cache[count] = rule;
	return cache[count];
}

template<NodeType node>
tref bvrhl(tref base, tref count, tref shifted) {
	// TODO (MEDIUM) check inputs
	auto rule = bvrhl_rule<node>(count);
	auto call = make_bvrhl_call<node>(base, count, shifted);
	auto rr = make_rr<node>({ rule }, call);
	return apply_rr_to_formula(rr);
}

} // namespace idni::tau_lang