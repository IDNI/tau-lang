// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file hoa_scan.h
 * @brief The one scanner of HOA text, shared by the strategy reader
 * (`parse_hoa`) and the synthesis game reader (`alg_d::parse_synth_game_hoa`).
 *
 * `scan` reads what both need: the lines up to `--END--`, the `States:`,
 * `Start:` and `AP:` headers, and the body with its acceptance marks. Every
 * other header line is handed back, so each reader keeps only its own items.
 */

#ifndef __IDNI__TAU__HOA_SCAN_H__
#define __IDNI__TAU__HOA_SCAN_H__

#include "tau_diagnostics.h"
#include "ltl_aba_limits.h"
#include "hoa_label.h"

#include <algorithm>
#include <cctype>
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// A single linear pass over the lines of the text: a strategy of a few
// hundred states runs to megabytes, so the reader must stay linear. It is
// tolerant only of what it does not read: an unknown printable line is
// skipped, and everything after the exact `--END--` line is ignored (SY-4).
// A line it does read must be well formed: a `States:`, `Start:`, `AP:` or
// `State:` line, an edge (a line that starts with `[`) and a marker (a line
// that starts with `-`). `--BODY--` is required, an edge must follow a
// `State:` line, and a byte outside printable ASCII, tab and CR refuses the
// text. A CR before the '\n' is dropped, and a header line that starts with
// a blank continues the line before it, as Spot wraps a long item.
//
// Do not replace this reader with the generated parser of parser/hoa.tgf.
// That grammar is unambiguous and accepts Spot's output
// (tests/unit/test_hoa_grammar.cpp), but the Earley parser costs about
// 40 µs and 10 KB of RSS per input byte. Measured with `tgf hoa.tgf parse`
// on `ltl2tgba -D -H` automata: 8.7 KB took 0.28 s and 118 MB, 71 KB (384
// states) took 2.8 s and 720 MB, and 144 KB took 6.5 s and 1.4 GB. A
// megabyte strategy would take tens of seconds and more than 10 GB. Parsing
// one `State:` block at a time bounds the memory but still costs about
// 7 µs per byte. This pass reads the same 71 KB in 6 ms, even unoptimized.

namespace idni::tau_lang::hoa_scan {

inline bool blank(char c) { return c == ' ' || c == '\t'; }
inline bool printable(char c) {
	return static_cast<unsigned char>(c) >= 0x20
		&& static_cast<unsigned char>(c) < 0x7f;
}
inline bool digit(char c) { return c >= '0' && c <= '9'; }

// A cursor over one line, without its '\n'.
struct line {
	std::string_view s;
	size_t i = 0;
	bool done() const { return i == s.size(); }
	char peek() const { return done() ? '\0' : s[i]; }
	bool eat(char c) {
		if (peek() != c) return false;
		++i;
		return true;
	}
	bool eat(std::string_view w) {
		if (s.substr(i, w.size()) != w) return false;
		i += w.size();
		return true;
	}
	void blanks() { while (!done() && blank(s[i])) ++i; }
	// at least one blank
	bool blanks1() {
		if (!blank(peek())) return false;
		blanks();
		return true;
	}
	// digits, read with strtol; -1 when out of range
	std::optional<long> num() {
		size_t b = i;
		while (!done() && digit(s[i])) ++i;
		if (b == i) return std::nullopt;
		std::string t(s.substr(b, i - b));
		errno = 0;
		long v = std::strtol(t.c_str(), nullptr, 10);
		return errno == ERANGE ? -1 : v;
	}
	// `( b num )* b` up to the end of the line, each number in range
	std::optional<std::vector<long>> nums() {
		std::vector<long> out;
		for (blanks(); !done(); blanks()) {
			auto n = num();
			if (!n || *n < 0) return std::nullopt;
			out.push_back(*n);
		}
		return out;
	}
	// `{ b ( num b )* }`: the acceptance sets of a mark
	std::optional<std::vector<long>> acc() {
		if (!eat('{')) return std::nullopt;
		std::vector<long> sets;
		for (blanks(); !eat('}'); blanks()) {
			auto n = num();
			if (!n || *n < 0) return std::nullopt;
			sets.push_back(*n);
		}
		return sets;
	}
	// `[ b1 acc ]`: the sets of a mark, nullopt for none; false when the
	// mark is malformed
	bool mark(std::optional<std::vector<long>>& sets) {
		size_t back = i;
		if (!(blanks1() && peek() == '{')) { i = back; return true; }
		sets = acc();
		return sets.has_value();
	}
};

// A line skipped as unknown: empty, or not starting with '-' or '[', and
// printable or tab throughout.
inline bool skippable(std::string_view s) {
	if (s.empty()) return true;
	if (s[0] == '-' || s[0] == '[') return false;
	for (char c : s) if (!printable(c) && c != '\t') return false;
	return true;
}

// `<keyword> b num b` with nothing else on the line
inline std::optional<long> keyword_num(std::string_view s,
	std::string_view keyword)
{
	line l{ s };
	if (!l.eat(keyword)) return std::nullopt;
	l.blanks();
	auto n = l.num();
	if (!n) return std::nullopt;
	l.blanks();
	if (!l.done()) return std::nullopt;
	return n;
}

// `AP: b num ( b1 name )* b`, a name quoted or a run of printable chars
// without blank or quote
inline std::optional<std::pair<long, std::vector<std::string>>> ap_line(
	std::string_view s)
{
	line l{ s };
	if (!l.eat("AP:")) return std::nullopt;
	l.blanks();
	auto n = l.num();
	if (!n) return std::nullopt;
	std::vector<std::string> names;
	for (;;) {
		size_t back = l.i;
		if (!l.blanks1()) break;
		size_t b = l.i;
		if (l.eat('"')) {
			while (!l.done() && l.s[l.i] != '"' && printable(l.s[l.i]))
				++l.i;
			if (!l.eat('"')) return std::nullopt;
			names.emplace_back(l.s.substr(b + 1, l.i - b - 2));
			continue;
		}
		while (!l.done() && printable(l.s[l.i]) && l.s[l.i] != ' '
			&& l.s[l.i] != '"') ++l.i;
		if (l.i == b) { l.i = back; break; }
		names.emplace_back(l.s.substr(b, l.i - b));
	}
	l.blanks();
	if (!l.done()) return std::nullopt;
	return std::make_pair(*n, std::move(names));
}

struct state_head {
	long n;
	std::optional<std::vector<long>> acc;
};

// `State: b num [ b1 acc ] b`
inline std::optional<state_head> state_line(std::string_view s) {
	line l{ s };
	if (!l.eat("State:")) return std::nullopt;
	l.blanks();
	auto n = l.num();
	if (!n) return std::nullopt;
	state_head st{ *n, std::nullopt };
	if (!l.mark(st.acc)) return std::nullopt;
	l.blanks();
	if (!l.done()) return std::nullopt;
	return st;
}

struct edge_line_t {
	std::string_view guard;
	hoa_label::expr label;
	long dst;
	std::optional<std::vector<long>> acc;
};

// `[ label ] b num [ b1 acc ] b`, the label read by hoa_label::read
inline std::optional<edge_line_t> edge_line(std::string_view s) {
	line l{ s };
	if (!l.eat('[')) return std::nullopt;
	size_t gb = l.i;
	size_t ge = s.find(']', gb);
	if (ge == std::string_view::npos) return std::nullopt;
	std::string_view g = s.substr(gb, ge - gb);
	auto lab = hoa_label::read(g);
	if (!lab) return std::nullopt;
	l.i = ge + 1;
	l.blanks();
	auto n = l.num();
	if (!n) return std::nullopt;
	edge_line_t e{ g, std::move(*lab), *n, std::nullopt };
	if (!l.mark(e.acc)) return std::nullopt;
	l.blanks();
	if (!l.done()) return std::nullopt;
	return e;
}

/// True when the mark @p acc names an acceptance set: an empty mark `{}`
/// names none.
inline bool marked(const std::optional<std::vector<long>>& acc) {
	return acc && !acc->empty();
}

/// An edge of a scanned automaton; `acc` holds the sets of its mark, and
/// is nullopt when it carries none.
struct edge {
	std::string guard;
	size_t dst = 0;
	std::optional<std::vector<long>> acc;
};

/// A `State:` block of a scanned automaton, in body order.
struct state {
	size_t n = 0;
	std::optional<std::vector<long>> acc;
	std::vector<edge> edges;
};

/// The parts of an automaton every reader needs.
struct automaton {
	size_t num_states = 0;
	size_t start = 0;
	std::vector<std::string> aps;
	/// The header lines `scan` does not read, wrapped lines joined.
	std::vector<std::string> other;
	std::vector<state> states;
};

/**
 * @brief Scan the HOA text of one automaton.
 *
 * The state count is bounded by `ltl_hoa_max_states()`.
 * @param hoa_text The HOA text, `--BODY--` included.
 * @param what How the errors name the text, e.g. "the HOA strategy".
 * @return The automaton, or a `code::parse_error` when the text is not an
 * automaton, a line it reads is malformed, a header count is out of
 * range, the AP count does not match the names, a state or transition
 * target is outside the state table, an edge comes before any state, or an
 * edge label is not a label (`hoa_label::read`) or names a proposition
 * outside the AP table.
 */
inline result<automaton> scan(const std::string& hoa_text,
	std::string_view what)
{
	result<automaton> r;
	const std::string w(what);
	auto error = [&](const std::string& msg, std::string_view ln = {})
		-> result<automaton>
	{
		if (ln.empty()) return r.with_error(code::parse_error, w + msg);
		return r.with_error(code::parse_error, w + msg,
			{{label::value, truncate_for_message(std::string(ln))}});
	};
	// LA-8 / SY-1: text with no `--BODY--`, or truncated before it, is not
	// an automaton. Refuse rather than execute an unconstrained program.
	auto not_hoa = [&](std::string_view ln = {}) {
		return error(" is not a HOA automaton (truncated, a malformed "
			"line, or no `--BODY--`)", ln);
	};

	// The lines of the automaton, each ended by '\n', up to `--END--`; the
	// rest must be printable or space.
	std::vector<std::string_view> lines;
	std::string_view text(hoa_text);
	for (size_t i = 0; i < text.size(); ) {
		size_t eol = text.find('\n', i);
		if (eol == std::string_view::npos) return not_hoa();
		std::string_view ln = text.substr(i, eol - i);
		i = eol + 1;
		if (!ln.empty() && ln.back() == '\r') ln.remove_suffix(1);
		if (ln == "--END--") {
			for (char c : text.substr(i))
				if (!printable(c) && !std::isspace(
					static_cast<unsigned char>(c))) return not_hoa();
			break;
		}
		lines.push_back(ln);
	}
	auto body_at = std::find(lines.begin(), lines.end(), "--BODY--");
	if (body_at == lines.end()) return not_hoa();

	std::vector<std::string> header;
	for (auto it = lines.begin(); it != body_at; ++it) {
		std::string_view ln = *it;
		if (!ln.empty() && blank(ln[0]) && !header.empty()) {
			size_t b = ln.find_first_not_of(" \t");
			if (b != std::string_view::npos)
				header.back() += " " + std::string(ln.substr(b));
			continue;
		}
		header.emplace_back(ln);
	}

	automaton aut;
	// A strategy with more states than this is not something ltlsynt
	// produces for any specification this pipeline builds; an absurd
	// count is a garbled header, not an automaton (SY-R3). Runtime
	// parameter `ltl_hoa_max_states()` (0 = unlimited).
	const size_t cap = ltl_hoa_max_states();
	const long max_states = cap
		? (long) std::min<size_t>(cap, (size_t) INT_MAX)
		: INT_MAX;
	long start = 0;
	bool seen_states = false;
	for (auto& ln : header) {
		if (ln.rfind("States:", 0) == 0) {
			// LA-8 / SY-1 / SY-R3: an automaton has at least one
			// state; refuse a garbled or absurd count.
			auto n = keyword_num(ln, "States:");
			if (!n || *n < 1 || *n > max_states)
				return error(" has a malformed state count",
					std::string_view(ln).substr(7));
			aut.num_states = static_cast<size_t>(*n);
			seen_states = true;
		} else if (ln.rfind("Start:", 0) == 0) {
			auto st = keyword_num(ln, "Start:");
			if (!st || *st < 0)
				return error(" has a malformed `Start:` state", ln);
			start = *st;
		} else if (ln.rfind("AP:", 0) == 0) {
			auto ap = ap_line(ln);
			if (!ap) return error(" has a malformed `AP:` line", ln);
			auto& [n, names] = *ap;
			if (n < 0 || names.size() < static_cast<size_t>(n))
				return error(" has an AP count that does not match "
					"the names it lists", ln);
			names.resize(static_cast<size_t>(n)); // extras are ignored
			aut.aps = std::move(names);
		} else if (!skippable(ln)) return not_hoa(ln);
		else aut.other.push_back(std::move(ln));
	}
	if (!seen_states) return error(" has no `States:` header");
	if (static_cast<size_t>(start) >= aut.num_states)
		return error(" has a `Start:` state that is not one of its "
			"states", std::to_string(start));
	aut.start = static_cast<size_t>(start);

	for (auto it = std::next(body_at); it != lines.end(); ++it) {
		std::string_view ln = *it;
		if (ln.rfind("State:", 0) == 0) {
			auto st = state_line(ln);
			if (!st) return error(" has a malformed `State:` line", ln);
			if (st->n < 0
				|| static_cast<size_t>(st->n) >= aut.num_states)
				return error(" has a state number outside the state "
					"table", ln);
			aut.states.push_back({ static_cast<size_t>(st->n),
				std::move(st->acc), {} });
		} else if (auto ed = edge_line(ln)) {
			if (aut.states.empty())
				return error(" has an edge before any `State:` line",
					ln);
			if (ed->dst < 0
				|| static_cast<size_t>(ed->dst) >= aut.num_states)
				return error(" has a transition to a state outside "
					"the state table", ln);
			if (hoa_label::ap_bound(ed->label) > aut.aps.size())
				return error(" has a label over a proposition outside "
					"its AP table", ln);
			aut.states.back().edges.push_back({ std::string(ed->guard),
				static_cast<size_t>(ed->dst), std::move(ed->acc) });
		} else if (!skippable(ln)) return not_hoa(ln);
	}
	return r.with_value(std::move(aut));
}

} // namespace idni::tau_lang::hoa_scan

#endif // __IDNI__TAU__HOA_SCAN_H__
