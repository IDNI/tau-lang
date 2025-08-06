// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "interpreter_types.h"

namespace idni::tau_lang {

/*
 * Struct to manage function and predicate definitions
 * as well as input and output stream definitions
 */
template<NodeType node>
struct definitions {
	using tau = tree<node>;
	size_t add (htref head, htref body) {
		// Check if rule is updated
		for (size_t i = 0; i < heads.size(); ++i) {
			if (heads[i] == head) {
				// update definition
				heads[i] = head;
				bodies[i] = body;
				return i;
			}
		}
		heads.push_back(head);
		bodies.push_back(body);
		return heads.size() - 1;
	}

	rewriter::rules get_sym_defs () const {
		rewriter::rules r;
		for (size_t i = 0; i < heads.size(); ++i)
			r.emplace_back(heads[i], bodies[i]);
		return r;
	}

	spec_context<node>& get_io_context () {
		return ctx;
	}

	io_defs<node>& get_input_defs () {
		return ctx.inputs;
	}

	io_defs<node>& get_output_defs () {
		return ctx.outputs;
	}

	size_t size () const {
		DBG(assert(heads.size() == bodies.size());)
		return heads.size();
	}

	rewriter::rule back () const {
		return std::make_pair(heads.back(), bodies.back());
	}

	rewriter::rule operator[](const size_t i) const {
		return std::make_pair(heads[i], bodies[i]);
	}

	static definitions& instance() {
		static definitions d;
		return d;
	}
private:
	definitions() = default;

	std::vector<htref> heads = {};
	std::vector<htref> bodies = {};

	spec_context<node> ctx; // input and output definitions
};

}

#endif //DEFINITIONS_H
