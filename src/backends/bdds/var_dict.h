// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BACKENDS__BDDS__VAR_DICT_H__
#define __IDNI__TAU__BACKENDS__BDDS__VAR_DICT_H__

#include <string>

#include "defs.h"
#include "tau_diagnostics.h"

namespace idni::tau_lang {

using sym_t = int_t;

// Global dictionary mapping BDD variable names to their numeric
// symbols and back (definitions in var_dict.cpp). The table is process
// global and not synchronized.

/**
 * @brief Intern a variable name.
 * @param s Null-terminated variable name.
 * @return The symbol of @p s; a fresh symbol (starting at 1) is allocated
 *         and registered the first time the name is seen.
 */
sym_t var_dict(const char*);
/// @brief std::string overload of var_dict(const char*).
sym_t var_dict(const std::string&);
/**
 * @brief Name of a symbol.
 * @param n Symbol id. When @p n is one past the last known symbol, a fresh
 *          name `x<k>` (the first k >= n not yet registered) is generated
 *          and registered.
 * @return The name; an out-of-range error when @p n is more than one past
 *         the last known symbol.
 */
result<std::string> var_dict(sym_t);

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BACKENDS__BDDS__VAR_DICT_H__
