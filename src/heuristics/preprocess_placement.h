/**
 * @file preprocess_placement.h
 * @brief Runtime parameters selecting WHETHER and WHERE a BA's preprocessing
 * pass may run, and what to do with a preprocessed result.
 *
 * Preprocessing is a pack-level *policy*: `pack_preprocess` is an optional
 * capability any BA may declare (today only bv does, as predicate blasting),
 * and core decides whether to invoke it, where, and how to fold its result
 * back in -- without knowing which algebra, if any, actually implements it.
 * That is why these three knobs live here, in core, rather than beside
 * `bv_predicate_blasting` where the pass itself lives: a pack without bv
 * still needs somewhere to keep "preprocessing is off" and its placement
 * defaults, and this header must compile the same way regardless of the
 * configured pack.
 *
 * They are split into their own dependency-free header purely so that they
 * can be read and written without the tau tree behind them:
 * `tests/test_init.h` defines every test binary's `main()` and is included
 * before any tau header, so it cannot pull in
 * `heuristics/bv_predicate_blasting.h` -- it includes this file instead and
 * applies the `TAU_*` environment overrides before the first test runs.
 * `bv_predicate_blasting.h` includes this file too, so bv's own pass reads
 * the same knobs core writes.
 *
 * All three are plain runtime globals, per the project's runtime-parameter
 * policy (placement and resource limits belong in a runtime parameter, never in
 * a header constant). The api setters are `api::set_preprocessing`,
 * `api::set_preprocess_placement` and `api::set_preprocess_method`.
 *
 * `solver_site`/`solver_placement` used to be the fourth knob here; they
 * moved to `backends/cvc5/cvc5_options.h` (included below, so every
 * existing user keeps seeing them unchanged) next to the cvc5 option-set
 * selection they belong with.
 *
 * All defaults reproduced today's behaviour exactly at introduction;
 * every call site gated on them wraps the pre-existing code, and the
 * non-default branches take the decline/fall-through path that site already
 * had. Task 9 (2026-08-15) measured the full matrix and flipped `preprocessing`
 * to `false` -- see the measurement table above its definition below.
 * `preprocess_placement`/`preprocess_method` keep their original defaults as the
 * best-measured fallback for a caller that re-enables `preprocessing`
 * (`solver_placement=eager`, the measured winner, is documented at its new
 * home).
 */

// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__PREPROCESS_PLACEMENT_H__
#define __IDNI__TAU__PREPROCESS_PLACEMENT_H__

// `solver_site`/`solver_placement` lived here until 2026-08-16; kept visible
// through this include so existing users compile unchanged.
#include "backends/cvc5/cvc5_options.h"

namespace idni::tau_lang {

// Task 9 measurement matrix (Release, `./dev release -DTAU_BUILD_JOBS=12`,
// 2026-08-15). W1 = test_integration-satisfiability2 wall (regression
// canary, baseline ~24s). W2 = test_integration-bv_stress_check wall over
// its default cases. W3 = the same binary against
// andrei-bv-stress-artifacts/oldrules16.txt with TAU_STRESS_OLDRULES_N=2,
// 600s cap -- wall time or TIMEOUT. Combo names: `A0/A1/A2` are
// `preprocessing=off` crossed with `solver_placement`
// (`eager`/`per_closed_block`/`per_formula`); `B <placement>,<method>` is
// `preprocessing=on` crossed with `preprocess_placement`/`preprocess_method`,
// `solver_placement` held at `eager`.
//
// | combo                         | W1  | W2         | W3      |
// |--------------------------------|-----|------------|---------|
// | DEFAULTS (on,leaf,apx,eager)   | 24s | 14s        | TIMEOUT |
// | A0 off,eager                   | 25s | 13s        | 0s PASS |
// | A1 off,per_closed_block        | 24s | 12s        | 0s PASS |
// | A2 off,per_formula              | 25s | 12s        | 0s PASS |
// | B leaf,defer                   | 25s | 14s        | TIMEOUT |
// | B block,apx                    | 24s | 12s        | TIMEOUT |
// | B block,defer                  | 25s | 14s        | TIMEOUT |
// | B formula,apx                  | 24s | W2 TIMEOUT | TIMEOUT |
// | B formula,defer                | 25s | W2 TIMEOUT | TIMEOUT |
//
// A1/A2 rejected on correctness, not speed: non-eager `solver_placement`
// fails 2 wff_normalization cases (Task 8 smoke) -- so A0 is the only Row A
// cell left standing, and it already wins W2/W3 outright. A0 was then
// verified suite-clean with the placement env-forced: Release 329/329,
// 26.4s. A supplementary run at A0 against oldrules16.txt N=3 (W4, the open
// mixed `*`/`/`/`%` problem) now returns fast but the case still fails --
// previously a hang under preprocessing-on; still open, tracked separately, not
// a regression from this flip.
//
// Skip-probe results under the shipped defaults, Release
// (tests/integration/test_integration-bv_stress_check.cpp): "template 5:
// network style locals", "2 iterations at bv[2]" and "2 iterations at
// bv[4]" all now pass fast and were unskipped; "14 iterations at bv[64],
// all templates" (the load tester's own default workload) passes for the
// first time ever, in 99.1s, but stays opt-in -- over the 60s
// default-suite bar.
//
// Decision: `preprocessing` defaults to `false`. cvc5 already bit-blasts
// internally, so predicate blasting on top of it hands cvc5 thousands of
// auxiliary quantifiers to re-decide instead of letting it work on the
// arithmetic natively -- the 2026-08-08 finding (whole suite faster,
// bv[16]x50 iterations 122s vs never), replicated here 2026-08-15 across
// the full matrix. `solver_placement` stays `eager` (A0's own row: beats
// A1/A2 on correctness, and W2/W3 outright). `preprocess_placement` stays
// `per_leaf` and `preprocess_method` stays `anti_prenex_result` -- not because
// either measures better than `preprocessing=false`, but as the
// when-enabled fallback: best-measured among the preprocessing-on (Row B)
// cells for a caller that re-enables preprocessing explicitly. `per_formula`
// breaks the stress suite outright (W2 itself times out), and every Row B
// cell still times out on W3 -- `per_leaf`/`anti_prenex_result` is simply
// the row that regresses least elsewhere while preprocessing is on.
//
// NOT thread-safe: Controls whether a BA's preprocessing pass (bv's predicate
// blasting today) is enabled. The tau library assumes single-threaded access.
// Do not call set_preprocessing() concurrently from multiple threads.
inline bool preprocessing = false;

/**
 * @brief Where a BA's preprocessing pass is allowed to run.
 */
enum class preprocess_site {
	/// Today's behaviour: `blast_block` on every leaf clause the block
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
// preprocessing is re-enabled. `per_leaf` is the when-enabled fallback: the
// Task 9 matrix's Row B (see the table above `preprocessing`) found no
// `preprocess_placement`/`preprocess_method` combo that beat
// `preprocessing=false` outright, but among the preprocessing-on cells
// `per_formula` broke the stress suite (W2 itself timed out) and every cell
// still timed out on W3 -- `per_leaf` is simply the row that regressed least
// elsewhere.
//
// NOT thread-safe, exactly like `preprocessing` above: the tau library
// assumes single-threaded access. Do not call set_preprocess_placement()
// concurrently.
inline preprocess_site preprocess_placement = preprocess_site::per_leaf;

/**
 * @brief What to do with a formula the preprocessing pass rewrote.
 */
enum class preprocess_mode {
	/// Today's behaviour: re-enter `anti_prenex` (with a bv-only floor) on
	/// the rewritten formula, so the auxiliary quantifiers preprocessing
	/// introduced are pushed and resolved right away.
	anti_prenex_result = 0,
	/// Rewrite only; the introduced quantifiers are left for whatever
	/// resolve pass comes next.
	defer = 1,
};

// Inert at the shipped `preprocessing=false` default, same as
// `preprocess_placement` above. `anti_prenex_result` is the same
// when-enabled fallback pick, for the same Task 9 matrix reasoning.
//
// NOT thread-safe, exactly like `preprocessing` above: the tau library
// assumes single-threaded access. Do not call set_preprocess_method()
// concurrently.
inline preprocess_mode preprocess_method = preprocess_mode::anti_prenex_result;

} // namespace idni::tau_lang

#endif // __IDNI__TAU__PREPROCESS_PLACEMENT_H__
