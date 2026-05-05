// Extern-template declarations matching src/instantiate_test_pack.cpp.
//
// Force-included into every test TU by tests/CMakeLists.txt's `add()`
// function (via `-include` in target_compile_options).  When a test TU
// sees these `extern template` lines, the compiler does NOT generate a
// local instantiation — the linker resolves the symbol against
// libTAU.a's pre-instantiated copy.
//
// Production builds (main.cpp, tau_codegen_cli.cpp) use the same full
// pack and could benefit from the same extern declarations, but they
// aren't force-included here so per-TU instantiation continues to
// work identically to before — preserving build invariants for the
// CLI binaries.
//
// Recipe: mirror the list at the top of src/instantiate_test_pack.cpp.

#ifndef __IDNI__TAU__EXTERN_TEMPLATE_TEST_PACK_H__
#define __IDNI__TAU__EXTERN_TEMPLATE_TEST_PACK_H__

#ifdef TAU_USE_DOCTEST

#include "ltl_aba.h"
#include "satisfiability.h"
#include "boolean_algebras/qint.h"
#include "boolean_algebras/qlt.h"
#include "boolean_algebras/nlang_ba.h"
#include "boolean_algebras/bv_ba.h"
#include "boolean_algebras/sbf_ba.h"
#include "boolean_algebras/hsb.h"
#include "boolean_algebras/tau_ba.h"

namespace idni::tau_lang {

using test_node_t = node<tau_ba<qint, qlt, nlang_ba, bv, sbf_ba, hsb>,
                         qint, qlt, nlang_ba, bv, sbf_ba, hsb>;

extern template bool is_ltl_aba_realizable<test_node_t>(tref, int_t, bool);
extern template bool is_tau_formula_sat   <test_node_t>(tref, int_t, bool);
extern template bool has_ltl_operators    <test_node_t>(tref);

} // namespace idni::tau_lang

#endif // TAU_USE_DOCTEST

#endif // __IDNI__TAU__EXTERN_TEMPLATE_TEST_PACK_H__
