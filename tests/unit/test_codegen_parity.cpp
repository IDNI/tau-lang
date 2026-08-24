// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Black-box codegen parity: for every tests/codegen_specs/*.tau fixture, run
// the spec through the interpreter (the `tau` CLI) and through its
// compile_spec-built artifact, feed both the same stdin, and diff what each
// prints. Both sides write through console_prompt_output_stream's
// "name[t] := value" wire format (io_context.tmpl.h), so the comparison never
// parses or introspects the pipeline -- it only reads two stdout streams.
//
// Corpus discovery: tests/codegen_specs/*.tau, globbed (adding a case never
// touches this file). A sibling tests/codegen_specs/<name>.in, if present, is
// the literal stdin fed to both sides -- one value per line, in prompt order;
// absent means the spec has no inputs (empty stdin).
//
// Two tiers:
//
//   Always on: the corpus is well-formed -- every fixture readable and
//   non-empty, every .in belongs to a fixture. No solving, no compiling.
//
//   Opt-in (TAU_CODEGEN_RUN_PARITY_TEST): compile_spec the fixture, run both
//   the CLI and the artifact over the same stdin, and compare stdout after
//   stripping each side's own leading and trailing framing (everything
//   before the first, and after the last, "name[t] := value" line); the
//   interior between them -- including chatter both sides print, such as
//   "Execution step: N" -- is compared verbatim. Stderr is recorded and
//   reported, never asserted. A compile_spec refusal is a real check: the
//   CLI run must also show no console-stream output. An artifact that runs
//   successfully but prints no console-stream line is a real failure -- the
//   one artifact shape is always stream-based.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "tau_compile.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <vector>

using namespace idni::tau_lang;

namespace {

namespace fs = std::filesystem;

// Reserved for the deferred revisable-artifact (PWR) work; not yet codegen-able.
const std::string excluded_fixture = "interpreter_pwr_update_stream";

// Corpus directory, tried both from the ctest working directory (tests/) and
// the repo root (a manually invoked binary's likely cwd).
std::optional<fs::path> codegen_specs_dir() {
	for (const char* prefix : {"codegen_specs", "tests/codegen_specs"}) {
		fs::path dir(prefix);
		if (fs::exists(dir) && fs::is_directory(dir)) return dir;
	}
	return std::nullopt;
}

std::string read_file(const fs::path& p) {
	std::ifstream f(p, std::ios::binary);
	if (!f) return "";
	std::ostringstream ss;
	ss << f.rdbuf();
	return ss.str();
}

std::vector<fs::path> discover_fixtures(const fs::path& dir) {
	std::vector<fs::path> found;
	for (auto& e : fs::directory_iterator(dir))
		if (e.is_regular_file() && e.path().extension() == ".tau")
			found.push_back(e.path());
	std::sort(found.begin(), found.end());
	return found;
}

// TAU_CODEGEN_PARITY_ONLY: comma-separated fixture basenames (no .tau) that
// restrict the opt-in loop to; unset runs the corpus unfiltered, as before.
std::vector<fs::path> filter_parity_only(std::vector<fs::path> fixtures) {
	const char* v = std::getenv("TAU_CODEGEN_PARITY_ONLY");
	if (!v || !*v) return fixtures;
	std::vector<fs::path> kept;
	std::stringstream ss(v);
	for (std::string name; std::getline(ss, name, ','); ) {
		if (name.empty()) continue;
		auto it = std::find_if(fixtures.begin(), fixtures.end(),
			[&](const fs::path& p) { return p.stem().string() == name; });
		REQUIRE_MESSAGE(it != fixtures.end(),
			"TAU_CODEGEN_PARITY_ONLY: no fixture named \"" << name << "\"");
		kept.push_back(*it);
	}
	return kept;
}

// console_prompt_output_stream::put's wire format: "name[t] := value".
bool is_console_output_line(const std::string& line) {
	static const std::regex re(R"(^[A-Za-z_][A-Za-z0-9_]*\[[0-9]+\]\s*:=)");
	return std::regex_search(line, re);
}

// Drop everything before the first and after the last console-output-shaped
// line; the interior between them (whatever it is) stays verbatim, and
// trailing whitespace/newlines are trimmed. "" if the text has no such line.
std::string extract_console_body(const std::string& text) {
	std::optional<size_t> first_start, last_end;
	size_t line_start = 0;
	while (line_start <= text.size()) {
		size_t nl = text.find('\n', line_start);
		size_t line_end = nl == std::string::npos ? text.size() : nl;
		std::string line = text.substr(line_start, line_end - line_start);
		if (is_console_output_line(line)) {
			if (!first_start) first_start = line_start;
			last_end = line_end;
		}
		if (nl == std::string::npos) break;
		line_start = nl + 1;
	}
	if (!first_start) return "";
	std::string body = text.substr(*first_start, *last_end - *first_start);
	while (!body.empty()
		&& std::isspace(static_cast<unsigned char>(body.back())))
		body.pop_back();
	return body;
}

bool has_console_output(const std::string& text) {
	return !extract_console_body(text).empty();
}

// Opt-in: mirrors TAU_CODEGEN_RUN_SDK_LINK_TEST -- drives a real cmake
// configure+build per case, so it stays out of the default ctest budget.
bool run_parity_test() {
	const char* v = std::getenv("TAU_CODEGEN_RUN_PARITY_TEST");
	return v && *v && std::string(v) != "0";
}

// The tau CLI binary: an env override, else the path baked in at configure
// time from the tau target (tests/unit/CMakeLists.txt), when built.
std::optional<std::string> resolve_tau_exe() {
	if (const char* e = std::getenv("TAU_CODEGEN_TAU_EXE"); e && *e)
		return fs::exists(e) ? std::optional(std::string(e)) : std::nullopt;
#ifdef TAU_CLI_EXE_PATH
	if (fs::exists(TAU_CLI_EXE_PATH)) return std::string(TAU_CLI_EXE_PATH);
#endif
	return std::nullopt;
}

struct proc_result { std::string out, err; int exit_code = -1; };

// Run `exe_cmd`, feeding it stdin_file's content, into separate stdout/stderr
// captures identified by `tag` (unique per fixture and side).
proc_result run_piped(const std::string& exe_cmd, const fs::path& stdin_file,
	const std::string& tag)
{
	fs::path out_path = fs::temp_directory_path() / ("_tau_cg_parity_" + tag + ".out");
	fs::path err_path = fs::temp_directory_path() / ("_tau_cg_parity_" + tag + ".err");
	std::string cmd = exe_cmd
		+ " < \"" + stdin_file.string() + "\""
		+ " > \"" + out_path.string() + "\""
		+ " 2> \"" + err_path.string() + "\"";
	int status = std::system(cmd.c_str());
	proc_result r;
	r.exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
	r.out = read_file(out_path);
	r.err = read_file(err_path);
	std::error_code ec;
	fs::remove(out_path, ec);
	fs::remove(err_path, ec);
	return r;
}

// tests/codegen_specs/<name>.in sibling (literal stdin, absent = empty),
// written to a temp file so both sides read the exact same bytes.
fs::path write_stdin_tape(const fs::path& spec_path, const std::string& tag) {
	fs::path in_path = spec_path;
	in_path.replace_extension(".in");
	std::string content = fs::exists(in_path) ? read_file(in_path) : "";
	fs::path tape_path = fs::temp_directory_path() / ("_tau_cg_parity_" + tag + ".stdin");
	std::ofstream f(tape_path, std::ios::binary);
	f << content;
	return tape_path;
}

long long elapsed_ms(std::chrono::steady_clock::time_point t0) {
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::steady_clock::now() - t0).count();
}

// One value a side printed (or was fed) at a step: "name[t]" -- input or
// output resolved from the spec -- holds `value` in the BA's own printed
// syntax (e.g. "bot", "-1/2", "42").
struct ground_assignment { std::string name; int_t t; std::string value; };

// Parses one console-output-shaped line ("name[t] := value",
// is_console_output_line's own shape) into a ground_assignment;
// std::nullopt if it doesn't match.
std::optional<ground_assignment> parse_ground_assignment_line(const std::string& line) {
	static const std::regex re(
		R"(^([A-Za-z_][A-Za-z0-9_]*)\[([0-9]+)\]\s*:=\s*(.*)$)");
	std::smatch m;
	if (!std::regex_match(line, m, re)) return std::nullopt;
	std::string value = m[3].str();
	while (!value.empty()
		&& std::isspace(static_cast<unsigned char>(value.back())))
		value.pop_back();
	return ground_assignment{ m[1].str(),
		static_cast<int_t>(std::stoll(m[2].str())), value };
}

// Every "name[t] := value" line in a console body (extract_console_body's
// output), in order; chatter lines between them are skipped.
std::vector<ground_assignment> extract_output_trace(const std::string& body) {
	std::vector<ground_assignment> trace;
	std::istringstream iss(body);
	std::string line;
	while (std::getline(iss, line))
		if (auto a = parse_ground_assignment_line(line)) trace.push_back(*a);
	return trace;
}

// The stdin tape as one ground_assignment per line, round-robined across
// `io_vars`' distinct input-variable names (sorted, for determinism) at
// successive time steps -- exact for the corpus's usual single-input-stream
// fixtures, an approximation for fixtures with more than one input stream.
std::vector<ground_assignment> stdin_tape_trace(const trefs& io_vars,
	const std::string& tape)
{
	std::vector<std::string> names;
	for (tref v : io_vars)
		if (is_input_var<node_t>(v)) {
			const std::string& n = get_var_name<node_t>(v);
			if (std::find(names.begin(), names.end(), n) == names.end())
				names.push_back(n);
		}
	std::sort(names.begin(), names.end());
	std::vector<ground_assignment> trace;
	if (names.empty()) return trace;
	std::istringstream iss(tape);
	std::string line;
	size_t i = 0;
	while (std::getline(iss, line)) {
		while (!line.empty() && line.back() == '\r') line.pop_back();
		trace.push_back({ names[i % names.size()],
			static_cast<int_t>(i / names.size()), line });
		++i;
	}
	return trace;
}

// Admissibility: is spec_src satisfiable together with every ground
// assignment in output_trace (the side's printed values) and stdin_tape
// (the input fed to both sides), each value parsed back through its io
// var's own BA (ba_constants::get, the same string->constant path
// interpreter<node>::read uses)? std::nullopt when the spec or a value
// can't be resolved/parsed, or the solver can't decide -- callers must
// treat that as "can't decide", never as a pass.
//
// Inputs are grounded by substitution rather than conjoined as equalities:
// is_tau_formula_sat universally quantifies input streams still present in
// the checked formula, so a conjoined input equality is generally
// unsatisfiable together with that quantification; substituting a matched
// occurrence away removes it from scope instead. Output values stay
// conjoined as equalities, which is the existential question
// is_tau_formula_sat answers correctly. Note: a substitution keyed on an
// absolute-time node only matches an input occurrence already at a
// constant time point -- a relative ("t"/"t-k") occurrence inside a
// temporal formula is left unconstrained rather than grounded.
std::optional<bool> trace_is_admissible(const std::string& spec_src,
	const std::vector<ground_assignment>& output_trace,
	const std::string& stdin_tape = "")
{
	using tau = tree<node_t>;
	try {
		// compile_spec's own isolation (tau_compile.tmpl.h): a spec's
		// parse+type stage must start from and leave behind a clean
		// process-wide type scope, or one fixture's io_var types leak
		// into the next (see "parse+infer starts from a clean type
		// scope..." above) -- this runs mid-loop over the same corpus.
		compile_detail::scoped_clean_definitions<node_t> clean_defs;
		tref spec_tree = api<node_t>::get_spec(spec_src);
		if (!spec_tree) return std::nullopt;
		auto nso_rr = get_nso_rr<node_t>(spec_tree);
		if (!nso_rr.has_value()) return std::nullopt;
		tref spec_fm = nso_rr.value().main->get();
		if (!spec_fm) return std::nullopt;

		trefs io_vars = tau::get(spec_fm)
			.select_top(is_child<node_t, tau::io_var>);

		std::vector<ground_assignment> trace = output_trace;
		auto input_trace = stdin_tape_trace(io_vars, stdin_tape);
		trace.insert(trace.end(), input_trace.begin(), input_trace.end());

		assignment<node_t> input_subs;
		std::vector<std::pair<tref, tref>> output_eqs;
		for (const auto& a : trace) {
			tref found = nullptr;
			for (tref v : io_vars)
				if (get_var_name<node_t>(v) == a.name) {
					found = v;
					break;
				}
			if (!found) return std::nullopt;
			size_t type_id = tau::get(found).get_ba_type();
			if (type_id == 0) return std::nullopt;
			tref type_tree = tau::get(found).get_ba_type_tree();
			auto cnst = ba_constants<node_t>::get(a.value, type_tree);
			if (!cnst) return std::nullopt;
			tref const_bf = build_bf_ba_constant<node_t>(
				cnst.value().first, type_id);
			if (is_input_var<node_t>(found)) {
				tref var_bf = build_in_var_at_n<node_t>(
					a.name, a.t, type_id);
				input_subs[var_bf] = const_bf;
			} else {
				tref var_bf = build_out_var_at_n<node_t>(
					a.name, a.t, type_id);
				output_eqs.emplace_back(var_bf, const_bf);
			}
		}
		tref conj = input_subs.empty() ? spec_fm
			: rewriter::replace<node_t>(spec_fm, input_subs);
		for (auto& [var_bf, const_bf] : output_eqs)
			conj = build_wff_and<node_t>(conj,
				build_bf_eq<node_t>(var_bf, const_bf));
		return is_tau_formula_sat<node_t>(conj);
	} catch (...) {
		return std::nullopt;
	}
}

// compile_spec<Node>'s step 1 (tau_compile.tmpl.h) under the same
// scoped_clean_definitions guard compile_spec wraps its whole body in --
// parse + infer, stopping well before the cmake configure+build. Exercises
// the exact isolation mechanism compile_spec relies on without paying for an
// artifact build.
tref parse_like_compile_spec_step1(const std::string& src) {
	compile_detail::scoped_clean_definitions<node_t> clean_defs;
	if (tref spec_tree = api<node_t>::get_spec(src); spec_tree)
		if (auto nso_rr = get_nso_rr<node_t>(spec_tree); nso_rr)
			if (tref applied = nso_rr_apply<node_t>(*nso_rr); applied)
				return normalizer<node_t>(applied);
	return nullptr;
}

// always_one and atomless2's specs verbatim (tests/codegen_specs/*.tau) --
// the pair that proved the leak: both bind o1, always_one as bv[8] and
// atomless2 as tau, so hash-consing makes their o1 tree nodes pointer-equal
// and a stale global_scope entry from one spec corrupts the other's typing.
const std::string always_one_src = "G(o1[t]:bv = { 1 }:bv[8])";
const std::string atomless2_src =
	"G (\n"
	"    ! (o1[t]:tau = {T.}:tau)\n"
	" && ! (o1[t]:tau = {F.}:tau)\n"
	" && ! (o2[t]:tau = {T.}:tau)\n"
	" && ! (o2[t]:tau = {F.}:tau)\n"
	" && ! (o1[t]:tau = o2[t]:tau)\n"
	" && ! (o1[t]:tau = o1[t-1]:tau)\n"
	" && ! (o1[t]:tau = o1[t-2]:tau)\n"
	" && ! (o2[t]:tau = o2[t-1]:tau)\n"
	" && ! (o2[t]:tau = o2[t-2]:tau)\n"
	" && ! (o1[t]:tau = o2[t-1]:tau)\n"
	" && ! (o2[t]:tau = o1[t-1]:tau)\n"
	" && ! (o1[t]:tau = o2[t-2]:tau)\n"
	" && ! (o2[t]:tau = o1[t-2]:tau)\n"
	" && ! (o1[t]:tau = i1[t]:tau)\n"
	" && ! (o1[t]:tau = i1[t-1]:tau)\n"
	" && ! (o1[t]:tau = i1[t-2]:tau)\n"
	" && ! (o2[t]:tau = i2[t]:tau)\n"
	" && ! (o2[t]:tau = i2[t-1]:tau)\n"
	" && ! (o2[t]:tau = i2[t-2]:tau)\n"
	" && ! (o2[t]:tau = i1[t]:tau)\n"
	").";

} // namespace

TEST_SUITE("codegen_parity") {

	// Checks the corpus is well-formed, not parity: every fixture is
	// readable/non-empty and every .in belongs to a fixture. No solving, no
	// compiling, no subprocess -- stays in the millisecond range.
	TEST_CASE("codegen_specs corpus is well-formed") {
		auto dir = codegen_specs_dir();
		REQUIRE_MESSAGE(dir.has_value(), "codegen_specs directory not found");
		auto fixtures = discover_fixtures(*dir);
		REQUIRE_MESSAGE(!fixtures.empty(),
			"no .tau fixtures found under " << dir->string());

		std::vector<std::string> stems;
		for (auto& p : fixtures) {
			std::string src = read_file(p);
			CHECK_MESSAGE(!src.empty(),
				p.string() << ": fixture is empty or unreadable");
			stems.push_back(p.stem().string());
		}
		for (auto& e : fs::directory_iterator(*dir)) {
			if (!e.is_regular_file() || e.path().extension() != ".in") continue;
			std::string stem = e.path().stem().string();
			CHECK_MESSAGE(
				std::find(stems.begin(), stems.end(), stem) != stems.end(),
				e.path().string() << ": .in file has no matching .tau fixture");
		}
	}

	// Always on: a spec's parse+infer stage starts from and leaves behind a
	// clean definitions<node> type scope, so in-process compile order never
	// matters. Regression for the process-wide global_scope leak that made
	// atomless2 fail whenever it compiled right after always_one in the same
	// process (o1 hash-conses across specs; a stale bv[8] type from
	// always_one collided with atomless2's tau typing). No cmake
	// configure+build, no subprocess -- seconds, not minutes (always_one's
	// bv[8] normalization drives CVC5 in-process).
	TEST_CASE("parse+infer starts from a clean type scope regardless of prior compiles") {
		CHECK_MESSAGE(parse_like_compile_spec_step1(always_one_src) != nullptr,
			"always_one: failed to parse+infer standalone");
		CHECK_MESSAGE(parse_like_compile_spec_step1(atomless2_src) != nullptr,
			"atomless2: failed to parse+infer standalone");

		CHECK_MESSAGE(parse_like_compile_spec_step1(always_one_src) != nullptr,
			"always_one -> atomless2 order: always_one failed");
		CHECK_MESSAGE(parse_like_compile_spec_step1(atomless2_src) != nullptr,
			"always_one -> atomless2 order: atomless2 failed right after always_one");

		CHECK_MESSAGE(parse_like_compile_spec_step1(atomless2_src) != nullptr,
			"atomless2 -> always_one order: atomless2 failed");
		CHECK_MESSAGE(parse_like_compile_spec_step1(always_one_src) != nullptr,
			"atomless2 -> always_one order: always_one failed right after atomless2");
	}

	// Always on: trace_is_admissible on the graded comparison's calibration
	// case (qlt_bv_cross_type.tau) -- an admissible trace under each side's
	// own rendering of the same witness must come out true, and a trace
	// that actually violates the spec must come out false. No solving of a
	// full corpus, no subprocess -- one is_tau_formula_sat call per check.
	TEST_CASE("trace_is_admissible: calibration spec") {
		const std::string spec_src =
			"G (o1[t]:qlt > {1/2}:qlt -> o2[t]:bv[8] = {42}:bv[8]).";

		// CLI's rendering of the witness: o1 prints as "bot".
		auto cli_trace = trace_is_admissible(spec_src,
			{ {"o1", 0, "bot"}, {"o2", 0, "0"} });
		REQUIRE_MESSAGE(cli_trace.has_value(),
			"admissibility check could not decide the CLI-style trace");
		CHECK_MESSAGE(*cli_trace,
			"CLI-style trace (o1=bot, o2=0) should be admissible");

		// artifact's rendering of the same witness: o1 prints as "-1/2".
		auto artifact_trace = trace_is_admissible(spec_src,
			{ {"o1", 0, "-1/2"}, {"o2", 0, "42"} });
		REQUIRE_MESSAGE(artifact_trace.has_value(),
			"admissibility check could not decide the artifact-style trace");
		CHECK_MESSAGE(*artifact_trace,
			"artifact-style trace (o1=-1/2, o2=42) should be admissible");

		// violating trace: o1=1 > 1/2 forces o2=42, but o2=7.
		auto violating_trace = trace_is_admissible(spec_src,
			{ {"o1", 0, "1"}, {"o2", 0, "7"} });
		REQUIRE_MESSAGE(violating_trace.has_value(),
			"admissibility check could not decide the violating trace");
		CHECK_MESSAGE(!*violating_trace,
			"trace (o1=1, o2=7) violates the spec and must be inadmissible");
	}

	// Opt-in: drives a real compile_spec (cmake configure+build) and two
	// process runs per fixture -- minutes, not milliseconds.
	TEST_CASE("compile+run vs interpreter over the codegen_specs corpus") {
		if (!run_parity_test()) {
			MESSAGE("TAU_CODEGEN_RUN_PARITY_TEST not set; skipping the "
				"compile+run comparison (drives cmake configure+build "
				"per case)");
			return;
		}
		auto tau_exe = resolve_tau_exe();
		if (!tau_exe) {
			MESSAGE("tau CLI binary not found (build a *-tau/*-all preset, "
				"or set TAU_CODEGEN_TAU_EXE); skipping");
			return;
		}
		auto dir = codegen_specs_dir();
		REQUIRE_MESSAGE(dir.has_value(), "codegen_specs directory not found");
		auto fixtures = filter_parity_only(discover_fixtures(*dir));
		REQUIRE_MESSAGE(!fixtures.empty(),
			"no .tau fixtures found under " << dir->string());

		for (auto& spec_path : fixtures) {
			std::string name = spec_path.stem().string();
			if (name == excluded_fixture) {
				MESSAGE(name, ": excluded (reserved for the deferred "
					"revisable-artifact/PWR work)");
				continue;
			}
			std::string src = read_file(spec_path);
			REQUIRE_MESSAGE(!src.empty(),
				name << ": spec file not found or empty");

			fs::path stdin_file = write_stdin_tape(spec_path, name);
			std::error_code ec;

			auto t0 = std::chrono::steady_clock::now();
			auto cli = run_piped("\"" + *tau_exe + "\" \"" + spec_path.string()
				+ "\" -q -b off", stdin_file, name + "_cli");
			auto cli_ms = elapsed_ms(t0);

			fs::path build_dir = fs::temp_directory_path()
				/ ("_tau_cg_parity_build_" + name);
			fs::remove_all(build_dir, ec);
			auto t1 = std::chrono::steady_clock::now();
			auto res = compile_spec<node_t>(src, "", build_dir.string());
			auto compile_ms = elapsed_ms(t1);
			MESSAGE(name, ": tau run ", cli_ms, " ms, compile_spec ",
				compile_ms, " ms");

			if (!res.ok()) {
				// Refusal parity: the artifact didn't build, so the CLI run
				// must show no console-stream output either.
				CHECK_MESSAGE(!has_console_output(cli.out),
					name << ": compile_spec refused (" << res.error
					     << ") but the CLI printed console output:\n"
					     << cli.out);
				MESSAGE(name, ": compile_spec refused: ", res.error);
				fs::remove_all(build_dir, ec);
				fs::remove(stdin_file, ec);
				continue;
			}

			auto t2 = std::chrono::steady_clock::now();
			auto artifact = run_piped("\"" + res.exe_path + "\"", stdin_file,
				name + "_artifact");
			auto run_ms = elapsed_ms(t2);
			MESSAGE(name, ": artifact run ", run_ms, " ms, exit ",
				artifact.exit_code);

			std::string cli_body = extract_console_body(cli.out);
			std::string artifact_body = extract_console_body(artifact.out);
			if (cli_body == artifact_body) {
				MESSAGE(name, ": pass (exact)");
			} else {
				// Graded fallback: an exact-text mismatch is not
				// necessarily a real disagreement -- ask whether each
				// side's trace is still admissible under the spec (e.g.
				// qlt's "bot" vs "-1/2" print the same admissible
				// witness differently).
				fs::path in_path = spec_path;
				in_path.replace_extension(".in");
				std::string tape = fs::exists(in_path) ? read_file(in_path) : "";
				auto cli_ok = trace_is_admissible(src,
					extract_output_trace(cli_body), tape);
				auto artifact_ok = trace_is_admissible(src,
					extract_output_trace(artifact_body), tape);
				if (!cli_ok.has_value() || !artifact_ok.has_value()) {
					CHECK_MESSAGE(false,
						name << ": stdout mismatch (admissibility "
						"undecidable)\n--- tau body ---\n" << cli_body
						<< "\n--- artifact body ---\n" << artifact_body);
				} else if (*cli_ok && *artifact_ok) {
					MESSAGE(name, ": pass (containment) -- stdout "
						"differs but both traces are admissible under "
						"the spec\n--- tau body ---\n", cli_body,
						"\n--- artifact body ---\n", artifact_body);
				} else if (*cli_ok && !*artifact_ok) {
					CHECK_MESSAGE(false,
						name << ": artifact trace is inadmissible under "
						"the spec (proven spec violation)\n"
						"--- artifact body ---\n" << artifact_body);
				} else if (!*cli_ok && *artifact_ok) {
					CHECK_MESSAGE(false,
						name << ": CLI trace is inadmissible under the "
						"spec (proven spec violation)\n"
						"--- tau body ---\n" << cli_body);
				} else {
					CHECK_MESSAGE(false,
						name << ": both traces are inadmissible under "
						"the spec\n--- tau body ---\n" << cli_body
						<< "\n--- artifact body ---\n" << artifact_body);
				}
			}

			if (cli.err != artifact.err)
				MESSAGE(name, ": stderr differs (not asserted)\n"
					"--- tau stderr ---\n", cli.err,
					"\n--- artifact stderr ---\n", artifact.err);

			fs::remove_all(build_dir, ec);
			fs::remove(stdin_file, ec);
		}
	}

	// Opt-in (TAU_PHI_DELTA_SWAP_MEASURE=1): synthesis-time wall clock for
	// parse->normalize->solve_ltl_aba (compile_spec's step 1+2, no artifact
	// build -- the parse_like_compile_spec_step1 pattern above, extended one
	// step further) on hello_world and atomless2, plus the Mechanism 1(a)
	// shadow-crosscheck's own eligible/agree/disagree counters when
	// TAU_PHI_DELTA_CROSSCHECK=1 is also set (ltl_aba_normalization.tmpl.h's
	// ocltl_swap_stats()). The crosscheck never changes solve_ltl_aba's own
	// answer (shadow-only, see that file's dated note); this only measures
	// it. Informational on timing; asserts zero disagreements among whatever
	// calls were eligible, since a disagreement is this batch's designated
	// stop condition, not a rate to tolerate.
	TEST_CASE("Mechanism 1(a) shadow crosscheck: synthesis-time swap measurement") {
		if (!std::getenv("TAU_PHI_DELTA_SWAP_MEASURE")) {
			MESSAGE("TAU_PHI_DELTA_SWAP_MEASURE not set; skipping "
				"(set TAU_PHI_DELTA_CROSSCHECK=1 alongside it for the "
				"correctness counters, off for a clean timing read)");
			return;
		}
		auto dir = codegen_specs_dir();
		REQUIRE_MESSAGE(dir.has_value(), "codegen_specs directory not found");

		for (const char* name : { "hello_world", "atomless2" }) {
			fs::path spec_path = *dir / (std::string(name) + ".tau");
			std::string src = read_file(spec_path);
			REQUIRE_MESSAGE(!src.empty(), name << ": spec file not found or empty");

			auto& stats = ocltl_swap_stats();
			size_t total0 = stats.total_calls, elig0 = stats.eligible,
				inelig0 = stats.ineligible, agree0 = stats.agree,
				dis0 = stats.disagree;

			auto t0 = std::chrono::steady_clock::now();
			tref fm = parse_like_compile_spec_step1(src);
			REQUIRE_MESSAGE(fm != nullptr, name << ": parse+normalize failed");
			auto sol = solve_ltl_aba<node_t>(fm);
			auto ms = elapsed_ms(t0);

			size_t total = stats.total_calls - total0,
				elig = stats.eligible - elig0, inelig = stats.ineligible - inelig0,
				agree = stats.agree - agree0, dis = stats.disagree - dis0;
			MESSAGE(name, ": solve_ltl_aba ", ms, " ms, realizable=",
				sol.has_value(), ", aba_existential_feasible calls=", total,
				", phi_delta-eligible=", elig,
				", ineligible=", inelig, ", agree=", agree, ", disagree=", dis);
			if (elig > 0)
				CHECK_MESSAGE(dis == 0,
					name << ": " << dis << " phi_delta swap disagreement(s) "
					"out of " << elig << " eligible calls -- see the "
					"LOG_ERROR output above for the offending formula(s)");
		}
	}
}
