// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file tau_ba_options.h
 * @brief The option set of the tau algebra and the binding of each option to
 * its field.
 *
 * tau_descriptor.tmpl.h includes this file. Its `declared_options()` gives
 * @ref tau_ba_option_set, which the pack declares, and its `bind_options()`
 * calls @ref tau_ba_bind_options.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__TAU__TAU_BA_OPTIONS_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__TAU__TAU_BA_OPTIONS_H__

#include <charconv>
#include <cstddef>
#include <string>
#include <string_view>

#include "utility/options.h"
#include "option_codecs.h"
#include "tau_diagnostics.h"
#include "tau_tree.h"
#include "boolean_algebras/tau/tau_ba.h"

namespace idni::tau_lang {

/// A count whose 0 keeps nothing, so it reads and takes `none` for 0.
struct none_is_zero_codec {
	std::string_view name;
	option_value to_value(std::size_t n) const {
		return n ? std::to_string(n) : std::string("none");
	}
	result<std::size_t> from_value(const option_value& v) const {
		result<std::size_t> r;
		const std::string* text =
			idni::detail::option_text_of(v, r.report());
		if (!text) return r;
		if (*text == "none") return r.with_value(0);
		std::size_t n = 0;
		const char* last = text->data() + text->size();
		auto [end, ec] = std::from_chars(text->data(), last, n);
		if (text->empty() || ec != std::errc{} || end != last)
			return r.with_error(code::invalid_argument,
				parser_strings::messages::option_bad_value,
				{ { label::name, name },
				  { label::value, *text } });
		return r.with_value(n);
	}
};

/// The options of the tau algebra. Each name carries the prefix `ba` (see
/// @ref ba_option_prefix).
inline const option_set tau_ba_option_set{ {
	{ "ba-component-factoring", "tau", bool{ ba_component_factoring },
		"decide a Tau-BA constant one variable-disjoint component at a "
		"time" },
	{ "ba-decision-pins", "tau",
		none_is_zero_codec{}.to_value(ba_decision_pins),
		"cap the decided Tau-BA rows whose key tree is kept alive "
		"across the sweep (none or 0 keeps none)" },
} };

/// Binds each option of @ref tau_ba_option_set to its field.
template <NodeType node>
result<void> tau_ba_bind_options(options_repository& repo) {
	result<void> r;
	// The factoring decides which verdicts the tree caches hold.
	TAU_TRY_VOID(repo.bind("ba-component-factoring", ba_component_factoring,
		ba_option_hook([] { tree<node>::clear_caches(); })));
	TAU_TRY_VOID(repo.bind("ba-decision-pins", ba_decision_pins,
		none_is_zero_codec{ "ba-decision-pins" }, ba_option_hook()));
	return r;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__TAU__TAU_BA_OPTIONS_H__
