// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

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
bool type_scoped_resolver<node>::merge(tref a, tref b) {
	auto type_a = this->type_of(a); auto scope_a = this->scope_of(a);
	auto type_b = this->type_of(b); auto scope_b = this->scope_of(b);
	DBG(LOG_TRACE << "type_scoped_resolver/merge: "
		<< LOG_FM(a) << ":" << ba_types<node>::name(type_a)
		<< " (scope " << scope_a << ")"
		<< " <-> "
		<< LOG_FM(b) << ":" << ba_types<node>::name(type_b)
		<< " (scope " << scope_b << ")\n";)
	auto merged = unify<node>(type_a, type_b);
	if (!merged) return false; // conflicting type info
	auto new_parent = this->uf.merge({scope_a, a}, {scope_b, b});
	this->kinds_.insert_or_assign(new_parent, merged.value());
	DBG(LOG_TRACE << "type_scoped_resolver/merge: merged to "
		<< LOG_FM(new_parent.second) << ":" << ba_types<node>::name(merged.value())
		<< "\n" << " (scope " << new_parent.first << ")";)
	// We also update the type of the merged elements
	//this->kinds_[{this->current, a}] = merged;
	//this->kinds_[{this->current, b}] = merged;
	return true;
}

template<NodeType node>
bool type_scoped_resolver<node>::merge(const trefs& ts) {
	if (ts.size() < 2) return true;
	auto it = ts.begin();
	tref first = *it;
	++it;
	for(; it != ts.end(); ++it)
		if (!merge(first, *it)) return false;
	return true;
}

#ifdef DEBUG

template<NodeType node>
std::ostream& type_scoped_resolver<node>::dump(std::ostream& os) {
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

template<NodeType node>
std::string type_scoped_resolver<node>::dump_to_str() {
	std::stringstream ss;
	return dump(ss), ss.str();
}

#endif // DEBUG

// type all symbols according to their children's types
template<NodeType node>
tref update_symbols(tref n) {
	using tau = tree<node>;
	subtree_map<node, tref> changes;
	auto using_hooks = tau::use_hooks;
	auto error = false;

	auto update = [&] (tref n) -> bool {

		auto update_symbol = [&](tref n) -> tref {
			// We have one child at least and we know that the types of the
			// children have already been updated and they are consistent.
			auto t = tau::get(n);
			auto chs = t.get_children();
			auto n_type = tau::get(chs[0]).get_ba_type();
			auto new_n = tau::get_raw(t.value.ba_retype(n_type), chs.data(), chs.size());
			DBG(LOG_TRACE << "infer_ba_types/update_symbol/n -> new_n:\n"
				<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(new_n);)
			return new_n;
		};

		auto nn = update_default<node>(n, changes);
		const tau& t = tau::get(nn);
		size_t nt = t.get_type();
		switch (nt) {
			// all types allowed
			case tau::bf_interval:
			case tau::bf: case tau::bf_eq: case tau::bf_neq:
			case tau::bf_lteq: case tau::bf_nlteq: case tau::bf_gt:
			case tau::bf_ngt: case tau::bf_gteq: case tau::bf_ngteq:
			case tau::bf_lt: case tau::bf_nlt: case tau::bf_or:
			case tau::bf_xor: case tau::bf_and: case tau::bf_neg: {
				auto new_n = update_symbol(nn);
				if (error) return nn;
				DBG(LOG_TRACE << "infer_ba_types/update_symbols/default/n -> new_n:\n"
					<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(new_n);)
				changes.insert_or_assign(n, new_n);
				break;
			}

			// only bv types allowed
			case tau::bf_add: case tau::bf_sub: case tau::bf_mul:
			case tau::bf_div: case tau::bf_mod: case tau::bf_shr:
			case tau::bf_shl: case tau::bf_xnor: case tau::bf_nand:
			case tau::bf_nor: {
				auto new_n = update_symbol(nn);
				if (error) return nn;
				if (!is_bv_type_family<node>(tau::get(new_n).get_ba_type())) {
					LOG_ERROR << "Invalid type for "
						<< LOG_FM(n) << "\n";
					return error = true, nn;
				}
				DBG(LOG_TRACE << "infer_ba_types/update_symbols/default/n -> new_n:\n"
					<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(new_n);)
				changes.insert_or_assign(n, new_n);
				break;
			}
			default:
				auto new_n = update_default<node>(nn, changes);
				if (new_n != nn) {
					DBG(LOG_TRACE << "infer_ba_types/update_symbols/default/n -> new_n:\n"
						<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(new_n);)
					changes.insert_or_assign(n, new_n);
				}
				break;
		}
		return true;
	};

	post_order<node>(n).search(update);
	if (error) return tau::use_hooks = using_hooks, nullptr;
	if (changes.find(n) != changes.end()) {
		DBG(LOG_TRACE << "infer_ba_types/update_symbols/n -> changes[n]:\n"
			<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(changes[n]);)
		return changes[n];
	}
	return n;
}

// Typeable trefs predicate
template<NodeType node>
auto is_typeable = [](tref t) -> bool {
	using tau = tree<node>;
	return is<node, tau::variable>(t)
		|| is<node, tau::ba_constant>(t)
		|| is<node, tau::bf_t>(t)
		|| is<node, tau::bf_f>(t);
		// || is<node, tau::ref>(t);
};

template<NodeType node>
auto untype = [](tref t) -> tref {
	using tau = tree<node>;
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

template<NodeType node>
auto canonize = [](tref t) -> tref {
	using tau = tree<node>;
	using tt = tau::traverser;

	tref new_t = untype<node>(t);
	if (auto var_name = tt(new_t) | tau::io_var | tau::var_name | tt::ref; var_name) {
		new_t = tau::get(tau::variable, tau::get(tau::io_var, { var_name }));
	} else if (auto sym = tt(new_t) | tau::sym | tt::ref; sym) {
		auto ref_args = tt(new_t) | tau::ref_args | tt::ref;
		auto offsets = tt(new_t) | tau::offsets | tt::ref;
		auto num_ref_args = (ref_args) ? (tau::get(ref_args).children_size()) : 0;
		auto size_offsets = (offsets) ? (tau::get(offsets).children_size()) : 0;
		new_t = tau::get(tau::ref, { sym, tau::get_num(num_ref_args), tau::get_num(size_offsets) });
	}
	DBG(LOG_TRACE << "canonize/t -> new_t:\n"
		<< LOG_FM_TREE(t) << " -> " << LOG_FM_TREE(new_t);)
	return new_t;
};

template<NodeType node>
auto is_top_level_bf = [](tref parent) -> bool {
	using tau = tree<node>;
	if (parent == nullptr) return true;
	auto nt = tau::get(parent).get_type();
	switch (nt) {
		// bf logical connectives
		case tau::bf_and: case tau::bf_or: case tau::bf_neg:
		case tau::bf_xor:
		// bv ops
		case tau::bf_nor: case tau::bf_nand: case tau::bf_xnor:
		case tau::bf_add: case tau::bf_sub: case tau::bf_mul:
		case tau::bf_div: case tau::bf_mod: case tau::bf_shr:
		case tau::bf_shl:
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

template<NodeType node>
tref update_default(tref n, subtree_map<node, tref>& changes) {
	using tau = tree<node>;
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
		/*DBG(LOG_TRACE << "infer_ba_types/on_leave/update_default<node>//n -> new_n:\n"
			<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(new_n);)*/
		changes.insert_or_assign(n, new_n);
	}
	return (changes.find(n) != changes.end()) ? changes[n] : n;
};

template<NodeType node>
tref retype(tref n, const size_t new_type) {
	using tau = tree<node>;
	auto t = tau::get(n);
	auto n_type = t.get_type();
	if (tau::get(n).has_child())
		return tau::get_typed(n_type, t.child(0), new_type);
	else
		return tau::get_typed(n_type, new_type);
};

template<NodeType node>
auto update_bf_ctes = [](type_scoped_resolver<node>& resolver, tref n, const std::map<tref, size_t, subtree_less<node>>& types) -> tref {
	using tau = tree<node>;
	subtree_map<node, tref> changes;
	auto using_hooks = tau::use_hooks;
	auto error = false;

	auto update = [&](tref n) -> bool {
		// DBG(LOG_TRACE <<"infer_ba_types/update_bf_ctes/tau_use_hooks: " << tau::use_hooks << "\n";)
		const auto t = tau::get(n);
		size_t nt = t.get_type();
		switch (nt) {
			case tau::bf_t: case tau::bf_f: {
				// If we have no type information for the element we do nothing
				tref canonized = canonize<node>(n);
				if (!types.contains(canonized)) break;
				// If the bf_t/bf_f is not typed
				tref new_n;
				if (get_type_id<node>(n) == untyped_type_id<node>()) {
					// We type it according to the inferred type or tau
					size_t type = (types.at(canonized) == untyped_type_id<node>())
						? tau_type_id<node>()
						: types.at(canonized);
					if (resolver.assign(canonized, type) == false) {
						LOG_ERROR << "Conflicting type information for  "
							<< LOG_FM(n) << ", expected "
							<< ba_types<node>::name(types.at(canonized)) << "\n";
						return error = true, false;
					}
					new_n = retype<node>(n, type);
				} else {
					// Otherwise, we remove type children if any
					new_n = retype<node>(n, get_type_id<node>(n));
				}
				if (new_n != n) {
					DBG(LOG_TRACE << "infer_ba_types/update_bf_ctes/update/bf_t-bf_f.../n -> new_n:\n"
						<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(new_n);)
					changes.insert_or_assign(n, new_n);
				}
				break;
			}
			default: {
				// We transform the node according to the transformation of
				// its children
				update_default<node>(n, changes);
				break;
			}
		}
		return !error;
	};

	post_order<node>(n).search(update);
	if (error) return tau::use_hooks = using_hooks, nullptr;
	if (changes.find(n) != changes.end()) {
		DBG(LOG_TRACE << "infer_ba_types/update_bf_ctes/n -> changes[n]:\n"
			<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(changes[n]);)
		return changes[n];
	}
	return n;
};

template<NodeType node>
auto parse_ba_constants = [](tref n, const std::map<tref, size_t, subtree_less<node>>& types) -> tref {
	using tau = tree<node>;
	subtree_map<node, tref> changes;
	auto error = false;

	auto update = [&](tref n) -> bool {
		// DBG(LOG_TRACE <<"infer_ba_types/parse_ba_constants/tau_use_hooks: " << tau::use_hooks << "\n";)
		const auto t = tau::get(n);
		size_t nt = t.get_type();
		switch (nt) {
			case tau::ba_constant: {
				// If we have no type information for the bf constant we
				// rise an error as we should have at least untyped info
				tref canonized = canonize<node>(n);
				if (!types.contains(canonized)) {
					LOG_ERROR << "No detected type for bf constant in "
						<< LOG_FM(n) << "\n";
					return error = true, false;
				}
				// We get the type info or use the default (tau) if untyped
				size_t type = (types.at(canonized) == untyped_type_id<node>())
					? tau_type_id<node>()
					: types.at(canonized);
				// We parse the constant
				auto new_n = tau::get_ba_constant_from_source(t.child_data(), type);
				if (new_n == nullptr) {
					LOG_ERROR << "Could not parse bf constant in "
						<< LOG_FM(n) << " with type "
						<< ba_types<node>::name(type) << "\n";
					return error = true, false;
				}
				changes.insert_or_assign(n, new_n);
				DBG(LOG_TRACE << "infer_ba_types/parse_ba_constants/update/ba_constant/n -> new_n:\n"
					<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(new_n);)
				break;
			}
			default: {
				// We transform the node according to the transformation of
				// its children
				update_default<node>(n, changes);
				break;
			}
		}
		return !error;
	};

	post_order<node>(n).search(update);
	if (error) return nullptr;
	if (changes.find(n) != changes.end()) {
		DBG(LOG_TRACE << "infer_ba_types/parse_ba_constants/n -> changes[n]:\n"
			<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(changes[n]);)
		return changes[n];
	}
	return n;
};

template<NodeType node>
auto get_scoped_elements = [](type_scoped_resolver<node>& resolver, size_t element_type) {
	using tau = tree<node>;
	std::map<tref, size_t, subtree_less<node>> elements;
	for(auto [e, resolved_type] : resolver.current_kinds())
		if (tau::get(e).is(element_type)) elements[e] = resolved_type;
#ifdef DEBUG
	LOG_TRACE << "infer_ba_types/on_leave/get_scoped_elements/elements:"
		<< " type = " << (element_type == tau::variable ? "variable" :
			(element_type == tau::ba_constant ? "ba_constant" : "unknown"))
		<< " scope = " <<resolver.scopes_.back() << "\n";
	for (auto [e, t] : elements)
		LOG_TRACE << "\t" << LOG_FM(e) << " : "
			<< ba_types<node>::name(t) << "\n";
	LOG_TRACE << "infer_ba_types/on_leave/get_scoped_elements/uf:\n";
	LOG_TRACE << resolver.dump_to_str();
#endif // DEBUG
	return elements;
};

/*template<NodeType node>
tref update_refs(type_scoped_resolver<node>& resolver, tref n, const std::map<tref, size_t, subtree_less<node>>& types) {
	using tau = tree<node>;
	subtree_map<node, tref> changes;
	auto error = false;

	auto update = [&](tref n) -> bool {
		DBG(LOG_TRACE <<"infer_ba_types/update_refs/tau_use_hooks: " << tau::use_hooks << "\n";)
		if (error) return false;
		const auto t = tau::get(n);
		size_t nt = t.get_type();
		switch (nt) {
			case tau::ref: {
				// If we have no type information for the element we do nothing
				tref canonized = canonize<node>(n);
				if (!types.contains(canonized)) break;
				// If the variable is not typed
				tref new_n;
				if (get_type_id<node>(n) == untyped_type_id<node>()) {
					// We type it according to the inferred type or default
					size_t type = (types.at(canonized) == untyped_type_id<node>())
						? tau_type_id<node>()
						: types.at(canonized);

					if (resolver.assign(canonized, type) == false) {
						LOG_ERROR << "Conflicting type information for ref "
							<< LOG_FM(n) << ", expected "
							<< ba_types<node>::name(types.at(canonized)) << "\n";
						return error = true, false;
					}
					new_n = retype<node>(n, type);
				} else {
					// Otherwise, we remove type children if any
					new_n = retype<node>(n, get_type_id<node>(n));
				}
				if (new_n != n) {
					DBG(LOG_TRACE << "infer_ba_types/update_refs/update/ref.../n -> new_n:\n"
						<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(new_n);)
					changes.insert_or_assign(n, new_n);
				}
				break;
			}
			default: {
				// We transform the node according to the transformation of
				// its children
				update_default<node>(n, changes);
				break;
			}
		}
		return !error;
	};

	post_order<node>(n).search(update);
	if (error) return nullptr;
	if (changes.find(n) != changes.end()) {
		DBG(LOG_TRACE << "infer_ba_types/update_refs/n -> changes[n]:\n"
			<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(changes[n]);)
		return changes[n];
	}
	return n;
};*/

template<NodeType node>
tref update_variables(type_scoped_resolver<node>& resolver, tref n, const std::map<tref, size_t, subtree_less<node>>& types) {
	using tau = tree<node>;
	subtree_map<node, tref> changes;
	auto error = false;

	auto update = [&](tref n) -> bool {
		// DBG(LOG_TRACE <<"infer_ba_types/update_variables/tau_use_hooks: " << tau::use_hooks << "\n";)
		if (error) return false;
		const auto t = tau::get(n);
		size_t nt = t.get_type();
		switch (nt) {
			case tau::variable: {
				// If we have no type information for the element we do nothing
				tref canonized = canonize<node>(n);
				if (!types.contains(canonized)) break;
				// If the variable is not typed
				tref new_n;
				if (get_type_id<node>(n) == untyped_type_id<node>()) {
					// We type it according to the inferred type or default
					size_t type = (types.at(canonized) == untyped_type_id<node>())
						? tau_type_id<node>()
						: types.at(canonized);

					if (resolver.assign(canonized, type) == false) {
						LOG_ERROR << "Conflicting type information for variable "
							<< LOG_FM(n) << ", expected "
							<< ba_types<node>::name(types.at(canonized)) << "\n";
						return error = true, false;
					}
					new_n = retype<node>(n, type);
				} else {
					// Otherwise, we remove type children if any
					new_n = retype<node>(n, get_type_id<node>(n));
				}
				if (new_n != n) {
					DBG(LOG_TRACE << "infer_ba_types/update_variables/update/variable.../n -> new_n:\n"
						<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(new_n);)
					changes.insert_or_assign(n, new_n);
				}
				break;
			}
			default: {
				// We transform the node according to the transformation of
				// its children
				update_default<node>(n, changes);
				break;
			}
		}
		return !error;
	};

	post_order<node>(n).search(update);
	if (error) return nullptr;
	if (changes.find(n) != changes.end()) {
		DBG(LOG_TRACE << "infer_ba_types/update_variables/n -> changes[n]:\n"
			<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(changes[n]);)
		return changes[n];
	}
	return n;
};

template<NodeType node>
auto bv_defaulting = [](tref n) -> tref {
	using tau = tree<node>;
	subtree_map<node, tref> changes;

	auto update = [&](tref n) -> bool {
		// DBG(LOG_TRACE <<"infer_ba_types/defaulting_bv/tau_use_hooks: " << tau::use_hooks << "\n";)
		auto new_n = update_default<node>(n, changes);
		auto t = tau::get(new_n);
		if (t.get_ba_type() == get_ba_type_id<node>(bv_base_type<node>())) {
			auto chs = t.get_children();
			new_n = tau::get_raw(t.value.ba_retype(bv_type_id<node>()), chs.data(), chs.size());
			DBG(LOG_TRACE << "infer_ba_types/bv_defaulting/n -> new_n:\n"
				<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(new_n);)
			changes.insert_or_assign(n, new_n);
		}
		return true;
	};

	post_order<node>(n).search(update);
	if (changes.find(n) != changes.end()) {
		DBG(LOG_TRACE << "infer_ba_types/defaulting_bv/n -> changes[n]:\n"
			<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(changes[n]);)
		return changes[n];
	}
	return n;
};

// Infers the types of variables and constants in the tree n. It assumes that
// the types of the scoped variables are known when closing the scope.
// If a variable or constant remains unassigned, it is assigned to tau.
// We assume that the types of the constants could also be propagated across
// scopes (in the future we will restrict it to equations)
// If conflicting type information is found, the function returns nullptr.
template <NodeType node>
std::pair<tref, subtree_map<node, size_t>> infer_ba_types(tref n, const subtree_map<node, size_t>& global_scope) {
	using tau = tree<node>;
	//using tt = tau::traverser;

	DBG(LOG_TRACE << "================================================";)
	DBG(LOG_TRACE << LOG_WARNING_COLOR << "infer_ba_types" << TC.CLEAR()
		<< " for: " << LOG_FM_DUMP(n);)

	// We restore the original value of use_hooks at the end of the function
	auto using_hooks = tau::use_hooks;
	tau::use_hooks = false;

	type_scoped_resolver<node> resolver;
	subtree_map<node, tref> transformed;
	bool error = false;

	auto is_offset = [] (tref n) {
		using tau = tree<node>;
		return is<node, tau::offset>(n);
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
			case tau::input_def: case tau::output_def: {
				// We just add the variables to the current (global) scope.
			}

			case tau::rec_relation: {
				// We open a new scope for the relation variables and constants.
				// We assume all scoped variables and constants are resolved when
				// closing the scope. Otherwise, we type then to the tau type.

				// We open the new scope
				resolver.open({});
				// We get all the typeable top nodes in the expression, which in
				// this case are only (sbf/tau) variables and constants.
				// We use the following predicate to avoid vist tyhe variables
				// inside of a offset (ref case).
				auto typeables = tau::get(n).select_top_until(is_typeable<node>, is_offset);
				// We infer the common type of all the typeables in the expression
				auto type = unify<node>(
					resolver.types_of(typeables),
					get_type_ids<node>(typeables),
					untyped_type_id<node>());
				// If no common type is found, we set error and stop traversal
				if (!type){
					LOG_ERROR << "Conflicting type information in rec. relation "
						<< LOG_FM(n) << "\n";
					return error = true, false;
				}
				DBG(LOG_TRACE << "infer_ba_types/on_enter/rec_relation.../type: "
					<< ba_types<node>::name(type.value())
					<< "\n";)
				// We add the variables and the constants to the current scope
				// and assign them the common type.
				trefs mergeables;
				for (tref typeable : typeables)	{
					tref canonized = canonize<node>(typeable);
					resolver.insert(canonized);
					resolver.assign(canonized, type.value());
					mergeables.push_back(canonized);
				}
				if (!resolver.merge(mergeables)) {
					LOG_ERROR << "Conflicting type information in rec. relation "
						<< LOG_FM(n) << "\n";
					return error = true, false;
				}
				// Anyway, we stop the traversal of children as we have already
				// processed all the typeables in the expression.
				DBG(LOG_TRACE << "infer_ba_types/on_enter/rec_relation/resolver:\n";)
				DBG(LOG_TRACE << resolver.dump_to_str();)
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
					var_list[canonize<node>(var)] =  get_type_id<node>(var);
				}
				resolver.open(var_list);
				// We continue the traversal of children
				DBG(LOG_TRACE << "infer_ba_types/on_enter/wff_all.../resolver:\n";)
				DBG(LOG_TRACE << resolver.dump_to_str();)
				break;
			}
			case tau::bf: {
				// If bf is top level we treat it as a bf equation (we
				// FALLTHROUGH). Otherwise we don't traverse its children.
				if (!is_top_level_bf<node>(parent)) return false;
				// We get all the typeable top nodes in the expression, which in
				// this case are only (sbf/tau) variables and constants.
				auto typeables = tau::get(n).select_top(is_typeable<node>);
				// We infer the common type of all the typeables in the expression
				auto type = unify<node>(
					resolver.types_of(typeables),
					get_type_ids<node>(typeables),
					untyped_type_id<node>());
				// If no common type is found, we set error and stop traversal
				if (!type){
					LOG_ERROR << "Conflicting type information in bf "
						<< LOG_FM(n) << "\n";
					return error = true, false;
				}
				DBG(LOG_TRACE << "infer_ba_types/on_enter/bf.../type: "
					<< ba_types<node>::name(type.value())
					<< "\n";)
				// We add the variables and the constants to the current scope
				// and assign them the common type.
				trefs mergeables;
				for (tref typeable : typeables)	{
					tref canonized = canonize<node>(typeable);
					resolver.insert(canonized);
					resolver.assign(canonized, type.value());
					mergeables.push_back(canonized);
				}
				if (!resolver.merge(mergeables)) {
					LOG_ERROR << "Conflicting type information in bf "
						<< LOG_FM(n) << "\n";
					return error = true, false;
				}
				// Anyway, we stop the traversal of children as we have already
				// processed all the typeables in the expression.
				DBG(LOG_TRACE << "infer_ba_types/on_enter/bf/resolver:\n";)
				DBG(LOG_TRACE << resolver.dump_to_str();)
				break;
			}
			case tau::bf_eq: case tau::bf_neq: case tau::bf_lteq: case tau::bf_nlteq:
			case tau::bf_gt: case tau::bf_ngt: case tau::bf_gteq: case tau::bf_ngteq:
			case tau::bf_lt: case tau::bf_nlt:
			case tau::bf_interval: {
				// We get all the (top) typeables in the expression and assign
				// then the type bv (and the subtype when available). We do the
				// same for the constants.

				// We get all the typeable top nodes in the expression, which in
				// this case are only (sbf/tau) variables and constants.
				auto typeables = tau::get(n).select_top(is_typeable<node>);
				// We infer the common type of all the typeables in the expression
				auto type = unify<node>(
					resolver.types_of(typeables),
					get_type_ids<node>(typeables),
					untyped_type_id<node>());
				// If no common type is found, we set error and stop traversal
				if (!type){
					LOG_ERROR << "Conflicting type information in bf equation "
						<< LOG_FM(n) << "\n";
					return error = true, false;
				}
				DBG(LOG_TRACE << "infer_ba_types/on_enter/bf_eq.../type: "
					<< ba_types<node>::name(type.value())
					<< "\n";)
				// We add the variables and the constants to the current scope
				// and assign them the common type.
				trefs mergeables;
				subtree_map<node, size_t> constants;
				for (tref typeable : typeables)	{
					tref canonized = canonize<node>(typeable);
					if (is<node, tau::ba_constant>(canonized) || is<node, tau::bf_f>(canonized) || is<node, tau::bf_t>(canonized)) {
						mergeables.push_back(canonized);
						constants.emplace(canonized, type.value());
						continue;
					}
					// We only add variables to the current scope
					// Constants will be added to an inner scope
					resolver.insert(canonized);
					resolver.assign(canonized, type.value());
					mergeables.push_back(canonized);
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
				DBG(LOG_TRACE << "infer_ba_types/on_enter/bf_eq.../resolver:\n";)
				DBG(LOG_TRACE << resolver.dump_to_str();)
				break;
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
		// Helper lambdas

		DBG(LOG_TRACE << "infer_ba_types/on_leave/n:\n"
			<< LOG_FM_TREE(n);)
		// Stop traversal on error
		if (error) return;
		// Get the node type
		const auto& t = tau::get(n);
		size_t nt = t.get_type();
		// Depoending on the node type...
		switch (nt) {
			case tau::wff_all: case tau::wff_ex: case tau::rec_relation: {
				tref new_n = update_default<node>(n, transformed);
				auto scoped_var_types = get_scoped_elements<node>(resolver, tau::variable);
				if(auto updated = update_variables<node>(resolver, new_n, scoped_var_types); updated != new_n) {
					DBG(LOG_TRACE << "infer_ba_types/on_leave/wff_all.../n -> updated:\n"
						<< LOG_FM_TREE(new_n) << " -> " << LOG_FM_TREE(updated);)
						transformed.insert_or_assign(n, updated);
					}
				resolver.close();
				DBG(LOG_TRACE << "infer_ba_types/on_leave/wff_all.../resolver:\n";)
				DBG(LOG_TRACE << resolver.dump_to_str();)
				return;
			}
			case tau::bf_eq: case tau::bf_neq: case tau::bf_lteq: case tau::bf_nlteq:
			case tau::bf_gt: case tau::bf_ngt: case tau::bf_gteq: case tau::bf_ngteq:
			case tau::bf_lt: case tau::bf_nlt:
			case tau::bf_interval: {
				auto scoped_ba_ctes_types = get_scoped_elements<node>(resolver, tau::ba_constant);
				auto updated_ba_ctes = parse_ba_constants<node>(n, scoped_ba_ctes_types);
				if(updated_ba_ctes == nullptr) { error = true; return; }
				if(updated_ba_ctes != n) {
					DBG(LOG_TRACE << "infer_ba_types/on_leave/bf_eq.../n -> updated:\n"
						<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(updated_ba_ctes);)
					transformed.insert_or_assign(n, updated_ba_ctes);
				}
				auto scoped_bf_t_types = get_scoped_elements<node>(resolver, tau::bf_t);
				auto updated_bf_t_ctes = update_bf_ctes<node>(resolver, updated_ba_ctes, scoped_bf_t_types);
				if(updated_bf_t_ctes == nullptr) { error = true; return; }
				if(updated_bf_t_ctes != updated_ba_ctes) {
					DBG(LOG_TRACE << "infer_ba_types/on_leave/bf_eq.../n -> updated:\n"
						<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(updated_bf_t_ctes);)
					transformed.insert_or_assign(n, updated_bf_t_ctes);
				}
				auto scoped_bf_f_types = get_scoped_elements<node>(resolver, tau::bf_f);
				auto updated_bf_f_ctes = update_bf_ctes<node>(resolver, updated_bf_t_ctes, scoped_bf_f_types);
				if(updated_bf_f_ctes == nullptr) { error = true; return; }
				if(updated_bf_f_ctes != updated_bf_t_ctes) {
					DBG(LOG_TRACE << "infer_ba_types/on_leave/bf_eq.../n -> updated:\n"
						<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(updated_bf_f_ctes);)
					transformed.insert_or_assign(n, updated_bf_f_ctes);
				}
				resolver.close();
				DBG(LOG_TRACE << "infer_ba_types/on_leave/bf_eq.../resolver:\n";)
				DBG(LOG_TRACE << resolver.dump_to_str();)
				return;
			}
			case tau::bf: {
				if (is_top_level_bf<node>(parent)) {
					// We only process top-level bf nodes, as the inner ones
					// are part of bf equations already processed.
					auto scoped_bf_ctes_types = get_scoped_elements<node>(resolver, tau::ba_constant);
					auto parsed_ba_constants = parse_ba_constants<node>(n, scoped_bf_ctes_types);
					if (parsed_ba_constants == nullptr) { error = true; return; }
					if (parsed_ba_constants != n) {
						DBG(LOG_TRACE << "infer_ba_types/on_leave/bf.../n -> updated:\n"
							<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(parsed_ba_constants);)
						transformed.insert_or_assign(n, parsed_ba_constants);
					}
					auto scoped_var_types = get_scoped_elements<node>(resolver, tau::variable);
					auto updated_variables = update_variables<node>(resolver, parsed_ba_constants, scoped_var_types);
					if (updated_variables == nullptr) { error = true; return; }
					if (updated_variables != parsed_ba_constants) {
						DBG(LOG_TRACE << "infer_ba_types/on_leave/bf.../n -> updated:\n"
							<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(updated_variables);)
						transformed.insert_or_assign(n, updated_variables);
					}
					auto scoped_bf_t_types = get_scoped_elements<node>(resolver, tau::bf_t);
					auto updated_bf_t_ctes = update_bf_ctes<node>(resolver, updated_variables, scoped_bf_t_types);
					if(updated_bf_t_ctes == nullptr) { error = true; return; }
					if(updated_bf_t_ctes != updated_variables) {
						DBG(LOG_TRACE << "infer_ba_types/on_leave/bf_eq.../n -> updated:\n"
							<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(updated_bf_t_ctes);)
						transformed.insert_or_assign(n, updated_bf_t_ctes);
					}
					auto scoped_bf_f_types = get_scoped_elements<node>(resolver, tau::bf_f);
					auto updated_bf_f_ctes = update_bf_ctes<node>(resolver, updated_bf_t_ctes, scoped_bf_f_types);
					if(updated_bf_f_ctes == nullptr) { error = true; return; }
					if(updated_bf_f_ctes != updated_bf_t_ctes) {
						DBG(LOG_TRACE << "infer_ba_types/on_leave/bf_eq.../n -> updated:\n"
							<< LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(updated_bf_f_ctes);)
						transformed.insert_or_assign(n, updated_bf_f_ctes);
					}
				}
				return;
			}
			default: {
				tref new_n = update_default<node>(n, transformed);
				// For the root node, we type untyped variables with tau.
				if (!parent) {
					auto scoped_var_types = get_scoped_elements<node>(resolver, tau::variable);
					new_n = update_variables<node>(resolver, new_n, scoped_var_types);
					if (new_n == nullptr) { error = true; return; }
				}
				if (new_n != n) {
					DBG(LOG_TRACE << "infer_ba_types/on_leave/default/n -> new_n:\n"
						 << LOG_FM_TREE(n) << " -> " << LOG_FM_TREE(new_n);)
					transformed.insert_or_assign(n, new_n);
				}
				DBG(LOG_TRACE << "infer_ba_types/on_leave/default/resolver:\n";)
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

	// Adding global_scope info to resolver
	for (auto [var, type] : global_scope) {
		auto untyped = canonize<node>(var);
		resolver.insert(untyped), resolver.assign(untyped, type);
	}
	// We visit the tree and return the transformed root if no error happened.
	// If an error happened we return nullptr.
	pre_order<node>(n).visit(on_enter, visit_outside_equations, on_leave, on_between);
	DBG(LOG_TRACE << "================================================";)

	if (error) return tau::use_hooks = using_hooks, std::pair<tref, subtree_map<node, size_t>>{ nullptr, subtree_map<node, size_t>{} };
	// We add to the transformed map the untyping of the bf_t's and the bf_f's.
	// ...some code here...
	tref new_n = transformed.contains(n) ? transformed[n] : n;
	new_n = bv_defaulting<node>(new_n);
	if (new_n == nullptr) return tau::use_hooks = using_hooks, std::pair<tref, subtree_map<node, size_t>>{ nullptr, subtree_map<node, size_t>{} };
	new_n = update_symbols<node>(new_n);
	if (new_n == nullptr) return tau::use_hooks = using_hooks, std::pair<tref, subtree_map<node, size_t>>{ nullptr, subtree_map<node, size_t>{} };
	auto n_global_scope = resolver.current_kinds();
	DBG(LOG_TRACE << LOG_WARNING_COLOR << "infer_ba_types" << TC.CLEAR()
		<< " of: " << LOG_FM(n) << " resulted into: " << LOG_FM_DUMP(new_n);)
	return std::pair<tref, subtree_map<node, size_t>>{ new_n, n_global_scope };
}

} // namespace idni::tau_lang
