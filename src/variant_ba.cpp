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
#include <ostream>
#include "variant_ba.h"

template<typename...BAs>
variant_ba<BAs...>::variant_ba() : v() {}

template<typename...BAs>
variant_ba<BAs...>::variant_ba(const std::variant<BAs...>& v) : v(v) {}


template<typename...BAs>
const variant_ba<BAs...>& variant_ba<BAs...>::zero() {
	// TODO (HIGH) find how to implement this
	static variant_ba<BAs...> b;
	return b;
}

template<typename...BAs>
const variant_ba<BAs...>& variant_ba<BAs...>::one() {
	// TODO (HIGH) find how to implement this
	static variant_ba<BAs...> b;
	return b;
}

template<typename...BAs>
variant_ba<BAs...> variant_ba<BAs...>::operator&(const variant_ba& x) const {
	return std::visit(overloaded([]<typename T>(const T& l, const T& r) -> std::variant<BAs...> {
			auto res = l & r;
			std::variant<BAs...> v(res);
			return v;
		}, [](const auto&, const auto&) -> std::variant<BAs...> { throw std::logic_error("wrong types"); }), v, x.v);
}

template<typename...BAs>
variant_ba<BAs...> variant_ba<BAs...>::operator|(const variant_ba& x) const {
	return std::visit(overloaded([]<typename T>(const T& l, const T& r) -> std::variant<BAs...> {
			auto res = l | r;
			std::variant<BAs...> v(res);
			return v;
		}, [](const auto&, const auto&) -> std::variant<BAs...> { throw std::logic_error("wrong types"); }), v, x.v);
}

template<typename...BAs>
variant_ba<BAs...> variant_ba<BAs...>::operator^(const variant_ba& x) const {
	return std::visit(overloaded([]<typename T>(const T& l, const T& r) -> std::variant<BAs...> {
			auto res = l ^ r;
			std::variant<BAs...> v(res);
			return v;
		}, [](const auto&, const auto&) -> std::variant<BAs...> { throw std::logic_error("wrong types"); }), v, x.v);
}

template<typename...BAs>
variant_ba<BAs...> variant_ba<BAs...>::operator+(const variant_ba& x) const {
	return std::visit(overloaded([]<typename T>(const T& l, const T& r) -> std::variant<BAs...> {
			auto res = l + r;
			std::variant<BAs...> v(res);
			return v;
		}, [](const auto&, const auto&) -> std::variant<BAs...> { throw std::logic_error("wrong types"); }), v, x.v);
}

template<typename...BAs>
variant_ba<BAs...> variant_ba<BAs...>::operator~() const {
	return std::visit(overloaded([]<typename T>(const T& l) -> std::variant<BAs...> {
			auto res = ~l;
			std::variant<BAs...> v(res);
			return v;
		}, [](const auto&) -> std::variant<BAs...> { throw std::logic_error("wrong types"); }), v);
}

template<typename...BAs>
bool variant_ba<BAs...>::is_zero() const {
	return std::visit(overloaded([]<typename T>(const T& l) -> bool {
			return l.is_zero();
		}, [](const auto&) -> bool { throw std::logic_error("wrong types"); }), v);
}

template<typename...BAs>
bool variant_ba<BAs...>::is_one() const {
	return std::visit(overloaded([]<typename T>(const T& l) -> bool {
			return l.is_one();
		}, [](const auto&) -> bool { throw std::logic_error("wrong types"); }), v);
}