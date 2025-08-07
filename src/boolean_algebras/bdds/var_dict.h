// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BDDS__VAR_DICT_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BDDS__VAR_DICT_H__

#include <string>

#include "defs.h"

namespace idni::tau_lang {

using sym_t = int_t;

sym_t var_dict(const char*);
sym_t var_dict(const std::string&);
const char* var_dict(sym_t);
bool has(sym_t);

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BDDS__VAR_DICT_H__
