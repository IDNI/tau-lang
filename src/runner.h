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

template<typename factory_t, typename...BAs>
struct inputs {

	inputs(factory_t factory, std::set<stream_ba<BAs...>>& streams): factory(factory) {
		// open the corresponding streams for input and store them in streams
	}

	~inputs() {
		// close the streams
	}

	// end of inputs
	bool eoi() {
		// check if all the streams are at the end of the file
		return true;
	}

	assignment<BAs...> next() {
		// read the next value from the streams and return it as an assignment,
		// the vars are set according to the time_point so we could just
		// replace them in the phi_inf clauses.
		time_point++;
		return {};
	}

	std::map<stream_ba<BAs...>, std::ifstream> streams;
	size_t time_point = 0;
	factory_t factory;
};

template<typename...BAs>
struct outputs {

	outputs(std::set<stream_ba<BAs...>>& streams) {
		// open the corresponding streams for output and store them in streams
	}

	~outputs() {
		// close the streams
	}

	std::map<stream_ba<BAs...>, std::ofstream> streams;
};


template<typename factory_t, typename...BAs>
void operator>>(inputs<factory_t, BAs...>& in, assignment<BAs...>& out) {
	out.clear();
	// for each stream in in.streams, read the value from the file,
	// parsed it and store it in out.
}

template<typename...BAs>
void operator<<(outputs<BAs...>& out, const solution<BAs...>& sol) {
	// for each stream in out.streams, write the value from the solution
}

template<typename factory_t, typename...BAs>
struct interpreter {

	using type = std::string;

	// A system represent a clause to be solved. It maps the different
	// equations of the clause according to its type.
	using system = std::map<type, nso<BAs...>>;

	interpreter(factory_t factory, nso<BAs...> phi_inf): factory(factory) {
		// 1.- split phi_inf in clauses
		// 2.- for each clause, split it into several equation systems according to
		// its type and store it in systems
	}

	std::optional<solution<BAs...>> run(const assignment<BAs...>& inputs) {
		// for each system in systems try to solve it, if it is not possible
		// continue with the next system.
		for (const auto& system: systems) {
			std::map<type, solution<BAs...>> solutions;
			bool solved = true;
			for (const auto& [type, equations]: system) {
				auto substituted = replace(equations, inputs);
				auto solution = solve(substituted, factory, type);
				if (solution.has_value()) solutions[type] = solution.value();
				else {
					solved = false;
					break;
				}
			}
			if (solved) {
				solution<BAs...> output;
				for (const auto& [type, solution]: solutions)
					output.insert(solution.begin(), solution.end());
				return output;
			}
		}
		return {};
	}

	// Set of all possible systems to be solved, each system corresponds to a
	// different clause.
	std::set<system> systems;
	std::map<var<BAs...>, nso<BAs...>> previous;
	factory_t factory;
};

template<typename factory_t, typename...BAs>
void run(const inputs<factory_t, BAs...>& in, const outputs<BAs...>& out,
		nso<BAs...>& phi_inf) {
	assignment<BAs...> inputs;
	interpreter<factory_t, BAs...> i(in.factory, phi_inf);
	while (!in.eoi()) {
		in >> inputs;
		auto solution = i.run(inputs);
		if (solution.has_value()) out << solution.value();
		else {
			std::cout << "no solution\n";
			return;
		}
	}
}

} // namespace idni::tau

# endif //__EXECUTOR_H__