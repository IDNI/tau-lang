// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <optional>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <unordered_map>
#include <mutex>

#include <curl/curl.h>

#include "boolean_algebras/nlang/nlang_ba.h"
#include "boolean_algebras/nlang/nlang_llm.h"
#include "format/json/json.h"

namespace idni::tau_lang {

namespace json = idni::format::json;

// One-time warning when the oracle is needed but no API key is configured.
static void warn_llm_unavailable() {
	static bool warned = false;
	if (warned) return;
	warned = true;
	fprintf(stderr,
		"WARNING: nlang oracle requires an API key: the nlang-api-key option,\n"
		"  TAU_LLM_API_KEY, or the provider's own OPENAI_API_KEY /\n"
		"  ANTHROPIC_API_KEY. The provider, endpoint and model are the\n"
		"  nlang-provider, nlang-endpoint and nlang-model options.\n"
		"  Without the API key, all oracle queries return conservative defaults\n"
		"  (NOT empty, NOT universal, NOT equivalent), which may produce\n"
		"  incorrect results for nlang formulas with irreducible atoms.\n");
}

// One-time warning when the endpoint answered but not with success.
static void warn_llm_http_status(long status) {
	static bool warned = false;
	if (warned) return;
	warned = true;
	fprintf(stderr,
		"WARNING: nlang oracle endpoint returned HTTP %ld; the query is treated\n"
		"  as unanswered and a conservative default is used. If the endpoint\n"
		"  requires an explicit model, set nlang-model (or TAU_LLM_MODEL).\n",
		status);
}

// One-time warning when the request never got an HTTP answer.
static void warn_llm_transport(const char* reason) {
	static bool warned = false;
	if (warned) return;
	warned = true;
	fprintf(stderr,
		"WARNING: nlang oracle request failed (%s); the query is treated as\n"
		"  unanswered and a conservative default is used. A timeout is the\n"
		"  nlang-http-timeout option (or TAU_NLANG_HTTP_TIMEOUT).\n", reason);
}

// One-time warning when a successful reply holds no answer.
static void warn_llm_no_answer(const std::string& why) {
	static bool warned = false;
	if (warned) return;
	warned = true;
	fprintf(stderr,
		"WARNING: nlang oracle reply holds no answer (%s); the query is\n"
		"  treated as unanswered and a conservative default is used.\n",
		why.c_str());
}

static bool llm_has_key() { return llm_config_from_env().has_key(); }

// --- cURL helpers ---

// curl write callback: appends the received bytes to *out.
static size_t nlang_curl_write_cb(char* ptr, size_t size, size_t nmemb,
	std::string* out)
{
	out->append(ptr, size * nmemb);
	return size * nmemb;
}

// Process-local cache: avoids redundant API calls for the same description.
// Keys repeat heavily in the fixpoint loop; caching converts O(N) calls to O(1).
namespace {

// Oracle answers per question, guarded by mtx.
struct nlang_cache {
	std::unordered_map<std::string, bool> is_empty_cache;
	std::unordered_map<std::string, bool> is_universal_cache;
	struct pair_hash {
		size_t operator()(const std::pair<std::string,std::string>& p) const {
			auto h = std::hash<std::string>{};
			return h(p.first) ^ (h(p.second) * 2654435761u);
		}
	};
	std::unordered_map<std::pair<std::string,std::string>, bool, pair_hash>
		equivalent_cache;
	std::unordered_map<std::string, std::string> sub_cache;
	std::unordered_map<std::string, nlang_ba::fptr> decompose_cache;
	std::mutex mtx;
};

// The process-wide cache.
nlang_cache& get_cache() {
	static nlang_cache cache;
	return cache;
}

// Extract the raw JSON object starting at position 'start' (which must be '{').
static std::string extract_balanced(const std::string& json, size_t start) {
	int depth = 0;
	for (size_t i = start; i < json.size(); ++i) {
		if (json[i] == '{') ++depth;
		else if (json[i] == '}') {
			if (--depth == 0) return json.substr(start, i - start + 1);
		} else if (json[i] == '"') {
			++i;
			while (i < json.size() && json[i] != '"') {
				if (json[i] == '\\') ++i;
				++i;
			}
		}
	}
	return "";
}

// Find and extract the outermost JSON object from a string (skips leading text).
static std::string extract_outermost_json(const std::string& s) {
	auto start = s.find('{');
	if (start == std::string::npos) return "";
	return extract_balanced(s, start);
}

// The formula a decomposition object spells. A node that is not one of the
// four shapes becomes the atom of the whole statement.
static nlang_ba::fptr formula_from_json(const json::value& v,
	const std::string& fallback_text)
{
	using F = nlang_ba::formula;
	auto text = [&](const char* key) -> std::string {
		const json::value* m = v.is_object() ? v.find(key) : nullptr;
		return m && m->is_string() ? m->as_string() : "";
	};
	auto child = [&](const char* key) -> const json::value* {
		const json::value* m = v.is_object() ? v.find(key) : nullptr;
		return m && m->is_object() ? m : nullptr;
	};
	const std::string op = text("op");
	if (op == "atom") {
		std::string t = text("text");
		return t.empty() ? F::mk_atom(fallback_text)
			: F::mk_atom(std::move(t));
	}
	if (op == "not") {
		const json::value* inner = child("inner");
		if (!inner) return F::mk_atom(fallback_text);
		return F::mk_not(formula_from_json(*inner, fallback_text));
	}
	if (op == "and" || op == "or") {
		const json::value* left = child("left");
		const json::value* right = child("right");
		if (!left || !right) return F::mk_atom(fallback_text);
		auto l = formula_from_json(*left, fallback_text);
		auto r = formula_from_json(*right, fallback_text);
		return op == "and" ? F::mk_and(std::move(l), std::move(r))
		                   : F::mk_or(std::move(l), std::move(r));
	}
	return F::mk_atom(fallback_text);
}

} // namespace

void llm_clear_cache() {
	auto& cache = get_cache();
	std::lock_guard<std::mutex> lk(cache.mtx);
	cache.is_empty_cache.clear();
	cache.is_universal_cache.clear();
	cache.equivalent_cache.clear();
	cache.sub_cache.clear();
	cache.decompose_cache.clear();
}

nlang_ba::fptr llm_parse_decomposition(const std::string& response,
	const std::string& statement)
{
	using F = nlang_ba::formula;
	const std::string object = extract_outermost_json(response);
	if (object.empty()) return F::mk_atom(statement);
	auto parsed = json::parse(object);
	if (parsed.has_error() || !parsed.has_value())
		return F::mk_atom(statement);
	return formula_from_json(parsed.value(), statement);
}

std::optional<std::string> llm_query(const std::string& prompt) {
	const llm_config cfg = llm_config_from_env();
	if (!cfg.has_key()) return std::nullopt;

	CURL* curl = curl_easy_init();
	if (!curl) return std::nullopt;

	const llm_http_request req = llm_build_request(cfg, prompt);
	std::string response;

	struct curl_slist* headers = nullptr;
	for (const std::string& h : req.headers)
		headers = curl_slist_append(headers, h.c_str());

	curl_easy_setopt(curl, CURLOPT_URL, req.url.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1L);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req.body.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, nlang_curl_write_cb);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	// Runtime parameter (nlang-http-timeout); 0 = no cap.
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, nlang_http_timeout_sec());

	CURLcode res = curl_easy_perform(curl);
	long status = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	if (res != CURLE_OK) {
		warn_llm_transport(curl_easy_strerror(res));
		return std::nullopt;
	}
	if (status < 200 || status >= 300) {
		warn_llm_http_status(status);
		return std::nullopt;
	}

	std::string why;
	std::string text = llm_extract_text(cfg.provider, response, &why);
	if (text.empty()) {
		warn_llm_no_answer(why);
		return std::nullopt;
	}
	return text;
}

// The first standalone yes/no word of an oracle reply, case-insensitive
// (words split on non-letters, so "Answer: YES" is yes and "cannot" is not
// no); false when there is none.
static bool parse_yes_no(const std::string& ans) {
	std::string word;
	auto flush = [&]() -> std::optional<bool> {
		if (word == "yes") return true;
		if (word == "no") return false;
		word.clear();
		return std::nullopt;
	};
	for (char c : ans) {
		if (std::isalpha(static_cast<unsigned char>(c)))
			word.push_back(static_cast<char>(std::tolower(
				static_cast<unsigned char>(c))));
		else if (!word.empty())
			if (auto r = flush(); r) return *r;
	}
	if (auto r = flush(); r) return *r;
	return false;
}

bool llm_is_empty(const std::string& description) {
	if (description == "nothing") return true;
	if (description == "everything") return false;
	auto& cache = get_cache();
	{
		std::lock_guard<std::mutex> lk(cache.mtx);
		auto it = cache.is_empty_cache.find(description);
		if (it != cache.is_empty_cache.end()) return it->second;
	}
	if (!llm_has_key()) {
		// Not cached: the conservative default is not an oracle answer,
		// and a key configured later in the process must not find it
		// pinned for the process lifetime. The same holds below for a
		// query that got no answer (a timeout, an HTTP error, a
		// refusal).
		warn_llm_unavailable();
		return false;
	}
	std::string prompt =
		"Is the statement '"
		+ description
		+ "' a logical contradiction (always false, impossible)?"
		  " Answer with only YES or NO.";
	auto ans = llm_query(prompt);
	if (!ans) return false; // unanswered: not cached, like a missing key
	bool result = parse_yes_no(*ans);
	std::lock_guard<std::mutex> lk(cache.mtx);
	return cache.is_empty_cache.emplace(description, result).first->second;
}

bool llm_is_universal(const std::string& description) {
	if (description == "everything") return true;
	if (description == "nothing") return false;
	auto& cache = get_cache();
	{
		std::lock_guard<std::mutex> lk(cache.mtx);
		auto it = cache.is_universal_cache.find(description);
		if (it != cache.is_universal_cache.end()) return it->second;
	}
	if (!llm_has_key()) {
		warn_llm_unavailable(); // see llm_is_empty: not cached
		return false;
	}
	std::string prompt =
		"Is the statement '"
		+ description
		+ "' a tautology (always true, necessarily true in all situations)?"
		  " Answer with only YES or NO.";
	auto ans = llm_query(prompt);
	if (!ans) return false; // unanswered: not cached, like a missing key
	bool result = parse_yes_no(*ans);
	std::lock_guard<std::mutex> lk(cache.mtx);
	return cache.is_universal_cache.emplace(description, result).first->second;
}

bool llm_equivalent(const std::string& a, const std::string& b) {
	if (a == b) return true;
	if ((a == "nothing") != (b == "nothing")) return false;
	if ((a == "everything") != (b == "everything")) return false;
	auto& cache = get_cache();
	auto key_pair = a < b ? std::make_pair(a, b) : std::make_pair(b, a);
	{
		std::lock_guard<std::mutex> lk(cache.mtx);
		auto it = cache.equivalent_cache.find(key_pair);
		if (it != cache.equivalent_cache.end()) return it->second;
	}
	if (!llm_has_key()) {
		warn_llm_unavailable(); // see llm_is_empty: not cached
		return false;
	}
	std::string prompt =
		"Are the two statements '"
		+ a + "' and '" + b
		+ "' logically equivalent (true in exactly the same situations)?"
		  " Answer with only YES or NO.";
	auto ans = llm_query(prompt);
	if (!ans) return false; // unanswered: not cached, like a missing key
	bool result = parse_yes_no(*ans);
	std::lock_guard<std::mutex> lk(cache.mtx);
	return cache.equivalent_cache.emplace(key_pair, result).first->second;
}

std::string llm_stronger_statement(const std::string& description) {
	if (description == "nothing") return "nothing";
	if (description == "everything") return "it is raining";
	auto& cache = get_cache();
	{
		std::lock_guard<std::mutex> lk(cache.mtx);
		auto it = cache.sub_cache.find(description);
		if (it != cache.sub_cache.end()) return it->second;
	}
	if (!llm_has_key()) {
		warn_llm_unavailable(); // see llm_is_empty: not cached
		return description + " and specifically so";
	}
	std::string prompt =
		"Give one logically stronger statement that strictly implies '"
		+ description
		+ "' but is not equivalent to it."
		  " Reply with only the statement, no punctuation.";
	auto reply = llm_query(prompt);
	if (!reply) return description + " and specifically so"; // not cached
	std::string ans = std::move(*reply);
	if (ans.empty()) ans = description + " and specifically so";
	ans.erase(0, ans.find_first_not_of(" \t\n\r."));
	auto last = ans.find_last_not_of(" \t\n\r.");
	if (last != std::string::npos) ans = ans.substr(0, last + 1);
	if (ans.empty()) ans = description + " and specifically so";
	std::lock_guard<std::mutex> lk(cache.mtx);
	return cache.sub_cache.emplace(description, ans).first->second;
}

size_t llm_cache_size() {
	auto& cache = get_cache();
	std::lock_guard<std::mutex> lk(cache.mtx);
	return cache.is_empty_cache.size() + cache.is_universal_cache.size()
		+ cache.equivalent_cache.size() + cache.sub_cache.size()
		+ cache.decompose_cache.size();
}

nlang_ba::fptr llm_decompose(const std::string& s) {
	using F = nlang_ba::formula;
	if (s == "nothing")     return F::mk_bot();
	if (s == "everything")  return F::mk_top();

	auto& cache = get_cache();
	{
		std::lock_guard<std::mutex> lk(cache.mtx);
		auto it = cache.decompose_cache.find(s);
		if (it != cache.decompose_cache.end()) return it->second;
	}

	if (!llm_has_key()) {
		warn_llm_unavailable(); // see llm_is_empty: not cached
		return F::mk_atom(s);
	}

	std::string prompt =
		"Decompose the English statement into its minimal atomic propositions "
		"connected by logical operators. Return ONLY a JSON object, no explanation.\n"
		"Format:\n"
		"  {\"op\":\"and\",\"left\":{...},\"right\":{...}}\n"
		"  {\"op\":\"or\",\"left\":{...},\"right\":{...}}\n"
		"  {\"op\":\"not\",\"inner\":{...}}\n"
		"  {\"op\":\"atom\",\"text\":\"<irreducible proposition>\"}\n"
		"An atomic proposition is one that cannot be further decomposed "
		"by logical 'and', 'or', or 'not'.\n"
		"Statement: \"" + s + "\"";

	auto response = llm_query(prompt);
	if (!response) return F::mk_atom(s); // unanswered: not cached
	nlang_ba::fptr result = llm_parse_decomposition(*response, s);

	std::lock_guard<std::mutex> lk(cache.mtx);
	return cache.decompose_cache.emplace(s, result).first->second;
}

} // namespace idni::tau_lang
