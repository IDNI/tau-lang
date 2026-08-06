// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for the I/O stream implementations in src/io_context.tmpl.h.
//
// Coverage measurement (2026-08-01) showed 98 of 215 lines in that file cold
// (54.4% covered) -- the thinnest substantial file in src/ after main.cpp.
// io_context<node> itself and its operator<< were already covered through the
// interpreter tests; what had no test anywhere were the concrete stream
// classes: console_input/output_stream, console_prompt_input/output_stream,
// repl_pending_input_stream, file_input/output_stream, and the get/put/clear/
// rebuild members of vector_input/output_stream.
//
// The console streams are exercised by swapping std::cin / std::cout rdbufs.
// term::enable_getline_mode() early-returns when no terminal is open, so this
// is safe under ctest.

#include "test_init.h"
#include "test_tau_helpers.h"

#include "io_context.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace {

// --- stdin/stdout redirection helpers -------------------------------------

struct cin_redirect {
	std::istringstream iss;
	std::streambuf* saved;
	explicit cin_redirect(const std::string& text)
		: iss(text), saved(std::cin.rdbuf(iss.rdbuf())) {}
	~cin_redirect() { std::cin.rdbuf(saved); }
};

struct cout_capture {
	std::ostringstream oss;
	std::streambuf* saved;
	cout_capture() : saved(std::cout.rdbuf(oss.rdbuf())) {}
	~cout_capture() { std::cout.rdbuf(saved); }
	std::string str() const { return oss.str(); }
};

struct cerr_capture {
	std::ostringstream oss;
	std::streambuf* saved;
	cerr_capture() : saved(std::cerr.rdbuf(oss.rdbuf())) {}
	~cerr_capture() { std::cerr.rdbuf(saved); }
	std::string str() const { return oss.str(); }
};

// --- temporary file helper ------------------------------------------------

// Unique path under the system temp dir, removed on scope exit.
struct temp_file {
	std::filesystem::path path;
	explicit temp_file(const std::string& tag)
		: path(std::filesystem::temp_directory_path()
			/ ("tau_test_io_context_" + tag)) {
		std::error_code ec;
		std::filesystem::remove(path, ec);
	}
	~temp_file() {
		std::error_code ec;
		std::filesystem::remove(path, ec);
	}
	std::string str() const { return path.string(); }
};

std::vector<std::string> read_lines(const std::filesystem::path& p) {
	std::vector<std::string> out;
	std::ifstream in(p);
	for (std::string line; std::getline(in, line); ) out.push_back(line);
	return out;
}

} // namespace

// ---------------------------------------------------------------------------
// vector_input_stream
// ---------------------------------------------------------------------------

TEST_SUITE("vector_input_stream") {

	TEST_CASE("reads pre-loaded values in order") {
		vector_input_stream s(std::vector<std::string>{"a", "b", "c"});
		CHECK(s.get() == std::optional<std::string>("a"));
		CHECK(s.get() == std::optional<std::string>("b"));
		CHECK(s.get() == std::optional<std::string>("c"));
	}

	// Deliberate design choice, documented in the implementation: exhaustion
	// yields an EMPTY STRING rather than nullopt, because running out of a
	// finite pre-supplied input list is the graceful end of a scripted run,
	// not a broken stream. Contrast vector_output_stream::get() below, which
	// returns nullopt. Pinning both so the asymmetry cannot drift silently.
	TEST_CASE("exhaustion yields an empty string, not nullopt") {
		vector_input_stream s(std::vector<std::string>{"only"});
		CHECK(s.get() == std::optional<std::string>("only"));
		CHECK(s.get() == std::optional<std::string>(std::string{}));
		CHECK(s.get().has_value());          // NOT nullopt
	}

	TEST_CASE("default-constructed stream is immediately exhausted") {
		vector_input_stream s;
		CHECK(s.get() == std::optional<std::string>(std::string{}));
	}

	TEST_CASE("put appends a readable value") {
		vector_input_stream s;
		s.put("x");
		s.put("y");
		CHECK(s.get() == std::optional<std::string>("x"));
		CHECK(s.get() == std::optional<std::string>("y"));
		CHECK(s.get() == std::optional<std::string>(std::string{}));
	}

	TEST_CASE("shared values and cursor constructor") {
		auto values = std::make_shared<std::vector<std::string>>(
			std::vector<std::string>{"p", "q"});
		auto current = std::make_shared<size_t>(1);
		vector_input_stream s(values, current);
		CHECK(s.get() == std::optional<std::string>("q"));  // starts at index 1
		CHECK(*current == 2);
	}

	// rebuild() semantics are INCONSISTENT across the stream implementations,
	// and the documented contract conflicts with one of its two call sites.
	//
	//   - The doc comments say "Produce a fresh, rewound copy of this stream"
	//     (base) and "Rebuild by resetting the cursor to the beginning"
	//     (vector_input_stream), i.e. rewind.
	//   - vector_input_stream::rebuild() passes the SAME shared_ptr<size_t>
	//     cursor, so it CONTINUES where it left off (asserted below).
	//   - file_input_stream::rebuild() reopens the file, so it REWINDS.
	//
	// The two call sites want different things. interpreter::make_interpreter()
	// rebuilds once at construction, where the cursor is 0 and both behaviours
	// coincide. But interpreter::update() (src/interpreter.tmpl.h:1014, reached
	// from run() when the tau-typed `u` output carries a pointwise spec
	// revision) rebuilds MID-RUN at time_point > 0 -- there, continuing is the
	// sensible behaviour and rewinding would replay already-consumed inputs.
	//
	// So vector_input_stream matches the mid-run call site while contradicting
	// its own doc, and file_input_stream matches the doc while looking wrong
	// for the mid-run update. One of the two must be wrong; the contract as
	// written does not settle which. Both behaviours are pinned by tests (see
	// also "file_input_stream rebuild rereads from the beginning" below) so
	// that whichever way the contract is resolved, the disagreement surfaces.
	TEST_CASE("rebuild CONTINUES from the cursor (shares it) -- see comment") {
		vector_input_stream s(std::vector<std::string>{"first", "second"});
		CHECK(s.get() == std::optional<std::string>("first"));
		auto rebuilt = s.rebuild();
		// The doc comments would give "first" here; the implementation
		// continues and gives "second".
		CHECK(rebuilt->get() == std::optional<std::string>("second"));
	}

	TEST_CASE("rebuild shares the backing values") {
		vector_input_stream s(std::vector<std::string>{"v"});
		auto rebuilt = s.rebuild();
		REQUIRE(rebuilt != nullptr);
		// Cursor is shared, so the original is advanced by the rebuilt read.
		CHECK(rebuilt->get() == std::optional<std::string>("v"));
		CHECK(s.get() == std::optional<std::string>(std::string{}));
	}
}

// ---------------------------------------------------------------------------
// vector_output_stream
// ---------------------------------------------------------------------------

TEST_SUITE("vector_output_stream") {

	TEST_CASE("put accumulates values") {
		vector_output_stream s;
		CHECK(s.put("one"));
		CHECK(s.put("two"));
		CHECK(s.get_values() == std::vector<std::string>{"one", "two"});
	}

	// Contrast with vector_input_stream::get(): this one returns nullopt.
	TEST_CASE("get reads back in order and returns nullopt when exhausted") {
		vector_output_stream s;
		s.put("a");
		s.put("b");
		CHECK(s.get() == std::optional<std::string>("a"));
		CHECK(s.get() == std::optional<std::string>("b"));
		CHECK(s.get() == std::nullopt);
	}

	TEST_CASE("get on an empty stream is nullopt") {
		vector_output_stream s;
		CHECK(s.get() == std::nullopt);
	}

	TEST_CASE("clear empties the store and resets the cursor") {
		vector_output_stream s;
		s.put("a");
		s.put("b");
		CHECK(s.get() == std::optional<std::string>("a"));
		s.clear();
		CHECK(s.get_values().empty());
		s.put("fresh");
		// Cursor was reset, so the newly put value is readable.
		CHECK(s.get() == std::optional<std::string>("fresh"));
	}

	TEST_CASE("shared-values constructor writes through") {
		auto values = std::make_shared<std::vector<std::string>>();
		vector_output_stream s(values);
		s.put("shared");
		CHECK(values->size() == 1);
		CHECK(values->at(0) == "shared");
	}

	// Unlike the input stream, this rebuild DOES reset the cursor (it builds a
	// fresh current), while sharing the values -- matching the contract.
	TEST_CASE("rebuild shares values but resets the cursor") {
		auto values = std::make_shared<std::vector<std::string>>();
		vector_output_stream s(values);
		s.put("x");
		CHECK(s.get() == std::optional<std::string>("x"));
		CHECK(s.get() == std::nullopt);
		auto rebuilt = s.rebuild();
		REQUIRE(rebuilt != nullptr);
		// Rebuilt stream sees the same values from the start again.
		auto* vos = dynamic_cast<vector_output_stream*>(rebuilt.get());
		REQUIRE(vos != nullptr);
		CHECK(vos->get() == std::optional<std::string>("x"));
	}

	// The time-point overload lives only on the base class: overriding
	// put(const string&) here HIDES the inherited put(value, time_point), so
	// it must be reached through a base reference. (console_prompt_output_stream
	// avoids this with `using console_output_stream::put;`, but
	// vector_output_stream and file_output_stream do not -- a usability trap
	// for direct users, though harmless for the interpreter, which always
	// holds these as shared_ptr<serialized_constant_output_stream>.)
	TEST_CASE("put via the base time-point overload delegates to put") {
		vector_output_stream s;
		serialized_constant_output_stream& base = s;
		CHECK(base.put("val", 7));
		CHECK(s.get_values() == std::vector<std::string>{"val"});
	}
}

// ---------------------------------------------------------------------------
// file_output_stream / file_input_stream
// ---------------------------------------------------------------------------

TEST_SUITE("file streams") {

	TEST_CASE("written values round-trip through a file") {
		temp_file tf("roundtrip");
		{
			file_output_stream out(tf.str());
			CHECK(out.put("line one"));
			CHECK(out.put("line two"));
		}   // destructor closes the file
		CHECK(read_lines(tf.path)
			== std::vector<std::string>{"line one", "line two"});

		file_input_stream in(tf.str());
		CHECK(in.get() == std::optional<std::string>("line one"));
		CHECK(in.get() == std::optional<std::string>("line two"));
	}

	TEST_CASE("reading past the end yields an empty string") {
		temp_file tf("eof");
		{
			file_output_stream out(tf.str());
			CHECK(out.put("only"));
		}
		file_input_stream in(tf.str());
		CHECK(in.get() == std::optional<std::string>("only"));
		CHECK(in.get() == std::optional<std::string>(std::string{}));
	}

	// The failure branch of the file_output_stream constructor: a path whose
	// parent directory does not exist cannot be opened for writing. It logs
	// an error rather than throwing, and put() then reports failure.
	TEST_CASE("opening an unwritable output path does not throw") {
		const std::string bad =
			(std::filesystem::temp_directory_path()
				/ "tau_test_io_context_absent_dir"
				/ "out.txt").string();
		REQUIRE(!std::filesystem::exists(
			std::filesystem::path(bad).parent_path()));
		file_output_stream out(bad);
		CHECK(!out.put("never written"));
	}

	// Opening a missing file logs an error rather than throwing; get() then
	// yields an empty string. This is the failure branch of the constructor.
	TEST_CASE("opening a missing file does not throw") {
		const std::string missing =
			(std::filesystem::temp_directory_path()
				/ "tau_test_io_context_definitely_absent").string();
		std::error_code ec;
		std::filesystem::remove(missing, ec);
		file_input_stream in(missing);
		CHECK(in.get() == std::optional<std::string>(std::string{}));
	}

	TEST_CASE("file_input_stream rebuild rereads from the beginning") {
		temp_file tf("in_rebuild");
		{
			file_output_stream out(tf.str());
			CHECK(out.put("alpha"));
			CHECK(out.put("beta"));
		}
		file_input_stream in(tf.str());
		CHECK(in.get() == std::optional<std::string>("alpha"));
		auto rebuilt = in.rebuild();
		REQUIRE(rebuilt != nullptr);
		// Contrast with vector_input_stream::rebuild(), which continues from
		// the cursor: this one REWINDS. See the long comment on
		// "rebuild CONTINUES from the cursor" above -- these two
		// implementations of the same virtual disagree, and a mid-run
		// interpreter::update() would replay inputs through this one.
		CHECK(rebuilt->get() == std::optional<std::string>("alpha"));
	}

	TEST_CASE("file_output_stream rebuild reopens the file") {
		temp_file tf("out_rebuild");
		file_output_stream out(tf.str());
		CHECK(out.put("first"));
		auto rebuilt = out.rebuild();
		REQUIRE(rebuilt != nullptr);
		CHECK(rebuilt->put("second"));
	}

	// As with vector_output_stream, the time-point overload is hidden by the
	// put(const string&) override and must be reached via the base class.
	TEST_CASE("put through the base time-point overload writes the value") {
		temp_file tf("tp");
		{
			file_output_stream out(tf.str());
			serialized_constant_output_stream& base = out;
			CHECK(base.put("tpvalue", 3));
		}
		CHECK(read_lines(tf.path) == std::vector<std::string>{"tpvalue"});
	}

	// Regression guard for an ordinary NAMED output file: a fresh open must
	// still start from a clean, truncated file -- this is the behaviour the
	// /dev/stdout fix below deliberately leaves untouched (a fresh `run`
	// should not accumulate a previous run's leftover content). Pinning it
	// explicitly since the /dev/stdout fix lives in the very same
	// constructor/put() this exercises.
	TEST_CASE("a fresh open of an ordinary named file truncates prior content") {
		temp_file tf("truncate");
		{
			file_output_stream out(tf.str());
			CHECK(out.put("stale"));
			CHECK(out.put("content"));
		}
		CHECK(read_lines(tf.path)
			== std::vector<std::string>{"stale", "content"});
		{
			// Re-open the SAME path fresh (same as a new `run` would): the
			// old two lines must be gone, not appended to.
			file_output_stream out(tf.str());
			CHECK(out.put("fresh"));
		}
		CHECK(read_lines(tf.path) == std::vector<std::string>{"fresh"});
	}

	// Defect A (private/2026-08-06-adt-demo-extension-report.md): opening
	// "/dev/stdout" as an ordinary named file creates a SECOND, independent
	// file description on the same underlying file std::cout already
	// writes to (fd 1); the two descriptions' independently-tracked
	// offsets, combined with the default open mode's truncation, produced
	// NUL-byte holes once both were flushed to a regular (seekable) file
	// (reproduced against the pre-fix binaries: 144 NUL bytes via the
	// report's own repro; zero on a pipe, since a pipe has no offset
	// concept to corrupt). The fix routes "/dev/stdout"/"/dev/stderr"
	// through the process's own std::cout/std::cerr instead of opening a
	// private ofstream on the same path, sidestepping the dual-description
	// problem entirely rather than trying to make two independent
	// descriptions agree on a shared offset. Directly testable via rdbuf
	// redirection (the coordinator's suggested "temp regular file
	// exercising the same write path" does not apply here: the fix does
	// NOT touch how ordinary named files are opened at all, only these two
	// specific paths -- see the truncation regression guard above for that
	// unchanged path instead).
	TEST_CASE("\"/dev/stdout\" writes through std::cout, not a private file") {
		cout_capture out;
		file_output_stream s("/dev/stdout");
		CHECK(s.put("hello"));
		CHECK(out.str() == "hello\n");
	}
	TEST_CASE("\"/dev/stderr\" writes through std::cerr, not a private file") {
		cerr_capture err;
		file_output_stream s("/dev/stderr");
		CHECK(s.put("world"));
		CHECK(err.str() == "world\n");
	}
}

// ---------------------------------------------------------------------------
// repl_pending_input_stream
// ---------------------------------------------------------------------------

TEST_SUITE("repl_pending_input_stream") {

	TEST_CASE("a fresh stream is not awaiting") {
		repl_pending_input_stream s;
		CHECK(!s.awaiting());
	}

	// With no value set, get() flags the stream as awaiting and returns "" so
	// the interpreter step stops cleanly (no exceptions, for WASM).
	TEST_CASE("get with no pending value flags awaiting and returns empty") {
		repl_pending_input_stream s;
		CHECK(s.get() == std::optional<std::string>(std::string{}));
		CHECK(s.awaiting());
		CHECK(s.awaiting_time_point() == 0);
	}

	TEST_CASE("the awaited time point is recorded") {
		repl_pending_input_stream s;
		CHECK(s.get(42) == std::optional<std::string>(std::string{}));
		CHECK(s.awaiting());
		CHECK(s.awaiting_time_point() == 42);
	}

	TEST_CASE("set then get returns the value and clears awaiting") {
		repl_pending_input_stream s;
		CHECK(s.get() == std::optional<std::string>(std::string{}));
		CHECK(s.awaiting());
		s.set("supplied");
		CHECK(!s.awaiting());                     // set() clears the flag
		CHECK(s.get() == std::optional<std::string>("supplied"));
		CHECK(!s.awaiting());
	}

	// A step needing several console inputs is entered once per value the
	// REPL collects (interpreter::read() stops at the first stream without
	// one), so every attempt re-reads the streams that already answered.
	// The value therefore stays available for the whole time point.
	TEST_CASE("the value is re-delivered within the same time point") {
		repl_pending_input_stream s;
		s.set("once");
		CHECK(s.get(3) == std::optional<std::string>("once"));
		CHECK(s.get(3) == std::optional<std::string>("once"));
		CHECK(!s.awaiting());
	}

	// ... and is gone once the step that asked for it has completed.
	TEST_CASE("the value does not carry over to the next time point") {
		repl_pending_input_stream s;
		s.set("once");
		CHECK(s.get(3) == std::optional<std::string>("once"));
		CHECK(s.get(4) == std::optional<std::string>(std::string{}));
		CHECK(s.awaiting());
		CHECK(s.awaiting_time_point() == 4);
	}

	TEST_CASE("rebuild yields a fresh, non-awaiting stream") {
		repl_pending_input_stream s;
		CHECK(s.get() == std::optional<std::string>(std::string{}));
		CHECK(s.awaiting());
		auto rebuilt = s.rebuild();
		REQUIRE(rebuilt != nullptr);
		auto* r = dynamic_cast<repl_pending_input_stream*>(rebuilt.get());
		REQUIRE(r != nullptr);
		CHECK(!r->awaiting());
	}
}

// ---------------------------------------------------------------------------
// console streams
// ---------------------------------------------------------------------------

TEST_SUITE("console_input_stream") {

	TEST_CASE("get reads one line from stdin") {
		cin_redirect in("hello\nworld\n");
		console_input_stream s;
		CHECK(s.get() == std::optional<std::string>("hello"));
		CHECK(s.get() == std::optional<std::string>("world"));
	}

	TEST_CASE("get at end of input yields an empty string") {
		cin_redirect in("");
		console_input_stream s;
		CHECK(s.get() == std::optional<std::string>(std::string{}));
	}

	TEST_CASE("rebuild yields a usable stream") {
		console_input_stream s;
		auto rebuilt = s.rebuild();
		REQUIRE(rebuilt != nullptr);
		cin_redirect in("via rebuild\n");
		CHECK(rebuilt->get() == std::optional<std::string>("via rebuild"));
	}
}

TEST_SUITE("console_output_stream") {

	TEST_CASE("put writes the value and a newline to stdout") {
		cout_capture out;
		console_output_stream s;
		CHECK(s.put("printed"));
		CHECK(out.str() == "printed\n");
	}

	TEST_CASE("rebuild yields a usable stream") {
		console_output_stream s;
		auto rebuilt = s.rebuild();
		REQUIRE(rebuilt != nullptr);
		cout_capture out;
		CHECK(rebuilt->put("from rebuild"));
		CHECK(out.str() == "from rebuild\n");
	}
}

TEST_SUITE("console_prompt_input_stream") {

	// The prompt is "<name>[<time_point>]<spacing> := "; spacing depends on a
	// class-static max_length shared across instances, so only the stable
	// parts are asserted.
	TEST_CASE("get prompts with name and time point, then reads") {
		cin_redirect in("42\n");
		cout_capture out;
		console_prompt_input_stream s("myvar");
		CHECK(s.get(7) == std::optional<std::string>("42"));
		const std::string prompt = out.str();
		CHECK(prompt.find("myvar[7]") != std::string::npos);
		CHECK(prompt.find(":=") != std::string::npos);
	}

	TEST_CASE("the inherited no-prompt get does not print") {
		cin_redirect in("plain\n");
		cout_capture out;
		console_prompt_input_stream s("v");
		CHECK(s.get() == std::optional<std::string>("plain"));
		CHECK(out.str().empty());
	}

	TEST_CASE("rebuild preserves the prompt name") {
		console_prompt_input_stream s("kept");
		auto rebuilt = s.rebuild();
		REQUIRE(rebuilt != nullptr);
		cin_redirect in("v\n");
		cout_capture out;
		CHECK(rebuilt->get(1) == std::optional<std::string>("v"));
		CHECK(out.str().find("kept[1]") != std::string::npos);
	}
}

// ---------------------------------------------------------------------------
// ADT tuple streams (adt_tuple_reader / adt_tuple_writer)
// ---------------------------------------------------------------------------

namespace {

// component.io_var/base_type are unused by adt_tuple_reader/adt_tuple_writer
// (both only ever consult component.path -- see adt_build_shape and
// adt_tuple_writer::format in io_context.tmpl.h), so these tests leave them
// default (null) and only fill in the paths that actually drive behaviour.
adt_stream_component<node_t> adt_test_component(std::vector<std::string> path) {
	adt_stream_component<node_t> c;
	for (const auto& p : path) c.path.push_back(dict(p));
	c.io_var = htref{};
	c.base_type = nullptr;
	return c;
}

// layout: { a }, { p, x } -- matches the "a"/"p.x" shape used throughout
// this suite (mirrors the brief's own sketch).
adt_stream_layout<node_t> adt_test_layout() {
	adt_stream_layout<node_t> layout;
	layout.root_name_sid = dict("root");
	layout.is_input = true;
	layout.stream_id = 0;
	layout.components = {
		adt_test_component({"a"}),
		adt_test_component({"p", "x"}),
	};
	return layout;
}

} // namespace

TEST_SUITE("adt tuple streams") {

	TEST_CASE("reader routes leaves") {
		// Also exercises per-time-point memoization: the vector stream
		// supplies only ONE line, so if leaf() re-read the physical stream
		// for the second member instead of reusing the first parse, the
		// sequential vector_input_stream would already be exhausted and the
		// second leaf() call below would fail.
		auto layout = adt_test_layout();
		adt_tuple_reader<node_t> reader(
			std::make_unique<vector_input_stream>(std::vector<std::string>{
				"{ a: \"0\", p: { x: \"1\" } }"
			}), layout);
		CHECK(reader.leaf(0, { dict("a") }) == std::optional<std::string>("0"));
		CHECK(reader.leaf(0, { dict("p"), dict("x") }) == std::optional<std::string>("1"));
	}

	// Regression test for a Critical review finding on the REPL `run`
	// integration (repl_evaluator.tmpl.h/interpreter.tmpl.h): an empty
	// physical read (the REPL's non-blocking repl_pending_input_stream
	// "awaiting" a value, or a plain exhausted vector_input_stream/
	// file_input_stream/EOF console) used to make leaf() return nullopt --
	// the SAME thing a genuine parse/validation failure returns -- so
	// interpreter::read() took its noisy `!maybe_line.has_value()`
	// LOG_ERROR branch on every single awaiting/EOF cycle, not just on an
	// actual failure. A plain (non-tuple) stream's own get() never has
	// this problem: it already returns a PRESENT-but-empty string for
	// exactly this case ("exhaustion yields an empty string, not nullopt",
	// vector_input_stream suite above) which read() treats as its quiet
	// `line.empty()` end-of-input path. leaf() must mirror that contract.
	TEST_CASE("reader propagates an empty physical read as an empty leaf, "
		  "not an error")
	{
		auto layout = adt_test_layout();
		// A default-constructed vector_input_stream is immediately
		// exhausted: get() returns std::optional<std::string>("").
		adt_tuple_reader<node_t> reader(
			std::make_unique<vector_input_stream>(), layout);
		auto leaf = reader.leaf(0, { dict("a") });
		CHECK(leaf.has_value());             // NOT nullopt/an error
		CHECK(*leaf == "");
	}

	// Two calls for the SAME time point, both empty: the empty result must
	// not be memoized as a permanent failure (unlike a genuine parse
	// failure, see "reader rejects malformed wire input" below) -- a later
	// call for the same time point (the REPL re-driving this reader's
	// physical stream after the user answers) has to re-consult the
	// physical stream each time, not get stuck on a stale "no value yet".
	TEST_CASE("an empty read is not memoized as a failure") {
		auto layout = adt_test_layout();
		adt_tuple_reader<node_t> reader(
			std::make_unique<vector_input_stream>(), layout);
		CHECK(reader.leaf(0, { dict("a") }).has_value());
		CHECK(*reader.leaf(0, { dict("a") }) == "");
		CHECK(*reader.leaf(0, { dict("p"), dict("x") }) == "");
	}

	TEST_CASE("reader rejects missing key") {
		auto layout = adt_test_layout();
		adt_tuple_reader<node_t> reader(
			std::make_unique<vector_input_stream>(std::vector<std::string>{
				"{ a: \"0\" }"
			}), layout);
		CHECK(reader.leaf(0, { dict("a") }) == std::nullopt);
	}

	TEST_CASE("reader rejects unknown key") {
		auto layout = adt_test_layout();
		adt_tuple_reader<node_t> reader(
			std::make_unique<vector_input_stream>(std::vector<std::string>{
				"{ a: \"0\", z: \"1\" }"
			}), layout);
		CHECK(reader.leaf(0, { dict("a") }) == std::nullopt);
	}

	TEST_CASE("reader rejects duplicate key") {
		auto layout = adt_test_layout();
		adt_tuple_reader<node_t> reader(
			std::make_unique<vector_input_stream>(std::vector<std::string>{
				"{ a: \"0\", a: \"1\", p: { x: \"1\" } }"
			}), layout);
		CHECK(reader.leaf(0, { dict("a") }) == std::nullopt);
	}

	TEST_CASE("reader rejects a leaf where an object is expected") {
		auto layout = adt_test_layout();
		adt_tuple_reader<node_t> reader(
			std::make_unique<vector_input_stream>(std::vector<std::string>{
				"{ a: \"0\", p: \"1\" }"
			}), layout);
		CHECK(reader.leaf(0, { dict("a") }) == std::nullopt);
	}

	TEST_CASE("reader rejects malformed wire input") {
		auto layout = adt_test_layout();
		adt_tuple_reader<node_t> reader(
			std::make_unique<vector_input_stream>(std::vector<std::string>{
				"not a tuple literal"
			}), layout);
		CHECK(reader.leaf(0, { dict("a") }) == std::nullopt);
	}

	// Regression test for a High review finding: read_time_point used to
	// latch memo_ok=false permanently the first time a malformed line was
	// read for a time point, short-circuiting on every later call WITHOUT
	// ever consulting the physical stream again -- so a corrected value the
	// REPL's user submitted after a bad attempt (continue_running
	// re-entering the SAME time point via repl_pending_input_stream::set())
	// was silently discarded and the stream stayed stuck failing that time
	// point forever, with no escape short of aborting the whole run (a
	// plain, non-tuple stream re-parses fresh on every retry and has no
	// such trap). A vector_input_stream simulates the two successive user
	// attempts as two queued lines: leaf() is called once per attempt here,
	// exactly like continue_running calls it once per resumed step.
	TEST_CASE("a corrected line after a failed one for the SAME time point "
		  "succeeds")
	{
		auto layout = adt_test_layout();
		adt_tuple_reader<node_t> reader(
			std::make_unique<vector_input_stream>(std::vector<std::string>{
				"not a tuple literal",             // attempt 1: malformed
				"{ a: \"1\", p: { x: \"0\" } }"     // attempt 2: corrected
			}), layout);
		CHECK(reader.leaf(0, { dict("a") }) == std::nullopt);
		CHECK(reader.leaf(0, { dict("a") }) == std::optional<std::string>("1"));
		CHECK(reader.leaf(0, { dict("p"), dict("x") })
			== std::optional<std::string>("0"));
	}

	// Contrast case: the SAME malformed text resubmitted (not a
	// correction -- e.g. the user retypes the identical mistake) must keep
	// failing, not spuriously succeed from a stale memo. Queues the
	// identical malformed line TWICE, so the second leaf() call's re-read
	// of the physical stream (necessary to tell "same line" from "a
	// correction arrived" apart in the first place) hands back the exact
	// same text; the raw-line comparison then reuses the cached failure
	// instead of re-parsing it. (An empty THIRD read here -- vector_input_
	// stream's own exhaustion-yields-"" contract -- would take the
	// unrelated `empty` path, not `failed`, which is why this only queues
	// and calls leaf() exactly twice.)
	TEST_CASE("resubmitting the exact same malformed line still fails") {
		auto layout = adt_test_layout();
		adt_tuple_reader<node_t> reader(
			std::make_unique<vector_input_stream>(std::vector<std::string>{
				"not a tuple literal",
				"not a tuple literal"
			}), layout);
		CHECK(reader.leaf(0, { dict("a") }) == std::nullopt);
		CHECK(reader.leaf(0, { dict("a") }) == std::nullopt);
	}

	TEST_CASE("writer formats when complete") {
		auto layout = adt_test_layout();
		auto values = std::make_shared<std::vector<std::string>>();
		adt_tuple_writer<node_t> writer(
			std::make_unique<vector_output_stream>(values), layout);
		CHECK(writer.collect(0, { dict("a") }, "0"));       // still buffering
		CHECK(writer.collect(0, { dict("p"), dict("x") }, "1")); // completes the record
		REQUIRE(values->size() == 1);
		CHECK(values->at(0) == "{ a: \"0\", p: { x: \"1\" } }");
	}

	TEST_CASE("writer keeps separate time points separate") {
		auto layout = adt_test_layout();
		auto values = std::make_shared<std::vector<std::string>>();
		adt_tuple_writer<node_t> writer(
			std::make_unique<vector_output_stream>(values), layout);
		writer.collect(0, { dict("a") }, "0");
		writer.collect(1, { dict("a") }, "10");
		CHECK(values->empty()); // neither time point complete yet
		writer.collect(0, { dict("p"), dict("x") }, "1");
		writer.collect(1, { dict("p"), dict("x") }, "11");
		REQUIRE(values->size() == 2);
		CHECK(values->at(0) == "{ a: \"0\", p: { x: \"1\" } }");
		CHECK(values->at(1) == "{ a: \"10\", p: { x: \"11\" } }");
	}

	TEST_CASE("writer rejects a repeated write to the same path at the same time point") {
		// Semantics implemented: a repeat is treated as record corruption --
		// the ENTIRE pending record for that time point is discarded (not
		// just the repeated path), so the record can never complete
		// afterward, even once the genuinely-missing member arrives (it
		// starts a fresh record missing the contributions lost when the
		// duplicate discarded the old one). See adt_tuple_writer::collect's
		// comment (io_context.tmpl.h) for the full rationale.
		auto layout = adt_test_layout();
		auto values = std::make_shared<std::vector<std::string>>();
		adt_tuple_writer<node_t> writer(
			std::make_unique<vector_output_stream>(values), layout);
		CHECK(writer.collect(0, { dict("a") }, "0"));       // first write: buffers
		CHECK_FALSE(writer.collect(0, { dict("a") }, "1")); // repeat: hard error
		// The genuinely-missing member arriving afterward starts a FRESH
		// record (still buffering, since it's the only entry so far) that
		// can never complete: "a" was lost when the repeat discarded the
		// old record, and nothing collects it again in this test.
		CHECK(writer.collect(0, { dict("p"), dict("x") }, "1"));
		CHECK(values->empty());
	}
}

TEST_SUITE("console_prompt_output_stream") {

	TEST_CASE("put prints the labelled value") {
		cout_capture out;
		console_prompt_output_stream s("outvar");
		CHECK(s.put("val", 3));
		const std::string printed = out.str();
		CHECK(printed.find("outvar[3]") != std::string::npos);
		CHECK(printed.find(":=") != std::string::npos);
		CHECK(printed.find("val") != std::string::npos);
	}

	TEST_CASE("the inherited no-label put prints only the value") {
		cout_capture out;
		console_prompt_output_stream s("v");
		CHECK(s.put("bare"));
		CHECK(out.str() == "bare\n");
	}

	TEST_CASE("rebuild preserves the label name") {
		console_prompt_output_stream s("label");
		auto rebuilt = s.rebuild();
		REQUIRE(rebuilt != nullptr);
		cout_capture out;
		CHECK(rebuilt->put("v", 2));
		CHECK(out.str().find("label[2]") != std::string::npos);
	}
}
