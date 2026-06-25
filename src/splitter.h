// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file splitter.h
 * @brief Splitter functions for Tau satisfiability solving.
 *
 * Provides the `tau_splitter`, `nso_tau_splitter`, and related helpers that
 * decompose a normalized Tau formula into an atomic "splitter" — a Boolean
 * function used by the satisfiability solver to guide case analysis.
 */

#ifndef __IDNI__TAU__SPLITTER_H__
#define __IDNI__TAU__SPLITTER_H__

#include "splitter_types.h"
#include "satisfiability.h"

namespace idni::tau_lang {

/**
 * @brief Identifies whether a conjunction or disjunction drives a split.
 */
enum class split_sym {
	conjunction, ///< Split along a conjunction.
	disjunction  ///< Split along a disjunction.
};

// template <typename... BAs>
// requires BAsPack<BAs...>
// tref split(tref fm, typename node<BAs...>::type fm_type, bool is_cnf,
// 	const splitter_type st, trefs& mem, size_t i, bool check_temps);

/**
 * @brief Extract a single path from @p fm and apply @p callback to it.
 *
 * Splits the DNF formula @p fm path-by-path; for each path, invokes
 * @p callback with the path's formula. Returns the first non-null callback result.
 * @tparam BAs Boolean-algebra type pack.
 * @param fm Formula to split (in DNF).
 * @param st Splitter type selector.
 * @param check_temps When `true`, also check temporal sub-formulas.
 * @param callback Callable invoked on each path candidate.
 * @return First non-null splitter returned by @p callback, or `nullptr`.
 */
template <typename... BAs>
requires BAsPack<BAs...>
tref split_path(tref fm, const splitter_type st, bool check_temps,
	const auto& callback);

/**
 * @brief Check whether @p splitter is a valid splitter for @p fm.
 *
 * For temporal formulas, @p fm holds the DNF clause, @p splitter holds the
 * candidate splitter for that clause, and @p spec_clause is the original temporal
 * Tau formula. For non-temporal formulas, only @p fm and @p splitter are needed.
 * @tparam BAs Boolean-algebra type pack.
 * @param fm Formula (or clause) to split.
 * @param splitter Candidate splitter function.
 * @param spec_clause Original temporal specification clause (or `nullptr`).
 * @return `true` if @p splitter is a valid splitter for @p fm.
 */
template <typename... BAs>
requires BAsPack<BAs...>
bool is_splitter(tref fm, tref splitter, tref spec_clause = nullptr);

/**
 * @brief Find a Boolean function that implies @p f, suitable as a splitter.
 * @tparam BAs Boolean-algebra type pack.
 * @param f Boolean function to exceed.
 * @param st Splitter type selector.
 * @param clause Current DNF clause being processed.
 * @param fm_without_clause Remainder of the formula excluding @p clause.
 * @param original_fm Full original formula.
 * @param spec_clause Temporal specification clause (or `nullptr`).
 * @return A valid splitter that implies @p f, or `nullptr` if none found.
 */
template <typename... BAs>
requires BAsPack<BAs...>
tref good_splitter_using_function(tref f, splitter_type st, tref clause,
	tref fm_without_clause, tref original_fm, tref spec_clause);

/**
 * @brief Find a Boolean function that is implied by @p f, suitable as a splitter.
 * @tparam BAs Boolean-algebra type pack.
 * @param f Boolean function to be implied.
 * @param st Splitter type selector.
 * @param clause Current DNF clause being processed.
 * @param fm_without_clause Remainder of the formula excluding @p clause.
 * @param original_fm Full original formula.
 * @param spec_clause Temporal specification clause (or `nullptr`).
 * @return A valid splitter implied by @p f, or `nullptr` if none found.
 */
template <typename... BAs>
requires BAsPack<BAs...>
tref good_reverse_splitter_using_function(tref f, splitter_type st,
	tref clause, tref fm_without_clause, tref original_fm, tref spec_clause);

/**
 * @brief Return a "bad" splitter (the `T` constant) for @p fm.
 *
 * Assumes @p fm is fully normalized by the normalizer.
 * @tparam BAs Boolean-algebra type pack.
 * @param fm Formula to produce a bad splitter for (default: `T`).
 * @return `T` constant node as a bad splitter.
 */
template <typename... BAs>
requires BAsPack<BAs...>
tref tau_bad_splitter(tref fm = tree<node<BAs...>>::_T());

/**
 * @brief Compute a splitter for a non-temporal Tau formula.
 *
 * Assumes @p fm is fully normalized by the normalizer.
 * @tparam BAs Boolean-algebra type pack.
 * @param fm Non-temporal formula in normalized form.
 * @param st Splitter type selector.
 * @param spec_clause Temporal specification clause (or `nullptr`).
 * @return Pair (splitter formula, the splitter type used).
 */
template <typename... BAs>
requires BAsPack<BAs...>
std::pair<tref, splitter_type> nso_tau_splitter(tref fm,
	splitter_type st, tref spec_clause = nullptr);

/**
 * @brief Compute a splitter for a normalized DNF Tau formula.
 *
 * Assumes @p fm is normalized in DNF.
 * @tparam BAs Boolean-algebra type pack.
 * @param fm Formula normalized in DNF.
 * @param st Splitter type selector.
 * @return Splitter formula, or `nullptr` if none found.
 */
template <typename... BAs>
requires BAsPack<BAs...>
tref tau_splitter(tref fm, splitter_type st);

/**
 * @brief Splitter function for a `nso` `tau::ba_constant` node holding a BA constant.
 * @tparam BAs Boolean-algebra type pack.
 * @param n Operand tree node.
 * @param st Splitter type (default is `splitter_type::upper`).
 * @return Result of the splitter operation on the constant.
 */
template <typename... BAs>
requires BAsPack<BAs...>
const tree<node<BAs...>>& tau_splitter(const tree<node<BAs...>>& n,
    splitter_type st = splitter_type::upper);

} // namespace idni::tau_lang

#include "splitter.tmpl.h"

#endif //__IDNI__TAU__SPLITTER_H__
