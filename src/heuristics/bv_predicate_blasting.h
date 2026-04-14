// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BV_BITBLASTING_H__
#define __IDNI__TAU__BV_BITBLASTING_H__

#include "tau_tree.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_predicate_blasting"

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;


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

} // namespace idni::tau_lang

#include "bv_predicate_blasting_helpers.tmpl.h"
#include "bv_predicate_blasting_logic.tmpl.h"
#include "bv_predicate_blasting_comparisons.tmpl.h"
#include "bv_predicate_blasting_arithmetic.tmpl.h"
#include "bv_predicate_blasting.tmpl.h"

#endif // __IDNI__TAU__BV_BITBLASTING_H__