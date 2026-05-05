// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "hooks"

#include <type_traits>

#include "boolean_algebras/cvc5/cvc5.h"
#include "boolean_algebras/qlt.h"
#include "ba_types.h"
#include "hooks.h"  // Only for IDE resolution, not really needed.

// Split into logical sub-files for readability.
// Include order matches the original code order.
#include "hooks_bf.tmpl.h"
#include "hooks_wff.tmpl.h"
#include "hooks_tau.tmpl.h"
