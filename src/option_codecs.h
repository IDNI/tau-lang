// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file option_codecs.h
 * @brief The option codecs that core and the algebras share. No engine
 * header is included, so a BA plugin can bind with them.
 */

#ifndef __IDNI__TAU__OPTION_CODECS_H__
#define __IDNI__TAU__OPTION_CODECS_H__

#include <cstddef>
#include <limits>
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

} // namespace idni::tau_lang

#endif // __IDNI__TAU__OPTION_CODECS_H__
