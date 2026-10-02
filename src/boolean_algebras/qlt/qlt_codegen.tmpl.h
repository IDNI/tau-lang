// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file qlt_codegen.tmpl.h
 * @brief How a qlt constant is spelled in generated C++.
 *
 * Included from qlt_descriptor.tmpl.h and nowhere else.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_CODEGEN_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_CODEGEN_TMPL_H__

#include <cstdio>
#include <sstream>
#include <string>

#include "boolean_algebras/qlt/qlt.h"

namespace idni::tau_lang {

// The C++ spelling of an endpoint of a qlt piece.
inline std::string qlt_endpoint_expr(const qlt_endpoint& e) {
	std::ostringstream ss;
	ss << "::idni::tau_lang::qlt_endpoint{";
	if (e.val.is_pos_inf())
		ss << "::idni::tau_lang::qlt_rational::make_pos_inf()";
	else if (e.val.is_neg_inf())
		ss << "::idni::tau_lang::qlt_rational::make_neg_inf()";
	else ss << "::idni::tau_lang::qlt_rational(" << e.val.p << ", "
		<< e.val.q << ")";
	ss << ", ::idni::tau_lang::qlt_bound::"
		<< (e.bound == qlt_bound::CLOSED ? "CLOSED" : "OPEN") << "}";
	return ss.str();
}

// A self-contained C++ expression of type tref: an IIFE that builds the exact
// set q and registers it through the BA's own constant pool, so the value
// reaching the generated program is a real qlt constant rather than an
// approximation of one.
inline std::string qlt_constant_expr(const qlt& q) {
	std::ostringstream ss;
	ss << "[]() -> ::idni::tref {\n"
	   << "\t\t\t\t::idni::tau_lang::qlt value;\n";
	for (const auto& p : q.pieces)
		ss << "\t\t\t\tvalue.pieces.push_back({ "
		   << qlt_endpoint_expr(p.lo) << ", "
		   << qlt_endpoint_expr(p.hi) << " });\n";
	ss << "\t\t\t\tusing node_t = ::idni::tau_lang::tau_pack::node_t;\n"
	   << "\t\t\t\t::idni::tau_lang::tree<node_t>::constant c = value;\n"
	   << "\t\t\t\t::idni::tref raw = "
	   << "::idni::tau_lang::ba_constants<node_t>::get(c, "
	   << "::idni::tau_lang::ba_descriptor<::idni::tau_lang::qlt, node_t>"
	   << "::type_tree());\n"
	   << "\t\t\t\treturn ::idni::tau_lang::tree<node_t>::get("
	   << "::idni::tau_lang::tree<node_t>::bf, raw);\n"
	   << "\t\t\t}()";
	return ss.str();
}

// The codegen_constant_expr capability: @p cst is a closed qlt constant; one
// with a named endpoint or an over-approximated value has no exact spelling.
template <NodeType node>
static std::optional<std::string> qlt_codegen_constant_expr(tref cst) {
	using tau = tree<node>;
	if (!tau::get(cst).is_ba_constant()) return std::nullopt;
	const qlt v = std::get<qlt>(tau::get(cst).get_ba_constant());
	if (v.inexact) return std::nullopt;
	for (const auto& p : v.pieces)
		if (p.lo.val.is_sym() || p.hi.val.is_sym()) return std::nullopt;
	return qlt_constant_expr(v);
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_CODEGEN_TMPL_H__
