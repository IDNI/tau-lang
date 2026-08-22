// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

// IN-RT4: execution of top-level LTL specifications through the PUBLIC
// api::get_interpreter / api::step path -- the one every REPL `run` and
// every embedding takes.  Every other execution test on this branch calls
// make_interpreter/run directly, which is why the IN-R1/IN-R2 layering
// split (normalize() rewriting temporal operators before the LTL pipeline
// saw them) was invisible to the suite.  One case per operator family;
// the assertions are the parts of the behaviour every correct strategy
// must share.

#include "test_init.h"
#include "test_tau_helpers.h"

#include <functional>
#include <map>
#include <string>
#include <vector>

using tau_api = api<node_t>;

namespace {

// Drive `spec` for `steps` steps through the api, feeding `input_at(step)`
// to every requested input; returns the output values in step order
// (one entry per step, the first output stream's value, "" if none).
std::vector<std::string> drive(const char* spec, size_t steps,
	const std::function<std::string(size_t)>& input_at)
{
	std::vector<std::string> values;
	auto maybe_i = tau_api::get_interpreter(spec);
	REQUIRE_MESSAGE(maybe_i.has_value(), spec);
	auto& i = maybe_i.value();
	for (size_t step = 0; step < steps; ++step) {
		std::map<stream_at, std::string> assigned;
		for (auto& input_at_ : tau_api::get_inputs_for_step(i))
			assigned[input_at_] = input_at(step);
		auto outputs = tau_api::step(i, assigned, /*interactive=*/false);
		REQUIRE_MESSAGE(outputs.has_value(), spec << " step " << step);
		std::string v;
		for (auto& [at, val] : *outputs) { v = val; break; }
		values.push_back(v);
	}
	return values;
}

} // namespace

TEST_SUITE("Tau API - LTL execution through get_interpreter") {

	TEST_CASE("[IN-RT4-G] always: every step satisfies the invariant") {
		auto v = drive("always o1[t] = 1.", 3, [](size_t) { return "T."; });
		REQUIRE(v.size() == 3);
		for (auto& x : v) CHECK(x == "T");
	}

	TEST_CASE("[IN-RT4-G-echo] always with an input: o1 mirrors i1") {
		auto v = drive("always o1[t] = i1[t].", 4,
			[](size_t s) { return s % 2 == 0 ? "T." : "F."; });
		REQUIRE(v.size() == 4);
		CHECK(v == std::vector<std::string>{"T", "F", "T", "F"});
	}

	TEST_CASE("[IN-RT4-F] F: the obligation is discharged within the run") {
		auto v = drive("F (o1[t] = 1).", 3, [](size_t) { return "T."; });
		REQUIRE(v.size() == 3);
		bool seen = false;
		for (auto& x : v) if (x == "T") seen = true;
		CHECK(seen);
	}

	TEST_CASE("[IN-RT4-U] U: zeros until the release, never a stray value") {
		auto v = drive("(o1[t] = 0) U (o1[t] = 1).", 3, [](size_t) { return "T."; });
		REQUIRE(v.size() == 3);
		size_t first_one = v.size();
		for (size_t k = 0; k < v.size(); ++k)
			if (v[k] == "T") { first_one = k; break; }
		CHECK(first_one < v.size());            // the release happens
		for (size_t k = 0; k < first_one; ++k)  // and nothing else before it
			CHECK(v[k] == "F");
	}

	TEST_CASE("[IN-RT4-W] W: the left side holds while the input release never comes") {
		auto v = drive("(o1[t] = 1) W (i1[t] = 1).", 3, [](size_t) { return "F."; });
		REQUIRE(v.size() == 3);
		for (auto& x : v) CHECK(x == "T");
	}

	TEST_CASE("[IN-RT4-R] R: the released side holds up to and including the release") {
		// (o1=1) R (o1=1): o1 must be 1 at every step.
		auto v = drive("(o1[t] = 1) R (o1[t] = 1).", 3, [](size_t) { return "T."; });
		REQUIRE(v.size() == 3);
		for (auto& x : v) CHECK(x == "T");
	}

	TEST_CASE("[IN-RT4-S] past: always(o2=1) beside a Since obligation executes") {
		auto v = drive("always (o1[t] = 1) && ((o1[t] = 1) S (o1[t] = 1)).", 3,
			[](size_t) { return "T."; });
		REQUIRE(v.size() == 3);
		for (auto& x : v) CHECK(x == "T");
	}

	TEST_CASE("[IN-RT4-CTL] ctl_star A at the root executes as its body") {
		auto v = drive("A (always o1[t] = 1).", 3, [](size_t) { return "T."; });
		REQUIRE(v.size() == 3);
		for (auto& x : v) CHECK(x == "T");
	}
}
