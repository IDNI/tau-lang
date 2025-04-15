// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __BA_H__
#define __BA_H__

#include <tuple>
#include <list>
#include <variant>

// product boolean algebra
template <typename...BAS>
struct product_ba: std::tuple<BAS...> {

	product_ba(): std::tuple<BAS...>() {}
	product_ba(BAS... bas): std::tuple<BAS...>(bas...) {}

	auto operator<=>(const product_ba<BAS...>& that) const = default;

	product_ba<BAS...> operator~() {
		product_ba<BAS...> result;
		auto __not = [](auto a, auto& c ) { return c = ~a; };
		(__not(get<BAS>(*this), get<BAS>(result)), ...);
		return result;
	}

	product_ba<BAS...> operator&(product_ba<BAS...>& that) {
		product_ba<BAS...> result;
		auto __and = [](auto a, auto b, auto& c ) { return c = (a&b); };
		(__and(get<BAS>(*this), get<BAS>(that), get<BAS>(result)), ...);
		return result;
	}

	product_ba<BAS...> operator|(product_ba<BAS...>& that) {
		product_ba<BAS...> result;
		auto __or = [](auto a, auto b, auto& c ) { return c = (a|b); };
		(__or(get<BAS>(*this), get<BAS>(that), get<BAS>(result)), ...);
		return result;
	}

	product_ba<BAS...> operator^(product_ba<BAS...>& that) {
		product_ba<BAS...> result;
		auto __xor = [](auto a, auto b, auto& c ) { return c = (a^b); };
		(__xor(get<BAS>(*this), get<BAS>(that), get<BAS>(result)), ...);
		return result;
	}
};

#endif // __BA_H__