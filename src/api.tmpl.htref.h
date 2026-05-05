// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "api.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "api"

namespace idni::tau_lang {

// Helper functions
// ------------------------------------------------------------

/// Convert a subtree_map<node,tref> to a map<htref,htref> by wrapping
/// every key and value with tree<node>::geth() for GC safety.
template <NodeType node>
std::map<htref, htref> geth(const subtree_map<node, tref>& m) {
	std::map<htref, htref> hm;
	for (auto [k, v] : m) hm[tree<node>::geth(k)] = tree<node>::geth(v);
	return hm;
}

// ------------------------------------------------------------
// htref API — GC-safe handle wrappers
// ------------------------------------------------------------
// Each method delegates to the corresponding tref overload and wraps
// the result with tau::geth() to produce a GC-safe shared-pointer handle.

// Parsing
// ------------------------------------------------------------

template <NodeType node>
htref api<node>::geth_term(const std::string& input, bool simplified) {
	return tau::geth(get_term(input, simplified));
}

template <NodeType node>
htref api<node>::geth_formula(const std::string& input, bool simplified) {
	return tau::geth(get_formula(input, simplified));
}

template <NodeType node>
htref api<node>::geth_function_def(const std::string& function_def, bool simplified) {
	return tau::geth(get_function_def(function_def, simplified));
}

template <NodeType node>
htref api<node>::geth_predicate_def(const std::string& predicate_def, bool simplified) {
	return tau::geth(get_predicate_def(predicate_def, simplified));
}

template <NodeType node>
htref api<node>::geth_stream_def(const std::string& stream_def) {
	return tau::geth(get_stream_def(stream_def));
}

template <NodeType node>
htref api<node>::geth_spec(const std::string& src) {
	return tau::geth(get_spec(src));
}

template <NodeType node>
htref api<node>::geth_definition(const std::string& definition, bool simplified) {
	return tau::geth(get_definition(definition, simplified));
}

template <NodeType node>
htref api<node>::geth_spec_or_term(const std::string& expr, bool simplified) {
	return tau::geth(get_spec_or_term(expr, simplified));
}

template <NodeType node>
htref api<node>::geth_formula_or_term(const std::string& expr, bool simplified) {
	return tau::geth(get_formula_or_term(expr, simplified));
}

// Querying
// ------------------------------------------------------------

template <NodeType node>
bool api<node>::contains(htref expr, typename node::type nt) {
	if (!expr) return false;
	return contains(expr->get(), nt);
}

template <NodeType node>
bool api<node>::is_term(htref term) {
	if (!term) return false;
	return is_term(term->get());
}

template <NodeType node>
bool api<node>::is_formula(htref fm) {
	if (!fm) return false;
	return is_formula(fm->get());
}

// Using definitions
// ------------------------------------------------------------

template <NodeType node>
htref api<node>::apply_def(htref def, htref expression) {
	if (!def || !expression) return nullptr;
	return tau::geth(apply_def(def->get(), expression->get()));
}

template <NodeType node>
htref api<node>::apply_defs(const std::set<htref>& defs, htref expression) {
	if (!expression) return nullptr;
	subtree_set<node> tdefs;
	for (htref def : defs) if (def) tdefs.insert(def->get());
	return tau::geth(apply_defs(tdefs, expression->get()));
}

template <NodeType node>
htref api<node>::apply_all_defs(htref expr) {
	if (!expr) return nullptr;
	return tau::geth(apply_all_defs(expr->get()));
}


// Printing
// ------------------------------------------------------------

template <NodeType node>
std::ostream& api<node>::print(std::ostream& os, htref expression) {
	if (!expression) return os;
	return print(os, expression->get());
}

template <NodeType node>
std::string api<node>::to_str(htref expression) {
	if (!expression) return {};
	return to_str(expression->get());
}


// Substitution
// ------------------------------------------------------------

template <NodeType node>
htref api<node>::substitute(htref expr, htref that, htref with) {
	if (!expr || !that || !with) return nullptr;
	return tau::geth(substitute(expr->get(), that->get(), with->get()));
}

template <NodeType node>
htref api<node>::substitute(htref expr, std::map<htref, htref> that_with) {
	if (!expr) return nullptr;
	// Apply substitutions sequentially; each step unwraps htref to tref
	tref e = expr->get();
	for (auto [that, with] : that_with) {
		if (!that || !with) continue;
		e = substitute(e, that->get(), with->get());
	}
	return tau::geth(e);
}

// Normal forms
// ------------------------------------------------------------

template <NodeType node>
htref api<node>::boole_normal_form(htref expr) {
	if (!expr) return nullptr;
	return tau::geth(boole_normal_form(expr->get()));
}

template <NodeType node>
htref api<node>::dnf(htref expr) {
	if (!expr) return nullptr;
	return tau::geth(dnf(expr->get()));
}

template <NodeType node>
htref api<node>::cnf(htref expr) {
	if (!expr) return nullptr;
	return tau::geth(cnf(expr->get()));
}

template <NodeType node>
htref api<node>::nnf(htref expr) {
	if (!expr) return nullptr;
	return tau::geth(nnf(expr->get()));
}

// Procedures
// ------------------------------------------------------------

template <NodeType node>
htref api<node>::syntactic_term_simplification(htref term) {
	if (!term) return nullptr;
	return tau::geth(syntactic_term_simplification(term->get()));
}

template <NodeType node>
htref api<node>::syntactic_formula_simplification(htref fm) {
	if (!fm) return nullptr;
	return tau::geth(syntactic_formula_simplification(fm->get()));
}
template <NodeType node>
htref api<node>::normalize_formula(htref fm) {
	if (!fm) return nullptr;
	return tau::geth(normalize_formula(fm->get()));
}

template <NodeType node>
htref api<node>::normalize_term(htref term) {
	if (!term) return nullptr;
	return tau::geth(normalize_term(term->get()));
}

template <NodeType node>
htref api<node>::anti_prenex(htref fm) {
	if (!fm) return nullptr;
	return tau::geth(anti_prenex(fm->get()));
}

template <NodeType node>
htref api<node>::eliminate_quantifiers(htref fm) {
	if (!fm) return nullptr;
	// Apply defs at the tref level before eliminating quantifiers
	// (the tref overload also calls apply_all_defs internally)
	if (tref a = apply_all_defs(fm->get()); a)
		if (tref e = eliminate_quantifiers(a); e) return tau::geth(e);
	return nullptr;
}

template <NodeType node>
bool api<node>::realizable(htref fm) {
	if (!fm) return false;
	return realizable(fm->get());
}

template <NodeType node>
bool api<node>::unrealizable(htref fm) {
	if (!fm) return false;
	return unrealizable(fm->get());
}

template <NodeType node>
bool api<node>::sat(htref fm) {
	if (!fm) return false;
	return sat(fm->get());
}

template <NodeType node>
bool api<node>::unsat(htref fm) {
	if (!fm) return false;
	return unsat(fm->get());
}

template <NodeType node>
bool api<node>::valid(htref fm) {
	if (!fm) return false;
	return valid(fm->get());
}

template <NodeType node>
bool api<node>::valid_spec(htref fm) {
	if (!fm) return false;
	return valid_spec(fm->get());
}

// Solving
// ------------------------------------------------------------

template <NodeType node>
std::optional<std::map<htref, htref>> api<node>::solve(htref fm,
	solver_mode mode)
{
	if (!fm) return {};
	if (auto solution = solve(fm->get(), mode); solution)
		return geth(solution.value());
	return {};
}

template <NodeType node>
std::optional<std::map<htref, htref>> api<node>::lgrs(htref equation) {
	if (!equation) return {};
	if (auto solution = lgrs(equation->get()); solution)
		return geth(solution.value());
	return {};
}

template <NodeType node>
htref api<node>::simplify(htref expr, bool use_defaults) {
	if (!expr) return nullptr;
	return tau::geth(simplify(expr->get(), use_defaults));
}

} // namespace idni::tau_lang
