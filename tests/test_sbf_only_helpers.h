// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__TESTS__TEST_SBF_ONLY_HELPERS_H__
#define __IDNI__TAU__TESTS__TEST_SBF_ONLY_HELPERS_H__

// Deliberately hardcoded fixture: sbf + Bool, independent of -DTAU_BAS=. Do
// not switch to TAU_PACK_*; tests including this are gated on sbf alone (not
// bv -- that is test_Bool_helpers.h's combination, and test_sbf_ba_helpers.h
// is the bv+sbf one).
#define bas_pack sbf_ba, Bool
#include "test_helpers.h"

#endif // __IDNI__TAU__TESTS__TEST_SBF_ONLY_HELPERS_H__
