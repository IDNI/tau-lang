// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file dag.tmpl.h
 * @brief Template implementations for dag.h (package A). Included by dag.h.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__DAG_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__DAG_TMPL_H__

namespace idni::tau_lang::anti_prenexing {

// TODO(package A): definitions of every declaration in dag.h —
//   formula_size, fv, fv_meets, fv_intersect, content_hash, content_less,
//   members, is_member, canonical_and, canonical_or, complement_of, neg_of,
//   set_neg, the classification predicates, atom_of, the binder accessors.
// Reuse: get_free_vars (tau_tree_extractors.tmpl.h), hash_lcrs_tref,
//   subtree_less / subtree_equality, get_leaves, is_atomic_fm, equals_T/F,
//   is_child_quantifier, is_child_temporal_quantifier, build_wff_and/or
//   (binary), build_wff_neg; ctx.h's find/store for size_memo, neg_memo,
//   negative_tree_memo (lazy fill; the accessor is the only writer).
// Tables: the facet tables are the UNCONDITIONAL structural entries of
//   ctx.h's `enum class table` (size_memo, neg_memo, negative_tree_memo),
//   reached through ctx.h's find/store — never a private `#ifdef TAU_CACHE`
//   table here (fwd.h, cache gating). `members` is NOT cached: a linear
//   walk of the chain, returned by value (ruling Sep 7).
// Tests: tests/unit/test_anti_prenex_dag.cpp.

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__DAG_TMPL_H__
