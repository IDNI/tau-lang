// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "tau_pack.h"
#include "boolean_algebras/nlang/nlang_ba.h"

using namespace idni::tau_lang;

namespace {

// Sets an environment variable for the scope and restores its old value.
struct scoped_env {
	scoped_env(const char* name, const char* value) : name(name) {
		if (const char* old = std::getenv(name)) saved = old;
		setenv(name, value, 1);
	}
	~scoped_env() {
		if (saved) setenv(name, saved->c_str(), 1);
		else unsetenv(name);
	}
	const char* name;
	std::optional<std::string> saved;
};

// Points the oracle at a closed local port, so every request fails at once
// with no network access.
struct failing_endpoint {
	scoped_env no_proxy_upper{ "NO_PROXY", "*" };
	scoped_env no_proxy_lower{ "no_proxy", "*" };
	llm_options saved = nlang_llm_options();
	size_t saved_timeout = nlang_http_timeout_sec;
	failing_endpoint() {
		nlang_llm_options().api_key = "test-key";
		nlang_llm_options().endpoint = "http://127.0.0.1:9";
		nlang_http_timeout_sec = 2;
	}
	~failing_endpoint() {
		nlang_llm_options() = saved;
		nlang_http_timeout_sec = saved_timeout;
	}
};

} // namespace

TEST_SUITE("nlang oracle") {

TEST_CASE("a failed request is not cached") {
	failing_endpoint ep;
	CHECK( !llm_query("ping").has_value() );
	const size_t before = llm_cache_size();
	CHECK( !llm_is_empty("the sky is green") );
	CHECK( !llm_is_universal("the sky is green") );
	CHECK( !llm_equivalent("the sky is green", "grass is blue") );
	CHECK( llm_stronger_statement("the sky is green")
		== "the sky is green and specifically so" );
	CHECK( llm_decompose("the sky is green")->atom_str
		== "the sky is green" );
	CHECK( llm_cache_size() == before );
}

}
