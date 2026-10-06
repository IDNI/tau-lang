// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__TAU_STRING_HASH_H__
#define __IDNI__TAU__TAU_STRING_HASH_H__

#include <cstdint>
#include <functional>
#include <string_view>

#include "utility/hashing.h"

/// Nonzero selects the platform-independent string hash (default 1).
#ifndef TAU_USE_PORTABLE_HASH
#define TAU_USE_PORTABLE_HASH 1
#endif

namespace idni::tau_lang {

/**
 * @brief Hashes a string (a name, a type, the text of a constant) for the tree.
 *
 * The tree order compares hashes first. With `TAU_USE_PORTABLE_HASH` (the
 * default) this is the parser's portable MurmurHash64A, which gives the same
 * value, and so the same printed order, on every platform; with it set to 0
 * it is `std::hash`, kept to benchmark against.
 * @param s The string to hash.
 * @return The 64-bit hash of @p s.
 */
inline std::uint64_t tau_string_hash(std::string_view s) {
#if TAU_USE_PORTABLE_HASH
	return idni::portable_string_hash(s);
#else
	return std::hash<std::string_view>{}(s);
#endif
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__TAU_STRING_HASH_H__
