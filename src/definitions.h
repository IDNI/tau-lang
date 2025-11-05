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

	subtree_map<node, size_t>& get_global_scope() {
		auto add = [this](size_t sid, size_t type) {
			tref var_name = build_var_name<node>(sid);
			tref var = tau::get(tau::variable,
					tau::get(tau::io_var, { var_name }));
			if (auto it = global_scope.find(var);
				it != global_scope.end()) return;
			global_scope[var] = type;
		};
		for (auto [sid, x] : ctx.inputs)  add(sid, x.first);
		for (auto [sid, x] : ctx.outputs) add(sid, x.first);
		return global_scope;
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

	spec_context<node> ctx; // input and output definitions
	subtree_map<node, size_t> global_scope = {};
};

}

#endif //DEFINITIONS_H
