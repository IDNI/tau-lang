// Extern-template declarations matching src/instantiate_test_pack.cpp.
//
// Force-included into every test TU by tests/CMakeLists.txt's `add()`
// function (via `-include` in target_compile_options).  When a test TU
// sees these `extern template` lines, the compiler does NOT generate a
// local instantiation — the linker resolves the symbol against
// libTAU.a's pre-instantiated copy.
//
// Production builds (main.cpp) use the same full pack and could benefit
// from the same extern declarations, but they aren't force-included
// here so per-TU instantiation continues to work identically to before —
// preserving build invariants for the CLI binary.
//
// Recipe: mirror the list at the top of src/instantiate_test_pack.cpp.

#ifndef __IDNI__TAU__EXTERN_TEMPLATE_TEST_PACK_H__
#define __IDNI__TAU__EXTERN_TEMPLATE_TEST_PACK_H__

#ifdef TAU_USE_DOCTEST

#include "ltl_aba.h"
#include "satisfiability.h"
#include "tau_pack.h"

namespace idni::tau_lang {

using test_node_t = tau_pack::node_t;

extern template bool is_ltl_aba_realizable<test_node_t>(tref, int_t, bool);
extern template bool is_tau_formula_sat   <test_node_t>(tref, int_t, bool);
extern template bool has_ltl_operators    <test_node_t>(tref);
extern template struct tree    <test_node_t>;
extern template struct get_hook<test_node_t>;
extern template struct tau_ba  <TAU_PACK_BASE_BAS>;

// Mirrors src/instantiate_test_pack.cpp, guards included.
using bool_node_t = node<Bool>;

extern template struct tree    <bool_node_t>;
extern template struct get_hook<bool_node_t>;

// Normalizer pipeline; declarations arrive via ltl_aba.h -> normalizer.h.
extern template tref normalizer               <bool_node_t>(const rr<bool_node_t>&);
extern template tref normalizer               <bool_node_t>(tref);
extern template tref nso_rr_apply             <bool_node_t>(const rewriter::rule&, const tref&);
extern template tref nso_rr_apply             <bool_node_t>(const rewriter::rules&, tref);
extern template tref nso_rr_apply             <bool_node_t>(const rr<bool_node_t>&);
extern template tref calculate_all_fixed_points<bool_node_t>(const rr<bool_node_t>&);

#ifdef TAU_PACK_HAS_BA_BV

using bv_bool_node_t = node<bv, Bool>;

extern template struct tree    <bv_bool_node_t>;
extern template struct get_hook<bv_bool_node_t>;

#ifdef TAU_PACK_HAS_BA_SBF

using bv_sbf_node_t = node<bv, sbf_ba>;

extern template struct tree    <bv_sbf_node_t>;
extern template struct get_hook<bv_sbf_node_t>;

#endif // TAU_PACK_HAS_BA_SBF
#endif // TAU_PACK_HAS_BA_BV

} // namespace idni::tau_lang

#endif // TAU_USE_DOCTEST

#endif // __IDNI__TAU__EXTERN_TEMPLATE_TEST_PACK_H__
