// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file subst.tmpl.h
 * @brief Template implementations for subst.h (package C). Included by subst.h.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__SUBST_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__SUBST_TMPL_H__

namespace idni::tau_lang::anti_prenexing {

// TODO(package C): definitions of every declaration in subst.h —
//   subst_var, subst_atom, atoms, has_atom.
// Reuse: pre_order/post_order::apply_unique (per-call memo), rewriter::replace
//   for plain subtrees, fv / canonical_and / canonical_or (dag.h),
//   subst_term (terms.h). The atoms_memo table type is in ctx.h.
// Depends on packages A and B; starts in wave 2, or in wave 1 with a
//   temporary shim over get_free_vars and plain replace, swapped on merge.
// Tests: tests/unit/test_anti_prenex_subst.cpp.

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__SUBST_TMPL_H__
