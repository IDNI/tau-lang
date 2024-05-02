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

// TODO (LOW) move to a proper place and unify with that of nso_rr.h
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

template<typename...BAs>
struct variant_ba {

	variant_ba(const std::variant<BAs...>& v) : v(v) {};
	variant_ba(const bool v) : v(v) {};

	static const variant_ba& zero() { static const variant_ba v(false); return v; }
	static const variant_ba& one() { static const variant_ba v(true); return v; }

	variant_ba<BAs...> operator&(const variant_ba<BAs...>& x) const {
		return std::visit(overloaded(
				[]<typename T>(const bool& l, const bool& r) -> variant_ba<BAs...> {
					auto res = l & r;
					variant_ba<BAs...> v(res);
					return v;},
				[]<typename T>(const bool& l, const T& r) -> variant_ba<BAs...> {
					if (l) {
						variant_ba<BAs...> v(r);
						return v;
					}
					variant_ba<BAs...> v(false);
					return v;},
				[]<typename T>(const T& l, const bool& r) -> variant_ba<BAs...> {
					if (r) {
						variant_ba<BAs...> v(l);
						return v;
					}
					variant_ba<BAs...> v(false);
					return v;},
				[]<typename T>(const T& l, const T& r) -> variant_ba<BAs...> {
					auto res = l & r;
					variant_ba<BAs...> v(res);
					return v;},
				[](const auto&, const auto&) -> std::variant<BAs...> {
					throw std::logic_error("wrong types"); }
			), v, x.v);
	}

	variant_ba operator|(const variant_ba& x) const {
		return std::visit(overloaded(
				[]<typename T>(const bool& l, const bool& r) -> variant_ba<BAs...> {
					auto res = l | r;
					variant_ba<BAs...> v(res);
					return v;},
				[]<typename T>(const bool& l, const T& r) -> variant_ba<BAs...> {
					if (l) {
						variant_ba<BAs...> v(true);
						return v;
					}
					variant_ba<BAs...> v(r);
					return v;},
				[]<typename T>(const T& l, const bool& r) -> variant_ba<BAs...> {
					if (r) {
						variant_ba<BAs...> v(true);
						return v;
					}
					variant_ba<BAs...> v(l);
					return v;},
				[]<typename T>(const T& l, const T& r) -> variant_ba<BAs...> {
					auto res = l | r;
					variant_ba<BAs...> v(res);
					return v;},
				[](const auto&, const auto&) -> std::variant<BAs...> {
					throw std::logic_error("wrong types"); }
			), v, x.v);
	}


	variant_ba operator^(const variant_ba& x) const {
		return std::visit(overloaded(
				[]<typename T>(const bool& l, const bool& r) -> variant_ba<BAs...> {
					auto res = l ^ r;
					variant_ba<BAs...> v(res);
					return v;},
				[]<typename T>(const bool& l, const T& r) -> variant_ba<BAs...> {
					if (l) {
						variant_ba<BAs...> v(~r);
						return v;
					}
					variant_ba<BAs...> v(r);
					return v;},
				[]<typename T>(const T& l, const bool& r) -> variant_ba<BAs...> {
					if (r) {
						variant_ba<BAs...> v(~l);
						return v;
					}
					variant_ba<BAs...> v(l);
					return v;},
				[]<typename T>(const T& l, const T& r) -> variant_ba<BAs...> {
					auto res = l ^ r;
					variant_ba<BAs...> v(res);
					return v;},
				[](const auto&, const auto&) -> std::variant<BAs...> {
					throw std::logic_error("wrong types"); }
			), v, x.v);
	}

	variant_ba operator+(const variant_ba& x) const {
		return *this ^ x;
	}

	variant_ba operator~() const {
		return std::visit(overloaded(
				[]<typename T>(const bool& l) -> variant_ba<BAs...> {
					variant_ba<BAs...> v(!l);
					return v;},
				[]<typename T>(const T& l) -> variant_ba<BAs...> {
					variant_ba<BAs...> v(~l);
					return v;},
				[](const auto&) -> std::variant<BAs...> {
					throw std::logic_error("wrong types"); }
			), v);
	}

	auto operator<=>(const variant_ba& x) const = default;

	bool is_zero() const {
		return std::visit(overloaded(
				[]<typename T>(const bool& l) -> bool {
					return !l;
				},
				[]<typename T>(const T& l) -> bool {
					return l.is_zero();
				}
			), v);
	}

	bool is_one() const {
		return std::visit(overloaded(
				[]<typename T>(const bool& l) -> bool {
					return l;
				},
				[]<typename T>(const T& l) -> bool {
					return l.is_one();
				}
			), v);
	}

	std::variant<bool, std::variant<BAs...>> v;
};

template<typename...BAs>
std::ostream& operator<<(std::ostream& os, const variant_ba<BAs...>& b) {
	return std::visit(overloaded(
			[&os]<typename T>(const bool& l) -> std::ostream& {
				return os << (l ? 1 : 0);
			},
			[&os]<typename T>(const T& l) -> std::ostream& {
				return os << l;
			}
		), b.v);
}

#endif // __VARIANT_BA_H__