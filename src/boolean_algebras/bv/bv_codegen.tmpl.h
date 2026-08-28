// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file bv_codegen.tmpl.h
 * @brief The concrete witness bv contributes to generated C++.
 *
 * Included from bv_descriptor.tmpl.h and nowhere else. Mirrors
 * qlt_codegen.tmpl.h: the emitter places a literal it never names, this file
 * decides which value and how it is spelled.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_CODEGEN_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_CODEGEN_TMPL_H__

#include <sstream>
#include <string>

#include "boolean_algebras/bv/bv_ba.h"

namespace idni::tau_lang {

// A self-contained C++ expression of type tref: an IIFE that builds the exact
// bitvector value as a real bv constant via the BA's own constant pool, so
// the generated program never re-derives the value from anything else.
template <NodeType node>
inline std::string bv_witness_expr(const bv& value, size_t width) {
	std::ostringstream ss;
	ss << "[]() -> ::idni::tref {\n"
	   << "\t\t\t\tusing node_t = ::idni::tau_lang::tau_pack::node_t;\n"
	   << "\t\t\t\t::idni::tau_lang::bv v = "
	   << "::idni::tau_lang::make_bitvector_value(" << width << ", \""
	   << value.getBitVectorValue(10) << "\", 10);\n"
	   << "\t\t\t\t::idni::tau_lang::tree<node_t>::constant c = v;\n"
	   << "\t\t\t\t::idni::tref witness_raw = "
	   << "::idni::tau_lang::ba_constants<node_t>::get(c, "
	   << "::idni::tau_lang::ba_descriptor<::idni::tau_lang::bv, node_t>"
	   << "::type_tree_for(" << width << "));\n"
	   << "\t\t\t\treturn ::idni::tau_lang::tree<node_t>::get("
	   << "::idni::tau_lang::tree<node_t>::bf, witness_raw);\n"
	   << "\t\t\t}()";
	return ss.str();
}

// A single edge "var = constant" is the hello_world/always_one class: the
// constant is the witness, extracted directly rather than solved for.
template <NodeType node>
static std::optional<bv> bv_single_equality_constant(tref var, tref conj) {
	using tau = tree<node>;
	const auto& t = tau::get(conj);
	if (!t.has_child() || t[0].value.nt != tau::bf_eq) return std::nullopt;
	tref lhs = t[0].first(), rhs = t[0].second();
	bool var_in_lhs = contains<node>(lhs, var);
	bool var_in_rhs = contains<node>(rhs, var);
	if (var_in_lhs == var_in_rhs) return std::nullopt;
	tref var_side = var_in_lhs ? lhs : rhs;
	tref const_side = var_in_lhs ? rhs : lhs;
	const trefs& side_fv = get_free_vars<node>(var_side);
	if (side_fv.size() != 1 || side_fv[0] != var) return std::nullopt;
	tref cst = tau::trim(const_side);
	if (!tau::get(cst).is_ba_constant()) return std::nullopt;
	return std::get<bv>(tau::get(cst).get_ba_constant());
}

// The codegen_witness capability: a self-contained C++ expression of type
// tref satisfying the conjunction of equality atoms on var. Richer
// conjunctions fall back to bv's own cvc5-backed solve; the ABA oracle has
// already proved the edge feasible, so a solver failure here is an
// inconsistency between the oracle and the translation to cvc5, not a
// legitimate "no witness" answer -- both are reported the same way (nullopt)
// since the emitter's caller already turns either into a build-time error.
template <NodeType node>
static std::optional<std::string> bv_codegen_witness(tref var, tref conj) {
	using tau = tree<node>;
	size_t width = get_bv_size<node>(tau::get(var).get_ba_type_tree());
	if (auto value = bv_single_equality_constant<node>(var, conj); value)
		return bv_witness_expr<node>(*value, width);
	auto sol = solve_bv<node>(conj);
	if (!sol) return std::nullopt;
	auto it = sol->find(tau::get(tau::bf, var));
	if (it == sol->end()) return std::nullopt;
	tref cst = tau::trim(it->second);
	if (!tau::get(cst).is_ba_constant()) return std::nullopt;
	return bv_witness_expr<node>(std::get<bv>(tau::get(cst).get_ba_constant()), width);
}

// The codegen_constant_expr capability: @p cst is already a trimmed, known bitvector value, so this only extracts and spells it.
template <NodeType node>
static std::optional<std::string> bv_codegen_constant_expr(tref cst) {
	using tau = tree<node>;
	if (!tau::get(cst).is_ba_constant()) return std::nullopt;
	size_t width = get_bv_size<node>(tau::get(cst).get_ba_type_tree());
	return bv_witness_expr<node>(std::get<bv>(tau::get(cst).get_ba_constant()), width);
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_CODEGEN_TMPL_H__
