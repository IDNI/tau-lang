// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "api.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "api"

namespace idni::tau_lang {

// Helper functions
// ------------------------------------------------------------

// ------------------------------------------------------------
// String API — convenience wrappers that accept/return std::string
// ------------------------------------------------------------
// Each method parses its string arguments via the corresponding
// get_* function, delegates to the tref overload, and serializes
// the result back to a string (or returns nullopt on failure).

template <NodeType node>
bool api<node>::is_term(const std::string& term) {
	return get_term(term) != nullptr;
}

template <NodeType node>
bool api<node>::is_formula(const std::string& formula) {
	return get_formula(formula) != nullptr;
}

// Using definitions
// ------------------------------------------------------------

template <NodeType node>
std::optional<std::string> api<node>::apply_def(
	const std::string& def, const std::string& expr)
{
	return apply_defs(std::set<std::string>{ def }, expr);
}

template <NodeType node>
std::optional<std::string> api<node>::apply_defs(
	const std::set<std::string>& defs, const std::string& expr)
{
	// Parse each definition string, collecting them into a tref set;
	// then parse the expression and apply the definitions.
	subtree_set<node> tdefs;
	// A definition that fails to parse used to be inserted as nullptr and
	// then silently skipped by the tref-level apply_defs' "if (def)"
	// guard, so the caller had no way to tell a malformed definition was
	// dropped from a definition that legitimately had no effect. Report
	// the failure instead of silently continuing without it.
	for (const std::string& def : defs) {
		tref d = get_definition(def);
		if (!d) {
			TAU_LOG_ERROR << "Failed to parse definition: " << def;
			return {};
		}
		tdefs.insert(d);
	}
	if (tref a = apply_defs(tdefs, get_spec_or_term(expr)); a) {
		// get_spec_or_term() parses a bare formula as a one-line spec
		// (spec(main(wff(...)))); get_nso_rr()'s no-ref branch keeps
		// that shape rather than unwrapping it the way its ref branch
		// does (via tau_lang::get_nso_rr's main -> wff/bf navigation),
		// so content round-trips through nso_rr_apply but the shape
		// stays spec-wrapped. Only to_str() sees the difference: a
		// spec-shaped tree renders with the trailing '.' every other
		// string overload's result lacks. Unwrap here, at the point
		// content becomes a string, so the tref-level overloads --
		// which other callers (e.g. get_interpreter) rely on staying
		// spec-shaped -- are untouched.
		using tt = typename tau::traverser;
		if (tau::get(a).is(tau::spec)) {
			tref main = tt(a) | tau::main | tau::wff | tt::ref;
			if (!main) main = tt(a) | tau::main | tau::bf | tt::ref;
			if (main) a = main;
		}
		return to_str(a);
	}
	return {};
}

template <NodeType node>
std::optional<std::string> api<node>::apply_all_defs(const std::string& expr) {
	return apply_defs(std::set<std::string>{}, expr);
}

// Substitution
// ------------------------------------------------------------

template <NodeType node>
std::optional<std::string> api<node>::substitute(
	const std::string& expr,
	const std::string& that,
	const std::string& with)
{
	tref e = get_formula_or_term(expr); if (!e) return {};
	tref t = get_formula_or_term(that); if (!t) return {};
	tref w = get_formula_or_term(with); if (!w) return {};
	if (tref s = substitute(e, t, w); s) return to_str(s);
	return {};
}

template <NodeType node>
std::optional<std::string> api<node>::substitute(
	const std::string& expr,
	const std::map<std::string, std::string>& that_with)
{
	tref e = get_formula_or_term(expr); if (!e) return {};
	for (auto [that, with] : that_with) {
		tref t = get_formula_or_term(that); if (!t) return {};
		tref w = get_formula_or_term(with); if (!w) return {};
		e = substitute(e, t, w); if (!e) return {};
	}
	return to_str(e);
}


// Normal forms
// ------------------------------------------------------------

template <NodeType node>
std::optional<std::string> api<node>::boole_normal_form(const std::string& expr)
{
	// AP1-17: delegate to the tref overload (it runs simplify first);
	// the inlined copy skipped it and could diverge on canonization.
	if (tref b = boole_normal_form(get_formula_or_term(expr)); b)
		return to_str(b);
	return {};
}

template <NodeType node>
std::optional<std::string> api<node>::dnf(const std::string& expr) {
	if (tref e = get_formula_or_term(expr); e)
		if (tref d = dnf(e); d) return to_str(d);
	return {};
}

template <NodeType node>
std::optional<std::string> api<node>::cnf(const std::string& expr) {
	if (tref e = get_formula_or_term(expr); e)
		if (tref c = cnf(e); c) return to_str(c);
	return {};
}

template <NodeType node>
std::optional<std::string> api<node>::nnf(const std::string& expr) {
	if (tref e = get_formula_or_term(expr); e)
		if (tref n = nnf(e); n) return to_str(n);
	return {};
}

// Procedures
// ------------------------------------------------------------

template <NodeType node>
std::optional<std::string> api<node>::syntactic_term_simplification(
	const std::string& term)
{
	if (tref e = get_term(term); e)
		if (tref s = syntactic_term_simplification(e); s)
			return to_str(s);
	return {};
}

template <NodeType node>
std::optional<std::string> api<node>::syntactic_formula_simplification(
	const std::string& fm)
{
	if (tref e = get_formula(fm); e)
		if (tref s = syntactic_formula_simplification(e); s)
			return to_str(s);
	return {};
}

template <NodeType node>
std::optional<std::string> api<node>::normalize_term(const std::string& expr)
{
	if (tref term = get_term(expr); term)
		if (tref n = normalize_term(term); n) return to_str(n);
	return {};
}

template <NodeType node>
std::optional<std::string> api<node>::normalize_formula(
	const std::string& expr)
{
	if (tref fm = get_formula(expr); fm)
		if (tref n = normalize_formula(fm); n) return to_str(n);
	return {};
}

template <NodeType node>
std::optional<std::string> api<node>::anti_prenex(const std::string& expr) {
	if (tref fm = get_formula(expr); fm)
		if (tref a = anti_prenex(fm); a) return to_str(a);
	return {};
}

template <NodeType node>
std::optional<std::string> api<node>::eliminate_quantifiers(
	const std::string& expr)
{
	// AP1-17: delegate to the tref overload (see boole_normal_form).
	if (tref e = get_formula(expr); e)
		if (tref r = eliminate_quantifiers(e); r)
			return to_str(r);
	return {};
}

template <NodeType node>
bool api<node>::realizable(const std::string& expr) {
	return realizable(get_formula_or_term(expr));
}

template <NodeType node>
bool api<node>::unrealizable(const std::string& expr) {
	return unrealizable(get_formula_or_term(expr));
}

template <NodeType node>
bool api<node>::sat(const std::string& expr) {
	return sat(get_formula_or_term(expr));
}

template <NodeType node>
bool api<node>::unsat(const std::string& expr) {
	// AP1-11: route through the tref overload so unparseable input is
	// invalid (false), not "unsatisfiable".
	return unsat(get_spec_or_term(expr));
}

template <NodeType node>
bool api<node>::valid(const std::string& expr) {
	return valid(get_formula_or_term(expr));
}

template <NodeType node>
bool api<node>::valid_spec(const std::string& expr) {
	return valid_spec(get_formula_or_term(expr));
}


// Solving
// ------------------------------------------------------------

// Render a solved variable's value the way the REPL does
// (print_solver_cmd_solution, repl_evaluator.tmpl.h): a compound BA constant
// already stringifies with its own type-tagged form (tau_tree_printers.tmpl.h's
// `ba_constant` case), so it goes through the generic printer unchanged; the
// atomic bf_t/bf_f nodes carry no BA type of their own, so serialize_constant
// recovers the declared type's own literal instead of the generic "0"/"1".
// fallback_type mirrors the REPL, resolving the variable's own type first and
// falling back to the formula's type only for untyped variables.
template <NodeType node>
std::map<std::string, std::string> serialize_solution(
	const solution<node>& sol, size_t fallback_type)
{
	using tau = tree<node>;
	using tt = typename tau::traverser;
	std::map<std::string, std::string> s;
	for (auto& [var, val] : sol) {
		if (tt(val) | tau::ba_constant) {
			s.emplace(tau::get(var).to_str(), tau::get(val).to_str());
			continue;
		}
		size_t t = find_ba_type<node>(var);
		if (t == 0) t = fallback_type;
		std::stringstream ss;
		s.emplace(tau::get(var).to_str(), serialize_constant<node>(ss, val, t)
			? ss.str() : tau::get(val).to_str());
	}
	return s;
}

template <NodeType node>
std::optional<std::map<std::string, std::string>> api<node>::solve(
	const std::string& formula,
	solver_mode mode)
{
	tref fm = get_formula(formula);
	if (auto solution = solve(fm, mode); solution)
		return serialize_solution<node>(solution.value(),
			find_ba_type_or_default<node>(fm));
	return {};
}

template <NodeType node>
std::optional<std::map<std::string, std::string>> api<node>::lgrs(
	const std::string& equation)
{
	tref eq = get_formula(equation);
	if (auto solution = lgrs(eq); solution)
		return serialize_solution<node>(solution.value(),
			find_ba_type_or_default<node>(eq));
	return {};
}

// Execution
// ------------------------------------------------------------

template <NodeType node>
std::optional<interpreter<node>> api<node>::get_interpreter(
	const std::string& specification)
{
	interpreter_options options;
	return get_interpreter(specification, options);
}

template <NodeType node>
std::optional<interpreter<node>> api<node>::get_interpreter(
	const std::string& specification,
	interpreter_options& options)
{
	DBG(TAU_LOG_TRACE << "get_interpreter/specification: " << specification;);
	// Parse the specification string into a tau_spec, logging any
	// parse errors, then delegate to the tau_spec overload.
	tau_spec<node> spec;
	if (!spec.parse(specification)) {
		for (const auto& error : spec.errors()) TAU_LOG_ERROR << error;
		return {};
	}
	return get_interpreter(spec, options);
}

template <NodeType node>
std::vector<stream_at> api<node>::get_inputs_for_step(interpreter<node>& i) {
	// Build the set of input variables needed at the current time point,
	// filter to those within the spec's lookback window, and return
	// as (name, time_point) pairs.
	auto [step_inputs, _] = i.build_inputs_for_step(i.time_point);
	std::vector<stream_at> inputs;
	for (auto& var : i.appear_within_lookback(step_inputs)) {
		DBG(TAU_LOG_TRACE << "get_inputs_for_step/input: " << TAU_LOG_FM_DUMP(var);)
		inputs.emplace_back(get_var_name<node>(var), i.time_point);
	}
	return inputs;
}

template <NodeType node>
std::optional<std::map<stream_at, std::string>> api<node>::step(
	interpreter<node>& i, std::map<stream_at, std::string> inputs,
	bool interactive)
{
	DBG(using tau = tree<node>;)

	auto& ctx = i.ctx;

	if (!i.calculate_initial_spec()) return {};

	// Build inputs for the step
	DBG(TAU_LOG_TRACE << "number of inputs: " << inputs.size();)
	subtree_map<node, stream_at> step_input_map;
	trefs step_inputs;
	for (auto& [in, value] : inputs) {
		if (in.name == "this") continue;
		size_t var_name_sid = dict(in.name);
		auto has_var_name_sid = [&var_name_sid](const auto& it) {
			return get_var_name_sid<node>(it.first->get()) == var_name_sid;
		};
		auto it = std::find_if(ctx.inputs.begin(), ctx.inputs.end(),
					has_var_name_sid);
		if (it == ctx.inputs.end()) {
			TAU_LOG_ERROR << "Input stream " << in.name
						<< " not found in context";
			return {};
		}
		DBG(TAU_LOG_TRACE << "Input " << in.name << "[" << in.time_point << "] = `" << value << "` : " << TAU_LOG_BA_TYPE(i.ctx.type_of(it->first->get()));)
		step_inputs.emplace_back(
			build_in_var_at_n<node>(in.name, in.time_point,
				i.ctx.type_of(it->first->get())));
		step_input_map[step_inputs.back()] = in;
		DBG(TAU_LOG_TRACE << "added step input: " << TAU_LOG_FM_DUMP(step_inputs.back());)
	}
	DBG(TAU_LOG_TRACE << "Step inputs: " << step_inputs.size();)
	// step_inputs = i.appear_within_lookback(step_inputs);
	assignment<node> values;

	// parse input values
	DBG(TAU_LOG_TRACE << "Parsing input values";)
	for (tref step_input : step_inputs) {
		DBG(TAU_LOG_TRACE << "Step input: " << TAU_LOG_FM_DUMP(step_input);)
		const std::string& input_value =
					inputs[step_input_map[step_input]];
		size_t type_id = i.ctx.type_of(canonize<node>(step_input));
		auto cnst = ba_constants<node>::get(input_value,
					get_ba_type_tree<node>(type_id));
		if (!cnst) {
			TAU_LOG_ERROR << "Failed to parse input value "
								<< input_value;
			return {};
		}
		tref c = build_bf_ba_constant<node>(cnst.value().first, type_id);
		if (has_open_tau_fm_in_constant<node>(c)) {
			TAU_LOG_ERROR <<"Constant contains an open tau formula: "
								<< input_value;
			return {};
		}
		values[step_input] = c;
		DBG(TAU_LOG_TRACE << "Parsed input `" << input_value << "` : " << TAU_LOG_BA_TYPE(type_id);)
		DBG(TAU_LOG_TRACE << "Value: " << TAU_LOG_FM_DUMP(c);)
	}

	// Step the interpreter
	auto [output, auto_continue] = i.step(values);
	if (!output.has_value()) {
		DBG(TAU_LOG_TRACE << "No input provided or error."
			<< " Quit at time point " << i.time_point;)
		return {};
	}

	// Write output values so they are recorded for subsequent steps
	if (!i.write(output.value())) {
		TAU_LOG_ERROR << "Failed to write outputs";
		return {};
	}

	// Build outputs for the step
	std::map<stream_at, std::string> outputs;
	for (const auto& [out, val] : output.value()) {
		DBG(TAU_LOG_TRACE << "Output " << get_var_name<node>(out) << "[" << i.time_point << "] = `" << tau::get(val).to_str() <<"`";)
		DBG(TAU_LOG_TRACE << TAU_LOG_FM_DUMP(out);)
		DBG(TAU_LOG_TRACE << TAU_LOG_FM_DUMP(val);)
		std::stringstream ss;
		if (!serialize_constant<node>(ss, val, i.ctx.type_of(out))) {
			TAU_LOG_ERROR << "No Boolean algebra element assigned "
				"to output '" << TAU_TO_STR(out) << "'";
			return {};
		}
		outputs[{ get_var_name<node>(out), i.time_point }] = ss.str();
	}

	// Run update if update stream is present and unequal to 0
	if (tref update = get_update<node>(i, output.value()); update)
		i.update(update);
	else warn_if_update_dropped<node>(i, output.value());

	if (interactive && !auto_continue) {
		TAU_LOG_TRACE << "auto continue is false.";
		return {};
	}

	return outputs;
}

template <NodeType node>
std::optional<std::map<stream_at, std::string>> api<node>::step(
	interpreter<node>& i)
{
	// tau is only consulted by DBG tracing since AP1-12 switched the
	// output serialization to serialize_constant.
	using tau [[maybe_unused]] = tree<node>;

	if (!i.calculate_initial_spec()) return {};

	// Step the interpreter
	auto [output, auto_continue] = i.step();
	if (!output.has_value()) {
		DBG(TAU_LOG_TRACE << "No input provided or error."
			<< " Quit at time point " << i.time_point;)
		return {};
	}

	// Write output values
	if (!i.write(output.value())) {
		TAU_LOG_ERROR << "Failed to write outputs";
		return {};
	}

	// Build outputs for the step. AP1-12: serialize via
	// serialize_constant like the with-inputs overload -- raw to_str()
	// skipped the bf_t/bf_f-to-BA-element mapping and the no-element
	// failure check, so the two overloads printed different values for
	// the same step.
	std::map<stream_at, std::string> outputs;
	for (const auto& [out, val] : output.value()) {
		DBG(TAU_LOG_TRACE << "Output " << get_var_name<node>(out) << "[" << i.time_point << "] = `" << tau::get(val).to_str() <<"`";)
		DBG(TAU_LOG_TRACE << TAU_LOG_FM_DUMP(out);)
		DBG(TAU_LOG_TRACE << TAU_LOG_FM_DUMP(val);)
		std::stringstream ss;
		if (!serialize_constant<node>(ss, val, i.ctx.type_of(out))) {
			TAU_LOG_ERROR << "No Boolean algebra element assigned "
				"to output '" << TAU_TO_STR(out) << "'";
			return {};
		}
		outputs[{ get_var_name<node>(out), i.time_point }] = ss.str();
	}

	// Run update if update stream is present and unequal to 0
	if (tref update = get_update<node>(i, output.value()); update)
		i.update(update);
	else warn_if_update_dropped<node>(i, output.value());

	if (!auto_continue) {
		TAU_LOG_TRACE << "auto continue is false.";
		return {};
	}

	return outputs;
}

template <NodeType node>
std::optional<std::string> api<node>::simplify(const std::string& expr,
	bool use_defaults)
{
	if (tref e = get_formula_or_term(expr); e)
		if (tref s = simplify(e, use_defaults); s) return to_str(s);
	return {};
}

} // namespace idni::tau_lang
