// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file prims.tmpl.h
 * @brief Template implementations for prims.h (package D). Included by prims.h.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__PRIMS_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__PRIMS_TMPL_H__

namespace idni::tau_lang::anti_prenexing {

// TODO(package D): definitions of every declaration in prims.h —
//   canonicalise_binder_ids, rewrap, fold_degenerate_binders.
// Reuse: canonize_quantifier_ids (tau_tree_builders.tmpl.h:21-82) — extend
//   to bf_fall/bf_fex inside terms via a flag or a new function, never by
//   changing the old function's output; build_wff_ex / build_wff_all with
//   calculate_quant_id = false; fv (dag.h) for the degenerate test.
// Tests: tests/unit/test_anti_prenex_prims.cpp.

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__PRIMS_TMPL_H__
