// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// TODO (MEDIUM) clean execution api code

/**
 * @file execution.h
 * @brief Step-based formula-rewriting execution engine.
 *
 * Provides the `step`, `steps`, `repeat_each`, `repeat_all`, and `repeat_once`
 * functors for applying rewriting libraries to formulas, together with pipe
 * `operator|` overloads that integrate them into the `tree<node>::traverser`
 * pipeline.
 */

#ifndef __IDNI__TAU__EXECUTION_H__
#define __IDNI__TAU__EXECUTION_H__

#include "tau_tree.h"

namespace idni::tau_lang {

/**
 * @brief Single-library rewriting step.
 *
 * Applies all rules in `lib` to a formula node in one pass.
 * @tparam node Tree node type.
 */
// TODO (MEDIUM) clean execution api code
template <NodeType node>
struct step {
	/** @brief Construct a step that applies @p lib. */
	step(rewriter::library lib);

	/**
	 * @brief Apply the library to @p n and return the result.
	 * @param n Tree node to rewrite.
	 * @return Rewritten node.
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
	 * @brief Apply all steps in sequence to @p n.
	 * @param n Tree node to rewrite.
	 * @return Rewritten node after all steps.
	 */
	tref operator()(tref n) const;

	std::vector<step_t> libraries; ///< Ordered steps to apply.
};

/**
 * @brief Repeatedly apply a set of steps to a formula in sequence until a
 *        fixpoint is reached in each one.
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
	 * @brief Apply steps repeatedly until fixpoint.
	 * @param n Formula to rewrite.
	 * @return Fixpoint formula.
	 */
	tref operator()(tref n) const;

	steps<node, step_t> s; ///< Steps to repeat.
};

/**
 * @brief Repeatedly apply a set of steps to a formula, restarting after every
 *        successful application, until the full sequence produces no change.
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
	 * @brief Apply all steps, restarting until no step fires.
	 * @param n Formula to rewrite.
	 * @return Fixpoint formula, or `nullptr` if neither a fixpoint nor a
	 *         cycle was reached within `max_rounds` rounds.
	 */
	tref operator()(tref n) const;

	/**
	 * @brief Round cap for a rewrite that neither settles nor cycles.
	 *
	 * A rewriting system given by user definitions need not terminate, and a
	 * non-terminating one typically *grows* the formula rather than revisiting
	 * an earlier state, so the `visited` cycle check never fires.
	 *
	 * STATUS (RR-2, issue #36): the cap is currently UNREACHABLE
	 * (size_t::max), so the non-termination report below is dead and a
	 * growing rewrite hangs. Per project policy the effective cap belongs
	 * in a runtime parameter, not a header constant; when that lands, pick
	 * a value small enough that hitting it is reported in seconds
	 * (legitimate expansion collapses a depth-k call chain in O(k) rounds),
	 * and un-skip the regression test in tests/unit/test_execution.cpp.
	 */
	// TODO (HIGH) This should be a runtime parameter, not a compile-time constant.
	static constexpr size_t max_rounds = std::numeric_limits<size_t>::max();

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

// (RR-4: to_steps deleted -- zero callers.)

// (RR-1: the repeat_each|repeat_each and repeat_all|repeat_all compose
// overloads were deleted -- their template arguments were swapped, so any
// instantiation failed to compile, and nothing ever called them.)

/** @brief Append a step to an existing `steps` sequence. */
template <NodeType node, typename step_t>
steps<node, step<node>> operator|(const steps<node, step<node>>& s,
	const step_t& l);

/** @brief Append a raw library to a `steps` sequence (step_t overload). */
template <NodeType node, typename step_t>
steps<node, step<node>> operator|(const steps<node, step<node>>& s,
	const rewriter::library& l);

/** @brief Append a raw library to a `steps` sequence. */
template <NodeType node>
steps<node, step<node>> operator|(const steps<node, step<node>>& s,
	const rewriter::library& l);

/** @brief Apply a library directly to a `tree<node>::traverser`. */
template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& n, const rewriter::library& l);

// (RR-1: the traverser|steps overload was deleted -- it took the swapped
// steps<step_t, node> form no live steps object can match.)

/** @brief Apply a `repeat_once` to a `tree<node>::traverser`. */
template <NodeType node, typename step_t>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& n,
	const repeat_once<node, step_t>& r);

/** @brief Apply a `repeat_all` to a `tree<node>::traverser`. */
template <NodeType node, typename step_t>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& n,
	const repeat_all<node, step_t>& r);

/** @brief Apply a `repeat_each` to a `tree<node>::traverser`. */
template <NodeType node, typename step_t>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& n,
	const repeat_each<node, step_t>& r);

} // namespace idni::tau_lang

#include "execution.tmpl.h"

#endif // __IDNI__TAU__EXECUTION_H__
