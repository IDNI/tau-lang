// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Coverage for the normalization/normalizer helpers that the 2026-07-30 review
// found to have no direct test at all (report sections 4.1/4.2):
// denorm_equation, syntactic_variable_simplification,
// syntactic_atomic_formula_simplification, term_boole_decomposition (both
// overloads), term_boole_normal_form, is_non_temp_nso_unsat,
// eliminate_bv_and_quantifiers, is_valid, is_well_founded,
// calculate_fixed_point, plus the fold_trivial_quantifiers and
// push_negation_one_in branches added while fixing NZ-9 and NF-13.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "normal_forms.h"
#include "normalizer.h"

using namespace idni::tau_lang;

namespace {

// A wff parsed on its own (no trailing '.'), i.e. what get_formula_or_term
// hands the normal-form entry points.
tref wff(const char* s) { return tau::get(s, parse_wff()); }

// A bf term parsed on its own.
tref bf(const char* s) { return tau::get(s, parse_bf()); }

std::string str(tref n) { return n ? tau::get(n).to_str() : "<null>"; }

} // namespace

// --- denorm_equation (report 4.1: no direct test) -----------------------------

TEST_SUITE("denorm_equation") {

	// norm_equation turns X = Y into (X xor Y) = 0; denorm_equation is
	// documented as its exact inverse.
	TEST_CASE("inverts norm_equation on an equality") {
		tref eq = wff("x = y");
		tref normed = norm_equation<node_t>(eq);
		tref back = denorm_equation<node_t>(normed);
		CHECK( tau::get(back) == tau::get(eq) );
	}

	TEST_CASE("inverts norm_equation on a disequality") {
		tref neq = wff("x != y");
		tref normed = norm_equation<node_t>(neq);
		tref back = denorm_equation<node_t>(normed);
		CHECK( tau::get(back) == tau::get(neq) );
	}

	// Documented contract: anything that is not a bf_xor-shaped (!)= 0
	// equation comes back unchanged.
	TEST_CASE("non-xor equation is returned unchanged") {
		tref eq = wff("x = 0");
		CHECK( denorm_equation<node_t>(eq) == eq );
	}

	TEST_CASE("non-equation is returned unchanged") {
		tref fm = wff("x = 0 && y = 0");
		CHECK( denorm_equation<node_t>(fm) == fm );
	}
}

// --- syntactic_variable_simplification / _atomic_formula_ ---------------------

TEST_SUITE("syntactic simplification of atomic formulas") {

	// Documented early return: T and F are returned as-is whatever the
	// variable is.
	TEST_CASE("T and F are returned unchanged") {
		tref v = tau::build_variable("x", 0);
		CHECK( tau::get(syntactic_variable_simplification<node_t>(
			tau::_T(), v)).equals_T() );
		CHECK( tau::get(syntactic_variable_simplification<node_t>(
			tau::_F(), v)).equals_F() );
	}

	// A variable that does not occur in the atom cannot change it.
	TEST_CASE("variable absent from the atom leaves it equivalent") {
		tref atom = wff("x y = 0");
		tref z = tau::build_variable("z", 0);
		tref res = syntactic_variable_simplification<node_t>(atom, z);
		CHECK( res != nullptr );
		CHECK( are_nso_equivalent<node_t>(res, atom) );
	}

	// The pass must be meaning-preserving for a variable that does occur.
	TEST_CASE("variable present in the atom: result stays equivalent") {
		tref atom = wff("x y = 0");
		tref x = tau::build_variable("x", 0);
		tref res = syntactic_variable_simplification<node_t>(atom, x);
		CHECK( res != nullptr );
		CHECK( are_nso_equivalent<node_t>(res, atom) );
	}

	TEST_CASE("atomic formula simplification keeps meaning") {
		for (const char* s : { "x y = 0", "x y != 0", "x = y",
					"x y | x z = 0" }) {
			tref atom = wff(s);
			tref res = syntactic_atomic_formula_simplification<node_t>(
				atom);
			CAPTURE(s);
			CHECK( res != nullptr );
			CHECK( are_nso_equivalent<node_t>(res, atom) );
		}
	}

	// A contradictory atom must collapse; a tautological one likewise.
	TEST_CASE("atomic formula simplification detects a tautology") {
		tref atom = wff("x x' = 0");
		tref res = syntactic_atomic_formula_simplification<node_t>(atom);
		CHECK( tau::get(res).equals_T() );
	}
}

// --- term_boole_decomposition / term_boole_normal_form -----------------------

TEST_SUITE("term Boole decomposition") {

	// The documented example of term_boole_normal_form: xy|xy' = x.
	TEST_CASE("term_boole_normal_form reduces xy|xy' = 0 to x = 0") {
		tref fm = wff("xy|xy' = 0");
		tref res = term_boole_normal_form<node_t>(fm);
		CHECK( str(res) == "x = 0" );
	}

	TEST_CASE("term_boole_normal_form is the identity on T and F") {
		CHECK( tau::get(term_boole_normal_form<node_t>(tau::_T()))
			.equals_T() );
		CHECK( tau::get(term_boole_normal_form<node_t>(tau::_F()))
			.equals_F() );
	}

	// Constant terms take the documented early return.
	TEST_CASE("term_boole_decomposition is the identity on constants") {
		tref zero = tau::_0(0);
		tref one = tau::_1(0);
		CHECK( term_boole_decomposition<node_t>(zero) == zero );
		CHECK( term_boole_decomposition<node_t>(one) == one );
	}

	// A single decomposition step on a variable must produce a term that is
	// still equal to the input as a Boolean function.
	TEST_CASE("single-variable step preserves the function") {
		tref term = bf("x y | x z");
		tref x = tau::build_variable("x", 0);
		tref step = term_boole_decomposition<node_t>(term, x);
		CHECK( step != nullptr );
		CHECK( are_bf_equal<node_t>(step, term) );
	}

	TEST_CASE("full decomposition preserves the function") {
		for (const char* s : { "x y | x z", "x y' | x' y", "x", "x y z" }) {
			tref term = bf(s);
			tref bd = term_boole_decomposition<node_t>(term);
			CAPTURE(s);
			CHECK( bd != nullptr );
			CHECK( are_bf_equal<node_t>(bd, term) );
		}
	}
}

// --- is_non_temp_nso_unsat / eliminate_bv_and_quantifiers --------------------

TEST_SUITE("non-temporal satisfiability predicates") {

	// The documented example of is_non_temp_nso_unsat.
	TEST_CASE("is_non_temp_nso_unsat on a contradiction") {
		CHECK( is_non_temp_nso_unsat<node_t>(wff("x = 0 && x != 0")) );
	}

	TEST_CASE("is_non_temp_nso_unsat on a satisfiable formula") {
		CHECK( !is_non_temp_nso_unsat<node_t>(wff("x = 0")) );
		CHECK( !is_non_temp_nso_unsat<node_t>(wff("x = 0 || x != 0")) );
	}

	TEST_CASE("is_non_temp_nso_satisfiable agrees with its dual") {
		for (const char* s : { "x = 0", "x = 0 || x != 0",
					"x = 0 && x != 0", "x y = 0" }) {
			CAPTURE(s);
			tref fm = wff(s);
			CHECK( is_non_temp_nso_satisfiable<node_t>(fm)
				== !is_non_temp_nso_unsat<node_t>(fm) );
		}
	}

	// The documented example of eliminate_bv_and_quantifiers: ex x (x|y = 0)
	// eliminates x entirely.
	TEST_CASE("eliminate_bv_and_quantifiers removes an eliminable binder") {
		tref fm = wff("ex x x|y = 0");
		tref res = eliminate_bv_and_quantifiers<node_t>(fm);
		CHECK( res != nullptr );
		CHECK( !tau::get(res).find_top(is_quantifier<node_t>) );
	}

	TEST_CASE("eliminate_bv_and_quantifiers keeps meaning") {
		for (const char* s : { "ex x x|y = 0", "all x x y = 0",
					"ex x (x = 0 && y = 0)",
					"all x (x = 0 || y != 0)" }) {
			CAPTURE(s);
			tref fm = wff(s);
			tref res = eliminate_bv_and_quantifiers<node_t>(fm);
			CHECK( res != nullptr );
			CHECK( are_nso_equivalent<node_t>(res, fm) );
		}
	}
}

// --- fold_trivial_quantifiers: the wff_neg branch added for NZ-9 -------------

TEST_SUITE("fold_trivial_quantifiers negation branch") {

	// Substitution-based elimination rebuilds nodes without the construction
	// hooks, so a !T / !F residue can survive; before the NZ-9 fix
	// fold_trivial_quantifiers folded and/or/quantifier constants but not
	// negation.
	TEST_CASE("negated constants fold") {
		tref not_t = tau::get(tau::wff, tau::get(tau::wff_neg,
			tau::_T()));
		tref not_f = tau::get(tau::wff, tau::get(tau::wff_neg,
			tau::_F()));
		CHECK( tau::get(fold_trivial_quantifiers<node_t>(not_t))
			.equals_F() );
		CHECK( tau::get(fold_trivial_quantifiers<node_t>(not_f))
			.equals_T() );
	}

	TEST_CASE("non-constant negation is left untouched (control)") {
		tref fm = wff("!(x = 0)");
		CHECK( fold_trivial_quantifiers<node_t>(fm) != nullptr );
	}
}

// --- push_negation_one_in: the double-negation cases added for NF-13 ---------

TEST_SUITE("push_negation_one_in double negation") {

	// to_nnf could not remove a double negation before the NF-13 fix; it
	// relied entirely on the construction hooks, which squeeze_absorb
	// deliberately disables.
	TEST_CASE("wff double negation collapses") {
		tref inner = wff("x = 0");
		tref neg = tau::get(tau::wff, tau::get(tau::wff_neg, inner));
		tref negneg = tau::get(tau::wff, tau::get(tau::wff_neg, neg));
		tref res = push_negation_one_in<node_t>(negneg);
		CHECK( tau::get(res) == tau::get(inner) );
	}

	TEST_CASE("bf double negation collapses") {
		tref inner = bf("x");
		tref neg = tau::get(tau::bf, tau::get(tau::bf_neg, inner));
		tref negneg = tau::get(tau::bf, tau::get(tau::bf_neg, neg));
		tref res = push_negation_one_in<node_t, false>(negneg);
		CHECK( tau::get(res) == tau::get(inner) );
	}

	TEST_CASE("single negation of an atom is left alone (control)") {
		tref neg = wff("!(x = 0)");
		CHECK( push_negation_one_in<node_t>(neg) != nullptr );
	}
}

// --- is_valid / is_well_founded / calculate_fixed_point ----------------------
// All three are recurrence-relation gates with no direct test (report 4.1).
// The formulas below are the ones argued in their own doc comments
// (src/normalizer.tmpl.h:1026-1035, 1106-1116, 1188-1201).

TEST_SUITE("recurrence relation validity") {

	TEST_CASE("is_valid accepts a well-formed relative recurrence") {
		auto nso_rr = get_bf_nso_rr(
			"h[n](X):tau := h[n - 1](X)'."
			"h[0](X):tau := X.", "h[8](Y)").value();
		auto rr_captures = transform_ref_args_to_captures<node_t>(nso_rr);
		CHECK( is_valid<node_t>(rr_captures) );
	}

	TEST_CASE("is_valid rejects a relative offset in main") {
		// A capture offset in the main formula is explicitly rejected
		// (normalizer.tmpl.h:1042-1050).
		auto nso_rr = get_bf_nso_rr(
			"h[n](X):tau := h[n - 1](X)'."
			"h[0](X):tau := X.", "h[n](Y)").value();
		auto rr_captures = transform_ref_args_to_captures<node_t>(nso_rr);
		CHECK( !is_valid<node_t>(rr_captures) );
	}

	TEST_CASE("is_well_founded accepts the same recurrence") {
		auto nso_rr = get_bf_nso_rr(
			"h[n](X):tau := h[n - 1](X)'."
			"h[0](X):tau := X.", "h[8](Y)").value();
		auto rr_captures = transform_ref_args_to_captures<node_t>(nso_rr);
		CHECK( is_well_founded<node_t>(rr_captures) );
	}

	TEST_CASE("is_well_founded rejects initial conditions only") {
		// With no capture-offset rule there is no relative rule, which the
		// implementation reports as "no rules other than initial
		// conditions" (normalizer.tmpl.h:1159-1163).
		auto nso_rr = get_bf_nso_rr(
			"h[0](X):tau := X.", "h[0](Y)").value();
		auto rr_captures = transform_ref_args_to_captures<node_t>(nso_rr);
		CHECK( !is_well_founded<node_t>(rr_captures) );
	}

	// The documented example: h alternates between X and X' forever, so no
	// fixed point exists and the supplied fallback is returned.
	TEST_CASE("calculate_fixed_point returns the fallback on a loop") {
		auto nso_rr = get_bf_nso_rr(
			"h[n](X):tau := h[n - 1](X)'."
			"h[0](X):tau := X.", "h(Y)").value();
		auto rr_captures = transform_ref_args_to_captures<node_t>(nso_rr);
		tref main_fm = rr_captures.main->get();
		tref fallback = tau::_0(tau_type_id<node_t>());
		tref fp = calculate_fixed_point<node_t>(rr_captures, main_fm,
			tau::bf, 1, fallback);
		CHECK( fp != nullptr );
		CHECK( str(fp) == "0" );
	}

	TEST_CASE("calculate_fixed_point rejects a fallback of the wrong type") {
		// ft != nt and neither first_sym nor last_sym => nullptr with an
		// error log (normalizer.tmpl.h:1214-1219).
		auto nso_rr = get_bf_nso_rr(
			"h[n](X):tau := h[n - 1](X)'."
			"h[0](X):tau := X.", "h(Y)").value();
		auto rr_captures = transform_ref_args_to_captures<node_t>(nso_rr);
		tref main_fm = rr_captures.main->get();
		CHECK( calculate_fixed_point<node_t>(rr_captures, main_fm,
			tau::bf, 1, tau::_T()) == nullptr );
	}

	TEST_CASE("calculate_fixed_point rejects a non-well-founded relation") {
		auto nso_rr = get_bf_nso_rr(
			"h[0](X):tau := X.", "h(Y)").value();
		auto rr_captures = transform_ref_args_to_captures<node_t>(nso_rr);
		tref main_fm = rr_captures.main->get();
		CHECK( calculate_fixed_point<node_t>(rr_captures, main_fm,
			tau::bf, 1, tau::_0(tau_type_id<node_t>())) == nullptr );
	}
}
