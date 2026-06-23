// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// TODO (LOW) rename file to msnso_rr.h
// TODO (MEDIUM) fix proper types (alias) at this level of abstraction
//
// We should talk about statement, nso_rr (nso_with_rr?), library, rule, builder,
// bindings, etc... instead of sp_tau_node,...

#ifndef __IDNI__TAU__NSO_RR_H__
#define __IDNI__TAU__NSO_RR_H__

#include "tau_tree.h"

namespace idni::tau_lang {

// apply one tau rule to the given expression
// IDEA maybe this could be operator|
template <NodeType node>
tref nso_rr_apply(const rewriter::rule& r, const tref& n);

// apply the given rules to the given expression
// IDEA maybe this could be operator|
template <NodeType node>
tref nso_rr_apply(const rewriter::rules& rs, tref n);

// transform ref args to captures
// this has to be called before rr apply or before calculating fixed points in normalizer
// TODO find better place for this? maybe normalization?
template <NodeType node>
rr<node> transform_ref_args_to_captures(const rr<node>& nso_rr);

// forward declarations needed by nso_rr_apply(rr) below
// - calculate_all_fixed_points is defined in normalizer.tmpl.h
// - step and repeat_all are defined in execution.h
// these are template-dependent names so the full definitions are resolved
// at instantiation time; forward declarations are required so the parser
// knows they are templates (not comparison operators).
template <NodeType node>
tref calculate_all_fixed_points(const rr<node>& nso_rr);

template <NodeType node> struct step;
template <NodeType node, typename step_t> struct repeat_all;

/**
 * @brief Unfold a recurrence relation into a plain formula.
 *
 * Transforms reference arguments to captures, calculates all fixed points,
 * then applies all recurrence relation rules via `step` until no rule fires.
 * @tparam node Tree node type.
 * @param nso_rr Recurrence relation to unfold.
 * @return The main formula with all recurrence relation definitions applied.
 */
template <NodeType node>
tref nso_rr_apply(const rr<node>& nso_rr);

} // namespace idni::tau_lang

#include "nso_rr.tmpl.h"

#endif // __IDNI__TAU__NSO_RR_H__
