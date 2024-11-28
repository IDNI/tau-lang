// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __BA_H__
#define __BA_H__

#include <tuple>
#include <list>
#include <variant>

#include "bdd_handle.h"

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
		auto __and = [](auto a, auto b, auto& c ) { return c = (a & b); };
		(__and(get<BAS>(*this), get<BAS>(that), get<BAS>(result)), ...);
		return result;
	}

	product_ba<BAS...> operator|(product_ba<BAS...>& that) {
		product_ba<BAS...> result;
		auto __or = [](auto a, auto b, auto& c ) { return c = (a | b); };
		(__or(get<BAS>(*this), get<BAS>(that), get<BAS>(result)), ...);
		return result;
	}

	product_ba<BAS...> operator^(product_ba<BAS...>& that) {
		product_ba<BAS...> result;
		auto __xor = [](auto a, auto b, auto& c ) { return c = (a ^ b); };
		(__xor(get<BAS>(*this), get<BAS>(that), get<BAS>(result)), ...);
		return result;
	}
};

// reference boolean algebra
template <typename B>
struct ba_ref {
	//TODO add proper builders/constructors to
	// all the classes involved in this algebra
	struct var {
		size_t id;
		int offset;
	};

	struct ref {
		// id of the cbf or wff that this ref belongs to
		std::size_t id;
		// TODO check if maybe a pair of variants is better?
		std::list<std::variant<var, size_t>> idxs;
		std::list<std::variant<var, B>> args;
	};

	// each ref corresponds to a unique bdd variable
	static std::map<ref, size_t> refs_map;
	hbdd<B> h;

	explicit ba_ref(hbdd<B> h): h(h) {}

	ba_ref<B> operator~() {	return {~h}; }
	ba_ref<B> operator&(ba_ref<B>& that) { return {h & that.h}; }
	ba_ref<B> operator|(ba_ref<B>& that) { return {h | that.h}; }
	ba_ref<B> operator^(ba_ref<B>& that) { return {h ^ that.h};	}
};

#endif // __BA_H__