// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "ga1/worker.h"

using namespace idni::tau_lang;

namespace {

const std::string rule =
	"always ( i1[t]:bv[24] > {5000}:bv[24]\n"
	"  ? o5[t]:bv[1] = {1}:bv[1] : o5[t]:bv[1] = {0}:bv[1] ).";

ga1::json call(ga1::worker& w, ga1::json c) {
	c.set("schema", ga1::json::str(ga1::SCHEMA));
	if (!c.get("cap")) c.set("cap", ga1::json::integer(100000));
	ga1::json j = ga1::json::object();
	j.set("call", std::move(c));
	return w.request(j);
}

std::string field(const ga1::json& j, const char* key) {
	std::string s;
	if (const auto* v = j.get(key)) v->ascii(s);
	return s;
}

bool completed(const ga1::json& j) {
	const auto* outcome = j.get("outcome");
	return outcome && field(*outcome, "status") == "Completed";
}

ga1::json control(const char* name, const std::string& digest) {
	ga1::json j = ga1::json::object();
	j.set(name, ga1::json::str(digest));
	return j;
}

} // namespace

TEST_CASE("GA1 saved plan steps in a fresh worker and can be dropped") {
	using ga1::json;
	for (const char* fmt : {"tba1", "varint", "packed"}) {
		CAPTURE(fmt);
		ga1::worker original, fresh;
		json build = json::object();
		build.set("op", json::str("get_interpreter"));
		build.set("text", json::str(rule));
		build.set("strategy", json::str("dag"));
		build.set("format", json::str(fmt));
		const auto b = call(original, build);
		REQUIRE(completed(b));
		auto plan = *b.get("plan");
		auto digest = field(plan, "digest");
		auto hex = field(plan, "saved");
		CHECK(original.held_count() == 1);
		CHECK(fresh.held_count() == 0);

		json step = json::object();
		step.set("op", json::str("step"));
		step.set("state", *b.get("post"));
		step.set("eval", json::str("lazy"));
		json inputs = json::object();
		inputs.set("i1", json::str("001389"));
		step.set("inputs", std::move(inputs));
		const auto unavailable = call(fresh, step);
		CHECK(field(*unavailable.get("outcome"), "kind") == "plan_not_held");
		CHECK(ga1_wire::dump(*unavailable.get("post")) == ga1_wire::dump(*step.get("state")));

		json load = json::object();
		load.set("op", json::str("load"));
		load.set("format", json::str(fmt));
		load.set("bind", json::str(digest));
		if (std::string_view(fmt) == "tba1") {
			std::string text;
			auto nibble = [](char c) { return c <= '9' ? c - '0' : c - 'a' + 10; };
			for (std::size_t i = 0; i + 1 < hex.size(); i += 2)
				text.push_back((char)(16 * nibble(hex[i]) + nibble(hex[i + 1])));
			load.set("artifact", json::str(text));
		} else load.set("bytes", json::str(hex));

		json bad = load;
		bad.set("bind", json::str(std::string(64, '0')));
		CHECK_FALSE(completed(call(fresh, bad)));
		CHECK(fresh.held_count() == 0);
		if (std::string_view(fmt) != "tba1") {
			for (const auto& bytes : {hex + "0", "G" + hex.substr(1),
			                          "A" + hex.substr(1)}) {
				auto invalid = load;
				invalid.set("bytes", json::str(bytes));
				auto r = call(fresh, std::move(invalid));
				REQUIRE(r.get("outcome"));
				CHECK(field(*r.get("outcome"), "stage") == "load.admit");
				CHECK(fresh.held_count() == 0);
			}
			auto nonascii = load;
			nonascii.set("bytes", *ga1_wire::parse_json(std::string_view(R"("\u01000")")));
			auto invalid_unicode = call(fresh, std::move(nonascii));
			CHECK(field(*invalid_unicode.get("outcome"), "stage") == "load.admit");
			CHECK(fresh.held_count() == 0);
			auto small_cap = load;
			small_cap.set("cap", json::integer(0));
			auto r = call(fresh, std::move(small_cap));
			CHECK(field(*r.get("outcome"), "status") == "UnknownBudget");
			CHECK(fresh.held_count() == 0);
		}
		const auto l = call(fresh, load);
		REQUIRE(completed(l));
		REQUIRE(l.get("plan"));
		CHECK(ga1_wire::dump(*l.get("plan")) == ga1_wire::dump(plan));

		const auto next = call(fresh, step);
		REQUIRE(completed(next));
		CHECK(ga1_wire::dump(next) == ga1_wire::dump(call(original, step)));
		CHECK(ga1_wire::dump(*next.get("post")->get("time")) == "1");
		CHECK(ga1_wire::dump(*next.get("outcome")->get("value")->get("outputs"))
			== R"([["o5",0,"1"]])");
		CHECK(ga1_wire::dump(*fresh.request(control("save", digest)).get("plan"))
			== ga1_wire::dump(plan));
		CHECK(field(fresh.request(control("drop", digest)), "status") == "Completed");
		CHECK(fresh.held_count() == 0);
		CHECK(field(*call(fresh, step).get("outcome"), "kind") == "plan_not_held");
		CHECK(completed(call(fresh, load)));
		CHECK(ga1_wire::dump(call(fresh, step)) == ga1_wire::dump(next));
	}
}

TEST_CASE("GA1 worker releases explicitly dropped plans without touching another worker") {
	using ga1::json;
	ga1::worker a, b;
	for (int i = 0; i < 32; ++i) {
		json build = json::object();
		build.set("op", json::str("get_interpreter"));
		build.set("text", json::str("always ( o5[t]:bv[24] = {" + std::to_string(i) + "}:bv[24] )."));
		build.set("format", json::str("varint"));
		const auto r = call(a, std::move(build));
		REQUIRE(completed(r));
		CHECK(a.held_count() == 1);
		CHECK(b.held_count() == 0);
		const auto plan = r.get("plan");
		REQUIRE(plan);
		a.request(control("drop", field(*plan, "digest")));
		CHECK(a.held_count() == 0);
	}
}
