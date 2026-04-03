// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_bitblasting"

#include <bit>

namespace idni::tau_lang {

using bool_node = idni::tau_lang::node<cvc5::Term, Bool>;
using bool_tree = tree<bool_node>;
static auto bool_id = bool_type_id<bool_node>();

// Custom factory methods for creating rules and rr's.
template<NodeType node>
static rewriter::rule make_rule(tref head, tref body) {
	using tau = tree<node>;

	return rewriter::rule(tau::geth(head), tau::geth(body));
}

template<NodeType node>
static rr<node> make_rr(const rewriter::rules& rules, tref main) {
	using tau = tree<node>;

	return rr<node>{rules, tau::geth(main)};
}

// Helper functions to create calls to the bitblasting recurrence functions and relations.
template<NodeType node>
static tref make_bvadd_call_with_index(tref left, tref right, tref result, size_t index) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_bvadd", { index }, { left, right, result })));
}

template<NodeType node>
static tref make_bvadd_call(tref left, tref right, tref result, tref offset) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_bvadd", { offset }, { left, right, result })));
}

template<NodeType node>
static tref make_bvsub_call_with_index(tref left, tref right, tref result, size_t index) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_bvsub", { index }, { left, right, result })));
}

template<NodeType node>
static tref make_bvsub_call(tref left, tref right, tref result, tref offset) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_bvsub", { offset }, { left, right, result })));
}

template<NodeType node>
static tref make_bvshl_by_one_call(tref operand,  tref shifted) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvshl_by_one", { operand, shifted })));
}

template<NodeType node>
static tref make_bvrhl_by_one_call(tref operand, tref shifted) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvrhl_by_one", { operand, shifted })));
}

template<NodeType node>
static tref make_bvmul_call(tref left, size_t right, tref result) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_bvmul", { right }, { left, result })));
}

template<NodeType node>
static tref make_euclidean_division_call(tref dividend, tref divisor, tref quotient, tref remainder) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_euclidean_division", { dividend, divisor, quotient, remainder })));
}

template<NodeType node>
static tref make_bvdiv_call(tref dividend, tref divisor, tref result) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvdiv", { dividend, divisor, result })));
}

template<NodeType node>
static tref make_bvmod_call(tref dividend, tref divisor, tref result) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvmod", { dividend, divisor, result })));
}

template<NodeType node>
static tref make_bvshl_call(tref left, tref right /* bv constant */, tref result) {
	using tau = tree<node>;

	DBG( assert(is_bv_constant<node>(right)); )

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvshl", { left, right, result })));
}

template<NodeType node>
static tref make_bvrhl_call(tref left, tref right /* bv copnstant */, tref result) {
	using tau = tree<node>;

	DBG( assert(is_bv_constant<node>(right)); )

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvrhl", { left, right, result })));
}

template<NodeType node>
static tref make_bit_call(tref operand, size_t bit) {
	using tau = tree<node>;

	return tau::get(tau::bf, tau::get(tau::bf_ref, tau::build_rr_ref("_bit", bit, { operand })));
}

template<NodeType node>
static tref make_bit_call(tref operand, tref bit) {
	using tau = tree<node>;

	return tau::get(tau::bf, tau::get(tau::bf_ref, tau::build_rr_ref("_bit", {bit}, { operand })));
}

template<NodeType node>
static tref make_is_bit_zero_call(tref operand, size_t bit) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_is_bit_zero", bit, { operand })));
}

template<NodeType node>
static tref make_is_bit_zero_call(tref operand, tref bit) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_is_bit_zero", {bit}, { operand })));
}

template<NodeType node>
static tref make_is_bit_one_call(tref operand, size_t bit) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_is_bit_one", bit, { operand })));
}

template<NodeType node>
static tref make_is_bit_one_call(tref operand, tref bit) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_is_bit_one", {bit}, { operand })));
}

template<NodeType node>
static tref make_bvlt_call(tref left, tref right, size_t offset) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_bvlt", offset, { left, right })));
}

template<NodeType node>
static tref make_bvlt_call(tref left, tref right, tref offset) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_bvlt", {offset}, { left, right })));
}

template<NodeType node>
static tref make_bvgt_call(tref left, tref right, size_t offset) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_bvgt", offset, { left, right })));
}

template<NodeType node>
static tref make_bvgt_call(tref left, tref right, tref offset) {
	using tau = tree<node>;

	return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_rr_ref("_bvgt", {offset}, { left, right })));
}

// Factory methods returning the rule(s) needed in certain predicate blasting
// cases (bvadd,...).

// addition(x, y) = addition(x ^ y, (x & y) << 1, m) mod m;
template<NodeType node>
static rewriter::rules bvadd_rules(size_t bitwidth) {
	using tau = tree<node>;

	rewriter::rules rules;
	// general case: addition[n](x, y, z) = ex w addition[n-1](x ^ y, w, z) && shr_by_one(x & y, w);
	auto x = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto y = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto z = tau::build_bf_variable(bv_type_id<node>(bitwidth));
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
	return rules;
}

// subtraction(x,y) = subtraction(x ^ y, (~x & y) << 1, m) mod m;
template<NodeType node>
static rewriter::rules bvsub_rules(size_t bitwidth) {
	using tau = tree<node>;

	rewriter::rules rules;
	// general case: subtraction[n](x, y, z) = ex w subtraction[n-1](x ^ y, w, z) && shr_by_one((~x & y), w);
	auto x = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto y = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto z = tau::build_bf_variable(bv_type_id<node>(bitwidth));
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
	return rules;
}

template<NodeType node>
static rewriter::rules bvshl_by_one_rules(size_t bitwidth) {
	using tau = tree<node>;

	// bvshl_by_one(x, y) = (bit[0](y) = 0) && (bit[1](y) = bit[0](x)) && (bit[2](y) = bit[1](x)) && ... && (bit[n](y) = bit[n-1](x))
	auto var = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto result = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto header = tau::build_rr_ref("_bvshl_by_one", {}, { var, result });
	// the right most bit is zero, the rest of the bits are the same as the original variable shifted by one
	auto result_bit_bitwidth = tau::get(tau::bf, tau::build_rr_ref("_bit", bitwidth - 1, { result }));
	static auto bit_width = tau::_0(bv_type_id<node>(bitwidth));
	tref body = tau::build_bf_eq(bit_width, result_bit_bitwidth);
	for (size_t i = 1; i < bitwidth; ++i) {
		auto bit_i = tau::get(tau::bf, tau::build_rr_ref("_bit", i, { var }));
		auto bit_i_minus_1 = tau::get(tau::bf, tau::build_rr_ref("_bit", i - 1, { result }));
		auto bit_i_eq_bit_i_minus_1 = tau::build_bf_eq(bit_i, bit_i_minus_1);
		body = body ? tau::build_wff_and(body, bit_i_eq_bit_i_minus_1) : bit_i_eq_bit_i_minus_1;
	}
	return { make_rule<node>(header, body) };
}

template<NodeType node>
static rewriter::rules bvrhl_by_one_rules(size_t bitwidth) {
	using tau = tree<node>;

	// bvrhl_by_one(x, y) = (bit[n-1](y) = 0) && (bit[0](y) = bit[1](x)) && (bit[1](y) = bit[2](x)) && ... && (bit[n-2](y) = bit[n-1](x))
	auto var = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto result = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto header = tau::build_rr_ref("_bvrhl_by_one", {}, { var, result });
	// the left most bit is zero, the rest of the bits are the same as the original variable shifted by one
	auto result_bit_0 = tau::get(tau::bf, tau::build_rr_ref("_bit", 0, { result }));
	static auto bit_width_minus_1 = tau::_0(bv_type_id<node>(bitwidth));
	tref body = tau::build_bf_eq(bit_width_minus_1, result_bit_0);
	for (size_t i = 0; i < bitwidth - 1; ++i) {
		auto bit_i = tau::get(tau::bf, tau::build_rr_ref("_bit", i, { var }));
		auto bit_i_plus_1 = tau::get(tau::bf, tau::build_rr_ref("_bit", i + 1, { result }));
		auto bit_i_eq_bit_i_plus_1 = tau::build_bf_eq(bit_i, bit_i_plus_1);
		body = body ? tau::build_wff_and(body, bit_i_eq_bit_i_plus_1) : bit_i_eq_bit_i_plus_1;
	}
	return { make_rule<node>(header, body) };
}

template<NodeType node>
static rewriter::rule bvmul_rec_rule(tref y /* cvc5 constant */, size_t bitwidth) {
	using tau = tree<node>;

	DBG( assert(is_bv_constant<node>(y)); )

	auto x = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto z = tau::build_bf_variable(bv_type_id<node>(bitwidth));

	// multiplication[n](x, 0, z) = (z = 0) or multiplication[n](0, x, z) = (z = 0);
	if (is_zero_bv_constant<node>(y)) {
		auto zero_right = tau::build_rr_ref("_bvmul", { x, y, z });
		auto body = tau::build_bf_eq(tau::_0(bv_type_id<node>(bitwidth)), z);
		return make_rule<node>(zero_right, body);
	}

	auto w = tau::build_variable(bv_type_id<node>(bitwidth));

	// case y & 1 = 1: multiplication(x, y, w) = ex z ex v multiplication(x << 1, v) && addition(x, v, z);
	if (lsb<node>(y)) {
		auto v = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto odd_right = tau::build_rr_ref("_bvmul", { x, y, z });
		auto odd_body = tau::build_wff_ex(w,
			tau::build_wff_ex(w,
				tau::build_wff_and(
					tau::build_wff_ex(v,
						tau::build_wff_and(
							tau::build_wff_and(
								make_bvshl_by_one_call<node>(x, v)),
								make_bvmul_call<node>(v, shr_by_one<node>(y), w)),
							make_bvadd_call<node>(x, w, z)
		))));
		return make_rule<node>(odd_right, odd_body);
	}

	// case y & 1 = 0: multiplication(x, y, z) = ex v multiplication[y >> 1](x << 1, v) && (z = v);
	auto even_right = tau::build_rr_ref("_bvmul", { x, y, z });
	auto even_body = tau::build_wff_ex(w,
		tau::build_wff_and(
			make_bvmul_call<node>(tau::build_bf_shl(x), shr_by_one<node>(y), w),
			tau::build_bf_eq(z, w)
	));
	return make_rule<node>(even_right, even_body);
}

template<NodeType node>
static rewriter::rules bvlt_rules(size_t bitwidth) {
	using tau = tree<node>;

	auto x = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto y = tau::build_bf_variable(bv_type_id<node>(bitwidth));

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
	return rules;
}

template<NodeType node>
static rewriter::rule bvlt_rule(size_t bitwidth) {
	using tau = tree<node>;

	auto x = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto y = tau::build_bf_variable(bv_type_id<node>(bitwidth));

	auto main = make_bvlt_call<node>(x, y, bitwidth);
	auto rules = bvlt_rules<node>(bitwidth);
	auto temp = make_rr<node>(rules, main);
	auto body = apply_rr_to_formula(temp);
	if (!body) {
		LOG_ERROR << "Failed to apply rules for bvlt predicate.";
		return make_rule<node>(main, body);
	}
	return make_rule<node>(main, body);
}

template<NodeType node>
static rewriter::rules bvgt_rules(size_t bitwidth) {
	using tau = tree<node>;

	auto x = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto y = tau::build_bf_variable(bv_type_id<node>(bitwidth));

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
	return rules;
}

template<NodeType node>
static rewriter::rule bvgt_rule(size_t bitwidth) {
	using tau = tree<node>;

	auto x = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto y = tau::build_bf_variable(bv_type_id<node>(bitwidth));

	auto main = make_bvgt_call<node>(x, y, bitwidth);
	auto rules = bvgt_rules<node>(bitwidth);
	auto temp = make_rr<node>(rules, main);
	auto body = apply_rr_to_formula(temp);
	if (!body) {
		LOG_ERROR << "Failed to apply rules for bvgt predicate.";
		return make_rule<node>(main, body);
	}
	return make_rule<node>(main, body);
}

template<NodeType node>
static rewriter::rule bit_rule(size_t bitwidth, size_t bit) {
	using tau = tree<node>;

	auto bit_offset = tau::get_num(bit);
	auto bit_cte =
		tau::get(tau::bf,
			tau::get_ba_constant(
				make_bitvector_value(1 <<bit, bitwidth), bv_type_id<node>(bitwidth)));
	auto var = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto head = tau::build_rr_ref("_bit", { bit_offset }, { var });
	auto body = tau::build_bf_and( var, bit_cte);
	return make_rule<node>(head, body);
}

template<NodeType node>
static rewriter::rule is_bit_zero_rule(size_t bitwidth, size_t bit) {
	using tau = tree<node>;

	auto bit_offset = tau::get_num(bit);
	auto bit_cte =
		tau::get(tau::bf,
			tau::get_ba_constant(
				make_bitvector_value(1 << bit, bitwidth), bv_type_id<node>(bitwidth)));
	auto var = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto head = tau::build_rr_ref("_is_bit_zero", { bit_offset }, { var });
	auto body =	tau::build_bf_eq_0(tau::build_bf_and(var, bit_cte));
	return make_rule<node>(head, body);
}

template<NodeType node>
static rewriter::rule is_bit_one_rule(size_t bitwidth, size_t bit) {
	using tau = tree<node>;

	auto bit_offset = tau::get_num(bit);
	auto bit_cte =
		tau::get(tau::bf,
			tau::get_ba_constant(
				make_bitvector_value(1 << bit, bitwidth), bv_type_id<node>(bitwidth)));
	auto var = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto head = tau::build_rr_ref("_is_bit_one", { bit_offset }, { var });
	auto body =	tau::build_bf_eq(tau::build_bf_and(var, bit_cte), bit_cte);
	return make_rule<node>(head, body);
}

// Factory methods to create predicates from their arguments and the rules defined above.
// The rules are cached so that they are only computed once per bitwidth (or bv constants
// argument).
template<NodeType node>
static rewriter::rule bvadd_rule(size_t bitwidth) {
	static std::map<size_t, rewriter::rule> cache;
	// if the rule is already computed for the given bitwidth, we return it from the cache
	if (cache.find(bitwidth) != cache.end()) return cache[bitwidth];
	// Otherwise, we compute the rule, store it in the cache and return it.
	// First we collect all the rules.
	rewriter::rules rs;
	rs.insert(rs.end(), bvadd_rules<node>(bitwidth).begin(), bvadd_rules<node>(bitwidth).end());
	// Then we build a main term to compute the actual predicate.
	auto left = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto right = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto result = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto head = make_bvadd_call_with_index<node>(left, right, result, bitwidth);
	auto temp = make_rr<node>(rs, head);
	auto body = apply_rr_to_formula(temp);
	if (!body) {
		DBG( LOG_DEBUG << "Failed to compute bvadd rule. Returning empty rule."; )
		return make_rule<node>(head, body);
	}
	cache[bitwidth] = make_rule<node>(head, body);
	return cache[bitwidth];
}

template<NodeType node>
static rewriter::rule bvsub_rule(size_t bitwidth) {
	static std::map<size_t, rewriter::rule> cache;
	// If the rule is already computed for the given bitwidth, we return it from the cache
	if (cache.find(bitwidth) != cache.end()) return cache[bitwidth];
	// Otherwise, we compute the rule, store it in the cache and return it.
	// First we collect all the rules.
	rewriter::rules rs;
	rs.insert(rs.end(), bvsub_rules<node>(bitwidth).begin(), bvsub_rules<node>(bitwidth).end());
	// Then we build a main term to compute the actual predicate.
	auto left = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto right = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto result = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto head = make_bvsub_call_with_index<node>(left, right, result, bitwidth);
	auto temp = make_rr<node>(rs, head);
	auto body = apply_rr_to_formula(temp);
	if (!body) {
		DBG( LOG_DEBUG << "Failed to compute bvsub rule. Returning empty rule."; )
		return make_rule<node>(head, body);
	}
	cache[bitwidth] = make_rule<node>(head, body);
	return cache[bitwidth];
}

template<NodeType node>
static rewriter::rule bvshl_by_one_rule(size_t bitwidth) {
	static std::map<size_t, rewriter::rule> cache;
	// If the rule is already computed for the given bitwidth, we return it from the cache
	if (cache.find(bitwidth) != cache.end()) return cache[bitwidth];
	// Otherwise, we compute the rule, store it in the cache and return it.
	// First we collect all the rules.
	rewriter::rules rs;
	rs.insert(rs.end(), bvshl_by_one_rules<node>(bitwidth).begin(), bvshl_by_one_rules<node>(bitwidth).end());
	for (size_t i = 0; i < bitwidth; ++i) {
		auto br = ::idni::tau_lang::bit_rule<node>(bitwidth, i);
		rs.insert(rs.end(), br);
	}
	// Then we build a main term to compute the actual predicate.
	auto operand = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto result = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto head = make_bvshl_by_one_call<node>(operand, result);
	auto temp = make_rr<node>(rs, head);
	auto body = apply_rr_to_formula(temp);
	if (!body) {
		DBG( LOG_DEBUG << "Failed to compute bvshl_by_one rule. Returning empty rule."; )
		return make_rule<node>(head, body);
	}
	cache[bitwidth] = make_rule<node>(head, body);
	return cache[bitwidth];
}

template<NodeType node>
static rewriter::rule bvrhl_by_one_rule(size_t bitwidth) {
	static std::map<size_t, rewriter::rule> cache;
	// If the rule is already computed for the given bitwidth, we return it from the cache
	if (cache.find(bitwidth) != cache.end()) return cache[bitwidth];
	// Otherwise, we compute the rule, store it in the cache and return it.
	// First we collect all the rules.
	rewriter::rules rs;
	rs.insert(rs.end(), bvrhl_by_one_rules<node>(bitwidth).begin(), bvrhl_by_one_rules<node>(bitwidth).end());
	for (size_t i = 0; i < bitwidth; ++i)
		rs.insert(rs.end(), bit_rule<node>(bitwidth, i));
	// Then we build a main term to compute the actual predicate.
	auto operand = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto result = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto head = make_bvrhl_by_one_call<node>(operand, result);
	auto temp = make_rr<node>(rs, head);
	auto body = apply_rr_to_formula(temp);
	if (!body) {
		DBG( LOG_DEBUG << "Failed to compute bvrhl_by_one rule. Returning empty rule."; )
		return make_rule<node>(head, body);
	}
	cache[bitwidth] = make_rule<node>(head, body);
	return cache[bitwidth];
}

template<NodeType node>
static rewriter::rule bvmul_rule(tref y, size_t bitwidth) {
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
			rs.insert(rs.end(), rule.begin(), rule.end());
			partial_cache[bitwidth][ny] = rule;
		}
		ny = shr_by_one<node>(ny);
	}
	// Then we build a main term to compute the actual predicate.
	auto left = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto result = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto main = make_bvmul_call<node>(left, y, result);
	auto temp = make_rr<node>(rs, main);
	auto body = apply_rr_to_formula(temp);
	if (!body) {
		DBG( LOG_DEBUG << "Failed to compute bvmul rule. Returning empty rule."; )
		return make_rule<node>(main, body);
	}
	cache[bitwidth][y] = make_rule<node>(main, body);
	return cache[bitwidth][y];
}

template<NodeType node>
static rewriter::rule bvdiv_rule(tref divisor /* bv copnstant */, size_t bitwidth) {
	static std::map<size_t, std::map<tref, rewriter::rule>> cache;
	// If the rule is already computed for the given bitwidth and right operand, we return it from the cache
	if (cache.find(bitwidth) != cache.end()) return cache[bitwidth][divisor];
	// Otherwise, we compute the rule, store it in the cache and return it.
	// First we collect all the rules needed for computing the euclidean
	// division: addition, multiplication, less then,...
	rewriter::rules rs;
	rs.insert(rs.end(), bvadd_rules<node>(bitwidth).begin(), bvadd_rules<node>(bitwidth).end());
	rs.insert(rs.end(), bvmul_rules<node>(bitwidth).begin(), bvmul_rules<node>(bitwidth).end());
	rs.insert(rs.end(), bvlt_rules<node>(bitwidth).begin(), bvlt_rules<node>(bitwidth).end());
	// Then we build the main term to compute the actual predicate.
	auto quotient = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto remainder = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto dividend = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto exact = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto main = tau::build_wff_ex(remainder,
		tau::build_wff_ex(exact,
			tau::build_wff_and({
				make_bvsub_call<node>(dividend, remainder, exact),
				make_bvadd_call<node>(quotient, divisor, exact),
				make_bvlt_call<node>(remainder, divisor, bitwidth)})));
	auto temp = make_rr<node>(rs, main);
	auto head = make_bvdiv_call<node>(dividend, divisor, quotient);
	auto body = apply_rr_to_formula(temp);
	if (!body) {
		DBG( LOG_DEBUG << "Failed to compute bvdiv rule. Returning empty rule."; )
		return make_rule<node>(head, body);
	}
	cache[bitwidth][divisor] = make_rule<node>(head, body);
	return cache[bitwidth][divisor];
}

template<NodeType node>
static rewriter::rule bvmod_rule(tref divisor /* bv copnstant */, size_t bitwidth) {
	static std::map<size_t, std::map<tref, rewriter::rule>> cache;
	// If the rule is already computed for the given bitwidth and right operand, we return it from the cache
	if (cache.find(bitwidth) != cache.end()) return cache[bitwidth][divisor];
	// Otherwise, we compute the rule, store it in the cache and return it.
	// First we collect all the rules needed for computing the euclidean
	// division: addition, multiplication, less then,...
	rewriter::rules rs;
	rs.insert(rs.end(), bvadd_rules<node>(bitwidth).begin(), bvadd_rules<node>(bitwidth).end());
	rs.insert(rs.end(), bvmul_rules<node>(bitwidth).begin(), bvmul_rules<node>(bitwidth).end());
	rs.insert(rs.end(), bvlt_rules<node>(bitwidth).begin(), bvlt_rules<node>(bitwidth).end());
	// Then we build the main term to compute the actual predicate.
	auto quotient = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto remainder = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto dividend = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto exact = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto main = tau::build_wff_ex(quotient,
		tau::build_wff_ex(exact,
			tau::build_wff_and({
				make_bvsub_call<node>(dividend, remainder, exact),
				make_bvadd_call<node>(quotient, divisor, exact),
				make_bvlt_call<node>(remainder, divisor, bitwidth)})));
	auto temp = make_rr<node>(rs, main);
	auto head = make_bvmod_call<node>(dividend, divisor, remainder);
	auto body = apply_rr_to_formula(temp);
	if (!body) {
		DBG( LOG_DEBUG << "Failed to compute bvmod rule. Returning empty rule."; )
		return make_rule<node>(head, body);
	}
	cache[bitwidth][divisor] = make_rule<node>(head, body);
	return cache[bitwidth][divisor];
}

template<NodeType node>
static rewriter::rule bvshl_rule(tref shift /* bv constant */, size_t bitwidth) {
	static std::map<size_t, std::map<tref, rewriter::rule>> cache;
	// If the rule is already computed for the given bitwidth and right operand, we return it from the cache
	if (cache.find(bitwidth) != cache.end()) return cache[bitwidth][shift];
	// Get bv constant value
	auto shift_value = get_bv_constant_value<node>(shift);
	// If the shift is greater or equal to the bitwidth, the result is always zero
	auto var = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto shifted = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto head = make_bvshl_call<node>(var, shift, shifted);
	if (shift_value >= bitwidth) {
		auto zero = tau::get(tau::bf, build_bv_zero<node>(bitwidth));
		auto body = tau::build_bf_eq(shifted, zero);
		cache[bitwidth][shift] = make_rule<node>(head, body);
		return cache[bitwidth][shift];
	}
	// Otherwise, we compute the rule, store it in the cache and return it.
	tref body = nullptr;
	for (size_t i = 0; i < bitwidth; ++i) {
		if (i + shift_value > bitwidth) {
			auto shifted_bit = tau::get(tau::bf, tau::build_rr_ref("_bit", { i }, { shifted }));
			auto zero = tau::get(tau::bf, build_bv_zero<node>(bitwidth));
			auto shift_eq = tau::build_bf_eq(shifted_bit, zero);
			body = body ? tau::build_wff_and(body, shift_eq) : shift_eq;
			continue;
		}
		auto bit = tau::get(tau::bf, tau::build_rr_ref("_bit", i, { var }));
		auto shifted_bit = tau::get(tau::bf, tau::build_rr_ref("_bit", i + shift_value, { shifted }));
		auto shift_eq = tau::build_bf_eq(shifted_bit, bit);
		body = body ? tau::build_wff_and(body, shift_eq) : shift_eq;
	}
	cache[bitwidth][shift] = make_rule<node>(head, body);
	return cache[bitwidth][shift];
}

template<NodeType node>
static rewriter::rule bvrhl_rule(tref shift /* bv constant */, size_t bitwidth) {
	static std::map<size_t, std::map<tref, rewriter::rule>> cache;
	// If the rule is already computed for the given bitwidth and right operand, we return it from the cache
	if (cache.find(bitwidth) != cache.end()) return cache[bitwidth][shift];
	// Unsupported operation for now
	// Get bv constant value
	auto shift_value = get_bv_constant_value<node>(shift);
	// If the shift is greater or equal to the bitwidth, the result is always zero
	auto var = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto shifted = tau::build_bf_variable(bv_type_id<node>(bitwidth));
	auto head = make_bvshl_call<node>(var, shift, shifted);
	if (shift_value >= bitwidth) {
		auto zero = tau::get(tau::bf, build_bv_zero<node>(bitwidth));
		auto body = tau::build_bf_eq(shifted, zero);
		cache[bitwidth][shift] = make_rule<node>(head, body);
		return cache[bitwidth][shift];
	}
	// Otherwise, we compute the rule, store it in the cache and return it.
	tref body = nullptr;
	for (size_t i = 0; i < bitwidth; ++i) {
		if (i - shift_value < 0) {
			auto shifted_bit = tau::get(tau::bf, tau::build_rr_ref("_bit", i, { shifted }));
			auto zero = tau::get(tau::bf, build_bv_zero<node>(bitwidth));
			auto shift_eq = tau::build_bf_eq(shifted_bit, zero);
			body = body ? tau::build_wff_and(body, shift_eq) : shift_eq;
			continue;
		}
		auto bit = tau::get(tau::bf, tau::build_rr_ref("_bit", i, { var }));
		auto shifted_bit = tau::get(tau::bf, tau::build_rr_ref("_bit", i - shift_value, { shifted }));
		auto bit_eq = tau::build_bf_eq(bit, shifted_bit);
		body = body ? tau::build_wff_and(body, bit_eq) : bit_eq;
	}
	cache[bitwidth][shift] = make_rule<node>(head, body);
	return cache[bitwidth][shift];
}

template<NodeType node>
tref bvadd(tref left, tref right, tref result) {
	auto bitwidth = get_bv_type_bitwidth<node>(left);
	auto rule = bvadd_rule<node>(bitwidth);
	auto call = make_bvadd_call<node>(left, right, result, bitwidth);
	return apply_rule(rule, call);
}

template<NodeType node>
tref bvmul([[maybe_unused]] tref left, [[maybe_unused]] tref right, [[maybe_unused]] tref var) {
	auto bitwidth = get_bv_type_bitwidth<node>(left);
	if (!is_bv_constant<node>(right)) {
		DBG( LOG_DEBUG << "Currently only multiplication by constant is supported in predicate blasting."; )
		return nullptr;
	}
	auto rule = bvmul_rule<node>(bitwidth);
	auto call = make_bvmul_call<node>(left, right, var, bitwidth);
	return apply_rule(rule, call);
}

template<NodeType node>
tref bvsub(tref left, tref right, tref result) {
	auto bitwidth = get_bv_type_bitwidth<node>(left);
	auto rule = bvsub_rule<node>(bitwidth);
	auto call = make_bvsub_call<node>(left, right, result, bitwidth);
	return apply_rule(rule, call);
}

template<NodeType node>
tref bvdiv(tref dividend, tref divisor, tref quotient) {
	auto bitwidth = get_bv_type_bitwidth<node>(divisor);
	if (!is_bv_constant<node>(divisor)) {
		DBG( LOG_DEBUG << "Currently only division by constant is supported in predicate blasting."; )
		return nullptr;
	}
	auto rule = bvdiv_rule<node>(divisor, bitwidth);
	auto call = make_bvdiv_call<node>(dividend, divisor, quotient, bitwidth);
	return apply_rule(rule, call);
}

template<NodeType node>
tref bvmod(tref dividend, tref divisor, tref remainder) {
	auto bitwidth = get_bv_type_bitwidth<node>(divisor);
	if (!is_bv_constant<node>(divisor)) {
		DBG( LOG_DEBUG << "Currently only division by constant is supported in predicate blasting."; )
		return nullptr;
	}
	auto rule = bvdiv_rule<node>(divisor, bitwidth);
	auto call = make_bvdiv_call<node>(dividend, divisor, remainder, bitwidth);
	return apply_rule(rule, call);
}

template<NodeType node>
tref bvrhl_one([[maybe_unused]] tref left, [[maybe_unused]] tref var) {
	auto bitwidth = get_bv_type_bitwidth<node>(left);
	auto predicate = bvrhl_by_one_rule<node>(bitwidth);
	auto call = make_bvrhl_by_one_call<node>(left, var, bitwidth);
	return apply_rule(predicate, call);
}

template<NodeType node>
tref bvshl_one([[maybe_unused]] tref left, [[maybe_unused]] tref var) {
	auto bitwidth = get_bv_type_bitwidth<node>(left);
	auto predicate = bvshl_by_one_rule<node>(bitwidth);
	auto call = make_bvshl_by_one_call<node>(left, var, bitwidth);
	return apply_rule(predicate, call);
}

template<NodeType node>
tref bvrhl(tref var, tref shift) {
	auto bitwidth = get_bv_type_bitwidth<node>(shift);
	auto predicate = bvrhl_rule<node>(shift, bitwidth);
	auto call = make_bvrhl<node>(var, shift, bitwidth);
	return apply_rule(predicate, call);
}

template<NodeType node>
tref bvshl(tref var, tref shift) {
	auto bitwidth = get_bv_type_bitwidth<node>(shift);
	auto predicate = bvshl_rule<node>(shift, bitwidth);
	auto call = make_bvshl<node>(var, shift, bitwidth);
	return apply_rule(predicate, call);
}

template <NodeType node>
std::pair<tref, std::optional<tref>> get_bvmul_arguments(tref term) {
	using tau = tree<node>;

	auto left = tau::get(term).child(0);
	auto right = tau::get(term).child(1);

	if (is_bv_constant<node>(left)) return std::make_pair(right, left);
	if (is_bv_constant<node>(right)) return std::make_pair(left, right);

	return std::make_pair(nullptr, nullptr);
}

template <NodeType node>
std::pair<tref, std::optional<tref>> get_bved_arguments(tref term) {
	using tau = tree<node>;

	auto left = tau::get(term).child(0);
	auto right = tau::get(term).child(1);

	if (is_bv_constant<node>(left)) return std::make_pair(right, left);

	return std::make_pair(nullptr, nullptr);
}

template<NodeType node>
tref bf_predicate_blasting(tref term, subtree_map<node, tref>& changes, trefs& vars) {
	using tau = tree<node>;

	tref predicate;
	bool error = false;
	auto type_id = tau::get(term).get_ba_type();

	auto f = [&](tref t) {
		auto nt = tau::get(t).get_type();

		switch (nt) {
			case tau::bf_add: case tau::bf_sub: {
				auto var = build_variable<node>(type_id);
				vars.push_back(var);
				changes[t] = var;
				auto left = changes[tau::get(t).child(0)];
				auto right = changes[tau::get(t).child(1)];
				auto current = (nt == tau::bf_add)
					? bvadd<node>(left, right, var)
					: bvsub<node>(left, right, var);
				predicate = predicate
					? build_bf_and<node>(predicate, current)
					: current;
				break;
			}
			case tau::bf_shl: case tau::bf_shr: {
				auto [shiftand, count] = get_bvsh_arguments<node>(t);
				if (!count) break;
				auto result = build_bf_var<node>(type_id);
				vars.push_back(result);
				auto left = tau::get(t).child(0);
				changes[t] = result;
				auto current = (nt == tau::bf_shl)
					? bvshl<node>(left, count, result)
					: bvrhl<node>(left, count, result);
				predicate = predicate
					? build_bf_and<node>(predicate, current)
					: current;
				break;
			}
			case tau::bf_mul: {
				auto [factor, constant] = get_bvmul_arguments<node>(t);
				if (!constant) break;
				auto product = build_bf_var<node>(type_id);
				vars.push_back(product);
				changes[t] = product;
				predicate = predicate
					? build_bf_and<node>(predicate, bvmul<node>(factor, constant.value(), product))
					: bvmul<node>(factor, constant, product);
				break;
			}
			case tau::bf_div: case tau::bf_mod: {
				auto [dividend, divisor] = get_bved_arguments<node>(t);
				if (!divisor) break;
				auto result = build_bf_var<node>(type_id);
				vars.push_back(result);
				changes[t] = result;
				auto current = (nt == tau::bf_mod)
					? bvmod<node>(dividend, divisor, result)
					: bvdiv<node>(dividend, divisor, result);
				predicate = predicate
					? build_bf_and<node>(predicate, current)
					: current;
				break;
			}
			// Nand, nor and xnor are treated in the hooks so we never get those
			// cases at this point.
			default: {
				trefs ch;
				for (tref c : tau::get(t).children()) {
					if (changes.find(c) != changes.end())
						ch.push_back(changes[c]);
					else ch.push_back(c);
				}

				if (auto new_n = tau::get(tau::get(t).value, ch.data(), ch.size()); new_n != t)
					changes[t] = new_n;
				break;
			}
		}
		return true;
	};

	// If we have an unsupported operation, we return nullptr to indicate failure
	if (error) return nullptr;
	post_order<node>(term).search_unique(f);
	auto modified = rewriter::replace<node>(term, changes);
	// We add the existentials for the variables we introduced
	for (auto it = vars.rbegin(); it != vars.rend(); ++it) {
		predicate = build_wff_ex<node>(*it, predicate);
	}

	return (predicate)
		? tau::build_wff_and(predicate, modified)
		: term;
}


template<NodeType node>
tref eq_predicate(tref atomic) {
	// TODO (HIGH) add simplifications to avoid the top level variable if possible
	subtree_map<node, tref> changes;
	trefs vars;

	return bf_predicate_blasting<node>(atomic, changes, vars);
}

template<NodeType node>
tref neq_predicate([[maybe_unused]] tref n) {
	using tau = tree<node>;

	return tau::build_wff_neg(eq_predicate<node>(n));
}

template<NodeType node>
tref lt_predicate(tref atomic) {
	subtree_map<node, tref> changes;
	trefs vars;

	auto bitwidth = get_bv_type_bitwidth<node>(atomic);
	auto predicate = bvlt_rule<node>(bitwidth);
	auto blasted = bf_predicate_blasting<node>(atomic, changes, vars);
	if (!blasted) return nullptr;
	auto left = tau::get(blasted).child(0);
	auto right = tau::get(blasted).child(1);
	auto call = make_bvlt<node>(left, right, bitwidth);
	return apply_rule(predicate, call);
}

template<NodeType node>
tref gt_predicate(tref atomic) {
	subtree_map<node, tref> changes;
	trefs vars;

	auto bitwidth = get_bv_type_bitwidth<node>(atomic);
	auto predicate = bvgt_rule<node>(bitwidth);
	auto blasted = bf_predicate_blasting<node>(atomic, changes, vars);
	if (!blasted) return nullptr;
	auto left = tau::get(blasted).child(0);
	auto right = tau::get(blasted).child(1);
	auto call = make_bvgt<node>(left, right, bitwidth);
	return apply_rule(predicate, call);
}

template<NodeType node>
tref lteq_predicate(tref atomic) {
	using tau = tree<node>;

	return tau::build_wff_neg(gt_predicate<node>(atomic));
}

template<NodeType node>
tref gteq_predicate(tref atomic) {
	using tau = tree<node>;

	return tau::build_wff_neg(lt_predicate<node>(atomic));
}

template<NodeType node>
tref nlt_predicate(tref atomic) {
	return gteq_predicate<node>(atomic);
}

template<NodeType node>
tref ngt_predicate(tref atomic) {
	return lteq_predicate<node>(atomic);
}

template<NodeType node>
tref nlteq_predicate(tref atomic) {
	return gt_predicate<node>(atomic);
}

template<NodeType node>
tref ngteq_predicate(tref atomic) {
	return lt_predicate<node>(atomic);
}

template<NodeType node>
tref atomic_predicate_blasting(tref atomic) {

	// We only blast atomic predicates over bitvector terms. If the atomic formula
	// is not over bitvectors, we return it unchanged.
	auto type = tau::get(atomic).get_ba_type();
	if(!is_bv_type_family<node>(type)) return atomic;

	auto nt = tau::get(atomic).get_type();
	subtree_map<node, tref> changes;
	trefs vars;
	tref predicate;

	switch (nt) {
		case tau::bf_eq: { predicate = eq_predicate<node>(atomic); break; }
		case tau::bf_neq: { predicate = neq_predicate<node>(atomic); break; }
		case tau::bf_lt: { predicate = lt_predicate<node>(atomic); break; }
		case tau::bf_gt: { predicate = gt_predicate<node>(atomic); break; }
		case tau::bf_lteq: { predicate = lteq_predicate<node>(atomic); break; }
		case tau::bf_gteq: { predicate = gteq_predicate<node>(atomic); break; }
		case tau::bf_nlt: { predicate = nlt_predicate<node>(atomic); break; }
		case tau::bf_ngt: { predicate = ngt_predicate<node>(atomic); break; }
		case tau::bf_nlteq: { predicate = nlteq_predicate<node>(atomic); break; }
		case tau::bf_ngteq: { predicate = ngteq_predicate<node>(atomic); break; }
		default: {
			// error, unknown atomic predicate, we return nullptr to indicate failure
			DBG( LOG_DEBUG << "Unknown atomic predicate in blasting: " << LOG_NT(nt) << ". It will be left unchanged."; )
			return nullptr;
		}
	}

	if (!predicate) {
		DBG( LOG_DEBUG << "Failed to compute predicate in blasting: " << LOG_FM(atomic) << ". It will be left unchanged."; )
		return atomic;
	}

	return predicate;
}

template<NodeType node>
tref wff_predicate_blasting(tref term) {
	using tau = tree<node>;

	subtree_map<node, tref> changes;

	auto f = [&](tref t) {
		auto nt = tau::get(t).get_type();

		switch (nt) {
			case tau::bf_lt: case tau::bf_gt: case tau::bf_lteq: case tau::bf_gteq:
			case tau::bf_nlt: case tau::bf_ngt: case tau::bf_nlteq: case tau::bf_ngteq:
			case tau::bf_eq: case tau::bf_neq: {
				changes[t] = atomic_predicate_blasting<node>(t);
				break;
			}
			default: {
				trefs ch;
				for (tref c : tau::get(t).children()) {
					if (changes.find(c) != changes.end())
						ch.push_back(changes[c]);
					else ch.push_back(c);
				}

				if (auto new_n = tau::get(tau::get(t).value, ch.data(), ch.size()); new_n != t)
					changes.insert_or_assign(t, new_n);
				break;
			}
		}
		return true;
	};

	post_order<node>(term).search_unique(f);

	auto n_term = changes.find(term) != changes.end() ? changes[term] : term;
	DBG(LOG_TRACE << "bv_predicate_blasting/wff/n_term: " << LOG_FM(n_term);)

	return n_term;
}

template<NodeType node>
tref bv_blasting(tref term) {
	return wff_predicate_blasting<node>(term);
}


} // namespace idni::tau_lang