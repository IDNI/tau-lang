// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file utils.h
 * @brief Miscellaneous utility helpers shared across Tau components.
 */

#ifndef __IDNI__TAU__UTILS_H__
#define __IDNI__TAU__UTILS_H__

namespace idni::tau_lang {

/**
 * @brief Visitor helper that inherits `operator()` from each callable in @p Ts.
 *
 * Enables `std::visit` with inline lambdas: `std::visit(overloaded{...}, variant)`.
 * @tparam Ts Callable types to combine.
 */
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
/** @brief Deduction guide so `overloaded{f, g}` deduces template args automatically. */
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

} // namespace idni::tau_lang

#endif // __IDNI__TAU__UTILS_H__