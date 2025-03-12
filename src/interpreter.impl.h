// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef INTERPRETER_IMPL_H
#define INTERPRETER_IMPL_H

#include "interpreter.h"

namespace idni::tau_lang {

template<typename input_t, typename output_t, typename...BAs>
std::optional<interpreter<input_t, output_t, BAs...>>
interpreter<input_t, output_t, BAs...>::make_interpreter(
	const tau<BAs...>& spec, auto& inputs, auto& outputs) {
	// Find a satisfiable unbound continuation from spec
	auto [ubd_ctn, clause] = get_executable_spec(spec);
	if (ubd_ctn == nullptr) {
		BOOST_LOG_TRIVIAL(error)
			<< "(Error) Tau specification is unsat\n";
		return {};
	}

	//after the above, we have the interpreter ready to be used.
	assignment<BAs...> memory;
	return interpreter {
		ubd_ctn, clause, memory, inputs, outputs
	};
}

template<typename input_t, typename output_t, typename...BAs>
std::pair<std::optional<assignment<BAs...>>, bool> interpreter<input_t, output_t, BAs...>::step() {
	// Compute systems for the current step
	if (!calculate_initial_systems())
		return {};
	BOOST_LOG_TRIVIAL(info) << "Execution step: " << time_point << "\n";
	bool auto_continue = false;
	// Get inputs for this step
	auto step_inputs = build_inputs_for_step(time_point);
	step_inputs = appear_within_lookback(step_inputs);
	// Get values for inputs which do not exceed time_point
	auto [values, is_quit] = inputs.read(
		step_inputs, time_point);
	// Empty input
	if (is_quit) return {};
	// Error during input
	if (!values.has_value()) return {assignment<BAs...>{}, true};
	// Save inputs in memory
	for (const auto& [var, value] : values.value()) {
		assert(get_io_time_point(trim(var)) <= (int_t)time_point);
		// If there is at least one input, continue automatically in execution
		auto_continue = true;
		memory[var] = value;
	}
	// for each system in systems try to solve it, if it is not possible
	// continue with the next system.
	for (const auto& system: this->systems) {
		std::map<type, solution<BAs...>> solutions;
		bool solved = true;
		// solve the equations for each type in the system
		for (const auto& [type, equations]: system) {
			// rewriting the inputs and inserting them into memory
			auto updated = update_to_time_point(equations, formula_time_point);
			auto current = replace(updated, memory);
			// Simplify after updating stream variables
			current = normalize_non_temp(current);

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

			auto solution = solution_with_max_update(current);

			#ifdef DEBUG
			if (solution) {
				BOOST_LOG_TRIVIAL(trace)
					<< "step/solution: ";
				if (solution.value().empty())
					BOOST_LOG_TRIVIAL(trace) << "\t{}";
				else for (const auto& [k, v]: solution.value())
					BOOST_LOG_TRIVIAL(trace)
						<< "\t" << k << " := " << v << " ";
				auto substituted = replace(current, solution.value());
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
					if (var | tau_parser::variable
						| tau_parser::io_var)
					{
						if (get_io_time_point(trim(var)) <= (int_t)time_point) {
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

template<typename input_t, typename output_t, typename ... BAs>
tau<BAs...> interpreter<input_t, output_t, BAs...>::get_ubt_ctn_at(int_t t) {
	using p = tau_parser;
	const int_t ut = t < (int_t)formula_time_point ? (int_t)formula_time_point : t;
	if (t >= std::max(highest_initial_pos, (int_t)formula_time_point)) {
		return update_to_time_point(ubt_ctn, ut);
	}
	// Adjust ubt_ctn to time_point by eliminating inputs and outputs
	// which are greater than current time_point in a time-compatible fashion
	auto step_ubt_ctn = update_to_time_point(ubt_ctn, ut);
	auto io_vars = select_top(step_ubt_ctn,
			is_grandchild_non_terminal<p::io_var, BAs...>);
	sort(io_vars.begin(), io_vars.end(), constant_io_comp);
	// All io_vars in fm have to refer to constant time positions
	assert(all_of(io_vars.begin(), io_vars.end(),
		[](const auto& el){return is_io_initial(el);}));
	while (!io_vars.empty()) {
		auto& v = io_vars.back();
		if (get_io_time_point(v) <= t) {
			io_vars.pop_back();
			continue;
		}
		if (v | p::variable | p::io_var | p::in)
			step_ubt_ctn = build_wff_all(v, step_ubt_ctn);
		else step_ubt_ctn = build_wff_ex(v, step_ubt_ctn);
		io_vars.pop_back();
	}
	// Eliminate added quantifiers
	return normalize_non_temp(step_ubt_ctn);
}

template<typename input_t, typename output_t, typename...BAs>
bool interpreter<input_t, output_t, BAs...>::calculate_initial_systems() {
	if (final_system) return true;

	size_t initial_segment = std::max(highest_initial_pos, (int_t)formula_time_point);
	BOOST_LOG_TRIVIAL(trace) << "cis/initial_segment: " << initial_segment << "\n";
	BOOST_LOG_TRIVIAL(trace) << "cis/time_point: " << time_point << "\n";
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

template<typename input_t, typename output_t, typename ... BAs>
std::vector<tau<BAs...>> interpreter<input_t, output_t, BAs...>::
build_inputs_for_step(const size_t t) {
	std::vector<tau<BAs...>> step_inputs;
	for (auto& [var_name, _] : inputs.streams) {
		step_inputs.emplace_back(
			trim(build_in_variable_at_n(var_name, t)));
	}
	return step_inputs;
}

template<typename input_t, typename output_t, typename...BAs>
tau<BAs...> interpreter<input_t, output_t, BAs...>::update_to_time_point(
	const tau<BAs...>& f, const int_t t) {
	// update the f according to current time_point, i.e. for each
	// input/output var which has a shift, we replace it with the value
	// corresponding to the current time_point minus the shift.
	auto io_vars = select_top(f,
		is_grandchild_non_terminal<tau_parser::io_var, BAs...>);
	return fm_at_time_point(f, io_vars, t);
}

template<typename input_t, typename output_t, typename ... BAs>
bool interpreter<input_t, output_t, BAs...>::is_memory_access_valid(
	const auto& io_vars) {
	using p = tau_parser;
	// Check for each constant time point accessing memory, if it is available
	for (const auto& io_var : io_vars) {
		if (is_io_initial(io_var) &&
			get_io_time_point(io_var) < (int_t)time_point) {
			const auto& v = wrap(p::bf, io_var);
			if (!memory.contains(v)) return false;
		}
	}
	return true;
}

template<typename input_t, typename output_t, typename...BAs>
void interpreter<input_t, output_t, BAs...>::resolve_solution_dependencies(solution<BAs...>& s) {
	using p = tau_parser;
	for (auto& [v, a] : s) {
		if (is_child_non_terminal(p::bf_variable, a)) {
			// The assigned value is a variables
			auto new_a = a;
			while (is_child_non_terminal(p::bf_variable, new_a)) {
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
		is_grandchild_non_terminal< tau_parser::io_var , BAs...>);
	lookback = get_max_shift(io_vars);
	formula_time_point = time_point + lookback;
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
		std::vector<tau<BAs...>> to_erase_fms;
		for (const auto& fm : pending_atomic_fms) {
			if (auto l = get_type_atomic_fm(fm, inputs, outputs); l) {
				// Skip atomic fms which have no type yet
				const std::string& t = l.value();
				if (l.value() == "") continue;
				to_erase_fms.push_back(fm);
				if (!sys.contains(t)) sys[t] = fm;
				else sys[t] = build_wff_and(sys[t], fm);
				new_choice = true;
			} else {
				// Error message is already printed in get_type_fm
				return {};
			}
		}
		for (const auto& fm : to_erase_fms)
			pending_atomic_fms.erase(fm);
	}
	// All remaining formulas in pending_atomic_fms can be typed by default
	for (const auto& fm : pending_atomic_fms) {
		// std::cout << "def. type for: " << fm << "\n";
		auto io_vars = select_top(fm,
			is_grandchild_non_terminal<p::io_var, BAs...>);
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
		const auto io_name = get_tau_io_name(io_var);
		if (io_var | p::variable | p::io_var | p::in) {
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
		auto& inputs, auto& outputs) {
	using p = tau_parser;
	auto io_vars = select_top(fm, is_grandchild_non_terminal<p::io_var, BAs...>);

	// Check if any io_var has a predefined type
	std::string type;
	for (const auto& io_var : io_vars) {
		if (auto t = inputs.type_of(get_tau_io_name(io_var))) {
			if (!type.empty() && type != t.value()) {
				// Type mismatch in atomic fm
				BOOST_LOG_TRIVIAL(error) <<
					"(Error) stream variable type mismatch between '"
 << type << "' and '" << t.value() << "' in atomic formula: " << fm << "\n";
				return {};
			} else if (type.empty()) type = t.value();
		}
		if (auto t = outputs.type_of(get_tau_io_name(io_var))) {
			if (!type.empty() && type != t.value()) {
				// Type mismatch in atomic fm
				BOOST_LOG_TRIVIAL(error) <<
					"(Error) stream variable type mismatch between '"
 << type << "' and '" << t.value() << "' in atomic formula: " << fm << "\n";
				return {};
			} else if (type.empty()) type = t.value();
		}
	}
	// std::cout << "type before const: " << type << "\n";
	// Check if all constants match the type, if present, else infer from there
	auto consts = select_top(fm, is_non_terminal<p::bf_constant, BAs...>);
	for (const auto& c : consts) {
		assert(is_non_terminal(p::type, c->child[1]));
		auto c_type = tau_to_str(c->child[1]);
		// std::cout << "c_type: " << c_type << "\n";
		if (type.empty()) type = c_type;
		else if (type != c_type) {
			// Type mismatch in atomic fm
			BOOST_LOG_TRIVIAL(error) <<
				"(Error) stream variable or constant type mismatch between '"
 << type << "' and '" << c_type << "' in atomic formula: " << fm << "\n";
			return {};
		}
	}

	// Check if any other type information is available
	if (auto alt_type = find_top(fm, is_non_terminal<p::type, BAs...>)) {
		if (type != tau_to_str(alt_type.value())) {
			// Type mismatch in atomic fm
			BOOST_LOG_TRIVIAL(error) <<
				"(Error) stream variable or constant type mismatch between '"
 << type << "' and '" << alt_type.value() << "' in atomic formula: " << fm << "\n";
			return {};
		} else type = tau_to_str(alt_type.value());
	}

	// std::cout << "type: " << type << "\n";
	// No type information was found, delay typing until all equations have
	// been visited
	if (type.empty()) return "";

	// If a type is found, set all io_vars to found type
	type_io_vars(io_vars, type, inputs, outputs);
	return type;
}

template<typename input_t, typename output_t, typename...BAs>
std::pair<tau<BAs...>, tau<BAs...>>
interpreter<input_t, output_t, BAs...>::get_executable_spec(
	const tau<BAs...>& fm, const size_t start_time) {
	for (auto& clause : get_dnf_wff_clauses(fm)) {
#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "compute_systems/clause: " << clause;
#endif // DEBUG

		auto executable = transform_to_execution(clause, start_time, true);
#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "compute_systems/executable: " << executable;
#endif // DEBUG
		if (executable == _F<BAs...>) continue;
		// Make sure that no constant time position is smaller than 0
		auto io_vars = select_top(executable,
			is_grandchild_non_terminal<tau_parser::io_var, BAs...>);
		for (const auto& io_var : io_vars) {
			if (is_io_initial(io_var) && get_io_time_point(io_var) < 0) {
				BOOST_LOG_TRIVIAL(error) << "(Error) Constant time position is smaller than 0\n";
				return std::make_pair(nullptr, nullptr);
			}
		}
		// compute model for uninterpreted constants and solve it
		auto constraints =
			get_uninterpreted_constants_constraints(executable, io_vars);
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
		return std::make_pair(spec, clause);
	}
	return std::make_pair(nullptr, nullptr);
}

template<typename input_t, typename output_t, typename ... BAs>
void interpreter<input_t, output_t, BAs...>::update(const tau<BAs...>& update) {
	auto io_vars = select_top(
		update, is_grandchild_non_terminal<tau_parser::io_var, BAs...>);
	// the constant time positions in update are seen relative to
	// time_point, i.e. time point 0 is shifted to time_point
	tau<BAs...> shifted_update = shift_const_io_vars_in_fm(
		update, io_vars, time_point);
	if (shifted_update == _F<BAs...>) {
		BOOST_LOG_TRIVIAL(info) << "(Warning) no update performed: constant time position below 0 was found\n";
		return;
	}
	io_vars = select_top(shifted_update,
		is_grandchild_non_terminal<tau_parser::io_var, BAs...>);
	if (!is_memory_access_valid(io_vars)) {
		BOOST_LOG_TRIVIAL(info) << "(Warning) no update performed: invalid memory access was found\n";
		return;
	}
	shifted_update = replace(shifted_update, memory);
	BOOST_LOG_TRIVIAL(trace) << "update/shifted_update: " << shifted_update << "\n";

	// The constant time positions in original_spec need to be replaced
	// by present assignments from memory and already executed sometimes statements need to be removed
	auto current_spec = replace(original_spec, memory);
	BOOST_LOG_TRIVIAL(debug) << "update/current_spec: " << current_spec << "\n";

	// TODO: current_spec = remove_happend_sometimes(current_spec);

	auto new_raw_spec = pointwise_revision(current_spec, shifted_update, time_point);
	BOOST_LOG_TRIVIAL(debug) << "update/new_spec: " << new_raw_spec << "\n";
	// std::cout << "update/new_spec: " << new_raw_spec << "\n";
	if (new_raw_spec == _F<BAs...>) {
		BOOST_LOG_TRIVIAL(info) << "(Warning) no updated performed: updated specification is unsat\n";
		return;
	}

	// If the unbound continuation from start_time is possible,
	// it is safe to swap the current spec by update_unbound
	auto [new_ubd_ctn, new_spec] = get_executable_spec(new_raw_spec, time_point);
	if (new_ubd_ctn == nullptr) {
		BOOST_LOG_TRIVIAL(info) << "(Warning) no update performed: updated specification is unsat\n";
		return;
	}

	BOOST_LOG_TRIVIAL(info) << "Updated specification: \n" << new_spec << "\n\n";

	// Set new specification for interpreter
	ubt_ctn = new_ubd_ctn;
	original_spec = new_spec;
	// The systems for solver need to be recomputed at beginning of next step
	final_system = false;
	compute_lookback_and_initial();
	compute_systems(ubt_ctn, inputs, outputs);
	//TODO: remove inputs and outputs which no longer appear in the updated specification
}

template<typename input_t, typename output_t, typename ... BAs>
tau<BAs...> interpreter<input_t, output_t, BAs...>::pointwise_revision(
	tau<BAs...> spec, tau<BAs...> update, const int_t start_time) {
	using p = tau_parser;
	spec = normalizer(spec);
	update = normalizer(update);
	for (const auto& clause : get_dnf_wff_clauses(update)) {
		auto upd_always = find_top(
			clause, is_child_non_terminal<p::wff_always, BAs...>);
		auto upd_sometime = select_top(
			clause, is_child_non_terminal<p::wff_sometimes, BAs...>);
		auto spec_sometimes = select_top(
			spec, is_child_non_terminal<p::wff_sometimes, BAs...>);
		auto spec_always = find_top(
			spec, is_child_non_terminal<p::wff_always, BAs...>);

		const tau<BAs...> new_spec = clause;
		// Check if the update by itself is sat from current time point onwards
		// taking the memory into account
		BOOST_LOG_TRIVIAL(trace) << "pwr/new_spec: " << new_spec << "\n";
		if (!is_tau_formula_sat(new_spec, start_time))
			continue;

		// Now try to add always part of old spec in a pointwise way
		tau<BAs...> new_spec_pointwise;
		if (spec_always) {
			if (upd_always) {
				tau<BAs...> aw = always_conjunction(
					upd_always.value(),
					spec_always.value());
				auto aw_io_vars = select_top(aw,
					is_grandchild_non_terminal<
							p::io_var, BAs...>);
				for (const auto& io_var : aw_io_vars)
					if (io_var | p::variable | p::io_var | p::out)
						aw = build_wff_ex(io_var, aw);
				new_spec_pointwise = build_wff_or(
					always_conjunction(upd_always.value(), build_wff_neg(aw)),
					always_conjunction(upd_always.value(), spec_always.value())
				);
			} else new_spec_pointwise = trim2(spec_always.value());

			new_spec_pointwise = build_wff_always(new_spec_pointwise);
			new_spec_pointwise = build_wff_and(
				new_spec_pointwise,
				build_wff_and<BAs...>(upd_sometime));

			BOOST_LOG_TRIVIAL(trace) << "pwr/new_spec_pointwise: " << new_spec_pointwise << "\n";
			if (!is_tau_formula_sat(new_spec_pointwise, start_time))
				return new_spec;
		} else new_spec_pointwise = new_spec;

		if (spec_sometimes.empty()) return normalizer_step(new_spec_pointwise);
		// Now try to add sometimes part of old spec
		auto new_spec_pointwise_sometimes =
			build_wff_and(new_spec_pointwise,
				build_wff_and<BAs...>(spec_sometimes));

		BOOST_LOG_TRIVIAL(trace) << "pwr/new_spec_pointwise_sometimes: " << new_spec_pointwise_sometimes << "\n";
		if (!is_tau_formula_sat(new_spec_pointwise_sometimes, start_time))
			return normalizer_step(new_spec_pointwise);

		return normalize_with_temp_simp(new_spec_pointwise_sometimes);
	}
	// If no clause is sat, return F
	return _F<BAs...>;
}

template<typename input_t, typename output_t, typename ... BAs>
std::optional<assignment<BAs...>> interpreter<input_t, output_t, BAs...>::
solution_with_max_update(const tau<BAs...>& spec) {
	auto get_solution = [](const auto& fm) {
		// setting proper options for the solver
		solver_options<BAs...> options = {
			.splitter_one = nso_factory<BAs...>::instance().splitter_one(""),
			.mode = solver_mode::general
		};
		// solve the given system of equations
		return solve(fm, options);
	};
	auto u = build_out_variable_at_n<BAs...>("u", time_point);
	auto is_u_stream = [&u](const auto& n) {
		return n == u;
	};
	for (const auto& clause : get_dnf_wff_clauses(spec)) {
		// Find update stream in clause
		auto update = find_top(clause, is_u_stream);
		// If there is no update in clause
		if (!update.has_value()) {
			if (auto sol = get_solution(clause)) return sol;
			else continue;
		}

		// Obtain single f = 0 part of clause
		auto f = squeeze_positives(clause);
		// If no positive parts exists, the update cannot be maximized
		if (!f.has_value()) {
			if (auto sol = get_solution(clause)) return sol;
			else continue;
		}

		// Check that f is wide (not 0 and has more than one zero), otherwise continue
		f.value() = f.value() | bf_reduce_canonical<BAs...>();
		if (f.value() == _0<BAs...>) continue;
		auto f0 = replace(f.value(), u, _0<BAs...>);
		auto f1 = replace(f.value(), u, _1<BAs...>);
		auto f0_xor_f1 = build_bf_xor(f0, f1) | bf_reduce_canonical<BAs...>();
		if (f0_xor_f1 == _0<BAs...> || f0_xor_f1 == _1<BAs...>) continue;

		// Here we know that f is wide
		auto max_u = build_bf_neg(f1);
		auto max_u_spec = replace(clause, u, max_u);
		auto sol = get_solution(max_u_spec);
		if (!sol.has_value()) continue;
		// Now we need to add solution for u[t]
		max_u = replace(max_u, sol.value());
		max_u = replace_free_vars_by(max_u, _0_trimmed<BAs...>)
				| bf_reduce_canonical<BAs...>();
		sol.value().emplace(u, max_u);
		return sol;
	}
	// In case there is no maximal solution for u
	return get_solution(spec);
}

template<typename input_t, typename output_t, typename...BAs>
bool interpreter<input_t, output_t, BAs...>::is_excluded_output (const tau<BAs...>& var) {
	using p = tau_parser;
	std::string io_name = is_non_terminal(p::out_var_name, var)
					? tau_to_str(var)
					: get_io_name(var);
	return io_name[0] == '_' && io_name.size() > 1 && io_name[1] == 'e';
}

template<typename input_t, typename output_t, typename ... BAs>
std::vector<tau<BAs...>> interpreter<input_t, output_t, BAs...>::appear_within_lookback(
	const std::vector<tau<BAs...>>& vars) {
	std::vector<tau<BAs...>> appeared;
	for (size_t t = time_point; t <= time_point + (size_t)lookback; ++t) {
		auto step_ubt_ctn = get_ubt_ctn_at(t);
		step_ubt_ctn = replace(step_ubt_ctn, memory);
		step_ubt_ctn = normalize_non_temp(step_ubt_ctn);
		// Try to find var in step_ubt_ctn
		for (const auto& v : vars) {
			const auto has_var = [&v](const auto& n){return n == v;};
			if (find_top(step_ubt_ctn, has_var)) {
				if (std::ranges::find(appeared, v) == appeared.end())
					appeared.emplace_back(v);
			}
		}
	}
	return appeared;
}
} // namespace idni::tau_lang
#endif //INTERPRETER_IMPL_H
