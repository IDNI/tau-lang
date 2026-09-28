// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file env_limits.h
 * @brief Environment fallback of a numeric runtime limit.
 *
 * Every runtime limit of the pipeline resolves as option > environment >
 * default: the CLI flag, the REPL `set` command and the `api::set_*` setter
 * write one parameter, and the limit's own accessor consults the parameter
 * first and this file's reader second. Only the reader knows how a variable
 * is validated, so a garbage value behaves the same for every limit that
 * has an environment form instead of once per limit.
 */

#ifndef __IDNI__TAU__ENV_LIMITS_H__
#define __IDNI__TAU__ENV_LIMITS_H__

#include "logging.h"
#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <set>
#include <string>

namespace idni::tau_lang {

/**
 * @brief Value of the environment variable @p var as a count, or @p dflt.
 *
 * An absent variable is the default. A value that is not a non-negative
 * decimal number -- a negative one, garbage, out of range, or empty --
 * keeps the default and says so once per variable and thread: an accessor
 * may be read per HOA guard, and a warning per read would bury the rest of
 * the output. Zero is accepted and passed through; what it means (unlimited,
 * or a limit's own hard bound) belongs to the limit, not to its reader.
 */
inline size_t env_limit_count(const char* var, size_t dflt) {
	const char* v = std::getenv(var);
	if (!v) return dflt;
	char* end = nullptr;
	errno = 0;
	const long n = std::strtol(v, &end, 10);
	if (end == v || *end != '\0' || n < 0 || errno == ERANGE) {
		static thread_local std::set<std::string> warned;
		if (warned.insert(var).second)
			TAU_LOG_WARNING << var << "='" << v << "' is not a "
				"non-negative number; keeping the default "
				<< dflt;
		return dflt;
	}
	return (size_t) n;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__ENV_LIMITS_H__
