// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file cli_options.h
 * @brief The options of the tau executable and of a compiled artifact's own
 * main.
 *
 * The tau executable declares @ref tau_cli_option_set and its surfaces with
 * @ref declare_tau_cli and parses its command line with
 * `options().parse_args()`. The main that `tau gen` emits
 * (tau_compile.tmpl.h) builds its options with @ref tau_cli_options and the
 * artifact subset.
 */

#ifndef __IDNI__TAU__CLI_OPTIONS_H__
#define __IDNI__TAU__CLI_OPTIONS_H__

#include <algorithm>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <boost/log/trivial.hpp>

#include "utility/cli.h"
#include "utility/options.h"
#include "tau_diagnostics.h"

namespace idni::tau_lang {

/// The `severity` words: `trace`, `debug`, `info` and `error`. Any other
/// word is an error.
struct severity_codec {
	using level = boost::log::trivial::severity_level;
	option_value to_value(level s) const {
		switch (s) {
		case boost::log::trivial::trace: return std::string("trace");
		case boost::log::trivial::debug: return std::string("debug");
		case boost::log::trivial::error: return std::string("error");
		default: return std::string("info");
		}
	}
	result<level> from_value(const option_value& v) const {
		result<level> r;
		const auto* text = std::get_if<std::string>(&v);
		if (!text) return r.with_error(code::type_error,
			parser_strings::messages::option_value_kind);
		if (*text == "trace") return r.with_value(boost::log::trivial::trace);
		if (*text == "debug") return r.with_value(boost::log::trivial::debug);
		if (*text == "info") return r.with_value(boost::log::trivial::info);
		if (*text == "error") return r.with_value(boost::log::trivial::error);
		return r.with_error(code::invalid_argument,
			parser_strings::messages::option_bad_value,
			{ { label::name, "severity" }, { label::value, *text } });
	}
};

/// The options of the tau executable that no library field holds. The first
/// seven are the options of a compiled artifact. The command line alone
/// writes them, so none reads the environment. The REPL evaluator binds
/// seven of them to its own fields.
inline const option_set tau_cli_option_set{
	{
		{ "help", "global", false, "detailed information about options",
			{}, false },
		{ "version", "global", false,
			"show the current Tau executable version", {}, false },
		{ "license", "global", false, "show license for Tau", {}, false },
		{ "severity", "ui", std::string("info"),
			"severity level (trace/debug/info/error)", {}, false },
		{ "benchmarks", "run", true,
			"print benchmarks (enabled by default)", {}, false },
		{ "json", "ui", false, "output in JSON format", {}, false },
		{ "quit", "cli", false, "quit when no input", {}, false },
		{ "charvar", "run", true, "charvar (enabled by default)", {},
			false },
		{ "indenting", "ui", false, "indenting of formulas", {}, false },
		{ "highlighting", "ui", false, "syntax highlighting", {}, false },
		// raw: a REPL command keeps its quotes and its spaces
		{ "evaluate", "cli", std::string(), "REPL command to evaluate",
			{}, false, true },
		{ "legacy-repl", "cli", false,
			"use legacy terminal REPL instead of FTXUI", {}, false },
		{ "status", "ui", true, "display status (enabled by default)", {},
			false },
		{ "color", "ui", true, "use colors (enabled by default)", {},
			false },
#ifdef DEBUG
		{ "debug", "run", true, "debug mode", {}, false },
#endif // DEBUG
		{ "experimental", "cli", false,
			"enables transitioning features", {}, false },
		{ "output-dir", "cli", std::string(),
			"output directory (default: <spec>.build; only with a "
			"single spec file)", {}, false },
		{ "output", "cli", std::string(),
			"output executable path (default: spec file path without "
			"extension)", {}, false },
		{ "cxx", "cli", std::string(),
			"C++ compiler for the emitted project (default: TAU_CXX, "
			"else cmake's compiler, or the compiler of the --preset "
			"platform)", {}, false },
		{ "preset", "cli", std::string(),
			"target platform or ./dev preset name; without it tau "
			"compiles for this machine", {}, false },
		{ "define", "cli", option_list{},
			"cmake cache variable NAME=VALUE for the emitted project's "
			"configure; repeatable", {}, false },
		{ "generator", "cli", std::string(),
			"cmake generator for the emitted project's configure (the "
			"preset's generator otherwise)", {}, false },
	},
	{
		{ "gen", "generates the C++ artifact for a Tau spec file "
			"(no build)" },
		{ "codegen", "another name for gen" },
		{ "compile", "compiles a Tau spec file into a standalone "
			"executable" },
	}
};

/// Where an option of the tau executable shows, with its short letter. A
/// core or BA option not named here gets its long form on the global surface.
inline const std::vector<std::pair<std::string, option_surface>> tau_surfaces{
	{ "help", { "", 'h' } }, { "help", { "gen", 'h' } },
	{ "help", { "codegen", 'h' } }, { "help", { "compile", 'h' } },
	{ "version", { "", 'v' } }, { "license", { "", 'l' } },
	{ "severity", { "", 'S' } }, { "benchmarks", { "", 'b' } },
	{ "json", { "", 'J' } }, { "quit", { "", 'q' } },
	{ "charvar", { "", 'V' } }, { "indenting", { "", 'I' } },
	{ "highlighting", { "", 'H' } }, { "evaluate", { "", 'e' } },
	{ "legacy-repl", { "", 'X' } }, { "status", { "", 's' } },
	{ "color", { "", 'c' } }, { "debug", { "", 'd' } },
	{ "experimental", { "", 'x' } },
	{ "output-dir", { "gen", 'o' } }, { "output-dir", { "codegen", 'o' } },
	{ "output", { "compile", 'o' } }, { "cxx", { "compile", 'c' } },
	{ "preset", { "compile" } }, { "define", { "compile", 'D' } },
	{ "generator", { "compile", 'G' } },
	// core
	{ "preprocessing", { "", 'B' } }, { "spec-size-warn", { "", 'w' } },
	{ "pwr-semantic", { "", 'Z' } },
	{ "step-definitional-propagation", { "", 't' } },
	{ "max-revision-alts", { "", 'a' } },
	{ "block-max-splits", { "", 'p' } }, { "block-max-rounds", { "", 'r' } },
	{ "cqe-max-clauses", { "", 'Q' } }, { "lgrs-max-vars", { "", 'g' } },
	{ "max-fixpoint-steps", { "", 'f' } },
	{ "max-flag-search-steps", { "", 'F' } },
	{ "block-squeeze-cap", { "", 'z' } },
	{ "max-simplify-rounds", { "", 'm' } },
	{ "max-def-passes", { "", 'P' } }, { "max-probe-steps", { "", 'M' } },
	{ "max-enum-steps", { "", 'E' } }, { "max-rewrite-rounds", { "", 'R' } },
	{ "max-consistency-subsets", { "", 'j' } },
	{ "max-cover-products", { "", 'n' } },
	{ "max-constant-size", { "", 'u' } }, { "cache-bound", { "", 'A' } },
	{ "ltl-timeout", { "", 'T' } }, { "ltl-alg", { "", 'L' } },
	{ "ltl-qe-max-vars", { "", 'k' } },
	{ "ltl-hoa-max-states", { "", 'Y' } },
	{ "ltl-guard-max-cubes", { "", 'U' } },
	{ "ltl-refinement-rounds", { "", 'D' } },
	{ "ltl-window-max-paths", { "", 'O' } },
	{ "tref-budget", { "", 'y' } }, { "tref-budget-soft", { "", 'C' } },
	{ "gc-min-size", { "", 'G' } }, { "gc-growth-factor", { "", 'W' } },
	// the tau algebra
	{ "ba-component-factoring", { "", 'K' } },
	{ "ba-decision-pins", { "", 'N' } },
};

/// The program data of the tau executable for `parse_args()` and `help()`.
inline const args_config tau_args_config{
	.name = "tau", .help_header = "tau [ <specification file> ]",
	.description = "Tau language. A global option comes before the "
		"command.", .default_command = "",
	.default_command_when_inputs = "", .global_surface = "" };

/// Declares @ref tau_cli_option_set and adds the surfaces of the tau
/// executable. Call it after `tau_init()`, so the core and BA options get
/// their surfaces too.
inline result<void> declare_tau_cli(options_repository& repo) {
	result<void> r;
	TAU_TRY_VOID(repo.declare(tau_cli_option_set));
	for (const auto& [name, surface] : tau_surfaces) {
		// a BA outside the pack and `debug` outside DEBUG declare nothing
		if (!repo.find(name)) continue;
		TAU_TRY_VOID(repo.add_surface(name, surface));
	}
	auto is_cli = [](const std::string& name) {
		return std::ranges::any_of(tau_cli_option_set.options,
			[&](const option_spec& s) { return s.name == name; });
	};
	auto is_global = [](const option_surface& s) { return s.command.empty(); };
	for (const std::string& name : repo.names()) {
		if (is_cli(name)
			|| std::ranges::any_of(repo.find(name)->surfaces, is_global))
			continue;
		TAU_TRY_VOID(repo.add_surface(name, option_surface{ "" }));
	}
	return r;
}

/// Which option table @ref tau_cli_options builds: `full`, the general and
/// REPL flags of the tau executable (main.cpp adds `--preprocessing`,
/// `--ba-component-factoring` and the runtime limits), or `artifact`, the
/// run-time-meaningful subset a compiled artifact's own main parses.
enum class cli_option_set { full, artifact };

/**
 * @brief Build the option table for @p set.
 *
 * The artifact subset is a prefix of the full table -- same names, short
 * letters, defaults and descriptions -- so both `--help` and an unknown-flag
 * error read identically.
 * @param set The table to build.
 * @return The options, keyed by long name.
 */
inline idni::cli::options tau_cli_options(cli_option_set set = cli_option_set::full) {
	idni::cli::options opts;
	opts["help"] = idni::cli::option("help", 'h', false)
		.set_description("detailed information about options");
	opts["version"] = idni::cli::option("version", 'v', false)
		.set_description("show the current Tau executable version");
	opts["license"] = idni::cli::option("license", 'l', false)
		.set_description("show license for Tau");
	opts["severity"] = idni::cli::option("severity", 'S', "info")
		.set_description("severity level (trace/debug/info/error)");
	opts["benchmarks"] = idni::cli::option("benchmarks", 'b', true)
		.set_description("print benchmarks (enabled by default)");
	opts["json"] = idni::cli::option("json", 'J', false)
		.set_description("output in JSON format");
	opts["quit"] = idni::cli::option("quit", 'q', false)
		.set_description("quit when no input");
	if (set == cli_option_set::artifact) return opts;
	opts["charvar"] = idni::cli::option("charvar", 'V', true)
		.set_description("charvar (enabled by default)");
	opts["indenting"] = idni::cli::option("indenting", 'I', false)
		.set_description("indenting of formulas");
	opts["highlighting"] = idni::cli::option("highlighting", 'H', false)
		.set_description("syntax highlighting");
	// REPL specific options
	opts["evaluate"] = idni::cli::option("evaluate", 'e', "")
		.set_description("REPL command to evaluate");
	opts["legacy-repl"] = idni::cli::option("legacy-repl", 'X', false)
		.set_description("use legacy terminal REPL instead of FTXUI");
	opts["status"] = idni::cli::option("status", 's', true)
		.set_description("display status (enabled by default)");
	opts["color"] = idni::cli::option("color", 'c', true)
		.set_description("use colors (enabled by default)");
	DBG(opts["debug"] = idni::cli::option("debug", 'd', true)
		.set_description("debug mode");)
	opts["experimental"] = idni::cli::option("experimental", 'x', false)
		.set_description("enables transitioning features");
	return opts;
}

/**
 * @brief The Boost.Log severity level that `--severity`'s value @p s names.
 * @param s "trace", "debug", "info" or "error".
 * @return The level; anything other than trace/debug/error (including the
 * default "info") is info.
 */
inline boost::log::trivial::severity_level tau_cli_parse_severity(
	const std::string& s)
{
	return s == "error" ? boost::log::trivial::error :
	       s == "trace" ? boost::log::trivial::trace :
	       s == "debug" ? boost::log::trivial::debug :
	                      boost::log::trivial::info;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__CLI_OPTIONS_H__
