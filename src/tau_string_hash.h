// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__TAU_STRING_HASH_H__
#define __IDNI__TAU__TAU_STRING_HASH_H__

#include <cstdint>
#include <functional>
#include <string_view>

#include "utility/hashing.h"

#ifndef TAU_USE_PORTABLE_HASH
#define TAU_USE_PORTABLE_HASH 1
#endif

namespace idni::tau_lang {

// The tree order compares hashes first. The portable hash gives the same order
// on every platform, and std::hash is there to benchmark against it.
inline std::uint64_t tau_string_hash(std::string_view s) {
#if TAU_USE_PORTABLE_HASH
	return idni::portable_string_hash(s);
#else
	return std::hash<std::string_view>{}(s);
#endif
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__TAU_STRING_HASH_H__
