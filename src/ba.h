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

	ba_product<BAS...> operator!() {
		ba_product<BAS...> result;
		auto __not = [](auto a, auto& c ) { return c = !a; };
		(__not(get<BAS>(*this), get<BAS>(result)), ...);
		return result;
	}

	ba_product<BAS...> operator&(ba_product<BAS...>& that) {
		ba_product<BAS...> result;
		auto __and = [](auto a, auto b, auto& c ) { return c = (a & b); };
		(__and(get<BAS>(*this), get<BAS>(that), get<BAS>(result)), ...);
		return result;
	}

	ba_product<BAS...> operator|(ba_product<BAS...>& that) {
		ba_product<BAS...> result;
		auto __or = [](auto a, auto b, auto& c ) { return c = (a | b); };
		(__or(get<BAS>(*this), get<BAS>(that), get<BAS>(result)), ...);
		return result;
	}

	ba_product<BAS...> operator^(ba_product<BAS...>& that) {
		ba_product<BAS...> result;
		auto __xor = [](auto a, auto b, auto& c ) { return c = (a ^ b); };
		(__xor(get<BAS>(*this), get<BAS>(that), get<BAS>(result)), ...);
		return result;
	}
};
