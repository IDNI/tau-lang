// Explicit template instantiations for the BA pack used by every
// test binary and by the production CLI binaries.
//
// The dominant node pack (defined as bas_pack in
// tests/test_tau_helpers.h and used by 40+ unit tests AND by
// src/main.cpp / src/tau_codegen_cli.cpp) is:
//
//   node<tau_ba<qint, qlt, nlang_ba, bv, sbf_ba, hsb>,
//        qint, qlt, nlang_ba, bv, sbf_ba, hsb>
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

namespace idni::tau_lang {

// Mirror tests/test_tau_helpers.h's bas_pack.
using test_node_t = node<tau_ba<qint, qlt, nlang_ba, bv, sbf_ba, hsb>,
                         qint, qlt, nlang_ba, bv, sbf_ba, hsb>;

// Top-level synthesis entries.
template bool is_ltl_aba_realizable<test_node_t>(tref, int_t, bool);
template bool is_tau_formula_sat   <test_node_t>(tref, int_t, bool);
template bool has_ltl_operators    <test_node_t>(tref);

} // namespace idni::tau_lang
