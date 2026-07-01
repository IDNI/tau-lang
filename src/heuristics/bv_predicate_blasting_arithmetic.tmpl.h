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
// bvadd computes the addition of two bitvectors by unrolling the classic
// carry-propagation recurrence
//
//	(a, b) -> (a ^ b, (a & b) << 1)
//
// bitwidth times. Each step shifts the pending carry one position to the
// left, so after bitwidth steps the carry is provably zero and the sum
// equals the accumulated xor term.
//
//

template<NodeType node>
tref bvadd(tref augend, tref addend, tref sum, trefs& aux) {
	using tau = tree<node>;

	auto bitwidth = get_bv_type_bitwidth<node>(augend);
	if (bitwidth == 0) return nullptr;

	tref a = augend, b = addend;
	tref body = nullptr;
	for (size_t i = 0; i < bitwidth; ++i) {
		auto carry = tau::build_variable(bv_type_id<node>(bitwidth));
		auto bf_carry = tau::get(tau::bf, carry);
		aux.push_back(carry);
		// carry = (a & b) << 1
		auto constraint = bvshl_by_one<node>(
			tau::build_bf_and(a, b), bf_carry);
		if (!constraint) return nullptr;
		body = body ? tau::build_wff_and(body, constraint) : constraint;
		a = tau::build_bf_xor(a, b);
		b = bf_carry;
	}
	// after bitwidth steps the pending carry is zero, so sum = a
	return tau::build_wff_and(body, tau::build_bf_eq(a, sum));
}

//
//
// bvsub computes the subtraction of two bitvectors by unrolling the borrow
// recurrence
//
//	(a, b) -> (a ^ b, (a' & b) << 1)
//
// bitwidth times: after bitwidth steps the pending borrow is provably zero
// and the difference equals the accumulated xor term.
//
//

template<NodeType node>
tref bvsub(tref minuend, tref subtrahend, tref difference, trefs& aux) {
	using tau = tree<node>;

	auto bitwidth = get_bv_type_bitwidth<node>(minuend);
	if (bitwidth == 0) return nullptr;

	tref a = minuend, b = subtrahend;
	tref body = nullptr;
	for (size_t i = 0; i < bitwidth; ++i) {
		auto borrow = tau::build_variable(bv_type_id<node>(bitwidth));
		auto bf_borrow = tau::get(tau::bf, borrow);
		aux.push_back(borrow);
		// borrow = (a' & b) << 1
		auto constraint = bvshl_by_one<node>(
			tau::build_bf_and(tau::build_bf_neg(a), b), bf_borrow);
		if (!constraint) return nullptr;
		body = body ? tau::build_wff_and(body, constraint) : constraint;
		a = tau::build_bf_xor(a, b);
		b = bf_borrow;
	}
	// after bitwidth steps the pending borrow is zero, so difference = a
	return tau::build_wff_and(body, tau::build_bf_eq(a, difference));
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
tref bvmul(tref multiplicand, tref multiplier, tref product, trefs& aux) {
	using tau = tree<node>;

	// zero multiplier (the hooks normalize zero constants to bf_f)
	if (tau::get(tau::trim(multiplier)).is(tau::bf_f))
		return tau::build_bf_eq_0(product);
	if (!tau::get(tau::trim(multiplier)).is_ba_constant()
		|| !is_bv_constant<node>(tau::trim(multiplier))) {
		DBG(LOG_DEBUG << "Only multiplication by constant is supported in predicate blasting.";)
		return nullptr;
	}

	auto bitwidth = get_bv_type_bitwidth<node>(multiplier);
	if (bitwidth == 0) return nullptr;

	// set bit positions of the multiplier (position 0 = least significant)
	auto cte = std::get<bv>(
		tau::get(tau::trim(multiplier)).get_ba_constant());
	if (!cte.isBitVectorValue()) return nullptr;
	const std::string bv_str = cte.getBitVectorValue();
	std::vector<size_t> bits;
	for (size_t i = 0; i < bv_str.size(); ++i)
		if (bv_str[bv_str.size() - 1 - i] == '1') bits.push_back(i);
	if (bits.empty()) return tau::build_bf_eq_0(product);

	auto shift_count = [&](size_t i) {
		typename node::constant c = { make_bitvector_value(bitwidth, i) };
		return tau::build_bf_ba_constant(c, bv_type_id<node>(bitwidth));
	};

	// single summand: constrain the product directly
	if (bits.size() == 1) {
		if (bits[0] == 0)
			return tau::build_bf_eq(multiplicand, product);
		return bvshl<node>(multiplicand, shift_count(bits[0]), product);
	}

	tref body = nullptr;
	auto conjoin = [&](tref constraint) -> bool {
		if (!constraint) return false;
		body = body ? tau::build_wff_and(body, constraint) : constraint;
		return true;
	};

	// shifted summands
	trefs terms;
	for (size_t i : bits) {
		if (i == 0) { terms.push_back(multiplicand); continue; }
		auto shifted = tau::build_variable(bv_type_id<node>(bitwidth));
		auto bf_shifted = tau::get(tau::bf, shifted);
		aux.push_back(shifted);
		if (!conjoin(bvshl<node>(multiplicand, shift_count(i),
			bf_shifted))) return nullptr;
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
		if (!conjoin(bvadd<node>(acc, terms[k], out, aux)))
			return nullptr;
		acc = out;
	}
	return body;
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
 */
template<NodeType node>
static tref bv_euclidean_constraints(tref dividend, tref divisor,
	tref quotient, tref remainder, trefs& aux)
{
	using tau = tree<node>;

	if (!tau::get(tau::trim(divisor)).is_ba_constant()
		|| !is_bv_constant<node>(tau::trim(divisor))) {
		DBG(LOG_DEBUG << "Only division/modulo by constant is supported in predicate blasting.";)
		return nullptr;
	}
	auto bitwidth = get_bv_type_bitwidth<node>(divisor);
	// the quotient bound below is computed with 64 bit arithmetic
	if (bitwidth == 0 || bitwidth > 64) return nullptr;
	auto divisor_value = get_bv_constant_value<node>(tau::trim(divisor));
	// division by zero falls back to the solver semantics
	if (!divisor_value || *divisor_value == 0) return nullptr;

	auto exact = tau::build_variable(bv_type_id<node>(bitwidth));
	auto bf_exact = tau::get(tau::bf, exact);
	aux.push_back(exact);

	tref body = nullptr;
	auto conjoin = [&](tref constraint) -> bool {
		if (!constraint) return false;
		body = body ? tau::build_wff_and(body, constraint) : constraint;
		return true;
	};

	// exact = dividend - remainder
	if (!conjoin(bvsub<node>(dividend, remainder, bf_exact, aux)))
		return nullptr;
	// quotient * divisor = exact
	if (!conjoin(bvmul<node>(quotient, divisor, bf_exact, aux)))
		return nullptr;
	// remainder < divisor
	if (!conjoin(bvlt<node>(remainder, divisor))) return nullptr;
	// remainder <= dividend, so the subtraction cannot wrap around
	auto r_gt_d = bvgt<node>(remainder, dividend);
	if (!r_gt_d) return nullptr;
	if (!conjoin(tau::build_wff_neg(r_gt_d))) return nullptr;
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
		auto q_gt_bound = bvgt<node>(quotient, bound_cte);
		if (!q_gt_bound) return nullptr;
		if (!conjoin(tau::build_wff_neg(q_gt_bound))) return nullptr;
	}
	return body;
}

template<NodeType node>
tref bvdiv(tref dividend, tref divisor, tref quotient, trefs& aux) {
	using tau = tree<node>;

	auto bitwidth = get_bv_type_bitwidth<node>(divisor);
	if (bitwidth == 0) return nullptr;
	auto remainder = tau::build_variable(bv_type_id<node>(bitwidth));
	auto bf_remainder = tau::get(tau::bf, remainder);
	aux.push_back(remainder);
	return bv_euclidean_constraints<node>(dividend, divisor, quotient,
		bf_remainder, aux);
}

template<NodeType node>
tref bvmod(tref dividend, tref divisor, tref remainder, trefs& aux) {
	using tau = tree<node>;

	auto bitwidth = get_bv_type_bitwidth<node>(divisor);
	if (bitwidth == 0) return nullptr;
	auto quotient = tau::build_variable(bv_type_id<node>(bitwidth));
	auto bf_quotient = tau::get(tau::bf, quotient);
	aux.push_back(quotient);
	return bv_euclidean_constraints<node>(dividend, divisor, bf_quotient,
		remainder, aux);
}

template<NodeType node>
tref bved(tref dividend, tref divisor, tref quotient, tref remainder,
	trefs& aux)
{
	return bv_euclidean_constraints<node>(dividend, divisor, quotient,
		remainder, aux);
}

} // namespace idni::tau_lang
