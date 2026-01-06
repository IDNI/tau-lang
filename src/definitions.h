// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "io_context.h"

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
			if (tau::get(heads[i]) == tau::get(head)) {
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

	io_context<node>& get_io_context () {
		return ctx;
	}

	subtree_map<node, size_t>& get_input_defs () {
		return ctx.inputs;
	}

	subtree_map<node, size_t>& get_output_defs () {
		return ctx.outputs;
	}

	subtree_map<node, size_t>& get_global_scope() {
		for (auto [var, type] : ctx.types) {
			if (auto it = ctx.global_scope.find(var);
				it == ctx.global_scope.end())
					ctx.global_scope[var] = type;
		}
		return ctx.global_scope;
	}

	subtree_map<node, size_t>& get_types() {
		return ctx.types;
	}

	size_t size() const {
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

	io_context<node> ctx; // input and output definitions
};

}

#endif //DEFINITIONS_H
