// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// cli_options.h holds the options of the tau executable and of a compiled
// artifact. Both declare with declare_program_options() and parse with
// parse_args(). No NodeType/BA setup is needed here, just the header.

#include <sstream>

#include "test_init.h"
#include "cli_options.h"

using namespace idni::tau_lang;

namespace {

idni::diagnostics::result<idni::parsed_args> parse(
	idni::options_repository& repo, std::vector<std::string> args,
	const idni::args_config& cfg = tau_args_config)
{
	std::vector<char*> argv;
	for (auto& a : args) argv.push_back(a.data());
	return repo.parse_args(static_cast<int>(argv.size()), argv.data(), cfg);
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

	TEST_CASE("the artifact options are the first seven of the cli set") {
		const auto& art = tau_artifact_option_set.options;
		REQUIRE(art.size() == tau_artifact_shared_options + 1);
		for (size_t i = 0; i < tau_artifact_shared_options; ++i) {
			CAPTURE(art[i].name);
			CHECK(art[i] == tau_cli_option_set.options[i]);
		}
		std::vector<std::string> names;
		for (const auto& spec : art) names.push_back(spec.name);
		CHECK(names == std::vector<std::string>{ "help", "version",
			"license", "severity", "benchmarks", "json", "quit",
			"print-spec" });
	}

	TEST_CASE("the artifact letters are the letters of the tau executable") {
		size_t shared = 0;
		for (const auto& [name, sf] : tau_artifact_surfaces) {
			CAPTURE(name);
			CHECK(sf.command.empty());
			if (name == "print-spec") {
				CHECK(sf.short_name == '\0');
				continue;
			}
			CHECK(sf.short_name == short_letter(name, ""));
			++shared;
		}
		CHECK(shared == tau_artifact_shared_options);
	}

	TEST_CASE("an artifact parses its options and a core option by name") {
		idni::options_repository repo;
		idni::options_scope scope(repo);
		REQUIRE(repo.declare(core_sample).has_value());
		REQUIRE(declare_program_options(repo, tau_artifact_option_set,
			tau_artifact_surfaces).has_value());
		auto r = parse(repo, { "program", "-S", "trace", "-q",
			"--print-spec", "--max-fixpoint-steps", "9" },
			artifact_args_config);
		REQUIRE(r.has_value());
		CHECK(repo.get<std::string>("severity") == "trace");
		CHECK(repo.get<bool>("quit"));
		CHECK(repo.get<bool>("print-spec"));
		CHECK(repo.get<bool>("benchmarks"));
		CHECK(repo.get<std::size_t>("max-fixpoint-steps") == 9);
		// a core option has no short letter on an artifact
		CHECK_FALSE(parse(repo, { "program", "-f", "9" },
			artifact_args_config).has_value());
		auto unknown = parse(repo, { "program", "--color" },
			artifact_args_config);
		CHECK_FALSE(unknown.has_value());
		CHECK(has_text(unknown.report(), "Unknown option"));
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

	TEST_CASE("parse_args writes the cli options") {
		idni::options_repository repo;
		idni::options_scope scope(repo);
		REQUIRE(declare_program_options(repo, tau_cli_option_set,
			tau_surfaces).has_value());
		auto r = parse(repo, { "tau", "-S", "trace", "-q" });
		REQUIRE(r.has_value());
		CHECK(repo.get<std::string>("severity") == "trace");
		CHECK(repo.get<bool>("quit"));
		CHECK(repo.get<bool>("benchmarks"));
		CHECK_FALSE(repo.get<bool>("json"));
	}

	TEST_CASE("an unknown flag and a bad severity are errors") {
		idni::options_repository repo;
		idni::options_scope scope(repo);
		REQUIRE(declare_program_options(repo, tau_cli_option_set,
			tau_surfaces).has_value());
		auto unknown = parse(repo, { "tau", "--nope" });
		CHECK_FALSE(unknown.has_value());
		CHECK(has_text(unknown.report(), "Unknown option"));
		// a field bound with the codec, as the REPL binds it, refuses it
		auto level = boost::log::trivial::info;
		REQUIRE(repo.bind("severity", level, severity_codec{}).has_value());
		REQUIRE(parse(repo, { "tau", "-S", "error" }).has_value());
		CHECK(level == boost::log::trivial::error);
		CHECK_FALSE(parse(repo, { "tau", "-S", "bogus" }).has_value());
		CHECK(level == boost::log::trivial::error);
		repo.unbind("severity");
		// main() reads an unbound severity through the codec
		REQUIRE(parse(repo, { "tau", "-S", "bogus" }).has_value());
		auto read = severity_codec{}.from_value(repo.value("severity"));
		CHECK_FALSE(read.has_value());
		CHECK(has_text(read.report(), "does not take the value"));
	}

	TEST_CASE("compile takes its options after the command, define repeats") {
		idni::options_repository repo;
		idni::options_scope scope(repo);
		REQUIRE(declare_program_options(repo, tau_cli_option_set,
			tau_surfaces).has_value());
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

	TEST_CASE("cxx and sdk-dir read the environment, and the flag wins") {
		idni::options_repository repo;
		idni::options_scope scope(repo);
		REQUIRE(declare_program_options(repo, tau_cli_option_set,
			tau_surfaces).has_value());
		REQUIRE(setenv("TAU_CXX", "env-cc", 1) == 0);
		REQUIRE(setenv("TAU_SDK_DIR", "env-sdk", 1) == 0);
		REQUIRE(setenv("TAU_OUTPUT", "env-out", 1) == 0);
		auto env = repo.load_env("TAU_");
		unsetenv("TAU_CXX");
		unsetenv("TAU_SDK_DIR");
		unsetenv("TAU_OUTPUT");
		REQUIRE(env.has_value());
		CHECK(repo.get<std::string>("cxx") == "env-cc");
		CHECK(repo.get<std::string>("sdk-dir") == "env-sdk");
		CHECK(repo.get<std::string>("output").empty());
		REQUIRE(parse(repo, { "tau", "compile", "-c", "flag-cc",
			"--sdk-dir", "flag-sdk" }).has_value());
		CHECK(repo.get<std::string>("cxx") == "flag-cc");
		CHECK(repo.get<std::string>("sdk-dir") == "flag-sdk");
	}

	TEST_CASE("-o is the output directory of gen and codegen") {
		for (const char* verb : { "gen", "codegen" }) {
			CAPTURE(verb);
			idni::options_repository repo;
			idni::options_scope scope(repo);
			REQUIRE(declare_program_options(repo, tau_cli_option_set,
				tau_surfaces).has_value());
			REQUIRE(parse(repo, { "tau", verb, "-o", "dir" }).has_value());
			CHECK(repo.get<std::string>("output-dir") == "dir");
			CHECK(repo.get<std::string>("output").empty());
		}
	}

	TEST_CASE("a core option gets its long form and its short letter") {
		idni::options_repository repo;
		idni::options_scope scope(repo);
		REQUIRE(repo.declare(core_sample).has_value());
		REQUIRE(declare_program_options(repo, tau_cli_option_set,
			tau_surfaces).has_value());
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
