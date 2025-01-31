// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef INTERPRETER_IMPL_H
#define INTERPRETER_IMPL_H

#include "interpreter.h"

namespace idni::tau_lang {

template<typename input_t, typename output_t, typename...BAs>
std::optional<interpreter<input_t, output_t, BAs...>>
interpreter<input_t, output_t, BAs...>::make_interpreter(
	const tau<BAs...>& spec, const auto& inputs, const auto& outputs) {
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
		// TODO: spec needs to be clause matching ubd_ctn
		ubd_ctn, spec, memory, inputs, outputs
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

			// setting proper options for the solver
			solver_options<BAs...> options = {
				.splitter_one = nso_factory<BAs...>::instance().splitter_one(""),
				.mode = solver_mode::general };

			// solve the given system of equations
			auto solution = solve(current, options);

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
							if (get_io_name(trim(var))[0] != '_')
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
					// If it is not in global, see if is in memory
					// Can happen due to pointwise revision
					if (auto it2 = memory.find(ot); it2 != memory.end()) {
						global.emplace(ot, it2->second);
					}
					//TODO: Enable autocomplete again after type inference is there
					// else {
					// 	memory.emplace(ot, _0<BAs...>);
					// 	global.emplace(ot, _0<BAs...>);
					// }
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
				++time_point;
				formula_time_point = time_point;
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
	if (final_system) return true;

	size_t initial_segment = std::max(highest_initial_pos, (int_t)formula_time_point);
	std::cout << "initial_segment: " << initial_segment << "\n";
	std::cout << "time_point: " << time_point << "\n";
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
		final_system = true;
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
void interpreter<input_t, output_t, BAs...>::compute_lookback_and_initial() {
	std::vector<tau<BAs...> > io_vars = select_top(ubt_ctn,
		is_child_non_terminal< tau_parser::io_var , BAs...>);
	lookback = get_max_shift(io_vars);
	if ((int_t)time_point < lookback)
		formula_time_point = lookback;
	highest_initial_pos = get_max_initial(io_vars);
}


template<typename input_t, typename output_t, typename...BAs>
std::vector<system<BAs...>> interpreter<input_t, output_t, BAs...>::compute_systems(const tau<BAs...>& ubd_ctn,
		const auto& inputs, const auto& outputs) {
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
	const auto& inputs, const auto& outputs) {
	auto is_atomic_fm = [](const tau<BAs...>& n) {
		return is_child_non_terminal<tau_parser::bf_eq, BAs...>(n)
			|| is_child_non_terminal<tau_parser::bf_neq, BAs...>(n);
	};

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "compute_system/clause: " << clause;
	#endif // DEBUG

	system<BAs...> sys;
	for (const auto& atomic_fm: select_top(clause, is_atomic_fm)) {
		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "compute_system/atomic_fm " << atomic_fm;
		#endif // DEBUG

		if (auto l = get_type_fm(atomic_fm, inputs, outputs); l) {
			if (sys.find(l.value().first) == sys.end()) sys[l.value().first] = l.value().second;
			else sys[l.value().first] = build_wff_and(sys[l.value().first], l.value().second);
		} else {
			// Error message is already printed in get_type_fm
			return {};
		}
	}
	return { sys };
}

template<typename input_t, typename output_t, typename...BAs>
std::optional<std::pair<type, tau<BAs...>>> interpreter<input_t, output_t, BAs...>::get_type_fm(const tau<BAs...>& fm,
		const auto& inputs, const auto& outputs) {
	if (auto io_var = find_top(fm,
			is_non_terminal<tau_parser::io_var, BAs...>); io_var) {
		if (auto in_var_name = io_var
				| tau_parser::in
				| tau_parser::in_var_name; in_var_name) {
			if(auto t = inputs.type_of(in_var_name.value()); t)
				return { make_pair(t.value() , fm) };
		} else if (auto out_var_name = io_var
				| tau_parser::out
				| tau_parser::out_var_name; out_var_name) {
			if (auto t = outputs.type_of(out_var_name.value()); t)
				return { make_pair(t.value() , fm) };
		}
	}
	BOOST_LOG_TRIVIAL(error)
		<< "(Error) atomic formula contains no stream variable: " << fm << "\n";
	return {};
}

template<typename input_t, typename output_t, typename...BAs>
tau<BAs...> interpreter<input_t, output_t, BAs...>::get_executable_spec(const tau<BAs...>& fm) {
	for (auto& clause : get_dnf_wff_clauses(fm)) {
#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "compute_systems/clause: " << clause;
#endif // DEBUG

		auto executable = transform_to_execution(clause, 0, true);
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
			// setting proper options for the solver
			solver_options<BAs...> options = {
				.splitter_one = nso_factory<BAs...>::instance().splitter_one(""),
				.mode = solver_mode::general };

			auto model = solve(constraints, options);
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

template<typename input_t, typename output_t, typename ... BAs>
void interpreter<input_t, output_t, BAs...>::update(const tau<BAs...>& update) {
	auto io_vars = select_top(
		update, is_child_non_terminal<tau_parser::io_var, BAs...>);
	int_t lb = get_max_shift(io_vars);
	// Use bool conversion to integer to decide minimal lookback
	int_t art_lb = (lb == 0 && has_stream_flag(update));
	std::cout << "artificial_lookback: " << art_lb << "\n";
	int_t start_time = std::max(int_t(time_point - std::max(lb, art_lb)), 0);
	std::cout << "start_time: " << start_time << "\n";
	// the constant time positions in update are seen relative to
	// start_time, i.e. time point 0 is shifted to start_time
	tau<BAs...> shifted_update = shift_const_io_vars_in_fm(update, start_time);
	std::cout << "shifted_update: " << shifted_update << "\n";

	// The constant time positions in original_spec need to be replaced
	// by present assignments from memory and already executed sometimes statements need to be removed
	auto memory_copy = memory;
	auto current_spec = replace(original_spec, memory_copy);
	std::cout << "current_spec: " << current_spec << "\n";

	// TODO: current_spec = remove_happend_sometimes(current_spec);

	auto new_spec = pointwise_revision(current_spec, shifted_update, time_point);
	std::cout << "new_spec: " << new_spec << "\n";
	if (new_spec == _F<BAs...>) {
		std::cout << "(Warning) no updated performed\n";
		return;
	}

	// If the unbound continuation from start_time is possible,
	// it is save to swap the current spec by update_unbound
	auto new_ubd_ctn = transform_to_execution(new_spec, time_point, true);
	if (new_ubd_ctn == _F<BAs...>) {
		std::cout << "(Warning) updated specification is unsat\n";
		return;
	}

	// Set new specification for interpreter
	ubt_ctn = new_ubd_ctn;
	original_spec = new_spec;
	formula_time_point = time_point;
	time_point = start_time;
	// The systems for solver need to be recomputed at beginning of next step
	final_system = false;
	compute_lookback_and_initial();
}

template<typename input_t, typename output_t, typename ... BAs>
tau<BAs...> interpreter<input_t, output_t, BAs...>::pointwise_revision(
	const tau<BAs...>& spec, const tau<BAs...>& update, const int_t start_time) {
	using p = tau_parser;
	// TODO: if update has more than one clause, need to choose one

	auto upd_always = find_top(
		update, is_child_non_terminal<p::wff_always, BAs...>);
	auto spec_sometimes = select_top(
		spec, is_child_non_terminal<p::wff_sometimes, BAs...>);
	auto spec_always = find_top(
		spec, is_child_non_terminal<p::wff_always, BAs...>);

	tau<BAs...> new_spec = normalizer(update);
	// Check if the update by itself is sat from current time point onwards
	// taking the memory into account
	std::cout << "pwr/new_spec: " << new_spec << "\n";
	if (!is_tau_formula_sat(new_spec, start_time, memory))
		return _F<BAs...>;

	// Now try to add always part of old spec in a pointwise way
	tau<BAs...> new_spec_pointwise;
	if (spec_always) {
		tau<BAs...> aw;
		if (upd_always)
			aw = build_wff_and(trim2(upd_always.value()),
				trim2(spec_always.value()));
		else aw = trim2(spec_always.value());

		auto aw_io_vars = select_top(aw, is_child_non_terminal<p::io_var, BAs...>);
		for (const auto& io_var : aw_io_vars)
			if (io_var | p::io_var | p::out)
				aw = build_wff_ex(io_var, aw);
		new_spec_pointwise = build_wff_and(
			new_spec, build_wff_imply(aw, trim2(spec_always.value())));

		std::cout << "pwr/new_spec_pointwise: " << new_spec_pointwise << "\n";
		if (!is_tau_formula_sat(new_spec_pointwise, start_time, memory))
			return new_spec;
	} else new_spec_pointwise = new_spec;

	if (spec_sometimes.empty()) return normalizer_step(new_spec_pointwise);
	// Now try to add sometimes part of old spec
	auto new_spec_pointwise_sometimes =
		build_wff_and(new_spec_pointwise,
			build_wff_and<BAs...>(spec_sometimes));

	std::cout << "pwr/new_spec_pointwise_sometimes: " << new_spec_pointwise_sometimes << "\n";
	if (!is_tau_formula_sat(new_spec_pointwise_sometimes, start_time, memory))
		return normalizer_step(new_spec_pointwise);

	return normalize_with_temp_simp(new_spec_pointwise_sometimes);
}

} // namespace idni::tau_lang
#endif //INTERPRETER_IMPL_H
