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
#include "boolean_algebras/bv_ba.h"


#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "resolver"

namespace idni::tau_lang {

template<typename data_t, typename kind_t, class less_t = std::less<data_t>>
struct scoped_resolver {
	using scope_t = size_t;
	using element_t = std::pair<scope_t, data_t>;

	struct scoped_less {
		bool operator()(const element_t& a, const element_t& b) const {
				static const less_t comp;
				if (a.first < b.first) return true;
				if (a.first > b.first) return false;
				// same scope
				return (comp(a.second, b.second));
		}
	};

	using kinds_t = std::map<element_t, kind_t, scoped_less>;
	using union_find_by_less_t = union_find_by_less<element_t, scoped_less>;

	union_find_by_less_t uf;
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
		/*std::erase_if(kinds_, [&](const auto& e)
			{ return e.first.first == current; });*/
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
				kinds_.insert_or_assign(uf.root(element_t{*it, data}), kind);
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
		auto scope = scopes_.back();
		for(auto [element, _] : uf)
			if (element.first == scope) result[element.second] = type_of(element.second);
		return result;
	}
};

template<NodeType node>
std::optional<typename tree<node>::type_t> merge_ba_types(const typename tree<node>::type_t& t1, const typename tree<node>::type_t& t2) {
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
struct type_scoped_resolver : public scoped_resolver<tref, typename tree<node>::type_t, idni::subtree_less<node>> {
	using tau = tree<node>;
	using tt = tau::traverser;

	type_scoped_resolver():
		scoped_resolver<tref, typename tree<node>::type_t, idni::subtree_less<node>>({0, nullptr}) {}

	// merge two trefs if the types are compatible
	// returns true if merge was successful, false otherwise
	bool merge(tref a, tref b) {
		auto type_a = this->type_of(a); auto scope_a = this->scope_of(a);
		auto type_b = this->type_of(b); auto scope_b = this->scope_of(b);
		DBG(LOG_TRACE << "type_scoped_resolver/merge: "
			<< LOG_FM(a) << ":" << type_a.first << "["
			<< type_a.second
			<< "] (scope " << scope_a << ")"
			<< " <-> "
			<< LOG_FM(b) << ":" << type_b.first << "["
			<< type_b.second
			<< "] (scope " << scope_b << ")\n";)
		auto merged = merge_ba_types<node>(type_a, type_b);
		if (!merged) return false; // conflicting type info
		auto new_parent = this->uf.merge({scope_a, a}, {scope_b, b});
		this->kinds_.insert_or_assign(new_parent, merged.value());
		DBG(LOG_TRACE << "type_scoped_resolver/merge: merged to "
			<< LOG_FM(new_parent.second) << ":" << merged.value().first << "["
			<< merged.value().second
			<< "]\n" << " (scope " << new_parent.first << ")";)
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

	std::ostream& dump(std::ostream& os) {
		os << "scopes: ";
		for (auto scope: this->scopes_)
			os << scope << ", ";
		os << "\n";
		for (auto [e,_]: this->uf) {
			auto type = this->kinds_.at(e);
			os << "\tscope: " << e.first << ", tref: " << LOG_FM(e.second)
			<< ", type: "
				<< type.first << "["
				<< ((type.second) ? tau::get(type.second).to_str() : "") << "]\n";
		}
		return os;
	}

	std::string dump_to_str() {
		std::stringstream ss;
		return dump(ss), ss.str();
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
	using type_t = tree<node>::type_t;
	using tt = tau::traverser;

	// Some type definitions
	static type_t untyped = { ba_types<node>::id("untyped"), nullptr };
	static type_t bv_type = { ba_types<node>::id("bv"), nullptr };
	static type_t tau_type = { ba_types<node>::id("tau"), nullptr };
	[[maybe_unused]]
	static type_t sbf_type = { ba_types<node>::id("sbf"), nullptr };

	// The following variables conform the state of the traversal
	// transformed: map from original tref to transformed tref
	// resolver: the type_scoped_resolver used to resolve types and scopes
	// error: true if an error happened during traversal
	type_scoped_resolver<node> resolver;
	subtree_map<node, tref> transformed;
	bool error = false;

	auto get_type_of = [](tref t) -> type_t {
		using tau = tree<node>;
		using tt = tau::traverser;

		size_t t_type;
		if (auto check = tt(t) | tau::type | tt::ref ; check) {
			t_type = ba_types<node>::id(tau::get(check).get_string());
		} else {
			t_type = tt(t) | tt::ba_type;
		}
		return { t_type, tt(t) | tau::subtype | tt::ref };
	};

	// Extract the type and subtype of a vector consisting of typeable trefs.
	// If the vector is empty, we return the default type (untyped). If there is
	// conflicting type information, we return nullopt.
	auto get_type = [&](trefs ts, type_t default_type) -> std::optional<type_t> {
		// If trefs is empty we return the default (untyped)
		if (ts.empty())	return std::optional<type_t>(default_type);
		std::optional<type_t> result = default_type;
		for (const auto& t : ts) {
			type_t current_type = get_type_of(t);
			type_t stored_type = resolver.type_of(t);
			auto type = merge_ba_types<node>(current_type, stored_type);
			if (!type) return std::nullopt; // conflicting type info
			result = merge_ba_types<node>(result.value(), type.value());
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

	auto untype = [](tref t) -> tref {
		using tau = tree<node>;
		if (is<node, tau::bf_t>(t))
			return tau::get(tau::bf_t);
		if (is<node, tau::bf_f>(t))
			return tau::get(tau::bf_f);
		if (is<node, tau::bf_constant>(t)) {
			auto nn = tau::get(t).value.ba_retype(0);
			return (tau::get(t).children_size() > 0) ? tau::get(nn, tau::get(t).first()) : tau::get(nn);
		}
		if (is<node, tau::bv_constant>(t))
			return tau::get(tau::bv_constant, {tau::get(t).first()});
		if (is<node, tau::variable>(t))
			return tau::get(tau::variable, {tau::get(t).first()});
		return t;
	};

	// We gather info about types and scopes while entering nodes
	auto on_enter = [&](tref n, tref) {
		DBG(assert(n != nullptr);)

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
			case tau::wff_all: case tau::wff_ex: {
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
				// We continue the traversal of children
				DBG(LOG_TRACE << "new_infer_ba_types/on_enter/wff_all.../resolver:\n";)
				DBG(LOG_TRACE << resolver.dump_to_str();)
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
				DBG(LOG_TRACE << "new_infer_ba_types/on_enter/bv_eq.../type: "
					<< type.value().first
					<< "[" << (type.value().second ? tau::get(type.value().second).to_str() : "") << "]\n";)
				if (type.value() != untyped && type.value().first != bv_type.first) {
					// We only allow bv type in bv equations
					return error = true, false;
				}
				// We add the variables  to the current scope, constants to an inner
				// scope and assign them the common type. We also collect them in a vector
				// to be merged together later.
				trefs mergeables;
				subtree_map<node, type_t> constants;
				for (const auto& t : typeables)	{
					auto ut = untype(t);
					if (is<node, tau::bv_constant>(ut)) {
						constants.emplace(ut, type.value());
						mergeables.push_back(ut);
						continue;
					}
					// We only add variables to the current scope
					// Constants will be added to an inner scope
					resolver.insert(ut);
					resolver.assign(ut, type.value());
					mergeables.push_back(ut);
				}
				// We create an inner scope for the constants
				resolver.open(constants);
				// We merge all the variables and constants in union-find data
				// structure. If there is a conflict, we set error and stop the
				// traversal.
				if (!resolver.merge(mergeables)) error = true;
				// Anyway, we stop the traversal of children as we have already
				// processed all the typeables in the expression.
				DBG(LOG_TRACE << "new_infer_ba_types/on_enter/bv_eq.../resolver:\n";)
				DBG(LOG_TRACE << resolver.dump_to_str();)
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
				DBG(LOG_TRACE << "new_infer_ba_types/on_enter/bf_eq.../type: "
					<< type.value().first
					<< "[" << (type.value().second ? tau::get(type.value().second).to_str() : "") << "]\n";)
				// We add the variables and the constants to the current scope
				// and assign them the common type.
				trefs mergeables;
				subtree_map<node, type_t> constants;
				for (const auto& t : typeables)	{
					auto ut = untype(t);
					if (is<node, tau::bf_constant>(ut) || is<node, tau::bf_f>(ut) || is<node, tau::bf_t>(ut)) {
						mergeables.push_back(ut);
						constants.emplace(ut, type.value());
						continue;
					}
					// We only add variables to the current scope
					// Constants will be added to an inner scope
					resolver.insert(ut);
					resolver.assign(ut, type.value());
					mergeables.push_back(ut);
				}
				// We create an inner scope for the constants
				resolver.open(constants);
				// We merge all the variables and constants in union-find data
				// structure. If there is a conflict, we set error and stop the
				// traversal.
				if (!resolver.merge(mergeables)) error = true;
				// Anyway, we stop the traversal of children as we have already
				// processed all the typeables in the expression.
				DBG(LOG_TRACE << "new_infer_ba_types/on_enter/bf_eq.../resolver:\n";)
				DBG(LOG_TRACE << resolver.dump_to_str();)
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

	/*auto is_bv_constant_parsed = [](tref t) -> bool {
		using tau = tree<node>;
		if (tau::get(t).children_size() > 0) {
			if (auto first = tau::get(t).child(0);
					 is<node, tau::decimal>(first)
					|| is<node, tau::hexadecimal>(first)
					|| is<node, tau::binary>(first))
				return false;
		}
		return true;
	};

	auto is_bf_constant_parsed = [](tref t) -> bool {
		using tau = tree<node>;
		if (tau::get(t).children_size() > 0) return false;
		return true;
	};*/

	// We update types (changing nodes while leaving them) while closing scopes
	auto on_leave = [&] (tref n, tref parent) {
		// We use transformed map to update children if they were any changes
		// and add the current node if resulted changed.
		DBG(assert(n != nullptr);)
		// Helper lambdas
		auto update_default = [&](tref n, subtree_map<node, tref>& changes) -> tref{
			// We transform the node according to the transformation of
			// its children skipping constants and variables as they
			// are replaced are higher levels.
			using tau = tree<node>;

			trefs ch;
			for (tref c : tau::get(n).children()) {
				if (changes.find(c) != changes.end())
					ch.push_back(changes[c]);
				else ch.push_back(c);
			}
			if (auto new_n = tau::get_raw(tau::get(n).value, ch.data(), ch.size()); new_n != n) {
				/*DBG(LOG_TRACE << "new_infer_ba_types/on_leave/update_default//n -> new_n:\n"
					<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(new_n);)*/
				changes.insert_or_assign(n, new_n);
			}
			return (changes.find(n) != changes.end()) ? changes[n] : n;
		};

		auto retype = [&](tref n, const type_t& new_type) -> tref {
			const auto& t = tau::get(n);
			auto n_type = t.get_type();
			/*if (t.is(tau::bf_constant) && !is_bf_constant_parsed(n)) // parse the source of the constant
				return tau::get_ba_constant_from_source(
					t.child_data(), new_type.first);
			if (t.is(tau::bv_constant) && !is_bv_constant_parsed(n)) { // parse the source of the constant
				auto bv_size = (new_type.second) ? tt(new_type.second) | tau::num | tt::num : default_bv_size;
				return tau::get_bv_constant_from_source(
					t.child_data(), bv_size);
			}*/
			if (tau::get(n).has_child()) {
				return (new_type.second == nullptr)
					? tau::get_typed(n_type, t.child(0), new_type.first)
					: tau::get_typed(n_type, t.child(0), new_type.second,
						new_type.first);
			} else {
				return (new_type.second == nullptr)
					? tau::get_typed(n_type, new_type.first)
					: tau::get_typed(n_type, new_type.second, new_type.first);
			}
		};

		auto update_variables = [&](tref n, const std::map<tref, type_t, subtree_less<node>>& types) -> tref {
			subtree_map<node, tref> changes;

			auto update = [&](tref n) -> bool {
				DBG(LOG_TRACE <<"new_infer_ba_types/update_variables/tau_use_hooks: " << tau::use_hooks << "\n";)
				const auto t = tau::get(n);
				size_t nt = t.get_type();
				switch (nt) {
					case tau::variable: {
						// If we have no type information for the element we do nothing
						const auto un = untype(n);
						if (types.find(un) == types.end()) break;
						// If the variable is not typed
						tref new_n;
						if (get_type_of(n) == untyped) {
							// We type it according to the inferred type or tau
							auto type = (types.at(un) == untyped)
								? tau_type
								: types.at(un);
							if (resolver.assign(un, type) == false) {
								LOG_ERROR << "Conflicting type information for variable "
									<< LOG_FM(n) << ", expected "
									<< types.at(un).first << "[" << (types.at(un).second ? tau::get(types.at(un).second).to_str() : "") << "]\n";
								return error = true, !error;
							}
							new_n = retype(n, type);
						} else {
							// Otherwise, we remove type children if any
							new_n = retype(n, get_type_of(n));
						}
						if (new_n != n) {
							DBG(LOG_TRACE << "new_infer_ba_types/update_variables/update/variable.../n -> new_n:\n"
								<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(new_n);)
							changes.insert_or_assign(n, new_n);
						}
						break;
					}
					default: {
						// We transform the node according to the transformation of
						// its children
						update_default(n, changes);
						break;
					}
				}
				return !error;
			};

			post_order<node>(n).search(update);
			if (changes.find(n) != changes.end()) {
				DBG(LOG_TRACE << "new_infer_ba_types/update_variables/n -> changes[n]:\n"
					<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(changes[n]);)
				return changes[n];
			}
			return n;
		};

		auto parse_bv_constants = [&](tref n, const std::map<tref, type_t, subtree_less<node>>& types) -> tref {
			subtree_map<node, tref> changes;

			auto update = [&](tref n) -> bool {
				DBG(LOG_TRACE <<"new_infer_ba_types/parse_bv_constants/tau_use_hooks: " << tau::use_hooks << "\n";)
				const auto t = tau::get(n);
				size_t nt = t.get_type();
				switch (nt) {
					case tau::bv_constant: {
						// If we have no type information for the bv constant we
						// rise an error as we should have at least untyped info
						const auto un = untype(n);
						if (types.find(un) == types.end()) {
							LOG_ERROR << "No detected type for bv constant in "
								<< LOG_FM(n) << "\n";
							return error = true, true;
						}
						// We compute the bitvector size from the type info or
						// use the default size if untyped
						type_t type = (types.at(un) == untyped)
							? bv_type
							: types.at(un);
						size_t bv_size = (type.second)
							? tt(type.second) | tau::num | tt::num
							: default_bv_size;
						// We parse the constant
						auto new_n = tau::get_bv_constant(n, bv_size);
						changes.insert_or_assign(n, new_n);
						DBG(LOG_TRACE << "new_infer_ba_types/parse_bv_constants/update/bv_constant/n -> new_n:\n"
							<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(new_n);)
						break;
					}
					default: {
						// We transform the node according to the transformation of
						// its children
						update_default(n, changes);
						break;
					}
				}
				return !error;
			};

			post_order<node>(n).search(update);
			if (changes.find(n) != changes.end()) {
				DBG(LOG_TRACE << "new_infer_ba_types/parse_bv_constants/n -> changes[n]:\n"
					<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(changes[n]);)
				return changes[n];
			}
			return n;
		};

		auto parse_bf_constants = [&](tref n, const std::map<tref, type_t, subtree_less<node>>& types) -> tref {
			subtree_map<node, tref> changes;

			auto update = [&](tref n) -> bool {
				DBG(LOG_TRACE <<"new_infer_ba_types/parse_bf_constants/tau_use_hooks: " << tau::use_hooks << "\n";)
				const auto t = tau::get(n);
				size_t nt = t.get_type();
				switch (nt) {
					case tau::bf_constant: {
						// If we have no type information for the bf constant we
						// rise an error as we should have at least untyped info
						const auto un = untype(n);
						if (types.find(un) == types.end()) {
							LOG_ERROR << "No detected type for bf constant in "
								<< LOG_FM(n) << "\n";
							return error = true, true;
						}
						// We get the type info or use the default (tau) if untyped
						type_t type = (types.at(un) == untyped)
							? tau_type
							: types.at(un);
						// We parse the constant
						auto new_n = tau::get_ba_constant_from_source(t.child_data(), type.first);
						if (new_n == nullptr) return error = true, true;
						changes.insert_or_assign(n, new_n);
						DBG(LOG_TRACE << "new_infer_ba_types/parse_bf_constants/update/bf_constant/n -> new_n:\n"
							<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(new_n);)
						break;
					}
					case tau::bf_t: case tau::bf_f: {
						// We untype bf_t and bf_f
						if (auto new_n = untype(n); new_n != n) {
							DBG(LOG_TRACE << "new_infer_ba_types/parse_bf_constants/update/bf_t.../n -> new_n:\n"
								<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(new_n);)
							changes.insert_or_assign(n, new_n);
						}
						break;
					}
					default: {
						// We transform the node according to the transformation of
						// its children
						update_default(n, changes);
						break;
					}
				}
				return !error;
			};

			post_order<node>(n).search(update);
			if (changes.find(n) != changes.end()) {
				DBG(LOG_TRACE << "new_infer_ba_types/parse_bf_constants/n -> changes[n]:\n"
					<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(changes[n]);)
				return changes[n];
			}
			return n;
		};

		auto get_scoped_elements = [&](size_t element_type) {
			std::map<tref, type_t, subtree_less<node>> elements;
			for(auto [e, resolved_type] : resolver.current_kinds())
				if (tau::get(e).is(element_type)) elements[e] = resolved_type;
#ifdef DEBUG
			LOG_TRACE << "new_infer_ba_types/on_leave/get_scoped_elements/elements:"
				<< " type = " << (element_type == tau::variable ? "variable" :
					(element_type == tau::bv_constant ? "bv_constant" :
						(element_type == tau::bf_constant ? "bf_constant" : "unknown")))
				<< " scope = " <<resolver.scopes_.back() << "\n";
			for (auto [e, t] : elements)
				LOG_TRACE << "\t" << LOG_FM(e) << " : "
					<< t.first << "["
					<< ((t.second) ? tau::get(t.second).to_str() : "") << "]\n";
			LOG_TRACE << "new_infer_ba_types/on_leave/get_scoped_elements/uf:\n";
			LOG_TRACE << resolver.dump_to_str();
#endif // DEBUG
			return elements;
		};

		DBG(LOG_TRACE << "new_infer_ba_types/on_leave/n:\n"
			<< LOG_FM_TREE(n);)
		// Stop traversal on error
		if (error) return;
		// Get the node type
		const auto& t = tau::get(n);
		size_t nt = t.get_type();
		// Depoending on the node type...
		switch (nt) {
			case tau::wff_all: case tau::wff_ex: /* case tau::bf_fall: case tau::bf_fex:*/
			case tau::rec_relation: {
				tref new_n = update_default(n, transformed);
				auto scoped_var_types = get_scoped_elements(tau::variable);
				if(auto updated = update_variables(new_n, scoped_var_types); updated != new_n) {
					DBG(LOG_TRACE << "new_infer_ba_types/on_leave/wff_all.../n -> updated:\n"
						<< LOG_FM_TREE(new_n) << " -> " << LOG_FM_TREE(updated);)
						transformed.insert_or_assign(n, updated);
					}
				resolver.close();
				DBG(LOG_TRACE << "new_infer_ba_types/on_leave/wff_all.../resolver:\n";)
				DBG(LOG_TRACE << resolver.dump_to_str();)
				return;
			}
			case tau::bv_eq: case tau::bv_neq: case tau::bv_lteq: case tau::bv_nlteq:
			case tau::bv_gt: case tau::bv_ngt: case tau::bv_gteq: case tau::bv_ngteq:
			case tau::bv_lt: case tau::bv_nlt: {
				auto scoped_bv_ctes_types = get_scoped_elements(tau::bv_constant);
				if(auto updated = parse_bv_constants(n, scoped_bv_ctes_types); updated != n) {
					DBG(LOG_TRACE << "new_infer_ba_types/on_leave/bv_eq.../n -> updated:\n"
						<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(updated);)
					transformed.insert_or_assign(n, updated);
				}
				resolver.close();
				DBG(LOG_TRACE << "new_infer_ba_types/on_leave/bv_eq.../resolver:\n";)
				DBG(LOG_TRACE << resolver.dump_to_str();)
				return;
			}
			case tau::bf_eq: case tau::bf_neq: case tau::bf_lteq: case tau::bf_nlteq:
			case tau::bf_gt: case tau::bf_ngt: case tau::bf_gteq: case tau::bf_ngteq:
			case tau::bf_lt: case tau::bf_nlt: {
				auto scoped_bf_ctes_types = get_scoped_elements(tau::bf_constant);
				if(auto updated = parse_bf_constants(n, scoped_bf_ctes_types); updated != n) {
					DBG(LOG_TRACE << "new_infer_ba_types/on_leave/bf_eq.../n -> updated:\n"
						<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(updated);)
					transformed.insert_or_assign(n, updated);
				}
				resolver.close();
				DBG(LOG_TRACE << "new_infer_ba_types/on_leave/bf_eq.../resolver:\n";)
				DBG(LOG_TRACE << resolver.dump_to_str();)
				return;
			}
			default: {
				tref new_n = update_default(n, transformed);
				// For the root node, we type untyped variables with tau.
				if (!parent) {
					auto scoped_var_types = get_scoped_elements(tau::variable);
					if(auto updated = update_variables(new_n, scoped_var_types); updated != new_n) {
						new_n = updated;
					}
				}
				if (new_n != n) {
					DBG(LOG_TRACE << "new_infer_ba_types/on_leave/default/n -> new_n:\n"
						 << LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(new_n);)
					transformed.insert_or_assign(n, new_n);
				}
				DBG(LOG_TRACE << "new_infer_ba_types/on_leave/default/resolver:\n";)
				DBG(LOG_TRACE << resolver.dump_to_str();)
				return;
			}
		}
	};

	// All nodes
	auto visit_outside_equations = [](tref n) {
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
	pre_order<node>(n).visit(on_enter, visit_outside_equations, on_leave, on_between);
	if (error) return tau::use_hooks = using_hooks, nullptr;
	// We add to the transformed map the untypping of the bf_t's and the bf_f's.
	// ...some code here...
	auto new_n = transformed.contains(n) ? transformed[n] : n;
	return  tau::use_hooks = using_hooks, new_n;
}

} // namespace idni::tau_lang
#endif // __IDNI__TAU__RESOLVER_H__