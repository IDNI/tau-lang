// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file my_ba_ba_hooks_ext.tmpl.h
 * @brief Optional rewrite hooks for my_ba. Delete this file if you have none.
 *
 * `ba_wff_hooks` and `ba_term_hooks` (ba_descriptor.h) are defined and empty,
 * so specializing neither, one, or both is fine — core probes each member with
 * `requires`. Include this from your descriptor header, after the declarations
 * of whatever the members call.
 *
 * See docs/adding_base_bas.md, "Rewrite hooks".
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__TEMPLATE__MY_BA_BA_HOOKS_EXT_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__TEMPLATE__MY_BA_BA_HOOKS_EXT_TMPL_H__

#include "boolean_algebras/ba_descriptor.h"

namespace idni::tau_lang {

// Comparison hooks. Return nullptr to decline: core then keeps the comparison
// as an atom for the solver, because it asks pack_ba_type_has_wff_<op>_hook
// separately and knows the operator is yours. Returning a T/F node instead
// commits to a value, so only do it when both operands are ground constants.
//
// template <typename... PackBAs>
// struct ba_wff_hooks<my_ba, node<PackBAs...>> {
// 	using node_t = node<PackBAs...>;
// 	using tau = tree<node_t>;
//
// 	// ch[0] is the bf_<op> node; [i][0] is the i-th raw operand.
// 	static tref wff_lt(const tref* ch, tref r) {
// 		const tau& l = tau::get(ch[0])[0][0];
// 		const tau& rr = tau::get(ch[0])[1][0];
// 		if (!l.is_ba_constant() || !rr.is_ba_constant()) return nullptr;
// 		return tau::get(/* l < rr */ true ? tau::_T() : tau::_F(), r);
// 	}
// 	// ... wff_nlt, wff_lteq, wff_nlteq, wff_gt, wff_ngt, wff_gteq, wff_ngteq
// };

// Term hooks. `term_cast` retypes the constant inside a cast node; return
// @p symbol unchanged when the operand is not a constant you can convert.
//
// template <typename... PackBAs>
// struct ba_term_hooks<my_ba, node<PackBAs...>> {
// 	using node_t = node<PackBAs...>;
//
// 	static tref term_cast(tref symbol, size_t ba_type) {
// 		return symbol;
// 	}
// };

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__TEMPLATE__MY_BA_BA_HOOKS_EXT_TMPL_H__
