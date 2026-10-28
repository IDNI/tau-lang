// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <bit>

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_predicate_blasting"

namespace idni::tau_lang {

//
//
// The comparisons are recurrences over the bits of two operands of a given
// bitwidth. The convention is that _name[k](x, y) compares the bits 0..k of
// x and y, and the main call starts at k = bitwidth - 1.
//
// _bvlt is the less-than relation:
// - _bvlt[0](x, y) = (bit[0](x) = 0) && (bit[0](y) = 1); (base case: only bit 0 left)
// - _bvlt[n](x, y) = ((bit[n](x) = 0) && (bit[n](y) = 1))
//		 || ((bit[n](x) = bit[n](y)) && _bvlt[n-1](x, y)); (general case: compare the most significant bit and recurse on the rest)
//
// _bvneq is the not-equal relation: two bitvectors are unequal iff there
// exists a bit position where one is zero and the other is one.
// - _bvneq[0](x, y) = (bit[0](x) = 0 && bit[0](y) = 1)
//                  || (bit[0](y) = 0 && bit[0](x) = 1);
// - _bvneq[n](x, y) = (bit[n](x) = 0 && bit[n](y) = 1)
//                  || (bit[n](y) = 0 && bit[n](x) = 1)
//                  || _bvneq[n-1](x, y);
//
// Greater-than is less-than with the operands swapped.
//
//

/**
 * @brief Creates a call `name[offset](left, right)` of a comparison recurrence.
 * @tparam node Node type
 * @param name Name of the recurrence (`_bvlt`, `_bvneq`)
 * @param left Left operand
 * @param right Right operand
 * @param offset Bit index as an integer offset tree
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bv_cmp_call_from_offset(const std::string& name, tref left,
	tref right, tref offset)
{
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref,
		tau::build_rr_ref(name, { offset }, { left, right })));
}

/**
 * @brief Creates a call `name[index](left, right)` of a comparison recurrence.
 * @tparam node Node type
 * @param name Name of the recurrence (`_bvlt`, `_bvneq`)
 * @param left Left operand
 * @param right Right operand
 * @param index Bit index
 * @return The constructed call term
 */
template<NodeType node>
static tref make_bv_cmp_call_from_index(const std::string& name, tref left,
	tref right, size_t index)
{
	using tau = tree<node>;

	return make_bv_cmp_call_from_offset<node>(name, left, right,
		tau::get_integer((int_t)index));
}

/** @brief Creates a call of the less-than recurrence at bit @p index. */
template<NodeType node>
static tref make_bvlt_call_from_index(tref left, tref right, size_t index) {
	return make_bv_cmp_call_from_index<node>("_bvlt", left, right, index);
}

/** @brief Creates a call of the not-equal recurrence at bit @p index. */
template<NodeType node>
static tref make_bvneq_call_from_index(tref left, tref right, size_t index) {
	return make_bv_cmp_call_from_index<node>("_bvneq", left, right, index);
}

/**
 * @brief The predicate "bit @p offset of @p low is 0 and that of @p high is 1".
 * @tparam node Node type
 * @param low Operand whose bit is 0
 * @param high Operand whose bit is 1
 * @param offset Bit index as an integer offset tree
 * @return The conjunction of the two bit tests
 */
template<NodeType node>
static tref make_bit_below(tref low, tref high, tref offset) {
	using tau = tree<node>;

	return tau::build_wff_and(
		make_is_bit_zero_call_from_offset<node>(low, offset),
		make_is_bit_one_call_from_offset<node>(high, offset));
}

/**
 * @brief Returns the rules for bitvector less-than recurrence.
 *
 * Generates the base and general case rules for bitvector less-than.
 *
 * @tparam node Node type
 * @param bitwidth Bitwidth of the operands
 * @return The set of rules for less-than, memoized per bitwidth in a
 *         function-local static map.
 *
 * @par Example
 * @code{.cpp}
 * // Base case: _bvlt[0](x,y) := is_bit_zero[0](x) && is_bit_one[0](y).
 * // General case: _bvlt[n](x,y) := (bit[n](x)=0 && bit[n](y)=1)
 * //                              || (_bvlt[n-1](x,y) && bit[n](x) = bit[n](y)).
 * auto rules = bvlt_rules<node_t>(2);
 * // rules.size() == 2 (one base-case rule, one general recursive rule)
 * @endcode
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
	auto zero = tau::get_integer((int_t)0);
	auto n = tau::build_variable(untyped_type_id<node>());
	auto n_minus_1 = tau::build_shift(n, 1);

	rewriter::rules rules;
	rules.push_back(make_rule<node>(
		make_bv_cmp_call_from_offset<node>("_bvlt", left, right, zero),
		make_bit_below<node>(left, right, zero)));
	rules.push_back(make_rule<node>(
		make_bv_cmp_call_from_offset<node>("_bvlt", left, right, n),
		tau::build_wff_or(
			make_bit_below<node>(left, right, n),
			tau::build_wff_and(
				make_bv_cmp_call_from_offset<node>("_bvlt", left, right, n_minus_1),
				tau::build_bf_eq(
					make_bit_call_from_offset<node>(left, n),
					make_bit_call_from_offset<node>(right, n))))));

	LOG_TRACE << "bvlt_rules/rules: " << LOG_RULE(rules);

	cache[bitwidth] = rules;
	return rules;
}

/**
 * @brief Returns the rules for bitvector not-equal recurrence.
 *
 * Generates the base and general case rules for bitvector not-equal.
 *
 * @tparam node Node type
 * @param bitwidth Bitwidth of the operands
 * @return The set of rules for not-equal, memoized per bitwidth in a
 *         function-local static map.
 *
 * @par Example
 * @code{.cpp}
 * // Base case: _bvneq[0](x,y) := (bit[0](x)=0 && bit[0](y)=1)
 * //                            || (bit[0](y)=0 && bit[0](x)=1).
 * // General case: _bvneq[n](x,y) := (bit[n](x)=0 && bit[n](y)=1)
 * //                               || (bit[n](y)=0 && bit[n](x)=1)
 * //                               || _bvneq[n-1](x,y).
 * auto rules = bvneq_rules<node_t>(2);
 * // rules.size() == 2 (one base-case rule, one general recursive rule)
 * @endcode
 */
template<NodeType node>
static rewriter::rules bvneq_rules(size_t bitwidth) {
	using tau = tree<node>;

	static std::map<size_t, rewriter::rules> cache;
	if (auto it = cache.find(bitwidth); it != cache.end()) {
		return it->second;
	}

	auto left = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto right = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto zero = tau::get_integer((int_t)0);
	auto n = tau::build_variable(untyped_type_id<node>());
	auto n_minus_1 = tau::build_shift(n, 1);
	auto differ_at = [&](tref offset) {
		return tau::build_wff_or(
			make_bit_below<node>(left, right, offset),
			make_bit_below<node>(right, left, offset));
	};

	rewriter::rules rules;
	rules.push_back(make_rule<node>(
		make_bv_cmp_call_from_offset<node>("_bvneq", left, right, zero),
		differ_at(zero)));
	rules.push_back(make_rule<node>(
		make_bv_cmp_call_from_offset<node>("_bvneq", left, right, n),
		tau::build_wff_or(differ_at(n),
			make_bv_cmp_call_from_offset<node>("_bvneq", left, right, n_minus_1))));

	LOG_TRACE << "bvneq_rules/rules: " << LOG_RULE(rules);

	cache[bitwidth] = rules;
	return rules;
}

/**
 * @brief Unfolds a comparison recurrence into one rule for its bitwidth.
 *
 * Applies @p rules, together with the bit, is_bit_zero and is_bit_one rules
 * of @p bitwidth, to the main call `name[bitwidth - 1](x, y)`.
 *
 * @tparam node Node type
 * @param name Name of the recurrence (`_bvlt`, `_bvneq`)
 * @param rules The base and general rules of the recurrence
 * @param bitwidth Bitwidth of the operands
 * @return The constructed rule, memoized per name and bitwidth; an error
 *         from applying the recurrence is carried in the report.
 */
template<NodeType node>
static result<rewriter::rule> bv_cmp_rule(const std::string& name,
	rewriter::rules rules, size_t bitwidth)
{
	using tau = tree<node>;

	static std::map<std::pair<std::string, size_t>, rewriter::rule> cache;
	if (auto it = cache.find({ name, bitwidth }); it != cache.end()) {
		return result<rewriter::rule>(it->second);
	}

	auto left = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto right = tau::build_bf_variable(bv_type_id<node>(bitwidth));

	auto call = make_bv_cmp_call_from_index<node>(name, left, right, bitwidth - 1);
	auto bits = bit_rules<node>(bitwidth);
	auto bit_zeros = is_bit_zero_rules<node>(bitwidth);
	auto bit_ones = is_bit_one_rules<node>(bitwidth);
	rules.insert(rules.end(), bits.begin(), bits.end());
	rules.insert(rules.end(), bit_zeros.begin(), bit_zeros.end());
	rules.insert(rules.end(), bit_ones.begin(), bit_ones.end());
	result<rewriter::rule> r;
	TAU_TRY(tref body, nso_rr_apply(make_rr<node>(rules, call)));
	auto rule = make_rule<node>(call, body);

	LOG_TRACE << "bv_cmp_rule/rule: " << LOG_RULE(rule);

	cache[{ name, bitwidth }] = rule;
	return r.with_value(rule);
}

/**
 * @brief Returns the rule for bitvector less-than recurrence.
 *
 * @tparam node Node type
 * @param bitwidth Bitwidth of the operands
 * @return The constructed rule, see @ref bv_cmp_rule
 *
 * @par Example
 * @code{.cpp}
 * // For bitwidth 2, fully unfolds bvlt_rules + the bit/is_bit_zero/
 * // is_bit_one rule chains into one concrete rule, e.g. (verified output):
 * // _bvlt[1](X, Y) := X{2}:bv[2]=0 && Y{2}:bv[2]={2}:bv[2]
 * //                || X{1}:bv[2]=0 && Y{1}:bv[2]={1}:bv[2] && X{2}:bv[2]=Y{2}:bv[2].
 * auto rule = bvlt_rule<node_t>(2);
 * @endcode
 */
template<NodeType node>
static result<rewriter::rule> bvlt_rule(size_t bitwidth) {
	return bv_cmp_rule<node>("_bvlt", bvlt_rules<node>(bitwidth), bitwidth);
}

/**
 * @brief Returns the rule for bitvector not-equal recurrence.
 *
 * @tparam node Node type
 * @param bitwidth Bitwidth of the operands
 * @return The constructed rule, see @ref bv_cmp_rule
 *
 * @par Example
 * @code{.cpp}
 * auto rule = bvneq_rule<node_t>(2);
 * @endcode
 */
template<NodeType node>
static result<rewriter::rule> bvneq_rule(size_t bitwidth) {
	return bv_cmp_rule<node>("_bvneq", bvneq_rules<node>(bitwidth), bitwidth);
}

/** @internal @copydoc bvlt @endinternal */
template<NodeType node>
result<tref> bvlt(tref left, tref right) {
	result<tref> r;
	TAU_TRY(auto bitwidth, get_bv_type_bitwidth<node>(left));
	TAU_TRY(auto rule, bvlt_rule<node>(bitwidth));
	TAU_TRY(auto body, apply_rule<node>(rule,
		make_bvlt_call_from_index<node>(left, right, bitwidth - 1)));
	return r.with_value(body);
}

/** @internal @copydoc bvgt @endinternal */
template<NodeType node>
result<tref> bvgt(tref left, tref right) {
	result<tref> r;
	TAU_TRY(auto bitwidth, get_bv_type_bitwidth<node>(left));
	TAU_TRY(auto rule, bvlt_rule<node>(bitwidth));
	TAU_TRY(auto body, apply_rule<node>(rule,
		make_bvlt_call_from_index<node>(right, left, bitwidth - 1)));
	return r.with_value(body);
}

/** @internal @copydoc bvneq @endinternal */
template<NodeType node>
result<tref> bvneq(tref left, tref right) {
	result<tref> r;
	TAU_TRY(auto bitwidth, get_bv_type_bitwidth<node>(left));
	TAU_TRY(auto rule, bvneq_rule<node>(bitwidth));
	TAU_TRY(auto body, apply_rule<node>(rule,
		make_bvneq_call_from_index<node>(left, right, bitwidth - 1)));
	return r.with_value(body);
}

} // namespace idni::tau_lang
