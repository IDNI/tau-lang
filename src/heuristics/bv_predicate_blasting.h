/**
 * @file bv_predicate_blasting.h
 * @brief Bitvector predicate blasting: rewrite BV ops to quantified Boolean predicates.
 *
 * Provides `bv_predicate_blasting` (the main entry point) and helpers that
 * decompose BV relational/arithmetic operations into quantifier-free Boolean
 * formulas over per-bit variables.  Unsupported operations cause a `nullptr`
 * return so the caller can fall back to the cvc5 path.
 */

// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BV_PREDICATE_BLASTING_H__
#define __IDNI__TAU__BV_PREDICATE_BLASTING_H__

#include <functional>

#include "tau_tree.h"
#include "tau_bdd.h"

namespace idni::tau_lang {

// Forward declarations needed by wff_predicate_blasting/quantify_aux_vars
// (bv_predicate_blasting.tmpl.h) to anti-prenex/eliminate its own
// freshly-introduced auxiliary bv-typed quantifiers. The full
// declarations/definitions come from
// normal_forms_transformations.h/normal_forms.tmpl.h; including those
// headers directly here would create a header include cycle
// (normal_forms_transformations.h includes this file). No default argument
// is given here, since a default cannot be redeclared and these call sites
// always pass an explicit predicate.
template<NodeType node>
tref anti_prenex_block(tref formula, std::function<bool(tref)> skip);

template<NodeType node>
tref resolve_quantifiers2(tref formula, const typename term_handle<node>::order& order,
	std::function<bool(tref)> skip);

// NOT thread-safe: Controls whether bitvector blasting is enabled.
// The tau library assumes single-threaded access.
// Do not call set_blasting() concurrently from multiple threads.
inline bool bv_blasting = true;

/**
 * @brief Entry point for predicate blasting on bitvector formulas.
 *
 * Rewrites bitvector formulas into equivalent Boolean formulas containing
 * existentially quantified variables and predicates. Only supports a limited set of
 * bitvector operations. If unsupported operations or predicates are encountered,
 * it returns nullptr to indicate failure, and the caller can choose to leave the
 * formula unchanged.
 *
 * See implementation in bv_predicate_blasting.tmpl.h and its companion
 * files bv_predicate_blasting_logic.tmpl.h,
 * bv_predicate_blasting_comparisons.tmpl.h and
 * bv_predicate_blasting_arithmetic.tmpl.h for details.
 *
 * @tparam node Node type
 * @param term The formula to blast
 * @return The formula with predicates blasted, or nullptr on error
 *
 * @par Example
 * @code{.cpp}
 * // 3 + 5 = 8 for 4-bit bitvectors (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:70-72).
 * tref fm = get_nso_rr(
 *     "ex x (x = { 3 }:bv[4] && x + { 5 }:bv[4] = { 8 }:bv[4]).").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
tref bv_predicate_blasting(tref term);

/**
 * @brief Computes a term giving the value of a specific bit in a bitvector.
 * @tparam node Node type
 * @param operand Operand
 * @param bit Bit index
 * @return The resulting term
 *
 * @par Example
 * @code{.cpp}
 * // bit(x, 0) is x masked-and with the { 1 } bit-0 selector
 * auto x = tau::build_bf_variable(bv_type_id<node_t>(4));
 * tref bit0 = bit<node_t>(x, 0);
 * CHECK( tau::get(bit0).find_top(is<node_t, tau::bf_and>) != nullptr );
 * @endcode
 */
template<NodeType node>
tref bit(tref operand, int_t bit);

/**
 * @brief Computes a predicate checking if shifted is the result of shifting operand left by one bit.
 *
 * @tparam node Node type
 * @param base Operand to shift
 * @param shifted Result variable
 * @return The resulting term
 *
 * @par Example
 * @code{.cpp}
 * // Constrains bit 0 of "shifted" to 0 and bit i+1 of "shifted" to bit i of x
 * auto x = tau::build_bf_variable(bv_type_id<node_t>(4));
 * auto shifted = tau::build_bf_variable(bv_type_id<node_t>(4));
 * tref constraint = bvshl_by_one<node_t>(x, shifted);
 * CHECK( is_non_temp_nso_satisfiable<node_t>(constraint) );
 * @endcode
 */
template<NodeType node>
tref bvshl_by_one(tref base, tref shifted);

/**
 * @brief Computes a predicate checking if shifted is the result of shifting operand right by one bit.
 *
 * @tparam node Node type
 * @param base Operand to shift
 * @param shifted Result variable
 * @return The resulting predicate
 *
 * @par Example
 * @code{.cpp}
 * // Constrains the top bit of "shifted" to 0 and bit i of "shifted" to bit
 * // i+1 of x -- structurally distinct from bvshl_by_one's constraint.
 * auto x = tau::build_bf_variable(bv_type_id<node_t>(4));
 * auto shifted = tau::build_bf_variable(bv_type_id<node_t>(4));
 * tref constraint = bvshr_by_one<node_t>(x, shifted);
 * CHECK( is_non_temp_nso_satisfiable<node_t>(constraint) );
 * @endcode
 */
template<NodeType node>
tref bvshr_by_one(tref base, tref shifted);

/**
 * @brief Computes a predicate checking if shifted is the result of shifting
 * operand left by a constant amount.
 *
 * @tparam node Node type
 * @param base Operand to shift
 * @param count Shift amount (constant)
 * @param shifted Result variable
 * @return The resulting predicate
 *
 * @par Example
 * @code{.cpp}
 * // 3 << 3 = 8 for 4-bit: only the lowest source bit still fits (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:239-241).
 * tref fm = get_nso_rr(
 *     "ex x ex y (x = { 3 }:bv[4] && x << { 3 }:bv[4] = y && "
 *     "y = { 8 }:bv[4]).").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
tref bvshl(tref base, tref count, tref shifted);

/**
 * @brief Computes a predicate checking if shifted is the result of shifting
 * operand right by a constant amount.
 *
 * @tparam node Node type
 * @param base Operand to shift
 * @param count Shift amount (constant)
 * @param shifted Result variable
 * @return The resulting predicate
 *
 * @par Example
 * @code{.cpp}
 * // 15 >> 4 = 0 for 4-bit: the whole value shifts out (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:297-299).
 * tref fm = get_nso_rr(
 *     "ex x ex y (x = { 15 }:bv[4] && x >> { 4 }:bv[4] = y && "
 *     "y = { 0 }:bv[4]).").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
tref bvshr(tref base, tref count, tref shifted);

/**
 * @brief Computes a predicate constraining result to be the cast of src to a
 * different bitwidth.
 *
 * For zero-extension (src bitwidth < result bitwidth):
 *   - Low bits of result match source bits
 *   - High bits of result are zero
 *
 * For truncation (src bitwidth > result bitwidth):
 *   - Result bits match the low bits of source
 *
 * @tparam node Node type
 * @param src Source bitvector
 * @param result Result bitvector (fresh variable of target type)
 * @return The resulting predicate term
 *
 * @par Example
 * @code{.cpp}
 * // Zero-extending {3}:bv[2] to bv[4] gives {3}:bv[4], not {11}:bv[4] (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:779-789).
 * tref fm_ok = get_nso_rr(
 *     "ex x (x = { 3 }:bv[2] && (bv[4]) x = { 3 }:bv[4]).").value().main->get();
 * CHECK( tau::get(normalizer<node_t>(
 *     bv_predicate_blasting<node_t>(fm_ok))).equals_T() );
 *
 * tref fm_bad = get_nso_rr(
 *     "ex x (x = { 3 }:bv[2] && (bv[4]) x = { 11 }:bv[4]).").value().main->get();
 * CHECK( tau::get(normalizer<node_t>(
 *     bv_predicate_blasting<node_t>(fm_bad))).equals_F() );
 * @endcode
 */
template<NodeType node>
tref bvcast(tref src, tref result);

/**
 * @brief Computes a predicate checking if left is not equal to right as bitvectors.
 *
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @return The resulting predicate
 *
 * @note Unlike @ref bvlt / @ref bvgt, this function is never invoked by the
 * WFF dispatcher: `!=` surface syntax routes through `neq_predicate`, which
 * calls `keep_comparison_predicate` instead of this bit-recurrence. It is a
 * standalone entry point; both operands must carry a proper bv type (e.g.
 * built via `bv_type_id<node>(width)`) for the underlying recurrence to
 * resolve -- a bare `ba_constant` operand does not carry a bitwidth
 * `get_bv_type_bitwidth` can read, and the recurrence never resolves.
 *
 * @par Example
 * @code{.cpp}
 * // x != x is never satisfiable (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting_predicates.cpp,
 * // TEST_CASE("bvneq: x != x is never satisfiable")).
 * auto x = tau::build_bf_variable(bv_type_id<node_t>(4));
 * tref pred = bvneq<node_t>(x, x);
 * CHECK( tau::get(normalizer<node_t>(pred)).equals_F() );
 * @endcode
 */
template<NodeType node>
tref bvneq(tref left, tref right);

/**
 * @brief Computes a predicate checking if left is not equal to right as bitvectors.
 *
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @return The resulting predicate
 */
template<NodeType node>
tref bvneq(tref left, tref right);

/**
 * @brief Computes a predicate checking if left is less than right as bitvectors.
 *
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @return The resulting predicate
 *
 * @par Example
 * @code{.cpp}
 * // 2 < 3 for 2-bit bitvectors, differing only at the LSB (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:340-341).
 * tref fm = get_nso_rr(
 *     "ex x (x = { 2 }:bv[2] && x < { 3 }:bv[2]).").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
tref bvlt(tref left, tref right);

/**
 * @brief Computes a predicate checking if left is greater than right as bitvectors.
 *
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @return The resulting predicate
 *
 * @par Example
 * @code{.cpp}
 * // x > x is never satisfiable (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:362-363).
 * tref fm = get_nso_rr("ex x x:bv[4] > x:bv[4].").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_F() );
 * @endcode
 */
template<NodeType node>
tref bvgt(tref left, tref right);

/**
 * @brief Computes a predicate checking if left is less than or equal to right as bitvectors.
 *
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @return The resulting predicate
 *
 * @note Unlike @ref bvlt / @ref bvgt, this wrapper is never invoked by
 * `wff_predicate_blasting`'s dispatcher: `<=` surface syntax is routed
 * through `lteq_predicate` instead, which independently negates
 * `lt_predicate` rather than calling this function. `bvlteq` is a
 * standalone convenience entry point (see
 * tests/integration/test_integration-heuristics-bv_predicate_blasting_predicates.cpp,
 * `TEST_CASE("bvlteq delegates to bvgt")`).
 *
 * @par Example
 * @code{.cpp}
 * // By construction, bvlteq(l, r) is exactly !bvgt(l, r)
 * auto x = tau::build_bf_variable(bv_type_id<node_t>(4));
 * auto y = tau::build_bf_variable(bv_type_id<node_t>(4));
 * CHECK( tau::get(bvlteq<node_t>(x, y))
 *     == tau::get(tau::build_wff_neg(bvgt<node_t>(x, y))) );
 * @endcode
 */
template<NodeType node>
tref bvlteq(tref left, tref right) {
	using tau = tree<node>;

	return tau::build_wff_neg(bvgt<node>(left, right)); }

/**
 * @brief Computes a predicate checking if left is greater or equal to right as bitvectors.
 *
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @return The resulting predicate
 *
 * @note Like @ref bvlteq, this is a standalone wrapper never invoked by the
 * WFF dispatcher (`>=` routes through `gteq_predicate`, which independently
 * negates `lt_predicate`).
 *
 * @par Example
 * @code{.cpp}
 * // By construction, bvgteq(l, r) is exactly !bvlt(l, r)
 * auto x = tau::build_bf_variable(bv_type_id<node_t>(4));
 * auto y = tau::build_bf_variable(bv_type_id<node_t>(4));
 * CHECK( tau::get(bvgteq<node_t>(x, y))
 *     == tau::get(tau::build_wff_neg(bvlt<node_t>(x, y))) );
 * @endcode
 */
template<NodeType node>
tref bvgteq(tref left, tref right) {
	using tau = tree<node>;

	return tau::build_wff_neg(bvlt<node>(left, right));
}

/**
 * @brief Computes a predicate checking if left is not less than or equal to right as bitvectors.
 *
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @return The resulting predicate
 *
 * @note Standalone wrapper, never invoked by the WFF dispatcher (`!<=`
 * routes through `nlteq_predicate`, which independently calls
 * `gt_predicate`).
 *
 * @par Example
 * @code{.cpp}
 * // By construction, bvnlteq(l, r) is exactly bvgt(l, r)
 * auto x = tau::build_bf_variable(bv_type_id<node_t>(4));
 * auto y = tau::build_bf_variable(bv_type_id<node_t>(4));
 * CHECK( tau::get(bvnlteq<node_t>(x, y)) == tau::get(bvgt<node_t>(x, y)) );
 * @endcode
 */
template<NodeType node>
tref bvnlteq(tref left, tref right) { return bvgt<node>(left, right); }

/**
 * @brief Computes a predicate checking if left is not greater or equal to right as bitvectors.
 *
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @return The resulting predicate
 *
 * @note Standalone wrapper, never invoked by the WFF dispatcher (`!>=`
 * routes through `ngteq_predicate`, which independently calls
 * `lt_predicate`). See tests/integration/test_integration-heuristics-bv_predicate_blasting_predicates.cpp,
 * `TEST_CASE("bvngteq delegates to bvlt")`.
 *
 * @par Example
 * @code{.cpp}
 * // By construction, bvngteq(l, r) is exactly bvlt(l, r)
 * auto x = tau::build_bf_variable(bv_type_id<node_t>(4));
 * auto y = tau::build_bf_variable(bv_type_id<node_t>(4));
 * CHECK( tau::get(bvngteq<node_t>(x, y)) == tau::get(bvlt<node_t>(x, y)) );
 * @endcode
 */
template<NodeType node>
tref bvngteq(tref left, tref right) { return bvlt<node>(left, right); }

/**
 * @brief Computes a predicate checking if left is not less than right as bitvectors.
 *
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @return The resulting predicate
 *
 * @note Standalone wrapper, never invoked by the WFF dispatcher (`!<`
 * routes through `nlt_predicate`, which independently calls `lt_predicate`).
 *
 * @par Example
 * @code{.cpp}
 * // By construction, bvnlt(l, r) is exactly bvgteq(l, r)
 * auto x = tau::build_bf_variable(bv_type_id<node_t>(4));
 * auto y = tau::build_bf_variable(bv_type_id<node_t>(4));
 * CHECK( tau::get(bvnlt<node_t>(x, y)) == tau::get(bvgteq<node_t>(x, y)) );
 * @endcode
 */
template<NodeType node>
tref bvnlt(tref left, tref right) { return bvgteq<node>(left, right); }

/**
 * @brief Computes a predicate checking if left is not greater than right as bitvectors.
 *
 * @tparam node Node type
 * @param left Left operand
 * @param right Right operand
 * @return The resulting predicate
 *
 * @note Standalone wrapper, never invoked by the WFF dispatcher (`!>` routes
 * through `ngt_predicate`, which independently calls `gt_predicate`). See
 * tests/integration/test_integration-heuristics-bv_predicate_blasting_predicates.cpp,
 * `TEST_CASE("bvngt delegates to bvlteq")`.
 *
 * @par Example
 * @code{.cpp}
 * // By construction, bvngt(l, r) is exactly bvlteq(l, r)
 * auto x = tau::build_bf_variable(bv_type_id<node_t>(4));
 * auto y = tau::build_bf_variable(bv_type_id<node_t>(4));
 * CHECK( tau::get(bvngt<node_t>(x, y)) == tau::get(bvlteq<node_t>(x, y)) );
 * @endcode
 */
template<NodeType node>
tref bvngt(tref left, tref right) { return bvlteq<node>(left, right); }

/**
 * @brief Computes a predicate constraining sum to be the addition of the operands.
 *
 * The constraint formula is quantifier free; the fresh auxiliary variables it
 * introduces (the carries) are appended to aux and must be existentially
 * quantified by the caller.
 *
 * @tparam node Node type
 * @param augend Left operand
 * @param addend Right operand
 * @param sum Result variable
 * @param aux Collects the fresh auxiliary variables
 * @return The resulting predicate term, or nullptr on error
 *
 * @par Example
 * @code{.cpp}
 * // 15 + 1 = 0 for 4-bit (overflow wraps mod 2^4) (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:82-84).
 * tref fm = get_nso_rr(
 *     "ex x (x = { 15 }:bv[4] && x + { 1 }:bv[4] = { 0 }:bv[4]).").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
tref bvadd(tref augend, tref addend, tref sum, trefs& aux);

/**
 * @brief Computes a predicate constraining difference to be the subtraction
 * of the operands.
 *
 * The constraint formula is quantifier free; the fresh auxiliary variables it
 * introduces (the borrows) are appended to aux and must be existentially
 * quantified by the caller.
 *
 * @tparam node Node type
 * @param minuend Left operand
 * @param subtrahend Right operand
 * @param difference Result variable
 * @param aux Collects the fresh auxiliary variables
 * @return The resulting predicate term, or nullptr on error
 *
 * @par Example
 * @code{.cpp}
 * // 0 - 1 = 15 for 4-bit (underflow wraps mod 2^4) (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:116-118).
 * tref fm = get_nso_rr(
 *     "ex x (x = { 0 }:bv[4] && x - { 1 }:bv[4] = { 15 }:bv[4]).").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
tref bvsub(tref minuend, tref subtrahend, tref difference, trefs& aux);

/**
 * @brief Computes a predicate constraining product to be the multiplication
 * of the operands.
 *
 * Only supports multiplication by a constant multiplier. The constraint
 * formula is quantifier free; the fresh auxiliary variables it introduces
 * (shifted summands and partial sums) are appended to aux and must be
 * existentially quantified by the caller.
 *
 * @tparam node Node type
 * @param multiplicand Left operand
 * @param multiplier Right operand (must be constant)
 * @param product Result variable
 * @param aux Collects the fresh auxiliary variables
 * @return The resulting predicate term, or nullptr if unsupported
 *
 * @par Example
 * @code{.cpp}
 * // 3 * 6 = 2 for 4-bit (18 mod 16 = 2, overflow) (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:151-152).
 * tref fm = get_nso_rr(
 *     "ex x (x = { 3 }:bv[4] && x * { 6 }:bv[4] = { 2 }:bv[4]).").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
tref bvmul(tref multiplicand, tref multiplier, tref product, trefs& aux);

/**
 * @brief Computes the Euclidean division predicate constraining quotient and
 * remainder at once.
 *
 * Only supports division by a constant divisor. The constraint formula is
 * quantifier free; the fresh auxiliary variables it introduces are appended
 * to aux and must be existentially quantified by the caller.
 *
 * @tparam node Node type
 * @param dividend Dividend
 * @param divisor Divisor (must be constant)
 * @param quotient Quotient variable
 * @param remainder Remainder variable
 * @param aux Collects the fresh auxiliary variables
 * @return The resulting predicate term, or nullptr if unsupported
 *
 * @note Never invoked by the library itself: `atomic_blasting` dispatches
 * `/` and `%` to @ref bvdiv and @ref bvmod separately rather than fusing
 * them into a single `bved` call, so this has no WFF-level coverage. See
 * tests/integration/test_integration-heuristics-bv_predicate_blasting_predicates.cpp,
 * `TEST_CASE("bved: 10 / 3 gives quotient=3, remainder=1")`.
 *
 * @par Example
 * @code{.cpp}
 * // 10 = 3*3 + 1, so quotient=3, remainder=1 satisfies the constraint;
 * // quotient=3, remainder=2 does not.
 * trefs aux;
 * auto x = tau::build_bf_variable(bv_type_id<node_t>(4));
 * auto q = tau::build_bf_variable(bv_type_id<node_t>(4));
 * auto r = tau::build_bf_variable(bv_type_id<node_t>(4));
 * auto divisor = tau::get(tau::bf, tau::get_bv_constant(4, 3));
 * tref constraint = bved<node_t>(x, divisor, q, r, aux);
 * tref x10 = tau::build_bf_eq(x, tau::get(tau::bf, tau::get_bv_constant(4, 10)));
 * tref q3 = tau::build_bf_eq(q, tau::get(tau::bf, tau::get_bv_constant(4, 3)));
 * tref r1 = tau::build_bf_eq(r, tau::get(tau::bf, tau::get_bv_constant(4, 1)));
 * tref good = tau::build_wff_and(tau::build_wff_and(constraint, x10),
 *     tau::build_wff_and(q3, r1));
 * CHECK( is_non_temp_nso_satisfiable<node_t>(good) );
 * @endcode
 */
template<NodeType node>
tref bved(tref dividend, tref divisor, tref quotient, tref remainder,
	trefs& aux);

/**
 * @brief Computes a predicate constraining quotient to be the division of
 * dividend by divisor.
 *
 * Only supports division by a constant divisor. The constraint formula is
 * quantifier free; the fresh auxiliary variables it introduces are appended
 * to aux and must be existentially quantified by the caller.
 *
 * @tparam node Node type
 * @param dividend Dividend
 * @param divisor Divisor (must be constant)
 * @param quotient Quotient variable
 * @param aux Collects the fresh auxiliary variables
 * @return The resulting predicate term, or nullptr if unsupported
 *
 * @par Example
 * @code{.cpp}
 * // 10 / 3 = 3 (integer division) (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:451-452).
 * tref fm = get_nso_rr(
 *     "ex x ex y (x = { 10 }:bv[4] && x / { 3 }:bv[4] = y && "
 *     "y = { 3 }:bv[4]).").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
tref bvdiv(tref dividend, tref divisor, tref quotient, trefs& aux);

/**
 * @brief Computes a predicate constraining remainder to be the modulo of
 * dividend by divisor.
 *
 * Only supports modulo by a constant divisor. The constraint formula is
 * quantifier free; the fresh auxiliary variables it introduces are appended
 * to aux and must be existentially quantified by the caller.
 *
 * @tparam node Node type
 * @param dividend Dividend
 * @param divisor Divisor (must be constant)
 * @param remainder Remainder variable
 * @param aux Collects the fresh auxiliary variables
 * @return The resulting predicate term, or nullptr if unsupported
 *
 * @par Example
 * @code{.cpp}
 * // 10 % 3 = 1 (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:486-487).
 * tref fm = get_nso_rr(
 *     "ex x ex y (x = { 10 }:bv[4] && x % { 3 }:bv[4] = y && "
 *     "y = { 1 }:bv[4]).").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
tref bvmod(tref dividend, tref divisor, tref remainder, trefs& aux);

} // namespace idni::tau_lang



#include "bv_predicate_blasting_helpers.tmpl.h"
#include "bv_predicate_blasting_logic.tmpl.h"
#include "bv_predicate_blasting_comparisons.tmpl.h"
#include "bv_predicate_blasting_arithmetic.tmpl.h"
#include "bv_predicate_blasting.tmpl.h"

#endif // __IDNI__TAU__BV_PREDICATE_BLASTING_H__