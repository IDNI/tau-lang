// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <filesystem>
#include <iostream>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <optional>
#include <fstream>
#include <sstream>

#ifdef TAU_WASM_NODE_CLI
#include <unistd.h>
#endif

#include "tau_pack.h"

#ifdef DEBUG
// including nso_ba, sbf_ba and interpreter directly instead of
// #include "tau.h" to avoid error lines pointing to a generated tau.h
#include "boolean_algebras/nso_ba.h"
#include "boolean_algebras/variant_ba.h"
#include "base_ba_dispatcher.h"
#include "api.h"
#else
#	include "tau.h"
#endif // DEBUG
#include "cli_options.h"
#include "repl_evaluator.h"
#include "tau_compile.h"
#include "utility/cli.h"

using namespace std;
using namespace idni;
using namespace idni::tau_lang;

using node_t = tau_lang::tau_pack::node_t;
using tau = tree<node_t>;
using tau_api = api<node_t>;

/// @brief The option table of the tau executable.
///
/// Every runtime limit flag defaults to the empty string, which means "not
/// given": main() then leaves the limit at the value its TAU_* environment
/// variable or its default gave it, both named in the flag's description. A
/// BA-declared option of the configured pack is added as
/// `--<family>-<option>`.
/// @return The options, keyed by long name.
cli::options tau_options() {
	cli::options opts = tau_cli_options(cli_option_set::full);
	// The default is the library's `preprocessing` global: a value of the
	// CLI's own would override the library for every plain `tau` run.
	opts["preprocessing"] = cli::option("preprocessing", 'B', preprocessing)
		.set_description(std::string("BA preprocessing, e.g. bv predicate "
			"blasting (")
			+ (preprocessing ? "enabled" : "disabled")
			+ " by default)");
	opts["ba-component-factoring"] = cli::option("ba-component-factoring",
		'K', ba_component_factoring)
		.set_description(std::string("decide tau-algebra constants per "
			"support component (")
			+ (ba_component_factoring ? "enabled" : "disabled")
			+ " by default)");
	opts["spec-size-warn"] = cli::option("spec-size-warn", 'w', "")
		.set_description("warn when an updated specification exceeds "
			"this many characters (default: TAU_SPEC_SIZE_WARN or 0; "
			"0 = off)");
	opts["pwr-semantic"] = cli::option("pwr-semantic", 'Z', false)
		.set_description("enable the semantic (winning-region) fallback "
			"of the temporal pointwise revision (off by default)");
	opts["step-definitional-propagation"] =
		cli::option("step-definitional-propagation", 't', true)
		.set_description("propagate the constants a step formula determines "
			"before its paths are enumerated (enabled by default)");
	opts["max-revision-alts"] = cli::option("max-revision-alts", 'a', "")
		.set_description("cap the revision alternatives kept per "
			"specification part, dropping middle preference tiers "
			"(default: TAU_MAX_REVISION_ALTS or 0; 0 = unlimited)");
	opts["block-max-splits"] = cli::option("block-max-splits", 'p', "")
		.set_description("cap per-block Boole-decomposition splits in "
			"anti-prenexing (default: TAU_BLOCK_MAX_SPLITS or 0; "
			"0 = unlimited)");
	opts["ba-decision-pins"] = cli::option("ba-decision-pins", 'N', "")
		.set_description("decided tau-algebra rows whose key tree is kept "
			"alive across the step sweep (default: "
			"TAU_BA_DECISION_PINS or 4096; 0 = none)");
	opts["block-max-rounds"] = cli::option("block-max-rounds", 'r', "")
		.set_description("cap anti-prenexing quantifier-block driver "
			"rounds (default: TAU_BLOCK_MAX_ROUNDS or 0; "
			"0 = unlimited)");
	opts["cqe-max-clauses"] = cli::option("cqe-max-clauses", 'Q', "")
		.set_description("cap the DNF clauses complete quantifier "
			"elimination may distribute one scope into (default: "
			"TAU_CQE_MAX_CLAUSES or 0; 0 = unlimited)");
	opts["lgrs-max-vars"] = cli::option("lgrs-max-vars", 'g', "")
		.set_description("hand a pure-equality bitvector system with more "
			"distinct variables than this to the solver instead of "
			"solving it algebraically per width (default: "
			"TAU_LGRS_MAX_VARS or 8; 0 = unlimited)");
	opts["max-fixpoint-steps"] = cli::option("max-fixpoint-steps", 'f', "")
		.set_description("cap temporal-normalization fixpoint steps "
			"(default: TAU_MAX_FIXPOINT_STEPS or 500; 0 = unlimited)");
	opts["max-flag-search-steps"] =
		cli::option("max-flag-search-steps", 'F', "")
		.set_description("cap the eventual-flag search past the flag "
			"boundary; a give-up reports an error, not a verdict "
			"(default: TAU_MAX_FLAG_SEARCH_STEPS or 500; "
			"0 = unlimited)");
	opts["block-squeeze-cap"] = cli::option("block-squeeze-cap", 'z', "")
		.set_description("skip block squeezing above this operand-set "
			"size (default: TAU_BLOCK_SQUEEZE_CAP or 0; 0 = unlimited)");
	opts["max-simplify-rounds"] =
		cli::option("max-simplify-rounds", 'm', "")
		.set_description("cap bitvector simplification rewrite rounds "
			"(default: TAU_MAX_SIMPLIFY_ROUNDS or 0; 0 = unlimited)");
	opts["max-def-passes"] = cli::option("max-def-passes", 'P', "")
		.set_description("cap definition-expansion passes "
			"(default: TAU_MAX_DEF_PASSES or 0; 0 = unlimited)");
	opts["max-probe-steps"] = cli::option("max-probe-steps", 'M', "")
		.set_description("cap the untyped saturation probe over a residual "
			"recurrence reference (default: TAU_MAX_PROBE_STEPS or "
			"10000; 0 = unlimited)");
	opts["max-enum-steps"] = cli::option("max-enum-steps", 'E', "")
		.set_description("cap recurrence-relation enumeration steps "
			"(default: TAU_MAX_ENUM_STEPS or 0; 0 = unlimited)");
	opts["max-rewrite-rounds"] = cli::option("max-rewrite-rounds", 'R', "")
		.set_description("cap rewrite-to-fixpoint rounds "
			"(default: TAU_MAX_REWRITE_ROUNDS or 0; 0 = unlimited)");
	// The k-ary walk cap ships FINITE: the walk is 2^n synthesis checks
	// on mostly-feasible atoms; a fired cap is sound (false UNREALIZABLE
	// at worst, warned loudly), an uncapped walk is a hang.
	opts["max-consistency-subsets"] =
		cli::option("max-consistency-subsets", 'j', "")
		.set_description("cap k-ary consistency subset checks per atom "
			"group (default: TAU_MAX_CONSISTENCY_SUBSETS or 4096; "
			"0 = unlimited)");
	opts["max-cover-products"] =
		cli::option("max-cover-products", 'n', "")
		.set_description("cap the ABA oracle's mixed-type coverage "
			"expansion (default: TAU_MAX_COVER_PRODUCTS or 256; "
			"0 = unlimited)");
	opts["max-constant-size"] =
		cli::option("max-constant-size", 'u', "")
		.set_description("largest region of fresh values, in tree nodes, "
			"a run keeps across steps (default: TAU_MAX_CONSTANT_SIZE "
			"or 2000; 0 = unlimited)");
	opts["cache-bound"] = cli::option("cache-bound", 'A', "")
		.set_description("bound the string-keyed synthesis caches, "
			"FIFO eviction (default: TAU_CACHE_BOUND or 4096; "
			"0 = unbounded)");
	// An empty default means "not given", so the value of the environment
	// variable stays in force unless the flag is passed.
	opts["ltl-timeout"] = cli::option("ltl-timeout", 'T', "")
		.set_description("wall-clock cap in seconds on each ltlsynt call "
			"(0 = no watchdog; default: TAU_LTL_TIMEOUT or 60)");
	opts["ltl-alg"] = cli::option("ltl-alg", 'L', "")
		.set_description("omcat synthesis algorithm: A, B, D or auto "
			"(default: TAU_LTL_ALG or auto)");
	opts["ltl-qe-max-vars"] = cli::option("ltl-qe-max-vars", 'k', "")
		.set_description("free-variable cap of the omcat QE fast path; "
			"above 2 is not sound (default: TAU_LTL_QE_MAX_VARS or 2; "
			"0 = off)");
	opts["ltl-hoa-max-states"] =
		cli::option("ltl-hoa-max-states", 'Y', "")
		.set_description("largest state count accepted from an ltlsynt "
			"HOA strategy (default: TAU_LTL_HOA_MAX_STATES or "
			"4194304; 0 = unlimited)");
	opts["ltl-guard-max-cubes"] =
		cli::option("ltl-guard-max-cubes", 'U', "")
		.set_description("cap the DNF cubes a HOA guard may expand into "
			"in the Algorithm D game (default: "
			"TAU_LTL_GUARD_MAX_CUBES or 512; 0 = unlimited)");
	opts["ltl-refinement-rounds"] =
		cli::option("ltl-refinement-rounds", 'D', "")
		.set_description("cap the ABA-oracle refinement rounds of a "
			"realizability check; the cap answers UNKNOWN (default: "
			"TAU_LTL_REFINEMENT_ROUNDS or 64; 0 = unlimited)");
	opts["ltl-window-max-paths"] =
		cli::option("ltl-window-max-paths", 'O', "")
		.set_description("cap the strategy paths the multi-step window "
			"oracle examines per check (default: "
			"TAU_LTL_WINDOW_MAX_PATHS or 4096; 0 = unlimited)");
	opts["ltl-closed-regions-timeout"] =
		cli::option("ltl-closed-regions-timeout", '\0', "")
		.set_description("cap in seconds the data game's attempt on regions "
			"that keep their quantifiers, all its questions together, "
			"each at most a quarter of it (default: "
			"TAU_LTL_CLOSED_REGIONS_TIMEOUT or 20; 0 = no such "
			"attempt)");
	opts["ltl-data-game-max-nodes"] =
		cli::option("ltl-data-game-max-nodes", '\0', "")
		.set_description("cap the live nodes of the BDD of a data game "
			"over codes; a full table leaves the game undecided "
			"(default: TAU_LTL_DATA_GAME_MAX_NODES or 8388608; "
			"0 = unlimited)");
	opts["ltl-data-game-max-memo"] =
		cli::option("ltl-data-game-max-memo", '\0', "")
		.set_description("cap the operation memo entries of the BDD of a "
			"data game over codes; a full memo is emptied (default: "
			"TAU_LTL_DATA_GAME_MAX_MEMO or 33554432; 0 = unlimited)");
	opts["ltl-data-game-max-combinations"] =
		cli::option("ltl-data-game-max-combinations", '\0', "")
		.set_description("cap the value combinations the data game "
			"tabulates for one comparison its circuits do not encode "
			"(default: TAU_LTL_DATA_GAME_MAX_COMBINATIONS or 4096; "
			"0 = unlimited)");
	opts["ltl-max-observations"] =
		cli::option("ltl-max-observations", '\0', "")
		.set_description("cap the observation props whose impossible "
			"joint values the synthesis skeleton assumes away (default: "
			"TAU_LTL_MAX_OBSERVATIONS or 8; at most 30, 0 = 30)");
	opts["ltl-mealy-max-states"] =
		cli::option("ltl-mealy-max-states", '\0', "")
		.set_description("most states of the Mealy view a data-game "
			"strategy is played through (default: "
			"TAU_LTL_MEALY_MAX_STATES or 4096; 0 = no view)");
	opts["ltl-mealy-max-edges"] =
		cli::option("ltl-mealy-max-edges", '\0', "")
		.set_description("most edges of the Mealy view a data-game "
			"strategy is played through (default: "
			"TAU_LTL_MEALY_MAX_EDGES or 65536; 0 = no view)");
	opts["compile-max-table-edges"] =
		cli::option("compile-max-table-edges", '\0', "")
		.set_description("most edges of a Mealy view gen/compile carries "
			"as a table instead of solving as the program runs (default: "
			"TAU_COMPILE_MAX_TABLE_EDGES or 400; 0 = none)");
	opts["compile-build-timeout"] =
		cli::option("compile-build-timeout", '\0', "")
		.set_description("seconds the cmake build of compile may take "
			"before it is stopped (default: TAU_COMPILE_BUILD_TIMEOUT "
			"or 3600; 0 = no timeout)");
	opts["bf-dependence-max-nodes"] =
		cli::option("bf-dependence-max-nodes", '\0', "")
		.set_description("cap the BDD nodes built to tell whether a "
			"Boolean function depends on a variable (default: "
			"TAU_BF_DEPENDENCE_MAX_NODES or 65536; 0 = unlimited)");
	opts["tref-budget"] = cli::option("tref-budget", 'y', "")
		.set_description("cap the live interned tree nodes; an api call "
			"that starts with the store at or above the cap fails "
			"instead of running (default: TAU_TREF_BUDGET or 0; "
			"0 = unlimited)");
	opts["tref-budget-soft"] = cli::option("tref-budget-soft", 'C', "")
		.set_description("percentage of --tref-budget at which a sweep "
			"is forced regardless of the gc growth trigger "
			"(default: TAU_TREF_BUDGET_SOFT or 75)");
	opts["gc-min-size"] = cli::option("gc-min-size", 'G', "")
		.set_description("tree-node count floor before gc may trigger "
			"(default: TAU_GC_MIN_SIZE or 256)");
	opts["gc-growth-factor"] = cli::option("gc-growth-factor", 'W', "")
		.set_description("gc triggers when node count grows by this "
			"factor since last sweep (default: TAU_GC_GROWTH_FACTOR or "
			"1.5; <= 0 disables gc)");
	// BA-declared options: one CLI flag per option a BA in the configured
	// pack declares about itself, registered as --<family>-<option> (e.g.
	// --bv-blasting), with default and description taken from the BA's own
	// descriptor.
	for (const auto& e : pack_ba_options<node_t>()) {
		std::string cli_name = e.family + "-" + e.option.name;
		// A count or text option is registered with an empty default
		// on purpose: writing the descriptor's own value back would
		// shadow whatever environment fallback the algebra resolves
		// for itself, and the option's own help text names its default.
		if (e.option.kind == ba_option_kind::flag)
			opts[cli_name] = cli::option(cli_name, '\0',
				e.option.get_flag())
				.set_description(e.option.help);
		else opts[cli_name] = cli::option(cli_name, '\0', "")
			.set_description(e.option.help);
	}
	return opts;
}

/// @brief The verbs of the tau executable: `gen` (with its alias `codegen`)
/// and `compile`, with their options.
/// @return The commands, keyed by name.
cli::commands tau_commands() {
	cli::commands cs;
	cli::command gen("gen",
		"generates the C++ artifact for a Tau spec file (no build)");
	const cli::option gen_output = cli::option("output", 'o', "")
		.set_description("output directory (default: <spec>.build; "
			"only with a single spec file)");
	gen.add_option(gen_output);
	cs[gen.name()] = gen;
	// `codegen` is a second key for the same command: the map is keyed by name
	// and cli has no alias support.
	cli::command codegen("codegen", "another name for gen");
	codegen.add_option(gen_output);
	cs[codegen.name()] = codegen;
	cli::command compile("compile",
		"compiles a Tau spec file into a standalone executable");
	compile.add_option(cli::option("output", 'o', "")
		.set_description("output executable path (default: spec "
			"file path without extension)"));
	compile.add_option(cli::option("cxx", 'c', "")
		.set_description("C++ compiler for the emitted project (default: "
			"TAU_CXX, else cmake's compiler, or the compiler of the "
			"--preset platform)"));
	compile.add_option(cli::option("preset", '\0', "")
		.set_description("target platform or ./dev preset name; without it "
			"tau compiles for this machine"));
	compile.add_option(cli::option("define", 'D', "")
		.set_description("cmake cache variable NAME=VALUE for the emitted "
			"project's configure; repeatable"));
	compile.add_option(cli::option("generator", 'G', "")
		.set_description("cmake generator for the emitted project's "
			"configure (the preset's generator otherwise)"));
	cs[compile.name()] = compile;
	return cs;
}

/// @brief Log @p s as an error.
/// @return 1, the exit status of a failed run.
int error(const string& s) { TAU_LOG_ERROR << "" << s; return 1; }

/// @brief Read the spec file @p spec_file into @p src; "-" reads stdin.
///
/// `tau gen` and `tau compile` share it with the interpreter's own spec-file
/// path.
/// @param spec_file Path of the file, or "-" for stdin.
/// @param src Receives the whole content; untouched when the file cannot be
/// opened.
/// @return False when the file cannot be opened.
bool read_spec_file(const std::string& spec_file, std::string& src) {
	if (spec_file == "-") {
		std::ostringstream oss;
		oss << std::cin.rdbuf(), src = oss.str();
		return true;
	}
	DBG(TAU_LOG_TRACE << "open file: " << spec_file;)
	std::ifstream ifs(spec_file, std::ios::binary | std::ios::ate);
	if (!ifs) return false;
	auto l = ifs.tellg();
	// A spec file's length fits streamsize, which is 32 bits on wasm32.
	auto len = static_cast<std::streamsize>(l);
	// A successfully opened file's tellg is non-negative; resize takes a size_t.
	src.resize(static_cast<size_t>(len));
	if (len > 0) ifs.seekg(0), ifs.read(&src[0], len);
	return true;
}

/// @brief Split `-DNAME=VALUE` and `-GNinja` after the `compile` verb into
/// the option and its value, in place.
///
/// The cli table has no attached short-option value, and these are the
/// spellings `./dev preset` accepts. Arguments before `compile`, and every
/// argument when there is no `compile`, are left alone.
/// @param args The command line, argv[0] first; modified in place.
void expand_attached_short_values(std::vector<std::string>& args) {
	size_t start = args.size();
	for (size_t i = 1; i < args.size(); ++i)
		if (args[i] == "compile") { start = i + 1; break; }
	for (size_t i = start; i < args.size(); ++i) {
		const std::string& a = args[i];
		if (a.size() <= 2 || a[0] != '-' || (a[1] != 'D' && a[1] != 'G'))
			continue;
		// a aliases args[i]: keep the tail before the element is overwritten.
		std::string value = a.substr(2);
		args[i] = a.substr(0, 2);
		// i is a bounded index; the iterator offset is a difference_type.
		args.insert(args.begin() + static_cast<std::ptrdiff_t>(i + 1),
			std::move(value));
		++i;
	}
}

/// @brief The -D and -G arguments `tau compile` forwards to the emitted
/// project's configure.
///
/// The cli table keeps only the last value of a repeated option, so the raw
/// arguments from the verb onward are scanned: a define is -DNAME=VALUE,
/// -D NAME=VALUE or --define NAME=VALUE and may repeat, a generator is
/// -G <gen>, -G<gen> or --generator <gen>.
/// @param args The command line, argv[0] first.
/// @return The arguments in cmake form (`-DNAME=VALUE`, `-G`, `<gen>`), in
/// command-line order; empty when there is no `compile` verb.
std::vector<std::string> collect_compile_extra_args(
	const std::vector<std::string>& args)
{
	std::vector<std::string> extra;
	size_t start = args.size();
	for (size_t i = 1; i < args.size(); ++i)
		if (args[i] == "compile") { start = i + 1; break; }
	for (size_t i = start; i < args.size(); ++i) {
		const std::string& a = args[i];
		if (a == "-D" || a == "--define") {
			if (i + 1 < args.size()) extra.push_back("-D" + args[++i]);
		} else if (a.rfind("-D", 0) == 0 && a.size() > 2) {
			extra.push_back(a);
		} else if (a == "-G" || a == "--generator") {
			if (i + 1 < args.size()) {
				extra.push_back("-G");
				extra.push_back(args[++i]);
			}
		} else if (a.rfind("-G", 0) == 0 && a.size() > 2) {
			extra.push_back("-G");
			extra.push_back(a.substr(2));
		}
	}
	return extra;
}

/// @brief Run the specification in @p spec_file with the interpreter until it
/// ends.
///
/// Prints the setup warnings before the run, and the benchmark report (in
/// plain text, to stderr) after it when `--benchmarks` is on. With `--quit`
/// the run ends when no input is left.
/// @param spec_file Path of the spec file, or "-" for stdin.
/// @param opts The processed command-line options.
/// @return 0 when the file is empty or the run succeeded, 1 otherwise.
int run_tau_spec(string spec_file, cli::options& opts) {
	const bool benchmarks = opts["benchmarks"].get<bool>();
	report rep;
	auto root = rep.open_if(benchmarks, "run");
	string src;
	auto finish = [&](int code) -> int {
		// The root scope must be closed before the report is printed:
		// report::print() rejects a report with open scopes, whose elapsed
		// time is not yet written.
		root.close();
		// Benchmarks stay plain text: the parser's global TC colorizes
		// report::print() output unconditionally, which would corrupt a
		// piped or logged benchmark stream.
		if (benchmarks) {
			bool was_enabled = idni::TC.enabled;
			idni::TC.disable();
			rep.print(std::cerr);
			idni::TC.set(was_enabled);
		}
		return code;
	};
	{
		auto _ = rep.open_if(benchmarks, "reading input");
		if (!read_spec_file(spec_file, src))
			return error("Cannot open file " + spec_file);
	}
	if (src.empty()) return finish(0);
	auto gi = tau_api::get_interpreter(src);
	if (!gi.has_value()) {
		gi.print(std::cerr);
		rep.append(std::move(gi).report());
		return finish(1);
	}
	auto& i = gi.value();
	// Its warnings come before the run; the report prints only with the
	// benchmarks, once the run ends.
	gi.report().print(idni::diagnostics::sinks{
		.error = {}, .warning = &std::cerr, .info = {} });
	rep.append(std::move(gi).report());
	bool quit_on_idle = opts["quit"].get<bool>();
	auto run_ok = tau_api::run(i, quit_on_idle);
	if (!run_ok.has_value()) run_ok.print(std::cerr);
	rep.append(std::move(run_ok).report());
	if (quit_on_idle) TAU_LOG_INFO << "No more inputs provided."
		<< " Terminating.";
	return finish(run_ok.has_value() && run_ok.value() ? 0 : 1);
}

/// @brief Log the REPL's welcome banner.
void welcome() {
	TAU_LOG_INFO << "Welcome to the " << full_version << " by IDNI AG.\n"
		<< "This product is protected by patents and copyright."
			" By using this product, you agree to the license terms."
			" To view the license run \"tau --license\".\n\n"
		<< "For documentation, open issues and reporting issues "
			"please visit https://github.com/IDNI/tau-lang/\n\n"
		<< "For built-in help, type \"help\" or \"help command\".\n\n";
}

// TODO (MEDIUM) add command to read input file,...
/// @brief Entry point of the tau executable.
///
/// Applies every option through its api setter, then runs one of: the `gen`
/// / `codegen` or `compile` verb, the given spec file, the `--evaluate` REPL
/// command, or the interactive REPL (FTXUI unless `--legacy-repl`).
/// @return The exit status: 0 on success, 1 on an error; for `--evaluate`,
/// the REPL's code (0 done, 1 quit or error with error-quits, 2 incomplete
/// input).
int main(int argc, char** argv) {
	auto init = tau_init<node_t>();
	init.print_pending();
	if (!init.has_value()) return 1;
	auto env = idni::options().load_env("TAU_");
	env.print_pending();
	if (!env.has_value()) return 1;

	vector<string> args;
	for (int i = 0; i < argc; i++) args.push_back(argv[i]);
	expand_attached_short_values(args);

	cli cl("tau", args, tau_commands(), "", tau_options());
	cl.set_help_header("Usage: tau [ <specification file> ]");

	if (cl.process_args() != 0) return cl.status();
	auto cmd = cl.get_processed_command();

	auto opts  = cl.get_processed_options();
	auto files = cl.get_files();

	if (opts["help"].get<bool>()) return cl.help(), 0;
	if (opts["version"].get<bool>())
		return std::cout << full_version << "\n"
			<< "algebras: " << node_t::ba::types_joined() << "\n", 0;
	if (opts["license"].get<bool>()) return std::cout << license, 0;

	std::string sevstr = opts["severity"].get<string>();
	boost::log::trivial::severity_level sev = tau_cli_parse_severity(sevstr);


	tau_api::set_highlighting(opts["highlighting"].get<bool>());
	tau_api::set_indenting(opts["indenting"].get<bool>());
	tau_api::set_json(opts["json"].get<bool>());
	bool charvar = opts["charvar"].get<bool>();
	bool preprocess = opts["preprocessing"].get<bool>();
	tau_api::set_ba_component_factoring(
		opts["ba-component-factoring"].get<bool>());
	bool exp = opts["experimental"].get<bool>();
	// Every numeric limit goes through its api setter so the CLI and the
	// REPL `set` command share one wiring surface; 0 means what the flag's
	// description says (unlimited for most caps). A limit is applied only
	// when its flag was given: a flag that always wrote its own default
	// would shadow the TAU_* variable the limit falls back to. A value that
	// is not a non-negative number is an error, never read as 0; the first
	// bad value is reported once, below.
	string bad_option;
	auto given_count = [&opts, &bad_option](const char* name)
		-> std::optional<size_t>
	{
		const string v = opts[name].get<string>();
		if (v.empty()) return {};
		char* end = nullptr;
		errno = 0;
		const long n = std::strtol(v.c_str(), &end, 10);
		if (end == v.c_str() || *end != '\0' || n < 0 || errno == ERANGE) {
			if (bad_option.empty())
				bad_option = string("--") + name + " expects a "
					"non-negative number, got '" + v + "'";
			return {};
		}
		return (size_t) n;
	};
	tau_api::set_pwr_semantic_fallback(opts["pwr-semantic"].get<bool>());
	tau_api::set_step_definitional_propagation(
		opts["step-definitional-propagation"].get<bool>());
	const std::pair<const char*, void (*)(size_t)> count_flags[] = {
		{ "spec-size-warn", &tau_api::set_spec_size_warn },
		{ "max-revision-alts", &tau_api::set_max_revision_alts },
		{ "block-max-splits", &tau_api::set_block_max_splits },
		{ "block-max-rounds", &tau_api::set_block_max_rounds },
		{ "ba-decision-pins", &tau_api::set_ba_decision_pins },
		{ "cqe-max-clauses", &tau_api::set_cqe_max_clauses },
		{ "lgrs-max-vars", &tau_api::set_lgrs_max_vars },
		{ "max-fixpoint-steps", &tau_api::set_max_fixpoint_steps },
		{ "max-flag-search-steps", &tau_api::set_max_flag_search_steps },
		{ "block-squeeze-cap", &tau_api::set_block_squeeze_cap },
		{ "max-simplify-rounds", &tau_api::set_max_simplify_rounds },
		{ "max-def-passes", &tau_api::set_max_def_passes },
		{ "max-enum-steps", &tau_api::set_max_enum_steps },
		{ "max-probe-steps", &tau_api::set_max_probe_steps },
		{ "max-rewrite-rounds", &tau_api::set_max_rewrite_rounds },
		{ "max-constant-size", &tau_api::set_max_constant_size },
		{ "cache-bound", &tau_api::set_cache_bound },
		{ "gc-min-size", &tau_api::set_gc_min_size },
		{ "ltl-data-game-max-combinations",
			&tau_api::set_ltl_data_game_max_combinations },
		{ "ltl-max-observations", &tau_api::set_ltl_max_observations },
		{ "ltl-mealy-max-states", &tau_api::set_ltl_mealy_max_states },
		{ "ltl-mealy-max-edges", &tau_api::set_ltl_mealy_max_edges },
		{ "compile-max-table-edges", &tau_api::set_compile_max_table_edges },
		{ "compile-build-timeout", &tau_api::set_compile_build_timeout },
		{ "bf-dependence-max-nodes", &tau_api::set_bf_dependence_max_nodes },
	};
	for (const auto& [name, set] : count_flags)
		if (auto n = given_count(name); n) set(*n);
	if (const string t = opts["ltl-timeout"].get<string>(); !t.empty()) {
		char* end = nullptr;
		errno = 0;
		long v = std::strtol(t.c_str(), &end, 10);
		if (end == t.c_str() || *end != '\0' || v < 0 || errno == ERANGE)
			return error("--ltl-timeout expects a non-negative number "
				"of seconds, got '" + t + "'");
		tau_api::set_ltl_timeout_sec(v);
	}
	if (const string a = opts["ltl-alg"].get<string>(); !a.empty())
		tau_api::set_ltl_algorithm(a);
	if (auto n = given_count("ltl-qe-max-vars"); n)
		tau_api::set_ltl_qe_max_vars(*n);
	if (auto n = given_count("ltl-hoa-max-states"); n)
		tau_api::set_ltl_hoa_max_states(*n);
	if (auto n = given_count("ltl-guard-max-cubes"); n)
		tau_api::set_ltl_guard_max_cubes(*n);
	if (auto n = given_count("ltl-refinement-rounds"); n)
		tau_api::set_ltl_max_refinement_rounds(*n);
	if (auto n = given_count("ltl-window-max-paths"); n)
		tau_api::set_ltl_window_max_paths(*n);
	if (auto n = given_count("ltl-closed-regions-timeout"); n)
		tau_api::set_ltl_closed_regions_timeout(*n);
	if (auto n = given_count("ltl-data-game-max-nodes"); n)
		tau_api::set_ltl_data_game_max_nodes(*n);
	if (auto n = given_count("ltl-data-game-max-memo"); n)
		tau_api::set_ltl_data_game_max_memo(*n);
	if (auto n = given_count("max-consistency-subsets"); n)
		tau_api::set_max_consistency_subsets(*n);
	if (auto n = given_count("max-cover-products"); n)
		tau_api::set_max_cover_products(*n);
	if (auto n = given_count("tref-budget"); n)
		tau_api::set_tref_budget(*n);
	if (auto n = given_count("tref-budget-soft"); n)
		tau_api::set_tref_budget_soft_percent(*n);
	if (!bad_option.empty()) return error(bad_option);
	if (const string g = opts["gc-growth-factor"].get<string>(); !g.empty()) {
		char* end = nullptr;
		errno = 0;
		const double f = std::strtod(g.c_str(), &end);
		if (end == g.c_str() || *end != '\0' || errno == ERANGE
			|| !std::isfinite(f))
			return error("--gc-growth-factor expects a number, got '"
				+ g + "'");
		tau_api::set_gc_growth_factor(f);
	}
	// Apply each BA-declared CLI option through its own getter/setter pair
	// -- the same "two views of the same knob" wiring every option above
	// already uses, just addressed by family-option instead of a bare name.
	for (const auto& e : pack_ba_options<node_t>()) {
		std::string cli_name = e.family + "-" + e.option.name;
		if (e.option.kind == ba_option_kind::flag) {
			// Written only when given: a flag left alone keeps
			// whatever its algebra resolves for it.
			if (std::ranges::find(args, "--" + cli_name) != args.end())
				e.option.set_flag(opts[cli_name].get<bool>());
			continue;
		}
		// Not given: the algebra keeps whatever it resolves itself --
		// its own environment fallback, else its default.
		if (e.option.kind == ba_option_kind::text) {
			const string v = opts[cli_name].get<string>();
			if (!v.empty() && !e.option.set_text(v))
				return error("Invalid value for --" + cli_name
					+ ": " + v);
			continue;
		}
		auto n = given_count(cli_name.c_str());
		if (!bad_option.empty()) return error(bad_option);
		if (n) e.option.set_count(*n);
	}

	// After the options, so a budget given with the verb (--ltl-timeout,
	// --max-consistency-subsets, ...) applies to its synthesis too.
	if (cmd.ok() && (cmd.name() == "gen" || cmd.name() == "codegen")) {
		if (files.empty())
			return error("Usage: tau " + cmd.name()
				+ " <spec.tau>... [-o out_dir]");
		size_t dash_count = 0;
		for (const auto& f : files) if (f == "-") ++dash_count;
		if (dash_count > 1)
			return error("tau " + cmd.name() + ": '-' reads the spec from stdin "
				"and may appear only once");
		std::string out_dir = cmd.get<std::string>("output");
		if (!out_dir.empty() && files.size() > 1)
			return error("tau " + cmd.name() + ": -o names one output "
				"directory, but several spec files were given");
		for (const auto& spec_file : files) {
			std::string src;
			if (!read_spec_file(spec_file, src))
				return error("Cannot open file: " + spec_file);
			if (src.empty())
				return error("Spec file is empty: " + spec_file);
			// A spec file names its own dir; stdin has no name, so it uses
			// the same `a` a compiler gives an unnamed input.
			std::string dir = !out_dir.empty() ? out_dir
				: (spec_file == "-" ? "a.build" : spec_file + ".build");
			TAU_LOG_INFO << "tau " << cmd.name() << ": " << spec_file;
			auto res = gen_spec<node_t>(src, dir, "program", cmd.name());
			if (!res.has_value()) {
				res.print();
				return 1;
			}
			TAU_LOG_INFO << "generated: " << res.value().exe_path;
		}
		return 0;
	}

	if (cmd.ok() && cmd.name() == "compile") {
		if (files.empty())
			return error("Usage: tau compile <spec.tau> [-o out_exe] "
				"[--preset <name>] [-D NAME=VALUE]... [-G <gen>]");
		if (files.size() > 1)
			return error("tau compile: exactly one spec file is expected");
		std::string spec_file = files.front();
		std::string out_exe = cmd.get<std::string>("output");
		std::string build_dir;
		if (spec_file == "-") {
			// gcc's names for an input read from stdin: a.out (a.exe on
			// Windows) beside a.build/.
			if (out_exe.empty()) {
#ifdef _WIN32
				out_exe = "a.exe";
#else
				out_exe = "a.out";
#endif
				build_dir = "a.build";
			} else build_dir = out_exe + ".build";
		} else {
			if (out_exe.empty()) {
				std::filesystem::path p(spec_file);
				out_exe = (p.parent_path() / p.stem()).string();
			}
			build_dir = spec_file + ".build";
		}

		std::string src;
		if (!read_spec_file(spec_file, src))
			return error("Cannot open file: " + spec_file);
		if (src.empty()) return error("Spec file is empty: " + spec_file);

		// A spec with no extension is its own default output; never build a
		// spec over itself.
		if (spec_file != "-") {
			std::error_code sec, oec;
			auto spec_abs = std::filesystem::absolute(spec_file, sec)
				.lexically_normal();
			auto out_abs = std::filesystem::absolute(out_exe, oec)
				.lexically_normal();
			if (!sec && !oec && spec_abs == out_abs)
				return error("tau compile: the output path is the spec "
					"path: " + spec_file + " and " + out_exe
					+ "; pass -o <path> to name a different output");
		}

		TAU_LOG_INFO << "tau compile: " << spec_file;
		auto res = compile_spec<node_t>(src, out_exe, build_dir,
			cmd.get<std::string>("cxx"),
			cmd.get<std::string>("preset"),
			collect_compile_extra_args(args));
		if (!res.has_value()) {
			res.print();
			return 1;
		}
		TAU_LOG_INFO << "compiled: " << res.value().exe_path;
		return 0;
	}

	// Rule counting piggybacks on the benchmarks flag: both paths below
	// (file spec and REPL) read it from the same "benchmarks" option.
	rule_counting = opts["benchmarks"].get<bool>();

	if (files.size()) {
		DBG(TAU_LOG_TRACE << "running specification file: "
			<< files.front();)
		tau_api::set_severity(sev);
		tau_api::set_charvar(charvar);
		tau_api::set_preprocessing(preprocess);
		return run_tau_spec(files.front(), opts);
	}

	repl_evaluator<TAU_PACK_BASE_BAS> re({
		.status = opts["status"].get<bool>(),
		.colors = opts["color"].get<bool>(),
		.charvar = charvar,
		.preprocessing = preprocess,
		.print_benchmarks = opts["benchmarks"].get<bool>(),
#ifdef DEBUG
		.debug_repl = opts["debug"].get<bool>(),
#endif // DEBUG
		.severity = sev,
		.experimental = exp
	});
	string e = opts["evaluate"].get<string>();
	if (e.size()) {
		DBG(TAU_LOG_TRACE << "evaluating REPL command: " << e;)
		return re.eval(e).value_or(0);
	}
	DBG(TAU_LOG_TRACE << "running REPL";)
	welcome();
#ifdef TAU_PARSER_HAS_FTXUI
	bool use_ftxui = !opts["legacy-repl"].get<bool>();
#ifdef TAU_WASM_NODE_CLI
	// repl_ftxui runs interactive under Emscripten whatever stdin is, and
	// waits forever after the first line of a piped stdin.
	if (!isatty(STDIN_FILENO)) use_ftxui = false;
#endif
	if (use_ftxui) {
		repl_ftxui<decltype(re)> rftx(re, "tau> ", ".tau_history");
		re.reprompt();
		return rftx.run();
	}
#endif
	repl<decltype(re)> r(re, "tau> ", ".tau_history");
	re.reprompt();
	return r.run();

}
