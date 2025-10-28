// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__DEFS_H__
#define __IDNI__TAU__DEFS_H__

#include <cstddef>
#include <variant>
#ifdef DEBUG
#	include <cxxabi.h>     // unmangle symbol names for debugging
#endif

//-----------------------------------------------------------------------------
// GIT_* macros are populated at compile time by -D or they're set to "n/a"
#ifndef TAU_GIT_DESCRIBED
#define TAU_GIT_DESCRIBED   "n/a"
#endif
#ifndef TAU_GIT_COMMIT_HASH
#define TAU_GIT_COMMIT_HASH "n/a"
#endif
#ifndef TAU_GIT_BRANCH
#define TAU_GIT_BRANCH      "n/a"
#endif

// include generated version and license constants from VERSION and LICENSE.txt
#include "version_license.h"

// initialize logging and include logging helper macros
#include "logging.h"

// include parser defs for DBG macro, int_t (int32_t) and mostly for
// common std::hash templates and specializations
#include "../external/parser/src/defs.h"

// -----------------------------------------------------------------------------
// helper macros for printing

// following macros work only if `node` type alias is defined
// `using node = tau_lang::node<BAs...>;` where `BAs...` is a pack of Boolean Algebras)
// argument `ref` is a tree pointer reference `tref`,
// or shared pointer handle `htref`

// helper macro for pretty printing a tau tree tref into std::cout
#define TAU_PRINT(ref) (tree<node>::get(ref).print(std::cout))
// helper macro for printing a tau tree tref into std::cout
#define TAU_PRINT_TREE(ref) (tree<node>::get(ref).print_tree(std::cout))
// helper macro for dumping a tau tree tref into std::cout
#define TAU_DUMP(ref) (tree<node>::get(ref).dump(std::cout))
// helper macro for pretty printing a tau tree tref into to stream
#define TAU_PRINT_TO(ref, to) (tree<node>::get(ref).print(to))
// helper macro for printing a tau tree tref into to stream
#define TAU_PRINT_TREE_TO(ref, to) (tree<node>::get(ref).print_tree(to))
// helper macro for dumping a tau tree tref into to stream
#define TAU_DUMP_TO(ref, to) (tree<node>::get(ref).dump(to))
// helper macro for pretty printing a tau tree tref into a string
#define TAU_TO_STR(ref) (tree<node>::get(ref).to_str())
// helper macro for printing a tau tree tref into a string
#define TAU_TREE_TO_STR(ref) (tree<node>::get(ref).tree_to_str())
// helper macro for printing a tau tree tref into a string
#define TAU_DUMP_TO_STR(ref) (tree<node>::get(ref).dump_to_str())

namespace idni::tau_lang {

// helper to get size of a type pack
template <typename... Pack>
constexpr size_t Pack_size = std::variant_size<std::variant<Pack...>>::value;

// helper to get bitsize required for size of a type pack
template <typename... Pack>
constexpr size_t Pack_bitsize = Pack_size<Pack...> <= 1 ? 1 :
        (sizeof(size_t) * 8 - __builtin_clzl(Pack_size<Pack...> - 1));

} // namespace idni::tau_lang

#endif // __IDNI__TAU__DEFS_H__
