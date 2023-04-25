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

// The product boolean algebra
template <typename...BAS>
struct ba_product: std::tuple<BAS...> {


	ba_product(): std::tuple<BAS...>() {}
	ba_product(BAS... bas): std::tuple<BAS...>(bas...) {}

	auto operator<=>(const ba_product<BAS...>& that) const = default;

	template <size_t...idx>
	void __not(ba_product<BAS...>& to, std::index_sequence<idx...>) {
		auto f = [](auto a) { return !a; };
		(void)std::initializer_list<int>{
			(std::get<idx>(to) = f(std::get<idx>(std::forward<ba_product<BAS...>>(*this))), 0)...};  
	}

	ba_product<BAS...> operator!() {
		ba_product<BAS...> result;
		__not(result, std::make_integer_sequence<size_t, sizeof...(BAS)>());
		return result;
	}

	template <size_t...idx>
	void __and(ba_product<BAS...>& that, ba_product<BAS...>& to, std::index_sequence<idx...>) {
		auto f = [](auto a, auto b) { return a & b; };
		(void)std::initializer_list<int>{
			(std::get<idx>(to) = f(std::get<idx>(std::forward<ba_product<BAS...>>(*this)), 
				std::get<idx>(std::forward<ba_product<BAS...>>(that))), 0)...};  
	}

	ba_product<BAS...> operator&(ba_product<BAS...>& that) {
		ba_product<BAS...> result;
		__and(that, result, std::make_integer_sequence<size_t, sizeof...(BAS)>());
		return result;
	}
};
