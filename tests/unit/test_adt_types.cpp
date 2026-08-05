// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_Bool_helpers.h"

// helper: parse a spec source into a tau tree without inference
static tref parse_no_infer(const std::string& src) {
	return tau::get(src, { .infer_ba_types = false });
}

static std::optional<adt_registry<node_t>> reg(const std::string& src) {
	tref spec = parse_no_infer(src);
	REQUIRE(spec != nullptr);
	return adt_registry<node_t>::build(spec);
}
static size_t sid(const std::string& s) { return dict(s); } // same dict the parser uses

TEST_SUITE("adt registry") {
	TEST_CASE("alias to subtyped base") {
		auto r = reg("type byte = bv[8]. x = 0.");
		REQUIRE(r); CHECK(r->is_alias(sid("byte")));
		CHECK(tau::get(r->alias_target(sid("byte"))).to_str() == tau::get(bv_type<node_t>(8)).to_str());
	}
	TEST_CASE("tuple flat order with inheritance") {
		auto r = reg("type Tagged = {tag: bv[8]}. type Point = {a: sbf, b: sbf}."
			" type Line of (Tagged) is {p: Point, q: Point}. x = 0.");
		REQUIRE(r);
		const auto& ms = r->members(sid("Line"));
		REQUIRE(ms.size() == 5);
		CHECK(ms[0].path == std::vector<size_t>{sid("tag")});      // parents first
		CHECK(ms[1].path == (std::vector<size_t>{sid("p"), sid("a")}));
		CHECK(ms[4].path == (std::vector<size_t>{sid("q"), sid("b")}));
	}
	TEST_CASE("alias of tuple is a tuple") {
		auto r = reg("type Point = {a: sbf, b: sbf}. type P2 = Point. x = 0.");
		REQUIRE(r); CHECK(r->is_tuple(sid("P2")));
	}
	TEST_CASE("duplicate member via inheritance fails") {
		CHECK_FALSE(reg("type A = {m: sbf}. type B of (A) is {m: sbf}. x = 0."));
	}
	TEST_CASE("cycle fails")             { CHECK_FALSE(reg("type A = {m: B}. type B = {m: A}. x = 0.")); }
	TEST_CASE("self-cycle fails")        { CHECK_FALSE(reg("type A = {m: A}. x = 0.")); }
	TEST_CASE("duplicate type fails")    { CHECK_FALSE(reg("type A = {m: sbf}. type A = {n: sbf}. x = 0.")); }
	TEST_CASE("inherit from alias-of-base fails") { CHECK_FALSE(reg("type byte = bv[8]. type A of (byte) is {m: sbf}. x = 0.")); }
	TEST_CASE("unknown member type is base, not error") {
		auto r = reg("type A = {m: sbf}. x = 0.");   // sbf has no type_def -> base
		REQUIRE(r); CHECK(r->members(sid("A"))[0].base_type != nullptr);
	}
	TEST_CASE("empty registry") { auto r = reg("x = 0."); REQUIRE(r); CHECK(r->empty()); }
}
