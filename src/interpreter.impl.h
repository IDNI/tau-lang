// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include <string>

#include "interpreter.h"

#include "utility/term.h"

namespace idni::tau_lang {

template <NodeType node>
typed_stream get_typed_stream(const std::string& type,
				const std::string& filename)
{
	return std::make_pair(get_ba_type_id<node>(type), string_id(filename));
}

// -----------------------------------------------------------------------------
// finputs

template <NodeType node>
finputs<node>::finputs(typed_io_vars inputs) {
	// open the corresponding streams for input and store them in streams
	for (const auto& [var_sid, desc] : inputs) {
		tref var = build_var_name<node>(var_sid);
		this->types[var] = desc.first;
		this->streams[var] = desc.second == 0
			? std::optional<std::ifstream>()
			: std::ifstream(string_from_id(desc.second));
		if (this->streams[var]
			&& !this->streams[var].value().is_open())
				LOG_ERROR << "Failed to open input file: '"
					<< string_from_id(desc.second) << "'";
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
			: std::ifstream(string_from_id(filename_sid)));
		if (this->streams[var]
			&& !this->streams[var].value().is_open())
				LOG_ERROR << "Failed to open input file: '"
					<< string_from_id(filename_sid) << "'";
	}
}

template <NodeType node>
std::optional<assignment<node>> finputs<node>::read() {
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
			// TODo (MEDIUM) add echo for input from a file instead of console
		}
		if (line.empty()) return {}; // no more inputs
		auto cnst = node::nso_factory::instance().parse(line,
			get_ba_type_name<node>(types[var]));
		if (!cnst) {
			LOG_ERROR << "Failed to parse input value '"
				<< line << "' for stream '"
				<< get_var_name<node>(var) << "'\n";
			return {};
		}
		current[var] =
			build_bf_ba_constant<node>(cnst.value(), types[var]);
	}
	time_point += 1;
	return current;
}

template <NodeType node>
std::pair<std::optional<assignment<node>>, bool> finputs<node>::read(
	trefs& in_vars, size_t time_step)
{
	std::ranges::sort(in_vars, constant_io_comp<node>);
	assignment<node> value;
	for (tref var : in_vars) {
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
			LOG_ERROR << "Failed to find input stream for stream '"
				<< get_var_name<node>(vn) << "'\n";
			return {};
		} else {
			std::cout << get_var_name<node>(vn) << " := ";
			term::enable_getline_mode();
			std::getline(std::cin, line);
			term::disable_getline_mode();
		}
		if (line.empty()) return { value, true }; // no more inputs
		const auto it = types.find(vn);
		if (it == types.end()) {
			LOG_ERROR << "Failed to find type for "
				<< get_var_name<node>(vn);
			return {};
		}
		auto cnst = node::nso_factory::instance().parse(line,
				get_ba_type_name<node>(it->second));
		if (!cnst) {
			LOG_ERROR << "Failed to parse input value '" << line
				<< "' for stream '" << get_var_name<node>(vn)
				<< "'\n";
			return {};
		}
		tref wrapped_const = build_bf_ba_constant<node>(
						cnst.value(), it->second);
		// Check that the input is a closed formula
		if (has_open_tau_fm_in_constant<node>(wrapped_const))
			return {};
		value[tau::get(tau::bf, var)] = wrapped_const;
	}
	return { value, false };
}

template <NodeType node>
size_t finputs<node>::type_of(tref var) const {
	if (auto type = types.find(var);
		type != types.end()) return type->second;

	const std::string& name = get_var_name<node>(var);
	if (name.size() > 1 && name[0] == '_' && name[1] == 'e')
		return get_ba_type_id<node>("sbf");

	// LOG_ERROR << "Failed to find type for stream: "
	// 	<< var << "\n";
	return 0;
}

// -----------------------------------------------------------------------------
// foutputs

template <NodeType node>
foutputs<node>::foutputs(typed_io_vars outputs) {
	// open the corresponding streams for input and store them in streams
	for (const auto& [var_sid, desc] : outputs) {
		tref var = build_var_name<node>(var_sid);
		this->types[var] = desc.first;
		this->streams[var] = desc.second == 0
			? std::optional<std::ofstream>()
			: std::ofstream(string_from_id(desc.second));
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
			: std::ofstream(string_from_id(filename_sid)));
	}
}

template <NodeType node>
bool foutputs<node>::write(const assignment<node>& outputs) {
	// Sort variables in output by time
	trefs io_vars;
	for (const auto& [var, _ ] : outputs) {
		assert(tau::get(var)[0].child_is(tau::io_var));
		io_vars.push_back(var);
	}
	std::ranges::sort(io_vars, constant_io_comp<node>);

	// for each stream in out.streams, write the value from the solution
	for (tref io_var : io_vars) {
		// get the BA element associated with io_var_name
		tref var = get_var_name_node<node>(io_var);
		auto value = tt(outputs.find(var)->second) | tau::bf_constant;
		std::stringstream ss;
		if (!value) {
			// is bf_t
			if (auto check = tt(outputs.find(var)->second)
					| tau::bf_t; check) {
				size_t type = types.find(var)->second;
				ss << node::nso_factory::instance()
					.one(get_ba_type_name<node>(type));
			// is bf_f
			} else if (auto check = tt(outputs.find(var)->second)
					| tau::bf_f; check) {
				size_t type = types.find(var)->second;
				ss << node::nso_factory::instance()
					.zero(get_ba_type_name<node>(type));
			// is something else but not a BA element
			} else {
				LOG_ERROR << "No Boolean algebra element "
					<< "assigned to output '"
					<< TAU_TO_STR(var) << "'\n";
				return false;
			}
		} else {
			ss << value.value();
		}
		// get the out_var_name tag
		if (auto stream = streams.find(var); stream != streams.end())
			if (stream->second) stream->second.value() << ss.str() << "\n";
			else std::cout << get_var_name<node>(var) << " := " << ss.str() << "\n";
		else {
			if (auto name = get_var_name<node>(var);
				!name.empty() && name.front() == '_') continue;

			LOG_ERROR << "Failed to find output stream for stream '"
				<< get_var_name<node>(var) << "'\n";
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
		return get_ba_type_id<node>("sbf");
	}

	// LOG_ERROR << "Failed to find type for stream '" << var << "'\n";
	return 0;
}


// -----------------------------------------------------------------------------
// interpreter


template <NodeType node, typename in_t, typename out_t>
interpreter<node, in_t, out_t>::interpreter(
	tref ubt_ctn, tref original_spec, assignment<node>& memory,
	in_t& input, out_t& output)
	: ubt_ctn(ubt_ctn), original_spec(original_spec), memory(memory),
		inputs(std::move(input)), outputs(std::move(output))
{
	compute_lookback_and_initial();
	// Put all streams in ubt_ctn into outputs/inputs
	compute_systems(this->ubt_ctn, inputs, outputs);
}

template <NodeType node, typename in_t, typename out_t>
std::optional<interpreter<node, in_t, out_t>>
	interpreter<node, in_t, out_t>::make_interpreter(tref spec,
						auto& inputs, auto& outputs)
{
	// Find a satisfiable unbound continuation from spec
	auto [ubd_ctn, clause] = get_executable_spec(spec);
	if (ubd_ctn == nullptr) {
		LOG_ERROR << "Tau specification is unsat\n";
		return {};
	}
	//after the above, we have the interpreter ready to be used.
	assignment<node> memory;
	return interpreter{ ubd_ctn, clause, memory, inputs, outputs };
}

template <NodeType node, typename in_t, typename out_t>
std::pair<std::optional<assignment<node>>, bool>
	interpreter<node, in_t, out_t>::step()
{
	// Compute systems for the current step
	if (!calculate_initial_systems()) return {};
	LOG_INFO << "Execution step: " << time_point << "\n";
	bool auto_continue = false;
	// Get inputs for this step
	auto step_inputs = build_inputs_for_step(time_point);
	step_inputs = appear_within_lookback(step_inputs);
	// Get values for inputs which do not exceed time_point
	auto [values, is_quit] = inputs.read(step_inputs, time_point);
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
	// for each system in systems try to solve it, if it is not possible
	// continue with the next system.
	for (const auto& system : this->systems) {
		std::map<size_t, solution<node>> solutions;
		bool solved = true;
		// solve the equations for each type in the system
		for (const auto& [ba_type_id, equations] : system) {
			// rewriting the inputs and inserting them into memory
			tref updated = update_to_time_point(equations, formula_time_point);
			tref current = rewriter::replace<node>(updated, memory);
			// Simplify after updating stream variables
			current = normalize_non_temp<node>(current);

			#ifdef DEBUG
			LOG_TRACE << "step/type: " << ba_type_id << "\n"
				<< "step/equations: " << equations << "\n"
				<< "step/updated: " << updated << "\n"
				<< "step/current: " << current << "\n"
				<< "step/memory: ";
			for (const auto& [k, v]: memory)
				LOG_TRACE << "\t" << k << " := " << v << " ";
			#endif // DEBUG

			auto solution = solution_with_max_update(current);

			#ifdef DEBUG
			if (solution) {
				LOG_TRACE << "step/solution: ";
				if (solution.value().empty())
					LOG_TRACE << "\t{}";
				else for (const auto& [k, v]: solution.value())
					LOG_TRACE << "\t" << k << " := "
								<< v << " ";
				auto substituted = rewriter::replace<node>(
						current, solution.value());
				auto check = snf_wff<node>(substituted);
				LOG_TRACE << "step/check: " << check << "\n";
			} else {
				LOG_TRACE << "step/solution: no solution\n";
			}
			#endif // DEBUG

			if (solution.has_value()) {
				// std::cout << "solution: \n";
				// for (const auto& [var, val] : solution.value()) {
				// 	std::cout << "var: " << var << ", val: " << val << "\n";
				// }
				// It can happen that variables are assigned
				// to variables -> those need to be replaced
				resolve_solution_dependencies(solution.value());
				solutions[ba_type_id] = solution.value();
			}
			else {
				solved = false;
				// We need to clear the solutions to current clause since it is unsat
				solutions.clear();
				break;
			}
		}
		if (solved) {
			solution<node> global;
			// merge the solutions
			for (const auto& [ba_type_id, solution]: solutions) {
				for (const auto& [var, value]: solution) {
					// Check if we are dealing with a stream variable
					if (tt(var) | tau::variable | tau::io_var) {
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
			}
			// Complete outputs using time_point and current solution
			for (const auto& [o, _] : outputs.streams) {
				tref ot = build_out_var_at_n<node>(o, time_point);
				if (auto it = global.find(ot); it == global.end()) {
					memory.emplace(ot, tau::_0());
					global.emplace(ot, tau::_0());
				}
			}
			if (global.empty())
				LOG_INFO << "currently no output is specified";
			// update time_point and formula_time_point
			if ((int_t) time_point < lookback) {
				// auto continue until lookback
				auto_continue = true;
				++time_point;
			}
			else {
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
	sort(io_vars.begin(), io_vars.end(), constant_io_comp<node>);
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
	// Eliminate added quantifiers
	return normalize_non_temp<node>(step_ubt_ctn);
}

template <NodeType node, typename in_t, typename out_t>
bool interpreter<node, in_t, out_t>::calculate_initial_systems() {
	if (final_system) return true;

	size_t initial_segment = std::max(highest_initial_pos, (int_t)formula_time_point);
	LOG_TRACE << "cis/initial_segment: " << initial_segment << "\n";
	LOG_TRACE << "cis/time_point: " << time_point << "\n";
	// If time_point < initial_segment, recompute systems
	if (time_point < initial_segment) {
		auto step_ubt_ctn = get_ubt_ctn_at(time_point);
		systems = compute_systems(step_ubt_ctn, inputs, outputs);
		if (systems.empty()) return false;
	} else if (time_point == initial_segment) {
		systems = compute_systems(ubt_ctn, inputs, outputs);
		final_system = true;
		if (systems.empty()) return false;
	}
	return true;
}

template <NodeType node, typename in_t, typename out_t>
std::vector<tref> interpreter<node, in_t, out_t>::build_inputs_for_step(
	const size_t t)
{
	trefs step_inputs;
	for (auto& [var, _] : inputs.streams)
		step_inputs.emplace_back(tau::trim(
					build_in_var_at_n<node>(var, t)));
	return step_inputs;
}

template <NodeType node, typename in_t, typename out_t>
tref interpreter<node, in_t, out_t>::update_to_time_point(
	tref f, const int_t t) {
	// update the f according to current time_point, i.e. for each
	// input/output var which has a shift, we replace it with the value
	// corresponding to the current time_point minus the shift.
	auto io_vars = tau::get(f).select_top(is_child<node, tau::io_var>);
	return fm_at_time_point<node>(f, io_vars, t);
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
	trefs io_vars = tau::get(ubt_ctn).select_top(
					is_child<node, tau::io_var>);
	lookback = get_max_shift<node>(io_vars);
	formula_time_point = time_point + lookback;
	highest_initial_pos = get_max_initial<node>(io_vars);
}

template <NodeType node, typename in_t, typename out_t>
std::vector<system> interpreter<node, in_t, out_t>::compute_systems(
	tref ubd_ctn, auto& inputs, auto& outputs)
{
	std::vector<system> systems;
	// Create blue-print for solver for each clause
	for (tref clause : get_dnf_wff_clauses<node>(ubd_ctn)) {
		if (auto system = compute_atomic_fm_types(clause, inputs, outputs); system)
			systems.emplace_back(std::move(system.value()));
		else {
			LOG_TRACE << "unable to compute all types in equations"
					<< " in: " << clause << "\n";
			continue;
		}
	}
	return systems;
}

template <NodeType node, typename in_t, typename out_t>
std::optional<system> interpreter<node, in_t, out_t>::compute_atomic_fm_types(
	tref clause, auto& inputs, auto& outputs)
{
	auto is_atomic_fm = [](tref n) {
		return is_child<node, tau::bf_eq>(n)
			|| is_child<node, tau::bf_neq>(n);
	};

	DBG(LOG_TRACE << "compute_system/clause: " << TAU_TO_STR(clause);)

	typename tau::subtree_set pending_atomic_fms;
	for (tref atomic_fm : tau::get(clause).select_top(is_atomic_fm)) {
		DBG(LOG_TRACE << "compute_system/atomic_fm " << TAU_TO_STR(atomic_fm);)
		pending_atomic_fms.emplace(atomic_fm);
	}
	system sys;
	bool new_choice = true;
	while (new_choice) {
		new_choice = false;
		trefs to_erase_fms;
		for (tref fm : pending_atomic_fms) {
			if (size_t t = get_type_atomic_fm(fm, inputs, outputs);
				t > 0)
			{
				// Skip atomic fms which have no type yet
				to_erase_fms.push_back(fm);
				if (!sys.contains(t)) sys[t] = fm;
				else sys[t] = build_wff_and<node>(sys[t], fm);
				new_choice = true;
			} else {
				// Error message is already printed in get_type_fm
				return {};
			}
		}
		for (tref fm : to_erase_fms)
			pending_atomic_fms.erase(fm);
	}
	// All remaining formulas in pending_atomic_fms can be typed by default
	for (tref fm : pending_atomic_fms) {
		// std::cout << "def. type for: " << fm << "\n";
		trefs io_vars = tau::get(fm).select_top(
			is_child<node, tau::io_var>);
		// TODO: shouldn't be the default taken from nso_factory?
		size_t t = get_ba_type_id<node>("tau");
		type_io_vars(io_vars, t, inputs, outputs);
		if (sys.find(t) == sys.end()) sys[t] = fm;
		else sys[t] = build_wff_and<node>(sys[t], fm);
	}
	return { sys };
}

template <NodeType node, typename in_t, typename out_t>
void interpreter<node, in_t, out_t>::type_io_vars(
	trefs io_vars, size_t type, auto& inputs, auto& outputs)
{
	for (tref io_var : io_vars) if (tau::get(io_var).is_input_variable())
		// Add type to inputs
		inputs.add_input(get_var_name_node<node>(io_var), type, 0);
	else    // Add type to outputs
		if (!is_excluded_output(io_var)) outputs.add_output(
				get_var_name_node<node>(io_var), type, 0);
}

template <NodeType node, typename in_t, typename out_t>
size_t interpreter<node, in_t, out_t>::get_type_atomic_fm(tref fm,
		auto& inputs, auto& outputs)
{
	trefs io_vars = tau::get(fm).select_top(is_child<node, tau::io_var>);

	// Check if any io_var has a predefined type
	size_t type;
	for (tref io_var : io_vars) {
		if (size_t t = inputs.type_of(get_var_name_node<node>(io_var))) {
			if (type != 0 && type != t) {
				// Type mismatch in atomic fm
				LOG_ERROR
				<< "Stream variable type mismatch between '"
				<< get_ba_type_name<node>(type) <<"' and '"
				<< get_ba_type_name<node>(t)
				<< "' in atomic formula: " << TAU_TO_STR(fm)
				<< "\n";
				return 0;
			} else if (type == 0) type = t;
		}
		if (size_t t = outputs.type_of(get_var_name_node<node>(io_var))) {
			if (type != 0 && type != t) {
				// Type mismatch in atomic fm
				LOG_ERROR
				<< "Stream variable type mismatch between '"
				<< get_ba_type_name<node>(type)
				<< "' and '" << get_ba_type_name<node>(t)
				<< "' in atomic formula: " << TAU_TO_STR(fm)
				<< "\n";
				return 0;
			} else if (type == 0) type = t;
		}
	}
	// std::cout << "type before const: " << type << "\n";
	// Check if all constants match the type, if present, else infer from there
	trefs consts = tau::get(fm).select_top(is<node, tau::bf_constant>);
	for (tref c : consts) {
		size_t c_type = tau::get(c).get_ba_type();
		// std::cout << "c_type: " << c_type << "\n";
		if (type == 0) type = c_type;
		else if (type != c_type) {
			// Type mismatch in atomic fm
			LOG_ERROR << "Stream variable or constant type mismatch "
			<< "between '" << get_ba_type_name<node>(type)
			<< "' and '" << get_ba_type_name<node>(c_type)
			<< "' in atomic formula: " << TAU_TO_STR(fm)<<"\n";
			return 0;
		}
	}

	// Check if any other type information is available
	// following should be covered by ba type inference...
// 	if (tref alt_type = tau::get(fm).find_top(is<node, tau::type>)) {
// 		if (type != to_str(alt_type.value())) {
// 			// Type mismatch in atomic fm
// 			LOG_ERROR << "Stream variable or constant type mismatch"
//				<< " between '" << type << "' and '"
//				<< alt_type.value() << "' in atomic formula: "
//				<< fm << "\n";
// 			return {};
// 		} else type = to_str(alt_type.value());
// 	}

	// std::cout << "type: " << type << "\n";
	// No type information was found, delay typing until all equations have
	// been visited
	if (type == 0) return 0;

	// If a type is found, set all io_vars to found type
	type_io_vars(io_vars, type, inputs, outputs);
	return type;
}

template <NodeType node, typename in_t, typename out_t>
std::pair<tref, tref> interpreter<node, in_t, out_t>::get_executable_spec(
	tref fm, const size_t start_time)
{
	for (tref clause : get_dnf_wff_clauses<node>(fm)) {
		DBG(LOG_TRACE << "compute_systems/clause: " << TAU_TO_STR(clause);)
		tref executable = transform_to_execution<node>(clause, start_time, true);
		DBG(LOG_TRACE << "compute_systems/executable: " << TAU_TO_STR(executable);)
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
				.splitter_one = node::nso_factory::instance()
							.splitter_one(""),
				.mode = solver_mode::general
			};

			auto model = solve<node>(constraints, options);
			if (!model) continue;

			LOG_INFO << "Tau specification is executed setting ";
			for (const auto& [uc, v] : model.value())
				LOG_INFO << TAU_TO_STR(uc) << " := " << TAU_TO_STR(v);

#ifdef DEBUG
			LOG_TRACE << "compute_systems/constraints/model: ";
			for (const auto& [k, v]: model.value())
				LOG_TRACE << "\t" << TAU_TO_STR(k) << " := "
					<< TAU_TO_STR(v) << " ";
#endif // DEBUG
			spec = rewriter::replace<node>(executable, model.value());
			LOG_INFO << "Resulting Tau specification: "
						<< TAU_TO_STR(spec) << "\n";
		}
		DBG(LOG_TRACE << "compute_systems/program: " << spec;)
		return std::make_pair(spec, clause);
	}
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
	LOG_TRACE << "update/shifted_update: " << TAU_TO_STR(shifted_update) << "\n";

	// The constant time positions in original_spec need to be replaced
	// by present assignments from memory and already executed sometimes statements need to be removed
	tref current_spec = rewriter::replace<node>(original_spec, memory);
	LOG_DEBUG << "update/current_spec: " << TAU_TO_STR(current_spec) << "\n";

	// TODO: current_spec = remove_happend_sometimes(current_spec);

	tref new_raw_spec = pointwise_revision(current_spec, shifted_update, time_point);
	LOG_DEBUG << "update/new_spec: " << TAU_TO_STR(new_raw_spec) << "\n";
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
	compute_systems(ubt_ctn, inputs, outputs);
	//TODO: remove inputs and outputs which no longer appear in the updated specification
}

template <NodeType node, typename in_t, typename out_t>
tref interpreter<node, in_t, out_t>::pointwise_revision(
	tref spec, tref update, const int_t start_time)
{
	spec = normalizer<node>(spec);
	update = normalizer<node>(update);
	for (const auto& clause : get_dnf_wff_clauses<node>(update)) {
		tref upd_always = tau::get(clause).find_top(
			is_child<node, tau::wff_always>);
		trefs upd_sometime = tau::get(clause).select_top(
			is_child<node, tau::wff_sometimes>);
		tref spec_always = tau::get(spec).find_top(
			is_child<node, tau::wff_always>);
		trefs spec_sometimes = tau::get(spec).select_top(
			is_child<node, tau::wff_sometimes>);

		const tref new_spec = clause;
		// Check if the update by itself is sat from current time point onwards
		// taking the memory into account
		LOG_TRACE << "pwr/new_spec: " << TAU_TO_STR(new_spec) << "\n";
		if (!is_tau_formula_sat<node>(new_spec, start_time))
			continue;

		// Now try to add always part of old spec in a pointwise way
		tref new_spec_pointwise;
		if (spec_always) {
			if (upd_always) {
				tref aw = always_conjunction<node>(
						upd_always, spec_always);
				trefs aw_io_vars = tau::get(aw).select_top(
					is_child<node, tau::io_var>);
				for (tref io_var : aw_io_vars)
					if (tau::get(io_var).is_output_variable())
						aw = build_wff_ex<node>(io_var, aw);
				new_spec_pointwise = build_wff_or<node>(
					always_conjunction<node>(upd_always, build_wff_neg<node>(aw)),
					always_conjunction<node>(upd_always, spec_always)
				);
			} else new_spec_pointwise = tau::get(spec_always)[0].first();

			new_spec_pointwise = build_wff_always<node>(new_spec_pointwise);
			new_spec_pointwise = build_wff_and<node>(
				new_spec_pointwise,
				build_wff_and<node>(upd_sometime));

			LOG_TRACE << "pwr/new_spec_pointwise: "
				<< TAU_TO_STR(new_spec_pointwise) << "\n";
			if (!is_tau_formula_sat<node>(new_spec_pointwise, start_time))
				return new_spec;
		} else new_spec_pointwise = new_spec;

		if (spec_sometimes.empty())
			return normalizer_step<node>(new_spec_pointwise);
		// Now try to add sometimes part of old spec
		tref new_spec_pointwise_sometimes =
			build_wff_and<node>(new_spec_pointwise,
				build_wff_and<node>(spec_sometimes));

		LOG_TRACE << "pwr/new_spec_pointwise_sometimes: "
			<< TAU_TO_STR(new_spec_pointwise_sometimes) << "\n";
		if (!is_tau_formula_sat<node>(new_spec_pointwise_sometimes, start_time))
			return normalizer_step<node>(new_spec_pointwise);

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
			.splitter_one = node::nso_factory::instance()
							.splitter_one(""),
			.mode = solver_mode::general
		};
		// solve the given system of equations
		return solve<node>(fm, options);
	};
	tref u = build_out_var_at_n<node>("u", time_point);
	auto is_u_stream = [&u](const auto& n) {
		return n == u;
	};
	for (tref clause : get_dnf_wff_clauses<node>(spec)) {
		// Find update stream in clause
		tref update = tau::get(clause).find_top(is_u_stream);
		// If there is no update in clause
		if (!update) {
			if (auto sol = get_solution(clause)) return sol;
			else continue;
		}

		// Obtain single f = 0 part of clause
		tref f = squeeze_positives<node>(clause);
		// If no positive parts exists, the update cannot be maximized
		if (!f) {
			if (auto sol = get_solution(clause)) return sol;
			else continue;
		}

		// Check that f is wide (not 0 and has more than one zero), otherwise continue
		f = tt(f) | bf_reduce_canonical<node>() | tt::ref;
		if (tau::get(f).equals_0()) continue;
		tref f0 = rewriter::replace<node>(f, u, tau::_0());
		tref f1 = rewriter::replace<node>(f, u, tau::_1());
		tref f0_xor_f1 = tt(build_bf_xor<node>(f0, f1))
			| bf_reduce_canonical<node>() | tt::ref;
		if (tau::get(f0_xor_f1).equals_0()
			|| tau::get(f0_xor_f1).equals_1()) continue;

		// Here we know that f is wide
		tref max_u = build_bf_neg<node>(f1);
		tref max_u_spec = rewriter::replace<node>(clause, u, max_u);
		auto sol = get_solution(max_u_spec);
		if (!sol.has_value()) continue;
		// Now we need to add solution for u[t]
		max_u = rewriter::replace<node>(max_u, sol.value());
		max_u = tt(replace_free_vars_by<node>(max_u, tau::_0_trimmed()))
			| bf_reduce_canonical<node>() | tt::ref;
		sol.value().emplace(u, max_u);
		return sol;
	}
	// In case there is no maximal solution for u
	return get_solution(spec);
}

template <NodeType node, typename in_t, typename out_t>
bool interpreter<node, in_t, out_t>::is_excluded_output(tref var) {
	if (tau::get(var).is_input_variable()) return false;
	const std::string& io_name = get_var_name<node>(var);
	return io_name[0] == '_' && io_name.size() > 1 && io_name[1] == 'e';
}

template <NodeType node, typename in_t, typename out_t>
trefs interpreter<node, in_t, out_t>::appear_within_lookback(const trefs& vars){
	trefs appeared;
	for (size_t t = time_point; t <= time_point + (size_t)lookback; ++t) {
		auto step_ubt_ctn = get_ubt_ctn_at(t);
		step_ubt_ctn = rewriter::replace<node>(step_ubt_ctn, memory);
		step_ubt_ctn = normalizer<node>(step_ubt_ctn);
		// Try to find var in step_ubt_ctn
		for (tref v : vars) {
			const auto has_var = [&v](tref n) {
				return tau::subtree_equals(n, v);
			};
			if (tau::get(step_ubt_ctn).find_top(has_var))
				if (std::ranges::find(appeared, v)
					== appeared.end())
						appeared.emplace_back(v);
		}
	}
	return appeared;
}

template <NodeType node>
std::optional<tref> unpack_tau_constant(tref constant) {
	const auto& c = tree<node>::get(constant);
	if (!c.is_ba_constant()) return {};
	return node::nso_factory::instance().unpack_tau_ba(c.get_ba_constant());
}

template <NodeType node, typename in_t, typename out_t>
std::optional<interpreter<node, in_t, out_t>> run(tref form,
	in_t& inputs, out_t& outputs, const size_t steps)
{
	using tau = tree<node>;
	tref spec = normalizer<node>(form);
	auto intrprtr_o = interpreter<node, in_t, out_t>::make_interpreter(
							spec, inputs, outputs);
	if (!intrprtr_o) return {};
	auto& intrprtr = intrprtr_o.value();

	LOG_INFO << "-----------------------------------------------------------------------------------------------------------";
	LOG_INFO << "Please provide requested input, or press ENTER to terminate                                               |";
	LOG_INFO << "If no input is requested, press ENTER to continue to the next execution step, or type q(uit) to terminate |";
	LOG_INFO << "-----------------------------------------------------------------------------------------------------------\n\n";

	// Continuously perform execution step until user quits
	while (true) {
		auto [output, auto_continue] = intrprtr.step();
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
			"u", intrprtr.time_point - 1);
		// Update only if u is of type tau
		if (size_t t = intrprtr.outputs.type_of(tau::trim(update_stream));
			t != 0 && t == get_ba_type_id<node>("tau"))
		{
			auto it = output.value().find(update_stream);
			if (it != output.value().end() && !tau::get(it->second)
				.equals_0())
			{
				if (auto update = unpack_tau_constant<node>(
					it->second); update)
				{
					LOG_TRACE << "update: "
						<< update.value() << "\n";
					intrprtr.update(update.value());
				}
			}
		}
		if (steps != 0 && intrprtr.time_point == steps) break;
	}
	return intrprtr_o;
}

} // namespace idni::tau_lang
