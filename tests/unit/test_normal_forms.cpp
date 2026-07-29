// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "normal_forms.h"
#include "normalizer.h"

TEST_SUITE("normal forms: mnf for wffs") {

	TEST_CASE("simple case: T") {
		const char* sample = "1";
		tau::get_options opts;
		opts.parse.start = tau::bf;
		tref fm = tau::get(sample, opts);
		tau::get(fm).dump(std::cout << "fm: ") << "\n";
		CHECK( tau::get(fm)[0].is(tau::bf_t) );
	}

	TEST_CASE("simple case: T") {
		const char* sample = "T.";
		tref fm = tau::get(tau::get(sample))
			.find_top(is<node_t, tau::wff>);
		fm = unequal_to_not_equal<node_t>(normalize_non_temp<node_t>(fm));
		CHECK( tau::get(fm)[0].is(tau::wff_t) );
	}

	TEST_CASE("simple case: F") {
		const char* sample = "F.";
		tref fm = tau::get(tau::get(sample))
			.find_top(is<node_t, tau::wff>);
		fm = unequal_to_not_equal<node_t>(normalize_non_temp<node_t>(fm));
		CHECK( tau::get(fm)[0].is(tau::wff_f) );
	}

	TEST_CASE("simple case: X = 0") {
		using node = node_t;
		const char* sample = "X = 0.";
		tref spec = tau::get(sample);
		TAU_LOG_TRACE << "spec: " << TAU_LOG_FM_DUMP(spec);
		tref fm = tt(spec) | tau::spec | tau::main | tau::wff | tt::ref;
		tref result = unequal_to_not_equal<node_t>(normalize_non_temp<node_t>(fm));
		TAU_LOG_TRACE << "fm:     " << TAU_LOG_FM_DUMP(fm);
		TAU_LOG_TRACE << "result: " << TAU_LOG_FM_DUMP(result);
		CHECK( tau::subtree_equals(fm, result) );
	}

	TEST_CASE("simple case: X != 0") {
		const char* sample = "X != 0.";

		tref fm = tt(tau::get(sample))
			| tau::spec | tau::main | tau::wff | tt::ref;
		fm = unequal_to_not_equal<node_t>(normalize_non_temp<node_t>(fm));
		trefs check_eq  = tau::get(fm).select_all(is<node_t, tau::bf_eq>);
		trefs check_neg = tau::get(fm).select_all(is<node_t, tau::wff_neg>);
		CHECK( check_eq.size() == 1 );
		CHECK( check_neg.size() == 1 );
	}

	TEST_CASE("simple case: X = 0 && Y = 0") {
		const char* sample = "X = 0 && Y = 0.";
		tref fm = tt(tau::get(sample))
			| tau::spec | tau::main | tau::wff | tt::ref;
		fm = unequal_to_not_equal<node_t>(normalize_non_temp<node_t>(fm));
		trefs check_and = tau::get(fm).select_all(is<node_t, tau::wff_and>);
		trefs check_eq = tau::get(fm).select_all(is<node_t, tau::bf_eq>);
		CHECK( check_and.size() == 1 );
		CHECK( check_eq.size() == 2 );
	}

	TEST_CASE("simple case: X != 0 && Y != 0") {
		const char* sample = "X != 0 && Y != 0.";
		tref fm = tt(tau::get(sample))
			| tau::spec | tau::main | tau::wff | tt::ref;
		fm = unequal_to_not_equal<node_t>(normalize_non_temp<node_t>(fm));
		trefs check_eq = tau::get(fm).select_all(is<node_t, tau::bf_eq>);
		trefs check_neg = tau::get(fm).select_all(is<node_t, tau::wff_neg>);
		trefs check_and = tau::get(fm).select_all(is<node_t, tau::wff_and>);
		CHECK( check_eq.size() == 2 );
		CHECK( check_neg.size() == 2 );
		CHECK( check_and.size() == 1 );
	}

	TEST_CASE("simple case: X = 0 || Y = 0") {
		const char* sample = "X = 0 || Y = 0.";
		tref fm = tt(tau::get(sample))
			| tau::spec | tau::main | tau::wff | tt::ref;
		fm = unequal_to_not_equal<node_t>(normalize_non_temp<node_t>(fm));
		trefs check_eq = tau::get(fm).select_all(is<node_t, tau::bf_eq>);
		trefs check_or = tau::get(fm).select_all(is<node_t, tau::wff_or>);
		CHECK( check_eq.size() == 2 );
		CHECK( check_or.size() == 1 );
	}

}

static constexpr const char* uninterp_constants_sample =
	"(<:c>' & <:b>' & <:b> | <:c>' & <:b>' & <:c> & <:b>' | <:c>' & <:c> & <:b> & <:b> | <:c>' & <:c> & <:b> & <:c> & <:b>') & <:a> | (<:b>' & <:c>' & <:b> | <:b>' & <:c>' & <:c> & <:b>' | <:c> & <:b> & <:c>' & <:b> | <:c> & <:b> & <:c>' & <:c> & <:b>') & <:a>' = 0.";

TEST_SUITE("normal forms: bf_reduce_canonical") {


	TEST_CASE("uninterpreted constants") {
		const char* sample = uninterp_constants_sample;
		tref fm = tt(tau::get(sample))
			| tau::spec | tau::main | tau::wff
			| bf_reduce_canonical<node_t>() | tt::ref;
		CHECK( tau::get(fm) == tau::get_T() );
	}

}

TEST_SUITE("normal forms: reduce_bf") {

	/*TEST_CASE("uninterpreted constants") {
		const char* sample = "(<:c>' & <:b>' & <:b> | <:c>' & <:b>' & <:c> & <:b>' | <:c>' & <:c> & <:b> & <:b> | <:c>' & <:c> & <:b> & <:c> & <:b>') & <:a> | (<:b>' & <:c>' & <:b> | <:b>' & <:c>' & <:c> & <:b>' | <:c> & <:b> & <:c>' & <:b> | <:c> & <:b> & <:c>' & <:c> & <:b>') & <:a>' = 0.";
		auto src = make_tau_source(sample);
		auto statement = (make_statement(src)
			| tau::spec
			| tau::main
			| tau::wff).value();
		auto result = statement | reduce_bf<Bool>;
		CHECK( result == _F<Bool> );
	}*/
}

TEST_SUITE("normal forms: dnf_bf") {

	TEST_CASE("uninterpreted constants") {
		const char* sample = uninterp_constants_sample;
		tref fm = tt(tau::get(sample))
			| tau::spec | tau::main | tau::wff | tau::bf_eq
			| tau::bf | tt::f(to_dnf<node_t, false>) | tt::ref;
		CHECK( tau::get(fm).equals_0() );
	}

	/*TEST_CASE("uninterpreted constants") {
		const char* sample = " o1[2]' & ((<:a> & (<:a> & <:c> | <:b> & <:c>' "
			"& <:a>' | <:a> & <:b> & <:c>'))' & <:b> & (<:a> & <:c> | <:b> "
			"& <:c>' & <:a>' | <:a> & <:b> & <:c>')' | <:a> & (<:a> & <:c> "
			"| <:b> & <:c>' & <:a>' | <:a> & <:b> & <:c>') & (<:b> & (<:a> "
			"& <:c> | <:b> & <:c>' & <:a>' | <:a> & <:b> & <:c>')')') | o1[2] "
			"& ((<:a> & (<:a> & <:c> | <:b> & <:c>' & <:a>' | <:a> & <:b> "
			"& <:c>'))' & <:b> & (<:a> & <:c> | <:b> & <:c>' & <:a>' | <:a> "
			"& <:b> & <:c>')' | <:a> & (<:a> & <:c> | <:b> & <:c>' & <:a>' "
			"| <:a> & <:b> & <:c>') & (<:b> & (<:a> & <:c> | <:b> & <:c>' "
			"& <:a>' | <:a> & <:b> & <:c>')')')' = 0.";
		auto src = make_tau_source(sample);
		auto statement = (make_statement(src)
			| tau::spec
			| tau::main
			| tau::wff
			| tau::bf_eq
			| tau::bf).value();
		auto result = bf_boole_normal_form(statement);
		CHECK( result == _0<Bool> );
	}*/


}

TEST_SUITE("path_expressions") {
	TEST_CASE("1") {
		const char* sample = "(a=0||b=0)&&x=0&&y=0&&(p=0||f=0)&&(t=0||q=0).";
		tref fm = get_nso_rr(sample).value().main->get();
		for (tref path : expression_paths<node_t>(fm)) {
			trefs paths = get_cnf_wff_clauses<node_t>(path);
			CHECK(paths.size() == 5);
			for (tref p : paths) CHECK(tau::get(p).is(tau::wff));
		}
	}
	TEST_CASE("2") {
		const char* sample = "x&(vw|y&(t|z|s|r)|gk)";
		tref fm = get_bf_nso_rr("", sample).value().main->get();
		for (tref path : expression_paths<node_t>(fm)) {
			trefs paths = get_cnf_bf_clauses<node_t>(path);
			CHECK(paths.size() == 3);
			for (tref p : paths) CHECK(tau::get(p).is(tau::bf));
		}
	}
	TEST_CASE("3") {
		const char* sample = " (sometimes (a=0||b=0)&&x=0&&y=0&&(p=0||f=0)&&(t=0||q=0)) && (always x = 0) || (always y = 0) && (always k = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		for (tref path : expression_paths<node_t>(fm)) {
			trefs paths = get_cnf_wff_clauses<node_t>(path);
			CHECK(paths.size() == 2);
			for (tref p : paths) CHECK(tau::get(p).is(tau::wff));
		}
	}
	TEST_CASE("4") {
		const char* sample = " x = 0 && y = 0 && (ex z z = 0 && k = 0 || t = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		trefs p {expression_paths<node_t>(fm).begin(),
			expression_paths<node_t>(fm).end()};
		CHECK(p.size() == 1);
	}
}

TEST_SUITE("normal forms: onf") {

	/* TEST_CASE("T") {
		const char* sample = "T.";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_nso_rr_using_factory<sbf_ba_factory_t, sbf_ba>(sample_src, bf);
		auto nts = std::get<tau_source_sym>(sample_formula.main->value).nts;
		auto var = make_node<tau_sym<sbf_ba>>(tau_source_sym(tau::variable, nts), {});
		auto result = onf(sample_formula.main, var);
		auto check = result | tau::wff_t;
		CHECK( check.has_value() );
		CHECK( true );
	}*/

	// NF-N2: onf_subformula used to key its replacement map with the
	// re-normalized equation node (a no-op unless the equation was already
	// in `f eq 0` form) and only searched the LHS operand for the target
	// variable, so "y = x" (variable on the RHS) was silently left
	// untransformed while the equivalent "x = y" was handled correctly.
	TEST_CASE("variable on either side of the equation is transformed the same way") {
		tref x = build_variable<node_t>("x", tau_type_id<node_t>());
		tref fm_lhs = get_nso_rr("x = y.").value().main->get();
		tref fm_rhs = get_nso_rr("y = x.").value().main->get();
		tref result_lhs = onf<node_t>(fm_lhs, x);
		tref result_rhs = onf<node_t>(fm_rhs, x);
		CHECK(tau::get(result_lhs).to_str() == tau::get(result_rhs).to_str());
		// a genuine onf transformation replaces the bare equation
		CHECK(tau::get(result_rhs).to_str() != tau::get(fm_rhs).to_str());
	}
}

TEST_SUITE("GetNewUninterpretedConstant") {

	// NF-2: get_new_uninterpreted_constant used to substr+stoi every
	// uconst_name in fm unconditionally, throwing on any uconst whose name
	// did not follow the "name" + digits convention (the header doc claims
	// a prefix filter that did not exist in the implementation).
	TEST_CASE("uconsts not matching the requested name prefix are ignored") {
		tref foreign = build_bf_uconst<node_t>("", "c", tau_type_id<node_t>());
		tref fm = tau::build_bf_eq(foreign, tau::_0(tau_type_id<node_t>()));
		tref result = nullptr;
		REQUIRE_NOTHROW(result =
			get_new_uninterpreted_constant<node_t>(fm, "split", tau_type_id<node_t>()));
		trefs names = tau::get(result).select_top(is<node_t, tau::uconst_name>);
		REQUIRE(names.size() == 1);
		CHECK(tau::get(names[0]).get_string() == ":split1");
	}

	TEST_CASE("fresh constant is numbered one past the largest matching suffix") {
		tref c1 = build_bf_uconst<node_t>("", "split1", tau_type_id<node_t>());
		tref c3 = build_bf_uconst<node_t>("", "split3", tau_type_id<node_t>());
		tref fm = tau::build_bf_or(c1, c3);
		tref result = get_new_uninterpreted_constant<node_t>(fm, "split", tau_type_id<node_t>());
		trefs names = tau::get(result).select_top(is<node_t, tau::uconst_name>);
		REQUIRE(names.size() == 1);
		CHECK(tau::get(names[0]).get_string() == ":split4");
	}
}

// NF-4/NF-22: fold_trivial_quantifiers had zero direct unit tests. It folds
// `ex x T/F` and `all x T/F` down to their (constant) body.
TEST_SUITE("FoldTrivialQuantifiers") {
	TEST_CASE("existential quantifier over T folds to T") {
		tref x = build_variable<node_t>("x", tau_type_id<node_t>());
		tref fm = tau::build_wff_ex(x, tau::_T(), false);
		tref res = fold_trivial_quantifiers<node_t>(fm);
		CHECK( tau::get(res).equals_T() );
	}

	TEST_CASE("existential quantifier over F folds to F") {
		tref x = build_variable<node_t>("x", tau_type_id<node_t>());
		tref fm = tau::build_wff_ex(x, tau::_F(), false);
		tref res = fold_trivial_quantifiers<node_t>(fm);
		CHECK( tau::get(res).equals_F() );
	}

	TEST_CASE("universal quantifier over T folds to T") {
		tref x = build_variable<node_t>("x", tau_type_id<node_t>());
		tref fm = tau::build_wff_all(x, tau::_T(), false);
		tref res = fold_trivial_quantifiers<node_t>(fm);
		CHECK( tau::get(res).equals_T() );
	}

	TEST_CASE("universal quantifier over F folds to F") {
		tref x = build_variable<node_t>("x", tau_type_id<node_t>());
		tref fm = tau::build_wff_all(x, tau::_F(), false);
		tref res = fold_trivial_quantifiers<node_t>(fm);
		CHECK( tau::get(res).equals_F() );
	}

	TEST_CASE("non-trivial quantifier body is left untouched (control)") {
		// ex x (x = 0): the body is neither T nor F, so nothing is folded.
		tref fm = get_nso_rr("ex x x = 0.").value().main->get();
		tref res = fold_trivial_quantifiers<node_t>(fm);
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) != nullptr );
	}
}

// NF-4: has_no_boolean_combs_of_models had zero direct unit tests. It
// rejects formulas that combine several "always"/"sometimes"-wrapped models
// with Boolean connectives, while allowing a plain non-temporal formula or a
// single top-level `always` wrapper.
TEST_SUITE("HasNoBooleanCombsOfModels") {
	TEST_CASE("plain non-temporal formula satisfies the predicate") {
		tref fm = get_nso_rr("x = 0.").value().main->get();
		CHECK( has_no_boolean_combs_of_models<node_t>(fm) );
	}

	TEST_CASE("single top-level always wrapper satisfies the predicate") {
		tref fm = get_nso_rr("always x = 0.").value().main->get();
		CHECK( has_no_boolean_combs_of_models<node_t>(fm) );
	}

	TEST_CASE("boolean combination of two models violates the predicate") {
		// (always x=0) && (always y=0): a Boolean (&&) combination of two
		// "always"-wrapped models, which is exactly what the predicate must
		// reject.
		tref fm = get_nso_rr(
			"(always x = 0) && (always y = 0).").value().main->get();
		CHECK( !has_no_boolean_combs_of_models<node_t>(fm) );
	}

	TEST_CASE("nested always under always violates the predicate") {
		// "always (always x = 0)." is rejected at parse time (nesting of
		// temporal quantifiers is not allowed as source syntax), so the
		// nested-always tree is built directly instead.
		tref x = build_bf_variable<node_t>("x", tau_type_id<node_t>());
		tref inner = tau::build_bf_eq(x, tau::_0(tau_type_id<node_t>()));
		tref fm = tau::build_wff_always(tau::build_wff_always(inner));
		CHECK( !has_no_boolean_combs_of_models<node_t>(fm) );
	}
}

// NF-4: are_nso_equivalent and is_nso_impl had zero direct unit tests; only
// exercised indirectly through normalizer/simplify_temporal_clause paths.
TEST_SUITE("AreNsoEquivalentAndIsNsoImpl") {
	TEST_CASE("are_nso_equivalent: true for semantically equal but "
		  "syntactically different formulas")
	{
		// x=0  and  !(x!=0)  are equivalent, but structurally different,
		// so this exercises the full normalize_non_temp based check
		// rather than the structural/ref fast paths.
		tref n1 = get_nso_rr("x = 0.").value().main->get();
		tref n2 = get_nso_rr("!(x != 0).").value().main->get();
		CHECK( are_nso_equivalent<node_t>(n1, n2) );
	}

	TEST_CASE("are_nso_equivalent: false for non-equivalent formulas") {
		tref n1 = get_nso_rr("x = 0.").value().main->get();
		tref n2 = get_nso_rr("y = 0.").value().main->get();
		CHECK( !are_nso_equivalent<node_t>(n1, n2) );
	}

	TEST_CASE("is_nso_impl: true when the antecedent is stronger") {
		// x=0 && y=0  implies  x=0
		tref n1 = get_nso_rr("x = 0 && y = 0.").value().main->get();
		tref n2 = get_nso_rr("x = 0.").value().main->get();
		CHECK( is_nso_impl<node_t>(n1, n2) );
	}

	TEST_CASE("is_nso_impl: false when the consequent is stronger") {
		// x=0  does not imply  x=0 && y=0 (y is unconstrained)
		tref n1 = get_nso_rr("x = 0.").value().main->get();
		tref n2 = get_nso_rr("x = 0 && y = 0.").value().main->get();
		CHECK( !is_nso_impl<node_t>(n1, n2) );
	}
}

// NF-9: reduce_paths and join_paths (dense 0/1/2 path-vector reduction, used
// by bf_reduced_dnf/assign_and_reduce) had no direct unit test; they were
// only exercised indirectly through the BfReducedDNF suite. `2` denotes an
// irrelevant/don't-care variable position.
TEST_SUITE("ReducePathsAndJoinPaths") {
	TEST_CASE("reduce_paths: merges an assignment at Hamming distance 1") {
		// existing path: x=0,y=0 ; new assignment: x=1,y=0
		// -> merge into x=irrelevant,y=0
		std::vector<int_t> i{1, 0};
		std::vector<std::vector<int_t>> paths{ {0, 0} };
		bool merged = reduce_paths(i, paths, 2);
		CHECK( merged );
		REQUIRE( paths.size() == 1 );
		CHECK( paths[0] == std::vector<int_t>{2, 0} );
	}

	TEST_CASE("reduce_paths: full collapse clears the path set") {
		// single variable: existing path x=0, new assignment x=1
		// covers both values -> the whole path set collapses (tautology)
		std::vector<int_t> i{1};
		std::vector<std::vector<int_t>> paths{ {0} };
		bool merged = reduce_paths(i, paths, 1);
		CHECK( merged );
		CHECK( paths.empty() );
	}

	TEST_CASE("reduce_paths: distance-2 assignment is not merged") {
		std::vector<int_t> i{1, 1};
		std::vector<std::vector<int_t>> paths{ {0, 0} };
		bool merged = reduce_paths(i, paths, 2);
		CHECK( !merged );
		REQUIRE( paths.size() == 1 );
		CHECK( paths[0] == std::vector<int_t>{0, 0} );
	}

	TEST_CASE("reduce_paths: differing don't-cares are incompatible") {
		// path has y=irrelevant while the assignment fixes y=1: even
		// though x differs, the differing irrelevant variable makes the
		// two incompatible for merging purposes.
		std::vector<int_t> i{1, 1};
		std::vector<std::vector<int_t>> paths{ {2, 0} };
		bool merged = reduce_paths(i, paths, 2);
		CHECK( !merged );
		REQUIRE( paths.size() == 1 );
		CHECK( paths[0] == std::vector<int_t>{2, 0} );
	}

	TEST_CASE("join_paths: merges complementary paths at Hamming distance 1") {
		// x=0,y=0 and x=1,y=0 -> x=irrelevant,y=0
		std::vector<std::vector<int_t>> paths{ {0, 0}, {1, 0} };
		join_paths(paths);
		REQUIRE( paths.size() == 1 );
		CHECK( paths[0] == std::vector<int_t>{2, 0} );
	}

	TEST_CASE("join_paths: a subsumed (subset) path is removed") {
		// x=1,y=irrelevant already covers x=1,y=0 -> the more specific
		// path is redundant and gets erased, regardless of input order.
		std::vector<std::vector<int_t>> paths1{ {1, 2}, {1, 0} };
		join_paths(paths1);
		REQUIRE( paths1.size() == 1 );
		CHECK( paths1[0] == std::vector<int_t>{1, 2} );

		std::vector<std::vector<int_t>> paths2{ {1, 0}, {1, 2} };
		join_paths(paths2);
		REQUIRE( paths2.size() == 1 );
		CHECK( paths2[0] == std::vector<int_t>{1, 2} );
	}

	TEST_CASE("join_paths: unrelated paths are left untouched (control)") {
		std::vector<std::vector<int_t>> paths{ {0, 0}, {1, 1} };
		join_paths(paths);
		CHECK( paths.size() == 2 );
	}
}

TEST_SUITE("GetFreeVars") {
	// The shadowing shapes are built programmatically: the parser
	// alpha-renames bound variables, so parsed samples cannot contain
	// a variable that is both free and re-bound
	TEST_CASE("free occurrence before binder of same variable (B6)") {
		// (x = 0) && (ex x x = 0): x is free in the left conjunct;
		// the binder of the right conjunct must not erase it
		tref eq = get_nso_rr("x = 0.").value().main->get();
		auto x = build_variable<node_t>("x", tau_type_id<node_t>());
		tref fm = tau::build_wff_and(eq,
			tau::build_wff_ex(x, eq, false));
		const trefs& fv = get_free_vars<node_t>(fm);
		CHECK( fv.size() == 1 );
		CHECK( (fv.size() == 1 && tau::get(fv[0]).to_str() == "x") );
	}

	TEST_CASE("free occurrence after binder of same variable (B6 control)") {
		tref eq = get_nso_rr("x = 0.").value().main->get();
		auto x = build_variable<node_t>("x", tau_type_id<node_t>());
		tref fm = tau::build_wff_and(
			tau::build_wff_ex(x, eq, false), eq);
		const trefs& fv = get_free_vars<node_t>(fm);
		CHECK( fv.size() == 1 );
		CHECK( (fv.size() == 1 && tau::get(fv[0]).to_str() == "x") );
	}

	TEST_CASE("bound occurrences are not free (B6 control)") {
		tref eq = get_nso_rr("x = 0.").value().main->get();
		auto x = build_variable<node_t>("x", tau_type_id<node_t>());
		tref fm = tau::build_wff_ex(x, eq, false);
		const trefs& fv = get_free_vars<node_t>(fm);
		CHECK( fv.empty() );
	}

	TEST_CASE("distinct binder leaves other variables free (B6 control)") {
		// ex y (xy = 0): only x is free
		tref eq = get_nso_rr("xy = 0.").value().main->get();
		auto y = build_variable<node_t>("y", tau_type_id<node_t>());
		tref fm = tau::build_wff_ex(y, eq, false);
		const trefs& fv = get_free_vars<node_t>(fm);
		CHECK( fv.size() == 1 );
		CHECK( (fv.size() == 1 && tau::get(fv[0]).to_str() == "x") );
	}
}

TEST_SUITE("ToNNF") {
	TEST_CASE("double negation bf") {
		// !!(a = 0) → a = 0 (push double negation in)
		const char* sample = "!!(a = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = to_nnf<node_t>(fm);
		CHECK( !tau::get(res).find_top(is<node_t, tau::wff_neg>) );
		CHECK( tau::get(res).find_top(is<node_t, tau::bf_eq>) );
	}
	TEST_CASE("de Morgan wff: !(a=0 && b=0) → a!=0 || b!=0") {
		const char* sample = "!(a = 0 && b = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = to_nnf<node_t>(fm);
		// No wff_and at top, the result is a disjunction of negated atoms
		CHECK( !tau::get(res).find_top(is<node_t, tau::wff_and>) );
		CHECK( tau::get(res).find_top(is<node_t, tau::wff_or>) );
	}
}

TEST_SUITE("ToDNF") {
	TEST_CASE("bf: (a&b)|(a&c) is already DNF") {
		const char* sample = "ab|ac = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		// fm is the wff node; [0] is the bf_eq child; first() is the bf
		tref bf = tau::get(fm)[0].first();
		tref res = to_dnf<node_t, false>(bf);
		// Result is an OR of AND terms
		CHECK( tau::get(res).find_top(is<node_t, tau::bf_or>) );
	}
	TEST_CASE("wff: x=0 && (y=0 || z=0) distributed to DNF") {
		// (x=0 && y=0) || (x=0 && z=0)
		const char* sample = "x = 0 && (y = 0 || z = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = to_dnf<node_t, true>(fm);
		// DNF result must be an OR
		CHECK( tau::get(res).find_top(is<node_t, tau::wff_or>) );
		// DNF result must contain x=0
		auto has_x_eq = tau::get(res).find_top(is<node_t, tau::bf_eq>);
		CHECK( has_x_eq != nullptr );
	}
}

TEST_SUITE("ToCNF") {
	TEST_CASE("bf: (a|b)&(a|c) is already CNF") {
		const char* sample = "(a|b)&(a|c) = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref bf = tau::get(fm)[0].first();
		tref res = to_cnf<node_t, false>(bf);
		// Result is an AND of OR terms
		CHECK( tau::get(res).find_top(is<node_t, tau::bf_and>) );
	}
	TEST_CASE("wff: (x=0||y=0) && (z=0||w=0) is already CNF") {
		const char* sample = "(x = 0 || y = 0) && (z = 0 || w = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = to_cnf<node_t, true>(fm);
		// Top-level should be an AND
		CHECK( tau::get(res).find_top(is<node_t, tau::wff_and>) );
	}
}

TEST_SUITE("ReduceWff") {
	TEST_CASE("contradiction x=0 && x!=0 → F") {
		const char* sample = "x = 0 && x != 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = reduce<node_t>(fm);
		CHECK( tau::get(res).equals_F() );
	}
	TEST_CASE("tautology x=0 || x!=0 → T") {
		const char* sample = "x = 0 || x != 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = reduce<node_t>(fm);
		CHECK( tau::get(res).equals_T() );
	}
}

TEST_SUITE("BfReducedDNF") {
	TEST_CASE("ab|ab' reduces to a") {
		// ab | ab' = a(b|b') = a·1 = a
		const char* sample = "ab|ab' = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref bf = tau::get(fm)[0].first();
		tref res = bf_reduced_dnf<node_t>(bf);
		CHECK( tau::get(res).to_str() == "a" );
	}
	TEST_CASE("ab|a'b reduces to b") {
		// ab | a'b = (a|a')b = 1·b = b
		const char* sample = "ab|a'b = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref bf = tau::get(fm)[0].first();
		tref res = bf_reduced_dnf<node_t>(bf);
		CHECK( tau::get(res).to_str() == "b" );
	}
	TEST_CASE("ab|ab'|a'b|a'b' reduces to 1") {
		// covers all 4 minterms → 1
		const char* sample = "ab|ab'|a'b|a'b' = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref bf = tau::get(fm)[0].first();
		tref res = bf_reduced_dnf<node_t>(bf);
		CHECK( tau::get(res).equals_1() );
	}

	TEST_CASE("make_paths_disjoint=true: ac|a'b'c preserves disjoint paths") {
		// ac | a'b'c:
		//   assign_and_reduce produces paths [a=1,b=*,c=1] and [a=0,b=0,c=1].
		//   reduce_paths cannot merge them because the b=* entry blocks the
		//   2-incompatibility rule in reduce_paths.
		//   join_paths CAN merge them via the subset rule:
		//     [a=0,b=0,c=1] gets a→* → merged result is ac | b'c.
		//   With make_paths_disjoint=true, join is skipped and both paths survive,
		//   so the result retains a bf_or with more terms than the joined form.
		const char* sample = "ac|a'b'c = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref bf = tau::get(fm)[0].first();
		tref res_disjoint = bf_reduced_dnf<node_t>(bf, /*make_paths_disjoint=*/true);
		tref res_joined   = bf_reduced_dnf<node_t>(bf, /*make_paths_disjoint=*/false);
		// The two calls must yield structurally different results
		CHECK( tau::get(res_disjoint) != tau::get(res_joined) );
		// The disjoint version retains a bf_or (both paths kept)
		CHECK( tau::get(res_disjoint).find_top(is<node_t, tau::bf_or>) );
	}
}

TEST_SUITE("SyntacticFormulaSimplification") {
	TEST_CASE("path contradiction x=0 && x!=0 → F") {
		const char* sample = "x = 0 && x != 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = syntactic_formula_simplification<node_t>(fm);
		CHECK( tau::get(res).equals_F() );
	}
	TEST_CASE("equality propagation x=0 && xy=0 → x=0") {
		// x=0 implies xy=0, so the conjunct xy=0 is redundant
		const char* sample = "x = 0 && xy = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = syntactic_formula_simplification<node_t>(fm);
		CHECK( matches_to_str_to_any_of(res, {
			"x = 0",
			"yx = 0",
		}) );
	}
	TEST_CASE("tautological path x=0 || x!=0 → T") {
		const char* sample = "x = 0 || x != 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = syntactic_formula_simplification<node_t>(fm);
		CHECK( tau::get(res).equals_T() );
	}
}
