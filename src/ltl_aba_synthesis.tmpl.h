// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// ltl_aba_synthesis.tmpl.h - HOA parsing and the tau-lang side of ltlsynt
// calls. The Spot tool calls (argv, tempfiles, the exit-code convention)
// live in backends/spot/spot.h, the spawn in spawn_capture.h; call_ltlsynt builds
// the request, calls the backend, and interprets the verdict.

#include "backends/spot/spot.h"
#include "reset_hooks.h"
#include "ltl_strategy_minimize.h"
#include "hoa_scan.h"
#include <algorithm>
#include <cctype>
#include <cerrno>
#include <climits>
#include <string_view>

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ltl_aba"

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
			LOG_DEBUG << "Strategy minimised from "
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
	LOG_TRACE << "call_ltlsynt/ltl_formula: " << ltl_formula;

	// LT-7: a backend failure (not on PATH, timeout watchdog,
	// usage error, no recognized verdict line) is forwarded as an error
	// here, never read as a decided UNREALIZABLE.
	TAU_TRY(auto verdict, synthesize(ltl_formula, input_props, output_props,
		timeout_sec, ltlsynt_options()));

	if (!verdict.realizable) {
		LOG_DEBUG << "The ltlsynt verdict: UNREALIZABLE, no strategy for the "
			"skeleton";
		print_env_counter_strategy_witness(r.report(), ltl_formula,
			input_props, output_props, legend, timeout_sec);
		return r.with_value(std::make_pair(false, std::string()));
	}

	TAU_TRY_VOID(bound_strategy(verdict.hoa, ltl_formula, input_props,
		output_props, timeout_sec));
	export_strategy(r.report(), verdict.hoa, timeout_sec);

	if (auto pos = verdict.hoa.find("States:"); pos != std::string::npos) {
		auto eol = verdict.hoa.find('\n', pos);
		LOG_DEBUG << "The ltlsynt verdict: REALIZABLE, strategy "
			<< verdict.hoa.substr(pos, eol - pos);
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

/**
 * @brief Parse Spot's HOA text of a strategy automaton.
 *
 * The text is read by `hoa_scan::scan`. State-based acceptance marks are
 * propagated to the incoming edges.
 * @param hoa_text The HOA text, `--BODY--` included.
 * @return The automaton, or the `code::parse_error` of `hoa_scan::scan`.
 */
inline result<hoa_automaton> parse_hoa(const std::string& hoa_text) {
	result<hoa_automaton> r;
	auto& last = last_hoa_parse;
	if (last && last->cap == ltl_hoa_max_states() && last->text == hoa_text)
		return r.with_value(hoa_automaton(last->aut));
	TAU_TRY(auto scanned, hoa_scan::scan(hoa_text, "the HOA strategy"));

	hoa_automaton aut;
	aut.num_states = scanned.num_states;
	aut.initial_state = scanned.start;
	aut.aps = std::move(scanned.aps);
	aut.edges.assign(aut.num_states, {});
	aut.state_accepting.assign(aut.num_states, false);
	for (auto& st : scanned.states) {
		if (st.acc) aut.state_accepting[st.n] = true;
		for (auto& ed : st.edges)
			aut.edges[st.n].push_back(hoa_edge{ std::move(ed.guard),
				ed.dst, ed.acc.has_value() });
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
