// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"
#include "tau_pack.h"
#include "boolean_algebras/nlang/nlang_llm.h"
#include "format/json/json.h"

using namespace idni::tau_lang;
namespace json = idni::format::json;

namespace {

// The provider key variables the resolver reads, cleared for the life of one
// case and put back afterwards, so a key exported in the shell never reaches
// a case.
struct clean_llm_env {
	static constexpr const char* vars[] = {
		"OPENAI_API_KEY", "ANTHROPIC_API_KEY" };
	std::vector<std::pair<std::string, std::optional<std::string>>> saved;
	clean_llm_env() {
		for (const char* v : vars) {
			const char* old = std::getenv(v);
			saved.emplace_back(v, old ? std::optional<std::string>(old)
				: std::nullopt);
			unsetenv(v);
		}
	}
	~clean_llm_env() {
		for (const auto& [k, v] : saved)
			if (v) setenv(k.c_str(), v->c_str(), 1);
			else unsetenv(k.c_str());
	}
};

bool has_header(const llm_http_request& r, const std::string& h) {
	return std::find(r.headers.begin(), r.headers.end(), h)
		!= r.headers.end();
}

json::value parsed(const std::string& s) {
	auto r = json::parse(s);
	REQUIRE(!r.has_error());
	return r.value();
}

std::string str_member(const json::value& v, const char* key) {
	const json::value* m = v.find(key);
	REQUIRE(m);
	REQUIRE(m->is_string());
	return m->as_string();
}

llm_config anthropic_default() {
	return { llm_provider::anthropic, "sk-ant", "https://api.anthropic.com/v1",
		"claude-opus-5-5", 16000, "low", true };
}

} // namespace

TEST_SUITE("nlang llm: configuration") {

TEST_CASE("nothing given: openai, default endpoint, no model, no key") {
	clean_llm_env env;
	const llm_config c = llm_config_from_env(llm_options{});
	CHECK(c.provider == llm_provider::openai);
	CHECK(c.endpoint == "https://api.openai.com/v1");
	CHECK(c.model.empty());
	CHECK(c.api_key.empty());
	CHECK(!c.has_key());
	CHECK(c.effort.empty());
	CHECK(c.max_tokens == 16000);
	CHECK(!c.server_fallback);
}

TEST_CASE("the openai key is the option, else OPENAI_API_KEY") {
	clean_llm_env env;
	setenv("OPENAI_API_KEY", "sk-openai", 1);
	CHECK(llm_config_from_env(llm_options{}).api_key == "sk-openai");
	llm_options o;
	o.api_key = "sk-tau";
	CHECK(llm_config_from_env(o).api_key == "sk-tau");
}

TEST_CASE("an empty key variable reads as no key") {
	clean_llm_env env;
	setenv("OPENAI_API_KEY", "", 1);
	CHECK(!llm_config_from_env(llm_options{}).has_key());
}

TEST_CASE("ANTHROPIC_API_KEY as the only key selects anthropic") {
	clean_llm_env env;
	setenv("ANTHROPIC_API_KEY", "sk-ant", 1);
	const llm_config c = llm_config_from_env(llm_options{});
	CHECK(c.provider == llm_provider::anthropic);
	CHECK(c.api_key == "sk-ant");
	CHECK(c.endpoint == "https://api.anthropic.com/v1");
	CHECK(c.model == "claude-opus-5-5");
	CHECK(c.effort == "low");
	CHECK(c.max_tokens == 16000);
	CHECK(c.server_fallback);
}

TEST_CASE("a key given as an option does not undo the detection") {
	clean_llm_env env;
	setenv("ANTHROPIC_API_KEY", "sk-ant", 1);
	llm_options o;
	o.api_key = "sk-ant-other";
	const llm_config c = llm_config_from_env(o);
	CHECK(c.provider == llm_provider::anthropic);
	CHECK(c.api_key == "sk-ant-other");
}

TEST_CASE("ANTHROPIC_API_KEY beside another key leaves openai") {
	clean_llm_env env;
	setenv("ANTHROPIC_API_KEY", "sk-ant", 1);
	setenv("OPENAI_API_KEY", "sk-openai", 1);
	const llm_config c = llm_config_from_env(llm_options{});
	CHECK(c.provider == llm_provider::openai);
	CHECK(c.api_key == "sk-openai");
}

TEST_CASE("an anthropic endpoint selects anthropic") {
	clean_llm_env env;
	llm_options o;
	o.endpoint = "https://api.anthropic.com/v1";
	o.api_key = "sk-tau";
	const llm_config c = llm_config_from_env(o);
	CHECK(c.provider == llm_provider::anthropic);
	CHECK(c.api_key == "sk-tau");
}

TEST_CASE("a provider option wins over detection") {
	clean_llm_env env;
	setenv("ANTHROPIC_API_KEY", "sk-ant", 1);
	llm_options o;
	o.provider = "openai";
	const llm_config c = llm_config_from_env(o);
	CHECK(c.provider == llm_provider::openai);
	// the key of the other provider is not this provider's key
	CHECK(!c.has_key());
}

TEST_CASE("a provider word that names no provider leaves detection") {
	clean_llm_env env;
	setenv("ANTHROPIC_API_KEY", "sk-ant", 1);
	llm_options o;
	o.provider = "nobody";
	CHECK(llm_config_from_env(o).provider == llm_provider::anthropic);
}

TEST_CASE("a model named by the user gets no effort") {
	clean_llm_env env;
	setenv("ANTHROPIC_API_KEY", "sk-ant", 1);
	llm_options o;
	o.model = "claude-haiku-4-5";
	const llm_config c = llm_config_from_env(o);
	CHECK(c.model == "claude-haiku-4-5");
	CHECK(c.effort.empty());
	CHECK(c.server_fallback);
}

TEST_CASE("an effort applies to a named model") {
	clean_llm_env env;
	setenv("ANTHROPIC_API_KEY", "sk-ant", 1);
	llm_options o;
	o.model = "claude-sonnet-5-5";
	o.effort = "high";
	const llm_config c = llm_config_from_env(o);
	CHECK(c.effort == "high");
	CHECK(c.server_fallback);
}

TEST_CASE("the fallback option off switches the fallback off") {
	clean_llm_env env;
	setenv("ANTHROPIC_API_KEY", "sk-ant", 1);
	llm_options o;
	o.fallback = false;
	CHECK(!llm_config_from_env(o).server_fallback);
}

TEST_CASE("another endpoint keeps the fallback") {
	clean_llm_env env;
	llm_options o;
	o.provider = "anthropic";
	o.endpoint = "https://proxy.example/v1";
	const llm_config c = llm_config_from_env(o);
	CHECK(c.provider == llm_provider::anthropic);
	CHECK(c.effort == "low");
	CHECK(c.server_fallback);
}

TEST_CASE("openai gets no fallback") {
	clean_llm_env env;
	llm_options o;
	o.provider = "openai";
	CHECK(!llm_config_from_env(o).server_fallback);
}

TEST_CASE("the max tokens option is read, and zero keeps the default") {
	clean_llm_env env;
	llm_options o;
	o.max_tokens = 2048;
	CHECK(llm_config_from_env(o).max_tokens == 2048);
	o.max_tokens = 0;
	CHECK(llm_config_from_env(o).max_tokens == 16000);
}

TEST_CASE("a trailing slash of the endpoint is dropped") {
	clean_llm_env env;
	llm_options o;
	o.endpoint = "https://api.openai.com/v1/";
	CHECK(llm_config_from_env(o).endpoint == "https://api.openai.com/v1");
}

TEST_CASE("provider and effort words") {
	CHECK(llm_provider_from_name("openai") == llm_provider::openai);
	CHECK(llm_provider_from_name("anthropic") == llm_provider::anthropic);
	CHECK(!llm_provider_from_name("Anthropic "));
	CHECK(!llm_provider_from_name(""));
	CHECK(std::string(llm_provider_name(llm_provider::anthropic))
		== "anthropic");
	for (const char* e : { "low", "medium", "high", "xhigh", "max" })
		CHECK(llm_effort_is_valid(e));
	CHECK(!llm_effort_is_valid("extreme"));
	CHECK(!llm_effort_is_valid(""));
}

}

TEST_SUITE("nlang llm: request") {

TEST_CASE("openai request: url, headers and body") {
	const llm_config c{ llm_provider::openai, "sk-openai",
		"https://api.openai.com/v1", "gpt-x", 16000, "", false };
	const llm_http_request r = llm_build_request(c, "hello");
	CHECK(r.url == "https://api.openai.com/v1/chat/completions");
	CHECK(r.headers.size() == 2);
	CHECK(has_header(r, "Content-Type: application/json"));
	CHECK(has_header(r, "Authorization: Bearer sk-openai"));
	const json::value b = parsed(r.body);
	CHECK(str_member(b, "model") == "gpt-x");
	const json::value* t = b.find("temperature");
	REQUIRE(t);
	CHECK(static_cast<size_t>(t->as_number()) == 0);
	CHECK(!b.find("max_tokens"));
	const json::value* m = b.find("messages");
	REQUIRE(m);
	REQUIRE(m->is_array());
	REQUIRE(m->size() == 1);
	CHECK(str_member((*m)[0], "role") == "user");
	CHECK(str_member((*m)[0], "content") == "hello");
}

TEST_CASE("openai request names no model when none is configured") {
	const llm_config c{ llm_provider::openai, "k",
		"https://api.openai.com/v1", "", 16000, "", false };
	CHECK(!parsed(llm_build_request(c, "p").body).find("model"));
}

TEST_CASE("anthropic request: url, headers and body") {
	const llm_http_request r = llm_build_request(anthropic_default(), "hello");
	CHECK(r.url == "https://api.anthropic.com/v1/messages");
	CHECK(r.headers.size() == 4);
	CHECK(has_header(r, "Content-Type: application/json"));
	CHECK(has_header(r, "x-api-key: sk-ant"));
	CHECK(has_header(r, "anthropic-version: 2023-06-01"));
	CHECK(has_header(r, "anthropic-beta: server-side-fallback-2026-07-01"));
	const json::value b = parsed(r.body);
	CHECK(str_member(b, "model") == "claude-opus-5-5");
	CHECK(str_member(b, "fallbacks") == "default");
	const json::value* mt = b.find("max_tokens");
	REQUIRE(mt);
	CHECK(static_cast<size_t>(mt->as_number()) == 16000);
	const json::value* oc = b.find("output_config");
	REQUIRE(oc);
	CHECK(str_member(*oc, "effort") == "low");
	// current models reject both
	CHECK(!b.find("temperature"));
	CHECK(!b.find("thinking"));
	const json::value* m = b.find("messages");
	REQUIRE(m);
	REQUIRE(m->size() == 1);
	CHECK(str_member((*m)[0], "role") == "user");
	CHECK(str_member((*m)[0], "content") == "hello");
}

TEST_CASE("max_tokens is spelled as an integer whatever its size") {
	for (size_t n : { size_t(1), size_t(16000), size_t(100000),
		size_t(200000), size_t(1000000), size_t(128000) })
	{
		llm_config c = anthropic_default();
		c.max_tokens = n;
		const std::string body = llm_build_request(c, "p").body;
		CAPTURE(body);
		CHECK(body.find("\"max_tokens\":" + std::to_string(n) + ",")
			!= std::string::npos);
	}
}

TEST_CASE("anthropic request without effort and fallback sends neither") {
	llm_config c = anthropic_default();
	c.model = "claude-haiku-4-5";
	c.effort.clear();
	c.server_fallback = false;
	const llm_http_request r = llm_build_request(c, "p");
	CHECK(r.headers.size() == 3);
	const json::value b = parsed(r.body);
	CHECK(!b.find("output_config"));
	CHECK(!b.find("fallbacks"));
}

TEST_CASE("the prompt survives the body: quotes, backslash, controls, utf-8") {
	const std::string prompt =
		std::string("a \"q\" \\ back\nline\ttab\r") + '\x01' + '\x1f'
		+ " caf\xc3\xa9";
	for (const llm_config& c : { anthropic_default(), llm_config{
		llm_provider::openai, "k", "https://api.openai.com/v1", "", 1,
		"", false } })
	{
		const std::string body = llm_build_request(c, prompt).body;
		// no raw control character may reach the wire
		for (char ch : body)
			CHECK(static_cast<unsigned char>(ch) >= 0x20);
		const json::value b = parsed(body);
		CHECK(str_member((*b.find("messages"))[0], "content") == prompt);
	}
}

}

TEST_SUITE("nlang llm: reply") {

TEST_CASE("openai reply: choices[0].message.content") {
	const std::string r = R"({"id":"x","choices":[{"index":0,"message":)"
		R"({"role":"assistant","content":"YES"},"finish_reason":"stop"}]})";
	CHECK(llm_extract_text(llm_provider::openai, r) == "YES");
}

TEST_CASE("openai reply: reasoning_content after content is not the answer") {
	const std::string r = R"({"choices":[{"message":{"content":"NO",)"
		R"("reasoning_content":"the \"content\": \"YES\" trap"}}]})";
	CHECK(llm_extract_text(llm_provider::openai, r) == "NO");
}

TEST_CASE("openai reply: escapes are decoded") {
	const std::string r = R"({"choices":[{"message":{"content":)"
		R"("a \"b\"\nc é \\"}}]})";
	CHECK(llm_extract_text(llm_provider::openai, r)
		== "a \"b\"\nc \xc3\xa9 \\");
}

TEST_CASE("openai reply: an error object is no answer") {
	std::string why;
	const std::string r = R"({"error":{"message":"bad key","type":"x"}})";
	CHECK(llm_extract_text(llm_provider::openai, r, &why).empty());
	CHECK(why.find("bad key") != std::string::npos);
}

TEST_CASE("openai reply: a null error member is not an error") {
	const std::string r = R"({"error":null,"choices":[{"message":)"
		R"({"content":"YES"}}]})";
	CHECK(llm_extract_text(llm_provider::openai, r) == "YES");
}

TEST_CASE("openai reply: a null content is no answer") {
	std::string why;
	const std::string r = R"({"choices":[{"message":{"content":null}}]})";
	CHECK(llm_extract_text(llm_provider::openai, r, &why).empty());
	CHECK(!why.empty());
}

TEST_CASE("anthropic reply: text after a thinking block") {
	const std::string r = R"({"type":"message","role":"assistant","content":[)"
		R"({"type":"thinking","thinking":"","signature":"s"},)"
		R"({"type":"text","text":"YES"}],"stop_reason":"end_turn"})";
	CHECK(llm_extract_text(llm_provider::anthropic, r) == "YES");
}

TEST_CASE("anthropic reply: text blocks are concatenated") {
	const std::string r = R"({"content":[{"type":"text","text":"a \"b\""},)"
		R"({"type":"text","text":"\nc"}],"stop_reason":"end_turn"})";
	CHECK(llm_extract_text(llm_provider::anthropic, r) == "a \"b\"\nc");
}

TEST_CASE("anthropic reply: a refusal is no answer") {
	std::string why;
	const std::string r = R"({"content":[{"type":"text","text":"I cannot"}],)"
		R"("stop_reason":"refusal"})";
	CHECK(llm_extract_text(llm_provider::anthropic, r, &why).empty());
	CHECK(why.find("refusal") != std::string::npos);
}

TEST_CASE("anthropic reply: a reply cut at max_tokens is no answer") {
	std::string why;
	const std::string r = R"({"content":[{"type":"text","text":"YE"}],)"
		R"("stop_reason":"max_tokens"})";
	CHECK(llm_extract_text(llm_provider::anthropic, r, &why).empty());
	CHECK(why.find("max_tokens") != std::string::npos);
}

TEST_CASE("anthropic reply: an error object is no answer") {
	std::string why;
	const std::string r = R"({"type":"error","error":{"type":)"
		R"("invalid_request_error","message":"model: not found"}})";
	CHECK(llm_extract_text(llm_provider::anthropic, r, &why).empty());
	CHECK(why.find("model: not found") != std::string::npos);
}

TEST_CASE("anthropic reply: no text block is no answer") {
	std::string why;
	const std::string r = R"({"content":[{"type":"thinking","thinking":""}],)"
		R"("stop_reason":"end_turn"})";
	CHECK(llm_extract_text(llm_provider::anthropic, r, &why).empty());
	CHECK(!why.empty());
}

TEST_CASE("garbage is no answer for either provider") {
	for (llm_provider p : { llm_provider::openai, llm_provider::anthropic })
		for (const char* r : { "", "<html>502</html>", "{\"content\":",
			"[]", "42", "{}", "{\"choices\":[]}", "{\"content\":{}}" })
		{
			std::string why;
			CHECK(llm_extract_text(p, r, &why).empty());
			CHECK(!why.empty());
		}
}

}

TEST_SUITE("nlang llm: decomposition") {

TEST_CASE("a nested object becomes the formula it spells") {
	const std::string r = R"({"op":"and","left":{"op":"atom","text":"a \"b\""},)"
		R"("right":{"op":"not","inner":{"op":"or","left":)"
		R"({"op":"atom","text":"c"},"right":{"op":"atom","text":"d"}}}})";
	auto f = llm_parse_decomposition(r, "whole");
	REQUIRE(f);
	CHECK(f->to_string() == "(a \"b\") and (not ((c) or (d)))");
}

TEST_CASE("prose and a code fence around the object are skipped") {
	auto f = llm_parse_decomposition(
		"Here it is:\n```json\n{ \"op\" : \"atom\", \"text\" : \"x\" }\n```",
		"whole");
	CHECK(f->to_string() == "x");
}

TEST_CASE("a text holding the key of another field is not that field") {
	auto f = llm_parse_decomposition(
		R"({"text":"the \"op\":\"not\" trap","op":"atom"})", "whole");
	CHECK(f->to_string() == "the \"op\":\"not\" trap");
}

TEST_CASE("a reply that spells no formula is the atom of the statement") {
	for (const char* r : { "", "no json here", "{\"op\":\"and\"}",
		"{\"op\":\"xor\"}", "{\"op\":\"atom\"}", "{\"op\":\"not\",",
		"{\"op\":\"not\",\"inner\":\"x\"}" })
		CHECK(llm_parse_decomposition(r, "whole")->to_string()
			== "whole");
}

TEST_CASE("a malformed child is the atom of the statement, the rest stays") {
	auto f = llm_parse_decomposition(
		R"({"op":"or","left":{"op":"atom","text":"a"},"right":{"op":"?"}})",
		"whole");
	CHECK(f->to_string() == "(a) or (whole)");
}

}

TEST_SUITE("nlang llm: oracle cache") {

// Nothing listens on port 1, so every query fails at once and gets no answer.
TEST_CASE("a query that gets no answer is not cached") {
	clean_llm_env env;
	const llm_options saved = nlang_llm_options();
	const size_t saved_timeout = nlang_http_timeout_sec;
	nlang_llm_options() = {};
	nlang_llm_options().provider = "openai";
	nlang_llm_options().endpoint = "http://127.0.0.1:1/v1";
	nlang_llm_options().api_key = "sk-test";
	nlang_http_timeout_sec = 5;
	llm_clear_cache();
	REQUIRE(llm_cache_size() == 0);

	CHECK_FALSE(llm_is_empty("it rains"));
	CHECK_FALSE(llm_is_universal("it rains"));
	CHECK_FALSE(llm_equivalent("it rains", "it pours"));
	CHECK(llm_stronger_statement("it rains")
		== "it rains and specifically so");
	CHECK(llm_decompose("it rains and it pours")->to_string()
		== "it rains and it pours");
	CHECK(llm_cache_size() == 0);

	nlang_http_timeout_sec = saved_timeout;
	nlang_llm_options() = saved;
}

}
