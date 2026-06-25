// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file definitions.h
 * @brief Singleton registry of function/predicate definitions and I/O stream definitions.
 *
 * `definitions<node>` manages rewrite rules (head–body pairs), the I/O context
 * (named input/output stream bindings), and the global type scope used during
 * type inference.
 */

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "io_context.h"

namespace idni::tau_lang {

/**
 * @brief Registry of function/predicate definitions and I/O stream bindings.
 *
 * Singleton (access via `instance()`). Stores ordered head–body rule pairs,
 * delegates I/O stream definitions to an embedded `io_context<node>`, and
 * maintains a global type scope.
 * @tparam node Tree node type satisfying `NodeType`.
 */
template<NodeType node>
struct definitions {
	using tau = tree<node>;

	/**
	 * @brief Add or update the rule @p head → @p body.
	 *
	 * If a rule with the same head already exists, it is updated in-place.
	 * @param head Rewrite rule head.
	 * @param body Rewrite rule body.
	 * @return Index of the (possibly updated) rule.
	 */
	size_t add(htref head, htref body) {
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

	/** @brief Return a pointer to the ordered list of rule heads. */
	const std::vector<htref>* get_definition_heads() const {
		return &heads;
	}

	/** @brief Build and return the full list of head–body rewriter rules. */
	rewriter::rules get_sym_defs() const {
		rewriter::rules r;
		for (size_t i = 0; i < heads.size(); ++i)
			r.emplace_back(heads[i], bodies[i]);
		return r;
	}

	/** @brief Return a pointer to the embedded I/O context. */
	io_context<node>* get_io_context() {
		return &ctx;
	}

	/** @brief Return a reference to the named input-stream definition map. */
	subtree_map<node, size_t>& get_input_defs() {
		return ctx.inputs;
	}

	/** @brief Return a reference to the named output-stream definition map. */
	subtree_map<node, size_t>& get_output_defs() {
		return ctx.outputs;
	}

	/**
	 * @brief Return a pointer to the global type scope, merging in I/O-context types.
	 *
	 * New bindings from the I/O context are inserted into `global_scope` on each call.
	 */
	subtree_map<node, size_t>* get_global_scope() {
		for (auto [var, type] : ctx.types) {
			if (auto it = global_scope.find(var);
				it == global_scope.end())
					global_scope[var] = type;
		}
		return &global_scope;
	}

	/** @brief Replace the global type scope with @p global_scope. */
	void set_global_scope(subtree_map<node, size_t>& global_scope) {
		this->global_scope = std::move(global_scope);
	}

	/** @brief Return a reference to the per-node type map in the I/O context. */
	subtree_map<node, size_t>& get_types() {
		return ctx.types;
	}

	/** @brief Return the number of registered rules. */
	size_t size() const {
		DBG(assert(heads.size() == bodies.size());)
		return heads.size();
	}

	/** @brief Return the last registered rule as a head–body pair. */
	rewriter::rule back() const {
		return std::make_pair(heads.back(), bodies.back());
	}

	/** @brief Return rule @p i as a head–body pair. */
	rewriter::rule operator[](const size_t i) const {
		return std::make_pair(heads[i], bodies[i]);
	}

	/** @brief Return the process-global singleton `definitions` instance. */
	static definitions& instance() {
		static definitions d;
		return d;
	}

	/** @brief Clear all rules, I/O context, and type scope. */
	void clear() {
		heads.clear();
		bodies.clear();
		ctx.clear();
		global_scope.clear();
	}
private:
	definitions() = default;

	std::vector<htref> heads = {};
	std::vector<htref> bodies = {};

	io_context<node> ctx; // input and output definitions
	subtree_map<node, size_t> global_scope;
};

}

#endif //DEFINITIONS_H
