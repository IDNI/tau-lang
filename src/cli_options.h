// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Shared option table for the tau CLI and a compiled artifact's own main:
// each caller declares which subset it supports so unknown/unsupported
// flags fail with the same idni::cli error/help both callers already share.

#ifndef __IDNI__TAU__CLI_OPTIONS_H__
#define __IDNI__TAU__CLI_OPTIONS_H__

#include <string>
#include <boost/log/trivial.hpp>

#include "utility/cli.h"

namespace idni::tau_lang {

// full: every flag the CLI supports; artifact: the run-time-meaningful
// subset a compiled artifact's own main parses.
enum class cli_option_set { full, artifact };

// Builds the tau option table for the requested subset. The artifact subset
// is a prefix of the full table -- same names, short letters, defaults and
// descriptions -- so both --help and an unknown-flag error read identically.
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
	opts["blasting"] = idni::cli::option("blasting", 'B', true)
		.set_description("blasting (enabled by default)");
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
		.set_description("display status");
	opts["color"] = idni::cli::option("color", 'c', true)
		.set_description("use colors");
	DBG(opts["debug"] = idni::cli::option("debug", 'd', true)
		.set_description("debug mode");)
	opts["experimental"] = idni::cli::option("experimental", 'x', false)
		.set_description("enables transitioning features");
	return opts;
}

// Maps --severity's string value to a Boost.Log severity level; anything
// other than trace/debug/error (including the default "info") is info.
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
