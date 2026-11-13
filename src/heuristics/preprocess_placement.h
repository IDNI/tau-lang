/**
 * @file preprocess_placement.h
 * @brief Runtime parameters selecting WHETHER and WHERE a BA's preprocessing
 * pass may run, and what to do with a preprocessed result.
 *
 * Preprocessing is a pack-level *policy*: `pack_preprocess` chains every BA's
 * `preprocess` in pack order (only bv rewrites anything, as predicate
 * blasting; the rest return their input unchanged), and core decides whether
 * to invoke it, where, and how to fold its result back in -- without knowing
 * which algebra, if any, actually rewrites the formula.
 * That is why these three knobs live here, in core, rather than beside
 * `bv_predicate_blasting` where the pass itself lives: a pack without bv
 * still needs somewhere to keep "preprocessing is off" and its placement
 * defaults, and this header must compile the same way regardless of the
 * configured pack.
 *
 * They are split into their own dependency-free header so that they can be
 * read and written without the tau tree behind them: `tests/test_init.h`
 * defines every test binary's `main()` and is included before any tau header,
 * so it cannot pull in `boolean_algebras/bv/heuristics/bv_predicate_blasting.h`
 * -- it includes this file instead and applies the `TAU_*` environment
 * overrides before the first test runs. `bv_predicate_blasting.h` includes
 * this file too, so bv's own pass reads the same knobs core writes.
 *
 * All three are plain runtime globals, per the project's runtime-parameter
 * policy (placement and resource limits belong in a runtime parameter, never in
 * a header constant). The api setters are `api::set_preprocessing`,
 * `api::set_preprocess_placement` and `api::set_preprocess_method`.
 *
 * `solver_site`/`solver_placement` live in `backends/cvc5/cvc5_options.h`,
 * next to the cvc5 option-set selection, and are included below so a user of
 * this header sees them too.
 *
 * At every call site gated on these knobs, a non-default value takes the
 * decline/fall-through path that site already has. `preprocessing` defaults
 * to `false` -- see the measurement table above its definition below.
 * `preprocess_placement`/`preprocess_method` default to the best-measured
 * fallback for a caller that re-enables `preprocessing`.
 */

// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__PREPROCESS_PLACEMENT_H__
#define __IDNI__TAU__PREPROCESS_PLACEMENT_H__

// `solver_site`/`solver_placement`, visible to every user of this header.
#include "backends/cvc5/cvc5_options.h"

namespace idni::tau_lang {

// Measurement matrix behind the defaults below (devel 254996f6, release
// build, hephaestus, 2026-11-13; ctest -j 1, median of three rounds; a
// cell that hit a cap in its first round ran once). W1 = the satisfiability
// suites: test_integration-satisfiability-bv, test_integration-satisfiability6
// and test_integration-wff_normalization (the correctness canary). W2 =
// test_integration-bv_stress_check over its default cases (its own 600 s
// cap). W3 = the bitvector suites the Benchmarks workflow watches:
// test_repl-sat_cmd-bv_falling_output, test_repl-realizable_cmd-bv_falling_output,
// the test_repl-blasting_equiv-* suites and
// test_integration-bv_quantifier_free_decision. Combo names: `A0/A1/A2` are
// `preprocessing=off` crossed with `solver_placement`
// (`eager`/`per_closed_block`/`per_formula`); `B <placement>,<method>` is
// `preprocessing=on` crossed with `preprocess_placement`/`preprocess_method`,
// `solver_placement` held at `eager`. The CLI reads `preprocessing` only,
// so the W3 REPL tests of a B row vary by that switch alone.
//
// | combo                   | W1      | W2      | W3                   |
// |-------------------------|---------|---------|----------------------|
// | A0 off,eager            | 2.9 s   | 6.4 s   | 24.8 s               |
// | A1 off,per_closed_block | 3.4 s   | 5.6 s   | FAIL (115 s)         |
// | A2 off,per_formula      | 3.4 s   | 5.8 s   | FAIL (119 s)         |
// | B leaf,apx              | 19.3 s  | TIMEOUT | TIMEOUT              |
// | B leaf,defer            | 19.7 s  | TIMEOUT | TIMEOUT              |
// | B block,apx             | 12.5 s  | TIMEOUT | TIMEOUT              |
// | B block,defer           | 12.5 s  | TIMEOUT | TIMEOUT              |
// | B formula,apx           | FAIL (643 s) | TIMEOUT | TIMEOUT         |
// | B formula,defer         | FAIL (650 s) | TIMEOUT | TIMEOUT         |
//
// A1/A2 lose the quantifier-free decision: under a non-eager
// `solver_placement` the closed questions of
// test_integration-bv_quantifier_free_decision never reach it (its counter
// case fails) and take the quantified path, 90 s against 0.8 s under A0.
// Every B row times out on W2 (the stress
// check at its 600 s cap) and on W3 (the two falling-output suites), and
// the `per_formula` rows break W1 as well (test_integration-satisfiability-bv
// times out, test_integration-wff_normalization fails). A0 is the only
// cell that passes every workload.
//
// `preprocessing` defaults to `false`, which is what the measurement above
// argues for. cvc5 already bit-blasts internally, so predicate blasting on
// top of it hands cvc5 thousands of auxiliary quantifiers to re-decide
// instead of letting it work on the arithmetic natively; the bits-first
// decisions of the bitvector algebra (`bv-bitblast-max-nodes`,
// `bv-bitblast-qf-max-nodes`) take the narrow questions that blasting used
// to win. The REPL, the CLI and the library all read this global, so the
// three agree on the value. `solver_placement` stays `eager` (A0's own row:
// beats A1/A2 on correctness, and W3 outright). `preprocess_placement` stays
// `per_leaf` and `preprocess_method` stays `anti_prenex_result`: among the
// preprocessing-on rows `per_block` is the faster one on W1 (12.5 s against
// 19.3 s), both methods alike, and no row makes preprocessing worth turning
// on, so the fallback stays the placement every leaf-level site is written
// for.
//
// NOT thread-safe: Controls whether a BA's preprocessing pass (bv's predicate
// blasting) is enabled. The tau library assumes single-threaded access.
// Do not call set_preprocessing() concurrently from multiple threads.
inline bool preprocessing = false;

/**
 * @brief Where a BA's preprocessing pass is allowed to run.
 */
enum class preprocess_site {
	/// `blast_block` on every leaf clause the block
	/// machinery hands it, `leaf_clause`'s own bv branch, and
	/// `resolve_quantifiers` on every bv-typed scope it walks.
	per_leaf = 0,
	/// Once per fully-processed quantifier block, and only when that block
	/// actually displaced blasteable binders.
	per_block = 1,
	/// Once per formula, after the final anti-prenex/resolve pass of
	/// `eliminate_arithmetic_and_quantifiers`.
	per_formula = 2,
};

// Inert at the shipped `preprocessing=false` default: only consulted once
// preprocessing is re-enabled. `per_leaf` is the when-enabled fallback: no
// preprocessing-on row of the matrix (see the table above `preprocessing`)
// passes W2 or W3, `per_formula` breaks W1 as well, and `per_block` is the
// faster row on W1 alone.
//
// NOT thread-safe, exactly like `preprocessing` above: the tau library
// assumes single-threaded access. Do not call set_preprocess_placement()
// concurrently.
inline preprocess_site preprocess_placement = preprocess_site::per_leaf;

/**
 * @brief What to do with a formula the preprocessing pass rewrote.
 */
enum class preprocess_mode {
	/// Re-enter `anti_prenex` (with an arithmetic-only floor) on
	/// the rewritten formula, so the auxiliary quantifiers preprocessing
	/// introduced are pushed and resolved right away.
	anti_prenex_result = 0,
	/// Rewrite only; the introduced quantifiers are left for whatever
	/// resolve pass comes next.
	defer = 1,
};

// Inert at the shipped `preprocessing=false` default, same as
// `preprocess_placement` above. `anti_prenex_result` is the same
// when-enabled fallback pick, for the same matrix reasoning.
//
// NOT thread-safe, exactly like `preprocessing` above: the tau library
// assumes single-threaded access. Do not call set_preprocess_method()
// concurrently.
inline preprocess_mode preprocess_method = preprocess_mode::anti_prenex_result;

} // namespace idni::tau_lang

#endif // __IDNI__TAU__PREPROCESS_PLACEMENT_H__
