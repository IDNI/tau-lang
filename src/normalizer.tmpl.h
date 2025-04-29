// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "normalizer.h"

namespace idni::tau_lang {

// // executes the normalizer on the given source code taking into account the
// // bindings provided.
// template <NodeType node>
// rr normalizer(std::string& source, bindings<BAs...>& binds) {
// 	auto form_source = make_tau_source(source, { .start = tau_parser::spec });
// 	auto form = make_nso_rr_using_bindings(form_source, binds);
// 	return normalizer(form);
// }

// // executes the normalizer on the given source code taking into account the
// // provided factory.
// template <typename factory_t, NodeType node>
// rr normalizer(std::string& source, factory_t& factory) {
// 	auto form_source = make_tau_source(source, { .start = tau_parser::spec });
// 	auto form = make_nso_rr_using_factory(form_source, factory);
// 	return normalizer(form);
// }

// IDEA (HIGH) rewrite steps as a tuple to optimize the execution
template <NodeType node>
tref normalizer_step(tref form) {
	using tau = tree<node>;
	using tt = tau::traverser;
	#ifdef TAU_CACHE_DEPRECIATING
	static unordered_tau_map<tref, BAs...> cache;
	if (auto it = cache.find(form); it != cache.end()) return it->second;
	#endif // TAU_CACHE_DEPRECIATING

	tref result = tt(form)
		// Push all quantifiers in and eliminate them
		| tt::f(eliminate_quantifiers<node>)
		// After removal of quantifiers, only subformulas previously under the scope of a quantifier
		// are reduced
		| bf_reduce_canonical<node>()
		// Normalize always and sometimes quantifiers and reduce Tau formula
		| sometimes_always_normalization<node>()
		| tt::ref;
	#ifdef TAU_CACHE_DEPRECIATING
	cache.emplace(form, result);
	#endif // TAU_CACHE_DEPRECIATING
	return result;
}

// Assumes that the formula passed does not have temporal quantifiers
// This normalization will non perform the temporal normalization
template <NodeType node>
tref normalize_non_temp(tref fm) {
	using tau = tree<node>;
	using tt = tau::traverser;
	#ifdef TAU_CACHE_DEPRECIATING
	static unordered_tau_map<tref, BAs...> cache;
	if (auto it = cache.find(fm); it != cache.end()) return it->second;
	#endif // TAU_CACHE_DEPRECIATING
	tref result = tt(fm)
		// Push all quantifiers in and eliminate them
		| tt::f(eliminate_quantifiers<node>)
		// After removal of quantifiers, only subformulas previously under the scope of a quantifier
		// are reduced
		| bf_reduce_canonical<node>()
		| tt::ref;
	result = reduce_across_bfs<node>(result, false);
	#ifdef TAU_CACHE_DEPRECIATING
	cache.emplace(fm, result);
	#endif // TAU_CACHE_DEPRECIATING
	return result;
}

template <NodeType node>
trefs get_vars_from_nso(tref n) {
	return tree<node>::get(n).select_top(is_var_or_capture<node>);
}

// Given a tref produce a number such that the variable x_i is
// neither a bool_variable nor a variable nor a capture
template <NodeType node>
int_t get_new_var_id(const tref fm) {
	trefs var_nodes = get_vars_from_nso<node>(fm);
	std::set vars{ 1 };
	for (tref var : var_nodes) {
		std::string nam = tree<node>::get(var).get_string();
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
tref get_new_uninterpreted_constant(const tref fm, const std::string& name) {
	using tau = tree<node>;
	trefs uninter_consts = tau::get(fm).select_top(is<node, tau::uconst_name>);
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
	//ptree<BAs...>(std::cout << "ref? ", ref) << "\n";
	std::pair<rr_sig, std::vector<offset_t>> ret{ get_rr_sig<node>(ref), {} };
	auto offsets = tt(ref) | tau::offsets || tau::offset;
	//BOOST_LOG_TRIVIAL(debug) << "(T) -- get_ref " << ref << " " << ret.first << " offsets.size: " << offsets.size();
	for (auto offset : offsets()) {
		auto t = offset | tt::only_child | tt::Tree;
		size_t nt = t.get_type();
		if (nt == tau::integer) ret.second.emplace_back(nt, t.data());
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
	assert((t == tau::get_T() || t == tau::get_F()
		|| t.find_top(is<node, tau::constraint>)));
	return t == tau::get_T();
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
	assert((t == tau::get_T() || t == tau::get_F()
		|| t.find_top(is<node, tau::constraint>)));
	return t == tau::get_F();
}

template <NodeType node>
bool are_nso_equivalent(tref n1, tref n2) {
	using tau = tree<node>;
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin are_nso_equivalent";
	BOOST_LOG_TRIVIAL(trace) << "(I) -- n1 " << tau::get(n1);
	BOOST_LOG_TRIVIAL(trace) << "(I) -- n2 " << tau::get(n2);

	// If this method is called on a formula that has Boolean combinations of models, it is used incorrectly
	DBG(assert((has_no_boolean_combs_of_models<node>(n1)
		&& has_no_boolean_combs_of_models<node>(n2)));)

	const auto& t1 = tau::get(n1);
	const auto& t2 = tau::get(n2);
	if (t1[0].is(tau::wff_always)) n1 = t1[0].first();
	if (t2[0].is(tau::wff_always)) n2 = t2[0].first();

	if (tau::get(n1) == tau::get(n2)) {
		BOOST_LOG_TRIVIAL(debug) << "(I) -- End are_nso_equivalent: true (equiv nodes)";
		return true;
	}

	// equivalence of references
	tref r1opt = get_ref<node>(n1), r2opt = get_ref<node>(n2);
	if (r1opt != nullptr && r2opt != nullptr) { // both are refs
		bool equiv = get_ref_info<node>(r1opt)
						== get_ref_info<node>(r2opt);
		BOOST_LOG_TRIVIAL(debug) << "(I) -- End are_nso_equivalent: "
						<< equiv << " (equiv refs)";
		return equiv;
	}
	else if (r1opt != nullptr || r2opt != nullptr) { // one is a ref
		BOOST_LOG_TRIVIAL(debug) << "(I) -- End are_nso_equivalent: "
						"false (ref and not ref)";
		return false;
	}

	auto vars = get_free_vars_from_nso<node>(n1);
	auto vars2 = get_free_vars_from_nso<node>(n2);
	vars.insert(vars2.begin(), vars2.end());

	tref imp1 = tau::build_wff_imply(n1, n2);
	tref imp2 = tau::build_wff_imply(n2, n1);

	for (tref v : vars) {
		imp1 = tau::build_wff_all(v, imp1);
		imp2 = tau::build_wff_all(v, imp2);
	}
	BOOST_LOG_TRIVIAL(debug)
		<< "(I) -- wff: " << tau::build_wff_and(imp1, imp2);

	const auto& tdir1 = tau::get(normalizer_step<node>(imp1));
	DBG(assert((tdir1 == tau::get_T() || tdir1 == tau::get_F()
		|| tdir1.find_top(is<node, tau::constraint>)));)
	if (tdir1 == tau::get_F()) {
		BOOST_LOG_TRIVIAL(debug) << "(I) -- End are_nso_equivalent: "
							<< tdir1;
		return false;
	}
	const auto& tdir2 = tau::get(normalizer_step<node>(imp2));
	DBG(assert((tdir2 == tau::get_T() || tdir2 == tau::get_F()
		|| tdir2.find_top(is<node, tau::constraint>))));
	bool res = (tdir1 == tau::get_T() && tdir2 == tau::get_T());

	BOOST_LOG_TRIVIAL(debug) << "(I) -- End are_nso_equivalent: " << res;

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
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin is_nso_impl";
	BOOST_LOG_TRIVIAL(trace) << "(I) -- n1 " << n1;
	BOOST_LOG_TRIVIAL(trace) << "(I) -- n2 " << n2;

	// If this method is called on a formula that has Boolean combinations of models, it is used incorrectly
	DBG(assert((has_no_boolean_combs_of_models<node>(n1)
		&& has_no_boolean_combs_of_models<node>(n2)));)

	const auto& t1 = tau::get(n1);
	const auto& t2 = tau::get(n2);

	if (t1[0].is(tau::wff_always)) n1 = t1[0].first();
	if (t2[0].is(tau::wff_always)) n2 = t2[0].first();

	if (tau::get(n1) == tau::get(n2)) {
		BOOST_LOG_TRIVIAL(debug) << "(I) -- End is_nso_impl: true (n1 implies n2)";
		return true;
	}

	auto vars  = get_free_vars_from_nso<node>(n1);
	auto vars2 = get_free_vars_from_nso<node>(n2);
	vars.insert(vars2.begin(), vars2.end());

	tref imp = tau::build_wff_imply(n1, n2);

	for (tref v : vars) {
		imp = tau::build_wff_all(v, imp);
	}
	BOOST_LOG_TRIVIAL(debug) << "(I) -- wff: " << tau::get(imp);

	const auto& res = tau::get(normalizer_step<node>(imp));
	DBG(assert((res == tau::_T() || res == tau::_F()
		|| res.find_top(is<node, tau::constraint>)));)
	BOOST_LOG_TRIVIAL(debug) << "(I) -- End is_nso_impl: " << res;
	return res == tau::get_T();
}

template <NodeType node>
bool are_bf_equal(tref n1, tref n2) {
	using tau = tree<node>;
	using tt = tau::traverser;
	
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin are_bf_equal";
	BOOST_LOG_TRIVIAL(trace) << "(I) -- n1 " << n1;
	BOOST_LOG_TRIVIAL(trace) << "(I) -- n2 " << n2;

	const auto& t1 = tau::get(n1);
	const auto& t2 = tau::get(n2);
	DBG(assert(t1.is(tau_parser::bf)));
	DBG(assert(t2.is(tau_parser::bf)));

	if (t1 == t2) {
		BOOST_LOG_TRIVIAL(debug) << "(I) -- End are_bf_equal: true (equal bf)";
		return true;
	}

	auto vars = get_free_vars_from_nso<node>(n1);
	auto vars2 = get_free_vars_from_nso<node>(n2);
	vars.insert(vars2.begin(), vars2.end());

	tref bf_equal_fm = tau::build_bf_eq(tau::build_bf_xor(n1, n2));

	for (tref v : vars) bf_equal_fm = tau::build_wff_all(v, bf_equal_fm);
	BOOST_LOG_TRIVIAL(trace) << "(I) -- wff: " << bf_equal_fm;

	tref normalized = normalizer_step<node>(bf_equal_fm);
	BOOST_LOG_TRIVIAL(trace) << "(T) -- Normalized: " << normalized;
	auto check = tt(normalized) | tau_parser::wff_t;
	BOOST_LOG_TRIVIAL(debug)
		<< "(I) -- End are_bf_equal: " << check.has_value();

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
	if (red_fm == tau::get_T() || red_fm == tau::get_F())
		return red_fm.get();
	trefs clauses = get_dnf_wff_clauses<node>(red_fm.get());
	tref nn = tau::_F();
	for (tref clause : clauses) {
		const auto& t = tau::get(clause);
		trefs aw_parts = t.select_top(is<node, tau::wff_always>);
		trefs st_parts = t.select_top(is<node, tau::wff_sometimes>);
		if (aw_parts.size() == 1 && st_parts.empty()) {
			nn = tau::build_wff_or(nn, clause);
			continue;
		}
		if (aw_parts.empty() && st_parts.size() == 1) {
			nn = tau::build_wff_or(nn, clause);
			continue;
		}

		// Replace always and sometimes parts by T
		typename tau::subtree_map changes;
		for (tref aw : aw_parts) changes.emplace(aw, tau::_T());
		for (tref st : st_parts) changes.emplace(st, tau::_T());
		tref new_clause = rewriter::replace<node>(clause, changes);

		// First check if any always statements are implied by others
		for (size_t i = 0; i < aw_parts.size(); ++i) {
			for (size_t j = i + 1; j < aw_parts.size(); ++j) {
				if (is_nso_impl<node>(aw_parts[i], aw_parts[j]))
					aw_parts[j] = tau::_T();
				else if (is_nso_impl<node>(aw_parts[j], aw_parts[i]))
					aw_parts[i] = tau::_T();
			}
		}
		// Check for unsat of always conjuncted with single sometimes part
		bool clause_false = false;
		for (tref aw : aw_parts) for (tref st : st_parts) {
			tref f = tau::build_wff_and(trim_q(aw), trim_q(st));
			if (is_non_temp_nso_unsat<node>(f)) clause_false = true;
		}
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
	}
	DBG(assert(nn != nullptr);)
	return nn;
}

template <NodeType node>
size_t get_max_loopback_in_rr(tref form) {
	using tau = tree<node>;
	using tt = tau::traverser;
	size_t max = 0;
	for (tref offsets : tau::get(form).select_top(is<node, tau::offsets>)) {
		for (auto offset : (tt(offsets) || tau_parser::offset)())
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
	typename tau::subtree_map changes{
		{ num.value(), tau::get_num(step - offset) }};
	return rewriter::replace<node>(shift, changes);
}

template <NodeType node>
tref build_main_step(tref form, size_t step) {
	using tau = tree<node>;
	using tt = tau::traverser;
	typename tau::subtree_map changes;
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
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin Boolean function normalizer";

	auto result = bf_boole_normal_form<node>(bf);

	BOOST_LOG_TRIVIAL(debug) << "(I) -- End Boolean function normalizer";

	return result;
}

// Normalizes a Boolean function in which recurrence relations are present
template <NodeType node>
tref bf_normalizer_with_rec_relation(const rr &bf) {
	BOOST_LOG_TRIVIAL(debug)<< "(I) -- Begin calculate recurrence relation";

	auto main = calculate_all_fixed_points<node>(bf);
	if (!main) return nullptr;

	auto bf_unfolded = main | repeat_all<node, step<node>>(
					step<node>(bf.rec_relations));

	BOOST_LOG_TRIVIAL(debug) << "(I) -- End calculate recurrence relation";

	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin Boolean function normalizer";

	auto result = bf_boole_normal_form<node>(bf_unfolded);

	BOOST_LOG_TRIVIAL(debug) << "(I) -- End Boolean function normalizer";

	return result;
}

// enumerates index in main with step i - used for finding a fixed point
template <NodeType node>
tref build_enumerated_main_step(tref form, size_t i, size_t offset_arity) {
	using tau = tree<node>;
	typename tau::subtree_map changes;
	trefs ofs; // create offsets node
	ofs.push_back(tau::get(tau::offset, tau::get_integer(i)));
	for (size_t o = 1; o < offset_arity; ++o)
		ofs.push_back(tau::get(tau::offset, tau::get_integer(0)));

	// create enumerated replacement
	const auto& t = tau::get(form).only_child_tree();
	changes[t.get()] = tau::get(t.value,
		{ t.first(), tau::get(tau::offsets, ofs), t.second() },
		t.right_sibling());
	form = rewriter::replace<node>(form, changes);
	BOOST_LOG_TRIVIAL(debug) << "(F*) " << tau::get(form);
	return build_main_step<node>(form, i);
}

template <NodeType node>
bool is_valid(const rr& nso_rr) {
	using tau = tree<node>;
	for (tref main_offsets : tau::get(nso_rr.main)
		.select_all(is<node,tau::offsets>)) if (tau::get(main_offsets)
			.find_top(is<node, tau::capture>))
	{
		BOOST_LOG_TRIVIAL(error) << "(Error) Main " << nso_rr.main
					<< " cannot contain a relative offset "
					<< main_offsets;
		return false; // capture in main's offset
	}
	for (size_t ri = 0; ri != nso_rr.rec_relations.size(); ++ri) {
		const auto& r = nso_rr.rec_relations[ri];
		auto left = get_ref_info<node>(get_ref<node>(r.first->get()));
		for (const auto& [ot, _] : left.second)
			if (ot == tau::shift) {
				BOOST_LOG_TRIVIAL(error) << "(Error) Recurrence "
					"relation " << r.first->get() << " cannot "
					"contain an offset shift";
				return false; // head ref cannot have shift
			}
		if (left.second.size() == 0) continue; // no offsets
		// take only first offset for consideration
		offset_t ho = left.second.front();
		//BOOST_LOG_TRIVIAL(debug) << "(T) -- head offset " << ho.first << " / " << ho.second;
		for (tref ref : tau::get(r.second)
			.select_all(is<node, tau::ref>))
		{
			auto right = get_ref_info<node>(ref);
			if (right.second.size() == 0) continue; // no offsets
			auto& bo = right.second.front();
			//BOOST_LOG_TRIVIAL(debug) << "(T) -- body offset " << bo.first << " / " << bo.second;
			if (ho.first == tau::integer) {
				if (bo.first == tau::capture) {
					BOOST_LOG_TRIVIAL(error)
						<< "(Error) Recurrence relation "
						<< r.first << " (having a fixed"
						" offset) cannot depend on a "
						"relative offset " << r.second;
					return false; // left num right capture
				}
				if (bo.first == tau::integer
					&& ho.second < bo.second) {
						BOOST_LOG_TRIVIAL(error)
							<<"(Error) Recurrence relation "
							<< r.first << " cannot "
							"depend on a future "
							"state " << r.second;
						return false; // l num < r num
				}
			}
		}
	}
	//BOOST_LOG_TRIVIAL(debug) << "(I) -- Recurrence relation is well founded";
	return true;
}

template <NodeType node>
bool is_well_founded(const rr& nso_rr) {
	using tau = tree<node>;
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
		const auto& r = nso_rr.rec_relations[ri];
		auto left = get_ref_info<node>(get_ref<node>(r.first->get()));
		for (const auto& [ot, _] : left.second)
			if (ot == tau::capture)
				has_relative_rule = true;
		if (left.second.size() == 0) continue; // no offsets
		// take only first offset for consideration
		offset_t ho = left.second.front();
		//BOOST_LOG_TRIVIAL(debug) << "(T) -- head offset " << ho.first << " / " << ho.second;
		for (const auto& ref : tau::get(r.second).select_all(
			is<node, tau::ref>))
		{
			auto right = get_ref_info<node>(ref);
			if (right.second.size() == 0) continue; // no offsets
			auto& bo = right.second.front();
			//BOOST_LOG_TRIVIAL(debug) << "(T) -- body offset " << bo.first << " / " << bo.second;
			if (ho == bo) graph[left.first].insert(right.first);
		}
		visited[left.first]  = false;
		visiting[left.first] = false;
	}
	if (!has_relative_rule) {
		BOOST_LOG_TRIVIAL(error) << "(Error) Recurrence relation has no rules "
						"other than initial conditions";
		return false;
	}
	for (const auto& [left, _] : graph)
		if (!visited[left] && is_cyclic(left)) {
			BOOST_LOG_TRIVIAL(error)
					<< "(Error) Recurrence relation is cyclic";
			return false;
		}
	BOOST_LOG_TRIVIAL(debug)<< "(I) -- Recurrence relation is well founded";
	return true;
}

template <NodeType node>
tref calculate_fixed_point(const rr& nso_rr,
	tref form, typename node::type nt, size_t offset_arity,
	tref fallback)
{
	using tau = tree<node>;
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Calculating fixed point: " << form;
	BOOST_LOG_TRIVIAL(debug) << "(F) " << to_str<node>(nso_rr);
	//ptree<BAs...>(std::cout << "form: ", form) << "\n";

	auto ft = tau::get(fallback).get_type();
	bool first = ft == tau::first_sym, last = ft == tau::last_sym;
	if (!first && !last && ft != nt) {
		BOOST_LOG_TRIVIAL(error) << "(Error) Fallback type mismatch";
		return nullptr;
	}

	if (!is_well_founded<node>(nso_rr)) return nullptr;

	trefs previous;
	tref current;
	auto eos = "(I) -- End enumeration step";

	size_t max_loopback = 0;
	std::vector<size_t> loopbacks;
	for (const auto& r : nso_rr.rec_relations) {
		size_t loopback = std::max(
			get_max_loopback_in_rr<node>(r.first->get()),
			get_max_loopback_in_rr<node>(r.second->get()));
		loopbacks.push_back(loopback);
		max_loopback = std::max(max_loopback, loopback);
	}
	BOOST_LOG_TRIVIAL(debug) << "(I) max loopback " << max_loopback;

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
					// BOOST_LOG_TRIVIAL(debug) << "(I) -- current step " << i << " < " << loopbacks[ri] << " loopback, skipping " << r;
					continue; // skip steps depending on future fixed offsets
				}
				auto prev = current;
				current = nso_rr_apply<node>(r, prev);
				if (current != prev) changed = true;
			}
		} while (changed);

		BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin enumeration step";
		BOOST_LOG_TRIVIAL(debug) << "(F) " << current;

		BOOST_LOG_TRIVIAL(debug) << "(I) -- Normalize step";
		current = nt == tau::wff ? normalizer_step<node>(current)
					: bf_boole_normal_form<node>(current);
		BOOST_LOG_TRIVIAL(debug) << "(T) -- Normalized step";
		BOOST_LOG_TRIVIAL(debug) << "(F) " << current;

		if (previous.size() && (nt == tau::wff
			? are_nso_equivalent<node>(current, previous.back())
			: are_bf_equal<node>(current, previous.back())))
		{
			BOOST_LOG_TRIVIAL(debug) << eos
				<< " - fixed point found at step: " << i;
			BOOST_LOG_TRIVIAL(debug) << "(F) " << previous.back();
			return previous.back();
		}
		else if (previous.size() > 1 && (nt == tau::wff
			? is_nso_equivalent_to_any_of<node>(current, previous)
			: is_bf_same_to_any_of<node>(current, previous)))
		{
			BOOST_LOG_TRIVIAL(debug) << eos
				<< " - loop (no fixed point) detected at step: "
				<< i << " returning fallback "
				<< (first ? "first" : last ? "last" : "");
			if (last) return previous.back();
			if (first) return current;
			BOOST_LOG_TRIVIAL(debug) << eos
				<< " - fallback: " << fallback;
			return fallback;
		}
		BOOST_LOG_TRIVIAL(debug) << eos
			<< " - no fixed point resolution at step: "
			<< i << " incrementing";
		BOOST_LOG_TRIVIAL(debug) << "(F) " << current;
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

	fixed_point_transformer(const rr& defs,
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
			BOOST_LOG_TRIVIAL(error)
				<< "(Error) Unresolved type of " << sig;
			return nullptr;
		}
		if (auto fpopt = types.fpcall(sig); fpopt) { // is fp call
			auto offset_arity = fpopt.value().offset_arity;
			// TODO we don't support FP calc for multiindex offsets yet
			if (offset_arity > 1) {
				BOOST_LOG_TRIVIAL(error) << "(Error) Fixed point"
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

	typename tau::subtree_map changes;
	rr defs;
	ref_types<node> types;
};

template <NodeType node>
tref calculate_all_fixed_points(const rr& nso_rr) {
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
		BOOST_LOG_TRIVIAL(debug) << "(I) -- Calculated fixed points. "
						"New main: " << new_main;
	}
	return new_main;
}

// This function applies the recurrence relations the formula comes with to
// the formula
template <NodeType node>
tref apply_rr_to_formula(const rr& nso_rr) {
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Start apply_rr_to_formula";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << to_str<node>(nso_rr);
	auto main = calculate_all_fixed_points<node>(nso_rr);
	if (!main) return nullptr;
	// Substitute function and recurrence relation definitions
	auto new_main = main
		| repeat_all<node, step<node>>(step<node>(nso_rr.rec_relations));
	BOOST_LOG_TRIVIAL(debug) << "(I) -- End apply_rr_to_formula";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << to_str<node>(nso_rr);
	return new_main;
}

// REVIEW (HIGH) review overall execution
template <NodeType node>
tref normalizer(const rr& nso_rr) {
	// IDEA extract this to an operator| overload

	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin normalizer";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << to_str<node>(nso_rr);

	auto fm = apply_rr_to_formula<node>(nso_rr);
	if (!fm) return nullptr;
	auto res = normalize_with_temp_simp<node>(fm);

	BOOST_LOG_TRIVIAL(debug) << "(I) -- End normalizer";
	return res;
}

template <NodeType node>
tref normalizer(tref form) {
	return normalize_with_temp_simp<node>(form);
}

} // namespace idni::tau_lang
