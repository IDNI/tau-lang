// Explicit template instantiations for the BA pack used by every
// test binary and by the production CLI binary.
//
// The dominant node pack is the configured one, tau_pack::node_t (also
// bas_pack in tests/test_tau_helpers.h, used by 40+ unit tests AND by
// src/main.cpp).
//
// Without explicit instantiations, every consumer TU re-instantiates
// the heaviest pipeline templates (is_ltl_aba_realizable,
// is_tau_formula_sat, has_ltl_operators) from scratch, paying the
// cost of expanding ~250 KB of .tmpl.h once per binary.
//
// With the matching `extern template` declarations in
// src/extern_template_test_pack.h (force-included into every test
// TU by tests/CMakeLists.txt's `add()`), the linker resolves these
// symbols against libTAU.a's pre-instantiated copy.
//
// Recipe to add a function: append `template <ret> fn<test_node_t>(...);`
// to the list below and mirror it with `extern template ...` in the
// header.

#include "tau.h"
#include "tau_pack.h"

namespace idni::tau_lang {

// The configured pack; mirrored by tests/test_tau_helpers.h's bas_pack.
using test_node_t = tau_pack::node_t;

// Top-level synthesis entries.
template bool is_ltl_aba_realizable<test_node_t>(tref, int_t, bool);
template bool is_tau_formula_sat   <test_node_t>(tref, int_t, bool);
template bool has_ltl_operators    <test_node_t>(tref);

template struct tree    <test_node_t>;
template struct get_hook<test_node_t>;
template struct tau_ba  <TAU_PACK_BASE_BAS>;

// The pack-agnostic fixture of tests/test_bool_only_helpers.h; Bool is
// described in every pack, so this needs no guard.
using bool_node_t = node<Bool>;

template struct tree    <bool_node_t>;
template struct get_hook<bool_node_t>;

// Normalizer pipeline.  Only the Bool pack is worth hoisting: the tau and
// sbf packs measured byte-identical objects without it.
template tref normalizer               <bool_node_t>(const rr<bool_node_t>&);
template tref normalizer               <bool_node_t>(tref);
template tref nso_rr_apply             <bool_node_t>(const rewriter::rule&, const tref&);
template tref nso_rr_apply             <bool_node_t>(const rewriter::rules&, tref);
template tref nso_rr_apply             <bool_node_t>(const rr<bool_node_t>&);
template tref calculate_all_fixed_points<bool_node_t>(const rr<bool_node_t>&);

// The hardcoded fixtures of tests/test_Bool_helpers.h and
// tests/test_sbf_ba_helpers.h, which name bv and sbf whatever the pack is.
#ifdef TAU_PACK_HAS_BA_BV

using bv_bool_node_t = node<bv, Bool>;

template struct tree    <bv_bool_node_t>;
template struct get_hook<bv_bool_node_t>;

#ifdef TAU_PACK_HAS_BA_SBF

using bv_sbf_node_t = node<bv, sbf_ba>;

template struct tree    <bv_sbf_node_t>;
template struct get_hook<bv_sbf_node_t>;

#endif // TAU_PACK_HAS_BA_SBF
#endif // TAU_PACK_HAS_BA_BV

} // namespace idni::tau_lang
