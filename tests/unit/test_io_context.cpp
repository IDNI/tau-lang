// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// file_input_stream, file_output_stream, console_input_stream,
// console_output_stream and the free function spacing() (src/io_context.h /
// src/io_context.tmpl.h) had zero test coverage anywhere in the repo -- none
// of these names appear in any other test file. They are plain (non-template)
// structs/functions in namespace idni::tau_lang, so no NodeType/BA setup is
// needed here, just the header itself.
//
// console_prompt_input_stream/console_prompt_output_stream are intentionally
// NOT covered here: both share a process-wide `static size_t max_length`
// mutated by every instance's constructor across the whole test binary, so
// asserting exact padded output would be order-dependent on whatever other
// tests happen to run in the same process.

#include "test_init.h"
#include "io_context.h"

#include <filesystem>
#include <fstream>
#include <sstream>

using namespace idni::tau_lang;

namespace {

// Unique-per-call path under the system temp directory so parallel/ordered
// test cases never collide on the same file.
std::filesystem::path unique_temp_path(const std::string& tag) {
	static size_t counter = 0;
	auto dir = std::filesystem::temp_directory_path();
	return dir / ("test_io_context_" + tag + "_"
		+ std::to_string(++counter) + ".tmp");
}

// RAII guard restoring a redirected std::ios rdbuf even if a CHECK/REQUIRE
// fails (doctest turns failed CHECKs into control flow that still unwinds
// the stack, and REQUIRE always does), so std::cin/std::cout -- shared by
// the whole test process -- are never left redirected.
template <typename Stream>
struct rdbuf_guard {
	Stream& stream;
	std::streambuf* saved;
	rdbuf_guard(Stream& stream, std::streambuf* new_buf)
		: stream(stream), saved(stream.rdbuf(new_buf)) {}
	~rdbuf_guard() { stream.rdbuf(saved); }
};

} // namespace

TEST_SUITE("file_output_stream") {

	TEST_CASE("put() writes lines that a fresh file_input_stream reads back in order") {
		auto path = unique_temp_path("out_roundtrip");
		{
			file_output_stream out(path.string());
			CHECK(out.put("first"));
			CHECK(out.put("second"));
			CHECK(out.put("third"));
		}
		file_input_stream in(path.string());
		auto l1 = in.get(); REQUIRE(l1.has_value()); CHECK(l1.value() == "first");
		auto l2 = in.get(); REQUIRE(l2.has_value()); CHECK(l2.value() == "second");
		auto l3 = in.get(); REQUIRE(l3.has_value()); CHECK(l3.value() == "third");
		std::filesystem::remove(path);
	}

	TEST_CASE("rebuild() truncates the file") {
		auto path = unique_temp_path("out_truncate");
		auto out = std::make_shared<file_output_stream>(path.string());
		CHECK(out->put("some content"));
		CHECK(out->put("more content"));
		REQUIRE(std::filesystem::file_size(path) > 0);

		auto rebuilt = out->rebuild();
		REQUIRE(rebuilt);
		CHECK(std::filesystem::file_size(path) == 0);
		std::filesystem::remove(path);
	}
}

TEST_SUITE("file_input_stream") {

	TEST_CASE("rebuild() reopens the file and reads from the beginning again") {
		auto path = unique_temp_path("in_rebuild");
		{
			std::ofstream seed(path);
			seed << "alpha" << std::endl;
			seed << "beta" << std::endl;
			seed << "gamma" << std::endl;
		}
		auto in = std::make_shared<file_input_stream>(path.string());
		auto first = in->get();
		REQUIRE(first.has_value());
		CHECK(first.value() == "alpha");

		auto rebuilt = in->rebuild();
		REQUIRE(rebuilt);
		auto after_rebuild = rebuilt->get();
		REQUIRE(after_rebuild.has_value());
		CHECK(after_rebuild.value() == "alpha");
		std::filesystem::remove(path);
	}

	TEST_CASE("opening a nonexistent path does not crash and get() returns an empty value") {
		auto path = unique_temp_path("in_nonexistent");
		if (std::filesystem::exists(path)) std::filesystem::remove(path);

		file_input_stream in(path.string());
		auto result = in.get();
		CHECK(result.has_value());
		CHECK(result.value().empty());
	}

	TEST_CASE("get() past end-of-stream keeps returning has_value()==true with an empty string") {
		auto path = unique_temp_path("in_eof");
		{
			std::ofstream seed(path);
			seed << "only line" << std::endl;
		}
		file_input_stream in(path.string());
		auto l1 = in.get();
		REQUIRE(l1.has_value());
		CHECK(l1.value() == "only line");

		// Past EOF: documented quirk is has_value()==true with an empty
		// string, not nullopt, diverging from the base-class contract.
		auto l2 = in.get();
		CHECK(l2.has_value());
		CHECK(l2.value().empty());
		auto l3 = in.get();
		CHECK(l3.has_value());
		CHECK(l3.value().empty());
		std::filesystem::remove(path);
	}
}

TEST_SUITE("spacing") {

	TEST_CASE("name shorter than max_length pads with the difference in spaces") {
		CHECK(spacing("abc", 5) == "  ");
	}

	TEST_CASE("name exactly max_length long pads with nothing") {
		CHECK(spacing("abcde", 5) == "");
	}

	TEST_CASE("name longer than max_length pads with nothing") {
		CHECK(spacing("abcdefg", 5) == "");
	}
}

TEST_SUITE("console_output_stream") {

	TEST_CASE("put() writes the value followed by a newline to std::cout") {
		std::ostringstream captured;
		rdbuf_guard<std::ostream> guard(std::cout, captured.rdbuf());

		console_output_stream out;
		CHECK(out.put("hello console"));

		CHECK(captured.str() == "hello console\n");
	}
}

TEST_SUITE("console_input_stream") {

	TEST_CASE("get() reads a line from std::cin") {
		std::istringstream fed("seeded line\n");
		rdbuf_guard<std::istream> guard(std::cin, fed.rdbuf());

		console_input_stream in;
		auto result = in.get();

		REQUIRE(result.has_value());
		CHECK(result.value() == "seeded line");
	}
}
