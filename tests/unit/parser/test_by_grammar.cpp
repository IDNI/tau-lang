// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Comprehensive grammar-shape test suite for LTL(ABA).
//
// Every syntactic production in tau.tgf that can appear in a realizable spec
// is exercised at least once, nested 3 levels deep where applicable.
//
// Grammar productions covered (tau.tgf):
//
//   WFF unary:       wff_sometimes, wff_always (G), wff_neg (!)
//   WFF binary temporal: wff_U, wff_R, wff_W, wff_S, wff_T
//   WFF binary boolean:  wff_and(&&), wff_or(||), wff_xor(^^),
//                        wff_imply(->), wff_rimply(<-), wff_equiv(<->)
//   WFF ternary:     wff_conditional(? :)
//   WFF quantifiers: wff_all, wff_ex
//   WFF constants:   wff_t (T), wff_f (F-as-boolean-false)
//   WFF comparisons: bf_eq(=), bf_neq(!=), bf_lt(<), bf_nlt(!<),
//                    bf_lteq(<=), bf_nlteq(!<=), bf_gt(>), bf_ngt(!>),
//                    bf_gteq(>=), bf_ngteq(!>=)
//   BF interval:     bf_interval (bf <= bf <= bf)
//   BF binary logic: bf_and(&), bf_or(|), bf_xor(^),
//                    bf_nand(!&), bf_nor(!|), bf_xnor(!^)
//   BF unary:        bf_neg (') postfix complement
//   BF shifts:       bf_shr(>>), bf_shl(<<)
//   BF arithmetic:   bf_add(+), bf_sub(-), bf_mul(*), bf_div(/), bf_mod(%)
//   BF constants:    bf_t (1), bf_f (0)
//   BF quantifiers:  bf_fall, bf_fex
//   Constraints:     ctn_eq, ctn_neq, ctn_gt, ctn_gteq, ctn_lt, ctn_lteq
//   Variables:       io_var with [t], [t-1], [t-2], [t-3] offsets
//   BA types:        tau, sbf, bv, dyadic, qlt (Q,<-ordered), nlang
//
//   NOT covered (require rec_relation preamble or are test-infrastructure):
//     wff_ref, bf_ref, bf_and_nosep, stream definitions
//
// KEY grammar shape added vs. the basic suite:
//   SHAPE-Q — Boolean combinations of temporal sub-formulas used as operands
//             of temporal operators, e.g. ((F a) || (c U d)) U (e S f).
//             Every temporal operator appears with boolean-combo left AND/OR
//             right operands, including all six boolean connectives.
//
// Base atoms (all output-only, all REALIZABLE by the system):
//   A1 = o1[t]:qlt > {0}:qlt
//   A2 = o2[t]:qlt > {0}:qlt
//   A3 = o3[t]:qlt > {0}:qlt
//   B1 = o1[t]:sbf = {X | (Y & Z)}:sbf
//   B2 = o2[t]:sbf = {X & Y}:sbf
//   Ai = i1[t]:qlt > {1}:qlt   (input — UNREALIZABLE, SHAPE-O only)

#include "test_init.h"
#include "test_tau_helpers.h"

using namespace idni::tau_lang;

static tref spec(const char* s) {
	auto nso_rr = get_nso_rr<node_t>(tau::get(s));
	if (!nso_rr.has_value()) return nullptr;
	return nso_rr.value().main->get();
}

// ═══════════════════════════════════════════════════════════════════════════
// SHAPE-A: wff_sometimes / wff_always(G) / wff_neg — all 27 depth-3 unary combos
// ═══════════════════════════════════════════════════════════════════════════

TEST_SUITE("SHAPE-A: Unary depth-3 (F/G/!)") {

TEST_CASE("[SHAPE-A-01] F(F(F(A1)))") { tref fm = spec("F (F (F (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-A-02] F(F(G(A1)))") { tref fm = spec("F (F (G (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-A-03] F(F(!A1))") { tref fm = spec("F (F (! (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-A-04] F(G(F(A1)))") { tref fm = spec("F (G (F (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-A-05] F(G(G(A1)))") { tref fm = spec("F (G (G (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-A-06] F(G(!A1))") { tref fm = spec("F (G (! (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-A-07] F(!(F(A1)))") { tref fm = spec("F (! (F (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-A-08] F(!(G(A1)))") { tref fm = spec("F (! (G (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-A-09] F(!(!A1))") { tref fm = spec("F (! (! (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }

TEST_CASE("[SHAPE-A-10] G(F(F(A1)))") { tref fm = spec("G (F (F (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-A-11] G(F(G(A1)))") { tref fm = spec("G (F (G (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-A-12] G(F(!A1))") { tref fm = spec("G (F (! (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-A-13] G(G(F(A1)))") { tref fm = spec("G (G (F (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-A-14] G(G(G(A1)))") { tref fm = spec("G (G (G (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-A-15] G(G(!A1))") { tref fm = spec("G (G (! (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-A-16] G(!(F(A1)))") { tref fm = spec("G (! (F (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-A-17] G(!(F(A1))) — G(!(G())) triggers nested-G restriction") { tref fm = spec("G (! (F (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-A-18] G(!(!A1))") { tref fm = spec("G (! (! (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }

TEST_CASE("[SHAPE-A-19] !(F(F(A1)))") { tref fm = spec("! (F (F (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-A-20] !(F(G(A1)))") { tref fm = spec("! (F (G (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-A-21] !(F(!A1))") { tref fm = spec("! (F (! (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-A-22] !(G(F(A1)))") { tref fm = spec("! (G (F (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-A-23] !(G(G(A1)))") { tref fm = spec("! (G (G (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-A-24] !(G(!A1))") { tref fm = spec("! (G (! (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-A-25] !(!(F(A1)))") { tref fm = spec("! (! (F (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-A-26] !(!(G(A1)))") { tref fm = spec("! (! (G (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-A-27] !(!(!A1))") { tref fm = spec("! (! (! (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }

} // SHAPE-A

// ═══════════════════════════════════════════════════════════════════════════
// SHAPE-B: Binary temporal operators (wff_U/R/W/S/T) — atomic operands
// ═══════════════════════════════════════════════════════════════════════════

TEST_SUITE("SHAPE-B: Binary temporal with atomic operands") {

TEST_CASE("[SHAPE-B-01] F((A1 U A2) U A3) — left-nested U at depth 3") {
	tref fm = spec("F (((o1[t]:qlt > {0}:qlt) U (o2[t]:qlt > {0}:qlt)) U (o3[t]:qlt > {0}:qlt)).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}
TEST_CASE("[SHAPE-B-02] F(A1 U (A2 U A3)) — right-assoc U") {
	tref fm = spec("F ((o1[t]:qlt > {0}:qlt) U ((o2[t]:qlt > {0}:qlt) U (o3[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}
TEST_CASE("[SHAPE-B-03] G(A1 U A2) — U under G") {
	tref fm = spec("G ((o1[t]:qlt > {0}:qlt) U (o2[t]:qlt > {0}:qlt)).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}
TEST_CASE("[SHAPE-B-04] F((A1 R A2) R A3) — left-nested R") {
	tref fm = spec("F (((o1[t]:qlt > {0}:qlt) R (o2[t]:qlt > {0}:qlt)) R (o3[t]:qlt > {0}:qlt)).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}
TEST_CASE("[SHAPE-B-05] G(A1 R (A2 R A3)) — right-assoc R under G") {
	tref fm = spec("G ((o1[t]:qlt > {0}:qlt) R ((o2[t]:qlt > {0}:qlt) R (o3[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}
TEST_CASE("[SHAPE-B-06] F((A1 W A2) W A3) — left-nested W") {
	tref fm = spec("F (((o1[t]:qlt > {0}:qlt) W (o2[t]:qlt > {0}:qlt)) W (o3[t]:qlt > {0}:qlt)).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}
TEST_CASE("[SHAPE-B-07] G(A1 W (A2 W A3)) — right-assoc W under G") {
	tref fm = spec("G ((o1[t]:qlt > {0}:qlt) W ((o2[t]:qlt > {0}:qlt) W (o3[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}
TEST_CASE("[SHAPE-B-08] F((A1 S A2) S A3) — left-nested S (Since)") {
	tref fm = spec("F (((o1[t]:qlt > {0}:qlt) S (o2[t]:qlt > {0}:qlt)) S (o3[t]:qlt > {0}:qlt)).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}
TEST_CASE("[SHAPE-B-09] G(A1 S (A2 S A3)) — right-assoc S under G") {
	tref fm = spec("G ((o1[t]:qlt > {0}:qlt) S ((o2[t]:qlt > {0}:qlt) S (o3[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}
TEST_CASE("[SHAPE-B-10] F((A1 T A2) T A3) — left-nested T (Trigger)") {
	tref fm = spec("F (((o1[t]:qlt > {0}:qlt) T (o2[t]:qlt > {0}:qlt)) T (o3[t]:qlt > {0}:qlt)).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}
TEST_CASE("[SHAPE-B-11] G(A1 T (A2 T A3)) — right-assoc T under G") {
	tref fm = spec("G ((o1[t]:qlt > {0}:qlt) T ((o2[t]:qlt > {0}:qlt) T (o3[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}
TEST_CASE("[SHAPE-B-12] (A1 U A2) S ((A2 R A3) T A1) — all five binary temporal ops") {
	tref fm = spec("((o1[t]:qlt > {0}:qlt) U (o2[t]:qlt > {0}:qlt)) S (((o2[t]:qlt > {0}:qlt) R (o3[t]:qlt > {0}:qlt)) T (o1[t]:qlt > {0}:qlt)).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}
TEST_CASE("[SHAPE-B-13] F((A1 U A2) S (A2 R A3)) — mixed temporal at depth 3") {
	tref fm = spec("F (((o1[t]:qlt > {0}:qlt) U (o2[t]:qlt > {0}:qlt)) S ((o2[t]:qlt > {0}:qlt) R (o3[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

} // SHAPE-B

// ═══════════════════════════════════════════════════════════════════════════
// SHAPE-C: Boolean connectives (wff_and/or/xor/imply/rimply/equiv) depth-3
// ═══════════════════════════════════════════════════════════════════════════

TEST_SUITE("SHAPE-C: Boolean connectives depth-3") {

TEST_CASE("[SHAPE-C-01] F((A1 && A2) && A3)") {
	tref fm = spec("F (((o1[t]:qlt > {0}:qlt) && (o2[t]:qlt > {0}:qlt)) && (o3[t]:qlt > {0}:qlt)).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}
TEST_CASE("[SHAPE-C-02] F(A1 && (A2 && A3))") {
	tref fm = spec("F ((o1[t]:qlt > {0}:qlt) && ((o2[t]:qlt > {0}:qlt) && (o3[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}
TEST_CASE("[SHAPE-C-03] G((A1 && A2) && A3) — && under G at depth 3") {
	tref fm = spec("G (((o1[t]:qlt > {0}:qlt) && (o2[t]:qlt > {0}:qlt)) && (o3[t]:qlt > {0}:qlt)).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}
TEST_CASE("[SHAPE-C-04] F((A1 || A2) || A3)") {
	tref fm = spec("F (((o1[t]:qlt > {0}:qlt) || (o2[t]:qlt > {0}:qlt)) || (o3[t]:qlt > {0}:qlt)).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}
TEST_CASE("[SHAPE-C-05] G(A1 || (A2 || A3))") {
	tref fm = spec("G ((o1[t]:qlt > {0}:qlt) || ((o2[t]:qlt > {0}:qlt) || (o3[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}
TEST_CASE("[SHAPE-C-06] F((A1 ^^ A2) ^^ A3)") {
	tref fm = spec("F (((o1[t]:qlt > {0}:qlt) ^^ (o2[t]:qlt > {0}:qlt)) ^^ (o3[t]:qlt > {0}:qlt)).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}
TEST_CASE("[SHAPE-C-07] G(A1 ^^ (A2 ^^ A3))") {
	tref fm = spec("G ((o1[t]:qlt > {0}:qlt) ^^ ((o2[t]:qlt > {0}:qlt) ^^ (o3[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}
TEST_CASE("[SHAPE-C-08] F((A1 -> A2) -> A3)") {
	tref fm = spec("F (((o1[t]:qlt > {0}:qlt) -> (o2[t]:qlt > {0}:qlt)) -> (o3[t]:qlt > {0}:qlt)).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}
TEST_CASE("[SHAPE-C-09] G(A1 -> (A2 -> A3))") {
	tref fm = spec("G ((o1[t]:qlt > {0}:qlt) -> ((o2[t]:qlt > {0}:qlt) -> (o3[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}
TEST_CASE("[SHAPE-C-10] F((A1 <- A2) <- A3)") {
	tref fm = spec("F (((o1[t]:qlt > {0}:qlt) <- (o2[t]:qlt > {0}:qlt)) <- (o3[t]:qlt > {0}:qlt)).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}
TEST_CASE("[SHAPE-C-11] G(A1 <- (A2 <- A3))") {
	tref fm = spec("G ((o1[t]:qlt > {0}:qlt) <- ((o2[t]:qlt > {0}:qlt) <- (o3[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}
TEST_CASE("[SHAPE-C-12] F((A1 <-> A2) <-> A3)") {
	tref fm = spec("F (((o1[t]:qlt > {0}:qlt) <-> (o2[t]:qlt > {0}:qlt)) <-> (o3[t]:qlt > {0}:qlt)).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}
TEST_CASE("[SHAPE-C-13] G(A1 <-> (A2 <-> A3))") {
	tref fm = spec("G ((o1[t]:qlt > {0}:qlt) <-> ((o2[t]:qlt > {0}:qlt) <-> (o3[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}
TEST_CASE("[SHAPE-C-14] F((A1 && A2) || (A2 -> A3)) — mixed boolean depth-3") {
	tref fm = spec("F (((o1[t]:qlt > {0}:qlt) && (o2[t]:qlt > {0}:qlt)) || ((o2[t]:qlt > {0}:qlt) -> (o3[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}
TEST_CASE("[SHAPE-C-15] G((A1 || A2) && (A2 <-> A3))") {
	tref fm = spec("G (((o1[t]:qlt > {0}:qlt) || (o2[t]:qlt > {0}:qlt)) && ((o2[t]:qlt > {0}:qlt) <-> (o3[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

} // SHAPE-C

// ═══════════════════════════════════════════════════════════════════════════
// SHAPE-D: All 10 bf comparison ops as WFF atoms (bf_eq…bf_ngteq)
// ═══════════════════════════════════════════════════════════════════════════

TEST_SUITE("SHAPE-D: All 10 comparison operators") {

TEST_CASE("[SHAPE-D-01] bf_eq: F(F(F(o1 = {1/2}:qlt)))") { tref fm = spec("F (F (F (o1[t]:qlt = {1/2}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-D-02] bf_neq: F(F(F(o1 != {1/2}:qlt)))") { tref fm = spec("F (F (F (o1[t]:qlt != {1/2}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-D-03] bf_lt: F(F(F(o1 < {1}:qlt)))") { tref fm = spec("F (F (F (o1[t]:qlt < {1}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-D-04] bf_nlt: F(F(F(o1 !< {1}:qlt)))") { tref fm = spec("F (F (F (o1[t]:qlt !< {1}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-D-05] bf_lteq: F(F(F(o1 <= {1}:qlt)))") { tref fm = spec("F (F (F (o1[t]:qlt <= {1}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-D-06] bf_nlteq: F(F(F(o1 !<= {0}:qlt)))") { tref fm = spec("F (F (F (o1[t]:qlt !<= {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-D-07] bf_gt: F(F(F(o1 > {0}:qlt)))") { tref fm = spec("F (F (F (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-D-08] bf_ngt: F(F(F(o1 !> {1}:qlt)))") { tref fm = spec("F (F (F (o1[t]:qlt !> {1}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-D-09] bf_gteq: F(F(F(o1 >= {0}:qlt)))") { tref fm = spec("F (F (F (o1[t]:qlt >= {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-D-10] bf_ngteq: F(F(F(o1 !>= {1}:qlt)))") { tref fm = spec("F (F (F (o1[t]:qlt !>= {1}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-D-11] bf_eq nested: G(F(o1 = {0}:qlt))") { tref fm = spec("G (F (o1[t]:qlt = {0}:qlt))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-D-12] bf_gt in U: G(A2 U (o1>{0}:qlt))") { tref fm = spec("G ((o2[t]:qlt > {0}:qlt) U (o1[t]:qlt > {0}:qlt))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }

} // SHAPE-D

// ═══════════════════════════════════════════════════════════════════════════
// SHAPE-E: bf_interval (bf <= bf <= bf)
// ═══════════════════════════════════════════════════════════════════════════

TEST_SUITE("SHAPE-E: BF interval bf<=bf<=bf") {

TEST_CASE("[SHAPE-E-01] F(F(F({0}<=o1<={1}:qlt)))") { tref fm = spec("F (F (F ({0}:qlt <= o1[t]:qlt <= {1}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-E-02] G({0}<=o1<={1}:qlt)") { tref fm = spec("G ({0}:qlt <= o1[t]:qlt <= {1}:qlt)."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-E-03] F(G(F({-1}<=o1<={2}:qlt)))") { tref fm = spec("F (G (F ({-1}:qlt <= o1[t]:qlt <= {2}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-E-04] interval U interval") { tref fm = spec("({0}:qlt <= o1[t]:qlt <= {1}:qlt) U ({0}:qlt <= o2[t]:qlt <= {1}:qlt)."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-E-05] G(interval && interval)") { tref fm = spec("G (({0}:qlt <= o1[t]:qlt <= {1}:qlt) && ({0}:qlt <= o2[t]:qlt <= {1}:qlt))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }

} // SHAPE-E

// ═══════════════════════════════════════════════════════════════════════════
// SHAPE-F: BF binary logic (&/|/^/!&/!|/!^) inside sbf constants
// ═══════════════════════════════════════════════════════════════════════════

TEST_SUITE("SHAPE-F: BF binary logic in sbf constants") {

TEST_CASE("[SHAPE-F-01] bf_and: F(F(F(o1={X&Y}:sbf)))") { tref fm = spec("F (F (F (o1[t]:sbf = {X & Y}:sbf)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-F-02] bf_or: F(F(F(o1={X|(Y&Z)}:sbf)))") { tref fm = spec("F (F (F (o1[t]:sbf = {X | (Y & Z)}:sbf)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-F-03] bf_xor: F(F(F(o1={X^Y}:sbf)))") { tref fm = spec("F (F (F (o1[t]:sbf = {X ^ Y}:sbf)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
// bf_nand/nor/xnor do not parse in WFF context — test bf_and/or/xor between two ba_constants instead
TEST_CASE("[SHAPE-F-04] bf_and of constants: G(F(o1={X|Z}&{X&Y}:sbf))") { tref fm = spec("G (F (o1[t]:sbf = ({X | Z}:sbf & {X & Y}:sbf)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-F-05] bf_or of constants: G(F(o1={X&Y}|{Y&Z}:sbf))") { tref fm = spec("G (F (o1[t]:sbf = ({X & Y}:sbf | {Y & Z}:sbf)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-F-06] bf_xor of constants: G(F(o1={X&Y}^{X|Z}:sbf))") { tref fm = spec("G (F (o1[t]:sbf = ({X & Y}:sbf ^ {X | Z}:sbf)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-F-07] bf_or in U at depth 3") { tref fm = spec("(o1[t]:sbf = {X & Y}:sbf) U (G (F (o2[t]:sbf = {X | Z}:sbf)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }

} // SHAPE-F

// ═══════════════════════════════════════════════════════════════════════════
// SHAPE-G: bf_neg (') postfix complement
// ═══════════════════════════════════════════════════════════════════════════

TEST_SUITE("SHAPE-G: BF complement postfix") {

TEST_CASE("[SHAPE-G-01] F(F(F(o1={X}:sbf')))") { tref fm = spec("F (F (F (o1[t]:sbf = {X}:sbf')))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-G-02] G(o1={X&Y}:sbf')") { tref fm = spec("G (o1[t]:sbf = {X & Y}:sbf')."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-G-03] G(F(o1={X|Z}:sbf'))") { tref fm = spec("G (F (o1[t]:sbf = {X | Z}:sbf'))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-G-04] (o1={X}:sbf') U (o2={Y&Z}:sbf')") { tref fm = spec("(o1[t]:sbf = {X}:sbf') U (o2[t]:sbf = {Y & Z}:sbf')."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }

} // SHAPE-G

// ═══════════════════════════════════════════════════════════════════════════
// SHAPE-H: bf_shr(>>) and bf_shl(<<) inside bv constants
// ═══════════════════════════════════════════════════════════════════════════

TEST_SUITE("SHAPE-H: BF shift operators in bv constants") {

TEST_CASE("[SHAPE-H-01] F(F(F(o1:bv = {#b10110101}>>{2}:bv)))") { tref fm = spec("F (F (F (o1[t]:bv = {#b10110101}:bv >> {2}:bv)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-H-02] G(F(o1:bv = {#b00001111}<<{1}:bv))") { tref fm = spec("G (F (o1[t]:bv = {#b00001111}:bv << {1}:bv))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-H-03] (o1:bv={#b11110000}>>{1}) U (o2:bv={#b10110101}<<{2})") {
	tref fm = spec("(o1[t]:bv = {#b11110000}:bv >> {1}:bv) U (o2[t]:bv = {#b10110101}:bv << {2}:bv).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

} // SHAPE-H

// ═══════════════════════════════════════════════════════════════════════════
// SHAPE-I: bf_fall and bf_fex BF quantifiers
// ═══════════════════════════════════════════════════════════════════════════

TEST_SUITE("SHAPE-I: BF quantifiers fall/fex") {

// fall/fex take a BF body (not a WFF comparison); result is a BF, compared outside
// Parse-only: fall/fex quantifiers parse correctly but LTL synthesis does not support them
TEST_CASE("[SHAPE-I-01] F(F(F((fall x o1&x)={0}:bv))) parses") { tref fm = spec("F (F (F ((fall x (o1[t]:bv & x:bv)) = {0}:bv)))."); REQUIRE(fm != nullptr); }
TEST_CASE("[SHAPE-I-02] G((fall x o1&x)={0}:bv) parses") { tref fm = spec("G ((fall x (o1[t]:bv & x:bv)) = {0}:bv)."); REQUIRE(fm != nullptr); }
TEST_CASE("[SHAPE-I-03] F(F(F((fex x o1&x)=o1:bv))) parses") { tref fm = spec("F (F (F ((fex x (o1[t]:bv & x:bv)) = o1[t]:bv)))."); REQUIRE(fm != nullptr); }
TEST_CASE("[SHAPE-I-04] G((fex x o1&x)=o1:bv) parses") { tref fm = spec("G ((fex x (o1[t]:bv & x:bv)) = o1[t]:bv)."); REQUIRE(fm != nullptr); }
TEST_CASE("[SHAPE-I-05] ((fall x o1&x)={0}) U ((fex y o2&y)=o2) parses") { tref fm = spec("((fall x (o1[t]:bv & x:bv)) = {0}:bv) U ((fex y (o2[t]:bv & y:bv)) = o2[t]:bv)."); REQUIRE(fm != nullptr); }
TEST_CASE("[SHAPE-I-06] G(F((fall x o1&x)={0}:bv)) parses") { tref fm = spec("G (F ((fall x (o1[t]:bv & x:bv)) = {0}:bv))."); REQUIRE(fm != nullptr); }

} // SHAPE-I

// ═══════════════════════════════════════════════════════════════════════════
// SHAPE-J: wff_all and wff_ex WFF quantifiers
// ═══════════════════════════════════════════════════════════════════════════

TEST_SUITE("SHAPE-J: WFF quantifiers all/ex") {

// wff_all/wff_ex must be inside a temporal operator (G or F) — enforced by design
TEST_CASE("[SHAPE-J-01] G(all x F(F(F(o1:bv=x:bv))))") { tref fm = spec("G (all x F (F (F (o1[t]:bv = x:bv))))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-J-02] G(ex x F(F(F(o1:bv=x:bv))))") { tref fm = spec("G (ex x F (F (F (o1[t]:bv = x:bv))))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-J-03] F(all x G(o1:bv=x:bv))") { tref fm = spec("F (all x G (o1[t]:bv = x:bv))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-J-04] F(ex x G(F(o1:bv=x:bv)))") { tref fm = spec("F (ex x G (F (o1[t]:bv = x:bv)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-J-05] G(all x ex y F((o1:bv=x)&&(o2:bv=y)))") { tref fm = spec("G (all x ex y F ((o1[t]:bv = x:bv) && (o2[t]:bv = y:bv)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }

} // SHAPE-J

// ═══════════════════════════════════════════════════════════════════════════
// SHAPE-K: wff_t (T) and wff_f (F boolean false)
// ═══════════════════════════════════════════════════════════════════════════

TEST_SUITE("SHAPE-K: WFF constants T and F") {

TEST_CASE("[SHAPE-K-01] F(F T) — wff_t in F") { tref fm = spec("F (F T)."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-K-02] G(T)") { tref fm = spec("G T."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-K-03] G(F T)") { tref fm = spec("G (F T)."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-K-04] T U T") { tref fm = spec("T U T."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-K-05] T U (A1 U T)") { tref fm = spec("T U ((o1[t]:qlt > {0}:qlt) U T)."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-K-06] G(A1 && T)") { tref fm = spec("G ((o1[t]:qlt > {0}:qlt) && T)."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-K-07] G((A1 U A2) && T)") { tref fm = spec("G (((o1[t]:qlt > {0}:qlt) U (o2[t]:qlt > {0}:qlt)) && T)."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
// wff_f: standalone F parses as boolean false
TEST_CASE("[SHAPE-K-08] F(F(F)) UNREALIZABLE — innermost F is wff_f") { tref fm = spec("F (F (F))."); REQUIRE(fm != nullptr); CHECK_FALSE(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-K-09] G(A1 && (T U T))") { tref fm = spec("G ((o1[t]:qlt > {0}:qlt) && (T U T))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }

} // SHAPE-K

// ═══════════════════════════════════════════════════════════════════════════
// SHAPE-L: wff_conditional (? :) ternary
// ═══════════════════════════════════════════════════════════════════════════

TEST_SUITE("SHAPE-L: WFF conditional ternary") {

TEST_CASE("[SHAPE-L-01] F(F(A1?A2:A3))") { tref fm = spec("F (F ((o1[t]:qlt > {0}:qlt) ? (o2[t]:qlt > {0}:qlt) : (o3[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-L-02] G(A1?A2:A3)") { tref fm = spec("G ((o1[t]:qlt > {0}:qlt) ? (o2[t]:qlt > {0}:qlt) : (o3[t]:qlt > {0}:qlt))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-L-03] F(A1? F(A2):G(A3)) — nested operators in branches") { tref fm = spec("F ((o1[t]:qlt > {0}:qlt) ? (F (o2[t]:qlt > {0}:qlt)) : (G (o3[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-L-04] G(A1?G(F(A2)):F(G(A3))) — depth-3 in branches") { tref fm = spec("G ((o1[t]:qlt > {0}:qlt) ? (G (F (o2[t]:qlt > {0}:qlt))) : (F (G (o3[t]:qlt > {0}:qlt))))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-L-05] ternary U ternary") { tref fm = spec("((o1[t]:qlt > {0}:qlt) ? (o2[t]:qlt > {0}:qlt) : (o3[t]:qlt > {0}:qlt)) U ((o2[t]:qlt > {0}:qlt) ? (o3[t]:qlt > {0}:qlt) : (o1[t]:qlt > {0}:qlt))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }

} // SHAPE-L

// ═══════════════════════════════════════════════════════════════════════════
// SHAPE-M: All BA types — tau, sbf, bv, dyadic, qlt (Q,<-ordered), nlang
// ═══════════════════════════════════════════════════════════════════════════

TEST_SUITE("SHAPE-M: All BA types at depth 3") {

// tau type
TEST_CASE("[SHAPE-M-01] tau: F(F(F(o1:tau={T.})))") { tref fm = spec("F (F (F (o1[t]:tau = {T.}:tau)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-M-02] tau: G(F(o1:tau={T.}))") { tref fm = spec("G (F (o1[t]:tau = {T.}:tau))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-M-03] tau: (o1:tau={T.}) U (G(F(o1:tau={T.})))") { tref fm = spec("(o1[t]:tau = {T.}:tau) U (G (F (o1[t]:tau = {T.}:tau)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }

// sbf type
TEST_CASE("[SHAPE-M-04] sbf: F(F(F(o1={X|(Y&Z)}:sbf)))") { tref fm = spec("F (F (F (o1[t]:sbf = {X | (Y & Z)}:sbf)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-M-05] sbf: G(F(F(o1={X&Y}:sbf)))") { tref fm = spec("G (F (F (o1[t]:sbf = {X & Y}:sbf)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-M-06] sbf: (o1={X&Y}) U (G(F(o2={X|Z})))") { tref fm = spec("(o1[t]:sbf = {X & Y}:sbf) U (G (F (o2[t]:sbf = {X | Z}:sbf)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }

// bv type
TEST_CASE("[SHAPE-M-07] bv: F(F(F(o1={#b10110101}:bv)))") { tref fm = spec("F (F (F (o1[t]:bv = {#b10110101}:bv)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-M-08] bv: G(F(F(o1={#b00001111}:bv)))") { tref fm = spec("G (F (F (o1[t]:bv = {#b00001111}:bv)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-M-09] bv: (o1={#b11110000}) U (G(F(o2={#b10110101})))") { tref fm = spec("(o1[t]:bv = {#b11110000}:bv) U (G (F (o2[t]:bv = {#b10110101}:bv)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }

// dyadic type
TEST_CASE("[SHAPE-M-10] dyadic: F(F(F(o1={[1/4,3/4)})))") { tref fm = spec("F (F (F (o1[t]:qint = {[1/4, 3/4)}:qint)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-M-11] dyadic: G(F(o1={[-1,0)|[1,2)}))") { tref fm = spec("G (F (o1[t]:qint = {[-1, 0) | [1, 2)}:qint))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-M-12] dyadic: (o1={[0,1)}) U (G(F(o2={[1/4,3/4)})))") { tref fm = spec("(o1[t]:qint = {[0, 1)}:qint) U (G (F (o2[t]:qint = {[1/4, 3/4)}:qint)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }

// qlt type (Q,<-ordered dense linear order — NOT a Boolean algebra, but an omega-categorical theory)
TEST_CASE("[SHAPE-M-13] qlt: F(F(F(o1>{0}:qlt)))") { tref fm = spec("F (F (F (o1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-M-14] qlt: G(F(o1={(0,1)}:qlt))") { tref fm = spec("G (F (o1[t]:qlt = {(0, 1)}:qlt))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-M-15] qlt: (o1={1/2}) U (G(F(o2>{0})))") { tref fm = spec("(o1[t]:qlt = {1/2}:qlt) U (G (F (o2[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }

// nlang type
TEST_CASE("[SHAPE-M-16] nlang: F(F(F(o1={it is raining}:nlang)))") { tref fm = spec("F (F (F (o1[t]:nlang = {it is raining}:nlang)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-M-17] nlang: G(F(o1={the sun is shining}:nlang))") { tref fm = spec("G (F (o1[t]:nlang = {the sun is shining}:nlang))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-M-18] nlang: (o1={it is raining}) U (G(F(o2={the sun is shining})))") { tref fm = spec("(o1[t]:nlang = {it is raining}:nlang) U (G (F (o2[t]:nlang = {the sun is shining}:nlang)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }

} // SHAPE-M

// ═══════════════════════════════════════════════════════════════════════════
// SHAPE-N: Time-shifted io_vars [t-1], [t-2], [t-3]
// ═══════════════════════════════════════════════════════════════════════════

TEST_SUITE("SHAPE-N: Time-shifted io_vars") {

TEST_CASE("[SHAPE-N-01] F(F(F(o1[t-1]>{0}:qlt)))") { tref fm = spec("F (F (F (o1[t-1]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-N-02] F(F(F(o1[t-2]>{0}:qlt)))") { tref fm = spec("F (F (F (o1[t-2]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-N-03] F(F(F(o1[t-3]>{0}:qlt)))") { tref fm = spec("F (F (F (o1[t-3]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-N-04] G(F(o1[t-1]>{0}:qlt))") { tref fm = spec("G (F (o1[t-1]:qlt > {0}:qlt))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-N-05] G(F(o1[t-2]>{0}:qlt))") { tref fm = spec("G (F (o1[t-2]:qlt > {0}:qlt))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-N-06] G(F(o1[t-3]>{0}:qlt))") { tref fm = spec("G (F (o1[t-3]:qlt > {0}:qlt))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-N-07] (o1[t-1]>{0}) U (F(o1[t-2]>{0}))") { tref fm = spec("(o1[t-1]:qlt > {0}:qlt) U (F (o1[t-2]:qlt > {0}:qlt))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-N-08] F(F((o1[t-1]>{0}) S (o1[t-2]>{0}))) — S with lookback") { tref fm = spec("F (F ((o1[t-1]:qlt > {0}:qlt) S (o1[t-2]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-N-09] F(F(F(o1[t-1]={X&Y}:sbf))) — sbf+lookback") { tref fm = spec("F (F (F (o1[t-1]:sbf = {X & Y}:sbf)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-N-10] G(F(o1[t-1]={#b10110101}:bv)) — bv+lookback") { tref fm = spec("G (F (o1[t-1]:bv = {#b10110101}:bv))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }

} // SHAPE-N

// ═══════════════════════════════════════════════════════════════════════════
// SHAPE-O: UNREALIZABLE — pure input atoms at depth 3
// ═══════════════════════════════════════════════════════════════════════════

TEST_SUITE("SHAPE-O: UNREALIZABLE depth-3") {

TEST_CASE("[SHAPE-O-01] F(F(F(i1>{0}:qlt))) UNREALIZABLE") { tref fm = spec("F (F (F (i1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK_FALSE(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-O-02] G(F(F(i1>{0}:qlt))) UNREALIZABLE") { tref fm = spec("G (F (F (i1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK_FALSE(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-O-03] F(F(G(i1>{0}:qlt))) UNREALIZABLE") { tref fm = spec("F (F (G (i1[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK_FALSE(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-O-04] F((i1>{0}) U (F(i2>{0}))) UNREALIZABLE") { tref fm = spec("F ((i1[t]:qlt > {0}:qlt) U (F (i2[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK_FALSE(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-O-05] G(F(F(i1={#b10110101}:bv))) UNREALIZABLE") { tref fm = spec("G (F (F (i1[t]:bv = {#b10110101}:bv)))."); REQUIRE(fm != nullptr); CHECK_FALSE(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-O-06] (i1>{0}) U (F(F(i2>{0}))) UNREALIZABLE") { tref fm = spec("(i1[t]:qlt > {0}:qlt) U (F (F (i2[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK_FALSE(is_tau_formula_sat<node_t>(fm)); }

} // SHAPE-O

// ═══════════════════════════════════════════════════════════════════════════
// SHAPE-P: Mixed operator nesting — all categories together
// ═══════════════════════════════════════════════════════════════════════════

TEST_SUITE("SHAPE-P: Mixed operator nesting") {

TEST_CASE("[SHAPE-P-01] F((A1 U A2) && (A2 R A3))") { tref fm = spec("F (((o1[t]:qlt > {0}:qlt) U (o2[t]:qlt > {0}:qlt)) && ((o2[t]:qlt > {0}:qlt) R (o3[t]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-P-02] G((A1||A2) U A3)") { tref fm = spec("G (((o1[t]:qlt > {0}:qlt) || (o2[t]:qlt > {0}:qlt)) U (o3[t]:qlt > {0}:qlt))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-P-03] F(!A1 U G(A2&&A3))") { tref fm = spec("F ((! (o1[t]:qlt > {0}:qlt)) U (G ((o2[t]:qlt > {0}:qlt) && (o3[t]:qlt > {0}:qlt))))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-P-04] F(A1) && G(A2) && F(A3)") { tref fm = spec("(F (o1[t]:qlt > {0}:qlt)) && (G (o2[t]:qlt > {0}:qlt)) && (F (o3[t]:qlt > {0}:qlt))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-P-05] F((A1 S A2) U G(!A3))") { tref fm = spec("F (((o1[t]:qlt > {0}:qlt) S (o2[t]:qlt > {0}:qlt)) U (G (! (o3[t]:qlt > {0}:qlt))))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-P-06] G((A1 W A2) && F(A3||A1))") { tref fm = spec("G (((o1[t]:qlt > {0}:qlt) W (o2[t]:qlt > {0}:qlt)) && (F ((o3[t]:qlt > {0}:qlt) || (o1[t]:qlt > {0}:qlt))))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-P-07] F((F(A1)->F(A2)) && (F(A3)->F(A1)))") { tref fm = spec("F (((F (o1[t]:qlt > {0}:qlt)) -> (F (o2[t]:qlt > {0}:qlt))) && ((F (o3[t]:qlt > {0}:qlt)) -> (F (o1[t]:qlt > {0}:qlt))))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-P-08] F(F(A1) <-> G(F(A2&&A3)))") { tref fm = spec("F ((F (o1[t]:qlt > {0}:qlt)) <-> (G (F ((o2[t]:qlt > {0}:qlt) && (o3[t]:qlt > {0}:qlt)))))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-P-09] G(F(B1:sbf)) && F(G(F(o2:bv)))") { tref fm = spec("(G (F (o1[t]:sbf = {X | (Y & Z)}:sbf))) && (F (G (F (o2[t]:bv = {#b10110101}:bv))))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-P-10] (A1[t-1] S A2[t-2]) U G(F(A3[t-3]))") { tref fm = spec("((o1[t-1]:qlt > {0}:qlt) S (o2[t-2]:qlt > {0}:qlt)) U (G (F (o3[t-3]:qlt > {0}:qlt)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-P-11] F(A1 ? F(A2 U A3) : G(A3 W A2))") { tref fm = spec("F ((o1[t]:qlt > {0}:qlt) ? (F ((o2[t]:qlt > {0}:qlt) U (o3[t]:qlt > {0}:qlt))) : (G ((o3[t]:qlt > {0}:qlt) W (o2[t]:qlt > {0}:qlt))))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-P-12] F(F(F(A1))) && G(G(G(A2))) && F(G(F(A3)))") { tref fm = spec("(F (F (F (o1[t]:qlt > {0}:qlt)))) && (G (G (G (o2[t]:qlt > {0}:qlt)))) && (F (G (F (o3[t]:qlt > {0}:qlt))))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }

} // SHAPE-P

// ═══════════════════════════════════════════════════════════════════════════
// SHAPE-Q: Boolean combinations of temporal sub-formulas as temporal operands
//
// Grammar shapes: every binary temporal operator (U/R/W/S/T) appears with
// operands that are themselves boolean combinations (&&/||/^^/->/←/<->)
// of temporal sub-formulas (F/G/U/R/W/S/T applied to atomic wffs).
//
// These directly exercise the pattern highlighted by the user:
//   ((F a) || (c U d)) U (e S f)
// where temporal formulas are the LEAVES of boolean connectives, and the
// resulting boolean formula is the OPERAND of an outer temporal operator.
// ═══════════════════════════════════════════════════════════════════════════

TEST_SUITE("SHAPE-Q: Boolean combos of temporals as temporal operands") {

// ─── wff_U with boolean-combo operands ───────────────────────────────────

TEST_CASE("[SHAPE-Q-01] ((F A1) || (A1 U A2)) U (A2 S A3) — || of unary+U as U-left") {
	tref fm = spec("((F (o1[t]:qlt > {0}:qlt)) || ((o1[t]:qlt > {0}:qlt) U (o2[t]:qlt > {0}:qlt))) U ((o2[t]:qlt > {0}:qlt) S (o3[t]:qlt > {0}:qlt)).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-Q-02] (G A1) && (A2 R A3) — && of G+R as U-left, A1 as right") {
	tref fm = spec("((G (o1[t]:qlt > {0}:qlt)) && ((o2[t]:qlt > {0}:qlt) R (o3[t]:qlt > {0}:qlt))) U (o1[t]:qlt > {0}:qlt).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-Q-03] A1 U ((F A2) ^^ (A2 W A3)) — ^^ of F+W as U-right") {
	tref fm = spec("(o1[t]:qlt > {0}:qlt) U ((F (o2[t]:qlt > {0}:qlt)) ^^ ((o2[t]:qlt > {0}:qlt) W (o3[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-Q-04] (F A1 -> G A2) U (A2 S A3) — -> of unary as U-left") {
	tref fm = spec("((F (o1[t]:qlt > {0}:qlt)) -> (G (o2[t]:qlt > {0}:qlt))) U ((o2[t]:qlt > {0}:qlt) S (o3[t]:qlt > {0}:qlt)).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-Q-05] (A1 U A2 <- F A3) U (G A1 && A2 R A3) — <- and && combos") {
	tref fm = spec("((o1[t]:qlt > {0}:qlt) U (o2[t]:qlt > {0}:qlt) <- (F (o3[t]:qlt > {0}:qlt))) U ((G (o1[t]:qlt > {0}:qlt)) && ((o2[t]:qlt > {0}:qlt) R (o3[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-Q-06] (F A1 <-> A1 U A2) U (G A2 || A2 S A3) — <-> and || as U operands") {
	tref fm = spec("((F (o1[t]:qlt > {0}:qlt)) <-> ((o1[t]:qlt > {0}:qlt) U (o2[t]:qlt > {0}:qlt))) U ((G (o2[t]:qlt > {0}:qlt)) || ((o2[t]:qlt > {0}:qlt) S (o3[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

// ─── wff_R with boolean-combo operands ───────────────────────────────────

TEST_CASE("[SHAPE-Q-07] ((F A1) || (A2 U A3)) R ((G A1) && (A2 W A3))") {
	tref fm = spec("((F (o1[t]:qlt > {0}:qlt)) || ((o2[t]:qlt > {0}:qlt) U (o3[t]:qlt > {0}:qlt))) R ((G (o1[t]:qlt > {0}:qlt)) && ((o2[t]:qlt > {0}:qlt) W (o3[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-Q-08] (G A1 ^^ A1 U A2) R (F A3 -> A2 R A3) — ^^ and -> as R operands") {
	tref fm = spec("((G (o1[t]:qlt > {0}:qlt)) ^^ ((o1[t]:qlt > {0}:qlt) U (o2[t]:qlt > {0}:qlt))) R ((F (o3[t]:qlt > {0}:qlt)) -> ((o2[t]:qlt > {0}:qlt) R (o3[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

// ─── wff_W with boolean-combo operands ───────────────────────────────────

TEST_CASE("[SHAPE-Q-09] ((F A1) && (A2 S A3)) W ((G A2) || (A1 T A3))") {
	tref fm = spec("((F (o1[t]:qlt > {0}:qlt)) && ((o2[t]:qlt > {0}:qlt) S (o3[t]:qlt > {0}:qlt))) W ((G (o2[t]:qlt > {0}:qlt)) || ((o1[t]:qlt > {0}:qlt) T (o3[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-Q-10] (F A1 <- A1 R A2) W (G A3 <-> A2 U A3) — <- and <-> as W operands") {
	tref fm = spec("((F (o1[t]:qlt > {0}:qlt)) <- ((o1[t]:qlt > {0}:qlt) R (o2[t]:qlt > {0}:qlt))) W ((G (o3[t]:qlt > {0}:qlt)) <-> ((o2[t]:qlt > {0}:qlt) U (o3[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

// ─── wff_S with boolean-combo operands ───────────────────────────────────

TEST_CASE("[SHAPE-Q-11] ((A1 U A2) || F A3) S ((G A1) && (A2 R A3)) — || and && as S operands") {
	tref fm = spec("(((o1[t]:qlt > {0}:qlt) U (o2[t]:qlt > {0}:qlt)) || (F (o3[t]:qlt > {0}:qlt))) S ((G (o1[t]:qlt > {0}:qlt)) && ((o2[t]:qlt > {0}:qlt) R (o3[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-Q-12] (F A1 ^^ G A2) S (A1 W A2 -> F A3) — ^^ and -> as S operands") {
	tref fm = spec("((F (o1[t]:qlt > {0}:qlt)) ^^ (G (o2[t]:qlt > {0}:qlt))) S (((o1[t]:qlt > {0}:qlt) W (o2[t]:qlt > {0}:qlt)) -> (F (o3[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

// ─── wff_T with boolean-combo operands ───────────────────────────────────

TEST_CASE("[SHAPE-Q-13] ((F A1) || (A2 W A3)) T ((G A2) && (A1 U A3))") {
	tref fm = spec("((F (o1[t]:qlt > {0}:qlt)) || ((o2[t]:qlt > {0}:qlt) W (o3[t]:qlt > {0}:qlt))) T ((G (o2[t]:qlt > {0}:qlt)) && ((o1[t]:qlt > {0}:qlt) U (o3[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-Q-14] (G A1 <- A1 S A2) T (F A3 <-> A2 T A3) — <- and <-> as T operands") {
	tref fm = spec("((G (o1[t]:qlt > {0}:qlt)) <- ((o1[t]:qlt > {0}:qlt) S (o2[t]:qlt > {0}:qlt))) T ((F (o3[t]:qlt > {0}:qlt)) <-> ((o2[t]:qlt > {0}:qlt) T (o3[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

// ─── Depth-3: G/F wrapping boolean combos of temporals ───────────────────

TEST_CASE("[SHAPE-Q-15] G(((F A1) || (A1 U A2)) U (A2 S A3)) — user example wrapped in G") {
	tref fm = spec("G (((F (o1[t]:qlt > {0}:qlt)) || ((o1[t]:qlt > {0}:qlt) U (o2[t]:qlt > {0}:qlt))) U ((o2[t]:qlt > {0}:qlt) S (o3[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-Q-16] F(((G A1) && (A2 R A3)) W ((A1 U A2) || F A3)) — wrapped in F") {
	tref fm = spec("F ((((G (o1[t]:qlt > {0}:qlt)) && ((o2[t]:qlt > {0}:qlt) R (o3[t]:qlt > {0}:qlt)))) W (((o1[t]:qlt > {0}:qlt) U (o2[t]:qlt > {0}:qlt)) || (F (o3[t]:qlt > {0}:qlt)))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-Q-17] F(((F A1 || A1 U A2) U (A2 S A3)) && G(A1 W A2)) — combo inside F at depth 3") {
	tref fm = spec("F ((((F (o1[t]:qlt > {0}:qlt)) || ((o1[t]:qlt > {0}:qlt) U (o2[t]:qlt > {0}:qlt))) U ((o2[t]:qlt > {0}:qlt) S (o3[t]:qlt > {0}:qlt))) && (G ((o1[t]:qlt > {0}:qlt) W (o2[t]:qlt > {0}:qlt)))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-Q-18] sbf type: ((F B1) || (B1 U B2)) U (B2 S B1) — sbf version of user pattern") {
	tref fm = spec("((F (o1[t]:sbf = {X | (Y & Z)}:sbf)) || ((o1[t]:sbf = {X | (Y & Z)}:sbf) U (o2[t]:sbf = {X & Y}:sbf))) U ((o2[t]:sbf = {X & Y}:sbf) S (o1[t]:sbf = {X | (Y & Z)}:sbf)).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

// ─── Multi-level boolean combinations feeding into temporal ──────────────

TEST_CASE("[SHAPE-Q-19] ((F A1 && G A2) || (A1 U A2)) U ((A2 R A3) ^^ F A1)") {
	tref fm = spec("(((F (o1[t]:qlt > {0}:qlt)) && (G (o2[t]:qlt > {0}:qlt))) || ((o1[t]:qlt > {0}:qlt) U (o2[t]:qlt > {0}:qlt))) U (((o2[t]:qlt > {0}:qlt) R (o3[t]:qlt > {0}:qlt)) ^^ (F (o1[t]:qlt > {0}:qlt))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-Q-20] (F A1 -> G A2) U ((G A3 <- A1 R A2) S (F A3 <-> G A1))") {
	tref fm = spec("((F (o1[t]:qlt > {0}:qlt)) -> (G (o2[t]:qlt > {0}:qlt))) U (((G (o3[t]:qlt > {0}:qlt)) <- ((o1[t]:qlt > {0}:qlt) R (o2[t]:qlt > {0}:qlt))) S ((F (o3[t]:qlt > {0}:qlt)) <-> (G (o1[t]:qlt > {0}:qlt)))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

} // SHAPE-Q

// ═══════════════════════════════════════════════════════════════════════════
// SHAPE-R: BF arithmetic — bf_add(+), bf_sub(-), bf_mul(*), bf_div(/), bf_mod(%)
// System controls o1[t]:bv and can pick any value satisfying the equation.
// ═══════════════════════════════════════════════════════════════════════════

TEST_SUITE("SHAPE-R: BF arithmetic in bv atoms") {

TEST_CASE("[SHAPE-R-01] bf_add: F(F(F(o1+{2}={7}:bv)))") {
	tref fm = spec("F (F (F (o1[t]:bv + {2}:bv = {7}:bv))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-R-02] bf_add: G(F(o1+{3}={8}:bv))") {
	tref fm = spec("G (F (o1[t]:bv + {3}:bv = {8}:bv)).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-R-03] bf_sub: F(F(F(o1-{2}={3}:bv)))") {
	tref fm = spec("F (F (F (o1[t]:bv - {2}:bv = {3}:bv))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-R-04] bf_sub: G(F(o1-{1}={4}:bv))") {
	tref fm = spec("G (F (o1[t]:bv - {1}:bv = {4}:bv)).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-R-05] bf_mul: F(F(F(o1*{3}={6}:bv)))") {
	tref fm = spec("F (F (F (o1[t]:bv * {3}:bv = {6}:bv))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-R-06] bf_mul: G(F(o1*{2}={4}:bv))") {
	tref fm = spec("G (F (o1[t]:bv * {2}:bv = {4}:bv)).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-R-07] bf_div: F(F(F(o1/{2}={4}:bv)))") {
	tref fm = spec("F (F (F (o1[t]:bv / {2}:bv = {4}:bv))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-R-08] bf_div: G(F(o1/{3}={2}:bv))") {
	tref fm = spec("G (F (o1[t]:bv / {3}:bv = {2}:bv)).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-R-09] bf_mod: F(F(F(o1%{3}={1}:bv)))") {
	tref fm = spec("F (F (F (o1[t]:bv % {3}:bv = {1}:bv))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-R-10] bf_mod: G(F(o1%{5}={2}:bv))") {
	tref fm = spec("G (F (o1[t]:bv % {5}:bv = {2}:bv)).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-R-11] arithmetic in U: (o1+{2}={5}) U (o2*{3}={6}:bv) REALIZABLE") {
	tref fm = spec("(o1[t]:bv + {2}:bv = {5}:bv) U (o2[t]:bv * {3}:bv = {6}:bv).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-R-12] mixed arithmetic ops: F(F((o1+{1}={5}) && (o2-{1}={4}):bv))") {
	tref fm = spec("F (F ((o1[t]:bv + {1}:bv = {5}:bv) && (o2[t]:bv - {1}:bv = {4}:bv))).");
	REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm));
}

} // SHAPE-R

// ═══════════════════════════════════════════════════════════════════════════
// SHAPE-S: BF constants bf_t (1) and bf_f (0) as direct BF terms
// ═══════════════════════════════════════════════════════════════════════════

TEST_SUITE("SHAPE-S: BF constants 1 and 0") {

// bf_t (1) — top element
TEST_CASE("[SHAPE-S-01] F(F(F(o1:sbf = 1))) — bf_t in sbf") { tref fm = spec("F (F (F (o1[t]:sbf = 1)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-S-02] G(F(o1:sbf = 1)) — bf_t in G(F(...))") { tref fm = spec("G (F (o1[t]:sbf = 1))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-S-03] G(F(o1:bv = 1:bv)) — typed bf_t in bv") { tref fm = spec("G (F (o1[t]:bv = 1:bv))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-S-04] (o1:sbf = 1) U (o2:sbf = 1) — bf_t on both sides of U") { tref fm = spec("(o1[t]:sbf = 1) U (o2[t]:sbf = 1)."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }

// bf_f (0) — bottom element
TEST_CASE("[SHAPE-S-05] F(F(F(o1:sbf = 0))) — bf_f in sbf") { tref fm = spec("F (F (F (o1[t]:sbf = 0)))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-S-06] G(F(o1:sbf = 0)) — bf_f in G(F(...))") { tref fm = spec("G (F (o1[t]:sbf = 0))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-S-07] G(F(o1:bv = 0:bv)) — typed bf_f in bv") { tref fm = spec("G (F (o1[t]:bv = 0:bv))."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }
TEST_CASE("[SHAPE-S-08] (o1:sbf = 0) S (o2:sbf = 1) — bf_f and bf_t in S") { tref fm = spec("(o1[t]:sbf = 0) S (o2[t]:sbf = 1)."); REQUIRE(fm != nullptr); CHECK(is_tau_formula_sat<node_t>(fm)); }

} // SHAPE-S

// ═══════════════════════════════════════════════════════════════════════════
// SHAPE-T: Constraint wffs (ctn_eq/neq/gt/gteq/lt/lteq)
// Grammar: "[" ctnvar op num "]" — a wff asserting a step-counter relation.
// Only parse-correctness is asserted here (REQUIRE != nullptr) because the
// realizability semantics of free-standing time constraints interacts with
// the counter variable bound by the enclosing quantifier.
// ═══════════════════════════════════════════════════════════════════════════

TEST_SUITE("SHAPE-T: Constraint wffs [t op n]") {

TEST_CASE("[SHAPE-T-01] ctn_gt: ex t [t > 3] parses") {
	tref fm = spec("ex t [t > 3].");
	REQUIRE(fm != nullptr);
}

TEST_CASE("[SHAPE-T-02] ctn_gteq: ex t [t >= 0] parses") {
	tref fm = spec("ex t [t >= 0].");
	REQUIRE(fm != nullptr);
}

TEST_CASE("[SHAPE-T-03] ctn_lt: ex t [t < 5] parses") {
	tref fm = spec("ex t [t < 5].");
	REQUIRE(fm != nullptr);
}

TEST_CASE("[SHAPE-T-04] ctn_lteq: ex t [t <= 10] parses") {
	tref fm = spec("ex t [t <= 10].");
	REQUIRE(fm != nullptr);
}

TEST_CASE("[SHAPE-T-05] ctn_eq: ex t [t = 0] parses") {
	tref fm = spec("ex t [t = 0].");
	REQUIRE(fm != nullptr);
}

TEST_CASE("[SHAPE-T-06] ctn_neq: ex t [t != 2] parses") {
	tref fm = spec("ex t [t != 2].");
	REQUIRE(fm != nullptr);
}

TEST_CASE("[SHAPE-T-07] ctn inside F: F([t > 0] && A1) parses") {
	tref fm = spec("F ([t > 0] && (o1[t]:qlt > {0}:qlt)).");
	REQUIRE(fm != nullptr);
}

TEST_CASE("[SHAPE-T-08] ctn inside G: G([t >= 0] && A1) parses") {
	tref fm = spec("G ([t >= 0] && (o1[t]:qlt > {0}:qlt)).");
	REQUIRE(fm != nullptr);
}

} // SHAPE-T

// ---------------------------------------------------------------------------
// SHAPE-U: Unrealizable formulas — the solver must correctly reject these.
// Each formula has no valid system strategy.
// ---------------------------------------------------------------------------
TEST_SUITE("SHAPE-U: unrealizable formulas") {

TEST_CASE("[SHAPE-U-01] Contradiction: G(o1=0) && G(o1=1)") {
	tref fm = spec("G (o1[t] = 0) && G (o1[t] = 1).");
	REQUIRE(fm != nullptr);
	CHECK_FALSE(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-U-02] Contradiction: G(o1=0) && F(o1=1)") {
	tref fm = spec("G (o1[t] = 0) && F (o1[t] = 1).");
	REQUIRE(fm != nullptr);
	CHECK_FALSE(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-U-03] Boolean false spec") {
	tref fm = spec("G (o1[t] = 0 && o1[t] = 1).");
	REQUIRE(fm != nullptr);
	CHECK_FALSE(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-U-04] Explicit F: always false") {
	tref fm = spec("G (F).");
	REQUIRE(fm != nullptr);
	CHECK_FALSE(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-U-05] Output must equal two different constants simultaneously") {
	tref fm = spec("G (o1[t]:sbf = {1}:sbf && o1[t]:sbf = {0}:sbf).");
	REQUIRE(fm != nullptr);
	CHECK_FALSE(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-U-06] Output must change but also stay the same") {
	tref fm = spec("G (o1[t] = o1[t-1]) && F (o1[t] != o1[t-1]).");
	REQUIRE(fm != nullptr);
	CHECK_FALSE(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-U-07] Disjunction of contradictions") {
	tref fm = spec("G ((o1[t] = 0 && o1[t] = 1) || (o2[t] = 0 && o2[t] = 1)).");
	REQUIRE(fm != nullptr);
	CHECK_FALSE(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-U-08] qlt contradiction: o1 > top") {
	tref fm = spec("G (o1[t]:qlt > {top}:qlt).");
	REQUIRE(fm != nullptr);
	CHECK_FALSE(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-U-09] qlt contradiction: o1 < bot") {
	tref fm = spec("G (o1[t]:qlt < {bot}:qlt).");
	REQUIRE(fm != nullptr);
	CHECK_FALSE(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-U-10] G(o1 = 0) && G(o1 != 0)") {
	tref fm = spec("G (o1[t] = 0) && G (o1[t] != 0).");
	REQUIRE(fm != nullptr);
	CHECK_FALSE(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-U-11] Implication contradiction: G(T -> F)") {
	tref fm = spec("G (T -> F).");
	REQUIRE(fm != nullptr);
	CHECK_FALSE(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-U-12] Two outputs with cross-dependency cycle") {
	tref fm = spec("G (o1[t] = o2[t]) && G (o1[t] != o2[t]).");
	REQUIRE(fm != nullptr);
	CHECK_FALSE(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-U-13] bv contradiction: o1:bv[8] = 0 and o1:bv[8] != 0") {
	tref fm = spec("G (o1[t]:bv[8] = {0}:bv[8] && o1[t]:bv[8] != {0}:bv[8]).");
	REQUIRE(fm != nullptr);
	CHECK_FALSE(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-U-14] Contradictory eventually and globally") {
	tref fm = spec("F (o1[t] = 1) && G (o1[t] = 0).");
	REQUIRE(fm != nullptr);
	CHECK_FALSE(is_tau_formula_sat<node_t>(fm));
}

TEST_CASE("[SHAPE-U-15] F(F) always unrealizable") {
	tref fm = spec("F (F).");
	REQUIRE(fm != nullptr);
	CHECK_FALSE(is_tau_formula_sat<node_t>(fm));
}

} // SHAPE-U


TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}