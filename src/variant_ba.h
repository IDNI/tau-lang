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
	variant_ba();
	variant_ba(const std::variant<BAs...>& v);
	variant_ba(const bool b);

	static const variant_ba& zero();
	static const variant_ba& one();

	variant_ba operator&(const variant_ba& x) const;
	variant_ba operator|(const variant_ba& x) const;
	variant_ba operator^(const variant_ba& x) const;
	variant_ba operator+(const variant_ba& x) const;
	variant_ba operator~() const;
	auto operator<=>(const variant_ba& x) const = default;

	bool is_zero() const;
	bool is_one() const;

	std::variant<BAs...> v;
};

template<typename...BAs>
std::ostream& operator<<(std::ostream& os, const variant_ba<BAs...>& b);

#endif // __VARIANT_BA_H__