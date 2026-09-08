// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file terms.tmpl.h
 * @brief Template implementations for terms.h (package B). Included by terms.h.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_TMPL_H__

namespace idni::tau_lang::anti_prenexing {

// TODO(package B): definitions of every declaration in terms.h —
//   prepare_terms, finish_terms, is_bdd_backed, cofactor, forall_over,
//   exists_over, functional_quantifier, carries_functional_quantifier,
//   subst_term, simplify_term, simplify_atom, term_of, norm_equation,
//   mem_size, leaf_fv.
// Reuse: tau_term_bdd_handle (tau_bdd.h: build, bdd_ex/all/quant,
//   bdd_compose, convert_to_tau_node, to_tau_term, get_free_tau_vars),
//   node::ba::simplify_term, syntactic_path_simplification's _simplify_bf
//   sweep (heuristics/), tau_lang::norm_equation
//   (normal_forms_transformations), bdd_node_count
//   (heuristics/bv_predicate_blasting.tmpl.h).
// Open checks: to_tau_term's output form and round-trip idempotence (D2);
//   how BDD_ID nodes print and pass the construction hooks (D4); where
//   bf_fall/bf_fex subscripts are stored, for a canonical symbolic term.
// Tests: tests/unit/test_anti_prenex_terms.cpp.

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_TMPL_H__
