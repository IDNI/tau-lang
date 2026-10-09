// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__TESTS__TEST_ENV_H__
#define __IDNI__TAU__TESTS__TEST_ENV_H__

#include <cstdlib>
#include <memory>
#include <optional>
#include <string>

#include "parser_strings.h"
#include "utility/diagnostics.h"

/// The value of the environment variable @p name, or nullopt when it is
/// unset. A failed read is an error in the report.
inline idni::diagnostics::result<std::optional<std::string>> test_env(
	const char* name)
{
	idni::diagnostics::result<std::optional<std::string>> r;
#ifdef _MSC_VER
	// the MSVC CRT marks getenv unsafe and asks for _dupenv_s
	char* raw = nullptr;
	std::size_t raw_size = 0;
	if (_dupenv_s(&raw, &raw_size, name) != 0)
		return r.with_error(idni::diagnostics::code::runtime_error,
			"Cannot read the environment variable",
			{ { idni::parser_strings::label::name, name } });
	const std::unique_ptr<char, decltype(&std::free)> owned(raw,
		&std::free);
	const char* text = owned.get();
#else
	const char* text = std::getenv(name);
#endif
	if (!text) return r.with_value(std::nullopt);
	return r.with_value(std::string(text));
}

#endif // __IDNI__TAU__TESTS__TEST_ENV_H__
