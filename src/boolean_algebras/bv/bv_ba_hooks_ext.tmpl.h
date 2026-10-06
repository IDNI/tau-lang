// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file bv_ba_hooks_ext.tmpl.h
 * @brief bv's comparison hooks, reached by core through ba_wff_hooks.
 *
 * Each member folds a comparison of two bv constants and returns nullptr when
 * either side is not one, which core reads as "bv owns this operator but has
 * nothing to say about these operands".
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_BA_HOOKS_EXT_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_BA_HOOKS_EXT_TMPL_H__

#include "boolean_algebras/ba_descriptor.h"

namespace idni::tau_lang {

/**
 * @brief bv's comparison folds.
 *
 * Every member takes the children @p ch of the `wff` node being interned
 * (`ch[0]` is the comparison, its two `bf` children the operands) and the
 * right sibling @p r the folded node must keep. It returns `T`/`F` (carrying
 * @p r) when both operands are bv constants of the same type, else nullptr.
 * Comparisons are unsigned.
 */
template <typename... PackBAs>
struct ba_wff_hooks<bv, node<PackBAs...>> {
	using node_t = node<PackBAs...>;

	/// @brief Fold `<`; see the struct comment for @p ch, @p r and the result.
	static tref wff_lt(const tref* ch, tref r) {
		return wff_bv_lt<node_t>(ch, r);
	}
	/// @brief Fold `!<`; see the struct comment for @p ch, @p r and the result.
	static tref wff_nlt(const tref* ch, tref r) {
		return wff_bv_nlt<node_t>(ch, r);
	}
	/// @brief Fold `<=`; see the struct comment for @p ch, @p r and the result.
	static tref wff_lteq(const tref* ch, tref r) {
		return wff_bv_lteq<node_t>(ch, r);
	}
	/// @brief Fold `!<=`; see the struct comment for @p ch, @p r and the result.
	static tref wff_nlteq(const tref* ch, tref r) {
		return wff_bv_nlteq<node_t>(ch, r);
	}
	/// @brief Fold `>`; see the struct comment for @p ch, @p r and the result.
	static tref wff_gt(const tref* ch, tref r) {
		return wff_bv_gt<node_t>(ch, r);
	}
	/// @brief Fold `!>`; see the struct comment for @p ch, @p r and the result.
	static tref wff_ngt(const tref* ch, tref r) {
		return wff_bv_ngt<node_t>(ch, r);
	}
	/// @brief Fold `>=`; see the struct comment for @p ch, @p r and the result.
	static tref wff_gteq(const tref* ch, tref r) {
		return wff_bv_gteq<node_t>(ch, r);
	}
	/// @brief Fold `!>=`; see the struct comment for @p ch, @p r and the result.
	static tref wff_ngteq(const tref* ch, tref r) {
		return wff_bv_ngteq<node_t>(ch, r);
	}
};

/// @brief bv's term rewrite: the width cast of a constant.
template <typename... PackBAs>
struct ba_term_hooks<bv, node<PackBAs...>> {
	using node_t = node<PackBAs...>;

	/**
	 * @brief Retype the constant in cast node @p symbol to @p ba_type.
	 *
	 * Zero-extends or truncates a bv constant, `0`, `1` or a complemented
	 * one, and returns @p symbol unchanged for any other operand or when
	 * either type is not a bv type. The
	 * call is qualified because this member hides the namespace-scope
	 * template of the same name.
	 * @param symbol A `bf` node whose child is the `bf_cast`.
	 * @param ba_type Type id to cast to.
	 * @return The retyped constant; @p symbol unchanged when nothing is
	 *         cast; nullptr when a bv width cannot be read.
	 */
	static tref term_cast(tref symbol, size_t ba_type) {
		return idni::tau_lang::term_cast<node_t>(symbol, ba_type);
	}
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_BA_HOOKS_EXT_TMPL_H__
