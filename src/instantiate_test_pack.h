// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Shared body of src/instantiate_test_pack_<shard>.cpp. Each of those defines
// TAU_TEST_PACK_IN_<shard>(...) to keep its own lines of
// src/test_pack_instances.def, which every test TU declares extern through
// src/extern_template_test_pack.h. The lines are spread over eight TUs so no
// one compile carries them all.
//
// Built into the TAU_test_pack library only (tests/CMakeLists.txt): the CLI,
// the SDK and an emitted artifact never use these instantiations.

#include "tau.h"
#include "api.h"
#include "tau_spec.h"
#include "tau_pack.h"

#ifndef TAU_TEST_PACK_IN_0
#define TAU_TEST_PACK_IN_0(...)
#endif
#ifndef TAU_TEST_PACK_IN_1
#define TAU_TEST_PACK_IN_1(...)
#endif
#ifndef TAU_TEST_PACK_IN_2
#define TAU_TEST_PACK_IN_2(...)
#endif
#ifndef TAU_TEST_PACK_IN_3
#define TAU_TEST_PACK_IN_3(...)
#endif
#ifndef TAU_TEST_PACK_IN_4
#define TAU_TEST_PACK_IN_4(...)
#endif
#ifndef TAU_TEST_PACK_IN_5
#define TAU_TEST_PACK_IN_5(...)
#endif
#ifndef TAU_TEST_PACK_IN_6
#define TAU_TEST_PACK_IN_6(...)
#endif
#ifndef TAU_TEST_PACK_IN_7
#define TAU_TEST_PACK_IN_7(...)
#endif

// The grammar every generated parser builds; the parser library leaves it to
// each user.
TAU_TEST_PACK_IN_0(
template struct idni::grammar<char, char>;
template idni::char_class_fns<char> idni::predefined_char_classes<char, char>(
	const std::vector<std::string>&, idni::nonterminals<char, char>&,
	idni::diagnostics::report*);
template idni::char_class_fns<char> idni::predefined_char_classes<char, char>(
	const std::vector<std::string>&, const std::map<std::string, std::string>&,
	idni::nonterminals<char, char>&, idni::diagnostics::report*);
)

namespace idni::tau_lang {

/// The configured pack, as src/extern_template_test_pack.h names it.
using test_node_t = tau_pack::node_t;

#define TAU_TEST_PACK_INST(shard, ...) \
	TAU_TEST_PACK_IN_##shard(template __VA_ARGS__;)

#include "test_pack_instances.def"

#undef TAU_TEST_PACK_INST

} // namespace idni::tau_lang
