// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"
#include <set>
#include <algorithm>
#include <string>

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

	TEST_CASE("functional quantifiers are numbered like formula binders") {
		// bf_fall/bf_fex share ONE id space and ONE depth count with
		// wff_all/wff_ex, so a chain of functional quantifiers is numbered
		// by depth exactly as a chain of formula binders is, and
		// build_bf_fex(..., calculate_quant_id=true) is the same
		// self-checking ground truth used above.
		tref x = tau::build_variable(std::string("x"), tau_type_id<node_t>());
		tref y = tau::build_variable(std::string("y"), tau_type_id<node_t>());
		tref body = tau::build_bf_or(x_eq_0_bf("x"), x_eq_0_bf("y"));
		tref raw = tau::build_bf_fex(x,
			tau::build_bf_fex(y, body, false), false);

		tref expected = tau::build_bf_fex(x,
			tau::build_bf_fex(y, body, true), true);

		CHECK(canonize_quantifier_ids<node_t>(raw) == expected);
	}

	TEST_CASE("a functional quantifier lowers an enclosing binder's id") {
		// The depth count spans the two kinds, so the formula binder over
		// a functional one is 2, not the 1 a formula-only count gives.
		tref x = tau::build_variable(std::string("x"), tau_type_id<node_t>());
		tref y = tau::build_variable(std::string("y"), tau_type_id<node_t>());
		tref inner = tau::build_bf_eq_0(
			tau::build_bf_fex(y, x_eq_0_bf("y"), false));
		tref raw = tau::build_wff_ex(x, inner, false);

		tref one = tau::build_variable(std::string("1"), tau_type_id<node_t>());
		tref two = tau::build_variable(std::string("2"), tau_type_id<node_t>());
		tref expected = tau::build_wff_ex(two, tau::build_bf_eq_0(
			tau::build_bf_fex(one, x_eq_0_bf("1"), false)), false);

		CHECK(canonize_quantifier_ids<node_t>(raw) == expected);
	}

	TEST_CASE("a fresh formula binder id clears the functional subscripts below") {
		// build_wff_ex(..., true) mints find_biggest_var_id + 1. The
		// scan has to see the `fex 1` inside the term: minting 1 again
		// would put the new binder on the subscript's name.
		tref x = tau::build_variable(std::string("x"), tau_type_id<node_t>());
		tref one = tau::build_variable(std::string("1"), tau_type_id<node_t>());
		tref term = tau::build_bf_eq_0(
			tau::build_bf_fex(one, x_eq_0_bf("1"), false));
		tref got = tau::build_wff_ex(x,
			tau::build_wff_and(x_eq_0("x"), term), true);
		CHECK(get_var_name<node_t>(tau::trim2(got)) == "2");
	}

	TEST_CASE("substitute re-canonicalizes across the two kinds") {
		// The API re-runs the pass whenever the replacement holds a
		// functional quantifier — `tree::substitute` leaves the ids it
		// finds alone — and the pass handles one, so the
		// enclosing formula binder is renumbered over the shared depth
		// count instead of colliding with the subscript.
		tref x = tau::build_variable(std::string("x"), tau_type_id<node_t>());
		tref z = tau::build_variable(std::string("z"), tau_type_id<node_t>());
		tref fm = tau::build_wff_ex(x,
			tau::build_wff_and(x_eq_0("x"), x_eq_0("y")), true);
		tref with = tau::build_bf_fex(z, x_eq_0_bf("z"), false);
		tref got = canonize_quantifier_ids<node_t>(
			tau::get(fm).substitute(x_eq_0_bf("y"), with));

		tref one = tau::build_variable(std::string("1"), tau_type_id<node_t>());
		tref two = tau::build_variable(std::string("2"), tau_type_id<node_t>());
		tref expected = tau::build_wff_ex(two, tau::build_wff_and(x_eq_0("2"),
			tau::build_bf_eq_0(tau::build_bf_fex(one, x_eq_0_bf("1"), false))),
			false);
		CHECK(got == expected);
	}
}

// ── build_binder: fresh ids ──────────────────────────────────────────────────
//
// The four binder builders share build_binder. With calculate_quant_id the
// fresh id is one above every purely numeric variable name in the body,
// bound or free, and only the free occurrences of the bound variable are
// renamed -- the rename stops at an inner binder of the same variable.

TEST_SUITE("build_binder: fresh ids") {

	namespace {
	tref v(const char* name) {
		return tau::build_variable(std::string(name), tau_type_id<node_t>());
	}
	std::string bound_name(tref n) {
		return get_var_name<node_t>(tau::trim2(n));
	}
	}

	TEST_CASE("wff: an inner binder of the same name keeps it") {
		// ex x. (x = 0 & ex x. x = 0): the outer x becomes 1, the inner
		// binder and its occurrence stay x.
		tref inner = tau::build_wff_ex(v("x"), x_eq_0("x"), false);
		tref got = tau::build_wff_ex(v("x"),
			tau::build_wff_and(x_eq_0("x"), inner), true);
		tref expected = tau::build_wff_ex(v("1"),
			tau::build_wff_and(x_eq_0("1"), inner), false);
		CHECK(got == expected);
	}

	TEST_CASE("bf: an inner functional quantifier of the same name keeps it") {
		tref inner = tau::build_bf_fex(v("x"), x_eq_0_bf("x"), false);
		tref got = tau::build_bf_fex(v("x"),
			tau::build_bf_or(x_eq_0_bf("x"), inner), true);
		tref expected = tau::build_bf_fex(v("1"),
			tau::build_bf_or(x_eq_0_bf("1"), inner), false);
		CHECK(got == expected);
	}

	TEST_CASE("wff: the fresh id clears a free numeric variable") {
		// (ex 1. 1 = 0) & 2 = 0, with 2 free -- a fragment of some
		// `ex 2. ...`. The fresh id must skip past 2, not take it.
		tref body = tau::build_wff_and(
			tau::build_wff_ex(v("1"), x_eq_0("1"), false), x_eq_0("2"));
		CHECK(bound_name(tau::build_wff_all(v("y"), body, true)) == "3");
	}

	TEST_CASE("bf: the fresh id clears a free numeric variable") {
		tref body = tau::build_bf_or(
			tau::build_bf_fex(v("1"), x_eq_0_bf("1"), false), x_eq_0_bf("2"));
		CHECK(bound_name(tau::build_bf_fall(v("y"), body, true)) == "3");
	}

	TEST_CASE("wff: a free numeric variable below a numeric binder is seen") {
		// ex 1. (1 = 0 & 2 = 0), 2 free: the scan must not stop at the
		// numeric binder on the way to it.
		tref body = tau::build_wff_ex(v("1"),
			tau::build_wff_and(x_eq_0("1"), x_eq_0("2")), false);
		CHECK(bound_name(tau::build_wff_ex(v("y"), body, true)) == "3");
	}

	TEST_CASE("bf: a free numeric variable below a numeric binder is seen") {
		tref body = tau::build_bf_fex(v("1"),
			tau::build_bf_or(x_eq_0_bf("1"), x_eq_0_bf("2")), false);
		CHECK(bound_name(tau::build_bf_fex(v("y"), body, true)) == "3");
	}

	TEST_CASE("the block form renames only the free occurrences too") {
		tref inner = tau::build_wff_ex(v("x"), x_eq_0("x"), false);
		tref got = tau::build_wff_all_many({ v("x") },
			tau::build_wff_and(x_eq_0("x"), inner));
		tref expected = tau::build_wff_all(v("1"),
			tau::build_wff_and(x_eq_0("1"), inner), false);
		CHECK(got == expected);
	}

	TEST_CASE("the block form numbers the last variable innermost, above the body") {
		// body binds 1 and has 2 free: x, y become 4, 3 -- y innermost.
		tref body = tau::build_wff_and(
			tau::build_wff_ex(v("1"), x_eq_0("1"), false), x_eq_0("2"));
		tref got = tau::build_wff_ex_many({ v("x"), v("y") },
			tau::build_wff_and(body, tau::build_wff_and(x_eq_0("x"), x_eq_0("y"))));
		CHECK(bound_name(got) == "4");
		CHECK(bound_name(tau::get(got)[0].second()) == "3");
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

	TEST_CASE("nested temporal quantifiers are rejected") {
		CHECK(tau::get("always (sometimes x = 0).") == nullptr);
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
		const trefs& fv = get_free_vars<node_t>(
			tau::build_bf_fall(x, body, false));
		REQUIRE(fv.size() == 1);
		CHECK(get_var_name<node_t>(fv[0]) == "y");
	}

	TEST_CASE("bf_fex binds its variable, leaving the other free") {
		tref x = tau::build_variable(std::string("x"), tau_type_id<node_t>());
		tref body = tau::build_bf_and(
			build_bf_variable<node_t>("x", tau_type_id<node_t>()),
			build_bf_variable<node_t>("y", tau_type_id<node_t>()));
		const trefs& fv = get_free_vars<node_t>(
			tau::build_bf_fex(x, body, false));
		REQUIRE(fv.size() == 1);
		CHECK(get_var_name<node_t>(fv[0]) == "y");
	}
}
// ── get_free_vars: per-subtree cache keeps scope handling intact ─────────────
//
// get_free_vars memoizes the free-variable sets of spine subtrees (binders,
// and connectives with a connective or binder child) across calls. These
// cases warm the cache through an enclosing formula and then ask for its
// subtrees, checking that a cached answer equals a direct one: binder
// subtraction still applies at every level, sibling context does not leak,
// and the result stays trimmed and sorted.
TEST_SUITE("get_free_vars cache") {

	static std::set<std::string> names(const trefs& vs) {
		std::set<std::string> out;
		for (tref v : vs) out.insert(get_var_name<node_t>(v));
		return out;
	}
	static tref var(const char* n) {
		return tau::build_variable(std::string(n), tau_type_id<node_t>());
	}

	TEST_CASE("subtree answers after warming through the parent match direct ones") {
		// F = ex a ((a=0 && (b=0 || c=0)) && ex d (d=0 && e=0))
		tref left = tau::build_wff_and(x_eq_0("a"),
			tau::build_wff_or(x_eq_0("b"), x_eq_0("c")));
		tref inner_body = tau::build_wff_and(x_eq_0("d"), x_eq_0("e"));
		tref inner = tau::build_wff_ex(var("d"), inner_body, false);
		tref body = tau::build_wff_and(left, inner);
		tref F = tau::build_wff_ex(var("a"), body, false);

		// Warm: the whole formula first.
		CHECK(names(get_free_vars<node_t>(F)) == std::set<std::string>{ "b", "c", "e" });
		// Then every spine subtree, each of which the walk above visited.
		CHECK(names(get_free_vars<node_t>(body)) == std::set<std::string>{ "a", "b", "c", "e" });
		CHECK(names(get_free_vars<node_t>(inner)) == std::set<std::string>{ "e" });
		CHECK(names(get_free_vars<node_t>(inner_body)) == std::set<std::string>{ "d", "e" });
		CHECK(names(get_free_vars<node_t>(left)) == std::set<std::string>{ "a", "b", "c" });
	}

	TEST_CASE("results are sorted by subtree_less and right-sibling-trimmed") {
		tref body = tau::build_wff_and(
			tau::build_wff_and(x_eq_0("q"), x_eq_0("p")),
			tau::build_wff_or(x_eq_0("s"), x_eq_0("r")));
		tref F = tau::build_wff_ex(var("s"), body, false);
		const trefs& fv = get_free_vars<node_t>(F);
		REQUIRE(fv.size() == 3);
		CHECK(std::is_sorted(fv.begin(), fv.end(), tau::subtree_less));
		for (tref v : fv) CHECK(!tau::get(v).has_right_sibling());
		// The cached body answer is the same vector shape, plus the bound one.
		const trefs& bv = get_free_vars<node_t>(body);
		REQUIRE(bv.size() == 4);
		CHECK(std::is_sorted(bv.begin(), bv.end(), tau::subtree_less));
		for (tref v : bv) CHECK(!tau::get(v).has_right_sibling());
	}

	TEST_CASE("an accumulating conjunction of independent clauses") {
		// The shape from IDNI/tau-lang#90: k independent clauses conjoined one
		// at a time, each query seeing the previous conjunction as a subtree.
		tref acc = nullptr;
		std::set<std::string> expect;
		for (int i = 0; i < 40; ++i) {
			std::string a = "u" + std::to_string(i), b = "v" + std::to_string(i);
			tref clause = tau::build_wff_or(x_eq_0(a.c_str()), x_eq_0(b.c_str()));
			acc = acc ? tau::build_wff_and(acc, clause) : clause;
			expect.insert(a); expect.insert(b);
			CHECK(names(get_free_vars<node_t>(acc)) == expect);
		}
		// Binding one variable at the top removes exactly that one.
		tref F = tau::build_wff_ex(var("u7"), acc, false);
		std::set<std::string> minus = expect; minus.erase("u7");
		CHECK(names(get_free_vars<node_t>(F)) == minus);
	}

	TEST_CASE("a chain is taken apart without losing a nested chain of another"
		" connective")
	{
		// The `||` inside the `&&` chain is an operand of it, not a link,
		// and keeps an answer of its own.
		tref nested = tau::build_wff_or(
			tau::build_wff_and(x_eq_0("c"), x_eq_0("d")), x_eq_0("e"));
		tref F = tau::build_wff_and(
			tau::build_wff_and(tau::build_wff_or(x_eq_0("a"), x_eq_0("b")),
				nested), x_eq_0("f"));
		CHECK(names(get_free_vars<node_t>(F))
			== std::set<std::string>{ "a", "b", "c", "d", "e", "f" });
		CHECK(names(get_free_vars<node_t>(nested))
			== std::set<std::string>{ "c", "d", "e" });
	}

	TEST_CASE("a subformula several branches reach is answered once and"
		" consistently")
	{
		// shared appears under both sides of the conjunction, and as a
		// whole formula of its own.
		tref shared = tau::build_wff_and(x_eq_0("s1"),
			tau::build_wff_and(x_eq_0("s2"), x_eq_0("s3")));
		tref F = tau::build_wff_and(tau::build_wff_and(x_eq_0("p"), shared),
			tau::build_wff_and(x_eq_0("q"), shared));
		CHECK(names(get_free_vars<node_t>(F))
			== std::set<std::string>{ "p", "q", "s1", "s2", "s3" });
		CHECK(names(get_free_vars<node_t>(shared))
			== std::set<std::string>{ "s1", "s2", "s3" });
		// And the other way round: the shared answer first, the whole
		// formula after it.
		tref other = tau::build_wff_or(x_eq_0("t1"), x_eq_0("t2"));
		CHECK(names(get_free_vars<node_t>(other))
			== std::set<std::string>{ "t1", "t2" });
		CHECK(names(get_free_vars<node_t>(tau::build_wff_and(other, F)))
			== std::set<std::string>{ "p", "q", "s1", "s2", "s3",
				"t1", "t2" });
	}

	TEST_CASE("a Boolean term grown one factor at a time") {
		// Term structure carries free variables as formula structure does,
		// and is asked about through the equation that wraps it.
		tref acc = nullptr;
		std::set<std::string> expect;
		for (int i = 0; i < 40; ++i) {
			std::string w = "w" + std::to_string(i);
			tref v = build_bf_variable<node_t>(w, tau_type_id<node_t>());
			acc = acc ? tau::build_bf_and(acc, v) : v;
			expect.insert(w);
			CHECK(names(get_free_vars<node_t>(tau::build_bf_eq_0(acc)))
				== expect);
		}
	}

	TEST_CASE("a binder over a term chain subtracts its own variable") {
		// all x ((x & y) = 0): the bound variable is taken out of a set
		// the term chain produced.
		tref x = build_bf_variable<node_t>(std::string("x"),
			tau_type_id<node_t>());
		tref y = build_bf_variable<node_t>(std::string("y"),
			tau_type_id<node_t>());
		tref body = tau::build_bf_eq_0(tau::build_bf_and(x, y));
		CHECK(names(get_free_vars<node_t>(body))
			== std::set<std::string>{ "x", "y" });
		tref F = tau::build_wff_all(var("x"), body, false);
		CHECK(names(get_free_vars<node_t>(F)) == std::set<std::string>{ "y" });
	}

	TEST_CASE("a closed formula has no free variables and says so twice") {
		tref F = tau::build_wff_ex(var("z"),
			tau::build_wff_and(x_eq_0("z"), x_eq_0("z")), false);
		CHECK(get_free_vars<node_t>(F).empty());
		CHECK(get_free_vars<node_t>(F).empty());
	}
}

// ── tree::substitute (tau_tree_substitute.tmpl.h) ───────────────────────
//
// Never assert on to_str() of a BDD-backed term: `BDD_ID` is an unreachable
// production of the grammar. Identity is checked through trefs (hash-consed)
// and through BDD refs (canonical under one order).

namespace {

/// Any `BDD_ID` node anywhere in the tree.
bool has_bdd_id(tref n) {
	return tau::get(n).find_top([](tref m) {
		return tau::get(m).is(tau::BDD_ID); }) != nullptr;
}

tref bf(const char* s) {
	tref t = tau::get(s, parse_bf());
	REQUIRE(t != nullptr);
	return t;
}
tref wff(const char* s) {
	tref t = tau::get(s, parse_wff());
	REQUIRE(t != nullptr);
	return t;
}
/// The trimmed `variable` node of a one-variable term — the shape order keys
/// and free-variable sets hold.
tref vr(const char* s) { return tau::trim(bf(s)); }

/// The first node of kind `nt` below `n` (the root included).
tref find_kind(tref n, size_t nt) {
	return tau::get(n).find_top([nt](tref m) {
		return tau::get(m).is(nt); });
}

/// The bound variable of the first binder of kind `nt` below `n`.
std::string binder_id(tref n, size_t nt) {
	tref q = find_kind(n, nt);
	REQUIRE(q != nullptr);
	return get_var_name<node_t>(tau::get(q).first());
}

/// Semantic equality of two terms over the variables `vs`: equal ROBDDs under
/// one order over all of them (leaves opaque). Both are spelled out first,
/// since a BDD-backed term's own ref belongs to its own order.
bool same_function(tref a, tref b, const trefs& vs) {
	using bdd  = tau_term_bdd<node_t>;
	using hbdd = term_handle<node_t>;
	bdd::order o;
	for (size_t i = 0; i < vs.size(); ++i) o.emplace(vs[i], int_t(i));
	return bdd::build_bdd(hbdd::convert_to_tau_terms(a), o)
		== bdd::build_bdd(hbdd::convert_to_tau_terms(b), o);
}

/// A reference `r(<args>)` as a `bf` term. The grammar's `ref_arg` is `bf`
/// only, so a FORMULA argument — the shape a re-simplification hook is
/// visible on — is built, not parsed.
tref ref_term(const trefs& args) {
	return tau::get(tau::bf, tau::get(tau::bf_ref, tau::build_ref("r", args)));
}

} // namespace

TEST_SUITE("tree::substitute") {
	using bdd  = tau_term_bdd<node_t>;
	using hbdd = term_handle<node_t>;

	TEST_CASE("substitute x→z in BDD of xy gives BDD of yz") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		tref tz = tau::trim(tau::get("z", opts));
		// Order: x(0) < y(1) < z(2); z is above both x and y
		bdd::order o = {{tx, 0}, {ty, 1}, {tz, 2}};
		// Build BDD for "xy" and register as a BDD_ID tau node in U
		tref node_xy = hbdd::convert_to_tau_node(tau::get("xy", opts), o);
		// Substitute x → z across the formula containing the BDD node
		tref result_node = tau::get(node_xy).substitute(
			tau::get("x", opts), tau::get("z", opts), o);
		// Retrieve the tau term for the resulting BDD (z has rank 2 > y rank 1, so y is above z)
		tref result_term = hbdd::U.find(hbdd::key_of(result_node))
			->second.to_tau_term(1);
		CHECK(tau::get(result_term).to_str() == "yz");
	}

	TEST_CASE("a plain formula: the occurrences go, and a formula without the key comes back as itself") {
		tref x = vr("x");
		tref phi = wff("x & a = 0 && b & c = 0");
		tref res = tau::get(phi).substitute(bf("x"), bf("z"));
		CHECK(res != phi);
		CHECK(res == wff("z & a = 0 && b & c = 0"));
		// the bare spelling of both key and replacement: the same result
		CHECK(tau::get(phi).substitute(x, tau::trim(bf("z"))) == res);
		// nothing to do: the same tref, spelling and all
		tref psi = wff("b & c = 0");
		CHECK(tau::get(psi).substitute(bf("x"), bf("z")) == psi);
	}

	TEST_CASE("a bare variable key with a bare replacement keeps the wrapper") {
		// A key is matched as given: a bare `variable` key is met at
		// the variable node, and the `bf` wrapper around it is rebuilt
		// over the replacement.
		tref x = vr("x");
		tref f = build_bf_and<node_t>(bf("a"), bf("x"));
		tref yz = build_bf_and<node_t>(bf("y"), bf("z"));
		tref want = build_bf_and<node_t>(bf("a"), yz);
		CHECK(tau::get(f).substitute(x, tau::trim(yz)) == want);
		// the wrapped spelling of both gives the same node
		CHECK(tau::get(f).substitute(bf("x"), yz) == want);
		// a bare constant replacement keeps the wrapper too
		CHECK(tau::get(bf("x")).substitute(x, tau::trim(bf("1")))
			== bf("1"));
	}

	TEST_CASE("a BDD-backed term: the leaves are entered and the decision variable composes") {
		tref x = vr("x"), y = vr("y"), z = vr("z");
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		bdd::order o {{y, 0}, {x, 1}};
		tref f = hbdd::convert_to_tau_node(bf("x & r(x)"), o);
		REQUIRE(hbdd::is_bdd_backed(f));
		// The replacement is plain: the compose builds its BDD under the
		// live order, and the leaf rewrite — x hides inside the
		// reference argument — puts it in as it stands.
		tref t = bf("y & z");
		REQUIRE(!has_bdd_id(t));
		tref s = tau::get(f).substitute(bf("x"), t, o);
		CHECK(hbdd::is_bdd_backed(s));
		CHECK(same_function(s, bf("y & z & r(y & z)"), { x, y, z }));
		// the finish: nothing BDD-backed remains anywhere
		CHECK(!has_bdd_id(hbdd::convert_to_tau_terms(s)));
		// a replacement carrying a BDD_ID is spelled out ONCE by the
		// entry step, and gives the same result
		tref backed = hbdd::convert_to_tau_node(t, o);
		REQUIRE(hbdd::is_bdd_backed(backed));
		CHECK(tau::get(f).substitute(bf("x"), backed, o) == s);
		// and the plain path over the same term agrees as a function,
		// with no BDD anywhere
		CHECK(same_function(s, tau::get(bf("x & r(x)")).substitute(bf("x"), t, {}),
			{ x, y, z }));
	}

	TEST_CASE("a leaf that gains a decision variable is re-canonicalised") {
		tref p = vr("p"), b = vr("b"), c = vr("c");
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		bdd::order o {{p, 0}};
		// p·a ∪ p′·b with the outer variable a in a leaf; a ← p·c puts
		// the decision variable into that leaf, which the rebuild lifts
		tref f = hbdd::convert_to_tau_node(bf("pa|p'b"), o);
		REQUIRE(hbdd::is_bdd_backed(f));
		tref s = tau::get(f).substitute(bf("a"), bf("p & c"), o);
		CHECK(hbdd::is_bdd_backed(s));
		CHECK(same_function(s, bf("pc|p'b"), { p, b, c }));
	}

	TEST_CASE("a reference argument is reached, and the hook runs once per touched argument") {
		tref x = vr("x"), z = vr("z");
		// (S·v2) ∪ (S·v3) with S = x·r(x, ψ(x)): the two occurrences of
		// S are two spellings of one content, so S is rewritten once
		// and each of the reference's two touched arguments is
		// re-emitted exactly once.
		tref psi = wff("x & v1 = 0");
		tref S = build_bf_and<node_t>(bf("x"),
			ref_term(trefs{ bf("x"), psi }));
		tref f = build_bf_or<node_t>(build_bf_and<node_t>(S, bf("v2")),
			build_bf_and<node_t>(S, bf("v3")));
		size_t calls = 0;
		auto count = [&calls](tref a) { ++calls; return a; };
		tref s = tau::get(f).substitute(bf("x"), bf("z"), {}, count);
		CHECK(calls == 2);
		CHECK(s != f);
		const trefs& fv = get_free_vars<node_t>(s);
		CHECK(!std::binary_search(fv.begin(), fv.end(), x, tau::subtree_less));
		CHECK(std::binary_search(fv.begin(), fv.end(), z, tau::subtree_less));
		// Both copies were rewritten, the argument formula with them.
		tref S_z = build_bf_and<node_t>(bf("z"),
			ref_term(trefs{ bf("z"), wff("z & v1 = 0") }));
		CHECK(s == build_bf_or<node_t>(build_bf_and<node_t>(S_z, bf("v2")),
			build_bf_and<node_t>(S_z, bf("v3"))));
		// An argument the key does not reach is not re-emitted.
		tref g = build_bf_and<node_t>(bf("x"),
			ref_term(trefs{ bf("v1"), wff("v1 & v4 = 0") }));
		calls = 0;
		(void) tau::get(g).substitute(bf("x"), bf("z"), {}, count);
		CHECK(calls == 0);
	}

	TEST_CASE("a shared reference argument is re-emitted once per call") {
		// The SAME `ref_arg` node under two different references. The
		// traversal's own cache does not cover it — a node the walk
		// changes never enters that cache — so the call-wide argument
		// memo is what keeps the hook to one run.
		tref psi = wff("x & v1 = 0");
		tref f = build_bf_and<node_t>(ref_term(trefs{ psi }),
			ref_term(trefs{ bf("v9"), psi }));
		size_t calls = 0;
		auto count = [&calls](tref a) { ++calls; return a; };
		tref s = tau::get(f).substitute(bf("x"), bf("z"), {}, count);
		CHECK(s != f);
		CHECK(calls == 1);
	}

	TEST_CASE("the substitution is simultaneous: a key inside the replacement stays") {
		tref x = vr("x"), a = vr("a"), b = vr("b");
		tref res = tau::get(bf("x & a")).substitute(bf("x"), bf("x & b"));
		CHECK(same_function(res, bf("x & b & a"), { x, a, b }));
	}

	TEST_CASE("a replacement carrying a functional quantifier is renamed apart") {
		tref unit = wff("ex z (x & z = 0)");    // z is "1"
		tref t = bf("fall w (w | q5)");         // w is "1" as well
		REQUIRE(binder_id(unit, tau::wff_ex) == "1");
		REQUIRE(binder_id(t, tau::bf_fall) == "1");
		tref res = tau::get(unit).substitute(bf("x"), t);
		// the unit's binder keeps its id; the subscript moved above it
		CHECK(binder_id(res, tau::wff_ex) == "1");
		CHECK(binder_id(res, tau::bf_fall) != "1");
		// the free variables are the replacement's, unchanged by the rename
		CHECK(get_free_vars<node_t>(res).size() == 1);
		CHECK(get_free_vars<node_t>(res)[0] == vr("q5"));
	}

	TEST_CASE("rename apart: the base counts ids that live only inside a BDD_ID") {
		// The largest ids around may occur ONLY inside a backed term —
		// a decision variable has no tree node at all. If the base
		// missed them, a renamed subscript could land on one and the
		// clash would surface when the BDD is spelled out again.
		tref x = vr("x");
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref p5 = tau::build_variable("5", tau::get(x).get_ba_type());
		bdd::order o {{p5, 0}};
		tref f = hbdd::convert_to_tau_node(tau::build_bf_and(
			tau::get(tau::bf, x), tau::get(tau::bf, p5)), o);
		REQUIRE(hbdd::is_bdd_backed(f));
		// "5" has no variable node left in the term, and the base still sees it
		CHECK(tau::get(f).find_top([](tref m) {
			return tau::get(m).is(tau::variable); }) == nullptr);
		CHECK(find_biggest_var_id<node_t>(f) == 5);
		tref t = bf("fall w (w | q5)");         // the subscript is "1"
		REQUIRE(binder_id(t, tau::bf_fall) == "1");
		tref res = tau::get(f).substitute(bf("x"), t, o);
		// the chain rode into a BDD leaf; spell the term to read its subscript
		CHECK(binder_id(hbdd::convert_to_tau_terms(res), tau::bf_fall)
			== "6");                        // 1 + base, base = 5
	}

	TEST_CASE("the walk descends into a binder and keeps it") {
		tref unit = wff("ex z (x & z = 0)");
		CHECK(tau::subtree_equals(tau::get(unit).substitute(bf("x"), bf("y")),
			wff("ex z (y & z = 0)")));
		// two levels, a mixed kind: the ids are 2 and 1, neither is `y`
		tref deep = wff("all w ex z (x & z = w)");
		CHECK(tau::subtree_equals(tau::get(deep).substitute(bf("x"), bf("y")),
			wff("all w ex z (y & z = w)")));
		// x ← x changes nothing, so nothing is re-shaped: the same tref
		tref two = wff("x = 0 && x = 1");
		CHECK(tau::get(two).substitute(bf("x"), bf("x")) == two);
		// and an untouched member of a rewritten chain is reused
		tref d = wff("a = 0 || b = 0");
		tref r = tau::get(build_wff_and<node_t>(wff("x = 0"), d))
			.substitute(bf("x"), bf("y"));
		tref kept = tau::get(r).find_top([](tref m) {
			return tau::get(m).is(tau::wff)
				&& tau::get(m).child_is(tau::wff_or); });
		REQUIRE(kept != nullptr);
		CHECK(tau::trim_right_sibling(kept) == d);
	}

	TEST_CASE("the rebuild goes through the construction hooks") {
		auto sub = [](const char* fm, const char* with) {
			return tau::get(wff(fm)).substitute(bf("x"), bf(with));
		};
		CHECK(tau::get(sub("x = 0", "1")).equals_F());
		CHECK(tau::get(sub("x = y", "y")).equals_T());
		// a chain folds around a decided atom
		CHECK(tau::get(sub("x = 0 && b = 0", "1")).equals_F());
		CHECK(tau::subtree_equals(sub("x = 0 || b = 0", "1"), wff("b = 0")));
		// the TERM hooks fold inside a rewritten side before the atom is
		// rebuilt: y′·y is 0, and 0 = 0 is T
		CHECK(tau::get(sub("x & y = 0", "y'")).equals_T());
		// what no hook folds stays: the deep folding is the caller's
		tref r = sub("x & y = 0", "z'");
		REQUIRE(is_atomic_fm<node_t>(r));
		CHECK(find_kind(r, tau::bf_neg) != nullptr);
		// a negated atom keeps its negation; a negated decided atom folds
		CHECK(tau::subtree_equals(sub("!(x = 0)", "y"), wff("!(y = 0)")));
		CHECK(tau::get(sub("!(x = 0)", "0")).equals_F());
	}

	TEST_CASE("a node built with the hooks off is rebuilt through them") {
		tref a = wff("x = 0"), b = wff("b = 0");
		// a genuine `wff_imply` node exists only with the hooks off;
		// rebuilt, it goes through the imply hook
		tref imp;
		{
			use_hooks_guard<node_t> g(false);
			imp = tau::get(tau::wff, tau::get(tau::wff_imply, a, b));
		}
		REQUIRE(tau::get(imp).child_is(tau::wff_imply));
		CHECK(tau::subtree_equals(tau::get(imp).substitute(bf("x"), bf("y")),
			build_wff_imply<node_t>(wff("y = 0"), b)));
		// without the key it is untouched, hooks or not
		CHECK(tau::get(imp).substitute(bf("q"), bf("y")) == imp);
		// an ORDER atom: the bitvector shape is kept …
		tref lt = wff("x:bv[8] & z:bv[8] <= y:bv[8]");
		tref r = tau::get(lt).substitute(bf("x:bv[8]"), bf("w:bv[8]"));
		REQUIRE(find_kind(r, tau::bf_lteq) != nullptr);
		// … and a Boolean one, built with the hooks off as the parser
		// builds it, comes back as the hooks' equation f·g′ = 0
		tref bx = tau::build_bf_variable("x", 0),
		     by = tau::build_bf_variable("y", 0),
		     bz = tau::build_bf_variable("z", 0);
		tref bl;
		{
			use_hooks_guard<node_t> g(false);
			bl = tau::build_bf_lteq(bx, by);
		}
		tref rb = tau::get(bl).substitute(bx, bz);
		CHECK(is_child<node_t>(rb, tau::bf_eq));
		CHECK(tau::subtree_equals(rb, tau::build_bf_eq_0(
			build_bf_and<node_t>(bz, build_bf_neg<node_t>(by)))));
	}

	TEST_CASE("a temporal operator is entered like any other node") {
		tref alw = tau::build_wff_always(wff("x = 0"));
		REQUIRE(is_child_temporal_quantifier<node_t>(alw));
		tref r = tau::get(alw).substitute(bf("x"), bf("y"));
		CHECK(r != alw);
		CHECK(tau::subtree_equals(r,
			tau::build_wff_always(wff("y = 0"))));
	}

	TEST_CASE("the hook's result IS the new argument") {
		tref R = ref_term(trefs{ wff("x = 0") });
		auto to_T = [](tref) { return tau::_T(); };
		tref m = tau::get(R).substitute(bf("x"), bf("y"), {}, to_T);
		tref ra = find_kind(m, tau::ref_arg);
		REQUIRE(ra != nullptr);
		CHECK(tau::get(ra)[0].equals_T());
	}

	TEST_CASE("a rebound key is left alone") {
		tref unit = wff("ex x (x & s1 = 0)");   // the bound x is "1"
		tref bx = tau::trim_right_sibling(
			tau::get(find_kind(unit, tau::wff_ex)).first());
		CHECK(tau::get(unit).substitute(tau::get(tau::bf, bx), bf("z")) == unit);
		// at a site where the key IS free, the rebinding unit rides along
		tref phi = build_wff_and<node_t>(wff("x & s2 = 0"), unit);
		tref res = tau::get(phi).substitute(bf("x"), bf("z"));
		CHECK(res != phi);
		CHECK(res == build_wff_and<node_t>(wff("z & s2 = 0"), unit));
	}

	TEST_CASE("the descent guard stops at an arithmetic operator") {
		// The key sits twice in one term: in a Boolean position and
		// under a bitvector sum.
		tref f = bf("x:bv[8] & (x:bv[8] + { 1 }:bv[8])");
		tref x = bf("x:bv[8]"), y = bf("y:bv[8]");
		// the default guard enters everything, so both occurrences go
		CHECK(tau::get(f).substitute(x, y)
			== bf("y:bv[8] & (y:bv[8] + { 1 }:bv[8])"));
		// the Boolean guard leaves the sum, and the occurrence in it,
		// as they stand
		tref kept = tau::get(f).substitute(x, y, {}, idni::identity,
			while_is_boolean_operation<node_t>);
		CHECK(kept == bf("y:bv[8] & (x:bv[8] + { 1 }:bv[8])"));
		CHECK(get_free_vars<node_t>(kept).size() == 2);
	}

	TEST_CASE("the descent guard stops at a reference, so the hook never runs") {
		tref f = build_bf_and<node_t>(bf("x"), ref_term(trefs{ bf("x") }));
		size_t calls = 0;
		auto count = [&calls](tref a) { ++calls; return a; };
		tref r = tau::get(f).substitute(bf("x"), bf("z"), {}, count,
			while_is_boolean_operation<node_t>);
		// the argument keeps its occurrence and is never re-emitted
		CHECK(calls == 0);
		CHECK(r == build_bf_and<node_t>(bf("z"),
			ref_term(trefs{ bf("x") })));
	}

	TEST_CASE("the descent guard rides into the leaves of a BDD-backed term") {
		tref p = vr("p"), b = vr("b"), z = vr("z");
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		bdd::order o {{p, 0}};
		// p·a ∪ p′·b with a and b in leaves; the key is a, which is no
		// decision variable of `o`, so the leaves are all there is to
		// rewrite
		tref f = hbdd::convert_to_tau_node(bf("pa|p'b"), o);
		REQUIRE(hbdd::is_bdd_backed(f));
		tref s = tau::get(f).substitute(bf("a"), bf("z"), o, idni::identity,
			while_is_boolean_operation<node_t>);
		REQUIRE(hbdd::is_bdd_backed(s));
		CHECK(bdd::is_ordered(hbdd::convert_to_handle(s).get(), o));
		CHECK(same_function(s, bf("pz|p'b"), { p, b, z }));
	}

	TEST_CASE("the descent guard enters the body of a functional quantifier") {
		tref t = bf("fall w (w | x)");
		CHECK(tau::subtree_equals(tau::get(t).substitute(bf("x"),
			bf("y"), {}, idni::identity,
			while_is_boolean_operation<node_t>),
			bf("fall w (w | y)")));
	}
}

TEST_SUITE("tree::substitute through a nested BDD_ID") {
	using bdd  = tau_term_bdd<node_t>;
	using hbdd = term_handle<node_t>;

	// Its own suite because it pins a `BDD_ID` inside a BDD leaf, and the
	// BDD store is never swept: the universe keeps that entry for the rest
	// of the process, which any later `U.size()` check would count. This
	// file has none, so the placement is free here.
	TEST_CASE("a backed term whose leaf holds a backed term of the same order") {
		tref x = vr("x"), p = vr("p"), c = vr("c"), z = vr("z");
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		bdd::order o {{x, 0}, {p, 1}};
		// x·c as a node of `o`, inside a reference argument, which
		// makes the whole reference a LEAF of an outer BDD over the
		// same order: x is a decision variable of BOTH and is
		// composed out of both by one call.
		tref inner = hbdd::convert_to_tau_node(bf("x & c"), o);
		REQUIRE(hbdd::is_bdd_backed(inner));
		tref f = hbdd::convert_to_tau_node(rewriter::replace<node_t>(
			bf("x & p & r(a)"), bf("a"), inner), o);
		REQUIRE(hbdd::is_bdd_backed(f));
		REQUIRE(bdd::is_ordered(hbdd::convert_to_handle(f).get(), o));
		tref s = tau::get(f).substitute(bf("x"), bf("z"), o);
		CHECK(hbdd::is_bdd_backed(s));
		const trefs& fv = get_free_vars<node_t>(s);
		CHECK(!std::binary_search(fv.begin(), fv.end(), x,
			tau::subtree_less));
		for (tref v : { p, c, z })
			CHECK(std::binary_search(fv.begin(), fv.end(), v,
				tau::subtree_less));
		// the finish reaches the nested node too
		CHECK(!has_bdd_id(hbdd::convert_to_tau_terms(s)));
	}

}
