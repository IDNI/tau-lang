// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "api.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "api"

namespace idni::tau_lang {

inline measuring& measuring::part() { return parts.emplace_back(), parts.back(); }

inline std::ostream& measuring::operator()(std::ostream& os, size_t level) const {
	if (print_json) return to_json(os) << "\n";
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

inline std::ostream& measuring::to_json(std::ostream& os, size_t level) const {
	auto indent0 = std::string(level, '\t');
	auto indent1 = std::string(level+1, '\t');
	os << indent0 << "{\n";
	os << indent1 << "\"measured\": \"" << name << "\",\n";
	os << indent1 << "\"micros\": " << (ms*1000);
	if (!parts.empty()) {
		os << ",\n";
		os << indent1 << "\"parts\":\n";
		for (size_t i = 0; i < parts.size(); ++i) {
			if (i) os << ",\n";
			parts[i].to_json(os, level + 1);
		}
	}
	os << "\n";
	os << indent0 << "}";
	return os;
}

inline api_measure::api_measure(std::string name, measuring& m) : m(m) {
	m.name = name, t.start();
}
inline api_measure::~api_measure() { m.ms = t.stop(); }

// helper macros

#define UNWRAP(...) __VA_ARGS__

#define MT(ret, name, params, args) \
template <NodeType node> \
ret api<node>::name(measuring& m, UNWRAP params) { \
	api_measure am(#name, m); \
	return name(UNWRAP args); \
}

// Parse-family handle wrapper: propagates the delegated call's report.
#define MH(hname, name, params, args) \
template <NodeType node> \
result<htref> api<node>::hname(measuring& m, UNWRAP params) { \
	api_measure am(#hname, m); \
	return name(m.part(), UNWRAP args).transform( \
		[](tref v) { return tau::geth(v); }); \
}

// ------------------------------------------------------------
// Parsing
// ------------------------------------------------------------

MT(result<tref>,  get_term,             (const std::string& input, bool simplified), (input, simplified))
MH(geth_term,  get_term,        (const std::string& input, bool simplified), (input, simplified))

MT(result<tref>,  get_formula,          (const std::string& input, bool simplified), (input, simplified))
MH(geth_formula,  get_formula,  (const std::string& input, bool simplified), (input, simplified))

MT(result<tref>,  get_function_def,     (const std::string& input, bool simplified), (input, simplified))
MH(geth_function_def,  get_function_def,  (const std::string& input, bool simplified), (input, simplified))

MT(result<tref>,  get_predicate_def,    (const std::string& input, bool simplified), (input, simplified))
MH(geth_predicate_def, get_predicate_def, (const std::string& input, bool simplified), (input, simplified))

MT(result<tref>,  get_stream_def,       (const std::string& input), (input))
MH(geth_stream_def, get_stream_def, (const std::string& input), (input))

MT(result<tref>,  get_spec,             (const std::string& input), (input))
MH(geth_spec, get_spec,         (const std::string& input), (input))

MT(result<tref>,  get_definition,       (const std::string& input, bool simplified), (input, simplified))
MH(geth_definition, get_definition, (const std::string& input, bool simplified), (input, simplified))

MT(result<tref>,  get_spec_or_term,     (const std::string& input, bool simplified), (input, simplified))
MH(geth_spec_or_term, get_spec_or_term, (const std::string& input, bool simplified), (input, simplified))

MT(result<tref>,  get_formula_or_term,  (const std::string& input, bool simplified), (input, simplified))
MH(geth_formula_or_term, get_formula_or_term, (const std::string& input, bool simplified), (input, simplified))

MT(result<size_t>, add_definition, (tref head, tref body), (head, body))

// Using definitions
// ------------------------------------------------------------

MT(result<std::string>, apply_def,
	(const std::string& def, const std::string& expression),
	(def, expression))
MT(tref,  apply_def, (tref def, tref expression), (def, expression))
MT(htref, apply_def, (htref def, htref expression), (def, expression))

MT(result<std::string>, apply_defs,
	(const std::set<std::string>& defs, const std::string& expression),
	(defs, expression))
MT(tref,  apply_defs, (subtree_set<node> defs, tref expression), (defs, expression))
MT(htref, apply_defs, (std::set<htref> defs, htref expression), (defs, expression))

MT(result<std::string>, apply_all_defs, (const std::string& expression), (expression))
MT(tref,  apply_all_defs, (tref expression), (expression))
MT(htref, apply_all_defs, (htref expression), (expression))

// Substitution
// ------------------------------------------------------------

MT(result<std::string>, substitute,
	(const std::string& expression, const std::string& that, const std::string& with),
	(expression, that, with))
MT(tref,  substitute, (tref expression, tref that, tref with), (expression, that, with))
MT(htref, substitute, (htref expression, htref that, htref with), (expression, that, with))

MT(result<std::string>, substitute,
	(const std::string& expression, const std::map<std::string, std::string>& that_with),
	(expression, that_with))
MT(tref,  substitute, (tref expression, std::map<tref, tref> that_with), (expression, that_with))
MT(htref, substitute, (htref expression, std::map<htref, htref> that_with), (expression, that_with))

// Normal forms
// ------------------------------------------------------------

MT(result<std::string>, boole_normal_form, (const std::string& expression), (expression))
MT(tref,  boole_normal_form, (tref expression), (expression))
MT(htref, boole_normal_form, (htref expression), (expression))

MT(result<std::string>, dnf, (const std::string& expression), (expression))
MT(tref,  dnf, (tref expression), (expression))
MT(htref, dnf, (htref expression), (expression))

MT(result<std::string>, cnf, (const std::string& expression), (expression))
MT(tref,  cnf, (tref expression), (expression))
MT(htref, cnf, (htref expression), (expression))

MT(result<std::string>, nnf, (const std::string& expression), (expression))
MT(tref,  nnf, (tref expression), (expression))
MT(htref, nnf, (htref expression), (expression))

// Procedures
// ------------------------------------------------------------

MT(result<std::string>, syntactic_term_simplification, (const std::string& term), (term))
MT(tref,  syntactic_term_simplification, (tref term), (term))
MT(htref, syntactic_term_simplification, (htref term), (term))

MT(result<std::string>, syntactic_formula_simplification, (const std::string& formula), (formula))
MT(tref,  syntactic_formula_simplification, (tref formula), (formula))
MT(htref, syntactic_formula_simplification, (htref formula), (formula))

MT(result<std::string>, normalize_term, (const std::string& term), (term))
MT(result<tref>,  normalize_term, (tref term), (term))
MT(result<htref>, normalize_term, (htref term), (term))

MT(result<std::string>, normalize_formula, (const std::string& fm), (fm))
MT(result<tref>,  normalize_formula, (tref fm), (fm))
MT(result<htref>, normalize_formula, (htref fm), (fm))

MT(result<std::string>, anti_prenex, (const std::string& fm), (fm))
MT(tref,  anti_prenex, (tref fm), (fm))
MT(htref, anti_prenex, (htref fm), (fm))

MT(result<std::string>, eliminate_quantifiers, (const std::string& fm), (fm))
MT(tref,  eliminate_quantifiers, (tref fm), (fm))
MT(htref, eliminate_quantifiers, (htref fm), (fm))

MT(result<bool>, realizable, (const std::string& spec), (spec))
MT(result<bool>, realizable, (tref spec), (spec))
MT(result<bool>, realizable, (htref spec), (spec))

MT(result<bool>, unrealizable, (const std::string& spec), (spec))
MT(result<bool>, unrealizable, (tref spec), (spec))
MT(result<bool>, unrealizable, (htref spec), (spec))

MT(result<bool>, sat, (const std::string& formula), (formula))
MT(result<bool>, sat, (tref formula), (formula))
MT(result<bool>, sat, (htref formula), (formula))

MT(result<bool>, unsat, (const std::string& formula), (formula))
MT(result<bool>, unsat, (tref formula), (formula))
MT(result<bool>, unsat, (htref formula), (formula))

MT(result<bool>, valid, (const std::string& formula), (formula))
MT(result<bool>, valid, (tref formula), (formula))
MT(result<bool>, valid, (htref formula), (formula))

MT(result<bool>, valid_spec, (const std::string& spec), (spec))
MT(result<bool>, valid_spec, (tref spec), (spec))
MT(result<bool>, valid_spec, (htref spec), (spec))

// Solving
// ------------------------------------------------------------

template <NodeType node>
result<std::map<std::string, std::string>>
api<node>::solve(measuring& m, const std::string& formula, solver_mode mode) {
	api_measure am("solve", m);
	return solve(formula, mode);
}

template <NodeType node>
result<subtree_map<node, tref>>
api<node>::solve(measuring& m, tref formula, solver_mode mode) {
	api_measure am("solve", m);
	return solve(formula, mode);
}

template <NodeType node>
result<std::map<htref, htref>>
api<node>::solve(measuring& m, htref formula, solver_mode mode) {
	api_measure am("solve", m);
	return solve(formula, mode);
}

template <NodeType node>
result<std::map<std::string, std::string>>
api<node>::lgrs(measuring& m, const std::string& equation) {
	api_measure am("lgrs", m);
	return lgrs(equation);
}

template <NodeType node>
result<subtree_map<node, tref>>
api<node>::lgrs(measuring& m, tref equation) {
	api_measure am("lgrs", m);
	return lgrs(equation);
}

template <NodeType node>
result<std::map<htref, htref>>
api<node>::lgrs(measuring& m, htref equation) {
	api_measure am("lgrs", m);
	return lgrs(equation);
}

// Execution
// ------------------------------------------------------------

MT(result<interpreter<node>>, get_interpreter,
	(const std::string& spec),
	(spec))

MT(result<interpreter<node>>, get_interpreter,
	(const std::string& spec, interpreter_options& options),
	(spec, options))

MT(result<interpreter<node>>, get_interpreter,
	(tref spec),
	(spec))

MT(result<interpreter<node>>, get_interpreter,
	(tref spec, interpreter_options& options),
	(spec, options))

MT(result<interpreter<node>>, get_interpreter,
	(tau_spec<node>& spec),
	(spec))

MT(result<interpreter<node>>, get_interpreter,
	(tau_spec<node>& spec, interpreter_options& options),
	(spec, options))

MT(std::vector<stream_at>, get_inputs_for_step, (interpreter<node>& i), (i))

template <NodeType node>
result<std::map<stream_at, std::string>>
api<node>::step(measuring& m, interpreter<node>& i,
	std::map<stream_at, std::string> inputs)
{
	api_measure am("step", m);
	return step(i, std::move(inputs));
}

template <NodeType node>
result<std::map<stream_at, std::string>>
api<node>::step(measuring& m, interpreter<node>& i) {
	api_measure am("step", m);
	return step(i);
}

MT(tref, infer, (tref expr, bool use_defaults), (expr, use_defaults))

MT(result<std::string>, simplify, (const std::string& expr, bool use_defaults), (expr, use_defaults))
MT(tref, simplify, (tref expr, bool use_defaults), (expr, use_defaults))
MT(htref, simplify, (htref expr, bool use_defaults), (expr, use_defaults))

#undef MH
#undef MT
#undef UNWRAP

} // namespace idni::tau_lang
