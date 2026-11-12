// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Boolean expressions over sets of programs, compiled into one specification
// whose realizability decides whether the set is empty. A specification S
// denotes the set of causal programs that satisfy it on every input; `&` is
// the intersection of two such sets, `'` the complement within all causal
// programs, `|` the union. The complement and the union are not the
// negation and the disjunction of the specifications: a program is outside
// S when one input path breaks S, and a program is in S1 | S2 when it is
// in one of them for every input. Both are compiled with hidden outputs the
// system chooses before each step: one prediction stream per input for a
// complement, which pins the input path the program breaks S on, and one
// constant selector for a union.

#ifndef __IDNI__TAU__PROGRAM_SET_H__
#define __IDNI__TAU__PROGRAM_SET_H__

#include <cctype>

#include "ltl_aba.h"

namespace idni::tau_lang {

/**
 * @brief A Boolean expression over sets of programs.
 *
 * A literal is the main formula of a specification; the other kinds combine
 * their operands. The expression is a tree; `nnf_program_set` pushes the
 * complements to the literals before compilation.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct program_set_expr {
	/// The kind of a node of the expression.
	enum class kind { literal, complement, intersection, set_union };

	kind op = kind::literal;              ///< The operation of this node.
	tref spec = nullptr;                  ///< The specification of a literal.
	std::vector<program_set_expr> args;   ///< The operands of an operation.

	/// The set of programs of specification @p spec (its main formula).
	static program_set_expr literal(tref spec);
	/// The complement of @p e within all causal programs.
	static program_set_expr complement(program_set_expr e);
	/// The programs in both @p a and @p b.
	static program_set_expr intersection(program_set_expr a,
		program_set_expr b);
	/// The programs in @p a or in @p b.
	static program_set_expr set_union(program_set_expr a, program_set_expr b);
};

/**
 * @brief A complemented literal of a compiled expression: the specification
 * the program breaks on the predicted input path, and the hidden streams
 * that predict that path.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct program_set_negative {
	tref spec = nullptr;                   ///< The literal, prepared.
	int_t lookback = 0;                    ///< Its warm-up (body_max_lookback).
	/// The prediction stream of each input, in the order of
	/// program_set_compiled::inputs.
	std::vector<std::string> predictions;
};

/**
 * @brief A parameter of a program-set expression: an uninterpreted
 * constant `<:c>` of one of its literals. One name is one parameter in
 * every literal it appears in.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct program_set_parameter {
	std::string name;         ///< The name inside `<>`, as spelled (`:c`).
	size_t type = 0;          ///< Its type id.
	/// The constant as the literals hold it, a `bf`: the key of its value
	/// in program_set_with_values.
	tref constant = nullptr;
};

/**
 * @brief The result of compile_program_set.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct program_set_compiled {
	/// The specification whose realizability decides the expression's
	/// nonemptiness: a wff over the visible streams and the hidden ones.
	tref formula = nullptr;
	/// The input streams the literals read, name and type id, in the
	/// order they are first met.
	std::vector<std::pair<std::string, size_t>> inputs;
	/// The hidden output streams, name and type id: the predictions of
	/// every complemented literal and the selector of every union.
	std::vector<std::pair<std::string, size_t>> hidden;
	/// The complemented literals, in the order they are compiled.
	std::vector<program_set_negative<node>> negatives;
	/// The selector stream of each union, in the order they are compiled.
	std::vector<std::string> selectors;
	/// The parameters of the expression, in the order they are first met.
	std::vector<program_set_parameter<node>> parameters;
	/// Under the universal reading, the hidden input stream of each
	/// parameter, in the order of `parameters`; empty under the
	/// existential one.
	std::vector<std::string> parameter_inputs;
	/// True when no literal reads an input: the expression is then the
	/// plain Boolean combination of its literals, with no hidden stream.
	bool input_free = false;
};

/**
 * @brief The program-set expression a term of type tau spells: `{S}:tau`
 * is a literal, `'` the complement, `&` the intersection, `|` the union,
 * `^` the symmetric difference.
 *
 * The term is read as written, so it has to be built with the construction
 * hooks off (use_hooks_guard): the hooks fold the Boolean operations of two
 * tau constants into one constant, where the structure is lost.
 * @tparam node Tree node type.
 * @param term A `bf` of type tau.
 * @return The expression, or an error naming the first subterm that is no
 * Boolean combination of tau constants, or a constant that carries
 * recurrence relations.
 */
template <NodeType node>
result<program_set_expr<node>> program_set_of_term(tref term);

/**
 * @brief The expression with every complement pushed to a literal.
 * @tparam node Tree node type.
 * @param e Expression to normalize.
 * @return An expression whose complements have literal operands only.
 */
template <NodeType node>
program_set_expr<node> nnf_program_set(const program_set_expr<node>& e);

/**
 * @brief The parameters of the literals of @p e (program_set_parameter),
 * in the order they are first met; one entry per distinct name.
 * @tparam node Tree node type.
 * @param e Expression to inspect.
 * @return Its parameters.
 */
template <NodeType node>
std::vector<program_set_parameter<node>> program_set_parameters(
	const program_set_expr<node>& e);

/**
 * @brief The expression with the parameters given in @p values replaced by
 * their values in every literal: the fixed reading of those parameters. A
 * key is the constant as a literal holds it (program_set_parameter::constant,
 * or the `bf` that `build_bf_uconst` builds for that name and type); a
 * value is a `bf`. A parameter without a value keeps the reading of
 * `ltl-parameters`.
 * @tparam node Tree node type.
 * @param e Expression whose parameters are fixed.
 * @param values The value of each fixed parameter.
 * @return The expression over the remaining parameters.
 */
template <NodeType node>
program_set_expr<node> program_set_with_values(const program_set_expr<node>& e,
	const subtree_map<node, tref>& values);

/**
 * @brief Compile a program-set expression into one specification.
 *
 * Every literal is prepared as a specification is for a decision: every
 * conjunct with no future operator scoped by its `always`, forward
 * references shifted. A literal holding a reference to a definition, a tau constant
 * or a hidden stream of another compilation is refused, and so is a
 * complemented literal holding a CTL* operator (a tree property has no
 * per-path negation); a positive one is conjoined like any clause.
 *
 * With no input stream the expression is the plain Boolean combination of
 * its literals, since every program has one trace. Otherwise, after the
 * NNF, a positive literal is its specification; a complemented literal
 * `{ψ}:tau'` of warm-up `l` gets one hidden output `w_ps<n>_d_<x>` per
 * input `x` of the whole expression and the clause
 * `(always (x[t] = z[t-1] && … && x[t-l] = z[t-l-1])) -> !ψ`, which asks
 * ψ to fail on the input path the predictions name, from the value chosen
 * before step 0 on; an intersection is a conjunction; a union gets one
 * hidden output `w_ps<n>_b`, constant from before step 0
 * (`always b[t] = b[t-1]`), that selects which operand's clauses hold
 * (`(always b[t-1] = 0) -> c` for the first, `(always b[t-1] != 0) -> c`
 * for the second).
 * A lag on a hidden stream gives no clause a warm-up (body_max_lookback).
 *
 * An uninterpreted constant `<:c>` of a literal is a parameter of the
 * expression. Under the existential reading the literals keep it: the
 * decision chooses its value as it does for any specification. Under the
 * universal reading each parameter becomes one hidden input `w_ps<n>_p_<c>`
 * of its type, read where the constant stood in every literal, and the
 * compiled specification is `(always ([t > 0] -> w[t] = w[t-1])) -> formula`:
 * the environment picks the value at step 0 and keeps it, so the expression
 * has to be nonempty for every value, and the program may read it. The
 * parameter is no input of the literals: a complement predicts the visible
 * inputs only, and an expression whose literals read none is still
 * input-free.
 *
 * The decision merges the always parts of the literals into one clause
 * with one start, the deepest lookback among them, so a positive literal
 * whose always part starts earlier is written with each statement read
 * also at the steps back to that start (delay_always_part), and the
 * constancy of a selector the same way: each literal keeps its own
 * warm-up, while one literal holding several statements has one.
 * @tparam node Tree node type.
 * @param e Expression to compile.
 * @param universal_parameters Read the parameters universally; by default
 * what `ltl-parameters` says (ltl_universal_parameters).
 * @return The compiled specification and its hidden streams, or the error
 * of a refused literal.
 */
template <NodeType node>
result<program_set_compiled<node>> compile_program_set(
	const program_set_expr<node>& e,
	bool universal_parameters = ltl_universal_parameters());

} // namespace idni::tau_lang

#include "program_set.tmpl.h"

#endif // __IDNI__TAU__PROGRAM_SET_H__
