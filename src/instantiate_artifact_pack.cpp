// Explicit instantiations for the pack templates a `tau compile` artifact's
// main.cpp uses, built into libTAU.a so the artifact TU links against these
// instead of re-instantiating the same heavy templates itself. See
// artifact_pack.def for the shared list and artifact_pack_extern.h for the
// matching extern template declarations.

#include "tau.h"
#include "tau_pack.h"
#include "table_step_provider.h"

namespace idni::tau_lang {

using artifact_node_t = tau_pack::node_t;

#define TAU_ARTIFACT_PACK_FN(ret, name, args) \
	template ret name<artifact_node_t> args;
#define TAU_ARTIFACT_PACK_CLASS(name) \
	template struct name<artifact_node_t>;
#define TAU_ARTIFACT_PACK_TAU_BA() \
	template struct tau_ba<TAU_PACK_BASE_BAS>;

#include "artifact_pack.def"

#undef TAU_ARTIFACT_PACK_FN
#undef TAU_ARTIFACT_PACK_CLASS
#undef TAU_ARTIFACT_PACK_TAU_BA

} // namespace idni::tau_lang
