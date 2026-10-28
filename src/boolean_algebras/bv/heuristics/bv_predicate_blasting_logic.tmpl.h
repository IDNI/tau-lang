// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <bit>

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_predicate_blasting"

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
 * @brief Creates a call to extract a specific bit from an operand (by offset tref).
 * @tparam node Node type
 * @param operand Operand
 * @param offset Bit position as an integer offset tree
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
 *
 * @par Example
 * @code{.cpp}
 * // Rule head "_bit[0](X)" with body "X & { 1 }:bv[4]" (bit 0's mask)
 * auto rule = bit_rule<node_t>(0, 4);
 * @endcode
 */
template<NodeType node>
static rewriter::rule bit_rule(size_t bit, size_t bitwidth) {
	using tau = tree<node>;

	static std::map<std::pair<size_t, size_t>, rewriter::rule> cache;
	if (auto it = cache.find({ bit, bitwidth }); it != cache.end()) {
		return it->second;
	}

	auto cte = bit_mask_cte<node>(bit, bitwidth);
	auto base = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto head = make_bit_call_from_index<node>(base, bit);
	auto body = tau::build_bf_and( base, cte);
	auto rule = make_rule<node>(head, body);

	LOG_TRACE << "bit_rule/rule: " << LOG_RULE(rule);

	cache[{ bit, bitwidth }] = rule;
	return rule;
}

/**
 * @brief Returns the rules @p bit_rule gives for each bit of a bitwidth.
 *
 * @tparam node Node type
 * @tparam bit_rule Rule of one bit position, given the bit and the bitwidth
 * @param bitwidth Bitwidth of the operand
 * @return The constructed rules, one per bit position, memoized per bitwidth
 *
 * @par Example
 * @code{.cpp}
 * auto rules = per_bit_rules<node_t, bit_rule<node_t>>(4);
 * // rules.size() == 4, one bit_rule per bit position 0..3
 * @endcode
 */
template<NodeType node, rewriter::rule (*bit_rule)(size_t, size_t)>
static rewriter::rules per_bit_rules(size_t bitwidth) {
	static std::map<size_t, rewriter::rules> cache;
	if (auto it = cache.find(bitwidth); it != cache.end()) {
		return it->second;
	}

	rewriter::rules rules;
	for (size_t bit = 0; bit < bitwidth; ++bit) {
		rules.push_back(bit_rule(bit, bitwidth));
	}

	cache[bitwidth] = rules;
	return rules;
}

/** @brief The bit extraction rules of each bit of @p bitwidth. */
template<NodeType node>
static rewriter::rules bit_rules(size_t bitwidth) {
	return per_bit_rules<node, bit_rule<node>>(bitwidth);
}

/** @internal @copydoc bit @endinternal */
template<NodeType node>
result<tref> bit(tref operand, int_t bit) {
	result<tref> r;
	TAU_TRY(auto bitwidth, get_bv_type_bitwidth<node>(operand));
	auto rules = bit_rules<node>(bitwidth);
	// bit is a non-negative bit index; the helper takes a size_t.
	auto call = make_bit_call_from_index<node>(operand, static_cast<size_t>(bit));
	auto rr = make_rr<node>(rules, call);
	TAU_TRY(auto applied, nso_rr_apply(rr));
	return r.with_value(applied);
}

//
//
// _bvshl_by_one is a predicate that holds between two bitvectors if the second
// is the result of left-shifting the first by one. Similarly, _bvshr_by_one is
// a predicate that holds between two bitvectors if the second is the result of
// right-shifting the first by one.
//
//

/** @brief The direction of a shift. */
enum class bv_shift_dir { left, right };

/**
 * @brief Creates a call to the bitvector shift-by-one recurrence.
 * @tparam node Node type
 * @param dir Direction of the shift
 * @param base Operand to shift
 * @param shifted Result variable
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvshift_by_one_call(bv_shift_dir dir, tref base, tref shifted) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref(
		dir == bv_shift_dir::left ? "_bvshl_by_one" : "_bvshr_by_one",
		{ base, shifted })));
}

/**
 * @brief Returns the rule for bitvector shift-by-one recurrence.
 *
 * A left shift zeroes the rightmost bit of the result and moves bit i of
 * the operand to bit i+1; a right shift zeroes the leftmost bit and moves
 * bit i to bit i-1.
 *
 * @tparam node Node type
 * @param bitwidth Bitwidth of the operand
 * @param dir Direction of the shift
 * @return The constructed rule
 *
 * @par Example
 * @code{.cpp}
 * // Rule: shifted's bit 0 is 0, and shifted's bit i+1 equals base's bit i
 * auto rule = bvshift_by_one_rule<node_t>(4, bv_shift_dir::left);
 * @endcode
 */
template<NodeType node>
static result<rewriter::rule> bvshift_by_one_rule(size_t bitwidth, bv_shift_dir dir) {
	using tau = tree<node>;

	static std::map<std::pair<size_t, bv_shift_dir>, rewriter::rule> cache;
	if (auto it = cache.find({ bitwidth, dir }); it != cache.end()) {
		return result<rewriter::rule>(it->second);
	}

	const bool left = dir == bv_shift_dir::left;
	auto base = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto shifted = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto header = make_bvshift_by_one_call<node>(dir, base, shifted);
	// The bit shifted in is zero, the rest of the bits are the same as the
	// original variable shifted by one. Note that bit<node>(x, i) yields the
	// masked value x & 2^i, so bits at different positions are related by
	// equivalence of their zero tests, not by equality of the masked values.
	result<rewriter::rule> r;
	TAU_TRY(auto shifted_in, bit<node>(shifted,
		static_cast<int_t>(left ? 0 : bitwidth - 1)));
	auto body = tau::build_bf_eq_0(shifted_in);
	for (size_t i = left ? 0 : 1; i < (left ? bitwidth - 1 : bitwidth); ++i) {
		TAU_TRY(auto base_i, bit<node>(base, static_cast<int_t>(i)));
		TAU_TRY(auto shifted_i, bit<node>(shifted,
			static_cast<int_t>(left ? i + 1 : i - 1)));
		body = tau::build_wff_and(body,
			tau::build_wff_equiv(
				tau::build_bf_eq_0(base_i),
				tau::build_bf_eq_0(shifted_i)));
	}
	auto rule = make_rule<node>(header, body);

	LOG_TRACE << "bvshift_by_one_rule/rule: " << LOG_RULE(rule);

	cache[{ bitwidth, dir }] = rule;
	return r.with_value(rule);
}

/**
 * @brief Computes the predicate "@p shifted is @p base shifted by one".
 * @tparam node Node type
 * @param base Operand to shift
 * @param shifted Result variable
 * @param dir Direction of the shift
 * @return The resulting predicate
 */
template<NodeType node>
static result<tref> bvshift_by_one(tref base, tref shifted, bv_shift_dir dir) {
	result<tref> r;
	TAU_TRY(auto bitwidth, get_bv_type_bitwidth<node>(base));
	TAU_TRY(auto rule, bvshift_by_one_rule<node>(bitwidth, dir));
	TAU_TRY(auto applied, apply_rule<node>(rule,
		make_bvshift_by_one_call<node>(dir, base, shifted)));
	return r.with_value(applied);
}

/** @internal @copydoc bvshl_by_one @endinternal */
template<NodeType node>
result<tref> bvshl_by_one(tref base, tref shifted) {
	return bvshift_by_one<node>(base, shifted, bv_shift_dir::left);
}

/** @internal @copydoc bvshr_by_one @endinternal */
template<NodeType node>
result<tref> bvshr_by_one(tref base, tref shifted) {
	return bvshift_by_one<node>(base, shifted, bv_shift_dir::right);
}

//
//
// _is_bit_zero and _is_bit_one are predicate recurrence relations that hold
// if a specific bit in a bitvector is zero, respectively one. Both mask the
// operand with 2^bit: the bit is zero iff the masked value is 0, and one iff
// it is the mask itself.
//
//

/**
 * @brief Creates a call to check if a specific bit is zero or one (by tref).
 * @tparam node Node type
 * @param operand Operand
 * @param bit Bit tref
 * @param one Whether the call checks for one rather than zero
 * @return The constructed call term
 */
template<NodeType node>
static tref make_is_bit_call_from_offset(tref operand, tref bit, bool one) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref(
		one ? "_is_bit_one" : "_is_bit_zero", { bit }, { operand })));
}

/** @brief Creates a call to check if bit @p bit (a tref) of @p operand is zero. */
template<NodeType node>
static tref make_is_bit_zero_call_from_offset(tref operand, tref bit) {
	return make_is_bit_call_from_offset<node>(operand, bit, false);
}

/** @brief Creates a call to check if bit @p bit (a tref) of @p operand is one. */
template<NodeType node>
static tref make_is_bit_one_call_from_offset(tref operand, tref bit) {
	return make_is_bit_call_from_offset<node>(operand, bit, true);
}

/** @brief Creates a call to check if bit @p bit (an index) of @p operand is zero. */
template<NodeType node>
static tref make_is_bit_zero_call_from_index(tref operand, size_t bit) {
	using tau = tree<node>;

	return make_is_bit_zero_call_from_offset<node>(operand,
		tau::get_integer((int_t)bit));
}

/** @brief Creates a call to check if bit @p bit (an index) of @p operand is one. */
template<NodeType node>
static tref make_is_bit_one_call_from_index(tref operand, size_t bit) {
	using tau = tree<node>;

	return make_is_bit_one_call_from_offset<node>(operand,
		tau::get_integer((int_t)bit));
}

/**
 * @brief Returns the rule for checking if a bit is zero or one in a bitvector.
 *
 * @tparam node Node type
 * @param bit Bit index
 * @param bitwidth Bitwidth of the operand
 * @param one Whether the rule checks for one rather than zero
 * @return The constructed rule
 */
template<NodeType node>
static rewriter::rule is_bit_rule(size_t bit, size_t bitwidth, bool one) {
	using tau = tree<node>;

	static std::map<std::tuple<size_t, size_t, bool>, rewriter::rule> cache;
	if (auto it = cache.find({ bit, bitwidth, one }); it != cache.end()) {
		return it->second;
	}

	auto cte = bit_mask_cte<node>(bit, bitwidth);
	auto var = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto head = make_is_bit_call_from_offset<node>(var,
		tau::get_integer((int_t)bit), one);
	auto masked = tau::build_bf_and(var, cte);
	auto body = one ? tau::build_bf_eq(masked, cte)
		: tau::build_bf_eq_0(masked);
	auto rule = make_rule<node>(head, body);

	LOG_TRACE << "is_bit_rule/rule: " << LOG_RULE(rule);

	cache[{ bit, bitwidth, one }] = rule;
	return rule;
}

/**
 * @brief Returns the rule for checking if a bit is zero in a bitvector.
 *
 * @par Example
 * @code{.cpp}
 * // Rule: "_is_bit_zero[0](X)" holds iff bit(X, 0) equals 0
 * auto rule = is_bit_zero_rule<node_t>(0, 4);
 * @endcode
 */
template<NodeType node>
static rewriter::rule is_bit_zero_rule(size_t bit, size_t bitwidth) {
	return is_bit_rule<node>(bit, bitwidth, false);
}

/**
 * @brief Returns the rule for checking if a bit is one in a bitvector.
 *
 * @par Example
 * @code{.cpp}
 * // Rule: "_is_bit_one[0](X)" holds iff bit(X, 0) does not equal 0
 * auto rule = is_bit_one_rule<node_t>(0, 4);
 * @endcode
 */
template<NodeType node>
static rewriter::rule is_bit_one_rule(size_t bit, size_t bitwidth) {
	return is_bit_rule<node>(bit, bitwidth, true);
}

/** @brief The is_bit_zero rules of each bit of @p bitwidth. */
template<NodeType node>
static rewriter::rules is_bit_zero_rules(size_t bitwidth) {
	return per_bit_rules<node, is_bit_zero_rule<node>>(bitwidth);
}

/** @brief The is_bit_one rules of each bit of @p bitwidth. */
template<NodeType node>
static rewriter::rules is_bit_one_rules(size_t bitwidth) {
	return per_bit_rules<node, is_bit_one_rule<node>>(bitwidth);
}

//
//
// _bvshl and _bvshr are more complex recurrences that can be defined using
// the _bit predicate and checking the relationships between the bits of the
// operands and the result. We can also define them directly by checking the
// relationships between the bits of the operands and the result without using
// the _bit predicate, which is what we do in the rules below. We also provide
// optimized versions for constant shifts.
//
//

/**
 * @brief Creates a call to the bitvector shift recurrence (constant shift).
 * @tparam node Node type
 * @param dir Direction of the shift
 * @param base Operand to shift
 * @param count Shift count (constant)
 * @param shifted Result variable
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bvshift_call(bv_shift_dir dir, tref base,
	tref count /* bv constant */, tref shifted)
{
	using tau = tree<node>;

	DBG( assert(is_bv_constant<node>(tau::trim(count))); )

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref(
		dir == bv_shift_dir::left ? "_bvshl" : "_bvshr",
		{ base, count, shifted })));
}

/**
 * @brief Returns the rule for bitvector shift (caching by direction, bitwidth
 * and shift amount).
 *
 * @tparam node Node type
 * @param count Shift amount (bv constant); its type gives the bitwidth
 * @param dir Direction of the shift
 * @return The constructed rule; a `code::invalid_argument` error when
 * @p count has no concrete value
 *
 * @par Example
 * @code{.cpp}
 * // count=3 for a 4-bit operand: shift-out is total once count >= bitwidth
 * // for smaller widths, or a per-bit shift-by-3 correspondence otherwise
 * auto count = tau::get(tau::bf, bv_constant<node_t>(4, 3));
 * auto rule = bvshift_rule<node_t>(count, bv_shift_dir::left);
 * @endcode
 */
template<NodeType node>
static result<rewriter::rule> bvshift_rule(tref count /* bv constant */,
	bv_shift_dir dir)
{
	using tau = tree<node>;

	const bool left = dir == bv_shift_dir::left;
	// offset is the shift amount before the bitwidth bound below, so it
	// can hold a full 64 bit value; the cache key must not truncate it.
	static std::map<std::tuple<bv_shift_dir, size_t, uint64_t>,
		rewriter::rule> cache;
	result<rewriter::rule> r;
	TAU_TRY(auto bitwidth, get_bv_type_bitwidth<node>(count));
	TAU_TRY(auto offset_value,
		get_bv_constant_value<node>(tau::trim(count)));
	if (!offset_value) return r.with_error(code::invalid_argument,
		std::string("bvshift_rule: the ") + (left ? "left" : "right")
			+ " shift count has no concrete value",
		{{label::value, truncate_for_message(TAU_TO_STR(count))}});
	auto offset = *offset_value;
	auto key = std::make_tuple(dir, bitwidth, offset);
	if (auto it = cache.find(key); it != cache.end())
		return r.with_value(it->second);
	auto base = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto shifted = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto head = make_bvshift_call<node>(dir, base, count, shifted);
	// If the shift is greater or equal to the bitwidth, the result is always zero
	if (offset >= bitwidth) {
		auto body = tau::build_bf_eq_0(shifted);
		auto rule = make_rule<node>(head, body);

		LOG_TRACE << "bvshift_rule/rule: " << LOG_RULE(rule);

		cache.emplace(key, rule);
		return r.with_value(rule);
	}
	// Otherwise, we compute the rule, store it in the cache and return it.
	// Iterate over destination bits j. A left shift zeroes the low bits,
	// shifted[j] = 0 for j < offset, and takes shifted[j] = base[j - offset]
	// otherwise. A right shift (logical, it fills with zero) takes
	// shifted[j] = base[j + offset] for j < bitwidth - offset and zeroes the
	// high bits. Bits at different positions are related by equivalence of
	// their zero tests (see bvshift_by_one_rule).
	tref body = nullptr;
	for (size_t j = 0; j < bitwidth; ++j) {
		TAU_TRY(auto shifted_j, bit<node>(shifted, static_cast<int_t>(j)));
		tref shift_eq;
		if (left ? j < offset : j + offset >= bitwidth) {
			shift_eq = tau::build_bf_eq_0(shifted_j);
		} else {
			// offset < bitwidth here, so the source is a bit position
			// and fits bit()'s int_t parameter.
			TAU_TRY(auto base_j, bit<node>(base,
				static_cast<int_t>(left ? j - offset : j + offset)));
			shift_eq = tau::build_wff_equiv(
				tau::build_bf_eq_0(shifted_j),
				tau::build_bf_eq_0(base_j));
		}
		body = body ? tau::build_wff_and(body, shift_eq) : shift_eq;
	}
	auto rule = make_rule<node>(head, body);

	LOG_TRACE << "bvshift_rule/rule: " << LOG_RULE(rule);

	cache.emplace(key, rule);
	return r.with_value(rule);
}

/**
 * @brief Computes the predicate "@p shifted is @p base shifted by @p count".
 * @tparam node Node type
 * @param base Operand to shift
 * @param count Shift amount
 * @param shifted Result variable
 * @param dir Direction of the shift
 * @return The resulting predicate; a null value when @p count is not a bv
 * constant with a concrete value
 */
template<NodeType node>
static result<tref> bvshift(tref base, tref count, tref shifted, bv_shift_dir dir) {
	using tau = tree<node>;

	result<tref> r;
	// The shift amount must be a constant whose value is extractable
	if (!tau::get(tau::trim(count)).is_ba_constant()
		|| !is_bv_constant<node>(tau::trim(count)))
		return r.with_value(nullptr);
	TAU_TRY(auto count_value,
		get_bv_constant_value<node>(tau::trim(count)));
	if (!count_value) return r.with_value(nullptr);
	TAU_TRY(auto rule, bvshift_rule<node>(count, dir));
	TAU_TRY(auto applied, apply_rule<node>(rule,
		make_bvshift_call<node>(dir, base, count, shifted)));
	return r.with_value(applied);
}

/** @internal @copydoc bvshl @endinternal */
// A null value when count is not a bv constant with a concrete value.
template<NodeType node>
result<tref> bvshl(tref base, tref count, tref shifted) {
	return bvshift<node>(base, count, shifted, bv_shift_dir::left);
}

/** @internal @copydoc bvshr @endinternal */
// A null value when count is not a bv constant with a concrete value.
template<NodeType node>
result<tref> bvshr(tref base, tref count, tref shifted) {
	return bvshift<node>(base, count, shifted, bv_shift_dir::right);
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
 *
 * @par Example
 * @code{.cpp}
 * // Zero-extension bv[2] -> bv[4]: see @ref bvcast for a full worked
 * // WFF-level example ({3}:bv[2] zero-extends to {3}:bv[4], not {11}:bv[4]).
 * auto rule = bvcast_rule<node_t>(2, 4);
 * @endcode
 */
template<NodeType node>
static result<rewriter::rule> bvcast_rule(size_t src_width, size_t target_width) {
	using tau = tree<node>;

	static std::map<std::pair<size_t, size_t>, rewriter::rule> cache;
	if (auto it = cache.find({ src_width, target_width }); it != cache.end()) {
		return result<rewriter::rule>(it->second);
	}

	auto src = tau::build_bf_variable(bv_type_id<node>(src_width));
	auto res = tau::build_bf_variable(bv_type_id<node>(target_width));
	auto head = make_bvcast_call<node>(src, res);
	auto min_width = std::min(src_width, target_width);
	tref body = nullptr;
	result<rewriter::rule> r;

	// Constrain shared bits: (bit i of src == 0) <-> (bit i of result == 0)
	for (size_t i = 0; i < min_width; ++i) {
		TAU_TRY(auto src_bit_i, bit<node>(src, static_cast<int_t>(i)));
		TAU_TRY(auto res_bit_i, bit<node>(res, static_cast<int_t>(i)));
		auto src_bit_zero = tau::build_bf_eq_0(src_bit_i);
		auto res_bit_zero = tau::build_bf_eq_0(res_bit_i);
		auto bit_eq = tau::build_wff_equiv(src_bit_zero, res_bit_zero);
		body = body ? tau::build_wff_and(body, bit_eq) : bit_eq;
	}

	// For zero-extension: constrain extended bits to zero
	for (size_t i = min_width; i < target_width; ++i) {
		TAU_TRY(auto res_bit_i, bit<node>(res, static_cast<int_t>(i)));
		auto res_bit_zero = tau::build_bf_eq_0(res_bit_i);
		body = body ? tau::build_wff_and(body, res_bit_zero) : res_bit_zero;
	}

	auto rule = make_rule<node>(head, body);

	LOG_TRACE << "bvcast_rule/rule: " << LOG_RULE(rule);

	cache[{ src_width, target_width }] = rule;
	return r.with_value(rule);
}

/** @internal @copydoc bvcast @endinternal */
template<NodeType node>
result<tref> bvcast(tref src, tref res) {
	result<tref> r;
	TAU_TRY(auto src_width, get_bv_type_bitwidth<node>(src));
	TAU_TRY(auto target_width, get_bv_type_bitwidth<node>(res));
	TAU_TRY(auto rule, bvcast_rule<node>(src_width, target_width));
	TAU_TRY(auto applied, apply_rule<node>(rule,
		make_bvcast_call<node>(src, res)));
	return r.with_value(applied);
}

} // namespace idni::tau_lang