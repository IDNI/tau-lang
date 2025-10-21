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
#include "ba_types.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "resolver"

namespace idni::tau_lang {

template<NodeType node>
std::optional<size_t> merge_ba_types(size_t tid1, size_t tid2) {
	tref t1 = ba_types<node>::type_tree(tid1);
	tref t2 = ba_types<node>::type_tree(tid2);
	if (is_untyped<node>(t1)) {
		if (!is_untyped<node>(t2)) {
			return { tid2 };
		} else {
			return { tid1 }; // both untyped
		}
	}
	if (is_untyped<node>(t2)) return { tid1 };
	if (is_same_ba_type<node>(t1, t2)) return { tid1 };
	if (tref u = unify<node>(t1, t2); u) return { get_ba_type_id<node>(u) };
	return std::nullopt;
}

template<NodeType node>
struct type_scoped_resolver : public scoped_union_find<tref, size_t, idni::subtree_less<node>> {
	using tau = tree<node>;
	using tt = tau::traverser;

	type_scoped_resolver():
		scoped_union_find<tref, size_t, idni::subtree_less<node>>(0) {}

	// merge two trefs if the types are compatible
	// returns true if merge was successful, false otherwise
	bool merge(tref a, tref b) {
		auto type_a = this->type_of(a); auto scope_a = this->scope_of(a);
		auto type_b = this->type_of(b); auto scope_b = this->scope_of(b);
		DBG(LOG_TRACE << "type_scoped_resolver/merge: "
			<< LOG_FM(a) << ":" << ba_types<node>::name(type_a)
			<< " (scope " << scope_a << ")"
			<< " <-> "
			<< LOG_FM(b) << ":" << ba_types<node>::name(type_b)
			<< " (scope " << scope_b << ")\n";)
		auto merged = merge_ba_types<node>(type_a, type_b);
		if (!merged) return false; // conflicting type info
		auto new_parent = this->uf.merge({scope_a, a}, {scope_b, b});
		this->kinds_.insert_or_assign(new_parent, merged.value());
		DBG(LOG_TRACE << "type_scoped_resolver/merge: merged to "
			<< LOG_FM(new_parent.second) << ":" << ba_types<node>::name(merged.value())
			<< "\n" << " (scope " << new_parent.first << ")";)
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
			<< ", type: " << ba_types<node>::name(type) << "\n";
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
	using tt = tau::traverser;

	// Some type definitions
	static size_t untyped_id = ba_types<node>::id(untyped_type<node>());
	static size_t bv_type_id = ba_types<node>::id(bv_type<node>());
	static size_t tau_type_id = ba_types<node>::id(tau_type<node>());

	// The following variables conform the state of the traversal
	// transformed: map from original tref to transformed tref
	// resolver: the type_scoped_resolver used to resolve types and scopes
	// error: true if an error happened during traversal
	type_scoped_resolver<node> resolver;
	subtree_map<node, tref> transformed;
	bool error = false;

	auto get_type_of = [](tref t) -> size_t {
		size_t t_type;
		if (auto check = tt(t) | tau::typed | tt::ref ; check) {
			t_type = ba_types<node>::id(check);
		} else {
			t_type = tt(t) | tt::ba_type;
		}
		return t_type;
	};

	// Extract the type and subtype of a vector consisting of typeable trefs.
	// If the vector is empty, we return the default type (untyped). If there is
	// conflicting type information, we return nullopt.
	auto get_type = [&](trefs ts, size_t default_type) -> std::optional<size_t> {
		// If trefs is empty we return the default (untyped)
		if (ts.empty())	return { default_type };
		std::optional<size_t> result = default_type;
		for (tref t : ts) {
			size_t current_type = get_type_of(t);
			size_t stored_type = resolver.type_of(t);
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
			|| is<node, tau::ba_constant>(t)
			|| is<node, tau::bf_t>(t)
			|| is<node, tau::bf_f>(t);
	};

	auto untype = [](tref t) -> tref {
		if (is<node, tau::bf_t>(t)) {
			return tau::get(tau::bf_t);
		} else if (is<node, tau::bf_f>(t)) {
			return tau::get(tau::bf_f);
		} else if (is<node, tau::ba_constant>(t)) {
			auto nn = tau::get(t).value.ba_retype(0);
			return (tau::get(t).children_size() > 0) ? tau::get(nn, tau::get(t).first()) : tau::get(nn);
		} else if (is<node, tau::variable>(t)) {
			return tau::get(tau::variable, {tau::get(t).first()});
		}
		return t;
	};

	auto is_top_level_bf = [&](tref parent) -> bool {
		if (parent == nullptr) return true;
		auto nt = tau::get(parent).get_type();
		switch (nt) {
			// bf logical connectives
			case tau::bf_and: case tau::bf_or: case tau::bf_neg:
			case tau::bf_xor:
			// bf quantifiers
			case tau::bf_fall: case tau::bf_fex:
			// bf atomic formulas
			case tau::bf_eq: case tau::bf_neq: case tau::bf_lteq:
			case tau::bf_nlteq: case tau::bf_gt: case tau::bf_ngt:
			case tau::bf_gteq: case tau::bf_ngteq: case tau::bf_lt:
			case tau::bf_nlt: case tau::bf_interval:
				return false;
			default:
				return true;
		}
	};

	// We gather info about types and scopes while entering nodes
	auto on_enter = [&](tref n, tref parent) {
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
				subtree_map<node, size_t> var_list;
				auto vars = tau::get(n)[0].select_top(is<node, tau::variable>);
				for (tref var : vars) {
					var_list[untype(var)] =  get_type_of(var);
				}
				resolver.open(var_list);
				// We continue the traversal of children
				DBG(LOG_TRACE << "new_infer_ba_types/on_enter/wff_all.../resolver:\n";)
				DBG(LOG_TRACE << resolver.dump_to_str();)
				break;
			}
			// TODO (HIGH) deal with those eq appropiately
			/*case tau::bv_eq: case tau::bv_neq: case tau::bv_lteq: case tau::bv_nlteq:
			case tau::bv_gt: case tau::bv_ngt: case tau::bv_gteq: case tau::bv_ngteq:
			case tau::bv_lt: case tau::bv_nlt: {
				// We get all the (top) typeables in the expression and assign
				// then the type bv (and the subtype when available). We do the
				// same for the constants.

				// We get all the typeable top nodes in the expression, which in
				// this case are only (bitvector) variables and constants.
				auto typeables = tau::get(n).select_top(is_typeable);
				// We infer the common type of all the typeables in the expression
				// TODO (HIGH) make scoped_union_find accept a partial order of
				// types and change untyped to bv as default type here.
				auto type = get_type(typeables,
					get_ba_type_id<node>(bv_base_type<node>()));
				// If no common type is found, we set error and stop traversal
				if (!type) {
					LOG_ERROR << "Conflicting type information in bv equation "
						<< LOG_FM(n) << "\n";
					return error = true, false;
				}
				DBG(LOG_TRACE << "new_infer_ba_types/on_enter/bv_eq.../type: "
					<< ba_types<node>::name(type.value()) << "\n";)
				if (type.value() != untyped_id && !is_bv_type_family<node>(
					type.value())) {
					// We only allow bv type in bv equations
					LOG_ERROR << "Invalid type information in bv equation "
						<< LOG_FM(n) << ": "
						<< ba_types<node>::name(type.value())
						<< "\n";
					return error = true, false;
				}
				// We add the variables  to the current scope, constants to an inner
				// scope and assign them the common type. We also collect them in a vector
				// to be merged together later.
				trefs mergeables;
				subtree_map<node, size_t> constants;
				for (tref typeable : typeables)	{
					tref ut = untype(typeable);
					if (is<node, tau::ba_constant>(ut)) {
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
				if (!resolver.merge(mergeables)) {
					LOG_ERROR << "Conflicting type information in bv equation "
						<< LOG_FM(n) << "\n";
					return error = true, false;
				}
				// Anyway, we stop the traversal of children as we have already
				// processed all the typeables in the expression.
				DBG(LOG_TRACE << "new_infer_ba_types/on_enter/bv_eq.../resolver:\n";)
				DBG(LOG_TRACE << resolver.dump_to_str();)
				break;
			}*/
			case tau::bf: {
				// If bf is top level we treat it as a bf equation (we
				// FALLTHROUGH). Otherwise we don't traverse its children.
				if (!is_top_level_bf(parent)) return false;
				// We get all the typeable top nodes in the expression, which in
				// this case are only (sbf/tau) variables and constants.
				auto typeables = tau::get(n).select_top(is_typeable);
				// We infer the common type of all the typeables in the expression
				auto type = get_type(typeables, untyped_id);
				// If no common type is found, we set error and stop traversal
				if (!type){
					LOG_ERROR << "Conflicting type information in bf "
						<< LOG_FM(n) << "\n";
					return error = true, false;
				}
				DBG(LOG_TRACE << "new_infer_ba_types/on_enter/bf.../type: "
					<< ba_types<node>::name(type.value())
					<< "\n";)
				// We add the variables and the constants to the current scope
				// and assign them the common type.
				trefs mergeables;
				for (tref typeable : typeables)	{
					tref ut = untype(typeable);
					resolver.insert(ut);
					resolver.assign(ut, type.value());
					mergeables.push_back(ut);
				}
				if (!resolver.merge(mergeables)) {
					LOG_ERROR << "Conflicting type information in bf "
						<< LOG_FM(n) << "\n";
					return error = true, false;
				}
				// Anyway, we stop the traversal of children as we have already
				// processed all the typeables in the expression.
				DBG(LOG_TRACE << "new_infer_ba_types/on_enter/b.../resolver:\n";)
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
				auto type = get_type(typeables, untyped_id);
				// If no common type is found, we set error and stop traversal
				if (!type){
					LOG_ERROR << "Conflicting type information in bf equation "
						<< LOG_FM(n) << "\n";
					return error = true, false;
				}
				DBG(LOG_TRACE << "new_infer_ba_types/on_enter/bf_eq.../type: "
					<< ba_types<node>::name(type.value())
					<< "\n";)
				// We add the variables and the constants to the current scope
				// and assign them the common type.
				trefs mergeables;
				subtree_map<node, size_t> constants;
				for (tref typeable : typeables)	{
					tref ut = untype(typeable);
					if (is<node, tau::ba_constant>(ut) || is<node, tau::bf_f>(ut) || is<node, tau::bf_t>(ut)) {
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
				if (!resolver.merge(mergeables)) {
					LOG_ERROR << "Conflicting type information in bf equation "
						<< LOG_FM(n) << "\n";
					return error = true, false;
				}
				// Anyway, we stop the traversal of children as we have already
				// processed all the typeables in the expression.
				DBG(LOG_TRACE << "new_infer_ba_types/on_enter/bf_eq.../resolver:\n";)
				DBG(LOG_TRACE << resolver.dump_to_str();)
				break;
			}
			/*case tau::bv: {
				// As we are inside an equation already visited, we do not need
				// to continue the traversal of children.
				return false;
			}*/
			default:
				// Otherwise, we continue the traversal of children
				// without doing anything special in this node.
				break;
		}
		// Stop traversal on error
		return !error;
	};

	auto update_default = [&](tref n, subtree_map<node, tref>& changes) -> tref{
		// We transform the node according to the transformation of
		// its children skipping constants and variables as they
		// are replaced are higher levels.
		trefs ch;
		for (tref c : tau::get(n).children()) {
			if (changes.find(c) != changes.end())
				ch.push_back(changes[c]);
			else ch.push_back(c);
		}
		// TODO: Can get_raw cause problems?
		if (auto new_n = tau::get_raw(tau::get(n).value, ch.data(), ch.size()); new_n != n) {
			/*DBG(LOG_TRACE << "new_infer_ba_types/on_leave/update_default//n -> new_n:\n"
				<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(new_n);)*/
			changes.insert_or_assign(n, new_n);
		}
		return (changes.find(n) != changes.end()) ? changes[n] : n;
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
		// Helper lambdas

		auto retype = [&](tref n, const size_t new_type) -> tref {
			const tau& t = tau::get(n);
			auto n_type = t.get_type();
			/*if (t.is(tau::bf_constant) && !is_bf_constant_parsed(n)) // parse the source of the constant
				return tau::get_ba_constant_from_source(
					t.child_data(), new_type.first);
			if (t.is(tau::bv_constant) && !is_bv_constant_parsed(n)) { // parse the source of the constant
				auto bv_size = (new_type.second) ? tt(new_type.second) | tau::num | tt::num : default_bv_size;
				return tau::get_bv_constant_from_source(
					t.child_data(), bv_size);
			}*/
			if (tau::get(n).has_child())
				return tau::get_typed(n_type, t.child(0), new_type);
			else
				return tau::get_typed(n_type, new_type);
		};

		auto update_variables = [&](tref n, const std::map<tref, size_t, subtree_less<node>>& types, const size_t default_type) -> tref {
			subtree_map<node, tref> changes;

			auto update = [&](tref n) -> bool {
				DBG(LOG_TRACE <<"new_infer_ba_types/update_variables/tau_use_hooks: " << tau::use_hooks << "\n";)
				const auto t = tau::get(n);
				size_t nt = t.get_type();
				switch (nt) {
					case tau::variable: {
						// If we have no type information for the element we do nothing
						tref un = untype(n);
						if (!types.contains(un)) break;
						// If the variable is not typed
						tref new_n;
						if (get_type_of(n) == untyped_id) {
							// We type it according to the inferred type or tau
							size_t type = (types.at(un) == untyped_id)
								? default_type
								: types.at(un);
							if (resolver.assign(un, type) == false) {
								LOG_ERROR << "Conflicting type information for variable "
									<< LOG_FM(n) << ", expected "
									<< ba_types<node>::name(types.at(un)) << "\n";
								return error = true, false;
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

		/*auto parse_bv_constants = [&](tref n, const std::map<tref, size_t, subtree_less<node>>& types) -> tref {
			subtree_map<node, tref> changes;

			auto update = [&](tref n) -> bool {
				DBG(LOG_TRACE <<"new_infer_ba_types/parse_bv_constants/tau_use_hooks: " << tau::use_hooks << "\n";)
				const auto t = tau::get(n);
				size_t nt = t.get_type();
				switch (nt) {
					case tau::ba_constant: {
						// If we have no type information for the bv constant we
						// rise an error as we should have at least untyped info
						tref un = untype(n);
						if (!types.contains(un)) {
							LOG_ERROR << "No detected type for bv constant in "
								<< LOG_FM(n) << "\n";
							return error = true, false;
						}
						// We compute the bitvector size from the type info or
						// use the default size if untyped
						size_t type = (types.at(un) == untyped_id)
							? bv_type_id
							: types.at(un);
						// We parse the constant
						auto new_n = tau::get_bv_constant_from_source(t.child_data(), type);
						if (new_n == nullptr) {
							LOG_ERROR << "Could not parse bv constant in "
								<< LOG_FM(n) << " with type "
								<< ba_types<node>::name(type) << "\n";
							return error = true, false;
						}
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
		};*/

		auto parse_ba_constants = [&](tref n, const std::map<tref, size_t, subtree_less<node>>& types) -> tref {
			subtree_map<node, tref> changes;

			auto update = [&](tref n) -> bool {
				DBG(LOG_TRACE <<"new_infer_ba_types/parse_ba_constants/tau_use_hooks: " << tau::use_hooks << "\n";)
				const auto t = tau::get(n);
				size_t nt = t.get_type();
				switch (nt) {
					case tau::ba_constant: {
						// If we have no type information for the bf constant we
						// rise an error as we should have at least untyped info
						tref un = untype(n);
						if (!types.contains(un)) {
							LOG_ERROR << "No detected type for bf constant in "
								<< LOG_FM(n) << "\n";
							return error = true, false;
						}
						// We get the type info or use the default (tau) if untyped
						size_t type = (types.at(un) == untyped_id)
							? tau_type_id
							: types.at(un);
						// We parse the constant
						auto new_n = tau::get_ba_constant_from_source(t.child_data(), type);
						if (new_n == nullptr) {
							LOG_ERROR << "Could not parse bf constant in "
								<< LOG_FM(n) << " with type "
								<< ba_types<node>::name(type) << "\n";
							return error = true, false;
						}
						changes.insert_or_assign(n, new_n);
						DBG(LOG_TRACE << "new_infer_ba_types/parse_ba_constants/update/ba_constant/n -> new_n:\n"
							<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(new_n);)
						break;
					}
					case tau::bf_t: case tau::bf_f: {
						// We untype bf_t and bf_f
						if (auto new_n = untype(n); new_n != n) {
							DBG(LOG_TRACE << "new_infer_ba_types/parse_ba_constants/update/bf_t.../n -> new_n:\n"
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
				DBG(LOG_TRACE << "new_infer_ba_types/parse_ba_constants/n -> changes[n]:\n"
					<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(changes[n]);)
				return changes[n];
			}
			return n;
		};

		auto get_scoped_elements = [&](size_t element_type) {
			std::map<tref, size_t, subtree_less<node>> elements;
			for(auto [e, resolved_type] : resolver.current_kinds())
				if (tau::get(e).is(element_type)) elements[e] = resolved_type;
#ifdef DEBUG
			LOG_TRACE << "new_infer_ba_types/on_leave/get_scoped_elements/elements:"
				<< " type = " << (element_type == tau::variable ? "variable" :
					(element_type == tau::ba_constant ? "ba_constant" : "unknown"))
				<< " scope = " <<resolver.scopes_.back() << "\n";
			for (auto [e, t] : elements)
				LOG_TRACE << "\t" << LOG_FM(e) << " : "
					<< ba_types<node>::name(t) << "\n";
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
				if(auto updated = update_variables(new_n, scoped_var_types, tau_type_id); updated != new_n) {
					DBG(LOG_TRACE << "new_infer_ba_types/on_leave/wff_all.../n -> updated:\n"
						<< LOG_FM_TREE(new_n) << " -> " << LOG_FM_TREE(updated);)
						transformed.insert_or_assign(n, updated);
					}
				resolver.close();
				DBG(LOG_TRACE << "new_infer_ba_types/on_leave/wff_all.../resolver:\n";)
				DBG(LOG_TRACE << resolver.dump_to_str();)
				return;
			}
			// TODO (HIGH) deal with those eq appropiately
			/*case tau::bv_eq: case tau::bv_neq: case tau::bv_lteq: case tau::bv_nlteq:
			case tau::bv_gt: case tau::bv_ngt: case tau::bv_gteq: case tau::bv_ngteq:
			case tau::bv_lt: case tau::bv_nlt: {
				auto scoped_bv_ctes_types = get_scoped_elements(tau::ba_constant);
				if(auto updated = parse_ba_constants(n, scoped_bv_ctes_types); updated != n) {
					DBG(LOG_TRACE << "new_infer_ba_types/on_leave/bv_eq.../n -> updated:\n"
						<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(updated);)
					transformed.insert_or_assign(n, updated);
				}
				resolver.close();
				DBG(LOG_TRACE << "new_infer_ba_types/on_leave/bv_eq.../resolver:\n";)
				DBG(LOG_TRACE << resolver.dump_to_str();)
				return;
			}*/
			case tau::bf_eq: case tau::bf_neq: case tau::bf_lteq: case tau::bf_nlteq:
			case tau::bf_gt: case tau::bf_ngt: case tau::bf_gteq: case tau::bf_ngteq:
			case tau::bf_lt: case tau::bf_nlt: {
				auto scoped_bf_ctes_types = get_scoped_elements(tau::ba_constant);
				if(auto updated = parse_ba_constants(n, scoped_bf_ctes_types); updated != n) {
					DBG(LOG_TRACE << "new_infer_ba_types/on_leave/bf_eq.../n -> updated:\n"
						<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(updated);)
					transformed.insert_or_assign(n, updated);
				}
				resolver.close();
				DBG(LOG_TRACE << "new_infer_ba_types/on_leave/bf_eq.../resolver:\n";)
				DBG(LOG_TRACE << resolver.dump_to_str();)
				return;
			}
			case tau::bf: {
				if (is_top_level_bf(parent)) {
					// We only process top-level bf nodes, as the inner ones
					// are part of bf equations already processed.
					auto scoped_bf_ctes_types = get_scoped_elements(tau::ba_constant);
					if(auto updated = parse_ba_constants(n, scoped_bf_ctes_types); updated != n) {
						DBG(LOG_TRACE << "new_infer_ba_types/on_leave/bf.../n -> updated:\n"
							<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(updated);)
						transformed.insert_or_assign(n, updated);
					}
					auto scoped_var_types = get_scoped_elements(tau::variable);
					if(auto updated = update_variables(n, scoped_var_types, tau_type_id); updated != n) {
						DBG(LOG_TRACE << "new_infer_ba_types/on_leave/bf.../n -> updated:\n"
							<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(updated);)
						transformed.insert_or_assign(n, updated);
					}
				}
				return;
			}
			default: {
				tref new_n = update_default(n, transformed);
				// For the root node, we type untyped variables with tau.
				if (!parent) {
					auto scoped_var_types = get_scoped_elements(tau::variable);
					new_n = update_variables(new_n, scoped_var_types, tau_type_id);
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
		const tau& t = tau::get(n);
		size_t nt = t.get_type();
		switch (nt) {
			// TODO (HIGH) deal with those eq appropiately
			/*case tau::bv:
				return false;*/
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
	// We add to the transformed map the untyping of the bf_t's and the bf_f's.
	// ...some code here...
	tref new_n = transformed.contains(n) ? transformed[n] : n;
	// TODO: unify the following with rest of type inference algorithm, once bv tag is gone

	// Convert all bv default types to bv[16]
	auto f = [](tref n) {
		const tau& n_t = tau::get(n);
		if (n_t.get_ba_type() ==
			get_ba_type_id<node>(bv_base_type<node>())) {
			return n_t.replace_value(n_t.value.ba_retype(bv_type_id));
		}
		return n;
	};
	new_n = pre_order<node>(new_n).apply_unique(f);

	// type all symbols according to their children's types
	auto update_symbols = [&](tref n) -> tref {
		subtree_map<node, tref> changes;

		auto update = [&] (tref n) -> tref {

			auto update_symbol = [&](tref n) -> tref {
				// We have one child at least and we know that the types of the
				// children have already been updated and they are consistent.
				auto t = tau::get(n);
				auto chs = t.get_children();
				auto n_type = tau::get(chs[0]).get_ba_type();
				auto new_n = tau::get(t.value.ba_retype(n_type), chs);
				DBG(LOG_TRACE << "new_infer_ba_types/update_symbols/n -> new_n:\n"
					<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(new_n);)
				return new_n;
			};

			const tau& t = tau::get(n);
			size_t nt = t.get_type();
			switch (nt) {
				// no bv types allowed
				case tau::bf_interval: {
					auto new_n = update_symbol(n);
					if (error) return n;
					if (is_bv_type_family<node>(new_n)) {
						LOG_ERROR << "Invalid bv type for bf_interval "
							<< LOG_FM(n) << "\n";
						return error = true, n;
					}
					changes.insert_or_assign(n, new_n);
					break;
				}

				// all types allowed
				case tau::bf: case tau::bf_eq: case tau::bf_neq:
				case tau::bf_lteq: case tau::bf_nlteq: case tau::bf_gt:
				case tau::bf_ngt: case tau::bf_gteq: case tau::bf_ngteq:
				case tau::bf_lt: case tau::bf_nlt: case tau::bf_or:
				case tau::bf_xor: case tau::bf_and: case tau::bf_neg: {
					auto new_n = update_symbol(n);
					if (error) return n;
					changes.insert_or_assign(n, new_n);
					break;
				}

				// only bv types allowed
				case tau::bf_add: case tau::bf_sub: case tau::bf_mul:
				case tau::bf_div: case tau::bf_mod: case tau::bf_shr:
				case tau::bf_shl: {
					auto new_n = update_symbol(n);
					if (error) return n;
					if (!is_bv_type_family<node>(new_n)) {
						LOG_ERROR << "Invalid bv type for bf_interval "
							<< LOG_FM(n) << "\n";
						return error = true, n;
					}
					changes.insert_or_assign(n, new_n);
					break;
				}
				default:
					update_default(n, changes);
					break;
			}
			return n;
		};

		post_order<node>(n).search(update);
		if (error) return tau::use_hooks = using_hooks, nullptr;
		if (changes.find(n) != changes.end()) {
			DBG(LOG_TRACE << "new_infer_ba_types/update_variables/n -> changes[n]:\n"
				<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(changes[n]);)
			return changes[n];
		}
		return n;
	};

	new_n = update_symbols(new_n);
	return  tau::use_hooks = using_hooks, new_n;
}

template <NodeType node>
tref infer_ba_types(tref n) {
	using tau = tree<node>;

	return tau::reget(new_infer_ba_types<node>(n));
}

} // namespace idni::tau_lang
#endif // __IDNI__TAU__RESOLVER_H__