// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// The two questions the benchmark asks the tau library about a closed formula
// given as text; each is meant to run inside a child (runner.h). Both parse the
// text afresh, so a failing instance replays from its printed formula alone,
// and both throw when the text does not parse. `ms` times the question only.

#ifndef __IDNI__TAU__BV_NO_ARITH_BENCH__ENGINE_H__
#define __IDNI__TAU__BV_NO_ARITH_BENCH__ENGINE_H__

#include "runner.h"

namespace bv_bench {

/// `anti_prenexing::anti_prenex`: `T`, `F`, or `open` with the binders left.
verdict engine_verdict(const std::string& formula);

/// `bv_formula_sat_status` on the closed formula: `T` (sat), `F` (unsat),
/// `unknown`, or `untranslated` when the solver cannot read it.
verdict oracle_verdict(const std::string& formula);

} // namespace bv_bench

#endif // __IDNI__TAU__BV_NO_ARITH_BENCH__ENGINE_H__
