// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_bitblasting"

namespace idni::tau_lang {

using bool_node = idni::tau_lang::node<cvc5::Term, Bool>;
using bool_tree = tree<bool_node>;
static auto bool_id = bool_type_id<bool_node>();

// This is a factory for creating the rules needed for each bitvector operation.
// We also include factory methods for creating the calls to easy the creation
// of the predicates.
template<NodeType node>
struct bv_bitblasting_rules {

	// addition(x, y) = addition(x ^ y, (x & y) << 1, m) mod m;
	static rewriter::rules bvadd(size_t bitwidth) {
		using tau = tree<node>;

		rewriter::rules rules;
		// general case: addition[n](x, y, z) = ex w addition[n-1](x ^ y, w, z) && shr_by_one(x & y, w);
		auto x = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto y = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto z = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto n = tau::build_variable(untyped_type_id<node>());
		auto n_minus_1 = tau::build_ref_shift_offset(n, 1);
		auto general_header = tau::build_ref("_bvadd", { n }, { x, y, z });
		auto w = tau::build_variable(bv_type_id<node>(bitwidth));
		auto bf_w = tau::get(tau::bf, w);
		auto general_body = tau::build_wff_ex(w,
			tau::build_wff_and(
				make_bvadd_call(tau::build_bf_xor(x, y), bf_w, z, n_minus_1),
				make_bvshl_by_one_call(tau::build_bf_and(x, y), bf_w)
		));

		// base case: addition[0](x, y, z) = (z = x);
		auto base_header = make_bvadd_call_with_index(x, y, z, 0);
		auto base_body = tau::build_bf_eq(x, z);

		// create rules
		rules.push_back(rewriter::rule(tau::geth(base_header), tau::geth(base_body)));
		rules.push_back(rewriter::rule(tau::geth(general_header), tau::geth(general_body)));
		return rules;
	}

	// subtraction(x,y) = subtraction(x ^ y, (~x & y) << 1, m) mod m;
	static rewriter::rules bvsub(size_t bitwidth) {
		using tau = tree<node>;

		rewriter::rules rules;
		// general case: subtraction[n](x, y, z) = ex w subtraction[n-1](x ^ y, w, z) && shr_by_one((~x & y), w);
		auto x = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto y = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto z = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto n = tau::build_variable(untyped_type_id<node>());
		auto n_minus_1 = tau::build_ref_shift_offset(n, 1);
		auto general_header = tau::build_ref("_bvsub", { n }, { x, y, z });
		auto w = tau::build_variable(bv_type_id<node>(bitwidth));
		auto bf_w = tau::get(tau::bf, w);
		auto general_body = tau::build_wff_ex(w,
			tau::build_wff_and(
				make_bvsub_call(tau::build_bf_xor(x, y), bf_w, z, n_minus_1),
				make_bvshl_by_one_call(tau::build_bf_and(tau::build_bf_neg(x), y), bf_w)
		));

		// base case: subtraction[0](x, y, z) = (z = x);
		auto base_header = make_bvsub_call_with_index(x, y, z, 0);
		auto base_body = tau::build_bf_eq(x, z);
		rules.push_back(rewriter::rule(tau::geth(base_header), tau::geth(base_body)));
		rules.push_back(rewriter::rule(tau::geth(general_header), tau::geth(general_body)));
		return rules;
	}

	static rewriter::rules bvshl_by_one(size_t bitwidth) {
		using tau = tree<node>;

		// bvshl_by_one(x, y) = (bit[0](y) = 0) && (bit[1](y) = bit[0](x)) && (bit[2](y) = bit[1](x)) && ... && (bit[n](y) = bit[n-1](x))
		auto var = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto result = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto header = tau::build_ref_with_indexes("_bvshl_by_one", {}, { var, result });
		// the right most bit is zero, the rest of the bits are the same as the original variable shifted by one
		auto result_bit_bitwidth = tau::get(tau::bf, tau::build_ref_with_indexes("_bit", { bitwidth - 1 }, { result }));
		static auto bit_width = tau::_0(bv_type_id<node>(bitwidth));
		tref body = tau::build_bf_eq(bit_width, result_bit_bitwidth);
		for (size_t i = 1; i < bitwidth; ++i) {
			auto bit_i = tau::get(tau::bf, tau::build_ref_with_indexes("_bit", { i }, { var }));
			auto bit_i_minus_1 = tau::get(tau::bf, tau::build_ref_with_indexes("_bit", { i - 1 }, { result }));
			auto bit_i_eq_bit_i_minus_1 = tau::build_bf_eq(bit_i, bit_i_minus_1);
			body = body ? tau::build_wff_and(body, bit_i_eq_bit_i_minus_1) : bit_i_eq_bit_i_minus_1;
		}
		return {rewriter::rule( tau::geth(header), tau::geth(body) )};
	}


	static rewriter::rules bvrhl_by_one(size_t bitwidth) {
		using tau = tree<node>;

		// bvrhl_by_one(x, y) = (bit[n-1](y) = 0) && (bit[0](y) = bit[1](x)) && (bit[1](y) = bit[2](x)) && ... && (bit[n-2](y) = bit[n-1](x))
		auto var = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto result = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto header = tau::build_ref_with_indexes("_bvrhl_by_one", {}, { var, result });
		// the left most bit is zero, the rest of the bits are the same as the original variable shifted by one
		auto result_bit_0 = tau::get(tau::bf, tau::build_ref_with_indexes("_bit", { 0 }, { result }));
		static auto bit_width_minus_1 = tau::_0(bv_type_id<node>(bitwidth));
		tref body = tau::build_bf_eq(bit_width_minus_1, result_bit_0);
		for (size_t i = 0; i < bitwidth - 1; ++i) {
			auto bit_i = tau::get(tau::bf, tau::build_ref_with_indexes("_bit", { i }, { var }));
			auto bit_i_plus_1 = tau::get(tau::bf, tau::build_ref_with_indexes("_bit", { i + 1 }, { result }));
			auto bit_i_eq_bit_i_plus_1 = tau::build_bf_eq(bit_i, bit_i_plus_1);
			body = body ? tau::build_wff_and(body, bit_i_eq_bit_i_plus_1) : bit_i_eq_bit_i_plus_1;
		}
		return {rewriter::rule( tau::geth(header), tau::geth(body) )};
	}

	static rewriter::rule bvmul(tref y /* cvc5 constant */, size_t bitwidth) {
		using tau = tree<node>;

		DBG( assert(is_bv_constant<node>(y)); )

		auto x = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto z = tau::build_bf_variable(bv_type_id<node>(bitwidth));

		// multiplication[n](x, 0, z) = (z = 0) or multiplication[n](0, x, z) = (z = 0);
		if (is_zero_bv_constant<node>(y)) {
			auto zero_right = tau::build_ref_with_indexes("_bvmul", { x, y, z });
			auto body = tau::build_bf_eq(tau::_0(bv_type_id<node>(bitwidth)), z);
			return rewriter::rule(tau::geth(zero_right), tau::geth(body));
		}

		auto w = tau::build_variable(bv_type_id<node>(bitwidth));

		// case y & 1 = 1: multiplication(x, y, w) = ex z ex v multiplication(x << 1, v) && addition(x, v, z);
		if (lsb<node>(y)) {
			auto v = tau::build_bf_variable(bv_type_id<node>(bitwidth));
			auto odd_right = tau::build_ref_with_indexes("_bvmul", { x, y, z });
			auto odd_body = tau::build_wff_ex(w,
				tau::build_wff_ex(w,
					tau::build_wff_and(
						tau::build_wff_ex(v,
							tau::build_wff_and(
								tau::build_wff_and(
									make_bvshl_by_one_call(x, v)),
									make_bvmul_call(v, shr_by_one<node>(y), w)),
								make_bvadd_call(x, w, z)
			))));
			return rewriter::rule(tau::geth(odd_right), tau::geth(odd_body));
		}

		// case y & 1 = 0: multiplication(x, y, z) = ex v multiplication[y >> 1](x << 1, v) && (z = v);
		auto even_right = tau::build_ref_with_indexes("_bvmul", { x, y, z });
		auto even_body = tau::build_wff_ex(w,
			tau::build_wff_and(
				make_bvmul_call(tau::build_bf_shl(x), shr_by_one<node>(y), w),
				tau::build_bf_eq(z, w)
		));
		return rewriter::rule(tau::geth(even_right), tau::geth(even_body));
	}

	static rewriter::rules bvdiv([[maybe_unused]] size_t bitwidth) {
		// Unsupported operation for now
		LOG_ERROR << "Not yet implemented.";
		return rewriter::rules();
	}

	static rewriter::rules bvmod([[maybe_unused]] size_t bitwidth) {
		// Unsupported operation for now
		LOG_ERROR << "Not yet implemented.";
		return rewriter::rules();
	}

	static rewriter::rules bvshl([[maybe_unused]] size_t bitwidth) {
		// Unsupported operation for now
		LOG_ERROR << "Not yet implemented.";
		return rewriter::rules();
	}

	static rewriter::rules bvrhl([[maybe_unused]] size_t bitwidth) {
		// Unsupported operation for now
		LOG_ERROR << "Not yet implemented.";
		return rewriter::rules();
	}

	static rewriter::rule bvlt([[maybe_unused]] size_t index, [[maybe_unused]] size_t bitwidth) {
		// Unsupported operation for now
		LOG_ERROR << "Not yet implemented.";
		return rewriter::rule();
	}

	static rewriter::rules bvlt(size_t bitwidth) {
		using tau = tree<node>;

		auto x = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto y = tau::build_bf_variable(bv_type_id<node>(bitwidth));

		rewriter::rules rules;

		// base case: bvlt[0](x, y) = F;
		auto base_header = tau::build_ref_with_indexes("_bvlt", { 0 }, { x, y });
		auto base_body = tau::_F(tau_type_id<node>());
		rules.push_back(rewriter::rule(tau::geth(base_header), tau::geth(base_body)));
		// general case: bvlt[n](x, y) = (bit[n](x) = 0) && (bit[n](y) = 1) || ((bit[n](x) = bit[n](y)) && bvlt[n-1](x, y));
		auto n = tau::build_variable(untyped_type_id<node>());
		auto n_minus_1 = tau::build_ref_shift_offset(n, 1);
		auto general_header = make_bvlt_call_with_index(x, y, n, bitwidth);
		auto general_body = tau::build_wff_or(
			tau::build_wff_and(
				make_bvlt_call_with_offset(x, y, n, bitwidth),
				tau::build_bf_eq(
					make_bit_call(x, n),
					make_bit_call(y, n))),
			tau::build_wff_and(
				make_is_bit_zero_call(x, n),
				make_is_bit_one_call(y, n)));
		rules.push_back(rewriter::rule(tau::geth(general_header), tau::geth(general_body)));
		return rules;
	}

	static rewriter::rules bvgt(size_t bitwidth) {
		using tau = tree<node>;

		auto x = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto y = tau::build_bf_variable(bv_type_id<node>(bitwidth));

		rewriter::rules rules;

		// base case: bvgt[0](x, y) = F;
		auto base_header = make_bvgt_call(x, y, bitwidth);
		auto base_body = tau::_F(tau_type_id<node>());
		rules.push_back(rewriter::rule(tau::geth(base_header), tau::geth(base_body)));
		// general case: bvgt[n](x, y) = (bit[n-1](x) = 1) && (bit[n-1](y) = 0) || ((bit[n-1](x) = bit[n-1](y)) && bvgt[n-1](x, y));
		auto n = tau::build_variable(untyped_type_id<node>());
		auto n_minus_1 = tau::build_ref_shift_offset(n, 1);
		auto general_header = make_bvgt_call_with_index(x, y, n);
		auto general_body = tau::build_wff_or(
			tau::build_wff_and(
				make_bvgt_call_with_offset(x, y, n_minus_1),
				tau::build_bf_eq(
					make_bit_call(x, n),
					make_bit_call(y, n))),
			tau::build_wff_and(
				make_is_bit_one_call(x, n),
				make_is_bit_zero_call(y, n)));
		rules.push_back(rewriter::rule(tau::geth(general_header), tau::geth(general_body)));
		return rules;
	}

	static rewriter::rule bit(size_t bitwidth, size_t bit) {
		using tau = tree<node>;

		auto bit_offset = tau::get_num(bit);
		auto bit_cte =
			tau::get(tau::bf,
				tau::get_ba_constant(
					make_bitvector_value(1 <<bit, bitwidth)), bv_type_id<node>(bitwidth));
		auto var = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto header = tau::build_ref_with_indexes("_bit", { bit_offset }, { var });
		auto body = tau::build_bf_and( var, bit_cte);
		return rewriter::rule(header, body);
	}

	static rewriter::rule is_bit_zero(size_t bitwidth, size_t bit) {
		using tau = tree<node>;

		auto bit_offset = tau::get_num(bit);
		auto bit_cte =
			tau::get(tau::bf,
				tau::get_ba_constant(
					make_bitvector_value(1 << bit, bitwidth)), bv_type_id<node>(bitwidth));
		auto var = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto header = tau::build_ref_with_indexes("_is_bit_zero", { bit_offset }, { var });
		auto body =	tau::build_bf_eq_0(tau::build_bf_and(var, bit_cte));
		return rewriter::rule(header, body);
	}

	static rewriter::rule is_bit_one(size_t bitwidth, size_t bit) {
		using tau = tree<node>;

		auto bit_offset = tau::get_num(bit);
		auto bit_cte =
			tau::get(tau::bf,
				tau::get_ba_constant(
					make_bitvector_value(1 << bit, bitwidth)), bv_type_id<node>(bitwidth));
		auto var = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto header = tau::build_ref_with_indexes("_is_bit_one", { bit_offset }, { var });
		auto body =	tau::build_bf_eq(tau::build_bf_and(var, bit_cte), bit_cte);
		return rewriter::rule(header, body);
	}

	static tref make_bvadd_call_with_index(tref left, tref right, tref result, size_t index) {
		using tau = tree<node>;

		return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref_with_indexes("_bvadd", { index }, { left, right, result })));
	}

	static tref make_bvadd_call(tref left, tref right, tref result, tref offset) {
		using tau = tree<node>;

		return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvadd", { offset }, { left, right, result })));
	}

	static tref make_bvsub_call_with_index(tref left, tref right, tref result, size_t index) {
		using tau = tree<node>;

		return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref_with_indexes("_bvsub", { index }, { left, right, result })));
	}

	static tref make_bvsub_call(tref left, tref right, tref result, tref offset) {
		using tau = tree<node>;

		return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvsub", { offset }, { left, right, result })));
	}

	static tref make_bvshl_by_one_call(tref operand,  tref shifted) {
		using tau = tree<node>;

		return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvshl_by_one", {}, { operand, shifted })));
	}

	static tref make_bvrhl_by_one_call([[maybe_unused]] tref operand) {
		using tau = tree<node>;

		return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvrhl_by_one", {}, { operand })));
	}

	static tref make_bvmul_call(tref left, size_t right, tref result) {
		using tau = tree<node>;

		return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref_with_indexes("_bvmul", { right }, { left, result })));
	}

	static tref make_euclidean_division_call(tref dividend, tref divisor, tref quotient, tref remainder) {
		using tau = tree<node>;

		return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_euclidean_division", { dividend, divisor, quotient, remainder })));
	}

	static tref make_bvdiv_call(tref dividend, tref divisor, tref result) {
		using tau = tree<node>;

		return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvdiv", { dividend, divisor, result })));
	}

	static tref make_bvmod_call(tref dividend, tref divisor, tref result) {
		using tau = tree<node>;

		return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvmod", { dividend, divisor, result })));
	}

	static tref make_bvshl_call(tref left, tref right /* bv constant */, tref result) {
		using tau = tree<node>;

		DBG( assert(is_bv_constant<node>(right)); )

		return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvshl", { left, right, result })));
	}

	static tref make_bvrhl_call(tref left, tref right /* bv copnstant */, tref result) {
		using tau = tree<node>;

		DBG( assert(is_bv_constant<node>(right)); )

		return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_bvrhl", { left, right, result })));
	}

	static tref make_bit_call(tref operand, size_t bit) {
		using tau = tree<node>;

		return tau::get(tau::bf, tau::get(tau::bf_ref, tau::build_ref("_bit", { bit }, { operand })));
	}

	static tref make_is_bit_zero_call(tref operand, size_t bit) {
		using tau = tree<node>;

		return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_is_bit_zero", { bit }, { operand })));
	}

	static tref make_is_bit_one_call(tref operand, size_t bit) {
		using tau = tree<node>;

		return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref("_is_bit_one", { bit }, { operand })));
	}

	static tref make_bvlt_call(tref left, tref right, size_t bitwidth) {
		using tau = tree<node>;

		return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref_with_indexes("_bvlt", { bitwidth }, { left, right })));
	}

	static tref make_bvgt_call(tref left, tref right, size_t bitwidth) {
		using tau = tree<node>;

		return tau::get(tau::wff, tau::get(tau::wff_ref, tau::build_ref_with_indexes("_bvgt", { bitwidth }, { left, right })));
	}

};

// This is a factory for creating the predicates needed for each bitvector operation.
// We also include factory methods for creating the calls to easy the creation of
// calls. We aplly the above rules to create the final predicates
// for the bitvector operations.
template<NodeType node>
struct bv_bitblasting_predicates {

	static rewriter::rule bvadd_predicate(size_t bitwidth) {
		static std::map<size_t, rewriter::rule> cache;
		// if the rule is already computed for the given bitwidth, we return it from the cache
		if (cache.find(bitwidth) != cache.end()) return cache[bitwidth];
		// Otherwise, we compute the rule, store it in the cache and return it.
		// First we collect all the rules.
		rewriter::rules rs;
		rs.insert(bv_bitblasting_rules<node>::bvadd(bitwidth));
		// Then we build a main term to compute the actual predicate.
		auto left = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto right = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto result = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto head = make_bvadd_call(left, right, result, bitwidth);
		rr<node> temp{rs, head};
		auto body = apply_rr_to_formula(temp);
		if (!body) {
			LOG_ERROR << "Failed to compute bvadd predicate.";
			return rewriter::rule();
		}
		cache[bitwidth] = rewriter::rule(head, body);
		return cache[bitwidth];
	}

	static rewriter::rule bvsub_predicate(size_t bitwidth) {
		static std::map<size_t, rewriter::rule> cache;
		// If the rule is already computed for the given bitwidth, we return it from the cache
		if (cache.find(bitwidth) != cache.end()) return cache[bitwidth];
		// Otherwise, we compute the rule, store it in the cache and return it.
		// First we collect all the rules.
		rewriter::rules rs;
		rs.insert(bv_bitblasting_rules<node>::bvsub(bitwidth));
		// Then we build a main term to compute the actual predicate.
		auto left = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto right = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto result = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto head = make_bvsub_call(left, right, result, bitwidth);
		rr<node> temp{rs, head};
		auto body = apply_rr_to_formula(temp);
		if (!body) {
			LOG_ERROR << "Failed to compute bvsub predicate.";
			return rewriter::rule();
		}
		cache[bitwidth] = rewriter::rule(head, body);
		return cache[bitwidth];
	}

	static rewriter::rule bvshl_by_one_predicate(size_t bitwidth) {
		static std::map<size_t, rewriter::rule> cache;
		// If the rule is already computed for the given bitwidth, we return it from the cache
		if (cache.find(bitwidth) != cache.end()) return cache[bitwidth];
		// Otherwise, we compute the rule, store it in the cache and return it.
		// First we collect all the rules.
		rewriter::rules rs;
		rs.insert(bv_bitblasting_rules<node>::bvshl_by_one(bitwidth));
		for (size_t i = 0; i < bitwidth; ++i)
			rs.insert(bv_bitblasting_rules<node>::bit(bitwidth, i));
		// Then we build a main term to compute the actual predicate.
		auto operand = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto result = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto head = make_bvshl_by_one_call(operand, result, bitwidth);
		rr<node> temp{rs, head};
		auto body = apply_rr_to_formula(temp);
		if (!body) {
			LOG_ERROR << "Failed to compute bvshl_by_one predicate.";
			return rewriter::rule();
		}
		cache[bitwidth] = rewriter::rule(head, body);
		return cache[bitwidth];
	}

	static rewriter::rule bvrhl_by_one_predicate(size_t bitwidth) {
		static std::map<size_t, rewriter::rule> cache;
		// If the rule is already computed for the given bitwidth, we return it from the cache
		if (cache.find(bitwidth) != cache.end()) return cache[bitwidth];
		// Otherwise, we compute the rule, store it in the cache and return it.
		// First we collect all the rules.
		rewriter::rules rs;
		rs.insert(bv_bitblasting_rules<node>::bvrhl_by_one(bitwidth));
		for (size_t i = 0; i < bitwidth; ++i)
			rs.insert(bv_bitblasting_rules<node>::bit(bitwidth, i));
		// Then we build a main term to compute the actual predicate.
		auto operand = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto result = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto head = make_bvrhl_by_one_call(operand, result, bitwidth);
		rr<node> temp{rs, head};
		auto body = apply_rr_to_formula(temp);
		if (!body) {
			LOG_ERROR << "Failed to compute bvrhl_by_one predicate.";
			return rewriter::rule();
		}
		cache[bitwidth] = rewriter::rule(head, body);
		return cache[bitwidth];
	}


	static rewriter::rule bvmul_predicate(tref y, size_t bitwidth) {
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
				auto rule = bv_bitblasting_rules<node>::bvmul(ny, bitwidth);
				rs.insert(rule);
				partial_cache[bitwidth][ny] = { rule };
			}
			ny = shr_by_one<node>(ny);
		}
		// Then we build a main term to compute the actual predicate.
		auto left = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto result = tau::build_bf_variable(bv_type_id<node>(bitwidth));
		auto head = make_bvmul_call(left, y, result);
		rr<node> temp{rs, head};
		auto body = apply_rr_to_formula(temp);
		if (!body) {
			LOG_ERROR << "Failed to compute bvmul predicate.";
			return rewriter::rule();
		}
		cache[bitwidth][y] = rewriter::rule(head, body);
		return cache[bitwidth][y];

	}

	static rewriter::rule bvdiv_predicate([[maybe_unused]] size_t bitwidth) {
		// Unsupported operation for now
		LOG_ERROR << "Not yet implemented.";
		return rewriter::rule();
	}

	static rewriter::rule bvmod_predicate([[maybe_unused]] size_t bitwidth) {
		// Unsupported operation for now
		LOG_ERROR << "Not yet implemented.";
		return rewriter::rule();
	}

	static rewriter::rule bvshl_predicate([[maybe_unused]] size_t bitwidth) {
		// Unsupported operation for now
		LOG_ERROR << "Not yet implemented.";
		return rewriter::rule();
	}

	static rewriter::rule bvrhl_predicate([[maybe_unused]] size_t bitwidth) {
		// Unsupported operation for now
		LOG_ERROR << "Not yet implemented.";
		return rewriter::rule();
	}

	static rewriter::rule bvlt_predicate(size_t bitwidth) {
		return bv_bitblasting_rules<node>::bvlt(bitwidth);
	}

	static rewriter::rule bvgt_predicate(size_t bitwidth) {
		return bv_bitblasting_rules<node>::bvgt(bitwidth);
	}
};


template<NodeType node>
tref bvadd_predicate(tref left, tref right, tref result) {
	auto bitwidth = get_bv_type_bitwidth<node>(tau::get(left));
	auto predicate = bv_bitblasting_predicates<node>::bvadd_predicate(bitwidth);
	auto call = bv_bitblasting_rules<node>::make_bvadd_call(left, right, result, bitwidth);
	return apply_rule(predicate, call);
}

template<NodeType node>
tref bvmul_predicate([[maybe_unused]] tref left, [[maybe_unused]] tref right, [[maybe_unused]] tref var) {
	auto bitwidth = get_bv_type_bitwidth<node>(tau::get(left));
	if (!is_bv_constant<node>(right)) {
		LOG_ERROR << "Currently only multiplication by constant is supported in predicate blasting.";
		return nullptr;
	}
	auto predicate = bv_bitblasting_predicates<node>::bvmul_predicate(bitwidth);
	auto call = bv_bitblasting_rules<node>::make_bvmul_call(left, right, var, bitwidth);
	return apply_rule(predicate, call);

}

template<NodeType node>
tref bvsub_predicate(tref left, tref right, tref result) {
	auto bitwidth = get_bv_type_bitwidth<node>(tau::get(left));
	auto predicate = bv_bitblasting_predicates<node>::bvsub_predicate(bitwidth);
	auto call = bv_bitblasting_rules<node>::make_bvsub_call(left, right, result, bitwidth);
	return apply_rule(predicate, call);
}

template<NodeType node>
tref bvdiv_predicate([[maybe_unused]] tref left, [[maybe_unused]] tref right, [[maybe_unused]] tref var) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref bvrhl_one_predicate([[maybe_unused]] tref left, [[maybe_unused]] tref var) {
	auto bitwidth = get_bv_type_bitwidth<node>(tau::get(left));
	auto predicate = bv_bitblasting_predicates<node>::bvrhl_by_one_predicate(bitwidth);
	auto call = bv_bitblasting_rules<node>::make_bvrhl_by_one_call(left, var, bitwidth);
	return apply_rule(predicate, call);
}

template<NodeType node>
tref bvshl_one_predicate([[maybe_unused]] tref left, [[maybe_unused]] tref var) {
	auto bitwidth = get_bv_type_bitwidth<node>(tau::get(left));
	auto predicate = bv_bitblasting_predicates<node>::bvshl_by_one_predicate(bitwidth);
	auto call = bv_bitblasting_rules<node>::make_bvshl_by_one_call(left, var, bitwidth);
	return apply_rule(predicate, call);
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
					? bvadd_predicate<node>(left, right, var)
					: bvsub_predicate<node>(left, right, var);
				predicate = predicate
					? build_bf_and<node>(predicate, current)
					: current;
				break;
			}
			case tau::bf_shl: case tau::bf_shr: {
				// TODO (HIGH) check for 1 in the by one shift
				// operations and for general case when we support it.
				auto var = build_bf_var<node>(type_id);
				vars.push_back(var);
				auto left = tau::get(t).child(0);
				changes[t] = var;
				auto current = (nt == tau::bf_shl)
					? bvshl_one_predicate<node>(left, var)
					: bvrhl_one_predicate<node>(left, var);
				predicate = predicate
					? build_bf_and<node>(predicate, current)
					: current;
				break;
			}
			case tau::bf_mul: case tau::bf_div: case tau::bf_mod: {
				// Unsupported operation for now
				LOG_ERROR << "Operation "
					<< LOG_NT(nt) << " not supported yet in predicate blasting.";
				return error = true, false;
			}
			case tau::bf_nand: case tau::bf_xnor: case tau::bf_nor:{
				// Unsupported operation for now. They must be replaced by their
				// equivalent in terms of and, or and not for blasting.
				LOG_ERROR << "Operation "
					<< LOG_NT(nt) << " not supported yet in predicate blasting.";
				return error = true, false;
			}
			default: {
				break;
			}
		}
		return true;
	};

	// if we have an unsupported operation, i.e. we have an unsupported operation
	//we return nullptr to indicate failure
	if (error) return nullptr;
	return predicate;
}


template<NodeType node>
tref eq_predicate([[maybe_unused]] tref n, [[maybe_unused]] trefs& vars) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref neq_predicate([[maybe_unused]] tref n, [[maybe_unused]] trefs& vars) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref lt_predicate([[maybe_unused]] tref n, [[maybe_unused]] trefs& vars) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref gt_predicate([[maybe_unused]] tref n, [[maybe_unused]] trefs& vars) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref lteq_predicate([[maybe_unused]] tref n, [[maybe_unused]] trefs& vars) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref gteq_predicate([[maybe_unused]] tref n, [[maybe_unused]] trefs& vars) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref nlt_predicate([[maybe_unused]] tref n, [[maybe_unused]] trefs& vars) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref ngt_predicate([[maybe_unused]] tref n, [[maybe_unused]] trefs& vars) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref nlteq_predicate([[maybe_unused]] tref n, [[maybe_unused]] trefs& vars) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref ngteq_predicate([[maybe_unused]] tref n, [[maybe_unused]] trefs& vars) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref interval_predicate([[maybe_unused]] tref n, [[maybe_unused]] trefs& vars) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref atomic_predicate_blasting(tref atomic) {

	auto type_id = tau::get(atomic).get_ba_type();
	if(!is_bv_type_family<node>(type_id))	return atomic;

	auto atomic_nt = tau::get(atomic)[0].get_type();
	tref predicate;
	trefs vars;

	tref current;
	switch (atomic_nt) {
		case tau::bf_eq: { current = eq_predicate<node>(atomic, vars); break; }
		case tau::bf_neq: { current = neq_predicate<node>(atomic, vars); break; }
		case tau::bf_lt: { current = lt_predicate<node>(atomic, vars); break; }
		case tau::bf_gt: { current = gt_predicate<node>(atomic, vars); break; }
		case tau::bf_lteq: { current = lteq_predicate<node>(atomic, vars); break; }
		case tau::bf_gteq: { current = gteq_predicate<node>(atomic, vars); break; }
		case tau::bf_nlt: { current = nlt_predicate<node>(atomic, vars); break; }
		case tau::bf_ngt: { current = ngt_predicate<node>(atomic, vars); break; }
		case tau::bf_nlteq: { current = nlteq_predicate<node>(atomic, vars); break; }
		case tau::bf_ngteq: { current = ngteq_predicate<node>(atomic, vars); break; }
		case tau::bf_interval: { current = interval_predicate<node>(atomic, vars); break; }
		default: {
			// error, unknown atomic predicate, we return nullptr to indicate failure
			LOG_ERROR << "Unknown atomic predicate: " << LOG_NT(atomic_nt);
			return nullptr;
		}
	}

	predicate = predicate
		? build_bf_and<node>(predicate, current)
		: current;

	// we add the existentials for the variables we introduced
	for (auto it = vars.rbegin(); it != vars.rend(); ++it) {
		predicate = build_wff_ex<node>(*it, predicate);
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
			case tau::bf_interval:
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



} // namespace idni::tau_lang