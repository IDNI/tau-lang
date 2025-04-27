// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__DEFS_H__
#define __IDNI__TAU__DEFS_H__

#include <algorithm>
#ifdef DEBUG
#	include <cxxabi.h>
#endif

#include "version_license.h" // include generated version and license constants

#include "init_log.h"

// include parser defs for DBG macro, int_t (int32_t) and mostly for
// common std::hash templates and specializations
#include "../external/parser/src/defs.h"

#define pfst(x) (*(x).begin())
#define hasbc(x, y, f) std::binary_search(x.begin(), x.end(), y, f)

// following macros work only if tau type is defined
// helper macro for pretty printing a tau tree tref into std::cout
#define TAU_PRINT(ref) (tau::get(ref).print(std::cout))
// helper macro for printing a tau tree tref into std::cout
#define TAU_TREE(ref) (tau::get(ref).print_tree(std::cout))
// helper macro for dumping a tau tree tref into std::cout
#define TAU_DUMP(ref) (tau::get(ref).dump(std::cout))
// helper macro for pretty printing a tau tree tref into to stream
#define TAU_PRINT_TO(ref, to) (tau::get(ref).print(to))
// helper macro for printing a tau tree tref into to stream
#define TAU_TREE_TO(ref, to) (tau::get(ref).print_tree(to))
// helper macro for dumping a tau tree tref into to stream
#define TAU_DUMP_TO(ref, to) (tau::get(ref).dump(to))
// helper macro for pretty printing a tau tree tref into a string
#define TAU_TO_STR(ref) (tau::get(ref).to_str())
// helper macro for printing a tau tree tref into a string
#define TAU_TREE_TO_STR(ref) (tau::get(ref).tree_to_str())
// helper macro for printing a tau tree tref into a string
#define TAU_DUMP_TO_STR(ref) (tau::get(ref).dump_to_str())

namespace idni::tau_lang {

using sym_t = int_t;

template <typename B>
struct zero {
	bool operator()(const B&) const;
};

template <typename B>
struct one {
	bool operator()(const B&) const;
};

template <typename T, typename V>
bool has(const T& t, const V& v) {
	return t.find(v) != t.end();
}

template <typename T, typename V>
bool hasv(const T& t, const V& v) {
	return std::find(t.begin(), t.end(), v) != t.end();
}

} // namespace idni::tau_lang

//-----------------------------------------------------------------------------
// GIT_* macros are populated at compile time by -D or they're set to "n/a"
#ifndef GIT_DESCRIBED
#define GIT_DESCRIBED   "n/a"
#endif
#ifndef GIT_COMMIT_HASH
#define GIT_COMMIT_HASH "n/a"
#endif
#ifndef GIT_BRANCH
#define GIT_BRANCH      "n/a"
#endif

#define SS(X) ((ss.str({}), idni::tau_lang_depreciating::operator<<(ss, X)), ss.str())

#endif // __IDNI__TAU__DEFS_H__
