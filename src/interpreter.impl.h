// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef INTERPRETER_IMPL_H
#define INTERPRETER_IMPL_H

#include "interpreter.h"

namespace idni::tau_lang {

template<typename input_t, typename output_t, typename...BAs>
std::optional<interpreter<input_t, output_t, BAs...>>
interpreter<input_t, output_t, BAs...>::make_interpreter(
	tau<BAs...> spec, auto& inputs, auto& outputs) {
	// Find a satisfiable unbound continuation from spec
	auto ubd_ctn = get_executable_spec(spec);
	if (ubd_ctn == nullptr) {
		BOOST_LOG_TRIVIAL(error)
			<< "(Error) Tau specification is unsat\n";
		return {};
	}

	// compute the different systems to be solved
	// TODO: This is currently only here to catch the error if some equation does not have a stream variable
	auto systems = compute_systems(ubd_ctn, inputs, outputs);
	if (systems.empty()) {
		return {}; // error
	}
	//after the above, we have the interpreter ready to be used.
	assignment<BAs...> memory;
	return interpreter {
		ubd_ctn, memory, inputs, outputs
	};
};

template<typename input_t, typename output_t, typename...BAs>
std::pair<std::optional<assignment<BAs...>>, bool> interpreter<input_t, output_t, BAs...>::step() {
	using p = tau_parser;

	// Compute systems for the current step
	if (!calculate_initial_systems())
		return {};
	// for each system in systems try to solve it, if it is not possible
	// continue with the next system.
	std::cout << "Execution step: " << time_point << "\n";
	bool auto_continue = false;
	for (const auto& system: this->systems) {
		std::map<type, solution<BAs...>> solutions;
		bool solved = true;
		// solve the equations for each type in the system
		for (const auto& [type, equations]: system) {
			// rewriting the inputs and inserting them into memory
			auto updated = update_to_time_point(equations);
			auto memory_copy = memory;
			auto current = replace(updated, memory_copy);
			// Simplify after updating stream variables
			current = normalize_non_temp(current);

			// Find open input vars
			auto io_vars = select_top(current,
				is_child_non_terminal<p::io_var, BAs...>);

			// Get values for open inputs which do not exceed time_point
			auto [values, is_quit] = inputs.read(
				io_vars, time_point);
			// Empty input
			if (is_quit) return {};
			// Error during input
			if (!values.has_value()) return {assignment<BAs...>{}, true};
			// Save inputs in memory
			for (const auto& [var, value] : values.value()) {
				assert(get_io_time_point(trim(var)) <= time_point);
				// If there is at least one input, continue automatically in execution
				auto_continue = true;
				memory[var] = value;
			}
			// Plug values for inputs into formula
			current = replace(current, values.value());

			#ifdef DEBUG
			BOOST_LOG_TRIVIAL(trace)
				<< "step/type: " << type << "\n"
				<< "step/equations: " << equations << "\n"
				<< "step/updated: " << updated << "\n"
				<< "step/current: " << current << "\n"
				<< "step/memory: ";
			for (const auto& [k, v]: memory)
				BOOST_LOG_TRIVIAL(trace)
					<< "\t" << k << " := " << v << " ";
			BOOST_LOG_TRIVIAL(trace) << "\n";
			#endif // DEBUG

			auto solution = solve(current, type);

			#ifdef DEBUG
			if (solution) {
				BOOST_LOG_TRIVIAL(trace)
					<< "step/solution: ";
				if (solution.value().empty())
					BOOST_LOG_TRIVIAL(trace) << "\t{}";
				else for (const auto& [k, v]: solution.value())
					BOOST_LOG_TRIVIAL(trace)
						<< "\t" << k << " := " << v << " ";
				auto copy = solution.value();
				auto substituted = replace(current, copy);
				auto check = snf_wff(substituted);
				BOOST_LOG_TRIVIAL(trace)
					<< "step/check: " << check << "\n";
			} else {
				BOOST_LOG_TRIVIAL(trace)
					<< "step/solution: no solution\n";
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
				solutions[type] = solution.value();
			}
			else {
				solved = false;
				// We need to clear the solutions to current clause since it is unsat
				solutions.clear();
				break;
			}
		}
		if (solved) {
			solution<BAs...> global;
			// merge the solutions
			for (const auto& [type, solution]: solutions) {
				for (const auto& [var, value]: solution) {
					// Check if we are dealing with a stream variable
					if (var | tau_parser::variable | tau_parser::io_var) {
						if (get_io_time_point(trim(var)) <= time_point) {
							// std::cout << "time_point: " << time_point << "\n";
							// std::cout << "var: " << var << "\n";
							memory.emplace(var, value);
							// Exclude temporary streams in solution
							if (!is_excluded_output(trim(var)))
								global.emplace(var, value);
						}
					} else {
						memory.emplace(var, value);
						global.emplace(var, value);
					}

				}
			}
			// Complete outputs using time_point and current solution
			for (const auto& [o, _ ] : outputs.streams) {
				auto ot = build_out_variable_at_n(o, time_point);
				if (auto it = global.find(ot); it == global.end()) {
					memory.emplace(ot, _0<BAs...>);
					global.emplace(ot, _0<BAs...>);
				}
			}
			if (global.empty()) {
				BOOST_LOG_TRIVIAL(info) <<
					"currently no output is specified";
			}
			// update time_point and formula_time_point
			if ((int_t)time_point < lookback) {
				// auto continue until lookback
				auto_continue = true;
				++time_point;
			}
			else {
				// auto continue until highest initial position
				if ((int_t)time_point < highest_initial_pos)
					auto_continue = true;
				++formula_time_point, ++time_point;
			}
			// TODO (HIGH) remove old values from memory
			return {global, auto_continue};
		}
	}
	BOOST_LOG_TRIVIAL(error)
		<< "(Error) internal error: Tau specification is unexpectedly unsat\n";
	return {};
}

template<typename input_t, typename output_t, typename...BAs>
bool interpreter<input_t, output_t, BAs...>::calculate_initial_systems() {
	using p = tau_parser;
	size_t initial_segment = std::max(highest_initial_pos, lookback);
	// If time_point < initial_segment, recompute systems
	if (time_point < initial_segment) {
		// Adjust ubt_ctn to time_point by eliminating inputs and outputs
		// which are greater than current time_point in a time-compatible fashion
		auto step_ubt_ctn = update_to_time_point(ubt_ctn);
		auto io_vars = select_top(step_ubt_ctn,
				is_child_non_terminal<p::io_var, BAs...>);
		sort(io_vars.begin(), io_vars.end(), constant_io_comp);
		// All io_vars in fm have to refer to constant time positions
		assert(all_of(io_vars.begin(), io_vars.end(),
			[](const auto& el){return is_io_initial(el);}));
		while (!io_vars.empty()) {
			auto& v = io_vars.back();
			if (get_io_time_point(v) <= time_point) {
				io_vars.pop_back();
				continue;
			}
			if (v | p::io_var | p::in)
				step_ubt_ctn = build_wff_all(v, step_ubt_ctn);
			else step_ubt_ctn = build_wff_ex(v, step_ubt_ctn);
			io_vars.pop_back();
		}
		// Eliminate added quantifiers
		step_ubt_ctn = normalize_non_temp(step_ubt_ctn);
		systems = compute_systems(step_ubt_ctn, inputs, outputs);
		if (systems.empty()) return false;
	} else if (time_point == initial_segment) {
		systems = compute_systems(ubt_ctn, inputs, outputs);
		if (systems.empty()) return false;
	}
	return true;
}

template<typename input_t, typename output_t, typename...BAs>
tau<BAs...> interpreter<input_t, output_t, BAs...>::update_to_time_point(const tau<BAs...>& f) {
	// update the f according to current time_point, i.e. for each
	// input/output var which has a shift, we replace it with the value
	// corresponding to the current time_point minus the shift.
	std::map<tau<BAs...>, tau<BAs...> > changes;
	for (const auto& io_var:
		select_top(
			f, is_non_terminal<tau_parser::io_var, BAs...>))
		if (auto shift = io_var
				| only_child_extractor<BAs...>
				| tau_parser::offset
				| tau_parser::shift,
					var = shift
						| tau_parser::variable,
					num = shift
						| tau_parser::num
						| only_child_extractor<
							BAs...>
						| offset_extractor<BAs
							...>;
			num && var)
			changes[shift.value()] = build_num<BAs...>(
				formula_time_point - num.value());
		else if (auto offset = io_var
					| only_child_extractor<BAs...>
					| tau_parser::offset,
					variable = offset
						| tau_parser::variable;
			variable)
			changes[offset.value()] = wrap(
				tau_parser::offset,
				build_num<BAs...>(formula_time_point));
	return changes.empty() ? f : replace(f, changes);
}

template<typename input_t, typename output_t, typename...BAs>
void interpreter<input_t, output_t, BAs...>::resolve_solution_dependencies(solution<BAs...>& s) {
	using p = tau_parser;
	for (auto& [v, a] : s) {
		if (is_child_non_terminal(p::variable, a)) {
			// The assigned value is a variables
			auto new_a = a;
			while (is_child_non_terminal(p::variable, new_a)) {
				auto it = s.find(new_a);
				if (it == s.end()) {
					BOOST_LOG_TRIVIAL(error) << "(Error) cannot eliminate variable in solution\n";
					break;
				}
				new_a = it->second;
			}
			a = new_a;
		}
	}
}

template<typename input_t, typename output_t, typename...BAs>
void interpreter<input_t, output_t, BAs...>::compute_lookback_and_initial( const tau<BAs...>& ubd_ctn) {
	std::vector<tau<BAs...> > io_vars = select_top(ubd_ctn,
		is_child_non_terminal< tau_parser::io_var , BAs...>);
	lookback = get_max_shift(io_vars);
	formula_time_point = lookback;
	highest_initial_pos = get_max_initial(io_vars);
}


template<typename input_t, typename output_t, typename...BAs>
std::vector<system<BAs...>> interpreter<input_t, output_t, BAs...>::compute_systems(const tau<BAs...>& ubd_ctn,
		auto& inputs, auto& outputs) {
	std::vector<system<BAs...>> systems;
	// Create blue-print for solver for each clause
	for (const auto& clause : get_dnf_wff_clauses(ubd_ctn)) {
		if (auto system = compute_atomic_fm_types(clause, inputs, outputs); system)
			systems.emplace_back(std::move(system.value()));
		else {
			BOOST_LOG_TRIVIAL(trace)
				<< "unable to compute all types in equations in: " << clause << "\n";
			continue;
		}
	}
	return systems;
}

template<typename input_t, typename output_t, typename...BAs>
std::optional<system<BAs...>> interpreter<input_t, output_t, BAs...>::compute_atomic_fm_types(const tau<BAs...>& clause,
	auto& inputs, auto& outputs) {
	auto is_atomic_fm = [](const tau<BAs...>& n) {
		return is_child_non_terminal<tau_parser::bf_eq, BAs...>(n)
			|| is_child_non_terminal<tau_parser::bf_neq, BAs...>(n);
	};

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "compute_system/clause: " << clause;
	#endif // DEBUG

	std::set<tau<BAs...>> pending_atomic_fms;
	for (auto& atomic_fm: select_top(clause, is_atomic_fm)) {
		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "compute_system/atomic_fm " << atomic_fm;
		#endif // DEBUG
		pending_atomic_fms.emplace(std::move(atomic_fm));
	}
	using p = tau_parser;
	system<BAs...> sys;
	bool new_choice = true;
	while (new_choice) {
		new_choice = false;
		for (const auto& fm : pending_atomic_fms) {
			if (auto l = get_type_atomic_fm(fm, inputs, outputs, pending_atomic_fms); l) {
				// Skip atomic fms which have no type yet
				const std::string t = l.value();
				if (l.value() == "") continue;
				if (sys.find(t) == sys.end()) sys[t] = fm;
				else sys[t] = build_wff_and(sys[t], fm);
				new_choice = true;
			} else {
				// Error message is already printed in get_type_fm
				return {};
			}
		}
	}
	// All remaining formulas in pending_atomic_fms can be typed by default
	for (const auto& fm : pending_atomic_fms) {
		std::cout << "def. type for: " << fm << "\n";
		auto io_vars = select_top(fm,
			is_child_non_terminal<p::io_var, BAs...>);
		type_io_vars(io_vars, "tau", inputs, outputs);
		if (sys.find("tau") == sys.end()) sys["tau"] = fm;
		else sys["tau"] = build_wff_and(sys["tau"], fm);
	}
	return { sys };
}

template<typename input_t, typename output_t, typename...BAs>
void interpreter<input_t, output_t, BAs...>::type_io_vars(
	const auto& io_vars, const std::string& type, auto& inputs,
	auto& outputs) {
	using p = tau_parser;
	for (const auto& io_var : io_vars) {
		const auto io_name = trim2(trim(io_var));
		if (io_var | p::io_var | p::in) {
			// Add type to inputs
			inputs.add_input(io_name, type, "");
		} else {
			// Add type to outputs
			if (!is_excluded_output(io_var))
				outputs.add_output(io_name, type, "");
		}
	}
}

template<typename input_t, typename output_t, typename...BAs>
std::optional<type> interpreter<input_t, output_t, BAs...>::get_type_atomic_fm(const tau<BAs...>& fm,
		auto& inputs, auto& outputs, std::set<tau<BAs...>>& pending) {
	using p = tau_parser;
	auto io_vars = select_top(fm, is_child_non_terminal<p::io_var, BAs...>);

	// Check if any io_var has a predefined type
	std::string type;
	for (const auto& io_var : io_vars) {
		if (auto it = inputs.types.find(trim2(trim(io_var))); it != inputs.types.end()) {
			if (!type.empty() && type != it->second) {
				// Type mismatch in atomic fm
				BOOST_LOG_TRIVIAL(error) <<
					"(Error) stream variable type mismatch between '"
 << type << "' and '" << it->second << "' in atomic formula: " << fm << "\n";
				return {};
			} else if (type.empty()) type = it->second;
		}
		if (auto it = outputs.types.find(trim2(trim(io_var))); it != outputs.types.end()) {
			if (!type.empty() && type != it->second) {
				// Type mismatch in atomic fm
				BOOST_LOG_TRIVIAL(error) <<
					"(Error) stream variable type mismatch between '"
 << type << "' and '" << it->second << "' in atomic formula: " << fm << "\n";
				return {};
			} else if (type.empty()) type = it->second;
		}
	}
	std::cout << "type before const: " << type << "\n";
	// Check if all constants match the type, if present, else infer from there
	auto consts = select_top(fm, is_non_terminal<p::bf_constant, BAs...>);
	for (const auto& c : consts) {
		assert(is_non_terminal(p::type, c->child[1]));
		auto c_type = tau_to_str(c->child[1]);
		std::cout << "c_type: " << c_type << "\n";
		if (type.empty()) type = c_type;
		else if (type != c_type) {
			// Type mismatch in atomic fm
			BOOST_LOG_TRIVIAL(error) <<
				"(Error) stream variable or constant type mismatch between '"
 << type << "' and '" << c_type << "' in atomic formula: " << fm << "\n";
			return {};
		}
	}
	std::cout << "type: " << type << "\n";
	// No type information was found, delay typing until all equations have
	// been visited
	if (type.empty()) return "";

	// If a type is found, set all io_vars to found type
	type_io_vars(io_vars, type, inputs, outputs);
	// Remove from pending if present
	if (auto it = pending.find(fm); it != pending.end())
		pending.erase(it);
	return type;
}

template<typename input_t, typename output_t, typename...BAs>
tau<BAs...> interpreter<input_t, output_t, BAs...>::get_executable_spec(const tau<BAs...>& fm) {
	for (auto& clause : get_dnf_wff_clauses(fm)) {
#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "compute_systems/clause: " << clause;
#endif // DEBUG

		auto executable = transform_to_execution(clause, true);
#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "compute_systems/executable: " << executable;
#endif // DEBUG
		if (executable == _F<BAs...>) continue;

		// compute model for uninterpreted constants and solve it
		auto constraints = get_uninterpreted_constants_constraints(executable);
		if (constraints == _F<BAs...>) continue;
#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "compute_systems/constraints: " << constraints;
#endif // DEBUG
		auto spec = executable;
		if (constraints != _T<BAs...>) {
			auto model = solve(constraints);
			if (!model) continue;

			BOOST_LOG_TRIVIAL(info) << "Tau specification is executed setting ";
			for (const auto& [uc, v] : model.value()) {
				BOOST_LOG_TRIVIAL(info) << uc << " := " << v;
			}

#ifdef DEBUG
			BOOST_LOG_TRIVIAL(trace)
				<< "compute_systems/constraints/model: ";
			for (const auto& [k, v]: model.value())
				BOOST_LOG_TRIVIAL(trace)
					<< "\t" << k << " := " << v << " ";
#endif // DEBUG
			spec = replace(executable, model.value());
			BOOST_LOG_TRIVIAL(info) << "Resulting Tau specification: " << spec << "\n\n";
		}
#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "compute_systems/program: " << spec;
#endif // DEBUG
		return spec;
	}
	return nullptr;
}

template<typename input_t, typename output_t, typename...BAs>
bool interpreter<input_t, output_t, BAs...>::is_excluded_output (const auto& var) {
	std::string io_name = get_io_name(var);
	return io_name[0] == '_' && io_name.size() > 1 && io_name[1] == 'e';
}

} // namespace idni::tau_lang
#endif //INTERPRETER_IMPL_H
