// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "api.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "api"

namespace idni::tau_lang {

measuring& measuring::part() { return parts.emplace_back(), parts.back(); }
std::ostream& measuring::operator()(std::ostream& os, size_t level) const {
	auto indent = std::string(level, '\t');
	os << indent << name << ": " << ms << " ms\n";
	if (!parts.empty()) {
		for (size_t i = 0; i < parts.size(); ++i) {
			parts[i](os, level + 1);
		}
	}
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const measuring& m) { return m(os); }

api_measure::api_measure(std::string name, measuring& m) : m(m) {
	m.name = name, t.start();
}
api_measure::~api_measure() { m.ms = t.stop(); }

// helper macros

#define UNWRAP(...) __VA_ARGS__

#define MT(ret, name, params, args) \
template <NodeType node> \
ret api<node>::name(measuring& m, UNWRAP params) { \
	api_measure am(#name, m); \
	return name(UNWRAP args); \
}

#define MH(hname, name, params, args) \
template <NodeType node> \
htref api<node>::hname(measuring& m, UNWRAP params) { \
	api_measure am(#hname, m); \
	return tau::geth(name(m.part(), UNWRAP args)); \
}

// ------------------------------------------------------------
// Parsing
// ------------------------------------------------------------

MT(tref,  get_term,             (const std::string& input, bool simplified), (input, simplified))
MH(geth_term,  get_term,        (const std::string& input, bool simplified), (input, simplified))

MT(tref,  get_formula,          (const std::string& input, bool simplified), (input, simplified))
MH(geth_formula,  get_formula,  (const std::string& input, bool simplified), (input, simplified))

MT(tref,  get_function_def,     (const std::string& input, bool simplified), (input, simplified))
MH(geth_function_def,  get_function_def,  (const std::string& input, bool simplified), (input, simplified))

MT(tref,  get_predicate_def,    (const std::string& input, bool simplified), (input, simplified))
MH(geth_predicate_def, get_predicate_def, (const std::string& input, bool simplified), (input, simplified))

MT(tref,  get_stream_def,       (const std::string& input), (input))
MH(geth_stream_def, get_stream_def, (const std::string& input), (input))

MT(tref,  get_spec,             (const std::string& input), (input))
MH(geth_spec, get_spec,         (const std::string& input), (input))

MT(tref,  get_definition,       (const std::string& input, bool simplified), (input, simplified))
MH(geth_definition, get_definition, (const std::string& input, bool simplified), (input, simplified))

MT(tref,  get_spec_or_term,     (const std::string& input, bool simplified), (input, simplified))
MH(geth_spec_or_term, get_spec_or_term, (const std::string& input, bool simplified), (input, simplified))

MT(tref,  get_formula_or_term,  (const std::string& input, bool simplified), (input, simplified))
MH(geth_formula_or_term, get_formula_or_term, (const std::string& input, bool simplified), (input, simplified))

MT(size_t, add_definition, (tref head, tref body), (head, body))

// Using definitions
// ------------------------------------------------------------

MT(std::optional<std::string>, apply_def,
	(const std::string& def, const std::string& expression),
	(def, expression))
MT(tref,  apply_def, (tref def, tref expression), (def, expression))
MT(htref, apply_def, (htref def, htref expression), (def, expression))

MT(std::optional<std::string>, apply_defs,
	(const std::set<std::string>& defs, const std::string& expression),
	(defs, expression))
MT(tref,  apply_defs, (std::set<tref> defs, tref expression), (defs, expression))
MT(htref, apply_defs, (std::set<htref> defs, htref expression), (defs, expression))

MT(std::optional<std::string>, apply_all_defs, (const std::string& expression), (expression))
MT(tref,  apply_all_defs, (tref expression), (expression))
MT(htref, apply_all_defs, (htref expression), (expression))

// Substitution
// ------------------------------------------------------------

MT(std::optional<std::string>, substitute,
	(const std::string& expression, const std::string& that, const std::string& with),
	(expression, that, with))
MT(tref,  substitute, (tref expression, tref that, tref with), (expression, that, with))
MT(htref, substitute, (htref expression, htref that, htref with), (expression, that, with))

MT(std::optional<std::string>, substitute,
	(const std::string& expression, const std::map<std::string, std::string>& that_with),
	(expression, that_with))
MT(tref,  substitute, (tref expression, std::map<tref, tref> that_with), (expression, that_with))
MT(htref, substitute, (htref expression, std::map<htref, htref> that_with), (expression, that_with))

// Normal forms
// ------------------------------------------------------------

MT(std::optional<std::string>, boole_normal_form, (const std::string& expression), (expression))
MT(tref,  boole_normal_form, (tref expression), (expression))
MT(htref, boole_normal_form, (htref expression), (expression))

MT(std::optional<std::string>, dnf, (const std::string& expression), (expression))
MT(tref,  dnf, (tref expression), (expression))
MT(htref, dnf, (htref expression), (expression))

MT(std::optional<std::string>, cnf, (const std::string& expression), (expression))
MT(tref,  cnf, (tref expression), (expression))
MT(htref, cnf, (htref expression), (expression))

MT(std::optional<std::string>, nnf, (const std::string& expression), (expression))
MT(tref,  nnf, (tref expression), (expression))
MT(htref, nnf, (htref expression), (expression))

// Procedures
// ------------------------------------------------------------

MT(std::optional<std::string>, simplify, (const std::string& expression), (expression))
MT(tref,  simplify, (tref expression), (expression))
MT(htref, simplify, (htref expression), (expression))

MT(std::optional<std::string>, syntactic_term_simplification, (const std::string& term), (term))
MT(tref,  syntactic_term_simplification, (tref term), (term))
MT(htref, syntactic_term_simplification, (htref term), (term))

MT(std::optional<std::string>, syntactic_formula_simplification, (const std::string& formula), (formula))
MT(tref,  syntactic_formula_simplification, (tref formula), (formula))
MT(htref, syntactic_formula_simplification, (htref formula), (formula))

MT(std::optional<std::string>, normalize_term, (const std::string& term), (term))
MT(tref,  normalize_term, (tref term), (term))
MT(htref, normalize_term, (htref term), (term))

MT(std::optional<std::string>, normalize_formula, (const std::string& fm), (fm))
MT(tref,  normalize_formula, (tref fm), (fm))
MT(htref, normalize_formula, (htref fm), (fm))

MT(std::optional<std::string>, anti_prenex, (const std::string& fm), (fm))
MT(tref,  anti_prenex, (tref fm), (fm))
MT(htref, anti_prenex, (htref fm), (fm))

MT(std::optional<std::string>, eliminate_quantifiers, (const std::string& fm), (fm))
MT(tref,  eliminate_quantifiers, (tref fm), (fm))
MT(htref, eliminate_quantifiers, (htref fm), (fm))

MT(bool, realizable, (const std::string& spec), (spec))
MT(bool, realizable, (tref spec), (spec))
MT(bool, realizable, (htref spec), (spec))

MT(bool, unrealizable, (const std::string& spec), (spec))
MT(bool, unrealizable, (tref spec), (spec))
MT(bool, unrealizable, (htref spec), (spec))

MT(bool, sat, (const std::string& formula), (formula))
MT(bool, sat, (tref formula), (formula))
MT(bool, sat, (htref formula), (formula))

MT(bool, unsat, (const std::string& formula), (formula))
MT(bool, unsat, (tref formula), (formula))
MT(bool, unsat, (htref formula), (formula))

MT(bool, valid, (const std::string& formula), (formula))
MT(bool, valid, (tref formula), (formula))
MT(bool, valid, (htref formula), (formula))

MT(bool, valid_spec, (const std::string& spec), (spec))
MT(bool, valid_spec, (tref spec), (spec))
MT(bool, valid_spec, (htref spec), (spec))

// Solving
// ------------------------------------------------------------

template <NodeType node>
std::optional<std::map<std::string, std::string>>
api<node>::solve(measuring& m, const std::string& formula, solver_mode mode) {
	api_measure am("solve", m);
	return solve(formula, mode);
}

template <NodeType node>
std::optional<subtree_map<node, tref>>
api<node>::solve(measuring& m, tref formula, solver_mode mode) {
	api_measure am("solve", m);
	return solve(formula, mode);
}

template <NodeType node>
std::optional<std::map<htref, htref>>
api<node>::solve(measuring& m, htref formula, solver_mode mode) {
	api_measure am("solve", m);
	return solve(formula, mode);
}

template <NodeType node>
std::optional<std::map<std::string, std::string>>
api<node>::lgrs(measuring& m, const std::string& equation) {
	api_measure am("lgrs", m);
	return lgrs(equation);
}

template <NodeType node>
std::optional<subtree_map<node, tref>>
api<node>::lgrs(measuring& m, tref equation) {
	api_measure am("lgrs", m);
	return lgrs(equation);
}

template <NodeType node>
std::optional<std::map<htref, htref>>
api<node>::lgrs(measuring& m, htref equation) {
	api_measure am("lgrs", m);
	return lgrs(equation);
}

// Execution
// ------------------------------------------------------------

MT(std::optional<interpreter<node>>, get_interpreter,
	(const std::string& spec),
	(spec))

MT(std::optional<interpreter<node>>, get_interpreter,
	(const std::string& spec, interpreter_options& options),
	(spec, options))

MT(std::optional<interpreter<node>>, get_interpreter,
	(tref spec),
	(spec))

MT(std::optional<interpreter<node>>, get_interpreter,
	(tref spec, interpreter_options& options),
	(spec, options))

MT(std::optional<interpreter<node>>, get_interpreter,
	(tau_spec<node>& spec),
	(spec))

MT(std::optional<interpreter<node>>, get_interpreter,
	(tau_spec<node>& spec, interpreter_options& options),
	(spec, options))

MT(std::vector<stream_at>, get_inputs_for_step, (interpreter<node>& i), (i))

template <NodeType node>
std::optional<std::map<stream_at, std::string>>
api<node>::step(measuring& m, interpreter<node>& i,
	std::map<stream_at, std::string> inputs)
{
	api_measure am("step", m);
	return step(i, std::move(inputs));
}

template <NodeType node>
std::optional<std::map<stream_at, std::string>>
api<node>::step(measuring& m, interpreter<node>& i) {
	api_measure am("step", m);
	return step(i);
}

MT(tref, infer, (tref expr, bool use_defaults), (expr, use_defaults))

#undef MH
#undef MT
#undef UNWRAP

} // namespace idni::tau_lang
