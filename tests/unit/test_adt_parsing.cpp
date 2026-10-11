// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_Bool_helpers.h"

// These tests exercise only the ADT *grammar* (type_def, member_path, ...):
// they parse with BA type inference OFF so no ADT semantics (flattening,
// registry, type checking) are required yet. See task 1 of the ADT plan.

// helper: parse a spec source into a tau tree without inference
static tref parse_no_infer(const std::string& src) {
	return tau::get(src, { .infer_ba_types = false, .flatten_adts = false })
		.value_or(nullptr);
}

TEST_SUITE("adt grammar") {
	TEST_CASE("type_def alias")   { CHECK(parse_no_infer("type byte = bv[8]. x = 0.") != nullptr); }
	TEST_CASE("type_def tuple")   { CHECK(parse_no_infer("type Point = {a: bool, b: bool}. x = 0.") != nullptr); }
	// Point declared before Line: this fixture's pack (bv, Bool) validates
	// type_name against the pack, so a forward reference to an as-yet-
	// undeclared type name no longer parses (see test_adt_types.cpp's
	// "forward reference resolves order-independently" for that property).
	// This case is about the "of (...) is {...}" grammar form, not order.
	TEST_CASE("type_def is/of")   { CHECK(parse_no_infer("type Tagged = {tag: bv[8]}. type Point = {a: bool, b: bool}. type Line of (Tagged) is {p: Point, q: Point}. x = 0.") != nullptr); }
	// The declared name is live inside its own PARENTS group, not just after
	// the declaration completes: distinct from "type_def is/of" above, which
	// parents a PREVIOUSLY declared type.
	TEST_CASE("type_def self-reference in own parents group") { CHECK(parse_no_infer("type Y of (Y) is {m: bool}. x = 0.") != nullptr); }
	TEST_CASE("member_path var")  { CHECK(parse_no_infer("x.a = 0.") != nullptr); }
	TEST_CASE("nested path")      { CHECK(parse_no_infer("l.p.a = l.q.a.") != nullptr); }
	TEST_CASE("io var occurrence with member path") { CHECK(parse_no_infer("p[t].a = 0.") != nullptr); }
	// I4-alt (final review): input_def/output_def's own `[member_path]`
	// production STAYS in parser/tau.tgf as committed -- removing it
	// re-drifted unrelated nonterminal-id-derived test orderings and
	// deterministically tripped an unrelated, pre-existing latent DBG
	// assertion in anti_prenex/hooks.tmpl.h, so the final reviewer
	// sanctioned rejecting this in the FLATTENER instead of the grammar.
	// A member_path on the def itself still PARSES (grammar unchanged)...
	TEST_CASE("io def with member path still parses at the grammar level") {
		CHECK(parse_no_infer("p.a := in console. p[t].a = 0.") != nullptr);
	}
	// ...but is always meaningless (nothing ever consumed it: it only emits
	// member io defs into ctx, never as printed defs; a live occurrence
	// prints/parses via VARIABLE/io_var occurrences instead, see the case
	// above), and process_io_def used to silently mis-register it under the
	// bare root name -- adt_flatten_check_io_def_head
	// (src/adt/adt_flatten.tmpl.h) now rejects it. Default options here
	// (unlike parse_no_infer's `.flatten_adts = false` above): flattening
	// is what actually performs the rejection, not parsing.
	TEST_CASE("io def with member path is rejected by the flattener") {
		CHECK(tau::get(std::string("p.a := in console. p[t].a = 0.")).value_or(nullptr) == nullptr);
	}
	TEST_CASE("dot then space is statement end, not member") {
		// rec_relation body ends at "x." — the following "y" starts main
		CHECK(parse_no_infer("f(v) := x. y = 0.") != nullptr);
	}
	TEST_CASE("member access inside rec relation body") {
		CHECK(parse_no_infer("f(v) := x.a = 0. f(z).") != nullptr);
	}
	// Minor #9 (final review): `typed` only ever accepts a `type` (bare
	// chars, i.e. a NAMED type) per parser/tau.tgf -- an anonymous/inline
	// tuple annotation on a variable is rejected at the grammar level:
	// variables are annotated with named types only.
	TEST_CASE("anonymous tuple annotation on a variable fails to parse") {
		CHECK(parse_no_infer("x:{a: sbf} = 0.") == nullptr);
	}
	// `file_name` is `printable+` (parser/tau.tgf), so with TWO file(...)
	// stream defs on one source line the quoted capture can span greedily
	// from the first def's opening quote to the last def's closing one,
	// silently swallowing the second def and registering the first under a
	// garbage name (found 2026-08-17; the REPL is unaffected -- it splits
	// commands at periods before parsing). The grammar stays as committed
	// (nonterminal-id shifts trip the latent anti_prenex assertion, see the
	// I4-alt note above), so adt_flatten's upfront scan turns the mis-parse
	// into a hard error instead: a captured file name containing '"' is
	// never legitimate. Default options here: flattening performs the
	// rejection, not parsing.
	TEST_CASE("two file streams on one line are rejected, not mis-captured") {
		CHECK(tau::get(std::string(
			"i1:bool := in file(\"a.in\"). i2:bool := in file(\"b.in\"). "
			"always o1[t] = i1[t].")).value_or(nullptr) == nullptr);
	}
	TEST_CASE("two file streams on separate lines parse fine") {
		CHECK(tau::get(std::string(
			"i1:bool := in file(\"a.in\").\ni2:bool := in file(\"b.in\").\n"
			"always o1[t] = i1[t] && o2[t] = i2[t].")).value_or(nullptr) != nullptr);
	}
}
