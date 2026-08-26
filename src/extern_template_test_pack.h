// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Extern-template declarations matching src/instantiate_test_pack.cpp.
//
// Force-included into every test TU by tests/CMakeLists.txt's `add()`
// function (via `-include` in target_compile_options). When a test TU sees
// these `extern template` lines, the compiler does NOT generate a local
// instantiation -- the linker resolves the symbol against libTAU's
// pre-instantiated copy.
//
// Production builds (main.cpp) use the same pack and could benefit from the
// same extern declarations, but they aren't force-included here so per-TU
// instantiation there continues to work as before.
//
// Recipe: mirror the list at the top of src/instantiate_test_pack.cpp.

#ifndef __IDNI__TAU__EXTERN_TEMPLATE_TEST_PACK_H__
#define __IDNI__TAU__EXTERN_TEMPLATE_TEST_PACK_H__

#ifdef TAU_USE_DOCTEST

#include "tau.h"

namespace idni::tau_lang {

// Mirror tests/test_tau_helpers.h's bas_pack.
using test_node_t = node<tau_ba<bv, sbf_ba>, bv, sbf_ba>;
using sbf_node_t  = node<bv, sbf_ba>;

extern template bool is_tau_formula_sat<test_node_t>(tref, int_t, bool);

extern template struct tree    <test_node_t>;
extern template struct get_hook<test_node_t>;
extern template struct tau_ba  <bv, sbf_ba>;

extern template struct tree    <sbf_node_t>;
extern template struct get_hook<sbf_node_t>;

// Normalizer pipeline.
extern template tref normalizer                <test_node_t>(const rr<test_node_t>&);
extern template tref normalizer                <test_node_t>(tref);
extern template tref nso_rr_apply              <test_node_t>(const rewriter::rule&, const tref&);
extern template tref nso_rr_apply              <test_node_t>(const rewriter::rules&, tref);
extern template tref nso_rr_apply              <test_node_t>(const rr<test_node_t>&);
extern template tref calculate_all_fixed_points<test_node_t>(const rr<test_node_t>&);

} // namespace idni::tau_lang

#endif // TAU_USE_DOCTEST

#endif // __IDNI__TAU__EXTERN_TEMPLATE_TEST_PACK_H__
