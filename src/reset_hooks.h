// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file reset_hooks.h
 * @brief Hooks `api::reset` runs to empty a process-wide cache.
 *
 * A cache made with `tree<node>::create_cache` needs no hook: the sweep that
 * follows the hooks drops its rows. A cache that holds `htref`s, or that lives
 * outside the tree store, registers here once, from the static that owns it.
 */

#ifndef __IDNI__TAU__RESET_HOOKS_H__
#define __IDNI__TAU__RESET_HOOKS_H__

#include <functional>
#include <mutex>
#include <vector>

namespace idni::tau_lang {

/// The hooks registered with @ref on_reset, in registration order.
inline std::vector<std::function<void()>>& reset_hooks() {
	static std::vector<std::function<void()>> hooks;
	return hooks;
}

/// Guards @ref reset_hooks.
inline std::mutex& reset_hooks_mutex() {
	static std::mutex m;
	return m;
}

/// Register @p hook to run on every `api::reset`.
inline void on_reset(std::function<void()> hook) {
	std::lock_guard lock(reset_hooks_mutex());
	reset_hooks().push_back(std::move(hook));
}

/// Run every hook registered with @ref on_reset.
inline void run_reset_hooks() {
	std::lock_guard lock(reset_hooks_mutex());
	for (const auto& hook : reset_hooks()) hook();
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__RESET_HOOKS_H__
