/**
 * @file blast_placement.h
 * @brief Runtime parameters selecting WHERE predicate blasting and the cvc5
 * solver may run, and what to do with a blasted result.
 *
 * These four knobs conceptually belong next to `bv_predicate_blasting`, and
 * `bv_blasting` used to live there. They are split into this dependency-free
 * header purely so that they can be read and written without the tau tree
 * behind them: `tests/test_init.h` defines every test binary's `main()` and is
 * included before any tau header, so it cannot pull in
 * `heuristics/bv_predicate_blasting.h` -- it includes this file instead and
 * applies the `TAU_*` environment overrides before the first test runs.
 * `bv_predicate_blasting.h` includes this file, so every existing user of
 * `bv_blasting` keeps seeing it unchanged.
 *
 * All four are plain runtime globals, per the project's runtime-parameter
 * policy (placement and resource limits belong in a runtime parameter, never in
 * a header constant). The api setters are `api::set_blasting`,
 * `api::set_blast_placement`, `api::set_blast_method` and
 * `api::set_solver_placement`.
 *
 * All four defaults reproduce today's behaviour exactly; every call site gated
 * on them wraps the pre-existing code, and the non-default branches take the
 * decline/fall-through path that site already had.
 */

// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BLAST_PLACEMENT_H__
#define __IDNI__TAU__BLAST_PLACEMENT_H__

namespace idni::tau_lang {

// NOT thread-safe: Controls whether bitvector blasting is enabled.
// The tau library assumes single-threaded access.
// Do not call set_blasting() concurrently from multiple threads.
inline bool bv_blasting = true;

/**
 * @brief Where predicate blasting is allowed to run.
 */
enum class blast_site {
	/// Today's behaviour: `blast_block` on every leaf clause the block
	/// machinery hands it, `leaf_clause`'s own bv branch, and
	/// `resolve_quantifiers` on every bv-typed scope it walks.
	per_leaf = 0,
	/// Once per fully-processed quantifier block, and only when that block
	/// actually displaced blasteable binders.
	per_block = 1,
	/// Once per formula, after the final anti-prenex/resolve pass of
	/// `eliminate_bv_and_quantifiers`.
	per_formula = 2,
};

// NOT thread-safe, exactly like `bv_blasting` above: the tau library assumes
// single-threaded access. Do not call set_blast_placement() concurrently.
inline blast_site blast_placement = blast_site::per_leaf;

/**
 * @brief What to do with a formula predicate blasting rewrote.
 */
enum class blast_mode {
	/// Today's behaviour: re-enter `anti_prenex` (with a bv-only floor) on
	/// the blasted formula, so the auxiliary quantifiers blasting
	/// introduced are pushed and resolved right away.
	anti_prenex_result = 0,
	/// Rewrite only; the introduced quantifiers are left for whatever
	/// resolve pass comes next.
	defer = 1,
};

// NOT thread-safe, exactly like `bv_blasting` above: the tau library assumes
// single-threaded access. Do not call set_blast_method() concurrently.
inline blast_mode blast_method = blast_mode::anti_prenex_result;

/**
 * @brief Where the cvc5 solver is allowed to be queried.
 *
 * The final closed-formula check at the end of `eliminate_bv_and_quantifiers`
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

// NOT thread-safe, exactly like `bv_blasting` above: the tau library assumes
// single-threaded access. Do not call set_solver_placement() concurrently.
inline solver_site solver_placement = solver_site::eager;

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BLAST_PLACEMENT_H__
