// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file exact_rational.h
 * @brief The exact arithmetic the rational endpoints of qint and qlt share:
 * a 128-bit intermediate, the reduction of a quotient to a 64-bit fraction,
 * the reader of a decimal number or a fraction of two, and the placement of
 * the point in a terminating decimal, which hsb prints with too.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__EXACT_RATIONAL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__EXACT_RATIONAL_H__

#include <cstddef>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#if defined(_MSC_VER)
#	include <__msvc_int128.hpp>
#endif

namespace idni::tau_lang::exact_rational {

/// A 128-bit signed integer: the product of two 64-bit parts is exact in it.
#if defined(_MSC_VER)
using int128 = std::_Signed128;
#else
__extension__ typedef __int128 int128;
#endif

/// |v|; @p v must not be the least int128.
inline int128 abs(int128 v) { return v < 0 ? -v : v; }

/// gcd(|a|, |b|), or 1 when both are 0, so it is always a safe divisor.
inline int128 gcd(int128 a, int128 b) {
	a = abs(a), b = abs(b);
	while (b) { int128 t = a % b; a = b; b = t; }
	return a ? a : 1;
}

/// Whether @p v fits a long long.
inline bool fits(int128 v) {
	return v >= std::numeric_limits<long long>::min()
		&& v <= std::numeric_limits<long long>::max();
}

/// A reduced fraction p/q with q > 0.
struct fraction { long long p = 0, q = 1; };

/// num/den reduced, the sign on the numerator; nullopt when @p den is 0 or
/// the reduced value does not fit a 64-bit numerator and denominator.
inline std::optional<fraction> reduce(int128 num, int128 den) {
	if (den == 0) return std::nullopt;
	if (den < 0) num = -num, den = -den;
	int128 g = gcd(num, den);
	num /= g, den /= g;
	if (!fits(num) || !fits(den)) return std::nullopt;
	return fraction{ (long long) num, (long long) den };
}

/// @p s without its leading and trailing blanks.
inline std::string_view trim(std::string_view s) {
	constexpr std::string_view blanks = " \t\n\r";
	auto first = s.find_first_not_of(blanks);
	if (first == std::string_view::npos) return {};
	return s.substr(first, s.find_last_not_of(blanks) - first + 1);
}

/// A decimal number with an optional sign, fraction and exponent, exactly:
/// "12", "+3", "-1.5", ".5", "5.", "2e-3", "1.25E+2"; nullopt for any other
/// text or a value that does not fit.
inline std::optional<fraction> parse_decimal(std::string_view t) {
	size_t i = 0;
	bool negative = false;
	if (i < t.size() && (t[i] == '+' || t[i] == '-'))
		negative = t[i++] == '-';
	int128 mant = 0;
	int scale = 0; // value = mant * 10^(exp - scale)
	bool digits = false, dot = false;
	const int128 cap = (int128) 1 << 100;
	for (; i < t.size(); ++i) {
		const char c = t[i];
		if (c == '.') {
			if (dot) return std::nullopt;
			dot = true;
		} else if (c >= '0' && c <= '9') {
			digits = true;
			mant = mant * 10 + (c - '0');
			if (mant > cap) return std::nullopt;
			if (dot) ++scale;
		} else break;
	}
	if (!digits) return std::nullopt;
	long long exp = 0;
	if (i < t.size() && (t[i] == 'e' || t[i] == 'E')) {
		++i;
		bool eneg = false;
		if (i < t.size() && (t[i] == '+' || t[i] == '-'))
			eneg = t[i++] == '-';
		if (i == t.size()) return std::nullopt;
		for (; i < t.size(); ++i) {
			if (t[i] < '0' || t[i] > '9') return std::nullopt;
			exp = exp * 10 + (t[i] - '0');
			if (exp > 40) return std::nullopt;
		}
		if (eneg) exp = -exp;
	}
	if (i != t.size()) return std::nullopt;
	long long shift = exp - scale;
	if (shift > 38 || shift < -38) return std::nullopt;
	int128 num = negative ? -mant : mant, den = 1;
	for (; shift > 0; --shift) {
		if (abs(num) > cap) return std::nullopt;
		num *= 10;
	}
	for (; shift < 0; ++shift) den *= 10;
	return reduce(num, den);
}

/// The two sides of the '/' of @p t, each trimmed; nullopt without a '/'.
inline std::optional<std::pair<std::string_view, std::string_view>>
split_fraction(std::string_view t)
{
	auto slash = t.find('/');
	if (slash == std::string_view::npos) return std::nullopt;
	return std::make_pair(trim(t.substr(0, slash)), trim(t.substr(slash + 1)));
}

/// A decimal number (parse_decimal) or a fraction a/b of two, after the
/// blanks around @p s are trimmed; nullopt for any other text, a zero
/// denominator or a value that does not fit.
inline std::optional<fraction> parse_number(std::string_view s) {
	auto t = trim(s);
	auto sides = split_fraction(t);
	if (!sides) return parse_decimal(t);
	auto a = parse_decimal(sides->first), b = parse_decimal(sides->second);
	if (!a || !b || b->p == 0) return std::nullopt;
	return reduce((int128) a->p * b->q, (int128) a->q * b->p);
}

/// The terminating decimal whose digits, without the point, are @p digits
/// and which has @p places digits after the point; negative when @p negative.
inline std::string place_decimal_point(std::string digits, size_t places,
	bool negative)
{
	if (digits.size() <= places)
		digits.insert(0, places + 1 - digits.size(), '0');
	digits.insert(digits.end() - (std::ptrdiff_t) places, '.');
	return negative ? "-" + digits : digits;
}

} // namespace idni::tau_lang::exact_rational

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__EXACT_RATIONAL_H__
