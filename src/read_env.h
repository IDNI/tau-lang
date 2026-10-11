// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__READ_ENV_H__
#define __IDNI__TAU__READ_ENV_H__

#include <cstdlib>
#include <memory>
#include <optional>
#include <string>

#include "parser_strings.h"
#include "tau_diagnostics.h"

namespace idni::tau_lang {

/// The value of the environment variable @p name, or nullopt when it is
/// unset. A failed read is an error in the report. A `TAU_` variable is an
/// option, so it goes through `options().load_env("TAU_")` instead.
inline result<std::optional<std::string>> read_env(const char* name) {
	result<std::optional<std::string>> r;
#ifdef _MSC_VER
	// the MSVC CRT marks getenv unsafe and asks for _dupenv_s
	char* raw = nullptr;
	std::size_t raw_size = 0;
	if (_dupenv_s(&raw, &raw_size, name) != 0)
		return r.with_error(code::runtime_error,
			parser_strings::messages::option_env_read_failed,
			{ { label::name, name } });
	const std::unique_ptr<char, decltype(&std::free)> owned(raw,
		&std::free);
	const char* text = owned.get();
#else
	const char* text = std::getenv(name);
#endif
	if (!text) return r.with_value(std::nullopt);
	return r.with_value(std::string(text));
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__READ_ENV_H__
