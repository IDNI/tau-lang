// Explicit instantiations for the artifact-only remainder of the pack
// templates a `tau compile` artifact's main.cpp uses -- on top of the
// shared set instantiate_pack.cpp already provides -- built into libTAU.a
// so the artifact TU links against these instead of re-instantiating the
// same heavy templates itself. See artifact_pack.def for the list and
// artifact_pack_extern.h for the matching extern template declarations.

#include "tau.h"
#include "tau_pack.h"
#include "table_step_provider.h"

namespace idni::tau_lang {

using artifact_node_t = tau_pack::node_t;

#define TAU_ARTIFACT_PACK_FN(ret, name, args) \
	template ret name<artifact_node_t> args;
#define TAU_ARTIFACT_PACK_CLASS(name) \
	template struct name<artifact_node_t>;

#include "artifact_pack.def"

#undef TAU_ARTIFACT_PACK_FN
#undef TAU_ARTIFACT_PACK_CLASS

} // namespace idni::tau_lang
