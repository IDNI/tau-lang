// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentioned cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

#ifndef __RUNNER_H__
#define __RUNNER_H__

#include <fstream>

#include "tau_ba.h"
#include "solver.h"
#include "satisfiability.h"

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

template<typename...BAs>
struct finputs {

	finputs() = default;

	finputs(std::map<nso<BAs...>, type>& types,
			std::map<nso<BAs...>, std::ifstream>& streams): streams(streams), types(types) {}

	finputs(std::map<nso<BAs...>, type>& types,
			std::map<nso<BAs...>, filename>& streams): types(types) {
		// open the corresponding streams for input and store them in streams
		for (const auto& [vd, fn]: streams)
			this->streams[vd] = std::ifstream(fn);
	}

	~finputs() {
		// close the streams
		for (auto& [_, file]: streams) file.close();
	}


	std::optional<assignment<BAs...>> read() {
		// for each stream in in.streams, read the value from the file,
		// parsed it and store it in out.
		static nso_factory<BAs...> factory;
		assignment<BAs...> current;
		for (const auto& [var, file]: streams) {
			std::string line;
			std::getline(file, line);
			// TODO MEDIUM add logging in case of error
			if (!line.empty()) return {}; // error
			current[var] = factory.parse(line, types[var]);
		}
		return current;
	}

	std::map<nso<BAs...>, type> types;
	std::map<nso<BAs...>, std::ifstream> streams;
};

template<typename...BAs>
struct foutputs {

	foutputs() = default;

	foutputs(std::map<io_var_name<BAs...>, std::ofstream>& streams): streams(streams) {}

	foutputs(std::map<io_var_name<BAs...>, filename>& streams) {
		// open the corresponding streams for output and store them in streams
		for (const auto& [var, file]: streams)
			this->streams[var] = std::ofstream(file);
	}

	~foutputs() {
		// close the streams
		for (auto& [_, file]: streams) file.close();
	}

	bool write(const assignment<BAs...>& outputs) {
		// sorting by time
		auto sorted = sort(outputs);
		// for each stream in out.streams, write the value from the solution
		for (const auto& output: sorted) {
			for (const auto& [var, value]: output)
				if (auto stream = streams.find(var); stream)
					stream->second << value;
				else return false; // error
		}
		return true; // success
	}

	std::map<nso<BAs...>, std::ofstream> streams;

private:

	std::vector<assignment<BAs...>> sort(const solution<BAs...>& sol) {
		auto compute_range = [](const solution<BAs...>& sol) {
			size_t lower = std::numeric_limits<size_t>::max(), upper = 0;
			for (const auto& [var, value]: sol) {
				if (auto num = var
						| tau_parser::io_var
						| tau_parser::out
						| tau_parser::offset
						| tau_parser::num; num) {
					lower = std::min(lower, num | offset_extractor<BAs...>
						| optional_value_extractor<size_t>);
					upper = std::max(upper, num | offset_extractor<BAs...>
						| optional_value_extractor<size_t>);
				}
			}
			return std::make_pair(lower, upper);
		};

		auto [lower, upper] = compute_range(sol);
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
				auto io_var = var | tau_parser::io_var | tau_parser::out
					| tau_parser::out_var_name |  offset_extractor<BAs...>;

				result[num | optional_value_extractor<size_t>][io_var] = value;
			}
		}
		return result;
	}
};

// A system represent a clause to be solved. It maps the different
// equations of the clause according to its type.
template<typename...BAs>
using system = std::map<type, nso<BAs...>>;

template<typename...BAs>
size_t compute_initial_time_point(const nso<BAs...>& phi_inf) {
	// for each loopback in phi_inf, store the input and output lookbacks
	// in a pair and return it.
	size_t initial_time_point = 0;
	for (const auto& io_var: select_top(phi_inf, is_non_terminal<tau_parser::io_var, BAs...>)) {
		if (auto shift = io_var
				| tau_parser::in
				| tau_parser::offset
				| tau_parser::shift
				| tau_parser::num; shift) {
			initial_time_point = max(initial_time_point, shift | optional_value_extractor<size_t>);
		} else if (auto shift = io_var
				| tau_parser::out
				| tau_parser::offset
				| tau_parser::shift
				| tau_parser::num; shift) {
			initial_time_point = max(initial_time_point, shift | optional_value_extractor<size_t>);
		} else if (auto num = io_var
				| tau_parser::in
				| tau_parser::offset
				| tau_parser::num; num) {
			initial_time_point = max(initial_time_point, num | optional_value_extractor<size_t>);
		}
	}
	return initial_time_point;
}

template<typename...BAs>
struct interpreter {

	assignment<BAs...> step(const assignment<BAs...>& inputs) {
		// update the memory with the inputs
		for (const auto& [var, value]: inputs)
			memory[build_in_var(var, time_point)] = value;
		// for each system in systems try to solve it, if it is not possible
		// continue with the next system.
		bool unsolvable = false;
		// for each system in systems try to solve it, if it is not possible
		// continue with the next system.
		for (const auto& system: this->systems) {
			std::map<type, solution<BAs...>> solutions;
			unsolvable = false;
			// solve the equations fro each type in the system
			for (const auto& [type, equations]: system) {
				// rewriting the inputs and inserting them into memory
				auto current = update_to_time_point(equations);
				auto solution = solve(current, type);
				if (solution.has_value()) solutions[type] = solution.value();
				else { unsolvable = true; break; }
			}
			if (!unsolvable) {
				solution<BAs...> global;
				for (const auto& [type, solution]: solutions) {
					memory.insert(solution.begin(), solution.end());
					for (const auto& [var, value]: solution) {
						global[var] = value;
					}
				}
				time_point += 1;
				// TODO (HIGH) remove old values from memory
				return global;
			}
		}
		return {}; // error
	}

	// store all the possible systems to be solved, each system corresponds to a
	// different clause.
	std::set<system<BAs...>> systems;
	assignment<BAs...> memory;
	size_t time_point;

private:

	nso<BAs...> update_to_time_point(const nso<BAs...>& f) {
		// update the f according to current time_point, i.e. for each
		// input/ouput var which has a shift, we replace it with the value
		// corresponding to the current time_point minnus the shift.
		std::map<nso<BAs...>, nso<BAs...>> changes;
		for (const auto& io_var:
				select_top(f, is_non_terminal<tau_parser::io_var, BAs...>))
			if (auto shift = io_var
						| only_child_extractor<BAs...>
						| tau_parser::offset
						| tau_parser::shift,
					num = shift
						| only_child_extractor<BAs...>
						| offset_extractor<BAs...>; shift && num)
				changes[shift.value()] = build_num<BAs...>(time_point - num.value());
		return replace(f, memory);
	}
};

template<typename input_t, typename...BAs>
std::optional<assignment<BAs...>> compute_initial_memory(const nso<BAs...>& phi_inf,
		const size_t& initial_execution_time, input_t& inputs) {
	// we include the max lookback in the memory to deal with the initial
	// case.
	assignment<BAs...> memory;
	for (size_t n = 0; n < initial_execution_time; ++n) {
		assignment<BAs...> current;
		if (auto current = inputs.read(); current)
			for (const auto& [var, value]: current)
				memory[build_in_variable_at_n(var, n)] = value;
		else return {}; // error
	}
	return memory;
}

template<typename input_t, typename output_t, typename...BAs>
std::set<system<BAs...>> compute_system(const nso<BAs...>& dnf_clause,
		input_t& inputs, output_t& outputs) {
	if (auto io_var = find_top(dnf_clause,
			is_child_non_terminal<tau_parser::io_var, BAs...>); io_var) {
		if (auto in_var_name = io_var
				| tau_parser::in
				| tau_parser::in_var_name; in_var_name) {
			if(auto it = inputs.types.find(in_var_name); it)
				return { { it->second , dnf_clause } };
		} else if (auto out_var_name = io_var
				| tau_parser::out
				| tau_parser::out_var_name; out_var_name) {
			if (auto it = outputs.types.find(out_var_name); it)
				return { { it->second, dnf_clause } };
		}
	}

	return {}; // error
}

template<typename input_t, typename output_t, typename...BAs>
std::set<system<BAs...>> compute_systems(const nso<BAs...>& phi_inf,
		input_t& inputs, output_t& outputs) {
	std::set<system<BAs...>> systems;
	// split phi_inf in clauses
	for (auto& clause: get_dnf_wwf_clauses(phi_inf))
		if (auto system = compute_system(clause, inputs, outputs); system)
			systems.insert(system);
		else return {}; // error
	return systems;
}

template<typename input_t, typename...BAs>
interpreter<BAs...> make_interpreter(nso<BAs...> phi_inf, input_t& inputs) {
	// compute the different systems to be solved
	auto systems = compute_systems(phi_inf, inputs);
	// compute the initial time point for execution
	size_t initial_execution_time = compute_initial_execution_time(phi_inf);
	// compute initial memory
	auto memory = compute_initial_memory(phi_inf, initial_execution_time, inputs);
	//after the bove, we have the interpreter ready to be used.
	return interpreter<BAs...>{ systems, memory, initial_execution_time };
};


template<typename input_t, typename output_t, typename...BAs>
void run(const nso<BAs...>& phi_inf, input_t& inputs, output_t& outputs) {
	auto intrprtr = make_interpreter(phi_inf, inputs, outputs);
	while (true) {
		if (auto current = inputs.read(); inputs) {
			if (auto output = intrprtr.step(current.value()); output.size()) {
				if (!outputs.write(output)) return;
			} else return;
		} else return;
	}
}

} // namespace idni::tau

# endif //__RUNNER_TMPL_H__