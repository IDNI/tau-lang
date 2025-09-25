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

template<typename data_t, typename kind_t, class less_t = std::less<data_t>>
struct scoped_resolver {
	using scope_t = size_t;

	struct scoped_less {
		bool operator()(const std::pair<scope_t, data_t>& a,
			const std::pair<scope_t, data_t>& b) const {
				static const less_t comp;
				if (a.first < b.first) return true;
				if (a.first > b.first) return false;
				// same scope
				return (comp(a.second, b.second));
		}
	};

	using element_t = std::pair<scope_t, data_t>;
	using kinds_t = std::map<element_t, kind_t, scoped_less>;
	using union_find_by_rank_t = union_find_by_rank<element_t, scoped_less>;

	union_find_by_rank_t uf;
	scope_t current = 0;
	std::deque<size_t> scopes_ { current };
	kinds_t kinds_;
	kind_t unknown;
	kind_t default_kind = unknown;

	scoped_resolver(const kind_t& unknown): unknown(unknown) {}

	void open(const std::map<data_t, kind_t, less_t>& kinds) {
		current++;
		scopes_.push_back(current);
		for (const auto& [data, kind] : kinds) {
			uf.insert({current, data});
			kinds_.emplace(element_t{current, data}, kind);
		}
	}

	void close() {
		if (scopes_.size() == 1) return;
		std::erase_if(kinds_, [&](const auto& e)
			{ return e.first.first == current; });
		scopes_.pop_back();
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

	scope_t scope_of(const data_t& data) {
		for(auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
			if (uf.contains({*it, data}))
				return (*it);
		return 0; // global scope
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

	std::map<data_t, kind_t, less_t> kinds() {
		std::map<data_t, kind_t, less_t> result ;
		// Note that we will overwrite the kind of a data if it appears in
		// multiple scopes, keeping only the inner one.
		for(auto it = uf.begin(); it != uf.end(); ++it)
			if (kinds_.find(it->second) != kinds_.end())
				result[it->second.second] = type_of(it->second.second);
		return result;
	}

	std::map<data_t, kind_t, less_t> current_kinds() {
		std::map<data_t, kind_t, less_t> result ;
		// TODO (HIGH) traverse in reverse until an element of other scope is found
		for(auto [element, _] : uf)
			if (element.first == current) result[element.second] = type_of(element.second);
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
struct type_scoped_resolver : public scoped_resolver<tref, type_t, idni::subtree_less<node>> {
	using tau = tree<node>;
	using tt = tau::traverser;

	type_scoped_resolver():
		scoped_resolver<tref, type_t, idni::subtree_less<node>>({0, nullptr}) {}

	// merge two trefs if the types are compatible
	// returns true if merge was successful, false otherwise
	bool merge(tref a, tref b) {
		auto type_a = this->type_of(a); auto scope_a = this->scope_of(a);
		auto type_b = this->type_of(b); auto scope_b = this->scope_of(b);
		auto merged = merge_ba_types<node>(type_a, type_b);
		if (!merged) return false; // conflicting type info
		this->uf.merge({scope_a, a}, {scope_b, b});
		this->kinds_[this->uf.root({scope_a, a})] = merged.value();
		// We also update the type of the merged elements
		//this->kinds_[{this->current, a}] = merged.value();
		//this->kinds_[{this->current, b}] = merged.value();
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
// If a variable or constant remains unassigned, it is assigned to tau.
// We assume that the types of the constants could also be propagated across
// scopes (in the future we will restrict it to equations)
// If conflicting type information is found, the function returns nullptr.
template <NodeType node>
tref new_infer_ba_types(tref n) {
	using tau = tree<node>;
	//using tt = tau::traverser;

	// Some type definitions
	static type_t untyped = { ba_types<node>::id("untyped"), nullptr };
	static type_t bv_type = { ba_types<node>::id("bv"), nullptr };
	static type_t tau_type = { ba_types<node>::id("tau"), nullptr };
	static type_t sbf_type = { ba_types<node>::id("sbf"), nullptr };

	// The following variables conform the state of the traversal
	// transformed: map from original tref to transformed tref
	// resolver: the type_scoped_resolver used to resolve types and scopes
	// error: true if an error happened during traversal
	type_scoped_resolver<node> resolver;
	subtree_map<node, tref> transformed;
	bool error = false;

	DBG(LOG_TRACE << "new_infer_ba_types/n:\n" << LOG_FM_TREE(n);)

	auto get_type_of = [](tref t) -> type_t {
		using tau = tree<node>;
		using tt = tau::traverser;

		DBG(LOG_TRACE << "new_infer_ba_types/get_type_of/t:\n" << LOG_FM_TREE(t);)

		size_t t_type;
		if (auto check = tt(t) | tau::type | tt::ref ; check) {
			t_type = ba_types<node>::id(tau::get(check).get_string());
		} else {
			t_type = tt(t) | tt::ba_type;
		}

		DBG(LOG_TRACE << "new_infer_ba_types/get_type_of/result/type:\n"
			<< t_type;)
		DBG(LOG_TRACE << "new_infer_ba_types/get_type_of/result/subtype:\n"
			<< (tt(t) | tau::subtype | tt::ref);)

		return { t_type, tt(t) | tau::subtype | tt::ref };
	};

	// Extract the type and subtype of a vector consisting of typeable trefs.
	// If the vector is empty, we return the default type (untyped). If there is
	// conflicting type information, we return nullopt.
	auto get_type = [&](trefs ts, type_t default_type) -> std::optional<type_t> {
		using tau = tree<node>;
		//using tt = tau::traverser;

		// If trefs is empty we return the default (untyped)
		if (ts.empty())	return std::optional<type_t>(default_type);
		std::optional<type_t> result = default_type;
		for (const auto& t : ts) {
			type_t current_type = get_type_of(t);
			type_t stored_type = resolver.type_of(t);
			auto type = merge_ba_types<node>(current_type, stored_type);
			if (!type) return std::nullopt; // conflicting type info

			DBG(LOG_TRACE << "new_infer_ba_types/get_type/t:\n" << LOG_FM_TREE(t);)

			result = merge_ba_types<node>(result.value(), type.value());
			// If types are conflicting, return nullopt
			if (!result) return std::nullopt;
		}

#ifdef DEBUG
		if (result)
			LOG_TRACE << "new_infer_ba_types/get_type/result: "
				<< ba_types<node>::name(result.value().first) << "["
				<< (result.value().second ? tau::get(result.value().second).dump_to_str() : "") << "]";
#endif // DEBUG

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

	auto untype = [](tref t) -> tref {
		using tau = tree<node>;

		DBG(LOG_TRACE << "new_infer_ba_types/untype/t:\n" << LOG_FM_TREE(t);)

		if (is<node, tau::bf_t>(t))
			return tau::get(tau::bf_t);
		if (is<node, tau::bf_f>(t))
			return tau::get(tau::bf_f);
		if (is<node, tau::bf_constant>(t))
			return (tau::get(t).children_size() > 0) ? tau::get(tau::bf_constant, {tau::get(t).first()}) : t;
		if (is<node, tau::bv_constant>(t))
			return tau::get(tau::bv_constant, {tau::get(t).first()});
		if (is<node, tau::variable>(t))
			return tau::get(tau::variable, {tau::get(t).first()});
		return t;
	};

	// We gather info about types and scopes while entering nodes
	auto on_enter = [&](tref n, tref) {
		DBG(assert(n != nullptr);)

		DBG(LOG_TRACE << "new_infer_ba_types/on_enter/n: " << LOG_FM(n);)

		// Stop traversal on error
		if (error) return false;
		// Get the node type
		auto t = tau::get(n);
		size_t nt = t.get_type();
		// Depoending on the node type...
		switch (nt) {
			case tau::rec_relation: {
				// We open a new scope for the relation variables and constants.
				// We assume all scoped variables and constants are resolved when
				// closing the scope. Otherwise, we type then to the tau type.

				// We open the new scope
				resolver.open({});
				break;
			}
			case tau::wff_all: case tau::wff_ex: case tau::bf_fall: case tau::bf_fex: {
				// We open a new scope, we get all the quantified variables,
				// add them to the scope (with the given type if any).

				// We collect all the variables/type pairs in a map. Note that
				// repeated variables will be overwritten, keeping the inner one.
				subtree_map<node, type_t> var_list;
				auto vars = tau::get(n)[0].select_top(is<node, tau::variable>);
				for (const auto& var : vars) {
					var_list[untype(var)] =  get_type_of(var);
				}
				resolver.open(var_list);
#ifdef DEBUG
				LOG_TRACE << "new_infer_ba_types/on_enter/resolver/uf:\n";
				for (const auto& [v, _] : resolver.uf) {
					LOG_TRACE << "  " << v.first << "," << LOG_FM_TREE(v.second) << ":"
						<< resolver.type_of(v.second).first
						<< "[" << (resolver.type_of(v.second).second ?
							tau::get(resolver.type_of(v.second).second).dump_to_str() : "") << "]";
				}
				LOG_TRACE << "new_infer_ba_types/on_enter/resolver/kinds_:\n";
				for (const auto& [v, t] : resolver.kinds_) {
					LOG_TRACE << "  " << v.first << LOG_FM_TREE(v.second) << ":"
						<< t.first
						<< "[" << (t.second ?
							tau::get(t.second).dump_to_str() : "") << "]";
				}
#endif // DEBUG

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
				// TODO (HIGH) make scoped_resolver accept a partial order of
				// types and change untyped to bv as default type here.
				auto type = get_type(typeables, untyped);
				// If no common type is found, we set error and stop traversal
				if (!type) return error = true, false;
				// We add the variables and constants to the current scope and
				// assign them the common type. We also collect them in a vector
				// to be merged together later.
				trefs mergeables;
				for (const auto& t : typeables)	{
					auto ut = untype(t);
					resolver.insert(ut);
					resolver.assign(ut, type.value());
					mergeables.push_back(ut);
				}
				// We merge all the variables and constants in union-find data
				// structure. If there is a conflict, we set error and stop the
				// traversal.
				if (!resolver.merge(typeables)) error = true;
				// Anyway, we stop the traversal of children as we have already
				// processed all the typeables in the expression.
#ifdef DEBUG
				LOG_TRACE << "new_infer_ba_types/on_enter/resolver/uf:\n";
				for (const auto& [v, _] : resolver.uf) {
					LOG_TRACE << "  " << v.first << "," << LOG_FM_TREE(v.second) << ":"
						<< resolver.type_of(v.second).first
						<< "[" << (resolver.type_of(v.second).second ?
							tau::get(resolver.type_of(v.second).second).dump_to_str() : "") << "]";
				}
				LOG_TRACE << "new_infer_ba_types/on_enter/resolver/kinds_:\n";
				for (const auto& [v, t] : resolver.kinds_) {
					LOG_TRACE << "  " << v.first << LOG_FM_TREE(v.second) << ":"
						<< t.first
						<< "[" << (t.second ?
							tau::get(t.second).dump_to_str() : "") << "]";
				}
#endif // DEBUG

				break;
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
				auto type = get_type(typeables, untyped);
				// If no common type is found, we set error and stop traversal
				if (!type) return error = true, false;
				// We add the variables and the constants to the current scope
				// and assign them the common type.
				trefs mergeables;
				for (const auto& t : typeables)	{
					auto ut = untype(t);
					resolver.insert(ut);
					resolver.assign(ut, type.value());
					mergeables.push_back(ut);
				}
				// We merge all the variables and constants in union-find data
				// structure. If there is a conflict, we set error and stop the
				// traversal.
				if (!resolver.merge(mergeables)) error = true;
				// Anyway, we stop the traversal of children as we have already
				// processed all the typeables in the expression.
				break;
			}
			case tau::bf: case tau::bv: {
				// As we are inside an equation already visited, we do not need
				// to continue the traversal of children.
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
	auto on_between = [&] (tref, tref) {
		// Stop traversal on error
		return !error;
	};

	// We update types (changing nodes while leaving them) while closing scopes
	auto on_leave = [&] (tref n, tref parent) {
		// We use transformed map to update children if they were any changes
		// and add the current node if resulted changed.
		DBG(assert(n != nullptr);)

		DBG(LOG_TRACE << "new_infer_ba_types/on_leave/n: " << LOG_FM(n);)

#ifdef DEBUG
		LOG_TRACE << "new_infer_ba_types/on_enter/resolver/uf:\n";
		for (const auto& [v, _] : resolver.uf) {
			LOG_TRACE << "  " << v.first << "," << LOG_FM_TREE(v.second) << ":"
				<< resolver.type_of(v.second).first
				<< "[" << (resolver.type_of(v.second).second ?
					tau::get(resolver.type_of(v.second).second).dump_to_str() : "") << "]";
		}
		LOG_TRACE << "new_infer_ba_types/on_enter/resolver/kinds_:\n";
		for (const auto& [v, t] : resolver.kinds_) {
			LOG_TRACE << "  " << v.first << LOG_FM_TREE(v.second) << ":"
				<< t.first
				<< "[" << (t.second ?
					tau::get(t.second).dump_to_str() : "") << "]";
		}
#endif // DEBUG


		// Stop traversal on error
		if (error) return;
		// Get the node type
		const auto& t = tau::get(n);
		size_t nt = t.get_type();
		// Depoending on the node type...
		switch (nt) {
			case tau::rec_relation: {
				// we type untyped variables with tau and close the scope.
				subtree_map<node, tref> changes;
				for(auto [e, _] : resolver.current_kinds()) {
					auto resolved_type = resolver.type_of(e); // already untyped
					auto e_type = tau::get(e).get_type();
					auto final_type = resolved_type == untyped ? tau_type : resolved_type;
					// assign
					if (tau::get(e).children_size()) {
						auto new_e = (final_type.second == nullptr)
						? tau::get_typed(e_type, tau::get(e).child(0), final_type.first)
						: tau::get_typed(e_type, tau::get(e).child(0), final_type.second,
							final_type.first);
						if (new_e != e) changes[e] = new_e;
					} else {
						auto new_e = (final_type.second == nullptr)
						? tau::get_typed(e_type, final_type.first)
						: tau::get_typed(e_type, final_type.second, final_type.first);
						if (new_e != e) changes[e] = new_e;
					}
				}
				auto new_n = rewriter::replace<node>(n, changes);
				DBG(LOG_TRACE << "new_infer_ba_types/on_leave/new_n: " << LOG_FM(new_n);)
				if (new_n != n) transformed[n] = new_n;
				resolver.close();
				return;
			}
			case tau::wff_all: case tau::wff_ex: /* case tau::bf_fall: case tau::bf_fex:*/ {
				// We get the quantified variables, its inferred type (defaulting
				// to type if they are still untyped) and close the scope.
				auto vars = tau::get(n)[0].select_top(is<node, tau::variable>);
				// We type untyped variables with tau and close the scope.
				subtree_map<node, tref> changes;
				for(auto v : vars) {
					DBG(LOG_TRACE << "new_infer_ba_types/on_leave/wff_all/v: " << LOG_FM_TREE(v);)
					auto resolved_type = resolver.type_of(untype(v)); // already untyped
					auto v_type = tau::get(v).get_type();
					auto final_type = resolved_type == untyped ? tau_type : resolved_type;
					// assign
					if (tau::get(v).children_size()) {
						auto new_v = (final_type.second == nullptr)
						? tau::get_typed(v_type, tau::get(v).child(0), final_type.first)
						: tau::get_typed(v_type, tau::get(v).child(0), final_type.second,
							final_type.first);
						if (new_v != v) changes[untype(v)] = new_v;
						DBG(LOG_TRACE << "new_infer_ba_types/on_leave/wff_all/new_v: " << LOG_FM_TREE(new_v);)
					} else {
						auto new_v = (final_type.second == nullptr)
						? tau::get_typed(v_type, final_type.first)
						: tau::get_typed(v_type, final_type.second, final_type.first);
						if (new_v != v) changes[untype(v)] = new_v;
						DBG(LOG_TRACE << "new_infer_ba_types/on_leave/wff_all/new_v: " << LOG_FM_TREE(new_v);)
					}
				}
				auto new_n = rewriter::replace<node>(n, changes);
				DBG(LOG_TRACE << "new_infer_ba_types/on_leave/new_n: " << LOG_FM_TREE(new_n);)
				if (new_n != n) transformed[n] = new_n;
				resolver.close();
				return;
			}
			case tau::bv_eq: case tau::bv_neq: case tau::bv_lteq: case tau::bv_nlteq:
			case tau::bv_gt: case tau::bv_ngt: case tau::bv_gteq: case tau::bv_ngteq:
			case tau::bv_lt: case tau::bv_nlt: {
				// We type all the constants
				auto constants = tau::get(n).select_top(is<node, tau::bv_constant>);
				subtree_map<node, tref> changes;
				for (const auto& c : constants) {
					auto c_type = resolver.type_of(c); // already untyped
					auto new_c = (c_type.second == nullptr)
						? tau::get_typed(tau::bv_constant, tau::get(c).child(0), bv_type.first)
						: tau::get_typed(tau::bv_constant, tau::get(c).child(0), c_type.second, bv_type.first);
					if (new_c != c) changes[c] = new_c;
				}
				auto new_n = rewriter::replace<node>(n, changes);
				DBG(LOG_TRACE << "new_infer_ba_types/on_leave/bv_*/new_n: " << LOG_FM_TREE(new_n);)
				if (new_n != n) transformed[n] = new_n;
				return;
			}
			case tau::bf_eq: case tau::bf_neq: case tau::bf_lteq: case tau::bf_nlteq:
			case tau::bf_gt: case tau::bf_ngt: case tau::bf_gteq: case tau::bf_ngteq:
			case tau::bf_lt: case tau::bf_nlt: {
				// We type all the constants
				/*auto constants = tau::get(n).select_top(is<node, tau::bf_constant>);
				subtree_map<node, tref> changes;
				for (const auto& c : constants) {
					auto c_type = resolver.type_of(c); // already untyped
					tref bound = tau::get_ba_constant_from_source(tau::get(c).data(), c_type.first);
					if (bound == nullptr) {
						error = true;
						return;
					}
					changes[c] = bound;
				}
				auto new_n = rewriter::replace<node>(n, changes);
				DBG(LOG_TRACE << "new_infer_ba_types/on_leave/new_n: " << LOG_FM(new_n);)
				if (new_n != n) transformed[n] = new_n;*/
				return;
			}
			default: {
				// For other intermediate nodes, we only need to update the
				// current node with the transformed children if any.
				// Note that we do not need to close any scope here.
				auto new_n = rewriter::replace<node>(n, transformed);
				DBG(LOG_TRACE << "new_infer_ba_types/on_leave/default/new_n: " << LOG_FM(new_n);)
				// For the root node, we type untyped variables with tau.
				if (!parent) {
					subtree_map<node, tref> changes;
					for(auto [e, _] : resolver.current_kinds()) {
						DBG(LOG_TRACE << "new_infer_ba_types/on_leave/default/e: " << LOG_FM_TREE(e);)
						if (!is<node, tau::variable>(e)) continue; // must be already typed
						auto resolved_type = resolver.type_of(untype(e)); // already untyped
						auto e_type = tau::get(e).get_type();
						auto final_type = resolved_type == untyped ? tau_type : resolved_type;
						if (tau::get(e).children_size()) {
							auto new_e = (final_type.second == nullptr)
							? tau::get_typed(e_type, tau::get(e).child(0), final_type.first)
							: tau::get_typed(e_type, tau::get(e).child(0), final_type.second,
								final_type.first);
							if (new_e != e) changes[untype(e)] = new_e;
							DBG(LOG_TRACE << "new_infer_ba_types/on_leave/default/new_e: " << LOG_FM_TREE(new_e);)
						} else { // bf_t/bf_f case
							auto new_e = (final_type.second == nullptr)
							? tau::get_typed(e_type, final_type.first)
							: tau::get_typed(e_type, final_type.second, final_type.first);
							if (new_e != e) changes[untype(e)] = new_e;
							DBG(LOG_TRACE << "new_infer_ba_types/on_leave/default/new_e: " << LOG_FM_TREE(new_e);)
						}
					}
					new_n = rewriter::replace<node>(new_n, changes);
					DBG(LOG_TRACE << "new_infer_ba_types/on_leave/default/new_n: " << LOG_FM_TREE(new_n);)
				}
				DBG(LOG_TRACE << "new_infer_ba_types/on_leave/default/new_n: " << LOG_FM_TREE(new_n);)
				if (new_n != n) transformed[n] = new_n;
				return;
			}
		}
	};

	// All nodes
	auto visit_all = [](tref n) {
		using tau = tree<node>;

		auto t = tau::get(n);
		size_t nt = t.get_type();
		switch (nt) {
			case tau::bf: case tau::bv:
				return false;
			default:
				return true;
		}
	};
	// We restore the original value of use_hooks at the end of the function
	auto using_hooks = tau::use_hooks;
	tau::use_hooks = false;
	// We visit the tree and return the transformed root if no error happened.
	// If an error happened we return nullptr.
	pre_order<node>(n).visit(on_enter, visit_all, on_leave, on_between);
	if (error) return tau::use_hooks = using_hooks,nullptr;
	// We add to the transformed map the untypping of the bf_t's and the bf_f's.
	// ...some code here...
	auto new_n = transformed.contains(n) ? transformed[n] : n;
	DBG(LOG_TRACE << "new_infer_ba_types/new_n:\n" << LOG_FM_TREE(new_n);)
	return  tau::use_hooks = using_hooks, new_n;
}

} // namespace idni::tau_lang
#endif // __IDNI__TAU__RESOLVER_H__