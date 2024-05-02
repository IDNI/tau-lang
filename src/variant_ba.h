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

#ifndef __VARIANT_BA_H__
#define __VARIANT_BA_H__

#include <variant>
#include <ostream>

#include "utils.h"

template<typename...BAs>
struct variant_ba {

	variant_ba(const std::variant<BAs...>& v) : v(v) {};
	variant_ba(const bool v) : v(v) {};

	static const variant_ba<BAs...>& zero() { static const variant_ba<BAs...> v(false); return v; }
	static const variant_ba<BAs...>& one() { static const variant_ba<BAs...> v(true); return v; }

	variant_ba<BAs...> operator&(const variant_ba<BAs...>& x) const {
		return std::visit(overloaded(
				[]<typename T>(const bool& l, const bool& r) -> variant_ba<BAs...> {
					auto res = l & r;
					variant_ba<BAs...> v(res);
					return v;},
				[]<typename T>(const bool& l, const std::variant<T>& r) -> variant_ba<BAs...> {
					if (l) {
						variant_ba<BAs...> v(r);
						return v;
					}
					variant_ba<BAs...> v(false);
					return v;},
				[]<typename T>(const std::variant<T>& l, const bool& r) -> variant_ba<BAs...> {
					if (r) {
						variant_ba<BAs...> v(l);
						return v;
					}
					variant_ba<BAs...> v(false);
					return v;},
				[]<typename T>(const std::variant<T>& l, const std::variant<T>& r) -> variant_ba<BAs...> {
					return std::visit(overloaded(
							[]<typename B>(const B& l, const B& r) -> std::variant<B> {
								auto res = l & r;
								std::variant<B> v(res);
								return v;}
						), l, r);},
				[](const auto&, const auto&) -> variant_ba<BAs...> {
					throw std::logic_error("wrong types"); }
			), v, x.v);
	}

	variant_ba<BAs...> operator|(const variant_ba<BAs...>& x) const {
		return std::visit(overloaded(
				[]<typename T>(const bool& l, const bool& r) -> variant_ba<BAs...> {
					auto res = l | r;
					variant_ba<BAs...> v(res);
					return v;},
				[]<typename T>(const bool& l, const std::variant<T>& r) -> variant_ba<BAs...> {
					if (l) {
						variant_ba<BAs...> v(true);
						return v;
					}
					variant_ba<BAs...> v(r);
					return v;},
				[]<typename T>(const std::variant<T>& l, const bool& r) -> variant_ba<BAs...> {
					if (r) {
						variant_ba<BAs...> v(true);
						return v;
					}
					variant_ba<BAs...> v(l);
					return v;},
				[]<typename T>(const std::variant<T>& l, const std::variant<T>& r) -> variant_ba<BAs...> {
					return std::visit(overloaded(
							[]<typename B>(const B& l, const B& r) -> std::variant<B> {
								auto res = l | r;
								std::variant<B> v(res);
								return v;}
						), l, r);},
				[](const auto&, const auto&) -> variant_ba<BAs...> {
					throw std::logic_error("wrong types"); }
			), v, x.v);
	}


	variant_ba<BAs...> operator^(const variant_ba<BAs...>& x) const {
		return std::visit(overloaded(
				[]<typename T>(const bool& l, const bool& r) -> variant_ba<BAs...> {
					auto res = l ^ r;
					variant_ba<BAs...> v(res);
					return v;},
				[]<typename T>(const bool& l, const std::variant<T>& r) -> variant_ba<BAs...> {
					if (l) {
						auto res = std::visit(overloaded(
								[]<typename B>(const B& r) -> B {
									return ~r;
								}
							), r);
						variant_ba<BAs...> v(res);
						return v;
					}
					variant_ba<BAs...> v(r);
					return v;},
				[]<typename T>(const std::variant<T>& l, const bool& r) -> variant_ba<BAs...> {
					if (r) {
						auto res = std::visit(overloaded(
								[]<typename B>(const B& l) -> B {
									return ~l;
								}
							), l);
						variant_ba<BAs...> v(res);
						return v;
					}
					variant_ba<BAs...> v(l);
					return v;},
				[]<typename T>(const std::variant<T>& l, const std::variant<T>& r) -> variant_ba<BAs...> {
					return std::visit(overloaded(
							[]<typename B>(const B& l, const B& r) -> std::variant<B> {
								auto res = l ^ r;
								std::variant<B> v(res);
								return v;}
						), l, r);},
				[](const auto&, const auto&) -> variant_ba<BAs...> {
					throw std::logic_error("wrong types"); }
			), v, x.v);
	}

	variant_ba<BAs...> operator+(const variant_ba<BAs...>& x) const {
		return *this ^ x;
	}

	variant_ba<BAs...> operator~() const {
		return std::visit(overloaded(
				[]<typename T>(const bool& l) -> variant_ba<BAs...> {
					variant_ba<BAs...> v(!l);
					return v;},
				[]<typename T>(const std::variant<T>& l) -> variant_ba<BAs...> {
					return std::visit(overloaded(
							[]<typename B>(const B& l) -> std::variant<B> {
								auto res = ~l;
								std::variant<B> v(res);
								return v;}
						), l);},
				[](const auto&) -> variant_ba<BAs...> {
					throw std::logic_error("wrong types"); }
			), v);
	}

	auto operator<=>(const variant_ba<BAs...>& x) const = default;

	bool is_zero() const {
		return std::visit(overloaded(
				[]<typename T>(const bool& l) -> bool {
					return !l;
				},
				[]<typename T>(const std::variant<T>& l) -> bool {
					return std::visit(overloaded(
							[]<typename B>(const B& l) -> bool {
								return l.is_zero();
							}
						), l);},
				[](const auto&) -> variant_ba<BAs...> {
					throw std::logic_error("wrong types");}
			), v);
	}

	bool is_one() const {
		return std::visit(overloaded(
				[]<typename T>(const bool& l) -> bool {
					return l;
				},
				[]<typename T>(const std::variant<T>& l) -> bool {
					return std::visit(overloaded(
							[]<typename B>(const B& l) -> bool {
								return l.is_one();
							}
						), l);
				}
			), v);
	}

	std::variant<bool, std::variant<BAs...>> v;
};

template<typename...BAs>
bool operator==(const variant_ba<BAs...>& l, const bool& r) {
	if (std::holds_alternative<bool>(l.v))
		return std::get<bool>(l.v) == r;
	if (r) 	return std::visit(overloaded(
			[](const auto& l) -> bool {
				return l.is_one();
			}
		), std::get<std::variant<BAs...>>(l.v));
	return std::visit(overloaded(
			[](const auto& l) -> bool {
				return l.is_zero();
			}
		), std::get<std::variant<BAs...>>(l.v));
}

template<typename...BAs>
bool operator==(const bool& l, const variant_ba<BAs...>& r) {
	return r == l;
}

template<typename...BAs>
bool operator!=(const variant_ba<BAs...>& l, const bool& r) {
	return !(l == r);
}

template<typename...BAs>
bool operator!=(const bool& l, const variant_ba<BAs...>& r) {
	return r != l;
}

template<typename...BAs>
std::ostream& operator<<(std::ostream& os, const variant_ba<BAs...>& b) {
	return std::visit(overloaded(
			[&os]<typename T>(const bool& l) -> std::ostream& {
				return os << (l ? 1 : 0);
			},
			[&os]<typename T>(const std::variant<T>& l) -> std::ostream& {
				return std::visit(overloaded(
						[&os]<typename B>(const B& l) -> std::ostream& {
							return os << l;
						}
					), l);},
			[&os](const auto&) -> std::ostream& {
				throw std::logic_error("wrong types");}
		), b.v);
}

#endif // __VARIANT_BA_H__