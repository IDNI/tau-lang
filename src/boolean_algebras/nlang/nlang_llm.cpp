// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <cstdlib>
#include <sstream>

#include "boolean_algebras/nlang/nlang_llm.h"
#include "format/json/json.h"
#include "read_env.h"

namespace idni::tau_lang {

namespace json = idni::format::json;

std::optional<llm_provider> llm_provider_from_name(std::string_view name) {
	if (name == "openai") return llm_provider::openai;
	if (name == "anthropic") return llm_provider::anthropic;
	return std::nullopt;
}

const char* llm_provider_name(llm_provider p) {
	return p == llm_provider::anthropic ? "anthropic" : "openai";
}

bool llm_effort_is_valid(std::string_view effort) {
	for (const char* e : { "low", "medium", "high", "xhigh", "max" })
		if (effort == e) return true;
	return false;
}

llm_options& nlang_llm_options() {
	static llm_options opts;
	return opts;
}

namespace {

// Empty when unset or empty, so a blank variable reads as "not given".
std::string env_text(const char* var) {
	auto v = read_env(var);
	// TODO (HIGH) dropped error: a failed read reads as unset --
	// llm_config_from_env returns a plain llm_config, with no report.
	if (!v.has_value() || !v.value()) return "";
	return *v.value();
}

std::string quoted(std::string_view text) {
	std::ostringstream os;
	json::escape(os, text);
	return os.str();
}

// A JSON object written member by member. A count is written as an integer:
// json::value holds a double, which prints 100000 as 1e+05.
struct json_object {
	std::string text = "{";
	json_object& raw(std::string_view key, const std::string& value) {
		if (text.size() > 1) text += ',';
		text += quoted(key) + ':' + value;
		return *this;
	}
	json_object& string(std::string_view key, std::string_view value) {
		return raw(key, quoted(value));
	}
	json_object& count(std::string_view key, size_t value) {
		return raw(key, std::to_string(value));
	}
	std::string close() const { return text + '}'; }
};

std::string user_message(const std::string& prompt) {
	return '[' + json_object().string("role", "user")
		.string("content", prompt).close() + ']';
}

const json::value* member(const json::value& v, std::string_view key) {
	return v.is_object() ? v.find(key) : nullptr;
}

const std::string* string_member(const json::value& v, std::string_view key) {
	const json::value* m = member(v, key);
	return m && m->is_string() ? &m->as_string() : nullptr;
}

std::string no_answer(std::string* why, std::string reason) {
	if (why) *why = std::move(reason);
	return "";
}

// Both providers report a failure as {"error":{"message":..}}; anthropic
// also marks the object with "type":"error".
std::optional<std::string> error_of(const json::value& reply) {
	const json::value* err = member(reply, "error");
	if (err && err->is_null()) err = nullptr;
	const std::string* type = string_member(reply, "type");
	if (!err && !(type && *type == "error")) return std::nullopt;
	if (err) {
		if (err->is_string()) return err->as_string();
		if (const std::string* m = string_member(*err, "message"))
			return *m;
	}
	return "the endpoint reported an error";
}

std::string extract_openai(const json::value& reply, std::string* why) {
	const json::value* choices = member(reply, "choices");
	if (!choices || !choices->is_array() || choices->size() == 0)
		return no_answer(why, "the reply has no choices");
	const json::value* message = member((*choices)[0], "message");
	const std::string* content = message
		? string_member(*message, "content") : nullptr;
	if (!content || content->empty())
		return no_answer(why, "the reply has no message content");
	return *content;
}

std::string extract_anthropic(const json::value& reply, std::string* why) {
	if (const std::string* stop = string_member(reply, "stop_reason");
		stop && (*stop == "refusal" || *stop == "max_tokens"))
			return no_answer(why, "the reply stopped with " + *stop);
	const json::value* content = member(reply, "content");
	if (!content || !content->is_array())
		return no_answer(why, "the reply has no content blocks");
	std::string text;
	for (const json::value& block : *content) {
		const std::string* type = string_member(block, "type");
		const std::string* t = string_member(block, "text");
		if (type && *type == "text" && t) text += *t;
	}
	if (text.empty()) return no_answer(why, "the reply has no text block");
	return text;
}

} // namespace

llm_config llm_config_from_env(const llm_options& opts) {
	llm_config c;
	c.endpoint = opts.endpoint;
	while (!c.endpoint.empty() && c.endpoint.back() == '/')
		c.endpoint.pop_back();

	auto provider = llm_provider_from_name(opts.provider);
	if (!provider) {
		const bool anthropic_host =
			c.endpoint.find("anthropic") != std::string::npos;
		const bool only_anthropic_key =
			env_text("OPENAI_API_KEY").empty()
			&& !env_text("ANTHROPIC_API_KEY").empty();
		provider = anthropic_host || (c.endpoint.empty()
				&& only_anthropic_key)
			? llm_provider::anthropic : llm_provider::openai;
	}
	c.provider = *provider;
	const bool anthropic = c.provider == llm_provider::anthropic;

	c.api_key = !opts.api_key.empty() ? opts.api_key
		: env_text(anthropic ? "ANTHROPIC_API_KEY" : "OPENAI_API_KEY");

	if (c.endpoint.empty()) c.endpoint = anthropic
		? llm_default_anthropic_endpoint : llm_default_openai_endpoint;

	c.model = opts.model;
	if (c.model.empty() && anthropic) c.model = llm_default_anthropic_model;
	const bool default_model = anthropic
		&& c.model == llm_default_anthropic_model;

	c.effort = opts.effort;
	if (!llm_effort_is_valid(c.effort))
		c.effort = default_model ? "low" : "";

	c.max_tokens = opts.max_tokens ? opts.max_tokens
		: llm_default_max_tokens;
	c.server_fallback = anthropic && opts.fallback;
	return c;
}

llm_http_request llm_build_request(const llm_config& cfg,
	const std::string& prompt)
{
	llm_http_request r;
	r.headers.push_back("Content-Type: application/json");
	json_object body;
	if (cfg.provider == llm_provider::anthropic) {
		r.url = cfg.endpoint + "/messages";
		r.headers.push_back("x-api-key: " + cfg.api_key);
		r.headers.push_back("anthropic-version: 2023-06-01");
		body.string("model", cfg.model);
		body.count("max_tokens", cfg.max_tokens);
		if (!cfg.effort.empty())
			body.raw("output_config", json_object()
				.string("effort", cfg.effort).close());
		if (cfg.server_fallback) {
			r.headers.push_back("anthropic-beta: "
				"server-side-fallback-2026-07-01");
			body.string("fallbacks", "default");
		}
		// No temperature and no thinking field: the current models
		// reject a temperature and think on their own.
		body.raw("messages", user_message(prompt));
	} else {
		r.url = cfg.endpoint + "/chat/completions";
		r.headers.push_back("Authorization: Bearer " + cfg.api_key);
		if (!cfg.model.empty()) body.string("model", cfg.model);
		body.raw("messages", user_message(prompt));
		body.count("temperature", 0);
	}
	r.body = body.close();
	return r;
}

std::string llm_extract_text(llm_provider provider,
	const std::string& response, std::string* why)
{
	auto parsed = json::parse(response);
	if (parsed.has_error() || !parsed.has_value())
		return no_answer(why, "the reply is not JSON");
	const json::value& reply = parsed.value();
	if (!reply.is_object())
		return no_answer(why, "the reply is not a JSON object");
	if (auto err = error_of(reply)) return no_answer(why, *err);
	return provider == llm_provider::anthropic
		? extract_anthropic(reply, why) : extract_openai(reply, why);
}

} // namespace idni::tau_lang
