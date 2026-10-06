// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file hooks.h
 * @brief Tree-construction hook dispatcher (`get_hook`).
 *
 * `get_hook<node>` is called by `tree<node>` during node construction to
 * apply simplification and normalization reductions at build time. It
 * dispatches on the node type and delegates to specialized private static
 * methods (one per grammar production).
 */

#ifndef __IDNI__TAU__HOOKS_H__
#define __IDNI__TAU__HOOKS_H__

#include "tau_tree.h"

#ifdef HOOK_LOGGING_ENABLED
#define HOOK_LOGGING(x) x
#else
#define HOOK_LOGGING(x)
#endif // HOOK_LOGGING_ENABLED

namespace idni::tau_lang {

/**
 * @brief Hook called by `tree<node>` when a new node is constructed.
 *
 * Inspects the node type and its children and may return a simplified or
 * canonical replacement instead of the raw node. Returns the default result
 * unchanged when no reduction applies.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct get_hook {
	friend tree<node>;
private:
	using tau = tree<node>;
	using tt = tau::traverser;

	/**
	 * @brief Apply the hook to a node being constructed.
	 * @param v Node descriptor.
	 * @param ch Array of child tree-refs (length @p len).
	 * @param len Number of children.
	 * @param r The default result node, passed to `tree::get_raw` when no
	 * reduction applies.
	 * @return Reduced or canonical node, the raw node when no reduction
	 * applies, or nullptr when the node is ill-formed (a negative shift).
	 */
	tref operator()(const node& v, const tref* ch, size_t len, tref r);

#ifdef HOOK_LOGGING_ENABLED
	/** @brief Log hook invocation details when HOOK_LOGGING_ENABLED is defined. */
	static void log(const char* msg, const node& v, const tref* ch,
			size_t len, tref r, bool track_each_call = false);
#endif // HOOK_LOGGING

	// helpers
	/// @brief Return the formula child at argument position 1.
	static inline const tree<node>& arg1_fm(const tref* ch);
	/// @brief Return the formula child at argument position 2.
	static inline const tree<node>& arg2_fm(const tref* ch);
	/// @brief Return the formula child at argument position 3.
	static inline const tree<node>& arg3_fm(const tref* ch);
	/// @brief Return the raw child at argument position 1.
	static inline const tree<node>& arg1(const tref* ch);
	/// @brief Return the raw child at argument position 2.
	static inline const tree<node>& arg2(const tref* ch);
	/// @brief Return the raw child at argument position 3.
	static inline const tree<node>& arg3(const tref* ch);
	/// @brief Return the logic-operator child from @p ch.
	static inline const tree<node>& logic_operator(const tref* ch);
	/// @brief Return the quantifier child from @p ch.
	static inline const tree<node>& quantifier(const tref* ch);
	/// @brief Return the quantified formula child from @p ch.
	static inline const tree<node>& quantified_formula(const tref* ch);

	// hooks
	/// @brief Build the `bf` constant `0` typed @p ba_type, reusing @p r as the
	/// default result.
	static tref _0_typed(size_t ba_type, tref r);
	/// @brief Build the `bf` constant `1` typed @p ba_type, reusing @p r as the
	/// default result.
	static tref _1_typed(size_t ba_type, tref r);
	/// @brief Return `0` typed by the operands of the binary term in @p ch (the
	/// one concrete type if only one side has it); the raw node if they clash.
	static tref _0(const node& v, const tref* ch, size_t len, tref r);
	/// @brief Return `1` typed by the operands of the binary term in @p ch (the
	/// one concrete type if only one side has it); the raw node if they clash.
	static tref _1(const node& v, const tref* ch, size_t len, tref r);
	/// @brief Return `F`, unless the two operands in @p ch carry distinct
	/// concrete BA types, in which case the raw node.
	static tref _F(const node& v, const tref* ch, size_t len, tref r);
	/// @brief Return `T`, unless the two operands in @p ch carry distinct
	/// concrete BA types, in which case the raw node.
	static tref _T(const node& v, const tref* ch, size_t len, tref r);
	/// @brief Negate the time constraint @p n (`[t op k]`) by flipping its
	/// comparison; any other shape is wrapped in a `wff_neg`.
	static tref ctn_neg(const tree<node>& n);
	/// @brief Hook for `bf` term nodes: dispatch on the operator child, else
	/// let the pack simplify a typed symbol.
	static tref term           (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `bf_or` (term disjunction) nodes.
	static tref term_or        (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `bf_and` (term conjunction) nodes.
	static tref term_and       (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `bf_neg` (term negation) nodes.
	static tref term_neg       (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `bf_xor` (term exclusive-or) nodes.
	static tref term_xor       (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `bf_cast` (type-cast) nodes: the BA owning the target
	/// type folds the cast when it can.
	static tref term_cast      (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `ba_constant` terms: fold a typed syntactic zero or one
	/// to the typed `0` / `1`.
	static tref cte            (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Fold the disjunction of two constants into one normalized
	/// constant; keep the raw node when the owner reports it inexact.
	static tref cte_or         (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Fold the conjunction of two constants into one normalized
	/// constant; keep the raw node when the owner reports it inexact.
	static tref cte_and        (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Fold the complement of a constant into one normalized constant;
	/// keep the raw node when the owner reports it inexact.
	static tref cte_neg        (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Fold the exclusive-or of two constants into one normalized
	/// constant; keep the raw node when the owner reports it inexact.
	static tref cte_xor        (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `wff` (well-formed formula) nodes.
	static tref wff            (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `wff_and` conjunction nodes.
	static tref wff_and        (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `wff_or` disjunction nodes.
	static tref wff_or         (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `wff_neg` negation nodes.
	static tref wff_neg        (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `wff_xor` exclusive-or nodes.
	static tref wff_xor        (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for time `constraint` nodes: rewrite `=` into `<=` and `>=`,
	/// `!=` into `<` or `>`.
	static tref wff_ctn        (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `bf_eq` equality nodes.
	static tref wff_eq         (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Decide `{c} = 0` for a typed constant `c`; an inexact constant is
	/// left to its owner's hook, or kept raw.
	static tref wff_eq_cte     (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `bf_neq` disequality nodes.
	static tref wff_neq        (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Decide `{c} != 0` for a typed constant `c`; an inexact constant
	/// is left to its owner's hook, or kept raw.
	static tref wff_neq_cte    (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `wff_sometimes` (existential-temporal) nodes.
	static tref wff_sometimes  (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `wff_always` (universal-temporal) nodes.
	static tref wff_always     (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for the binary temporal nodes `U`, `R`, `W`, `S`, `T`.
	static tref wff_binary_temporal(const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `wff_A` (universal path quantifier) nodes.
	static tref wff_A          (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `wff_E` (existential path quantifier) nodes.
	static tref wff_E          (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `wff_semantic_neg` (semantic negation, `-`) nodes.
	static tref wff_semantic_neg(const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `wff_conditional` (if-then-else) nodes.
	static tref wff_conditional(const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `wff_imply` (implication) nodes.
	static tref wff_imply      (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `wff_rimply` (reverse-implication) nodes.
	static tref wff_rimply     (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `wff_equiv` (equivalence) nodes.
	static tref wff_equiv      (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `bf_lt` (less-than) nodes.
	static tref wff_lt         (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `bf_nlt` (not-less-than) nodes.
	static tref wff_nlt        (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `bf_lteq` (less-than-or-equal) nodes.
	static tref wff_lteq       (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `bf_nlteq` (not-less-than-or-equal) nodes.
	static tref wff_nlteq      (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `bf_gt` (greater-than) nodes.
	static tref wff_gt         (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `bf_ngt` (not-greater-than) nodes.
	static tref wff_ngt        (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `bf_gteq` (greater-than-or-equal) nodes.
	static tref wff_gteq       (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `bf_ngteq` (not-greater-than-or-equal) nodes.
	static tref wff_ngteq      (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `bf_interval` nodes: a type with arithmetic operations
	/// splits `a <= x <= b` into two `<=` atoms.
	static tref wff_interval   (const node& v, const tref* ch, size_t len, tref r);
	/// @brief Hook for `shift` nodes: fold a numeric `n - k` into the integer
	/// it denotes; keep a variable or capture `t - k` raw.
	/// @return nullptr when `k > n` (a negative time point), which the
	/// rewriter reads as the rule not applying.
	static tref shift          (const node& v, const tref* ch, size_t len, tref r);
};

} // namespace idni::tau_lang

#include "hooks.tmpl.h"

#undef HOOK_LOGGING

#endif // __IDNI__TAU__HOOKS_H__
