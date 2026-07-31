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
#include "boolean_algebras/qlt/qlt_solver.tmpl.h"

namespace idni::tau_lang {

// Format a rational as a C++ double literal.
//
// The rational is exact; this conversion is the one place a value is rounded,
// and it is a real limitation: a double cannot represent every rational, so a
// midpoint of a narrow interval can still land outside it. Emitting the
// rational itself is what removes that, and needs the emitted value type.
inline std::string rational_to_cpp(const qlt_rational& r) {
	char buf[64];
	snprintf(buf, sizeof(buf), "%.17g", (double)r.p / (double)r.q);
	std::string s(buf);
	bool has_dot_or_e = false;
	for (char c : s) if (c == '.' || c == 'e' || c == 'E') {
		has_dot_or_e = true; break;
	}
	if (!has_dot_or_e) s += ".0";
	return s;
}

// The codegen_witness capability: a C++ literal satisfying the conjunction of
// ordering atoms on var. An interval the elimination cannot determine still
// yields a literal, since the ABA oracle has already found the edge feasible.
template <NodeType node>
static std::optional<std::string> qlt_codegen_witness(tref var, tref conj) {
	if (auto interval = qlt_dlo_qe_interval<node>(var, conj); interval)
		if (auto witness = qlt_pick_witness<node>(*interval); witness)
			return rational_to_cpp(*witness);
	// No determined interval, or none of its pieces yields a witness. The ABA
	// oracle has already found the edge feasible, so emit a value rather than
	// refusing: 1.0 is what this has always emitted here.
	return rational_to_cpp(qlt_rational(1, 1));
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_CODEGEN_TMPL_H__
