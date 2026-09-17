/**
 * @file bv_case_split.h
 * @brief Test-point elimination of quantified bitvector variables.
 *
 * Provides `bv_case_split_quantifiers`, which eliminates a quantified
 * bitvector variable that occurs only in comparisons against constants of
 * its own type, replacing the quantifier with a finite case split over the
 * cells those constants cut the domain into.
 */

// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_CASE_SPLIT_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_CASE_SPLIT_H__

#include <cstdlib>
#include <functional>
#include <limits>
#include <optional>
#include <unordered_set>

#include "nso_rr.h"

namespace idni::tau_lang {

/// Test-point elimination of quantified bitvector variables that occur only
/// in comparisons against constants (see `bv_case_split_quantifiers` in
/// boolean_algebras/bv/heuristics/bv_case_split.tmpl.h). On by default: it is
/// an identity on the formula, and it is what keeps a conditional controller
/// over a bitvector command independent of the Boole split budget (GitHub
/// #107). Disabled via `--bv-case-split=false`, the REPL option
/// `bv-case-split`, or the environment variable TAU_BV_CASE_SPLIT=0 (any
/// other value enables; the variable overrides the flag in both directions).
inline bool bv_case_split = true;

inline bool bv_case_split_enabled() {
	static const std::optional<bool> env = []() -> std::optional<bool> {
		const char* v = std::getenv("TAU_BV_CASE_SPLIT");
		if (!v || !*v) return std::nullopt;
		return !(v[0] == '0' && v[1] == '\0');
	}();
	return env ? *env : bv_case_split;
}

/// Cap on the number of distinct constants a quantified bitvector variable
/// may be tested against for the case split to apply; above it the binder is
/// left to the pipeline. The split builds one instance per cell (2k+1 for k
/// order-tested constants, k+1 with equalities only), so this bounds the
/// instance count. SIZE_MAX = unlimited (0 through the option setter); set
/// via --bv-case-split-max-tests or the REPL option
/// bv-case-split-max-tests.
inline size_t bv_case_split_max_tests = std::numeric_limits<size_t>::max();

// Test-point elimination of a quantified bitvector variable v that occurs
// only in comparisons (`=`, `!=`, `<`, `<=`, `>`, `>=`, possibly negated)
// against constants c_1 < ... < c_k of its own type.
//
// Over the unsigned, finite bitvector order every such atom changes its value
// only at a c_i, so phi is constant on each cell the c_i cut the domain into
// -- every c_i on its own, and the open intervals below c_1, between
// consecutive c_i, and above c_k -- and one witness per non-empty cell
// decides it:
//
//     ex v phi   ==   \/ over cells  phi[v := witness(cell)]
//     all v phi  ==   /\ over cells  phi[v := witness(cell)]
//
// When v is only ever tested for equality, all interval cells agree (every
// test is false there) and a single complement witness stands for them:
// k + 1 instances instead of at most 2k + 1. This is the finite-domain form
// of test-point elimination (Cooper / Ferrante-Rackoff), an identity on the
// formula; the substituted tests fold to T/F in the normalization that
// follows, which is what makes the instances cheap.
//
// The same identity holds for symbolic bounds t_i (test points 0, t_i and
// t_i + 1), but its instances then carry `t_i + 1` arithmetic and atoms such
// as `t + 1 = t` that the normalization does not decide, so they would be
// handed to blasting -- the path this pass exists to avoid. Measured on the
// integration tests: such instances come back as residues, not as T/F. The
// symbolic case is therefore left to the existing pipeline.
//
// Why it is here: a conditional over a bitvector command, `i[t] = c ? A : B`,
// desugars to `(i[t] != c || A) && (i[t] = c || B)` -- the guard in both
// polarities. When the interpreter closes a boundary step it quantifies such
// commands and result codes, and bitvector atoms are reserved for blasting and
// the solver, so the Boole decomposition splits on the remaining atomless
// equalities and carries every guard unchanged into both branches of every
// split. Eliminating the tested variable first lets the guards fold before
// any block is formed.
//
// Structural criterion only: any other occurrence of v (inside a term, a
// comparison with a non-constant, both sides of one atom, a nested binder of
// the same variable), or a variable of a foreign type, leaves the quantifier
// to the existing pipeline untouched. A cell exists for every tested
// constant, so the domain is never exhausted and no width limit is needed.
template <NodeType node>
tref bv_case_split_quantifiers(tref formula);

} // namespace idni::tau_lang

#include "bv_case_split.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_CASE_SPLIT_H__
