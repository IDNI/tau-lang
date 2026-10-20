// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// ltl_aba_synthesis.tmpl.h - HOA parsing and the tau-lang side of ltlsynt
// calls. The Spot tool calls (argv, tempfiles, the exit-code convention)
// live in backends/spot/spot.h, the spawn in spawn_capture.h; call_ltlsynt builds
// the request, calls the backend, and interprets the verdict.

#include "backends/spot/spot.h"
#include "reset_hooks.h"
#include "ltl_strategy_minimize.h"
#include <algorithm>
#include <cctype>
#include <cerrno>
#include <climits>
#include <string_view>

namespace idni::tau_lang {

// The ltlsynt watchdog is the runtime parameter `ltl_timeout_sec_param`
// (ltl_aba_limits.h): `--ltl-timeout`, REPL `set ltl-timeout`,
// `api::set_ltl_timeout_sec`, or the environment variable TAU_LTL_TIMEOUT.

// ltlsynt_available() is declared in ltl_aba.h and used throughout the test
// suites as a doctest::skip() gate; it delegates to the backend so there is
// exactly one PATH probe.
/** @internal @copydoc ltlsynt_available @endinternal */
inline bool ltlsynt_available() { return available(); }

// The ltlsynt options of a verdict, from ltl-synth-algo, ltl-decompose and
// ltl-simplification.
inline synthesis_options ltlsynt_options() {
	return { ltl_synth_algo_param, ltl_decompose_param,
		ltl_simplification_param };
}

// The constructions a parity game is asked with, in order: ltl-synth-algo
// first, then acd, which usually gives the smallest parity game, then sd,
// which always gives one.
inline std::vector<std::string> ltlsynt_game_algorithms() {
	std::vector<std::string> algos;
	if (!ltl_synth_algo_param.empty()) algos.push_back(ltl_synth_algo_param);
	for (const char* a : { "acd", "sd" })
		if (ltl_synth_algo_param != a) algos.emplace_back(a);
	return algos;
}

// ── stderr side channels (opt-in debug output; no report, no return value) ─

// ltl-witness: on UNREALIZABLE, print the environment's winning
// strategy. By determinacy of ω-regular two-player games, UNREAL means
// ∃env.∀sys.¬φ, so swapping ins/outs and negating the formula turns the
// environment into "sys" of a new, REALIZABLE-iff-witness game. The
// system of the original game reads the inputs of a step before it picks
// its outputs (Mealy), so the environment picks the inputs without seeing
// those outputs: the dual game is Moore. The witness also goes into the
// report, with the atom each prop stands for; when there is none, the
// reason takes its place, so `ltl-witness` and `explain` never go silent.
inline void print_env_counter_strategy_witness(report& rep,
	const std::string& ltl_formula,
	const std::vector<std::string>& input_props,
	const std::vector<std::string>& output_props,
	const ltlsynt_legend& legend, int timeout_sec)
{
	if (!ltl_witness_param && !ltl_witness_into_report) return;
	auto moore = ltlsynt_options();
	moore.moore = true;
	auto neg = synthesize("!(" + ltl_formula + ")",
		output_props, input_props, timeout_sec, moore);
	if (!neg.has_value() || !neg.value().realizable) {
		std::string reason =
			neg.has_value() ? "ltlsynt found the dual game unrealizable"
			: report_has_attr(neg.report(), label::timeout)
				? "ltlsynt did not solve the dual game within ltl-timeout"
			: "ltlsynt did not solve the dual game, which runs "
				"--semantics=Moore (Spot >= 2.16)";
		if (ltl_witness_param) std::fprintf(stderr,
		    "=== ENV COUNTER-STRATEGY (UNREAL witness) ===\nnone: %s\n",
		    reason.c_str());
		// a side channel: its failure decides nothing
		auto sc = rep.open("rejected candidate");
		rep.info("no environment counter-strategy",
			{{label::value, reason}});
		report cand = std::move(neg).report();
		cand.demote_errors_to_warnings();
		rep.append(std::move(cand));
		return;
	}
	std::string props;
	for (const auto& [p, text] : legend)
		props += "  " + p + " := " + text + "\n";
	if (ltl_witness_param) std::fprintf(stderr,
	    "=== ENV COUNTER-STRATEGY (UNREAL witness) ===\n%s%s\n",
	    props.c_str(), neg.value().hoa.c_str());
	rep.info("environment counter-strategy",
		{{label::value, props + neg.value().hoa}});
}

// ltl-export-strategy hoa|dot: print the winning strategy to stderr.
// ltl-export-strategy-file <path>: also write the HOA text to that
// (persistent, user-named) path; a write failure is a report warning, not
// a silently dropped error.
inline void export_strategy(report& rep, const std::string& hoa,
	int timeout_sec)
{
	const std::string& fmt = ltl_export_strategy_param;
	const std::string& path = ltl_export_strategy_file_param;

	if (!hoa.empty()) {
		if (fmt == "hoa") {
			std::fprintf(stderr, "=== STRATEGY HOA ===\n%s\n", hoa.c_str());
		} else if (fmt == "dot") {
			auto dot = to_dot(hoa, timeout_sec);
			if (dot.has_value())
				std::fprintf(stderr, "=== STRATEGY DOT ===\n%s\n",
					dot.value().c_str());
			else
				std::fprintf(stderr,
					"=== STRATEGY HOA (dot unavailable) ===\n%s\n",
					hoa.c_str());
		}
	}
	if (!path.empty()) {
#ifdef _MSC_VER
		// the MSVC CRT marks fopen unsafe and asks for fopen_s
		FILE* f = nullptr;
		if (fopen_s(&f, path.c_str(), "w") != 0) f = nullptr;
#else
		FILE* f = std::fopen(path.c_str(), "w");
#endif
		// fwrite buffers, so a write failure can surface only at the
		// flush that fclose does.
		bool ok = f
			&& std::fwrite(hoa.data(), 1, hoa.size(), f) == hoa.size();
		if (f && std::fclose(f) != 0) ok = false;
		if (!ok) rep.warning("failed to write the strategy export file",
			{{label::path, path}});
	}
}

// ── strategy size ───────────────────────────────────────────────────────

// The last strategy parse_hoa read, with the state cap it was read under.
// The strategy a verdict sizes (bound_strategy) is parsed again by its
// caller, and a large one takes seconds to parse.
struct hoa_parse_memo {
	std::string text;
	size_t cap = 0;
	hoa_automaton aut;
};
inline thread_local std::optional<hoa_parse_memo> last_hoa_parse;

// The strategy text of @p hoa, minimised when ltl-minimize-strategy is set.
// Returns its state count, or nullopt when the text does not parse (the
// caller's parse_hoa then reports it).
inline std::optional<size_t> minimized_strategy(std::string& hoa) {
	auto aut = parse_hoa(hoa);
	if (!aut.has_value()) return std::nullopt;
	if (!ltl_minimize_strategy_param) return aut.value().num_states;
	hoa_automaton q = minimize_strategy(aut.value());
	if (q.num_states < aut.value().num_states)
		if (auto text = hoa_text_of(q, hoa)) {
			LOG_INFO << "[ltl_aba] strategy minimised from "
				<< aut.value().num_states << " to " << q.num_states
				<< " states";
			hoa = std::move(*text);
			last_hoa_parse = hoa_parse_memo{ hoa, ltl_hoa_max_states(), q };
		}
	return q.num_states;
}

// Applies ltl-minimize-strategy and ltl-strategy-max-states to the strategy
// @p hoa of a REALIZABLE verdict. A strategy above the bound is asked again
// with the SAT minimisation of ltlsynt; one still above it is an error.
inline result<void> bound_strategy(std::string& hoa,
	const std::string& ltl_formula,
	const std::vector<std::string>& input_props,
	const std::vector<std::string>& output_props, int timeout_sec)
{
	result<void> r;
	const size_t bound = ltl_strategy_max_states();
	if (!ltl_minimize_strategy_param && bound == 0) return r;
	auto states = minimized_strategy(hoa);
	if (!states || bound == 0 || *states <= bound) return r;
	if (ltl_simplification_param.find("sat") == std::string::npos) {
		synthesis_options opts = ltlsynt_options();
		opts.simplify = "bwoa-sat";
		TAU_TRY(auto again, synthesize(ltl_formula, input_props,
			output_props, timeout_sec, opts));
		if (again.realizable) {
			auto fewer = minimized_strategy(again.hoa);
			if (fewer && *fewer < *states) {
				hoa = std::move(again.hoa);
				states = fewer;
			}
		}
	}
	if (*states <= bound) return r;
	return r.with_error(code::solver_error, std::string("UNKNOWN: the "
		"strategy has ") + std::to_string(*states) + " states, above "
		"ltl-strategy-max-states (" + std::to_string(bound) + "); a "
		"smaller one may exist");
}

// ── call_ltlsynt ─────────────────────────────────────────────────────────

/** @internal @copydoc call_ltlsynt @endinternal */
inline result<std::pair<bool, std::string>> call_ltlsynt(
    const std::string& ltl_formula,
    const std::vector<std::string>& input_props,
    const std::vector<std::string>& output_props,
    const ltlsynt_legend& legend)
{
	result<std::pair<bool, std::string>> r;
	int timeout_sec = ltl_timeout_sec();

	// One ltlsynt process per call. The count rides the report, so a caller
	// that memoises queries (pointwise revision) can measure how many ran.
	r.report().count("ltlsynt_calls", (size_t) 1);

	// LT-7: a backend failure (not on PATH, timeout watchdog,
	// usage error, no recognized verdict line) is forwarded as an error
	// here, never read as a decided UNREALIZABLE.
	TAU_TRY(auto verdict, synthesize(ltl_formula, input_props, output_props,
		timeout_sec, ltlsynt_options()));

	if (!verdict.realizable) {
		print_env_counter_strategy_witness(r.report(), ltl_formula,
			input_props, output_props, legend, timeout_sec);
		return r.with_value(std::make_pair(false, std::string()));
	}

	TAU_TRY_VOID(bound_strategy(verdict.hoa, ltl_formula, input_props,
		output_props, timeout_sec));
	export_strategy(r.report(), verdict.hoa, timeout_sec);

	if (auto pos = verdict.hoa.find("States:"); pos != std::string::npos) {
		auto eol = verdict.hoa.find('\n', pos);
		LOG_INFO << "[ltl_aba] strategy " << verdict.hoa.substr(pos, eol - pos);
	}

	return r.with_value(std::make_pair(true, std::move(verdict.hoa)));
}

inline result<std::pair<bool, std::string>> call_ltlsynt(
    const std::string& ltl_formula,
    const std::vector<std::string>& input_props,
    const std::vector<std::string>& output_props)
{
	return call_ltlsynt(ltl_formula, input_props, output_props, {});
}

// ── HOA parser ────────────────────────────────────────────────────────────────
//
// A single linear pass over the lines of the text: a strategy of a few
// hundred states runs to megabytes, so the reader must stay linear. It is
// tolerant: an unknown header or body line is skipped, a `States:`,
// `Start:` or `AP:` line whose value is not a plain number is skipped too
// (so `Start: garbage` leaves the default and a garbled `States:` count is
// refused as a missing header), and everything after the exact `--END--`
// line is ignored (SY-4). It is strict where a skipped line could hide an
// automaton: a line that starts with `-` must be a marker, a line that
// starts with `[` must be a well-formed edge, `--BODY--` is required, and a
// byte outside printable ASCII, tab and CR refuses the text.

namespace hoa_scan {

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
	// `{...}`: printable but '}', or tab
	bool acc() {
		if (!eat('{')) return false;
		while (!done() && s[i] != '}' && (printable(s[i]) || s[i] == '\t'))
			++i;
		return eat('}');
	}
};

// guard := [ gb disj gb ];  disj := conj ( gb '|' gb conj )*;
// conj := lit ( gb '&' gb lit )*;
// lit := 't' | 'f' | ['!'] digits | '(' gb disj gb ')'
inline bool guard_disj(line& l);
inline bool guard_lit(line& l) {
	if (l.eat('t') || l.eat('f')) return true;
	if (l.eat('(')) {
		l.blanks();
		if (!guard_disj(l)) return false;
		l.blanks();
		return l.eat(')');
	}
	l.eat('!');
	return l.num().has_value();
}
inline bool guard_conj(line& l) {
	if (!guard_lit(l)) return false;
	for (;;) {
		size_t back = l.i;
		l.blanks();
		if (!l.eat('&')) { l.i = back; return true; }
		l.blanks();
		if (!guard_lit(l)) return false;
	}
}
inline bool guard_disj(line& l) {
	if (!guard_conj(l)) return false;
	for (;;) {
		size_t back = l.i;
		l.blanks();
		if (!l.eat('|')) { l.i = back; return true; }
		l.blanks();
		if (!guard_conj(l)) return false;
	}
}
inline bool guard(line& l) {
	if (l.peek() == ']') return true;
	l.blanks();
	if (!guard_disj(l)) return false;
	l.blanks();
	return true;
}

// A line skipped as unknown: empty, or not starting with '-' or '[', and
// printable, tab or CR throughout.
inline bool skippable(std::string_view s) {
	if (s.empty()) return true;
	if (s[0] == '-' || s[0] == '[') return false;
	for (char c : s)
		if (!printable(c) && c != '\t' && c != '\r') return false;
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

// `State: b num [ b1 acc ] b`: the state and whether it is marked
inline std::optional<std::pair<long, bool>> state_line(std::string_view s) {
	line l{ s };
	if (!l.eat("State:")) return std::nullopt;
	l.blanks();
	auto n = l.num();
	if (!n) return std::nullopt;
	bool marked = false;
	size_t back = l.i;
	if (l.blanks1() && l.peek() == '{') {
		if (!l.acc()) return std::nullopt;
		marked = true;
	} else l.i = back;
	l.blanks();
	if (!l.done()) return std::nullopt;
	return std::make_pair(*n, marked);
}

struct edge {
	std::string_view guard;
	long dst;
	bool marked;
};

// `[ guard ] b num [ b1 acc ] b`
inline std::optional<edge> edge_line(std::string_view s) {
	line l{ s };
	if (!l.eat('[')) return std::nullopt;
	size_t gb = l.i;
	if (!guard(l)) return std::nullopt;
	std::string_view g = s.substr(gb, l.i - gb);
	if (!l.eat(']')) return std::nullopt;
	l.blanks();
	auto n = l.num();
	if (!n) return std::nullopt;
	bool marked = false;
	size_t back = l.i;
	if (l.blanks1() && l.peek() == '{') {
		if (!l.acc()) return std::nullopt;
		marked = true;
	} else l.i = back;
	l.blanks();
	if (!l.done()) return std::nullopt;
	return edge{ g, *n, marked };
}

} // namespace hoa_scan

/**
 * @brief Parse Spot's HOA text of a strategy automaton.
 *
 * State-based acceptance marks are propagated to the incoming edges. The
 * state count is bounded by `ltl_hoa_max_states()`.
 * @param hoa_text The HOA text, `--BODY--` included.
 * @return The automaton, or a `code::parse_error` when the text is not an
 * automaton, a header count is malformed or out of range, the AP count does
 * not match the names, or a state or transition target is outside the
 * state table.
 */
inline result<hoa_automaton> parse_hoa(const std::string& hoa_text) {
	using namespace hoa_scan;
	result<hoa_automaton> r;
	auto& last = last_hoa_parse;
	if (last && last->cap == ltl_hoa_max_states() && last->text == hoa_text)
		return r.with_value(hoa_automaton(last->aut));
	auto not_hoa = [&r]() -> result<hoa_automaton> {
		// LA-8 / SY-1: text with no `--BODY--`, or truncated before it,
		// is not an automaton. Refuse rather than execute an
		// unconstrained program.
		return r.with_error(code::parse_error, "the HOA strategy is not a "
			"HOA automaton (truncated, or no `--BODY--`)");
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

	hoa_automaton aut;
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

	for (auto it = lines.begin(); it != body_at; ++it) {
		std::string_view ln = *it;
		if (auto n = keyword_num(ln, "States:")) {
			// LA-8 / SY-1 / SY-R3: a strategy has at least one
			// state; refuse a garbled or absurd count.
			if (*n < 1 || *n > max_states) {
				return r.with_error(code::parse_error,
					"the HOA strategy has a malformed state count",
					{{label::value, std::string(ln.substr(7))}});
			}
			aut.num_states = static_cast<size_t>(*n);
			seen_states = true;
			aut.edges.assign(aut.num_states, {});
			aut.state_accepting.assign(aut.num_states, false);
		} else if (auto st = keyword_num(ln, "Start:")) {
			start = *st;
		} else if (auto ap = ap_line(ln)) {
			auto& [n, names] = *ap;
			if (n < 0 || names.size() < static_cast<size_t>(n)) {
				return r.with_error(code::parse_error,
					"the HOA strategy's AP count does not match "
					"the names it lists",
					{{label::value, std::to_string(n)}});
			}
			names.resize(static_cast<size_t>(n)); // extras are ignored
			for (auto& nm : names) aut.aps.push_back(std::move(nm));
		} else if (!skippable(ln)) return not_hoa();
	}

	if (!seen_states) {
		return r.with_error(code::parse_error,
			"the HOA strategy has no `States:` header");
	}
	if (start < 0 || static_cast<size_t>(start) >= aut.num_states) {
		return r.with_error(code::parse_error,
			"the HOA strategy's `Start:` state is not one of its states",
			{{label::value, std::to_string(start)}});
	}
	aut.initial_state = static_cast<size_t>(start);

	std::optional<size_t> cur_state;
	for (auto it = std::next(body_at); it != lines.end(); ++it) {
		std::string_view ln = *it;
		if (auto st = state_line(ln)) {
			auto [n, marked] = *st;
			if (n < 0 || static_cast<size_t>(n) >= aut.num_states)
				return r.with_error(code::parse_error,
					"the HOA strategy has a state number outside "
					"the state table",
					{{label::value, truncate_for_message(
						std::string(ln))}});
			cur_state = static_cast<size_t>(n);
			if (marked) aut.state_accepting[*cur_state] = true;
		} else if (auto ed = edge_line(ln)) {
			if (!cur_state) continue;
			if (ed->dst < 0
				|| static_cast<size_t>(ed->dst) >= aut.num_states)
				return r.with_error(code::parse_error,
					"the HOA strategy has a transition to a state "
					"outside the state table",
					{{label::value, truncate_for_message(
						std::string(ln))}});
			hoa_edge e;
			e.guard_label = std::string(ed->guard);
			e.dst = static_cast<size_t>(ed->dst);
			e.accepting = ed->marked;
			aut.edges[*cur_state].push_back(e);
		} else if (!skippable(ln)) return not_hoa();
	}

	// Propagate state-based acceptance to edges (for Büchi with state marks)
	for (size_t s = 0; s < aut.edges.size(); ++s)
		for (auto& e : aut.edges[s])
			if (aut.state_accepting[e.dst])
				e.accepting = true;

	last = hoa_parse_memo{ hoa_text, ltl_hoa_max_states(), aut };
	return r.with_value(std::move(aut));
}

// ── Algorithm D: ltlsynt → parity game ───────────────────────────────────────
//
// Declared in algorithm_d_game.h and defined here so it can use the same
// backend `call_ltlsynt` uses. The formula goes in a file (`-F path`), not
// in an argument: a single argument is capped at the Linux MAX_ARG_STRLEN
// of 131072, which a grown φ* (Algorithm B with many constants, or the
// semantic-PWR fallback) exceeds.

namespace alg_d {

inline result<synth_game> call_ltlsynt_game(
	const std::string& phi_prop,
	const std::vector<std::string>& ins,
	const std::vector<std::string>& outs,
	const std::string& algo)
{
	// ltlsynt's default construction may name the condition Streett, which
	// is no parity game.
	if (algo.empty()) {
		result<synth_game> game;
		for (const auto& a : ltlsynt_game_algorithms()) {
			game = call_ltlsynt_game(phi_prop, ins, outs, a);
			if (!game.has_value() || (game.value().acc_known
				&& !game.value().multi_colored)) break;
		}
		return game;
	}
	result<synth_game> r;

	// Cache: avoid re-running ltlsynt on identical (formula, ins, outs),
	// parsed under the same state and cube caps.
	// TT2-13: a bounded_cache in runtime-bound mode (`set cachebound`, 0 =
	// unbounded, FIFO eviction). Callers get their own copy of the
	// cached entry.
	static bounded_cache<std::string, synth_game> cache{&cache_bound};
	static const bool reset_registered =
		(on_reset([] { cache.clear(); }), true);
	(void)reset_registered;
	// '\x1e' (record separator) cannot occur in an LTL formula or an AP
	// name, so the concatenation is injective. Local to this cache key --
	// the backend's own comma-joiner formats an argv flag, a different job.
	auto csv = [](const std::vector<std::string>& v) {
		std::string s;
		for (size_t i = 0; i < v.size(); ++i) { if (i) s += ","; s += v[i]; }
		return s;
	};
	const std::string key = phi_prop + '\x1e' + csv(ins) + '\x1e' + csv(outs)
		+ '\x1e' + algo + '\x1e' + std::to_string(ltl_hoa_max_states())
		+ '\x1e' + std::to_string(ltl_guard_max_cubes());
	if (auto it = cache.find(key); it != cache.end()) { return r.with_value(it->second); }

	int timeout_sec = ltl_timeout_sec();

	// A timeout, a missing binary or a usage error is a backend error, not
	// an empty game a caller would read as UNREALIZABLE. Nothing transient
	// is cached.
	TAU_TRY(auto hoa, synthesize_game(phi_prop, ins, outs, timeout_sec,
		algo));

	// Insert-then-copy: the freshly inserted entry is the newest in FIFO
	// order, so an eviction triggered by this insert can only remove
	// OLDER entries (bound >= 1) — reading it back right after is safe.
	// A rejected header (bad counts, too many APs, a decomposed
	// multi-game text) is a parser error; like a spot failure that is no
	// verdict, not an UNREALIZABLE one, so refuse here instead of caching
	// it.
	TAU_TRY(auto game, parse_synth_game_hoa(hoa));
	auto [it, inserted] = cache.emplace(key, std::move(game));
	(void) inserted;  // the find above missed, so this always inserts
	return r.with_value(it->second);
}

} // namespace alg_d

} // namespace idni::tau_lang
