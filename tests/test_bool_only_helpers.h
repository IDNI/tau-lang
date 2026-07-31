// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__TESTS__TEST_BOOL_ONLY_HELPERS_H__
#define __IDNI__TAU__TESTS__TEST_BOOL_ONLY_HELPERS_H__

// Pack-agnostic fixture: node<Bool> alone, so its tests build under any
// -DTAU_BAS=. Tests exercising bv or sbf dispatch use test_Bool_helpers.h.
#define bas_pack Bool
#include "test_helpers.h"

#endif // __IDNI__TAU__TESTS__TEST_BOOL_ONLY_HELPERS_H__
