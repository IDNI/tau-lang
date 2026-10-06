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
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <limits>
#include <mutex>
#include <set>
#include <string>
#include <type_traits>

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
 * @param var Name of the environment variable; must not be null.
 * @param dflt Value returned when @p var is unset or invalid.
 * @return The parsed value, or @p dflt.
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

/**
 * @brief Value of the environment variable @p var as a real number, or
 * @p dflt.
 *
 * The real-valued sibling of @ref env_limit_count: an absent variable is the
 * default, and a value that is not a finite decimal number keeps the default
 * and says so once per variable and thread. The sign is the limit's to read.
 * @param var Name of the environment variable; must not be null.
 * @param dflt Value returned when @p var is unset or invalid.
 * @return The parsed value, or @p dflt.
 */
inline double env_limit_real(const char* var, double dflt) {
	const char* v = std::getenv(var);
	if (!v) return dflt;
	char* end = nullptr;
	errno = 0;
	const double d = std::strtod(v, &end);
	if (end == v || *end != '\0' || errno == ERANGE || !std::isfinite(d)) {
		static thread_local std::set<std::string> warned;
		if (warned.insert(var).second)
			TAU_LOG_WARNING << var << "='" << v << "' is not a "
				"number; keeping the default " << dflt;
		return dflt;
	}
	return d;
}

/// What a limit's environment variable set to 0 means: 0 itself
/// (`value`), the largest value (`unlimited`: a budget whose loops read
/// that as unlimited), or the default (`keep_default`: a limit whose setter
/// ignores 0).
enum class env_zero { value, unlimited, keep_default };

/**
 * @brief A runtime limit resolved as option > environment > default.
 *
 * Assigning the limit is the option: the CLI flag, the REPL `set` command
 * and the `api::set_*` setter all assign it, and the assigned value wins.
 * Until it is assigned, reading the limit yields the environment variable
 * @ref variable, validated by @ref env_limit_count (or @ref env_limit_real),
 * and the default when the variable is absent or garbage.
 *
 * The variable is read once per process, on the first read that needs it:
 * these limits are read inside rewrite loops, where a `getenv` per read
 * would cost more than the work it bounds.
 *
 * A variable set to 0 reads as its setter reads 0, chosen by @p zero (see
 * @ref env_zero).
 * @tparam T The limit's value type: an integer count, or a floating-point
 * number read with @ref env_limit_real.
 */
template <typename T>
class env_limit {
public:
	/// @param var Environment variable read when no option is set; must
	/// outlive the limit (a string literal).
	/// @param dflt Value when neither the option nor the variable is given.
	/// @param zero What the variable set to 0 reads as.
	constexpr env_limit(const char* var, T dflt,
		env_zero zero = env_zero::value)
		: var_(var), dflt_(dflt), zero_(zero) {}
	env_limit(const env_limit&) = delete;
	env_limit& operator=(const env_limit&) = delete;

	/// Set the option; it wins over the environment from now on.
	env_limit& operator=(T v) { value_ = v; given_ = true; return *this; }
	/// The effective value: the option, else the environment, else the
	/// default.
	operator T() const { return get(); }
	/// @copydoc operator T()
	T get() const { return given_ ? value_ : from_env(); }
	/// Forget the option, so the environment and the default apply again.
	void unset() { given_ = false; }
	/// The name of the environment variable of this limit.
	const char* variable() const { return var_; }
	/// The value that applies when neither the option nor the variable
	/// is given.
	T default_value() const { return dflt_; }

private:
	T from_env() const {
		std::call_once(once_, [this] {
			T v;
			if constexpr (std::is_floating_point_v<T>)
				v = (T) env_limit_real(var_, dflt_);
			else v = (T) env_limit_count(var_, dflt_);
			if (v != T{} || zero_ == env_zero::value) env_ = v;
			else env_ = zero_ == env_zero::unlimited
				? std::numeric_limits<T>::max() : dflt_;
		});
		return env_;
	}

	const char* var_;
	T dflt_;
	env_zero zero_;
	T value_{};
	bool given_ = false;
	mutable std::once_flag once_;
	mutable T env_{};
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__ENV_LIMITS_H__
