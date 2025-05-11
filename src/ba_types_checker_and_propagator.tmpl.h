// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "ba_types_checker_and_propagator.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ba_types_inference"

namespace idni::tau_lang {

template <typename... BAs>
requires BAsPack<BAs...>
tref ba_types_checker_and_propagator<BAs...>::operator()(tref n) {
	if (disabled) return n;
	LOG_TRACE << "(T) BA type check and propagate: " << TAU_TO_STR(n);
	ts.clear(), untyped_n = 0;
	var_scopes_t vsc; size_t tsid = 0;
	tref x = replace_types_with_scope_ids(n, vsc, 0, tsid);
	if (x == nullptr) return nullptr;

	LOG_TRACE << "(T) BA type scoped: " << TAU_TO_STR(x);

	static std::string untyped{"???"};
	for (auto& [k, v] : ts) LOG_TRACE << "(T) \t"
			<< (v.empty() ? untyped : v)<< " \t ( " << k << ")";
	LOG_TRACE << "(T) check_and_propagate BA types";
	auto nn = check_and_propagate(x);
	if (!nn) return nullptr;

	auto dflt = nso_factory<BAs...>::instance().default_type();
	bool once_more = false;
	for (auto& [k, v] : ts) if (v.empty()) {
		ts[k] = dflt;
		LOG_TRACE << "(T) setting default type: " << dflt
							<< " to " << k;
		once_more = true;
	}
	if (once_more) {
		LOG_TRACE << "(T) check_and_propagate BA types "
						"once more with default type";
		untyped_n = 0;
		nn = check_and_propagate(x);
		if (!nn) return nullptr;
	}

	tref y = replace_scope_ids_with_types(x);
	LOG_TRACE << "(T) transformed BA types: " << TAU_TO_STR(y);
	ts.clear();
	return y;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref ba_types_checker_and_propagator<BAs...>::operator()(const tt& n) {
	if (!n.has_value()) return nullptr;
	return (*this)(n.value());
}

// removes type info from constants and vars and adds their scope_id
// used as a preparation (first) step before type checking and propagation
// vsc = vars scope ids
// csid = constant scope id
// tsid = temporal scope id
template <typename... BAs>
requires BAsPack<BAs...>
tref ba_types_checker_and_propagator<BAs...>::replace_types_with_scope_ids(
	tref n, var_scopes_t& vsc, size_t csid, size_t& tsid)
{
	LOG_TRACE
			<< "(T) replace_types_with_scope_ids: " << n;
	// ptree(std::cout << "tree: ", n) << "\n";
	const auto vsid = [this, &vsc](tref var) {
		auto n = tau::get(var).get_string();
		if (vsc.find(n) == vsc.end()) vsc[n] = 0;
		return vsc[n];
	};
	const auto inc_vsid = [this, &vsc](const node& var) {
		auto n = tau::get(var).get_string();
		if (vsc.find(n) == vsc.end()) vsc[n] = 0;
		else vsc[n]++;
	};
	const auto transform_element = [this, &csid, &tsid, &vsid](
		tref el, size_t nt)
	{
		LOG_TRACE << "(T) transform element: " << el;
		auto io_var = tt(el) | tau::variable | tau::io_var
							| tt::only_child;
		bool is_io_var = io_var.has_value();
		bool is_global = is_io_var || (tt(el) | tau::variable
				| tau::uconst_name).has_value();
		trefs ch;
		auto ch0 = tau::get(el).first();
		if (is_io_var) {
			auto tv = io_var | tau::offset | tau::bf_variable;
			if (!tv) tv = io_var
				| tau::offset | tau::shift | tau::bf_variable;
			if (tv) {
				auto tn = tau::get(tau::bf_variable, {
					tv.value_tree().first(),
					tau::get_num(tau::scope_id, tsid)
				});
				ts[tn] = "_temporal";
				ch0 = replace<node>(ch0, {
					{ tv.value(), tn } });
			}
		}
		ch.push_back(ch0);
		if (!is_global) {
			size_t scope_id = nt == tau::bf_constant
					? csid++
					: vsid(tau::get(el).first());
			ch.push_back(tau::get_num(tau::scope_id, scope_id));
		}
		auto r = tau::get(nt, ch);
		const auto& t = string_from_id(get_type_sid<node>(el));
		if (t.size()) {
			bool found = false;
			for (const auto& st : nso_factory<BAs...>::instance().types())
				if (t == st) { found = true; break; }
			if (!found) {
				LOG_ERROR << "Unsupported type: " << t << "\n";
				return nullptr;
			}
		}
		ts[get_var_key_node(r)] = t;
		LOG_TRACE << "(T) transformed element: " << r;
		return r;
	};
	const auto transformer =
		[this, &vsc, &csid, &tsid, &inc_vsid, &transform_element]
		(tref el)
	{
		const auto& t = tau::get(el);
		auto nt = t.get_type();
		switch (nt) {
		case tau::spec: {
			trefs rrch, ch;
			auto rrs = tt(el) | tau::rec_relations
							|| tau::rec_relation;
			auto main = tt(el) | tau::main | tt::ref;
			for (tref r : rrs.values()) {
				var_scopes_t nvsc;
				// LOG_TRACE << "(T) replacing rec_relation";
				auto x = replace_types_with_scope_ids(r,
							nvsc, csid, ++tsid);
				if (x == nullptr) return nullptr;
				rrch.push_back(x);
			}
			var_scopes_t nvsc;
			// LOG_TRACE << "(T) replacing main";
			auto x = replace_types_with_scope_ids(
				main, nvsc, csid, ++tsid);
			if (x == nullptr) return nullptr;
			ch.push_back(x);
			if (rrch.size())
				ch.push_back(tau::get(tau::rec_relations, rrch));
			return tau::get(tau::spec, ch);
		}
		case tau::wff_always:
		case tau::wff_sometimes: {
			auto x = replace_types_with_scope_ids(t.first(),
							vsc, csid, ++tsid);
			if (x == nullptr) return nullptr;
			return get(nt, x);
		}
		case tau::wff_ex:
		case tau::wff_all: {
			// ptree(std::cout << "quant: ", el->child[0]) << "\n";
			auto v = transform_element(t.first(), tau::bf_variable);
			if (v == nullptr) return nullptr;
			// ptree(std::cout << "quant var: ", v) << "\n";
			inc_vsid(get_var_key_node(v));
			auto x = replace_types_with_scope_ids(t.second(),
							vsc, csid, tsid);
			if (x == nullptr) return nullptr;
			auto r = tau::get(t.value, { v, x });
			// LOG_TRACE << "(T) quant replaced: " << r;
			return r;
		}
		case tau::bf_constant:
		case tau::bf_variable:
			return transform_element(el, nt);
		}
		return el;
	};
	auto r = pre_order<node>(n).apply_until_change(transformer);
	LOG_TRACE << "(T) replaced_types_with_scope_ids: " << r;
	return r;
}

// checks and propagates types within the scope (global or quantifier)
template <typename... BAs>
requires BAsPack<BAs...>
tref ba_types_checker_and_propagator<BAs...>::check_and_propagate(tref n) {
	bool err = false;
	const auto checker_and_propagator = [this, &err](tref el) -> bool {
		if (err) return false;
		auto nt = tau::get(el).get_type();
		switch (nt) {
			case tau::bf_interval:
			case tau::bf_eq:
			case tau::bf_neq:
			case tau::bf_less_equal:
			case tau::bf_nleq:
			case tau::bf_greater:
			case tau::bf_ngreater:
			case tau::bf_greater_equal:
			case tau::bf_ngeq:
			case tau::bf_less:
			case tau::bf_nless:
			case tau::bf:
				// prop known type within atomic formula
				if (!propagate(el))
					return err = true, false;
				break;
			case tau::wff_ex:
			case tau::wff_all:
				// check and propagate q_var sub scope first
				LOG_TRACE << "(T) check and propagate "
					<< "quantifier subscope: " << el;
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
		// LOG_TRACE << "(T) untyped_n0: " << untyped_n0;
		pre_order<node>(n).visit(checker_and_propagator);
		if (err) return nullptr; // type mismatch
		// LOG_TRACE << "(T) untyped_n: " << untyped_n;
	} while (untyped_n0 != untyped_n);
	return n;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref ba_types_checker_and_propagator<BAs...>::get_var_key_node(tref n) const {
	auto io_var = tt(n) | tau::variable | tau::io_var | tt::only_child;
	if (!io_var) return n;
	return io_var.value_tree().first();
}

// check appearance of a single type in all BA elems, then propagate it
template <typename... BAs>
requires BAsPack<BAs...>
bool ba_types_checker_and_propagator<BAs...>::propagate(tref n) {
	std::set<std::string> types;
	auto els = tau::get(n).select_all(is_ba_element);
	// collect types from BA elements
	for (tref el : els) {
		auto key = get_var_key_node(el);
		if (ts[key].size() && ts[key] != "_temporal") {
			auto it = types.begin();
			types.insert(ts[key]);
			if (types.size() > 1) { // multiple types found, error
				LOG_ERROR << "BA type mismatch: " << el << " : "
					<< ts[key] <<" is expected to be "<<*it;
				return false;
			}
		}
	}
	if (types.size() == 1) { // single type found, propagate it
		auto t = *types.begin();
		for (auto& el : els) {
			auto key = get_var_key_node(el);
			if (ts[key].empty()) {
				LOG_TRACE << "(T) propagating type: "
							<< t << " to " << el;
				ts[key] = t;
				untyped_n--;
			}
		}
	}
	return true; 
}

// transform type info - remove scope_id and add type subnode
// used as a cleaning (last) step after type checking and propagation
template <typename... BAs>
requires BAsPack<BAs...>
tref ba_types_checker_and_propagator<BAs...>::replace_scope_ids_with_types(
	tref n) const
{
	const auto transformer = [this](const auto& el) {
		if (is_ba_element(el)) {
			auto key = get_var_key_node(el);
			LOG_TRACE << "(T) transformer of ba_element: " << el;
			for (auto& [k, v] : ts) {
				LOG_TRACE << "(T) ident: " << key
					<< (key == k ? " == " : " != ")
					<< k << " : " << v;
			}
			LOG_TRACE << "(T) type: " << ts.at(key);
			const auto& t = tau::get(el);
			trefs ch{ t.first() };
			if (t.is(tau::bf_constant)) ch.push_back(
					tau::get(tau::type, ts.at(key)));
			return get(t.value, ch);
		}
		return el;
	};
	return pre_order<node>(n).apply_until_change(transformer);
}

} // idni::tau_lang namespace
