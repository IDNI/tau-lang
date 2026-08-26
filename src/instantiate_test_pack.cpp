// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Explicit template instantiations for the BA pack used by (almost) every
// test binary and by the tau CLI.
//
// The dominant node pack (defined as bas_pack in tests/test_tau_helpers.h,
// and used by the bulk of the unit/integration tests as well as
// src/main.cpp) is:
//
//   node<tau_ba<bv, sbf_ba>, bv, sbf_ba>
//
// Without explicit instantiations, every consumer TU re-instantiates the
// heaviest pipeline templates (is_tau_formula_sat, the normalizer pipeline,
// tree, get_hook, tau_ba) from scratch.
//
// With the matching `extern template` declarations in
// src/extern_template_test_pack.h (force-included into every test TU by
// tests/CMakeLists.txt's `add()`), the linker resolves these symbols against
// libTAU's pre-instantiated copy.
//
// Recipe to add an entry: append it here and mirror it with
// `extern template ...` in extern_template_test_pack.h.

#include "tau.h"

namespace idni::tau_lang {

// Mirror tests/test_tau_helpers.h's bas_pack. One alias to update if the
// pack ever changes.
using test_node_t = node<tau_ba<bv, sbf_ba>, bv, sbf_ba>;
using sbf_node_t  = node<bv, sbf_ba>;

// Top-level entry point.
template bool is_tau_formula_sat<test_node_t>(tref, int_t, bool);

template struct tree    <test_node_t>;
template struct get_hook<test_node_t>;
template struct tau_ba  <bv, sbf_ba>;

template struct tree    <sbf_node_t>;
template struct get_hook<sbf_node_t>;

// Normalizer pipeline.
template tref normalizer                <test_node_t>(const rr<test_node_t>&);
template tref normalizer                <test_node_t>(tref);
template tref nso_rr_apply              <test_node_t>(const rewriter::rule&, const tref&);
template tref nso_rr_apply              <test_node_t>(const rewriter::rules&, tref);
template tref nso_rr_apply              <test_node_t>(const rr<test_node_t>&);
template tref calculate_all_fixed_points<test_node_t>(const rr<test_node_t>&);

} // namespace idni::tau_lang
