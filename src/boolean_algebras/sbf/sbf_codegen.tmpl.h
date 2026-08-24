// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file sbf_codegen.tmpl.h
 * @brief The concrete constant sbf contributes to generated C++.
 *
 * Included from sbf_descriptor.tmpl.h and nowhere else. sbf owns no
 * codegen_witness -- every sbf atom lands on witness_template, solved at
 * runtime -- so this file supplies codegen_constant_expr alone: rebuilding a
 * ground sbf constant's BDD as a self-contained C++ expression rather than
 * ever round-tripping it through sbf's own parser at artifact startup.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__SBF__SBF_CODEGEN_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__SBF__SBF_CODEGEN_TMPL_H__

#include <algorithm>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "boolean_algebras/sbf/sbf_ba.h"

namespace idni::tau_lang {

// One DNF literal, named rather than numbered: the artifact's BDD tables
// start empty, so a variable's numeric id in *this* process's var_dict is
// never portable across processes -- only its name is. The emitted call
// interns the name through the artifact's own var_dict at startup and builds
// the literal from it, mirroring sbf_eval_node's `type::variable` case
// (sbf_ba.tmpl.h) rather than baking the id this process happened to assign.
inline std::string sbf_literal_expr(const std::string& name, bool negated) {
	std::ostringstream ss;
	ss << "::idni::tau_lang::bdd_handle<::idni::tau_lang::Bool>::bit("
	   << (negated ? "false" : "true")
	   << ", static_cast<::idni::tau_lang::uint_t>("
	      "::idni::tau_lang::var_dict(std::string(\"" << name << "\"))))";
	return ss.str();
}

// A self-contained C++ expression of type sbf_ba rebuilding @p v from its
// DNF: a disjunction of conjunctions of named literals, combined with the
// BDD's own &/|/~ exactly as sbf_eval_node combines a parsed conjunction/
// disjunction. Clause and literal order are sorted by variable name (never by
// the numeric BDD ids this process happened to assign), so the same logical
// constant always emits the same text regardless of parse/interning order.
template <NodeType node>
inline std::string sbf_value_expr(const sbf_ba& v) {
	if (v->is_zero())
		return "::idni::tau_lang::bdd_handle<::idni::tau_lang::Bool>::hfalse";
	if (v->is_one())
		return "::idni::tau_lang::bdd_handle<::idni::tau_lang::Bool>::htrue";
	std::vector<std::pair<std::string, std::string>> clauses; // sort key, expr
	for (const auto& clause : v->dnf()) {
		std::vector<std::pair<std::string, bool>> lits; // name, negated
		for (int_t lit : clause.second)
			lits.emplace_back(var_dict(lit < 0 ? -lit : lit), lit < 0);
		std::sort(lits.begin(), lits.end());
		std::ostringstream expr, key;
		expr << "(";
		bool first = true;
		for (const auto& [name, negated] : lits) {
			if (!first) expr << " & "; else first = false;
			expr << sbf_literal_expr(name, negated);
			key << (negated ? "!" : "") << name << ",";
		}
		expr << ")";
		clauses.emplace_back(key.str(), expr.str());
	}
	std::sort(clauses.begin(), clauses.end());
	std::ostringstream ss;
	ss << "(";
	bool first = true;
	for (const auto& [key, expr] : clauses) {
		if (!first) ss << " | "; else first = false;
		ss << expr;
	}
	ss << ")";
	return ss.str();
}

// A self-contained C++ expression of type tref: an IIFE that rebuilds the
// exact sbf BDD via sbf_value_expr and registers it through the BA's own
// constant pool, mirroring qlt_witness_expr / bv_witness_expr.
template <NodeType node>
inline std::string sbf_constant_expr(const sbf_ba& v) {
	std::ostringstream ss;
	ss << "[]() -> ::idni::tref {\n"
	   << "\t\t\t\tusing node_t = ::idni::tau_lang::tau_pack::node_t;\n"
	   << "\t\t\t\t::idni::tau_lang::sbf_ba v = "
	   << sbf_value_expr<node>(v) << ";\n"
	   << "\t\t\t\t::idni::tau_lang::tree<node_t>::constant c = v;\n"
	   << "\t\t\t\t::idni::tref witness_raw = "
	   << "::idni::tau_lang::ba_constants<node_t>::get(c, "
	   << "::idni::tau_lang::ba_descriptor<::idni::tau_lang::sbf_ba, node_t>"
	   << "::type_tree());\n"
	   << "\t\t\t\treturn ::idni::tau_lang::tree<node_t>::get("
	   << "::idni::tau_lang::tree<node_t>::bf, witness_raw);\n"
	   << "\t\t\t}()";
	return ss.str();
}

// The codegen_constant_expr capability: @p cst is already a trimmed, known
// sbf BDD value, so this only extracts and spells it.
template <NodeType node>
static std::optional<std::string> sbf_codegen_constant_expr(tref cst) {
	using tau = tree<node>;
	if (!tau::get(cst).is_ba_constant()) return std::nullopt;
	return sbf_constant_expr<node>(
		std::get<sbf_ba>(tau::get(cst).get_ba_constant()));
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__SBF__SBF_CODEGEN_TMPL_H__
