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

#include <iostream>

#include "normalizer2.h"

using namespace std;
using namespace idni::tau;

// we need a data structure that checks the appropriate size
template<size_t size, typename...BAs>
using tau_input = std::map<sp_tau_node<BAs...>, std::variant<BAs...>>;

// we need a data structure that roll on new inputs
template<size_t size, typename...BAs>
using tau_inputs = std::vector<tau_output>;

// we need a data structure that checks the appropriate size
template<size_t size, typename...BAs>
using tau_output = std::map<sp_tau_node<BAs...>, std::variant<BAs...>>;

// we need a data structure that roll on new outputs
template<size_t size, typename...BAs>
using tau_outputs = std::vector<tau_output>;

// we need to bind the input/output values to sp_tau_nodes in the program,
// the inputs would be the input variables of the program and would be binded from
// actual parsed inputs or given constants if executed as a library.

template<size_t input_size, size_t output_size, typename...BAs>
struct tau_execution_state {

	// maybe we could use promise/futures for input/outputs
	tau_outputs<output_size, BAs...> step(const tau_input<input_size, BAs...>& f) {
		// TODO (HIGH) implement tau_step
		return tau_output<output_size, BAs...>();
	}

	// they should be a rolling fixed size queue
	// index in the datastructre denotes time
	tau_outputs<output_size, BAs...> outputs;
	tau_inputs<input_size, BAs...> inputs;
};

template<typename...BAs>
bool satisfiable(const formula<BAs...>& f) {
	// TODO (HIGH) check that the formula is satisfiable
	return false;
}

template<size_t input_size, size_t output_size, typename...BAs>
std::optional<tau_execution_state<input_size, output_size, BAs...>> execute(const formula<BAs...>& f) {
	// TODO (HIGH) prepare formula for execution
	// the optional would be empty if the formula is not satisfiable
	// otherwise it would retunr an initial tau_execution_state
	return std::optional<tau_execution_state<input_size, output_size, BAs...>>();
}
