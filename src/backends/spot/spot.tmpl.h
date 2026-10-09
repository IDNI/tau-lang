// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "backends/spot/spot.h" // Only for IDE resolution, not really needed.

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "spot"

#include <algorithm>
#include <array>
#include <atomic>
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <optional>
#include <thread>

#include "utility/temp_file.h"

namespace idni::tau_lang {

// ── argv / CSV helpers ───────────────────────────────────────────────────

// ltlsynt's --ins=/--outs= take a single comma-joined argument; this is the
// only place in the backend that needs it.
inline std::string csv_join(const std::vector<std::string>& v) {
	std::string s;
	for (size_t i = 0; i < v.size(); ++i) { if (i) s += ","; s += v[i]; }
	return s;
}

// A verdict line is "REALIZABLE\n<hoa>" or "UNREALIZABLE\n", anywhere else
// there is no verdict. Used only by `synthesize`.
struct verdict_line { bool realizable; std::string body; };

// The verdict of ltlsynt's output when it starts with a verdict word, with
// the text after the first newline as body; nullopt otherwise.
inline std::optional<verdict_line> parse_verdict_line(const std::string& out) {
	auto body = [&] {
		auto nl = out.find('\n');
		return nl == std::string::npos ? std::string() : out.substr(nl + 1);
	};
	if (out.compare(0, 12, "UNREALIZABLE") == 0)
		return verdict_line{false, body()};
	if (out.compare(0, 10, "REALIZABLE") == 0)
		return verdict_line{true, body()};
	return std::nullopt;
}

// ── public surface ───────────────────────────────────────────────────────

inline result<synthesis_verdict> synthesize(const std::string& formula,
	const std::vector<std::string>& ins,
	const std::vector<std::string>& outs, int timeout_sec)
{
	result<synthesis_verdict> r;

	// -F path avoids both shell-escaping and the Linux MAX_ARG_STRLEN
	// limit (131072) that an inline --formula="..." argument hits once a
	// formula grows (Algorithm B with many constants).
	TAU_TRY(auto tmp, fs::temp_file::create("tau_lang", formula + "\n"));

	std::vector<std::string> argv = {"ltlsynt", "-F", tmp.path()};
	std::string ins_str = csv_join(ins), outs_str = csv_join(outs);
	if (!ins_str.empty())  argv.push_back("--ins="  + ins_str);
	if (!outs_str.empty()) argv.push_back("--outs=" + outs_str);
	if (const char* simp = std::getenv("TAU_LTL_SIMPLIFICATION"); simp && *simp)
		argv.push_back("--simplification=" + std::string(simp));

	auto exit_ok = [](int c) { return c == 0 || c == 1; };
	auto spawned = spawn_capture(argv, timeout_sec, exit_ok);
	if (!spawned.has_value()) {
		// spawn_capture decided the process-level fact (not found, killed,
		// exited oddly) and recorded it; this adds what that means for a
		// realizability request, on top of the merged child report.
		bool not_found = report_has_code(spawned.report(), code::not_found);
		bool timed_out = report_has_attr(spawned.report(), label::timeout);
		r.merge(std::move(spawned));
		if (not_found)
			return r.with_error(code::solver_error,
				"ltlsynt not found on PATH; install Spot (>= 2.10) -- "
				"realizability is UNKNOWN");
		if (timed_out)
			return r.with_error(code::solver_error,
				"ltlsynt produced no verdict, killed by the ltl-timeout "
				"watchdog (--ltl-timeout / `set ltltimeout` / "
				"TAU_LTL_TIMEOUT); the realizability of this "
				"specification is UNKNOWN");
		return r.with_error(code::solver_error,
			"ltlsynt produced no verdict; the realizability of this "
			"specification is UNKNOWN");
	}

	auto verdict = parse_verdict_line(spawned.value());
	if (!verdict)
		return r.with_error(code::solver_error,
			"ltlsynt output carried no verdict line");
	return r.with_value(synthesis_verdict{verdict->realizable,
		verdict->realizable ? std::move(verdict->body) : std::string()});
}

inline result<std::string> synthesize_game(const std::string& formula,
	const std::vector<std::string>& ins,
	const std::vector<std::string>& outs, int timeout_sec,
	const std::string& algo)
{
	result<std::string> r;

	TAU_TRY(auto tmp, fs::temp_file::create("tau_lang_game", formula + "\n"));

	// --polarity=no: keep the genuine arena (every sys alternative, real
	// acceptance) instead of ltlsynt's constant-output-substituted one, so
	// a re-solve under a different feasibility notion is sound (ALG-D-28).
	// --decompose=no: a decomposed spec prints one game per part; the
	// product game needs a single game carrying every atom.
	std::vector<std::string> argv = {
	    "ltlsynt", "-F", tmp.path(),
	    "--polarity=no", "--decompose=no", "--print-game-hoa"};
	std::string ins_str = csv_join(ins), outs_str = csv_join(outs);
	if (!ins_str.empty())  argv.push_back("--ins="  + ins_str);
	if (!outs_str.empty()) argv.push_back("--outs=" + outs_str);
	if (!algo.empty()) argv.push_back("--algo=" + algo);

	auto exit_ok = [](int c) { return c == 0 || c == 1; };
	auto spawned = spawn_capture(argv, timeout_sec, exit_ok);
	if (!spawned.has_value()) {
		bool not_found = report_has_code(spawned.report(), code::not_found);
		bool timed_out = report_has_attr(spawned.report(), label::timeout);
		r.merge(std::move(spawned));
		if (not_found)
			return r.with_error(code::solver_error,
				"ltlsynt not found on PATH; install Spot (>= 2.10) -- "
				"the parity game could not be built");
		if (timed_out)
			return r.with_error(code::solver_error,
				"ltlsynt --print-game-hoa produced no game, killed by "
				"the ltl-timeout watchdog (--ltl-timeout / "
				"`set ltltimeout` / TAU_LTL_TIMEOUT)");
		return r.with_error(code::solver_error,
			"ltlsynt --print-game-hoa produced no game");
	}
	return r.with_value(std::move(spawned.value()));
}

inline result<std::string> to_dot(const std::string& hoa_text, int timeout_sec) {
	result<std::string> r;
	TAU_TRY(auto tmp, fs::temp_file::create("tau_strategy", hoa_text));
	TAU_TRY(auto dot, spawn_capture({"autfilt", "--dot", tmp.path()},
		timeout_sec, [](int c) { return c == 0; }));
	return r.with_value(std::move(dot));
}

inline result<bool> is_tautology(const std::string& formula, int timeout_sec) {
	result<bool> r;
	// a file, as in synthesize: a long formula passed inline exceeds
	// MAX_ARG_STRLEN
	TAU_TRY(auto tmp, fs::temp_file::create("tau_lang", formula + "\n"));
	TAU_TRY(auto out, spawn_capture({"ltlfilt", "-F", tmp.path()},
		timeout_sec, [](int c) { return c == 0; }));
	while (!out.empty() && std::isspace((unsigned char) out.back()))
		out.pop_back();
	return r.with_value(out == "1");
}

inline bool available() {
	static const bool avail = [] {
		auto probe = spawn_capture({"ltlsynt", "--version"}, 0,
			[](int) { return true; });
		return probe.has_value() ||
			!report_has_code(probe.report(), code::not_found);
	}();
	return avail;
}

} // namespace idni::tau_lang
