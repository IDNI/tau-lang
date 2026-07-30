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

template <typename... PackBAs>
struct ba_wff_hooks<bv, node<PackBAs...>> {
	using node_t = node<PackBAs...>;

	static tref wff_lt(const tref* ch, tref r) {
		return wff_bv_lt<node_t>(ch, r);
	}
	static tref wff_nlt(const tref* ch, tref r) {
		return wff_bv_nlt<node_t>(ch, r);
	}
	static tref wff_lteq(const tref* ch, tref r) {
		return wff_bv_lteq<node_t>(ch, r);
	}
	static tref wff_nlteq(const tref* ch, tref r) {
		return wff_bv_nlteq<node_t>(ch, r);
	}
	static tref wff_gt(const tref* ch, tref r) {
		return wff_bv_gt<node_t>(ch, r);
	}
	static tref wff_ngt(const tref* ch, tref r) {
		return wff_bv_ngt<node_t>(ch, r);
	}
	static tref wff_gteq(const tref* ch, tref r) {
		return wff_bv_gteq<node_t>(ch, r);
	}
	static tref wff_ngteq(const tref* ch, tref r) {
		return wff_bv_ngteq<node_t>(ch, r);
	}
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_BA_HOOKS_EXT_TMPL_H__
