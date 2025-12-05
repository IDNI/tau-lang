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
#include "type_scoped_resolver.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "inference"

namespace idni::tau_lang {

template<NodeType node>
tref canonize(tref t) {
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
}

template<NodeType node>
trefs canonize(trefs ts) {
	trefs new_ts;
	for (auto t : ts)
		new_ts.push_back(canonize<node>(t));
	return new_ts;
}

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


template <NodeType node>
std::map<size_t, trefs> get_typeables_by_type(tref n,
		const std::function<bool(tref)>& query = is_typeable<node>,
		const std::function<bool(tref)>& stop = is<node, tree<node>::offset>) {
	using tau = tree<node>;
	auto typeables = tau::get(n).select_top_until(query, stop);
	std::map<size_t, trefs> typeables_by_type;
	for (tref typeable : typeables) {
		auto type = get_type_id<node>(typeable);
		if (typeables_by_type.find(type) == typeables_by_type.end())
			typeables_by_type[type] = trefs{};
		auto canonized = canonize<node>(typeable);
		typeables_by_type[type].push_back(canonized);
	}
	return typeables_by_type;
}

template <NodeType node>
std::map<size_t, trefs> get_typeables_by_type(tref n,
		const std::initializer_list<size_t>& types) {
	return get_typeable_by_type<node>(n, is<node>(types));
}

template <NodeType node>
std::map<size_t, trefs> get_typeable_of_type(tref n,
		const size_t type) {
	return get_typeable_by_type<node>(n, is<node>(type));
}

template <NodeType node>
std::optional<std::map<size_t, subtree_map<node, size_t>>> get_typeable_type_ids_by_type(tref n, 
		const std::function<bool(tref)>& query = is_typeable<node>,
		const std::function<bool(tref)>& stop = is<node, tree<node>::offset>) {
	using tau = tree<node>;

	auto typeables = tau::get(n).select_top_until(query, stop);
	std::map<size_t, subtree_map<node, size_t>> typeable_type_ids_by_type;
	for (tref typeable : typeables) {
		auto canonized = canonize<node>(typeable);
		auto nt = tau::get(typeable).get_type();
		if (typeable_type_ids_by_type.find(nt) == typeable_type_ids_by_type.end())
			typeable_type_ids_by_type[nt] = subtree_map<node, size_t>();
		if (auto it = typeable_type_ids_by_type[nt].find(canonized); it !=
				typeable_type_ids_by_type[nt].end()) {
			if (auto type_id = unify<node>(it->second, get_type_id<node>(typeable)); type_id) {
				typeable_type_ids_by_type[nt][canonized] = type_id.value();
				continue;
			}
			return std::nullopt; // incompatible types
		}
		typeable_type_ids_by_type[nt][canonized] = get_type_id<node>(typeable);
	}
#ifdef DEBUG
	LOG_TRACE << "get_typeable_type_ids_by_type/typeable_type_ids_by_type:\n";
	for (auto [type, typeables] : typeable_type_ids_by_type) {
		LOG_TRACE << "\ttype: " << LOG_NT(type) << "\n";
		for (auto [t, tid] : typeables) {
			LOG_TRACE << "\t\t" << LOG_FM(t) << " : "
				<< ba_types<node>::name(tid) << "\n";
		}
	}
#endif // DEBUG
	return typeable_type_ids_by_type;
}

template <NodeType node>
std::optional<std::map<size_t, subtree_map<node, size_t>>> get_typeable_type_ids_by_type(tref n, const std::initializer_list<size_t>& types) {
	return get_typeable_type_ids_by_type<node>(n, is<node>(types));
}

template<NodeType node>
bool is_functional_relation(tref n) {
	using tau = tree<node>;
	auto t = tau::get(n);
	if (!is<node, tau::rec_relation>(n)) return false;
	// If the head is typed we have a functional relation.
	if (auto return_type = get_type_id<node>(t.child(0)); 
			return_type != untyped_type_id<node>() ) {
		return true;
	}
	// Otherwise, we have a predicate relation.
	return false;
}

// type all symbols according to their children's types
template<NodeType node>
tref update_ba_symbol(tref n) {
	using tau = tree<node>;
	// We have one child at least and we know that the types of the
	// children have already been updated and they are consistent.
	auto t = tau::get(n);
	auto chs = t.get_children();
	auto n_type = tau::get(chs[0]).get_ba_type();
	auto new_n = tau::get_raw(t.value.ba_retype(n_type), chs.data(), chs.size());
	return new_n;
}

// type all symbols according to their children's types
template<NodeType node>
tref update_bv_symbol(tref n) {
	using tau = tree<node>;
	// We have one child at least and we know that the types of the
	// children have already been updated and they are consistent.
	// We only need to check that the type is bv type family.
	auto t = tau::get(n)[0].get_ba_type();
	return is_bv_type_family<node>(t)
		? update_ba_symbol<node>(n)
		: nullptr;
}

template<NodeType node>
tref update_bf_constant(type_scoped_resolver<node>& resolver, tref n, const subtree_map<node, size_t>& types) {
	//using tau = tree<node>;
	// DBG(LOG_TRACE <<"infer_ba_types/update_bf_ctes/tau_use_hooks: " << tau::use_hooks << "\n";)
	// If we have no type information for the element we do nothing
	tref canonized = canonize<node>(n);
	if (!types.contains(canonized)) return nullptr;
	// If the bf_t/bf_f is not typed
	if (get_type_id<node>(n) == untyped_type_id<node>()) {
		// We type it according to the inferred type or tau
		size_t type = (types.at(canonized) == untyped_type_id<node>())
			? tau_type_id<node>()
			: types.at(canonized);
		if (resolver.assign(canonized, type) == false) return nullptr;
		return retype<node>(n, type);
	} 
	return untyped<node>(n);
}

template<NodeType node>
tref update_ba_constant([[maybe_unused]] type_scoped_resolver<node>& resolver, tref n, const subtree_map<node, size_t>& types) {
	using tau = tree<node>;

	// If we have no type information for the bf constant we
	// rise an error as we should have at least untyped info
	tref canonized = canonize<node>(n);
	if (!types.contains(canonized)) return nullptr;
	// We get the type info or use the default (tau) if untyped
	size_t type = (types.at(canonized) == untyped_type_id<node>())
		? tau_type_id<node>()
		: types.at(canonized);
	// We parse the constant
	return tau::get_ba_constant_from_source(tau::get(n).child_data(), type);
}

template<NodeType node>
tref update_ref(type_scoped_resolver<node>& resolver, tref n, const subtree_map<node, size_t>& types) {
	// If we have no type information for the element we do nothing
	tref canonized = canonize<node>(n);
	if (!types.contains(canonized)) return n;
	// If the variable is not typed
	if (get_type_id<node>(n) == untyped_type_id<node>()) {
		// We type it according to the inferred type or default
		size_t type = (types.at(canonized) == untyped_type_id<node>())
			? tau_type_id<node>()
			: types.at(canonized);

		if (resolver.assign(canonized, type) == false) return nullptr;
		return retype<node>(n, type);
	}
	return untyped<node>(n);
}

template<NodeType node>
tref update_variable(type_scoped_resolver<node>& resolver, tref n, const subtree_map<node, size_t>& types) {
	using tau = tree<node>;

	// If we have no type information for the element we do nothing
	tref canonized = canonize<node>(n);
	if (!types.contains(canonized)) return n;
	// If the variable is not typed
	if (tau::get(n).get_ba_type() == untyped_type_id<node>()) {
		// We type it according to the inferred type or default
		size_t type = (types.at(canonized) == untyped_type_id<node>())
			? tau_type_id<node>()
			: types.at(canonized);

		if (resolver.assign(canonized, type) == false) return nullptr;
		return retype<node>(n, type);
	} 
	return untyped<node>(n);
}

template<NodeType node>
tref update_functional_rr(type_scoped_resolver<node>& resolver, tref n) {
	using tau = tree<node>;
	// First we update the ba_constant, the ariables and bf_t/bf_f in the rr and
	// close the body scope
	auto updated = update<node>(resolver, n, { tau::ba_constant, tau::bf_t, tau::bf_f, tau::variable });
	if (updated == nullptr) return nullptr;
	resolver.close();
	// Finally, we get the new body and reference and create a new rr
	// assuming the type of the head
	auto head = untype<node>(tau::get(updated).child(0));
	auto body = untype<node>(tau::get(updated).child(1));
	auto type = tau::get(updated)[0].get_ba_type();
	auto new_head = 
		tau::get_typed(tau::bf, 
			tau::get_typed(tau::bf_ref, 
				head, type), type);
	auto new_body = is<node, tau::ref>(body)
		? tau::get_typed(tau::bf, 
			tau::get_typed(tau::bf_ref, 
				body, type), type)
		: tau::get(updated).child(1);
	return tau::get(tau::rec_relation, { new_head, new_body });
}

template<NodeType node>
tref update_predicate_rr(type_scoped_resolver<node>& resolver, tref n) {
	using tau = tree<node>;
	// First we update the variables in the rr head and body and close 
	// the rr scope
	auto updated = update<node>(resolver, n, { tau::variable });
	if (updated == nullptr) return nullptr;
	resolver.close();
	// Finally, we get the new body and reference and create a new rr
	// assuming boolean type
	auto head = tau::get(updated).child(0);
	auto body = tau::get(updated).child(1);
	auto new_head = 
		tau::get(tau::wff, 
			tau::get(tau::wff_ref, 
				head));
	auto new_body = is<node, tau::ref>(body)
		? tau::get(tau::wff, 
			tau::get(tau::wff_ref, 
				body))
		: tau::get(updated).child(1);
	return tau::get(tau::rec_relation, { new_head, new_body });
}

template<NodeType node>
tref update_default(tref n, subtree_map<node, tref>& changes) {
	using tau = tree<node>;

	trefs ch;
	for (tref c : tau::get(n).children()) {
		if (changes.find(c) != changes.end())
			ch.push_back(changes[c]);
		else ch.push_back(c);
	}

	if (auto new_n = tau::get_raw(tau::get(n).value, ch.data(), ch.size()); new_n != n)
		changes.insert_or_assign(n, new_n);
	return (changes.find(n) != changes.end()) ? changes[n] : n;
};

template<NodeType node>
tref update(type_scoped_resolver<node>& resolver, tref n, std::initializer_list<size_t> types_to_update) {
	using tau = tree<node>;

	subtree_map<node, tref> changes;

	bool error = false;
	auto types = resolver.current_types();
	auto to_be_updated = std::set<size_t>(types_to_update.begin(), types_to_update.end());
	
	auto f = [&](tref n) -> bool {
		auto t = tau::get(n);
		size_t nt = t.get_type();
	
		if (error) return false;
		switch (nt) {
			/*case tau::ref: {
				if (!to_be_updated.contains(nt)) break;
				auto nn = update_default<node>(n, changes);
				if (auto updated = update_ref<node>(resolver, nn, types); updated) {
					DBG(LOG_TRACE << "infer_ba_types/update/" << LOG_NT(nt) << "/updated:\n"
						<< LOG_FM_TREE(updated);)
					if (updated != n) changes.insert_or_assign(n, updated);
				} else error = true; 
				break;
			}*/
			case tau::variable: {
				if (!to_be_updated.contains(nt)) break;
				if (auto updated = update_variable<node>(resolver, n, types); updated) {
					DBG(LOG_TRACE << "infer_ba_types/update/" << LOG_NT(nt) << "/updated:\n"
						<< LOG_FM_TREE(updated);)
					if (updated != n) changes.insert_or_assign(n, updated);
				} else error = true; 
				break;
			}
			case tau::ba_constant: {
				if (!to_be_updated.contains(nt)) break;
				if (auto updated = update_ba_constant<node>(resolver, n, types); updated) {
					DBG(LOG_TRACE << "infer_ba_types/update/" << LOG_NT(nt) << "/updated:\n"
						<< LOG_FM_TREE(updated);)
					if (updated != n) changes.insert_or_assign(n, updated);
				} else error = true; 
				break;
			}
			case tau::bf_t: case tau::bf_f: {
				if (!to_be_updated.contains(nt)) break;
				if (auto updated = update_bf_constant<node>(resolver, n, types); updated) {
					DBG(LOG_TRACE << "infer_ba_types/update/" << LOG_NT(nt) << "/updated:\n"
						<< LOG_FM_TREE(updated);)
					if (updated != n) changes.insert_or_assign(n, updated);
				} else error = true; 
				break;
			}
			case tau::bf:
			case tau::bf_interval: case tau::bf_eq: case tau::bf_neq:
			case tau::bf_lteq: case tau::bf_nlteq: case tau::bf_gt:
			case tau::bf_ngt: case tau::bf_gteq: case tau::bf_ngteq:
			case tau::bf_lt: case tau::bf_nlt: case tau::bf_or:
			case tau::bf_xor: case tau::bf_and: case tau::bf_neg: {
				// all types allowed
				auto nn = update_default<node>(n, changes);
				if(!to_be_updated.contains(nt) && !to_be_updated.contains(tau::typeable_symbol)) {
					if (nn != n) changes.insert_or_assign(n, nn);
				} else if (auto updated = update_ba_symbol<node>(nn); n != updated)
					changes.insert_or_assign(n, updated);
				break;
			}
			case tau::bf_add: case tau::bf_sub: case tau::bf_mul:
			case tau::bf_div: case tau::bf_mod: case tau::bf_shr:
			case tau::bf_shl: case tau::bf_xnor: case tau::bf_nand:
			case tau::bf_nor: {
				// only bv types allowed
				auto nn = update_default<node>(n, changes);
				if(!to_be_updated.contains(nt) && !to_be_updated.contains(tau::typeable_symbol)) {
					if (nn != n) changes.insert_or_assign(n, nn);
				} else if (auto updated = update_bv_symbol<node>(nn); n != updated) {
					if (updated != nullptr)	changes.insert_or_assign(n, updated);
					else error = true;
				}
				break;
			}
			default: {
				if (auto updated = update_default<node>(n, changes); updated != n) {
					DBG(LOG_TRACE << "infer_ba_types/update/" << LOG_NT(nt) << "/updated:\n"
						<< LOG_FM_TREE(updated);)
					changes.insert_or_assign(n, updated);
				}
			}
		}

		if (error)
			LOG_ERROR << "Incompatible or absent type information in " << LOG_FM(n) << "\n";
		DBG(if (!error) LOG_TRACE << "infer_ba_types/update/" << LOG_NT(nt) << "/n -> new_n:\n"
				<< LOG_FM_TREE(n) << " -> "
				<< LOG_FM_TREE(changes.contains(n) ? changes[n] : n);)
		DBG(LOG_TRACE << "infer_ba_types/update/" << LOG_NT(nt) << "/resolver:\n"
				<< resolver.dump_to_str();)
		return !error;
	};
	
	post_order<node>(n).search(f);
	if (error) return nullptr;
	return changes.find(n) != changes.end() ? changes[n] : n;
}

// Infers the types of variables and constants in the tree n. It assumes that
// the types of the scoped variables are known when closing the scope.
// If a variable or constant remains unassigned, it is assigned to tau.
// We assume that the types of the constants could also be propagated across
// scopes (in the future we will restrict it to equations)
// If conflicting type information is found, the function returns nullptr.
template <NodeType node>
std::pair<tref, subtree_map<node, size_t>> infer_ba_types(tref n, const subtree_map<node, size_t>& global_scope) {
	type_scoped_resolver<node> resolver;
	return infer_ba_types<node>(n, global_scope, resolver);
}

// This function version is introduced for debugging purposes as it allows
// to inspect the resolver state after the type inference.
template <NodeType node>
std::pair<tref, subtree_map<node, size_t>> infer_ba_types(tref n, const subtree_map<node, size_t>& global_scope, 
		type_scoped_resolver<node>& resolver) {
	using tau = tree<node>;
	//using tt = tau::traverser;

	DBG(LOG_TRACE << "================================================";)
	DBG(LOG_TRACE << LOG_WARNING_COLOR << "infer_ba_types" << TC.CLEAR()
		<< " for: " << LOG_FM_DUMP(n);)

	// We restore the original value of use_hooks at the end of the function
	auto using_hooks = tau::use_hooks;
	tau::use_hooks = false;

	subtree_map<node, tref> transformed;
	bool error = false;
	
	// We gather info about types and scopes while entering nodes
	auto on_enter = [&](tref n, tref parent) {
		DBG(assert(n != nullptr);)
		
		// Stop traversal on error
		if (error) return false;

		// if set to true, we skip traversing the children
		bool skip = false;

		// Get the node type
		auto t = tau::get(n);
		size_t nt = t.get_type();
		// Depoending on the node type...
		switch (nt) {
			case tau::input_def: case tau::output_def: {
				// Samples:
				// i1 : tau = in console
				// o1 : bv[8] = out console
				// ...

				// We just add the variables to the current (global) scope.
			}
			case tau::rec_relation: {
				// Samples:
				// p(x):sbf := x'    (F) head is typed, body is bf
				// p(x):sbf := q(x)  (F) head is typed, body is ref
				// p(x) := x' ^1:sbf (E) head is untyped, body is bf
				// p(x:sbf) := q(x)  (P) head untyped, body is a ref
				// p(x:sbf) := x = 0 (P) head untyped, body is a wff
				// p(x) := x = 0     (E) x is untyped
				// p(x) := x = 1:sbf (P) x is sbf typed in body
				// ...
				// p(x) := q(x):sbf  (F) x is sbf typed in body
				// p(x, y) := x + y  (F) x and y are typed in body

				// We open a new scope with all the vars in the header.
				auto header_type = get_type_id<node>(t[0].get());
				auto arguments = get_typeable_type_ids_by_type<node>(t[0].get(), { tau::variable });
				if (!arguments) { error = true; break; } // Incompatible types
				auto arguments_map = arguments.value();
				if (is_functional_relation<node>(n)) {
					auto unified = unify<node>(arguments_map, header_type);
					if (!unified) {	error = true; break; } // Incompatible types
					auto inferred_type = unified.value();
					// We gather all the data about the body typeables
					auto rec_type_ids = get_typeable_type_ids_by_type<node>(n, { 
						tau::variable, tau::ba_constant, tau::bf_t, tau::bf_f });
					if (!rec_type_ids) { error = true; break; } // Incompatible types
					auto rec_type_ids_map = rec_type_ids.value();
					// We create a new scope with all the inferable typeables
					// taking into account that they should have the same type.
					if (!open_same_type<node>(resolver, rec_type_ids_map, inferred_type)) { error = true; break; }
					// We merge all the header and the body typeables together.
					if (!merge<node>(resolver, rec_type_ids_map)) { error = true; break; }
					break;
				}
				// Otherwise, we have a predicate relation. We create a new scope
				// with all the variables in the header.
				if (!open<node>(resolver, { arguments_map[tau::variable] })) { error = true; break; }
				break;
			}
			case tau::wff_all: case tau::wff_ex: {
				// Samples:
				// all x:sbf x = 0
				// ex x x = 0:sbf
				//...

				// We open a new scope, we get all the quantified variables,
				// add them to the scope (with the given type if any).
				auto quantified_vars = get_typeable_type_ids_by_type<node>(t.child(0), { tau::variable });
				if (!quantified_vars) { error = true; break; } // Incompatible types
				auto quantified_vars_map = quantified_vars.value();
				open<node>(resolver, quantified_vars_map);
				break;
			}
			case tau::bf: {
				// Be aware that if we have a command with unconnected bf's
				// this would fail.
				//
				// If bf is not a top level one, it must have been treated somewhere else.
				if (!is_top_level_bf<node>(parent)) { skip = true; break; }
				// Otherwise we have to treat it as a global scope
				auto typeables = get_typeable_type_ids_by_type<node>(n, { 
					tau::variable, tau::ba_constant, tau::bf_t, tau::bf_f });
				if (!typeables) { error = true; break; } // Incompatible types
				auto typeables_map = typeables.value();
				if (!insert<node>(resolver, { 
					typeables_map[tau::variable],
					typeables_map[tau::ba_constant], 
					typeables_map[tau::bf_t], 
					typeables_map[tau::bf_f] })) { error = true; break; }
				if (!merge<node>(resolver, typeables_map)) { error = true; break; }
				break;
			}
			case tau::ref: {
				// We skip the traversal if the parent is not a wff_ref or 
				// is a functional ref as are treated elsewhere.
				if (is_functional_relation<node>(parent) /* && 
						tau::get(parent).child(0) == n*/ ) { 
					skip = true; break;
				}
				// Otherwise, we continue the traversal so that we can treat
				// the ref_args as above.
				break;
			}
			case tau::ref_arg:
				// This case happens when we are in a wff_ref node in a rec 
				// relation (thanks to the above check).We must treat 
				// the argument as an atomic formula 
				resolver.open();
				[[fallthrough]];
			case tau::bf_eq: case tau::bf_neq: case tau::bf_lteq: case tau::bf_nlteq:
			case tau::bf_gt: case tau::bf_ngt: case tau::bf_gteq: case tau::bf_ngteq:
			case tau::bf_lt: case tau::bf_nlt:
			case tau::bf_interval: {
				auto typeables = get_typeable_type_ids_by_type<node>(n, { 
					tau::variable, tau::ba_constant, tau::bf_t, tau::bf_f });
				if (!typeables) { error = true; break; } // Incompatible types
				auto typeables_map = typeables.value();
				if (!open<node>(resolver, { 
						typeables_map[tau::ba_constant], 
						typeables_map[tau::bf_t], 
						typeables_map[tau::bf_f] })) { error = true; break; }
				if (!insert<node>(resolver, { 
						typeables_map[tau::variable] })) { error = true; break; }
				if (!merge<node>(resolver, typeables_map)) { error = true; break; }
				break;
			}
			default:
			// Otherwise, we continue the traversal of children
			// without doing anything special in this node.
			break;
		}
		if (error) LOG_ERROR << "Incompatible or absent type information in " << LOG_FM(n) << "\n";
		DBG(LOG_TRACE << "infer_ba_types/on_enter/" << LOG_NT(nt) << "/resolver:\n"
			<< resolver.dump_to_str();)
		// Stop traversal on error
		return !error && !skip;
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

		// Stop traversal on error
		if (error) return;
		// Get the node type
		const auto& t = tau::get(n);
		size_t nt = t.get_type();
		// Depoending on the node type...
		switch (nt) {
			case tau::input_def: case tau::output_def: {
				// Resolve everything in the rec relation and close the scope
				return;
			}
			case tau::rec_relation: {
				// We need to adjust the wrapping around refs in the body and
				// the header accordingly.
				auto new_n = update_default<node>(n, transformed);
				auto updated = is_functional_relation<node>(new_n)
					? update_functional_rr<node>(resolver, new_n)
					: update_predicate_rr<node>(resolver, new_n);
				if(updated == nullptr) { error = true; break; }
				if(updated != new_n) transformed.insert_or_assign(n, updated);
				resolver.close();
				break;
			}
			case tau::wff_all: case tau::wff_ex: {
				tref new_n = update_default<node>(n, transformed);
				auto updated = update<node>(resolver, new_n, { tau::variable});
				if(updated == nullptr) { error = true; break; }
				if(updated != new_n) transformed.insert_or_assign(n, updated);
				resolver.close();
				break;
			}
			case tau::ref_arg:
				resolver.close();
				[[fallthrough]];
			case tau::bf_eq: case tau::bf_neq: case tau::bf_lteq: case tau::bf_nlteq:
			case tau::bf_gt: case tau::bf_ngt: case tau::bf_gteq: case tau::bf_ngteq:
			case tau::bf_lt: case tau::bf_nlt:
			case tau::bf_interval: {
				tref new_n = update_default<node>(n, transformed);
				auto updated = update<node>(resolver, new_n, { tau::ba_constant, tau::bf_t, tau::bf_f });
				if(updated == nullptr) { error = true; break; }
				if(updated != new_n) transformed.insert_or_assign(n, updated);
				resolver.close();
				break;
			}
			case tau::bf: {
				tref new_n = update_default<node>(n, transformed);
				if (is_top_level_bf<node>(parent)) {
					auto updated = update<node>(resolver, new_n, { tau::variable, tau::ba_constant, tau::bf_t, tau::bf_f });
					if(updated == nullptr) { error = true; break; }
					if(updated != new_n) transformed.insert_or_assign(n, updated);
				} else {
					if (new_n != n) transformed.insert_or_assign(n, new_n);
				}
				break;
			}
			default: {
				tref new_n = update_default<node>(n, transformed);
				// For the root node, we type untyped variables with tau.
				if (!parent) {
					new_n = update<node>(resolver, new_n, { tau::variable });
					if (new_n == nullptr) { error = true; break; }
				}
				if (new_n != n) transformed.insert_or_assign(n, new_n);	
				break;
			}
		}
		if (error) LOG_ERROR << "Incompatible or absent type information in " << LOG_FM(n) << "\n";
		DBG(if (!error) LOG_TRACE << "infer_ba_types/on_leave/" << LOG_NT(nt) << "/n -> new_n:\n"
			<< LOG_FM_TREE(n) << " -> "
			<< LOG_FM_TREE(transformed.contains(n) ? transformed[n] : n);)
		DBG(LOG_TRACE << "infer_ba_types/on_leave/" << LOG_NT(nt) << "/resolver:\n"
			<< resolver.dump_to_str();)
		return;
	};

	// All nodes
	// TODO (LOW) replace by all predicate
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
		resolver.insert(untyped);
		resolver.assign(untyped, type);
	}
	// We visit the tree and return the transformed root if no error happened.
	// If an error happened we return nullptr.
	pre_order<node>(n).visit(on_enter, visit_outside_equations, on_leave, on_between);
	if (error) return tau::use_hooks = using_hooks, std::pair<tref, subtree_map<node, size_t>>{ nullptr, subtree_map<node, size_t>{} };
	// We add to the transformed map the untyping of the bf_t's and the bf_f's.
	// ...some code here...
	tref new_n = transformed.contains(n) ? transformed[n] : n;
	new_n = bv_defaulting<node>(new_n);
	if (new_n == nullptr) return tau::use_hooks = using_hooks, std::pair<tref, subtree_map<node, size_t>>{ nullptr, subtree_map<node, size_t>{} };
	new_n = update<node>(resolver, new_n, { tau::typeable_symbol });
	if (new_n == nullptr) return tau::use_hooks = using_hooks, std::pair<tref, subtree_map<node, size_t>>{ nullptr, subtree_map<node, size_t>{} };
	auto n_global_scope = resolver.current_types();
	DBG(LOG_TRACE << LOG_WARNING_COLOR << "infer_ba_types" << TC.CLEAR()
	<< " of: " << LOG_FM(n) << " resulted into: " << LOG_FM_DUMP(new_n);)
	
	DBG(LOG_TRACE << "================================================";)
	
	tau::use_hooks = using_hooks;
	return std::pair<tref, subtree_map<node, size_t>>{ new_n, n_global_scope };
}

} // namespace idni::tau_lang
