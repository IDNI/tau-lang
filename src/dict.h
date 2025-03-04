// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __DICT_H__
#define __DICT_H__

#include "defs.h"
#include <string>

sym_t dict(const char*);
sym_t dict(const std::string&);
const char* dict(sym_t);
bool has(sym_t);

#endif // __DICT_H__
