// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "normal_forms.h"

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

TEST_SUITE("normal forms: bf_reduce_canonical") {


	TEST_CASE("uninterpreted constants") {
		const char* sample = "(<:c>' & <:b>' & <:b> | <:c>' & <:b>' & <:c> & <:b>' | <:c>' & <:c> & <:b> & <:b> | <:c>' & <:c> & <:b> & <:c> & <:b>') & <:a> | (<:b>' & <:c>' & <:b> | <:b>' & <:c>' & <:c> & <:b>' | <:c> & <:b> & <:c>' & <:b> | <:c> & <:b> & <:c>' & <:c> & <:b>') & <:a>' = 0.";
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
		const char* sample = "(<:c>' & <:b>' & <:b> | <:c>' & <:b>' & <:c> & <:b>' | <:c>' & <:c> & <:b> & <:b> | <:c>' & <:c> & <:b> & <:c> & <:b>') & <:a> | (<:b>' & <:c>' & <:b> | <:b>' & <:c>' & <:c> & <:b>' | <:c> & <:b> & <:c>' & <:b> | <:c> & <:b> & <:c>' & <:c> & <:b>') & <:a>' = 0.";
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
	TEST_CASE("3") {
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

TEST_SUITE("TreatExQuantifiedClause") {
	TEST_CASE("surviving inner quantifier blocks elimination (B5)") {
		// The inner ex y survived elimination; its equations are not
		// top-level conjuncts of the outer clause, so the eliminator
		// must keep the whole quantified clause instead of squeezing
		// them and silently dropping the inner binder (leaking y free)
		const char* sample =
			"ex x (xw = 0 && (ex y (xy = 0 && f(y) != 0))).";
		tref fm = get_nso_rr(sample).value().main->get();
		bool quant_eliminated = true;
		tref res = treat_ex_quantified_clause<node_t>(fm, quant_eliminated);
		CHECK( !quant_eliminated );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) != nullptr );
	}

	TEST_CASE("plain clause still eliminated (B5 control)") {
		const char* sample = "ex x (xw = 0 && xz != 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		bool quant_eliminated = true;
		tref res = treat_ex_quantified_clause<node_t>(fm, quant_eliminated);
		CHECK( quant_eliminated );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) == nullptr );
		CHECK( res != fm );
	}
}

TEST_SUITE("AntiPrenexBlock") {
	// Helper: peel the leading ex-quantifier prefix into a block,
	// innermost variable gets the lowest order index
	static std::tuple<tref, trefs, term_handle<node_t>::order> peel_block(
		tref fm)
	{
		trefs block;
		term_handle<node_t>::order order;
		while (tau::get(fm)[0].is(tau::wff_ex)) {
			block.push_back(tau::get(fm)[0].first());
			fm = tau::get(fm)[0].second();
		}
		for (size_t i = 0; i < block.size(); ++i)
			order.emplace(block[i], block.size() - 1 - i);
		return {fm, block, order};
	}

	// Helper: run anti_prenex_block on a parsed "ex ... (...)" sample
	static std::pair<tref, size_t> run_apb(const char* sample) {
		tref fm = get_nso_rr(sample).value().main->get();
		auto [body, block, order] = peel_block(fm);
		subtree_unordered_set<node_t> used_atms;
		subtree_unordered_map<node_t, int_t> quant_pattern;
		for (size_t i = 0; i < block.size(); ++i)
			quant_pattern.emplace(block[i], i + 1);
		tref res = anti_prenex_block<node_t>(body, block,
			used_atms, quant_pattern, order);
		return {res, used_atms.size()};
	}

	TEST_CASE("disjunction push (B11)") {
		// The disjunction-case recursions must compile and push the
		// block into each disjunct independently
		auto [res, used] = run_apb("ex x (xy = 0 || xw = 0).");
		CHECK( matches_to_str_to_any_of(res, {
			"(ex b1 b1 y = 0) || (ex b1 b1 w = 0)",
			"(ex b1 b1 w = 0) || (ex b1 b1 y = 0)",
		}) );
		CHECK( used == 0 );
	}

	TEST_CASE("decomposition keeps independent conjuncts (B12/B13)") {
		// z = 0 must survive (B13) and the negative branch must be
		// built from formula[atm:=F], so the remaining equation gets
		// resolved by the clause eliminator (B12): for atm = xw = 0
		// the branch ex x (xw != 0 && xy = 0) resolves to wy' != 0
		auto [res, used] = run_apb("ex x (z = 0 && (xy = 0 || xw = 0)).");
		CHECK( matches_to_str_to_any_of(res, {
			"z = 0 && ((ex b1 b1 w = 0) || !wy' = 0)",
			"z = 0 && ((ex b1 b1 w = 0) || !y'w = 0)",
			"z = 0 && ((ex b1 b1 y = 0) || !yw' = 0)",
			"z = 0 && ((ex b1 b1 y = 0) || !w'y = 0)",
		}) );
		CHECK( used == 0 );
	}

	TEST_CASE("no decomposable atoms keeps block (B14)") {
		// The dependent part contains only != atoms, which the
		// decomposition cannot use: the block must be kept on the
		// dependent part (instead of dereferencing end())
		auto [res, used] = run_apb("ex x (z = 0 && (xy != 0 || xw != 0)).");
		CHECK( matches_to_str_to_any_of(res, {
			"z = 0 && (ex b1 b1 y != 0 || b1 w != 0)",
			"z = 0 && (ex b1 b1 w != 0 || b1 y != 0)",
		}) );
		CHECK( used == 0 );
	}

	TEST_CASE("T branch shortcut restores used_atms (B15)") {
		// The positive decomposition branch resolves to T (the clause
		// ex x (atm && xk = 0) is satisfiable by x := 0), triggering
		// the early return, which must leave used_atms balanced
		auto [res, used] = run_apb(
			"ex x (z = 0 && (xy = 0 || xw = 0) && xk = 0).");
		CHECK( tau::get(res).to_str() == "z = 0" );
		CHECK( used == 0 );
	}
}

TEST_SUITE("AntiPrenexBlock0Arg") {
	// Tests for the zero-arg anti_prenex_block (the full pipeline:
	// NNF+simplify → subs_elim → normalize_atomic → process_block post-order).

	static tref run_apb0(const char* sample) {
		return anti_prenex_block<node_t>(
			get_nso_rr(sample).value().main->get());
	}

	TEST_CASE("quantifier-free formula is returned unchanged") {
		// Short-circuit: no quantifiers → original tref returned as-is.
		tref fm = get_nso_rr("xy = 0 && wz = 0.").value().main->get();
		CHECK( anti_prenex_block<node_t>(fm) == fm );
	}

	TEST_CASE("subs_elim: ex x (xy=0 && x=w) → wy=0") {
		// Step 2 (subs_elim): ex x (x=w && xy=0) → (xy=0)[x:=w] = wy=0
		tref res = run_apb0("ex x (xy = 0 && x = w).");
		CHECK( matches_to_str_to_any_of(res, {"wy = 0", "yw = 0"}) );
	}

	TEST_CASE("subs_elim: ex x (x=w) → T") {
		// ex x (x=w): body after subs_elim is empty (T), since the only
		// conjunct was the substitution witness x=w itself.
		tref res = run_apb0("ex x (x = w).");
		CHECK( tau::get(res)[0].is(tau::wff_t) );
	}

	TEST_CASE("all-block dualization: all x (x=0 || x!=0) → T") {
		// ∀x. (x=0 ∨ x≠0): law of excluded middle in BA → T.
		tref res = run_apb0("all x (x = 0 || x != 0).");
		CHECK( tau::get(res)[0].is(tau::wff_t) );
	}

	TEST_CASE("all-block dualization: all x (xy!=0) → F") {
		// ∀x. xy≠0: pick x=0 → 0·y=0=0, contradiction → F.
		tref res = run_apb0("all x xy != 0.");
		CHECK( tau::get(res)[0].is(tau::wff_f) );
	}

	TEST_CASE("ex block conjunction decomposition") {
		// ∃x. (xy=0 ∧ wz=0): wz=0 is independent of x → factor out;
		// ∃x. xy=0 → T (pick x=0).  Result: T ∧ wz=0 = wz=0.
		tref res = run_apb0("ex x (xy = 0 && wz = 0).");
		CHECK( matches_to_str_to_any_of(res, {"wz = 0", "zw = 0"}) );
	}
}

TEST_SUITE("QuantBlockPush") {
	TEST_CASE("1") {
		const char* sample = "ex x ex y xy = 0 && yx = 0 && !(x|y = 0) && !(x = y).";
		tref fm = get_nso_rr(sample).value().main->get();
		trefs quant_block;
		quant_block.push_back(tau::get(fm)[0].first());
		fm = tau::get(fm)[0].second();
		quant_block.push_back(tau::get(fm)[0].first());
		fm = tau::get(fm)[0].second();
		term_handle<node_t>::order order;
		tref res = push_ex_block_into_clause<node_t>(fm, quant_block, order);
		// tau::get(res).print(std::cout << "res: ") << "\n";
		CHECK(tau::get(res).to_str() == "(ex b2, b1 b2 b1|b1 b2 = 0) && (ex b2, b1 !(b2 b1|b1 b2)'&(b2|b1) = 0) && (ex b2, b1 !(b2 b1|b1 b2)'&(b2^b1) = 0)");
	}
	TEST_CASE("2") {
		const char* sample = "all u ex v (u<v && v<x).";
		tref fm = get_nso_rr(sample).value().main->get();
		fm = unequal_to_not_equal<node_t>(fm);
		trefs quant_block;
		term_handle<node_t>::order order;
		tref uvar = tau::trim2(fm);
		order.emplace(uvar, 1);
		fm = tau::get(fm)[0].second();
		quant_block.push_back(tau::trim2(fm));
		order.emplace(tau::trim2(fm), 0);
		fm = tau::get(fm)[0].second();
		tref res = push_ex_block_into_clause<node_t>(fm, quant_block, order);
		// tau::get(res).print(std::cout << "ex: ") << "\n";
		res = tau::build_wff_all(uvar, res, false);
		res = push_quantifiers_in<node_t>(res);
		// tau::get(res).print(std::cout << "all: ") << "\n";
		res = resolve_quantifiers2<node_t>(res, order);
		// tau::get(res).print(std::cout << "res: ") << "\n";
		CHECK(tau::get(res).equals_F());
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
