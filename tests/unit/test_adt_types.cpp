// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_Bool_helpers.h"

// helper: parse a spec source into a tau tree without inference or
// flattening: adt_registry::build (below) needs the raw type_defs still
// present in the tree -- task 5's default get_options.flatten_adts = true
// would otherwise erase them before the registry ever sees them.
static tref parse_no_infer(const std::string& src) {
	return tau::get(src, { .infer_ba_types = false, .flatten_adts = false });
}

static std::optional<adt_registry<node_t>> reg(const std::string& src) {
	tref spec = parse_no_infer(src);
	REQUIRE(spec != nullptr);
	return adt_registry<node_t>::build(spec);
}
static size_t sid(const std::string& s) { return dict(s); } // same dict the parser uses

// helper: collect every type_def under a parsed spec into an htref list,
// mimicking repl_evaluator::type_defs (one htref per accepted `type` line).
static std::vector<htref> session_type_defs_from(const std::string& src) {
	tref spec = parse_no_infer(src);
	REQUIRE(spec != nullptr);
	std::vector<htref> v;
	for (tref td : tau::get(spec).select_all(is<node_t, tau::type_def>))
		v.push_back(tau::geth(td));
	return v;
}

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
	TEST_CASE("is_tuple/is_alias are safe probes on an unregistered name") {
		// sbf has no type_def in this spec, and neither does an arbitrary
		// unused name: is_tuple/is_alias must return false, not throw --
		// this is the natural guard pattern for the common (base-typed) case.
		auto r = reg("type A = {m: sbf}. x = 0.");
		REQUIRE(r);
		CHECK_FALSE(r->defines(sid("sbf")));
		CHECK_FALSE(r->is_tuple(sid("sbf")));
		CHECK_FALSE(r->is_alias(sid("sbf")));
		CHECK_FALSE(r->is_tuple(sid("not_a_registered_type")));
		CHECK_FALSE(r->is_alias(sid("not_a_registered_type")));
	}
	TEST_CASE("inherit from an undeclared type fails") {
		CHECK_FALSE(reg("type A of (Nope) is {m: sbf}. x = 0."));
	}
	TEST_CASE("two parents: members in declaration order") {
		auto r = reg("type P1 = {a: sbf}. type P2 = {b: sbf}. "
			"type C of (P1, P2) is {c: sbf}. x = 0.");
		REQUIRE(r);
		const auto& ms = r->members(sid("C"));
		REQUIRE(ms.size() == 3);
		CHECK(ms[0].path == std::vector<size_t>{sid("a")});
		CHECK(ms[1].path == std::vector<size_t>{sid("b")});
		CHECK(ms[2].path == std::vector<size_t>{sid("c")});
	}
	TEST_CASE("duplicate member across two parents fails") {
		CHECK_FALSE(reg("type P1 = {m: sbf}. type P2 = {m: sbf}. "
			"type C of (P1, P2) is {c: sbf}. x = 0."));
	}
	TEST_CASE("alias of a registered alias chains to the base") {
		auto r = reg("type byte = bv[8]. type b2 = byte. x = 0.");
		REQUIRE(r); CHECK(r->is_alias(sid("b2")));
		CHECK(tau::get(r->alias_target(sid("b2"))).to_str()
			== tau::get(bv_type<node_t>(8)).to_str());
	}
	TEST_CASE("alias cycle fails")  { CHECK_FALSE(reg("type A = B. type B = A. x = 0.")); }
	TEST_CASE("parent cycle fails") {
		CHECK_FALSE(reg("type A of (B) is {a: sbf}. type B of (A) is {b: sbf}. x = 0."));
	}
	TEST_CASE("duplicate member declared directly fails") {
		CHECK_FALSE(reg("type A = {m: sbf, m: sbf}. x = 0."));
	}
	TEST_CASE("member typed by a registered alias resolves to the base") {
		auto r = reg("type byte = bv[8]. type A = {m: byte}. x = 0.");
		REQUIRE(r);
		const auto& ms = r->members(sid("A"));
		REQUIRE(ms.size() == 1);
		CHECK(ms[0].path == std::vector<size_t>{sid("m")});
		CHECK(tau::get(ms[0].base_type).to_str()
			== tau::get(bv_type<node_t>(8)).to_str());
	}
	TEST_CASE("forward reference resolves order-independently") {
		auto r = reg("type Line = {p: Point, q: Point}. "
			"type Point = {a: sbf, b: sbf}. x = 0.");
		REQUIRE(r);
		const auto& ms = r->members(sid("Line"));
		REQUIRE(ms.size() == 4);
		CHECK(ms[0].path == (std::vector<size_t>{sid("p"), sid("a")}));
		CHECK(ms[3].path == (std::vector<size_t>{sid("q"), sid("b")}));
	}

	// Task 3: session type_defs (REPL-accepted `type` lines from earlier,
	// separately parsed lines) pre-register into the registry before the
	// current spec's own type_defs, with the current spec always winning on
	// a same-name clash -- see adt_registry::build's header comment.
	TEST_CASE("spec's own type_def overrides a same-name session entry") {
		auto session = session_type_defs_from(
			"type T = {a: sbf, b: sbf}. x = 0.");
		tref spec = parse_no_infer("type T = {a: sbf}. x = 0.");
		REQUIRE(spec != nullptr);
		auto r = adt_registry<node_t>::build(spec, &session);
		REQUIRE(r);
		REQUIRE(r->is_tuple(sid("T")));
		// the SPEC's one-member layout wins, not the session's two-member one
		REQUIRE(r->members(sid("T")).size() == 1);
		CHECK(r->members(sid("T"))[0].path == std::vector<size_t>{sid("a")});
	}
	TEST_CASE("a session-only type_def is visible when the spec does not redeclare it") {
		auto session = session_type_defs_from(
			"type T = {a: sbf, b: sbf}. x = 0.");
		tref spec = parse_no_infer("x = 0."); // no `type T` here
		REQUIRE(spec != nullptr);
		auto r = adt_registry<node_t>::build(spec, &session);
		REQUIRE(r);
		REQUIRE(r->is_tuple(sid("T")));
		const auto& ms = r->members(sid("T"));
		REQUIRE(ms.size() == 2);
		CHECK(ms[0].path == std::vector<size_t>{sid("a")});
		CHECK(ms[1].path == std::vector<size_t>{sid("b")});
	}
	TEST_CASE("a later session entry overrides an earlier same-name session entry") {
		std::vector<htref> session;
		for (const auto& part : { "type T = {a: sbf}. x = 0.",
			"type T = {a: sbf, b: sbf}. x = 0." })
		{
			auto part_defs = session_type_defs_from(part);
			session.insert(session.end(), part_defs.begin(), part_defs.end());
		}
		tref spec = parse_no_infer("x = 0.");
		REQUIRE(spec != nullptr);
		auto r = adt_registry<node_t>::build(spec, &session);
		REQUIRE(r);
		CHECK(r->members(sid("T")).size() == 2); // the later session def wins
	}
	TEST_CASE("a duplicate type name WITHIN the spec itself still fails, session or not") {
		auto session = session_type_defs_from(
			"type T = {a: sbf}. x = 0.");
		tref spec = parse_no_infer(
			"type T = {a: sbf}. type T = {b: sbf}. x = 0.");
		REQUIRE(spec != nullptr);
		CHECK_FALSE(adt_registry<node_t>::build(spec, &session));
	}
	TEST_CASE("the 1-arg build() form still works (session_type_defs defaults to null)") {
		auto r = reg("type T = {a: sbf}. x = 0."); // reg() calls the 1-arg build()
		REQUIRE(r);
		CHECK(r->members(sid("T")).size() == 1);
	}
}
