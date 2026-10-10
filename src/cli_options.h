// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file cli_options.h
 * @brief The options of the tau executable and of a compiled artifact's own
 * main.
 *
 * The tau executable declares @ref tau_cli_option_set and @ref tau_surfaces
 * with @ref declare_program_options. The main that `tau gen` emits
 * (tau_compile.tmpl.h) declares @ref tau_artifact_option_set and
 * @ref tau_artifact_surfaces the same way. Both parse their command line with
 * `options().parse_args()`.
 */

#ifndef __IDNI__TAU__CLI_OPTIONS_H__
#define __IDNI__TAU__CLI_OPTIONS_H__

#include <algorithm>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <boost/log/trivial.hpp>

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
/// seven are the options of a compiled artifact. Only `cxx` and `sdk-dir`
/// read the environment, and the command line wins over it. The REPL
/// evaluator binds seven of them to its own fields.
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
			"C++ compiler for the emitted project (default: cmake's "
			"compiler, or the compiler of the --preset platform)" },
		{ "sdk-dir", "cli", std::string(),
			"directory of the tau SDK that builds the emitted project "
			"(default: the SDK found beside this tau)" },
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
	{ "sdk-dir", { "compile" } },
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

/// The number of options at the start of @ref tau_cli_option_set that a
/// compiled artifact declares too.
inline constexpr std::size_t tau_artifact_shared_options = 7;

/// The options of a compiled artifact: the first options of
/// @ref tau_cli_option_set, so the help texts stay equal, and `print-spec`.
inline const option_set tau_artifact_option_set = [] {
	option_set set;
	set.options.assign(tau_cli_option_set.options.begin(),
		tau_cli_option_set.options.begin() + tau_artifact_shared_options);
	set.options.push_back({ "print-spec", "global", false,
		"print the Tau specification of this program", {}, false });
	return set;
}();

/// The surfaces of a compiled artifact. The shared options take their
/// letters from @ref tau_surfaces, so the letters stay equal.
inline const std::vector<std::pair<std::string, option_surface>>
	tau_artifact_surfaces = []
{
	std::vector<std::pair<std::string, option_surface>> surfaces;
	auto shared = [](const std::string& name) {
		return std::ranges::any_of(tau_artifact_option_set.options,
			[&](const option_spec& s) { return s.name == name; });
	};
	for (const auto& [name, surface] : tau_surfaces)
		if (surface.command.empty() && shared(name))
			surfaces.emplace_back(name, surface);
	surfaces.emplace_back("print-spec", option_surface{ "" });
	return surfaces;
}();

/// The program data of a compiled artifact for `parse_args()` and `help()`.
inline const args_config artifact_args_config{
	.name = "program", .help_header = "",
	.description = "A program compiled from a Tau specification.",
	.default_command = "", .default_command_when_inputs = "",
	.global_surface = "" };

/// Declares @p set and adds @p surfaces. Every other declared option gets
/// its long form on the global surface. Call it after `tau_init()`, so the
/// core and BA options get their surfaces too.
inline result<void> declare_program_options(options_repository& repo,
	const option_set& set,
	const std::vector<std::pair<std::string, option_surface>>& surfaces)
{
	result<void> r;
	TAU_TRY_VOID(repo.declare(set));
	for (const auto& [name, surface] : surfaces) {
		// a BA outside the pack and `debug` outside DEBUG declare nothing
		if (!repo.find(name)) continue;
		TAU_TRY_VOID(repo.add_surface(name, surface));
	}
	auto in_set = [&](const std::string& name) {
		return std::ranges::any_of(set.options,
			[&](const option_spec& s) { return s.name == name; });
	};
	auto is_global = [](const option_surface& s) { return s.command.empty(); };
	for (const std::string& name : repo.names()) {
		if (in_set(name)
			|| std::ranges::any_of(repo.find(name)->surfaces, is_global))
			continue;
		TAU_TRY_VOID(repo.add_surface(name, option_surface{ "" }));
	}
	return r;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__CLI_OPTIONS_H__
