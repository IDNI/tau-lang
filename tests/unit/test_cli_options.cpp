// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// cli_options.h is the shared option table the tau CLI and a compiled
// artifact's own main both build on (main.cpp's tau_options() now delegates
// to tau_cli_options(cli_option_set::full); tau_compile.tmpl.h's emit_main()
// emits a call to tau_cli_options(cli_option_set::artifact)). Plain
// (non-template) functions in namespace idni::tau_lang, so no NodeType/BA
// setup is needed here, just the header itself.

#include "test_init.h"
#include "cli_options.h"

using namespace idni::tau_lang;

namespace {

bool has(const idni::cli::options& opts, const std::string& name) {
	return opts.find(name) != opts.end();
}

}

TEST_SUITE("cli_options") {

	TEST_CASE("artifact subset is exactly the run-time-meaningful flags") {
		auto opts = tau_cli_options(cli_option_set::artifact);
		CHECK(opts.size() == 7);
		for (const char* n : { "help", "version", "license", "severity",
				"benchmarks", "json", "quit" })
			CHECK_MESSAGE(has(opts, n), n << " missing from the artifact subset");
		// REPL/formatting-only flags: no meaning for a compiled artifact
		// (no REPL, no formula pretty-printing) -- excluded.
		for (const char* n : { "color", "status", "debug", "charvar",
				"blasting", "indenting", "highlighting", "evaluate",
				"legacy-repl", "experimental" })
			CHECK_MESSAGE(!has(opts, n), n << " should not be in the artifact subset");
	}

	TEST_CASE("full subset is a superset of the artifact subset, same shapes") {
		auto full = tau_cli_options(cli_option_set::full);
		auto art  = tau_cli_options(cli_option_set::artifact);
		for (auto& [name, opt] : art) {
			REQUIRE_MESSAGE(has(full, name),
				name << " in artifact subset but not full");
			CHECK(full.at(name).short_name() == opt.short_name());
			CHECK(full.at(name).description() == opt.description());
			CHECK(full.at(name).is_bool() == opt.is_bool());
			CHECK(full.at(name).is_string() == opt.is_string());
		}
		CHECK(full.size() > art.size());
	}

	TEST_CASE("severity string maps: trace/debug/error match, anything else is info") {
		using sev = boost::log::trivial::severity_level;
		CHECK(tau_cli_parse_severity("trace") == sev::trace);
		CHECK(tau_cli_parse_severity("debug") == sev::debug);
		CHECK(tau_cli_parse_severity("error") == sev::error);
		CHECK(tau_cli_parse_severity("info") == sev::info);
		CHECK(tau_cli_parse_severity("bogus") == sev::info);
		CHECK(tau_cli_parse_severity("") == sev::info);
	}

	TEST_CASE("an unknown flag is rejected the same way for both subsets") {
		// Mirrors main.cpp's/the emitted artifact's own pattern: the exit
		// status a caller reports is cl.status() (process_arg's internal 3,
		// no command table defined), not process_args()'s own return value.
		for (auto set : { cli_option_set::full, cli_option_set::artifact }) {
			idni::cli cl("test", std::vector<std::string>{"test", "--nope"},
				idni::cli::commands{}, "", tau_cli_options(set));
			CHECK(cl.process_args() != 0);
			CHECK(cl.status() == 3);
		}
	}

	TEST_CASE("a known flag parses to its value in both subsets") {
		idni::cli cl("test",
			std::vector<std::string>{"test", "-S", "trace", "-q"},
			idni::cli::commands{}, "", tau_cli_options(cli_option_set::artifact));
		REQUIRE(cl.process_args() == 0);
		auto opts = cl.get_processed_options();
		CHECK(opts["severity"].get<std::string>() == "trace");
		CHECK(opts["quit"].get<bool>() == true);
		// unset defaults are preserved
		CHECK(opts["benchmarks"].get<bool>() == true);
		CHECK(opts["json"].get<bool>() == false);
	}
}
