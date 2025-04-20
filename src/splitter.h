// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__SPLITTER_H__
#define __IDNI__TAU__SPLITTER_H__

#include "satisfiability.h"

namespace idni::tau_lang {

enum class split_sym {
	conjunction, disjunction
};

// Return a bad splitter for the provided formula
// We assume the formula is fully normalized by normalizer
template <BAsPack... BAs>
tref tau_bad_splitter(tref fm = tree<node<BAs...>>::_T().get()) {
	using tau = tree<node<BAs...>>;
	auto new_uniter_const = tau::build_wff_neq(
				get_new_uniter_const<BAs...>(fm, "split"));
	auto clauses = tau::get_dnf_wff_clauses(fm);
	// Add bad splitter only to a single disjunct if possible
	if (!clauses.empty()) {
		clauses[0] = tau::build_wff_and(clauses[0], new_uniter_const);
		return tau::build_wff_or(clauses);
	} else return tau::build_wff_and(fm, new_uniter_const);
}

} // idni::tau_lang namespace

#endif //__IDNI__TAU__SPLITTER_H__
