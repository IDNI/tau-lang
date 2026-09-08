// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ctx.tmpl.h
 * @brief Template implementations for ctx.h (package E). Included by ctx.h.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__CTX_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__CTX_TMPL_H__

namespace idni::tau_lang::anti_prenexing {

// TODO(package E): definitions of every declaration in ctx.h —
//   ctx::for_component, taint_count, taint, table_instance (TAU_CACHE only),
//   lookup, store, memoised, flush_solver_dependent.
// The `#ifdef TAU_CACHE` lives HERE and in the table_instance declaration
//   only: lookup/store/memoised/flush are passthroughs when off.
// Reuse: tree<node>::template create_cache<map_t>() for every instance
//   (GC-registered; keys and cof_entry values are walked by
//   for_each_tref_in — pairs and HasForEachTref types; a trefs value is
//   not walked, which is safe for atoms_memo because its atoms are
//   subtrees of the key).
// Tests: tests/unit/test_anti_prenex_ctx.cpp (the persistence and taint
//   tests under #ifdef TAU_CACHE, i.e. Release).

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__CTX_TMPL_H__
