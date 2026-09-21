// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <bit>

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_predicate_blasting"

namespace idni::tau_lang {

//
//
// The arithmetic predicates are built directly over the actual operands,
// without going through the recurrence relation machinery. Auxiliary
// variables (carries, borrows, partial products, remainders...) are created
// fresh per call and returned through the aux output parameter; the caller
// is responsible for existentially quantifying them (see quantify_aux_vars
// in bv_predicate_blasting.tmpl.h). The generated constraints contain no
// quantifiers, which makes the construction capture-free: no bound variable
// can collide with variables occurring in the operands.
//
//

//
//
// bvadd computes the addition of two bitvectors with a textbook per-bit
// ripple-carry adder: a single fresh bv[N] carry variable C, plus two
// families of constraints, all built directly (no per-bit recurrence
// relation calls):
//
//	- carry-in is zero:            bit(C, 0) = 0
//	- sum bit i, for i in [0, N):  bit(s,i) ^ bit(a,i) ^ bit(b,i) ^ bit(C,i) = 0
//	- carry-out i+1, for i in [0, N-1):
//	      bit(C,i+1) = maj(bit(a,i), bit(b,i), bit(C,i))
//	  where maj(x,y,z) = (x&y) | (x&z) | (y&z), expressed as an
//	  equivalence of zero-tests since it relates two different bit
//	  positions (i and i+1) -- see bvshl_by_one_rule for the same idiom.
//	  The sum-bit equation instead relates four terms at the SAME
//	  position i, so it is a plain term equation (no equivalence needed).
//
// N constraints have no carry-out to define (i = N-1), so mod-2^N
// wraparound is automatic: the final carry simply never appears in a
// constraint.
//
//

template<NodeType node>
result<tref> bvadd(tref augend, tref addend, tref sum, trefs& aux) {
	using tau = tree<node>;

	result<tref> r;
	TAU_TRY(auto bitwidth, get_bv_type_bitwidth<node>(augend));

	auto carry = tau::build_variable(bv_type_id<node>(bitwidth));
	auto bf_carry = tau::get(tau::bf, carry);
	aux.push_back(carry);

	TAU_TRY(auto carry0, bit<node>(bf_carry, 0));
	tref body = tau::build_bf_eq_0(carry0);
	for (size_t i = 0; i < bitwidth; ++i) {
		TAU_TRY(auto sum_i, bit<node>(sum, (int_t)i));
		TAU_TRY(auto augend_i, bit<node>(augend, (int_t)i));
		TAU_TRY(auto addend_i, bit<node>(addend, (int_t)i));
		TAU_TRY(auto carry_i, bit<node>(bf_carry, (int_t)i));
		auto sum_eq = tau::build_bf_eq_0(tau::build_bf_xor(
			tau::build_bf_xor(sum_i, augend_i),
			tau::build_bf_xor(addend_i, carry_i)));
		body = tau::build_wff_and(body, sum_eq);
		if (i + 1 == bitwidth) continue;
		auto maj = tau::build_bf_or(tau::build_bf_or(
				tau::build_bf_and(augend_i, addend_i),
				tau::build_bf_and(augend_i, carry_i)),
			tau::build_bf_and(addend_i, carry_i));
		TAU_TRY(auto carry_i1, bit<node>(bf_carry, (int_t)(i + 1)));
		body = tau::build_wff_and(body, tau::build_wff_equiv(
			tau::build_bf_eq_0(carry_i1),
			tau::build_bf_eq_0(maj)));
	}
	return r.with_value(body);
}

//
//
// bvsub computes the subtraction of two bitvectors with the mirror-image
// ripple-borrow subtractor: a single fresh bv[N] borrow variable B, with
//
//	- borrow-in is zero:            bit(B, 0) = 0
//	- diff bit i, for i in [0, N):  bit(d,i) ^ bit(a,i) ^ bit(b,i) ^ bit(B,i) = 0
//	- borrow-out i+1, for i in [0, N-1):
//	      bit(B,i+1) = (~a_i & b_i) | (~a_i & B_i) | (b_i & B_i)
//	  the standard full-subtractor borrow-generate term, again related to
//	  bit(B,i+1) by zero-test equivalence (cross-position). bit() yields
//	  the MASKED value a & 2^i, so ~a_i is formed by negating that masked
//	  term and re-masking: (~bit(a,i)) & 2^i -- this keeps ~a_i in the
//	  same "masked at position i" representation as every other operand,
//	  so it combines with them via plain bf & / | like any other bit(x,i).
//
// Hand-verified by exhaustive 1-bit enumeration of the full subtractor
// (a, b, borrow-in) -> (diff, borrow-out), diff = a^b^Bin:
//
//	a b Bin | diff Bout | (~a&b)|(~a&Bin)|(b&Bin)
//	0 0  0  |  0    0   |  0
//	0 0  1  |  1    1   |  1
//	0 1  0  |  1    1   |  1
//	0 1  1  |  0    1   |  1
//	1 0  0  |  1    0   |  0
//	1 0  1  |  0    0   |  0
//	1 1  0  |  0    0   |  0
//	1 1  1  |  1    1   |  1
//
// Bout matches the formula in all 8 rows, so the recurrence is correct.
// As with bvadd, N constraints have no borrow-out to define (i = N-1),
// so mod-2^N wraparound is automatic.
//
//

template<NodeType node>
result<tref> bvsub(tref minuend, tref subtrahend, tref difference, trefs& aux) {
	using tau = tree<node>;

	result<tref> r;
	TAU_TRY(auto bitwidth, get_bv_type_bitwidth<node>(minuend));
	DBG( LOG_TRACE << "bvsub_rule/bitwidth: " << bitwidth << "\n"; )

	auto borrow = tau::build_variable(bv_type_id<node>(bitwidth));
	auto bf_borrow = tau::get(tau::bf, borrow);
	aux.push_back(borrow);

	TAU_TRY(auto borrow0, bit<node>(bf_borrow, 0));
	tref body = tau::build_bf_eq_0(borrow0);
	for (size_t i = 0; i < bitwidth; ++i) {
		TAU_TRY(auto difference_i, bit<node>(difference, (int_t)i));
		TAU_TRY(auto minuend_i, bit<node>(minuend, (int_t)i));
		TAU_TRY(auto subtrahend_i, bit<node>(subtrahend, (int_t)i));
		TAU_TRY(auto borrow_i, bit<node>(bf_borrow, (int_t)i));
		auto diff_eq = tau::build_bf_eq_0(tau::build_bf_xor(
			tau::build_bf_xor(difference_i, minuend_i),
			tau::build_bf_xor(subtrahend_i, borrow_i)));
		body = tau::build_wff_and(body, diff_eq);
		if (i + 1 == bitwidth) continue;
		auto mask_i = bit_mask_cte<node>(i, bitwidth);
		auto not_a_i = tau::build_bf_and(
			tau::build_bf_neg(minuend_i), mask_i);
		auto borrow_gen = tau::build_bf_or(tau::build_bf_or(
				tau::build_bf_and(not_a_i, subtrahend_i),
				tau::build_bf_and(not_a_i, borrow_i)),
			tau::build_bf_and(subtrahend_i, borrow_i));
		TAU_TRY(auto borrow_i1, bit<node>(bf_borrow, (int_t)(i + 1)));
		body = tau::build_wff_and(body, tau::build_wff_equiv(
			tau::build_bf_eq_0(borrow_i1),
			tau::build_bf_eq_0(borrow_gen)));
	}
	return r.with_value(body);
}

//
//
// bvmul computes the multiplication of a bitvector by a constant multiplier
// as the sum of the multiplicand shifted by each set bit position of the
// multiplier:
//
//	product = sum over { multiplicand << i : bit i of multiplier is 1 }
//
// Shifts use the constant-shift predicate bvshl and the additions chain
// through bvadd, with fresh auxiliary variables for the shifted summands
// and the partial sums.
//
//

template<NodeType node>
result<tref> bvmul(tref multiplicand, tref multiplier, tref product, trefs& aux) {
	using tau = tree<node>;

	result<tref> r;
	// zero multiplier (the hooks normalize zero constants to bf_f)
	if (tau::get(tau::trim(multiplier)).is(tau::bf_f))
		return r.with_value(tau::build_bf_eq_0(product));
	if (!tau::get(tau::trim(multiplier)).is_ba_constant()
		|| !is_bv_constant<node>(tau::trim(multiplier))) {
		DBG(LOG_DEBUG << "Only multiplication by constant is supported in predicate blasting.";)
		return r.with_value(nullptr);
	}

	TAU_TRY(auto bitwidth, get_bv_type_bitwidth<node>(multiplier));

	// set bit positions of the multiplier (position 0 = least significant)
	auto cte = std::get<bv>(
		tau::get(tau::trim(multiplier)).get_ba_constant());
	if (!cte.isBitVectorValue()) return r.with_value(nullptr);
	const std::string bv_str = cte.getBitVectorValue();
	std::vector<size_t> bits;
	for (size_t i = 0; i < bv_str.size(); ++i)
		if (bv_str[bv_str.size() - 1 - i] == '1') bits.push_back(i);
	if (bits.empty()) return r.with_value(tau::build_bf_eq_0(product));

	auto shift_count = [&](size_t i) {
		typename node::constant c = { make_bitvector_value(bitwidth, i) };
		return tau::build_bf_ba_constant(c, bv_type_id<node>(bitwidth));
	};

	// single summand: constrain the product directly
	if (bits.size() == 1) {
		if (bits[0] == 0)
			return r.with_value(tau::build_bf_eq(multiplicand, product));
		return bvshl<node>(multiplicand, shift_count(bits[0]), product);
	}

	tref body = nullptr;
	auto conjoin = [&](tref constraint) {
		body = body ? tau::build_wff_and(body, constraint) : constraint;
	};

	// shifted summands
	trefs terms;
	for (size_t i : bits) {
		if (i == 0) { terms.push_back(multiplicand); continue; }
		auto shifted = tau::build_variable(bv_type_id<node>(bitwidth));
		auto bf_shifted = tau::get(tau::bf, shifted);
		aux.push_back(shifted);
		TAU_TRY(auto shifted_c, bvshl<node>(multiplicand, shift_count(i),
			bf_shifted));
		if (!shifted_c) return r.with_value(nullptr);
		conjoin(shifted_c);
		terms.push_back(bf_shifted);
	}

	// addition chain; the last partial sum is the product itself
	tref acc = terms[0];
	for (size_t k = 1; k < terms.size(); ++k) {
		tref out;
		if (k + 1 == terms.size()) out = product;
		else {
			auto partial = tau::build_variable(
				bv_type_id<node>(bitwidth));
			out = tau::get(tau::bf, partial);
			aux.push_back(partial);
		}
		TAU_TRY(auto sum_c, bvadd<node>(acc, terms[k], out, aux));
		if (!sum_c) return r.with_value(nullptr);
		conjoin(sum_c);
		acc = out;
	}
	return r.with_value(body);
}

//
//
// bvdiv, bvmod and bved encode Euclidean division by a constant divisor:
//
//	dividend = quotient * divisor + remainder  &&  remainder < divisor
//
// expressed over modular arithmetic as
//
//	exact = dividend - remainder  &&  quotient * divisor = exact
//
// Two extra side conditions make the solution unique under wrap-around
// (mod 2^bitwidth) semantics:
//   - remainder <= dividend, so the subtraction cannot wrap, and
//   - quotient <= (2^bitwidth - 1) / divisor, so the product cannot wrap.
//
//

/**
 * @brief Builds the Euclidean division constraints for a constant divisor.
 *
 * @tparam node Node type
 * @param dividend Dividend term
 * @param divisor Divisor (bv constant)
 * @param quotient Quotient term
 * @param remainder Remainder term
 * @param aux Collects the fresh auxiliary variables
 * @return The constraint conjunction, or nullptr if unsupported
 *
 * @par Example
 * Shared by @ref bvdiv, @ref bvmod, and @ref bved (each introduces a fresh
 * variable for the quotient and/or remainder it doesn't otherwise need and
 * discards it). See @ref bved's example: `10 / 3` gives quotient 3,
 * remainder 1, satisfying `exact = 10 - 1 = 9` and `quotient * 3 = 9`.
 */
template<NodeType node>
static result<tref> bv_euclidean_constraints(tref dividend, tref divisor,
	tref quotient, tref remainder, trefs& aux)
{
	using tau = tree<node>;
	DBG( LOG_TRACE << "bvdiv_rule/divisor: " << LOG_FM(divisor) << "\n"; )

	result<tref> r;
	if (!tau::get(tau::trim(divisor)).is_ba_constant()
		|| !is_bv_constant<node>(tau::trim(divisor))) {
		DBG(LOG_DEBUG << "Only division/modulo by constant is supported in predicate blasting.";)
		return r.with_value(nullptr);
	}
	TAU_TRY(auto bitwidth, get_bv_type_bitwidth<node>(divisor));
	// the quotient bound below is computed with 64 bit arithmetic
	if (bitwidth > 64) return r.with_value(nullptr);
	auto divisor_value = get_bv_constant_value<node>(tau::trim(divisor));
	// division by zero falls back to the solver semantics
	if (!divisor_value || *divisor_value == 0) return r.with_value(nullptr);

	auto exact = tau::build_variable(bv_type_id<node>(bitwidth));
	auto bf_exact = tau::get(tau::bf, exact);
	aux.push_back(exact);

	tref body = nullptr;
	auto conjoin = [&](tref constraint) {
		body = body ? tau::build_wff_and(body, constraint) : constraint;
	};

	// exact = dividend - remainder
	TAU_TRY(auto exact_eq, bvsub<node>(dividend, remainder, bf_exact, aux));
	if (!exact_eq) return r.with_value(nullptr);
	conjoin(exact_eq);
	// quotient * divisor = exact
	TAU_TRY(auto quot_eq, bvmul<node>(quotient, divisor, bf_exact, aux));
	if (!quot_eq) return r.with_value(nullptr);
	conjoin(quot_eq);
	// remainder < divisor
	TAU_TRY(auto r_lt_d, bvlt<node>(remainder, divisor));
	if (!r_lt_d) return r.with_value(nullptr);
	conjoin(r_lt_d);
	// remainder <= dividend, so the subtraction cannot wrap around
	TAU_TRY(auto r_gt_d, bvgt<node>(remainder, dividend));
	if (!r_gt_d) return r.with_value(nullptr);
	conjoin(tau::build_wff_neg(r_gt_d));
	// quotient <= (2^bitwidth - 1) / divisor, so the product cannot wrap
	// around; omitted when the bound covers the whole domain
	const size_t max_value = (bitwidth == 64)
		? ~size_t(0) : ((size_t(1) << bitwidth) - 1);
	if (const size_t bound = max_value / *divisor_value;
		bound < max_value)
	{
		typename node::constant c =
			{ make_bitvector_value(bitwidth, bound) };
		auto bound_cte = tau::build_bf_ba_constant(c,
			bv_type_id<node>(bitwidth));
		TAU_TRY(auto q_gt_bound, bvgt<node>(quotient, bound_cte));
		if (!q_gt_bound) return r.with_value(nullptr);
		conjoin(tau::build_wff_neg(q_gt_bound));
	}
	return r.with_value(body);
}

template<NodeType node>
result<tref> bvdiv(tref dividend, tref divisor, tref quotient, trefs& aux) {
	using tau = tree<node>;

	result<tref> r;
	TAU_TRY(auto bitwidth, get_bv_type_bitwidth<node>(divisor));
	auto remainder = tau::build_variable(bv_type_id<node>(bitwidth));
	auto bf_remainder = tau::get(tau::bf, remainder);
	aux.push_back(remainder);
	return bv_euclidean_constraints<node>(dividend, divisor, quotient,
		bf_remainder, aux);
}

template<NodeType node>
result<tref> bvmod(tref dividend, tref divisor, tref remainder, trefs& aux) {
	using tau = tree<node>;
	DBG( LOG_TRACE << "bvmod/dividend: " << LOG_FM(dividend) << "\n"; )
	DBG( LOG_TRACE << "bvmod/divisor: " << LOG_FM(divisor) << "\n"; )
	DBG( LOG_TRACE << "bvmod/remainder: " << LOG_FM(remainder) << "\n"; )

	result<tref> r;
	TAU_TRY(auto bitwidth, get_bv_type_bitwidth<node>(divisor));
	auto quotient = tau::build_variable(bv_type_id<node>(bitwidth));
	auto bf_quotient = tau::get(tau::bf, quotient);
	aux.push_back(quotient);
	return bv_euclidean_constraints<node>(dividend, divisor, bf_quotient,
		remainder, aux);
}

template<NodeType node>
result<tref> bved(tref dividend, tref divisor, tref quotient, tref remainder,
	trefs& aux)
{
	return bv_euclidean_constraints<node>(dividend, divisor, quotient,
		remainder, aux);
}

// Shared body of bvmin/bvmax: the predicate
//   ((left < right) -> result = low) && (!(left < right) -> result = high)
// where (low, high) is (left, right) for min and (right, left) for max --
// max reuses the same single bvlt recurrence with the copied sides swapped,
// which at equal operands still picks the shared value through the negated
// branch. Equality is spelled !bvneq, the one bit-level equality entry point.
template<NodeType node>
static result<tref> bv_pick_by_order(tref left, tref right, tref res, bool pick_smaller) {
	using tau = tree<node>;

	result<tref> r;
	// bvlt/bvneq read the bitwidth off their first operand (left and
	// res respectively); the fresh result variable always carries one,
	// the operands do after type inference -- bail out like the other
	// builders if either is missing (each checked on its own: a failure
	// on one must not be masked by comparing it against the other).
	if (auto w = r.merge_take(get_bv_type_bitwidth<node>(res)); !w) return r;
	if (auto w = r.merge_take(get_bv_type_bitwidth<node>(left)); !w) return r;
	TAU_TRY(auto lt, bvlt<node>(left, right));
	if (!lt) return r.with_value(nullptr);
	auto eq = [&](tref operand) -> result<tref> {
		result<tref> r;
		TAU_TRY(auto neq, bvneq<node>(res, operand));
		if (!neq) return r.with_value(nullptr);
		return r.with_value(tau::build_wff_neg(neq));
	};
	TAU_TRY(auto eq_low, eq(pick_smaller ? left : right));
	TAU_TRY(auto eq_high, eq(pick_smaller ? right : left));
	if (!eq_low || !eq_high) return r.with_value(nullptr);
	return r.with_value(tau::build_wff_and(
		tau::build_wff_imply(lt, eq_low),
		tau::build_wff_imply(tau::build_wff_neg(lt), eq_high)));
}

template<NodeType node>
result<tref> bvmin(tref left, tref right, tref result) {
	return bv_pick_by_order<node>(left, right, result, true);
}

template<NodeType node>
result<tref> bvmax(tref left, tref right, tref result) {
	return bv_pick_by_order<node>(left, right, result, false);
}

} // namespace idni::tau_lang
