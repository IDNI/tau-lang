// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BV_BITBLASTING_H__
#define __IDNI__TAU__BV_BITBLASTING_H__

#include "tau_tree.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_predicate_blasting"

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

template<NodeType node>
tref bv_predicate_blasting(tref term);

} // namespace idni::tau_lang

#include "bv_predicate_blasting_predicates.tmpl.h"
#include "bv_predicate_blasting.tmpl.h"

#endif // __IDNI__TAU__BV_BITBLASTING_H__