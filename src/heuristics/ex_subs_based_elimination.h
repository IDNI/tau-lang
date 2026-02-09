// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__EX_SUBS_BASED_ELIMINATION_H__
#define __IDNI__TAU__EX_SUBS_BASED_ELIMINATION_H__

namespace idni::tau_lang {

// We eliminate the existential quiantifier by substituting the variable
// according to the substitutions found in the formula.
// If no compatible substitution is found, we return the original clause.
template <NodeType node>
tref ex_subs_based_elimination(tref ex_clause, tref var);

} // namespace idni::tau_lang

#include "ex_subs_based_elimination.tmpl.h"

#endif // __IDNI__TAU__EX_SUBS_BASED_ELIMINATION_H__