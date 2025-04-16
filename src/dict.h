// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__DICT_H__
#define __IDNI__TAU__DICT_H__

#include <string>

#include "defs.h"

namespace idni::tau_lang {

// bdd var dict
sym_t dict(const char*);
sym_t dict(const std::string&);
const char* dict(sym_t);
bool has(sym_t);

// tau strings dict
const std::string& string_from_id(size_t id);
size_t string_id(const std::string& s);

} // namespace idni::tau_lang

#endif // __IDNI__TAU__DICT_H__
