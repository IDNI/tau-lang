// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ba_pack_traits.h
 * @brief Traits about a BA pack as a whole, asked without naming a BA.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BA_PACK_TRAITS_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BA_PACK_TRAITS_H__

#include <cstddef>
#include <optional>
#include <tuple>
#include <type_traits>

#include "boolean_algebras/ba_descriptor.h"

namespace idni::tau_lang {

/**
 * @brief `true` for the wrapper BA that embeds a whole Tau spec.
 *
 * The primary sits in core so generic dispatch can ask without including the
 * tau plugin; the tau plugin specializes it for `tau_ba<BAs...>`.
 */
template <typename T>
struct is_tau_ba : std::false_type {};

template <typename T>
inline constexpr bool is_tau_ba_v = is_tau_ba<T>::value;

/**
 * @brief `true` for a BA with arithmetic terms and its own decision procedure.
 *
 * The primary sits in core so the normalizer can ask whether the configured
 * pack needs the arithmetic machinery (predicate blasting, the arithmetic
 * skip, the theory solver) at all; a BA that has it specializes this in its
 * own header.
 */
template <typename BA>
struct ba_has_arithmetic_theory : std::false_type {};

template <typename BA>
inline constexpr bool ba_has_arithmetic_theory_v =
	ba_has_arithmetic_theory<BA>::value;

/** @internal @brief Fold of @ref ba_has_arithmetic_theory over a pack. */
template <typename Node, std::size_t... Is>
constexpr bool pack_has_arithmetic_theory_impl(std::index_sequence<Is...>) {
	using pack = typename Node::bas_tuple;
	return (ba_has_arithmetic_theory_v<std::tuple_element_t<Is, pack>>
		|| ...);
}

/** @brief `true` when any BA of @p Node's pack has an arithmetic theory. */
template <typename Node>
inline constexpr bool pack_has_arithmetic_theory_v =
	pack_has_arithmetic_theory_impl<Node>(
		std::make_index_sequence<
			std::tuple_size_v<typename Node::bas_tuple>>{});

/**
 * @brief Result of @p probe on the first BA of @p Node's pack that returns one.
 *
 * @p probe is invoked as `probe.template operator()<BA>()` for each BA in pack
 * order and must return a `std::optional`; the first engaged result wins.
 */
template <typename Node, typename Probe>
auto pack_first_owner(Probe&& probe) {
	using pack = typename Node::bas_tuple;
	return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
		using result_t = decltype(probe.template operator()<
			std::tuple_element_t<0, pack>>());
		result_t out = std::nullopt;
		(void)((out = probe.template operator()<
			std::tuple_element_t<Is, pack>>(), out.has_value())
				|| ...);
		return out;
	}(std::make_index_sequence<std::tuple_size_v<pack>>{});
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BA_PACK_TRAITS_H__
