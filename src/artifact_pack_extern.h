// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__ARTIFACT_PACK_EXTERN_H__
#define __IDNI__TAU__ARTIFACT_PACK_EXTERN_H__

// extern template declarations matching instantiate_artifact_pack.cpp's
// definitions in libTAU.a, so an emitted artifact TU reuses that copy of the
// pack templates instead of instantiating them again itself. Include after
// tau.h, tau_pack.h and table_step_provider.h: extern template needs the
// templates already declared. See artifact_pack.def for the shared list.

#include <string>

namespace idni::tau_lang {

using artifact_node_t = tau_pack::node_t;

#define TAU_ARTIFACT_PACK_FN(ret, name, args) \
	extern template ret name<artifact_node_t> args;
#define TAU_ARTIFACT_PACK_CLASS(name) \
	extern template struct name<artifact_node_t>;
#define TAU_ARTIFACT_PACK_TAU_BA() \
	extern template struct tau_ba<TAU_PACK_BASE_BAS>;

#include "artifact_pack.def"

#undef TAU_ARTIFACT_PACK_FN
#undef TAU_ARTIFACT_PACK_CLASS
#undef TAU_ARTIFACT_PACK_TAU_BA

} // namespace idni::tau_lang

#endif // __IDNI__TAU__ARTIFACT_PACK_EXTERN_H__
