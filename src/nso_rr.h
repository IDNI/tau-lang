// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

// TODO (LOW) rename file to msnso_rr.h
// TODO (MEDIUM) fix proper types (alias) at this level of abstraction
//
// We should talk about statement, nso_rr (nso_with_rr?), library, rule, builder,
// bindings, etc... instead of sp_tau_node,...

#ifndef __IDNI__TAU__NSO_RR_H__
#define __IDNI__TAU__NSO_RR_H__

#include "tau_tree.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "nso_rr"

namespace idni::tau_lang {

// apply one tau rule to the given expression
// IDEA maybe this could be operator|
template <NodeType node>
tref nso_rr_apply(const rewriter::rule& r, const tref& n);

// apply the given rules to the given expression
// IDEA maybe this could be operator|
template <NodeType node>
tref nso_rr_apply(const rewriter::rules& rs, tref n);

} // namespace idni::tau_lang

#include "nso_rr.tmpl.h"

#endif // __IDNI__TAU__NSO_RR_H__
