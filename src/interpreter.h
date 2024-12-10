// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__

#include <fstream>

#include "tau_ba.h"
#include "solver.h"
#include "satisfiability.h"
#include "term.h"
#include "variant_ba.h"

#ifdef DEBUG
#include "debug_helpers.h"
#endif // DEBUG

namespace idni::tau {

using type = std::string;

using filename = std::string;

template<typename...BAs>
using io_var_name = nso<BAs...>; // out_var_name or in_var_name

template<typename...BAs>
using var_desc = std::pair<io_var_name<BAs...>, type>;

// represents an solution of variables to values.
template<typename...BAs>
using assignment = std::map<nso<BAs...>, nso<BAs...>>;

// A system represent a clause to be solved. It maps the different
// equations of the clause according to its type.
template<typename...BAs>
using system = std::map<type, nso<BAs...>>;

template<typename...BAs>
struct finputs {

	finputs() = delete;

	finputs(std::map<nso<BAs...>, std::pair<type, filename>> inputs) {
		// open the corresponding streams for input and store them in streams
		for (const auto& [var, desc]: inputs) {
			this->types[var] = desc.first;
			this->streams[var] = desc.second.empty()
				? std::optional<std::ifstream>()
				: std::ifstream(desc.second);
			if (this->streams[var]
				&& !this->streams[var].value().is_open())
					BOOST_LOG_TRIVIAL(error)
						<< "(Error) failed to open input file '"
						<< desc.second << "': ";
		}
	}

	~finputs() {
		// close the streams
		for (auto& [_, file]: streams)
			if (file) file.value().close();
	}

	std::optional<assignment<BAs...>> read() {
		// for each stream in in streams, read the value from the file/stdin,
		// parsed it and store it in out.
		if (streams.empty()) return assignment<BAs...>();
		assignment<BAs...> current;
		for (auto& [var, file]: streams) {
			std::string line;
			if (file) {
				std::getline(file.value(), line);
				std::cout << line << "\n";
			} else {
				std::cout << var << "[" << time_point << "] := ";
				term::enable_getline_mode();
				std::getline(std::cin, line);
				term::disable_getline_mode();
				// TODo (MEDIUM) add echo for input from a file instead of console
			}
			if (line.empty()) return {}; // no more inputs
			auto cnst = nso_factory<BAs...>::instance().parse(line, types[var]);
			if (!cnst) {
				BOOST_LOG_TRIVIAL(error)
					<< "(Error) failed to parse input value '"
					<< line << "' for variable '"
					<< var << "'\n";
				return {};
			}
			current[var] = build_bf_constant(cnst.value());
		}
		time_point += 1;
		return current;
	}

	// Read input from command line and return mapping from in_vars to this input
	std::pair<std::optional<assignment<BAs...> >, bool> read(
		std::vector<nso<BAs...> >& in_vars, size_t time_step) {
		std::ranges::sort(in_vars, constant_io_comp);
		assignment<BAs...> value;
		for (const auto& var : in_vars) {
			// Skip output stream variables
			if (var | tau_parser::io_var | tau_parser::out)
				continue;
			// Skip input stream variables with time point greater time_step
			if (get_io_time_point(var) > time_step)
				continue;
			assert(is_non_terminal(tau_parser::variable, var));
			std::string line;
			auto var_name = trim(trim2(var));
			if (auto it = streams.find(var_name); it != streams.end() && it->second.has_value()) {
				std::getline(it->second.value(), line);
				std::cout << line << "\n";
			} else if (it == streams.end()) {
				BOOST_LOG_TRIVIAL(error)
					<< "(Error) failed to find input stream for variable '" << var_name << "'\n";
				return {};
			} else {
				std::cout << var << " := ";
				term::enable_getline_mode();
				std::getline(std::cin, line);
				term::disable_getline_mode();
			}
			if (line.empty()) return {value, true}; // no more inputs
			auto cnst = nso_factory<BAs...>::instance().parse(line, types[var_name]);
			if (!cnst) {
				BOOST_LOG_TRIVIAL(error)
					<< "(Error) failed to parse input value '"
					<< line << "' for variable '"
					<< var << "'\n";
				return {};
			}
			value[wrap(tau_parser::bf, var)] = build_bf_constant(cnst.value());
		}
		return {value, false};
	}

	std::optional<type> type_of(const nso<BAs...>& var) {
		if (auto type = types.find(var); type != types.end())
			return type->second;

		std::stringstream ss; ss << var;
		if (auto name = ss.str(); !name.empty() && name.front() == '_') {
			return { "sbf" };
		}

		BOOST_LOG_TRIVIAL(error)
			<< "(Error) failed to find type for variable: "
			<< var << "\n";
		return {};
	}

	std::map<nso<BAs...>, type> types;
	std::map<nso<BAs...>, std::optional<std::ifstream>> streams;
	size_t time_point = 0;
};

template<typename...BAs>
struct foutputs {

	foutputs() = delete;

	foutputs(std::map<nso<BAs...>, std::pair<type, filename>> outputs) {
		// open the corresponding streams for input and store them in streams
		for (const auto& [var, desc]: outputs) {
			this->types[var] = desc.first;
			this->streams[var] = desc.second.empty()
				? std::optional<std::ofstream>()
				: std::ofstream(desc.second);
		}
	}

	~foutputs() {
		// close the streams
		for (auto& [_, file]: streams)
			if (file) file.value().close();
	}

	bool write(const assignment<BAs...>& outputs) {
		// Sort variables in output by time
		std::vector<nso<BAs...>> io_vars;
		for (const auto& [var, ass] : outputs)
			io_vars.push_back(var);
		std::ranges::sort(io_vars, constant_io_comp);

		// for each stream in out.streams, write the value from the solution
		for (const auto& io_var : io_vars) {
			// get the BA element associated with io_var_name
			auto value = outputs.find(io_var)->second
				| tau_parser::bf_constant
				| tau_parser::constant
				| only_child_extractor<BAs...>
				| ba_extractor<BAs...>;
			auto io_var_name = trim2(trim2(io_var));
			std::stringstream ss;
			if (!value) {
				// is bf_t
				if (auto check = outputs.find(io_var)->second
						| tau_parser::bf_t; check) {
					auto type = types.find(io_var_name)->second;
					ss << nso_factory<BAs...>::instance().one(type);
				// is bf_f
				} else if (auto check = outputs.find(io_var)->second
						| tau_parser::bf_f; check) {
					auto type = types.find(io_var_name)->second;
					ss << nso_factory<BAs...>::instance().zero(type);
				// is something else but not a BA element
				} else {
					BOOST_LOG_TRIVIAL(error)
						<< "(Error): no Boolean algebra element assigned to variable '"
						<< io_var << "'\n";
					return false;
				}
			} else {
				ss << value.value();
			}
			// get the out_var_name tag
			if (auto stream = streams.find(io_var_name); stream != streams.end())
				if (stream->second) stream->second.value() << ss.str() << "\n";
				else std::cout << io_var << " := " << ss.str() << "\n";
			else {
				std::stringstream ss; ss << io_var;
				if (auto name = ss.str(); !name.empty() && name.front() == '_') continue;

				BOOST_LOG_TRIVIAL(error)
					<< "(Error) failed to find output stream for variable '"
					<< io_var << "'\n";
				return false;
			}
		}
		return true; // success
	}

	std::optional<type> type_of(const nso<BAs...>& var) {
		if (auto type = types.find(var); type != types.end())
			return type->second;

		std::stringstream ss; ss << var;
		if (auto name = ss.str(); !name.empty() && name.front() == '_') {
			return { "sbf" };
		}

		BOOST_LOG_TRIVIAL(error)
			<< "(Error) failed to find type for variable '"
			<< var << "'\n";
		return {};
	}

	std::map<nso<BAs...>, type> types;
	std::map<nso<BAs...>, std::optional<std::ofstream>> streams;

private:

	std::vector<assignment<BAs...>> sort(const assignment<BAs...>& sol) {
		auto compute_range = [](const assignment<BAs...>& sol) {
			size_t lower = std::numeric_limits<size_t>::max(), upper = 0;
			for (const auto& [var, value]: sol) {
				if (auto num = var
						| tau_parser::variable
						| tau_parser::io_var
						| tau_parser::out
						| tau_parser::offset
						| tau_parser::num
						| only_child_extractor<BAs...>
						| offset_extractor<BAs...>; num) {
					lower = std::min(lower, num | optional_value_extractor<size_t>);
					upper = std::max(upper, num | optional_value_extractor<size_t>);
				}
			}
			return std::make_pair(lower, upper);
		};

		auto [lower, upper] = compute_range(sol);

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "sort/lower: " << lower << "\n"
			<< "sort/upper: " << upper << "\n";
		#endif // DEBUG

		std::vector<assignment<BAs...>> result(upper - lower + 1);

		for (auto& [var, value]: sol) {
			if (auto num = var
					| tau_parser::variable
					| tau_parser::io_var
					| tau_parser::out
					| tau_parser::offset
					| tau_parser::num
					| only_child_extractor<BAs...>
					| offset_extractor<BAs...>; num) {
				auto io_var = var
					| tau_parser::variable
					| tau_parser::io_var
					| tau_parser::out
					| tau_parser::out_var_name
					| optional_value_extractor<nso<BAs...>>;
				result[num.value() - lower][io_var] = value;
			}
		}

		#ifdef DEBUG
		for (size_t i = 0; i < result.size(); ++i) {
			BOOST_LOG_TRIVIAL(trace)
				<< "sort/result/[" << i <<"]: ";
			for (const auto& [k, v]: result[i])
				BOOST_LOG_TRIVIAL(trace)
					<< "\t" << k << " := " << v << " ";
			BOOST_LOG_TRIVIAL(trace) << "\n";
		}
		#endif // DEBUG

		return result;
	}

	std::vector<assignment<BAs...>> complete(const std::vector<assignment<BAs...>>& sols) {
		std::vector<assignment<BAs...>> result(sols.size());
		for (size_t i = 0; i < sols.size(); ++i) {
			assignment<BAs...> nsol = sols[i];
			for (const auto& [var, _]: types)
				if ( !nsol.contains(var) ) nsol[var] = _0<BAs...>;
			result[i] = nsol;
		}

		#ifdef DEBUG
		for (size_t i = 0; i < result.size(); ++i) {
			BOOST_LOG_TRIVIAL(trace)
				<< "complete/result/[" << i <<"]: ";
			for (const auto& [k, v]: result[i])
				BOOST_LOG_TRIVIAL(trace)
					<< "\t" << k << " := " << v << " ";
			BOOST_LOG_TRIVIAL(trace) << "\n";
		}
		#endif // DEBUG

		return result;
	}
};

template<typename...BAs>
struct interpreter {
	interpreter(const std::vector<system<BAs...> >& systems,
		assignment<BAs...>& memory, const size_t time_point) :
						systems(systems),
						memory(memory),
						formula_time_point(time_point) {
		// initialize the variables
		for (const auto& system: systems) {
			for (const auto& [type, equations]: system) {
				for (const auto& o:
					select_top(equations,
						is_non_terminal<
							tau_parser::out_var_name
							, BAs...>)) {
					std::stringstream ss; ss << o;
					// Exclude temporary vars
					if (ss.str()[0] != '_')
						outputs.insert(o);
				}
			}
		}
	}

	std::pair<std::optional<assignment<BAs...>>, bool> step(auto& inputs) {
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
					is_child_non_terminal<tau_parser::io_var, BAs...>);

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
				for (const auto& o : outputs) {
					auto ot = build_out_variable_at_n(o, time_point);
					if (auto it = global.find(ot); it == global.end()) {
						global.emplace(ot, _0<BAs...>);
					}
				}
				if (global.empty()) {
					BOOST_LOG_TRIVIAL(info) <<
						"currently no output is specified";
				}
				// update time_point and formula_time_point
				if (time_point < formula_time_point) {
					++time_point;
					// Auto continue in the initial segment
					return {global, true};
				}
				else ++formula_time_point, ++time_point;
				// TODO (HIGH) remove old values from memory

				return {global, auto_continue};
			}
		}
		BOOST_LOG_TRIVIAL(error)
			<< "(Error) internal error: Tau specification is unexpectedly unsat\n";
		return {};
	}

	// store all the possible systems to be solved, each system corresponds to a
	// different clause.
	std::vector<system<BAs...>> systems;
	assignment<BAs...> memory;
	size_t formula_time_point;
	size_t time_point = 0;
	std::set<nso<BAs...>> outputs;

private:

	nso<BAs...> update_to_time_point(const nso<BAs...>& f) {
		// update the f according to current time_point, i.e. for each
		// input/output var which has a shift, we replace it with the value
		// corresponding to the current time_point minus the shift.
		std::map<nso<BAs...>, nso<BAs...>> changes;
		for (const auto& io_var:
				select_top(f, is_non_terminal<tau_parser::io_var, BAs...>))
			if (auto shift = io_var
						| only_child_extractor<BAs...>
						| tau_parser::offset
						| tau_parser::shift,
					var = shift
						| tau_parser::variable,
					num = shift
						| tau_parser::num
						| only_child_extractor<BAs...>
						| offset_extractor<BAs...>;
							num && var)
				changes[shift.value()] = build_num<BAs...>(formula_time_point - num.value());
			else if (auto offset = io_var
						| only_child_extractor<BAs...>
						| tau_parser::offset,
					variable = offset
						| tau_parser::variable;
							variable)
				changes[offset.value()] = wrap(
					tau_parser::offset,
						build_num<BAs...>(formula_time_point));
		return replace(f, changes);
	}

	// If a variable is assigned a variable V in a solution from the solver,
	// try to find a non-variable value by checking the solution for V
	void resolve_solution_dependencies(solution<BAs...>& s) {
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
};

// Return the lookback of the given unbound continuation
template<typename...BAs>
size_t compute_lookback(const nso<BAs...>& ubd_ctn) {
	std::vector<nso<BAs...>> io_vars = select_top(ubd_ctn, is_child_non_terminal<tau_parser::io_var, BAs...>);
	int_t max_shift = get_max_shift(io_vars);
	return (size_t)max_shift;
}

// Compute the type of the equation f = 0 or f != 0 stored in literal for the solver
template<typename input_t, typename output_t, typename...BAs>
std::optional<std::pair<type, nso<BAs...>>> compute_literal(const nso<BAs...>& literal,
		input_t& inputs, output_t& outputs) {
	if (auto io_var = find_top(literal,
			is_non_terminal<tau_parser::io_var, BAs...>); io_var) {
		if (auto in_var_name = io_var
				| tau_parser::in
				| tau_parser::in_var_name; in_var_name) {
			if(auto t = inputs.type_of(in_var_name.value()); t)
				return { make_pair(t.value() , literal) };
		} else if (auto out_var_name = io_var
				| tau_parser::out
				| tau_parser::out_var_name; out_var_name) {
			if (auto t = outputs.type_of(out_var_name.value()); t)
				return { make_pair(t.value() , literal) };
		}
	}
	BOOST_LOG_TRIVIAL(error)
		<< "(Error) unable to find variable in literal: " << literal << "\n";
	return {};
}

// Get the type for a clause of a local specification
template<typename input_t, typename output_t, typename...BAs>
std::optional<system<BAs...>> compute_system(const nso<BAs...>& clause,
		input_t& inputs, output_t& outputs) {
	auto is_literal = [](const nso<BAs...>& n) {
		return is_child_non_terminal<tau_parser::bf_eq, BAs...>(n)
			|| is_child_non_terminal<tau_parser::bf_neq, BAs...>(n);
	};

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace)
		<< "compute_system/clause: " << clause;
	#endif // DEBUG

	system<BAs...> sys;
	for (const auto& literal: select_top(clause, is_literal)) {
		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "compute_system/literal: " << literal;
		#endif // DEBUG

		if (auto l = compute_literal(literal, inputs, outputs); l) {
			if (sys.find(l.value().first) == sys.end()) sys[l.value().first] = l.value().second;
			else sys[l.value().first] = build_wff_and(sys[l.value().first], l.value().second);
		} else {
			BOOST_LOG_TRIVIAL(error)
				<< "(Error) unable to found equations in clause: " << clause << "\n";
			return {};
		}
	}
	return { sys };
}

// Find an executable specification from DNF
template<typename... BAs>
nso<BAs...> get_executable_spec(const nso<BAs...>& fm) {
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

// Return typed systems of equations for the solver corresponding to each clause
// in the unbound continuation
template<typename...BAs>
std::optional<std::vector<system<BAs...>>> compute_systems(const nso<BAs...>& ubd_ctn,
		auto& inputs, auto& outputs) {
	std::vector<system<BAs...>> systems;
	// Create blue-print for solver for each clause
	for (const auto& clause : get_dnf_wff_clauses(ubd_ctn)) {
		if (auto system = compute_system(clause, inputs, outputs); system)
			systems.emplace_back(std::move(system.value()));
		else {
			BOOST_LOG_TRIVIAL(trace)
				<< "unable to compute system of equations in: " << clause << "\n";
			continue;
		}
	}
	return systems;
}

template<typename...BAs>
std::optional<interpreter<BAs...>> make_interpreter(nso<BAs...> spec, auto& inputs, auto& outputs) {
	// Find a satisfiable unbound continuation from spec
	auto ubd_ctn = get_executable_spec(spec);
	if (ubd_ctn == nullptr) {
		BOOST_LOG_TRIVIAL(error)
			<< "(Error) Tau specification is unsat\n";
		return {};
	}

	// compute the different systems to be solved
	auto systems = compute_systems(ubd_ctn, inputs, outputs);
	if (!systems.has_value() || systems.value().empty()) {
		return {}; // error
	}
	// compute the initial time point for execution
	size_t lookback = compute_lookback(ubd_ctn);
	//after the above, we have the interpreter ready to be used.
	assignment<BAs...> memory;
	return interpreter<BAs...>{
		systems.value(), memory, lookback
	};
};

template<typename...BAs>
void run(const nso<BAs...>& form, auto& inputs, auto& outputs) {
	auto spec = normalizer(form);
	auto intrprtr = make_interpreter(spec, inputs, outputs);
	if (!intrprtr) return;

	BOOST_LOG_TRIVIAL(info) << "Please provide requested input or press ENTER to terminate";
	BOOST_LOG_TRIVIAL(info) << "If no input is requested, press ENTER to proceed to next output, or type q(uit) to terminate\n\n";

	// Continuously perform execution step until user quits
	while (true) {
		auto [output, auto_continue] = intrprtr.value().step(inputs);
		// If the user provided empty input for an input stream, quit
		if (!output.has_value()) return;
		if (!outputs.write(output.value())) return;
		// If there is no input, ask the user if execution should continue
		if (!auto_continue) {
			std::string line;
			term::enable_getline_mode();
			std::getline(std::cin, line);
			term::disable_getline_mode();
			if (line == "q" || line == "quit")
				return;
		} else std::cout << "\n";
	}
}

} // namespace idni::tau

# endif //__INTERPRETER_H__