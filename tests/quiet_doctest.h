// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__TESTS__QUIET_DOCTEST_H__
#define __IDNI__TAU__TESTS__QUIET_DOCTEST_H__

// doctest.h is a downloaded file, so the build reports none of its warnings
#if defined(__clang__)
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Weverything"
#elif defined(_MSC_VER)
#	pragma warning(push, 0)
#endif
#include "doctest.h"
#if defined(__clang__)
#	pragma clang diagnostic pop
#elif defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif // __IDNI__TAU__TESTS__QUIET_DOCTEST_H__
