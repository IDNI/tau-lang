// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Direct tests for src/satisfiability.tmpl.h.
//
// Coverage gap closed by this file: before it, the only entry points of
// satisfiability.tmpl.h exercised anywhere in the test-suite were
// `is_tau_formula_sat` and `transform_to_execution` (see
// test_integration-satisfiability1.cpp .. 5.cpp). The public functions
// `is_tau_impl`, `are_tau_equivalent` and `simp_tau_unsat_valid` had no direct
// test at all, and none of the internal helpers had one either.

#include "test_integration-satisfiability_helper.h"

// -----------------------------------------------------------------------------
// local helpers
// -----------------------------------------------------------------------------

// Parse a bare wff (no trailing '.'), mirroring test_integration-wff_hooks.cpp
static tref wff_of(const char* s) {
	auto pwff = parse_wff();
	return tau::get(s, pwff);
}

// The `variable` nodes whose child is an io_var, exactly as every helper in
// satisfiability.tmpl.h collects them.
static trefs io_vars_of(tref fm) {
	return tau::get(fm).select_top(is_child<node_t, tau::io_var>);
}

static tref first_io_var(const char* s) {
	trefs vs = io_vars_of(wff_of(s));
	REQUIRE(vs.size() == 1);
	return vs[0];
}

// io_vars whose input/output classification is set. A bare `wff` parse leaves
// both is_input_variable() and is_output_variable() false -- the in/out data bit
// is assigned while parsing a *spec*, not derived from the `i`/`o` name prefix
// (verified: `i1[t-2]` parsed via parse_wff() reports in=0, out=0, and
// transform_io_var then takes its output branch for it). Any test that depends
// on the distinction must therefore go through create_spec.
static trefs spec_io_vars(const char* spec) {
	return io_vars_of(create_spec(spec));
}

// The matrix of `always <wff>.`, spec-parsed so its io_vars carry the in/out
// data bit. Any test that hands a whole formula to a helper which classifies
// io_vars must build it this way, not with wff_of.
static tref spec_always_body(const char* spec) {
	tref fm = create_spec(spec);
	REQUIRE(fm != nullptr);
	// The wff *wrapping* the wff_always, so trim2 yields the scoped formula.
	tref aw = tau::get(fm).find_top(is_child<node_t, tau::wff_always>);
	REQUIRE(aw != nullptr);
	return tau::trim2(aw);
}

static tref io_var_named(const trefs& vs, const std::string& name) {
	for (tref v : vs) if (get_var_name<node_t>(v) == name) return v;
	return nullptr;
}

// Build a bare `constraint` node (not wrapped in a `wff`).
//
// A `wff`-wrapped constraint cannot be used here: the `wff` hook dispatches
// `tau::constraint` to `get_hook::wff_ctn` (src/hooks.tmpl.h:553,780), which
// rewrites `ctn_eq` into `lteq && gteq` and `ctn_neq` into `lt || gt`. So
// neither parsing `[t = 3]` nor calling `tau::build_wff_ctn_eq` can ever yield
// a `ctn_eq` node. Building the `constraint` node directly is safe because
// `get_hook::operator()` (src/hooks.tmpl.h:20-23) only fires for `bf`, `wff`
// and `shift` node types.
static tref make_ctn(node_t::type kind, bool num_first, size_t n) {
	tref cv = tau::get(tau::ctnvar, std::string("t"));
	tref nm = tau::get_num(n);
	return tau::get(tau::constraint, num_first
		? tau::get(kind, { nm, cv })
		: tau::get(kind, { cv, nm }));
}

static bool str_has(const std::string& hay, const char* needle) {
	return hay.find(needle) != std::string::npos;
}

TEST_SUITE("Configuration") {
	TEST_CASE("bdd init") {
		bdd_init<Bool>();
	}
	TEST_CASE("logging") {
		// logging::trace();
	}
}

// -----------------------------------------------------------------------------
TEST_SUITE("satisfiability public API") {

	// Closes: `is_tau_impl` (src/satisfiability.tmpl.h:1517) had zero direct
	// tests. Formulas taken from its doc example at src/satisfiability.h:136-148.
	TEST_CASE("is_tau_impl: true case") {
		tref f1 = create_spec("always o1[t] = 1.");
		tref f2 = create_spec("always (o1[t] = 1 || o2[t] = 0).");
		CHECK( is_tau_impl<node_t>(f1, f2) );
	}

	TEST_CASE("is_tau_impl: false case") {
		// `always o1[t] = 1` says nothing about o2, so the negated
		// implication `(always o1[t] = 1) && (sometimes o2[t] != 0)` is
		// satisfiable and the implication does not hold.
		tref f1 = create_spec("always o1[t] = 1.");
		tref f2 = create_spec("always o2[t] = 0.");
		CHECK( !is_tau_impl<node_t>(f1, f2) );
	}

	// Closes: the "is fm valid" idiom `is_tau_impl(tau::_T(), fm)` used at
	// src/api.tmpl.h:439 and src/boolean_algebras/tau_ba.tmpl.h:105 was never
	// invoked directly from a test.
	TEST_CASE("is_tau_impl: validity idiom with _T() antecedent") {
		// A tautology: o1[t] is either 1 or not 1 at every time step.
		tref valid_fm = create_spec("always (o1[t] = 1 || o1[t] != 1).");
		CHECK( is_tau_impl<node_t>(tau::_T(), valid_fm) );
		// Not a tautology: o1 need not be constantly 1.
		tref not_valid_fm = create_spec("always o1[t] = 1.");
		CHECK( !is_tau_impl<node_t>(tau::_T(), not_valid_fm) );
	}

	// Closes: `are_tau_equivalent` (src/satisfiability.tmpl.h:1533) had zero
	// direct tests. Formulas from its doc example at src/satisfiability.h:162-171.
	TEST_CASE("are_tau_equivalent: equivalent pair") {
		tref f1 = create_spec("always o1[t] = 1.");
		tref f2 = create_spec("always !(o1[t] != 1).");
		CHECK( are_tau_equivalent<node_t>(f1, f2) );
	}

	TEST_CASE("are_tau_equivalent: non-equivalent pair") {
		tref f1 = create_spec("always o1[t] = 1.");
		tref f2 = create_spec("always o1[t] = 0.");
		CHECK( !are_tau_equivalent<node_t>(f1, f2) );
	}

	// Closes: `simp_tau_unsat_valid` (src/satisfiability.tmpl.h:1549) had zero
	// direct tests. The valid case exercises the early `is_tau_impl(_T(), fm)`
	// return at line 1553.
	TEST_CASE("simp_tau_unsat_valid: valid formula collapses to T") {
		tref fm = create_spec("always (o1[t] = 1 || o1[t] != 1).");
		CHECK( simp_tau_unsat_valid<node_t>(fm) == tau::_T() );
	}

	// Exercises the clause-filtering loop at src/satisfiability.tmpl.h:1557-1561:
	// the unsatisfiable disjunct is dropped, the satisfiable one survives.
	TEST_CASE("simp_tau_unsat_valid: drops an unsatisfiable disjunct") {
		// First disjunct is unsatisfiable: o2[t] = 0 together with
		// o2[t-1] = o2[t] forces o2[t-1] = 0, contradicting o2[t-1] != 0.
		// Second disjunct is satisfiable.
		tref fm = create_spec(
			"(always (o2[t] = 0 && o2[t-1] = o2[t] && o2[t-1] != 0))"
			" || (always o1[t] = 1).");
		tref res = simp_tau_unsat_valid<node_t>(fm);
		REQUIRE( res != nullptr );
		CHECK( !tau::get(res).equals_F() );
		// simp_tau_unsat_valid guarantees no particular normal form for the
		// surviving disjuncts, so only satisfiability-equivalence is asserted.
		CHECK( is_tau_formula_sat<node_t>(res) );
	}

	// The whole formula is unsatisfiable, so `clauses` stays `{ _F() }` and
	// `build_wff_or({ _F() })` reduces back to `_F()`.
	TEST_CASE("simp_tau_unsat_valid: fully unsatisfiable formula yields F") {
		// From test_integration-satisfiability1.cpp:13 (known unsat).
		tref fm = create_spec(
			"(always o1[t-1] = 0) && (sometimes o1[t] = 1 && o1[t-1] = 0).");
		tref res = simp_tau_unsat_valid<node_t>(fm);
		REQUIRE( res != nullptr );
		CHECK( tau::get(res).equals_F() );
	}
}

// -----------------------------------------------------------------------------
TEST_SUITE("satisfiability helpers") {

	// Closes: `transform_io_var` (src/satisfiability.tmpl.h:109) was only ever
	// reached indirectly. The three cases are the ones argued in its doc
	// comment at src/satisfiability.tmpl.h:100-106.
	TEST_CASE("transform_io_var: relative, current and constant time points") {
		// Every case here goes through create_spec, because the in/out data
		// bit is assigned by *spec* parsing (see spec_io_vars). This used to
		// use bare wff parses, which left the io_vars classified as neither --
		// and the function's `else` branch then rebuilt them as outputs, which
		// happens to be the right answer for an `o`-named variable. So the
		// test passed while pinning the defect NEW-1 describes; it now
		// exercises the real is_output_variable() branch.
		trefs prev_vars = spec_io_vars("always o1[t-1] = 0.");
		tref v_prev = io_var_named(prev_vars, "o1");
		REQUIRE( v_prev != nullptr );
		REQUIRE( tau::get(v_prev).is_output_variable() );
		REQUIRE( get_io_var_shift<node_t>(v_prev) == 1 );
		tref r_prev = transform_io_var<node_t>(v_prev, 5);
		CHECK( is_io_initial<node_t>(r_prev) );
		CHECK( get_io_time_point<node_t>(r_prev) == 4 );
		CHECK( get_var_name<node_t>(r_prev) == "o1" );
		// UNVERIFIED: expected printed form of a lone `variable` node holding
		// an output io_var at a constant time point; derived from the printer
		// (src/tau_tree_printers.tmpl.h:813-827) and from the io-var spelling
		// used in test_integration-interpreter.cpp expectations.
		CHECK( tau::get(r_prev).to_str() == "o1[4]:tau" );

		// o1[t] @ 5 -> o1[5]  (shift 0)
		tref v_now = io_var_named(spec_io_vars("always o1[t] = 0."), "o1");
		REQUIRE( v_now != nullptr );
		REQUIRE( get_io_var_shift<node_t>(v_now) == 0 );
		tref r_now = transform_io_var<node_t>(v_now, 5);
		CHECK( is_io_initial<node_t>(r_now) );
		CHECK( get_io_time_point<node_t>(r_now) == 5 );

		// o1[0] is already constant and must be returned unchanged. This one
		// takes the is_io_initial early return before classification is ever
		// consulted, so a bare wff parse is fine here.
		tref v_const = first_io_var("o1[0] = 0");
		REQUIRE( is_io_initial<node_t>(v_const) );
		CHECK( transform_io_var<node_t>(v_const, 5) == v_const );

		// input streams take the build_in_var_at_n branch -- reachable only
		// with a classified io_var, hence the spec parse (see spec_io_vars)
		tref v_in = io_var_named(spec_io_vars("always i1[t-2] = 0."), "i1");
		REQUIRE( v_in != nullptr );
		REQUIRE( tau::get(v_in).is_input_variable() );
		tref r_in = transform_io_var<node_t>(v_in, 5);
		CHECK( tau::get(r_in).is_input_variable() );
		CHECK( get_io_time_point<node_t>(r_in) == 3 );
	}

	// Closes: `fm_at_time_point` (src/satisfiability.tmpl.h:277) is declared
	// public in src/satisfiability.h:41 but had no test.
	TEST_CASE("fm_at_time_point: instantiates a whole formula") {
		// Spec-parsed: fm_at_time_point calls transform_io_var, which needs the
		// in/out classification (see spec_io_vars). A wff_of parse leaves it
		// unset, and the old `else`-defaults-to-output behaviour made that look
		// like it worked for `o`-named variables.
		tref fm = spec_always_body("always o1[t] = o1[t-1].");
		REQUIRE( fm != nullptr );
		trefs vs = io_vars_of(fm);
		REQUIRE( vs.size() == 2 );
		tref res = fm_at_time_point<node_t>(fm, vs, 5);
		REQUIRE( res != nullptr );
		trefs rvs = io_vars_of(res);
		REQUIRE( rvs.size() == 2 );
		bool has4 = false, has5 = false;
		for (tref v : rvs) {
			CHECK( is_io_initial<node_t>(v) );
			int_t tp = get_io_time_point<node_t>(v);
			if (tp == 4) has4 = true;
			if (tp == 5) has5 = true;
		}
		CHECK( has4 );
		CHECK( has5 );
		// UNVERIFIED: expected printed form; the substitution is structural so
		// the shape is preserved, but the conjunct/operand order after
		// re-interning through the hooks is not something the code guarantees.
		CHECK( tau::get(res).to_str() == "o1[5]:tau = o1[4]:tau" );
	}

	TEST_CASE("fm_at_time_point: constant time points are left alone") {
		// Spec-parsed for the same reason as the case above.
		tref fm = spec_always_body("always o1[t] = o1[0].");
		REQUIRE( fm != nullptr );
		trefs vs = io_vars_of(fm);
		REQUIRE( vs.size() == 2 );
		tref res = fm_at_time_point<node_t>(fm, vs, 5);
		trefs rvs = io_vars_of(res);
		REQUIRE( rvs.size() == 2 );
		bool has0 = false, has5 = false;
		for (tref v : rvs) {
			int_t tp = get_io_time_point<node_t>(v);
			if (tp == 0) has0 = true;
			if (tp == 5) has5 = true;
		}
		CHECK( has0 );
		CHECK( has5 );
	}

	// Closes: `calculate_ctn` (src/satisfiability.tmpl.h:200) had no test at
	// all, and in particular its `is_left` operand-order handling
	// (lines 213-230) was completely unverified.
#ifdef TAU_PACK_HAS_BA_SBF
	TEST_CASE("calculate_ctn: all six kinds, both operand orders") {
		// calculate_ctn (src/satisfiability.tmpl.h) types its Boolean
		// constants through pack_bool_carrier_type, not a hardcoded sbf --
		// core naming no BA (AGENTS.md). Match that here instead of
		// assuming sbf specifically, which only holds when sbf outranks
		// every other carrier the configured pack offers.
		const size_t carrier_tid =
			get_ba_type_id<node_t>(pack_bool_carrier_type<node_t>());
		tref ba_1 = tau::_1(carrier_tid), ba_0 = tau::_0(carrier_tid);

		// equality / inequality are operand-order independent by construction
		for (bool num_first : { false, true }) {
			tref eq = make_ctn(tau::ctn_eq, num_first, 3);
			CHECK( calculate_ctn<node_t>(eq, 3) == ba_1 );
			CHECK( calculate_ctn<node_t>(eq, 2) == ba_0 );

			tref neq = make_ctn(tau::ctn_neq, num_first, 3);
			CHECK( calculate_ctn<node_t>(neq, 3) == ba_0 );
			CHECK( calculate_ctn<node_t>(neq, 2) == ba_1 );
		}

		// `[t >= 3]` (num on the right) means time_point >= 3
		tref t_gteq_3 = make_ctn(tau::ctn_gteq, false, 3);
		CHECK( calculate_ctn<node_t>(t_gteq_3, 2) == ba_0 );
		CHECK( calculate_ctn<node_t>(t_gteq_3, 3) == ba_1 );
		CHECK( calculate_ctn<node_t>(t_gteq_3, 4) == ba_1 );
		// `[3 >= t]` (num on the left) means 3 >= time_point
		tref three_gteq_t = make_ctn(tau::ctn_gteq, true, 3);
		CHECK( calculate_ctn<node_t>(three_gteq_t, 2) == ba_1 );
		CHECK( calculate_ctn<node_t>(three_gteq_t, 3) == ba_1 );
		CHECK( calculate_ctn<node_t>(three_gteq_t, 4) == ba_0 );

		// `[t > 3]` vs `[3 > t]`
		tref t_gt_3 = make_ctn(tau::ctn_gt, false, 3);
		CHECK( calculate_ctn<node_t>(t_gt_3, 3) == ba_0 );
		CHECK( calculate_ctn<node_t>(t_gt_3, 4) == ba_1 );
		tref three_gt_t = make_ctn(tau::ctn_gt, true, 3);
		CHECK( calculate_ctn<node_t>(three_gt_t, 2) == ba_1 );
		CHECK( calculate_ctn<node_t>(three_gt_t, 3) == ba_0 );

		// `[t <= 3]` vs `[3 <= t]` (the doc example at
		// src/satisfiability.tmpl.h:194-197 uses `t <= 3`)
		tref t_lteq_3 = make_ctn(tau::ctn_lteq, false, 3);
		CHECK( calculate_ctn<node_t>(t_lteq_3, 2) == ba_1 );
		CHECK( calculate_ctn<node_t>(t_lteq_3, 3) == ba_1 );
		CHECK( calculate_ctn<node_t>(t_lteq_3, 5) == ba_0 );
		tref three_lteq_t = make_ctn(tau::ctn_lteq, true, 3);
		CHECK( calculate_ctn<node_t>(three_lteq_t, 2) == ba_0 );
		CHECK( calculate_ctn<node_t>(three_lteq_t, 3) == ba_1 );
		CHECK( calculate_ctn<node_t>(three_lteq_t, 5) == ba_1 );

		// `[t < 3]` vs `[3 < t]`
		tref t_lt_3 = make_ctn(tau::ctn_lt, false, 3);
		CHECK( calculate_ctn<node_t>(t_lt_3, 2) == ba_1 );
		CHECK( calculate_ctn<node_t>(t_lt_3, 3) == ba_0 );
		tref three_lt_t = make_ctn(tau::ctn_lt, true, 3);
		CHECK( calculate_ctn<node_t>(three_lt_t, 3) == ba_0 );
		CHECK( calculate_ctn<node_t>(three_lt_t, 4) == ba_1 );
	}
#endif // TAU_PACK_HAS_BA_SBF

	// Closes: `is_initial_ctn_phase` (src/satisfiability.tmpl.h:257) had no
	// test. Unlike calculate_ctn it reads `condition` without looking at which
	// side the numeral is on, i.e. it is deliberately side-independent; that
	// property was entirely unverified. Equality/inequality kinds are excluded
	// because the function DBG-asserts they were converted away already.
	TEST_CASE("is_initial_ctn_phase: boundaries and side-independence") {
		for (bool num_first : { false, true }) {
			// gteq: condition >= time_point, condition == 3
			tref gteq = make_ctn(tau::ctn_gteq, num_first, 3);
			CHECK(  is_initial_ctn_phase<node_t>(gteq, 0) );
			CHECK(  is_initial_ctn_phase<node_t>(gteq, 3) );
			CHECK( !is_initial_ctn_phase<node_t>(gteq, 4) );

			// gt: condition + 1 >= time_point
			tref gt = make_ctn(tau::ctn_gt, num_first, 3);
			CHECK(  is_initial_ctn_phase<node_t>(gt, 4) );
			CHECK( !is_initial_ctn_phase<node_t>(gt, 5) );

			// lteq: condition + 1 >= time_point (doc example: t <= 3 is in
			// the initial phase for time_point in {0..4})
			tref lteq = make_ctn(tau::ctn_lteq, num_first, 3);
			CHECK(  is_initial_ctn_phase<node_t>(lteq, 0) );
			CHECK(  is_initial_ctn_phase<node_t>(lteq, 4) );
			CHECK( !is_initial_ctn_phase<node_t>(lteq, 5) );

			// lt: condition >= time_point
			tref lt = make_ctn(tau::ctn_lt, num_first, 3);
			CHECK(  is_initial_ctn_phase<node_t>(lt, 3) );
			CHECK( !is_initial_ctn_phase<node_t>(lt, 4) );
		}
	}

	// Closes: `constant_io_comp` (src/satisfiability.tmpl.h:385) had no test.
	// Case taken from its doc example at src/satisfiability.tmpl.h:377-381.
	TEST_CASE("constant_io_comp: sorts { o1[2], i1[2], o1[0] }") {
		trefs vs = {
			tau::trim(build_out_var_at_n<node_t>("o1", 2, 0)),
			tau::trim(build_in_var_at_n<node_t>("i1", 2, 0)),
			tau::trim(build_out_var_at_n<node_t>("o1", 0, 0)),
		};
		std::sort(vs.begin(), vs.end(), constant_io_comp<node_t>);
		// ascending time point first
		CHECK( get_io_time_point<node_t>(vs[0]) == 0 );
		CHECK( tau::get(vs[0]).is_output_variable() );
		// tie on time point 2: input before output
		CHECK( get_io_time_point<node_t>(vs[1]) == 2 );
		CHECK( tau::get(vs[1]).is_input_variable() );
		CHECK( get_io_time_point<node_t>(vs[2]) == 2 );
		CHECK( tau::get(vs[2]).is_output_variable() );
	}

	// Closes: `is_run_satisfiable` (src/satisfiability.tmpl.h:440) had no
	// test. The two short-circuits at lines 448-449 and the two quantification
	// branches at lines 466-468 are pinned here. Formulas from the doc example
	// at src/satisfiability.tmpl.h:425-437.
	TEST_CASE("is_run_satisfiable: T and F short-circuits") {
		CHECK(  is_run_satisfiable<node_t>(tau::_T()) );
		CHECK( !is_run_satisfiable<node_t>(tau::_F()) );
	}

	TEST_CASE("is_run_satisfiable: satisfiable fully instantiated run") {
		// The always-part of "smaller_lookback_one_st"
		// (test_integration-satisfiability1.cpp:17) instantiated at 0 and 1:
		// an output stream constantly equal to 1 exists.
		tref run = wff_of("o1[0] = 1 && o1[1] = o1[0]");
		REQUIRE( run != nullptr );
		CHECK( is_run_satisfiable<node_t>(run) );
	}

	TEST_CASE("is_run_satisfiable: unsatisfiable fully instantiated run") {
		// o1[1] = o1[0] = 0 contradicts o1[1] != 0. Output streams are
		// existentially quantified, so no assignment exists.
		tref run = wff_of("o1[0] = 0 && o1[1] = o1[0] && o1[1] != 0");
		REQUIRE( run != nullptr );
		CHECK( !is_run_satisfiable<node_t>(run) );
	}

	TEST_CASE("is_run_satisfiable: input streams are universally quantified") {
		// `i1[0] = 0` does not hold for every value of the input stream, so
		// the universally quantified run is unsatisfiable. The input/output
		// distinction only exists for spec-parsed io_vars (see
		// spec_io_vars), so the run has to be built from one.
		tref in_run = tau::trim2(create_spec("always i1[0] = 0."));
		REQUIRE( in_run != nullptr );
		REQUIRE( io_var_named(io_vars_of(in_run), "i1") != nullptr );
		REQUIRE( tau::get(io_var_named(io_vars_of(in_run), "i1"))
			.is_input_variable() );
		CHECK( !is_run_satisfiable<node_t>(in_run) );
		// ...while the same formula over an output stream is satisfiable.
		tref out_run = tau::trim2(create_spec("always o1[0] = 0."));
		REQUIRE( out_run != nullptr );
		CHECK( is_run_satisfiable<node_t>(out_run) );
	}

	// Closes: `get_lookback_after_normalization` (src/satisfiability.tmpl.h:67)
	// had no test; `get_max_shift`/`get_max_initial` had no direct test on a
	// hand-built io_var vector either. Case from the doc example at
	// src/satisfiability.tmpl.h:60-64, extended with a shift and a temporary.
	TEST_CASE("get_lookback_after_normalization / get_max_shift / get_max_initial") {
		trefs vs = {
			tau::trim(build_out_var_at_n<node_t>("o1", 0, 0)),
			tau::trim(build_out_var_at_n<node_t>("o1", 2, 0)),
			tau::trim(build_in_var_at_t<node_t>(
				build_var_name<node_t>("i1"), 0, "t")),
			tau::trim(build_out_var_at_t_minus<node_t>("o2", 3, 0, "t")),
			tau::trim(build_out_var_at_t_minus<node_t>("_f0", 5, 0, "t")),
		};
		// only the constant-time variables count, largest wins
		CHECK( get_lookback_after_normalization<node_t>(vs) == 2 );
		CHECK( get_max_initial<node_t>(vs) == 2 );
		// largest relative shift, temporaries included
		CHECK( get_max_shift<node_t>(vs) == 5 );
		// ...and excluded (the `_`-prefixed flag stream is skipped)
		CHECK( get_max_shift<node_t>(vs, true) == 3 );

		// degenerate inputs
		trefs empty;
		CHECK( get_lookback_after_normalization<node_t>(empty) == 0 );
		CHECK( get_max_shift<node_t>(empty) == 0 );
		CHECK( get_max_initial<node_t>(empty) == -1 );
	}

	// Closes: `has_temporary_io_var` (src/satisfiability.tmpl.h:79) had no test.
	TEST_CASE("has_temporary_io_var") {
		tref with_temp = tau::build_bf_eq_0(
			build_out_var_at_n<node_t>("_f0", 0, 0));
		REQUIRE( with_temp != nullptr );
		CHECK( has_temporary_io_var<node_t>(with_temp) );

		tref without = wff_of("o1[0] = 0 && i1[t] = 1");
		REQUIRE( without != nullptr );
		CHECK( !has_temporary_io_var<node_t>(without) );
	}

	// Closes: `create_guard` (src/satisfiability.tmpl.h:1000) had no test.
	TEST_CASE("create_guard: T without input streams, one uconst per input") {
		// Classification-dependent: create_guard conjoins one uconst per
		// *input* stream, and a bare wff parse leaves every io_var
		// unclassified (see spec_io_vars).
		trefs out_only = spec_io_vars("always o1[t] = 0.");
		REQUIRE( out_only.size() == 1 );
		CHECK( create_guard<node_t>(out_only, 0) == tau::_T() );

		trefs with_inputs =
			spec_io_vars("always o1[t] = i1[t] & i2[t].");
		REQUIRE( with_inputs.size() == 3 );
		tref guard = create_guard<node_t>(with_inputs, 0);
		REQUIRE( guard != nullptr );
		CHECK( guard != tau::_T() );
		// one uninterpreted constant per input stream
		trefs ucs = tau::get(guard)
			.select_top(is_child<node_t, tau::uconst_name>);
		CHECK( ucs.size() == 2 );
	}

	// Closes: SO-7. `transform_ctn_to_streams`
	// (src/satisfiability.tmpl.h:794) numbers its generated flag streams from a
	// function-local *static* counter (line 816), so results depend on call
	// history unless `reset_ctn_id` is passed. Nothing pinned that before.
	TEST_CASE("transform_ctn_to_streams: static flag counter and reset") {
		tref fm = wff_of("[t <= 3] && o1[t] = 0");
		REQUIRE( fm != nullptr );
		REQUIRE( tau::get(fm).find_top(is<node_t, tau::constraint>) != nullptr );

		// reset first, so the test does not depend on earlier test cases
		tref inits = tau::_T(), rules = tau::_T();
		tref r0 = transform_ctn_to_streams<node_t>(fm, inits, rules, 0, 0, true);
		REQUIRE( r0 != nullptr );
		std::string s0 = tau::get(r0).to_str();
		CHECK( str_has(s0, "_f0") );
		// the constraint is gone, replaced by the flag stream
		CHECK( tau::get(r0).find_top(is<node_t, tau::constraint>) == nullptr );
		// `t <= 3` with start_time 0 is in its initial phase for t in {0..4},
		// so initial conditions and a recurrence rule were produced
		CHECK( inits != tau::_T() );
		CHECK( rules != tau::_T() );

		// no reset: the static counter keeps counting up
		tref inits1 = tau::_T(), rules1 = tau::_T();
		tref r1 = transform_ctn_to_streams<node_t>(fm, inits1, rules1, 0, 0,
									false);
		std::string s1 = tau::get(r1).to_str();
		CHECK( str_has(s1, "_f1") );
		CHECK( !str_has(s1, "_f0") );

		tref inits2 = tau::_T(), rules2 = tau::_T();
		tref r2 = transform_ctn_to_streams<node_t>(fm, inits2, rules2, 0, 0,
									false);
		CHECK( str_has(tau::get(r2).to_str(), "_f2") );

		// reset again: numbering restarts at _f0 and the very first result is
		// reproduced exactly
		tref inits3 = tau::_T(), rules3 = tau::_T();
		tref r3 = transform_ctn_to_streams<node_t>(fm, inits3, rules3, 0, 0,
									true);
		CHECK( str_has(tau::get(r3).to_str(), "_f0") );
		CHECK( r3 == r0 );
	}

	TEST_CASE("transform_ctn_to_streams: formula without constraints is returned as is") {
		tref fm = wff_of("o1[t] = 0");
		tref inits = tau::_T(), rules = tau::_T();
		tref res = transform_ctn_to_streams<node_t>(fm, inits, rules, 0, 0,
									true);
		CHECK( res == fm );
		// flag_initials is unconditionally (re)set to T at line 812
		CHECK( inits == tau::_T() );
		CHECK( rules == tau::_T() );
	}

	// Closes: `transform_back_non_initials` (src/satisfiability.tmpl.h:700)
	// had no test. With io_vars at constant time points 2 and 3 the lookback
	// is 3, so o1[3] becomes o1[t] (3 - 3 == 0) and o1[2] becomes o1[t-1].
	TEST_CASE("transform_back_non_initials: constant time points become relative") {
		tref fm = wff_of("o1[3] = 0 && o1[2] = 0");
		REQUIRE( fm != nullptr );
		REQUIRE( io_vars_of(fm).size() == 2 );
		tref res = transform_back_non_initials<node_t>(fm, 1);
		REQUIRE( res != nullptr );
		trefs rvs = io_vars_of(res);
		REQUIRE( rvs.size() == 2 );
		bool shift0 = false, shift1 = false;
		for (tref v : rvs) {
			CHECK( !is_io_initial<node_t>(v) );
			int_t sh = get_io_var_shift<node_t>(v);
			if (sh == 0) shift0 = true;
			if (sh == 1) shift1 = true;
		}
		CHECK( shift0 );
		CHECK( shift1 );
	}

	TEST_CASE("transform_back_non_initials: variables at or below the bound stay put") {
		// highest_init_cond = 3 skips both io_vars (line 714), so nothing
		// is rewritten.
		tref fm = wff_of("o1[3] = 0 && o1[2] = 0");
		tref res = transform_back_non_initials<node_t>(fm, 3);
		REQUIRE( res != nullptr );
		for (tref v : io_vars_of(res))
			CHECK( is_io_initial<node_t>(v) );
	}

	// Closes: `make_initial_run` (src/satisfiability.tmpl.h:1203) had no test.
	TEST_CASE("make_initial_run: nullptr for zero lookback, instantiated run otherwise") {
		// Spec-parsed: make_initial_run instantiates through
		// transform_io_var, which needs the in/out classification.
		tref aw = spec_always_body("always o1[t] = o1[t-1].");
		REQUIRE( aw != nullptr );
		// the loop body never runs, so `run` stays nullptr
		CHECK( make_initial_run<node_t>(aw, 0) == nullptr );

		tref run = make_initial_run<node_t>(aw, 2);
		REQUIRE( run != nullptr );
		// every io_var of the run refers to a constant time point
		trefs rvs = io_vars_of(run);
		CHECK( !rvs.empty() );
		for (tref v : rvs) CHECK( is_io_initial<node_t>(v) );
	}

	// Closes: `transform_to_eventual_variables`
	// (src/satisfiability.tmpl.h:1056) had no test. The no-`sometimes`
	// early return at line 1062 is the identity, which
	// `transform_to_execution` relies on to detect non-temporal formulas.
	TEST_CASE("transform_to_eventual_variables: identity without a sometimes") {
		tref fm = create_spec("always o1[t] = 1.");
		auto res = transform_to_eventual_variables<node_t>(fm, true, 0);
		CHECK( res.first == fm );
		CHECK( res.second == 0 );
	}

	TEST_CASE("transform_to_eventual_variables: introduces an _eN flag stream") {
		tref fm = create_spec(
			"(always o1[t] = 1) && (sometimes o2[t-2] = 0).");
		auto res = transform_to_eventual_variables<node_t>(fm, true, 0);
		REQUIRE( res.first != nullptr );
		CHECK( res.first != fm );
		// max_st_lookback is the greatest lookback among the `sometimes`
		// clauses only (select_top_until stops at the always-part)
		CHECK( res.second == 2 );
		// a generated `_e0` stream must now be present
		CHECK( has_temporary_io_var<node_t>(res.first) );
		// the result keeps a `sometimes` clause tracking the flags
		CHECK( tau::get(res.first)
			.find_top(is_child<node_t, tau::wff_sometimes>) != nullptr );
	}

	// Closes: `always_to_unbounded_continuation`
	// (src/satisfiability.tmpl.h:904) had no test. The `_F()` return at
	// line 980 (run became unsatisfiable) is the interesting one.
	TEST_CASE("always_to_unbounded_continuation: unsatisfiable always-part yields F") {
		// o1[t] = 0 with o1[t-1] = o1[t] forces o1[t-1] = 0, contradicting
		// o1[t-1] != 0, so the very first run is already unsatisfiable.
		tref spec = create_spec(
			"always (o1[t] = 0 && o1[t-1] = o1[t] && o1[t-1] != 0).");
		tref aw = tau::get(spec).find_top(is_child<node_t, tau::wff_always>);
		REQUIRE( aw != nullptr );
		tref res = always_to_unbounded_continuation<node_t>(aw, 0, false);
		CHECK( res == tau::_F() );
	}

	TEST_CASE("always_to_unbounded_continuation: satisfiable always-part is not F") {
		// Always-part of "smaller_lookback_one_st"
		// (test_integration-satisfiability1.cpp:17): o1 constantly 1.
		tref spec = create_spec("always o1[t] = o1[t-1] && o1[t-1] = 1.");
		tref aw = tau::get(spec).find_top(is_child<node_t, tau::wff_always>);
		REQUIRE( aw != nullptr );
		tref res = always_to_unbounded_continuation<node_t>(aw, 0, false);
		REQUIRE( res != nullptr );
		CHECK( !tau::get(res).equals_F() );
	}
}

// -----------------------------------------------------------------------------
TEST_SUITE("satisfiability regression") {

	// Closes: the unresolved-`ref` guard at
	// src/satisfiability.tmpl.h:1411-1416 was unpinned. An unresolved
	// function/predicate symbol must make transform_to_execution report unsat
	// rather than proceed with a formula it cannot decide.
	TEST_CASE("transform_to_execution: unresolved ref returns F") {
		// `f` has no recurrence relation defining it, so it survives as a
		// `ref` node under `wff_ref` (the bare `f(x).` spec shape is the one
		// used in test_integration-nso_rr_fixed_point.cpp:84).
		tref fm = create_spec("f(x).");
		REQUIRE( fm != nullptr );
		REQUIRE( tau::get(fm).find_top(is<node_t, tau::ref>) != nullptr );
		CHECK( transform_to_execution<node_t>(fm) == tau::_F() );
	}

	// Closes: the purely non-temporal branch at
	// src/satisfiability.tmpl.h:1451-1466 was unpinned: with no always-part
	// and no sometimes-part the decision is delegated to
	// is_non_temp_nso_satisfiable, and the formula itself is returned when
	// satisfiable.
	TEST_CASE("transform_to_execution: non-temporal unsatisfiable formula returns F") {
		tref fm = create_spec("o1[0] = 0 && o1[1] = o1[0] && o1[1] != 0.");
		REQUIRE( fm != nullptr );
		REQUIRE( tau::get(fm)
			.find_top(is_child<node_t, tau::wff_always>) == nullptr );
		REQUIRE( tau::get(fm)
			.find_top(is_child<node_t, tau::wff_sometimes>) == nullptr );
		CHECK( transform_to_execution<node_t>(fm) == tau::_F() );
	}

	TEST_CASE("transform_to_execution: non-temporal satisfiable formula is returned unchanged") {
		tref fm = create_spec("o1[0] = 0 && o1[1] = o1[0].");
		REQUIRE( fm != nullptr );
		tref res = transform_to_execution<node_t>(fm);
		REQUIRE( res != nullptr );
		CHECK( !tau::get(res).equals_F() );
		// `elim_aw(fm)` is the identity here: there is no `wff_always` child
		CHECK( res == fm );
	}
}

	// Closes: SO-16. `get_uninterpreted_constants_constraints` is a live
	// interpreter dependency (model computation for uninterpreted guard
	// constants during spec execution) with zero direct coverage.
	TEST_CASE("get_uninterpreted_constants_constraints: guarded continuation") {
		trefs with_inputs = spec_io_vars("always o1[t] = i1[t].");
		REQUIRE( with_inputs.size() == 2 );
		tref guard = create_guard<node_t>(with_inputs, 0);
		REQUIRE( guard != nullptr );
		REQUIRE( guard != tau::_T() );
		// The guard alone is a valid unbound continuation: its io_vars
		// sit at constant time positions, per the function's contract.
		trefs io_vars = tau::get(guard)
			.select_top(is_child<node_t, tau::io_var>);
		REQUIRE( !io_vars.empty() );
		tref ctns = get_uninterpreted_constants_constraints<node_t>(
			guard, io_vars, 0);
		REQUIRE( ctns != nullptr );
		// The function computes a MODEL for the guard's uninterpreted
		// constants: no uconst survives into the returned constraints.
		CHECK( tau::get(ctns).find_top(
			is_child<node_t, tau::uconst_name>) == nullptr );
		// io_vars is rewritten to constant-time instances by contract
		for (tref v : io_vars)
			CHECK( is_io_initial<node_t>(v) );
	}
