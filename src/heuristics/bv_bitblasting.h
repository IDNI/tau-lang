// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BV_BITBLASTING_H__
#define __IDNI__TAU__BV_BITBLASTING_H__

#include "solver.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_bitblasting"

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

template<NodeType node>
std::optional<solution<node>> bv_bitblasting_solve(tref term);

template<NodeType node>
bool bv_bitblasting_sat(tref term);

template<NodeType node>
bool bv_bitblasting_unsat(tref term);

template<NodeType node>
bool bv_bitblasting_valid(tref term);

} // namespace idni::tau_lang

#include "bv_bitblasting.tmpl.h"

#endif // __IDNI__TAU__BV_BITBLASTING_H__