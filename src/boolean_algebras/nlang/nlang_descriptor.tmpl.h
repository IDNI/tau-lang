// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file nlang_descriptor.tmpl.h
 * @brief Descriptor through which core reaches the nlang Boolean algebra.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__NLANG__NLANG_DESCRIPTOR_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__NLANG__NLANG_DESCRIPTOR_TMPL_H__

#include "boolean_algebras/nlang/parser/nlang_parser.generated.h"
#include "boolean_algebras/ba_descriptor.h"
#include <array>
#include "ba_types.h"

namespace idni::tau_lang {

/**
 * @brief Descriptor of the nlang Boolean algebra in the pack @p PackBAs.
 * @tparam PackBAs The BAs of the configured pack.
 */
template <typename... PackBAs>
struct ba_descriptor<nlang_ba, node<PackBAs...>> {
	/// The node type of the pack.
	using node_t = node<PackBAs...>;
	/// The tree type of the pack.
	using tau = tree<node_t>;

	/// Type name used in annotations (`:nlang`).
	static constexpr const char* type_name = "nlang";
	/// Default-type priority; lower wins.
	static constexpr int default_type_priority = 50;

	/** @brief The 64-bit content hash: std::hash<nlang_ba> keeps 32 bits on wasm32. */
	static std::uint64_t hash_constant(const nlang_ba& x) { return nlang_hash(x); }
	/// nlang is atomless.
	static constexpr bool atomless = true;
	/// nlang is a Boolean algebra, not a non-aba omcat.
	static constexpr bool non_aba_omcat = false;
	// answers come from an LLM oracle, so comparing two constants can leave
	// the process and need not be reproducible
	static constexpr bool uses_oracle = true;

	/**
	 * @brief No past output can leave the current constraint unsatisfiable.
	 *
	 * There is no operation here by which history exhausts the admissible
	 * outputs: `o = p` is met by emitting `p` whatever came before, and being
	 * atomless, even a chain such as `o[t] < o[t-1]` can always be continued.
	 * So satisfiable at one step means satisfiable at every step, and core may
	 * answer a formula no input constrains by feasibility alone instead of the
	 * safety fixpoint. A bounded or finite algebra must not declare this --
	 * `bv[8]` runs out of room at `o[t] = o[t-1] + 1`.
	 */
	static constexpr bool output_always_satisfiable_by_system = true;

	/// @brief `true` if @p type_tree names the nlang type.
	static bool matches_type(tref type_tree) {
		return ba_types_detail::type_tree_name_is<nlang_ba, node_t>(
			type_tree, type_name);
	}

	/// @name nlang-declared CLI/REPL options
	/// A getter reads the value in force (option, else environment, else
	/// default). A text setter given an empty text clears the option.
	/// @{
	/// @brief Current HTTP timeout of the oracle, in seconds (0 = no cap).
	static size_t get_http_timeout_option() {
		return (size_t) nlang_http_timeout_sec();
	}
	/// @brief Sets the HTTP timeout of the oracle to @p n seconds (0 = no cap).
	static void set_http_timeout_option(size_t n) {
		nlang_http_timeout_sec_param = (long) n;
	}
	static size_t get_max_tokens_option() {
		return llm_config_from_env().max_tokens;
	}
	static void set_max_tokens_option(size_t n) {
		nlang_llm_options().max_tokens = n;
	}
	static bool get_fallback_option() {
		return llm_config_from_env().server_fallback;
	}
	static void set_fallback_option(bool b) {
		nlang_llm_options().fallback = b;
	}
	// An answer of one model is not another's, so a setter that can
	// change who answers drops the cached answers.
	static bool set_text_option(std::optional<std::string>& slot,
		const std::string& v, bool changes_answers = true)
	{
		if (v.empty()) slot.reset(); else slot = v;
		if (changes_answers) llm_clear_cache();
		return true;
	}
	static std::string get_provider_option() {
		return llm_provider_name(llm_config_from_env().provider);
	}
	static bool set_provider_option(const std::string& v) {
		if (!v.empty() && !llm_provider_from_name(v)) return false;
		return set_text_option(nlang_llm_options().provider, v);
	}
	static std::string get_endpoint_option() {
		return llm_config_from_env().endpoint;
	}
	static bool set_endpoint_option(const std::string& v) {
		return set_text_option(nlang_llm_options().endpoint, v);
	}
	static std::string get_model_option() {
		return llm_config_from_env().model;
	}
	static bool set_model_option(const std::string& v) {
		return set_text_option(nlang_llm_options().model, v);
	}
	/// Never the key: only whether one is configured.
	static std::string get_api_key_option() {
		return llm_config_from_env().has_key() ? "set" : "unset";
	}
	static bool set_api_key_option(const std::string& v) {
		return set_text_option(nlang_llm_options().api_key, v, false);
	}
	static std::string get_effort_option() {
		return llm_config_from_env().effort;
	}
	static bool set_effort_option(const std::string& v) {
		if (!v.empty() && !llm_effort_is_valid(v)) return false;
		return set_text_option(nlang_llm_options().effort, v);
	}
	/// @}

	/**
	 * @brief The options nlang declares about itself, all of the LLM
	 * oracle. Each is read as option, else environment, else default.
	 *
	 * | option               | environment              | default |
	 * |----------------------|--------------------------|---------|
	 * | `nlang-provider`     | `TAU_LLM_PROVIDER`       | detected, else `openai` |
	 * | `nlang-endpoint`     | `TAU_LLM_ENDPOINT`       | the provider's |
	 * | `nlang-model`        | `TAU_LLM_MODEL`          | none (openai), `claude-opus-5-5` (anthropic) |
	 * | `nlang-api-key`      | `TAU_LLM_API_KEY`, then `OPENAI_API_KEY` / `ANTHROPIC_API_KEY` | none |
	 * | `nlang-effort`       | `TAU_LLM_EFFORT`         | `low` for the default anthropic model |
	 * | `nlang-max-tokens`   | `TAU_LLM_MAX_TOKENS`     | 16000 |
	 * | `nlang-http-timeout` | `TAU_NLANG_HTTP_TIMEOUT` | 15 (0 = no cap) |
	 * | `nlang-fallback`     | `TAU_LLM_FALLBACK`       | on for the default anthropic model on the default endpoint |
	 *
	 * `nlang-api-key` reads back as `set` or `unset`, never as the key.
	 */
	static std::array<ba_option, 8> options() {
		return {{
			{ "http-timeout", ba_option_kind::count,
				nullptr, nullptr,
				get_http_timeout_option, set_http_timeout_option,
				"cap each LLM oracle HTTP request at this many "
				"seconds (default: TAU_NLANG_HTTP_TIMEOUT or 15; "
				"0 = no cap)" },
			{ "max-tokens", ba_option_kind::count,
				nullptr, nullptr,
				get_max_tokens_option, set_max_tokens_option,
				"cap each LLM oracle reply at this many tokens, "
				"anthropic only (default: TAU_LLM_MAX_TOKENS or "
				"16000; 0 = the default)" },
			{ "fallback", ba_option_kind::flag,
				get_fallback_option, set_fallback_option,
				nullptr, nullptr,
				"let the anthropic API answer from another model "
				"when the named one declines (default: "
				"TAU_LLM_FALLBACK, else on for the default model "
				"on the default endpoint)" },
			{ "provider", ba_option_kind::text,
				nullptr, nullptr, nullptr, nullptr,
				"LLM oracle API, openai or anthropic (default: "
				"TAU_LLM_PROVIDER, else detected from the "
				"endpoint and the keys, else openai)",
				get_provider_option, set_provider_option },
			{ "endpoint", ba_option_kind::text,
				nullptr, nullptr, nullptr, nullptr,
				"LLM oracle API base URL (default: "
				"TAU_LLM_ENDPOINT, else the provider's)",
				get_endpoint_option, set_endpoint_option },
			{ "model", ba_option_kind::text,
				nullptr, nullptr, nullptr, nullptr,
				"LLM oracle model (default: TAU_LLM_MODEL, else "
				"none for openai and claude-opus-5-5 for "
				"anthropic)",
				get_model_option, set_model_option },
			{ "api-key", ba_option_kind::text,
				nullptr, nullptr, nullptr, nullptr,
				"LLM oracle API key, read back as set or unset "
				"(default: TAU_LLM_API_KEY, else OPENAI_API_KEY "
				"or ANTHROPIC_API_KEY by provider)",
				get_api_key_option, set_api_key_option },
			{ "effort", ba_option_kind::text,
				nullptr, nullptr, nullptr, nullptr,
				"anthropic effort: low, medium, high, xhigh or "
				"max (default: TAU_LLM_EFFORT, else low for the "
				"default model)",
				get_effort_option, set_effort_option },
		}};
	}

	/// @brief The nlang type tree.
	static tref type_tree() {
		return ba_types_detail::make_syntactic_type_tree<node_t>(
			type_name);
	}

	/// @brief `true` if @p ba_type_id is the nlang type id.
	static bool owns_type(size_t ba_type_id) {
		return ba_types_detail::type_tree_name_is<nlang_ba, node_t>(
			ba_type_id, type_name);
	}


	/// @brief `true` if @p x is the one constant (`is_nlang_one`).
	static bool is_syntactic_one(const nlang_ba& x) { return is_nlang_one(x); }

	/// @brief `true` if @p x is the zero constant (`is_nlang_zero`).
	static bool is_syntactic_zero(const nlang_ba& x) { return is_nlang_zero(x); }

	/// @brief Same test as `is_syntactic_one`; never an error.
	static result<bool> is_one(const nlang_ba& x) { return result<bool>{is_nlang_one(x)}; }

	/// @brief Same test as `is_syntactic_zero`; never an error.
	static result<bool> is_zero(const nlang_ba& x) { return result<bool>{is_nlang_zero(x)}; }

	/// @brief Every nlang constant is closed.
	static result<bool> is_closed(const nlang_ba&) { return result<bool>{true}; }

	/// @brief Source text of the one constant, "everything".
	static std::string literal_one(tref) { return "everything"; }

	/// @brief Source text of the zero constant, "nothing".
	static std::string literal_zero(tref) { return "nothing"; }

	/// @brief @p x normalized by `normalize_nlang`; never an error.
	static result<nlang_ba> normalize(const nlang_ba& x) {
		return result<nlang_ba>{normalize_nlang(x)};
	}

	/// @brief A splitter of @p x (`nlang_splitter`); never an error.
	static result<nlang_ba> splitter(const nlang_ba& x, splitter_type st) {
		return result<nlang_ba>{nlang_splitter(x, st)};
	}

	/// @brief The splitter of one, as a bf constant tree of the nlang type.
	static tref splitter_one(tref) {
		return tau::get(tau::bf, tau::get_ba_constant(
			typename tau::constant(nlang_splitter_one()),
			type_tree()));
	}

	/// @brief Returns @p sym: nlang has no symbol simplification.
	static tref simplify_symbol(tref sym) {
		return simplify_nlang_symbol(sym);
	}

	/// @brief Returns @p term: nlang has no term simplification.
	static result<tref> simplify_term(tref term) { return result<tref>{simplify_nlang_term(term)}; }

	/**
	 * @brief Parses @p src as an nlang constant; the type tree is ignored.
	 * @return The constant with the nlang type, or the parse error.
	 */
	static result<typename node_t::constant_with_type>
	parse(const std::string& src, tref)
	{
		return parse_nlang<PackBAs...>(src);
	}
	/**
	 * @brief `true` when @p src is a truncated nlang literal, not a bad one.
	 *
	 * Distinct from `parse` failing, which cannot tell the two apart; the REPL
	 * keeps reading on truncation and stops on a genuine syntax error.
	 */
	static bool literal_incomplete(const std::string& src) {
		auto result = nlang_parser::instance()
			.parse(src.c_str(), src.size());
		return !result.found && result.parse_error.at_eof();
	}
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__NLANG__NLANG_DESCRIPTOR_TMPL_H__
