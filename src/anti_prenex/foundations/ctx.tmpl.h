// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ctx.tmpl.h
 * @brief Template implementations for ctx.h (package E). Included by ctx.h.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__CTX_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__CTX_TMPL_H__

namespace idni::tau_lang::anti_prenexing {

// TODO(package E): definitions of every declaration in ctx.h —
//   ctx::for_component, taint_count, taint, table_ptr, find, lookup, store,
//   memoised, flush_solver_dependent.
// The `#ifdef TAU_CACHE` lives in table_ptr's definition HERE and nowhere
//   else: for a `gated` table it returns nullptr when off; for an
//   unconditional (structural) table it always returns the instance.
//   find/lookup/store/memoised/flush treat a nullptr table as miss /
//   no-op / plain compute. Template parameter order is <node, table>.
// Reuse: tree<node>::template create_cache<map_t>() bound to a
//   function-local static reference for every instance (as get_free_vars
//   does); GC introspection walks pair keys, bare tref values, and the
//   HasForEachTref value types cof_entry and tref_set (fwd.h).
// Tests: tests/unit/test_anti_prenex_ctx.cpp (the persistence and taint
//   tests under #ifdef TAU_CACHE, i.e. Release).

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__CTX_TMPL_H__
