// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BV_BITBLASTING_H__
#define __IDNI__TAU__BV_BITBLASTING_H__

#include "tau_tree.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_predicate_blasting"

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

bool bv_blasting = true;

/**
 * @brief Entry point for predicate blasting on bitvector formulas.
 *
 * Rewrites bitvector formulas into equivalent Boolean formulas containing
 * existentially quantified variables and predicates. Only supports a limited set of
 * bitvector operations. If unsupported operations or predicates are encountered,
 * it returns nullptr to indicate failure, and the caller can choose to leave the
 * formula unchanged.
 *
 * See implementation in bv_predicate_blasting.tmpl.h and
 * bv_predicate_blasting_predicates.tmpl.h for details.
 *
 * @tparam node Node type
 * @param term The formula to blast
 * @return The formula with predicates blasted, or nullptr on error
 */
template<NodeType node>
tref bv_predicate_blasting(tref term);

/**
 * @brief Computes a term giving the value of a specific bit in a bitvector.
 * @tparam node Node type
 * @param operand Operand
 * @param bit Bit index
 * @return The resulting term
 */
template<NodeType node>
tref bit(tref operand, int_t bit);

/**
 * @brief Computes a predicate checking if shifted is the result of shifting operand left by one bit.
 *
 * @tparam node Node type
 * @param base Operand to shift
 * @param shifted Result variable
 * @return The resulting term
 */
template<NodeType node>
tref bvshl_by_one(tref base, tref shifted);

/**
 * @brief Computes a predicate checking if shifted is the result of shifting operand right by one bit.
 *
 * @tparam node Node type
 * @param base Operand to shift
 * @param shifted Result variable
 * @return The resulting predicate
 */
template<NodeType node>
tref bvrhl_by_one(tref base, tref shifted);

/**
 * @brief Computes a predicate checking if a specific bit of an operand is zero.
 *
 * @tparam node Node type
 * @param operand Operand to check
 * @param bit Bit index
 * @return The resulting predicate
 */
template<NodeType node>
tref is_bit_zero(tref operand, int_t bit);

/**
 * @brief Computes a predicate checking if a specific bit of an operand is one.
 *
 * @tparam node Node type
 * @param operand Operand to check
 * @param bit Bit index
 * @return The resulting predicate
 */
template<NodeType node>
tref is_bit_one(tref operand, int_t bit);

/**
 * @brief Computes a predicate checking if shifted is the result of shifting
 * operand left by a constant amount.
 *
 * @tparam node Node type
 * @param base Operand to shift
 * @param count Shift amount (constant)
 * @param shifted Result variable
 * @return The resulting predicate
 */
template<NodeType node>
tref bvshl(tref base, tref count, tref shifted);

/**
 * @brief Computes a predicate checking if shifted is the result of shifting
 * operand right by a constant amount.
 *
 * @tparam node Node type
 * @param base Operand to shift
 * @param count Shift amount (constant)
 * @param shifted Result variable
 * @return The resulting predicate
 */
template<NodeType node>
tref bvrhl(tref base, tref count, tref shifted);

/**
 * @brief Computes a predicate checking if left is less than right as bitvectors.
 *
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @return The resulting predicate
 */
template<NodeType node>
tref bvlt(tref left, tref right);

/**
 * @brief Computes a predicate checking if left is greater than right as bitvectors.
 *
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @return The resulting predicate
 */
template<NodeType node>
tref bvgt(tref left, tref right);

/**
 * @brief Computes a predicate checking if left is less than or equal to right as bitvectors.
 *
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @return The resulting predicate
 */
template<NodeType node>
tref bvlteq(tref left, tref right) {
	using tau = tree<node>;

	return tau::build_wff_neg(bvgt<node>(left, right)); }

/**
 * @brief Computes a predicate checking if left is greater or equal to right as bitvectors.
 *
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @return The resulting predicate
 */
template<NodeType node>
tref bvgteq(tref left, tref right) {
	using tau = tree<node>;

	return tau::build_wff_neg(bvlt<node>(left, right));
}

/**
 * @brief Computes a predicate checking if left is not less than or equal to right as bitvectors.
 *
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @return The resulting predicate
 */
template<NodeType node>
tref bvnlteq(tref left, tref right) { return bvgt<node>(left, right); }

/**
 * @brief Computes a predicate checking if left is not greater or equal to right as bitvectors.
 *
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @return The resulting predicate
 */
template<NodeType node>
tref bvngteq(tref left, tref right) { return bvlt<node>(left, right); }

/**
 * @brief Computes a predicate checking if left is not less than right as bitvectors.
 *
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @return The resulting predicate
 */
template<NodeType node>
tref bvnlt(tref left, tref right) { return bvgteq<node>(left, right); }

/**
 * @brief Computes a predicate checking if left is notgreater than right as bitvectors.
 *
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @return The resulting predicate
 */
template<NodeType node>
tref bvngt(tref left, tref right) { return bvlteq<node>(left, right); }

/**
 * @brief Applies the bitvector addition rule to the given operands.
 *
 * @tparam node Node type
 * @param augend Left operand
 * @param addend Right operand
 * @param sum Result variable
 * @return The resulting predicate term
 */
template<NodeType node>
tref bvadd(tref augend, tref addend, tref sum);

/**
 * @brief Applies the bitvector subtraction rule to the given operands.
 *
 * @tparam node Node type
 * @param minuend Left operand
 * @param subtrahend Right operand
 * @param difference Result variable
 * @return The resulting predicate term
 */
template<NodeType node>
tref bvsub(tref minuend, tref subtrahend, tref difference);

/**
 * @brief Applies the bitvector multiplication rule to the given operands.
 *
 * Only supports multiplication by a constant right operand.
 *
 * @tparam node Node type
 * @param multiplicand Left operand
 * @param multiplier Right operand (must be constant)
 * @param product Result variable
 * @return The resulting predicate term, or nullptr if unsupported
 */
template<NodeType node>
tref bvmul(tref multiplicand, tref multiplier, tref product);

/**
 * @brief Applies the bitvector division rule to the given operands.
 *
 * Only supports division by a constant divisor.
 *
 * @tparam node Node type
 * @param dividend Dividend
 * @param divisor Divisor (must be constant)
 * @param quotient Quotient variable
 * @param remainder Remainder variable
 * @return The resulting predicate term, or nullptr if unsupported
 */
template<NodeType node>
tref bved(tref dividend, tref divisor, tref quotient, tref remainder);

/**
 * @brief Applies the bitvector division rule to the given operands.
 *
 * Only supports division by a constant divisor.
 *
 * @tparam node Node type
 * @param dividend Dividend
 * @param divisor Divisor (must be constant)
 * @param quotient Quotient variable
 * @return The resulting predicate term, or nullptr if unsupported
 */
template<NodeType node>
tref bvdiv(tref dividend, tref divisor, tref quotient);

/**
 * @brief Applies the bitvector modulo rule to the given operands.
 *
 * Only supports modulo by a constant divisor.
 *
 * @tparam node Node type
 * @param dividend Dividend
 * @param divisor Divisor (must be constant)
 * @param remainder Remainder variable
 * @return The resulting predicate term, or nullptr if unsupported
 */
template<NodeType node>
tref bvmod(tref dividend, tref divisor, tref remainder);

} // namespace idni::tau_lang



#include "bv_predicate_blasting_helpers.tmpl.h"
#include "bv_predicate_blasting_logic.tmpl.h"
#include "bv_predicate_blasting_comparisons.tmpl.h"
#include "bv_predicate_blasting_arithmetic.tmpl.h"
#include "bv_predicate_blasting.tmpl.h"

#endif // __IDNI__TAU__BV_BITBLASTING_H__