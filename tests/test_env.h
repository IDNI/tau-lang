// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__TESTS__TEST_ENV_H__
#define __IDNI__TAU__TESTS__TEST_ENV_H__

#include <optional>
#include <string>

#include "read_env.h"

/// The value of the environment variable @p name, or nullopt when it is
/// unset. A failed read is an error in the report.
inline idni::diagnostics::result<std::optional<std::string>> test_env(
	const char* name)
{
	return idni::tau_lang::read_env(name);
}

#endif // __IDNI__TAU__TESTS__TEST_ENV_H__
