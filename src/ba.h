// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentioned cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

#include <tuple>

/*
 * This is a template for a general product of boolean algebras.
 */
template <typename...BAS>
struct ba_product: std::tuple<BAS...> {

	ba_product(BAS... bas): std::tuple<BAS...>(bas...) {}

	template <std::size_t I = 0, typename P, typename... PS>
	inline typename std::enable_if<(0 < sizeof...(PS)), std::tuple<P, PS...>>::type
	_not() {
		return std::tuple_cat(
			std::make_tuple(!std::get<I>(*this)),
			__not<I+1, PS...>());
	}

	template <std::size_t I = 0, typename P, typename... PS>
	inline typename std::enable_if<(0 == sizeof...(PS)), std::tuple<P>>::type
	__not() {
		return std::make_tuple(!std::get<I>(*this));
	}

	std::tuple<BAS...> operator!() {
		return __not<0, BAS...>();
	}

	template <std::size_t I = 0, typename P, typename... PS>
	inline typename std::enable_if<(0 < sizeof...(PS)), std::tuple<PS...>>::type
	__and(std::tuple<PS...>& that) {
		return std::tuple_cat(
			std::make_tuple(std::get<I>(*this) && std::get<I>(that)),
			__and<I+1, PS...>(that));
	}

	template <std::size_t I = 0, typename P, typename... PS>
	inline typename std::enable_if<(I == sizeof...(PS)), std::tuple<P>>::type
	__and(std::tuple<BAS...>& that) {
		return std::make_tuple(std::get<I>(*this) && std::get<I>(that));
	}

	std::tuple<BAS...> operator&&(std::tuple<BAS...>& that) {
		return __and<0, BAS...>(that);
	}

};

/*
 * This is the implementation of the classical 0/1 algebraa.
 */

struct ba_01 {
	bool value;

	ba_01 operator&&(ba_01 that) { return {value && that.value}; }
	ba_01 operator!() { return {!value}; }
};
