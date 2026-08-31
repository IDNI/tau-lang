// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BDDS__VAR_DICT_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BDDS__VAR_DICT_H__

#include <string>

#include "defs.h"

namespace idni::tau_lang {

using sym_t = int_t;

// Global dictionary mapping BDD variable names to their numeric
// symbols and back (definitions in var_dict.cpp).

// Intern a variable name, allocating a fresh symbol on first sight
sym_t var_dict(const char*);
sym_t var_dict(const std::string&);
// Name of symbol n; when n is one past the last known symbol, a fresh
// unique name of the form "x<k>" is generated and registered
std::string var_dict(sym_t);

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BDDS__VAR_DICT_H__
