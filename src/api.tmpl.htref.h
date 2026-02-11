// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "api.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "api"

namespace idni::tau_lang {

// Helper functions
// ------------------------------------------------------------

template <NodeType node>
std::map<htref, htref> geth(const subtree_map<node, tref>& m) {
	std::map<htref, htref> hm;
	for (auto [k, v] : m) hm[tree<node>::geth(k)] = tree<node>::geth(v);
	return hm;
}

// ------------------------------------------------------------
// htref API
// ------------------------------------------------------------

// Parsing
// ------------------------------------------------------------

template <NodeType node>
htref api<node>::geth_term(const std::string& input) {
	return tau::geth(get_term(input));
}

template <NodeType node>
htref api<node>::geth_formula(const std::string& input) {
	return tau::geth(get_formula(input));
}

template <NodeType node>
htref api<node>::geth_function_def(const std::string& function_def) {
	return tau::geth(get_function_def(function_def));
}

template <NodeType node>
htref api<node>::geth_predicate_def(const std::string& predicate_def) {
	return tau::geth(get_predicate_def(predicate_def));
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
htref api<node>::geth_definition(const std::string& definition) {
	return tau::geth(get_definition(definition));
}

template <NodeType node>
htref api<node>::geth_spec_or_term(const std::string& expr) {
	return tau::geth(get_spec_or_term(expr));
}

template <NodeType node>
htref api<node>::geth_formula_or_term(const std::string& expr) {
	return tau::geth(get_formula_or_term(expr));
}

// Querying
// ------------------------------------------------------------

template <NodeType node>
bool api<node>::contains(htref expr, typename node::type nt) {
	return contains(expr->get(), nt);
}

template <NodeType node>
bool api<node>::is_term(htref term) {
	return is_term(term->get());
}

template <NodeType node>
bool api<node>::is_formula(htref fm) {
	return is_formula(fm->get());
}

// Using definitions
// ------------------------------------------------------------

template <NodeType node>
htref api<node>::apply_def(htref def, htref expression) {
	return tau::geth(apply_def(def->get(), expression->get()));
}

template <NodeType node>
htref api<node>::apply_defs(std::set<htref> defs, htref expression) {
	std::set<tref> tdefs;
	for (htref def : defs) tdefs.insert(def->get());
	return tau::geth(apply_defs(tdefs, expression->get()));
}

template <NodeType node>
htref api<node>::apply_all_defs(htref expr) {
	return tau::geth(apply_all_defs(expr->get()));
}


// Printing
// ------------------------------------------------------------

template <NodeType node>
std::ostream& api<node>::print(std::ostream& os, htref expression) {
	return print(os, expression->get());
}

template <NodeType node>
std::string api<node>::to_str(htref expression) {
	return to_str(expression->get());
}


// Substitution
// ------------------------------------------------------------

template <NodeType node>
htref api<node>::substitute(htref expr, htref that, htref with) {
	return tau::geth(substitute(expr->get(), that->get(), with->get()));
}

template <NodeType node>
htref api<node>::substitute(htref expr, std::map<htref, htref> that_with) {
	tref e = expr->get();
	for (auto [that, with] : that_with)
		e = substitute(e, that->get(), with->get());
	return tau::geth(e);
}

// Normal forms
// ------------------------------------------------------------

template <NodeType node>
htref api<node>::boole_normal_form(htref expr) {
	return tau::geth(boole_normal_form(expr->get()));
}

template <NodeType node>
htref api<node>::dnf(htref expr) {
	return tau::geth(dnf(expr->get()));
}

template <NodeType node>
htref api<node>::cnf(htref expr) {
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
htref api<node>::simplify(htref expr) {
	return tau::geth(simplify(expr->get()));
}

template <NodeType node>
htref api<node>::syntactic_term_simplification(htref term) {
	return tau::geth(syntactic_term_simplification(term->get()));
}

template <NodeType node>
htref api<node>::syntactic_formula_simplification(htref fm) {
	return tau::geth(syntactic_formula_simplification(fm->get()));
}
template <NodeType node>
htref api<node>::normalize_formula(htref fm) {
	return tau::geth(normalize_formula(fm->get()));
}

template <NodeType node>
htref api<node>::normalize_term(htref term) {
	return tau::geth(normalize_term(term->get()));
}

template <NodeType node>
htref api<node>::anti_prenex(htref fm) {
	return tau::geth(anti_prenex(fm->get()));
}

template <NodeType node>
htref api<node>::eliminate_quantifiers(htref fm) {
	if (!fm) return nullptr;
	if (tref a = apply_all_defs(fm->get()); a)
		if (tref e = eliminate_quantifiers(a); e) return tau::geth(e);
	return nullptr;
}

template <NodeType node>
bool api<node>::realizable(htref fm) {
	return realizable(fm->get());
}

template <NodeType node>
bool api<node>::unrealizable(htref fm) {
	return unrealizable(fm->get());
}

template <NodeType node>
bool api<node>::sat(htref fm) {
	return sat(fm->get());
}

template <NodeType node>
bool api<node>::unsat(htref fm) {
	return unsat(fm->get());
}

template <NodeType node>
bool api<node>::valid(htref fm) {
	return valid(fm->get());
}

template <NodeType node>
bool api<node>::valid_spec(htref fm) {
	return valid_spec(fm->get());
}

// Solving
// ------------------------------------------------------------

template <NodeType node>
std::optional<std::map<htref, htref>> api<node>::solve(htref fm,
	solver_mode mode)
{
	if (auto solution = solve(fm->get(), mode); solution)
		return geth(solution.value());
	return {};
}

template <NodeType node>
std::optional<std::map<htref, htref>> api<node>::lgrs(htref equation) {
	if (auto solution = lgrs(equation->get()); solution)
		return geth(solution.value());
	return {};
}

} // namespace idni::tau_lang