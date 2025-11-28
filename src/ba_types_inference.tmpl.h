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


/*template<NodeType node>
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
	};*/

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

/*template <NodeType node>
subtree_map<node, size_t> get_typeable_type_ids(tref n) {
	using tau = tree<node>;
	subtree_map<node, size_t> typeable_type_ids;
	auto typeables = tau::get(n).select_top_until(
		is_typeable<node>,
		is<node, tau::offset>);
	for (tref typeable : typeables) {
		auto canonized = canonize<node>(typeable);
		typeable_type_ids[canonized] = get_type_id<node>(typeable);
	}
	return typeable_type_ids;
}*/

template <NodeType node>
std::map<size_t, subtree_map<node, size_t>> get_typeable_type_ids_by_type(tref n, 
		const std::function<bool(tref)>& query = is_typeable<node>,
		const std::function<bool(tref)>& stop = is<node, tree<node>::offset>) {
	using tau = tree<node>;

	std::map<size_t, subtree_map<node, size_t>> typeable_type_ids;
	auto typeables = tau::get(n).select_top_until(query, stop);
	for (tref typeable : typeables) {
		auto canonized = canonize<node>(typeable);
		auto nt = tau::get(typeable).get_type();
		if (typeable_type_ids.find(nt) == typeable_type_ids.end())
			typeable_type_ids[nt] = subtree_map<node, size_t>();
		typeable_type_ids[nt][canonized] = get_type_id<node>(typeable);
	}
	return typeable_type_ids;
}
template <NodeType node>
std::map<size_t, subtree_map<node, size_t>> get_typeable_type_ids_by_type(tref n, const std::initializer_list<size_t>& types) {
	using tau = tree<node>;

	auto query = [&] (tref n) -> bool {
		for (auto type : types) {
			if (tau::get(n).is(type)) return true;
		}
		return false;
	};

	return get_typeable_type_ids_by_type<node>(n, query);
}

template<NodeType node>
bool insert(type_scoped_resolver<node>& resolver, const std::map<size_t, subtree_map<node, size_t>>& types) {
	for (auto [_, typeables] : types) {
		for (auto [t, type] : typeables) {
			resolver.insert(t);
			if (!resolver.assign(t, type)) return false;
		}
	}
	return true;
}

template<NodeType node>
bool insert(type_scoped_resolver<node>& resolver, const std::initializer_list<subtree_map<node, size_t>>& types) {
	for (auto typeables : types) {
		for (auto [t, type] : typeables) {
			resolver.insert(t);
			if (!resolver.assign(t, type)) return false;
		}
	}
	return true;
}

template<NodeType node>
bool open(type_scoped_resolver<node>& resolver, const std::map<size_t, subtree_map<node, size_t>>& types) {
	subtree_map<node, size_t> scoped;
	for (auto [_, typeables] : types)
		for (auto [t, type] : typeables)
			scoped[t] = type;
	resolver.open(scoped);
	return true;
}

template<NodeType node>
bool open(type_scoped_resolver<node>& resolver, const std::initializer_list<subtree_map<node, size_t>>& types) {
	subtree_map<node, size_t> scoped;
	for (auto typeables : types)
		for (auto [t, type] : typeables)
			scoped[t] = type;
	resolver.open(scoped);
	return true;
}

template<NodeType node>
bool open_same_type(type_scoped_resolver<node>& resolver, const std::map<size_t, subtree_map<node, size_t>>& types) {
	subtree_map<node, size_t> scoped;
	auto inferred_type = untyped_type_id<node>();
	for (auto [_, typeables] : types) {
		for (auto [t, type] : typeables) {
			inferred_type = unify<node>(inferred_type, type);
			if (!inferred_type) return false;
			scoped[t] = inferred_type;
		}
	}
	resolver.open(scoped);
	return true;
}

template<NodeType node>
bool open_same_type(type_scoped_resolver<node>& resolver, const std::initializer_list<subtree_map<node, size_t>>& types) {
	subtree_map<node, size_t> scoped;
	auto inferred_type = untyped_type_id<node>();
	for (auto typeables : types) {
		for (auto [t, type] : typeables) {
			if (auto new_inferred_type = unify<node>(inferred_type, type); new_inferred_type) {
				inferred_type = new_inferred_type.value();
			} else return false;
			scoped[t] = inferred_type;
		}
	}
	resolver.open(scoped);
	return true;
}

template<NodeType node>
bool merge(type_scoped_resolver<node>& resolver, const std::initializer_list<subtree_map<node, size_t>>& types) {
	trefs mergeables;
	for (auto typeables : types)
		for (auto [t, _] : typeables)
			mergeables.push_back(t);
	return resolver.merge(mergeables);
}


template<NodeType node>
bool merge(type_scoped_resolver<node>& resolver, const std::map<size_t, subtree_map<node, size_t>>& types) {
	trefs mergeables;
	for (auto [_, typeables] : types)
		for (auto [t, _] : typeables)
			mergeables.push_back(t);
	return resolver.merge(mergeables);
}

template<NodeType node>
bool is_functional_relation(tref n) {
	using tau = tree<node>;
	auto t = tau::get(n);
	// If the head is typed we have a functional relation.
	if (auto return_type = get_type_id<node>(t.child(0)); 
			return_type != untyped_type_id<node>() ) {
		return true;
	}
	// Otherwise, we have a predicate relation.
	return false;
}
	
template<NodeType node>
tref update_bf_constant(type_scoped_resolver<node>& resolver, tref n, const std::map<tref, size_t, subtree_less<node>>& types) {
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
	return retype<node>(n, get_type_id<node>(n));
}

template<NodeType node>
tref update_ba_constant([[maybe_unused]] type_scoped_resolver<node>& resolver, tref n, const std::map<tref, size_t, subtree_less<node>>& types) {
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
tref update_ref(type_scoped_resolver<node>& resolver, tref n, const std::map<tref, size_t, subtree_less<node>>& types) {
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
	return retype<node>(n, get_type_id<node>(n));
}

template<NodeType node>
tref update_variable(type_scoped_resolver<node>& resolver, tref n, const std::map<tref, size_t, subtree_less<node>>& types) {
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
	return retype<node>(n, get_type_id<node>(n));
	return n;
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
	auto types = resolver.current_kinds();

	auto to_be_updated = std::set<size_t>(types_to_update.begin(), types_to_update.end());
	
	auto f = [&](tref n) -> bool {
		auto t = tau::get(n);
		size_t nt = t.get_type();
	
		if (error) return false;
		switch (nt) {
			case tau::ref: {
				if (!to_be_updated.contains(nt)) return true;
				if (auto updated = update_ref<node>(resolver, n, types); updated) {
					if (updated != n) changes.insert_or_assign(n, updated);
				} else error = true; break;
			}
			case tau::variable: {
				if (!to_be_updated.contains(nt)) return true;
				if (auto updated = update_variable<node>(resolver, n, types); updated) {
					if (updated != n) changes.insert_or_assign(n, updated);
				} else error = true; break;
			}
			case tau::ba_constant: {
				if (!to_be_updated.contains(nt)) return true;
				if (auto updated = update_ba_constant<node>(resolver, n, types); updated) {
					if (updated != n) changes.insert_or_assign(n, updated);
				} else error = true; break;
			}
			case tau::bf_t: case tau::bf_f: {
				if (!to_be_updated.contains(nt)) return true;
				if (auto updated = update_bf_constant<node>(resolver, n, types); updated) {
					if (updated != n) changes.insert_or_assign(n, updated);
				} else error = true; break;
			}
			default: {
				if (auto updated = update_default<node>(n, changes); updated != n)
					changes.insert_or_assign(n, updated);
			}
		}
		if (error) LOG_ERROR << "Incompatible or absent type information in " << LOG_FM(n) << "\n";
		DBG(if (!error) LOG_TRACE << "infer_ba_types/update/" << LOG_NT(nt) << "/n -> new_n:\n"
			<< LOG_FM_TREE(n) << " -> "
			<< LOG_FM_TREE(changes.contains(n) ? changes[n] : n);)
		DBG(LOG_TRACE << "infer_ba_types/update/" << LOG_NT(nt) << "/resolver:\n"
			<< resolver.dump_to_str();)
		return !error;
	};
	
	post_order<node>(n).search(f);
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
				// ...

				auto header_type_ids = get_typeable_type_ids_by_type<node>(
					t[1].get(),	{ tau::variable, tau::ref });
				if (is_functional_relation<node>(n)) {
					// We gather all the data about the body typeables
					auto body_type_ids = get_typeable_type_ids_by_type<node>(
						t[1].get(), { tau::variable, tau::ba_constant, tau::bf_t, tau::bf_f, tau::ref });
					// We create a new scope with all the inferable typeables
					// taking into account that they should have the same type.
					if (!open_same_type<node>(resolver, { 
							header_type_ids[tau::variable], header_type_ids[tau::ref], 
							body_type_ids[tau::ba_constant], body_type_ids[tau::bf_t], 
							body_type_ids[tau::bf_f], body_type_ids[tau::variable], 
							body_type_ids[tau::ref] })) { error = true; break; }
					// We merge all the header and the body typeables together.
					// Note that we also include the header ref to type evrything
					// consistently.
					if (!merge<node>(resolver,  {
							header_type_ids[tau::variable], header_type_ids[tau::ref], 
							body_type_ids[tau::ba_constant], body_type_ids[tau::bf_t], 
							body_type_ids[tau::bf_f], body_type_ids[tau::variable], 
							body_type_ids[tau::ref]	})) { error = true; break; }
					break;
				}
				// Otherwise, we have a predicate relation and we just open a new
				// scope with all the vars in the body.
				if (!open<node>(resolver, {	header_type_ids[tau::variable] })) { 
					error = true; break; 
				}
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
				open<node>(resolver, quantified_vars);
				break;
			}
			case tau::bf: {
				// If bf is not a top level one, it must have been treated before:
				if (!is_top_level_bf<node>(parent)) { skip = true; break; }
				// Otherwise we have to treat it as an atomic formula.
				auto typeables = get_typeable_type_ids_by_type<node>(n, { tau::variable, tau::ba_constant, tau::bf_t, tau::bf_f });
				if (!open<node>(resolver, { typeables[tau::ba_constant], typeables[tau::bf_t], typeables[tau::bf_f] })) { error = true; break; }
				if (!insert<node>(resolver, { typeables[tau::variable] })) { error = true; break; }
				if (!merge<node>(resolver, typeables)) { error = true; break; }
				break;
			}
			case tau::ref_arg: {
				// This case happens when we are in a wff_ref node. We must treat 
				// the argument as an atomic formula.
				auto typeables = get_typeable_type_ids_by_type<node>(n, { tau::variable, tau::ba_constant, tau::bf_t, tau::bf_f });
				if (!open<node>(resolver, { typeables[tau::ba_constant], typeables[tau::bf_t], typeables[tau::bf_f] })) { error = true; break; }
				if (!insert<node>(resolver, { typeables[tau::variable] })) { error = true; break; }
				if (!merge<node>(resolver, typeables)) { error = true; break; }
				break;
			}
			case tau::bf_eq: case tau::bf_neq: case tau::bf_lteq: case tau::bf_nlteq:
			case tau::bf_gt: case tau::bf_ngt: case tau::bf_gteq: case tau::bf_ngteq:
			case tau::bf_lt: case tau::bf_nlt:
			case tau::bf_interval: {
				auto typeables = get_typeable_type_ids_by_type<node>(n, { tau::variable, tau::ba_constant, tau::bf_t, tau::bf_f });
				if (!open<node>(resolver, { typeables[tau::ba_constant], typeables[tau::bf_t], typeables[tau::bf_f] })) { error = true; break; }
				if (!insert<node>(resolver, { typeables[tau::variable] })) { error = true; break; }
				if (!merge<node>(resolver, typeables)) { error = true; break; }
				break;
			}
			case tau::ref: {
				// We skip the traversal if the parent is not a wff_ref as it
				// must have been treated before.
				if (!is<node, tau::wff_ref>(parent)) { skip = true; break; }
				// Otherwise, we continue the traversal so that we can treat
				// the ref_args as above.
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
			case tau::wff_ref: {
				// TODO (HIGH) Traverse all the aguments in reverse order closing scopes.
				
				// Resolve everything in the rec relation and close the scope
				auto updated = update<node>(resolver, n, { tau::ref, tau::variable, tau::ba_constant, tau::bf_t, tau::bf_f });
				if(updated == nullptr) { error = true; return; }
				if(updated != n) transformed.insert_or_assign(n, updated);
				break;
			}
			case tau::rec_relation: {
				// Resolve everything in the rec relation and close the scope
				// Adjust wrapping around refs accordingly (if needed)
				if (!is_functional_relation<node>(n)) {
					// We have a predicate relation, so we need to adjust the
					// wrapping around refs in the body and the head accordingly.
					// The rest is treated as wff_ref or bf_eq,... cases.
					auto new_head = tau::get(tau::wff,
						tau::get(tau::wff_ref,
							tau::get(t[0].value.ba_retype(bool_type_id<node>()), t[0].get_children())));
					tref new_body = is<node, tau::ref>(t.child(1))
						? tau::get(tau::wff,
							tau::get(tau::wff_ref,
								tau::get(t[1].value.ba_retype(bool_type_id<node>()), t[1].get_children())))
						: t.child(1);
					auto new_n = tau::get(tau::rec_relation, {new_head, new_body});
					transformed.insert_or_assign(n, new_n);
					break;
				}	
				// Otherwise, we have a functional relation and we need to 
				// resolve everything in the head and the body. We also need to
				// adjust the wrapping around refs in the body and the head
				// accordingly.
				auto updated = update<node>(resolver, n, { tau::ref, tau::variable, tau::ba_constant, tau::bf_t, tau::bf_f });
				if(updated == nullptr) { error = true; break; }
				if(updated != n) transformed.insert_or_assign(n, updated);
				auto type = tau::get(updated)[0].get_ba_type();
				auto new_head = tau::get_typed(tau::bf, 
					tau::get_typed(tau::bf_ref, tau::get(updated).child(0), type), type);
				auto new_body = is<node, tau::ref>(t.child(1))
					? tau::get_typed(tau::bf, tau::get_typed(tau::bf_ref, tau::get(updated).child(1), type), type)
					: t.child(1);
				auto new_n = tau::get(tau::rec_relation, {new_head, new_body});
				transformed.insert_or_assign(n, new_n);
				resolver.close();
				break;

			}
			case tau::wff_all: case tau::wff_ex: {
				tref new_n = update_default<node>(n, transformed);
				auto updated = update<node>(resolver, new_n, { tau::ref, tau::variable, tau::ba_constant, tau::bf_t, tau::bf_f });
				if(updated == nullptr) { error = true; break; }
				if(updated != new_n) transformed.insert_or_assign(n, updated);
				resolver.close();
				break;
			}
			case tau::bf_eq: case tau::bf_neq: case tau::bf_lteq: case tau::bf_nlteq:
			case tau::bf_gt: case tau::bf_ngt: case tau::bf_gteq: case tau::bf_ngteq:
			case tau::bf_lt: case tau::bf_nlt:
			case tau::bf_interval: {
				auto updated = update<node>(resolver, n, { tau::ref, tau::variable, tau::ba_constant, tau::bf_t, tau::bf_f });
				if(updated == nullptr) { error = true; break; }
				if(updated != n) transformed.insert_or_assign(n, updated);
				break;
			}
			case tau::bf: {
				if (is_top_level_bf<node>(parent)) {
					auto updated = update<node>(resolver, n, { tau::ref, tau::variable, tau::ba_constant, tau::bf_t, tau::bf_f });
					if(updated == nullptr) { error = true; break; }
					if(updated != n) transformed.insert_or_assign(n, updated);
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
	tau::use_hooks = using_hooks;
	return std::pair<tref, subtree_map<node, size_t>>{ new_n, n_global_scope };
}

} // namespace idni::tau_lang
