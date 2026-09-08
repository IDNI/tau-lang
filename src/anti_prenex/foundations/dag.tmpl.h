// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file dag.tmpl.h
 * @brief Template implementations for dag.h (package A). Included by dag.h.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__DAG_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__DAG_TMPL_H__

namespace idni::tau_lang::anti_prenexing {

// TODO(package A): definitions of every declaration in dag.h —
//   size, fv, fv_meets, fv_intersect, content_hash, content_less,
//   members, is_member, canonical_and, canonical_or, cached_neg, cache_neg,
//   the classification predicates, atom_of, the binder accessors.
// Reuse: get_free_vars (tau_tree_extractors.tmpl.h), node::hash,
//   subtree_less / subtree_equality, get_leaves, create_cache.
// Cache gating: every per-node table is `#ifdef TAU_CACHE` (fwd.h).
// Tests: tests/unit/test_anti_prenex_dag.cpp.

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__DAG_TMPL_H__
