// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file nlang_llm.h
 * @brief The provider layer of the nlang oracle: which LLM API a query goes
 * to, the HTTP request that carries it and the text its reply holds.
 *
 * Nothing here opens a connection. `llm_query` in nlang_ba.cpp owns the
 * transport and calls these functions around it.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__NLANG__NLANG_LLM_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__NLANG__NLANG_LLM_H__

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace idni::tau_lang {

/** @brief The wire protocol of the LLM endpoint. */
enum class llm_provider {
	openai,   ///< chat completions (`<endpoint>/chat/completions`)
	anthropic ///< Messages API (`<endpoint>/messages`)
};

/// The provider @p name spells (`openai`, `anthropic`), or nothing.
std::optional<llm_provider> llm_provider_from_name(std::string_view name);
/// The word @ref llm_provider_from_name reads back.
const char* llm_provider_name(llm_provider p);
/// `true` for `low`, `medium`, `high`, `xhigh` and `max`.
bool llm_effort_is_valid(std::string_view effort);

/// The model an anthropic request names when none is configured.
inline constexpr const char* llm_default_anthropic_model = "claude-opus-5-5";
inline constexpr const char* llm_default_openai_endpoint =
	"https://api.openai.com/v1";
inline constexpr const char* llm_default_anthropic_endpoint =
	"https://api.anthropic.com/v1";
/// Default of `nlang-max-tokens`.
inline constexpr size_t llm_default_max_tokens = 16000;

/**
 * @brief The `nlang-*` LLM options as given: an empty member is "not set",
 * and its environment variable, then its default, applies.
 *
 * | member       | option             | environment fallback |
 * |--------------|--------------------|----------------------|
 * | `provider`   | `nlang-provider`   | `TAU_LLM_PROVIDER`   |
 * | `endpoint`   | `nlang-endpoint`   | `TAU_LLM_ENDPOINT`   |
 * | `model`      | `nlang-model`      | `TAU_LLM_MODEL`      |
 * | `api_key`    | `nlang-api-key`    | `TAU_LLM_API_KEY`, then the provider's own (`OPENAI_API_KEY`, `ANTHROPIC_API_KEY`) |
 * | `effort`     | `nlang-effort`     | `TAU_LLM_EFFORT`     |
 * | `max_tokens` | `nlang-max-tokens` | `TAU_LLM_MAX_TOKENS` |
 * | `fallback`   | `nlang-fallback`   | `TAU_LLM_FALLBACK`   |
 */
struct llm_options {
	std::optional<std::string> provider, endpoint, model, api_key, effort;
	std::optional<size_t> max_tokens;
	std::optional<bool> fallback;
};

/// The options of this process, written by the `nlang-*` option setters.
llm_options& nlang_llm_options();

/** @brief One resolved configuration: everything a request needs. */
struct llm_config {
	llm_provider provider = llm_provider::openai;
	std::string api_key;
	std::string endpoint;  ///< base URL, no trailing slash
	std::string model;     ///< empty: the request names none (openai only)
	size_t max_tokens = llm_default_max_tokens; ///< anthropic only
	std::string effort;    ///< empty: none sent (anthropic only)
	/// Ask the anthropic API to answer from another model when the
	/// named one declines the request.
	bool server_fallback = false;

	/// Without a key no query leaves the process.
	bool has_key() const { return !api_key.empty(); }
};

/** @brief An HTTP POST, ready for a transport. */
struct llm_http_request {
	std::string url;
	std::vector<std::string> headers; ///< `Name: value`
	std::string body;
};

/**
 * @brief Resolve a configuration: each option of @p opts when set, else its
 * environment variable, else its default.
 *
 * The provider, when neither the option nor `TAU_LLM_PROVIDER` names one, is
 * `anthropic` when the endpoint names an anthropic host or when
 * `ANTHROPIC_API_KEY` is the only key configured, and `openai` otherwise. An
 * anthropic request needs a model, so it defaults to
 * @ref llm_default_anthropic_model; that default model alone gets effort
 * `low` and, on the default endpoint, the server-side fallback. A model the
 * user names gets neither unless asked for, since not every model accepts
 * them. A `max_tokens` of 0 is the default.
 */
llm_config llm_config_from_env(const llm_options& opts = nlang_llm_options());

/// The request that puts @p prompt to the endpoint of @p cfg as one user
/// message.
llm_http_request llm_build_request(const llm_config& cfg,
	const std::string& prompt);

/**
 * @brief The answer text in @p response, the body of a successful reply.
 *
 * Empty when the reply holds no answer: it is not JSON, it is an error
 * object, it has no text, or (anthropic) the model refused or was cut off at
 * `max_tokens`. @p why then receives the reason.
 */
std::string llm_extract_text(llm_provider provider,
	const std::string& response, std::string* why = nullptr);

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__NLANG__NLANG_LLM_H__
