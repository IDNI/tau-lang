// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__TESTS__TEST_SBF_BA_HELPERS_H__
#define __IDNI__TAU__TESTS__TEST_SBF_BA_HELPERS_H__

// Deliberately hardcoded fixture: the bv + sbf combination specifically,
// independent of -DTAU_BAS=. Do not switch to TAU_PACK_*; tests including this
// are gated on bv and sbf.
#define bas_pack bv, sbf_ba
#include "test_helpers.h"

#endif // __IDNI__TAU__TESTS__TEST_SBF_BA_HELPERS_H__
