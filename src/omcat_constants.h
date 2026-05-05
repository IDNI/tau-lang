// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Formula-level constant collection for Algorithm D / exocat reduction.
//
// Extracts the named rational constants appearing in qlt atoms of a
// formula.  These constants define the signature extension for
// enumerate_qlt_T1 / enumerate_qlt_T2: |T_k| grows with the number of
// distinct constants, so precise collection keeps the type spaces
// minimal.
//
// Depends on tau_tree.h (for tree walking) and omcat_types.h (for the Rat
// type).  Keeps the heavy tau-lang template machinery encapsulated.

#ifndef __IDNI__TAU__OMCAT_CONSTANTS_H__
#define __IDNI__TAU__OMCAT_CONSTANTS_H__

#include <algorithm>
#include <string>
#include <vector>

#include "boolean_algebras/qlt.h"
#include "ba_types.h"
#include "omcat_types.h"
#include "tau_tree.h"

namespace idni::tau_lang::omcat {

// Parse a rational-literal source string like "1/4", "-3/7", "0.25" into
// a Rat.  Returns {0, 0} if it doesn't look like a valid rational (the
// caller should check).
inline Rat parse_rat_literal(const std::string& src) {
	// Try "p/q" first.
	auto slash = src.find('/');
	if (slash != std::string::npos) {
		try {
			long long p = std::stoll(src.substr(0, slash));
			long long q = std::stoll(src.substr(slash + 1));
			if (q == 0) return Rat(0, 0);
			return Rat(p, q);
		} catch (...) {
			LOG_WARNING << "rational parse failed for 'p/q', returning sentinel";
			return Rat(0, 0);
		}
	}
	// Try decimal "<int>.<frac>".
	auto dot = src.find('.');
	if (dot != std::string::npos) {
		try {
			std::string ipart = src.substr(0, dot);
			std::string fpart = src.substr(dot + 1);
			long long ival = ipart.empty() ? 0 : std::stoll(ipart);
			long long fval = fpart.empty() ? 0 : std::stoll(fpart);
			long long denom = 1;
			for (size_t i = 0; i < fpart.size(); ++i) denom *= 10;
			long long sign = (ipart.size() && ipart[0] == '-') ? -1 : 1;
			long long num = sign * (std::abs(ival) * denom + fval);
			return Rat(num, denom);
		} catch (...) {
			LOG_WARNING << "rational parse failed for decimal, returning sentinel";
			return Rat(0, 0);
		}
	}
	// Plain integer.
	try {
		return Rat(std::stoll(src), 1);
	} catch (...) {
		LOG_WARNING << "rational parse failed for integer, returning sentinel";
		return Rat(0, 0);
	}
}

// Walk the formula AST and gather every qlt-constant literal we find,
// returned sorted and deduplicated.
template <NodeType node>
inline std::vector<Rat> collect_qlt_constants(tref fm) {
	using tau = tree<node>;
	using tt = typename tau::traverser;
	std::vector<Rat> out;
	if (!fm) return out;

	if constexpr (ba_variant_includes_v<qlt, typename tau::constant>) {
		for (tref c : tau::get(fm).select_all(is<node, tau::ba_constant>)) {
			const tau& t = tau::get(c);
			// get_ba_type_name() returns ":qlt" (with leading colon); use type ID.
			if (!is_omcat_type_family<node>(t.get_ba_type())) continue;
			auto cv = t.get_ba_constant();
			if (std::holds_alternative<qlt>(cv)) {
				const qlt& qba = std::get<qlt>(cv);
				// Collect all finite rational endpoints across all pieces.
				for (const auto& piece : qba.pieces) {
					if (piece.lo.val.is_finite())
						out.push_back(Rat(piece.lo.val.p, piece.lo.val.q));
					if (piece.hi.val.is_finite()
					    && !(piece.lo.val.is_finite() && piece.lo.val == piece.hi.val))
						out.push_back(Rat(piece.hi.val.p, piece.hi.val.q));
				}
				continue;
			}
			// Fall back to source-string for uncompiled parse-time constants.
			if (tref src = tt(c) | tau::source | tt::ref; src) {
				Rat r = parse_rat_literal(tau::get(src).get_string());
				if (r.q != 0) out.push_back(r);
			}
		}
	}

	// Dedup + sort.
	std::sort(out.begin(), out.end(),
	    [](const Rat& a, const Rat& b) { return cmp(a, b) < 0; });
	out.erase(std::unique(out.begin(), out.end(),
	    [](const Rat& a, const Rat& b) { return cmp(a, b) == 0; }),
	    out.end());
	return out;
}

} // namespace idni::tau_lang::omcat

#endif // __IDNI__TAU__OMCAT_CONSTANTS_H__
