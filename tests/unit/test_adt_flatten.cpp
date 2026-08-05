// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_Bool_helpers.h"

// helper: parse a spec source into a tau tree without inference, and WITHOUT
// the automatic flatten-on-parse hook (get_options.flatten_adts, added by
// task 5) so that the explicit adt_flatten<node_t> call below is the thing
// actually under test here, not a no-op re-run of an already-flattened tree
// (adt_flatten's empty-registry fast path would otherwise make the second
// call vacuous once type_defs are already erased by the hook).
static tref flat(const std::string& src) {
	tref t = tau::get(src, { .infer_ba_types = false, .flatten_adts = false });
	if (!t) return nullptr;
	return adt_flatten<node_t>(t);
}
static void check_flat(const std::string& src, const std::string& expected) {
	tref a = flat(src), b = flat(expected);
	REQUIRE(a != nullptr); REQUIRE(b != nullptr);
	CHECK(tau::get(a).to_str() == tau::get(b).to_str());
}
#define PT "type Point = {a: sbf, b: sbf}. "

TEST_SUITE("adt flatten") {
	// NOTE: the brief's first "member path" case (`PT "x:Point x.a = 0."`) is
	// self-acknowledged invalid syntax (`x:Point` and `x.a` on one bare
	// variable occurrence isn't how the grammar attaches an annotation to a
	// later occurrence) and is superseded by "annotation propagates in
	// scope" below, which exercises the same rewrite rule through valid
	// syntax (the annotation carried by the quantifier binder).
	TEST_CASE("annotation propagates in scope") {
		check_flat(PT "ex x:Point x.a = 0.", "ex x.a:sbf, x.b:sbf x.a = 0.");
	}
	TEST_CASE("tuple equality") {
		check_flat(PT "ex x:Point ex y:Point x = y.",
			"ex x.a:sbf, x.b:sbf ex y.a:sbf, y.b:sbf (x.a = y.a && x.b = y.b).");
	}
	TEST_CASE("tuple inequality with constant") {
		check_flat(PT "ex x:Point x != 0.",
			"ex x.a:sbf, x.b:sbf (x.a != 0 || x.b != 0).");
	}
	TEST_CASE("partial path equality") {
		check_flat("type Point = {a: sbf, b: sbf}. type Line = {p: Point, q: Point}. "
			"ex l:Line l.p = l.q.",
			"ex l.p.a:sbf, l.p.b:sbf, l.q.a:sbf, l.q.b:sbf "
			"(l.p.a = l.q.a && l.p.b = l.q.b).");
	}
	TEST_CASE("alias annotation")  { check_flat("type byte = bv[8]. ex x:byte x = 0.", "ex x:bv[8] x = 0."); }
	TEST_CASE("type defs erased")  { check_flat(PT "x = 0.", "x = 0."); }
	// errors -> nullptr
	TEST_CASE("untyped member access")       { CHECK(flat(PT "x.a = 0.") == nullptr); }
	// (x has no ADT annotation anywhere in scope: 'a' could be any type's member)
	TEST_CASE("tuple var in bf op")          { CHECK(flat(PT "ex x:Point ex y:Point (x | y) = 0.") == nullptr); }
	TEST_CASE("tuple var in inequality op")  { CHECK(flat(PT "ex x:Point ex y:Point x < y.") == nullptr); }
	TEST_CASE("unknown member")              { CHECK(flat(PT "ex x:Point x.c = 0.") == nullptr); }
	TEST_CASE("shape mismatch")              { CHECK(flat(PT "type Q = {a: sbf}. ex x:Point ex y:Q x = y.") == nullptr); }
	TEST_CASE("conflicting annotations")     { CHECK(flat(PT "type Q = {a: sbf}. ex x:Point x.a = 0 && x:Q x.a = 1.") == nullptr); }
	TEST_CASE("idempotent") {
		tref once = flat(PT "ex x:Point x = 0.");
		REQUIRE(once);
		tref twice = adt_flatten<node_t>(once);
		CHECK(tau::get(once).to_str() == tau::get(twice).to_str());
	}
	TEST_CASE("rec_relation formals get their own scope, not the enclosing one") {
		// Two unrelated definitions each locally annotate a formal parameter
		// named "x" with a different registry type. Per design section 3,
		// pass 1, a ref formal's annotation is a valid ADT-typing source for
		// member paths used on it in that definition's own body -- but it
		// must not leak into the enclosing (here: global) scope, or these
		// two definitions' same-named-but-unrelated formals would
		// spuriously "conflict". This only asserts flattening succeeds (no
		// spurious conflict); it does not assert anything about the heads'
		// resulting shape -- see the "adt flatten refs" suite below (Task 6)
		// for the actual ref-arg expansion (`f(x:Point)` -> `f(x.a,x.b)`)
		// this scoping enables.
		CHECK(flat("type Point = {a: sbf, b: sbf}. type Q = {c: sbf}. "
			"f(x:Point) := x.a = 0. g(x:Q) := x.c = 0. y = 0.") != nullptr);
	}
	TEST_CASE("type defs erased under spec_multiline") {
		// spec_multiline (used e.g. for REPL scripting) inlines spec_part
		// directly, so type_def sits as a direct child alongside
		// rec_relation/input_def/output_def/main -- same erasure rule as
		// under `definitions`, exercised here via an explicit parse start
		// symbol since check_flat's normal `spec` start never produces a
		// spec_multiline node to erase from.
		tau::get_options opts{ .parse = { .start = tau::spec_multiline },
			.infer_ba_types = false, .flatten_adts = false };
		tref t = tau::get(std::string(PT "x = 0."), opts);
		REQUIRE(t != nullptr);
		tref flattened = adt_flatten<node_t>(t);
		REQUIRE(flattened != nullptr);
		CHECK(tau::get(flattened).select_all(is<node_t, tau::type_def>).empty());
	}
}

TEST_SUITE("adt flatten refs") {
	TEST_CASE("formal and call site expand") {
		check_flat(PT "f(x:Point, v) := x.a = v && x.b = 0. ex y:Point f(y, 1).",
			"f(x.a:sbf, x.b:sbf, v) := x.a = v && x.b = 0. "
			"ex y.a:sbf, y.b:sbf f(y.a, y.b, 1).");
	}
	TEST_CASE("call with non-variable tuple arg fails") {
		// NOTE: the brief's own `f(y | y)` is not usable here -- `y | y`
		// (identical operand on both sides of `|`) folds to bare `y` during
		// PARSING itself (Boolean idempotence, before adt_flatten ever
		// runs), so the ref_arg the flattener would actually see is just a
		// plain variable, not an error case. `y'` (negation) has no such
		// self-folding and exercises the same "any other bf ... = error"
		// path (rule 5, via adt_flatten_rewrite_variable's k_partial
		// branch, reached through the ordinary generic recursive rewrite
		// that adt_flatten_rewrite_ref_args falls back to for a non-variable
		// ref_arg).
		CHECK(flat(PT "f(x:Point) := x.a = 0. ex y:Point f(y').") == nullptr);
	}
	TEST_CASE("tuple-typed ref result fails") {
		CHECK(flat(PT "f(v):Point := v = 0. f(0) = 0.") == nullptr);
	}
	TEST_CASE("tuple-typed capture fails") {
		// $X:Point is rejected at the grammar level already (capture has no
		// `typed` production, see Task 4/parser/tau.tgf's `capture` rule) --
		// flat() fails here because tau::get() itself returns nullptr, not
		// because of anything adt_flatten does.
		CHECK(flat(PT "ex x:Point $X:Point = x.") == nullptr);
	}
	TEST_CASE("fp_fallback with tuple-typed content fails") {
		CHECK(flat(PT "g(v) := v = 0. ex x:Point (g(1) fallback x) = 0.") == nullptr);
	}
	TEST_CASE("alias-typed formal rewrites to alias target") {
		check_flat("type byte = bv[8]. f(x:byte) := x = 0. f(1) = 0.",
			"f(x:bv[8]) := x = 0. f(1) = 0.");
	}
	TEST_CASE("alias-typed ref result rewrites to alias target") {
		check_flat("type byte = bv[8]. f(v):byte := v = 0. f(0) = 0.",
			"f(v):bv[8] := v = 0. f(0) = 0.");
	}
	TEST_CASE("call site full member path reaching nested tuple expands") {
		check_flat(
			"type Point = {a: sbf, b: sbf}. type Line = {p: Point, q: Point}. "
			"h(w:Point) := w.a = 0. ex l:Line h(l.p).",
			"h(w.a:sbf, w.b:sbf) := w.a = 0. "
			"ex l.p.a:sbf, l.p.b:sbf, l.q.a:sbf, l.q.b:sbf h(l.p.a, l.p.b).");
	}
}

TEST_SUITE("adt flatten hook") {
	TEST_CASE("default get flattens and infers") {
		// no manual adt_flatten call -- default options
		tref t = tau::get("type Point = {a: sbf, b: sbf}. ex x:Point x = 0.");
		REQUIRE(t != nullptr);
		tref e = tau::get("ex x.a:sbf, x.b:sbf (x.a = 0 && x.b = 0).");
		REQUIRE(e != nullptr);
		CHECK(tau::get(t).to_str() == tau::get(e).to_str());
	}
	TEST_CASE("round trip: print, reparse, same tree") {
		tref t = tau::get("type Point = {a: sbf, b: sbf}. ex x:Point x != 1.");
		REQUIRE(t != nullptr);
		tref r = tau::get(tau::get(t).to_str());
		REQUIRE(r != nullptr);
		CHECK(tau::get(t).to_str() == tau::get(r).to_str());
	}
	TEST_CASE("adt error fails the whole get") {
		CHECK(tau::get("type Point = {a: sbf, b: sbf}. ex x:Point x.c = 0.") == nullptr);
	}
}
