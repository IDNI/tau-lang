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
	// C1 (final review): tuple equality's shape check used to compare only
	// member COUNT and pairwise BASE TYPES, so two same-arity, same-base-
	// type-per-position but otherwise unrelated tuples expanded positionally
	// instead of failing. The fix also compares each member's path SUFFIX
	// relative to the resolved prefix.
	TEST_CASE("cross-name same-arity equality fails") {
		// Same arity (2) and same base types (sbf, sbf) at each position,
		// but DIFFERENT member names -- the pre-fix positional check would
		// have accepted this and expanded x.a = y.c, x.b = y.d.
		CHECK(flat("type A = {a: sbf, b: sbf}. type B = {c: sbf, d: sbf}. "
			"ex x:A ex y:B x = y.") == nullptr);
	}
	TEST_CASE("cross-depth same-base-types equality fails") {
		// C's one member "p" splices A's own two members in, giving C the
		// SAME arity (2) and SAME base types (sbf, sbf) as A itself -- but
		// at different, differently-named member paths ("a"/"b" vs
		// "p.a"/"p.b"). The pre-fix positional check would have accepted
		// this too.
		CHECK(flat("type A = {a: sbf, b: sbf}. type C = {p: A}. "
			"ex x:A ex y:C x = y.") == nullptr);
	}
	TEST_CASE("cross-depth same-shape equality via a different route still expands") {
		// l.p (a partial path into Line reaching Point) and x (a bare
		// Point) reach the SAME member paths ("a"/"b") by different routes
		// -- this positive case (previously untested) confirms the new
		// structural check compares PATH SUFFIXES, not raw dotted names, so
		// this still expands correctly rather than being caught as a
		// false-positive mismatch.
		check_flat(
			"type Point = {a: sbf, b: sbf}. type Line = {p: Point, q: Point}. "
			"ex l:Line ex x:Point l.p = x.",
			"ex l.p.a:sbf, l.p.b:sbf, l.q.a:sbf, l.q.b:sbf "
			"ex x.a:sbf, x.b:sbf (l.p.a = x.a && l.p.b = x.b).");
	}
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
	TEST_CASE("bf_fex expands the binder per flat member") {
		check_flat(PT "(fex x:Point (x.a)) = 0.",
			"(fex x.a:sbf, x.b:sbf (x.a)) = 0.");
	}
	TEST_CASE("bf_fall expands the binder per flat member") {
		check_flat(PT "(fall x:Point (x.a)) = 0.",
			"(fall x.a:sbf, x.b:sbf (x.a)) = 0.");
	}
	TEST_CASE("wff_all expands like wff_ex") {
		check_flat(PT "all x:Point x = 0.",
			"all x.a:sbf, x.b:sbf (x.a = 0 && x.b = 0).");
	}
	TEST_CASE("tuple equality broadcasts against constant 1") {
		check_flat(PT "ex x:Point x = 1.",
			"ex x.a:sbf, x.b:sbf (x.a = 1 && x.b = 1).");
	}
	TEST_CASE("constant on the left broadcasts too") {
		// Parse hooks may canonicalize a constant-left equality before the
		// flattener sees it; both src and expected go through the same
		// hooks, so this pins the semantic outcome regardless of which of
		// adt_flatten_rewrite_equality's two broadcast branches actually
		// fires for the surface form.
		check_flat(PT "ex x:Point 0 = x.",
			"ex x.a:sbf, x.b:sbf (0 = x.a && 0 = x.b).");
	}
	TEST_CASE("two-sided tuple inequality expands to a disjunction") {
		check_flat(PT "ex x:Point ex y:Point x != y.",
			"ex x.a:sbf, x.b:sbf ex y.a:sbf, y.b:sbf "
			"(x.a != y.a || x.b != y.b).");
	}
	TEST_CASE("tuple against a plain non-constant term fails") {
		CHECK(flat(PT "ex x:Point x = z.") == nullptr);
	}
	TEST_CASE("member access through an alias type fails") {
		CHECK(flat("type byte = bv[8]. ex x:byte x.a = 0.") == nullptr);
	}
	TEST_CASE("member path past a leaf fails") {
		CHECK(flat(PT "ex x:Point x.a.b = 0.") == nullptr);
	}
	TEST_CASE("free tuple variable equality keeps annotations on every occurrence") {
		check_flat(PT "x:Point = x && x.a = 1.",
			"x.a:sbf = x.a:sbf && x.b:sbf = x.b:sbf && x.a:sbf = 1.");
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

TEST_SUITE("adt io defs") {
	// Unlike every other suite in this file, these need an io_context, so
	// they call tau::get directly (mirroring test_io_context.cpp's own
	// construction style: a plain default-constructed io_context<node_t>)
	// rather than going through this file's flat()/check_flat() helpers,
	// which never pass one. `.infer_ba_types = false` isolates the
	// flattener's own ctx-mutation and tree-rewriting behaviour (this
	// task's actual responsibility) from whether the REWRITTEN dotted-name
	// io vars also survive inference/update_types end to end -- that full
	// pipeline is exercised by the interpreter integration tests (Task 8/10),
	// not here, matching how every other suite in this file already isolates
	// flattening from inference.
	TEST_CASE("tuple input def expands into context") {
		io_context<node_t> ctx;
		tref t = tau::get("type Point = {a: sbf, b: sbf}. p:Point := in console. "
			"always p[t] = 0.", { .infer_ba_types = false, .context = &ctx });
		REQUIRE(t != nullptr);
		auto it = ctx.adt_streams.find(dict("p"));
		REQUIRE(it != ctx.adt_streams.end());
		CHECK(it->second.is_input);
		CHECK(it->second.stream_id == 0);
		REQUIRE(it->second.components.size() == 2);
		CHECK(it->second.components[0].path == std::vector<size_t>{dict("a")});
		CHECK(it->second.components[1].path == std::vector<size_t>{dict("b")});
		CHECK(ctx.inputs.size() == 2);      // members registered, root removed
		CHECK(ctx.outputs.empty());
		// Each member is registered under its own canonized io var handle
		// (see adt_stream_component::io_var's comment, io_context.h) --
		// built the same way the flattener itself builds it (looked up
		// through the map's own transparent comparator, rather than
		// comparing htref values with `==`, which is only pointer identity
		// on the underlying shared_ptr<htree> and not guaranteed to reflect
		// structural equality).
		tref a_var = build_canonized_io_var<node_t>("p.a");
		tref b_var = build_canonized_io_var<node_t>("p.b");
		CHECK(ctx.inputs.contains(a_var));
		CHECK(ctx.inputs.contains(b_var));
		CHECK(ctx.inputs.find(it->second.components[0].io_var) != ctx.inputs.end());
		CHECK(ctx.inputs.find(it->second.components[1].io_var) != ctx.inputs.end());
		// formula flattened member-wise, printed as "p[t].a" / "p[t].b":
		// internally the flattener folds the member name into the io_var's
		// OWN var_name ("p.a", offset unchanged -- see
		// adt_flatten_build_flat_var's comment, src/adt/adt_flatten.tmpl.h,
		// and adt_stream_component::io_var's comment, io_context.h, for why
		// -- in short, so canonize<node>, unmodified by this task, keeps
		// telling members apart), but the pretty-printer (tau_tree_printers.tmpl.h,
		// the `var_name`/`io_var` cases) special-cases a dotted var_name
		// under an io_var to print the base name, then the offset bracket,
		// then the dotted suffix -- restoring the design note's source form
		// and its re-parse round-trip (see the "io member occurrence
		// round-trips through print/reparse" test below).
		std::string printed = tau::get(t).to_str();
		CHECK(printed.find("p[t].a") != std::string::npos);
		CHECK(printed.find("p[t].b") != std::string::npos);
		CHECK(printed.find("p.a[t]") == std::string::npos);
		CHECK(printed.find("p.b[t]") == std::string::npos);
	}
	TEST_CASE("io member occurrence round-trips through print/reparse") {
		io_context<node_t> ctx1;
		tref t1 = tau::get("type Point = {a: sbf, b: sbf}. p:Point := in console. "
			"always p[t].a = 0.", { .infer_ba_types = false, .context = &ctx1 });
		REQUIRE(t1 != nullptr);
		std::string printed = tau::get(t1).to_str();
		// Direct print assertion: renders as "p[t].a", not the internal
		// folded-var_name form "p.a[t]".
		CHECK(printed.find("p[t].a") != std::string::npos);
		CHECK(printed.find("p.a[t]") == std::string::npos);

		// Re-parse the printed form. The printed spec no longer has "type
		// Point"/"p:Point := in console." (both already erased/dropped by
		// the first flatten), so this second parse's registry is empty --
		// adt_flatten's fast path leaves the tree untouched -- and "p[t].a"
		// re-parses as an ordinary (now non-ADT) io_var `p[t]` plus a
		// sibling `member_path` ".a", a DIFFERENT tree shape from the first
		// parse's folded "p.a[t]" io_var than what produced this text, yet
		// prints back out to the exact same text -- the round-trip property
		// that matters here (print . reparse . print == print), which is
		// what a `this`-stream spec-as-a-value round trip actually relies
		// on, not tree-shape identity.
		io_context<node_t> ctx2;
		tref t2 = tau::get(printed, { .infer_ba_types = false, .context = &ctx2 });
		REQUIRE(t2 != nullptr);
		CHECK(tau::get(t2).to_str() == printed);
	}
	TEST_CASE("tuple io def removed from tree") {
		io_context<node_t> ctx;
		tref t = tau::get("type Point = {a: sbf, b: sbf}. p:Point := in console. "
			"always p[t] = 0.", { .infer_ba_types = false, .context = &ctx });
		REQUIRE(t != nullptr);
		CHECK(tau::get(t).to_str().find("Point") == std::string::npos);
	}
	TEST_CASE("tuple output def registers under outputs") {
		io_context<node_t> ctx;
		tref t = tau::get("type Point = {a: sbf, b: sbf}. p:Point := out console. "
			"always p[t] = 0.", { .infer_ba_types = false, .context = &ctx });
		REQUIRE(t != nullptr);
		auto it = ctx.adt_streams.find(dict("p"));
		REQUIRE(it != ctx.adt_streams.end());
		CHECK_FALSE(it->second.is_input);
		CHECK(ctx.outputs.size() == 2);
		CHECK(ctx.inputs.empty());
	}
	TEST_CASE("non-tuple io def is left alone") {
		// A registered type (Point) is present, so adt_flatten does NOT take
		// its empty-registry fast path and adt_flatten_rewrite_io_def does
		// run for "i:sbf" -- exercising its "ordinary base type: untouched"
		// branch specifically (as opposed to the fast path skipping io defs
		// entirely, which a registry-less spec would instead exercise).
		io_context<node_t> ctx;
		tref t = tau::get("type Point = {a: sbf, b: sbf}. i:sbf := in console. "
			"always i[t] = 0.", { .infer_ba_types = false, .context = &ctx });
		REQUIRE(t != nullptr);
		CHECK(ctx.adt_streams.empty());
		CHECK(ctx.inputs.size() == 1); // the plain (non-ADT) root, untouched
		CHECK(tau::get(t).to_str().find("Point") == std::string::npos); // type_def still erased
	}
	TEST_CASE("alias-typed io def rewrites the annotation, stays one stream") {
		io_context<node_t> ctx;
		tref t = tau::get("type byte = bv[8]. i:byte := in console. always i[t] = 0.",
			{ .infer_ba_types = false, .context = &ctx });
		REQUIRE(t != nullptr);
		CHECK(ctx.adt_streams.empty());
		CHECK(ctx.inputs.size() == 1); // still ONE root stream, not split into members
		CHECK(tau::get(t).to_str().find("byte") == std::string::npos); // alias rewritten away
	}
	// C2 (final review): adt_flatten_rewrite_io_def erased the tuple root
	// from ctx.inputs/outputs but left its ctx.types entry (if any) behind.
	TEST_CASE("tuple io def root has no BA type entry after flattening") {
		io_context<node_t> ctx;
		tref t = tau::get("type Point = {a: sbf, b: sbf}. p:Point := in console. "
			"always p[t] = 0.", { .infer_ba_types = false, .context = &ctx });
		REQUIRE(t != nullptr);
		tref root_var = build_canonized_io_var<node_t>("p");
		CHECK(ctx.type_of(root_var) == 0); // untyped: no phantom "p:Point" entry
	}
	// C2's actual live repro: the REPL's def_input_cmd/def_output_cmd path
	// (parser/tau.tgf's `cli` grammar) keeps the ORIGINAL, un-flattened def
	// node (still typed `Point`) in the tree for echo (see
	// adt_flatten_rewrite's `case tau::def_input_cmd` in adt_flatten.tmpl.h),
	// so -- unlike every other case in this suite, which parses with the
	// `spec`/`spec_multiline` start and .infer_ba_types = false -- this one
	// parses with the `cli` start and .infer_ba_types left at its true
	// default, exactly like the REPL's own make_cli (repl_evaluator.tmpl.h).
	// Before the fix, ordinary BA type inference resolved that surviving raw
	// copy's `typed: Point` annotation as if it were any other opaque custom
	// base type, and io_context::update_types then recreated the bare root's
	// ctx.types/ctx.inputs entries right after the flattener had erased them
	// -- all within this SAME parse -- so a later reference to the bare
	// root name (e.g. a separate REPL line's `i[t]`) would pick up a
	// phantom "Point" type.
	TEST_CASE("tuple io def root stays untyped through the REPL's cli grammar too") {
		io_context<node_t> ctx;
		// Mirrors repl_evaluator<BAs...>::make_cli's own options
		// (repl_evaluator.tmpl.h) as closely as a direct tau::get() call
		// allows, so this actually exercises the same pipeline the REPL
		// itself runs a `type ... ` + io def line through.
		tau::get_options opts{
			.parse = { .start = tau::cli },
			.infer_ba_types = true,
			.use_default_types = false,
			.reget_with_hooks = false,
			.context = &ctx
		};
		tref t = tau::get(std::string(PT "p:Point := in console."), opts);
		REQUIRE(t != nullptr);
		tref root_var = build_canonized_io_var<node_t>("p");
		CHECK(ctx.type_of(root_var) == 0);   // no phantom "p:Point" entry
		CHECK(ctx.inputs.size() == 2);       // members only, root not reinstated
		CHECK_FALSE(ctx.inputs.contains(root_var));
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
