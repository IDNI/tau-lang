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

namespace idni::tau {

template<typename...BAs>
std::variant<BAs...> operator&(const std::variant<BAs...>& l, const std::variant<BAs...>& r) {
	return std::visit(
		[](const auto& l, const auto& r) -> std::variant<BAs...> {
			return l & r;
		}, l, r);
}

template<typename...BAs>
std::variant<BAs...> operator|(const std::variant<BAs...>& l, const std::variant<BAs...>& r) {
	return std::visit(
		[](const auto& l, const auto& r) -> std::variant<BAs...> {
			return l | r;
		}, l, r);
}

template<typename...BAs>
std::variant<BAs...> operator^(const std::variant<BAs...>& l, const std::variant<BAs...>& r) {
	return std::visit(
		[](const auto& l, const auto& r) -> std::variant<BAs...> {
			return l ^ r;
		}, l, r);
}

template<typename...BAs>
std::variant<BAs...> operator+(const std::variant<BAs...>& l, const std::variant<BAs...>& r) {
	return l ^ r;
}

template<typename...BAs>
std::variant<BAs...> operator~(const std::variant<BAs...>& l) {
	return std::visit(overloaded(
			[](const auto& l) -> std::variant<BAs...> {
				return ~l;
			}
		), l);
}

template<typename...BAs>
bool operator==(const std::variant<BAs...>& l, const bool& r) {
	return std::visit(overloaded(
			[&r](const auto& l) -> bool {
				return l == r;
			}
		), l);
}

template<typename...BAs>
bool operator==(const bool& l, const std::variant<BAs...>& r) {
	return r == l;
}

template<typename...BAs>
bool operator!=(const std::variant<BAs...>& l, const bool& r) {
	return !(l == r);
}

template<typename...BAs>
bool operator!=(const bool& l, const std::variant<BAs...>& r) {
	return r != l;
}

template<typename...BAs>
struct variant_ba {

	variant_ba(const std::variant<BAs...>& v) : v(v) {};
	variant_ba(const bool v) : v(v) {};

	static const variant_ba<BAs...>& zero() { static const variant_ba<BAs...> v(false); return v; }
	static const variant_ba<BAs...>& one() { static const variant_ba<BAs...> v(true); return v; }

	variant_ba<BAs...> operator&(const variant_ba<BAs...>& x) const {
		return std::visit(overloaded(
				[](const bool& l, const bool& r) -> variant_ba<BAs...> {
					return variant_ba<BAs...>(l & r);},
				[](const bool& l, const std::variant<BAs...>& r) -> variant_ba<BAs...> {
					return l ? variant_ba<BAs...>(r) : variant_ba<BAs...>(false);},
				[](const std::variant<BAs...>& l, const bool& r) -> variant_ba<BAs...> {
					return r ? variant_ba<BAs...>(l) : variant_ba<BAs...>(false);},
				[](const std::variant<BAs...>& l, const std::variant<BAs...>& r) -> variant_ba<BAs...> {
					return variant_ba<BAs...>(l & r);},
				[](const auto&, const auto&) -> variant_ba<BAs...> {
					throw std::logic_error("wrong types"); }
			), v, x.v);
	}

	variant_ba<BAs...> operator|(const variant_ba<BAs...>& x) const {
		return std::visit(overloaded(
				[](const bool& l, const bool& r) -> variant_ba<BAs...> {
					return variant_ba<BAs...>(l | r);},
				[](const bool& l, const std::variant<BAs...>& r) -> variant_ba<BAs...> {
					return l ? variant_ba<BAs...>(true) : variant_ba<BAs...>(r);},
				[](const std::variant<BAs...>& l, const bool& r) -> variant_ba<BAs...> {
					return r ? variant_ba<BAs...>(true) : variant_ba<BAs...>(l);},
				[](const std::variant<BAs...>& l, const std::variant<BAs...>& r) -> variant_ba<BAs...> {
					return variant_ba<BAs...>(l | r);},
				[](const auto&, const auto&) -> variant_ba<BAs...> {
					throw std::logic_error("wrong types"); }
			), v, x.v);
	}


	variant_ba<BAs...> operator^(const variant_ba<BAs...>& x) const {
		return std::visit(overloaded(
				[](const bool& l, const bool& r) -> variant_ba<BAs...> {
					return variant_ba<BAs...>(l ^ r);},
				[](const bool& l, const std::variant<BAs...>& r) -> variant_ba<BAs...> {
					return l ? variant_ba<BAs...>(~r) : variant_ba<BAs...>(r);},
				[](const std::variant<BAs...>& l, const bool& r) -> variant_ba<BAs...> {
					return r ? variant_ba<BAs...>(~l) : variant_ba<BAs...>(l);},
				[](const std::variant<BAs...>& l, const std::variant<BAs...>& r) -> variant_ba<BAs...> {
					return variant_ba<BAs...>(l ^ r);},
				[](const auto&, const auto&) -> variant_ba<BAs...> {
					throw std::logic_error("wrong types");}
			), v, x.v);
	}

	variant_ba<BAs...> operator+(const variant_ba<BAs...>& x) const {
		return *this ^ x;
	}

	variant_ba<BAs...> operator~() const {
		return std::visit(overloaded(
				[](const bool& l) -> variant_ba<BAs...> {
					return variant_ba<BAs...>(!l);},
				[](const std::variant<BAs...>& l) -> variant_ba<BAs...> {
					return variant_ba<BAs...>(~l);},
				[](const auto&) -> variant_ba<BAs...> {
					throw std::logic_error("wrong types"); }
			), v);
	}

	auto operator<=>(const variant_ba<BAs...>& x) const = default;

	bool is_zero() const {
		return std::visit(overloaded(
				[](const bool& l) -> bool {
					return !l;},
				[](const std::variant<BAs...>& l) -> bool {
					return l.is_zero();},
				[](const auto&) -> bool {
					throw std::logic_error("wrong types");}
			), v);
	}

	bool is_one() const {
		return std::visit(overloaded(
				[](const bool& l) -> bool {
					return !l;},
				[](const std::variant<BAs...>& l) -> bool {
					return l.is_one();},
				[](const auto&) -> bool {
					throw std::logic_error("wrong types");}
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

} // namespace idni::tau

#endif // __VARIANT_BA_H__