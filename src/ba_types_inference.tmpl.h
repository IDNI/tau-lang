// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "ba_types.h"
#include "ba_types_inference.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ba_types_inference"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// BA types inference

template <NodeType node>
tref infer_ba_types(tref n) {
	LOG_TRACE << "-- Infer BA types: " << LOG_FM(n);
	ba_types_inference<node> infer;
	tref nn = infer(n);
	if (nn) LOG_TRACE << "-- Inferred BA types: " << LOG_FM(nn);
	else    LOG_TRACE << "-- Inferred BA types: nullptr";
	return nn;
}

// -----------------------------------------------------------------------------
// BA types checker and propagator

template <NodeType node>
tref ba_types_inference<node>::operator()(tref n) {
	if (disabled) return n;
	if (n == nullptr) return nullptr;

	static size_t counter = 0;
	counter++;
	std::string id = "[" + LOG_BRIGHT_COLOR + std::to_string(counter)
						 + TC.CLEAR() + "] ";
	LOG_TRACE << id << LOG_SPLITTER;
	LOG_TRACE << id << LOG_BRIGHT("BA type check and propagation: ")
							<< LOG_FM_DUMP(n);

	size_t dflt = get_ba_type_id<node>(
			node::nso_factory::instance().default_type());
	subtree_map<node, tref> defaulted;

	// LOG_TRACE << ba_types<node>::dump_to_str();

	untyped_n = 0;
	var_scopes_t vscids; size_t tsid = 0;
	tref x = add_scope_ids(n, vscids, 0, tsid);
	if (x == nullptr) return nullptr;

	LOG_TRACE << id << "-- BA elementes scoped: " << LOG_FM(x);
	// LOG_TRACE << dump_to_str();

	LOG_TRACE << id << LOG_SPLITTER;

	LOG_TRACE << id << "-- Check and propagate: " << LOG_FM(n);
	auto nn = check_and_propagate(x);

	// LOG_TRACE << dump_to_str();
	if (!nn) {
		LOG_TRACE << id << "-- Check failed, returning nullptr for: "
								<< LOG_FM(n);
		return nullptr;
	}

	LOG_TRACE << id << "-- Check and propagate finished, set default type "
		<< LOG_BA_TYPE(dflt) << " for untyped: " << LOG_FM(n);

	// LOG_TRACE << ba_types<node>::dump_to_str();

	for (auto& [key, tid] : types) if (tau::get(key).get_ba_type() == 0) {
		if (types[key] != 0) continue;
		types[key] = dflt;
		LOG_TRACE << dump_to_str();
		LOG_TRACE << id << "-- Setting default type: "
			<< LOG_BA_TYPE(dflt) << " to " << LOG_FM_DUMP(key);
		// TODO if tid > 0, already resolved???
		if (tau::get(key).is(tau::bf_constant)) {
			tref bound = tau::get_ba_constant_from_source(
						tau::get(key).data(), dflt);
			if (bound == nullptr) {
				LOG_TRACE << dump_to_str();
				LOG_ERROR << "Failed to bind default type: "
				<< LOG_BA_TYPE(dflt) << " to " << LOG_FM(key);
				return nullptr;
			}
			LOG_TRACE << dump_to_str();
			LOG_TRACE << id << "-- Resolved to a default type: "
								<< LOG_FM_DUMP(bound);
			defaulted[key] = bound;
			LOG_TRACE << dump_to_str();
		}
	}

	LOG_TRACE << id << LOG_SPLITTER;

	if (defaulted.size()) {
		LOG_TRACE << id << "-- Check and propagate "
				<< "BA types once more with default type";

		for (auto& [key, bound] : defaulted) {
			types[bound] = dflt;
			resolved[key] = bound;
		}

		// LOG_TRACE << dump_to_str();

		untyped_n = 0;
		nn = check_and_propagate(x);

		LOG_TRACE << dump_to_str();
		if (!nn) LOG_TRACE << id << "-- Check with default type failed, "
				<< "returning nullptr for: " << LOG_FM(n);
		if (!nn) return nullptr;
	}

	LOG_TRACE << id << LOG_SPLITTER;

	tref r = remove_scope_ids(x);

	LOG_TRACE << id << LOG_SPLITTER;
	LOG_TRACE << id << "-- BA constants: " << ba_constants<node>::dump_to_str();
	if (r) LOG_TRACE << id
		<< LOG_BRIGHT("-- Transformed BA types: ") << LOG_FM_DUMP(r);
	else   LOG_TRACE << id
		<< LOG_BRIGHT("-- Transformed BA types: nullptr");
	LOG_TRACE << id << "\t\tfor: " << LOG_FM(n);
	LOG_TRACE << id << LOG_SPLITTER;

	return r;
}

template <NodeType node>
tref ba_types_inference<node>::operator()(const tt& n) {
	if (!n.has_value()) return nullptr;
	return (*this)(n.value());
}

// removes type info from constants and vars and adds their scope_id
// used as a preparation (first) step before type checking and propagation
// vsc = vars scope ids
// csid = constant scope id
// tsid = temporal scope id
template <NodeType node>
tref ba_types_inference<node>::add_scope_ids(
	tref n, var_scopes_t& vscids, size_t csid, size_t& tsid)
{
	LOG_TRACE << "-- Add_scope_ids: " << LOG_FM(n);
	// ptree(std::cout << "tree: ", n) << "\n";
	const auto vsid = [this, &vscids](tref var) {
		size_t v = get_var_name_sid<node>(var);
		if (vscids.find(v) == vscids.end()) vscids[v] = 0;
		return vscids[v];
	};
	const auto inc_vsid = [this, &vscids](tref var) {
		size_t v = get_var_name_sid<node>(var);
		if (vscids.find(v) == vscids.end()) vscids[v] = 0;
		else vscids[v]++;
	};
	const auto transform_element = [this, &csid, &tsid, &vsid](
		tref el, size_t nt) -> tref
	{
		LOG_TRACE << "transform element: " << LOG_FM(el);

		static const size_t temporal = get_ba_type_id<node>("_temporal");
		auto is_io = is_io_var<node>(el);
		bool is_global = is_io || is_uconst<node>(el);
		const auto& t = tau::get(el);
		trefs ch;
		auto ch0 = t.first();
		if (is_io) {
			auto tv = tt(el) | tau::io_var
				| tau::offset | tau::variable;
			if (!tv) tv = tt(el) | tau::io_var
				| tau::offset | tau::shift | tau::variable;
			if (tv) {
				auto tn = tau::get(tau::variable, {
					tv.value_tree().first(),
					tau::get(tau::scope_id, tsid)
				});
				types[tn] = temporal;
				ch0 = rewriter::replace<node>(ch0,
					subtree_map<node, tref>{
							{ tv.value(), tn } });
			}
		}
		if (ch0) ch.push_back(ch0);
		bool is_constant = nt == tau::bf_constant;
		if (!is_global && !is_constant) {
			size_t scope_id = is_constant ? csid : vsid(el);
			ch.push_back(tau::get(node(tau::scope_id, scope_id)));
		}
		for (tref c : ch) LOG_TRACE << "child: " << LOG_FM_DUMP(c);
		auto r = tau::get(t.value, ch);
		size_t tid = t.get_ba_type();
		const auto& type = t.get_ba_type_name();
		LOG_TRACE << "type: " << type;
		if (type.size() && type != "untyped") {
			bool found = false;
			for (const auto& st : node::nso_factory::instance()
								.types())
				if (type == st) { found = true; break; }
			if (!found) {
				LOG_ERROR << "Unsupported type: " << type << "\n";
				return nullptr;
			}
		}
		types[get_var_key_node(r)] = tid;
		LOG_TRACE << "transformed element: " << LOG_FM_DUMP(r)
			<< " with type: " << LOG_BA_TYPE(tid);
		return r;
	};
	const auto transformer = [&](tref el) -> tref{
		const auto& t = tau::get(el);
		auto nt = t.get_type();
		switch (nt) {
		case tau::spec: {
			trefs rrch, ch;
			auto rrs = tt(el) | tau::rec_relations
							|| tau::rec_relation;
			auto main = tt(el) | tau::main | tt::ref;
			for (tref r : rrs.values()) {
				var_scopes_t nvscids;
				// LOG_TRACE << "replacing rec_relation";
				tref x = add_scope_ids(r, nvscids, csid,++tsid);
				if (x == nullptr) return nullptr;
				rrch.push_back(x);
			}
			var_scopes_t nvscids;
			// LOG_TRACE << "replacing main";
			tref x = add_scope_ids(main, nvscids, csid, ++tsid);
			if (x == nullptr) return nullptr;
			ch.push_back(x);
			if (rrch.size())
				ch.push_back(tau::get(tau::rec_relations, rrch));
			return tau::get(tau::spec, ch);
		}
		case tau::wff_always:
		case tau::wff_sometimes: {
			tref x = add_scope_ids(t.first(), vscids, csid, ++tsid);
			if (x == nullptr) return nullptr;
			return tau::get(nt, x);
		}
		case tau::wff_ex:
		case tau::wff_all: {
			// ptree(std::cout << "quant: ", el->child[0]) << "\n";
			tref v = transform_element(t.first(), tau::variable);
			if (v == nullptr) return nullptr;
			// ptree(std::cout << "quant var: ", v) << "\n";
			inc_vsid(get_var_key_node(v));
			tref x = add_scope_ids(t.second(), vscids, csid, tsid);
			if (x == nullptr) return nullptr;
			tref r = tau::get(t.value, { v, x });
			// LOG_TRACE << "quant replaced: " << LOG_FM(r);
			return r;
		}
		case tau::bf_constant:
		case tau::variable:
			return transform_element(el, nt);
		default: break;
		}
		return el;
	};
	auto r = pre_order<node>(n).apply_until_change(transformer);
	LOG_TRACE << "-- Added scope ids: " << LOG_FM(r);
	return r;
}

// checks and propagates types within the scope (global or quantifier)
template <NodeType node>
tref ba_types_inference<node>::check_and_propagate(tref n) {
	bool err = false;
	const auto checker_and_propagator = [this, &err](tref el) -> bool {
		if (err) return false;
		auto nt = tau::get(el).get_type();
		switch (nt) {
			case tau::bf_interval:
			case tau::bf_eq:
			case tau::bf_neq:
			case tau::bf_lteq:
			case tau::bf_nlteq:
			case tau::bf_gt:
			case tau::bf_ngt:
			case tau::bf_gteq:
			case tau::bf_ngteq:
			case tau::bf_lt:
			case tau::bf_nlt:
			case tau::bf:
				// prop known type within atomic formulas
				if (!propagate(el)) return err = true, false;
				break;
			case tau::wff_ex:
			case tau::wff_all:
				// check and propagate q_var sub scope first
				LOG_TRACE << "check and propagate quantifier "
					<< "subscope: " << LOG_FM(el);
				check_and_propagate(tau::get(el).second());
				return false;
			default: break;
		}
		return true;
	};
	// repeat visits of checker and propagator
	// until we are still resolving any untyped elements
	size_t untyped_n0;
	do {
		untyped_n0 = untyped_n;
		// LOG_TRACE << "untyped_n0: " << LOG_FM(untyped_n0);
		pre_order<node>(n).visit(checker_and_propagator);
		if (err) return nullptr; // type mismatch
		// LOG_TRACE << "untyped_n: " << LOG_FM(untyped_n);
	} while (untyped_n0 != untyped_n);
	return n;
}

template <NodeType node>
tref ba_types_inference<node>::get_var_key_node(tref n) const {
	DBG(assert(n));
	if (is_io_var<node>(n)) n = get_var_name_node<node>(n);
	if (tau::get(n).get_ba_type() == 0) {
		auto it = resolved.find(n);
		if (it != resolved.end()) return it->second;
	}
	return n;
}

// check appearance of a single type in all BA elems, then propagate it
template <NodeType node>
bool ba_types_inference<node>::propagate(tref n) {
	// LOG_TRACE << dump_to_str();
	// LOG_TRACE << LOG_SPLITTER;
	LOG_TRACE << "-- Propagate: " << LOG_FM(n);
	static const size_t temporal = get_ba_type_id<node>("_temporal");
	// collect types from BA elements
	std::set<size_t> collected;
	auto els = tau::get(n).select_all(is_ba_element<node>);
	for (tref el : els) {
		tref key = get_var_key_node(el);
		if (types[key] != 0 && types[key] != temporal) {
			// LOG_TRACE << "Collecting types from: " << LOG_FM(el);
			// LOG_TRACE << "key:        " << LOG_FM(key);
			// LOG_TRACE << "types[key]: " << LOG_BA_TYPE_DUMP(types[key]);
			collected.insert(types[key]);
			if (collected.size() > 1) { // multiple types found, error
				auto it = collected.begin();
				LOG_ERROR << "BA type mismatch: "
					<< TAU_TO_STR(el) << " : "
					<< get_ba_type_name<node>(types[key])
					<< " is expected to be "
					<< get_ba_type_name<node>(*it);
				return false;
			}
		}
	}
	if (collected.size() == 1) { // single type found, propagate it
		auto t = *collected.begin();
		for (auto& el : els) {
			LOG_TRACE << "Single type found: " << LOG_BA_TYPE(t)
				<< ", propagating from node: " << LOG_FM_DUMP(el);
			tref key = get_var_key_node(el);
			DBG(assert(key));
			// if (!key) LOG_TRACE << "key is nullptr";
			// else LOG_TRACE << "key:        " << LOG_FM_DUMP(key);
			// LOG_TRACE      << "types[key]: "
			// 			<< LOG_BA_TYPE_DUMP(types[key]);
			const auto& kt = tau::get(key);
			if (kt.get_ba_type() == 0)
			{
				LOG_TRACE << "-- Propagating type: "
					<< LOG_BA_TYPE(t) << " to "<<LOG_FM(el);
				// factory bind newly typed constant
				tref bound = tau::get_ba_constant(
						dict(kt.data()),
						get_ba_type_name<node>(t));
				if (bound == nullptr) return false;
				LOG_TRACE << "bound after type resolved: "
							<< LOG_FM(bound);
				if (types.find(bound) == types.end())
					untyped_n--;
				types[key] = t;
				types[bound] = t;
				resolved[key] = bound;
			}
		}
	}
	return true; 
}

// transform type info - remove scope_id and add type subnode
// used as a cleaning (last) step after type checking and propagation
template <NodeType node>
tref ba_types_inference<node>::remove_scope_ids(tref n) const {
	LOG_TRACE << "-- Remove scope ids: " << LOG_FM_DUMP(n);
	const auto transformer = [this](tref el) {
		if (is_ba_element<node>(el)) {
			LOG_TRACE << "transformer of ba_element: " <<LOG_FM_DUMP(el);
			tref key = get_var_key_node(el);
			if (!key) LOG_TRACE << "key is nullptr";
			else LOG_TRACE << "key: " << LOG_FM_DUMP(key);
			const auto& t = tau::get(el);
			LOG_TRACE << dump_to_str();
			LOG_TRACE <<"el type:    "<<LOG_BA_TYPE(t.get_ba_type());
			LOG_TRACE <<"types[key]: "<<LOG_BA_TYPE(types.at(key));
			LOG_TRACE <<"node: " << t.value;
			LOG_TRACE <<"tid:  " << t.get_ba_type();
			if (t.is(tau::bf_constant)) {
				if (el != key && t.get_ba_type() == 0) {
					LOG_TRACE << "-- propagated: "
						<< LOG_FM_DUMP(key);
					return key;
				}
				tref new_el = tau::build_ba_constant(
					t.get_ba_constant(),
					t.get_ba_type());
				LOG_TRACE << "-- transformed: "
					<<LOG_FM_DUMP(new_el);
				return new_el;
			}
			return tau::get(t.value, t.first());
		}
		return el;
	};
	return pre_order<node>(n).apply_until_change(transformer);
}

template <NodeType node>
std::ostream& ba_types_inference<node>::dump(std::ostream& os) const {
	os << "BA types[" << types.size() << "]:\n";
	for (const auto& [key, tid] : types) os << LOG_INDENT << LOG_FM(key)
						<< " : " << (tid) << "\n";
	os << LOG_PADDING << "  Resolved[" << resolved.size() << "]:\n";
	for (const auto& [key, val] : resolved)
		os << LOG_INDENT << LOG_FM(key) << " -> " << LOG_FM(val) <<"\n";
	return os;
}

template <NodeType node>
std::string ba_types_inference<node>::dump_to_str() const {
	std::stringstream ss;
	return dump(ss), ss.str();
}

} // namespace idni::tau_lang
