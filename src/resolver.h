// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__RESOLVER_H__
#define __IDNI__TAU__RESOLVER_H__

#include <map>
#include <memory>
#include <vector>
#include <ranges>
#include <limits.h>

#include "tau_tree.h"
#include "union_find.h"

namespace idni::tau_lang {

using type = std::pair<size_t, tref>; // (type_id, subtype)

template<typename data_t, typename kind_t>
struct scoped_resolver {
	using scope_t = size_t;
	using element_t = std::pair<scope_t, data_t>;
	using kinds_t = std::map<element_t, kind_t>;
	using union_find_t = union_find<element_t, std::less<element_t>>;

	union_find_t uf;
	scope_t current = 0;
	std::deque<size_t> scopes_ { current };
	std::map<element_t, kind_t> kinds_;
	data_t minimum;
	kind_t unknown;
	kind_t default_kind = unknown;

	scoped_resolver(const data_t& minimum, const kind_t& unknown):
		minimum(minimum), unknown(unknown) {}

	void open(const std::map<data_t, kind_t>& kinds) {
		scopes_.push_back(++current);
		for (const auto& [data, kind] : kinds) {
			uf.insert({current, data});
			kinds_.emplace(element_t{current, data}, kind);
		}
	}

	void close() {
		if (scopes_.size() == 1) return;
		kinds_.erase(
			kinds_.lower_bound({current, minimum}),
			kinds_.end());
		scopes_.pop_back();
		current = scopes_.back();
	}

	void insert(const data_t& data) {
		static size_t global = 0;
		for(auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
			if (uf.contains(element_t{*it, data})) return;
		uf.insert({global, data});
		kinds_.emplace(element_t{global, data}, unknown);
	}

	kind_t type_of(const data_t& data) {
		for(auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
			if (uf.contains({*it, data}))
				return kinds_.find(uf.root(element_t{*it, data}))->second;
		return unknown;
	}

	bool same_kind(const data_t& data1, const data_t& data2) {
		return type_of(data1) == type_of(data2);
	}

	bool assign(const data_t& data, const kind_t& kind) {
		for(auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
			if (uf.contains({*it, data})) {
				if (auto current = kinds_.find(uf.root(element_t{*it, data}))->second;
						current != unknown && current != kind)
					return false;
				kinds_[uf.root(element_t{*it, data})] = kind;
				return true;
			}
		return false;
	}

	bool assign(const data_t& data) {
		return assign(data, default_kind);
	}

	std::map<data_t, kind_t> kinds() {
		std::map<data_t, kind_t> result ;
		// Note that we will overwrite the kind of a data if it appears in
		// multiple scopes, keeping only the inner one.
		for(auto it = uf.begin(); it != uf.end(); ++it)
			if (kinds_.find(it->second) != kinds_.end())
				result[it->second.second] = type_of(it->second.second);
		return result;
	}

	std::map<data_t, kind_t> current_kinds() {
		std::map<data_t, kind_t> result ;
		for(auto it = uf.lower_bound(element_t{current, minimum}); it != uf.end(); ++it)
			if (kinds_.find(it->second) != kinds_.end())
				result[it->second.second] = type_of(it->second.second);
		return result;
	}
};

template<NodeType node>
std::optional<type> merge_ba_types(const type& t1, const type& t2) {
	static auto untyped = ba_types<node>::id("untyped");
	if (t1.first == untyped) {
		if (t2.first != untyped) {
			return { t2 };
		} else {
			return { std::make_pair(0, nullptr) }; // both untyped
		}
	}
	if (t2.first == untyped) return { t1 };
	if (t1.first == t2.first) {
		if (t1.second == t2.second) return { t1 };
		if (t1.second == nullptr) return { std::make_pair(t1.first, t2.second) };
		if (t2.second == nullptr) return { std::make_pair(t1.first, t1.second) };
		return std::nullopt; // conflicting subtype info
	}
	return std::nullopt;
}

template<NodeType node>
struct type_scoped_resolver : public scoped_resolver<tref, type> {
	using tau = tree<node>;
	using tt = tau::traverser;

	type_scoped_resolver():
		scoped_resolver<tref, type>(nullptr, {0, nullptr}) {}

	// merge two trefs if the types are compatible
	// returns true if merge was successful, false otherwise
	bool merge(tref a, tref b) {
		auto type_a = this->type_of(a);
		auto type_b = this->type_of(b);
		auto merged = merge_ba_types<node>(type_a, type_b);
		if (!merged) return false; // conflicting type info
		this->uf.merge({this->current, a}, {this->current, b});
		this->kinds_[this->uf.root({this->current, a})] = merged.value();
		return true;
	}
};

// Infers the types of variables and constants in the tree n. It assumes that
// the types of the scoped variables are known when closing the scope.
// If a variable or constant cannot be assigned a type, it is assigned to tau.
// If conflicting type information is found, the function returns nullptr.
template <NodeType node>
tref new_infer_ba_types(tref n) {
	using tau = tree<node>;
	using tt = tau::traverser;

	// The following variables conform the state of the traversal
	// transformed: map from original tref to transformed tref
	// resolver: the type_scoped_resolver used to resolve types and scopes
	// error: true if an error happened during traversal
	type_scoped_resolver<node> resolver;
	std::map<tref, tref> transformed;
	bool error = false;

	// Extract the type and subtype of a variable or constant node vector
	auto get_type = [&](trefs ts) -> std::optional<type> {
		using tau = tree<node>;
		using tt = tau::traverser;

		if (ts.empty()) return {resolver.unknown, nullptr}; // no type, no subtype info
		std::optional<type> result = { std::make_pair(0, nullptr) }; // (type_id, subtype)
		for (const auto& t : ts) {
			auto t_type = make_pair<type, type>(
				tau::tt(t) | tau::type | tt::ba_type,
				tau::tt(t) | tau::subtype | tt::ref);
			result = merge_ba_types(result.value(), t_type);
			if (!result) return std::nullopt; // conflicting type info
		}
		return result;
	};

	auto is_typeable = [](tref t) -> bool {
		return is<node, tau::variable>(t)
			|| is<node, tau::bf_constant>(t)
			|| is<node, tau::bv_constant>(t)
			|| is<node, tau::bf_t>(t)
			|| is<node, tau::bf_f>(t);
	};

	auto is_mergeable = [](tref t) -> bool {
		return is<node, tau::variable>(t)
			|| is<node, tau::bf_constant>(t)
			|| is<node, tau::bf_t>(t)
			|| is<node, tau::bf_f>(t);
	};

	// We gather info about types and scopes while entering nodes
	auto on_enter = [&] (tref n, [[maybe_unused]] tref parent) -> bool {
		// When traversing a bv we set the default type to bv.
		// When traversing a bf we set the default type to unknown as it can be
		// either tau or sbf.
		// When entering a quantifier we open a new scope and add its variables
		// to the scope, assigning them the type if known, the default type
		// otherwise.
		// We entering a variable, we should take into account that if the
		// parent is a qvar, we add it to the current scope, otherwise to the
		// default one. The same if it is a quantifier (check which one is
		// the correct one in the code)
		// When entering an atomic formula, we update the default kind
		// according to the formula type (bv or bf).
		DBG(assert(n != nullptr);)
		const auto& t = tau::get(n);
		size_t nt = t.get_type();
		switch (nt) {
			case tau::wff_all: case tau::wff_ex: case tau::bf_fall: case tau::bf_exists: {
				// Open a new scope. Add its variables to the scope, assigning
				// them the type and subtype if known.
				// We assume all scoped variables are resolved when closing the scope,
				// so if the current node is already transformed, we use the transformed
				// node and do not further continue visiting child nodes.
				if (transformed.find(n) != transformed.end())
					return false; // Skip children
				resolver.open();
				std::map<tref, type> vars;
				auto qvars = tau::get(n)[0].select_top(is<node, tau::q_var>);
				for (const auto& v : qvars) {
					auto var = tt(v) | tau::variable | tt::ref;
					auto var_type = make_pair<type, type>(
						tt(var) | tau::type | tt::ba_type,
						tt(var) | tau::subtype | tt::ref);
					resolver.insert(var); assign(var, var_type);
				}
				break;
			}
			case tau::bv_eq: case tau::bv_neq: case tau::bv_lteq: case tau::bv_nlteq:
			case tau::bv_gt: case tau::bv_ngt: case tau::bv_gteq: case tau::bv_ngteq:
			case tau::bv_lt: case tau::bv_nlt: {
				// We get all the top variables in the expression
				// assign then a type of bv, we subtype the ones
				// with subtype info and added to resolver
				// accordingly (inserting and merging).
				// We also consider all the constants in the expression.
				// The bf_t's and the bf_f's are considered only to hint
				// the type of the variables, but they are not added to
				// the resolver as they would be untypped.
				auto typeables = tau::get(n).select_top(is_typeable);
				auto type = get_type(typeables);
				if (!type) return error = true, false; // Error in type checking
				trefs mergeables;
				for (const auto& t : typeables)	{
					// We ignore the bv constants for a while. They need to be
					// handled at the another method after the whole formula is
					// processed.
					if (is<node, tau::variable>(t)) {
						resolver.insert(t);
						resolver.assign(t, type);
						mergeables.push_back(t);
					}
				}
				if (!resolver.merge(typeables)) error = true; // Error in type checking
				// Replace the typed variables in the expression and assign
				// it to n in transformed.
				return false; // Skip children
			}
			case tau::bf_eq: case tau::bf_neq: case tau::bf_lteq: case tau::bf_nlteq:
			case tau::bf_gt: case tau::bf_ngt: case tau::bf_gteq: case tau::bf_ngteq:
			case tau::bf_lt: case tau::bf_nlt: {
				// We get all the top variables in the expression
				// assign then the type and subtype if present and
				// added to resolver accordingly (inserting and merging).
				// We also consider all the constants in the expression.
				auto typeables = tau::get(n).select_top(is_typeable);
				auto type = get_type(typeables);
				if (!type) return error = true, false; // Error in type checking
				trefs mergeables, constants;
				for (const auto& t : typeables)	{
					// We ignore the bf_t's and the bf_f's as they must be
					// untyped.
					if (is<node, tau::variable>(t)) {
						resolver.insert(t);
						resolver.assign(t, type);
						mergeables.push_back(t);
					} else if (is<node, tau::bf_constant>(t)) {
						constants.push_back(t);
						mergeables.push_back(t);
					}
				}
				// Open a new scope for the constants and add them to it
				resolver.open();
				for (const auto& c : constants) {
					resolver.insert(c);
					resolver.assign(c, type);
				}
				// We merge all the info (variables and constants)
				if (!resolver.merge(mergeables)) error = true; // Error in type checking
				return false; // Skip children
		}
			default:
				break;
		}
		return !error; // Continue child traversal?
	};

	// Do nothing between nodes
	auto on_between = [&] ([[maybe_unused]] tref n, [[maybe_unused]]tref parent) -> bool {
		return !error; // Continue traversal?
	};

	// We update types (changing nodes while leaving them) while closing scopes
	auto on_leave = [&] (tref n, tref parent) -> void {
		// We use transformed map to update children if they were changed
		// and update it with the current node if it was changed.
		DBG(assert(n != nullptr);)
		const auto& t = tau::get(n);
		size_t nt = t.get_type();
		switch (nt) {
			case tau::wff_all: case tau::wff_ex: case tau::bf_fall: case tau::bf_exists: {
				// Close the current scope and replace occurrences of
				// untyped variables with the inferred ones (if any).
				// We resolve to tau all untyped variables.
				// We also update the current node with the transformed
				// children if any.
				std::map<tref, tref> changes;
				for(auto [var, type] : resolver.current_kinds()) {
					auto inferred = type;
					if (type.first == resolver.unknown ) {
						inferred = { ba_types<node>::id("tau"), nullptr };
						resolver.assign(var, { ba_types<node>::id("tau"), nullptr });
					}
					changes[var] = tau::build_variable(
						tt(var) | tt::string,
						inferred.first,
						inferred.second);
				}
				auto new_n = rewriter::replace<node>(n, changes);
				if (new_n != n) transformed[n] = new_n;
				resolver.close();
				break;
			}
			case tau::bf_eq: case tau::bf_neq: case tau::bf_lteq: case tau::bf_nlteq:
			case tau::bf_gt: case tau::bf_ngt: case tau::bf_gteq: case tau::bf_ngteq:
			case tau::bf_lt: case tau::bf_nlt: {
				// We update the type of the constants in the expression
				// according to the inferred types in the scope. If the type
				// is unknown, we assume is tau.
				// We also update the current node with the transformed
				// children if any.
				std::map<tref, tref> changes;
				for(auto [cte, type] : resolver.current_kinds()) {
					auto new_type = { type.first == resolver.unknown
							? ba_types<node>::id("tau")
							: type.first,
						type.second };
					// We replace all untyped vconstants with the inferred type.
					changes[cte] = tau::build_ba_constant(
					tt(cte) | tt::ba_constant,
					new_type);
				}
				auto new_n = rewriter::replace<node>(n, changes);
				if (new_n != n) transformed[n] = new_n;
				resolver.close();
				break;
			}
			default: {
				// If we are at the root, replace global untyped variables
				// with the inferred ones (if any).
				// In any case, we update the current node with the transformed
				// children if any.
				if (parent) return;
				std::map<tref, tref> changes;
				for(auto [var, type] : resolver.current_kinds()) {
					auto new_type = { type.first == resolver.unknown
							? ba_types<node>::id("tau")
							: type.first,
						type.second };
					// We replace all untyped variables with the inferred type.
					changes[var] = tau::build_ba_variable(
						tt(var) | tt::string,
						new_type);
				}
				auto new_n = rewriter::replace<node>(n, changes);
				if (new_n != n) transformed[n] = new_n;
				break;
			}
		}
	};

	// All nodes
	auto visit_all = [](tref) { return true; };
	// We visit the tree and return the transformed root if no error happened.
	// If an error happened we return nullptr.
	pre_order<node>(tau::get(n).get()).visit(on_enter, visit_all, on_leave, on_between);
	if (error) return nullptr;
	// We add to the transformed map the untypping of the bf_t's and the bf_f's.
	// ...some code here...
	return transformed.contains(n) ? transformed[n] : n;
}

} // namespace idni::tau_lang
#endif // __IDNI__TAU__RESOLVER_H__