// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md
//
// Contract taubudget/2: a current-only guarded account rule (the GA1
// fragment) built once into a plan and saved as canonical bytes, then loaded
// and stepped without parsing, deciding or decoding it again.
//
// The tariff is taubudget/1's, event for event (docs/ga1_plan.md). Every
// charge happens before the work it pays for; the first charge over the cap,
// or the first refusal, stops the call and is its outcome. Pure: no clock,
// no randomness, no global state, no Tau types. Raises no exception.
#pragma once

#include <array>
#include <functional>
#include <string_view>
#include <tuple>

#include "wire.h"

namespace idni::tau_lang::ga1 {

using ga1_wire::json;
using ga1_wire::meter;
using ga1_wire::site_t;
using ga1_wire::event;

inline constexpr const char* SCHEMA = "taubudget/2";
inline constexpr int64_t MAX_TEXT_BYTES = 8192, MAX_DEPTH = 64, MAX_WIDTH = 1024,
	MAX_ARTIFACT_BYTES = 1 << 18, BLOCK = 32;
inline constexpr int64_t MAX_COUNTER = (int64_t(1) << 62) - 1;   // revision and clock
inline constexpr size_t MAX_ARTIFACT_DEPTH = 16;
inline int64_t blocks(int64_t n) { return (n + BLOCK - 1) / BLOCK; }
/// The most a load can be charged: load.admit, a scan and a check block per
/// 32 bytes of the largest admissible artifact, load.commit.
inline constexpr int64_t LOAD_CAP = 1 + 2 * ((MAX_ARTIFACT_BYTES + BLOCK - 1) / BLOCK) + 1;

inline int64_t limbs(int64_t w) { return (w + 63) / 64; }
inline int64_t hex_digits(int64_t w) { return (w + 3) / 4; }

/// What a call did to the artifact and the rule, for tests: decodes (strict
/// artifact checks), hashes (SHA-256 of the artifact), builds (from text).
struct probe { uint64_t artifact_decodes = 0, artifact_hashes = 0, builds = 0; };

#define GA1_TRY(x) do { if (!(x)) return {}; } while (0)

// --------------------------------------------------------------------------
// Hex values of a fixed width: lowercase, zero-padded to hex_digits(width), so
// that two values of one width compare as strings.
// --------------------------------------------------------------------------

inline bool is_lower_hex(const std::string& s) {
	if (s.empty()) return false;
	for (char c : s) if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))) return false;
	return true;
}

/// Bit length of a lowercase hex string (leading zeros allowed).
inline int64_t bit_length(const std::string& h) {
	size_t i = 0;
	while (i < h.size() && h[i] == '0') ++i;
	if (i == h.size()) return 0;
	int d = h[i] <= '9' ? h[i] - '0' : h[i] - 'a' + 10, b = 0;
	while (d) { ++b; d >>= 1; }
	return (int64_t)(h.size() - i - 1) * 4 + b;
}

inline std::string pad_hex(const std::string& stripped, int64_t width) {
	size_t n = (size_t)hex_digits(width);
	return stripped.size() >= n ? stripped : std::string(n - stripped.size(), '0') + stripped;
}

/// The value of a literal in base 16, 2 or 10, as lowercase hex without
/// leading zeros ("0" for zero).
inline std::string literal_hex(int base, const std::string& digits) {
	static const char* hx = "0123456789abcdef";
	if (base == 16) {
		std::string s;
		for (char c : digits) s.push_back((char)(c >= 'A' && c <= 'F' ? c - 'A' + 'a' : c));
		size_t i = 0;
		while (i + 1 < s.size() && s[i] == '0') ++i;
		return s.substr(i);
	}
	std::vector<uint32_t> v{0};                         // little-endian base 2^32
	for (char c : digits) {
		uint64_t carry = (uint64_t)(c - '0');
		for (auto& limb : v) {
			uint64_t x = (uint64_t)limb * (uint64_t)base + carry;
			limb = (uint32_t)x;
			carry = x >> 32;
		}
		if (carry) v.push_back((uint32_t)carry);
	}
	std::string s;
	for (size_t k = v.size(); k-- > 0;)
		for (int sh = 28; sh >= 0; sh -= 4) s.push_back(hx[(v[k] >> sh) & 15]);
	size_t i = 0;
	while (i + 1 < s.size() && s[i] == '0') ++i;
	return s.substr(i);
}

// --------------------------------------------------------------------------
// Lexing
// --------------------------------------------------------------------------

struct token {
	std::string kind;                      // always, stream, const, or the operator
	char io = 0; int64_t idx = 0;          // stream
	int base = 0; std::string digits;      // const
	int64_t width = 0;                     // stream or const
};

namespace detail {

inline bool ws(char c) { return c == ' ' || c == '\t'; }
inline bool digit(char c) { return c >= '0' && c <= '9'; }
inline size_t skip(const std::string& t, size_t p, bool multiline) {
	while (p < t.size()) {
		if (ws(t[p]) || (multiline && t[p] == '\n')) ++p;
		else if (multiline && t[p] == '\r' && p + 1 < t.size() && t[p + 1] == '\n') p += 2;
		else break;
	}
	return p;
}

/// 0|[1-9][0-9]{0,extra}, then not followed by another digit's worth of the run.
inline bool number(const std::string& t, size_t& p, size_t extra, int64_t& v) {
	size_t q = p;
	while (q < t.size() && digit(t[q])) ++q;
	size_t n = q - p;
	if (n == 0 || (t[p] == '0' && n != 1) || n > extra + 1) return false;
	v = 0;
	for (size_t k = p; k < q; ++k) v = v * 10 + (t[k] - '0');
	p = q;
	return true;
}

/// _TYPED: ws ":" ws "bv" ws "[" ws width ws "]"
inline bool typed(const std::string& t, size_t& p, int64_t& w, bool multiline) {
	size_t q = skip(t, p, multiline);
	if (q >= t.size() || t[q] != ':') return false;
	q = skip(t, q + 1, multiline);
	if (t.compare(q, 2, "bv") != 0) return false;
	q = skip(t, q + 2, multiline);
	if (q >= t.size() || t[q] != '[') return false;
	q = skip(t, q + 1, multiline);
	if (!number(t, q, 4, w)) return false;
	q = skip(t, q, multiline);
	if (q >= t.size() || t[q] != ']') return false;
	p = q + 1;
	return true;
}

inline bool hexc(char c) { return digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }

inline bool match(const std::string& t, size_t pos, token& tok, size_t& end, bool multiline) {
	auto word = [](char c) { return digit(c) || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; };
	if (t.compare(pos, 6, "always") == 0 && !(pos + 6 < t.size() && word(t[pos + 6]))) {
		tok = token{}; tok.kind = "always"; end = pos + 6; return true;
	}
	if (t[pos] == 'i' || t[pos] == 'o') {
		size_t p = pos + 1;
		int64_t idx, w;
		if (number(t, p, 8, idx) && t.compare(p, 3, "[t]") == 0) {
			p += 3;
			if (typed(t, p, w, multiline)) {
				tok = token{}; tok.kind = "stream"; tok.io = t[pos]; tok.idx = idx; tok.width = w; end = p;
				return true;
			}
		}
	}
	if (t[pos] == '{') {
		size_t p = skip(t, pos + 1, multiline), q;
		int base = 0;
		if (t.compare(p, 2, "#x") == 0) {
			q = p + 2; while (q < t.size() && hexc(t[q])) ++q;
			if (q > p + 2) base = 16;
		} else if (t.compare(p, 2, "#b") == 0) {
			q = p + 2; while (q < t.size() && (t[q] == '0' || t[q] == '1')) ++q;
			if (q > p + 2) base = 2;
		} else {
			q = p; while (q < t.size() && digit(t[q])) ++q;
			if (q > p) base = 10;
		}
		if (base) {
			std::string digits = t.substr(base == 10 ? p : p + 2, q - (base == 10 ? p : p + 2));
			size_t r = skip(t, q, multiline);
			int64_t w;
			if (r < t.size() && t[r] == '}') {
				++r;
				if (typed(t, r, w, multiline)) {
					tok = token{}; tok.kind = "const"; tok.base = base; tok.digits = digits; tok.width = w; end = r;
					return true;
				}
			}
		}
	}
	static const char* ops[] = {"&&", "||", "!=", "<=", ">=", "=", "<", ">", "!", "?", ":", "(", ")", "."};
	for (const char* op : ops) {
		size_t n = std::char_traits<char>::length(op);
		if (t.compare(pos, n, op) == 0) { tok = token{}; tok.kind = op; end = pos + n; return true; }
	}
	return false;
}

} // namespace detail

inline std::optional<std::vector<token>> lex(const std::string& text, meter& m, bool multiline) {
	std::vector<token> toks;
	size_t pos = 0;
	for (;;) {
		pos = detail::skip(text, pos, multiline);
		if (pos >= text.size()) return toks;
		GA1_TRY(m.charge("build.lex", (int64_t)toks.size(), "tok", 1));
		token tok; size_t end;
		if (!detail::match(text, pos, tok, end, multiline))
			return m.unsupported("lex_error", "build.lex", (int64_t)toks.size()), std::nullopt;
		toks.push_back(std::move(tok));
		pos = end;
	}
}

// --------------------------------------------------------------------------
// Parsing (LL(1), one charge per token examined)
// --------------------------------------------------------------------------

struct pnode {
	std::string kind;                      // in out const cmp not and or cond
	int64_t idx = 0, width = 0;            // in, out; const width
	int base = 0; std::string digits;      // const
	std::string rel;                       // cmp
	int64_t a = -1, b = -1, c = -1;        // children
};

struct parser {
	parser(const std::vector<token>& t, meter& mm) : toks(t), m(mm) {}
	const std::vector<token>& toks;
	meter& m;
	size_t i = 0, charged = 0;
	int64_t depth = 0;
	std::vector<pnode> nodes;
	bool failed = false;

	/// The current token's kind ("" at the end), charging it on first examination.
	std::optional<std::string> kind() {
		if (i >= toks.size()) return std::string();
		if (charged <= i) {
			if (!m.charge("build.parse", (int64_t)i, toks[i].kind, 1)) return std::nullopt;
			charged = i + 1;
		}
		return toks[i].kind;
	}
	bool fail() { return m.unsupported("syntax_error", "build.parse", (int64_t)i); }
	bool expect(const char* k) { auto x = kind(); if (!x) return false; if (*x != k) return fail(); ++i; return true; }
	int64_t node(pnode n) { nodes.push_back(std::move(n)); return (int64_t)nodes.size() - 1; }
	bool enter() { if (++depth > MAX_DEPTH) return m.unsupported("nesting_too_deep", "build.parse", (int64_t)i); return true; }

	std::optional<int64_t> spec() {
		GA1_TRY(expect("always")); GA1_TRY(expect("("));
		auto root = e0(); GA1_TRY(root);
		GA1_TRY(expect(")")); GA1_TRY(expect("."));
		auto x = kind(); GA1_TRY(x);
		if (!x->empty()) return fail(), std::nullopt;
		return root;
	}
	std::optional<int64_t> e0() {
		auto g = e1(); GA1_TRY(g);
		auto x = kind(); GA1_TRY(x);
		if (*x != "?") return g;
		++i;
		auto t = primary(); GA1_TRY(t);
		GA1_TRY(expect(":"));
		auto e = primary(); GA1_TRY(e);
		pnode n; n.kind = "cond"; n.a = *g; n.b = *t; n.c = *e;
		return node(n);
	}
	std::optional<int64_t> e1() {
		auto l = e2(); GA1_TRY(l);
		for (;;) {
			auto x = kind(); GA1_TRY(x);
			if (*x != "||") return l;
			++i;
			auto r = e2(); GA1_TRY(r);
			pnode n; n.kind = "or"; n.a = *l; n.b = *r; l = node(n);
		}
	}
	std::optional<int64_t> e2() {
		auto l = e3(); GA1_TRY(l);
		for (;;) {
			auto x = kind(); GA1_TRY(x);
			if (*x != "&&") return l;
			++i;
			auto r = e3(); GA1_TRY(r);
			pnode n; n.kind = "and"; n.a = *l; n.b = *r; l = node(n);
		}
	}
	std::optional<int64_t> e3() {
		auto x = kind(); GA1_TRY(x);
		if (*x == "!") {
			GA1_TRY(enter());
			++i;
			auto in = e3(); GA1_TRY(in);
			--depth;
			pnode n; n.kind = "not"; n.a = *in;
			return node(n);
		}
		return primary();
	}
	std::optional<int64_t> primary() {
		auto x = kind(); GA1_TRY(x);
		if (*x == "(") {
			GA1_TRY(enter());
			++i;
			auto in = e0(); GA1_TRY(in);
			GA1_TRY(expect(")"));
			--depth;
			return in;
		}
		if (*x == "stream" || *x == "const") {
			auto l = term(); GA1_TRY(l);
			auto op = kind(); GA1_TRY(op);
			if (!(*op == "=" || *op == "!=" || *op == "<" || *op == "<=" || *op == ">" || *op == ">="))
				return fail(), std::nullopt;
			++i;
			auto r = term(); GA1_TRY(r);
			pnode n; n.kind = "cmp"; n.rel = *op; n.a = *l; n.b = *r;
			return node(n);
		}
		return fail(), std::nullopt;
	}
	std::optional<int64_t> term() {
		auto x = kind(); GA1_TRY(x);
		if (*x == "stream") {
			const token& t = toks[i++];
			pnode n; n.kind = t.io == 'i' ? "in" : "out"; n.idx = t.idx; n.width = t.width;
			return node(n);
		}
		if (*x == "const") {
			const token& t = toks[i++];
			pnode n; n.kind = "const"; n.base = t.base; n.digits = t.digits; n.width = t.width;
			return node(n);
		}
		return fail(), std::nullopt;
	}
};

// --------------------------------------------------------------------------
// Typing, shape decision, lowering, publication
// --------------------------------------------------------------------------

inline int64_t max_digits(int base) { return base == 16 ? MAX_WIDTH / 4 : base == 2 ? MAX_WIDTH : 309; }

/// Width per node (0 for formulas) and the padded hex of each constant.
inline bool type_nodes(const std::vector<pnode>& nodes, meter& m, std::vector<int64_t>& widths,
	std::map<int64_t, std::string>& values)
{
	widths.assign(nodes.size(), 0);
	for (size_t i = 0; i < nodes.size(); ++i) {
		const pnode& n = nodes[i];
		if (n.kind == "const") {
			int64_t w = n.width;
			int64_t weight = limbs(std::min(w, MAX_WIDTH));
			if (!m.charge("build.type", (int64_t)i, n.kind, weight ? weight : 1)) return false;
			if (!(1 <= w && w <= MAX_WIDTH)) return m.unsupported("width_out_of_range", "build.type", (int64_t)i);
			std::string d = n.digits;
			size_t z = 0;
			while (z < d.size() && d[z] == '0') ++z;
			std::string stripped = z == d.size() ? "0" : d.substr(z);
			if ((int64_t)stripped.size() > max_digits(n.base))
				return m.unsupported("literal_does_not_fit", "build.type", (int64_t)i);
			std::string h = literal_hex(n.base, stripped);
			if (bit_length(h) > w) return m.unsupported("literal_does_not_fit", "build.type", (int64_t)i);
			widths[i] = w;
			values[(int64_t)i] = pad_hex(h, w);
			continue;
		}
		if (!m.charge("build.type", (int64_t)i, n.kind, 1)) return false;
		if (n.kind == "in" || n.kind == "out") {
			if (!(1 <= n.width && n.width <= MAX_WIDTH))
				return m.unsupported("width_out_of_range", "build.type", (int64_t)i);
			widths[i] = n.width;
		} else if (n.kind == "cmp") {
			if (widths[(size_t)n.a] != widths[(size_t)n.b])
				return m.unsupported("width_mismatch", "build.type", (int64_t)i);
		}
	}
	return true;
}

using stream_key = std::pair<char, int64_t>;              // ('i' | 'o', index)

inline std::string stream_name(const stream_key& k) { return std::string(1, k.first) + std::to_string(k.second); }

inline bool decide(const std::vector<pnode>& nodes, int64_t root, meter& m,
	std::map<stream_key, int64_t>& table, int64_t& out)
{
	std::vector<std::pair<int64_t, char>> stack{{root, 'D'}};
	std::optional<int64_t> o;
	while (!stack.empty()) {
		auto [i, mode] = stack.back();
		stack.pop_back();
		const pnode& n = nodes[(size_t)i];
		if (!m.charge("build.decide", i, n.kind, 1)) return false;
		if (mode == 'D') {
			if (n.kind == "cond") { stack.push_back({n.c, 'D'}); stack.push_back({n.b, 'D'}); stack.push_back({n.a, 'G'}); }
			else if (n.kind == "cmp" && n.rel == "=") { stack.push_back({n.b, 'R'}); stack.push_back({n.a, 'L'}); }
			else return m.unsupported("not_guarded_definition", "build.decide", i);
			continue;
		}
		if (mode == 'L' && n.kind != "out") return m.unsupported("not_guarded_definition", "build.decide", i);
		if (mode == 'R' && n.kind == "out") return m.unsupported("output_on_rhs", "build.decide", i);
		if (mode == 'G') {
			if (n.kind == "out") return m.unsupported("output_in_guard", "build.decide", i);
			if (n.kind == "cond") return m.unsupported("conditional_in_guard", "build.decide", i);
			if (n.kind == "or" || n.kind == "and") { stack.push_back({n.b, 'G'}); stack.push_back({n.a, 'G'}); }
			else if (n.kind == "not") stack.push_back({n.a, 'G'});
			else if (n.kind == "cmp") { stack.push_back({n.b, 'G'}); stack.push_back({n.a, 'G'}); }
		}
		if (n.kind == "in" || n.kind == "out") {
			stream_key k{n.kind == "in" ? 'i' : 'o', n.idx};
			auto it = table.find(k);
			if (it == table.end()) table[k] = n.width;
			else if (it->second != n.width) return m.unsupported("stream_width_conflict", "build.decide", i);
			if (n.kind == "out") {
				if (!o) o = n.idx;
				else if (*o != n.idx) return m.unsupported("multiple_outputs", "build.decide", i);
			}
		}
	}
	for (auto& [k, w] : table) if (!m.charge("build.decide", stream_name(k), "stream", 1)) return false;
	out = *o;
	return true;
}

/// One instruction of an artifact: ["in", i] ["k", w, hex] ["cmp", rel, a, b]
/// ["not", a] ["and", a, b] ["or", a, b] ["sel", g, a, b].
struct op {
	std::string kind, rel, hex;
	int64_t i = 0, w = 0, a = 0, b = 0, c = 0;
	std::string key() const {
		return kind + "|" + rel + "|" + hex + "|" + std::to_string(i) + "|" + std::to_string(w) + "|"
			+ std::to_string(a) + "|" + std::to_string(b) + "|" + std::to_string(c);
	}
	json to_json() const {
		std::vector<json> v{json::str(kind)};
		if (kind == "in") v.push_back(json::integer(i));
		else if (kind == "k") { v.push_back(json::integer(w)); v.push_back(json::str(hex)); }
		else if (kind == "cmp") { v.push_back(json::str(rel)); v.push_back(json::integer(a)); v.push_back(json::integer(b)); }
		else if (kind == "not") v.push_back(json::integer(a));
		else if (kind == "and" || kind == "or") { v.push_back(json::integer(a)); v.push_back(json::integer(b)); }
		else { v.push_back(json::integer(a)); v.push_back(json::integer(b)); v.push_back(json::integer(c)); }
		return json::array(std::move(v));
	}
};

inline bool lower(const std::vector<pnode>& nodes, int64_t root, const std::map<int64_t, std::string>& values,
	const std::string& strategy, meter& m, std::vector<op>& ops, int64_t& root_op)
{
	std::map<std::string, int64_t> memo;
	std::vector<int64_t> at(nodes.size(), -1);
	auto emit = [&](op o) {
		if (strategy == "dag") {
			auto k = o.key();
			if (auto it = memo.find(k); it != memo.end()) return it->second;
			memo[k] = (int64_t)ops.size();
		}
		ops.push_back(std::move(o));
		return (int64_t)ops.size() - 1;
	};
	for (size_t i = 0; i < nodes.size(); ++i) {
		const pnode& n = nodes[i];
		if (!m.charge("build.lower", (int64_t)i, n.kind, 1)) return false;
		op o; o.kind = n.kind;
		if (n.kind == "in") { o.i = n.idx; at[i] = emit(o); }
		else if (n.kind == "const") { o.kind = "k"; o.w = n.width; o.hex = values.at((int64_t)i); at[i] = emit(o); }
		else if (n.kind == "cmp") {
			if (nodes[(size_t)n.a].kind == "out") at[i] = at[(size_t)n.b];
			else { o.rel = n.rel; o.a = at[(size_t)n.a]; o.b = at[(size_t)n.b]; at[i] = emit(o); }
		}
		else if (n.kind == "not") { o.a = at[(size_t)n.a]; at[i] = emit(o); }
		else if (n.kind == "and" || n.kind == "or") { o.a = at[(size_t)n.a]; o.b = at[(size_t)n.b]; at[i] = emit(o); }
		else if (n.kind == "cond") { o.kind = "sel"; o.a = at[(size_t)n.a]; o.b = at[(size_t)n.b]; o.c = at[(size_t)n.c]; at[i] = emit(o); }
	}
	root_op = at[(size_t)root];
	return true;
}

inline std::string artifact_text(const std::string& strategy, const std::map<stream_key, int64_t>& table,
	int64_t out, const std::vector<op>& ops, int64_t root_op)
{
	json a = json::object();
	a.set("fmt", json::str("TBA1"));
	a.set("strategy", json::str(strategy));
	json streams = json::array();
	for (auto& [k, w] : table)
		streams.a.push_back(json::array({json::str(std::string(1, k.first)), json::integer(k.second), json::integer(w)}));
	a.set("streams", streams);
	a.set("out", json::array({json::str("o"), json::integer(out), json::integer(table.at({'o', out}))}));
	json js = json::array();
	for (auto& o : ops) js.a.push_back(o.to_json());
	a.set("ops", js);
	a.set("root", json::integer(root_op));
	return *ga1_wire::canon(a);
}

/// A rule lowered: what an artifact or a typed plan writes down.
struct lowered {
	std::string strategy;
	std::map<stream_key, int64_t> table;
	int64_t out = 0;
	std::vector<op> ops;
	int64_t root = 0;
};

/// /1's build up to publication: admit, scan, lex, parse, type, decide, lower.
inline std::optional<lowered> build_lowered(const json* text_j, const json* strategy_j, meter& m, bool multiline = false) {
	GA1_TRY(m.charge("build.admit", (int64_t)0, "len", 1));
	lowered L;
	L.strategy = "tree";
	if (strategy_j && !(strategy_j->ascii(L.strategy) && (L.strategy == "tree" || L.strategy == "dag")))
		return m.unsupported("unknown_strategy", "build.admit"), std::nullopt;
	std::string text;
	if (!text_j || !text_j->is_str() || !text_j->asc || (int64_t)text_j->s.size() > MAX_TEXT_BYTES)
		return m.unsupported("text_not_admissible", "build.admit"), std::nullopt;
	text_j->ascii(text);
	for (int64_t b = 0; b < blocks((int64_t)text.size()); ++b) GA1_TRY(m.charge("build.scan", b, "block", 1));
	auto toks = lex(text, m, multiline); GA1_TRY(toks);
	parser p(*toks, m);
	auto root = p.spec(); GA1_TRY(root);
	std::vector<int64_t> widths;
	std::map<int64_t, std::string> values;
	GA1_TRY(type_nodes(p.nodes, m, widths, values));
	GA1_TRY(decide(p.nodes, *root, m, L.table, L.out));
	GA1_TRY(lower(p.nodes, *root, values, L.strategy, m, L.ops, L.root));
	return L;
}

/// Publication: one block event per 32 bytes written, then the commit.
inline bool publish(const std::string& bytes, meter& m) {
	for (int64_t b = 0; b < blocks((int64_t)bytes.size()); ++b)
		if (!m.charge("build.publish", b, "block", 1)) return false;
	return m.charge("build.publish", (int64_t)0, "commit", 1);
}

// --------------------------------------------------------------------------
// The header of saved bytes (planfmt.header and planfmt.split):
//   "tbp/1" SP format SP compiler SP semantics LF
// each name 1 to 32 characters from a-z 0-9 . / _ -
// --------------------------------------------------------------------------

/// The compiler: lexing, parsing, typing, deciding and lowering a rule, and
/// the three encoders. Bumped whenever a saved byte it produces could change;
/// ga1-prepared/compiler-ga1c-1.json pins its digests.
inline constexpr const char* COMPILER = "ga1c/1";
inline constexpr const char* MULTILINE_COMPILER = "ga1c/1-lf/1";
inline bool known_compiler(std::string_view compiler) {
	return compiler == COMPILER || compiler == MULTILINE_COMPILER;
}
inline constexpr std::string_view ENVELOPE = "tbp/1";
inline constexpr size_t MAX_ID = 32, MAX_HEADER = ENVELOPE.size() + 3 * (1 + MAX_ID) + 1;

inline bool is_id(std::string_view s) {
	if (s.empty() || s.size() > MAX_ID) return false;
	for (char c : s)
		if (!((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '.' || c == '/' || c == '_' || c == '-'))
			return false;
	return true;
}

inline std::string header_text(std::string_view fmt, std::string_view compiler, std::string_view semantics) {
	std::string h(ENVELOPE);
	for (auto part : {fmt, compiler, semantics}) { h.push_back(' '); h.append(part); }
	h.push_back('\n');
	return h;
}

struct header_t { std::string format, compiler, semantics; size_t size = 0; };

/// The header of saved bytes; nullopt unless they start with exactly one
/// header line. size is the header's length, newline included.
inline std::optional<header_t> header_of(std::string_view data) {
	size_t end = data.substr(0, MAX_HEADER).find('\n');
	if (end == std::string_view::npos) return std::nullopt;
	std::string_view line = data.substr(0, end);
	std::vector<std::string_view> parts;
	for (size_t from = 0;;) {
		size_t sp = line.find(' ', from);
		parts.push_back(line.substr(from, sp == std::string_view::npos ? std::string_view::npos : sp - from));
		if (sp == std::string_view::npos) break;
		from = sp + 1;
	}
	if (parts.size() != 4 || parts[0] != ENVELOPE) return std::nullopt;
	for (size_t k = 1; k < 4; ++k) if (!is_id(parts[k])) return std::nullopt;
	return header_t{std::string(parts[1]), std::string(parts[2]), std::string(parts[3]), end + 1};
}

// --------------------------------------------------------------------------
// The plan: an artifact decoded once
// --------------------------------------------------------------------------

struct info_t {
	std::vector<std::pair<int64_t, int64_t>> inputs;   // (index, width) in table order
	int64_t out_idx = 0, out_w = 0;
	std::vector<op> ops;
	std::vector<int64_t> weights;
	int64_t root = 0, step_cost = 0;
	std::map<stream_key, int64_t> table;
	std::vector<std::tuple<char, int64_t, int64_t>> streams;
};

// Compact runtime representation; op and info_t are still used by admission
// and the canonical encoders. Neither the wire format nor the tariff changes.
enum class rk : uint8_t { in, k, cmp, neg, conj, disj, sel };
inline constexpr const char* runtime_kinds[] = {"in", "k", "cmp", "not", "and", "or", "sel"};
inline constexpr std::string_view runtime_rels[] = {"=", "!=", "<", "<=", ">", ">="};

struct runtime_op {
	uint32_t a = 0, b = 0, c = 0;
	uint16_t literal_size = 0;
	uint8_t weight = 1;
	rk kind = rk::in;
};
static_assert(sizeof(runtime_op) == 16);
// The narrowing in runtime_of, against the limits admission and both
// decoders enforce: stream indices under 10^9 (the parser takes 8 digits),
// operand references under the operation count, literals of at most
// MAX_WIDTH / 4 hex digits, weights of at most limbs(MAX_WIDTH) limbs.
static_assert(1000000000 <= UINT32_MAX && MAX_ARTIFACT_BYTES <= UINT32_MAX);
static_assert(MAX_WIDTH / 4 <= UINT16_MAX && (MAX_WIDTH + 63) / 64 <= UINT8_MAX);

struct runtime_info {
	std::vector<std::pair<int64_t, int64_t>> inputs;
	int64_t out_idx = 0, out_w = 0;
	std::vector<runtime_op> ops;
	std::string literals;
	int64_t root = 0, step_cost = 0;
	std::map<stream_key, int64_t> table;
	std::vector<std::tuple<char, int64_t, int64_t>> streams;
};

/// Called only on checked info. 30-bit input IDs, references within the
/// admitted operation count, <=256 chars per checked literal, and <=16
/// charged limbs all fit. Reserve the exact total before appending.
inline runtime_info runtime_of(info_t&& old) {
	runtime_info r;
	r.inputs = std::move(old.inputs);
	r.out_idx = old.out_idx;
	r.out_w = old.out_w;
	r.root = old.root;
	r.step_cost = old.step_cost;
	r.table = std::move(old.table);
	r.streams = std::move(old.streams);
	r.ops.reserve(old.ops.size());
	size_t n = 0;
	for (const auto& o : old.ops) n += o.hex.size();
	r.literals.reserve(n);
	for (size_t i = 0; i != old.ops.size(); ++i) {
		const auto& o = old.ops[i];
		runtime_op q;
		if (o.kind == "in") { q.kind = rk::in; q.a = (uint32_t)o.i; }
		else if (o.kind == "k") {
			q.kind = rk::k;
			q.a = (uint32_t)r.literals.size();
			q.literal_size = (uint16_t)o.hex.size();
			r.literals.append(o.hex);
		} else {
			q.a = (uint32_t)o.a; q.b = (uint32_t)o.b; q.c = (uint32_t)o.c;
			if (o.kind == "cmp") {
				q.kind = rk::cmp;
				for (uint32_t k = 0; k != 6; ++k)
					if (o.rel == runtime_rels[k]) { q.c = k; break; }
			}
			else if (o.kind == "not") q.kind = rk::neg;
			else if (o.kind == "and") q.kind = rk::conj;
			else if (o.kind == "or") q.kind = rk::disj;
			else q.kind = rk::sel;
		}
		q.weight = (uint8_t)old.weights[i];
		r.ops.push_back(q);
	}
	return r;
}

struct plan {
	std::string digest;                                 // SHA-256 of the saved bytes
	std::string format;                                 // tba1, varint or packed
	std::string saved;                                  // the saved bytes: header, then body
	runtime_info info;
};
using plan_ptr = std::shared_ptr<const plan>;

namespace detail {

inline size_t depth(const std::string& s) {
	size_t d = 0, best = 0;
	bool in = false, esc = false;
	for (char c : s) {
		if (in) { if (esc) esc = false; else if (c == '\\') esc = true; else if (c == '"') in = false; }
		else if (c == '"') in = true;
		else if (c == '[' || c == '{') best = std::max(best, ++d);
		else if (c == ']' || c == '}') { if (d) --d; }
	}
	return best;
}

/// Python's int(j) for a JSON integer or boolean, when it fits.
inline std::optional<int64_t> num(const json& j) {
	if (j.t == json::type::boolean) return j.b ? 1 : 0;
	return j.i64();
}
inline bool strict_int(const json& j) { return j.t == json::type::integer; }
inline bool str_is(const json& j, const char* s) { std::string v; return j.ascii(v) && v == s; }

} // namespace detail

/// core.decode_artifact with the strict rules of /2: nesting past
/// MAX_ARTIFACT_DEPTH is not_json, a real anywhere is not_canonical, and
/// `out` must hold integers. The reason of a malformed artifact, or nullopt.
inline std::optional<info_t> decode(const std::string& artifact, std::string& why) {
	auto bad = [&](const char* w) { why = w; return std::optional<info_t>{}; };
	if (detail::depth(artifact) > MAX_ARTIFACT_DEPTH) return bad("not_json");
	auto pj = ga1_wire::parse_json(artifact);
	if (!pj) return bad("not_json");
	if (pj->contains_number()) {
		std::function<bool(const json&)> real = [&](const json& j) {
			if (j.t == json::type::real) return true;
			for (auto& x : j.a) if (real(x)) return true;
			for (auto& kv : j.o) if (real(kv.second)) return true;
			return false;
		};
		if (real(*pj)) return bad("not_canonical");
	}
	const json& art = *pj;
	if (!art.is_obj() || ga1_wire::canon(art) != artifact) return bad("not_canonical");
	static const std::set<std::string> fields{"fmt", "strategy", "streams", "out", "ops", "root"};
	std::set<std::string> have;
	for (auto& kv : art.o) have.insert(std::string(kv.first.begin(), kv.first.end()));
	if (have != fields) return bad("fields");
	if (!detail::str_is(*art.get("fmt"), "TBA1")
			|| !(detail::str_is(*art.get("strategy"), "tree") || detail::str_is(*art.get("strategy"), "dag")))
		return bad("header");
	const json& streams = *art.get("streams");
	const json& ops = *art.get("ops");
	const json& out = *art.get("out");
	if (!streams.is_arr() || !ops.is_arr()) return bad("fields");
	info_t info;
	std::vector<stream_key> order;
	for (auto& e : streams.a) {
		if (!e.is_arr() || e.a.size() != 3 || !(detail::str_is(e.a[0], "i") || detail::str_is(e.a[0], "o"))
				|| !detail::strict_int(e.a[1]) || !detail::strict_int(e.a[2]))
			return bad("stream");
		auto idx = e.a[1].i64(), w = e.a[2].i64();
		if (!idx || *idx < 0 || *idx >= 1000000000 || !w || *w < 1 || *w > MAX_WIDTH) return bad("stream");
		stream_key k{detail::str_is(e.a[0], "i") ? 'i' : 'o', *idx};
		order.push_back(k);
		info.table[k] = *w;
	}
	{
		std::vector<stream_key> sorted(order.begin(), order.end());
		std::sort(sorted.begin(), sorted.end());
		sorted.erase(std::unique(sorted.begin(), sorted.end()), sorted.end());
		if (sorted != order) return bad("stream_order");
	}
	std::vector<stream_key> outs;
	for (auto& k : order) if (k.first == 'o') outs.push_back(k);
	if (outs.size() != 1 || !out.is_arr() || out.a.size() != 3 || !detail::str_is(out.a[0], "o"))
		return bad("out");
	{
		auto o1 = detail::num(out.a[1]), o2 = detail::num(out.a[2]);
		if (!o1 || !o2 || *o1 != outs[0].second || *o2 != info.table[outs[0]]) return bad("out");
	}
	std::vector<int64_t> types;              // width, or 0 for a Boolean
	std::set<int64_t> used;
	for (size_t i = 0; i < ops.a.size(); ++i) {
		const json& o = ops.a[i];
		if (!o.is_arr() || o.a.empty() || !o.a[0].is_str()) return bad("op");
		std::string kind;
		if (!o.a[0].ascii(kind)) kind.clear();
		size_t na = o.a.size() - 1;
		const json* args = o.a.data() + 1;
		std::string fail_why;
		auto ref = [&](const json& j, char want) -> std::optional<int64_t> {
			auto v = detail::strict_int(j) ? j.i64() : std::nullopt;
			if (!v || *v < 0 || *v >= (int64_t)i) { fail_why = "operand"; return std::nullopt; }
			if (want == 'b' && types[(size_t)*v] != 0) { fail_why = "operand_type"; return std::nullopt; }
			if (want == 'v' && types[(size_t)*v] == 0) { fail_why = "operand_type"; return std::nullopt; }
			return types[(size_t)*v];
		};
		op x; x.kind = kind;
		auto in_idx = (kind == "in" && na == 1 && detail::strict_int(args[0])) ? args[0].i64() : std::nullopt;
		if (kind == "in" && na == 1 && detail::strict_int(args[0]) && in_idx && info.table.count({'i', *in_idx})) {
			int64_t w = info.table[{'i', *in_idx}];
			used.insert(*in_idx);
			types.push_back(w);
			info.weights.push_back(limbs(w));
			x.i = *in_idx;
		} else if (kind == "k" && na == 2) {
			std::string digits;
			auto w = detail::strict_int(args[0]) ? args[0].i64() : std::nullopt;
			if (!w || *w < 1 || *w > MAX_WIDTH || !args[1].ascii(digits) || !args[1].is_str()
					|| (int64_t)digits.size() != hex_digits(*w) || !is_lower_hex(digits) || bit_length(digits) > *w)
				return bad("constant");
			types.push_back(*w);
			info.weights.push_back(limbs(*w));
			x.w = *w; x.hex = digits;
		} else if (kind == "cmp" && na == 3 && (detail::str_is(args[0], "=") || detail::str_is(args[0], "!=")
				|| detail::str_is(args[0], "<") || detail::str_is(args[0], "<=") || detail::str_is(args[0], ">")
				|| detail::str_is(args[0], ">="))) {
			auto ta = ref(args[1], 'v'); if (!ta) { why = fail_why; return std::nullopt; }
			auto tb = ref(args[2], 'v'); if (!tb) { why = fail_why; return std::nullopt; }
			if (*ta != *tb) return bad("operand_width");
			types.push_back(0);
			info.weights.push_back(limbs(*ta));
			args[0].ascii(x.rel); x.a = *args[1].i64(); x.b = *args[2].i64();
		} else if (kind == "not" && na == 1) {
			if (!ref(args[0], 'b')) { why = fail_why; return std::nullopt; }
			types.push_back(0); info.weights.push_back(1);
			x.a = *args[0].i64();
		} else if ((kind == "and" || kind == "or") && na == 2) {
			if (!ref(args[0], 'b')) { why = fail_why; return std::nullopt; }
			if (!ref(args[1], 'b')) { why = fail_why; return std::nullopt; }
			types.push_back(0); info.weights.push_back(1);
			x.a = *args[0].i64(); x.b = *args[1].i64();
		} else if (kind == "sel" && na == 3) {
			if (!ref(args[0], 'b')) { why = fail_why; return std::nullopt; }
			auto ta = ref(args[1], 'v'); if (!ta) { why = fail_why; return std::nullopt; }
			auto tb = ref(args[2], 'v'); if (!tb) { why = fail_why; return std::nullopt; }
			if (*ta != *tb) return bad("operand_width");
			types.push_back(*ta); info.weights.push_back(limbs(*ta));
			x.a = *args[0].i64(); x.b = *args[1].i64(); x.c = *args[2].i64();
		} else return bad("op");
		info.ops.push_back(std::move(x));
	}
	const json& root = *art.get("root");
	auto r = detail::strict_int(root) ? root.i64() : std::nullopt;
	auto ow = detail::num(out.a[2]);
	if (!r || *r < 0 || *r >= (int64_t)ops.a.size() || types[(size_t)*r] != *ow) return bad("root");
	for (auto& k : order) if (k.first == 'i') info.inputs.push_back({k.second, info.table[k]});
	std::set<int64_t> declared;
	for (auto& [idx, w] : info.inputs) declared.insert(idx);
	if (used != declared) return bad("stream_use");
	if (!detail::strict_int(out.a[1]) || !detail::strict_int(out.a[2])) return bad("out");
	info.root = *r;
	info.out_idx = outs[0].second;
	info.out_w = info.table[outs[0]];
	for (auto& k : order) info.streams.emplace_back(k.first, k.second, info.table[k]);
	int64_t cost = std::max<int64_t>(1, (int64_t)info.inputs.size()) + limbs(info.out_w) + 1;
	for (auto& [idx, w] : info.inputs) cost += limbs(w);
	for (auto w : info.weights) cost += w;
	info.step_cost = cost;
	return info;
}

// --------------------------------------------------------------------------
// Operations
// --------------------------------------------------------------------------

inline json value_of(const plan& p) {
	json v = json::object();
	v.set("artifact_sha256", json::str(p.digest));
	auto h = header_of(p.saved);
	v.set("compiler", json::str(h ? h->compiler : std::string()));
	v.set("step_cost", json::integer(p.info.step_cost));
	json reads = json::array();
	for (auto& [idx, w] : p.info.inputs)
		reads.a.push_back(json::array({json::str("i" + std::to_string(idx)), json::integer(w), json::integer(0)}));
	v.set("reads", reads);
	v.set("writes", json::array({json::str("o" + std::to_string(p.info.out_idx)), json::integer(p.info.out_w)}));
	return v;
}

// --------------------------------------------------------------------------
// Typing a lowered rule: the checks core.decode_artifact makes, straight on
// the instructions (no JSON).
// --------------------------------------------------------------------------

inline std::optional<info_t> info_of(const lowered& L, std::string& why) {
	auto bad = [&](const char* w) { why = w; return std::optional<info_t>{}; };
	info_t info;
	info.table = L.table;
	for (auto& [k, w] : L.table) {
		info.streams.emplace_back(k.first, k.second, w);
		if (k.first == 'i') info.inputs.push_back({k.second, w});
	}
	info.out_idx = L.out;
	info.out_w = L.table.at({'o', L.out});
	std::vector<int64_t> types;
	std::set<int64_t> used;
	for (size_t i = 0; i < L.ops.size(); ++i) {
		const op& o = L.ops[i];
		auto ty = [&](int64_t j) { return types[(size_t)j]; };
		if (o.kind == "in") {
			int64_t w = L.table.at({'i', o.i});
			used.insert(o.i); types.push_back(w); info.weights.push_back(limbs(w));
		} else if (o.kind == "k") {
			types.push_back(o.w); info.weights.push_back(limbs(o.w));
		} else if (o.kind == "cmp") {
			if (ty(o.a) == 0 || ty(o.b) == 0) return bad("operand_type");
			if (ty(o.a) != ty(o.b)) return bad("operand_width");
			types.push_back(0); info.weights.push_back(limbs(ty(o.a)));
		} else if (o.kind == "not") {
			if (ty(o.a) != 0) return bad("operand_type");
			types.push_back(0); info.weights.push_back(1);
		} else if (o.kind == "and" || o.kind == "or") {
			if (ty(o.a) != 0 || ty(o.b) != 0) return bad("operand_type");
			types.push_back(0); info.weights.push_back(1);
		} else {
			if (ty(o.a) != 0 || ty(o.b) == 0 || ty(o.c) == 0) return bad("operand_type");
			if (ty(o.b) != ty(o.c)) return bad("operand_width");
			types.push_back(ty(o.b)); info.weights.push_back(limbs(ty(o.b)));
		}
		info.ops.push_back(o);
	}
	if (types[(size_t)L.root] != info.out_w) return bad("root");
	std::set<int64_t> declared;
	for (auto& [idx, w] : info.inputs) declared.insert(idx);
	if (used != declared) return bad("stream_use");
	info.root = L.root;
	int64_t cost = std::max<int64_t>(1, (int64_t)info.inputs.size()) + limbs(info.out_w) + 1;
	for (auto& [idx, w] : info.inputs) cost += limbs(w);
	for (auto w : info.weights) cost += w;
	info.step_cost = cost;
	return info;
}

// --------------------------------------------------------------------------
// Typed saved forms: varint and packed
// --------------------------------------------------------------------------

inline constexpr unsigned char TAG_VARINT = 0x11, TAG_PACKED = 0x12;
inline const char* const KINDS[7] = {"in", "k", "cmp", "not", "and", "or", "sel"};
inline const char* const RELS[6] = {"=", "!=", "<", "<=", ">", ">="};
inline int kind_code(const std::string& k) { for (int c = 0; c < 7; ++c) if (k == KINDS[c]) return c; return -1; }
inline int rel_code(const std::string& r) { for (int c = 0; c < 6; ++c) if (r == RELS[c]) return c; return -1; }
inline int arity(const std::string& k) { return k == "not" ? 1 : (k == "cmp" || k == "and" || k == "or") ? 2 : 3; }
inline int64_t clog2(int64_t n) { int64_t b = 0; if (n <= 1) return 0; --n; while (n) { ++b; n >>= 1; } return b; }

namespace detail {

inline std::vector<int64_t> refs(const op& o) {
	if (o.kind == "not") return {o.a};
	if (o.kind == "cmp" || o.kind == "and" || o.kind == "or") return {o.a, o.b};
	return {o.a, o.b, o.c};
}
inline int hexv(char c) { return c <= '9' ? c - '0' : c - 'a' + 10; }

/// The big-endian bytes of a padded hex constant of width w: ceil(w / 8) bytes.
inline std::string const_bytes(const std::string& hex, int64_t w) {
	size_t nb = (size_t)((w + 7) / 8);
	std::string h = std::string(nb * 2 > hex.size() ? nb * 2 - hex.size() : 0, '0') + hex;
	std::string out;
	for (size_t k = 0; k < nb; ++k) out.push_back((char)(hexv(h[2 * k]) * 16 + hexv(h[2 * k + 1])));
	return out;
}

inline void uv(std::string& out, uint64_t n) {
	for (;;) {
		unsigned char b = n & 0x7F;
		n >>= 7;
		if (n) out.push_back((char)(b | 0x80));
		else { out.push_back((char)b); return; }
	}
}

struct bits_out {
	std::string bytes;
	unsigned char cur = 0;
	int n = 0;
	void bit(unsigned v) { cur = (unsigned char)((cur << 1) | (v & 1)); if (++n == 8) { bytes.push_back((char)cur); cur = 0; n = 0; } }
	void put(uint64_t x, int64_t k) { for (int64_t j = k - 1; j >= 0; --j) bit((unsigned)((x >> j) & 1)); }
	void small(uint64_t x) { int64_t L = 0; for (uint64_t y = x; y; y >>= 1) ++L; put((uint64_t)L, 5); put(x, L); }
	void hex(const std::string& h, int64_t w) {             // w bits of a padded hex value
		int64_t skip = (int64_t)h.size() * 4 - w;
		for (size_t k = 0; k < h.size(); ++k) {
			int v = hexv(h[k]);
			for (int j = 3; j >= 0; --j) {
				if (skip > 0) { --skip; continue; }
				bit((unsigned)((v >> j) & 1));
			}
		}
	}
	std::string done() { while (n) bit(0); return bytes; }
};

} // namespace detail

inline std::string encode_varint(const info_t& I, const std::string& strategy) {
	std::string b;
	b.push_back((char)TAG_VARINT);
	b.push_back((char)(strategy == "tree" ? 0 : 1));
	std::map<int64_t, int64_t> pos;
	detail::uv(b, I.inputs.size());
	int64_t prev = -1;
	for (size_t k = 0; k < I.inputs.size(); ++k) {
		auto [idx, w] = I.inputs[k];
		pos[idx] = (int64_t)k;
		detail::uv(b, (uint64_t)(prev >= 0 ? idx - prev - 1 : idx));
		detail::uv(b, (uint64_t)(w - 1));
		prev = idx;
	}
	detail::uv(b, (uint64_t)I.out_idx);
	detail::uv(b, (uint64_t)(I.out_w - 1));
	detail::uv(b, I.ops.size());
	for (size_t i = 0; i < I.ops.size(); ++i) {
		const op& o = I.ops[i];
		b.push_back((char)(kind_code(o.kind) | (o.kind == "cmp" ? rel_code(o.rel) << 3 : 0)));
		if (o.kind == "in") detail::uv(b, (uint64_t)pos[o.i]);
		else if (o.kind == "k") { detail::uv(b, (uint64_t)(o.w - 1)); b += detail::const_bytes(o.hex, o.w); }
		else for (auto r : detail::refs(o)) detail::uv(b, (uint64_t)((int64_t)i - 1 - r));
	}
	detail::uv(b, (uint64_t)((int64_t)I.ops.size() - 1 - I.root));
	return b;
}

inline std::string encode_packed(const info_t& I, const std::string& strategy) {
	detail::bits_out o;
	o.put(strategy == "tree" ? 0 : 1, 1);
	std::map<int64_t, int64_t> pos;
	o.small(I.inputs.size());
	int64_t prev = -1;
	for (size_t k = 0; k < I.inputs.size(); ++k) {
		auto [idx, w] = I.inputs[k];
		pos[idx] = (int64_t)k;
		o.small((uint64_t)(prev >= 0 ? idx - prev - 1 : idx));
		o.put((uint64_t)(w - 1), 10);
		prev = idx;
	}
	o.small((uint64_t)I.out_idx);
	o.put((uint64_t)(I.out_w - 1), 10);
	o.small(I.ops.size() - 1);
	int64_t n_in = (int64_t)I.inputs.size();
	for (size_t i = 0; i < I.ops.size(); ++i) {
		const op& x = I.ops[i];
		o.put((uint64_t)kind_code(x.kind), 3);
		if (x.kind == "cmp") o.put((uint64_t)rel_code(x.rel), 3);
		if (x.kind == "in") o.put((uint64_t)pos[x.i], clog2(n_in));
		else if (x.kind == "k") { o.put((uint64_t)(x.w - 1), 10); o.hex(x.hex, x.w); }
		else for (auto r : detail::refs(x)) o.put((uint64_t)((int64_t)i - 1 - r), clog2((int64_t)i));
	}
	o.put((uint64_t)((int64_t)I.ops.size() - 1 - I.root), clog2((int64_t)I.ops.size()));
	return std::string(1, (char)TAG_PACKED) + o.done();
}

namespace detail {

struct byte_reader {
	const std::string& d;
	size_t p = 0;
	std::string why;
	bool fail(const char* w) { if (why.empty()) why = w; return false; }
	bool u8(unsigned& v) { if (p >= d.size()) return fail("truncated"); v = (unsigned char)d[p++]; return true; }
	bool uv(uint64_t limit, uint64_t& n) {
		n = 0;
		unsigned shift = 0;
		for (;;) {
			unsigned c;
			if (!u8(c)) return false;
			if (shift >= 63) return fail("varint");
			n |= (uint64_t)(c & 0x7F) << shift;
			shift += 7;
			if (!(c & 0x80)) {
				if (c == 0 && shift > 7) return fail("varint");
				if (n > limit) return fail("range");
				return true;
			}
		}
	}
};

struct bit_reader {
	const std::string& d;
	size_t p = 0, total;                                 // bits, after the tag byte
	std::string why;
	explicit bit_reader(const std::string& s) : d(s), total(s.size() > 0 ? (s.size() - 1) * 8 : 0) {}
	bool fail(const char* w) { if (why.empty()) why = w; return false; }
	bool get(int64_t k, uint64_t& v) {
		if (p + (size_t)k > total) return fail("truncated");
		v = 0;
		for (int64_t j = 0; j < k; ++j, ++p) v = (v << 1) | (((unsigned char)d[1 + (p >> 3)] >> (7 - (p & 7))) & 1);
		return true;
	}
	bool small(uint64_t limit, uint64_t& v) {
		uint64_t L;
		if (!get(5, L) || !get((int64_t)L, v)) return false;
		if (L && !(v >> (L - 1))) return fail("small");
		if (v > limit) return fail("range");
		return true;
	}
	bool hex_append(int64_t w, char* h) {                 // w bits as a padded hex value
		if (p + (size_t)w > total) return fail("truncated");
		int64_t nd = hex_digits(w), lead = nd * 4 - w;
		int v = 0, n = (int)lead;
		for (int64_t j = 0; j < w; ++j, ++p) {
			v = (v << 1) | (((unsigned char)d[1 + (p >> 3)] >> (7 - (p & 7))) & 1);
			if (++n == 4) { *h++ = "0123456789abcdef"[v]; v = 0; n = 0; }
		}
		return true;
	}
};

} // namespace detail


namespace detail {

inline unsigned runtime_arity(unsigned k) { return k == 3 ? 1 : k == 6 ? 3 : 2; }

struct typed_runtime_builder {
	runtime_info info;
	std::vector<uint16_t> types;
	std::set<uint32_t> used;
	const char* semantic = nullptr;
	int64_t eval_cost = 0;

	void start_ops(int64_t idx, int64_t width, uint64_t count, size_t wire) {
		info.out_idx = idx;
		info.out_w = width;
		// Don't reserve from the declaration alone. A typed op uses at
		// least three bits, so the reservation is bounded by actual bytes.
		const size_t cap = std::min(size_t(count), wire * 8 / 3);
		info.ops.reserve(cap);
		types.reserve(cap);
	}

	void literal_header(runtime_op& o, int64_t w) {
		const size_t digits = (size_t)hex_digits(w);
		o.a = (uint32_t)info.literals.size();
		o.literal_size = (uint16_t)digits;
	}

	// A wire failure (including trailing/padding) wins over every type
	// refusal. Remember the first semantic failure, but keep parsing.
	void add(runtime_op o, int64_t w) {
		if (semantic) return;
		int64_t weight = 1;
		switch (o.kind) {
		case rk::in: used.insert(o.a); weight = limbs(w); break;
		case rk::k: weight = limbs(w); break;
		case rk::cmp:
			if (!types[o.a] || !types[o.b]) { semantic = "operand_type"; return; }
			if (types[o.a] != types[o.b]) { semantic = "operand_width"; return; }
			weight = limbs(types[o.a]); break;
		case rk::neg:
			if (types[o.a]) { semantic = "operand_type"; return; }
			break;
		case rk::conj: case rk::disj:
			if (types[o.a] || types[o.b]) { semantic = "operand_type"; return; }
			break;
		case rk::sel:
			if (types[o.a] || !types[o.b] || !types[o.c]) { semantic = "operand_type"; return; }
			if (types[o.b] != types[o.c]) { semantic = "operand_width"; return; }
			w = types[o.b]; weight = limbs(w); break;
		}
		types.push_back((uint16_t)w);
		o.weight = (uint8_t)weight;
		eval_cost += weight;
		info.ops.push_back(o);
	}

	std::optional<runtime_info> finish(std::string& why) {
		if (semantic) { why = semantic; return {}; }
		if (types[(size_t)info.root] != info.out_w) { why = "root"; return {}; }
		// Each used index came from the wire's sorted, distinct inputs.
		if (used.size() != info.inputs.size()) { why = "stream_use"; return {}; }
		info.step_cost = eval_cost + std::max<int64_t>(1, (int64_t)info.inputs.size()) + limbs(info.out_w) + 1;
		info.streams.reserve(info.inputs.size() + 1);
		for (auto [i, w] : info.inputs) {
			info.streams.emplace_back('i', i, w);
			info.table.emplace(stream_key{'i', i}, w);
			info.step_cost += limbs(w);
		}
		info.streams.emplace_back('o', info.out_idx, info.out_w);
		info.table.emplace(stream_key{'o', info.out_idx}, info.out_w);
		return std::move(info);
	}
};

} // namespace detail

/// The rule in a typed saved form, typed and checked; nullopt with the reason.
inline std::optional<runtime_info> decode_typed_runtime(const std::string& data, const std::string& fmt, std::string& why,
	std::string& strategy)
{
	detail::typed_runtime_builder b;
	auto& inputs = b.info.inputs;
	auto set_why = [&](const std::string& w) { why = w; return std::optional<runtime_info>{}; };
	if (fmt == "varint") {
		detail::byte_reader r{data};
		unsigned tag, s;
		uint64_t n_in, n_ops, v, w;
		if (!r.u8(tag)) return set_why(r.why);
		if (tag != TAG_VARINT) return set_why("tag");
		if (!r.u8(s)) return set_why(r.why);
		if (s > 1) return set_why("strategy");
		if (!r.uv((uint64_t)MAX_ARTIFACT_BYTES, n_in)) return set_why(r.why);
		int64_t prev = -1;
		for (uint64_t k = 0; k < n_in; ++k) {
			if (!r.uv(1000000000, v) || !r.uv((uint64_t)MAX_WIDTH - 1, w)) return set_why(r.why);
			int64_t idx = prev < 0 ? (int64_t)v : prev + 1 + (int64_t)v;
			if (idx >= 1000000000) return set_why("range");
			inputs.push_back({idx, (int64_t)w + 1});
			prev = idx;
		}
		uint64_t oi, ow;
		if (!r.uv(1000000000 - 1, oi) || !r.uv((uint64_t)MAX_WIDTH - 1, ow) || !r.uv((uint64_t)MAX_ARTIFACT_BYTES, n_ops))
			return set_why(r.why);
		if (n_ops < 1) return set_why("range");
		b.start_ops((int64_t)oi, (int64_t)ow + 1, n_ops, data.size());
		for (uint64_t i = 0; i < n_ops; ++i) {
			unsigned head;
			if (!r.u8(head)) return set_why(r.why);
			unsigned kc = head & 7, rel = head >> 3;
			if (kc >= 7 || (rel && kc != 2) || rel >= 6) return set_why("op");
			runtime_op o; o.kind = (rk)kc;
			int64_t width = 0;
			if (kc == 0) {
				if (!n_in) return set_why("op");
				if (!r.uv(n_in - 1, v)) return set_why(r.why);
				o.a = (uint32_t)inputs[(size_t)v].first;
				width = inputs[(size_t)v].second;
			} else if (kc == 1) {
				if (!r.uv((uint64_t)MAX_WIDTH - 1, w)) return set_why(r.why);
				width = (int64_t)w + 1;
				size_t nb = (size_t)((width + 7) / 8);
				if (r.p + nb > data.size()) return set_why("truncated");
				const unsigned char* literal = (const unsigned char*)data.data() + r.p;
				r.p += nb;
				int64_t extra = (int64_t)nb * 8 - width;          // high bits that must be zero
				if (extra > 0 && (literal[0] >> (8 - extra))) return set_why("constant");
				b.literal_header(o, width);
				static constexpr char h[] = "0123456789abcdef";
				std::array<char, MAX_WIDTH / 4> buf;
				size_t pos=0;
				for (size_t j = 2 * nb - o.literal_size; j < 2 * nb; ++j)
					buf[pos++] = h[(literal[j / 2] >> ((j % 2) ? 0 : 4)) & 15];
				b.info.literals.append(buf.data(), o.literal_size);
			} else {
				if (i == 0) return set_why("operand");
				uint32_t* rs[3] = {&o.a, &o.b, &o.c};
				for (int k = 0; k < detail::runtime_arity(kc); ++k) {
					if (!r.uv(i - 1, v)) return set_why(r.why);
					*rs[k] = (uint32_t)(i - 1 - v);
				}
				if (kc == 2) o.c = (uint32_t)rel;
			}
			b.add(o, width);
		}
		if (!r.uv(n_ops - 1, v)) return set_why(r.why);
		b.info.root = (int64_t)n_ops - 1 - (int64_t)v;
		if (r.p != data.size()) return set_why("trailing");
		strategy = s ? "dag" : "tree";
	} else {
		if (data.empty() || (unsigned char)data[0] != TAG_PACKED) return set_why("tag");
		detail::bit_reader r(data);
		uint64_t s, n_in, v, w, n_ops;
		if (!r.get(1, s) || !r.small((uint64_t)MAX_ARTIFACT_BYTES, n_in)) return set_why(r.why);
		int64_t prev = -1;
		for (uint64_t k = 0; k < n_in; ++k) {
			if (!r.small(1000000000, v)) return set_why(r.why);
			int64_t idx = prev < 0 ? (int64_t)v : prev + 1 + (int64_t)v;
			if (idx >= 1000000000) return set_why("range");
			if (!r.get(10, w)) return set_why(r.why);
			inputs.push_back({idx, (int64_t)w + 1});
			prev = idx;
		}
		uint64_t oi, ow;
		if (!r.small(1000000000 - 1, oi) || !r.get(10, ow) || !r.small((uint64_t)MAX_ARTIFACT_BYTES, n_ops))
			return set_why(r.why);
		++n_ops;
		b.start_ops((int64_t)oi, (int64_t)ow + 1, n_ops, data.size());
		for (uint64_t i = 0; i < n_ops; ++i) {
			uint64_t kc, rel = 0;
			if (!r.get(3, kc)) return set_why(r.why);
			if (kc >= 7) return set_why("op");
			runtime_op o; o.kind = (rk)kc;
			int64_t width = 0;
			if (kc == 2) { if (!r.get(3, rel)) return set_why(r.why); }
			if (rel >= 6) return set_why("op");
			if (kc == 0) {
				if (!n_in) return set_why("op");
				if (!r.get(clog2((int64_t)n_in), v)) return set_why(r.why);
				if (v >= n_in) return set_why("range");
				o.a = (uint32_t)inputs[(size_t)v].first;
				width = inputs[(size_t)v].second;
			} else if (kc == 1) {
				if (!r.get(10, w)) return set_why(r.why);
				width = (int64_t)w + 1;
				b.literal_header(o, width);
				std::array<char, MAX_WIDTH / 4> buf;
				if (!r.hex_append(width, buf.data())) return set_why(r.why);
				b.info.literals.append(buf.data(), o.literal_size);
			} else {
				if (i == 0) return set_why("operand");
				uint32_t* rs[3] = {&o.a, &o.b, &o.c};
				for (int k = 0; k < detail::runtime_arity(kc); ++k) {
					if (!r.get(clog2((int64_t)i), v)) return set_why(r.why);
					if (v > i - 1) return set_why("range");
					*rs[k] = (uint32_t)(i - 1 - v);
				}
				if (kc == 2) o.c = (uint32_t)rel;
			}
			b.add(o, width);
		}
		if (!r.get(clog2((int64_t)n_ops), v)) return set_why(r.why);
		if (v > n_ops - 1) return set_why("range");
		b.info.root = (int64_t)n_ops - 1 - (int64_t)v;
		size_t pad = r.total - r.p;
		uint64_t z;
		if (pad >= 8 || !r.get((int64_t)pad, z) || z != 0) return set_why(r.why.empty() ? "padding" : r.why);
		strategy = s ? "dag" : "tree";
	}
	return b.finish(why);
}

// --------------------------------------------------------------------------
// Operations
// --------------------------------------------------------------------------

inline plan_ptr build_plan(const json& call, const std::string& fmt, meter& m, probe* seen, std::string_view compiler) {
	if (seen) ++seen->builds;
	auto L = build_lowered(call.get("text"), call.get("strategy"), m, compiler == MULTILINE_COMPILER);
	if (!L) return nullptr;
	std::string why;
	auto info = info_of(*L, why);                      // its own lowering, valid by construction
	auto p = std::make_shared<plan>();
	p->format = fmt;
	p->saved = header_text(fmt, compiler, SCHEMA)
		+ (fmt == "tba1" ? artifact_text(L->strategy, L->table, L->out, L->ops, L->root)
			: fmt == "varint" ? encode_varint(*info, L->strategy) : encode_packed(*info, L->strategy));
	if (!publish(p->saved, m)) return nullptr;
	if (seen) ++seen->artifact_hashes;
	p->digest = ga1_wire::sha256(p->saved);
	p->info = runtime_of(std::move(*info));
	return p;
}

inline bool hex64(const json* j) {
	std::string s;
	return j && j->ascii(s) && s.size() == 64 && is_lower_hex(s);
}

/// The saved bytes of a load call and their format: TBA1 text in "artifact",
/// or a typed form as lowercase hex in "bytes"; false if not admissible.
inline bool saved_bytes(const json& call, std::string& fmt, std::string& data) {
	fmt = "tba1";
	const json* fj = call.get("format");
	if (fj && !fj->ascii(fmt)) fmt = "?";
	const json* a = call.get("artifact");
	const json* h = call.get("bytes");
	if (fmt == "tba1" && !h) {
		if (!a || !a->is_str() || !a->asc || (int64_t)a->s.size() > MAX_ARTIFACT_BYTES) return false;
		a->ascii(data);
		return true;
	}
	if (!(fmt == "varint" || fmt == "packed") || a || !h || !h->is_str()
			|| !h->asc || h->s.size() % 2 || h->s.size() / 2 > MAX_ARTIFACT_BYTES)
		return false;
	const auto nibble = [](char16_t c) -> int {
		if (c >= u'0' && c <= u'9') return c - u'0';
		if (c >= u'a' && c <= u'f') return c - u'a' + 10;
		return -1;
	};
	data.clear();
	data.reserve(h->s.size() / 2);
	for (size_t k = 0; k < h->s.size(); k += 2) {
		const int hi = nibble(h->s[k]), lo = nibble(h->s[k + 1]);
		if (hi < 0 || lo < 0) return false;
		data.push_back(static_cast<char>(hi * 16 + lo));
	}
	return true;
}

/// Admit, scan (hash), bind, header, check (decode), commit: the bytes are
/// hashed and the binding checked before anything is parsed, and the header
/// is compared with the call (format, semantics, and the compiler if named)
/// before the body is decoded.
inline plan_ptr load_plan(const json& call, meter& m, probe* seen) {
	if (!m.charge("load.admit", (int64_t)0, "len", 1)) return nullptr;
	const json* bind = call.get("bind");
	const json* cj = call.get("compiler");
	std::string fmt, data, want_compiler;
	if (!saved_bytes(call, fmt, data) || (bind && !hex64(bind))
			|| (cj && !(cj->is_str() && cj->ascii(want_compiler) && is_id(want_compiler))))
		return m.unsupported("artifact_not_admissible", "load.admit"), nullptr;
	for (int64_t b = 0; b < blocks((int64_t)data.size()); ++b) if (!m.charge("load.scan", b, "block", 1)) return nullptr;
	if (seen) ++seen->artifact_hashes;
	std::string digest = ga1_wire::sha256(data);
	if (bind) {
		std::string want;
		bind->ascii(want);
		if (digest != want) return m.unsupported("artifact_not_bound", "load.scan"), nullptr;
	}
	auto h = header_of(data);
	if (!h) return m.unsupported("plan_malformed:header", "load.header"), nullptr;
	if (h->format != fmt) return m.unsupported("plan_header_mismatch:format", "load.header"), nullptr;
	if (h->semantics != SCHEMA) return m.unsupported("plan_header_mismatch:semantics", "load.header"), nullptr;
	if (cj && h->compiler != want_compiler) return m.unsupported("plan_header_mismatch:compiler", "load.header"), nullptr;
	for (int64_t b = 0; b < blocks((int64_t)data.size()); ++b) if (!m.charge("load.check", b, "block", 1)) return nullptr;
	if (seen) ++seen->artifact_decodes;
	std::string why, strategy;
	const std::string body = data.substr(h->size);
	std::optional<info_t> legacy;
	std::optional<runtime_info> direct;
	if (fmt == "tba1") legacy = decode(body, why);
	else direct = decode_typed_runtime(body, fmt, why, strategy);
	if (!legacy && !direct) return m.unsupported((fmt == "tba1" ? "artifact_malformed:" : "plan_malformed:") + why, "load.check"), nullptr;
	if (!m.charge("load.commit", (int64_t)0, "commit", 1)) return nullptr;
	auto p = std::make_shared<plan>();
	p->digest = std::move(digest);
	p->format = fmt;
	p->saved = std::move(data);
	p->info = direct ? std::move(*direct) : runtime_of(std::move(*legacy));
	if (direct && p->info.literals.capacity() - p->info.literals.size() >= 32)
		p->info.literals.shrink_to_fit();
	return p;
}

/// The state of a call, by its header: {plan, revision, time}.
struct state_in {
	bool ok = false;
	std::string name;
	int64_t revision = 0, time = 0;
	const plan* pl = nullptr;                          // the plan the host holds for it
};

inline state_in state_of(const json* s) {
	state_in st;
	if (!s || !s->is_obj() || s->o.size() != 3) return st;
	auto pj = s->get("plan"), rj = s->get("revision"), tj = s->get("time");
	if (!hex64(pj) || !rj || !tj || !rj->is_int() || !tj->is_int() || rj->negative() || tj->negative()) return st;
	auto r = rj->i64(), t = tj->i64();
	if (!r || !t || *r > MAX_COUNTER || *t > MAX_COUNTER) return st;
	pj->ascii(st.name);
	st.revision = *r; st.time = *t; st.ok = true;
	return st;
}

namespace detail {

inline bool cmp(std::string_view rel, const std::string& a, const std::string& b) {
	if (rel == "=") return a == b;
	if (rel == "!=") return a != b;
	if (rel == "<") return a < b;
	if (rel == "<=") return a <= b;
	if (rel == ">") return a > b;
	return a >= b;
}

struct reg { bool set = false, b = false; std::string v; };

inline void exec(const runtime_op& o, std::vector<reg>& r, size_t i,
	const std::map<int64_t, std::string>& values, const std::string& literals)
{
	reg& x = r[i]; x.set = true;
	switch (o.kind) {
	case rk::in: x.v = values.at(o.a); break;
	case rk::k: x.v.assign(literals.data() + o.a, o.literal_size); break;
	case rk::cmp: x.b = cmp(runtime_rels[o.c], r[o.a].v, r[o.b].v); break;
	case rk::neg: x.b = !r[o.a].b; break;
	case rk::conj: x.b = r[o.a].b && r[o.b].b; break;
	case rk::disj: x.b = r[o.a].b || r[o.b].b; break;
	case rk::sel: x = r[r[o.a].b ? o.b : o.c]; x.set = true; break;
	}
}

/// Same operand order, including cutoff cases, as the original.
inline int64_t next_needed(const runtime_op& o, const std::vector<reg>& r) {
	if (o.kind == rk::in || o.kind == rk::k) return -1;
	if (!r[o.a].set) return o.a;
	switch (o.kind) {
	case rk::cmp: return r[o.b].set ? -1 : (int64_t)o.b;
	case rk::conj: return !r[o.a].b || r[o.b].set ? -1 : (int64_t)o.b;
	case rk::disj: return r[o.a].b || r[o.b].set ? -1 : (int64_t)o.b;
	case rk::sel: { uint32_t j = r[o.a].b ? o.b : o.c; return r[j].set ? -1 : (int64_t)j; }
	default: return -1;
	}
}

} // namespace detail

struct stepped { json value; std::string output; };

inline std::optional<stepped> step(const plan& p, const state_in& st, const json* inputs, bool lazy, meter& m) {
	const runtime_info& info = p.info;
	if (!inputs || !inputs->is_obj()) return m.unsupported("input_noncanonical", "step.decode"), std::nullopt;
	GA1_TRY(m.charge("step.decode", (int64_t)0, "names", std::max<int64_t>(1, (int64_t)inputs->o.size())));
	std::set<std::u16string> names;
	for (auto& [idx, w] : info.inputs) { std::string n = "i" + std::to_string(idx); names.insert(std::u16string(n.begin(), n.end())); }
	for (auto& kv : inputs->o) if (!names.count(kv.first)) return m.unsupported("input_unexpected", "step.decode"), std::nullopt;
	std::map<int64_t, std::string> values;
	for (auto& [idx, w] : info.inputs) {
		std::string name = "i" + std::to_string(idx);
		GA1_TRY(m.charge("step.decode", name, "input", limbs(w)));
		const json* v = inputs->get(name);
		if (!v) return m.unsupported("input_missing", "step.decode", name), std::nullopt;
		std::string h;
		if (!v->ascii(h) || !v->is_str() || (int64_t)h.size() != hex_digits(w) || !is_lower_hex(h) || bit_length(h) > w)
			return m.unsupported("input_noncanonical", "step.decode", name), std::nullopt;
		values[idx] = h;
	}
	std::vector<detail::reg> r(info.ops.size());
	if (!lazy) {
		for (size_t i = 0; i < info.ops.size(); ++i) {
			GA1_TRY(m.charge("step.eval", (int64_t)i, runtime_kinds[(size_t)info.ops[i].kind], info.ops[i].weight));
			detail::exec(info.ops[i], r, i, values, info.literals);
		}
	} else {
		std::vector<int64_t> stack{info.root};
		while (!stack.empty()) {
			int64_t i = stack.back();
			if (r[(size_t)i].set) { stack.pop_back(); continue; }
			int64_t need = detail::next_needed(info.ops[(size_t)i], r);
			if (need >= 0) { stack.push_back(need); continue; }
			GA1_TRY(m.charge("step.eval", i, runtime_kinds[(size_t)info.ops[(size_t)i].kind], info.ops[(size_t)i].weight));
			detail::exec(info.ops[(size_t)i], r, (size_t)i, values, info.literals);
			stack.pop_back();
		}
	}
	std::string name = "o" + std::to_string(info.out_idx);
	GA1_TRY(m.charge("step.publish", name, "out", limbs(info.out_w)));
	GA1_TRY(m.charge("step.publish", (int64_t)0, "commit", 1));
	stepped s;
	s.output = r[(size_t)info.root].v;
	s.value = json::object();
	s.value.set("outputs", json::array({json::array({json::str(name), json::integer(st.time), json::str(s.output)})}));
	return s;
}

/// The result of a call: outcome, post-state (null for the pre-state), and
/// the plan a Completed get_interpreter, load or update made.
struct result_t { json outcome; std::optional<json> post; plan_ptr prepared; std::optional<stepped> step; };

inline json plan_state(const std::string& digest, int64_t revision, int64_t time) {
	json o = json::object();
	o.set("plan", json::str(digest));
	o.set("revision", json::integer(revision));
	o.set("time", json::integer(time));
	return o;
}

inline json completed(json value, int64_t used) {
	json o = json::object();
	o.set("status", json::str("Completed"));
	o.set("value", std::move(value));
	o.set("used", json::integer(used));
	return o;
}

inline result_t run(const json& call, const state_in& st, meter& m, probe* seen) {
	std::string schema, opn;
	auto scj = call.get("schema");
	if (!scj || !scj->ascii(schema) || schema != SCHEMA)
		return {ga1_wire::stopped_json("Unsupported", "schema_mismatch", "admit", {}, 0), {}, {}, {}};
	auto oj = call.get("op");
	if (oj) oj->ascii(opn);
	auto stop = [&] { return result_t{ga1_wire::outcome_of_stop(m), {}, {}, {}}; };
	if (opn == "get_interpreter") {
		std::string fmt = "tba1";
		if (auto fj = call.get("format"); fj && !(fj->ascii(fmt) && (fmt == "tba1" || fmt == "varint" || fmt == "packed"))) {
			if (!m.charge("build.admit", (int64_t)0, "len", 1)) return stop();
			m.unsupported("unknown_format", "build.admit");
			return stop();
		}
		std::string compiler = COMPILER;
		if (auto cj = call.get("compiler"); cj && !(cj->ascii(compiler) && known_compiler(compiler))) {
			if (!m.charge("build.admit", (int64_t)0, "len", 1)) return stop();
			m.unsupported("unknown_compiler", "build.admit");
			return stop();
		}
		auto p = build_plan(call, fmt, m, seen, compiler);
		if (!p) return stop();
		return {completed(value_of(*p), m.used), plan_state(p->digest, 0, 0), p, {}};
	}
	if (opn == "load") {
		auto p = load_plan(call, m, seen);
		if (!p) return stop();
		return {completed(value_of(*p), m.used), plan_state(p->digest, 0, 0), p, {}};
	}
	if (opn == "step" || opn == "update") {
		if (!st.ok) return {ga1_wire::stopped_json("Unsupported", "state_not_admissible", opn + ".admit", {}, 0), {}, {}, {}};
		if (!st.pl || st.pl->digest != st.name) { m.not_held(st.name); return stop(); }
		if (opn == "step") {
			std::string ev = "eager";
			if (auto e = call.get("eval"); e && !(e->ascii(ev) && (ev == "eager" || ev == "lazy")))
				return {ga1_wire::stopped_json("Unsupported", "unknown_evaluation", "step.admit", {}, 0), {}, {}, {}};
			auto s = step(*st.pl, st, call.get("inputs"), ev == "lazy", m);
			if (!s) return stop();
			return {completed(s->value, m.used), plan_state(st.name, st.revision, st.time + 1), {}, s};
		}
		auto old_header = header_of(st.pl->saved);
		if (!old_header || !known_compiler(old_header->compiler))
			return {ga1_wire::stopped_json("Unsupported", "unknown_compiler", "build.admit", {}, 0), {}, {}, {}};
		auto np = build_plan(call, st.pl->format, m, seen, old_header->compiler);
		if (!np) return stop();
		const runtime_info& old = st.pl->info;
		for (auto& [io, idx, w] : np->info.streams) {
			std::string site = std::string(1, io) + std::to_string(idx);
			if (!m.charge("update.revise", site, "stream", 1)) return stop();
			auto it = old.table.find({io, idx});
			if (it != old.table.end() && it->second != w) { m.unsupported("stream_width_conflict", "update.revise", site); return stop(); }
			if (io == 'o' && idx != old.out_idx) { m.unsupported("output_stream_change", "update.revise", site); return stop(); }
		}
		if (!m.charge("update.commit", (int64_t)0, "commit", 1)) return stop();
		json v = value_of(*np);
		v.set("accepted", json::boolean(true));
		return {completed(v, m.used), plan_state(np->digest, st.revision + 1, st.time), np, {}};
	}
	m.unsupported("unknown_operation", "admit");
	return stop();
}

/// The contract function: a call and the state the host attached.
inline result_t transition(const json& call, const state_in& st, probe* seen = nullptr) {
	const json* cap = call.get("cap");
	if (!cap || !cap->is_int() || cap->negative())
		return {ga1_wire::stopped_json("Unsupported", "cap_not_admissible", "admit", {}, 0), {}, {}, {}};
	meter m;
	auto c = cap->i64();
	m.cap = c ? *c : INT64_MAX;
	return run(call, st, m, seen);
}

inline std::vector<event> trace(const json& call, const state_in& st, result_t* result = nullptr, probe* seen = nullptr) {
	std::vector<event> ev;
	meter m; m.trace = &ev;
	auto r = run(call, st, m, seen);
	if (result) *result = std::move(r);
	return ev;
}

/// A host's plans, by digest.
struct plan_table {
	std::map<std::string, plan_ptr> by_digest;
	void keep(plan_ptr p) { if (p) by_digest[p->digest] = std::move(p); }
	void evict(const std::string& d) { by_digest.erase(d); }
	state_in attach(const json* state) const {
		state_in st = ga1::state_of(state);
		if (st.ok) if (auto it = by_digest.find(st.name); it != by_digest.end()) st.pl = it->second.get();
		return st;
	}
	/// Restore a plan from saved bytes bound to its digest: a load at LOAD_CAP
	/// on a meter of its own, expecting the header to name @p compiler if one
	/// is given. The load's outcome.
	json restore(const std::string& saved, const std::string& fmt, const std::string& digest, probe* seen = nullptr,
		const std::string* compiler = nullptr)
	{
		json c = json::object();
		c.set("schema", json::str(SCHEMA));
		c.set("op", json::str("load"));
		c.set("format", json::str(fmt));
		bool ascii = std::all_of(saved.begin(), saved.end(), [](char ch) { return (unsigned char)ch < 0x80; });
		if (fmt == "tba1" && ascii) c.set("artifact", json::str(saved));
		else {
			static const char* hx = "0123456789abcdef";
			std::string h;
			for (unsigned char ch : saved) { h.push_back(hx[ch >> 4]); h.push_back(hx[ch & 15]); }
			c.set("bytes", json::str(h));
		}
		c.set("bind", json::str(digest));
		if (compiler) c.set("compiler", json::str(*compiler));
		c.set("cap", json::integer(LOAD_CAP));
		auto r = transition(c, state_in{}, seen);
		keep(r.prepared);
		return r.outcome;
	}
};

#undef GA1_TRY

} // namespace idni::tau_lang::ga1
