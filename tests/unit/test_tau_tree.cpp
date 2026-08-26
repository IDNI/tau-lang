// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

// ── tree::build_shift(const std::string&, size_t) (TT-1) ────────────────────

TEST_SUITE("tree::build_shift") {

	TEST_CASE("string overload builds a shift over a fresh variable") {
		tref s = tau::build_shift(std::string("x"), 3);
		REQUIRE(s != nullptr);
		CHECK(tau::get(s).is(tau::shift));
		CHECK(tau::get(s)[1].get_num() == 3);
		CHECK(get_var_name<node_t>(tau::get(s)[0].get()) == "x");
	}
}

// ── tree::build_offsets(const std::string&/vector<string>&) (TT-10) ─────────

TEST_SUITE("tree::build_offsets") {

	TEST_CASE("string overload builds a single offset variable") {
		tref o = tau::build_offsets(std::string("n"));
		REQUIRE(o != nullptr);
		CHECK(tau::get(o).is(tau::offsets));
		CHECK(tau::get(o)[0].is(tau::offset));
		CHECK(tau::get(o)[0][0].is(tau::variable));
		CHECK(get_var_name<node_t>(tau::get(o)[0][0].get()) == "n");
	}

	TEST_CASE("vector overload builds an offset variable per name") {
		tref o = tau::build_offsets(strings{"n", "m"});
		REQUIRE(o != nullptr);
		CHECK(tau::get(o).is(tau::offsets));
		CHECK(get_var_name<node_t>(tau::get(o)[0][0].get()) == "n");
		CHECK(get_var_name<node_t>(tau::get(o)[1][0].get()) == "m");
	}
}

// (TT1-11: the node::extension round-trip suite was deleted with the API --
// the packing could not round-trip any nt >= 256 and had no callers.)

// ── get_var_name/get_var_name_sid null-node safety (TT-17) ──────────────────

TEST_SUITE("get_var_name null-node safety") {

	TEST_CASE("get_var_name returns empty string instead of crashing") {
		tref bogus = tau::get_num(5);
		CHECK(get_var_name<node_t>(bogus).empty());
	}

	TEST_CASE("get_var_name_sid returns 0 instead of crashing") {
		tref bogus = tau::get_num(5);
		CHECK(get_var_name_sid<node_t>(bogus) == 0);
	}
}

// ── canonize_quantifier_ids (TT-3) ───────────────────────────────────────────
//
// canonize_quantifier_ids renames bound variables to a canonical
// depth-based numeric name so that alpha-equivalent formulas become
// structurally identical. build_wff_ex/all(..., calculate_quant_id=true)
// and build_wff_ex_many/build_wff_all_many implement the exact same
// numbering scheme (see their doc comments), so they make a convenient,
// self-checking ground truth: build the same structure once with
// calculate_quant_id=false (an uncanonicalized "raw" input) and once with
// calculate_quant_id=true (the canonical "expected" output), and check
// that canonize_quantifier_ids maps the former to the latter.

namespace {

tref x_eq_0_bf(const char* name) {
	return build_bf_variable<node_t>(name, tau_type_id<node_t>());
}

tref x_eq_0(const char* name) {
	return tau::build_bf_eq_0(x_eq_0_bf(name));
}

} // namespace

TEST_SUITE("canonize_quantifier_ids") {

	TEST_CASE("nested quantifiers are renamed depth-first, innermost first") {
		tref x = tau::build_variable(std::string("x"), tau_type_id<node_t>());
		tref y = tau::build_variable(std::string("y"), tau_type_id<node_t>());
		tref body = tau::build_wff_and(x_eq_0("x"), x_eq_0("y"));

		// raw: ex x (ex y (x=0 && y=0)), bound vars left as-is
		tref raw = tau::build_wff_ex(x,
			tau::build_wff_ex(y, body, false), false);

		// expected: build_wff_ex_many computes ids the same way, with the
		// last bound var (y) innermost -- exactly the raw shape above
		tref expected = tau::build_wff_ex_many({ x, y }, body);

		CHECK(canonize_quantifier_ids<node_t>(raw) == expected);
	}

	TEST_CASE("sibling quantifiers (not nested) both get the innermost id") {
		tref x = tau::build_variable(std::string("x"), tau_type_id<node_t>());
		tref y = tau::build_variable(std::string("y"), tau_type_id<node_t>());

		tref raw = tau::build_wff_and(
			tau::build_wff_ex(x, x_eq_0("x"), false),
			tau::build_wff_ex(y, x_eq_0("y"), false));

		tref expected = tau::build_wff_and(
			tau::build_wff_ex(x, x_eq_0("x"), true),
			tau::build_wff_ex(y, x_eq_0("y"), true));

		CHECK(canonize_quantifier_ids<node_t>(raw) == expected);
	}

	TEST_CASE("shadowed quantifiers: reusing the same name renames by innermost active scope") {
		tref x = tau::build_variable(std::string("x"), tau_type_id<node_t>());
		tref body = x_eq_0("x");

		// raw: ex x (ex x (x=0)), same variable rebound in the inner scope
		tref raw = tau::build_wff_ex(x,
			tau::build_wff_ex(x, body, false), false);

		// expected: building the same nesting with calculate_quant_id=true
		// renames the inner x (and its occurrence in body) to "1" first;
		// the outer build_wff_ex then finds no remaining "x" to rename
		// (already renamed away by the inner call) so its own bound var
		// becomes an unused "2" -- exactly what shadowing should produce
		tref expected_inner = tau::build_wff_ex(x, body, true);
		tref expected = tau::build_wff_ex(x, expected_inner, true);

		CHECK(canonize_quantifier_ids<node_t>(raw) == expected);
	}

	TEST_CASE("functional quantifiers are left untouched") {
		// canonize_quantifier_ids only recognizes is_quantifier (wff_all/
		// wff_ex); bf_fall/bf_fex are functional quantifiers and are not
		// covered by it, so a formula built only from them must come back
		// unchanged.
		tref x = tau::build_variable(std::string("x"), tau_type_id<node_t>());
		tref y = tau::build_variable(std::string("y"), tau_type_id<node_t>());
		tref body = tau::build_bf_or(x_eq_0_bf("x"), x_eq_0_bf("y"));
		tref raw = tau::build_bf_fex(x, tau::build_bf_fex(y, body));

		CHECK(canonize_quantifier_ids<node_t>(raw) == raw);
	}
}

// ── build_wff_all_many / build_rr_ref (TT-4) ────────────────────────────────

TEST_SUITE("tree::build_wff_all_many") {

	TEST_CASE("nests with the last bound var innermost, mirroring build_wff_ex_many") {
		tref x = tau::build_variable(std::string("x"), tau_type_id<node_t>());
		tref y = tau::build_variable(std::string("y"), tau_type_id<node_t>());
		tref body = tau::build_wff_and(x_eq_0("x"), x_eq_0("y"));

		tref all_many = tau::build_wff_all_many({ x, y }, body);
		REQUIRE(all_many != nullptr);

		tref expected = tau::build_wff_all(x,
			tau::build_wff_all(y, body, false), false);
		expected = canonize_quantifier_ids<node_t>(expected);

		CHECK(all_many == expected);
	}
}

TEST_SUITE("build_rr_ref") {

	TEST_CASE("tref sym, offsets, args") {
		tref x = build_bf_variable<node_t>("x", tau_type_id<node_t>());
		tref sym = tau::build_sym("f");
		tref r = tau::build_rr_ref(sym, trefs{}, trefs{ x });
		REQUIRE(r != nullptr);
		CHECK(tau::get(r).is(tau::ref));
		CHECK(tau::get(r).children_size() == 2); // no offsets: sym, ref_args
		CHECK(tau::get(r).to_str() == "f(x)");
	}

	TEST_CASE("string sym_name, offsets, args matches the tref-sym overload") {
		tref x = build_bf_variable<node_t>("x", tau_type_id<node_t>());
		tref sym = tau::build_sym("f");
		tref by_tref = tau::build_rr_ref(sym, trefs{}, trefs{ x });
		tref by_name = tau::build_rr_ref(std::string("f"), trefs{}, trefs{ x });
		CHECK(by_tref == by_name);
	}

	TEST_CASE("string sym_name, string offset, args builds a named offset var") {
		tref x = build_bf_variable<node_t>("x", tau_type_id<node_t>());
		tref r = tau::build_rr_ref(std::string("f"), std::string("n"), trefs{ x });
		REQUIRE(r != nullptr);
		CHECK(tau::get(r).is(tau::ref));
		CHECK(tau::get(r).children_size() == 3); // sym, offsets, ref_args
	}

	TEST_CASE("string sym_name, size_t offset, args builds an integer offset") {
		tref x = build_bf_variable<node_t>("x", tau_type_id<node_t>());
		tref r = tau::build_rr_ref(std::string("f"), size_t{ 2 }, trefs{ x });
		REQUIRE(r != nullptr);
		CHECK(tau::get(r).is(tau::ref));
		CHECK(tau::get(r).children_size() == 3);
	}

	TEST_CASE("tref sym, tref offset, shift, args") {
		tref x = build_bf_variable<node_t>("x", tau_type_id<node_t>());
		tref sym = tau::build_sym("f");
		tref off = tau::build_variable(std::string("n"), tau_type_id<node_t>());
		tref r = tau::build_rr_ref(sym, off, size_t{ 3 }, trefs{ x });
		REQUIRE(r != nullptr);
		CHECK(tau::get(r).is(tau::ref));
		CHECK(tau::get(r).children_size() == 3);
	}

	TEST_CASE("string sym_name, tref offset, shift, args matches the tref-sym overload") {
		tref x = build_bf_variable<node_t>("x", tau_type_id<node_t>());
		tref sym = tau::build_sym("f");
		tref off = tau::build_variable(std::string("n"), tau_type_id<node_t>());
		tref by_tref = tau::build_rr_ref(sym, off, size_t{ 3 }, trefs{ x });
		tref by_name = tau::build_rr_ref(std::string("f"), off, size_t{ 3 }, trefs{ x });
		CHECK(by_tref == by_name);
	}

	TEST_CASE("string sym_name, string offset, shift, args") {
		tref x = build_bf_variable<node_t>("x", tau_type_id<node_t>());
		tref r = tau::build_rr_ref(std::string("f"), std::string("n"), size_t{ 3 }, trefs{ x });
		REQUIRE(r != nullptr);
		CHECK(tau::get(r).is(tau::ref));
		CHECK(tau::get(r).children_size() == 3);
	}
}

// ── expression_paths::apply / apply_only_if (TT-12) ─────────────────────────
//
// apply(transform, callback) uses a *pre-check* protocol: callback(res) is
// evaluated on the result accumulated so far, BEFORE each path is
// processed (the very first call sees res == nullptr). apply_only_if uses
// a *post-check-with-undo* protocol: it transforms a path first, checks
// the callback on that candidate, and if rejected, undoes the transform
// (restoring the untouched expression) before moving to the next path; on
// acceptance it stops immediately, applying only that single path's change.

TEST_SUITE("expression_paths::apply and apply_only_if") {

	// exactly one wff_or -> exactly two paths
	const char* sample = "(a=0||b=0)&&x=0.";

	TEST_CASE("apply: callback is checked before each path is processed") {
		tref fm = get_nso_rr(sample).value().main->get();
		trefs seen;
		auto record_and_continue = [&](tref res) {
			seen.push_back(res);
			return true;
		};
		auto identity = [](tref path) { return path; };
		expression_paths<node_t>(fm).apply(identity, record_and_continue);
		// num_paths (2) + 1: called once more before the loop finally
		// exits, and the very first call happens before any path is seen
		REQUIRE(seen.size() == 3);
		CHECK(seen.front() == nullptr);
	}

	TEST_CASE("apply: returning false immediately stops before touching any path") {
		tref fm = get_nso_rr(sample).value().main->get();
		size_t calls = 0;
		auto reject_immediately = [&](tref) { ++calls; return false; };
		auto to_T = [](tref) { return tau::_T(); };
		tref result = expression_paths<node_t>(fm).apply(to_T, reject_immediately);
		CHECK(calls == 1);
		CHECK(result == nullptr);
	}

	TEST_CASE("apply_only_if: rejecting every candidate undoes every transform") {
		tref fm = get_nso_rr(sample).value().main->get();
		trefs seen;
		auto reject_all = [&](tref res) {
			seen.push_back(res);
			return false;
		};
		auto to_T = [](tref) { return tau::_T(); };
		tref result = expression_paths<node_t>(fm).apply_only_if(to_T, reject_all);
		// the callback is only ever shown an already-transformed candidate
		for (tref s : seen) CHECK(s != nullptr);
		CHECK(seen.size() == 2); // both paths were tried and rejected
		// every transform was undone -> the original expression is restored
		CHECK(result == fm);
	}

	TEST_CASE("apply_only_if: accepting the first candidate stops immediately") {
		tref fm = get_nso_rr(sample).value().main->get();
		size_t calls = 0;
		auto accept_first = [&](tref) { ++calls; return true; };
		auto to_T = [](tref) { return tau::_T(); };
		tref result = expression_paths<node_t>(fm).apply_only_if(to_T, accept_first);
		CHECK(calls == 1); // never tries the second path
		REQUIRE(result != nullptr);
		CHECK(result != fm);
	}
}

// ── TT-21: assorted untested extractors / tau_spec / tau_bdd helpers ────────

TEST_SUITE("io helper extractors") {

	TEST_CASE("get_io_time_point/get_max_initial read an absolute time point") {
		tref v = build_in_var_at_n<node_t>("i", 5, tau_type_id<node_t>());
		tref tv = tau::trim(v);
		CHECK(get_io_time_point<node_t>(tv) == 5);
		CHECK(get_max_initial<node_t>(trefs{ tv }) == 5);
	}

	TEST_CASE("get_max_initial returns -1 when no io var is an initial one") {
		tref v = build_in_var<node_t>(build_var_name<node_t>("i"),
			tau::get(tau::offset, tau::build_shift(std::string("t"), 2)), tau_type_id<node_t>());
		tref tv = tau::trim(v);
		CHECK(get_max_initial<node_t>(trefs{ tv }) == -1);
	}

	TEST_CASE("get_io_shift reads a relative (shift) time point") {
		tref v = build_in_var<node_t>(build_var_name<node_t>("i"),
			tau::get(tau::offset, tau::build_shift(std::string("t"), 3)), tau_type_id<node_t>());
		tref tv = tau::trim(v);
		CHECK(get_io_shift<node_t>(tv) == 3);
	}

	TEST_CASE("get_max_shift takes the largest shift across io vars") {
		tref v1 = build_in_var<node_t>(build_var_name<node_t>("i"),
			tau::get(tau::offset, tau::build_shift(std::string("t"), 2)), tau_type_id<node_t>());
		tref v2 = build_in_var<node_t>(build_var_name<node_t>("j"),
			tau::get(tau::offset, tau::build_shift(std::string("t"), 5)), tau_type_id<node_t>());
		trefs vars{ tau::trim(v1), tau::trim(v2) };
		CHECK(get_max_shift<node_t>(vars) == 5);
	}

	TEST_CASE("get_max_shift with ignore_temps skips underscore-prefixed vars") {
		tref v1 = build_in_var<node_t>(build_var_name<node_t>("i"),
			tau::get(tau::offset, tau::build_shift(std::string("t"), 2)), tau_type_id<node_t>());
		tref v2 = build_in_var<node_t>(build_var_name<node_t>("_tmp"),
			tau::get(tau::offset, tau::build_shift(std::string("t"), 10)), tau_type_id<node_t>());
		trefs vars{ tau::trim(v1), tau::trim(v2) };
		CHECK(get_max_shift<node_t>(vars, false) == 10);
		CHECK(get_max_shift<node_t>(vars, true) == 2);
	}
}

TEST_SUITE("semantic-error predicates") {

	TEST_CASE("has_negative_offset flags a negative ref offset, accepts a positive one") {
		tref sym = tau::build_sym("f");
		trefs args = trefs{ x_eq_0_bf("x") };
		tref bad = tau::build_rr_ref(sym, trefs{ tau::get_integer(-1) }, args);
		tref good = tau::build_rr_ref(sym, trefs{ tau::get_integer(1) }, args);
		CHECK(has_negative_offset<node_t>(bad));
		CHECK(!has_negative_offset<node_t>(good));
	}

	TEST_CASE("invalid_nesting_of_temp_quants never flags nesting: full LTL allows it") {
		// Full LTL supports free nesting of temporal quantifiers (G(F p),
		// F(G p), etc.); the historical safety-fragment restriction that
		// rejected such nesting no longer applies (see
		// invalid_nesting_of_temp_quants's doc comment).
		tref inner = tau::build_wff_sometimes(x_eq_0("x"));
		tref nested = tau::build_wff_always(inner);
		CHECK(!invalid_nesting_of_temp_quants<node_t>(nested));
		CHECK(!invalid_nesting_of_temp_quants<node_t>(tau::build_wff_always(x_eq_0("x"))));
	}

	TEST_CASE("missing_temp_quants flags a part of the formula outside any temporal quantifier") {
		tref mixed = tau::build_wff_and(
			tau::build_wff_always(x_eq_0("x")), x_eq_0("y"));
		CHECK(missing_temp_quants<node_t>(mixed));
		CHECK(!missing_temp_quants<node_t>(tau::build_wff_always(x_eq_0("x"))));
	}

	TEST_CASE("invalid_nesting_of_quants on a non-temporal quantifier wrapping a temporal one") {
		// invalid_nesting_of_quants looks up get_free_vars on the temporal
		// quantifier node found by is_temporal_quantifier (the bare
		// wff_always/wff_sometimes node, not wrapped in an outer `wff`);
		// get_free_vars only recognizes nodes typed exactly `wff`/`bf`
		// (tau_tree_extractors.tmpl.h) and returns an empty set otherwise,
		// so the capture check can never see a free variable here and this
		// predicate is observed to always return false for this shape.
		// Pinning down the current (surprising) behavior rather than the
		// documented intent.
		tref x = tau::build_variable(std::string("x"), tau_type_id<node_t>());
		tref bad = tau::build_wff_ex(x, tau::build_wff_always(x_eq_0("x")), false);
		tref good = tau::build_wff_always(
			tau::build_wff_ex(x, x_eq_0("x"), false));
		CHECK(!invalid_nesting_of_quants<node_t>(bad));
		CHECK(!invalid_nesting_of_quants<node_t>(good));
	}

	TEST_CASE("has_semantic_error aggregates the individual predicates") {
		tref x = tau::build_variable(std::string("x"), tau_type_id<node_t>());
		tref bad = tau::build_wff_ex(x, tau::build_wff_always(x_eq_0("x")), false);
		CHECK(has_semantic_error<node_t>(bad));
		CHECK(!has_semantic_error<node_t>(get_nso_rr("x=0.").value().main->get()));
	}
}

// ── expression_paths: multi-level paths, terms, iterator equality ───────────
//
// Coverage-driven additions (2026-08-01). tau_tree_extractors.tmpl.h measured
// 85.3% line coverage, and its single largest cold region is inside
// expression_paths<node>::iterator::apply(). The TT-12 suite above uses
// "(a=0||b=0)&&x=0." -- exactly ONE wff_or -- so `decisions` never holds more
// than one element and three groups of lines never ran:
//
//   * the "go left" / "go right" arms that exclude the not-taken branch when
//     the current disjunction is NOT the last decision (needs >= 2 wff_or on
//     one path),
//   * the `term` arm, which looks for bf_or/bf_xor rather than wff_or (needs
//     expression_paths over a Boolean function rather than a formula),
//   * iterator::operator==' s two asymmetric branches, which compare decision
//     vectors of differing length.

TEST_SUITE("expression_paths: multi-level and term paths") {

	// Collect paths with a range-for. This is the only safe way: see the
	// ITERATOR CONTRACT note at the end of this suite -- building a container
	// from begin()/end() corrupts the heap.
	auto collect = [](tref e) {
		trefs out;
		for (tref p : expression_paths<node_t>(e)) out.push_back(p);
		return out;
	};

	// Two independent disjunctions -> 2 * 2 = 4 paths, and decisions grows to
	// size 2, so the non-final-decision arms of the remove lambda run.
	TEST_CASE("two disjunctions enumerate four paths") {
		tref fm = get_nso_rr("(a=0||b=0)&&(c=0||d=0)&&x=0.")
			.value().main->get();
		trefs paths = collect(fm);
		CHECK(paths.size() == 4);
		// every enumerated path is a distinct formula
		for (size_t i = 0; i < paths.size(); ++i)
			for (size_t j = i + 1; j < paths.size(); ++j)
				CHECK(paths[i] != paths[j]);
	}

	TEST_CASE("three disjunctions enumerate eight paths") {
		tref fm = get_nso_rr("(a=0||b=0)&&(c=0||d=0)&&(e=0||f=0).")
			.value().main->get();
		trefs paths = collect(fm);
		CHECK(paths.size() == 8);
	}

	// apply() over a multi-level expression exercises the excluded-subtree
	// bookkeeping that a single-disjunction expression never reaches.
	TEST_CASE("apply over two disjunctions visits every path") {
		tref fm = get_nso_rr("(a=0||b=0)&&(c=0||d=0)&&x=0.")
			.value().main->get();
		trefs seen;
		auto record = [&](tref res) { seen.push_back(res); return true; };
		auto identity = [](tref path) { return path; };
		expression_paths<node_t>(fm).apply(identity, record);
		// pre-check protocol: one call per path plus a final one
		CHECK(seen.size() == 5);
		CHECK(seen.front() == nullptr);
	}

	TEST_CASE("apply_only_if over two disjunctions undoes all on rejection") {
		tref fm = get_nso_rr("(a=0||b=0)&&(c=0||d=0)&&x=0.")
			.value().main->get();
		auto reject_all = [](tref) { return false; };
		auto to_T = [](tref) { return tau::_T(); };
		tref result = expression_paths<node_t>(fm)
			.apply_only_if(to_T, reject_all);
		CHECK(result == fm);   // fully restored
	}

	// The `term` arm: over a Boolean function, the split points are bf_or and
	// bf_xor rather than wff_or, and exhaustion yields _0 rather than _F.
	TEST_CASE("paths over a term split on bf_or") {
		static tau::get_options bf_opts{ .parse = { .start = tau::bf } };
		tref term = tau::get("a|b", bf_opts);
		REQUIRE(term != nullptr);
		trefs paths = collect(term);
		CHECK(paths.size() == 2);
	}

	TEST_CASE("paths over a term split on bf_xor") {
		static tau::get_options bf_opts{ .parse = { .start = tau::bf } };
		tref term = tau::get("a^b", bf_opts);
		REQUIRE(term != nullptr);
		trefs paths = collect(term);
		CHECK(paths.size() >= 2);
	}

	TEST_CASE("apply over a term transforms a path") {
		static tau::get_options bf_opts{ .parse = { .start = tau::bf } };
		tref term = tau::get("a|b", bf_opts);
		REQUIRE(term != nullptr);
		size_t calls = 0;
		auto accept_first = [&](tref) { ++calls; return true; };
		auto to_0 = [](tref) { return tau::_0(0); };
		tref result = expression_paths<node_t>(term)
			.apply_only_if(to_0, accept_first);
		CHECK(calls == 1);
		REQUIRE(result != nullptr);
	}

	// iterator::operator== compares decision vectors, and has separate arms
	// for "mine is shorter" and "theirs is shorter"; a begin() iterator (no
	// decisions yet) against an advanced one reaches both, depending on
	// which side is on the left.
	TEST_CASE("iterators at different decision depths compare correctly") {
		tref fm = get_nso_rr("(a=0||b=0)&&(c=0||d=0)&&x=0.")
			.value().main->get();
		expression_paths<node_t> paths(fm);
		auto first = paths.begin();
		auto advanced = paths.begin();
		++advanced;
		CHECK(first != advanced);
		CHECK(advanced != first);   // exercises the mirrored branch
		auto same = paths.begin();
		CHECK(first == same);
		// an exhausted iterator equals end() from either side
		auto it = paths.begin();
		size_t guard = 0;
		while (it != paths.end() && guard++ < 16) ++it;
		CHECK(it == paths.end());
		CHECK(paths.end() == it);
	}

	// ITERATOR CONTRACT BUG found while writing this suite (2026-08-01).
	//
	// expression_paths<node>::iterator declares
	//     using iterator_category = std::forward_iterator_tag;
	// (src/tau_tree.tmpl.h:57) but it is NOT a forward iterator:
	//
	//   * operator*() is non-const and mutates the iterator's state, and
	//   * operator++() carries the comment "WARNING: Only use after calling
	//     operator*() at least once on current state".
	//
	// That is a single-pass, dereference-before-increment iterator, i.e. at
	// most an input iterator. Standard algorithms trust the declared tag, and
	// for a forward iterator std::vector's range constructor is entitled to
	// call std::distance(first, last) to size its buffer -- which increments
	// WITHOUT dereferencing, violating the documented precondition.
	//
	// The observable result of
	//     trefs p{ ep.begin(), ep.end() };
	// is heap corruption ("free(): invalid size" / "corrupted size vs.
	// prev_size") once the expression has more than one path. The assertions
	// still pass and the path count is still right, so the damage is silent
	// and only aborts later in unrelated code.
	//
	// No production caller hits this: every use in src/ is either a range-for
	// over a single temporary or apply()/apply_only_if(), all of which are
	// genuine single passes that dereference before incrementing.
	// tests/unit/test_normal_forms.cpp ("normal forms: dnf paths" case 4) does
	// use the begin()/end() form and survives only because its sample yields
	// exactly one path.
	//
	// Fix belongs in src/: declare std::input_iterator_tag, which makes the
	// standard library stop assuming multi-pass and takes the distance-first
	// path out of play.
}

// ── TT-22: extractor branch coverage ────────────────────────────────────────
//
// Coverage-driven additions (2026-08-01), branch pass. After correcting the
// raw gcovr numbers for template-instantiation multiplicity (see
// private/add-tests-whole-code.md 6.1) and for the permanently one-sided
// branch that every LOG_TRACE/LOG_DEBUG contributes (BOOST_LOG_STREAM_SEV
// tests "is this channel enabled", which is always false in a test run),
// tau_tree_extractors.tmpl.h sat at 54.6% real branch coverage -- the worst
// of any substantial file whose gaps are reachable from a unit test.
//
// The suites below take the *untested polarity* of conditionals whose lines
// already execute. That is the class of gap line coverage cannot see: the
// line runs, but only ever one way.

namespace {

// An io_var as it exists before resolve_io_vars classifies it: data() == 0,
// i.e. neither an input nor an output yet.
tref unresolved_io_var(const std::string& name) {
	return tau::get(tau::bf, tau::get_typed(tau::variable,
		tau::get(tau::io_var, { build_var_name<node_t>(name),
			tau::get(tau::offset,
				tau::build_shift(std::string("t"), 0)) }),
		tau_type_id<node_t>()));
}

} // namespace

TEST_SUITE("resolve_io_vars direction classification") {

	// io_prefixed_io_var is a four-way condition
	//   (name[0]=='i' || name=="this") ? IN
	//                                  : (name[0]=='o' || name=="u") ? OUT
	//                                                                : unresolved
	// and only the leading-'i' arm had ever been taken.

	TEST_CASE("a leading 'i' marks an input") {
		io_context<node_t> ctx;
		tref r = resolve_io_vars<node_t>(ctx, unresolved_io_var("i1"));
		CHECK(tau::get(r).is_input_variable());
	}

	TEST_CASE("the name \"this\" marks an input") {
		io_context<node_t> ctx;
		tref r = resolve_io_vars<node_t>(ctx, unresolved_io_var("this"));
		CHECK(tau::get(r).is_input_variable());
	}

	TEST_CASE("a leading 'o' marks an output") {
		io_context<node_t> ctx;
		tref r = resolve_io_vars<node_t>(ctx, unresolved_io_var("o1"));
		CHECK(tau::get(r).is_output_variable());
	}

	TEST_CASE("the name \"u\" marks an output") {
		io_context<node_t> ctx;
		tref r = resolve_io_vars<node_t>(ctx, unresolved_io_var("u"));
		CHECK(tau::get(r).is_output_variable());
	}

	TEST_CASE("an unprefixed name stays unresolved") {
		io_context<node_t> ctx;
		tref r = resolve_io_vars<node_t>(ctx, unresolved_io_var("zzz"));
		CHECK(!tau::get(r).is_input_variable());
		CHECK(!tau::get(r).is_output_variable());
	}

	// The context lookups run before the name heuristic, so a name the
	// heuristic would reject is still resolved once it is registered.

	// EX-1 (FIXED) -- the io_context lookups used to be dead code.
	//
	// resolve_io_vars' lambda fires on the `io_var` node and used to look
	// the context up with canonize<node>(n). canonize expects the enclosing
	// `variable` node: it does `tt(new_t) | tau::io_var | tau::var_name`,
	// i.e. descends into an io_var CHILD. Handed the io_var itself that
	// selector found nothing, canonize returned its argument unchanged --
	// offset subtree and all -- so the key could never equal one registered
	// by add_input_console/add_output_console, which store
	// build_canonized_io_var<node>(name) == variable(io_var(var_name)).
	// Both lookups were therefore unreachable and classification always
	// fell through to the name heuristic.
	//
	// resolve_io_vars now builds the key the registrars' way, so an
	// explicitly registered stream resolves whatever its name looks like.

	TEST_CASE("EX-1: a registered input resolves an unprefixed name") {
		io_context<node_t> ctx;
		ctx.add_input_console("zzz", tau_type_id<node_t>());
		tref r = resolve_io_vars<node_t>(ctx, unresolved_io_var("zzz"));
		CHECK(tau::get(r).is_input_variable());
	}

	TEST_CASE("EX-1: a registered output resolves an unprefixed name") {
		io_context<node_t> ctx;
		ctx.add_output_console("zzz", tau_type_id<node_t>());
		tref r = resolve_io_vars<node_t>(ctx, unresolved_io_var("zzz"));
		CHECK(tau::get(r).is_output_variable());
	}

	TEST_CASE("EX-1: the context wins over the name heuristic") {
		// "o1" would be classified an output by name alone; registering
		// it as an input must take precedence, which only works if the
		// lookup actually hits.
		io_context<node_t> ctx;
		ctx.add_input_console("o1", tau_type_id<node_t>());
		tref r = resolve_io_vars<node_t>(ctx, unresolved_io_var("o1"));
		CHECK(tau::get(r).is_input_variable());
	}

	// canonize itself is unchanged: it is still only correct on the
	// enclosing `variable` node. This pins that contract so a future caller
	// cannot repeat EX-1 by handing it an io_var and assuming it copes.
	TEST_CASE("EX-1: canonize is only correct on the enclosing variable") {
		tref v = unresolved_io_var("zzz");
		tref var_node = tau::get(v)[0].get();
		tref io_node = tau::get(var_node)[0].get();
		tref key = build_canonized_io_var<node_t>("zzz");
		CHECK(canonize<node_t>(var_node) == key);  // the shape it wants
		CHECK(canonize<node_t>(io_node) != key);   // the shape it cannot take
	}
}

TEST_SUITE("get_nso_rr early returns") {

	// get_nso_rr accepts more than a whole spec; three of its four entry
	// shapes had no test, and each returns a differently-shaped rr.

	TEST_CASE("a null tree yields no rr") {
		CHECK(!get_nso_rr<node_t>(nullptr).has_value());
	}

	TEST_CASE("a bare bf becomes an rr that is all main and no relations") {
		tref b = tau::get("x", parse_bf());
		REQUIRE(b != nullptr);
		auto r = get_nso_rr<node_t>(b);
		REQUIRE(r.has_value());
		CHECK(r->rec_relations.empty());
		CHECK(r->main->get() == b);
	}

	TEST_CASE("a lone rec_relation becomes an rr that is all relations and no main") {
		tref defs = tau::get("f(x) := x.", parse_rec_relations());
		REQUIRE(defs != nullptr);
		tref rel = tau::get(defs)[0].get();
		REQUIRE(tau::get(rel).is(tau::rec_relation));
		auto r = get_nso_rr<node_t>(rel);
		REQUIRE(r.has_value());
		CHECK(r->rec_relations.size() == 1);
		CHECK(r->main == nullptr);
	}
}

TEST_SUITE("get_var_name_node unwrapping") {

	// get_var_name_node tries the terminals directly, then peels one
	// wrapper at a time (bf -> variable -> io_var). Only the plain
	// bf/variable route was exercised.

	TEST_CASE("a var_name node is its own name node") {
		tref vn = build_var_name<node_t>("x");
		CHECK(get_var_name_node<node_t>(vn) == vn);
		CHECK(get_var_name<node_t>(vn) == "x");
	}

#ifdef TAU_PACK_HAS_BA_BV
	TEST_CASE("a ba_constant is returned as-is, not descended into") {
		// A bv literal is used rather than a :tau one: a :tau
		// ba_constant only survives parsing when a :tau variable
		// anchors it, which would drag a whole spec into this test.
		tref c = tau::get("{1}:bv[8]", parse_bf());
		REQUIRE(c != nullptr);
		tref cte = tau::get(c).find_top(is<node_t, tau::ba_constant>);
		REQUIRE(cte != nullptr);
		CHECK(get_var_name_node<node_t>(cte) == cte);
	}
#endif // TAU_PACK_HAS_BA_BV

	TEST_CASE("unwraps bf -> variable -> var_name") {
		tref v = build_bf_variable<node_t>("x", tau_type_id<node_t>());
		REQUIRE(tau::get(v).is(tau::bf));
		CHECK(get_var_name<node_t>(v) == "x");
	}

	TEST_CASE("unwraps an io_var down to its var_name") {
		tref v = unresolved_io_var("i1");
		CHECK(get_var_name<node_t>(v) == "i1");
		CHECK(get_var_name_sid<node_t>(v) == dict("i1"));
	}

	TEST_CASE("finds a uconst_name rather than a var_name") {
		tref u = build_bf_uconst<node_t>("a", "b", tau_type_id<node_t>());
		REQUIRE(u != nullptr);
		tref n = get_var_name_node<node_t>(u);
		REQUIRE(n != nullptr);
		CHECK(tau::get(n).is(tau::uconst_name));
	}
}

TEST_SUITE("semantic-error predicates: the untested polarity") {

	// Each predicate below runs inside tau::get (tau_tree_from_parser.tmpl.h
	// calls has_semantic_error and returns nullptr when it fires), so the
	// rejection IS the observable contract. Asserting on tau::get also walks
	// the has_semantic_error || chain past the earlier predicates, which is
	// where most of its uncovered branches were.

	TEST_CASE("an open tau constant is rejected, a closed one accepted") {
		CHECK(tau::get("x:tau = { y = 0 }.") == nullptr);
		CHECK(tau::get("x:tau = { all y y = 0 }.") != nullptr);
	}

	TEST_CASE("a fallback inside a rec_relation body is rejected") {
		// `fallback` belongs on the main formula's reference. In a
		// definition body it is misplaced and has_missplaced_fallback
		// -- the last arm of the has_semantic_error chain -- rejects it.
		CHECK(tau::get("g[n](x) := g[n-1](x) fallback T."
			"g[0](x) := T."
			"g(x).") == nullptr);
		CHECK(tau::get("g[n](x) := !g[n-1](x)."
			"g[0](x) := T."
			"g(x) fallback T.") != nullptr);
	}

	TEST_CASE("nested temporal quantifiers are accepted") {
		// Full LTL allows temporal quantifiers to nest (G(F p), F(G p));
		// the historical safety-fragment restriction that rejected such
		// nesting is gone, so this parses rather than erroring out. See
		// "invalid_nesting_of_temp_quants never flags nesting: full LTL
		// allows it" in the suite above, which pins the predicate that
		// used to reject it.
		CHECK(tau::get("always (sometimes x = 0).") != nullptr);
		CHECK(tau::get("always x = 0.") != nullptr);
	}
}

TEST_SUITE("get_free_vars edge inputs") {

	TEST_CASE("a null tree has no free variables") {
		CHECK(get_free_vars<node_t>(nullptr).empty());
	}

	TEST_CASE("a node that is neither bf nor wff has no free variables") {
		// The early type guard: only bf/wff roots are analysed.
		CHECK(get_free_vars<node_t>(build_var_name<node_t>("x")).empty());
	}

	// get_free_vars' is_binder covers four node types; only the two wff
	// quantifiers had ever been seen. The functional (bf) quantifiers bind
	// exactly the same way.

	TEST_CASE("bf_fall binds its variable, leaving the other free") {
		tref x = tau::build_variable(std::string("x"), tau_type_id<node_t>());
		tref body = tau::build_bf_and(
			build_bf_variable<node_t>("x", tau_type_id<node_t>()),
			build_bf_variable<node_t>("y", tau_type_id<node_t>()));
		const trefs& fv = get_free_vars<node_t>(tau::build_bf_fall(x, body));
		REQUIRE(fv.size() == 1);
		CHECK(get_var_name<node_t>(fv[0]) == "y");
	}

	TEST_CASE("bf_fex binds its variable, leaving the other free") {
		tref x = tau::build_variable(std::string("x"), tau_type_id<node_t>());
		tref body = tau::build_bf_and(
			build_bf_variable<node_t>("x", tau_type_id<node_t>()),
			build_bf_variable<node_t>("y", tau_type_id<node_t>()));
		const trefs& fv = get_free_vars<node_t>(tau::build_bf_fex(x, body));
		REQUIRE(fv.size() == 1);
		CHECK(get_var_name<node_t>(fv[0]) == "y");
	}
}

// ── TT2-24: direct coverage for live extractor/builder functions ─────────────

TEST_SUITE("extractor/builder direct coverage (TT2-24)") {

	TEST_CASE("get_free_vars_appearance_order preserves appearance order") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
		tref fm = tau::get("zy | ax", opts);
		REQUIRE( fm != nullptr );
		trefs vars = get_free_vars_appearance_order<node_t>(fm);
		REQUIRE( vars.size() == 4 );
		CHECK( get_var_name<node_t>(vars[0]) == "z" );
		CHECK( get_var_name<node_t>(vars[1]) == "y" );
		CHECK( get_var_name<node_t>(vars[2]) == "a" );
		CHECK( get_var_name<node_t>(vars[3]) == "x" );
	}

	TEST_CASE("unnest_nested_always: G(A && G(B)) becomes G(A) && G(B)") {
		tref a = x_eq_0("x");
		tref b = x_eq_0("y");
		tref nested = build_wff_always<node_t>(tau::build_wff_and(
			a, build_wff_always<node_t>(b)));
		tref res = unnest_nested_always<node_t>(nested);
		// top must be a conjunction of two always blocks, none nested
		REQUIRE( tau::get(res)[0].is(tau::wff_and) );
		trefs gs = tau::get(res).select_all(is<node_t, tau::wff_always>);
		CHECK( gs.size() == 2 );
		for (tref g : gs)
			CHECK( !tau::get(g).find_top(is<node_t, tau::wff_and>) );
	}

	TEST_CASE("get_rr_sig: name and arities, stable across argument names") {
		tau::get_options opts = { .parse = { .start = tau::wff } };
		tref px = tau::get("p(x)", opts);
		tref py = tau::get("p(y)", opts);
		tref qxy = tau::get("q[t](x, y)", opts);
		REQUIRE( px != nullptr );
		REQUIRE( py != nullptr );
		REQUIRE( qxy != nullptr );
		rr_sig sp = get_rr_sig<node_t>(
			tau::get(px).find_top(is<node_t, tau::wff_ref>));
		rr_sig sq = get_rr_sig<node_t>(
			tau::get(qxy).find_top(is<node_t, tau::wff_ref>));
		CHECK( sp == get_rr_sig<node_t>(
			tau::get(py).find_top(is<node_t, tau::wff_ref>)) );
		CHECK( sp.arg_arity == 1 );
		CHECK( sp.offset_arity == 0 );
		CHECK( sq.arg_arity == 2 );
		CHECK( sq.offset_arity == 1 );
		CHECK( sp != sq );
	}

	TEST_CASE("build_bf_cast wraps the operand in a typed cast node") {
		tref x = build_bf_variable<node_t>("x", tau_type_id<node_t>());
		tref cast = build_bf_cast<node_t>(x, tau_type_id<node_t>());
		REQUIRE( cast != nullptr );
		CHECK( tau::get(cast).find_top(is<node_t, tau::bf_cast>) );
	}

	TEST_CASE("build_bf_interval desugars to x <= y && y <= z") {
		tref x = build_bf_variable<node_t>("x", tau_type_id<node_t>());
		tref y = build_bf_variable<node_t>("y", tau_type_id<node_t>());
		tref z = build_bf_variable<node_t>("z", tau_type_id<node_t>());
		tref iv = build_bf_interval<node_t>(x, y, z);
		REQUIRE( iv != nullptr );
		// No bf_interval node survives: the builder desugars to a
		// conjunction of the two bounds (whose lteq atoms the tau-type
		// hooks may rewrite further into equational form).
		CHECK( tau::get(iv)[0].is(tau::wff_and) );
		CHECK( iv == build_wff_and<node_t>(
			build_bf_lteq<node_t>(x, y),
			build_bf_lteq<node_t>(y, z)) );
	}
}

// NF-12: get_unbindable_relative_offset had a worked example in its own
// header doc (normalizer.h) that existed nowhere as a test.
TEST_SUITE("get_unbindable_relative_offset (NF-12)") {

	// Built directly with the ref builders rather than parsing the header
	// doc's spec: builder construction pins the function's own contract
	// without depending on parser typing of an ill-formed definition.
	TEST_CASE("unbound relative offset in a body is reported; bound is not") {
		tref x = build_bf_variable<node_t>("x", tau_type_id<node_t>());
		// body contains r[n](x) -- a ref carrying a variable offset `n`
		tref body = tau::build_rr_ref(std::string("r"),
			std::string("n"), trefs{ x });
		// head `f(x)` declares no offset: the `n` in the body is unbound
		tref f_head = tau::build_rr_ref(std::string("f"),
			trefs{}, trefs{ x });
		CHECK( get_unbindable_relative_offset<node_t>(f_head, body)
			!= nullptr );
		// head `r[n](x)` declares `[n]`, which binds the body's `n`
		tref r_head = tau::build_rr_ref(std::string("r"),
			std::string("n"), trefs{ x });
		CHECK( get_unbindable_relative_offset<node_t>(r_head, body)
			== nullptr );
	}
}

// RR-12: rr_dict had zero direct coverage (incl. the invalid-id throw).
TEST_SUITE("rr_dict (RR-12)") {

	TEST_CASE("string ids round-trip and are stable") {
		size_t a = rr_dict("rr_dict_test_a");
		size_t b = rr_dict("rr_dict_test_b");
		CHECK( a != b );
		CHECK( rr_dict("rr_dict_test_a") == a );
		CHECK( rr_dict(a) == "rr_dict_test_a" );
		CHECK( rr_dict(b) == "rr_dict_test_b" );
	}

	TEST_CASE("invalid id throws instead of reading out of bounds") {
		CHECK_THROWS_AS( rr_dict(size_t(-1)), std::logic_error );
	}
}
