// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md
// Canonical JSON transport, SHA-256 and call receipts for GA1.
#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <type_traits>
#include <limits>
#include <climits>
#include <utility>
#include <variant>
#include <vector>

namespace idni::tau_lang::ga1_wire {

// --------------------------------------------------------------------------
// JSON: what Python's json.loads accepts, and json.dumps(sort_keys=True,
// separators=(",", ":"), ensure_ascii=True) as the canonical form.
// --------------------------------------------------------------------------

struct json {
	enum class type { null, boolean, integer, real, string, array, object };
	type t = type::null;
	bool b = false;
	std::string num;              // integer or real token text
	std::u16string s;             // string, as UTF-16 code units
	bool asc = true;              // string: every unit ASCII, known from parsing or construction
	std::vector<json> a;
	std::vector<std::pair<std::u16string, json>> o;   // unique keys

	static json null_() { return json{}; }
	static json boolean(bool v) { json j; j.t = type::boolean; j.b = v; return j; }
	static json integer(int64_t v) { json j; j.t = type::integer; j.num = std::to_string(v); return j; }
	static json str(const std::string& v) {
		json j; j.t = type::string; j.s.assign(v.begin(), v.end());
		for (char c : v) if ((unsigned char)c >= 0x80) j.asc = false;
		return j; }
	static json array(std::vector<json> v = {}) {
		json j; j.t = type::array; j.a = std::move(v); return j; }
	static json object() { json j; j.t = type::object; return j; }

	bool is_str() const { return t == type::string; }
	bool is_int() const { return t == type::integer; }
	bool is_obj() const { return t == type::object; }
	bool is_arr() const { return t == type::array; }
	bool is_null() const { return t == type::null; }

	const json* get(const std::string& k) const {
		if (t != type::object) return nullptr;
		std::u16string key(k.begin(), k.end());
		for (auto& [kk, v] : o) if (kk == key) return &v;
		return nullptr;
	}
	json& set(const std::string& k, json v) {
		std::u16string key(k.begin(), k.end());
		for (auto& [kk, vv] : o) if (kk == key) return vv = std::move(v);
		o.emplace_back(key, std::move(v));
		return o.back().second;
	}
	// ASCII content of a string (false if any unit is not ASCII)
	bool ascii(std::string& out) const {
		if (t != type::string) return false;
		out.clear();
		for (char16_t c : s) { if (c >= 0x80) return false; out.push_back((char)c); }
		return true;
	}
	// int64 value of an integer token, if it fits
	std::optional<int64_t> i64() const {
		if (t != type::integer) return std::nullopt;
		bool neg = num[0] == '-';
		size_t p = neg ? 1 : 0;
		unsigned __int128 v = 0;
		for (; p < num.size(); ++p) {
			v = v * 10 + (num[p] - '0');
			if (v > (unsigned __int128)INT64_MAX + 1) return std::nullopt;
		}
		if (!neg && v > (unsigned __int128)INT64_MAX) return std::nullopt;
		return neg ? (int64_t)(-(__int128)v) : (int64_t)v;
	}
	bool negative() const { return t == type::integer && num[0] == '-' && num != "-0"; }
	bool contains_number() const {
		if (t == type::integer || t == type::real) return true;
		for (auto& x : a) if (x.contains_number()) return true;
		for (auto& kv : o) if (kv.second.contains_number()) return true;
		return false;
	}
};

/// A JSON reader over bytes (std::string) or UTF-16 code units
/// (std::u16string); a committed rule is read in whichever it is stored.
/// Every comparison is on whole code units.
template <class S>
struct json_parser {
	const S& in;
	size_t p = 0;
	size_t depth = 0, max_depth;
	json_parser(const S& s, size_t md) : in(s), max_depth(md) {}
	static constexpr bool bytes = sizeof(typename S::value_type) == 1;

	uint32_t at(size_t i) const { return (uint32_t)(std::make_unsigned_t<typename S::value_type>)in[i]; }
	bool digit(size_t i) const { return i < in.size() && at(i) >= '0' && at(i) <= '9'; }
	void ws() { while (p < in.size() && (at(p) == ' ' || at(p) == '\t' || at(p) == '\n' || at(p) == '\r')) ++p; }
	bool lit(const char* w) {
		size_t n = std::char_traits<char>::length(w);
		if (p + n > in.size()) return false;
		for (size_t k = 0; k < n; ++k) if (at(p + k) != (unsigned char)w[k]) return false;
		p += n;
		return true;
	}
	static int hexv(uint32_t c) {
		if (c >= '0' && c <= '9') return (int)(c - '0');
		if (c >= 'a' && c <= 'f') return (int)(c - 'a' + 10);
		if (c >= 'A' && c <= 'F') return (int)(c - 'A' + 10);
		return -1;
	}
	bool string(std::u16string& out, bool& asc) {
		if (p >= in.size() || at(p) != '"') return false;
		++p;
		asc = true;
		while (p < in.size()) {
			uint32_t c = at(p);
			if (c == '"') { ++p; return true; }
			if (c < 0x20) return false;                      // strict: no raw control
			if (c == '\\') {
				if (++p >= in.size()) return false;
				uint32_t e = at(p++);
				switch (e) {
				case '"': out.push_back('"'); break;
				case '\\': out.push_back('\\'); break;
				case '/': out.push_back('/'); break;
				case 'b': out.push_back('\b'); break;
				case 'f': out.push_back('\f'); break;
				case 'n': out.push_back('\n'); break;
				case 'r': out.push_back('\r'); break;
				case 't': out.push_back('\t'); break;
				case 'u': {
					if (p + 4 > in.size()) return false;
					int v = 0;
					for (int k = 0; k < 4; ++k) {
						int h = hexv(at(p + (size_t)k));
						if (h < 0) return false;
						v = v * 16 + h;
					}
					p += 4;
					if (v >= 0x80) asc = false;
					out.push_back((char16_t)v);
					break;
				}
				default: return false;
				}
				continue;
			}
			if (c < 0x80) { out.push_back((char16_t)c); ++p; continue; }
			asc = false;
			if constexpr (!bytes) { out.push_back((char16_t)c); ++p; continue; }
			else {
				// UTF-8 to UTF-16
				uint32_t cp; int n;
				if ((c & 0xE0) == 0xC0) { cp = c & 0x1F; n = 1; }
				else if ((c & 0xF0) == 0xE0) { cp = c & 0x0F; n = 2; }
				else if ((c & 0xF8) == 0xF0) { cp = c & 0x07; n = 3; }
				else return false;
				++p;
				for (int k = 0; k < n; ++k) {
					if (p >= in.size() || (at(p) & 0xC0) != 0x80) return false;
					cp = (cp << 6) | (at(p++) & 0x3F);
				}
				if (cp >= 0x10000) {
					cp -= 0x10000;
					out.push_back((char16_t)(0xD800 + (cp >> 10)));
					out.push_back((char16_t)(0xDC00 + (cp & 0x3FF)));
				} else out.push_back((char16_t)cp);
			}
		}
		return false;
	}
	bool number(json& j) {
		size_t s0 = p;
		if (p < in.size() && at(p) == '-') ++p;
		if (p >= in.size()) return false;
		if (at(p) == '0') ++p;
		else if (at(p) >= '1' && at(p) <= '9') { while (digit(p)) ++p; }
		else return false;
		bool real = false;
		if (p + 1 < in.size() && at(p) == '.' && digit(p + 1)) {
			real = true; p += 2;
			while (digit(p)) ++p;
		}
		if (p < in.size() && (at(p) == 'e' || at(p) == 'E')) {
			size_t q = p + 1;
			if (q < in.size() && (at(q) == '+' || at(q) == '-')) ++q;
			if (digit(q)) {
				real = true; p = q;
				while (digit(p)) ++p;
			}
		}
		j.num.clear();
		for (size_t k = s0; k < p; ++k) j.num.push_back((char)at(k));
		j.t = real ? json::type::real : json::type::integer;
		// Python: int() of more than 4,300 digits is a ValueError
		if (!real && j.num.size() - (j.num[0] == '-') > 4300) return false;
		return true;
	}
	bool value(json& j) {
		ws();
		if (p >= in.size()) return false;
		uint32_t c = at(p);
		if (c == '{') {
			if (++depth > max_depth) return false;
			++p; j.t = json::type::object; ws();
			if (p < in.size() && at(p) == '}') { ++p; --depth; return true; }
			for (;;) {
				ws();
				std::u16string k;
				bool kasc;
				if (!string(k, kasc)) return false;
				ws();
				if (p >= in.size() || at(p) != ':') return false;
				++p;
				json v;
				if (!value(v)) return false;
				bool found = false;
				for (auto& kv : j.o) if (kv.first == k) { kv.second = std::move(v); found = true; break; }
				if (!found) j.o.emplace_back(std::move(k), std::move(v));
				ws();
				if (p < in.size() && at(p) == ',') { ++p; continue; }
				if (p < in.size() && at(p) == '}') { ++p; --depth; return true; }
				return false;
			}
		}
		if (c == '[') {
			if (++depth > max_depth) return false;
			++p; j.t = json::type::array; ws();
			if (p < in.size() && at(p) == ']') { ++p; --depth; return true; }
			for (;;) {
				json v;
				if (!value(v)) return false;
				j.a.push_back(std::move(v));
				ws();
				if (p < in.size() && at(p) == ',') { ++p; continue; }
				if (p < in.size() && at(p) == ']') { ++p; --depth; return true; }
				return false;
			}
		}
		if (c == '"') { j.t = json::type::string; return string(j.s, j.asc); }
		if (lit("true")) { j = json::boolean(true); return true; }
		if (lit("false")) { j = json::boolean(false); return true; }
		if (lit("null")) { j = json{}; return true; }
		if (lit("NaN")) { j.t = json::type::real; j.num = "NaN"; return true; }
		if (lit("Infinity")) { j.t = json::type::real; j.num = "Infinity"; return true; }
		if (lit("-Infinity")) { j.t = json::type::real; j.num = "-Infinity"; return true; }
		return number(j);
	}
};

/// json.loads: nullopt when Python raises (ValueError, or nesting deeper
/// than max_depth).
template <class S = std::string>
inline std::optional<json> parse_json(const S& s, size_t max_depth = 100000) {
	json_parser<S> jp(s, max_depth);
	json j;
	if (!jp.value(j)) return std::nullopt;
	jp.ws();
	if (jp.p != s.size()) return std::nullopt;
	return j;
}

inline void dump_string(std::string& out, const std::u16string& s) {
	static const char* hx = "0123456789abcdef";
	out.push_back('"');
	for (char16_t c : s) {
		if (c == '"') out += "\\\"";
		else if (c == '\\') out += "\\\\";
		else if (c == '\n') out += "\\n";
		else if (c == '\r') out += "\\r";
		else if (c == '\t') out += "\\t";
		else if (c == '\b') out += "\\b";
		else if (c == '\f') out += "\\f";
		else if (c >= 0x20 && c <= 0x7e) out.push_back((char)c);
		else {
			out += "\\u";
			out.push_back(hx[(c >> 12) & 15]); out.push_back(hx[(c >> 8) & 15]);
			out.push_back(hx[(c >> 4) & 15]); out.push_back(hx[c & 15]);
		}
	}
	out.push_back('"');
}

inline std::vector<uint32_t> code_points(const std::u16string& s) {
	std::vector<uint32_t> r;
	for (size_t i = 0; i < s.size(); ++i) {
		char16_t c = s[i];
		if (c >= 0xD800 && c < 0xDC00 && i + 1 < s.size() && s[i + 1] >= 0xDC00 && s[i + 1] < 0xE000) {
			r.push_back(0x10000 + ((uint32_t)(c - 0xD800) << 10) + (s[i + 1] - 0xDC00));
			++i;
		} else r.push_back(c);
	}
	return r;
}

/// Canonical form into out; false when j holds a real and reals are refused
/// (no canonical form here), else a real is copied as its token.
inline bool dump_into(std::string& out, const json& j, bool reals = false) {
	switch (j.t) {
	case json::type::null: out += "null"; return true;
	case json::type::boolean: out += j.b ? "true" : "false"; return true;
	case json::type::integer: out += (j.num == "-0" ? std::string("0") : j.num); return true;
	case json::type::real: if (!reals) return false; out += j.num; return true;
	case json::type::string: dump_string(out, j.s); return true;
	case json::type::array:
		out.push_back('[');
		for (size_t i = 0; i < j.a.size(); ++i) {
			if (i) out.push_back(',');
			if (!dump_into(out, j.a[i], reals)) return false;
		}
		out.push_back(']');
		return true;
	case json::type::object: {
		std::vector<const std::pair<std::u16string, json>*> kv;
		for (auto& x : j.o) kv.push_back(&x);
		std::sort(kv.begin(), kv.end(), [](auto* x, auto* y) {
			return code_points(x->first) < code_points(y->first); });
		out.push_back('{');
		for (size_t i = 0; i < kv.size(); ++i) {
			if (i) out.push_back(',');
			dump_string(out, kv[i]->first);
			out.push_back(':');
			if (!dump_into(out, kv[i]->second, reals)) return false;
		}
		out.push_back('}');
		return true;
	}
	}
	return false;
}

inline std::optional<std::string> canon(const json& j) {
	std::string s;
	if (!dump_into(s, j)) return std::nullopt;
	return s;
}

/// Output form: canonical, except that a real is written as it was read.
inline std::string dump(const json& j) {
	std::string s;
	dump_into(s, j, true);
	return s;
}

// --------------------------------------------------------------------------
// SHA-256 (FIPS 180-4)
// --------------------------------------------------------------------------

/// SHA-256 of ASCII or byte content, read in place: whole 64-byte blocks
/// from the input itself, only the padded tail in a local buffer.
template <class S>
inline std::string sha256_of(const S& msg) {
	static const uint32_t K[64] = {
		0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
		0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
		0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
		0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
		0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
		0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
		0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
		0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2};
	uint32_t h[8] = {0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19};
	auto rotr = [](uint32_t x, int n) { return (x >> n) | (x << (32 - n)); };
	auto block = [&](auto byte_at) {
		uint32_t w[64];
		for (int i = 0; i < 16; ++i)
			w[i] = (byte_at(4 * i) << 24) | (byte_at(4 * i + 1) << 16) | (byte_at(4 * i + 2) << 8) | byte_at(4 * i + 3);
		for (int i = 16; i < 64; ++i) {
			uint32_t s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
			uint32_t s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
			w[i] = w[i - 16] + s0 + w[i - 7] + s1;
		}
		uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4], f = h[5], g = h[6], hh = h[7];
		for (int i = 0; i < 64; ++i) {
			uint32_t S1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
			uint32_t ch = (e & f) ^ (~e & g);
			uint32_t t1 = hh + S1 + ch + K[i] + w[i];
			uint32_t S0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
			uint32_t mj = (a & b) ^ (a & c) ^ (b & c);
			uint32_t t2 = S0 + mj;
			hh = g; g = f; f = e; e = d + t1; d = c; c = b; b = a; a = t1 + t2;
		}
		h[0] += a; h[1] += b; h[2] += c; h[3] += d; h[4] += e; h[5] += f; h[6] += g; h[7] += hh;
	};
	const size_t n = msg.size(), whole = n / 64 * 64;
	auto unit = [&](size_t i) { return (uint32_t)((std::make_unsigned_t<typename S::value_type>)msg[i] & 0xff); };
	for (size_t off = 0; off < whole; off += 64) block([&](int i) { return unit(off + (size_t)i); });
	unsigned char tail[128] = {};
	size_t t = n - whole;
	for (size_t k = 0; k < t; ++k) tail[k] = (unsigned char)unit(whole + k);
	tail[t] = 0x80;
	size_t len = (t + 1 + 8 <= 64) ? 64 : 128;
	uint64_t bits = (uint64_t)n * 8;
	for (int i = 0; i < 8; ++i) tail[len - 1 - (size_t)i] = (unsigned char)((bits >> (i * 8)) & 0xff);
	for (size_t off = 0; off < len; off += 64) block([&](int i) { return (uint32_t)tail[off + (size_t)i]; });
	static const char* hx = "0123456789abcdef";
	std::string r;
	for (uint32_t x : h) for (int i = 28; i >= 0; i -= 4) r.push_back(hx[(x >> i) & 15]);
	return r;
}

inline std::string sha256(const std::string& msg) { return sha256_of(msg); }

using site_t = std::variant<std::monostate, int64_t, std::string>;   // null, index, name

inline json site_json(const site_t& s) {
	if (std::holds_alternative<int64_t>(s)) return json::integer(std::get<int64_t>(s));
	if (std::holds_alternative<std::string>(s)) return json::str(std::get<std::string>(s));
	return json{};
}

struct event { std::string stage; int64_t seq; site_t site; std::string kind; int64_t weight; };

/// Why a call stopped.
struct stop_t {
	enum class why { blocked, unsupported, rejected, not_held } k;
	std::string reason, stage;
	site_t at;
	event e;
};

struct meter {
	std::optional<int64_t> cap;           // nullopt: uncapped (trace only)
	int64_t used = 0, seq = 0;
	std::vector<event>* trace = nullptr;
	std::optional<stop_t> stop;
	/// Counts before the event is done; false once the cap would be passed.
	bool charge(const std::string& stage, site_t site, const std::string& kind, int64_t weight) {
		if (cap && used + weight > *cap) {
			stop = stop_t{stop_t::why::blocked, "", stage, {}, {stage, seq, std::move(site), kind, weight}};
			return false;
		}
		used += weight;
		++seq;
		if (trace) trace->push_back({stage, seq - 1, std::move(site), kind, weight});
		return true;
	}
	bool unsupported(const std::string& reason, const std::string& stage, site_t at = {}) {
		stop = stop_t{stop_t::why::unsupported, reason, stage, std::move(at), {}};
		return false;
	}
	bool rejected(const std::string& reason, const std::string& stage, site_t at = {}) {
		stop = stop_t{stop_t::why::rejected, reason, stage, std::move(at), {}};
		return false;
	}
	/// A well-formed state whose plan the worker does not hold. Not a
	/// verdict and not metered: the host prepares the plan or fails operationally.
	bool not_held(const std::string& digest) {
		stop = stop_t{stop_t::why::not_held, digest, "step.admit", {}, {}};
		return false;
	}
};

inline json event_json(const event& e) {
	json o = json::object();
	o.set("stage", json::str(e.stage));
	o.set("seq", json::integer(e.seq));
	o.set("site", site_json(e.site));
	o.set("kind", json::str(e.kind));
	o.set("weight", json::integer(e.weight));
	return o;
}

inline json stopped_json(const char* status, const std::string& reason, const std::string& stage,
	const site_t& at, int64_t used)
{
	json o = json::object();
	o.set("status", json::str(status));
	o.set("reason", json::str(reason));
	o.set("stage", json::str(stage));
	o.set("at", site_json(at));
	o.set("used", json::integer(used));
	return o;
}

inline json outcome_of_stop(const meter& m) {
	const stop_t& s = *m.stop;
	if (s.k == stop_t::why::blocked) {
		json o = json::object();
		o.set("status", json::str("UnknownBudget"));
		o.set("used", json::integer(m.used));
		o.set("stage", json::str(s.e.stage));
		o.set("first_blocked_event", event_json(s.e));
		return o;
	}
	if (s.k == stop_t::why::not_held) {                  // no used: not a metered answer
		json o = json::object();
		o.set("status", json::str("Operational"));
		o.set("kind", json::str("plan_not_held"));
		o.set("plan", json::str(s.reason));
		return o;
	}
	return stopped_json(s.k == stop_t::why::rejected ? "Rejected" : "Unsupported", s.reason, s.stage, s.at, m.used);
}

} // namespace idni::tau_lang::ga1_wire
