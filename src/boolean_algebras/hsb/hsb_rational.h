// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file hsb_rational.h
 * @brief Exact arbitrary-precision integers and rationals for the hsb
 * coefficients and biases.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB__HSB_RATIONAL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB__HSB_RATIONAL_H__

#include <algorithm>
#include <bit>
#include <cassert>
#include <cmath>
#include <compare>
#include <concepts>
#include <cstdint>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "utility/hashing.h"
#include "boolean_algebras/exact_rational.h"

namespace idni::tau_lang {

// =============================================================================
/**
 * @brief Arbitrary-precision signed integer in sign-magnitude form.
 *
 * The magnitude is little-endian in base 2^32 with no leading zero limb, and
 * zero is the empty magnitude with a positive sign, so every value has one
 * representation and equality, ordering and hashing compare the limbs.
 */
// =============================================================================
class hsb_bigint {
public:
	/// Little-endian base-2^32 limbs.
	using limbs = std::vector<std::uint32_t>;

	/// @brief Zero.
	hsb_bigint() = default;

	/// @brief The integer @p v.
	template <std::integral I>
	hsb_bigint(I v) {
		if constexpr (std::is_signed_v<I>) {
			neg_ = v < 0;
			// -(v + 1) + 1 keeps the most negative value in range.
			std::uint64_t m = neg_
				? static_cast<std::uint64_t>(-(static_cast<std::int64_t>(v) + 1)) + 1
				: static_cast<std::uint64_t>(v);
			set_mag(m);
		} else set_mag(static_cast<std::uint64_t>(v));
	}

	/**
	 * @brief Reads a non-empty string of decimal digits.
	 * @return The value, or nullopt when @p s is empty or holds a non-digit.
	 */
	static std::optional<hsb_bigint> from_digits(std::string_view s) {
		if (s.empty()) return std::nullopt;
		hsb_bigint r;
		// Nine digits at a time: 10^9 < 2^32.
		size_t i = 0;
		while (i < s.size()) {
			size_t len = std::min<size_t>(9, s.size() - i);
			std::uint32_t chunk = 0, scale = 1;
			for (size_t k = 0; k < len; ++k) {
				char c = s[i + k];
				if (c < '0' || c > '9') return std::nullopt;
				chunk = chunk * 10 + static_cast<std::uint32_t>(c - '0');
				scale *= 10;
			}
			mul_small_add(r.mag_, scale, chunk);
			i += len;
		}
		return r;
	}

	/// @brief 10^@p k.
	static hsb_bigint pow10(size_t k) {
		hsb_bigint r(1);
		for (; k >= 9; k -= 9) mul_small_add(r.mag_, 1000000000u, 0);
		std::uint32_t s = 1;
		for (; k > 0; --k) s *= 10;
		mul_small_add(r.mag_, s, 0);
		return r;
	}

	/// @brief True iff the value is zero.
	bool is_zero() const noexcept { return mag_.empty(); }
	/// @brief -1, 0 or +1.
	int sign() const noexcept { return mag_.empty() ? 0 : (neg_ ? -1 : 1); }
	/// @brief True iff the value is odd.
	bool is_odd() const noexcept { return !mag_.empty() && (mag_[0] & 1u); }
	/// @brief The limbs of the magnitude.
	const limbs& magnitude() const noexcept { return mag_; }

	/// @brief The absolute value.
	hsb_bigint abs() const { hsb_bigint r = *this; r.neg_ = false; return r; }

	/// @brief The negated value.
	hsb_bigint operator-() const {
		hsb_bigint r = *this;
		if (!r.mag_.empty()) r.neg_ = !r.neg_;
		return r;
	}

	/// @brief The sum.
	friend hsb_bigint operator+(const hsb_bigint& a, const hsb_bigint& b) {
		hsb_bigint r;
		if (a.neg_ == b.neg_) {
			r.mag_ = mag_add(a.mag_, b.mag_);
			r.neg_ = a.neg_;
		} else if (mag_cmp(a.mag_, b.mag_) >= 0) {
			r.mag_ = mag_sub(a.mag_, b.mag_);
			r.neg_ = a.neg_;
		} else {
			r.mag_ = mag_sub(b.mag_, a.mag_);
			r.neg_ = b.neg_;
		}
		r.fix_zero();
		return r;
	}

	/// @brief The difference.
	friend hsb_bigint operator-(const hsb_bigint& a, const hsb_bigint& b) {
		return a + (-b);
	}

	/// @brief The product.
	friend hsb_bigint operator*(const hsb_bigint& a, const hsb_bigint& b) {
		hsb_bigint r;
		r.mag_ = mag_mul(a.mag_, b.mag_);
		r.neg_ = a.neg_ != b.neg_;
		r.fix_zero();
		return r;
	}

	/**
	 * @brief Truncating division: @p a = q * @p b + r with |r| < |b| and r
	 * of the sign of @p a.
	 * @pre @p b is not zero.
	 */
	static std::pair<hsb_bigint, hsb_bigint> divmod(const hsb_bigint& a,
		const hsb_bigint& b)
	{
		assert(!b.is_zero());
		hsb_bigint q, r;
		mag_divmod(a.mag_, b.mag_, q.mag_, r.mag_);
		q.neg_ = a.neg_ != b.neg_;
		r.neg_ = a.neg_;
		q.fix_zero();
		r.fix_zero();
		return { std::move(q), std::move(r) };
	}

	/// @brief The non-negative greatest common divisor; gcd(0, 0) is 0.
	static hsb_bigint gcd(hsb_bigint a, hsb_bigint b) {
		a.neg_ = b.neg_ = false;
		while (!b.is_zero()) {
			auto r = divmod(a, b).second;
			a = std::move(b);
			b = std::move(r);
		}
		return a;
	}

	/// @brief Equal values.
	friend bool operator==(const hsb_bigint&, const hsb_bigint&) = default;

	/// @brief Numeric order.
	friend std::strong_ordering operator<=>(const hsb_bigint& a,
		const hsb_bigint& b) noexcept
	{
		if (a.neg_ != b.neg_)
			return a.neg_ ? std::strong_ordering::less
				: std::strong_ordering::greater;
		int c = mag_cmp(a.mag_, b.mag_);
		if (a.neg_) c = -c;
		return c <=> 0;
	}

	/// @brief The decimal text, with a leading '-' when negative.
	std::string to_string() const {
		if (mag_.empty()) return "0";
		std::vector<std::uint32_t> chunks; // base 10^9, little-endian
		limbs m = mag_;
		while (!m.empty())
			chunks.push_back(div_small(m, 1000000000u));
		std::string s = neg_ ? "-" : "";
		s += std::to_string(chunks.back());
		for (size_t i = chunks.size() - 1; i-- > 0; ) {
			std::string part = std::to_string(chunks[i]);
			s.append(9 - part.size(), '0');
			s += part;
		}
		return s;
	}

	/// @brief The nearest double; ±inf beyond the double range.
	double to_double() const {
		double r = 0.0;
		for (size_t i = mag_.size(); i-- > 0; )
			r = r * 4294967296.0 + static_cast<double>(mag_[i]);
		return neg_ ? -r : r;
	}

	/// @brief Platform-independent content hash.
	std::uint64_t hash() const {
		std::uint64_t seed = neg_ ? 1 : 0;
		idni::hash_combine(seed, static_cast<std::uint64_t>(mag_.size()));
		for (auto l : mag_)
			idni::hash_combine(seed, static_cast<std::uint64_t>(l));
		return seed;
	}

private:
	bool neg_ = false;
	limbs mag_;

	void set_mag(std::uint64_t m) {
		mag_.clear();
		if (m) mag_.push_back(static_cast<std::uint32_t>(m));
		if (m >> 32) mag_.push_back(static_cast<std::uint32_t>(m >> 32));
	}

	void fix_zero() noexcept { if (mag_.empty()) neg_ = false; }

	static void trim(limbs& a) {
		while (!a.empty() && a.back() == 0) a.pop_back();
	}

	static int mag_cmp(const limbs& a, const limbs& b) noexcept {
		if (a.size() != b.size()) return a.size() < b.size() ? -1 : 1;
		for (size_t i = a.size(); i-- > 0; )
			if (a[i] != b[i]) return a[i] < b[i] ? -1 : 1;
		return 0;
	}

	static limbs mag_add(const limbs& a, const limbs& b) {
		const limbs& x = a.size() >= b.size() ? a : b;
		const limbs& y = a.size() >= b.size() ? b : a;
		limbs r(x.size() + 1);
		std::uint64_t carry = 0;
		for (size_t i = 0; i < x.size(); ++i) {
			std::uint64_t s = static_cast<std::uint64_t>(x[i])
				+ (i < y.size() ? y[i] : 0) + carry;
			r[i] = static_cast<std::uint32_t>(s);
			carry = s >> 32;
		}
		r[x.size()] = static_cast<std::uint32_t>(carry);
		trim(r);
		return r;
	}

	// @pre a >= b.
	static limbs mag_sub(const limbs& a, const limbs& b) {
		limbs r(a.size());
		std::int64_t borrow = 0;
		for (size_t i = 0; i < a.size(); ++i) {
			std::int64_t d = static_cast<std::int64_t>(a[i])
				- (i < b.size() ? b[i] : 0) - borrow;
			borrow = d < 0;
			r[i] = static_cast<std::uint32_t>(d);
		}
		trim(r);
		return r;
	}

	static limbs mag_mul(const limbs& a, const limbs& b) {
		if (a.empty() || b.empty()) return {};
		limbs r(a.size() + b.size());
		for (size_t i = 0; i < a.size(); ++i) {
			std::uint64_t carry = 0;
			for (size_t j = 0; j < b.size(); ++j) {
				std::uint64_t t = static_cast<std::uint64_t>(a[i]) * b[j]
					+ r[i + j] + carry;
				r[i + j] = static_cast<std::uint32_t>(t);
				carry = t >> 32;
			}
			r[i + b.size()] = static_cast<std::uint32_t>(carry);
		}
		trim(r);
		return r;
	}

	// a = a * m + add.
	static void mul_small_add(limbs& a, std::uint32_t m, std::uint32_t add) {
		std::uint64_t carry = add;
		for (auto& l : a) {
			std::uint64_t t = static_cast<std::uint64_t>(l) * m + carry;
			l = static_cast<std::uint32_t>(t);
			carry = t >> 32;
		}
		if (carry) a.push_back(static_cast<std::uint32_t>(carry));
		trim(a);
	}

	// a = a / d, returning the remainder. @pre d != 0.
	static std::uint32_t div_small(limbs& a, std::uint32_t d) {
		std::uint64_t rem = 0;
		for (size_t i = a.size(); i-- > 0; ) {
			std::uint64_t cur = (rem << 32) | a[i];
			a[i] = static_cast<std::uint32_t>(cur / d);
			rem = cur % d;
		}
		trim(a);
		return static_cast<std::uint32_t>(rem);
	}

	// Knuth's algorithm D (TAOCP vol. 2, 4.3.1). @pre v is not zero.
	static void mag_divmod(const limbs& u_in, const limbs& v_in,
		limbs& q, limbs& r)
	{
		if (mag_cmp(u_in, v_in) < 0) { q.clear(); r = u_in; return; }
		if (v_in.size() == 1) {
			q = u_in;
			std::uint32_t rem = div_small(q, v_in[0]);
			r.clear();
			if (rem) r.push_back(rem);
			return;
		}
		constexpr std::uint64_t base = std::uint64_t{1} << 32;
		const size_t n = v_in.size(), m = u_in.size() - n;
		const int s = std::countl_zero(v_in.back());
		// Shift both so the top limb of v has its high bit set.
		auto shl = [s](std::uint32_t hi, std::uint32_t lo) {
			return s == 0 ? hi : static_cast<std::uint32_t>(
				(hi << s) | (lo >> (32 - s)));
		};
		limbs v(n), u(m + n + 1);
		for (size_t i = n - 1; i > 0; --i) v[i] = shl(v_in[i], v_in[i - 1]);
		v[0] = shl(v_in[0], 0);
		u[m + n] = s == 0 ? 0 : u_in[m + n - 1] >> (32 - s);
		for (size_t i = m + n - 1; i > 0; --i) u[i] = shl(u_in[i], u_in[i - 1]);
		u[0] = shl(u_in[0], 0);

		q.assign(m + 1, 0);
		for (size_t j = m + 1; j-- > 0; ) {
			std::uint64_t num = (static_cast<std::uint64_t>(u[j + n]) << 32)
				| u[j + n - 1];
			std::uint64_t qhat = num / v[n - 1], rhat = num % v[n - 1];
			// qhat * v[n-2] is evaluated only once qhat < base, so it fits.
			while (qhat >= base || qhat * v[n - 2]
				> ((rhat << 32) | u[j + n - 2]))
			{
				--qhat;
				rhat += v[n - 1];
				if (rhat >= base) break;
			}
			std::int64_t borrow = 0;
			std::uint64_t carry = 0;
			for (size_t i = 0; i < n; ++i) {
				std::uint64_t p = qhat * v[i] + carry;
				carry = p >> 32;
				std::int64_t t = static_cast<std::int64_t>(u[i + j])
					- borrow - static_cast<std::int64_t>(p & 0xFFFFFFFFu);
				u[i + j] = static_cast<std::uint32_t>(t);
				borrow = t < 0;
			}
			std::int64_t t = static_cast<std::int64_t>(u[j + n]) - borrow
				- static_cast<std::int64_t>(carry);
			u[j + n] = static_cast<std::uint32_t>(t);
			q[j] = static_cast<std::uint32_t>(qhat);
			if (t < 0) { // qhat was one too large: add v back
				--q[j];
				std::uint64_t c = 0;
				for (size_t i = 0; i < n; ++i) {
					std::uint64_t sum = static_cast<std::uint64_t>(u[i + j])
						+ v[i] + c;
					u[i + j] = static_cast<std::uint32_t>(sum);
					c = sum >> 32;
				}
				u[j + n] = static_cast<std::uint32_t>(u[j + n] + c);
			}
		}
		r.assign(n, 0);
		for (size_t i = 0; i + 1 < n; ++i)
			r[i] = s == 0 ? u[i] : static_cast<std::uint32_t>(
				(u[i] >> s) | (u[i + 1] << (32 - s)));
		r[n - 1] = u[n - 1] >> s;
		trim(q);
		trim(r);
	}
};

// =============================================================================
/**
 * @brief Exact rational number p/q with arbitrary-precision p and q.
 *
 * Always reduced, with a positive denominator, so equal values share one
 * representation. There is no infinity and no NaN.
 */
// =============================================================================
class hsb_rational {
public:
	/// @brief Zero.
	hsb_rational() = default;

	/// @brief The integer @p v.
	template <std::integral I>
	hsb_rational(I v) : num_(v) {}

	/// @brief The integer @p v.
	hsb_rational(hsb_bigint v) : num_(std::move(v)) {}

	/// @brief @p n / @p d reduced; nullopt when @p d is zero.
	static std::optional<hsb_rational> fraction(hsb_bigint n, hsb_bigint d) {
		if (d.is_zero()) return std::nullopt;
		hsb_rational r;
		r.num_ = std::move(n);
		r.den_ = std::move(d);
		r.reduce();
		return r;
	}

	/// @brief The exact value of a finite double; nullopt for NaN or ±inf.
	static std::optional<hsb_rational> from_double(double v) {
		if (!std::isfinite(v)) return std::nullopt;
		int exp = 0;
		double frac = std::frexp(v, &exp); // v = frac * 2^exp, |frac| in [0.5, 1)
		// 53 bits make the mantissa an integer.
		auto mant = static_cast<std::int64_t>(std::ldexp(frac, 53));
		exp -= 53;
		hsb_bigint n(mant), d(1);
		hsb_bigint two_pow = pow2(static_cast<size_t>(exp < 0 ? -exp : exp));
		if (exp >= 0) n = n * two_pow; else d = two_pow;
		return fraction(std::move(n), std::move(d));
	}

	/**
	 * @brief Reads `[-]digits`, `[-]digits.digits` or `[-]digits/digits`.
	 * @return The exact value; nullopt for any other text or a zero
	 * denominator.
	 */
	static std::optional<hsb_rational> parse(std::string_view s) {
		bool neg = !s.empty() && s.front() == '-';
		if (neg) s.remove_prefix(1);
		std::optional<hsb_rational> r;
		if (auto slash = s.find('/'); slash != std::string_view::npos) {
			auto p = hsb_bigint::from_digits(s.substr(0, slash));
			auto q = hsb_bigint::from_digits(s.substr(slash + 1));
			if (!p || !q) return std::nullopt;
			r = fraction(std::move(*p), std::move(*q));
		} else if (auto dot = s.find('.'); dot != std::string_view::npos) {
			auto ip = s.substr(0, dot), fp = s.substr(dot + 1);
			auto i = hsb_bigint::from_digits(ip);
			auto f = hsb_bigint::from_digits(fp);
			if (!i || !f) return std::nullopt;
			auto scale = hsb_bigint::pow10(fp.size());
			r = fraction(*i * scale + *f, scale);
		} else if (auto i = hsb_bigint::from_digits(s)) {
			r = hsb_rational(std::move(*i));
		}
		if (r && neg) *r = -*r;
		return r;
	}

	/// @brief The numerator, of the sign of the value.
	const hsb_bigint& num() const noexcept { return num_; }
	/// @brief The denominator, always positive.
	const hsb_bigint& den() const noexcept { return den_; }

	/// @brief -1, 0 or +1.
	int sign() const noexcept { return num_.sign(); }
	/// @brief True iff the value is zero.
	bool is_zero() const noexcept { return num_.is_zero(); }
	/// @brief True iff the denominator is 1.
	bool is_integer() const noexcept { return den_ == hsb_bigint(1); }

	/// @brief The absolute value.
	hsb_rational abs() const { hsb_rational r = *this; r.num_ = r.num_.abs(); return r; }

	/// @brief The negated value.
	hsb_rational operator-() const { hsb_rational r = *this; r.num_ = -r.num_; return r; }

	/// @brief The sum.
	friend hsb_rational operator+(const hsb_rational& a, const hsb_rational& b) {
		return make(a.num_ * b.den_ + b.num_ * a.den_, a.den_ * b.den_);
	}
	/// @brief The difference.
	friend hsb_rational operator-(const hsb_rational& a, const hsb_rational& b) {
		return make(a.num_ * b.den_ - b.num_ * a.den_, a.den_ * b.den_);
	}
	/// @brief The product.
	friend hsb_rational operator*(const hsb_rational& a, const hsb_rational& b) {
		return make(a.num_ * b.num_, a.den_ * b.den_);
	}
	/// @brief The quotient. @pre @p b is not zero.
	friend hsb_rational operator/(const hsb_rational& a, const hsb_rational& b) {
		assert(!b.is_zero());
		return make(a.num_ * b.den_, a.den_ * b.num_);
	}
	/// @brief Adds @p o in place.
	hsb_rational& operator+=(const hsb_rational& o) { return *this = *this + o; }
	/// @brief Subtracts @p o in place.
	hsb_rational& operator-=(const hsb_rational& o) { return *this = *this - o; }

	/// @brief Equal values.
	friend bool operator==(const hsb_rational&, const hsb_rational&) = default;

	/// @brief Numeric order.
	friend std::strong_ordering operator<=>(const hsb_rational& a,
		const hsb_rational& b)
	{
		if (a.den_ == b.den_) return a.num_ <=> b.num_;
		return a.num_ * b.den_ <=> b.num_ * a.den_;
	}

	/**
	 * @brief The exact text that hsb_rational::parse reads back: an integer
	 * as `n`, a value with a terminating decimal expansion as `n.ddd`, and
	 * any other as `p/q`; negative values carry a leading '-'.
	 */
	std::string to_string() const {
		if (is_integer()) return num_.to_string();
		// The expansion terminates iff the denominator is 2^a * 5^b.
		size_t twos = 0, fives = 0;
		hsb_bigint d = den_;
		while (!d.is_odd()) { d = hsb_bigint::divmod(d, 2).first; ++twos; }
		while (true) {
			auto [q, r] = hsb_bigint::divmod(d, 5);
			if (!r.is_zero()) break;
			d = std::move(q);
			++fives;
		}
		if (d != hsb_bigint(1)) return num_.to_string() + "/" + den_.to_string();
		size_t k = std::max(twos, fives);
		// num / den = num * (10^k / den) / 10^k.
		hsb_bigint scaled = num_.abs()
			* hsb_bigint::divmod(hsb_bigint::pow10(k), den_).first;
		return exact_rational::place_decimal_point(scaled.to_string(), k,
			num_.sign() < 0);
	}

	/// @brief The text `p/q` (or `p` for an integer) for an SMT solver.
	std::string to_fraction_string() const {
		if (is_integer()) return num_.to_string();
		return num_.to_string() + "/" + den_.to_string();
	}

	/// @brief An approximation, for display and tests only.
	explicit operator double() const {
		return num_.to_double() / den_.to_double();
	}

	/// @brief Platform-independent content hash.
	std::uint64_t hash() const {
		std::uint64_t seed = num_.hash();
		idni::hash_combine(seed, den_.hash());
		return seed;
	}

private:
	hsb_bigint num_;
	hsb_bigint den_{1};

	static hsb_bigint pow2(size_t k) {
		hsb_bigint r(1), two(2);
		for (; k > 0; --k) r = r * two;
		return r;
	}

	static hsb_rational make(hsb_bigint n, hsb_bigint d) {
		hsb_rational r;
		r.num_ = std::move(n);
		r.den_ = std::move(d);
		r.reduce();
		return r;
	}

	void reduce() {
		if (den_.sign() < 0) { num_ = -num_; den_ = -den_; }
		if (num_.is_zero()) { den_ = 1; return; }
		auto g = hsb_bigint::gcd(num_, den_);
		if (g != hsb_bigint(1)) {
			num_ = hsb_bigint::divmod(num_, g).first;
			den_ = hsb_bigint::divmod(den_, g).first;
		}
	}
};

/// @brief Writes hsb_rational::to_string().
inline std::ostream& operator<<(std::ostream& os, const hsb_rational& r) {
	return os << r.to_string();
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB__HSB_RATIONAL_H__
