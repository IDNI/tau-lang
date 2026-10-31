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
 * @brief True when the definition heads @p a and @p b are one head up to the
 * names of their parameters: equal node for node, with a one-to-one renaming
 * between their variables, so `f(x)` matches `f(y)` and not `f(x, x)` `f(x, y)`.
 */
template <NodeType node>
bool same_definition_head(tref a, tref b) {
	using tau = tree<node>;
	std::map<tref, tref> forward, backward;
	std::function<bool(tref, tref)> same = [&](tref x, tref y) {
		const auto& nx = tau::get(x);
		const auto& ny = tau::get(y);
		if (!(nx.value == ny.value)) {
			// a variable's name is below it, so only its type can differ here
			if (!(nx.is(tau::variable) && ny.is(tau::variable))) return false;
		}
		if ((nx.is(tau::variable) || nx.is(tau::capture))
			&& !is_io_var<node>(x) && !is_io_var<node>(y))
		{
			if (nx.get_ba_type() != ny.get_ba_type()) return false;
			auto [f, fnew] = forward.try_emplace(x, y);
			auto [b, bnew] = backward.try_emplace(y, x);
			return f->second == y && b->second == x;
		}
		const trefs cx = nx.get_children(), cy = ny.get_children();
		if (cx.size() != cy.size()) return false;
		for (size_t i = 0; i < cx.size(); ++i)
			if (!same(cx[i], cy[i])) return false;
		return true;
	};
	return same(a, b);
}

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
	 * If a rule with the same head, up to the names of its parameters,
	 * already exists, it is updated in-place.
	 * Linear in the number of rules.
	 * @param head Rewrite rule head.
	 * @param body Rewrite rule body.
	 * @return Index of the (possibly updated) rule.
	 */
	size_t add(htref head, htref body) {
		// Check if rule is updated
		for (size_t i = 0; i < heads.size(); ++i) {
			if (same_definition_head<node>(heads[i]->get(), head->get())) {
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
	subtree_htref_map<node, size_t>& get_input_defs() {
		return ctx.inputs;
	}

	/** @brief Return a reference to the named output-stream definition map. */
	subtree_htref_map<node, size_t>& get_output_defs() {
		return ctx.outputs;
	}

	/**
	 * @brief Return a pointer to the global type scope, merging in I/O-context types.
	 *
	 * New bindings from the I/O context are inserted into `global_scope` on each call.
	 */
	subtree_map<node, size_t>* get_global_scope() {
		for (auto& [var, type] : ctx.types) {
			tref t = var->get();
			if (auto it = global_scope.find(t);
				it == global_scope.end())
					global_scope[t] = type;
		}
		return &global_scope;
	}

	/**
	 * @brief Replace the global type scope with @p global_scope.
	 * @param global_scope the new scope, taken by value; pass
	 * `std::move(...)` to avoid the copy.
	 */
	void set_global_scope(subtree_map<node, size_t> global_scope) {
		this->global_scope = std::move(global_scope);
	}

	/** @brief Capture of every mutable per-spec field (rule table, I/O
	 *  context, global type scope), for save()/restore() around an
	 *  isolated compile. */
	struct snapshot {
		std::vector<htref> heads;
		std::vector<htref> bodies;
		io_context<node> ctx;
		subtree_map<node, size_t> global_scope;
	};

	/** @brief A copy of the current rule table, I/O context and type scope. */
	snapshot save() const {
		return { heads, bodies, ctx, global_scope };
	}

	/**
	 * @brief Replace the rule table, I/O context and type scope with @p s.
	 * @param s a snapshot taken by save(); it is moved from.
	 */
	void restore(snapshot s) {
		heads = std::move(s.heads);
		bodies = std::move(s.bodies);
		ctx = std::move(s.ctx);
		global_scope = std::move(s.global_scope);
	}

	/** @brief Return a reference to the per-node type map in the I/O context. */
	subtree_htref_map<node, size_t>& get_types() {
		return ctx.types;
	}

	/** @brief Return the number of registered rules. */
	size_t size() const {
		DBG(assert(heads.size() == bodies.size());)
		return heads.size();
	}

	/** @brief Return the last registered rule as a head–body pair; requires size() > 0. */
	rewriter::rule back() const {
		return std::make_pair(heads.back(), bodies.back());
	}

	/** @brief Return rule @p i as a head–body pair; @p i must be below size(). */
	rewriter::rule operator[](const size_t i) const {
		return std::make_pair(heads[i], bodies[i]);
	}

	/** @brief Return the process-global singleton `definitions` instance.
	 *  NOTE: NOT thread-safe. The tau library assumes single-threaded access.
	 *  Do not access this singleton from multiple threads concurrently. */
	static definitions& instance() {
		static definitions d;
		// No gc callback may clear this registry: collect_live_refs
		// pins every global_scope key, and ctx.{types,inputs,outputs}
		// are htref-keyed maps whose nodes survive gc by construction.
		return d;
	}

	/** @brief Clear all rules, I/O context, and type scope. */
	void clear() {
		heads.clear();
		bodies.clear();
		ctx.clear();
		global_scope.clear();
	}

	/**
	 * @brief Insert every raw tref held by this registry into @p keep.
	 *
	 * `heads`/`bodies` are htrefs and `ctx.{types,inputs,outputs}` are
	 * htref-keyed (auto-tracked via the non-expired weak_ptr entries in
	 * `M`), so only the tref-keyed `global_scope` needs walking.
	 * @param keep Set of tree nodes to preserve across gc.
	 */
	void collect_live_refs(std::unordered_set<tref>& keep) const {
		for (auto& [k, _] : global_scope) keep.insert(k);
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
