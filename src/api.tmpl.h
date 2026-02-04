// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "api.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "api"

namespace idni::tau_lang {

template <NodeType node>
htref api<node>::parse_specification(const std::string& specification,
	io_context<node>& ctx)
{
	typename tree<node>::get_options options;
	options.context = &ctx;
	tref spec = tree<node>::get(specification, options);
	if (!spec) return {};
	return tree<node>::geth(spec);
}

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
	auto& ctx = *definitions<node>::instance().get_io_context();
	ctx.input_remaps = options.input_remaps;
	ctx.output_remaps = options.output_remaps;
	htref spec = parse_specification(specification, ctx);
	if (!spec) return {};
	auto maybe_nso_rr = get_nso_rr<node>(ctx, spec->get());
	if (!maybe_nso_rr) return {};
	tref applied = apply_rr_to_formula<node>(maybe_nso_rr.value());
	if (!applied) return {};
	tref normalized = normalizer<node>(applied);
	if (!normalized) return {};
	if (has_free_vars<node>(normalized)) return {};
	return interpreter<node>::make_interpreter(normalized, ctx);
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
tref get_update(interpreter<node>& i, const assignment<node>& output) {
	auto update_stream = build_out_var_at_n<node>(
		"u", i.time_point - 1, get_ba_type_id<node>(tau_type<node>()));
	// Update only if u is of type tau
	if (size_t t = i.ctx.type_of(update_stream);
		t != 0 && t == get_ba_type_id<node>(tau_type<node>()))
	{
		auto it = output.find(update_stream);
		if (it != output.end()
			&& !tree<node>::get(it->second).equals_0())
		{
			return unpack_tau_constant<node>(it->second);
		}
	}
	return nullptr;
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
		TAU_LOG_ERROR << "Failed to step interpreter at time point "
			<< i.time_point;
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


	return outputs;
}

// template <NodeType node>
// bool api<node>::validate_inputs(interpreter<node>& i, std::map<std::string, std::string> inputs) {
// }


} // namespace idni::tau_lang