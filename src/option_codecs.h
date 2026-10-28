// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file option_codecs.h
 * @brief The option codecs and the option hook that core and the algebras
 * share. No engine header is included, so a BA plugin can bind with them.
 */

#ifndef __IDNI__TAU__OPTION_CODECS_H__
#define __IDNI__TAU__OPTION_CODECS_H__

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

#include "grammar_options.h"
#include "utility/hashing.h"
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

/// A count that stores at most @p cap: a larger value reads as @p cap, and
/// so does 0 when @p zero_is_cap.
struct capped_count_codec {
	std::size_t cap;
	bool zero_is_cap = false;
	option_value to_value(std::size_t n) const { return n; }
	result<std::size_t> from_value(const option_value& v) const {
		result<std::size_t> r;
		const auto* n = std::get_if<std::size_t>(&v);
		if (!n) return r.with_error(code::type_error,
			parser_strings::messages::option_value_kind);
		return r.with_value((*n == 0 && zero_is_cap) || *n > cap
			? cap : *n);
	}
};

/// @p r with the error of option @p name refusing the value @p text.
template <typename T>
[[nodiscard]] result<T>&& option_bad_value(result<T>& r, std::string_view name,
	const std::string& text)
{
	return r.with_error(code::invalid_argument,
		parser_strings::messages::option_bad_value,
		{ { label::name, name }, { label::value, text } });
}

/// A text option that takes an empty text or a word @p valid accepts.
template <bool (*valid)(std::string_view)>
struct checked_word_codec {
	std::string_view name;
	option_value to_value(const std::string& w) const { return w; }
	result<std::string> from_value(const option_value& v) const {
		result<std::string> r;
		const std::string* text =
			idni::detail::option_text_of(v, r.report());
		if (!text) return r;
		if (!text->empty() && !valid(*text))
			return option_bad_value(r, name, *text);
		return r.with_value(*text);
	}
};

/// A text option that takes one of @p words, or empty. `none` is stored as
/// `no` where `no` is a word, since ltlsynt reads `no`.
struct word_set_codec {
	std::string_view name;
	std::vector<std::string_view> words;
	option_value to_value(const std::string& word) const { return word; }
	result<std::string> from_value(const option_value& v) const {
		result<std::string> r;
		const std::string* text =
			idni::detail::option_text_of(v, r.report());
		if (!text) return r;
		auto known = [&](std::string_view w) {
			return std::find(words.begin(), words.end(), w) != words.end();
		};
		if (*text == "none" && known("no"))
			return r.with_value(std::string("no"));
		if (!text->empty() && !known(*text))
			return option_bad_value(r, name, *text);
		return r.with_value(*text);
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
inline std::uint64_t ba_options_fingerprint(std::uint64_t seed = 0) {
	const std::uint64_t g =
		ba_options_generation.load(std::memory_order_relaxed);
	return seed ^ (g + grcprime + (seed << 6) + (seed >> 2));
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__OPTION_CODECS_H__
