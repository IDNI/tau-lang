// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__SPLITTER_H__
#define __IDNI__TAU__SPLITTER_H__

#include "splitter_types.h"
#include "satisfiability.h"

namespace idni::tau_lang {

enum class split_sym {
	conjunction, disjunction
};

// template <typename... BAs>
// requires BAsPack<BAs...>
// tref split(tref fm, typename node<BAs...>::type fm_type, bool is_cnf,
// 	const splitter_type st, trefs& mem, size_t i, bool check_temps);

template <typename... BAs>
requires BAsPack<BAs...>
tref split_path(tref fm, const splitter_type st, bool check_temps,
	const auto& callback);

// If checking a temporal Tau formula F, we split a single DNF clause.
// In order to check if the split clause yields a splitter for F, we have that
// "fm" holds the clause, "splitter" holds the splitter of the clause and
// "spec" holds the original temporal Tau formula
// If we check a non-temporal Tau formula, it suffices to place it in "fm" and
// the proposed splitter in "splitter".
template <typename... BAs>
requires BAsPack<BAs...>
bool is_splitter(tref fm, tref splitter, tref spec_clause = nullptr);

// Find a Boolean function which implies f
template <typename... BAs>
requires BAsPack<BAs...>
tref good_splitter_using_function(tref f, splitter_type st, tref clause,
	tref fm_without_clause, tref original_fm, tref spec_clause);

// Find a Boolean function which is implied by f
template <typename... BAs>
requires BAsPack<BAs...>
tref good_reverse_splitter_using_function(tref f, splitter_type st,
	tref clause, tref fm_without_clause, tref original_fm, tref spec_clause);

// Return a bad splitter for the provided formula
// We assume the formula is fully normalized by normalizer
template <typename... BAs>
requires BAsPack<BAs...>
tref tau_bad_splitter(tref fm = tree<node<BAs...>>::_T());

// Return a splitter for the provided non-temporal formula
// We assume the formula is fully normalized by normalizer
template <typename... BAs>
requires BAsPack<BAs...>
std::pair<tref, splitter_type> nso_tau_splitter(tref fm,
	splitter_type st, tref spec_clause = nullptr);

// We assume fm to be normalized in DNF
template <typename... BAs>
requires BAsPack<BAs...>
tref tau_splitter(tref fm, splitter_type st);

/**
 * @brief Splitter function for a nso tau::ba_constant node holding a BA constant.
 * @param n Operand.
 * @param st Splitter type (default is splitter_type::upper).
 * @return Result of the splitter operation.
 */
template <typename... BAs>
requires BAsPack<BAs...>
const tree<node<BAs...>>& tau_splitter(const tree<node<BAs...>>& n,
    splitter_type st = splitter_type::upper);

} // namespace idni::tau_lang

#include "splitter.tmpl.h"

#endif //__IDNI__TAU__SPLITTER_H__
