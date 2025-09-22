// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "ba_types.h"
#include "ba_types_inference.h"
#include "definitions.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ba_types_inference"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// BA types inference

template <NodeType node>
tref infer_ba_types(tref n) {
	DBG(LOG_TRACE << "-- Infer BA types: " << LOG_FM(n);)
	ba_types_inference<node> infer;
	tref nn = infer(n);
	return nn;
}

// -----------------------------------------------------------------------------
// BA types checker and propagator

template <NodeType node>
tref ba_types_inference<node>::operator()(tref n) {
	if (disabled) return n;
	if (n == nullptr) return nullptr;

#ifdef DEBUG
	static size_t counter = 0;
	counter++;
	std::string id = "[" + LOG_BRIGHT_COLOR + std::to_string(counter)
						+ TC.CLEAR() + "] ";
	LOG_TRACE << id << LOG_SPLITTER;
	LOG_TRACE << id << LOG_BRIGHT("BA type check and propagation: ")
							<< LOG_FM_DUMP(n);
#endif
	size_t dflt = get_ba_type_id<node>(
			node::nso_factory::instance().default_type());
	subtree_map<node, tref> defaulted;

	// DBG(LOG_TRACE << ba_types<node>::dump_to_str();)

	var_scopes_t vscids; size_t tsid = 0;
	tref scoped = add_scope_ids(n, vscids, 0, tsid);
	if (scoped == nullptr) return nullptr;

	DBG(LOG_TRACE << id << "-- BA elementes scoped: " << LOG_FM(scoped);)
	// DBG(LOG_TRACE << dump_to_str();)

	// DBG(LOG_TRACE << id << LOG_SPLITTER;)

	DBG(LOG_TRACE << id << "-- Check and propagate: " << LOG_FM(n);)
	auto nn = check_and_propagate(scoped);

	DBG(LOG_TRACE << dump_to_str();)
	if (!nn) {
		DBG(LOG_TRACE << id << "-- Check failed, returning nullptr for: "
								<< LOG_FM(n);)
		return nullptr;
	}

	DBG(LOG_TRACE << id << "-- Check and propagate finished, set default type "
		<< LOG_BA_TYPE(dflt) << " for untyped: " << LOG_FM(n);)

	// DBG(LOG_TRACE << ba_types<node>::dump_to_str();)

	for (auto& [key, tid] : types) if (tau::get(key).get_ba_type() == 0) {
		if (types[key] != 0) continue;
		types[key] = dflt;
		DBG(LOG_TRACE << dump_to_str();)
		DBG(LOG_TRACE << id << "-- Setting default type: "
			<< LOG_BA_TYPE(dflt) << " to " << LOG_FM(key);)
		// TODO if tid > 0, already resolved???
		if (tau::get(key).is(tau::bf_constant)) {
			tref bound = tau::get_ba_constant_from_source(
						tau::get(key).data(), dflt);
			if (bound == nullptr) {
				DBG(LOG_TRACE << dump_to_str();)
				LOG_ERROR << "Failed to bind default type: "
				<< LOG_BA_TYPE(dflt) << " to " << LOG_FM(key);
				return nullptr;
			}
			DBG(LOG_TRACE << id << "-- Resolved to a default type: "
							<< LOG_FM(bound);)
			defaulted[key] = bound;
		}
	}

	// DBG(LOG_TRACE << id << LOG_SPLITTER;)

	if (defaulted.size()) {
		DBG(LOG_TRACE << id << "-- Check and propagate "
				<< "BA types once more with default type";)

		for (auto& [key, res] : defaulted)
			types[res] = dflt,
			resolved[key] = res;

		// DBG(LOG_TRACE << dump_to_str();)

		nn = check_and_propagate(scoped);

		DBG(if (!nn) LOG_TRACE << id << "-- Check with default type failed, "
				<< "returning nullptr for: " << LOG_FM(n);)
		if (!nn) return nullptr;
	}

	// DBG(LOG_TRACE << id << LOG_SPLITTER;)

	tref r = remove_scope_ids(scoped);

#ifdef DEBUG
	// LOG_TRACE << id << LOG_SPLITTER;
	// LOG_TRACE << id << "-- BA constants: " << ba_constants<node>::dump_to_str();

	LOG_TRACE << dump_to_str();

	if (r) LOG_TRACE << id
		<< LOG_BRIGHT("-- Transformed BA types: ") << LOG_FM_DUMP(r);
	else   LOG_TRACE << id
		<< LOG_BRIGHT("-- Transformed BA types: nullptr");
	LOG_TRACE << id << "\t\tfor: " << LOG_FM(n);

	// LOG_TRACE << id << LOG_SPLITTER;
#endif

	clear();

	return r;
}

template <NodeType node>
tref ba_types_inference<node>::operator()(const tt& n) {
	if (!n.has_value()) return nullptr;
	return (*this)(n.value());
}

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "batinf_scoping"

// removes type info from constants and vars and adds their scope_id
// used as a preparation (first) step before type checking and propagation
// vsc = vars scope ids
// csid = constant scope id
// tsid = temporal scope id
template <NodeType node>
tref ba_types_inference<node>::add_scope_ids(
	tref n, var_scopes_t& vscids, size_t csid, size_t& tsid)
{
	DBG(LOG_TRACE << "-- Add_scope_ids: " << LOG_FM_DUMP(n);)
	// ptree(std::cout << "tree: ", n) << "\n";
	const auto vsid = [&vscids](tref var) -> size_t {
		size_t v = get_var_name_sid<node>(var);
		if (vscids.find(v) == vscids.end()) return 0;
		return vscids[v];
	};
	const auto inc_vsid = [&vscids](tref var) {
		size_t v = get_var_name_sid<node>(var);
		if (vscids.find(v) == vscids.end()) vscids[v] = 0;
		else vscids[v]++;
	};
	const auto transform_element = [this, &csid, &tsid, &vsid](
		tref el, size_t nt) -> tref
	{
		DBG(LOG_TRACE << "add_scope_ids - transform_element: " << LOG_FM_DUMP(el);)

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
				tref tn = tau::get(tau::variable, {
					tv.value_tree().first(),
					tau::get(node(tau::scope_id, tsid))
				});
				types[tau::get(tn).first()] = temporal;
				ch0 = rewriter::replace<node>(ch0,
					subtree_map<node, tref>{
							{ tv.value(), tn } });
			}
		}
		if (ch0) ch.push_back(ch0);
		bool is_constant = nt == tau::bf_constant;
		DBG(LOG_TRACE << "is_global: " << is_global
			<< "  is_constant: " << is_constant
			<< "  is_io: " << is_io << "  nt: " << nt;)

		if (!is_global && !is_constant) {
			size_t scope_id = is_constant ? csid : vsid(el);
			ch.push_back(tau::get(node(tau::scope_id, scope_id)));
		}
		tref r = tau::get(t.value, ch);
		size_t tid = tau::get(r).get_ba_type();
		if (is_io && tid == 0) {
			// Update type information from stream definitions
			// Get string id of stream name
			size_t sid = get_var_name_sid<node>(r);
			auto& defs = definitions<node>::instance();
			// See if stream is present in outputs
			const auto& mo = defs.get_output_defs();
			if (auto it = mo.find(sid); it != mo.end()) {
				// stream found, assign type id
				tid = it->second.first;
				r = tau::get(r).replace_value(
					tau::get(r).value.ba_retype(tid));
			} else {
				// See if stream is present in inputs
				const auto& mi = defs.get_input_defs();
				if (it = mi.find(sid); it != mi.end()) {
					// stream found, assign type id
					tid = it->second.first;
					r = tau::get(r).replace_value(
						tau::get(r).value.ba_retype(tid));
				}
			}
		}
		const auto& type = tau::get(r).get_ba_type_name();
		DBG(LOG_TRACE << "type: " << type;)
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
		DBG(LOG_TRACE << "r: " << LOG_FM_DUMP(r);)
		DBG(LOG_TRACE << "get_el_key(r): " << LOG_FM_DUMP(get_el_key(r));)
		types[get_el_key(r)] = tid;
		DBG(LOG_TRACE << "add_scope_ids - transformed element: " << LOG_FM_DUMP(r)
			<< " with type: " << LOG_BA_TYPE(tid);)
		return r;
	};
	const auto transformer = [&](tref el) -> tref{
		const auto& t = tau::get(el);
		auto nt = t.get_type();
		switch (nt) {
		case tau::spec: {
			trefs rrch, ch;
			auto rrs = tt(el) | tau::definitions
							|| tau::rec_relation;
			auto main = tt(el) | tau::main | tt::ref;
			for (tref r : rrs.values()) {
				var_scopes_t nvscids;
				// DBG(LOG_TRACE << "replacing rec_relation";)
				tref x = add_scope_ids(r, nvscids, csid,++tsid);
				if (x == nullptr) return nullptr;
				rrch.push_back(x);
			}
			var_scopes_t nvscids;
			// DBG(LOG_TRACE << "replacing main";)
			tref x = add_scope_ids(main, nvscids, csid, ++tsid);
			if (x == nullptr) return nullptr;
			ch.push_back(x);
			if (rrch.size())
				ch.push_back(tau::get(tau::definitions, rrch));
			return tau::get(tau::spec, ch);
		}
		case tau::wff_always:
		case tau::wff_sometimes: {
			tref x = add_scope_ids(t.first(), vscids, csid, tsid);
			if (x == nullptr) return nullptr;
			return tau::get(nt, x);
		}
		case tau::wff_ex:
		case tau::wff_all:
		case tau::bf_fall:
		case tau::bf_fex: {
			inc_vsid(t.first());
			tref v = transform_element(t.first(), tau::variable);
			if (v == nullptr) return nullptr;
			tref x = add_scope_ids(t.second(), vscids, csid, tsid);
			if (x == nullptr) return nullptr;
			tref r = tau::get(t.value, { v, x });
			// DBG(LOG_TRACE << "quant replaced: " << LOG_FM(r);)
			return r;
		}
		case tau::bf_constant:
		case tau::variable:
			return transform_element(el, nt);
		case tau::bf_f:
		case tau::bf_t:
			types[get_el_key(el)] = tau::get(el).get_ba_type();
			break;
		default: break;
		}
		return el;
	};
	auto r = pre_order<node>(n).apply_until_change(transformer);
	DBG(LOG_TRACE << "-- Added scope ids: " << LOG_FM(r);)
	return r;
}

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "batinf_propagation"

// checks and propagates types within the scope (global or quantifier)
template <NodeType node>
tref ba_types_inference<node>::check_and_propagate(tref n) {
	subtree_set<node> terms;
	const auto visitor = [&](tref el, tref parent) -> bool {
		auto is_atomic_nt = [](node::type nt) {
			return nt == tau::bf_interval
				|| nt == tau::bf_eq
				|| nt == tau::bf_neq
				|| nt == tau::bf_lteq
				|| nt == tau::bf_nlteq
				|| nt == tau::bf_gt
				|| nt == tau::bf_ngt
				|| nt == tau::bf_gteq
				|| nt == tau::bf_ngteq
				|| nt == tau::bf_lt
				|| nt == tau::bf_nlt
				|| nt == tau::bf;
		};

		auto nt = tau::get(el).get_type();
		if (is_atomic_nt(nt) &&
			(parent == nullptr ||
				!is_atomic_nt(tau::get(parent).get_type())))
					return terms.insert(el), false;
		else if (nt == tau::wff_ex || nt == tau::wff_all) {
			// check and propagate q_var sub scope first
			DBG(LOG_TRACE << "check and propagate quantifier "
				<< "subscope: " << LOG_FM(el);)
			check_and_propagate(tau::get(el).second());
			return false;
		}
		return true;
	};
	pre_order<node>(n).visit_unique(visitor);
	// repeat visits of checker and propagator
	// until we are still resolving any untyped elements
	size_t untyped_n0, untyped_n = 0;
	do {
		untyped_n0 = untyped_n;
		DBG(LOG_TRACE << "untyped_n0: " << untyped_n0;)
		for (const auto& el : terms) if (!propagate(el)) return nullptr;
		untyped_n = 0;
		for (const auto [_, tid] : types) if (tid == 0) untyped_n++;
		DBG(LOG_TRACE << "untyped_n: " << untyped_n;)
	} while (untyped_n0 != untyped_n);
	return n;
}

template <NodeType node>
tref ba_types_inference<node>::get_el_key(tref n) const {
	DBG(assert(n));
	if (tau::get(n).is(tau::bf_f) || tau::get(n).is(tau::bf_t))
		return n;
	n = get_var_name_node<node>(n);
	if (tau::get(n).get_ba_type() == 0) {
		auto it = resolved.find(n);
		if (it != resolved.end()) return it->second;
	}
	return n;
}

// check appearance of a single type in all BA elems, then propagate it
template <NodeType node>
bool ba_types_inference<node>::propagate(tref n) {
	DBG(LOG_TRACE << dump_to_str();)
	DBG(LOG_TRACE << LOG_SPLITTER;)
	DBG(LOG_TRACE << "-- Propagate: " << LOG_FM(n);)
	static const size_t temporal = get_ba_type_id<node>("_temporal");
	// collect types from BA elements
	std::set<size_t> collected;
	auto els = tau::get(n).select_all(is_ba_element<node>);
	for (tref el : els) {
		DBG(LOG_TRACE << "Collecting types from: " << LOG_FM_DUMP(el);)
		tref key = get_el_key(el);
		DBG(LOG_TRACE << "key:        " << LOG_FM_DUMP(key);)
		DBG(LOG_TRACE << "types[key]: " << LOG_BA_TYPE(types[key]);)
		if (types[key] != 0 && types[key] != temporal) {
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
			DBG(LOG_TRACE << "Single type found: " << LOG_BA_TYPE(t)
				<< ", propagating from node: " << LOG_FM_DUMP(el);)
			tref key = get_el_key(el);
			DBG(assert(key));
#ifdef DEBUG
			if (!key) LOG_TRACE << "key is nullptr";
			else LOG_TRACE << "key:        " << LOG_FM_DUMP(key);
			LOG_TRACE      << "types[key]: "
						<< LOG_BA_TYPE_DUMP(types[key]);
#endif
			const auto& kt = tau::get(key);
			if (kt.get_ba_type() == 0)
			{
				DBG(LOG_TRACE << "-- Propagating type: "
					<< LOG_BA_TYPE(t) << " to "
					<< LOG_FM_DUMP(el);)
				const auto& elt = tau::get(el);
				auto nt = elt.get_type();
				if (nt == tau::bf_constant) {
					// factory bind newly typed constant
					tref bound = tau::get_ba_constant(
						dict(kt.data()),
						get_ba_type_name<node>(t));
					if (bound == nullptr) return false;
					DBG(LOG_TRACE << "bound after type resolved: "
							<< LOG_FM(bound);)
					types[key]    = t;
					types[bound]  = t;
					resolved[key] = bound;
				}
				else {
					types[key] = t;
					DBG(LOG_TRACE << "resolved: "
						<< LOG_FM_DUMP(key)
						<< " -> " << LOG_BA_TYPE(t);)
					continue;
				}
			}
		}
	}
	return true; 
}

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "batinf_unscoping"

// transform type info - remove scope_id and replace with typed element
// scope_id is left in temporal vars to distinguish them by their scope further
// last step of BA types checking and propagation
template <NodeType node>
tref ba_types_inference<node>::remove_scope_ids(tref n) const {
	DBG(LOG_TRACE << "-- Remove scope ids: " << LOG_FM_DUMP(n);)
	const auto transformer = [this](tref el) {
		if (is_ba_element<node>(el)) {
			// DBG(LOG_TRACE << LOG_SPLITTER;)
			DBG(LOG_TRACE << "-- Transformer of ba_element: "
							<< LOG_FM_DUMP(el);)
			tref key = get_el_key(el);
			const auto& t = tau::get(el);
#ifdef DEBUG
			if (!key) LOG_TRACE << "key is nullptr";
			else LOG_TRACE << "key: " << LOG_FM_DUMP(key);
			LOG_TRACE << dump_to_str();
			LOG_TRACE <<"el type:    "<<LOG_BA_TYPE(t.get_ba_type());
			LOG_TRACE <<"types[key]: "<<LOG_BA_TYPE(types.at(key));
			LOG_TRACE <<"node: " << t.value;
			LOG_TRACE <<"tid:  " << t.get_ba_type();
#endif
			tref r;
			if (t.is(tau::bf_constant)) {
				if (el != key && t.get_ba_type() == 0) {
					DBG(LOG_TRACE << "-- Propagated: "
							<< LOG_FM_DUMP(key);)
					const auto& bfn = tau::get(
						tau::get(tau::bf, key));
					if (bfn[0].get_ba_type() > 0 &&
						(  bfn[0].is(tau::bf_t)
						|| bfn[0].is(tau::bf_f)))
					{ // strip type info from 0 and 1 coming from hooks
						tref typeless = tau::get(
							bfn[0].get_type());
						DBG(LOG_TRACE << "-- type stripped: "
							<< LOG_FM_DUMP(typeless);)
						return typeless;
					} else {
						DBG(LOG_TRACE << "-- Not propagated: "
							<< LOG_FM_DUMP(key);)
						return key;
					}
					return key;
				}
				tref new_el = tau::build_ba_constant(
					t.get_ba_constant(),
					t.get_ba_type());
				DBG(LOG_TRACE << "-- Transformed: "
							<< LOG_FM_DUMP(new_el);)
				return new_el;
			}
			else if (t.is(tau::bf_t) || t.is(tau::bf_f)) {
				// remove ba type info from 0 and 1
				// for equality checks
				r = tau::get(t.value.ba_retype(0));
			}
			else {
				node nv = t.value.ba_retype(types.at(key));
				r = t.is(tau::variable)
							? tau::get(nv, t.first())
							: tau::get(nv);
			}
			DBG(LOG_TRACE << "-- Transformed: "	<< LOG_FM_DUMP(r);)
			return r;
		}
		return el;
	};
	return pre_order<node>(n).apply_until_change(transformer);
}

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ba_types_inference"

template <NodeType node>
void ba_types_inference<node>::clear() {
	types.clear();
	resolved.clear();
}

template <NodeType node>
std::ostream& ba_types_inference<node>::dump(std::ostream& os) const {
	os << "BA inference types[" << types.size() << "]:\n";
	for (const auto& [key, tid] : types) os << LOG_INDENT << LOG_FM_DUMP(key)
						<< " : " << LOG_BA_TYPE(tid) << "\n";
	os << LOG_PADDING << "  Resolved[" << resolved.size() << "]:\n";
	for (const auto& [key, val] : resolved)
		os << LOG_INDENT << LOG_FM_DUMP(key) << " -> " << LOG_FM_DUMP(val) <<"\n";
	return os;
}

template <NodeType node>
std::string ba_types_inference<node>::dump_to_str() const {
	std::stringstream ss;
	return dump(ss), ss.str();
}

} // namespace idni::tau_lang
