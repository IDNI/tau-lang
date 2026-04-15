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
 * @param operand Operand to shift
 * @param shifted Result variable
 * @return The resulting predicate term
 */
template<NodeType node>
tref bvshl_by_one(tref operand, tref shifted);

/**
 * @brief Computes a predicate checking if shifted is the result of shifting operand right by one bit.
 *
 * @tparam node Node type
 * @param operand Operand to shift
 * @param shifted Result variable
 * @return The resulting predicate term
 */
template<NodeType node>
tref bvrhl_by_one(tref operand, tref shifted);

/**
 * @brief Computes a predicate checking if a specific bit of an operand is zero.
 *
 * @tparam node Node type
 * @param operand Operand to check
 * @param bit Bit index
 * @return The resulting predicate term
 */
template<NodeType node>
tref is_bit_zero(tref operand, int_t bit);

/**
 * @brief Computes a predicate checking if a specific bit of an operand is one.
 *
 * @tparam node Node type
 * @param operand Operand to check
 * @param bit Bit index
 * @return The resulting predicate term
 */
template<NodeType node>
tref is_bit_one(tref operand, int_t bit);

/**
 * @brief Computes a predicate checking if shifted is the result of shifting
 * operand left by a constant amount.
 *
 * @tparam node Node type
 * @param left Operand to shift
 * @param shift Shift amount (constant)
 * @param shifted Result variable
 * @return The resulting predicate term
 */
template<NodeType node>
tref bvshl(tref left, tref shift, tref shifted);

/**
 * @brief Computes a predicate checking if shifted is the result of shifting
 * operand right by a constant amount.
 *
 * @tparam node Node type
 * @param left Operand to shift
 * @param shift Shift amount (constant)
 * @param shifted Result variable
 * @return The resulting predicate term
 */
template<NodeType node>
tref bvrhl(tref left, tref shift, tref shifted);

} // namespace idni::tau_lang



#include "bv_predicate_blasting_helpers.tmpl.h"
#include "bv_predicate_blasting_logic.tmpl.h"
#include "bv_predicate_blasting_comparisons.tmpl.h"
#include "bv_predicate_blasting_arithmetic.tmpl.h"
#include "bv_predicate_blasting.tmpl.h"

#endif // __IDNI__TAU__BV_BITBLASTING_H__