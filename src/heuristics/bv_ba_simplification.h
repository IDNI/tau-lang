/**
 * @file bv_ba_simplification.h
 * @brief BV Boolean-algebra simplification passes.
 *
 * Provides two simplification strategies for BV terms:
 * - `bv_ba_custom_simplification` — algebraic rewrites without calling cvc5.
 * - `bv_ba_cvc5_simplification`   — delegates to cvc5's simplifier.
 */

// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BA_BV_SIMPLIFICATION_H__
#define __IDNI__TAU__BA_BV_SIMPLIFICATION_H__

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ba_bv-simplification"

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

/** @brief Simplify BV term @p term using custom algebraic rules (no cvc5 call). */
template <typename ... BAs> requires BAsPack<BAs...>
tref bv_ba_custom_simplification(tref term);

/** @brief Simplify BV term @p term by invoking the cvc5 `simplify` procedure. */
template<typename ... BAs> requires BAsPack<BAs...>
tref bv_ba_cvc5_simplification(tref term);

} // namespace idni::tau_lang

#include "bv_ba_custom_simplification.tmpl.h"
#include "bv_ba_cvc5_simplification.tmpl.h"

#endif // __IDNI__TAU__BA_BV_SIMPLIFICATION_H__