// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file defs.h
 * @brief Core macros, compile-time helpers, and GIT version constants for Tau.
 *
 * Defines `hasb`/`hasbc`/`sortc` search/sort macros, `erase_at` and the
 * `TAU_PRINT*` family of tree-printing macros. Also initializes logging and includes the
 * external parser's `defs.h`.
 */

#ifndef __IDNI__TAU__DEFS_H__
#define __IDNI__TAU__DEFS_H__

#include <cstddef>
#include <variant>
#ifdef DEBUG
#	if !defined(_MSC_VER)
#		include <cxxabi.h>     // unmangle symbol names for debugging
#	endif
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

// include generated version and license constants from VERSION and LICENSE.md
#include "version_license.h"

// initialize logging and include logging helper macros
#include "logging.h"

// include parser defs for DBG macro, int_t (int32_t) and mostly for
// common std::hash templates and specializations. In installed parser mode the
// parser is an SDK package and its defs.h comes from the package include path.
// Subdir mode names the in-tree path through TAU_PARSER_DEFS_INCLUDE so the
// header text never names the parser source path in an installed SDK.
#ifdef TAU_PARSER_DEFS_INSTALLED
#include <tauparser/defs.h>
#else
#include TAU_PARSER_DEFS_INCLUDE
#endif

// Macros to ease searching and sorting
/// @brief Binary search of @p y in the sorted range @p x.
#define hasb(x, y) (std::binary_search(x.begin(), x.end(), y))
/// @brief Binary search of @p y in @p x, sorted by comparator @p f.
#define hasbc(x, y, f) (std::binary_search(x.begin(), x.end(), y, f))
/// @brief Sort @p x in place with comparator @p f.
#define sortc(x, f) (std::sort(x.begin(), x.end(), f))

/**
 * @brief Erase the element at @p index of @p v.
 *
 * A vector offset is signed, so the index converts once here instead of at
 * each call site.
 * @tparam V A sequence container with random-access iterators.
 * @param v Container to erase from.
 * @param index Position of the element; must be less than `v.size()`.
 */
template <typename V>
void erase_at(V& v, size_t index) {
	v.erase(v.begin() + static_cast<std::ptrdiff_t>(index));
}

// -----------------------------------------------------------------------------
// helper macros for printing

// following macros work only if `node` type alias is defined
// `using node = tau_lang::node<BAs...>;` where `BAs...` is a pack of Boolean Algebras)
// argument `ref` is a tree pointer reference `tref`,
// or shared pointer handle `htref`

/// @brief Pretty print the tree @p ref into std::cout.
#define TAU_PRINT(ref) (tree<node>::get(ref).print(std::cout))
/// @brief Print the node structure of the tree @p ref into std::cout.
#define TAU_PRINT_TREE(ref) (tree<node>::get(ref).print_tree(std::cout))
/// @brief Dump the tree @p ref (debug dump form) into std::cout.
#define TAU_DUMP(ref) (tree<node>::get(ref).dump(std::cout))
/// @brief Pretty print the tree @p ref into the stream @p to.
#define TAU_PRINT_TO(ref, to) (tree<node>::get(ref).print(to))
/// @brief Print the node structure of the tree @p ref into the stream @p to.
#define TAU_PRINT_TREE_TO(ref, to) (tree<node>::get(ref).print_tree(to))
/// @brief Dump the tree @p ref (debug dump form) into the stream @p to.
#define TAU_DUMP_TO(ref, to) (tree<node>::get(ref).dump(to))
/// @brief Pretty print the tree @p ref into a string.
#define TAU_TO_STR(ref) (tree<node>::get(ref).to_str())
/// @brief Render the node structure of the tree @p ref as a string.
#define TAU_TREE_TO_STR(ref) (tree<node>::get(ref).tree_to_str())
/// @brief Dump the tree @p ref (debug dump form) into a string.
#define TAU_DUMP_TO_STR(ref) (tree<node>::get(ref).dump_to_str())

namespace idni::tau_lang {

} // namespace idni::tau_lang

#endif // __IDNI__TAU__DEFS_H__
