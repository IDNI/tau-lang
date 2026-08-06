// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_Bool_helpers.h"

// These tests exercise only the ADT *grammar* (type_def, member_path, ...):
// they parse with BA type inference OFF so no ADT semantics (flattening,
// registry, type checking) are required yet. See task 1 of the ADT plan.

// helper: parse a spec source into a tau tree without inference
static tref parse_no_infer(const std::string& src) {
	return tau::get(src, { .infer_ba_types = false, .flatten_adts = false });
}

TEST_SUITE("adt grammar") {
	TEST_CASE("type_def alias")   { CHECK(parse_no_infer("type byte = bv[8]. x = 0.") != nullptr); }
	TEST_CASE("type_def tuple")   { CHECK(parse_no_infer("type Point = {a: sbf, b: sbf}. x = 0.") != nullptr); }
	TEST_CASE("type_def is/of")   { CHECK(parse_no_infer("type Tagged = {tag: bv[8]}. type Line of (Tagged) is {p: Point, q: Point}. type Point = {a: sbf, b: sbf}. x = 0.") != nullptr); }
	TEST_CASE("member_path var")  { CHECK(parse_no_infer("x.a = 0.") != nullptr); }
	TEST_CASE("nested path")      { CHECK(parse_no_infer("l.p.a = l.q.a.") != nullptr); }
	TEST_CASE("io var occurrence with member path") { CHECK(parse_no_infer("p[t].a = 0.") != nullptr); }
	// I4-alt (final review): input_def/output_def's own `[member_path]`
	// production STAYS in parser/tau.tgf as committed -- removing it
	// re-drifted unrelated nonterminal-id-derived test orderings and
	// deterministically tripped an unrelated, pre-existing latent DBG
	// assertion in anti_prenex/hooks.tmpl.h (see the fixwave report's
	// "latent bug reproducer" section), so the final reviewer sanctioned
	// rejecting this in the FLATTENER instead of the grammar. A member_path
	// on the def itself still PARSES (grammar unchanged)...
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
		CHECK(tau::get(std::string("p.a := in console. p[t].a = 0.")) == nullptr);
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
	// tuple annotation on a variable is rejected at the grammar level,
	// matching the design note's "variables are annotated with named types
	// only" (private/2026-08-05-adt-design.md, section 1).
	TEST_CASE("anonymous tuple annotation on a variable fails to parse") {
		CHECK(parse_no_infer("x:{a: sbf} = 0.") == nullptr);
	}
}
