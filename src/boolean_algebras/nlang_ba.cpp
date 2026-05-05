// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <string>
#include <cstdlib>
#include <unordered_map>
#include <mutex>

#include <curl/curl.h>

#include "boolean_algebras/nlang_ba.h"

namespace idni::tau_lang {

static size_t nlang_curl_write_cb(char* ptr, size_t size, size_t nmemb,
	std::string* out)
{
	out->append(ptr, size * nmemb);
	return size * nmemb;
}

// Process-local cache: avoids redundant API calls for the same description.
// Keys repeat heavily in the fixpoint loop; caching converts O(N) calls to O(1).
namespace {

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

nlang_cache& get_cache() {
	static nlang_cache cache;
	return cache;
}

// --- Minimal JSON helpers for decompose response parsing ---

// Extract the value of a JSON string field "key":"value" from a JSON object string.
static std::string extract_str_field(const std::string& json, const std::string& key) {
	for (const auto& search : {
		"\"" + key + "\":\"",
		"\"" + key + "\": \""
	}) {
		auto pos = json.find(search);
		if (pos == std::string::npos) continue;
		pos += search.size();
		std::string val;
		while (pos < json.size() && json[pos] != '"') {
			if (json[pos] == '\\' && pos + 1 < json.size()) {
				++pos;
				switch (json[pos]) {
				case '"':  val += '"';  break;
				case '\\': val += '\\'; break;
				case 'n':  val += '\n'; break;
				case 'r':  val += '\r'; break;
				case 't':  val += '\t'; break;
				default:   val += json[pos]; break;
				}
			} else {
				val += json[pos];
			}
			++pos;
		}
		return val;
	}
	return "";
}

// Find the start of the '{' for a JSON object field "key":{...} or "key": {...}.
// Returns the position of '{', or npos.
static size_t find_obj_field_start(const std::string& json, const std::string& key) {
	for (const auto& search : {
		"\"" + key + "\":{",
		"\"" + key + "\": {"
	}) {
		auto pos = json.find(search);
		if (pos != std::string::npos)
			return pos + search.size() - 1;  // points to '{'
	}
	return std::string::npos;
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

// Extract a nested JSON object field by key.
static std::string extract_obj_field(const std::string& json, const std::string& key) {
	auto start = find_obj_field_start(json, key);
	if (start == std::string::npos) return "";
	return extract_balanced(json, start);
}

// Find and extract the outermost JSON object from a string (skips leading text).
static std::string extract_outermost_json(const std::string& s) {
	auto start = s.find('{');
	if (start == std::string::npos) return "";
	return extract_balanced(s, start);
}

// Recursively parse a formula JSON object into an nlang_ba formula tree.
// Falls back to mk_atom(original_text) on any parse failure.
static nlang_ba::fptr parse_formula_json(const std::string& json,
	const std::string& fallback_text)
{
	using F = nlang_ba::formula;
	std::string op = extract_str_field(json, "op");

	if (op == "atom") {
		std::string text = extract_str_field(json, "text");
		return text.empty() ? F::mk_atom(fallback_text) : F::mk_atom(std::move(text));
	}
	if (op == "not") {
		auto inner_json = extract_obj_field(json, "inner");
		if (inner_json.empty()) return F::mk_atom(fallback_text);
		return F::mk_not(parse_formula_json(inner_json, fallback_text));
	}
	if (op == "and" || op == "or") {
		auto left_json  = extract_obj_field(json, "left");
		auto right_json = extract_obj_field(json, "right");
		if (left_json.empty() || right_json.empty()) return F::mk_atom(fallback_text);
		auto l = parse_formula_json(left_json, fallback_text);
		auto r = parse_formula_json(right_json, fallback_text);
		return op == "and" ? F::mk_and(std::move(l), std::move(r))
		                   : F::mk_or(std::move(l), std::move(r));
	}
	// Unknown op or malformed: fall back to atom
	return F::mk_atom(fallback_text);
}

} // namespace

std::string deepseek_query(const std::string& prompt) {
	const char* key = std::getenv("DEEPSEEK_API_KEY");
	if (!key || std::string(key).empty()) return "";

	CURL* curl = curl_easy_init();
	if (!curl) return "";

	std::string response;

	auto escape_json = [](const std::string& s) {
		std::string out;
		out.reserve(s.size() + 8);
		for (char c : s) {
			if      (c == '"')  out += "\\\"";
			else if (c == '\\') out += "\\\\";
			else if (c == '\n') out += "\\n";
			else if (c == '\r') out += "\\r";
			else if (c == '\t') out += "\\t";
			else                out += c;
		}
		return out;
	};

	std::string body =
		"{\"model\":\"deepseek-chat\","
		"\"messages\":[{\"role\":\"user\",\"content\":\""
		+ escape_json(prompt) +
		"\"}],"
		"\"temperature\":0}";

	std::string auth_header = std::string("Authorization: Bearer ") + key;

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, auth_header.c_str());

	curl_easy_setopt(curl, CURLOPT_URL, "https://api.deepseek.com/chat/completions");
	curl_easy_setopt(curl, CURLOPT_POST, 1L);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, nlang_curl_write_cb);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);

	CURLcode res = curl_easy_perform(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	if (res != CURLE_OK) return "";

	// Extract "content" value from the last occurrence to skip any
	// "reasoning_content" that deepseek-chat may emit in extended mode.
	auto extract_content = [](const std::string& json) -> std::string {
		size_t pos = json.rfind("\"content\":");
		if (pos == std::string::npos) return "";
		pos += 10;
		while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) ++pos;
		if (pos >= json.size() || json[pos] != '"') return "";
		++pos;
		std::string content;
		while (pos < json.size() && json[pos] != '"') {
			if (json[pos] == '\\' && pos + 1 < json.size()) {
				++pos;
				switch (json[pos]) {
				case '"':  content += '"'; break;
				case '\\': content += '\\'; break;
				case 'n':  content += '\n'; break;
				case 'r':  content += '\r'; break;
				case 't':  content += '\t'; break;
				default:   content += json[pos]; break;
				}
			} else {
				content += json[pos];
			}
			++pos;
		}
		return content;
	};

	return extract_content(response);
}

bool deepseek_is_empty(const std::string& description) {
	if (description == "nothing") return true;
	if (description == "everything") return false;
	auto& cache = get_cache();
	{
		std::lock_guard<std::mutex> lk(cache.mtx);
		auto it = cache.is_empty_cache.find(description);
		if (it != cache.is_empty_cache.end()) return it->second;
	}
	const char* key = std::getenv("DEEPSEEK_API_KEY");
	if (!key || std::string(key).empty()) {
		std::lock_guard<std::mutex> lk(cache.mtx);
		return cache.is_empty_cache.emplace(description, false).first->second;
	}
	std::string prompt =
		"Is the statement '"
		+ description
		+ "' a logical contradiction (always false, impossible)?"
		  " Answer with only YES or NO.";
	auto ans = deepseek_query(prompt);
	bool result = false;
	for (auto& c : ans) {
		if (c == 'N' || c == 'n') { result = false; break; }
		if (c == 'Y' || c == 'y') { result = true;  break; }
	}
	std::lock_guard<std::mutex> lk(cache.mtx);
	return cache.is_empty_cache.emplace(description, result).first->second;
}

bool deepseek_is_universal(const std::string& description) {
	if (description == "everything") return true;
	if (description == "nothing") return false;
	auto& cache = get_cache();
	{
		std::lock_guard<std::mutex> lk(cache.mtx);
		auto it = cache.is_universal_cache.find(description);
		if (it != cache.is_universal_cache.end()) return it->second;
	}
	const char* key = std::getenv("DEEPSEEK_API_KEY");
	if (!key || std::string(key).empty()) {
		std::lock_guard<std::mutex> lk(cache.mtx);
		return cache.is_universal_cache.emplace(description, false).first->second;
	}
	std::string prompt =
		"Is the statement '"
		+ description
		+ "' a tautology (always true, necessarily true in all situations)?"
		  " Answer with only YES or NO.";
	auto ans = deepseek_query(prompt);
	bool result = false;
	for (auto& c : ans) {
		if (c == 'Y' || c == 'y') { result = true;  break; }
		if (c == 'N' || c == 'n') { result = false; break; }
	}
	std::lock_guard<std::mutex> lk(cache.mtx);
	return cache.is_universal_cache.emplace(description, result).first->second;
}

bool deepseek_equivalent(const std::string& a, const std::string& b) {
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
	const char* key = std::getenv("DEEPSEEK_API_KEY");
	if (!key || std::string(key).empty()) {
		std::lock_guard<std::mutex> lk(cache.mtx);
		return cache.equivalent_cache.emplace(key_pair, false).first->second;
	}
	std::string prompt =
		"Are the two statements '"
		+ a + "' and '" + b
		+ "' logically equivalent (true in exactly the same situations)?"
		  " Answer with only YES or NO.";
	auto ans = deepseek_query(prompt);
	bool result = false;
	for (auto& c : ans) {
		if (c == 'Y' || c == 'y') { result = true;  break; }
		if (c == 'N' || c == 'n') { result = false; break; }
	}
	std::lock_guard<std::mutex> lk(cache.mtx);
	return cache.equivalent_cache.emplace(key_pair, result).first->second;
}

std::string deepseek_stronger_statement(const std::string& description) {
	if (description == "nothing") return "nothing";
	if (description == "everything") return "it is raining";
	auto& cache = get_cache();
	{
		std::lock_guard<std::mutex> lk(cache.mtx);
		auto it = cache.sub_cache.find(description);
		if (it != cache.sub_cache.end()) return it->second;
	}
	const char* key = std::getenv("DEEPSEEK_API_KEY");
	if (!key || std::string(key).empty()) {
		std::string fallback = description + " and specifically so";
		std::lock_guard<std::mutex> lk(cache.mtx);
		return cache.sub_cache.emplace(description, fallback).first->second;
	}
	std::string prompt =
		"Give one logically stronger statement that strictly implies '"
		+ description
		+ "' but is not equivalent to it."
		  " Reply with only the statement, no punctuation.";
	auto ans = deepseek_query(prompt);
	if (ans.empty()) ans = description + " and specifically so";
	ans.erase(0, ans.find_first_not_of(" \t\n\r."));
	auto last = ans.find_last_not_of(" \t\n\r.");
	if (last != std::string::npos) ans = ans.substr(0, last + 1);
	if (ans.empty()) ans = description + " and specifically so";
	std::lock_guard<std::mutex> lk(cache.mtx);
	return cache.sub_cache.emplace(description, ans).first->second;
}

nlang_ba::fptr deepseek_decompose(const std::string& s) {
	using F = nlang_ba::formula;
	if (s == "nothing")     return F::mk_bot();
	if (s == "everything")  return F::mk_top();

	auto& cache = get_cache();
	{
		std::lock_guard<std::mutex> lk(cache.mtx);
		auto it = cache.decompose_cache.find(s);
		if (it != cache.decompose_cache.end()) return it->second;
	}

	const char* key = std::getenv("DEEPSEEK_API_KEY");
	if (!key || std::string(key).empty()) {
		auto result = F::mk_atom(s);
		std::lock_guard<std::mutex> lk(cache.mtx);
		return cache.decompose_cache.emplace(s, result).first->second;
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

	auto response = deepseek_query(prompt);
	auto json = extract_outermost_json(response);

	nlang_ba::fptr result;
	if (json.empty()) {
		result = F::mk_atom(s);  // fallback: treat whole string as atom
	} else {
		result = parse_formula_json(json, s);
		// Sanity check: if decompose returned a single atom equal to s, no benefit
		// (no structural simplification possible). That's fine — it's still correct.
	}

	std::lock_guard<std::mutex> lk(cache.mtx);
	return cache.decompose_cache.emplace(s, result).first->second;
}

} // namespace idni::tau_lang
