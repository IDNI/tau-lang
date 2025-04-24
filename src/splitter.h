// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__SPLITTER_H__
#define __IDNI__TAU__SPLITTER_H__

#include "satisfiability.h"
#include "boolean_algebras/nso_ba.h"

namespace idni::tau_lang {

enum class split_sym {
	conjunction, disjunction
};

template <BAsPack... BAs>
tref split(tref fm, const size_t fm_type, bool is_cnf, const splitter_type st,
	trefs& mem, size_t i, bool check_temps);

// If checking a temporal Tau formula F, we split a single DNF clause.
// In order to check if the split clause yields a splitter for F, we have that
// "fm" holds the clause, "splitter" holds the splitter of the clause and
// "spec" holds the original temporal Tau formula
// If we check a non-temporal Tau formula, it suffices to place it in "fm" and
// the proposed splitter in "splitter".
template <BAsPack... BAs>
bool is_splitter(tref fm, tref splitter, tref spec_clause = nullptr);

// Find a Boolean function which implies f
template <BAsPack... BAs>
tref good_splitter_using_function(tref f, splitter_type st, tref original_fm);

// Find a Boolean function which is implied by f
template <BAsPack... BAs>
tref good_reverse_splitter_using_function(tref f, splitter_type st,
							tref original_fm);

// Return a bad splitter for the provided formula
// We assume the formula is fully normalized by normalizer
template <BAsPack... BAs>
tref tau_bad_splitter(tref fm = tree<node<BAs...>>::_T());

// Return a splitter for the provided non-temporal formula
// We assume the formula is fully normalized by normalizer
template <BAsPack... BAs>
std::pair<tref, splitter_type> nso_tau_splitter(tref fm,
				splitter_type st, tref spec_clause = nullptr);

// We assume fm to be normalized in DNF
template <BAsPack... BAs>
tref tau_splitter(tref fm, splitter_type st);

} // idni::tau_lang namespace

#include "splitter.tmpl.h"

#endif //__IDNI__TAU__SPLITTER_H__
