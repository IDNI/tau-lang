// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BA_BV_SIMPLIFICATION_H__
#define __IDNI__TAU__BA_BV_SIMPLIFICATION_H__

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ba_bv-simplification"

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

template <typename ... BAs> requires BAsPack<BAs...>
tref ba_bv_custom_simplification(tref term);

template<typename ... BAs> requires BAsPack<BAs...>
tref ba_bv_cvc5_simplification(tref term);

} // namespace idni::tau_lang

#include "ba_bv_custom_simplification.tmpl.h"
#include "ba_bv_cvc5_simplification.tmpl.h"

#endif // __IDNI__TAU__BA_BV_SIMPLIFICATION_H__