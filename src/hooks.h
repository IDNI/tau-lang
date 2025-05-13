// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__HOOKS_H__
#define __IDNI__TAU__HOOKS_H__

#include "tau_tree.h"

#ifdef HOOK_LOGGING_ENABLED
#define HOOK_LOGGING(x) x
#else
#define HOOK_LOGGING(x)
#endif // HOOK_LOGGING_ENABLED

namespace idni::tau_lang {

template <NodeType node>
struct get_hook {
	friend tree<node>;
private:
	using tau = tree<node>;
	using tt = tau::traverser;

	tref operator()(const node& v, const tref* ch, size_t len, tref r);

#ifdef HOOK_LOGGING_ENABLED
	static void log(const char* msg, const node& v, const tref* ch,
			size_t len, tref r, bool track_each_call = false);
#endif // HOOK_LOGGING

	// helpers
	static inline const tree<node>& arg1_fm(const tref* ch);
	static inline const tree<node>& arg2_fm(const tref* ch);
	static inline const tree<node>& arg3_fm(const tref* ch);
	static inline const tree<node>& arg1(const tref* ch);
	static inline const tree<node>& arg2(const tref* ch);
	static inline const tree<node>& arg3(const tref* ch);
	static inline const tree<node>& logic_operator(const tref* ch);
	static inline const tree<node>& quantifier(const tref* ch);
	static inline const tree<node>& quantified_formula(const tref* ch);

	// hooks
	static tref _0_typed(size_t ba_type, tref r);
	static tref _1_typed(size_t ba_type, tref r);
	static tref _F_typed(size_t ba_type, tref r);
	static tref _T_typed(size_t ba_type, tref r);
	static tref _0(const node& v, const tref* ch, size_t len, tref r);
	static tref _1(const node& v, const tref* ch, size_t len, tref r);
	static tref _F(const node& v, const tref* ch, size_t len, tref r);
	static tref _T(const node& v, const tref* ch, size_t len, tref r);
	static tref ctn_neg(tref n);
	static tref term           (const node& v, const tref* ch, size_t len, tref r);
	static tref term_or        (const node& v, const tref* ch, size_t len, tref r);
	static tref term_and       (const node& v, const tref* ch, size_t len, tref r);
	static tref term_neg       (const node& v, const tref* ch, size_t len, tref r);
	static tref term_xor       (const node& v, const tref* ch, size_t len, tref r);
	static tref cte            (const node& v, const tref* ch, size_t len, tref r);
	static tref cte_or         (const node& v, const tref* ch, size_t len, tref r);
	static tref cte_and        (const node& v, const tref* ch, size_t len, tref r);
	static tref cte_neg        (const node& v, const tref* ch, size_t len, tref r);
	static tref cte_xor        (const node& v, const tref* ch, size_t len, tref r);
	static tref wff            (const node& v, const tref* ch, size_t len, tref r);
	static tref wff_and        (const node& v, const tref* ch, size_t len, tref r);
	static tref wff_or         (const node& v, const tref* ch, size_t len, tref r);
	static tref wff_neg        (const node& v, const tref* ch, size_t len, tref r);
	static tref wff_xor        (const node& v, const tref* ch, size_t len, tref r);
	static tref wff_ctn        (const node& v, const tref* ch, size_t len, tref r);
	static tref wff_eq         (const node& v, const tref* ch, size_t len, tref r);
	static tref wff_eq_cte     (const node& v, const tref* ch, size_t len, tref r);
	static tref wff_neq        (const node& v, const tref* ch, size_t len, tref r);
	static tref wff_neq_cte    (const node& v, const tref* ch, size_t len, tref r);
	static tref wff_sometimes  (const node& v, const tref* ch, size_t len, tref r);
	static tref wff_always     (const node& v, const tref* ch, size_t len, tref r);
	static tref wff_conditional(const node& v, const tref* ch, size_t len, tref r);
	static tref wff_imply      (const node& v, const tref* ch, size_t len, tref r);
	static tref wff_rimply     (const node& v, const tref* ch, size_t len, tref r);
	static tref wff_equiv      (const node& v, const tref* ch, size_t len, tref r);
	static tref wff_lt         (const node& v, const tref* ch, size_t len, tref r);
	static tref wff_nlt        (const node& v, const tref* ch, size_t len, tref r);
	static tref wff_lteq       (const node& v, const tref* ch, size_t len, tref r);
	static tref wff_nlteq      (const node& v, const tref* ch, size_t len, tref r);
	static tref wff_gt         (const node& v, const tref* ch, size_t len, tref r);
	static tref wff_ngt        (const node& v, const tref* ch, size_t len, tref r);
	static tref wff_gteq       (const node& v, const tref* ch, size_t len, tref r);
	static tref wff_ngteq      (const node& v, const tref* ch, size_t len, tref r);
	static tref wff_interval   (const node& v, const tref* ch, size_t len, tref r);
	static tref shift          (const node& v, const tref* ch, size_t len, tref r);
};

} // namespace idni::tau_lang

#include "hooks.tmpl.h"

#endif // __IDNI__TAU__HOOKS_H__
