// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <bit>

namespace idni::tau_lang {

/**
 * @brief Builds the single bit mask constant 2^bit for a given bitwidth.
 *
 * The mask is built from its binary string representation, so it stays
 * correct for any bitwidth (a plain 1 << bit would overflow for bit >= 64).
 *
 * @tparam node Node type
 * @param bit Bit position (0 = least significant)
 * @param bitwidth Bitwidth of the mask
 * @return The bf-wrapped mask constant
 */
template<NodeType node>
static tref bit_mask_cte(size_t bit, size_t bitwidth) {
	using tau = tree<node>;

	DBG(assert(bit < bitwidth);)
	std::string mask(bitwidth, '0');
	mask[bitwidth - 1 - bit] = '1';
	return tau::get(tau::bf,
		tau::get_ba_constant(
			make_bitvector_value(bitwidth, mask),
			bv_type_id<node>(bitwidth)));
}

//
//
// _bit is a functional recurrence relation that computes the value of a specific
// bit in a bitvector. The idea is really simple, we just compute a bitvector
// value of 2^bit and conjunct it with the operand.
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

	auto offset = tau::get_integer((int_t)bit);
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

	auto cte = bit_mask_cte<node>(bit, bitwidth);
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
	return nso_rr_apply(rr);
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
	// original variable shifted by one. Note that bit<node>(x, i) yields the
	// masked value x & 2^i, so bits at different positions are related by
	// equivalence of their zero tests, not by equality of the masked values.
	auto body = tau::build_bf_eq_0(bit<node>(shifted, 0));
	for (size_t i = 0; i < bitwidth - 1; ++i) {
		body = tau::build_wff_and(body,
			tau::build_wff_equiv(
				tau::build_bf_eq_0(bit<node>(base, i)),
				tau::build_bf_eq_0(bit<node>(shifted, i + 1))));
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
	return nso_rr_apply(rr);
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
	// original variable shifted by one. Bits at different positions are
	// related by equivalence of their zero tests (see bvshl_by_one_rule).
	auto body = tau::build_bf_eq_0(bit<node>(shifted, bitwidth - 1));
	for (size_t i = 1; i < bitwidth; ++i) {
		body = tau::build_wff_and(body,
			tau::build_wff_equiv(
				tau::build_bf_eq_0(bit<node>(base, i)),
				tau::build_bf_eq_0(bit<node>(shifted, i - 1))));
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
	return nso_rr_apply(rr);
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

	auto offset = tau::get_integer((int_t)bit);
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

	auto cte = bit_mask_cte<node>(bit, bitwidth);
	auto var = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto head = make_is_bit_zero_call_from_index<node>(var, bit);
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

	auto offset = tau::get_integer((int_t)bit);
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

	auto cte = bit_mask_cte<node>(bit, bitwidth);
	auto var = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto head = make_is_bit_one_call_from_index<node>(var, bit);
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
	// Iterate over destination bits j: shifted[j] = 0 for j < offset (low bits
	// are zeroed), shifted[j] = base[j - offset] for j >= offset. Bits at
	// different positions are related by equivalence of their zero tests
	// (see bvshl_by_one_rule).
	tref body = nullptr;
	for (size_t j = 0; j < bitwidth; ++j) {
		tref shift_eq;
		if (j < offset) {
			shift_eq = tau::build_bf_eq_0(bit<node>(shifted, j));
		} else {
			shift_eq = tau::build_wff_equiv(
				tau::build_bf_eq_0(bit<node>(shifted, j)),
				tau::build_bf_eq_0(bit<node>(base, j - offset)));
		}
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
	using tau = tree<node>;

	// The shift amount must be a constant whose value is extractable
	if (!tau::get(tau::trim(count)).is_ba_constant()
		|| !is_bv_constant<node>(tau::trim(count))
		|| !get_bv_constant_value<node>(tau::trim(count)))
		return nullptr;
	auto rule = bvshl_rule<node>(count);
	auto call = make_bvshl_call<node>(base, count, shifted);
	auto rr = make_rr<node>({ rule }, call);
	return nso_rr_apply(rr);
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

	DBG( assert(is_bv_constant<node>(tau::trim(count))); )

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
	auto head = make_bvrhl_call<node>(base, count, shifted);
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
	// Iterate over destination bits j: shifted[j] = base[j + offset] for j < bitwidth - offset
	// (low bits come from higher base bits), shifted[j] = 0 for j >= bitwidth - offset
	// (high bits are zeroed out, i.e., logical right shift fills with zero).
	// Bits at different positions are related by equivalence of their zero
	// tests (see bvshl_by_one_rule).
	tref body = nullptr;
	for (size_t j = 0; j < bitwidth; ++j) {
		tref shift_eq;
		if (j + offset >= bitwidth) {
			shift_eq = tau::build_bf_eq_0(bit<node>(shifted, j));
		} else {
			shift_eq = tau::build_wff_equiv(
				tau::build_bf_eq_0(bit<node>(shifted, j)),
				tau::build_bf_eq_0(bit<node>(base, j + offset)));
		}
		body = body ? tau::build_wff_and(body, shift_eq) : shift_eq;
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
	using tau = tree<node>;

	// The shift amount must be a constant whose value is extractable
	if (!tau::get(tau::trim(count)).is_ba_constant()
		|| !is_bv_constant<node>(tau::trim(count))
		|| !get_bv_constant_value<node>(tau::trim(count)))
		return nullptr;
	auto rule = bvrhl_rule<node>(count);
	auto call = make_bvrhl_call<node>(base, count, shifted);
	auto rr = make_rr<node>({ rule }, call);
	return nso_rr_apply(rr);
}

//
//
// _bvcast is a predicate that holds between two bitvectors if the second is the
// result of casting the first to a different bitwidth. For zero-extension, the
// low bits of the result match the source bits and the high bits are zero. For
// truncation, the result bits match the low bits of the source.
//
//

/**
 * @brief Creates a call to the bitvector cast predicate.
 * @tparam node Node type
 * @param src Source bitvector
 * @param result Result bitvector
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvcast_call(tref src, tref result) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvcast", { src, result })));
}

/**
 * @brief Returns the rule for bitvector cast (zero-extension or truncation).
 *
 * For zero-extension (src_width < target_width):
 *   - Low bits of result match source bits
 *   - High bits of result are zero
 *
 * For truncation (src_width > target_width):
 *   - Result bits match the low bits of source
 *
 * @tparam node Node type
 * @param src_width Bitwidth of the source bitvector
 * @param target_width Bitwidth of the result bitvector
 * @return The constructed rule
 */
template<NodeType node>
static rewriter::rule bvcast_rule(size_t src_width, size_t target_width) {
	using tau = tree<node>;

	static std::map<std::pair<size_t, size_t>, rewriter::rule> cache;
	if (auto it = cache.find({ src_width, target_width }); it != cache.end()) {
		return it->second;
	}

	auto src = tau::build_bf_variable(bv_type_id<node>(src_width));
	auto result = tau::build_bf_variable(bv_type_id<node>(target_width));
	auto head = make_bvcast_call<node>(src, result);
	auto min_width = std::min(src_width, target_width);
	tref body = nullptr;

	// Constrain shared bits: (bit i of src == 0) <-> (bit i of result == 0)
	for (size_t i = 0; i < min_width; ++i) {
		auto src_bit_zero = tau::build_bf_eq_0(bit<node>(src, i));
		auto res_bit_zero = tau::build_bf_eq_0(bit<node>(result, i));
		auto bit_eq = tau::build_wff_equiv(src_bit_zero, res_bit_zero);
		body = body ? tau::build_wff_and(body, bit_eq) : bit_eq;
	}

	// For zero-extension: constrain extended bits to zero
	for (size_t i = min_width; i < target_width; ++i) {
		auto res_bit_zero = tau::build_bf_eq_0(bit<node>(result, i));
		body = body ? tau::build_wff_and(body, res_bit_zero) : res_bit_zero;
	}

	auto rule = make_rule<node>(head, body);

#ifdef DEBUG
	LOG_TRACE << "bvcast_rule: " << LOG_RULE(rule) << "\n";
	LOG_TRACE << "bvcast_rule/head: " << LOG_FM(rule.first->get()) << "\n";
	LOG_TRACE << "bvcast_rule/body: " << LOG_FM(rule.second->get()) << "\n";
#endif // DEBUG

	cache[{ src_width, target_width }] = rule;
	return rule;
}

/**
 * @brief Computes a predicate constraining result to be the cast of src.
 * @tparam node Node type
 * @param src Source bitvector
 * @param result Result bitvector (fresh variable of target type)
 * @return The resulting predicate term
 */
template<NodeType node>
tref bvcast(tref src, tref result) {
	auto src_width = get_bv_type_bitwidth<node>(src);
	auto target_width = get_bv_type_bitwidth<node>(result);
	auto rule = bvcast_rule<node>(src_width, target_width);
	auto call = make_bvcast_call<node>(src, result);
	auto rr = make_rr<node>({ rule }, call);
	return nso_rr_apply(rr);
}

} // namespace idni::tau_lang