// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file bv_options.h
 * @brief The option set of the bitvector algebra and the binding of each
 * option to its field.
 *
 * bv_descriptor.tmpl.h includes this file after every bv field is declared.
 * Its `declared_options()` gives @ref bv_option_set, which the pack
 * declares, and its `bind_options()` calls @ref bv_bind_options.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_OPTIONS_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_OPTIONS_H__

#include <cstddef>

#include "utility/options.h"
#include "option_codecs.h"
#include "tau_diagnostics.h"
#include "tau_tree.h"

namespace idni::tau_lang {

/// The options of bv. Each name carries the type prefix, so the command
/// line, the REPL and the environment use one name.
inline const option_set bv_option_set{ {
	{ "bv-blasting", "bv", bool{ bv_blasting },
		"enable bv predicate blasting (the global `preprocessing` switch "
		"gates it too -- both must be on for bv to blast)" },
	{ "bv-blastdepth", "bv", std::size_t{ max_blast_reentry_depth },
		"cap blast-block re-entry nesting in anti-prenexing "
		"(0 = unlimited)" },
	{ "bv-case-split", "bv", bool{ bv_case_split },
		"bitvector case split of quantified variables tested against "
		"constants" },
	{ "bv-case-split-max-tests", "bv",
		zero_is_unlimited_codec{}.to_value(bv_case_split_max_tests),
		"cap the constants a quantified bitvector variable may be tested "
		"against for the case split (0 = unlimited)" },
	{ "bv-definitional-elimination", "bv",
		bool{ bv_definitional_elimination },
		"eliminate existentially quantified bitvector variables that a "
		"total definition determines, before the case split" },
	{ "bv-defelim-max-clauses", "bv",
		zero_is_unlimited_codec{}.to_value(bv_defelim_max_clauses),
		"cap the clauses a conjunct is flattened into for the "
		"definitional elimination (0 = unlimited)" },
	{ "bv-defelim-max-atoms", "bv",
		zero_is_unlimited_codec{}.to_value(bv_defelim_max_atoms),
		"cap the guard atoms the definitional elimination brute-forces "
		"over (at most 30, and 0 = 30)" },
	{ "bv-defelim-max-subset", "bv",
		zero_is_unlimited_codec{}.to_value(bv_defelim_max_subset),
		"cap the clause-subset size searched for a total definition "
		"(0 = unlimited)" },
	{ "bv-defelim-max-rounds", "bv",
		zero_is_unlimited_codec{}.to_value(bv_defelim_max_rounds),
		"cap the definitional-elimination rounds per existential block "
		"(0 = unlimited)" },
	{ "bv-quantifier-free-decision", "bv",
		bool{ bv_quantifier_free_decision },
		"decide a closed bitvector formula whose binders are all of one "
		"kind quantifier-free" },
	{ "bv-blasting-max-nodes", "bv", std::size_t{ bv_blasting_max_nodes },
		"cap the BDD nodes one bv predicate blasting may build before it "
		"declines (0 = unlimited)" },
	{ "bv-bitblast-max-width", "bv", std::size_t{ bv_bitblast_max_width },
		"widest bit-vector a formula may hold to be decided on its bits "
		"instead of by cvc5 (0 = always cvc5)" },
	{ "bv-bitblast-max-nodes", "bv", std::size_t{ bv_bitblast_max_nodes },
		"cap the BDD nodes in use at once when a bitvector formula of at "
		"most bv-bitblast-max-width bits is decided on its bits, before "
		"cvc5 takes it (0 = always cvc5)" },
	{ "bv-solve-timeout", "bv", std::size_t{ bv_solve_timeout },
		"cap in seconds each quantified bitvector question cvc5 decides, "
		"run in a separate process; past it the answer is unknown "
		"(0 = unbounded, in the process)" },
	{ "bv-widening", "bv", bool{ bv_widening },
		"exact (widened) bitvector arithmetic instead of modular "
		"wraparound" },
	{ "bv-max-width", "bv", std::size_t{ bv_max_width },
		"cap the width widening may compute up to (0 = the default)" },
} };

/// The hook of every bv option: a bv field takes part in the verdicts the
/// tree caches hold.
template <NodeType node>
void bv_clear_caches() { tree<node>::clear_caches(); }

/// Binds each option of @ref bv_option_set to its field.
template <NodeType node>
result<void> bv_bind_options(options_repository& repo) {
	result<void> r;
	const option_hook hook = ba_option_hook(bv_clear_caches<node>);
	TAU_TRY_VOID(repo.bind("bv-blasting", bv_blasting, hook));
	TAU_TRY_VOID(repo.bind("bv-blastdepth", max_blast_reentry_depth, hook));
	TAU_TRY_VOID(repo.bind("bv-case-split", bv_case_split, hook));
	TAU_TRY_VOID(repo.bind("bv-case-split-max-tests", bv_case_split_max_tests,
		zero_is_unlimited_codec{}, hook));
	TAU_TRY_VOID(repo.bind("bv-definitional-elimination",
		bv_definitional_elimination, hook));
	TAU_TRY_VOID(repo.bind("bv-defelim-max-clauses", bv_defelim_max_clauses,
		zero_is_unlimited_codec{}, hook));
	TAU_TRY_VOID(repo.bind("bv-defelim-max-atoms", bv_defelim_max_atoms,
		zero_is_unlimited_codec{}, hook));
	TAU_TRY_VOID(repo.bind("bv-defelim-max-subset", bv_defelim_max_subset,
		zero_is_unlimited_codec{}, hook));
	TAU_TRY_VOID(repo.bind("bv-defelim-max-rounds", bv_defelim_max_rounds,
		zero_is_unlimited_codec{}, hook));
	TAU_TRY_VOID(repo.bind("bv-quantifier-free-decision",
		bv_quantifier_free_decision, hook));
	TAU_TRY_VOID(repo.bind("bv-blasting-max-nodes", bv_blasting_max_nodes,
		hook));
	TAU_TRY_VOID(repo.bind("bv-bitblast-max-width", bv_bitblast_max_width,
		hook));
	TAU_TRY_VOID(repo.bind("bv-bitblast-max-nodes", bv_bitblast_max_nodes,
		hook));
	TAU_TRY_VOID(repo.bind("bv-solve-timeout", bv_solve_timeout, hook));
	TAU_TRY_VOID(repo.bind("bv-widening", bv_widening, hook));
	// 0 restores the default, as the table setter keeps the cap on 0.
	TAU_TRY_VOID(repo.bind("bv-max-width", bv_max_width, ba_option_hook([] {
		if (!bv_max_width) bv_max_width = bv_max_width_default;
		bv_clear_caches<node>();
	})));
	return r;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_OPTIONS_H__
