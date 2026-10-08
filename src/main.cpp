// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <filesystem>
#include <iostream>
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

using namespace std;
using namespace idni;
using namespace idni::tau_lang;

using node_t = tau_lang::tau_pack::node_t;
using tau = tree<node_t>;
using tau_api = api<node_t>;
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

/// @brief Run the specification in @p spec_file with the interpreter until it
/// ends.
///
/// Prints the setup warnings before the run, and the benchmark report (in
/// plain text, to stderr) after it when `--benchmarks` is on. With `--quit`
/// the run ends when no input is left.
/// @param spec_file Path of the spec file, or "-" for stdin.
/// @return 0 when the file is empty or the run succeeded, 1 otherwise.
int run_tau_spec(string spec_file) {
	const bool benchmarks = idni::options().get<bool>("benchmarks");
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
	bool quit_on_idle = idni::options().get<bool>("quit");
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
/// Declares the options, loads the environment and parses the command line
/// with the options repository, then runs one of: the `gen` / `codegen` or
/// `compile` verb, the given spec file, the `--evaluate` REPL command, or the
/// interactive REPL (FTXUI unless `--legacy-repl`).
/// @return The exit status: 0 on success, 1 on an error; for `--evaluate`,
/// the REPL's code (0 done, 1 quit or error with error-quits, 2 incomplete
/// input).
int main(int argc, char** argv) {
	auto& repo = idni::options();
	auto init = tau_init<node_t>();
	init.print_pending();
	if (!init.has_value()) return 1;
	auto declared = declare_tau_cli(repo);
	declared.print_pending();
	if (!declared.has_value()) return 1;
	auto env = repo.load_env("TAU_");
	env.print_pending();
	if (!env.has_value()) return 1;

	vector<string> args;
	for (int i = 0; i < argc; i++) args.push_back(argv[i]);
	expand_attached_short_values(args);
	vector<char*> arg_ptrs;
	for (auto& a : args) arg_ptrs.push_back(a.data());
	// a command line never comes near INT_MAX words
	auto parsed = repo.parse_args(static_cast<int>(arg_ptrs.size()),
		arg_ptrs.data(), tau_args_config);
	parsed.print_pending();
	if (!parsed.has_value()) return 1;
	const parsed_args& pa = parsed.value();
	const string& cmd = pa.command;
	const vector<string>& files = pa.inputs;

	if (repo.get<bool>("help")) return repo.help(std::cout, cmd), 0;
	if (repo.get<bool>("version"))
		return std::cout << full_version << "\n"
			<< "algebras: " << node_t::ba::types_joined() << "\n", 0;
	if (repo.get<bool>("license")) return std::cout << license, 0;

	auto sev = severity_codec{}.from_value(repo.value("severity"));
	sev.print_pending();
	if (!sev.has_value()) return 1;
	tau_api::set_highlighting(repo.get<bool>("highlighting"));
	tau_api::set_indenting(repo.get<bool>("indenting"));
	tau_api::set_json(repo.get<bool>("json"));
	const bool charvar = repo.get<bool>("charvar");

	if (cmd == "gen" || cmd == "codegen") {
		if (files.empty())
			return error("Usage: tau " + cmd + " <spec.tau>... [-o out_dir]");
		size_t dash_count = 0;
		for (const auto& f : files) if (f == "-") ++dash_count;
		if (dash_count > 1)
			return error("tau " + cmd + ": '-' reads the spec from stdin "
				"and may appear only once");
		const std::string out_dir = repo.get<std::string>("output-dir");
		if (!out_dir.empty() && files.size() > 1)
			return error("tau " + cmd + ": -o names one output "
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
			TAU_LOG_INFO << "tau " << cmd << ": " << spec_file;
			auto res = gen_spec<node_t>(src, dir, "program", cmd);
			if (!res.has_value()) {
				res.print();
				return 1;
			}
			TAU_LOG_INFO << "generated: " << res.value().exe_path;
		}
		return 0;
	}

	if (cmd == "compile") {
		if (files.empty())
			return error("Usage: tau compile <spec.tau> [-o out_exe] "
				"[--preset <name>] [-D NAME=VALUE]... [-G <gen>]");
		if (files.size() > 1)
			return error("tau compile: exactly one spec file is expected");
		std::string spec_file = files.front();
		std::string out_exe = repo.get<std::string>("output");
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

		std::vector<std::string> configure_args;
		for (const auto& d : repo.get<option_list>("define"))
			configure_args.push_back("-D" + d);
		if (const auto g = repo.get<std::string>("generator"); !g.empty()) {
			configure_args.push_back("-G");
			configure_args.push_back(g);
		}
		TAU_LOG_INFO << "tau compile: " << spec_file;
		auto res = compile_spec<node_t>(src, out_exe, build_dir,
			repo.get<std::string>("cxx"), repo.get<std::string>("preset"),
			configure_args);
		if (!res.has_value()) {
			res.print();
			return 1;
		}
		TAU_LOG_INFO << "compiled: " << res.value().exe_path;
		return 0;
	}

	// Rule counting piggybacks on the benchmarks flag: both paths below
	// (file spec and REPL) read it from the same "benchmarks" option.
	rule_counting = repo.get<bool>("benchmarks");

	if (files.size()) {
		DBG(TAU_LOG_TRACE << "running specification file: "
			<< files.front();)
		tau_api::set_severity(sev.value());
		tau_api::set_charvar(charvar);
		return run_tau_spec(files.front());
	}

	repl_evaluator<TAU_PACK_BASE_BAS> re;
	const string e = repo.get<string>("evaluate");
	if (e.size()) {
		DBG(TAU_LOG_TRACE << "evaluating REPL command: " << e;)
		return re.eval(e).value_or(0);
	}
	DBG(TAU_LOG_TRACE << "running REPL";)
	welcome();
#ifdef TAU_PARSER_HAS_FTXUI
	bool use_ftxui = !repo.get<bool>("legacy-repl");
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
