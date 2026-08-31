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
		const char* sample = " (F (a=0||b=0)&&x=0&&y=0&&(p=0||f=0)&&(t=0||q=0)) && (G x = 0) || (G y = 0) && (G k = 0).";
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

TEST_SUITE("get_leaves") {

	// Distinct atoms: the wff_or hook folds `A || A` into `A`, so a chain
	// of one repeated atom would collapse to a single leaf.
	static tref atom_n(size_t i) {
		return tau::build_bf_eq_0(
			tau::build_bf_variable("v" + std::to_string(i), 0));
	}

	TEST_CASE("left-deep or-spine of 300k clauses does not overflow the stack") {
		// The or-spine of a DNF is a left-deep binary chain, so a recursive
		// flatten needs one frame per clause (nomic ratchet / GitHub #90).
		const size_t n = 300000;
		tref first = atom_n(0);
		tref fm = first;
		for (size_t i = 1; i < n; ++i) fm = tau::build_wff_or(fm, atom_n(i));
		trefs leaves = get_dnf_wff_clauses<node_t>(fm);
		CHECK(leaves.size() == n);
		// Leaves are the subtrees as they sit in the spine (sibling links
		// included), so compare trees, not trefs.
		CHECK(tau::get(leaves.front()) == tau::get(first));
		CHECK(tau::get(leaves.back()) == tau::get(atom_n(n - 1)));
	}

	TEST_CASE("shared subtrees keep their multiplicity and order") {
		// (a || b) || (b || c)  ->  [a, b, b, c]: b is reached through two
		// parents and must be spliced in twice, in spine order.
		tref a = atom_n(0), b = atom_n(1), c = atom_n(2);
		tref fm = tau::build_wff_or(tau::build_wff_or(a, b),
			tau::build_wff_or(b, c));
		trefs leaves = get_dnf_wff_clauses<node_t>(fm);
		REQUIRE(leaves.size() == 4);
		auto same = [](tref l, tref r) { return tau::get(l) == tau::get(r); };
		CHECK(same(leaves[0], a)); CHECK(same(leaves[1], b));
		CHECK(same(leaves[2], b)); CHECK(same(leaves[3], c));
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

	// NF-5: onf_wff::operator() rewrote the body returned by
	// get_inner_quantified_wff and returned *only* that body, dropping the
	// binder and everything above it -- so `onf` on a quantified formula
	// silently returned a formula with a different meaning.  The existing
	// tests were all quantifier-free, so the branch was uncovered.
	TEST_CASE("onf keeps the quantifier prefix") {
		tref x = build_variable<node_t>("x", tau_type_id<node_t>());
		tref fm = get_nso_rr("ex y (x = y).").value().main->get();
		REQUIRE( tau::get(fm).find_top(is<node_t, tau::wff_ex>) != nullptr );
		tref result = onf<node_t>(fm, x);
		CHECK( tau::get(result).find_top(is<node_t, tau::wff_ex>) != nullptr );
	// The bf_neq loop of onf_subformula: a `!=` atom mentioning the onf
	// variable is rebuilt as the order-constraint pair
	// bf_nlteq(f_0, x) || bf_nlteq(x, f_1), which the downstream
	// normalization inside onf then reduces to the cofactor clauses
	// xy' != 0 || x'y != 0 (conjunct order is interning-order dependent,
	// so both orders are accepted).
	TEST_CASE("bf_neq atoms mentioning the variable are order-normalized") {
		tref x = build_variable<node_t>("x", tau_type_id<node_t>());
		tref fm = get_nso_rr("x != y.").value().main->get();
		tref result = onf<node_t>(fm, x);
		std::string s = tau::get(result).to_str();
		CHECK(( s == "yx' != 0 || xy' != 0"
			|| s == "xy' != 0 || yx' != 0" ));
	}

	// A bf_neq NOT mentioning the variable is skipped (the `continue`
	// guard at the top of the loop), while one mentioning it in the same
	// formula is still transformed.
	TEST_CASE("bf_neq atoms without the variable are left untouched") {
		tref x = build_variable<node_t>("x", tau_type_id<node_t>());
		tref fm = get_nso_rr("x != y && z != w.").value().main->get();
		tref result = onf<node_t>(fm, x);
		std::string s = tau::get(result).to_str();
		// z != w survives untouched, x != y is decomposed away
		CHECK( s.find("z != w") != std::string::npos );
		CHECK( s.find("x != y") == std::string::npos );
	}
}

TEST_SUITE("normal forms: atm_formula_order_for_simplification") {

	// The comparator behind boole_normal_form's stable_sort of BDD atoms.
	// Priority: (1) initial-time atoms before lookback atoms, lowest
	// initial first; (2) among lookback atoms, higher lookback (max
	// shift) first, then lowest high time point... then (3) fewer free io
	// variables first; atoms without io variables always sort last.
	TEST_CASE("comparator arms") {
		auto atom = [](const char* sample) {
			tref fm = get_nso_rr(sample).value().main->get();
			tref a = tau::get(fm).find_top(
					is_atomic_bdd_var<node_t>);
			REQUIRE( a != nullptr );
			return a;
		};
		auto& cmp = atm_formula_order_for_simplification<node_t>;
		tref a0  = atom("o1[0] = 0.");
		tref a1  = atom("o1[1] = 0.");
		tref at  = atom("always o1[t] = 0.");
		tref at1 = atom("always o1[t-1] = 0.");
		tref at2 = atom("always o1[t-2] = 0.");
		tref a2io  = atom("o1[0] = i1[0].");
		tref ahigh = atom("o1[0] = i1[1].");
		tref clow  = atom("always o1[t] = i1[t-1].");
		tref x   = atom("x = 0.");
		// initial atoms: lowest initial time point first
		CHECK(  cmp(a0, a1) );
		CHECK( !cmp(a1, a0) );
		// irreflexive (strict weak ordering)
		CHECK( !cmp(a0, a0) );
		// lookback atoms: higher lookback wins
		CHECK(  cmp(at1, at) );
		CHECK( !cmp(at, at1) );
		CHECK(  cmp(at2, at1) );
		// initial atoms sort before lookback atoms
		CHECK(  cmp(a0, at) );
		CHECK( !cmp(at, a0) );
		// atoms without io variables always sort last
		CHECK( !cmp(x, a0) );
		CHECK(  cmp(a0, x) );
		// same lowest initial: lower highest initial wins
		CHECK(  cmp(a0, ahigh) );
		CHECK( !cmp(ahigh, a0) );
		// same lowest/highest initial: fewer io variables first
		CHECK(  cmp(a0, a2io) );
		CHECK( !cmp(a2io, a0) );
		// same lookback: higher high time point wins
		CHECK(  cmp(at1, clow) );
		CHECK( !cmp(clow, at1) );
		// same lookback and high time point: fewer io variables first
		tref c2io = atom("always o1[t-1] = i1[t-1].");
		CHECK(  cmp(at1, c2io) );
		CHECK( !cmp(c2io, at1) );
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

	// GitHub #82: the implication used to be closed over every free
	// variable and normalized in one piece, so an antecedent/consequent
	// pair whose atoms fall into many variable-disjoint components (the
	// shape find_fixpoint_phi produces for a spec accumulating N clauses)
	// paid for one N-component Boole decomposition per check. It is now
	// decided per consequent conjunct against the antecedent conjuncts
	// its variables connect to. These pin the exactness of that split.
	TEST_CASE("is_nso_impl (#82): per-conjunct decomposition, all implied") {
		tref n1 = get_nso_rr("x = 0 && y = 0 && z = 0.").value().main->get();
		tref n2 = get_nso_rr("x = 0 && (y = 0 || w = 1).").value().main->get();
		CHECK( is_nso_impl<node_t>(n1, n2) );
	}

	TEST_CASE("is_nso_impl (#82): one unconnected conjunct is not implied") {
		// w is untouched by the antecedent, so `w = 0` is not implied even
		// though every other conjunct is.
		tref n1 = get_nso_rr("x = 0 && y = 0.").value().main->get();
		tref n2 = get_nso_rr("x = 0 && y = 0 && w = 0.").value().main->get();
		CHECK( !is_nso_impl<node_t>(n1, n2) );
	}

	TEST_CASE("is_nso_impl (#82): an unsatisfiable unconnected antecedent "
		  "component makes the implication vacuous")
	{
		// `x = 0 && x = 1` (over sbf, x = 1 means x is the top element)
		// is unsatisfiable, and it shares no variable with `w = 0`, so
		// deciding `w = 0` against its own (empty) component alone would
		// say false; the whole implication still holds because the
		// antecedent is unsatisfiable.
		tref n1 = get_nso_rr("x = 0 && x = 1 && y = 0.").value().main->get();
		tref n2 = get_nso_rr("y = 0 && w = 0.").value().main->get();
		CHECK( is_nso_impl<node_t>(n1, n2) );
	}

	TEST_CASE("is_nso_impl (#82): chained sharing merges components") {
		// z = 0 follows from x = 0 only through y: x=0 -> y=0 -> z=0, so
		// the component of `z = 0` must pull in both antecedent conjuncts.
		tref n1 = get_nso_rr(
			"x = 0 && (x != 0 || y = 0) && (y != 0 || z = 0).")
			.value().main->get();
		tref n2 = get_nso_rr("z = 0.").value().main->get();
		CHECK( is_nso_impl<node_t>(n1, n2) );
	}

	TEST_CASE("is_nso_impl (#82): 40 disjoint-support components") {
		// The reporter's shape, at a size the monolithic check could not
		// finish: N clauses with pairwise disjoint support, each implied
		// by its own antecedent clause.
		std::string a, c;
		for (int k = 0; k < 40; ++k) {
			std::string xk = "x" + std::to_string(k),
				yk = "y" + std::to_string(k);
			a += std::string(k ? " && " : "") + xk + " = 0 && " + yk + " = 0";
			c += std::string(k ? " && " : "") + "(" + xk + " != 0 || " + yk + " = 0)";
		}
		tref n1 = get_nso_rr((a + ".").c_str()).value().main->get();
		tref n2 = get_nso_rr((c + ".").c_str()).value().main->get();
		CHECK( is_nso_impl<node_t>(n1, n2) );
		// ... and a single broken component is detected.
		tref n3 = get_nso_rr((c + " && (x7 != 0 || y7 = 1).").c_str())
			.value().main->get();
		CHECK( !is_nso_impl<node_t>(n1, n3) );
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

	// Negation normal form is `&&`, `||` and negated atoms and nothing
	// else. The sugar connectives never survive `tau::get` -- the
	// construction hooks rewrite them away, which is why they have to be
	// built with hooks off here. That is not a contrivance: a Tau-BA
	// constant is parsed exactly that way (`tau_spec` sets
	// `reget_with_hooks = false`) and, unlike every other parsed formula,
	// never goes through `api::simplify`, whose `reget` is what desugars
	// them. to_nnf leaving them in place made every consumer of its output
	// read the tree wrong; simplify_using_equality took an implication's
	// antecedent for an asserted fact (issue #69).

	// Builds `wff(sym(args...))` without the hooks that would desugar it.
	static auto raw_sugar = [](typename node_t::type sym,
		const trefs& args)
	{
		use_hooks_guard<node_t> hooks_off(false);
		return tau::get(tau::wff, tau::get(sym, args));
	};

	TEST_CASE("sugar: a = 0 -> b = 0") {
		tref a = get_nso_rr("a = 0.").value().main->get();
		tref b = get_nso_rr("b = 0.").value().main->get();
		tref fm = raw_sugar(tau::wff_imply, { a, b });
		REQUIRE( tau::get(fm).find_top(is<node_t, tau::wff_imply>) );
		tref res = to_nnf<node_t>(fm);
		CHECK( !tau::get(res).find_top(is<node_t, tau::wff_imply>) );
		CHECK( tau::get(res).find_top(is<node_t, tau::wff_or>) );
	}

	TEST_CASE("sugar: !(a = 0 -> b = 0)") {
		tref a = get_nso_rr("a = 0.").value().main->get();
		tref b = get_nso_rr("b = 0.").value().main->get();
		tref fm = raw_sugar(tau::wff_neg,
			{ raw_sugar(tau::wff_imply, { a, b }) });
		REQUIRE( tau::get(fm).find_top(is<node_t, tau::wff_imply>) );
		tref res = to_nnf<node_t>(fm);
		CHECK( !tau::get(res).find_top(is<node_t, tau::wff_imply>) );
		// !(a=0 -> b=0) is a=0 && b!=0
		CHECK( tau::get(res).find_top(is<node_t, tau::wff_and>) );
	}

	TEST_CASE("sugar: a = 0 <- b = 0") {
		tref a = get_nso_rr("a = 0.").value().main->get();
		tref b = get_nso_rr("b = 0.").value().main->get();
		tref fm = raw_sugar(tau::wff_rimply, { a, b });
		REQUIRE( tau::get(fm).find_top(is<node_t, tau::wff_rimply>) );
		tref res = to_nnf<node_t>(fm);
		CHECK( !tau::get(res).find_top(is<node_t, tau::wff_rimply>) );
		CHECK( tau::get(res).find_top(is<node_t, tau::wff_or>) );
	}

	TEST_CASE("sugar: a = 0 <-> b = 0") {
		tref a = get_nso_rr("a = 0.").value().main->get();
		tref b = get_nso_rr("b = 0.").value().main->get();
		tref fm = raw_sugar(tau::wff_equiv, { a, b });
		REQUIRE( tau::get(fm).find_top(is<node_t, tau::wff_equiv>) );
		tref res = to_nnf<node_t>(fm);
		CHECK( !tau::get(res).find_top(is<node_t, tau::wff_equiv>) );
		CHECK( !tau::get(res).find_top(is<node_t, tau::wff_imply>) );
	}

	TEST_CASE("sugar: a = 0 ^^ b = 0") {
		tref a = get_nso_rr("a = 0.").value().main->get();
		tref b = get_nso_rr("b = 0.").value().main->get();
		tref fm = raw_sugar(tau::wff_xor, { a, b });
		REQUIRE( tau::get(fm).find_top(is<node_t, tau::wff_xor>) );
		tref res = to_nnf<node_t>(fm);
		CHECK( !tau::get(res).find_top(is<node_t, tau::wff_xor>) );
	}

	TEST_CASE("sugar: a = 0 ? b = 0 : c = 0") {
		tref a = get_nso_rr("a = 0.").value().main->get();
		tref b = get_nso_rr("b = 0.").value().main->get();
		tref c = get_nso_rr("c = 0.").value().main->get();
		tref fm = raw_sugar(tau::wff_conditional, { a, b, c });
		REQUIRE( tau::get(fm).find_top(is<node_t, tau::wff_conditional>) );
		tref res = to_nnf<node_t>(fm);
		CHECK( !tau::get(res).find_top(is<node_t, tau::wff_conditional>) );
		CHECK( !tau::get(res).find_top(is<node_t, tau::wff_imply>) );
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

	// NF-1. The construction hooks leave a bv comparison as a raw `bf_lt`
	// atom (they only expand comparisons for plain BAs), so `reduce`'s
	// BDD-variable classifier has to treat it as an opaque atom.  When it did
	// not, clause_to_vector matched no branch for the atom and just descended
	// past it: the atom was left out of the path vector and
	// build_reduced_formula rebuilt the clause without it.  With the
	// comparison as the *only* atom the variable set came out empty and
	// reduce answered F for a perfectly satisfiable formula.
	TEST_CASE("bv comparison as the only atom is not reduced to F") {
		const char* sample = "x:bv[8] < y:bv[8].";
		auto rr = get_nso_rr(sample);
		REQUIRE( rr.has_value() );
		tref fm = rr.value().main->get();
		REQUIRE( tau::get(fm).find_top(is<node_t, tau::bf_lt>) );
		tref res = reduce<node_t>(fm);
		CHECK( !tau::get(res).equals_F() );
		CHECK( tau::get(res).find_top(is<node_t, tau::bf_lt>) );
	}

	TEST_CASE("bv comparison is not dropped from a conjunction") {
		const char* sample = "x:bv[8] < y:bv[8] && z:bv[8] = 0.";
		auto rr = get_nso_rr(sample);
		REQUIRE( rr.has_value() );
		tref fm = rr.value().main->get();
		tref res = reduce<node_t>(fm);
		CHECK( tau::get(res).find_top(is<node_t, tau::bf_lt>) );
		CHECK( tau::get(res).find_top(is<node_t, tau::bf_eq>) );
	}

	// A negated comparison does not reach the BDD-variable selection as a
	// `wff_neg` wrapper: `dnf_cnf_to_reduced` runs `push_negation_in` first,
	// and that rewrites `!(a < b)` into the `bf_nlt` atom
	// (normal_forms_nnf.tmpl.h). So the negated comparison kinds have to be
	// classified as BDD variables in their own right -- classifying only
	// `bf_lt`/`bf_lteq` would still drop these.
	TEST_CASE("negated bv comparison as the only atom is not reduced to F") {
		const char* sample = "!(x:bv[8] < y:bv[8]).";
		auto rr = get_nso_rr(sample);
		REQUIRE( rr.has_value() );
		tref fm = rr.value().main->get();
		tref res = reduce<node_t>(fm);
		CHECK( !tau::get(res).equals_F() );
		CHECK( (tau::get(res).find_top(is<node_t, tau::bf_nlt>)
			|| tau::get(res).find_top(is<node_t, tau::bf_lt>)) );
	}

	TEST_CASE("negated bv comparison is not dropped from a conjunction") {
		const char* sample = "!(x:bv[8] < y:bv[8]) && z:bv[8] = 0.";
		auto rr = get_nso_rr(sample);
		REQUIRE( rr.has_value() );
		tref fm = rr.value().main->get();
		tref res = reduce<node_t>(fm);
		CHECK( !tau::get(res).equals_F() );
		CHECK( tau::get(res).find_top(is<node_t, tau::bf_eq>) );
		CHECK( (tau::get(res).find_top(is<node_t, tau::bf_nlt>)
			|| tau::get(res).find_top(is<node_t, tau::bf_lt>)) );
	}

	TEST_CASE("bv <= comparison is not dropped from a conjunction") {
		const char* sample = "x:bv[8] <= y:bv[8] && z:bv[8] = 0.";
		auto rr = get_nso_rr(sample);
		REQUIRE( rr.has_value() );
		tref fm = rr.value().main->get();
		tref res = reduce<node_t>(fm);
		CHECK( tau::get(res).find_top(is<node_t, tau::bf_lteq>) );
		CHECK( tau::get(res).find_top(is<node_t, tau::bf_eq>) );
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

// TC-4: simplify_temporal_clause's `return std::nullopt` at
// normalizer.tmpl.h:777 is the only way a temporal clause is dropped, and it
// had no test -- the existing coverage exercises the implied-always path only.
TEST_SUITE("SimplifyTemporalClauseUnsat") {

	TEST_CASE("an unsatisfiable always/sometimes pair drops the clause") {
		tref p = get_nso_rr("x = 0.").value().main->get();
		tref q = get_nso_rr("x != 0.").value().main->get();
		// always x = 0 && sometimes x != 0: the pair is unsatisfiable, so
		// the whole clause is dropped.
		tref clause = tau::build_wff_and(tau::build_wff_always(p),
			tau::build_wff_sometimes(q));
		CHECK( !simplify_temporal_clause<node_t>(clause).has_value() );
	}

	TEST_CASE("a satisfiable always/sometimes pair keeps the clause") {
		tref p = get_nso_rr("x = 0.").value().main->get();
		tref q = get_nso_rr("y = 0.").value().main->get();
		tref clause = tau::build_wff_and(tau::build_wff_always(p),
			tau::build_wff_sometimes(q));
		auto res = simplify_temporal_clause<node_t>(clause);
		REQUIRE( res.has_value() );
		CHECK( *res != nullptr );
	}

	// A sometimes part implied by an always part is replaced by T rather than
	// dropping the clause (normalizer.tmpl.h:781-782).
	TEST_CASE("a sometimes part implied by an always part is eliminated") {
		tref strong = get_nso_rr("x = 0 && y = 0.").value().main->get();
		tref weak = get_nso_rr("x = 0.").value().main->get();
		tref clause = tau::build_wff_and(tau::build_wff_always(strong),
			tau::build_wff_sometimes(weak));
		auto res = simplify_temporal_clause<node_t>(clause);
		REQUIRE( res.has_value() );
		// The sometimes part is gone; only the always part is left.
		CHECK( !tau::get(*res).find_top(is<node_t, tau::wff_sometimes>) );
	}

	// The single-part short-circuit at normalizer.tmpl.h:747-749.
	TEST_CASE("a clause with a single temporal part is returned as is") {
		tref p = get_nso_rr("x = 0.").value().main->get();
		tref only_aw = tau::build_wff_always(p);
		auto r1 = simplify_temporal_clause<node_t>(only_aw);
		REQUIRE( r1.has_value() );
		CHECK( *r1 == only_aw );
		tref only_st = tau::build_wff_sometimes(p);
		auto r2 = simplify_temporal_clause<node_t>(only_st);
		REQUIRE( r2.has_value() );
		CHECK( *r2 == only_st );
	}
}

// NZ-1 regression. normalize_non_temp can legitimately return a formula that is
// neither T nor F: a closed bv scope the solver cannot settle (here bv
// arithmetic plus an unresolved wff_ref, which is_bv_solvable_formula accepts
// because it inspects only variable nodes, and which cvc5 then fails to
// translate) comes back with its quantifier intact. All three predicates below
// asserted that could not happen, so each aborted a Debug build on this input.
// They now fall back to their negative answer and log it.
TEST_SUITE("UndecidableNormalizationFallback") {

	static tref undecidable() {
		return get_nso_rr("ex x (x:bv[8] * y:bv[8] = { 1 }:bv[8]"
			" && q(x)).").value().main->get();
	}

	TEST_CASE("normalize_non_temp leaves it quantified") {
		tref res = normalize_non_temp<node_t>(undecidable());
		REQUIRE( res != nullptr );
		CHECK( !tau::get(res).equals_T() );
		CHECK( !tau::get(res).equals_F() );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) != nullptr );
	}

	TEST_CASE("is_nso_impl answers false instead of aborting") {
		CHECK( !is_nso_impl<node_t>(tau::_T(), undecidable()) );
	}

	TEST_CASE("is_non_temp_nso_unsat answers false instead of aborting") {
		CHECK( !is_non_temp_nso_unsat<node_t>(undecidable()) );
	}

	TEST_CASE("is_non_temp_nso_satisfiable answers false instead of aborting") {
		CHECK( !is_non_temp_nso_satisfiable<node_t>(undecidable()) );
	}

	TEST_CASE("are_nso_equivalent answers false instead of aborting") {
		CHECK( !are_nso_equivalent<node_t>(tau::_T(), undecidable()) );
	}

	// Control: a decidable formula still gets a real answer.
	TEST_CASE("decidable formulas are unaffected") {
		tref taut = get_nso_rr("x = 0 || x != 0.").value().main->get();
		CHECK( is_nso_impl<node_t>(tau::_T(), taut) );
		CHECK( !is_non_temp_nso_unsat<node_t>(taut) );
		CHECK( is_non_temp_nso_satisfiable<node_t>(taut) );
	}
}

// check_decided's NZ-1 arm (added with the bc99a82b port): a temporal
// operator directly inside a quantifier scope is undecidable by any
// case-split on the bound variable alone, so complete_quantifier_elimination
// leaves it quantified and check_decided reports it at WARNING (message
// carries the "NZ-1" marker) instead of ERROR. Every other undecided shape
// must keep the ERROR path so a genuine regression still trips loudly.
TEST_SUITE("NZ1TemporalUnderQuantifier") {

	// The grammar has no quantifier-over-always position; build the shape
	// the way the pipeline meets it, internally.
	static tref nz1() {
		tref spec = get_nso_rr("always o1[t]b != 0.").value().main->get();
		return tau::build_wff_all_many(get_free_vars<node_t>(spec), spec);
	}

	static tref bv_undecided() {
		return get_nso_rr("ex x (x:bv[8] * y:bv[8] = { 1 }:bv[8]"
			" && q(x)).").value().main->get();
	}

	// Capture everything the logging core emits during f().
	static std::string log_of(const std::function<void()>& f) {
		auto ss = boost::make_shared<std::stringstream>();
		auto sink = boost::log::add_console_log(*ss);
		f();
		boost::log::core::get()->remove_sink(sink);
		return ss->str();
	}

	TEST_CASE("normalization keeps the NZ-1 shape quantified and temporal") {
		tref res = normalize_non_temp<node_t>(nz1());
		REQUIRE( res != nullptr );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) != nullptr );
		CHECK( tau::get(res).find_top(
			is_child<node_t, tau::wff_always>) != nullptr );
	}

	TEST_CASE("check_decided answers false with the NZ-1 marker") {
		tref res = normalize_non_temp<node_t>(nz1());
		bool decided = true;
		std::string log = log_of([&]() {
			decided = check_decided<node_t>("nz1-test", res); });
		CHECK( !decided );
		CHECK( log.find("NZ-1") != std::string::npos );
	}

	TEST_CASE("non-temporal undecided keeps the error path (no NZ-1 marker)") {
		tref res = normalize_non_temp<node_t>(bv_undecided());
		bool decided = true;
		std::string log = log_of([&]() {
			decided = check_decided<node_t>("bv-test", res); });
		CHECK( !decided );
		CHECK( log.find("could not decide") != std::string::npos );
		CHECK( log.find("NZ-1") == std::string::npos );
	}

	TEST_CASE("decided formulas stay decided") {
		CHECK( check_decided<node_t>("t-test", tau::_T()) );
		CHECK( check_decided<node_t>("f-test", tau::_F()) );
	}

	// No end-to-end are_nso_equivalent case here: its
	// has_no_boolean_combs_of_models precondition (DBG-asserted) rejects
	// a quantified temporal formula handed in directly. The pipeline
	// reaches check_decided's NZ-1 arm with formulas it built itself;
	// the direct check_decided cases above pin both arms.
}

// NF-6 / AP-16. squeeze_absorb disables the process-global tree<node>::use_hooks
// for the duration of its traversal and used to re-enable it by assigning
// `true` unconditionally at the end. Two ways that goes wrong: an exception
// thrown out of the traversal (this subsystem's bv paths do throw) skips the
// re-enable and leaves hooks disabled for the rest of the process, and a caller
// that had deliberately disabled them gets them force-enabled on return.
TEST_SUITE("UseHooksGuard") {

	TEST_CASE("restores the previous value on scope exit") {
		const bool before = tau::use_hooks;
		{
			use_hooks_guard<node_t> g(false);
			CHECK( tau::use_hooks == false );
		}
		CHECK( tau::use_hooks == before );
	}

	TEST_CASE("restores a false previous value, not `true`") {
		// The old code's second failure mode: it did not save, it assigned.
		tau::use_hooks = false;
		{
			use_hooks_guard<node_t> g(false);
			CHECK( tau::use_hooks == false );
		}
		CHECK( tau::use_hooks == false );
		tau::use_hooks = true;
	}

	TEST_CASE("restores on an exception thrown through the scope") {
		const bool before = tau::use_hooks;
		try {
			use_hooks_guard<node_t> g(false);
			throw std::runtime_error("unwind");
		} catch (const std::runtime_error&) {}
		CHECK( tau::use_hooks == before );
	}

	TEST_CASE("squeeze_absorb leaves hooks as it found them") {
		const bool before = tau::use_hooks;
		tref fm = get_nso_rr("x y = 0 && x z != 0.").value().main->get();
		tref var = tau::get(fm).find_top(is<node_t, tau::variable>);
		REQUIRE( var != nullptr );
		squeeze_absorb<node_t>(fm, var);
		CHECK( tau::use_hooks == before );
	}
}

TEST_SUITE("atm_formula_order_for_quant_elim stability") {

	// Pivot selection must not depend on parser nonterminal numbering:
	// a `./dev regen` with a newer pinned generator renumbers the
	// nonterminals, and the comparator's old subtree_less tie-breaks
	// flipped with them — the 8f1a74c1 regeneration re-rolled the
	// Boole-decomposition pivot order this way and with it the
	// decomposition cost (bisected 2026-08-18/19, GitHub #70 family).
	// Ties must therefore break on PRINTED form: surface syntax is the
	// one ordering a regeneration cannot change.
	TEST_CASE("ties break on printed form, not grammar numbering") {
		// All four equations tie on every semantic key: the same
		// quantified variable x (same max/min priority), none an
		// equational assignment, one extra unquantified variable each.
		tref fm = get_nso_rr(
			"ex x (x d = 0 && x b = 0 && x c = 0 && x a = 0).")
				.value().main->get();
		// The comparator receives wff-level atoms (wff{bf_eq ...}),
		// exactly what anti_prenex_block's candidate scan collects.
		trefs atms = tau::get(fm).select_top(
			[](tref n) {
				const auto& t = tau::get(n);
				return t.is(tau::wff) && t.child_is(tau::bf_eq);
			});
		REQUIRE( atms.size() == 4 );
		// x's binder node is interned identically to its occurrences.
		tref x = tau::get(fm).find_top(is<node_t, tau::variable>);
		REQUIRE( x != nullptr );
		subtree_unordered_map<node_t, int_t> qp;
		qp.emplace(x, 1);
		auto comp = atm_formula_order_for_quant_elim<node_t>(qp);
		// Spec: among fully tied candidates the minimum is the one with
		// the lexicographically smallest printed form.
		std::vector<std::string> strs;
		for (tref a : atms) strs.push_back(tau::get(a).to_str());
		const std::string smallest =
			*std::min_element(strs.begin(), strs.end());
		tref m = *std::min_element(atms.begin(), atms.end(), comp);
		CHECK( tau::get(m).to_str() == smallest );
		// Strict-weak-order sanity: irreflexive and asymmetric.
		for (tref p : atms) CHECK( !comp(p, p) );
		for (tref p : atms) for (tref q : atms)
			if (comp(p, q)) CHECK( !comp(q, p) );
	}
}
