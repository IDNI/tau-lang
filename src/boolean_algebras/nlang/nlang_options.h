// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file nlang_options.h
 * @brief The option set of the nlang algebra and the binding of each option
 * to its field.
 *
 * nlang_descriptor.tmpl.h includes this file. Its `declared_options()` gives
 * @ref nlang_option_set, which the pack declares, and its `bind_options()`
 * calls @ref nlang_bind_options.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__NLANG__NLANG_OPTIONS_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__NLANG__NLANG_OPTIONS_H__

#include <cstddef>
#include <string>
#include <string_view>

#include "utility/options.h"
#include "option_codecs.h"
#include "tau_diagnostics.h"
#include "boolean_algebras/nlang/nlang_ba.h"

namespace idni::tau_lang {

/// A text option that takes an empty text or a word @p valid accepts.
template <bool (*valid)(std::string_view)>
struct nlang_word_codec {
	std::string_view name;
	option_value to_value(const std::string& w) const { return w; }
	result<std::string> from_value(const option_value& v) const {
		result<std::string> r;
		const std::string* text =
			idni::detail::option_text_of(v, r.report());
		if (!text) return r;
		if (!text->empty() && !valid(*text))
			return r.with_error(code::invalid_argument,
				parser_strings::messages::option_bad_value,
				{ { label::name, name },
				  { label::value, *text } });
		return r.with_value(*text);
	}
};

inline bool nlang_provider_is_valid(std::string_view w) {
	return llm_provider_from_name(w).has_value();
}

/// The key reads back as `set` or `unset`, never as the key.
struct nlang_secret_codec {
	option_value to_value(const std::string& key) const {
		return std::string(key.empty() ? "unset" : "set");
	}
	result<std::string> from_value(const option_value& v) const {
		result<std::string> r;
		const std::string* text =
			idni::detail::option_text_of(v, r.report());
		if (!text) return r;
		return r.with_value(*text);
	}
};

/// The options of nlang, all of the LLM oracle. Each name carries the type
/// prefix, so the command line, the REPL and the environment use one name.
/// An empty text is "not given": @ref llm_config_from_env resolves it.
inline const option_set nlang_option_set{ {
	{ "nlang-http-timeout", "nlang", std::size_t{ nlang_http_timeout_sec },
		"cap each LLM oracle HTTP request at this many seconds "
		"(0 = no cap)" },
	{ "nlang-max-tokens", "nlang", std::size_t{ llm_default_max_tokens },
		"cap each LLM oracle reply at this many tokens, anthropic only "
		"(0 = the default)" },
	{ "nlang-fallback", "nlang", true,
		"let the anthropic API answer from another model when the named "
		"one declines" },
	{ "nlang-provider", "nlang", std::string{},
		"LLM oracle API, openai or anthropic (empty = detected from the "
		"endpoint and the keys, else openai)" },
	{ "nlang-endpoint", "nlang", std::string{},
		"LLM oracle API base URL (empty = the provider's)" },
	{ "nlang-model", "nlang", std::string{},
		"LLM oracle model (empty = none for openai and claude-opus-5-5 "
		"for anthropic)" },
	{ "nlang-api-key", "nlang", std::string{},
		"LLM oracle API key, read back as set or unset (empty = "
		"OPENAI_API_KEY or ANTHROPIC_API_KEY by provider)" },
	{ "nlang-effort", "nlang", std::string{},
		"anthropic effort: low, medium, high, xhigh or max (empty = low "
		"for the default model)" },
} };

/// Binds each option of @ref nlang_option_set to its field. An answer of one
/// model is not another's, so an option that can change who answers drops
/// the cached answers.
inline result<void> nlang_bind_options(options_repository& repo) {
	result<void> r;
	llm_options& o = nlang_llm_options();
	const option_hook hook = ba_option_hook();
	const option_hook answers = ba_option_hook(llm_clear_cache);
	TAU_TRY_VOID(repo.bind("nlang-http-timeout", nlang_http_timeout_sec,
		hook));
	TAU_TRY_VOID(repo.bind("nlang-max-tokens", o.max_tokens, hook));
	TAU_TRY_VOID(repo.bind("nlang-fallback", o.fallback, hook));
	TAU_TRY_VOID(repo.bind("nlang-provider", o.provider,
		nlang_word_codec<nlang_provider_is_valid>{ "nlang-provider" },
		answers));
	TAU_TRY_VOID(repo.bind("nlang-endpoint", o.endpoint, answers));
	TAU_TRY_VOID(repo.bind("nlang-model", o.model, answers));
	TAU_TRY_VOID(repo.bind("nlang-api-key", o.api_key,
		nlang_secret_codec{}, hook));
	TAU_TRY_VOID(repo.bind("nlang-effort", o.effort,
		nlang_word_codec<llm_effort_is_valid>{ "nlang-effort" }, answers));
	return r;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__NLANG__NLANG_OPTIONS_H__
