// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BV_PREDICATE_BLASTING_H__
#define __IDNI__TAU__BV_PREDICATE_BLASTING_H__

#include "tau_tree.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_predicate_blasting"

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

inline bool bv_blasting = true;

/**
 * @brief Entry point for predicate blasting on bitvector formulas.
 *
 * Rewrites bitvector formulas into equivalent Boolean formulas containing
 * existentially quantified variables and predicates. Only supports a limited set of
 * bitvector operations. If unsupported operations or predicates are encountered,
 * it returns nullptr to indicate failure, and the caller can choose to leave the
 * formula unchanged.
 *
 * See implementation in bv_predicate_blasting.tmpl.h and its companion
 * files bv_predicate_blasting_logic.tmpl.h,
 * bv_predicate_blasting_comparisons.tmpl.h and
 * bv_predicate_blasting_arithmetic.tmpl.h for details.
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
 * @brief Computes a predicate constraining result to be the cast of src to a
 * different bitwidth.
 *
 * For zero-extension (src bitwidth < result bitwidth):
 *   - Low bits of result match source bits
 *   - High bits of result are zero
 *
 * For truncation (src bitwidth > result bitwidth):
 *   - Result bits match the low bits of source
 *
 * @tparam node Node type
 * @param src Source bitvector
 * @param result Result bitvector (fresh variable of target type)
 * @return The resulting predicate term
 */
template<NodeType node>
tref bvcast(tref src, tref result);

/**
 * @brief Computes a predicate checking if left is not equal to right as bitvectors.
 *
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @return The resulting predicate
 */
template<NodeType node>
tref bvneq(tref left, tref right);

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
 * @brief Computes a predicate checking if left is not greater than right as bitvectors.
 *
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @return The resulting predicate
 */
template<NodeType node>
tref bvngt(tref left, tref right) { return bvlteq<node>(left, right); }

/**
 * @brief Computes a predicate constraining sum to be the addition of the operands.
 *
 * The constraint formula is quantifier free; the fresh auxiliary variables it
 * introduces (the carries) are appended to aux and must be existentially
 * quantified by the caller.
 *
 * @tparam node Node type
 * @param augend Left operand
 * @param addend Right operand
 * @param sum Result variable
 * @param aux Collects the fresh auxiliary variables
 * @return The resulting predicate term, or nullptr on error
 */
template<NodeType node>
tref bvadd(tref augend, tref addend, tref sum, trefs& aux);

/**
 * @brief Computes a predicate constraining difference to be the subtraction
 * of the operands.
 *
 * The constraint formula is quantifier free; the fresh auxiliary variables it
 * introduces (the borrows) are appended to aux and must be existentially
 * quantified by the caller.
 *
 * @tparam node Node type
 * @param minuend Left operand
 * @param subtrahend Right operand
 * @param difference Result variable
 * @param aux Collects the fresh auxiliary variables
 * @return The resulting predicate term, or nullptr on error
 */
template<NodeType node>
tref bvsub(tref minuend, tref subtrahend, tref difference, trefs& aux);

/**
 * @brief Computes a predicate constraining product to be the multiplication
 * of the operands.
 *
 * Only supports multiplication by a constant multiplier. The constraint
 * formula is quantifier free; the fresh auxiliary variables it introduces
 * (shifted summands and partial sums) are appended to aux and must be
 * existentially quantified by the caller.
 *
 * @tparam node Node type
 * @param multiplicand Left operand
 * @param multiplier Right operand (must be constant)
 * @param product Result variable
 * @param aux Collects the fresh auxiliary variables
 * @return The resulting predicate term, or nullptr if unsupported
 */
template<NodeType node>
tref bvmul(tref multiplicand, tref multiplier, tref product, trefs& aux);

/**
 * @brief Computes the Euclidean division predicate constraining quotient and
 * remainder at once.
 *
 * Only supports division by a constant divisor. The constraint formula is
 * quantifier free; the fresh auxiliary variables it introduces are appended
 * to aux and must be existentially quantified by the caller.
 *
 * @tparam node Node type
 * @param dividend Dividend
 * @param divisor Divisor (must be constant)
 * @param quotient Quotient variable
 * @param remainder Remainder variable
 * @param aux Collects the fresh auxiliary variables
 * @return The resulting predicate term, or nullptr if unsupported
 */
template<NodeType node>
tref bved(tref dividend, tref divisor, tref quotient, tref remainder,
	trefs& aux);

/**
 * @brief Computes a predicate constraining quotient to be the division of
 * dividend by divisor.
 *
 * Only supports division by a constant divisor. The constraint formula is
 * quantifier free; the fresh auxiliary variables it introduces are appended
 * to aux and must be existentially quantified by the caller.
 *
 * @tparam node Node type
 * @param dividend Dividend
 * @param divisor Divisor (must be constant)
 * @param quotient Quotient variable
 * @param aux Collects the fresh auxiliary variables
 * @return The resulting predicate term, or nullptr if unsupported
 */
template<NodeType node>
tref bvdiv(tref dividend, tref divisor, tref quotient, trefs& aux);

/**
 * @brief Computes a predicate constraining remainder to be the modulo of
 * dividend by divisor.
 *
 * Only supports modulo by a constant divisor. The constraint formula is
 * quantifier free; the fresh auxiliary variables it introduces are appended
 * to aux and must be existentially quantified by the caller.
 *
 * @tparam node Node type
 * @param dividend Dividend
 * @param divisor Divisor (must be constant)
 * @param remainder Remainder variable
 * @param aux Collects the fresh auxiliary variables
 * @return The resulting predicate term, or nullptr if unsupported
 */
template<NodeType node>
tref bvmod(tref dividend, tref divisor, tref remainder, trefs& aux);

} // namespace idni::tau_lang



#include "bv_predicate_blasting_helpers.tmpl.h"
#include "bv_predicate_blasting_logic.tmpl.h"
#include "bv_predicate_blasting_comparisons.tmpl.h"
#include "bv_predicate_blasting_arithmetic.tmpl.h"
#include "bv_predicate_blasting.tmpl.h"

#endif // __IDNI__TAU__BV_PREDICATE_BLASTING_H__