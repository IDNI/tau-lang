// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file option_codecs.h
 * @brief The option codecs and the option hook that core and the algebras
 * share. No engine header is included, so a BA plugin can bind with them.
 */

#ifndef __IDNI__TAU__OPTION_CODECS_H__
#define __IDNI__TAU__OPTION_CODECS_H__

#include <atomic>
#include <cstddef>
#include <limits>
#include <utility>
#include <variant>

#include "utility/options.h"
#include "tau_diagnostics.h"

namespace idni::tau_lang {

/// A budget whose loops count down from SIZE_MAX: the option reads and
/// writes 0 for it, as its api setter does.
struct zero_is_unlimited_codec {
	option_value to_value(std::size_t n) const {
		return n == std::numeric_limits<std::size_t>::max()
			? std::size_t{ 0 } : n;
	}
	result<std::size_t> from_value(const option_value& v) const {
		result<std::size_t> r;
		const auto* n = std::get_if<std::size_t>(&v);
		if (!n) return r.with_error(code::type_error,
			parser_strings::messages::option_value_kind);
		return r.with_value(*n ? *n
			: std::numeric_limits<std::size_t>::max());
	}
};

/// Moves on each write of a BA option, so a memo keyed on a formula drops
/// what the old option values decided.
inline std::atomic<std::size_t> ba_options_generation{ 0 };

/// The hook every BA option binds with: it moves
/// @ref ba_options_generation, then runs @p own.
inline option_hook ba_option_hook(option_hook own = {}) {
	return [own = std::move(own)] {
		ba_options_generation.fetch_add(1, std::memory_order_relaxed);
		if (own) own();
	};
}

/// @p seed mixed with @ref ba_options_generation.
inline std::size_t ba_options_fingerprint(std::size_t seed = 0) {
	const std::size_t g =
		ba_options_generation.load(std::memory_order_relaxed);
	return seed ^ (g + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2));
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__OPTION_CODECS_H__
