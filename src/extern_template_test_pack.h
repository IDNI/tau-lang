// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Extern-template declarations matching src/instantiate_pack.cpp.
//
// Force-included into each test TU by tests/CMakeLists.txt's
// `_tau_add_test_target` (via `-include`, `/FI` on MSVC), except the suites
// it lists in `_no_test_pack`. When a test TU sees these `extern template`
// lines, the compiler does NOT generate a local instantiation — the linker
// resolves the symbol against libTAU.a's pre-instantiated copy. The CLI
// (main.cpp) does not include it and instantiates per TU.
//
// Recipe: mirror the list at the top of src/instantiate_pack.cpp; the six
// entities shared with an emitted artifact's main.cpp live in
// pack_core.def instead (see artifact_pack_extern.h for its other user).

#ifndef __IDNI__TAU__EXTERN_TEMPLATE_TEST_PACK_H__
#define __IDNI__TAU__EXTERN_TEMPLATE_TEST_PACK_H__

#ifdef TAU_USE_DOCTEST

#include "ltl_aba.h"
#include "satisfiability.h"
#include "tau_pack.h"

namespace idni::tau_lang {

/// The node type of the configured pack, the one the tests instantiate.
using test_node_t = tau_pack::node_t;

/// pack_core.def callbacks: declare each entry as an extern instantiation.
#define TAU_PACK_FN(ret, name, args) \
	extern template ret name<test_node_t> args;
#define TAU_PACK_CLASS(name) \
	extern template struct name<test_node_t>;
#define TAU_PACK_TAU_BA() \
	extern template struct tau_ba<TAU_PACK_BASE_BAS>;

#include "pack_core.def"

#undef TAU_PACK_FN
#undef TAU_PACK_CLASS
#undef TAU_PACK_TAU_BA

// Mirrors src/instantiate_pack.cpp, guards included.
/// Node over Bool alone.
using bool_node_t = node<Bool>;

extern template struct tree    <bool_node_t>;
extern template struct get_hook<bool_node_t>;

// Normalizer pipeline; declarations arrive via ltl_aba.h -> normalizer.h.
extern template result<tref> normalizer       <bool_node_t>(const rr<bool_node_t>&);
extern template result<tref> normalizer       <bool_node_t>(tref);
extern template tref nso_rr_apply             <bool_node_t>(const rewriter::rule&, const tref&);
extern template tref nso_rr_apply             <bool_node_t>(const rewriter::rules&, tref);
extern template result<tref> nso_rr_apply     <bool_node_t>(const rr<bool_node_t>&);
extern template result<tref> calculate_all_fixed_points<bool_node_t>(const rr<bool_node_t>&);

#ifdef TAU_PACK_HAS_BA_BV

/// Node over bv and Bool.
using bv_bool_node_t = node<bv, Bool>;

extern template struct tree    <bv_bool_node_t>;
extern template struct get_hook<bv_bool_node_t>;

#ifdef TAU_PACK_HAS_BA_SBF

/// Node over bv and sbf.
using bv_sbf_node_t = node<bv, sbf_ba>;

extern template struct tree    <bv_sbf_node_t>;
extern template struct get_hook<bv_sbf_node_t>;

#endif // TAU_PACK_HAS_BA_SBF
#endif // TAU_PACK_HAS_BA_BV

// Mirrors src/instantiate_pack.cpp's sbf-only fixture; guarded on its
// own since a pack can hold sbf without bv (e.g. -DTAU_BAS=sbf,tau,qint).
#ifdef TAU_PACK_HAS_BA_SBF

/// Node over sbf and Bool.
using sbf_bool_node_t = node<sbf_ba, Bool>;

extern template struct tree    <sbf_bool_node_t>;
extern template struct get_hook<sbf_bool_node_t>;

#endif // TAU_PACK_HAS_BA_SBF

} // namespace idni::tau_lang

#endif // TAU_USE_DOCTEST

#endif // __IDNI__TAU__EXTERN_TEMPLATE_TEST_PACK_H__
