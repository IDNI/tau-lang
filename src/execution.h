// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// TODO (MEDIUM) clean execution api code

/**
 * @file execution.h
 * @brief Step-based formula-rewriting execution engine.
 *
 * Provides the `step`, `steps`, `repeat_each`, `repeat_all`, and `repeat_once`
 * functors for applying rewriting libraries to formulas, together with pipe
 * `operator|` overloads that integrate a library and `repeat_once` into the
 * `tree<node>::traverser` pipeline. `repeat_each` and `repeat_all` can fail
 * on the round cap, so they return a `result` and are called directly.
 */

#ifndef __IDNI__TAU__EXECUTION_H__
#define __IDNI__TAU__EXECUTION_H__

#include "tau_tree.h"
#include "env_limits.h"

namespace idni::tau_lang {

/**
 * @brief Single-library rewriting step.
 *
 * Applies the rules of `lib` in order, each to the result of the previous
 * one (`nso_rr_apply`).
 * @tparam node Tree node type.
 */
// TODO (MEDIUM) clean execution api code
template <NodeType node>
struct step {
	/** @brief Construct a step that applies the rules of @p lib. */
	step(rewriter::library lib);

	/**
	 * @brief Apply the library to @p n and return the result.
	 * @param n Tree node to rewrite.
	 * @return Rewritten node, or @p n if no rule applies.
	 */
	tref operator()(tref n) const;

	rewriter::library lib; ///< Library of rewriting rules.
};

/**
 * @brief Ordered sequence of `step_t` libraries.
 * @tparam node Tree node type.
 * @tparam step_t Individual step type (typically `step<node>`).
 */
template <NodeType node, typename step_t>
struct steps {
	/** @brief Construct from a vector of step objects. */
	steps(std::vector<step_t> libraries);
	/** @brief Construct from a single step object. */
	steps(step_t library);

	/**
	 * @brief Apply all steps in sequence to @p n, each once.
	 * @param n Tree node to rewrite.
	 * @return Rewritten node after all steps; @p n if there are none.
	 */
	tref operator()(tref n) const;

	std::vector<step_t> libraries; ///< Ordered steps to apply.
};

/**
 * @brief Apply each step of a sequence in turn, repeating that step until its
 *        result repeats an earlier one (a fixpoint or a cycle).
 *
 * Each step is bounded by `max_rewrite_rounds`, as `repeat_all` is.
 * @tparam node Tree node type.
 * @tparam step_t Individual step type.
 */
template <NodeType node, typename step_t>
struct repeat_each {
	/** @brief Construct with a `steps` sequence @p s. */
	repeat_each(steps<node, step_t> s);
	/** @brief Construct with a single step @p s. */
	repeat_each(step_t s);

	/**
	 * @brief Run each step to its fixpoint or cycle, in order.
	 * @param n Formula to rewrite.
	 * @return The formula after the last step settled, or a
	 *         `code::runtime_error` if `max_rewrite_rounds` is set and a
	 *         step's result did not repeat within that many rounds.
	 */
	result<tref> operator()(tref n) const;

	steps<node, step_t> s; ///< Steps to repeat.
};

/// Round cap for `repeat_all` and for each step of `repeat_each` — a rewrite
/// that neither settles nor cycles; 0 = unlimited (the default). A rewriting system given by user definitions
/// need not terminate, and a non-terminating one typically *grows* the
/// formula rather than revisiting an earlier state, so `repeat_all`'s
/// `visited` cycle check never fires on it — only a bound set here stops it.
/// Legitimate definition expansion needs few rounds: a round rewrites every
/// match at once and applies the whole rule set, so a call chain of depth k
/// collapses in O(k) rounds at worst. A namespace-level global (not a
/// `repeat_all` member) because `repeat_all` is templated on the step type —
/// a member would be one knob per instantiation. Set via
/// `--max-rewrite-rounds`, REPL `rewriterounds`, or
/// `api::set_max_rewrite_rounds`.
/// Environment fallback `TAU_MAX_REWRITE_ROUNDS`.
inline env_limit<size_t> max_rewrite_rounds{ "TAU_MAX_REWRITE_ROUNDS", 0 };

/**
 * @brief Repeatedly apply the whole sequence of steps to a formula, one
 *        round at a time, until a round's result repeats an earlier one
 *        (a fixpoint or a cycle).
 * @tparam node Tree node type.
 * @tparam step_t Individual step type.
 */
template <NodeType node, typename step_t>
struct repeat_all {
	/** @brief Construct with a `steps` sequence @p s. */
	repeat_all(steps<node, step_t> s);
	/** @brief Construct with a single step @p s. */
	repeat_all(step_t s);

	/**
	 * @brief Apply the sequence round by round until a result repeats.
	 * @param n Formula to rewrite.
	 * @return The first repeated formula (the fixpoint, or a member of the
	 *         cycle), or a `code::runtime_error` if `max_rewrite_rounds` is
	 *         set and no result repeated within that many rounds.
	 */
	result<tref> operator()(tref n) const;

	steps<node, step_t> s; ///< Steps to repeat.
};

/**
 * @brief Apply a set of steps to a formula exactly once (no repetition).
 * @tparam node Tree node type.
 * @tparam step_t Individual step type.
 */
template <NodeType node, typename step_t>
struct repeat_once {
	/** @brief Construct with a `steps` sequence @p s. */
	repeat_once(steps<node, step_t> s);
	/** @brief Construct with a single step @p s. */
	repeat_once(step_t s);

	/**
	 * @brief Apply steps once.
	 * @param n Formula to rewrite.
	 * @return Rewritten formula.
	 */
	tref operator()(tref n) const;

	steps<node, step_t> s; ///< Steps to apply.
};

/**
 * @brief Return a copy of @p s with @p l appended.
 * @tparam step_t Type of @p l; must convert to `step<node>`.
 */
template <NodeType node, typename step_t>
steps<node, step<node>> operator|(const steps<node, step<node>>& s,
	const step_t& l);

/** @brief Return a copy of @p s with a step of library @p l appended. */
template <NodeType node>
steps<node, step<node>> operator|(const steps<node, step<node>>& s,
	const rewriter::library& l);

/** @brief Apply `step<node>(l)` to the tree that @p n holds. */
template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& n, const rewriter::library& l);

/** @brief Apply a `repeat_once` to a `tree<node>::traverser`. */
template <NodeType node, typename step_t>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& n,
	const repeat_once<node, step_t>& r);



} // namespace idni::tau_lang

#include "execution.tmpl.h"

#endif // __IDNI__TAU__EXECUTION_H__
