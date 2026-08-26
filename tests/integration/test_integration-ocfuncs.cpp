// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Tests for heterogeneous OMCAT function symbols (OCFuncs).
// Covers: function declaration parsing, mode checking, purification,
// profile enumeration, static profile encoding, LTL generation, and
// function application parsing/detection.

#include "test_integration-satisfiability_helper.h"
#include "ocfuncs.h"

// ═════════════════════════════════════════════════════════════════════════════
// 1. FUNCTION APPLICATION PARSING
// ═════════════════════════════════════════════════════════════════════════════

TEST_SUITE("OCFuncs parsing - function application") {
	TEST_CASE("parse_func_app_single_arg") {
		auto r = get_nso_rr<node_t>(tau::get("f(o1[t]) = 1."));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_func_app_two_args") {
		auto r = get_nso_rr<node_t>(tau::get("g(o1[t], o2[t]) = 0."));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_func_app_three_args") {
		auto r = get_nso_rr<node_t>(tau::get("h(o1[t], o2[t], o3[t]) = 1."));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_func_app_in_always") {
		auto r = get_nso_rr<node_t>(tau::get("always f(o1[t]) = 1."));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_func_app_in_sometimes") {
		auto r = get_nso_rr<node_t>(tau::get("sometimes f(o1[t]) = 0."));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_func_app_in_F") {
		auto r = get_nso_rr<node_t>(tau::get("F f(o1[t]) = 1."));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_func_app_in_G") {
		auto r = get_nso_rr<node_t>(tau::get("G f(o1[t]) = 1."));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_func_app_eq_func_app") {
		auto r = get_nso_rr<node_t>(tau::get("f(o1[t]) = g(o2[t])."));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_func_app_neq") {
		auto r = get_nso_rr<node_t>(tau::get("f(o1[t]) != 0."));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_func_app_with_input") {
		auto r = get_nso_rr<node_t>(tau::get("f(i1[t]) = o1[t]."));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_func_app_with_constant") {
		auto r = get_nso_rr<node_t>(tau::get("f(1) = o1[t]."));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_func_app_and") {
		auto r = get_nso_rr<node_t>(tau::get("f(o1[t]) = 1 && g(o2[t]) = 0."));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_func_app_or") {
		auto r = get_nso_rr<node_t>(tau::get("f(o1[t]) = 1 || g(o2[t]) = 0."));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_func_app_neg") {
		auto r = get_nso_rr<node_t>(tau::get("!f(o1[t]) = 0."));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_func_app_imply") {
		auto r = get_nso_rr<node_t>(tau::get("f(i1[t]) = 1 -> f(o1[t]) = 1."));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_func_app_lookback") {
		auto r = get_nso_rr<node_t>(tau::get("f(o1[t-1]) = 0."));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_func_app_paren") {
		auto r = get_nso_rr<node_t>(tau::get("(f(o1[t]) = 1)."));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_func_app_with_A") {
		auto r = get_nso_rr<node_t>(tau::get("A f(o1[t]) = 1."));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_func_app_with_E") {
		auto r = get_nso_rr<node_t>(tau::get("E f(o1[t]) = 0."));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_func_app_with_sem_neg") {
		auto r = get_nso_rr<node_t>(tau::get("-(f(o1[t]) = 1)."));
		CHECK(r.has_value());
	}
}

// ═════════════════════════════════════════════════════════════════════════════
// 2. FUNCTION DECLARATION DATA STRUCTURES
// ═════════════════════════════════════════════════════════════════════════════

TEST_SUITE("OCFuncs - FuncDecl") {
	TEST_CASE("func_decl_dynamic") {
		FuncDecl d;
		d.name = "f";
		d.mode = FuncMode::DYNAMIC;
		d.arg_sorts = {"Bool"};
		d.result_sort = "Bool";
		CHECK(d.arity() == 1);
		CHECK(d.mode == FuncMode::DYNAMIC);
	}
	TEST_CASE("func_decl_static") {
		FuncDecl d;
		d.name = "g";
		d.mode = FuncMode::STATIC;
		d.arg_sorts = {"Bool", "Bool"};
		d.result_sort = "Bool";
		CHECK(d.arity() == 2);
		CHECK(d.mode == FuncMode::STATIC);
	}
	TEST_CASE("func_decl_ternary") {
		FuncDecl d;
		d.name = "h";
		d.mode = FuncMode::DYNAMIC;
		d.arg_sorts = {"Bool", "Bool", "Bool"};
		d.result_sort = "Bool";
		CHECK(d.arity() == 3);
	}
	TEST_CASE("func_decl_heterogeneous_sorts") {
		FuncDecl d;
		d.name = "f";
		d.mode = FuncMode::DYNAMIC;
		d.arg_sorts = {"Bool", "Int"};
		d.result_sort = "Real";
		CHECK(d.arity() == 2);
		CHECK(d.arg_sorts[0] == "Bool");
		CHECK(d.arg_sorts[1] == "Int");
		CHECK(d.result_sort == "Real");
	}
	TEST_CASE("func_decl_names") {
		FuncDecl d;
		d.name = "myFunc";
		d.mode = FuncMode::STATIC;
		d.arg_sorts = {"A"};
		d.result_sort = "B";
		CHECK(d.name == "myFunc");
	}
}

// ═════════════════════════════════════════════════════════════════════════════
// 3. MODE AND SUPPORT CHECKING (Step 1)
// ═════════════════════════════════════════════════════════════════════════════

TEST_SUITE("OCFuncs - Step 1: mode checking") {
	TEST_CASE("check_valid_dynamic") {
		OCFuncsContext ctx;
		std::vector<FuncDecl> decls = {
			{"f", FuncMode::DYNAMIC, {"Bool"}, "Bool"}
		};
		CHECK(ocfuncs_check_modes(decls, ctx));
	}
	TEST_CASE("check_valid_static") {
		OCFuncsContext ctx;
		std::vector<FuncDecl> decls = {
			{"g", FuncMode::STATIC, {"Bool"}, "Bool"}
		};
		CHECK(ocfuncs_check_modes(decls, ctx));
	}
	TEST_CASE("check_valid_mixed") {
		OCFuncsContext ctx;
		std::vector<FuncDecl> decls = {
			{"f", FuncMode::DYNAMIC, {"Bool"}, "Bool"},
			{"g", FuncMode::STATIC, {"Bool", "Bool"}, "Bool"}
		};
		CHECK(ocfuncs_check_modes(decls, ctx));
		CHECK(ctx.declarations.size() == 2);
	}
	TEST_CASE("check_empty_name_rejected") {
		OCFuncsContext ctx;
		std::vector<FuncDecl> decls = {
			{"", FuncMode::DYNAMIC, {"Bool"}, "Bool"}
		};
		CHECK(!ocfuncs_check_modes(decls, ctx));
	}
	TEST_CASE("check_empty_args_rejected") {
		OCFuncsContext ctx;
		std::vector<FuncDecl> decls = {
			{"f", FuncMode::DYNAMIC, {}, "Bool"}
		};
		CHECK(!ocfuncs_check_modes(decls, ctx));
	}
	TEST_CASE("check_empty_result_rejected") {
		OCFuncsContext ctx;
		std::vector<FuncDecl> decls = {
			{"f", FuncMode::DYNAMIC, {"Bool"}, ""}
		};
		CHECK(!ocfuncs_check_modes(decls, ctx));
	}
	TEST_CASE("check_static_creates_support_bound") {
		OCFuncsContext ctx;
		std::vector<FuncDecl> decls = {
			{"g", FuncMode::STATIC, {"Bool"}, "Bool"}
		};
		ocfuncs_check_modes(decls, ctx);
		CHECK(ctx.support_bounds.count("g") > 0);
	}
	TEST_CASE("check_static_creates_state") {
		OCFuncsContext ctx;
		std::vector<FuncDecl> decls = {
			{"g", FuncMode::STATIC, {"Bool"}, "Bool"}
		};
		ocfuncs_check_modes(decls, ctx);
		CHECK(ctx.static_states.count("g") > 0);
	}
	TEST_CASE("check_dynamic_no_support") {
		OCFuncsContext ctx;
		std::vector<FuncDecl> decls = {
			{"f", FuncMode::DYNAMIC, {"Bool"}, "Bool"}
		};
		ocfuncs_check_modes(decls, ctx);
		CHECK(ctx.support_bounds.count("f") == 0);
	}
	TEST_CASE("check_many_funcs") {
		OCFuncsContext ctx;
		std::vector<FuncDecl> decls;
		for (int i = 0; i < 10; ++i) {
			decls.push_back({
				"f" + std::to_string(i),
				(i % 2 == 0) ? FuncMode::DYNAMIC : FuncMode::STATIC,
				{"Bool"},
				"Bool"
			});
		}
		CHECK(ocfuncs_check_modes(decls, ctx));
		CHECK(ctx.declarations.size() == 10);
	}
}

// ═════════════════════════════════════════════════════════════════════════════
// 4. STATIC PROFILE ENCODING (Step 4)
// ═════════════════════════════════════════════════════════════════════════════

TEST_SUITE("OCFuncs - Step 4: static profile encoding") {
	TEST_CASE("encode_empty") {
		OCFuncsContext ctx;
		auto props = ocfuncs_encode_static_profiles(ctx);
		CHECK(props.empty());
	}
	TEST_CASE("encode_single_static_with_support") {
		OCFuncsContext ctx;
		ctx.declarations = {{"g", FuncMode::STATIC, {"Bool"}, "Bool"}};
		ctx.static_states["g"] = {"g", {}};
		ctx.support_bounds["g"] = {"g", {{"0"}, {"1"}}};
		auto props = ocfuncs_encode_static_profiles(ctx);
		CHECK(props.size() == 4); // 2 tuples * 2 props each (bound + val)
	}
	TEST_CASE("encode_no_static_funcs") {
		OCFuncsContext ctx;
		ctx.declarations = {{"f", FuncMode::DYNAMIC, {"Bool"}, "Bool"}};
		auto props = ocfuncs_encode_static_profiles(ctx);
		CHECK(props.empty());
	}
	TEST_CASE("encode_multiple_static") {
		OCFuncsContext ctx;
		ctx.declarations = {
			{"g1", FuncMode::STATIC, {"Bool"}, "Bool"},
			{"g2", FuncMode::STATIC, {"Bool"}, "Bool"}
		};
		ctx.static_states["g1"] = {"g1", {}};
		ctx.static_states["g2"] = {"g2", {}};
		ctx.support_bounds["g1"] = {"g1", {{"0"}}};
		ctx.support_bounds["g2"] = {"g2", {{"0"}, {"1"}}};
		auto props = ocfuncs_encode_static_profiles(ctx);
		CHECK(props.size() == 6); // 1*2 + 2*2
	}
}

// ═════════════════════════════════════════════════════════════════════════════
// 5. GRAPH EDGES AND PROFILES
// ═════════════════════════════════════════════════════════════════════════════

TEST_SUITE("OCFuncs - graph structures") {
	TEST_CASE("graph_edge_dynamic") {
		GraphEdge e;
		e.func_name = "f";
		e.args = {"x"};
		e.result_var = "v_f_x";
		e.is_static = false;
		CHECK(!e.is_static);
	}
	TEST_CASE("graph_edge_static") {
		GraphEdge e;
		e.func_name = "g";
		e.args = {"x", "y"};
		e.result_var = "v_g_x_y";
		e.is_static = true;
		CHECK(e.is_static);
	}
	TEST_CASE("graph_profile_empty") {
		GraphProfile p;
		p.type_id = 0;
		CHECK(p.edges.empty());
	}
	TEST_CASE("graph_profile_single_edge") {
		GraphProfile p;
		p.type_id = 0;
		p.edges.push_back({"f", {"x"}, "v_f_x", false});
		CHECK(p.edges.size() == 1);
	}
	TEST_CASE("graph_profile_multiple_edges") {
		GraphProfile p;
		p.type_id = 0;
		p.edges.push_back({"f", {"x"}, "v_f_x", false});
		p.edges.push_back({"g", {"y", "z"}, "v_g_y_z", true});
		CHECK(p.edges.size() == 2);
	}
}

// ═════════════════════════════════════════════════════════════════════════════
// 6. TERM CLOSURE
// ═════════════════════════════════════════════════════════════════════════════

TEST_SUITE("OCFuncs - term closure") {
	TEST_CASE("term_closure_empty") {
		TermClosure tc;
		CHECK(tc.variables.empty());
		CHECK(tc.func_apps.empty());
		CHECK(tc.purified_vars.empty());
	}
	TEST_CASE("term_closure_with_variables") {
		TermClosure tc;
		tc.variables = {"x", "y", "z"};
		CHECK(tc.variables.size() == 3);
	}
	TEST_CASE("term_closure_with_func_apps") {
		TermClosure tc;
		tc.func_apps = {{"f", {"x"}}, {"g", {"y", "z"}}};
		CHECK(tc.func_apps.size() == 2);
	}
	TEST_CASE("term_closure_purification") {
		TermClosure tc;
		tc.purified_vars["f(x)"] = "v_f_x";
		tc.purified_vars["g(y,z)"] = "v_g_y_z";
		CHECK(tc.purified_vars.size() == 2);
		CHECK(tc.purified_vars["f(x)"] == "v_f_x");
	}
}

// ═════════════════════════════════════════════════════════════════════════════
// 7. SUPPORT BOUNDS
// ═════════════════════════════════════════════════════════════════════════════

TEST_SUITE("OCFuncs - support bounds") {
	TEST_CASE("support_bound_empty") {
		SupportBound sb;
		sb.func_name = "f";
		CHECK(sb.tuples.empty());
	}
	TEST_CASE("support_bound_single") {
		SupportBound sb;
		sb.func_name = "f";
		sb.tuples = {{"0"}};
		CHECK(sb.tuples.size() == 1);
	}
	TEST_CASE("support_bound_multiple") {
		SupportBound sb;
		sb.func_name = "g";
		sb.tuples = {{"0", "0"}, {"0", "1"}, {"1", "0"}, {"1", "1"}};
		CHECK(sb.tuples.size() == 4);
	}
}

// ═════════════════════════════════════════════════════════════════════════════
// 8. STATIC PROFILE STATE
// ═════════════════════════════════════════════════════════════════════════════

TEST_SUITE("OCFuncs - static profile state") {
	TEST_CASE("static_state_empty") {
		StaticProfileState s;
		s.func_name = "g";
		CHECK(s.bindings.empty());
	}
	TEST_CASE("static_state_single_binding") {
		StaticProfileState s;
		s.func_name = "g";
		s.bindings["0"] = "1";
		CHECK(s.bindings.size() == 1);
	}
	TEST_CASE("static_state_multiple_bindings") {
		StaticProfileState s;
		s.func_name = "g";
		s.bindings["0"] = "1";
		s.bindings["1"] = "0";
		CHECK(s.bindings.size() == 2);
	}
}

// ═════════════════════════════════════════════════════════════════════════════
// 9. STRESS TESTS — many function declarations
// ═════════════════════════════════════════════════════════════════════════════

TEST_SUITE("OCFuncs stress - 200 declarations") {
	TEST_CASE("stress_many_dynamic_funcs") {
		OCFuncsContext ctx;
		std::vector<FuncDecl> decls;
		for (int i = 0; i < 50; ++i) {
			decls.push_back({
				"dyn_" + std::to_string(i),
				FuncMode::DYNAMIC,
				{"Bool"},
				"Bool"
			});
		}
		CHECK(ocfuncs_check_modes(decls, ctx));
		CHECK(ctx.declarations.size() == 50);
	}
	TEST_CASE("stress_many_static_funcs") {
		OCFuncsContext ctx;
		std::vector<FuncDecl> decls;
		for (int i = 0; i < 50; ++i) {
			decls.push_back({
				"stat_" + std::to_string(i),
				FuncMode::STATIC,
				{"Bool"},
				"Bool"
			});
		}
		CHECK(ocfuncs_check_modes(decls, ctx));
		CHECK(ctx.static_states.size() == 50);
	}
	TEST_CASE("stress_mixed_funcs") {
		OCFuncsContext ctx;
		std::vector<FuncDecl> decls;
		for (int i = 0; i < 100; ++i) {
			decls.push_back({
				"mix_" + std::to_string(i),
				(i % 2 == 0) ? FuncMode::DYNAMIC : FuncMode::STATIC,
				{"Bool"},
				"Bool"
			});
		}
		CHECK(ocfuncs_check_modes(decls, ctx));
		CHECK(ctx.declarations.size() == 100);
		CHECK(ctx.static_states.size() == 50);
	}
	TEST_CASE("stress_varying_arity") {
		OCFuncsContext ctx;
		std::vector<FuncDecl> decls;
		for (int i = 1; i <= 20; ++i) {
			std::vector<std::string> args(i, "Bool");
			decls.push_back({
				"ary_" + std::to_string(i),
				FuncMode::DYNAMIC,
				args,
				"Bool"
			});
		}
		CHECK(ocfuncs_check_modes(decls, ctx));
		for (int i = 0; i < 20; ++i) {
			CHECK(ctx.declarations[i].arity() == (size_t)(i + 1));
		}
	}
	TEST_CASE("stress_varying_sorts") {
		OCFuncsContext ctx;
		std::vector<FuncDecl> decls;
		const char* sorts[] = {"Bool", "Int", "Real", "Nat", "String"};
		for (int i = 0; i < 20; ++i) {
			decls.push_back({
				"sort_" + std::to_string(i),
				FuncMode::DYNAMIC,
				{sorts[i % 5], sorts[(i + 1) % 5]},
				sorts[(i + 2) % 5]
			});
		}
		CHECK(ocfuncs_check_modes(decls, ctx));
	}
}

// ═════════════════════════════════════════════════════════════════════════════
// 10. PARSING STRESS — function applications in formulas
// ═════════════════════════════════════════════════════════════════════════════

TEST_SUITE("OCFuncs parsing stress - 200 formulas") {
	TEST_CASE("stress_func_app_eq_variants") {
		for (int i = 0; i < 40; ++i) {
			std::string fn = "f" + std::to_string(i);
			std::string fm = fn + "(o1[t]) = " + std::to_string(i % 2) + ".";
			auto r = get_nso_rr<node_t>(tau::get(fm.c_str()));
			CHECK(r.has_value());
		}
	}
	TEST_CASE("stress_func_app_multi_arg") {
		for (int arity = 1; arity <= 10; ++arity) {
			std::string fm = "f(";
			for (int j = 1; j <= arity; ++j) {
				if (j > 1) fm += ", ";
				fm += "o" + std::to_string(j) + "[t]";
			}
			fm += ") = 0.";
			auto r = get_nso_rr<node_t>(tau::get(fm.c_str()));
			CHECK(r.has_value());
		}
	}
	TEST_CASE("stress_func_app_in_temporal") {
		const char* temps[] = {"always", "sometimes", "F", "G"};
		for (int i = 0; i < 40; ++i) {
			std::string fm = temps[i % 4];
			fm += " f(o1[t]) = " + std::to_string(i % 2) + ".";
			auto r = get_nso_rr<node_t>(tau::get(fm.c_str()));
			CHECK(r.has_value());
		}
	}
	TEST_CASE("stress_func_app_conjunction") {
		for (int n = 1; n <= 10; ++n) {
			std::string fm = "";
			for (int i = 1; i <= n; ++i) {
				if (i > 1) fm += " && ";
				fm += "f" + std::to_string(i) + "(o1[t]) = " +
					std::to_string(i % 2);
			}
			fm += ".";
			auto r = get_nso_rr<node_t>(tau::get(fm.c_str()));
			CHECK(r.has_value());
		}
	}
	TEST_CASE("stress_func_app_disjunction") {
		for (int n = 1; n <= 10; ++n) {
			std::string fm = "";
			for (int i = 1; i <= n; ++i) {
				if (i > 1) fm += " || ";
				fm += "f" + std::to_string(i) + "(o1[t]) = " +
					std::to_string(i % 2);
			}
			fm += ".";
			auto r = get_nso_rr<node_t>(tau::get(fm.c_str()));
			CHECK(r.has_value());
		}
	}
	TEST_CASE("stress_func_app_with_A_E") {
		for (int i = 0; i < 20; ++i) {
			std::string prefix = (i % 2 == 0) ? "A " : "E ";
			std::string fm = prefix + "f(o1[t]) = " +
				std::to_string(i % 2) + ".";
			auto r = get_nso_rr<node_t>(tau::get(fm.c_str()));
			CHECK(r.has_value());
		}
	}
	TEST_CASE("stress_func_app_with_sem_neg") {
		for (int i = 0; i < 20; ++i) {
			std::string fm = "-(f(o" + std::to_string(i + 1) +
				"[t]) = " + std::to_string(i % 2) + ").";
			auto r = get_nso_rr<node_t>(tau::get(fm.c_str()));
			CHECK(r.has_value());
		}
	}
	TEST_CASE("stress_func_app_with_U") {
		for (int i = 0; i < 20; ++i) {
			std::string fm = "f(o1[t]) = 0 U f(o1[t]) = 1.";
			auto r = get_nso_rr<node_t>(tau::get(fm.c_str()));
			CHECK(r.has_value());
		}
	}
	TEST_CASE("stress_func_nested_func") {
		for (int depth = 1; depth <= 5; ++depth) {
			std::string inner = "o1[t]";
			for (int d = 0; d < depth; ++d) {
				inner = "f(" + inner + ")";
			}
			std::string fm = inner + " = 0.";
			auto r = get_nso_rr<node_t>(tau::get(fm.c_str()));
			CHECK(r.has_value());
		}
	}
	TEST_CASE("stress_func_app_lookback_variants") {
		for (int lb = 0; lb <= 9; ++lb) {
			std::string fm = "f(o1[t-" + std::to_string(lb) + "]) = 0.";
			auto r = get_nso_rr<node_t>(tau::get(fm.c_str()));
			CHECK(r.has_value());
		}
	}
}

// ═════════════════════════════════════════════════════════════════════════════
// N. LG-22 — Steps 2/3/5/6 instantiation + the V1 stub contract
// ═════════════════════════════════════════════════════════════════════════════
//
// Before this suite the file called only `ocfuncs_check_modes` and
// `ocfuncs_encode_static_profiles`.  Everything else — Steps 2, 3, 5, 6, both
// `ocfuncs_compile` overloads, `extract_func_decl`, `resolve_func_decl_types`
// and `has_func_applications` — had no test and no production caller, so as
// templates most had never been instantiated: not even compiled.
//
// These cases force instantiation and pin the ACTUAL V1 behaviour (Steps 3, 5
// and 6 are stubs), so the day someone implements them the tests say so.

TEST_SUITE("OCFuncs LG-22 - instantiation and V1 stub contract") {

	// Declares "g", the symbol every formula in this suite applies.
	//
	// It used to declare "f" while the formulas said `f(...)`; when the
	// formulas were re-targeted to `g(...)` — because `f` is also the tau
	// `false` constant — this was not moved with them, and the suite spent
	// several rounds asserting that an application of an UNDECLARED symbol is
	// detected.  It must not be: that is exactly the `h(...)` case below.
	static std::vector<FuncDecl> one_dynamic_decl() {
		std::vector<FuncDecl> decls;
		decls.emplace_back("g", FuncMode::DYNAMIC,
		                   std::vector<std::string>{"sbf"}, "sbf");
		return decls;
	}

	// Step 2 — window purification.  Instantiation + the func-app walk.
	TEST_CASE("[OCF-INST-01] ocfuncs_purify_window instantiates") {
		tref fm = create_spec("g(o1[t]) = 1.");
		REQUIRE(fm != nullptr);
		auto closure = ocfuncs_purify_window<node_t>(fm, one_dynamic_decl());
		CHECK(closure.func_apps.size() == closure.purified_vars.size());
	}

	TEST_CASE("[OCF-INST-02] ocfuncs_purify_window on a func-free formula") {
		tref fm = create_spec("o1[t] = 1.");
		REQUIRE(fm != nullptr);
		auto closure = ocfuncs_purify_window<node_t>(fm, one_dynamic_decl());
		CHECK(closure.func_apps.empty());
		CHECK(closure.purified_vars.empty());
	}

	// Step 3 — profile enumeration.  V1 emits exactly ONE hardcoded profile
	// (type_id = 0) when the closure has any purified edge, and none at all
	// when it is empty.  The closure is built by hand so the case does not
	// depend on how the parser shapes a function application.
	TEST_CASE("[OCF-INST-03] ocfuncs_enumerate_profiles is a single-profile stub") {
		OCFuncsContext ctx;
		auto decls = one_dynamic_decl();
		REQUIRE(ocfuncs_check_modes(decls, ctx));

		TermClosure closure;
		closure.purified_vars["g(o1)"] = "v_g_o1";
		auto profiles = ocfuncs_enumerate_profiles<node_t>(closure, ctx);
		CHECK(profiles.size() == 1);
		if (!profiles.empty()) {
			CHECK(profiles[0].type_id == 0);
			CHECK(profiles[0].edges.size() == 1);
		}

		CHECK(ocfuncs_enumerate_profiles<node_t>(TermClosure{}, ctx).empty());
	}

	// Step 5 — quantifier elimination is a pass-through in V1.
	TEST_CASE("[OCF-INST-04] ocfuncs_eliminate_quantifiers is a pass-through") {
		OCFuncsContext ctx;
		tref fm = create_spec("o1[t] = 1.");
		REQUIRE(fm != nullptr);
		CHECK(ocfuncs_eliminate_quantifiers<node_t>(fm, ctx) == fm);
	}

	// Step 6 — LTL generation leaves bf_func_app nodes alone in V1.
	TEST_CASE("[OCF-INST-05] ocfuncs_generate_ltl leaves func apps unchanged") {
		OCFuncsContext ctx;
		tref fm = create_spec("g(o1[t]) = 1.");
		REQUIRE(fm != nullptr);
		CHECK(ocfuncs_generate_ltl<node_t>(fm, ctx) == fm);
	}

	// `f` is also the tau `false` constant, so `f(...)` is not the clean
	// witness it looks like; `g` is unambiguous.  The positive direction is
	// checked on the RAW wff parse, which is what Steps 2 and 6 consume — a
	// window formula, not a resolved spec.
	//
	// This case found a real defect AND the reason for it.
	// `has_func_applications` tested only "some node's FIRST CHILD is a
	// bf_func_app" and answered false for every input.  Dumping the tree
	// showed why: `g(o1[t]) = 1` parses as
	//
	//     bf_eq > [ bf > bf_ref > ref > { sym "g", ref_args > ref_arg > … },
	//               bf > bf_t ]
	//
	// `bf_func_app` never appears, and the shape that does appear is exactly
	// a recursive-relation reference — so the question is only answerable
	// against the declaration set, which the function now takes.  A `sym`
	// carries its name as an interned id in `value.data`, the same way
	// `get_function_signature` reads it.  No test and no production caller is
	// what let this sit: precisely the class of latent breakage LG-22 is
	// about.
	TEST_CASE("[OCF-INST-06] has_func_applications instantiates and discriminates") {
		tau::get_options opts;
		opts.parse.start = tau::wff;
		tref raw_with    = tau::get("g(o1[t]) = 1", opts);
		tref raw_without = tau::get("o1[t] = 1", opts);
		REQUIRE(raw_with != nullptr);
		REQUIRE(raw_without != nullptr);
		const auto decls = one_dynamic_decl();   // declares "g"

		CHECK(has_func_applications<node_t>(raw_with, decls));
		CHECK_FALSE(has_func_applications<node_t>(raw_without, decls));

		// A reference to a symbol that is NOT declared as a function is a
		// recursive-relation call, not an application: at parse time the two
		// have the identical `bf_ref > ref > { sym, ref_args }` shape, so the
		// declarations are what tells them apart.
		tref other_ref = tau::get("h(o1[t]) = 1", opts);
		REQUIRE(other_ref != nullptr);
		CHECK_FALSE(has_func_applications<node_t>(other_ref, decls));

		// No declarations: nothing can be a function application.
		CHECK_FALSE(has_func_applications<node_t>(raw_with, {}));
	}

	// `resolve_sort_name_to_id` scanned ids 1..63 and relied on
	// `get_ba_type_name` returning type 0's name past the end of the
	// registry.  It THROWS there instead, so this call — the first ever
	// instantiation of the function — aborted with
	// "ba_types::name: invalid ba_type_id 7".  Registered names also carry a
	// leading ':' (":sbf"), so a bare "sbf" would never have matched either.
	TEST_CASE("[OCF-INST-07] resolve_func_decl_types instantiates") {
		auto decls = one_dynamic_decl();
		resolve_func_decl_types<node_t>(decls);
		REQUIRE(decls.size() == 1);
		resolve_func_decl_types<node_t>(decls[0]);
		REQUIRE(decls[0].arg_type_ids.size() == 1);
		CHECK(decls[0].arg_type_ids[0] != 0);
		CHECK(decls[0].result_type_id != 0);
		CHECK(decls[0].types_resolved());

		// An unknown sort resolves to 0 (untyped) rather than throwing.
		FuncDecl unknown("h", FuncMode::DYNAMIC,
		                 std::vector<std::string>{"no_such_sort"},
		                 "no_such_sort");
		resolve_func_decl_types<node_t>(unknown);
		CHECK(unknown.result_type_id == 0);
		CHECK_FALSE(unknown.types_resolved());
	}

	// The contract fix: while Steps 3/5/6 are stubs, `ocfuncs_compile` returns
	// the input essentially unchanged.  ocfuncs.h documents it as compiling
	// "to a propositional LTL formula suitable for synthesis" — anyone wiring
	// it up gets silent wrongness.  It must refuse, unconditionally, until
	// the steps are real.
	TEST_CASE("[OCF-INST-08] ocfuncs_compile refuses a formula with func apps") {
		tref fm = create_spec("g(o1[t]) = 1.");
		REQUIRE(fm != nullptr);
		CHECK(ocfuncs_compile<node_t>(fm, one_dynamic_decl()) == nullptr);
	}

	TEST_CASE("[OCF-INST-09] ocfuncs_compile refuses a func-free formula too") {
		tref fm = create_spec("o1[t] = 1.");
		REQUIRE(fm != nullptr);
		CHECK(ocfuncs_compile<node_t>(fm, one_dynamic_decl()) == nullptr);
	}

	// Step 1 still runs, and still rejects, before anything else does.
	//
	// The previous version of this case asserted that a STATIC declaration
	// without a support certificate is rejected — behind an `if` that never
	// fired, so it asserted nothing.  `ocfuncs_check_modes` in fact ACCEPTS
	// such a declaration and creates an empty support bound for it; what it
	// rejects is a malformed declaration.  Both behaviours are pinned here
	// unconditionally.
	TEST_CASE("[OCF-INST-10] Step 1 rejects malformed declarations") {
		tref fm = create_spec("o1[t] = 1.");
		REQUIRE(fm != nullptr);

		auto rejects = [&](const FuncDecl& d) {
			OCFuncsContext ctx;
			std::vector<FuncDecl> decls{d};
			return !ocfuncs_check_modes(decls, ctx);
		};
		CHECK(rejects(FuncDecl("", FuncMode::DYNAMIC,
		                       std::vector<std::string>{"sbf"}, "sbf")));
		CHECK(rejects(FuncDecl("k", FuncMode::DYNAMIC,
		                       std::vector<std::string>{}, "sbf")));
		CHECK(rejects(FuncDecl("k", FuncMode::DYNAMIC,
		                       std::vector<std::string>{"sbf"}, "")));

		// A STATIC declaration is accepted and gets an empty support bound;
		// the finite-support certificate is left to later analysis.
		OCFuncsContext ctx;
		std::vector<FuncDecl> st;
		st.emplace_back("g", FuncMode::STATIC,
		                std::vector<std::string>{"sbf"}, "sbf");
		CHECK(ocfuncs_check_modes(st, ctx));
		CHECK(ctx.support_bounds.count("g") == 1);

		// Whatever Step 1 says, V1 compile refuses (see OCF-INST-08/09).
		CHECK(ocfuncs_compile<node_t>(fm, st) == nullptr);
	}

}

