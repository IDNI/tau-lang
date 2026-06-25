/**
 * @file product_ba.h
 * @brief Product Boolean algebra over a pack of BA types.
 *
 * `product_ba<BAS...>` stores one element of each BA in a `std::tuple` and
 * applies operators component-wise.  Used to combine multiple independent
 * Boolean algebras into a single algebraic structure.
 */

// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__PRODUCT_BA_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__PRODUCT_BA_H__

#include <tuple>
#include <list>
#include <variant>

namespace idni::tau_lang {

/**
 * @brief Component-wise product of multiple Boolean algebras.
 *
 * Holds one element per BA type in `BAS...` and applies operators
 * (AND, OR, XOR, NOT) component-wise across the tuple.
 *
 * @tparam BAS Pack of Boolean algebra types.
 */
template <typename...BAS>
struct product_ba: std::tuple<BAS...> {

	/** @brief Default-construct all components. */
	product_ba(): std::tuple<BAS...>() {}
	/** @brief Construct with one value per BA type. */
	product_ba(BAS... bas): std::tuple<BAS...>(bas...) {}

	/** @brief Three-way comparison (defaulted, compares tuple members lexicographically). */
	auto operator<=>(const product_ba<BAS...>& that) const = default;

	/** @brief Component-wise bitwise NOT. */
	product_ba<BAS...> operator~() {
		product_ba<BAS...> result;
		auto __not = [](auto a, auto& c ) { return c = ~a; };
		(__not(get<BAS>(*this), get<BAS>(result)), ...);
		return result;
	}

	/** @brief Component-wise bitwise AND. */
	product_ba<BAS...> operator&(product_ba<BAS...>& that) {
		product_ba<BAS...> result;
		auto __and = [](auto a, auto b, auto& c ) { return c = (a&b); };
		(__and(get<BAS>(*this), get<BAS>(that), get<BAS>(result)), ...);
		return result;
	}

	/** @brief Component-wise bitwise OR. */
	product_ba<BAS...> operator|(product_ba<BAS...>& that) {
		product_ba<BAS...> result;
		auto __or = [](auto a, auto b, auto& c ) { return c = (a|b); };
		(__or(get<BAS>(*this), get<BAS>(that), get<BAS>(result)), ...);
		return result;
	}

	/** @brief Component-wise bitwise XOR. */
	product_ba<BAS...> operator^(product_ba<BAS...>& that) {
		product_ba<BAS...> result;
		auto __xor = [](auto a, auto b, auto& c ) { return c = (a^b); };
		(__xor(get<BAS>(*this), get<BAS>(that), get<BAS>(result)), ...);
		return result;
	}
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__PRODUCT_BA_H__