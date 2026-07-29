// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "ref_variables_resolver.h"

TEST_SUITE("ref_variables_resolver") {

	TEST_CASE("insert declares in current scope, kind_of defaults to unused") {
		ref_variables_resolver<node_t> r;
		tref x = tau::build_variable("x", untyped_type_id<node_t>());
		r.insert(x, ref_variable_kind::unused);
		CHECK(r.kind_of(x) == ref_variable_kind::unused);
	}

	TEST_CASE("kind_of on a never-inserted node defaults to unused") {
		ref_variables_resolver<node_t> r;
		tref x = tau::build_variable("x", untyped_type_id<node_t>());
		CHECK(r.kind_of(x) == ref_variable_kind::unused);
	}

	TEST_CASE("assign joins kinds: unused then used becomes used") {
		ref_variables_resolver<node_t> r;
		tref x = tau::build_variable("x", untyped_type_id<node_t>());
		r.assign(x, ref_variable_kind::unused);
		r.assign(x, ref_variable_kind::used);
		CHECK(r.kind_of(x) == ref_variable_kind::used);
	}

	TEST_CASE("assign is monotone: used then unused stays used") {
		// `used` is the top element -- joining `unused` into it must not
		// demote it, or use could be lost depending on visit order.
		ref_variables_resolver<node_t> r;
		tref x = tau::build_variable("x", untyped_type_id<node_t>());
		r.assign(x, ref_variable_kind::used);
		r.assign(x, ref_variable_kind::unused);
		CHECK(r.kind_of(x) == ref_variable_kind::used);
	}

	TEST_CASE("merge joins two nodes' kinds and unifies their sets") {
		ref_variables_resolver<node_t> r;
		tref x = tau::build_variable("x", untyped_type_id<node_t>());
		tref y = tau::build_variable("y", untyped_type_id<node_t>());
		r.assign(x, ref_variable_kind::unused);
		r.assign(y, ref_variable_kind::used);
		r.merge(x, y);
		CHECK(r.kind_of(x) == ref_variable_kind::used);
		CHECK(r.kind_of(y) == ref_variable_kind::used);
	}

	TEST_CASE("open/close isolate a same-named variable across sibling scopes") {
		ref_variables_resolver<node_t> r;
		tref x1 = tau::build_variable("x", untyped_type_id<node_t>());
		r.open();
		r.insert(x1, ref_variable_kind::unused);
		r.assign(x1, ref_variable_kind::used);
		r.close();

		tref x2 = tau::build_variable("x", untyped_type_id<node_t>());
		r.open();
		r.insert(x2, ref_variable_kind::unused);
		CHECK(r.kind_of(x2) == ref_variable_kind::unused);
		r.close();
	}
}

namespace {

tref parse_wff(const std::string& spec) {
	tau::get_options opts{ .parse = { .start = tau::wff } };
	return tau::get(spec, opts);
}

bool is_used(tref formula, const std::string& var_name) {
	// select_all (not get_free_vars) so this also finds variables bound
	// by a quantifier, not just genuinely free ones.
	auto skip = make_ref_variables_skip<node_t>(formula);
	for (tref v : tau::get(formula).select_all(
		(bool(*)(tref)) is_var_or_capture<node_t>))
		if (get_var_name<node_t>(v) == var_name)
			return skip(v);
	FAIL("variable not found in formula: " << var_name);
	return false;
}

} // namespace

TEST_SUITE("make_ref_variables_skip") {

	TEST_CASE("a predicate's argument variables are used") {
		const std::string sample = "q(x, y)";
		tref fm = parse_wff(sample);
		CHECK(is_used(fm, "x"));
		CHECK(is_used(fm, "y"));
	}

	TEST_CASE("a ref-free formula has no used variables") {
		const std::string sample = "x = y";
		tref fm = parse_wff(sample);
		CHECK_FALSE(is_used(fm, "x"));
		CHECK_FALSE(is_used(fm, "y"));
	}

	TEST_CASE("use propagates through a shared atom to a non-argument variable") {
		// The motivating case for the union-find: y is not an argument of
		// q, but it shares the atom x = y with q's argument x, so it
		// cannot be eliminated independently either.
		const std::string sample = "q(x) && x = y";
		tref fm = parse_wff(sample);
		CHECK(is_used(fm, "x"));
		CHECK(is_used(fm, "y"));
	}

	TEST_CASE("a variable in an atom disjoint from every ref stays unused") {
		// z shares no atom with q's argument, so it remains eliminable.
		const std::string sample = "q(x) && z = 0";
		tref fm = parse_wff(sample);
		CHECK(is_used(fm, "x"));
		CHECK_FALSE(is_used(fm, "z"));
	}

	TEST_CASE("collect_used_ref_variables reports the atom as well as the variables") {
		// The set is keyed on both variable and atomic-formula nodes, so
		// a caller can skip either.
		const std::string sample = "q(x) && x = y";
		tref fm = parse_wff(sample);
		auto used = collect_used_ref_variables<node_t>(fm);
		CHECK(!used.empty());
		auto skip = make_ref_variables_skip<node_t>(fm);
		bool some_atom_used = false;
		for (tref atom : tau::get(fm).select_top(is_atomic_fm<node_t>))
			if (skip(atom)) some_atom_used = true;
		CHECK(some_atom_used);
	}

	TEST_CASE("unrelated sibling quantifiers with the same variable name don't cross-contaminate") {
		// The `x` bound by the second `ex x` is a structurally identical
		// but logically unrelated variable to the first block's `x`.
		// Only the first block's `x` reaches a predicate, so only that
		// block's atom may come out used.
		//
		// Counted over atoms, not variables: Tau's parser canonically
		// renames quantifier-bound variables, so both blocks' `x`
		// collapse to the same tref and a tref-keyed set cannot tell
		// them apart. The scoped union-find still keeps the two scopes
		// separate, which is what the atom counts observe.
		const std::string sample =
			"(ex x (q(x) && x = y)) && (ex x (x = 0))";
		tref fm = parse_wff(sample);
		auto skip = make_ref_variables_skip<node_t>(fm);
		int used_atoms = 0, unused_atoms = 0;
		for (tref atom : tau::get(fm).select_top(is_atomic_fm<node_t>))
			skip(atom) ? ++used_atoms : ++unused_atoms;
		CHECK(used_atoms == 1);
		CHECK(unused_atoms == 1);
	}

	TEST_CASE("use inside a nested quantifier surfaces on an outer-scoped variable") {
		// The outer-bound variable shares an atom with the inner ref's
		// argument. Checked structurally, not by source name: Tau's
		// parser canonically renames quantifier-bound variables (e.g.
		// to "b1"/"b2"), so the original spelling doesn't survive.
		const std::string sample = "ex w ex y (q(y) && w = y)";
		tref fm = parse_wff(sample);
		auto skip = make_ref_variables_skip<node_t>(fm);
		auto vars = tau::get(fm).select_all(
			(bool(*)(tref)) is_var_or_capture<node_t>);
		REQUIRE(vars.size() == 2);
		for (tref v : vars) CHECK(skip(v));
	}
}
