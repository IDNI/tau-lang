// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "api.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "api"

namespace idni::tau_lang {

// Helper functions
// ------------------------------------------------------------

// Renders a tref->tref map (e.g. a solver solution) as printed strings,
// skipping entries with a null key or value.
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
	return get_term(term).has_value();
}

template <NodeType node>
bool api<node>::is_formula(const std::string& formula) {
	return get_formula(formula).has_value();
}

// Using definitions
// ------------------------------------------------------------

template <NodeType node>
result<std::string> api<node>::apply_def(
	const std::string& def, const std::string& expr)
{
	return apply_defs(std::set<std::string>{ def }, expr);
}

template <NodeType node>
result<std::string> api<node>::apply_defs(
	const std::set<std::string>& defs, const std::string& expr)
{
	result<std::string> r;
	subtree_set<node> tdefs;
	// A definition that fails to parse used to be inserted as nullptr and
	// then silently skipped by the tref-level apply_defs' "if (def)"
	// guard, so the caller had no way to tell a malformed definition was
	// dropped from a definition that legitimately had no effect. Report
	// the failure instead of silently continuing without it.
	for (const std::string& def : defs) {
		auto d = r.merge_take(get_definition(def));
		if (!d) {
			TAU_LOG_ERROR << "Failed to parse definition: " << def;
			DBG(assert(r.is_well_formed());)
			return r;
		}
		tdefs.insert(*d);
	}
	TAU_TRY(tref parsed, get_spec_or_term(expr));
	tref a = apply_defs(tdefs, parsed);
	if (!a) {
		return r.with_error(code::internal_error, "Failed to apply definitions");
	}
	return r.with_assert_check_value(to_str(a));
}

template <NodeType node>
result<std::string> api<node>::apply_all_defs(const std::string& expr) {
	return apply_defs(std::set<std::string>{}, expr);
}

// Substitution
// ------------------------------------------------------------

template <NodeType node>
result<std::string> api<node>::substitute(
	const std::string& expr,
	const std::string& that,
	const std::string& with)
{
	result<std::string> r;
	TAU_TRY(tref e, get_formula_or_term(expr));
	TAU_TRY(tref t, get_formula_or_term(that));
	TAU_TRY(tref w, get_formula_or_term(with));
	tref s = substitute(e, t, w);
	if (!s) {
		return r.with_error(code::invalid_argument, messages::failed_to_substitute);
	}
	return r.with_assert_check_value(to_str(s));
}

template <NodeType node>
result<std::string> api<node>::substitute(
	const std::string& expr,
	const std::map<std::string, std::string>& that_with)
{
	result<std::string> r;
	TAU_TRY(tref e, get_formula_or_term(expr));
	for (auto [that, with] : that_with) {
		TAU_TRY(tref t, get_formula_or_term(that));
		TAU_TRY(tref w, get_formula_or_term(with));
		e = substitute(e, t, w);
		if (!e) {
			return r.with_error(code::invalid_argument, messages::failed_to_substitute);
		}
	}
	return r.with_assert_check_value(to_str(e));
}


// Normal forms
// ------------------------------------------------------------

template <NodeType node>
result<std::string> api<node>::boole_normal_form(const std::string& expr)
{
	result<std::string> r;
	TAU_TRY(tref e, get_formula_or_term(expr));
	tref a = apply_all_defs(e);
	tref b = a ? tau_lang::boole_normal_form<node>(a) : nullptr;
	if (!b) {
		return r.with_error(code::internal_error, "Failed to compute boole normal form");
	}
	return r.with_assert_check_value(to_str(b));
}

template <NodeType node>
result<std::string> api<node>::dnf(const std::string& expr)
{
	result<std::string> r;
	TAU_TRY(tref e, get_formula_or_term(expr));
	tref out = dnf(e);
	if (!out) {
		return r.with_error(code::internal_error, "Failed to compute dnf");
	}
	return r.with_assert_check_value(to_str(out));
}

template <NodeType node>
result<std::string> api<node>::cnf(const std::string& expr)
{
	result<std::string> r;
	TAU_TRY(tref e, get_formula_or_term(expr));
	tref out = cnf(e);
	if (!out) {
		return r.with_error(code::internal_error, "Failed to compute cnf");
	}
	return r.with_assert_check_value(to_str(out));
}

template <NodeType node>
result<std::string> api<node>::nnf(const std::string& expr)
{
	result<std::string> r;
	TAU_TRY(tref e, get_formula_or_term(expr));
	tref out = nnf(e);
	if (!out) {
		return r.with_error(code::internal_error, "Failed to compute nnf");
	}
	return r.with_assert_check_value(to_str(out));
}

// Procedures
// ------------------------------------------------------------

template <NodeType node>
result<std::string> api<node>::syntactic_term_simplification(
	const std::string& term)
{
	result<std::string> r;
	TAU_TRY(tref e, get_term(term));
	tref out = syntactic_term_simplification(e);
	if (!out) {
		return r.with_error(code::internal_error, "Failed to simplify term");
	}
	return r.with_assert_check_value(to_str(out));
}

template <NodeType node>
result<std::string> api<node>::syntactic_formula_simplification(
	const std::string& fm)
{
	result<std::string> r;
	TAU_TRY(tref e, get_formula(fm));
	tref out = syntactic_formula_simplification(e);
	if (!out) {
		return r.with_error(code::internal_error, "Failed to simplify formula");
	}
	return r.with_assert_check_value(to_str(out));
}

template <NodeType node>
result<std::string> api<node>::normalize_term(const std::string& expr)
{
	result<std::string> r;
	TAU_TRY(tref term, get_term(expr));
	TAU_TRY(tref out, normalize_term(term));
	return r.with_assert_check_value(to_str(out));
}

template <NodeType node>
result<std::string> api<node>::normalize_formula(
	const std::string& expr)
{
	result<std::string> r;
	TAU_TRY(tref fm, get_formula(expr));
	TAU_TRY(tref out, normalize_formula(fm));
	return r.with_assert_check_value(to_str(out));
}

template <NodeType node>
result<std::string> api<node>::anti_prenex(const std::string& expr)
{
	result<std::string> r;
	TAU_TRY(tref fm, get_formula(expr));
	tref out = anti_prenex(fm);
	if (!out) {
		return r.with_error(code::internal_error, "Failed to anti-prenex formula");
	}
	return r.with_assert_check_value(to_str(out));
}

template <NodeType node>
result<std::string> api<node>::eliminate_quantifiers(
	const std::string& expr)
{
	result<std::string> r;
	TAU_TRY(tref e, get_formula(expr));
	tref a = apply_all_defs(e);
	tref q = a ? resolve_quantifiers<node>(tau_lang::anti_prenex<node>(a))
		   : nullptr;
	if (!q) {
		return r.with_error(code::internal_error, "Failed to eliminate quantifiers");
	}
	return r.with_assert_check_value(to_str(q));
}

template <NodeType node>
result<bool> api<node>::realizable(const std::string& expr) {
	result<bool> r;
	TAU_TRY(tref e, get_spec_or_term(expr));
	return realizable(e);
}

template <NodeType node>
result<bool> api<node>::unrealizable(const std::string& expr) {
	result<bool> r;
	TAU_TRY(tref e, get_spec_or_term(expr));
	return unrealizable(e);
}

template <NodeType node>
result<bool> api<node>::sat(const std::string& expr) {
	result<bool> r;
	TAU_TRY(tref e, get_spec_or_term(expr));
	return sat(e);
}

template <NodeType node>
result<bool> api<node>::unsat(const std::string& expr) {
	result<bool> r;
	TAU_TRY(bool v, sat(expr));
	return r.with_value(!v);
}

template <NodeType node>
result<bool> api<node>::valid(const std::string& expr) {
	result<bool> r;
	TAU_TRY(tref e, get_spec_or_term(expr));
	return valid(e);
}

template <NodeType node>
result<bool> api<node>::valid_spec(const std::string& expr) {
	result<bool> r;
	TAU_TRY(tref e, get_spec_or_term(expr));
	return valid_spec(e);
}


// Solving
// ------------------------------------------------------------

template <NodeType node>
result<std::map<std::string, std::string>> api<node>::solve(
	const std::string& formula,
	solver_mode mode)
{
	result<std::map<std::string, std::string>> r;
	TAU_TRY(tref fm, get_formula(formula));
	TAU_TRY(auto solution, solve(fm, mode));
	std::map<std::string, std::string> s;
	for (auto& [var, val] : solution)
		s.emplace(to_str(var), to_str(val));
	return r.with_assert_check_value(std::move(s));
}

template <NodeType node>
result<std::map<std::string, std::string>> api<node>::lgrs(
	const std::string& equation)
{
	result<std::map<std::string, std::string>> r;
	TAU_TRY(tref eq, get_formula(equation));
	TAU_TRY(auto solution, lgrs(eq));
	std::map<std::string, std::string> s;
	for (auto& [var, val] : solution)
		s.emplace(to_str(var), to_str(val));
	return r.with_assert_check_value(std::move(s));
}

// Execution
// ------------------------------------------------------------

template <NodeType node>
result<interpreter<node>> api<node>::get_interpreter(
	const std::string& specification)
{
	interpreter_options options;
	return get_interpreter(specification, options);
}

template <NodeType node>
result<interpreter<node>> api<node>::get_interpreter(
	const std::string& specification,
	interpreter_options& options)
{
	result<interpreter<node>> r;
	DBG(TAU_LOG_TRACE << "get_interpreter/specification: " << specification;);
	tau_spec<node> spec;
	if (!spec.parse(specification)) {
		for (const auto& error : spec.errors()) {
			TAU_LOG_ERROR << error;
			r.error(code::parse_error, error);
		}
		if (r.has_error()) {
			DBG(assert(r.is_well_formed());)
			return r;
		}
		return r.with_error(code::parse_error,
			messages::failed_to_parse_spec);
	}
	r = get_interpreter(spec, options);
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
std::string api<node>::current_spec(const interpreter<node>& i) {
	return TAU_TO_STR(i.current_spec_fm());
}

template <NodeType node>
size_t api<node>::spec_revision(const interpreter<node>& i) {
	return i.spec_revision();
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
result<std::map<stream_at, std::string>> api<node>::step(
	interpreter<node>& i, std::map<stream_at, std::string> inputs)
{
	DBG(using tau = tree<node>;)

	result<std::map<stream_at, std::string>> r;
	auto& ctx = i.ctx;

	if (!i.calculate_initial_spec()) {
		return r.with_error(code::internal_error, messages::failed_to_calculate_initial_spec);
	}

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
			return r.with_error(code::invalid_input_stream,
				"Input stream not found in context");
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
			return r.with_error(code::parse_error, "Failed to parse input value");
		}
		tref c = build_bf_ba_constant<node>(cnst.value().first, type_id);
		if (has_open_tau_fm_in_constant<node>(c)) {
			TAU_LOG_ERROR <<"Constant contains an open tau formula: "
								<< input_value;
			return r.with_error(code::invalid_argument,
				"Constant contains an open tau formula");
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
		// invalid_state is the step-awaiting-input protocol; see
		// step_awaiting_input() in tau_diagnostics.h.
		return r.with_error(code::invalid_state, messages::no_input_provided);
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
			return r.with_error(code::invalid_output_stream,
				"No Boolean algebra element assigned to output");
		}
		outputs[{ get_var_name<node>(out), i.time_point }] = ss.str();
	}

	// Run update if update stream is present and unequal to 0
	if (tref update = get_update<node>(i, output.value()); update)
		i.update(update);
	else warn_if_update_dropped<node>(i, output.value());

	// Not an error: no value and no report means the spec asked not to
	// continue. An empty result that does carry an error is a real failure.
	if (!auto_continue) {
		TAU_LOG_TRACE << "auto continue is false.";
		return r;
	}

	return r.with_assert_check_value(std::move(outputs));
}

template <NodeType node>
result<std::map<stream_at, std::string>> api<node>::step(
	interpreter<node>& i)
{
	using tau = tree<node>;

	result<std::map<stream_at, std::string>> r;
	if (!i.calculate_initial_spec()) {
		return r.with_error(code::internal_error, messages::failed_to_calculate_initial_spec);
	}

	// Step the interpreter
	auto [output, auto_continue] = i.step();
	if (!output.has_value()) {
		DBG(TAU_LOG_TRACE << "No input provided or error."
			<< " Quit at time point " << i.time_point;)
		// invalid_state is the step-awaiting-input protocol; see
		// step_awaiting_input() in tau_diagnostics.h.
		return r.with_error(code::invalid_state, messages::no_input_provided);
	}

	// Write output values
	if (!i.write(output.value())) {
		TAU_LOG_ERROR << "Failed to write outputs";
		return r.with_error(code::io_error, "Failed to write outputs");
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
	else warn_if_update_dropped<node>(i, output.value());

	// Not an error: no value and no report means the spec asked not to
	// continue. An empty result that does carry an error is a real failure.
	if (!auto_continue) {
		TAU_LOG_TRACE << "auto continue is false.";
		return r;
	}

	return r.with_assert_check_value(std::move(outputs));
}

template <NodeType node>
result<std::string> api<node>::simplify(const std::string& expr,
	bool use_defaults)
{
	result<std::string> r;
	TAU_TRY(tref e, get_formula_or_term(expr));
	tref out = simplify(e, use_defaults);
	if (!out) {
		return r.with_error(code::internal_error, "Failed to simplify");
	}
	return r.with_assert_check_value(to_str(out));
}

} // namespace idni::tau_lang
