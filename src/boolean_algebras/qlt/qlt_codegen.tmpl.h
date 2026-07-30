// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file qlt_codegen.tmpl.h
 * @brief The concrete witness qlt contributes to generated C++.
 *
 * Included from qlt_descriptor.tmpl.h and nowhere else. Everything about how a
 * rational reaches generated source -- which value in the interval, and how it
 * is spelled -- is decided here rather than by the emitter.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_CODEGEN_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_CODEGEN_TMPL_H__

#include <cstdio>
#include <string>

#include "boolean_algebras/qlt/qlt.h"
#include "boolean_algebras/qlt/qlt_qe.tmpl.h"

namespace idni::tau_lang {

// Pick a concrete double witness from a non-empty qlt interval.
inline double witness_from_qlt_interval(const qlt& interval) {
	if (interval.is_empty()) return 0.0;
	const auto& piece = interval.pieces[0];
	const auto& lo = piece.lo.val;
	const auto& hi = piece.hi.val;
	if (lo.is_neg_inf() && hi.is_pos_inf()) return 0.0;
	if (lo.is_neg_inf()) {
		if (hi.is_sym()) return -1.0;
		return (double)hi.p / (double)hi.q - 1.0;
	}
	if (hi.is_pos_inf()) {
		if (lo.is_sym()) return 1.0;
		return (double)lo.p / (double)lo.q + 1.0;
	}
	// Bounded: midpoint
	if (lo.is_sym() || hi.is_sym()) return 0.0;
	double l = (double)lo.p / (double)lo.q;
	double h = (double)hi.p / (double)hi.q;
	return (l + h) / 2.0;
}

// Format a double as a C++ literal with full precision.
inline std::string double_to_cpp(double v) {
	char buf[64];
	snprintf(buf, sizeof(buf), "%.17g", v);
	// Ensure the literal looks like a floating-point constant
	std::string s(buf);
	bool has_dot_or_e = false;
	for (char c : s) if (c == '.' || c == 'e' || c == 'E') { has_dot_or_e = true; break; }
	if (!has_dot_or_e) s += ".0";
	return s;
}

// The codegen_witness capability: a C++ literal satisfying the conjunction of
// ordering atoms on var. An interval the elimination cannot determine still
// yields a literal, since the ABA oracle has already found the edge feasible.
template <NodeType node>
static std::optional<std::string> qlt_codegen_witness(tref var, tref conj) {
	auto interval = qlt_dlo_qe_interval<node>(var, conj);
	return double_to_cpp(interval
		? witness_from_qlt_interval(*interval) : 1.0);
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_CODEGEN_TMPL_H__
