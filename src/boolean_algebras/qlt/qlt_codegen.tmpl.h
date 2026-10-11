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
#include <sstream>
#include <string>

#include "boolean_algebras/qlt/qlt.h"
#include "boolean_algebras/qlt/qlt_qe.tmpl.h"
#include "boolean_algebras/qlt/qlt_solver.tmpl.h"

#include <limits>
#include <string>

namespace idni::tau_lang {

// A long long spelled as a C++ constant expression that compiles without a
// warning: the least value has no literal, since its magnitude does not fit.
inline std::string qlt_ll_literal(long long v) {
	if (v == std::numeric_limits<long long>::min())
		return "(-9223372036854775807LL - 1)";
	return std::to_string(v) + "LL";
}

// A self-contained C++ expression of type tref: an IIFE that builds the exact
// rational r as a qlt singleton and registers it through the BA's own
// constant pool, so the value reaching the generated program is a real qlt
// constant rather than a double approximation of one.
template <NodeType node>
inline std::string qlt_witness_expr(const qlt_rational& r) {
	std::ostringstream ss;
	ss << "[]() -> ::idni::tref {\n"
	   << "\t\t\t\t::idni::tau_lang::qlt_piece p;\n"
	   << "\t\t\t\tp.lo = ::idni::tau_lang::qlt_endpoint{"
	   << "::idni::tau_lang::qlt_rational("
	   << qlt_ll_literal(r.p) << ", " << qlt_ll_literal(r.q) << "), "
	   << "::idni::tau_lang::qlt_bound::CLOSED};\n"
	   << "\t\t\t\tp.hi = p.lo;\n"
	   << "\t\t\t\t::idni::tau_lang::qlt singleton{{p}};\n"
	   << "\t\t\t\tusing node_t = ::idni::tau_lang::tau_pack::node_t;\n"
	   << "\t\t\t\t::idni::tau_lang::tree<node_t>::constant c = singleton;\n"
	   << "\t\t\t\t::idni::tref witness_raw = "
	   << "::idni::tau_lang::ba_constants<node_t>::get(c, "
	   << "::idni::tau_lang::ba_descriptor<::idni::tau_lang::qlt, node_t>"
	   << "::type_tree());\n"
	   << "\t\t\t\treturn ::idni::tau_lang::tree<node_t>::get("
	   << "::idni::tau_lang::tree<node_t>::bf, witness_raw);\n"
	   << "\t\t\t}()";
	return ss.str();
}

// The codegen_witness capability: a self-contained C++ expression of type
// tref satisfying the conjunction of ordering atoms on var; nullopt when the
// elimination cannot determine the interval or none of its pieces holds a
// rational the representation fits, which the emitter reports as
// unsupported.
template <NodeType node>
static std::optional<std::string> qlt_codegen_witness(tref var, tref conj) {
	auto interval = qlt_dlo_qe_interval<node>(var, conj);
	if (!interval) return std::nullopt;
	auto witness = qlt_pick_witness<node>(*interval);
	if (!witness) return std::nullopt;
	return qlt_witness_expr<node>(*witness);
}

// The codegen_constant_expr capability: spells the rational of @p cst when it
// is a trimmed qlt constant holding one closed, finite point; nullopt
// otherwise.
template <NodeType node>
static result<std::optional<std::string>> qlt_codegen_constant_expr(tref cst) {
	using tau = tree<node>;
	result<std::optional<std::string>> r;
	if (!tau::get(cst).is_ba_constant()) return r.with_value(std::nullopt);
	qlt v = std::get<qlt>(tau::get(cst).get_ba_constant());
	if (v.pieces.size() != 1) return r.with_value(std::nullopt);
	const auto& p = v.pieces[0];
	if (p.lo.bound != qlt_bound::CLOSED || p.hi.bound != qlt_bound::CLOSED
	    || p.lo.val != p.hi.val || !p.lo.val.is_finite())
		return r.with_value(std::nullopt);
	return r.with_value(qlt_witness_expr<node>(p.lo.val));
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_CODEGEN_TMPL_H__
