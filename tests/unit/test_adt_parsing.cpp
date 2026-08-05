// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_Bool_helpers.h"

// These tests exercise only the ADT *grammar* (type_def, member_path, ...):
// they parse with BA type inference OFF so no ADT semantics (flattening,
// registry, type checking) are required yet. See task 1 of the ADT plan.

// helper: parse a spec source into a tau tree without inference
static tref parse_no_infer(const std::string& src) {
	return tau::get(src, { .infer_ba_types = false });
}
// NOTE for this task only: get_options.flatten_adts does not exist yet —
// omit it here and add it in Task 5; use { .infer_ba_types = false }.

TEST_SUITE("adt grammar") {
	TEST_CASE("type_def alias")   { CHECK(parse_no_infer("type byte = bv[8]. x = 0.") != nullptr); }
	TEST_CASE("type_def tuple")   { CHECK(parse_no_infer("type Point = {a: sbf, b: sbf}. x = 0.") != nullptr); }
	TEST_CASE("type_def is/of")   { CHECK(parse_no_infer("type Tagged = {tag: bv[8]}. type Line of (Tagged) is {p: Point, q: Point}. type Point = {a: sbf, b: sbf}. x = 0.") != nullptr); }
	TEST_CASE("member_path var")  { CHECK(parse_no_infer("x.a = 0.") != nullptr); }
	TEST_CASE("nested path")      { CHECK(parse_no_infer("l.p.a = l.q.a.") != nullptr); }
	TEST_CASE("io var with path") { CHECK(parse_no_infer("p.a := in console. p[t].a = 0.") != nullptr); }
	TEST_CASE("dot then space is statement end, not member") {
		// rec_relation body ends at "x." — the following "y" starts main
		CHECK(parse_no_infer("f(v) := x. y = 0.") != nullptr);
	}
	TEST_CASE("member access inside rec relation body") {
		CHECK(parse_no_infer("f(v) := x.a = 0. f(z).") != nullptr);
	}
}
