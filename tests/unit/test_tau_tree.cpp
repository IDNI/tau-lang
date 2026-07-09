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

// ── node::extension(T) / extension() round-trip (TT-2) ───────────────────────

TEST_SUITE("node::extension") {

	TEST_CASE("round-trips nt, term and ext through raw storage, resets ba_type") {
		node_t n(tau::bf_and, 42, true, 0, 1);
		auto raw = n.extension();
		node_t back = node_t::extension(raw);
		CHECK(back.nt == n.nt);
		CHECK(back.term == n.term);
		CHECK(back.ext == n.ext);
		CHECK(back.data == n.data);
		CHECK(back.ba_type == 0);
	}
}

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

	TEST_CASE("invalid_nesting_of_temp_quants flags a temporal quantifier nested in another") {
		tref inner = tau::build_wff_sometimes(x_eq_0("x"));
		tref nested = tau::build_wff_always(inner);
		CHECK(invalid_nesting_of_temp_quants<node_t>(nested));
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
