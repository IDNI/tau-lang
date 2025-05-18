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
	LOG_TRACE << "Infer BA types" << LOG_FM(n);
	static ba_types_inference<node> infer;
	tref nn = infer(n);
	LOG_TRACE << "Inferred BA types" << LOG_FM(nn);
	return n;
}

// -----------------------------------------------------------------------------
// BA types checker and propagator

template <NodeType node>
tref ba_types_inference<node>::operator()(tref n) {
	if (disabled) return n;
	LOG_TRACE << "BA type check and propagate: " << LOG_FM(n);
	ts.clear(), untyped_n = 0;
	var_scopes_t vscids; size_t tsid = 0;
	tref x = add_scope_ids(n, vscids, 0, tsid);
	if (x == nullptr) return nullptr;

	LOG_TRACE << "BA type scoped: " << LOG_FM(x);
	for (auto& [var, tid] : ts) LOG_TRACE << "\t"
		<< LOG_BA_TYPE(tid) << " \t ( " << LOG_FM(var) <<")";
	LOG_TRACE << "check_and_propagate BA types";

	auto nn = check_and_propagate(x);
	if (!nn) return nullptr;

	size_t dflt = get_ba_type_id<node>(
			node::nso_factory::instance().default_type());
	bool once_more = false;
	for (auto& [var, tid] : ts) if (tid == 0) {
		ts[var] = dflt;
		LOG_TRACE << "setting default type: "
			<< LOG_BA_TYPE(dflt) << " to " <<LOG_FM(var);
		once_more = true;
	}
	if (once_more) {
		LOG_TRACE << "check_and_propagate "
				<< "BA types once more with default type";
		untyped_n = 0;
		nn = check_and_propagate(x);
		if (!nn) return nullptr;
	}

	tref r = remove_scope_ids(x);
	LOG_TRACE << "Transformed BA types: " << LOG_FM(r);
	ts.clear();
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
	LOG_TRACE << "replace_types_with_scope_ids: " << LOG_FM(n);
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
		trefs ch;
		auto ch0 = tau::get(el).first();
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
				ts[tn] = temporal;
				ch0 = rewriter::replace<node>(ch0,
					subtree_map<node, tref>{
							{ tv.value(), tn } });
			}
		}
		ch.push_back(ch0);
		if (!is_global) {
			size_t scope_id = nt == tau::bf_constant
					? csid++
					: vsid(el);
			ch.push_back(tau::get(tau::scope_id, scope_id));
		}
		auto r = tau::get(nt, ch);
		const auto& t = string_from_id(get_type_sid<node>(el));
		if (t.size()) {
			bool found = false;
			for (const auto& st : node::nso_factory::instance()
								.types())
				if (t == st) { found = true; break; }
			if (!found) {
				LOG_ERROR << "Unsupported type: " << t << "\n";
				return nullptr;
			}
		}
		ts[get_var_key_node(r)] = get_ba_type_id<node>(t);
		LOG_TRACE << "transformed element: " << LOG_FM(r);
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
	LOG_TRACE << "added scope ids: " << LOG_FM(r);
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
				// prop known type within atomic formula
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
	if (is_io_var<node>(n)) return get_var_name_node<node>(n);
	return n;
}

// check appearance of a single type in all BA elems, then propagate it
template <NodeType node>
bool ba_types_inference<node>::propagate(tref n) {
	std::set<size_t> types;
	auto els = tau::get(n).select_all(is_ba_element<node>);
	static const size_t temporal = get_ba_type_id<node>("_temporal");
	// collect types from BA elements
	for (tref el : els) {
		tref key = get_var_key_node(el);
		if (ts[key] != 0 && ts[key] != temporal) {
			auto it = types.begin();
			types.insert(ts[key]);
			if (types.size() > 1) { // multiple types found, error
				LOG_ERROR << "BA type mismatch: "
					<< TAU_TO_STR(el) << " : "
					<< get_ba_type_name<node>(ts[key])
					<< " is expected to be "
					<< get_ba_type_name<node>(*it);
				return false;
			}
		}
	}
	if (types.size() == 1) { // single type found, propagate it
		auto t = *types.begin();
		for (auto& el : els) {
			tref key = get_var_key_node(el);
			if (ts[key] == 0) {
				LOG_TRACE << "(T) propagating type: "
					<< LOG_BA_TYPE(t) << " to "<<LOG_FM(el);
				ts[key] = t;
				untyped_n--;
			}
		}
	}
	return true; 
}

// transform type info - remove scope_id and add type subnode
// used as a cleaning (last) step after type checking and propagation
template <NodeType node>
tref ba_types_inference<node>::remove_scope_ids(tref n) const {
	const auto transformer = [this](const auto& el) {
		if (is_ba_element<node>(el)) {
			tref key = get_var_key_node(el);
			LOG_TRACE << "transformer of ba_element: " <<LOG_FM(el);
			for (auto& [k, v] : ts) {
				LOG_TRACE << "ident: " << LOG_FM(key)
					<< (key == k ? " == " : " != ")
					<< LOG_FM(k) << " : " << LOG_BA_TYPE(v);
			}
			LOG_TRACE << "type: " << LOG_BA_TYPE(ts.at(key));
			const auto& t = tau::get(el);
			trefs ch{ t.first() };
			if (t.is(tau::bf_constant)) ch.push_back(
					tau::get(tau::type, ts.at(key)));
			return tau::get(t.value, ch);
		}
		return el;
	};
	return pre_order<node>(n).apply_until_change(transformer);
}

} // namespace idni::tau_lang
