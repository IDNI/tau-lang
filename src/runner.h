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
struct inputs {

	inputs() = default;

	inputs(std::map<var_desc<BAs...>, std::ifstream>& streams): streams(streams) {}

	inputs(std::map<var_desc<BAs...>, filename>& streams) {
		// open the corresponding streams for input and store them in streams
		for (const auto& [vd, fn]: streams)
			this->streams[vd] = std::ifstream(fn);
	}

	~inputs() {
		// close the streams
		for (auto& [vd, file]: streams) file.close();
	}

	// end of inputs
	bool eoi() const {
		// check if all the streams are at the end of the file
		for (const auto& [stream, file]: streams)
			if (!file.eof()) return false;
		return true;
	}

	// is good
	bool good() const {
		// check if all the streams are good
		for (const auto& [stream, file]: streams)
			if (!file.good()) return false;
		return true;
	}

	bool ready() {
		return !eoi() && good();
	}

	std::map<var_desc<BAs...>, std::ifstream> streams;
};

template<typename...BAs>
bool operator>>(const inputs<BAs...>& ins, assignment<BAs...>& inputs) {
	// for each stream in in.streams, read the value from the file,
	// parsed it and store it in out.
	static nso_factory<BAs...> factory;
	for (const auto& [vd, file]: ins.streams) {
		std::string line;
		std::getline(file, line);
		if (!line.empty()) return false; // error
		inputs[vd.first] = factory.parse(line, vd.second);
	}
	return true; // success
}

template<typename...BAs>
struct outputs {

	outputs() = default;

	outputs(std::map<io_var_name<BAs...>, std::ofstream>& streams): streams(streams) {}

	outputs(std::map<io_var_name<BAs...>, filename>& streams) {
		// open the corresponding streams for output and store them in streams
		for (const auto& [vn, fn]: streams)
			this->streams[vn] = std::ofstream(fn);
	}

	~outputs() {
		// close the streams
		for (auto& [vn, file]: streams) file.close();
	}

	// is good
	bool good() const {
		// check if all the streams are good
		for (const auto& [vn, file]: streams)
			if (!file.good()) return false;
		return true;
	}

	bool ready() {
		return good();
	}

	std::map<io_var_name<BAs...>, std::ofstream> streams;
};

template<typename...BAs>
bool operator<<(outputs<BAs...>& outs, const std::vector<assignment<BAs...>>& outputs) {
	// for each stream in out.streams, write the value from the solution
	for (const auto& output: outputs) {
		for (const auto& [vn, file]: outs.streams) {
			auto value = output.find(vn);
			if (value == output.end()) return false; // error
			file << value->second;
		}
	}
	return true; // success
}

// A system represent a clause to be solved. It maps the different
// equations of the clause according to its type.
template<typename...BAs>
using system = std::map<type, nso<BAs...>>;

// Loopbacks, input, output and constants
struct loopbacks {
	size_t input = 0;
	size_t output = 0;
	size_t constant = 0;
};

template<typename...BAs>
loopbacks compute_loopbacks(const nso<BAs...>& phi_inf) {
	// for each loopback in phi_inf, store the input and output lookbacks
	// in a pair and return it.
	loopbacks lbs;
	for (const auto& io_var: select_top(phi_inf, is_non_terminal<tau_parser::io_var, BAs...>)) {
		if (auto shift = io_var | tau_parser::in | tau_parser::offset | tau_parser::shift | tau_parser::num; shift) {
			lbs.input = max(lbs.input, shift | optional_value_extractor<size_t>);
		} else if (auto shift = io_var | tau_parser::out | tau_parser::offset | tau_parser::shift | tau_parser::num; shift) {
			lbs.output = max(lbs.output, shift | optional_value_extractor<size_t>);
		} else if (auto num = io_var | tau_parser::in | tau_parser::offset | tau_parser::num; num) {
			lbs.constant = max(lbs.constant, num | optional_value_extractor<size_t>);
		}
	}
	return lbs;
}

template<typename...BAs>
struct interpreter {

	assignment<BAs...> timed_step(const assignment<BAs...>& inputs) {
		// update the memory with the inputs
		for (const auto& [var, value]: inputs) memory[var] = value;
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
				auto nsystem = update_inf(equations);
				auto substituted = replace(equations, memory);
				auto solution = solve(substituted, type);
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

	std::vector<assignment<BAs...>> step(const assignment<BAs...>& inputs) {
		// add time to inputs
		assignment<BAs...> ninput;
		for (const auto& [var, value]: inputs) {
			auto timed_var = build_in_var(var, this->time_point);
			ninput[timed_var] = value;
		}

		auto output = timed_step(ninput);
		return to_vector_assigment(output);
	}

	// store all the possible systems to be solved, each system corresponds to a
	// different clause.
	std::set<system<BAs...>> systems;
	assignment<BAs...> memory;
	size_t lookback;
	size_t time_point;

private:

	std::vector<assignment<BAs...>> to_vector_assigment(const solution<BAs...>& sol) {
		auto compute_range = [](const solution<BAs...>& sol) {
			size_t lower = std::numeric_limits<size_t>::max(), upper = 0;
			for (const auto& [var, value]: sol) {
				if (auto num = var | tau_parser::io_var | tau_parser::out
					| tau_parser::offset | tau_parser::num; num) {
					lower = std::min(lower, num | offset_extractor<BAs...> | optional_value_extractor<size_t>);
					upper = std::max(upper, num | offset_extractor<BAs...> | optional_value_extractor<size_t>);
				}
			}
			return std::make_pair(lower, upper);
		};

		auto [lower, upper] = compute_range(sol);
		std::vector<assignment<BAs...>> result(upper - lower + 1);

		for (auto& [var, value]: sol) {
			if (auto num = var | tau_parser::io_var | tau_parser::out
					| tau_parser::offset | tau_parser::num | only_child_extractor<BAs...>; num) {
				auto io_var = var | tau_parser::io_var | tau_parser::out
					| tau_parser::out_var_name |  offset_extractor<BAs...>;
				result[num | offset_extractor<BAs...>  | optional_value_extractor<size_t>][io_var] = value;
			}
		}
		return result;
	}

	nso<BAs...> update_inf(const nso<BAs...>& phi_inf) {
		// update the phi_inf according to current time_point, i.e. for each
		// input/ouput var which has a shift, we replace it with the value
		// corresponding to the current time_point minnus the shift.
		std::map<nso<BAs...>, nso<BAs...>> changes;
		for (const auto& io_var: select_top(phi_inf, is_non_terminal<tau_parser::io_var, BAs...>))
			if (auto shift = io_var | only_child_extractor<BAs...>
						| tau_parser::offset | tau_parser::shift,
					num = shift | only_child_extractor<BAs...>
						| offset_extractor<BAs...>; shift && num)
				changes[shift.value()] = build_num<BAs...>(time_point - num.value());
		return replace(phi_inf, memory);
	}
};

template<typename input_t, typename output_t, typename...BAs>
assignment<BAs...> compute_initial_memory(const nso<BAs...>& phi_inf, const loopbacks& lbcks, input_t& ins, output_t& outs) {
	// compute the initial
	size_t max_lookback = max(lbcks.input, lbcks.output, lbcks.constant);
	// we include the max lookback in the memory to deal with the initial
	// case.
	assignment<BAs...> memory;
	for (size_t time_point = 0; time_point < max_lookback; ++time_point) {
		assignment<BAs...> current_inputs;
		ins >> current_inputs;
		for (const auto& [var, value]: current_inputs) {
			auto timed_var = build_in_var(var, time_point);
			memory[timed_var] = value;
		}
	}
	return memory;
}

template<typename input_t, typename...BAs>
std::set<system<BAs...>> compute_systems(const nso<BAs...>& phi_inf, const input_t& in) {
	static auto is_literal = [](const nso<BAs...>& n) {
		return is_child_non_terminal<tau_parser::bf_eq, BAs...>(n)
			|| is_child_non_terminal<tau_parser::bf_neq, BAs...>(n);
	};

	std::set<system<BAs...>> systems;
	// split phi_inf in clauses
	for (auto& clause: get_dnf_wwf_clauses(phi_inf)) {
		// for each clause, split it into several equation systems according to
		// its type and store it in systems
		system<BAs...> clause_system;
		for (auto& literal: select_top(clause, is_literal)) {
			// for each literal, get the type and store it in the clause_system
			// conjuncted with the other literals of the same type.
			auto var = find_top(literal, is_child_non_terminal<tau_parser::variable, BAs...>);
			for (const auto& stream: in.streams) {
				if (var == stream.name) {
					auto type = stream.type;
					if (clause_system.find( stream.type) == clause_system.end())
						clause_system[stream.type] = literal;
					else clause_system[stream.type] = clause_system[stream.type] & literal;
				}
			}
		}
		systems.insert(clause_system);
	}
	return systems;
}

template<typename input_t, typename output_t, typename...BAs>
interpreter<BAs...> make_interpreter(nso<BAs...> phi_inf, input_t& ins, output_t& outs) {
	// compute the different systems to be solved
	auto systems = compute_systems(phi_inf, ins);
	// compute input and output lookback
	auto lkbck = compute_lookbacks(phi_inf);
	// compute initial memory
	auto memory = compute_initial_memory(phi_inf, lkbck, ins, outs);
	//after the bove, we have the interpreter ready to be used.
	auto max = max(lkbck.input, lkbck.output, lkbck.constant);
	return interpreter<BAs...>{ systems, memory, max, max };
};


template<typename...BAs>
bool run(const nso<BAs...>& phi_inf, const inputs<BAs...>& ins, const outputs<BAs...>& outs) {
	auto i = make_interpreter(phi_inf, ins, outs);
	while (ins.ready()) {
		assignment<BAs...> inputs;
		ins >> inputs;
		outs << i.step(inputs);
	}
	return i.error();
}

} // namespace idni::tau

# endif //__RUNNER_TMPL_H__