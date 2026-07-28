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

/** @brief `true` when @p BA's descriptor solves formulas itself. */
template <typename Node, typename BA, typename Form>
concept ba_solves = requires(Form f) { ba_descriptor<BA, Node>::solve(f); };

/** @brief `true` when @p BA's descriptor answers satisfiability itself. */
template <typename Node, typename BA, typename Form>
concept ba_checks_sat = requires(Form f) { ba_descriptor<BA, Node>::is_sat(f); };

namespace detail {

template <typename Node, typename Form, typename First, typename... Rest>
auto pack_solve_impl(Form form) {
	if constexpr (ba_solves<Node, First, Form>)
		return ba_descriptor<First, Node>::solve(form);
	else if constexpr (sizeof...(Rest) > 0)
		return pack_solve_impl<Node, Form, Rest...>(form);
	else static_assert(sizeof...(Rest) > 0,
		"pack_solve: no BA in this pack provides solve()");
}

template <typename Node, typename Form, typename First, typename... Rest>
bool pack_is_sat_impl(Form form) {
	if constexpr (ba_checks_sat<Node, First, Form>)
		return ba_descriptor<First, Node>::is_sat(form);
	else if constexpr (sizeof...(Rest) > 0)
		return pack_is_sat_impl<Node, Form, Rest...>(form);
	else static_assert(sizeof...(Rest) > 0,
		"pack_is_sat: no BA in this pack provides is_sat()");
}

} // namespace detail

/**
 * @brief Solve @p form with the first BA whose descriptor offers a solver.
 *
 * Templated on @p Form and returning `auto` so core need not name the solution
 * type, which would pull solver headers into these traits.
 */
template <typename Node, typename Form>
auto pack_solve(Form form) {
	return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
		return detail::pack_solve_impl<Node, Form,
			std::tuple_element_t<Is, typename Node::bas_tuple>...>(form);
	}(std::make_index_sequence<
		std::tuple_size_v<typename Node::bas_tuple>>{});
}

/** @brief Ask the first BA whose descriptor offers it whether @p form is sat. */
template <typename Node, typename Form>
bool pack_is_sat(Form form) {
	return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
		return detail::pack_is_sat_impl<Node, Form,
			std::tuple_element_t<Is, typename Node::bas_tuple>...>(form);
	}(std::make_index_sequence<
		std::tuple_size_v<typename Node::bas_tuple>>{});
}

/** @brief `true` when @p BA's descriptor preprocesses formulas itself. */
template <typename Node, typename BA, typename Form>
concept ba_preprocesses = requires(Form f) {
	ba_descriptor<BA, Node>::preprocess(f);
};

/**
 * @brief Run @p form through the preprocessing of every BA that offers it.
 *
 * A BA whose preprocessing is disabled returns @p form unchanged, so callers
 * test the result against the input rather than consulting a flag.
 */
template <typename Node, typename Form>
Form pack_preprocess(Form form) {
	Form out = form;
	[&]<std::size_t... Is>(std::index_sequence<Is...>) {
		([&] {
			using BA = std::tuple_element_t<Is, typename Node::bas_tuple>;
			if constexpr (ba_preprocesses<Node, BA, Form>)
				out = ba_descriptor<BA, Node>::preprocess(out);
		}(), ...);
	}(std::make_index_sequence<
		std::tuple_size_v<typename Node::bas_tuple>>{});
	return out;
}

/** @brief Enable or disable preprocessing on every BA that supports it. */
template <typename Node>
void pack_set_preprocessing(bool enabled) {
	[&]<std::size_t... Is>(std::index_sequence<Is...>) {
		([&] {
			using BA = std::tuple_element_t<Is, typename Node::bas_tuple>;
			if constexpr (requires {
				ba_descriptor<BA, Node>::set_preprocessing(enabled); })
				ba_descriptor<BA, Node>::set_preprocessing(enabled);
		}(), ...);
	}(std::make_index_sequence<
		std::tuple_size_v<typename Node::bas_tuple>>{});
}

/**
 * @brief `true` when @p BA declares it can host the pack's Boolean carrier.
 *
 * Optional capability: a BA that does not declare `can_host_bool` simply is not
 * a carrier. Probed rather than required, so core still names no BA when it
 * needs the carrier's literals.
 */
template <typename Node, typename BA>
constexpr bool ba_can_host_bool() {
	if constexpr (requires {
		{ ba_descriptor<BA, Node>::can_host_bool }
			-> std::convertible_to<bool>; })
		return ba_descriptor<BA, Node>::can_host_bool;
	else return false;
}

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
