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


#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "resolver"

namespace idni::tau_lang {

using type_t = std::pair<size_t, tref>; // (type_id, subtype)

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
std::optional<type_t> merge_ba_types(const type_t& t1, const type_t& t2) {
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
struct type_scoped_resolver : public scoped_resolver<tref, type_t> {
	using tau = tree<node>;
	using tt = tau::traverser;

	type_scoped_resolver():
		scoped_resolver<tref, type_t>(nullptr, {0, nullptr}) {}

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

	bool merge(const trefs& ts) {
		if (ts.size() < 2) return true;
		auto it = ts.begin();
		tref first = *it;
		++it;
		for(; it != ts.end(); ++it)
			if (!merge(first, *it)) return false;
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

	// Untyped type
	static type_t untyped = std::make_pair(ba_types<node>::id("untyped"), nullptr);

	// The following variables conform the state of the traversal
	// transformed: map from original tref to transformed tref
	// resolver: the type_scoped_resolver used to resolve types and scopes
	// error: true if an error happened during traversal
	type_scoped_resolver<node> resolver;
	std::map<tref, tref> transformed;
	bool error = false;

	// Extract the type and subtype of a variable or constant node vector
	auto get_type = [&](trefs ts) -> std::optional<type_t> {
		using tau = tree<node>;
		using tt = tau::traverser;

		// If trefs is empty we return the default (untyped)
		if (ts.empty())	return std::optional<type_t>(untyped);
		std::optional<type_t> result = untyped;
		for (const auto& t : ts) {
			auto t_type = std::make_pair(
				tt(t) | tau::type | tt::ba_type,
				tt(t) | tau::subtype | tt::ref);
			result = merge_ba_types<node>(result.value(), t_type);
			// If types are conflicting, return nullopt
			if (!result) return std::nullopt;
		}
		return result;
	};

	// Typeable trefs predicate
	auto is_typeable = [](tref t) -> bool {
		return is<node, tau::variable>(t)
			|| is<node, tau::bf_constant>(t)
			|| is<node, tau::bv_constant>(t)
			|| is<node, tau::bf_t>(t)
			|| is<node, tau::bf_f>(t);
	};

	/*auto is_mergeable = [](tref t) -> bool {
		return is<node, tau::variable>(t)
			|| is<node, tau::bf_constant>(t)
			|| is<node, tau::bf_t>(t)
			|| is<node, tau::bf_f>(t);
	};*/

	// We gather info about types and scopes while entering nodes
	auto on_enter = [&] (tref n, [[maybe_unused]] tref parent) -> bool {
		DBG(assert(n != nullptr);)
		// Stop traversal on error
		if (error) return false;
		// Get the node type
		const auto& t = tau::get(n);
		size_t nt = t.get_type();
		// Depoending on the node type...
		switch (nt) {
			case tau::wff_all: case tau::wff_ex: case tau::bf_fall: case tau::bf_fex: {
				// We open a new scope, we get all the quantified variables,
				// add them to the scope (with the given type if any).

				// We assume all scoped variables are resolved when closing the scope,
				// so if the current node is already transformed, we use the transformed
				// node and do not further continue visiting child nodes.
				if (transformed.find(n) != transformed.end())
					return false;
				// We collect all the variables/type pairs in a map. Note that
				// repeated variables will be overwritten, keeping the inner one.
				std::map<tref, type_t> var_list;
				auto qvars = tau::get(n)[0].select_top(is<node, tau::q_var>);
				for (const auto& v : qvars) {
					auto var = tt(v) | tau::variable | tt::ref;
					if (auto type = tt(var) | tau::type | tt::ba_type; type)
						var_list[var] = std::make_pair(type, tt(var) | tau::subtype | tt::ref);
					else var_list[var] = untyped;
				}
				resolver.open(var_list);
				// We continue the traversal of children
				break;
			}
			case tau::bv_eq: case tau::bv_neq: case tau::bv_lteq: case tau::bv_nlteq:
			case tau::bv_gt: case tau::bv_ngt: case tau::bv_gteq: case tau::bv_ngteq:
			case tau::bv_lt: case tau::bv_nlt: {
				// We get all the (top) typeables in the expression and assign
				// then the type bv (and the subtype when available). We do the
				// same for the constants.

				// We get all the typeable top nodes in the expression, which in
				// this case are only (bitvector) variables and constants.
				auto typeables = tau::get(n).select_top(is_typeable);
				// We infer the common type of all the typeables in the expression
				auto type = get_type(typeables);
				// If no common type is found, we set error and stop traversal
				if (!type) return error = true, false;
				// We add the variables and constants to the current scope and
				// assign them the common type. We also collect them in a vector
				// to be merged together later.
				trefs mergeables;
				for (const auto& t : typeables)	{
					resolver.insert(t);
					resolver.assign(t, type.value());
					mergeables.push_back(t);
				}
				// We merge all the variables and constants in union-find data
				// structure. If there is a conflict, we set error and stop the
				// traversal.
				if (!resolver.merge(typeables)) error = true;
				// We stop the traversal of children as we have already
				// processed all the typeables in the expression.
				return false;
			}
			case tau::bf_eq: case tau::bf_neq: case tau::bf_lteq: case tau::bf_nlteq:
			case tau::bf_gt: case tau::bf_ngt: case tau::bf_gteq: case tau::bf_ngteq:
			case tau::bf_lt: case tau::bf_nlt: {
				// We get all the (top) typeables in the expression and assign
				// then the type bv (and the subtype when available). We do the
				// same for the constants.

				// We get all the typeable top nodes in the expression, which in
				// this case are only (sbf/tau) variables and constants.
				auto typeables = tau::get(n).select_top(is_typeable);
				// We infer the common type of all the typeables in the expression
				auto type = get_type(typeables);
				// If no common type is found, we set error and stop traversal
				if (!type) return error = true, false;
				// We add the variables to the current scope and assign them the
				// common type. We also collect them in a vector to be merged
				// together later.
				// We also collect the constants in a separate vector to be
				// added to a new inner scope.
				trefs mergeables, constants;
				for (const auto& t : typeables)	{
					if (is<node, tau::variable>(t)) {
						resolver.insert(t);
						resolver.assign(t, type.value());
						mergeables.push_back(t);
					} else if (is<node, tau::bf_constant>(t)) {
						constants.push_back(t);
						mergeables.push_back(t);
					}
				}
				// We open a new scope and add the constants to it with
				// the inferred type. Constants must be resolved in the
				// inner scope to avoid conflicts with other constants
				// in outer scopes with the same source but different
				// types (e.g., 0 as sbf and 0 as tau).
				resolver.open({});
				for (const auto& c : constants) {
					resolver.insert(c);
					resolver.assign(c, type.value());
				}
				// We merge all the variables and constants in union-find data
				// structure. If there is a conflict, we set error and stop the
				// traversal.
				if (!resolver.merge(mergeables)) error = true;
				// We stop the traversal of children as we have already
				// processed all the typeables in the expression.
				return false;
			}
			default:
				// Otherwise, we continue the traversal of children
				// without doing anything special in this node.
				break;
		}
		// Stop traversal on error
		return !error;
	};

	// Do nothing between nodes
	auto on_between = [&] ([[maybe_unused]] tref n, [[maybe_unused]]tref parent) -> bool {
		// Stop traversal on error
		return !error;
	};

	// We update types (changing nodes while leaving them) while closing scopes
	auto on_leave = [&] (tref n, tref parent) -> void {
		// We use transformed map to update children if they were any changes
		// and add the current node if resulted changed.
		DBG(assert(n != nullptr);)
		// Stop traversal on error
		if (error) return;
		// Get the node type
		const auto& t = tau::get(n);
		size_t nt = t.get_type();
		// Depoending on the node type...
		switch (nt) {
			case tau::wff_all: case tau::wff_ex: case tau::bf_fall: case tau::bf_fex: {
				// Replace occurrences of untyped variables with the inferred
				// ones (if any). We resolve to tau all untyped variables.
				// We also update the current node with the transformed
				// children if any.
				// Finally, we close the scope.
				std::map<tref, tref> changes;
				for(auto [var, type] : resolver.current_kinds()) {
					auto inferred = type;
					if (type.first == untyped.first ) {
						inferred = { ba_types<node>::id("tau"), nullptr };
						if (resolver.assign(var, { ba_types<node>::id("tau"), nullptr })) {
							// We have a type mismatch
							error = true;
							return;
						}
					}
					changes[var] = tau::build_variable(
						tt(var) | tt::string,
						inferred.first,
						inferred.second);
				}
				auto new_n = rewriter::replace<node>(n, changes);
				if (new_n != n) transformed[n] = new_n;
				resolver.close();
				return;
			}
			case tau::bf_eq: case tau::bf_neq: case tau::bf_lteq: case tau::bf_nlteq:
			case tau::bf_gt: case tau::bf_ngt: case tau::bf_gteq: case tau::bf_ngteq:
			case tau::bf_lt: case tau::bf_nlt: {
				// We update the type of the constants in the expression
				// according to the inferred types in the scope. If the type
				// is unknown, we assume is tau.
				// We also update the current node with the transformed
				// children if any.
				// If a constant is untyped, we resolve it to tau.
				// We also transform the constants to proper constant nodes
				std::map<tref, tref> changes;
				for(auto [cte, type] : resolver.current_kinds()) {
					size_t inferred = type.first == untyped.first
						? ba_types<node>::id("tau")
						: type.first;
					tref bounded = tau::get_ba_constant_from_source(
						tau::get(cte).data(),
						inferred
					);
					if (bounded == nullptr) {
						LOG_ERROR << "Failed to bind default type: "
								<< LOG_BA_TYPE(inferred)
								<< " to " << LOG_FM(cte);
						error = true;
					}
					changes[cte] = bounded;
				}
				auto new_n = rewriter::replace<node>(n, changes);
				if (new_n != n) transformed[n] = new_n;
				resolver.close();
				return;
			}
			default: {
				// For other intermediate nodes, we only need to update the
				// current node with the transformed children if any.
				// Note that we do not need to close any scope here.
				if (parent) {
					auto new_n = rewriter::replace<node>(n, transformed);
					if (new_n != n) transformed[n] = new_n;
					return;
				}
				// For the root node, we type untyped variables with tau.
				std::map<tref, tref> changes;
				for(auto [var, type] : resolver.current_kinds()) {
					if (type.first == untyped.first) {
						if (resolver.assign(var, { ba_types<node>::id("tau"), nullptr })) {
							// We have a type mismatch
							error = true;
							return;
						}
						changes[var] = tau::build_variable(
							tt(var) | tt::string,
							ba_types<node>::id("tau"),
							nullptr);
					}
				}
				auto new_n = rewriter::replace<node>(n, changes);
				if (new_n != n) transformed[n] = new_n;
				return;
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