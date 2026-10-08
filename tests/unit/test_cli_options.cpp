// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// cli_options.h holds the options of the tau executable, which main.cpp
// declares with declare_tau_cli() and parses with parse_args(), and the old
// option table that a compiled artifact's own main builds with
// tau_cli_options(cli_option_set::artifact). Plain functions in namespace
// idni::tau_lang, so no NodeType/BA setup is needed here, just the header.

#include <sstream>

#include "test_init.h"
#include "cli_options.h"

using namespace idni::tau_lang;

namespace {

bool has(const idni::cli::options& opts, const std::string& name) {
	return opts.find(name) != opts.end();
}

idni::diagnostics::result<idni::parsed_args> parse(
	idni::options_repository& repo, std::vector<std::string> args)
{
	std::vector<char*> argv;
	for (auto& a : args) argv.push_back(a.data());
	return repo.parse_args(static_cast<int>(argv.size()), argv.data(),
		tau_args_config);
}

// print() marks what it prints, so one call checks every text.
template <typename... Texts>
bool has_text(const idni::diagnostics::report& rep, Texts... texts) {
	std::ostringstream os;
	rep.print(os);
	const std::string out = os.str();
	return ((out.find(std::string_view(texts)) != std::string::npos) && ...);
}

char short_letter(const std::string& name, const std::string& command) {
	for (const auto& [n, sf] : tau_surfaces)
		if (n == name && sf.command == command) return sf.short_name;
	return '\0';
}

// One core option with a short letter and one without, as tau_init()
// would declare them.
const idni::option_set core_sample{ {
	{ "max-fixpoint-steps", "solver", std::size_t{ 500 }, "fixpoint cap" },
	{ "ltl-max-observations", "ltl", std::size_t{ 8 }, "observation cap" },
} };

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

	TEST_CASE("full table leaves main.cpp's own flags and letters free") {
		auto full = tau_cli_options(cli_option_set::full);
		// main.cpp adds these, with 'B' and 'K', after the full table.
		for (const char* n : { "blasting", "preprocessing",
				"ba-component-factoring" })
			CHECK_MESSAGE(!has(full, n), n << " should not be in the full table");
		for (auto& [name, opt] : full) {
			CAPTURE(name);
			CHECK(opt.short_name() != 'B');
			CHECK(opt.short_name() != 'K');
		}
		for (const char* n : { "charvar", "indenting", "highlighting",
				"evaluate", "legacy-repl", "status", "color",
				"experimental" })
			CHECK_MESSAGE(has(full, n), n << " missing from the full table");
		CHECK(full.at("status").description()
			== "display status (enabled by default)");
		CHECK(full.at("color").description()
			== "use colors (enabled by default)");
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

	TEST_CASE("the artifact options are the first seven of the cli set") {
		auto art = tau_cli_options(cli_option_set::artifact);
		REQUIRE(tau_cli_option_set.options.size() > art.size());
		for (size_t i = 0; i < art.size(); ++i) {
			const auto& spec = tau_cli_option_set.options[i];
			CAPTURE(spec.name);
			REQUIRE(has(art, spec.name));
			CHECK(art.at(spec.name).description() == spec.help);
			CHECK(art.at(spec.name).short_name()
				== short_letter(spec.name, ""));
		}
	}

	TEST_CASE("severity words map to their level, any other word is an error") {
		using sev = boost::log::trivial::severity_level;
		severity_codec c;
		CHECK(c.from_value(std::string("trace")).value() == sev::trace);
		CHECK(c.from_value(std::string("debug")).value() == sev::debug);
		CHECK(c.from_value(std::string("info")).value() == sev::info);
		CHECK(c.from_value(std::string("error")).value() == sev::error);
		CHECK_FALSE(c.from_value(std::string("bogus")).has_value());
		CHECK_FALSE(c.from_value(std::string("")).has_value());
		CHECK_FALSE(c.from_value(std::size_t{ 1 }).has_value());
		CHECK(std::get<std::string>(c.to_value(sev::trace)) == "trace");
	}

	TEST_CASE("parse_args writes the cli options and the bound severity") {
		idni::options_repository repo;
		idni::options_scope scope(repo);
		REQUIRE(declare_tau_cli(repo).has_value());
		auto r = parse(repo, { "tau", "-S", "trace", "-q" });
		REQUIRE(r.has_value());
		CHECK(tau_cli_severity == boost::log::trivial::trace);
		CHECK(repo.get<bool>("quit"));
		CHECK(repo.get<bool>("benchmarks"));
		CHECK_FALSE(repo.get<bool>("json"));
		tau_cli_severity = boost::log::trivial::info;
	}

	TEST_CASE("an unknown flag and a bad severity are errors") {
		idni::options_repository repo;
		idni::options_scope scope(repo);
		REQUIRE(declare_tau_cli(repo).has_value());
		auto unknown = parse(repo, { "tau", "--nope" });
		CHECK_FALSE(unknown.has_value());
		CHECK(has_text(unknown.report(), "Unknown option"));
		auto bad = parse(repo, { "tau", "-S", "bogus" });
		CHECK_FALSE(bad.has_value());
		CHECK(has_text(bad.report(), "does not take the value"));
		CHECK(tau_cli_severity == boost::log::trivial::info);
	}

	TEST_CASE("compile takes its options after the command, define repeats") {
		idni::options_repository repo;
		idni::options_scope scope(repo);
		REQUIRE(declare_tau_cli(repo).has_value());
		auto r = parse(repo, { "tau", "compile", "-D", "A=1",
			"--define", "B=2", "-G", "Ninja", "-o", "out", "-c", "cc" });
		REQUIRE(r.has_value());
		CHECK(r.value().command == "compile");
		CHECK(repo.get<idni::option_list>("define")
			== idni::option_list{ "A=1", "B=2" });
		CHECK(repo.get<std::string>("generator") == "Ninja");
		CHECK(repo.get<std::string>("output") == "out");
		CHECK(repo.get<std::string>("cxx") == "cc");
		// -c is color before the command and cxx after it
		CHECK(repo.get<bool>("color"));
		// a global option is no option of a command
		CHECK_FALSE(parse(repo, { "tau", "gen", "-q" }).has_value());
	}

	TEST_CASE("-o is the output directory of gen and codegen") {
		for (const char* verb : { "gen", "codegen" }) {
			CAPTURE(verb);
			idni::options_repository repo;
			idni::options_scope scope(repo);
			REQUIRE(declare_tau_cli(repo).has_value());
			REQUIRE(parse(repo, { "tau", verb, "-o", "dir" }).has_value());
			CHECK(repo.get<std::string>("output-dir") == "dir");
			CHECK(repo.get<std::string>("output").empty());
		}
	}

	TEST_CASE("a core option gets its long form and its short letter") {
		idni::options_repository repo;
		idni::options_scope scope(repo);
		REQUIRE(repo.declare(core_sample).has_value());
		REQUIRE(declare_tau_cli(repo).has_value());
		REQUIRE(parse(repo, { "tau", "-f", "9" }).has_value());
		CHECK(repo.get<std::size_t>("max-fixpoint-steps") == 9);
		REQUIRE(parse(repo, { "tau", "--ltl-max-observations", "5" })
			.has_value());
		CHECK(repo.get<std::size_t>("ltl-max-observations") == 5);
		auto bad = parse(repo, { "tau", "--max-fixpoint-steps", "abc" });
		CHECK_FALSE(bad.has_value());
		CHECK(has_text(bad.report(), "does not take the value",
			"max-fixpoint-steps"));
	}

	TEST_CASE("every short letter is used once per command") {
		for (const auto& [name, sf] : tau_surfaces) {
			if (!sf.short_name) continue;
			CAPTURE(name);
			size_t uses = 0;
			for (const auto& [n2, sf2] : tau_surfaces)
				if (sf2.command == sf.command
					&& sf2.short_name == sf.short_name) ++uses;
			CHECK(uses == 1);
		}
	}
}
