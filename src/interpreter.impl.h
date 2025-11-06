// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include <string>

#include "ba_types.h"
#include "utility/term.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "interpreter"

namespace idni::tau_lang {

template <NodeType node>
typed_stream get_typed_stream(const tref type,
				const std::string& filename)
{
	return std::make_pair(get_ba_type_id<node>(type), dict(filename));
}

// -----------------------------------------------------------------------------
// finputs

template <NodeType node>
finputs<node>::finputs(const typed_io_vars& inputs) {
	// open the corresponding streams for input and store them in streams
	for (const auto& [var_sid, desc] : inputs) {
		tref var = build_var_name<node>(var_sid);
		this->types[var] = desc.first;
		this->streams[var] = desc.second == 0
			? std::optional<std::ifstream>()
			: std::ifstream(dict(desc.second));
		if (this->streams[var]
			&& !this->streams[var].value().is_open())
				LOG_ERROR << "Failed to open input file: '"
					<< dict(desc.second) << "'";
	}
}

template <NodeType node>
finputs<node>::finputs(finputs&& other) noexcept {
	types = std::move(other.types);
	streams = std::move(other.streams);
	time_point = other.time_point;
}

template <NodeType node>
finputs<node>::~finputs() {
	// close the streams
	for (auto& [_, file] : streams) if (file) file.value().close();
}

template <NodeType node>
void finputs<node>::add_input(tref var, size_t type_sid, size_t filename_sid) {
	// size_t var_sid = get_var_name_sid<node>(var);
	if (!types.contains(var)) {
		types.emplace(var, type_sid);
		streams.emplace(var, filename_sid == 0
			? std::optional<std::ifstream>()
			: std::ifstream(dict(filename_sid)));
		if (this->streams[var]
			&& !this->streams[var].value().is_open())
				LOG_ERROR << "Failed to open input file: '"
					<< dict(filename_sid) << "'";
	}
}

template <NodeType node>
std::optional<assignment<node>> finputs<node>::read() {
	DBG(LOG_TRACE << "read begin\n";)

	// for each stream in in streams, read the value from the file/stdin,
	// parsed it and store it in out.
	if (streams.empty()) return assignment<node>();
	assignment<node> current;
	for (auto& [var, file] : streams) {
		std::string line;
		if (file) {
			auto& fs = file.value();
			std::getline(fs, line);
			std::cout << line << "\n";
		} else {
			std::cout << get_var_name<node>(var) << "[" << time_point << "] := ";
			term::enable_getline_mode();
			std::getline(std::cin, line);
			term::disable_getline_mode();
			// TODO (MEDIUM) add echo for input from a file instead of console
		}
		if (line.empty()) return {}; // no more inputs
		auto cnst = ba_constants<node>::get(line,
			get_ba_type_tree<node>(types[var]));
		if (!cnst) {
			LOG_ERROR
				<< "Failed to parse input value '"
				<< line << "' for stream '"
				<< get_var_name<node>(var) <<
				" of type '" << get_ba_type_name<node>(types[var]) << "'\n";

			DBG(LOG_TRACE
				<< "read[result]: {}\n"
				<< "read end\n";)

			return {};
		}

		DBG(LOG_TRACE
			<< "read[var]: " << LOG_FM(var) << " = " << cnst.value().first << "\n"
			<< "read[types[var]]: " << ba_types<node>::name(types[var]) << "\n";)

		current[var] = tau::build_bf_ba_constant(cnst.value().first, types[var]);
	}
	time_point += 1;

	DBG(LOG_TRACE
			<< "read[time_point]: " << time_point << "\n"
			<< "read[result]: { ";
		for (const auto& [var, value] : current)
			LOG_TRACE << LOG_FM(var) << " = " << value << ", ";
		LOG_TRACE
			<< "}\nread end\n";)

	return current;
}

template <NodeType node>
std::pair<std::optional<assignment<node>>, bool> finputs<node>::read(
	trefs& in_vars, size_t time_step)
{
	DBG(LOG_TRACE
			<< "read begin\n"
			 << "read[time_point]: " << time_point << "\n"
			 << "read[in_vars]: { ";
		for (auto& var: in_vars) {
			LOG_TRACE << LOG_FM(var) << ", ";
		}
		LOG_TRACE << "}\n";)

	std::ranges::sort(in_vars, constant_io_comp<node>);
	assignment<node> value;
	for (tref var : in_vars) {

		DBG(LOG_TRACE << "read[var]: " << LOG_FM(var) << "\n";)

		// Skip output stream variables
		if (tau::get(var).is_output_variable())
			continue;
		// Skip input stream variables with time point greater time_step
		if (get_io_time_point<node>(var) > (int_t)time_step)
			continue;
		std::string line;
		tref vn = get_var_name_node<node>(var);
		if (auto it = streams.find(vn); it != streams.end()
			&& it->second.has_value())
		{
			std::getline(it->second.value(), line);
			std::cout << line << "\n";
		} else if (it == streams.end()) {
			LOG_ERROR
				<< "Failed to find input stream for stream '"
				<< get_var_name<node>(vn) << "'\n";

			DBG(LOG_TRACE << "read[result]: {}\n"
					 << "read end\n";)

			return {};
		} else {
			std::cout << tau::get(var) << " := ";
			term::enable_getline_mode();
			std::getline(std::cin, line);
			term::disable_getline_mode();
		}
		if (line.empty()) return { value, true }; // no more inputs
		const auto it = types.find(vn);
		if (it == types.end()) {
			LOG_ERROR
				<< "Failed to find type for "
				<< get_var_name<node>(vn);

			DBG(LOG_TRACE
				<< "read[result]: {}\n"
				<< "read end\n";)

			return {};
		}
		auto cnst = ba_constants<node>::get(line,
			get_ba_type_tree<node>(it->second));
		if (!cnst) {
			LOG_ERROR
				<< "Failed to parse input value '"
				<< line << "' for stream '"
				<< get_var_name<node>(var) <<
				" of type '" << get_ba_type_name<node>(types[vn]) << "'\n";
			DBG(LOG_TRACE
				<< "read[result]: {}\n"
				<< "read end\n";)

			return {};
		}

		tref wrapped_const = build_bf_ba_constant<node>(cnst.value().first, it->second);

		DBG(LOG_TRACE << "read[wrapped_const]: " << LOG_FM(wrapped_const) << "\n";)

		// Check that the input is a closed formula
		if (has_open_tau_fm_in_constant<node>(wrapped_const)) {

			DBG(LOG_TRACE
				<< "read[result]: {}\n"
				<< "read end\n";)
			return {};
		}
		value[tau::get(tau::bf, var)] = wrapped_const;
	}

	DBG(LOG_TRACE << "read end\n";)

	return { value, false };
}

template <NodeType node>
size_t finputs<node>::type_of(tref var) const {
	if (auto type = types.find(var);
		type != types.end()) return type->second;

	const std::string& name = get_var_name<node>(var);
	if (name.size() > 1 && name[0] == '_' && name[1] == 'e')
		return get_ba_type_id<node>(sbf_type<node>());

	// LOG_ERROR << "Failed to find type for stream: "
	// 	<< var << "\n";
	return 0;
}

template<NodeType node>
void finputs<node>::rebuild(const typed_io_vars& inputs) {
	// Delete old streams
	types.clear();
	streams.clear();
	// open the corresponding streams for input and store them in streams
	for (const auto& [var_sid, desc] : inputs) {
		tref var = build_var_name<node>(var_sid);
		this->types[var] = desc.first;
		this->streams[var] = desc.second == 0
			? std::optional<std::ifstream>()
			: std::ifstream(dict(desc.second));
		if (this->streams[var]
			&& !this->streams[var].value().is_open())
			LOG_ERROR << "Failed to open input file: '"
				<< dict(desc.second) << "'";
	}
}

// -----------------------------------------------------------------------------
// foutputs

template <NodeType node>
foutputs<node>::foutputs(const typed_io_vars& outputs) {
	// open the corresponding streams for input and store them in streams
	for (const auto& [var_sid, desc] : outputs) {
		LOG_TRACE << "output var_sid: " << var_sid;
		tref var = build_var_name<node>(var_sid);
		LOG_TRACE << "output var_name: " << LOG_FM_TREE(var);
		this->types[var] = desc.first;
		this->streams[var] = desc.second == 0
			? std::optional<std::ofstream>()
			: std::ofstream(dict(desc.second));
	}
}

template <NodeType node>
foutputs<node>::foutputs(foutputs&& other) noexcept {
	types = std::move(other.types);
	streams = std::move(other.streams);
}

template <NodeType node>
foutputs<node>::~foutputs() {
	// close the streams
	for (auto& [_, file]: streams) if (file) file.value().close();
}

template <NodeType node>
void foutputs<node>::add_output(tref var, size_t type_sid, size_t filename_sid) {
	if (!types.contains(var)) {
		types.emplace(var, type_sid);
		streams.emplace(var, filename_sid == 0
			? std::optional<std::ofstream>()
			: std::ofstream(dict(filename_sid)));
	}
}

template <NodeType node>
bool foutputs<node>::write(const assignment<node>& outputs) {
	// Sort variables in output by time
	trefs io_vars;
	for (const auto& [var, _ ] : outputs) {
		DBG(LOG_TRACE << LOG_FM_TREE(var));
		assert(tau::get(var)[0].child_is(tau::io_var));
		io_vars.push_back(var);
	}
	std::ranges::sort(io_vars, constant_io_comp<node>);

	// for each stream in out.streams, write the value from the solution
	for (tref io_var : io_vars) {
		// get the BA element associated with io_var_name
		DBG(LOG_TRACE << LOG_FM_TREE(io_var));
		tref var_name = get_var_name_node<node>(io_var);
		DBG(LOG_TRACE << LOG_FM_TREE(var_name));
		auto value = tt(outputs.find(io_var)->second) | tau::ba_constant;
		std::stringstream ss;
		if (!value) {
			// is bf_t
			if (auto check = tt(outputs.find(io_var)->second)
					| tau::bf_t; check) {
				size_t type = types.find(var_name)->second;
				ss << node::nso_factory::one(get_ba_type_tree<node>(type));
			// is bf_f
			} else if (auto check = tt(outputs.find(io_var)->second)
					| tau::bf_f; check) {
				size_t type = types.find(var_name)->second;
				ss << node::nso_factory::zero(get_ba_type_tree<node>(type));
			// is something else but not a BA element
			} else {
				LOG_ERROR << "No Boolean algebra element "
					<< "assigned to output '"
					<< TAU_TO_STR(io_var) << "'\n";
				return false;
			}
		} else {
			ss << (value | tt::ba_constant);
		}
		// get the out_var_name tag
		if (auto stream = streams.find(var_name); stream != streams.end())
			if (stream->second) stream->second.value() << ss.str() << "\n";
			else std::cout << tau::get(io_var) << " := " << ss.str() << "\n";
		else {
			if (auto name = get_var_name<node>(var_name);
				!name.empty() && name.front() == '_') continue;

			LOG_ERROR << "Failed to find output stream for stream '"
				<< get_var_name<node>(var_name) << "'\n";
			return false;
		}
	}
	return true; // success
}

template <NodeType node>
size_t foutputs<node>::type_of(tref var) const {
	if (auto type = types.find(var); type != types.end())
		return type->second;

	const std::string& name = get_var_name<node>(var);
	if (name.size() > 1 && name[0] == '_' && name[1] == 'e') {
		return get_ba_type_id<node>(sbf_type<node>());
	}

	// LOG_ERROR << "Failed to find type for stream '" << var << "'\n";
	return 0;
}

template<NodeType node>
void foutputs<node>::rebuild(const typed_io_vars& outputs) {
	// Delete old streams
	types.clear();
	streams.clear();
	// open the corresponding streams for input and store them in streams
	for (const auto& [var_sid, desc] : outputs) {
		tref var = build_var_name<node>(var_sid);
		this->types[var] = desc.first;
		this->streams[var] = desc.second == 0
			? std::optional<std::ofstream>()
			: std::ofstream(dict(desc.second));
	}
}


// -----------------------------------------------------------------------------
// interpreter


template <NodeType node, typename in_t, typename out_t>
interpreter<node, in_t, out_t>::interpreter(
	tref ubt_ctn, tref original_spec, assignment<node>& memory,
	in_t& input, out_t& output, const spec_context<node>& ctx)
	: ubt_ctn(ubt_ctn), original_spec(original_spec), memory(memory),
		inputs(std::move(input)), outputs(std::move(output)),
			ctx(ctx)
{
	compute_lookback_and_initial();
}

template <NodeType node, typename in_t, typename out_t>
std::optional<interpreter<node, in_t, out_t>>
	interpreter<node, in_t, out_t>::make_interpreter(tref spec,
						auto& inputs, auto& outputs,
						const auto& ctx)
{
	// Find a satisfiable unbound continuation from spec
	spec = normalizer<node>(spec);
	auto [ubd_ctn, clause] = get_executable_spec(spec);
	if (ubd_ctn == nullptr) {
		LOG_ERROR << "Tau specification is unsat\n";
		return {};
	}
	//after the above, we have the interpreter ready to be used.
	assignment<node> memory;
	return interpreter{ ubd_ctn, clause, memory, inputs, outputs, ctx };
}

template <NodeType node, typename in_t, typename out_t>
std::pair<std::optional<assignment<node>>, bool>
	interpreter<node, in_t, out_t>::step()
{
	// Compute systems for the current step
	if (!calculate_initial_spec()) return {};
	LOG_INFO << "Execution step: " << time_point << "\n";
	bool auto_continue = false;
	// Get inputs for this step
	auto [step_inputs, has_this_stream] = build_inputs_for_step(time_point);
	step_inputs = appear_within_lookback(step_inputs);
	// Get values for inputs which do not exceed time_point
	auto [values, is_quit] = inputs.read(step_inputs, time_point);
	DBG(if (values.has_value())
			for (auto [k, v] : values.value())
				LOG_DEBUG << "Input: " << LOG_FM_TREE(k) << " = " << LOG_FM_TREE(v) << "\n";)
	// Empty input
	if (is_quit) return {};
	// Error during input
	if (!values.has_value()) return { assignment<node>{}, true };
	// Save inputs in memory
	for (const auto& [var, value] : values.value()) {
		assert(get_io_time_point<node>(tau::trim(var)) <= (int_t)time_point);
		// If there is at least one input, continue automatically in execution
		auto_continue = true;
		memory[var] = value;
	}
	// If the "this" input stream is present, write the current spec into it
	if (has_this_stream) {
		tref current_this_stream = build_in_var_at_n<node>(
			"this", time_point, get_ba_type_id<node>(tau_type<node>()));
		tref wrapped_spec = build_bf_ba_constant<node>(
			node::nso_factory::pack_tau_ba(
				original_spec), get_ba_type_id<node>(tau_type<node>()));
		memory[current_this_stream] = wrapped_spec;
	}

	// Try to solve a path/disjunct of the current step specification
	for (tref path : expression_paths<node>(this->step_spec)) {
		// rewriting the inputs and inserting them into memory
		tref updated = update_to_time_point(path, formula_time_point);
		tref current = rewriter::replace<node>(updated, memory);
		// Simplify after updating stream variables
		// TODO: Maybe replace by syntactic simp?
		current = normalize_non_temp<node>(current);
#ifdef DEBUG
		LOG_TRACE << "step/equations: " << LOG_FM(path) << "\n"
			<< "step/updated: " << LOG_FM(updated) << "\n"
			<< "step/current: " << LOG_FM(current) << "\n"
			<< "step/memory: ";
		for (const auto& [k, v]: memory)
			LOG_TRACE << "\t" << k << " := " << v << " ";
#endif // DEBUG
		auto path_solution = solution_with_max_update(current);
#ifdef DEBUG
		if (path_solution) {
			LOG_TRACE << "step/solution: ";
			if (path_solution.value().empty())
				LOG_TRACE << "\t{}";
			else for (const auto& [k, v]: path_solution.value()) {
				LOG_TRACE << "\t" << TAU_TO_STR(k) << " := "
							<< TAU_TO_STR(v) << " ";
				LOG_TRACE << LOG_FM_TREE(k) << "\n";
				LOG_TRACE << LOG_FM_TREE(v) << "\n";
			}
			auto substituted = rewriter::replace<node>(
					current, path_solution.value());
			auto check = normalize_non_temp<node>(substituted);
			LOG_TRACE << "step/check: " << check << "\n";
		} else {
			LOG_TRACE << "step/solution: no solution\n";
		}
#endif // DEBUG
		// if (solution) {
		// 	// It can happen that variables are assigned
		// 	// to variables -> those need to be replaced
		// 	resolve_solution_dependencies(solution.value());
		// }
		if (path_solution) {
			solution<node> global;
			for (const auto& [var, value]: path_solution.value()) {
				// Check if we are dealing with a stream variable
				if (tt(var) | tau::variable | tau::io_var) {
					DBG(LOG_TRACE << LOG_FM_TREE(value));
					assert(tau::get(value).is(tau::bf));
					if (get_io_time_point<node>(tau::trim(var)) <= (int_t)time_point) {
						// std::cout << "time_point: " << time_point << "\n";
						// std::cout << "var: " << var << "\n";
						memory.emplace(var, value);
						// Exclude temporary streams in solution
						if (!is_excluded_output(tau::trim(var)))
							global.emplace(var, value);
					}
				} else {
					memory.emplace(var, value);
					global.emplace(var, value);
				}
			}
			// Complete outputs using time_point and current solution
			for (const auto& [o, _] : outputs.streams) {
				const size_t ctype = outputs.type_of(o);
				bool is_bv = is_bv_type_family<node>(ctype);
				tref ot = build_out_var_at_n<node>(o, time_point, ctype);
				if (auto it = global.find(ot); it == global.end()) {
					if (is_bv) {
						auto zero_bitvector = make_bitvector_zero(
							get_bv_size<node>(get_ba_type_tree<node>(ctype)));
						auto zero_term = tau::get(tau::bf, {
							tau::get_ba_constant(zero_bitvector, ctype)});
						memory.emplace(ot, zero_term);
						global.emplace(ot, zero_term);
					} else {
						memory.emplace(ot, tau::_0(ctype));
						global.emplace(ot, tau::_0(ctype));
					}
				}
			}
			if (global.empty())
				LOG_INFO << "currently no output is specified";
			// update time_point and formula_time_point
			if (time_point < formula_time_point) {
				// auto continue until lookback
				auto_continue = true;
				++time_point;
			} else {
				// auto continue until highest initial position
				if ((int_t) time_point < highest_initial_pos)
					auto_continue = true;
				++time_point;
				formula_time_point = time_point;
			}
			// TODO (HIGH) remove old values from memory
			return { global, auto_continue };
		}
	}
	LOG_ERROR << "Internal error: Tau specification is unexpectedly unsat\n";
	return {};
}

template <NodeType node, typename in_t, typename out_t>
tref interpreter<node, in_t, out_t>::get_ubt_ctn_at(int_t t) {
	LOG_TRACE << "get_ubt_ctn_at begin \n";
	LOG_TRACE << "get_ubt_ctn_at[t]: " << t << "\n";

	const int_t ut = t < (int_t)formula_time_point
					? (int_t)formula_time_point : t;
	if (t >= std::max(highest_initial_pos, (int_t)formula_time_point)) {
		return update_to_time_point(ubt_ctn, ut);
	}
	// Adjust ubt_ctn to time_point by eliminating inputs and outputs
	// which are greater than current time_point in a time-compatible fashion
	auto step_ubt_ctn = update_to_time_point(ubt_ctn, ut);
	auto io_vars = tau::get(step_ubt_ctn).select_top(
			is_child<node, tau::io_var>);
	std::sort(io_vars.begin(), io_vars.end(), constant_io_comp<node>);
	// All io_vars in fm have to refer to constant time positions
	assert(std::all_of(io_vars.begin(), io_vars.end(),
		[](const auto& el) { return is_io_initial<node>(el); }));
	while (!io_vars.empty()) {
		tref v = io_vars.back();
		if (get_io_time_point<node>(v) <= t) {
			io_vars.pop_back();
			continue;
		}
		if (tau::get(v).is_input_variable())
			step_ubt_ctn = build_wff_all<node>(v, step_ubt_ctn);
		else step_ubt_ctn = build_wff_ex<node>(v, step_ubt_ctn);
		io_vars.pop_back();
	}
	LOG_TRACE << "get_ubt_ctn_at[step_ubt_ctn]: " << tau::get(step_ubt_ctn) << "\n";

	// Eliminate added quantifiers
	auto result = normalize_non_temp<node>(step_ubt_ctn);
	LOG_TRACE << "get_ubt_ctn_at[result]: " << tau::get(result) << "\n";
	LOG_TRACE << "get_ubt_ctn_at end \n";
	return result;
}

template <NodeType node, typename in_t, typename out_t>
bool interpreter<node, in_t, out_t>::calculate_initial_spec() {
	LOG_TRACE << "calculate_initial_systems begin \n";
	if (final_system) return true;

	size_t initial_segment = std::max(highest_initial_pos, (int_t)formula_time_point);
	LOG_TRACE << "calculate_initial_systems[initial_segment]: " << initial_segment << "\n";
	LOG_TRACE << "calculate_initial_systems[time_point]: " << time_point << "\n";
	// If time_point < initial_segment, recompute systems
	if (time_point < initial_segment) {
		step_spec = get_ubt_ctn_at(time_point);
	} else if (time_point == initial_segment) {
		step_spec = ubt_ctn;
		final_system = true;
	}
	LOG_TRACE << "calculate_initial_systems[result]: true\n";
	LOG_TRACE << "calculate_initial_systems end\n";
	return true;
}

template <NodeType node, typename in_t, typename out_t>
std::pair<trefs, bool> interpreter<node, in_t, out_t>::build_inputs_for_step(
	const size_t t)
{
	LOG_TRACE << "build_inputs_for_step begin\n";
	trefs step_inputs;
	bool has_this_stream = false;
	for (auto& [var, _] : inputs.streams) {
		LOG_TRACE << "build_inputs_for_step[var]: " << LOG_FM_TREE(var);
		LOG_TRACE << "build_inputs_for_step[inputs.type_of(var)] " << inputs.type_of(var) << "\n";
		if (get_var_name<node>(var) == "this") {
			if (size_t vt = inputs.type_of(var);
				vt == get_ba_type_id<node>(tau_type<node>())) {
				has_this_stream = true;
				LOG_TRACE << "build_inputs_for_step[has_this_stream]: true\n";
				continue;
			}
		}
		step_inputs.emplace_back(tau::trim(
			build_in_var_at_n<node>(var, t, inputs.type_of(var))));
	}
	LOG_TRACE << "build_inputs_for_step end\n";
	return {step_inputs, has_this_stream};
}

template <NodeType node, typename in_t, typename out_t>
tref interpreter<node, in_t, out_t>::update_to_time_point(
	tref f, const int_t t) {
	LOG_TRACE << "update_to_time_point begin\n";
	// update the f according to current time_point, i.e. for each
	// input/output var which has a shift, we replace it with the value
	// corresponding to the current time_point minus the shift.
	auto io_vars = tau::get(f).select_top(is_child<node, tau::io_var>);
	auto result = fm_at_time_point<node>(f, io_vars, t);
	LOG_TRACE << "update_to_time_point[result]: " << tau::get(result) << "\n";
	LOG_TRACE << "update_to_time_point end\n";
	return result;
}

template <NodeType node, typename in_t, typename out_t>
bool interpreter<node, in_t, out_t>::is_memory_access_valid(const auto& io_vars)
{
	// Check for each constant time point accessing memory, if it is available
	for (tref io_var : io_vars) {
		if (is_io_initial<node>(io_var) &&
			get_io_time_point<node>(io_var) < (int_t)time_point) {
			const auto& v = tau::get(tau::bf, io_var);
			if (!memory.contains(v)) return false;
		}
	}
	return true;
}

template <NodeType node, typename in_t, typename out_t>
void interpreter<node, in_t, out_t>::resolve_solution_dependencies(
	solution<node>& s)
{
	for (auto& [v, a] : s) {
		if (is_child<node>(a, tau::variable)) {
			// The assigned value is a variables
			auto new_a = a;
			while (is_child<node>(new_a, tau::variable)) {
				auto it = s.find(new_a);
				if (it == s.end()) {
					LOG_ERROR << "Cannot eliminate variable"
							<< " in solution\n";
					break;
				}
				new_a = it->second;
			}
			a = new_a;
		}
	}
}

template <NodeType node, typename in_t, typename out_t>
void interpreter<node, in_t, out_t>::compute_lookback_and_initial() {
	trefs io_vars = tau::get(ubt_ctn).select_top(is_child<node, tau::io_var>);
	lookback = get_max_shift<node>(io_vars);
	formula_time_point = time_point + lookback;
	highest_initial_pos = get_max_initial<node>(io_vars);
}

template <NodeType node, typename in_t, typename out_t>
std::vector<system> interpreter<node, in_t, out_t>::compute_systems(tref ubd_ctn)
{
	std::vector<system> systems;
	// Create blue-print for solver for each clause
	for (tref clause : expression_paths<node>(ubd_ctn)) {
		if (auto system = compute_atomic_fm_types(clause); system)
			systems.emplace_back(std::move(system.value()));
		else {
			LOG_TRACE << "Unable to find all types in equations"
					<< " in: " << clause << "\n";
			continue;
		}
	}
	return systems;
}

template <NodeType node, typename in_t, typename out_t>
std::optional<system> interpreter<node, in_t, out_t>::compute_atomic_fm_types(
		tref clause) {
	DBG(LOG_TRACE << "compute_system/clause: " << LOG_FM(clause);)

	system sys;
	// Due to type inference all atomic formulas are typed
	for (tref atomic_fm : tau::get(clause).select_top(is_atomic_fm<node>())) {
		size_t type = find_ba_type<node>(atomic_fm);
		if (type == 0) {
			LOG_ERROR << "No type information found for "
				  << atomic_fm << "\n";
			return {};
		}
		if (!sys.contains(type)) sys[type] = atomic_fm;
		else sys[type] = build_wff_and<node>(sys[type], atomic_fm);
	}
	return { sys };
}

template <NodeType node, typename in_t, typename out_t>
std::pair<tref, tref> interpreter<node, in_t, out_t>::get_executable_spec(
	tref fm, const size_t start_time) {
	LOG_TRACE << "get_executable_spec begin\n";
	for (tref clause : expression_paths<node>(fm)) {
		DBG(LOG_TRACE << "compute_systems/clause: " << LOG_FM(clause);)
		tref executable = transform_to_execution<node>(clause, start_time, true);
		DBG(LOG_TRACE << "compute_systems/executable: " << LOG_FM(executable);)
		if (tau::get(executable).equals_F()) continue;
		// Make sure that no constant time position is smaller than 0
		trefs io_vars = tau::get(executable).select_top(
			is_child<node, tau::io_var>);
		for (tref io_var : io_vars) {
			if (is_io_initial<node>(io_var)
				&& get_io_time_point<node>(io_var) < 0)
			{
				LOG_ERROR << "Constant time position is smaller than 0\n";
				return std::make_pair(nullptr, nullptr);
			}
		}
		// compute model for uninterpreted constants and solve it
		tref constraints = get_uninterpreted_constants_constraints<node>(
							executable, io_vars);
		if (tau::get(constraints).equals_F()) continue;
		DBG(LOG_TRACE << "compute_systems/constraints: " << constraints;)
		tref spec = executable;
		if (!tau::get(constraints).equals_T()) {
			// setting proper options for the solver

			solver_options options = {
				.splitter_one = node::nso_factory::splitter_one(
					tau_type<node>()),
				.mode = solver_mode::general
			};
			bool solve_error = false;
			auto model = solve<node>(constraints, options, solve_error);
			if (solve_error) {
				LOG_ERROR << "Internal error in solver\n";
				return std::make_pair(nullptr, nullptr);
			}
			if (!model) continue;

			LOG_INFO << "Tau specification is executed setting ";
			for (const auto& [uc, v] : model.value())
				LOG_INFO << TAU_TO_STR(uc) << " := " << TAU_TO_STR(v);

			spec = rewriter::replace<node>(executable, model.value());
			LOG_INFO << "Resulting Tau specification: "	<< TAU_TO_STR(spec) << "\n";
			LOG_TRACE << "get_executable_spec[spec]: " << TAU_TO_STR(spec) << "\n";
		}
		LOG_TRACE << "get_executable_spec[spec]: " << spec;
		LOG_TRACE << "get_executable_spec end\n";
		return std::make_pair(spec, clause);
	}
	LOG_TRACE << "get_executable_spec[result]: {nullptr, nullptr}\n";
	LOG_TRACE << "get_executable_spec end\n";
	return std::make_pair(nullptr, nullptr);
}

template <NodeType node, typename in_t, typename out_t>
void interpreter<node, in_t, out_t>::update(tref update) {
	trefs io_vars = tau::get(update)
				.select_top(is_child<node, tau::io_var>);
	// the constant time positions in update are seen relative to
	// time_point, i.e. time point 0 is shifted to time_point
	tref shifted_update = shift_const_io_vars_in_fm<node>(
						update, io_vars, time_point);
	if (tau::get(shifted_update).equals_F()) {
		LOG_WARNING << "No update performed: constant time position below 0 was found\n";
		return;
	}
	io_vars = tau::get(shifted_update)
				.select_top(is_child<node, tau::io_var>);
	if (!is_memory_access_valid(io_vars)) {
		LOG_WARNING << "No update performed: invalid memory access was found\n";
		return;
	}
	shifted_update = rewriter::replace<node>(shifted_update, memory);
	LOG_TRACE << "update/shifted_update: " << LOG_FM(shifted_update) << "\n";

	// The constant time positions in original_spec need to be replaced
	// by present assignments from memory and already executed sometimes statements need to be removed
	tref current_spec = rewriter::replace<node>(original_spec, memory);
	LOG_DEBUG << "update/current_spec: " << LOG_FM(current_spec) << "\n";

	// TODO: current_spec = remove_happend_sometimes(current_spec);

	tref new_raw_spec = pointwise_revision(current_spec, shifted_update, time_point);
	LOG_DEBUG << "update/new_spec: " << LOG_FM(new_raw_spec) << "\n";
	// std::cout << "update/new_spec: " << new_raw_spec << "\n";
	if (tau::get(new_raw_spec).equals_F()) {
		LOG_WARNING << "No updated performed: updated specification is unsat\n";
		return;
	}

	// If the unbound continuation from start_time is possible,
	// it is safe to swap the current spec by update_unbound
	auto [new_ubd_ctn, new_spec] = get_executable_spec(new_raw_spec, time_point);
	if (new_ubd_ctn == nullptr) {
		LOG_WARNING << "No update performed: updated specification is unsat\n";
		return;
	}

	LOG_INFO << "Updated specification: \n" << TAU_TO_STR(new_spec) << "\n\n";

	// Set new specification for interpreter
	ubt_ctn = new_ubd_ctn;
	original_spec = new_spec;
	// The systems for solver need to be recomputed at beginning of next step
	final_system = false;
	compute_lookback_and_initial();
	outputs.rebuild(collect_output_streams(new_spec, ctx));
	inputs.rebuild(collect_input_streams(new_spec, ctx));
}

template <NodeType node, typename in_t, typename out_t>
tref interpreter<node, in_t, out_t>::pointwise_revision(
	tref spec, tref update, const int_t start_time)
{
	spec = normalizer<node>(spec);
	update = normalizer<node>(update);
	for (tref clause : expression_paths<node>(update)) {
		tref upd_always = tau::get(clause).find_top(
			is_child<node, tau::wff_always>);
		trefs upd_sometime = tau::get(clause).select_top(
			is_child<node, tau::wff_sometimes>);
		tref spec_always = tau::get(spec).find_top(
			is_child<node, tau::wff_always>);
		trefs spec_sometimes = tau::get(spec).select_top(
			is_child<node, tau::wff_sometimes>);

		// Check if the update by itself is sat from current time point onwards
		// taking the memory into account
		LOG_TRACE << "pwr/clause: " << LOG_FM(clause) << "\n";
		if (!is_tau_formula_sat<node>(clause, start_time))
			continue;

		// Now try to add always part of old spec in a pointwise way
		tref new_spec_pointwise = nullptr;
		// First try to conjunct update with previous always specification
		if (spec_always) {
			if (upd_always) {
				new_spec_pointwise = always_conjunction<node>(
					spec_always, upd_always);
			} else new_spec_pointwise = tau::trim2(spec_always);
			new_spec_pointwise = build_wff_always<node>(new_spec_pointwise);
			new_spec_pointwise = build_wff_and<node>(
				new_spec_pointwise,
				build_wff_and<node>(upd_sometime));
			if (!is_tau_formula_sat<node>(new_spec_pointwise, start_time)) {
				// Apply pointwise revision to always statements
				// if simply conjunction failed
				if (upd_always) {
					tref aw = always_conjunction<node>(
							spec_always, upd_always);
					trefs aw_io_vars = tau::get(aw).select_top(
						is_child<node, tau::io_var>);
					for (tref io_var : aw_io_vars)
						if (tau::get(io_var).is_output_variable())
							aw = build_wff_ex<node>(
								io_var, aw);
					new_spec_pointwise = build_wff_or<node>(
						always_conjunction<node>(
							build_wff_neg<node>(aw),
							upd_always),
						always_conjunction<node>(
							spec_always,
							upd_always)
					);
				} else new_spec_pointwise = tau::trim2(spec_always);
				new_spec_pointwise = build_wff_always<node>(
					new_spec_pointwise);
				new_spec_pointwise = build_wff_and<node>(
					new_spec_pointwise,
					build_wff_and<node>(upd_sometime));

				LOG_TRACE << "pwr/new_spec_pointwise: "
				<< LOG_FM(new_spec_pointwise) << "\n";
				if (!is_tau_formula_sat<node>(
					new_spec_pointwise, start_time))
					return clause;
			}
		} else new_spec_pointwise = clause;

		if (spec_sometimes.empty())
			return normalize<node>(new_spec_pointwise);
		// Now try to add sometimes part of old spec
		tref new_spec_pointwise_sometimes =
			build_wff_and<node>(new_spec_pointwise,
				build_wff_and<node>(spec_sometimes));

		LOG_TRACE << "pwr/new_spec_pointwise_sometimes: "
			<< LOG_FM(new_spec_pointwise_sometimes) << "\n";
		if (!is_tau_formula_sat<node>(new_spec_pointwise_sometimes, start_time))
			return normalize<node>(new_spec_pointwise);

		return normalize_with_temp_simp<node>(new_spec_pointwise_sometimes);
	}
	// If no clause is sat, return F
	return tau::_F();
}

template <NodeType node, typename in_t, typename out_t>
std::optional<assignment<node>> interpreter<node, in_t, out_t>::
solution_with_max_update(tref spec) {
	auto get_solution = [](const auto& fm) {
		// setting proper options for the solver
		solver_options options = {
			.splitter_one = node::nso_factory::splitter_one(tau_type<node>()),
			.mode = solver_mode::general
		};
		// solve the given system of equations
		bool solve_error = false;
		std::optional<solution<node>> s = solve<node>(fm, options, solve_error);
		if (solve_error) {
			LOG_ERROR << "Internal error in solver\n";
			return std::optional<solution<node>>();
		}
		return s;
	};
	tref u = build_out_var_at_n<node>("u", time_point,
		tau_type_id<node>());
	auto is_u_stream = [&u](const auto& n) {
		return n == u;
	};
	for (tref path : expression_paths<node>(spec)) {
		// Find update stream in clause
		tref update = tau::get(path).find_top(is_u_stream);
		// If there is no update in clause
		if (!update) continue;

		// Obtain single f = 0 part of clause
		tref f = squeeze_positives<node>(path, tau_type_id<node>());
		// If no positive parts exists, the update cannot be maximized
		if (!f) continue;

		// Check that f is wide (not 0 and has more than one zero), otherwise continue
		f = bf_reduced_dnf<node>(f);
		if (tau::get(f).equals_0()) continue;
		tref f0 = rewriter::replace<node>(f, u, tau::_0(
			get_ba_type_id<node>(tau_type<node>())));
		tref f1 = rewriter::replace<node>(f, u, tau::_1(
			get_ba_type_id<node>(tau_type<node>())));
		tref f0_xor_f1 = bf_reduced_dnf<node>(build_bf_xor<node>(f0, f1));
		if (tau::get(f0_xor_f1).equals_0()
			|| tau::get(f0_xor_f1).equals_1()) continue;

		// Here we know that f is wide
		tref max_u = build_bf_neg<node>(f1);
		tref max_u_spec = rewriter::replace<node>(path, u, max_u);
		auto sol = get_solution(max_u_spec);
		if (!sol.has_value()) continue;
		// Now we need to add solution for u[t]
		max_u = rewriter::replace<node>(max_u, sol.value());
		max_u = bf_reduced_dnf<node>(
			replace_free_vars_by<node>(max_u,
			tau::_0_trimmed(find_ba_type<node>(max_u))));
		sol.value().emplace(u, max_u);
		return sol;
	}
	// In case there is no maximal solution for u on any path of spec
	return get_solution(spec);
}

template <NodeType node, typename in_t, typename out_t>
bool interpreter<node, in_t, out_t>::is_excluded_output(tref var) {
	if (tau::get(var).is_input_variable()) return false;
	const std::string& io_name = get_var_name<node>(var);
	return io_name[0] == '_' && io_name.size() > 1 &&
		(io_name[1] == 'e' || io_name[1] == 'f');
}

template <NodeType node, typename in_t, typename out_t>
trefs interpreter<node, in_t, out_t>::appear_within_lookback(const trefs& vars){
	trefs appeared;
	for (size_t t = time_point; t <= time_point + (size_t)lookback; ++t) {
		tref step_ubt_ctn = update_to_time_point(ubt_ctn,
			t < formula_time_point ? formula_time_point : t);
		step_ubt_ctn = rewriter::replace<node>(step_ubt_ctn, memory);
		// We only apply a heuristic in order to decide if the variable still appears
		step_ubt_ctn = syntactic_formula_simplification<node>(step_ubt_ctn);
		// Try to find var in step_ubt_ctn
		for (tref v : vars) {
			if (contains<node>(step_ubt_ctn, v))
				if (std::ranges::find_if(
					appeared, [&v](const auto& n) {
						return tau::get(n) == tau::get(v);
					}) == appeared.end())
						appeared.emplace_back(v);
		}
	}
	return appeared;
}

template <NodeType node>
tref unpack_tau_constant(tref constant) {
	using tau = tree<node>;
	const auto& c = tree<node>::get(tau::trim(constant));
	if (!c.is_ba_constant()) return {};
	tref main = node::nso_factory::unpack_tau_ba(c.get_ba_constant());
	return main;
}

// returns true if there is a free variable in formula fm
// prints error messages by default
template <NodeType node>
bool has_free_vars(tref fm, bool silent = false) {
	using tau = tree<node>;
	const trefs& free_vars = get_free_vars<node>(fm);
	if (!free_vars.empty()) {
		// all elements of the set must be quantified
		std::stringstream ss; bool has_real_free_vars = false;
		for (auto it = free_vars.begin(), end = free_vars.end(); it != end; ++it) {
			if (is_child<node>(*it, tau::io_var)) {
				const tau& io_var_node = tau::get(*it)[0];
				if (!io_var_node.is_input_variable() && !
				io_var_node.is_output_variable()) {
					if (!silent) LOG_ERROR << "The stream "
					<< io_var_node
					<< " is not defined as an input or output stream\n";
					return true;
				}
			} else if (!is_child<node>(*it, tau::uconst_name)) {
				ss << tau::get(*it) << " ";
				has_real_free_vars = true;
			}
		}
		if (has_real_free_vars) {
			if (!silent) LOG_ERROR << "The following variable(s) must be "
				<< "quantified and cannot appear free: "
				<< ss.str() << "\n";
			return true;
		}
	}
	return false;
}

template <NodeType node, typename in_t, typename out_t>
std::optional<interpreter<node, in_t, out_t>> run(tref form,
	in_t& inputs, out_t& outputs, const auto& ctx, const size_t steps)
{
	DBG(LOG_TRACE << "run begin\n";
		LOG_TRACE << "run[form]: " << LOG_FM(form) << "\n";
		LOG_TRACE << "run[steps]: " << steps << "\n";)

	using tau = tree<node>;
	DBG(LOG_TRACE << "run[form]: " << LOG_FM(form) << "\n");
	auto intrprtr_o = interpreter<node, in_t, out_t>::make_interpreter(
							form, inputs, outputs,
							ctx);
	if (!intrprtr_o) return {};
	auto& intrprtr = intrprtr_o.value();

	LOG_INFO << "-----------------------------------------------------------------------------------------------------------";
	LOG_INFO << "Please provide requested input, or press ENTER to terminate                                               |";
	LOG_INFO << "If no input is requested, press ENTER to continue to the next execution step, or type q(uit) to terminate |";
	LOG_INFO << "-----------------------------------------------------------------------------------------------------------\n\n";

	// Continuously perform execution step until user quits
	while (true) {
		auto [output, auto_continue] = intrprtr.step();

		DBG(LOG_TRACE << "run[output]: ";
			if (output.has_value()) {
				LOG_TRACE << "{ ";
				for (const auto& [v, val] : output.value())
					LOG_TRACE << LOG_FM_TREE(v) << " := " << LOG_FM_TREE(val) << "; ";
				LOG_TRACE << "}\n";
			} else LOG_TRACE << "no output\n";
			LOG_TRACE << "run[auto_continue]: " << auto_continue << "\n";
		)

		// If the user provided empty input for an input stream, quit
		if (!output.has_value()) break;
		if (!intrprtr.outputs.write(output.value())) break;
		// If there is no input, ask the user if execution should continue
		if (!auto_continue && steps == 0) {
			std::string line;
			term::enable_getline_mode();
			std::getline(std::cin, line);
			term::disable_getline_mode();
			if (line == "q" || line == "quit") break;
		} else std::cout << "\n";

		// Update interpreter in case the output stream u is present and unequal to 0
		auto update_stream = build_out_var_at_n<node>(
			"u", intrprtr.time_point - 1, get_ba_type_id<node>(tau_type<node>()));
		// Update only if u is of type tau
		if (size_t t = intrprtr.outputs.type_of(get_var_name_node<node>(update_stream));
			t != 0 && t == get_ba_type_id<node>(tau_type<node>()))
		{
			auto it = output.value().find(update_stream);
			if (it != output.value().end() && !tau::get(it->second)
				.equals_0())
			{
				if (tref update = unpack_tau_constant<node>(
					it->second); update != nullptr)
				{
					DBG(LOG_TRACE << "update: "	<< update << "\n";)
					intrprtr.update(update);
				}
			}
		}
		if (steps != 0 && intrprtr.time_point == steps) break;
	}
	DBG(LOG_TRACE << "run end\n";)
	return intrprtr_o;
}

template<NodeType node>
typed_io_vars collect_input_streams(tref dnf, const spec_context<node>& ctx) {
	using tau = tree<node>;
	// select current input variables
	auto is_in_var = [](tref n) {
		const tau& tn = tau::get(n);
		if (tn.is(tau::variable))
			return tn[0].is_input_variable();
		return false;
	};
	trefs in_vars = tau::get(dnf).select_all(is_in_var);
	typed_io_vars current_inputs;
	for (tref var : in_vars) {
		size_t var_sid = get_var_name_sid<node>(var);
		// Get type of current input stream
		if (size_t type = tau::get(var).get_ba_type(); type > 0) {
			// input stream has type
			if (auto it = ctx.inputs.find(var_sid); it != ctx.inputs.end()) {
				// Check also predefined streams
				if (type != it->second.first) {
					TAU_LOG_ERROR << "Type mismatch due to predefinition detected for "
					<< tau::get(var).to_str() << "\n";
					return {};
				}
				current_inputs[var_sid] = it->second;
			} else current_inputs.emplace(var_sid, std::make_pair(type, 0));
		} else if (auto it = ctx.inputs.find(var_sid); it != ctx.inputs.end()) {
			// stream has predefined type
			current_inputs.emplace(var_sid, it->second);
		} else {
			// Untyped io stream error
			TAU_LOG_ERROR << "The following input stream must be typed: "
					<< tau::get(var).to_str() << "\n";
			return {};
		}
	}
	return current_inputs;
}

template<NodeType node>
typed_io_vars collect_output_streams(tref dnf, const spec_context<node>& ctx) {
	using tau = tree<node>;
	// select current output variables
	auto is_out_var = [](tref n) {
		const tau& tn = tau::get(n);
		if (tn.is(tau::variable))
			return tn[0].is_output_variable();
		return false;
	};
	trefs out_vars = tau::get(dnf).select_all(is_out_var);
	typed_io_vars current_outputs;
	for (tref var : out_vars) {
		size_t var_sid = get_var_name_sid<node>(var);
		// Get type of current output stream
		if (size_t type = tau::get(var).get_ba_type(); type > 0) {
			if (auto it = ctx.outputs.find(var_sid); it != ctx.outputs.end()) {
				// Check also predefined streams
				if (type != it->second.first) {
					TAU_LOG_ERROR << "Type mismatch due to predefinition detected for: "
					<< tau::get(var).to_str() << "\n";
					return {};
				}
				current_outputs.emplace(var_sid, it->second);
			} else current_outputs.emplace(var_sid, std::make_pair(type, 0));
		} else if (auto it = ctx.outputs.find(var_sid); it != ctx.outputs.end()) {
			// stream has predefined type
			current_outputs.emplace(var_sid, it->second);
		} else {
			// Untyped io stream error
			TAU_LOG_ERROR << "The following input stream must be typed: "
					<< tau::get(var).to_str() << "\n";
			return {};
		}
	}
	return current_outputs;
}
} // namespace idni::tau_lang
