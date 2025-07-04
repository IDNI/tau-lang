// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "normalizer.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "normalizer"

namespace idni::tau_lang {

// IDEA (HIGH) rewrite steps as a tuple to optimize the execution
template <NodeType node>
tref normalizer_step(tref form) {
	using tau = tree<node>;
	using tt = tau::traverser;
#ifdef TAU_CACHE
	// static unordered_tau_map<node, tref> cache;
	static subtree_map<node, tref> cache;
	if (auto it = cache.find(form); it != cache.end()) return it->second;
#endif // TAU_CACHE

	tref result = tt(form)
		// Push all quantifiers in and eliminate them
		| tt::f(eliminate_quantifiers<node>)
		// After removal of quantifiers, only subformulas previously under the scope of a quantifier
		// are reduced
		| bf_reduce_canonical<node>()
		// Normalize always and sometimes quantifiers and reduce Tau formula
		| sometimes_always_normalization<node>()
		| tt::ref;
#ifdef TAU_CACHE
	cache.emplace(form, result);
#endif // TAU_CACHE
	return result;
}

// Assumes that the formula passed does not have temporal quantifiers
// This normalization will non perform the temporal normalization
template <NodeType node>
tref normalize_non_temp(tref fm) {
	using tau = tree<node>;
	using tt = tau::traverser;
#ifdef TAU_CACHE
	// static unordered_tau_map<node, tref> cache;
	static subtree_map<node, tref> cache;
	if (auto it = cache.find(fm); it != cache.end()) return it->second;
#endif // TAU_CACHE
	tref result = tt(fm)
		// Push all quantifiers in and eliminate them
		| tt::f(eliminate_quantifiers<node>)
		// After removal of quantifiers, only subformulas previously under the scope of a quantifier
		// are reduced
		| bf_reduce_canonical<node>()
		| tt::ref;
	result = reduce_across_bfs<node>(result, false);
#ifdef TAU_CACHE
	cache.emplace(fm, result);
#endif // TAU_CACHE
	return result;
}

template <NodeType node>
trefs get_vars_from_nso(tref n) {
	return tree<node>::get(n).select_top(is_var_or_capture<node>());
}

// Given a tref produce a number such that the variable x_i is
// neither a bool_variable nor a variable nor a capture
template <NodeType node>
int_t get_new_var_id(tref fm) {
	// TODO (HIGH) why are captures here? don't captures start with '$'?
	using tau = tree<node>;
	trefs var_nodes = get_vars_from_nso<node>(fm);
	std::set vars{ 1 };
	for (tref var : var_nodes) {
		std::string nam = tau::get(var).is(tau::capture)
			? tau::get(var).get_string()
			: get_var_name<node>(var);
		if (nam[0] == 'x') {
			nam.erase(0, 1);
			if (!nam.empty()) vars.insert(stoi(nam));
		} else if (nam[0] == '?' && nam[1] == 'x') {
			nam.erase(0, 2);
			if (!nam.empty()) vars.insert(stoi(nam));
		}
	}
	return *vars.rbegin() + 1;
}


// Given a tref produce a number i such that the uninterpreted constant const_i is
// not present
template <NodeType node>
tref get_new_uninterpreted_constant(tref fm, const std::string& name) {
	using tau = tree<node>;
	trefs uninter_consts
		= tau::get(fm).select_top(is<node, tau::uconst_name>);
	std::set ids{ 0 };
	for (tref uninter_const : uninter_consts) {
		const auto& tmp = tau::get(uninter_const).get_string();
		std::string id = tmp.substr(name.length() + 1, tmp.size() - 1);
		if (!tmp.empty()) ids.insert(std::stoi(id));
	}
	std::string id = std::to_string(*ids.rbegin() + 1);
	tref uninter_const = tau::build_bf_uconst("", name + id);
	return uninter_const;
}

// extracts ref info. returns pair of rr_sig (name id and arity)
// and vector of its offsets (offset_t)
template <NodeType node>
std::pair<rr_sig, std::vector<offset_t>> get_ref_info(tref ref) {
	using tau = tree<node>;
	using tt = tau::traverser;
	LOG_TRACE << "get_ref_info: " << LOG_FM_DUMP(ref);
	std::pair<rr_sig, std::vector<offset_t>>
					ret{ get_rr_sig<node>(ref), {} };
	auto offsets = tt(ref) | tau::offsets || tau::offset;
	LOG_TRACE << "get ref " << LOG_RR_SIG(ret.first) << " offsets.size: " << offsets.size();
	for (auto offset : offsets()) {
		const auto& t = offset[0];
		int_t d = 0;
		if (t.is_integer()) d = t.get_integer();
		else if (t.get_type() == tau::capture) d = t.data();
		ret.second.emplace_back(t.get_type(), d);
		break; // consider only first offset for now
		// TODO (LOW) support multiindex offsets
		// need to find a canonical way of enumeration first
	}
	return ret;
};

template <NodeType node>
tref get_ref(tref n) {
	using tau = tree<node>;
	using tt = tau::traverser;
	auto ref = tt(n);
	while (ref && !ref.is(tau::ref)) ref = ref | tt::only_child;
	if (!ref || !ref.is(tau::ref)) return nullptr;
	return ref.value();
}

// Check that the Tau formula does not use Boolean combinations of models
template <NodeType node>
bool has_no_boolean_combs_of_models(tref n) {
	using tau = tree<node>;
	const auto& fm = tau::get(n);
	if (is<node>(fm.first(), tau::wff_always)) {
		// check that there is no wff_always or wff_sometimes in the subtree
		if (fm[0][0].find_top(is<node, tau::wff_always>))
			return false;
		if (fm[0][0].find_top(is<node, tau::wff_sometimes>))
			return false;
	} else {
		if (fm.find_top(is<node, tau::wff_always>))
			return false;
		if (fm.find_top(is<node, tau::wff_sometimes>))
			return false;
	}
	return true;
}

template <NodeType node>
bool is_non_temp_nso_satisfiable(tref n) {
	using tau = tree<node>;
	DBG(assert(n != nullptr));
	const auto& fm = tau::get(n);
	DBG(assert(!fm.find_top(is<node, tau::wff_always>));)
	DBG(assert(!fm.find_top(is<node, tau::wff_sometimes>));)

	tref nn = n;
	auto vars = get_free_vars_from_nso<node>(fm.get());
	for (tref v : vars) nn = tau::build_wff_ex(v, nn);
	tref normalized = normalize_non_temp<node>(nn);
	const auto& t = tau::get(normalized);
	assert((t.equals_T() || t.equals_F()
		|| t.find_top(is<node, tau::constraint>)));
	return t.equals_T();
}

template <NodeType node>
bool is_non_temp_nso_unsat(tref n) {
	using tau = tree<node>;
	DBG(assert(n != nullptr));
	DBG(assert(!tau::get(n).find_top(is<node, tau::wff_always>));)
	DBG(assert(!tau::get(n).find_top(is<node, tau::wff_sometimes>));)

	tref nn = n;
	auto vars = get_free_vars_from_nso<node>(nn);
	for (tref v : vars) nn = tau::build_wff_ex(v, nn);
	const auto& t = tau::get(normalize_non_temp<node>(nn));
	assert((t.equals_T() || t.equals_F()
		|| t.find_top(is<node, tau::constraint>)));
	return t.equals_F();
}

template <NodeType node>
bool are_nso_equivalent(tref n1, tref n2) {
	using tau = tree<node>;
	LOG_DEBUG << "Begin are_nso_equivalent";
	LOG_TRACE << "-- n1: " << LOG_FM(n1);
	LOG_TRACE << "-- n2: " << LOG_FM(n2);

	// If this method is called on a formula that has Boolean combinations of models, it is used incorrectly
	DBG(assert((has_no_boolean_combs_of_models<node>(n1)
		&& has_no_boolean_combs_of_models<node>(n2)));)

	const auto& t1 = tau::get(n1);
	const auto& t2 = tau::get(n2);
	if (t1[0].is(tau::wff_always)) n1 = t1[0].first();
	if (t2[0].is(tau::wff_always)) n2 = t2[0].first();

	if (tau::get(n1) == tau::get(n2)) {
		LOG_DEBUG << "-- End are_nso_equivalent: true (equiv nodes)";
		return true;
	}

	// equivalence of references
	tref r1opt = get_ref<node>(n1), r2opt = get_ref<node>(n2);
	if (r1opt != nullptr && r2opt != nullptr) { // both are refs
		bool equiv = get_ref_info<node>(r1opt)
						== get_ref_info<node>(r2opt);
		LOG_DEBUG << "End are_nso_equivalent: " << equiv
							<< " (equiv refs)";
		return equiv;
	}
	else if (r1opt != nullptr || r2opt != nullptr) { // one is a ref
		LOG_DEBUG << "End are_nso_equivalent: false (ref and not ref)";
		return false;
	}

	auto vars = get_free_vars_from_nso<node>(n1);
	for (tref v : vars) LOG_DEBUG << "var1: " << LOG_FM(v);
	auto vars2 = get_free_vars_from_nso<node>(n2);
	for (tref v : vars2) LOG_DEBUG << "var2: " << LOG_FM(v);
	vars.insert(vars2.begin(), vars2.end());
	for (tref v : vars) LOG_DEBUG << "var: " << LOG_FM(v);

	tref imp1 = tau::build_wff_imply(n1, n2);
	tref imp2 = tau::build_wff_imply(n2, n1);

	for (tref v : vars) {
		imp1 = tau::build_wff_all(v, imp1);
		imp2 = tau::build_wff_all(v, imp2);
	}
	LOG_DEBUG << "wff: " << LOG_FM(tau::build_wff_and(imp1, imp2));

	const auto& tdir1 = tau::get(normalizer_step<node>(imp1));
	DBG(assert((tdir1.equals_T() || tdir1.equals_F()
		|| tdir1.find_top(is<node, tau::constraint>)));)
	if (tdir1.equals_F()) {
		LOG_DEBUG << "End are_nso_equivalent: " << LOG_FM(tdir1.get());
		return false;
	}
	const auto& tdir2 = tau::get(normalizer_step<node>(imp2));
	DBG(assert((tdir2.equals_T() || tdir2.equals_F()
		|| tdir2.find_top(is<node, tau::constraint>))));
	bool res = (tdir1.equals_T() && tdir2.equals_T());
	LOG_DEBUG << "End are_nso_equivalent: " << res;
	return res;
}

template <NodeType node>
bool is_nso_equivalent_to_any_of(tref n, trefs& previous) {
	return std::any_of(previous.begin(), previous.end(), [n] (tref& p) {
			return are_nso_equivalent<node>(n, p);
		});
}

template <NodeType node>
bool is_nso_impl(tref n1, tref n2) {
	using tau = tree<node>;

	LOG_DEBUG << "Begin is_nso_impl";
	LOG_TRACE << "n1 " << LOG_FM(n1);
	LOG_TRACE << "n2 " << LOG_FM(n2);
	// If this method is called on a formula that has Boolean combinations of models, it is used incorrectly
	DBG(assert((has_no_boolean_combs_of_models<node>(n1)
		 && has_no_boolean_combs_of_models<node>(n2)));)

	const auto& t1 = tau::get(n1);
	const auto& t2 = tau::get(n2);
	if (t1[0].is(tau::wff_always)) n1 = t1[0].first();
	if (t2[0].is(tau::wff_always)) n2 = t2[0].first();

	if (tau::get(n1) == tau::get(n2)) {
		LOG_DEBUG << "End is_nso_impl: true (n1 implies n2)";
		return true;
	}

	auto vars  = get_free_vars_from_nso<node>(n1);
	auto vars2 = get_free_vars_from_nso<node>(n2);
	vars.insert(vars2.begin(), vars2.end());

	tref imp = tau::build_wff_imply(n1, n2);
	for (tref v : vars) imp = tau::build_wff_all(v, imp);
	LOG_DEBUG << "wff: " << LOG_FM(imp);

	const auto& res = tau::get(normalizer_step<node>(imp));
	DBG(assert((res.equals_T() || res.equals_F()
		|| res.find_top(is<node, tau::constraint>)));)
	LOG_DEBUG << "End is_nso_impl: " << res.get();
	return res.equals_T();
}

template <NodeType node>
bool are_bf_equal(tref n1, tref n2) {
	using tau = tree<node>;
	using tt = tau::traverser;
	
	LOG_DEBUG << "Begin are_bf_equal";
	LOG_TRACE << "n1 " << LOG_FM(n1);
	LOG_TRACE << "n2 " << LOG_FM(n2);
	const auto& t1 = tau::get(n1);
	const auto& t2 = tau::get(n2);
	DBG(assert(t1.is(tau::bf)));
	DBG(assert(t2.is(tau::bf)));

	if (t1 == t2) {
		LOG_DEBUG << "End are_bf_equal: true (equal bf)";
		return true;
	}

	auto vars = get_free_vars_from_nso<node>(n1);
	auto vars2 = get_free_vars_from_nso<node>(n2);
	vars.insert(vars2.begin(), vars2.end());

	tref bf_equal_fm = tau::build_bf_eq(tau::build_bf_xor(n1, n2));
	for (tref v : vars) bf_equal_fm = tau::build_wff_all(v, bf_equal_fm);
	LOG_TRACE << "wff: " << LOG_FM(bf_equal_fm);

	tref normalized = normalizer_step<node>(bf_equal_fm);
	LOG_TRACE << "Normalized: " << LOG_FM(normalized);

	auto check = tt(normalized) | tau::wff_t;
	LOG_DEBUG << "End are_bf_equal: " << check.has_value();
	return check.has_value();
}

template <NodeType node>
bool is_bf_same_to_any_of(tref n, trefs& previous) {
	return std::any_of(previous.begin(), previous.end(), [n](tref p) {
		return are_bf_equal<node>(n, p);
	});
}

template <NodeType node>
tref normalize_with_temp_simp(tref fm) {
	using tau = tree<node>;
	auto trim_q = [](tref n) {
		const auto& t =tau::get(n);
		if (t.child_is(tau::wff_sometimes)
			|| t.child_is(tau::wff_always)) return t[0].first();
		return n;
	};
	const auto& red_fm = tau::get(normalizer_step<node>(fm));
	LOG_TRACE << "red_fm: " << LOG_FM(red_fm.get());
	if (red_fm.equals_T() || red_fm.equals_F())
		return red_fm.get();
	trefs clauses = get_dnf_wff_clauses<node>(red_fm.get());
	tref nn = tau::_F();
	for (tref clause : clauses) {
		LOG_TRACE << "    clause: " << LOG_FM(clause);
		const auto& t = tau::get(clause);
		trefs aw_parts = t.select_top(is_child<node, tau::wff_always>);
		trefs st_parts = t.select_top(is_child<node, tau::wff_sometimes>);
		if (aw_parts.size() == 1 && st_parts.empty()) {
			nn = tau::build_wff_or(nn, clause);
			LOG_TRACE << "    nn: " << LOG_FM(nn);
			continue;
		}
		if (aw_parts.empty() && st_parts.size() == 1) {
			nn = tau::build_wff_or(nn, clause);
			LOG_TRACE << "    nn: " << LOG_FM(nn);
			continue;
		}

		// Replace always and sometimes parts by T
		subtree_map<node, tref> changes;
		for (tref aw : aw_parts) changes.emplace(aw, tau::_T());
		for (tref st : st_parts) changes.emplace(st, tau::_T());
		tref new_clause = rewriter::replace<node>(clause, changes);
		LOG_TRACE << "    new clause: " << LOG_FM(nn);

		// First check if any always statements are implied by others
		for (size_t i = 0; i < aw_parts.size(); ++i) {
			for (size_t j = i + 1; j < aw_parts.size(); ++j) {
				if (is_nso_impl<node>(aw_parts[i], aw_parts[j]))
					aw_parts[j] = tau::_T();
				else if (is_nso_impl<node>(
					aw_parts[j], aw_parts[i]))
						aw_parts[i] = tau::_T();
			}
		}
		// Check for unsat of always conjuncted with single sometimes part
		bool clause_false = false;
		for (tref aw : aw_parts) for (tref st : st_parts) {
			tref f = tau::build_wff_and(trim_q(aw), trim_q(st));
			if (is_non_temp_nso_unsat<node>(f)) clause_false = true;
		}
		if (clause_false) LOG_TRACE << "    clause false";
		if (clause_false) continue;

		// Next check if any always statement implies a sometimes statement
		for (tref aw : aw_parts) for (tref st : st_parts)
			if (is_nso_impl<node>(aw, trim_q(st))) st = tau::_T();

		// Now check if any sometimes statement implies another sometimes
		for (size_t i = 0; i < st_parts.size(); ++i) {
			for (size_t j = i + 1; j < st_parts.size(); ++j) {
				if (is_nso_impl<node>(trim_q(st_parts[i]),
					trim_q(st_parts[j])))
						st_parts[j] = tau::_T();
				else if (is_nso_impl<node>(trim_q(st_parts[j]),
					trim_q(st_parts[i])))
						st_parts[i] = tau::_T();
			}
		}
		new_clause = tau::build_wff_and(new_clause, tau::build_wff_and(
					tau::build_wff_and(aw_parts),
					tau::build_wff_and(st_parts)));
		nn = tau::build_wff_or(nn, new_clause);
		LOG_TRACE << "    nn: " << LOG_FM(nn);
	}
	DBG(assert(nn != nullptr);)
	LOG_TRACE << "normalize_with_temp_simp result: " << LOG_FM(nn);
	return nn;
}

template <NodeType node>
size_t get_max_loopback_in_rr(tref form) {
	using tau = tree<node>;
	using tt = tau::traverser;
	size_t max = 0;
	for (tref offsets : tau::get(form).select_top(is<node, tau::offsets>)) {
		for (auto offset : (tt(offsets) || tau::offset)())
			if (auto i = offset | tau::integer; i) {
				int_t c = i | tt::integer;
				max = std::max(max, static_cast<size_t>(c));
			}
	}
	return max;
}

template <NodeType node>
tref build_shift_from_shift(tref shift, size_t step) {
	using tau = tree<node>;
	using tt = tau::traverser;
	auto num = tt(shift) | tau::num;
	auto offset = num | tt::num;
	if (step == offset) return tt(shift) | tau::capture | tt::ref;
	subtree_map<node, tref> changes{
		{ num.value(), tau::get_num(step - offset) }};
	return rewriter::replace<node>(shift, changes);
}

template <NodeType node>
tref build_main_step(tref form, size_t step) {
	using tau = tree<node>;
	using tt = tau::traverser;
	subtree_map<node, tref> changes;
	for (tref offsets : tau::get(form).select_top(is<node, tau::offsets>)) {
		auto shift = tt(offsets) | tau::shift;
		if (!shift.has_value()) continue;
		auto nshift = build_shift_from_shift<node>(shift.value(), step);
		changes[shift.value()] = nshift;
	}
	return rewriter::replace<node>(form, changes);
}

// Normalizes a Boolean function having no recurrence relation
template <NodeType node>
tref bf_normalizer_without_rec_relation(tref bf) {
	LOG_DEBUG << "Begin Boolean function normalizer";

	auto result = bf_boole_normal_form<node>(bf);

	LOG_DEBUG << "End Boolean function normalizer";

	return result;
}

// Normalizes a Boolean function in which recurrence relations are present
template <NodeType node>
tref bf_normalizer_with_rec_relation(const rr<node> &bf) {
	using tt = typename tree<node>::traverser;
	rr<node> rr_ = transform_ref_args_to_captures<node>(bf);
	LOG_DEBUG << "Begin calculate recurrence relation";
	auto main = calculate_all_fixed_points<node>(rr_);
	if (!main) return nullptr;
	tref bf_unfolded = tt(main) | repeat_all<node, step<node>>(
					step<node>(rr_.rec_relations)) |tt::ref;
	LOG_DEBUG << "End calculate recurrence relation";

	LOG_DEBUG << "Begin Boolean function normalizer";
	auto result = bf_boole_normal_form<node>(bf_unfolded);
	LOG_DEBUG << "End Boolean function normalizer";

	return result;
}

// enumerates index in main with step i - used for finding a fixed point
template <NodeType node>
tref build_enumerated_main_step(tref form, size_t i, size_t offset_arity) {
	using tau = tree<node>;
	LOG_TRACE << "Begin build_enumerated_main_step: " << LOG_FM_DUMP(form)
		<< " step: " << i << " offset arity: " << offset_arity;
	subtree_map<node, tref> changes;
	trefs ofs; // create offsets node
	ofs.push_back(tau::get(tau::offset, tau::get_integer(i)));
	for (size_t o = 1; o < offset_arity; ++o)
		ofs.push_back(tau::get(tau::offset, tau::get_integer(0)));

	// create enumerated replacement
	const auto& t = tau::get(form)[0][0];
	LOG_TRACE << "t: " << LOG_FM_DUMP(t.get());
	LOG_TRACE << "t.value: " << t.value;
	LOG_TRACE << "t[0]: " << LOG_FM(t.first());
	LOG_TRACE << "t[1]: " << LOG_FM(t.second());
	changes[t.get()] = tau::get(t.value,
		{ t.first(), tau::get(tau::offsets, ofs), t.second() },
		t.right_sibling());
	form = rewriter::replace<node>(form, changes);
	LOG_DEBUG << "(F*) " << LOG_FM(form);
	return build_main_step<node>(form, i);
}

template <NodeType node>
bool is_valid(const rr<node>& nso_rr) {
	using tau = tree<node>;
	LOG_TRACE << "-- is_valid: " << LOG_RR(nso_rr);
	for (tref main_offsets : tau::get(nso_rr.main)
		.select_all(is<node,tau::offsets>)) if (tau::get(main_offsets)
			.find_top(is<node, tau::capture>))
	{
		LOG_ERROR << "Main " << TAU_TO_STR(nso_rr.main->get())
			<< " cannot contain a relative offset "
			<< TAU_TO_STR(main_offsets);
		return false; // capture in main's offset
	}
	for (size_t ri = 0; ri != nso_rr.rec_relations.size(); ++ri) {
		const auto& r = nso_rr.rec_relations[ri];
		auto left = get_ref_info<node>(get_ref<node>(r.first->get()));
		for (const auto& [ot, _] : left.second)
			if (ot == tau::shift) {
				LOG_ERROR << "Recurrence relation "
					<< r.first->get() << " cannot contain "
					<< "an offset shift";
				return false; // head ref cannot have shift
			}
		if (left.second.size() == 0) continue; // no offsets
		// take only first offset for consideration
		offset_t ho = left.second.front();
		LOG_TRACE << "head offset " << LOG_NT(ho.first) << " / " << ho.second;
		for (tref ref : tau::get(r.second)
			.select_all(is<node, tau::ref>))
		{
			auto right = get_ref_info<node>(ref);
			if (right.second.size() == 0) continue; // no offsets
			auto& bo = right.second.front();
			LOG_TRACE << "body offset " << LOG_NT(bo.first) << " / " << bo.second;
			if (ho.first == tau::integer) {
				if (bo.first == tau::capture) {
					LOG_ERROR << "Recurrence relation "
						<< r.first << " (having a fixed"
						" offset) cannot depend on a "
						"relative offset " << r.second;
					return false; // left num right capture
				}
				if (bo.first == tau::integer
					&& ho.second < bo.second)
				{
					LOG_ERROR << "Recurrence relation "
						<< r.first << " cannot depend "
						<< "on a future state "
						<< r.second;
					return false; // l num < r num
				}
			}
		}
	}
	LOG_TRACE << "-- Recurrence relation is valid";
	return true;
}

template <NodeType node>
bool is_well_founded(const rr<node>& nso_rr) {
	using tau = tree<node>;
	LOG_TRACE << "-- is_well_founded: " << LOG_RR(nso_rr);
	std::unordered_map<rr_sig, std::set<rr_sig>> graph;
	std::unordered_map<rr_sig, bool> visited, visiting;
	std::function<bool(rr_sig)> is_cyclic = [&](const rr_sig& sig) {
		if (visiting[sig]) return true;
		if (visited[sig]) return false;
		visiting[sig] = true;
		for (const auto& neighbor : graph[sig])
			if (is_cyclic(neighbor)) return true;
		visiting[sig] = false;
		visited[sig]  = true;
		return false;
	};
	bool has_relative_rule = false;
	for (size_t ri = 0; ri != nso_rr.rec_relations.size(); ++ri) {
		LOG_TRACE <<"rec relation "<<LOG_RULE(nso_rr.rec_relations[ri]);
		const auto& r = nso_rr.rec_relations[ri];
		auto left = get_ref_info<node>(get_ref<node>(r.first->get()));
		for (const auto& [ot, _] : left.second)
			if (ot == tau::capture)
				has_relative_rule = true;
		if (left.second.size() == 0) continue; // no offsets
		// take only first offset for consideration
		offset_t ho = left.second.front();
		LOG_TRACE << "head offset " << LOG_NT(ho.first) << " / " << ho.second;
		for (const auto& ref : tau::get(r.second).select_all(
			is<node, tau::ref>))
		{
			auto right = get_ref_info<node>(ref);
			if (right.second.size() == 0) continue; // no offsets
			auto& bo = right.second.front();
			LOG_TRACE << "body offset " << LOG_NT(bo.first) << " / " << bo.second;
			if (ho == bo) graph[left.first].insert(right.first);
		}
		visited[left.first]  = false;
		visiting[left.first] = false;
	}
	if (!has_relative_rule) {
		LOG_ERROR << "Recurrence relation has no rules"
			  << " other than initial conditions";
		return false;
	}
	for (const auto& [left, _] : graph)
		if (!visited[left] && is_cyclic(left)) {
			LOG_ERROR << "Recurrence relation is cyclic";
			return false;
		}
	LOG_DEBUG << "Recurrence relation is well founded";
	return true;
}

template <NodeType node>
tref calculate_fixed_point(const rr<node>& nso_rr,
	tref form, typename node::type nt, size_t offset_arity,
	tref fallback)
{
	using tau = tree<node>;
	LOG_DEBUG << "Calculating fixed point: " << LOG_FM(form);
	LOG_DEBUG << "Spec: " << LOG_RR(nso_rr);
	//ptree<BAs...>(std::cout << "form: ", form) << "\n";

	auto ft = tau::get(fallback).get_type();
	bool first = ft == tau::first_sym, last = ft == tau::last_sym;
	if (!first && !last && ft != nt) {
		LOG_ERROR << "Fallback type mismatch";
		return nullptr;
	}

	if (!is_well_founded<node>(nso_rr)) return nullptr;

	trefs previous;
	tref current;

	size_t max_loopback = 0;
	std::vector<size_t> loopbacks;
	for (const auto& r : nso_rr.rec_relations) {
		size_t loopback = std::max(
			get_max_loopback_in_rr<node>(r.first->get()),
			get_max_loopback_in_rr<node>(r.second->get()));
		loopbacks.push_back(loopback);
		max_loopback = std::max(max_loopback, loopback);
	}
	LOG_DEBUG << "max loopback " << max_loopback;

	for (size_t i = max_loopback; ; i++) {
		current = build_enumerated_main_step<node>(
							form, i, offset_arity);
		bool changed;
		do { // apply rec relation rules and check for cycle dependency
			changed = false;
			for (size_t ri = 0;
				ri != nso_rr.rec_relations.size(); ++ri)
			{
				const auto& r = nso_rr.rec_relations[ri];
				if (loopbacks[ri] > i) {
					// LOG_DEBUG << "(I) -- current step " << i << " < " << loopbacks[ri] << " loopback, skipping " << r;
					continue; // skip steps depending on future fixed offsets
				}
				auto prev = current;
				current = nso_rr_apply<node>(r, prev);
				if (current != prev) changed = true;
			}
		} while (changed);

		LOG_DEBUG << "Begin enumeration step";
		LOG_DEBUG << "current: " << LOG_FM(current);

		LOG_DEBUG << "Normalize step";
		current = nt == tau::wff ? normalizer_step<node>(current)
					 : bf_boole_normal_form<node>(current);
		LOG_DEBUG << "Normalized step";
		LOG_DEBUG << "current: " << LOG_FM(current);

		if (previous.size() && (nt == tau::wff
			? are_nso_equivalent<node>(current, previous.back())
			: are_bf_equal<node>(current, previous.back())))
		{
			LOG_DEBUG << "End enumeration step: fixed point"
						<< " found at step: " << i;
			LOG_DEBUG << "previous.back(): " << LOG_FM(previous.back());
			return previous.back();
		}
		else if (previous.size() > 1 && (nt == tau::wff
			? is_nso_equivalent_to_any_of<node>(current, previous)
			: is_bf_same_to_any_of<node>(current, previous)))
		{
			LOG_DEBUG << "End enumeration step - loop "
				<< "(no fixed point) detected at step: "
				<< i << " returning fallback "
				<< (first ? "first" : last ? "last" : "");

			if (last) return previous.back();
			if (first) return current;
			LOG_DEBUG << "End enumeration step - fallback: "
							<< LOG_FM(fallback);
			return fallback;
		}
		LOG_DEBUG << "End enumeration step - no fixed point resolution "
			<< "at step: " << i << " incrementing";
		LOG_DEBUG << "current: " << LOG_FM(current);
		previous.push_back(current);
	}
	DBG(assert(0);)
	return nullptr;
}

// calculate fixed points called from main and replace them by their results
template <NodeType node>
struct fixed_point_transformer {
	using tau = tree<node>;
	using tt = tau::traverser;
	using type = typename node::type;

	fixed_point_transformer(const rr<node>& defs,
		const ref_types<node>& types) : defs(defs), types(types) {}

	tref operator()(tref n) {
		const auto& t = tau::get(n);
		if (!t.has_child()) return n;
		if (auto it = changes.find(n); it != changes.end())
			return it->second;
		tref ref = t.first();
		bool is_ref = (t.is(tau::wff) && is<node, tau::wff_ref>(ref))
			|| (t.is(tau::bf) && is<node, tau::bf_ref>(ref));
		if (!is_ref) return n;
		auto sig = get_rr_sig<node>(ref);
		auto typopt = types.get(sig);
		if (!typopt) { // this should not happen if rr_types.ok()
			LOG_ERROR << "Unresolved type of " << sig;
			return nullptr;
		}
		if (auto fpopt = types.fpcall(sig); fpopt) { // is fp call
			auto offset_arity = fpopt.value().offset_arity;
			// TODO we don't support FP calc for multiindex offsets yet
			if (offset_arity > 1) {
				LOG_ERROR << "Fixed point"
					" calculation of multiindex offset "
					"relations is not supported yet";
				return nullptr;
			}
			auto typ = typopt.value();
			auto fp = calculate_fixed_point<node>(defs, n, typ,
				offset_arity, get_fallback(typ, ref));
			if (!fp) return nullptr;
			return changes.emplace(n, fp).first->second;
		}
		bool changed = false;
		trefs ch;
		if (changes.contains(ref))
			changed = true, ch.push_back(changes[ref]);
		else ch.push_back(ref);
		auto nn = tau::get(t.value, ch);
		if (changed) changes[n] = nn;
		return nn;
	}

	tref get_fallback(type nt, tref ref) {
		auto fallback = tt(ref) | tau::ref | tau::fp_fallback;
		if (!fallback) return nt == tau::wff
			? tau::_F() : tau::_0();
		return fallback | tt::only_child | tt::ref;
	}

	subtree_map<node, tref> changes;
	rr<node> defs;
	ref_types<node> types;
};

template <NodeType node>
tref calculate_all_fixed_points(const rr<node>& nso_rr) {
	// get types and do type checks and validation
	ref_types<node> types(nso_rr);
	if (!types.ok() || !is_valid<node>(nso_rr)) return nullptr;
	// transform fp calculation calls by calculation results
	fixed_point_transformer<node> fpt(nso_rr, types);
	// auto all = [](tref) -> bool { return true; };
	tref new_main = rewriter::post_order_traverser<node, decltype(fpt),
		decltype(all)>(fpt, all)(nso_rr.main->get());
	if (!new_main) return nullptr;
	if (fpt.changes.size()) {
		new_main = rewriter::replace<node>(new_main, fpt.changes);
		LOG_DEBUG << "Calculated fixed points.";
		LOG_DEBUG << "New main: " << LOG_FM(new_main);
	}
	return new_main;
}

// This function applies the recurrence relations the formula comes with to
// the formula
template <NodeType node>
tref apply_rr_to_formula(const rr<node>& nso_rr) {
	using tt = typename tree<node>::traverser;
	LOG_DEBUG << "Start apply_rr_to_formula";
	LOG_DEBUG << "Spec: " << LOG_RR(nso_rr);
	rr<node> rr_ = transform_ref_args_to_captures<node>(nso_rr);
	tref main = calculate_all_fixed_points<node>(rr_);
	if (!main) return nullptr;
	// Substitute function and recurrence relation definitions
	tref new_main = main
		| repeat_all<node, step<node>>(step<node>(rr_.rec_relations))
		| tt::ref;
	LOG_DEBUG << "End apply_rr_to_formula";
	LOG_DEBUG << "Spec: " << LOG_RR(nso_rr);
	return new_main;
}

// REVIEW (HIGH) review overall execution
template <NodeType node>
tref normalizer(const rr<node>& nso_rr) {
	// IDEA extract this to an operator| overload

	LOG_DEBUG << "Begin normalizer";
	LOG_DEBUG << "Spec: " << LOG_RR(nso_rr);

	auto fm = apply_rr_to_formula<node>(nso_rr);
	if (!fm) return nullptr;
	tref res = normalize_with_temp_simp<node>(fm);

	LOG_DEBUG << "End normalizer";
	LOG_DEBUG << "Result: " << LOG_FM(res);
	return res;
}

template <NodeType node>
tref normalizer(tref form) {
	return normalize_with_temp_simp<node>(form);
}

} // namespace idni::tau_lang
