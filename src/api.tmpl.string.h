// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "api.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "api"

namespace idni::tau_lang {

// Helper functions
// ------------------------------------------------------------

template <NodeType node>
std::map<std::string, std::string> to_str(const subtree_map<node, tref>& m) {
	std::map<std::string, std::string> sm;
	for (auto [k, v] : m) if (k && v) sm[to_str(k)] = to_str(v);
	return sm;
}

// ------------------------------------------------------------
// String API
// ------------------------------------------------------------

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
	std::set<tref> tdefs;
	for (const std::string& def : defs) tdefs.insert(get_definition(def));
	if (tref a = apply_defs(tdefs, get_spec_or_term(expr)); a)
		return to_str(a);
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
	if (tref a = apply_all_defs(expr); a)
		if (tref b = tau_lang::boole_normal_form<node>(a); b)
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
std::optional<std::string> api<node>::simplify(const std::string& expr) {
	if (tref e = get_formula_or_term(expr); e)
		if (tref s = simplify(e); s) return to_str(s);
	return {};
}

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
		if (tref n = normalize_formula<node>(fm); n) return to_str(n);
	return {};
}

template <NodeType node>
std::optional<std::string> api<node>::anti_prenex(const std::string& expr) {
	if (tref fm = get_formula(expr); fm)
		if (tref a = anti_prenex<node>(fm); a) return to_str(a);
	return {};
}

template <NodeType node>
std::optional<std::string> api<node>::eliminate_quantifiers(
	const std::string& expr)
{
	if (tref e = get_formula(expr); e)
		if (tref a = apply_all_defs(e); a)
			if (tref r = resolve_quantifiers<node>(
				tau_lang::anti_prenex<node>(a)); r)
					return to_str(r);
	return {};
}

template <NodeType node>
bool api<node>::realizable(const std::string& expr) {
	return realizable(get_spec_or_term(expr));
}

template <NodeType node>
bool api<node>::unrealizable(const std::string& expr) {
	return unrealizable(get_spec_or_term(expr));
}

template <NodeType node>
bool api<node>::sat(const std::string& expr) {
	return sat(get_spec_or_term(expr));
}

template <NodeType node>
bool api<node>::unsat(const std::string& expr) {
	return !sat(expr);
}

template <NodeType node>
bool api<node>::valid(const std::string& expr) {
	return valid(get_spec_or_term(expr));
}

template <NodeType node>
bool api<node>::valid_spec(const std::string& expr) {
	return valid_spec(get_spec_or_term(expr));
}


// Solving
// ------------------------------------------------------------

template <NodeType node>
std::optional<std::map<std::string, std::string>> api<node>::solve(
	const std::string& formula,
	solver_mode mode)
{
	if (auto solution = solve(get_formula(formula), mode); solution)
		return to_str(solution.value());
	return {};
}

template <NodeType node>
std::optional<std::map<std::string, std::string>> api<node>::lgrs(
	const std::string& equation)
{
	if (auto solution = lgrs(get_formula(equation)); solution)
		return to_str(solution.value());
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
	tau_spec<node> spec;
	if (!spec.parse(specification)) {
		for (const auto& error : spec.errors()) TAU_LOG_ERROR << error;
		return {};
	}
	return get_interpreter(spec, options);
}

template <NodeType node>
std::vector<stream_at> api<node>::get_inputs_for_step(interpreter<node>& i) {
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
	interpreter<node>& i, std::map<stream_at, std::string> inputs)
{
	DBG(using tau = tree<node>;)

	auto& ctx = *definitions<node>::instance().get_io_context();

	if (!i.calculate_initial_spec()) return {};

	// Build inputs for the step
	DBG(TAU_LOG_TRACE << "number of inputs: " << inputs.size();)
	subtree_map<node, stream_at> step_input_map;
	trefs step_inputs;
	for (auto& [in, value] : inputs) {
		if (in.name == "this") continue;
		size_t var_name_sid = dict(in.name);
		auto has_var_name_sid = [&var_name_sid](auto it) {
			return get_var_name_sid<node>(it.first) == var_name_sid;
		};
		auto it = std::find_if(ctx.inputs.begin(), ctx.inputs.end(),
					has_var_name_sid);
		if (it == ctx.inputs.end()) {
			TAU_LOG_ERROR << "Input stream " << in.name
						<< " not found in context";
			return {};
		}
		DBG(TAU_LOG_TRACE << "Input " << in.name << "[" << in.time_point << "] = `" << value << "` : " << TAU_LOG_BA_TYPE(i.ctx.type_of(it->first));)
		step_inputs.emplace_back(
			build_in_var_at_n<node>(in.name, in.time_point,
				i.ctx.type_of(it->first)));
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
	// Build outputs for the step
	std::map<stream_at, std::string> outputs;
	for (const auto& [out, val] : output.value()) {
		DBG(TAU_LOG_TRACE << "Output " << get_var_name<node>(out) << "[" << i.time_point << "] = `" << tau::get(val).to_str() <<"`";)
		DBG(TAU_LOG_TRACE << TAU_LOG_FM_DUMP(out);)
		DBG(TAU_LOG_TRACE << TAU_LOG_FM_DUMP(val);)
		std::stringstream ss;
		if (!i.serialize_constant(ss, val, i.ctx.type_of(out))) {
			TAU_LOG_ERROR << "No Boolean algebra element assigned "
				"to output '" << TAU_TO_STR(out) << "'";
			return {};
		}
		outputs[{ get_var_name<node>(out), i.time_point }] = ss.str();
	}

	// Run update if update stream is present and unequal to 0
	if (tref update = get_update<node>(i, output.value()); update)
		i.update(update);

	if (!auto_continue) {
		TAU_LOG_TRACE << "auto continue is false.";
		return {};
	}

	return outputs;
}

template <NodeType node>
std::optional<std::map<stream_at, std::string>> api<node>::step(
	interpreter<node>& i)
{
	using tau = tree<node>;

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

	// Build outputs for the step
	std::map<stream_at, std::string> outputs;
	for (const auto& [out, val] : output.value()) {
		DBG(TAU_LOG_TRACE << "Output " << get_var_name<node>(out) << "[" << i.time_point << "] = `" << tau::get(val).to_str() <<"`";)
		DBG(TAU_LOG_TRACE << TAU_LOG_FM_DUMP(out);)
		DBG(TAU_LOG_TRACE << TAU_LOG_FM_DUMP(val);)
		outputs[{ get_var_name<node>(out), i.time_point }] =
							tau::get(val).to_str();
	}

	// Run update if update stream is present and unequal to 0
	if (tref update = get_update<node>(i, output.value()); update)
		i.update(update);

	if (!auto_continue) {
		TAU_LOG_TRACE << "auto continue is false.";
		return {};
	}

	return outputs;
}

} // namespace idni::tau_lang