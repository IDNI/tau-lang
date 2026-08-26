/**
 * @file cvc5_options.h
 * @brief Runtime parameters selecting HOW the cvc5 solver is configured and
 * WHERE it may be queried.
 *
 * Both knobs are plain runtime globals, per the project's runtime-parameter
 * policy (solver strategy and placement belong in a runtime parameter, never
 * in a header constant). Like `heuristics/blast_placement.h`, this header is
 * deliberately dependency-free -- no cvc5, no tau tree -- so that
 * `tests/test_init.h` (which defines every test binary's `main()` and is
 * included before any tau header) can read and write them and apply the
 * `TAU_CVC5_OPTIONS` / `TAU_SOLVER_PLACEMENT` environment overrides before
 * the first test runs. The option strings themselves are applied by
 * `config_cvc5_solver` (boolean_algebras/bv_ba.h), the single place every
 * solver instance is configured. The api setters are `api::set_cvc5_options`
 * and `api::set_solver_placement`.
 *
 * `solver_site`/`solver_placement` moved here from
 * `heuristics/blast_placement.h` (which still includes this file, so every
 * existing user keeps seeing them unchanged): they select where cvc5 runs,
 * which belongs with how it is configured, not with the blasting knobs.
 */

// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BACKENDS__CVC5_OPTIONS_H__
#define __IDNI__TAU__BACKENDS__CVC5_OPTIONS_H__

namespace idni::tau_lang {

/**
 * @brief The considered cvc5 option combinations for the formulas tau
 * actually sends: quantified (usually alternating) bitvector scopes decided
 * during normalization, and quantifier-free systems solved for models at
 * each interpreter step. Logic "BV" always; `produce-proofs` always off;
 * `cegqi-innermost` turned off per-query when the quantification alternates
 * (see `config_cvc5_solver_alternating_quantifiers`, measured separately).
 *
 * Every enumerator below was measured -- see the matrix above
 * `cvc5_options` for the numbers and the selection rationale.
 */
enum class cvc5_option_set {
	/// The shipped pre-existing configuration: `produce-models=true`,
	/// `produce-proofs=false`, logic BV, plus the alternation gate.
	baseline = 0,
	/// `baseline`, but a decision-only query (checkSat with no model
	/// extraction afterwards, i.e. `bv_formula_sat_status`) drops
	/// `produce-models` and `incremental`. (`unconstrained-simp` was the
	/// intended companion, but cvc5 rejects it in any logic admitting
	/// quantifiers, and the decision path is exactly the quantified one.)
	decision_no_models = 1,
	/// + `miniscope-quant=agg`: aggressive miniscoping (cvc5's own
	/// anti-prenexing) on top of the default `conj-and-fv`.
	miniscope_agg = 2,
	/// + `ext-rewrite-quant=true`: extended rewriting on quantified bodies.
	ext_rewrite_quant = 3,
	/// + `pre-skolem-quant=agg`: eager aggressive skolemization.
	pre_skolem_agg = 4,
	/// + `sygus-inst=true`: SyGuS instantiation module for quantified BV.
	sygus_inst = 5,
	/// + `mbqi=true`: model-based quantifier instantiation.
	mbqi = 6,
	/// + `enum-inst=true`: enumerative instantiation as a fallback, meant
	/// to turn `unknown` answers into definite ones.
	enum_inst = 7,
	/// + `cegqi-bv-ineq=keep`: solve BV inequalities via invertibility
	/// side conditions instead of the default boundary-point rewrite.
	cegqi_bv_ineq_keep = 8,
	/// + `incremental=false` everywhere: cvc5's C++ API defaults to
	/// incremental solving (its command line does not), which disables
	/// several preprocessing passes; every solver instance here performs
	/// exactly one checkSat with no push/pop, so nothing needs it.
	non_incremental = 9,
	/// The combination of the single-option candidates that measured as
	/// (possible) improvements: `ext-rewrite-quant=true` +
	/// `incremental=false` everywhere, plus the `decision_no_models`
	/// behaviour on decision-only queries.
	ext_rewrite_no_models = 10,
	/// `ext_rewrite_no_models` + `cegqi-bv-ineq=keep`: everything that
	/// measured as a (possible) improvement in one set.
	combined_best = 11,
};

// Measurement matrix (Release, cvc5 1.3.1, 2026-08-16, `TAU_CVC5_OPTIONS`
// env-forced). H1 = test_integration-bv_stress_check `-tc="14 iterations*"`
// (the bv[64] load-tester default, the hard config this experiment targets),
// median of 3 interleaved same-window runs unless marked (1), 400s cap.
// H2 = the same binary's default suite. W1 = test_integration-satisfiability2
// (regression canary). H2/W1 medians of 3, only taken for the finalists.
//
// MEM = H1's peak RSS (`/usr/bin/time -v`, 2 agreeing runs), finalists only.
//
// | set                       | H1       | H2    | W1    | MEM     |
// |---------------------------|----------|-------|-------|---------|
// | 0 baseline                | 104.6s   | 14.4s | 26.8s | 1.07 GB |
// | 1 decision_no_models      |  96.6s   |       |       |         |
// | 2 miniscope_agg           | TIMEOUT  |       |       |         |
// | 3 ext_rewrite_quant       |  73.6s   | 14.3s | 26.5s | 517 MB  |
// | 4 pre_skolem_agg          |  93.9s(1)|       |       |         |
// | 5 sygus_inst              | TIMEOUT  |       |       |         |
// | 6 mbqi                    | 115.0s(1)|       |       |         |
// | 7 enum_inst               | 110.8s(1)|       |       |         |
// | 8 cegqi_bv_ineq_keep      |  83.3s   |       |       |         |
// | 9 non_incremental         |  97.1s   |       |       |         |
// | 10 ext_rewrite_no_models  |  73.9s   | 15.1s | 26.5s | 515 MB  |
// | 11 combined_best          |  76.6s   |       |       |         |
//
// A CLI pre-screen on the three archived monolithic alternating-quantifier
// queries (tau-meta performance/andrei-bv-stress-artifacts/q65-q67.smt2)
// additionally rejected `cegqi-nested-qe` (timeout), `cegqi-full`, `no-cbqi`,
// `inst-when=full`, `bv-solver=bitblast-internal`, `simplification=none`,
// `no-static-learning`, `user-pat=ignore` and `enum-inst-interleave` (all
// within noise of, or worse than, the shipped alternation gate on q65/q66,
// with no winner on any query). The historical-rules probes were
// option-insensitive: rules16.txt N=2 passes under 0 and 10 alike, and the
// open N=3 mixed `*`/`/`/`%` case times out under 0/3/10/11 alike.
//
// Decision: `ext_rewrite_no_models`. `ext-rewrite-quant` is the one option
// that actually moves the hard config (-30% H1 wall AND -52% peak RSS,
// neutral H2/W1 -- its CLI regression on the monolithic q65 does not
// materialize on the query mix tau actually produces), and dropping model
// production plus incrementality on decision-only queries is principled
// (models are only ever read while *solving*, `solve_bv`; the
// validity/sat/unsat checks never look at one, and every solver instance
// performs exactly one checkSat) and measured mildly positive on its own
// (sets 1/9, ~7-8% wall each). The two do not stack -- 10 ties 3 on H1
// wall and memory -- so the no-models half is kept on principle and for
// the solver-side bookkeeping it avoids, not for additional wall time.
// `cegqi_bv_ineq_keep` (-20% alone) does not stack either (11 vs 10) and is
// EXPERT-flagged, so it stays a considered-not-selected row.
//
// @warning Do NOT add a resource limit (`rlimit`/`rlimit-per`) to any set: a
// truncated instantiation search can report a plain `sat` for an unsat
// formula (reproduced on the 4-level bv[16] alternation chain), and callers
// treat a definite answer as truth. See
// `config_cvc5_solver_alternating_quantifiers`.
//
// Set once at startup (env override or api setter) before the first solver
// query: `bv_formula_sat_status` memoizes its verdicts keyed on the formula
// alone, so flipping this mid-process would serve answers computed under the
// previous option set.
//
// NOT thread-safe, exactly like the blasting knobs: the tau library assumes
// single-threaded access. Do not call set_cvc5_options() concurrently.
inline cvc5_option_set cvc5_options = cvc5_option_set::ext_rewrite_no_models;

/**
 * @brief Where the cvc5 solver is allowed to be queried.
 *
 * The final closed-formula check at the end of `eliminate_arithmetic_and_quantifiers`
 * is deliberately NOT gated: it is the single "final" solver site that both
 * `per_closed_block` and `per_formula` rely on.
 */
enum class solver_site {
	/// Today's behaviour: the resolve passes, `blast_block`'s solver-first
	/// attempt, `leaf_clause`'s bv branch, and the final check.
	eager = 0,
	/// Only on a fully-processed, closed quantifier block (plus the final
	/// check).
	per_closed_block = 1,
	/// Only the final closed-formula check.
	per_formula = 2,
};

// `eager` is the measured winner, not just the pre-existing default: the
// Task 9 matrix's Row A (`bv_blasting=false` crossed with
// `solver_placement`, see the table above `bv_blasting` in
// heuristics/blast_placement.h) found `per_closed_block`/`per_formula` fail
// 2 wff_normalization cases outright (Task 8 smoke), leaving `eager` the
// only cell standing -- and it already wins W2/W3 among the three.
//
// NOT thread-safe, exactly like `bv_blasting`: the tau library assumes
// single-threaded access. Do not call set_solver_placement() concurrently.
inline solver_site solver_placement = solver_site::eager;

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BACKENDS__CVC5_OPTIONS_H__
