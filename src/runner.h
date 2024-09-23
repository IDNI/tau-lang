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

// TODO (IMPORTANT) execution
// TODO (IMPORTANT) basic implementations
// TODO (IMPORTANT) testing


#ifndef __EXECUTOR_H__
#define __EXECUTOR_H__

#include <fstream>

#include "tau_ba.h"
#include "solver.h"
#include "satisfiability.h"

namespace idni::tau {

template<typename... BAs>
struct stream_ba {
	std::string file_name;
	nso<BAs...> name;
	std::string type;

	auto operator<=>(const stream_ba&) const = default;
	bool operator==(const stream_ba&) const = default;
	bool operator!=(const stream_ba&) const = default;
};

// Represents an solution of variables to values.
template<typename...BAs>
using assignment = solution<BAs...>;

template<typename...BAs>
struct inputs {

	inputs(std::set<stream_ba<BAs...>>& streams) {
		// open the corresponding streams for input and store them in streams
		for (const auto& stream: streams)
			this->streams[stream] = std::ifstream(stream.file_name);
	}

	~inputs() {
		// close the streams
		for (const auto& [stream, file]: streams) file.close();
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

	std::map<stream_ba<BAs...>, std::ifstream> streams;
};

template<typename...BAs>
struct outputs {

	outputs(std::set<stream_ba<BAs...>>& streams) {
		// open the corresponding streams for output and store them in streams
		for (const auto& stream: streams)
			this->streams[stream] = std::ofstream(stream.file_name);
	}

	~outputs() {
		// close the streams
		for (const auto& [stream, file]: streams) file.close();
	}

	// is good
	bool good() const {
		// check if all the streams are good
		for (const auto& [stream, file]: streams)
			if (!file.good()) return false;
		return true;
	}

	bool ready() {
		return good();
	}

	std::map<stream_ba<BAs...>, std::ofstream> streams;
};

template<typename...BAs>
struct interpreter {
	// A type is a string representing the type of the system to be solved.
	using type = std::string;
	// A system represent a clause to be solved. It maps the different
	// equations of the clause according to its type.
	using system = std::map<type, nso<BAs...>>;
	// Loopbacks, input, output and constants
	struct loopbacks {
		size_t input = 0;
		size_t output = 0;
		size_t constant = 0;
	};

	interpreter(nso<BAs...> phi_inf, inputs<BAs...>& ins, outputs<BAs...>& outs): ins(ins), outs(outs) {
		// compute the different systems to be solved
		compute_systems(phi_inf, ins);
		// compute input and output lookback
		compute_lookbacks(phi_inf);
		// compute initial memory
		compute_initial_memory(phi_inf);
		//after the bove, we have the interpreter ready to be used.
	}

	void compute_initial_memory(const nso<BAs...>& phi_inf) {
		// compute th initial
		size_t max_lookback = max(lbcks.input, lbcks.output, lbcks.constant);
		// we include the max lookback in the memory to deal with the initial
		// case.
		for (; this->time_point < max_lookback; ++this->time_point) {
			if (!ready()) return;
			assignment<BAs...> current_inputs;
			ins >> current_inputs;
			for (const auto& [var, value]: current_inputs) {
				auto timed_var = get_timed_var(var);
				this->memory[timed_var] = value;
			}
			for (const auto& out: this->outs.streams) {
				auto current_timed_output = get_timed_var(out.first.name);
				if (!memory.contains(current_timed_output))
					this->memory[current_timed_output] = _0<BAs...>;
			}
		}

		if (this->error = error()) return;
		this->time_point = max_lookback;
	}

	void step() {
		// for each system in systems try to solve it, if it is not possible
		// continue with the next system.
		bool unsolvable = false;
		for (const auto& system: systems) {
			std::map<type, solution<BAs...>> solutions;
			unsolvable = false;
			// solve the equations fro each type in the system
			for (const auto& [type, equations]: system) {
				// rewriting the inputs and inserting them into memory
				auto nsystem = update_inf(system);
				auto substituted = replace(equations, memory);
				auto solution = solve(substituted, type);
				if (solution.has_value()) solutions[type] = solution.value();
				else { unsolvable = true; break; }
			}
			if (!unsolvable) {
				for (const auto& [type, solution]: solutions)
					this->memory.insert(solution.begin(), solution.end());
				break;
			}
		}
		err = unsolvable;
	}

	bool ready() {
		return ins.ready() && outs.ready() && !error();
	}

	bool error() const {
		return err;
	}

	// store all the possible systems to be solved, each system corresponds to a
	// different clause.
	std::set<system> systems;
	assignment<BAs...> memory;
	inputs<BAs...> ins;
	outputs<BAs...> outs;
	loopbacks lbcks;
	size_t time_point = 0;
	bool err = false;

private:

	nso<BAs...> get_timed_var(const nso<BAs...>& var_name) {
		auto type = is_non_terminal<tau_parser::in_var_name, BAs...>(var_name)
			? tau_parser::in : tau_parser::out;
		return wrap<BAs...>(tau_parser::io_var, {
			type, {var_name,
				wrap<BAs...>(tau_parser::offset,
					wrap<BAs...>(tau_parser::num, {tau_sym<BAs...>(this->time_point)}))}});

	}

	nso<BAs...> update_inf(const nso<BAs...>& phi_inf) {
		// update the phi_inf according to current time_point
		std::map<nso<BAs...>, nso<BAs...>> changes;
		for (const auto& io_var: select_top(phi_inf, is_non_terminal<tau_parser::io_var, BAs...>)) {
			auto shift = io_var | only_child_extractor<BAs...> | tau_parser::offset | tau_parser::shift | tau_parser::num;
			if (shift) {
				auto shift = io_var | only_child_extractor<BAs...> | tau_parser::offset | tau_parser::shift;
				if (auto num = shift | tau_parser::num; num) {
					auto nnum = this->time_point - num.value() | only_child_extractor<BAs...> | optional_value_extractor<size_t>;
					changes[shift] = wrap<BAs...>(tau_parser::num, {tau_sym<BAs...>(nnum)});
				}
			}
		}
		return replace(phi_inf, memory);
	}

	void compute_loopbacks(const nso<BAs...>& phi_inf) {
		// for each loopback in phi_inf, store the input and output lookbacks
		// in a pair and return it.
		loopbacks lbs;
		for (const auto& io_var: select_top(phi_inf, is_non_terminal<tau_parser::io_var, BAs...>)) {
			if (auto shift = io_var | tau_parser::in | tau_parser::offset | tau_parser::shift | tau_parser::num; shift) {
				lbcks.input = max(lbs.input, shift | optional_value_extractor<size_t>);
			} else if (auto shift = io_var | tau_parser::out | tau_parser::offset | tau_parser::shift | tau_parser::num; shift) {
				lbcks.output = max(lbs.output, shift | optional_value_extractor<size_t>);
			} else if (auto num = io_var | tau_parser::in | tau_parser::offset | tau_parser::num; num) {
				lbcks.constant = max(lbs.constant, num | optional_value_extractor<size_t>);
			}
		}
	}

	void compute_systems(const nso<BAs...>& phi_inf, const inputs<BAs...>& in) {
			static auto is_literal = [](const nso<BAs...>& n) {
			return is_child_non_terminal<tau_parser::bf_eq, BAs...>(n)
				|| is_child_non_terminal<tau_parser::bf_neq, BAs...>(n);
		};

		// split phi_inf in clauses
		for (auto& clause: get_dnf_wwf_clauses(phi_inf)) {
			// for each clause, split it into several equation systems according to
			// its type and store it in systems
			system clause_system;
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
	}
};

template<typename...BAs>
void operator>>(const inputs<BAs...>& ins, assignment<BAs...>& inputs) {
	// for each stream in in.streams, read the value from the file,
	// parsed it and store it in out.
	static nso_factory<BAs...> factory;
	for (const auto& [stream, file]: ins.streams) {
		std::string line;
		std::getline(file, line);
		inputs[stream.name] = factory.parse(line, stream.type);
	}
}

template<typename...BAs>
void operator<<(outputs<BAs...>& outs, const interpreter<BAs...>& state) {
	// for each stream in out.streams, write the value from the solution
	for (const auto& [stream, file]: outs.streams) {
		// consider also the time_point
		auto value = outs.find(stream.name);
		if (value != outs.end()) file << value->second;
	}
}

template<typename...BAs>
void operator>>(const interpreter<BAs...>& state, assignment<BAs...>& outputs) {
	// for each stream in out.streams, write the value from the solution
	outputs = state.current_outputs;
}

template<typename...BAs>
bool run(const inputs<BAs...>& ins, const outputs<BAs...>& outs, nso<BAs...>& phi_inf) {
	assignment<BAs...> inputs;
	interpreter<BAs...> i(phi_inf, ins, outs);
	while (i.ready()) i.step();
	return i.error();
}

} // namespace idni::tau

# endif //__EXECUTOR_H__