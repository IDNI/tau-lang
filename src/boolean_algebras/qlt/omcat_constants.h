// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Formula-level constant collection for Algorithm D / exocat reduction.
//
// Extracts the named rational constants appearing in qlt atoms of a
// formula.  These constants define the signature extension for
// enumerate_qlt_T1 / enumerate_qlt_T2: |T_k| grows with the number of
// distinct constants, so precise collection keeps the type spaces
// minimal.
//
// Depends on tau_tree.h (for tree walking) and core's omcat_types.h (for the
// rational type, which Algorithm D shares).  Reachable only with qlt in the pack,
// so it needs no guard of its own.

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__OMCAT_CONSTANTS_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__OMCAT_CONSTANTS_H__

#include <algorithm>
#include <climits>
#include <string>
#include <vector>

#include "boolean_algebras/qlt/qlt.h"
#include "ba_types.h"
#include "omcat_types.h"
#include "tau_tree.h"

namespace idni::tau_lang::omcat {

// Parse a rational-literal source string like "1/4", "-3/7", "0.25" into
// a rational.  Anything that is not an exact rational literal is an error.
inline result<rational> parse_rat_literal(const std::string& src) {
	result<rational> r;
	// Try "p/q" first.
	auto slash = src.find('/');
	if (slash != std::string::npos) {
		try {
			long long p = std::stoll(src.substr(0, slash));
			long long q = std::stoll(src.substr(slash + 1));
			if (q == 0)
				return r.with_error(code::invalid_argument,
					"rational parse failed for 'p/q', returning sentinel",
					{{label::value, src}});
			return r.with_value(rational(p, q));
		} catch (...) {
			return r.with_error(code::invalid_argument,
				"rational parse failed for 'p/q', returning sentinel",
				{{label::value, src}});
		}
	}
	// Try decimal "<int>.<frac>".
	auto dot = src.find('.');
	if (dot != std::string::npos) {
		try {
			std::string ipart = src.substr(0, dot);
			std::string fpart = src.substr(dot + 1);
			// 10^k must stay within long long: 19 or more fractional
			// digits used to overflow `denom` silently (signed overflow,
			// no exception) and yield a garbage rational.
			if (fpart.size() > 18)
				return r.with_error(code::invalid_argument,
					"rational parse: '" + src + "' has "
					+ std::to_string(fpart.size()) + " fractional digits, "
					"more than the 18 an exact rational literal supports; "
					"returning sentinel",
					{{label::value, src}});
			long long ival = ipart.empty() ? 0 : std::stoll(ipart);
			long long fval = fpart.empty() ? 0 : std::stoll(fpart);
			long long denom = 1;
			for (size_t i = 0; i < fpart.size(); ++i) denom *= 10;
			long long sign = (ipart.size() && ipart[0] == '-') ? -1 : 1;
			long long scaled = 0, num = 0;
#if defined(_MSC_VER) && !defined(__clang__)
			// MSVC has no __builtin_*_overflow; widen through 128-bit.
			omcat_int128_ scaled128 =
				(omcat_int128_) std::abs(ival) * denom;
			omcat_int128_ num128 = scaled128 + fval;
			num128 *= sign;
			if (num128 > LLONG_MAX || num128 < LLONG_MIN)
				return r.with_error(code::invalid_argument,
					"rational parse: '" + src + "' does not fit an "
					"exact rational literal; returning sentinel",
					{{label::value, src}});
			num = (long long) num128;
			(void)scaled;
#else
			if (__builtin_mul_overflow(std::abs(ival), denom, &scaled)
				|| __builtin_add_overflow(scaled, fval, &num)
				|| __builtin_mul_overflow(num, sign, &num))
			{
				return r.with_error(code::invalid_argument,
					"rational parse: '" + src + "' does not fit an "
					"exact rational literal; returning sentinel",
					{{label::value, src}});
			}
#endif
			return r.with_value(rational(num, denom));
		} catch (...) {
			return r.with_error(code::invalid_argument,
				"rational parse failed for decimal, returning sentinel",
				{{label::value, src}});
		}
	}
	// Plain integer.
	try {
		return r.with_value(rational(std::stoll(src), 1));
	} catch (...) {
		return r.with_error(code::invalid_argument,
			"rational parse failed for integer, returning sentinel",
			{{label::value, src}});
	}
}

// Walk the formula AST and gather every qlt-constant literal we find,
// returned sorted and deduplicated.
template <NodeType node>
inline result<std::vector<rational>> collect_qlt_constants(tref fm) {
	using tau = tree<node>;
	using tt = typename tau::traverser;
	result<std::vector<rational>> r;
	std::vector<rational> out;
	if (!fm) return r.with_value(std::move(out));

	for (tref c : tau::get(fm).select_all(is<node, tau::ba_constant>)) {
		const tau& t = tau::get(c);
		// get_ba_type_name() returns ":qlt" (with leading colon); use type ID.
		if (!ba_descriptor<qlt, node>::owns_type(t.get_ba_type())) continue;
		auto cv = t.get_ba_constant();
		if (std::holds_alternative<qlt>(cv)) {
			const qlt& qba = std::get<qlt>(cv);
			// Collect all finite rational endpoints across all pieces.
			for (const auto& piece : qba.pieces) {
				if (piece.lo.val.is_finite())
					out.push_back(rational(piece.lo.val.p, piece.lo.val.q));
				if (piece.hi.val.is_finite()
				    && !(piece.lo.val.is_finite() && piece.lo.val == piece.hi.val))
					out.push_back(rational(piece.hi.val.p, piece.hi.val.q));
			}
			continue;
		}
		// Fall back to source-string for uncompiled parse-time constants.
		if (tref src = tt(c) | tau::source | tt::ref; src) {
			TAU_TRY(rational val,
				parse_rat_literal(tau::get(src).get_string()));
			out.push_back(val);
		}
	}

	// Dedup + sort.
	std::sort(out.begin(), out.end(),
	    [](const rational& a, const rational& b) { return cmp(a, b) < 0; });
	out.erase(std::unique(out.begin(), out.end(),
	    [](const rational& a, const rational& b) { return cmp(a, b) == 0; }),
	    out.end());
	return r.with_value(std::move(out));
}

} // namespace idni::tau_lang::omcat

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__OMCAT_CONSTANTS_H__
