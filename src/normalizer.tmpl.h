// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "normalizer.h"
#include "normal_forms.h"
#include "definitions.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "normalizer"

namespace idni::tau_lang {

/**
 * @brief Descriptor of a single reference offset.
 *
 * The first element identifies the offset kind (one of `tau_parser::nonterminal`
 * values: `num`, `capture`, `shift`, or `variable`). The second element holds
 * the numeric value for `num`, the `rr_dict` identifier for `capture`, or 0 for
 * `shift` and `variable`.
 */
using offset_t = std::pair<tau_parser::nonterminal, size_t>;

// IDEA (HIGH) rewrite steps as a tuple to optimize the execution
/** @internal @copydoc normalize */
template <NodeType node>
tref normalize(tref form) {
	using tau = tree<node>;
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, tref>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	if (auto it = cache.find(form); it != cache.end()) return it->second;
#endif // TAU_CACHE
	// First resolve quantifiers in formulas below temporal quantifiers
	auto st_aw = [](tref n) {
		return is_child<node>(n, tau::wff_sometimes)
			|| is_child<node>(n, tau::wff_always);
	};
	trefs temps = tau::get(form).select_top(st_aw);
	// Case that the formula has no temporal quantifier
	if (temps.empty()) {
		// Resolve closed quantified bv subformulas before anti_prenex_block:
		// pushing the quantifiers of a blasted bitvector formula through
		// the Boolean normalization is exponential, while the solver
		// decides the closed formula directly.
		form = resolve_quantifiers<node>(form);
		form = anti_prenex_block<node>(form);
		form = resolve_quantifiers<node>(form);
	} else {
		subtree_map<node, tref> changes;
		for (tref temp : temps) {
			bool is_aw = is_child<node>(temp, tau::wff_always);
			// Remove temporal quantifier
			tref f = tau::trim2(temp);
			f = resolve_quantifiers<node>(f);
			f = anti_prenex_block<node>(f);
			f = resolve_quantifiers<node>(f);
			// Add quantifier again and save as change
			if (is_aw) changes.emplace(temp, tau::build_wff_always(f));
			else changes.emplace(temp, tau::build_wff_sometimes(f));
		}
		form =  rewriter::replace(form, changes);
	}
	// Now normalize the temporal layer and convert the formulas below the temporal
	// quantifiers to Boole normal form
	tref result = normalize_temporal_quantifiers<node>(form);
#ifdef TAU_CACHE
	cache.emplace(form, result);
#endif // TAU_CACHE
	return result;
}

// Assumes that the formula passed does not have temporal quantifiers
// This normalization will non perform the temporal normalization
/** @internal @copydoc normalize_non_temp */
template <NodeType node>
tref normalize_non_temp(tref fm) {
	using tau = tree<node>;
	using tt = tau::traverser;
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, tref>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	if (auto it = cache.find(fm); it != cache.end()) return it->second;
#endif // TAU_CACHE
	tref result = tt(fm)
		// Resolve closed quantified bv subformulas first (see normalize),
		// then push all quantifiers in, eliminate them and normalize result
		| tt::f(resolve_quantifiers<node>)
		| tt::f(anti_prenex_block<node>)
		| tt::f(resolve_quantifiers<node>)
		| tt::f(term_boole_normal_form<node>)
		| tt::ref;
	// NOTE: Do NOT add fold_trivial_quantifiers or reget here.
	// tau::reget strips the explicit bitwidth subtype from BV-typed nodes
	// (io_vars and BV constants) causing get_bv_size assertions downstream.
	// Residual trivial quantifiers are folded by normalize_with_temp_simp
	// (which already calls fold_trivial_quantifiers after normalize).
#ifdef TAU_CACHE
	cache.emplace(fm, result);
#endif // TAU_CACHE
	return result;
}

/** @brief Collects all variable and capture nodes from tree node @p n using `select_top`.
 *  @internal
 *  @tparam node Tree node type.
 *  @param n The root node to search within.
 *  @return A flat list of all variable and capture sub-nodes found.
 */
template <NodeType node>
trefs get_vars_from_nso(tref n) {
	return tree<node>::get(n).select_top(is_var_or_capture<node>());
}

/** @brief Returns a fresh integer `i` such that variable `x_i` does not already appear in @p fm.
 *  @internal
 *
 *  Scans all variable and capture nodes whose names start with `x` or `?x`,
 *  collects their numeric suffixes, and returns one past the maximum found.
 *  @tparam node Tree node type.
 *  @param fm The formula to scan for existing variable identifiers.
 *  @return An integer identifier not yet used by any variable in @p fm.
 */
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
/** @internal @copydoc get_new_uninterpreted_constant */
template <NodeType node>
tref get_new_uninterpreted_constant(tref fm, const std::string& name, size_t type) {
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
	tref uninter_const = tau::build_bf_uconst("", name + id, type);
	return uninter_const;
}

/** @brief Extracts reference metadata from a `ref` node.
 *  @internal
 *
 *  Returns a pair of `rr_sig` (name id and arity) and a vector of `offset_t`
 *  values. Currently only the first offset is considered.
 *  @tparam node Tree node type.
 *  @param ref The `ref` tree node to extract information from.
 *  @return A pair of the recurrence relation signature and its offset list.
 */
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

/** @brief Traverses @p n downward through single-child nodes to find a `ref` node.
 *  @internal
 *  @tparam node Tree node type.
 *  @param n The node to start the search from.
 *  @return The first `ref` node found, or `nullptr` if none exists.
 */
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
/** @internal @copydoc has_no_boolean_combs_of_models */
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

/** @internal @copydoc is_non_temp_nso_satisfiable */
template <NodeType node>
bool is_non_temp_nso_satisfiable(tref n) {
	using tau = tree<node>;

	DBG(assert(n != nullptr));

	const auto& fm = tau::get(n);
	DBG(assert(!fm.find_top(is<node, tau::wff_always>));)
	DBG(assert(!fm.find_top(is<node, tau::wff_sometimes>));)
	tref nn = n;
	const trefs& vars = fm.get_free_vars();
	nn = tau::build_wff_ex_many(vars, nn);
	tref normalized = normalize_non_temp<node>(nn);
	const auto& t = tau::get(normalized);

	DBG(LOG_TRACE << "is_non_temp_nso_satisfiable/normalized: "
		  << LOG_FM(normalized);)

	DBG(assert((t.equals_T() || t.equals_F()
		|| t.find_top(is<node, tau::constraint>)));)

	return t.equals_T();
}

/** @brief Checks whether a non-temporal NSO formula is unsatisfiable.
 *  @internal
 *
 *  Wraps free variables with existential quantifiers, normalizes via
 *  `normalize_non_temp`, and returns `true` if the result is `F`.
 *  @tparam node Tree node type.
 *  @param n The non-temporal formula to test.
 *  @return `true` if the formula is unsatisfiable, `false` otherwise.
 */
template <NodeType node>
bool is_non_temp_nso_unsat(tref n) {
	using tau = tree<node>;
	DBG(assert(n != nullptr));
	DBG(assert(!tau::get(n).find_top(is<node, tau::wff_always>));)
	DBG(assert(!tau::get(n).find_top(is<node, tau::wff_sometimes>));)

	tref nn = n;
	const trefs& vars = get_free_vars<node>(nn);
	nn = tau::build_wff_ex_many(vars, nn);
	tref normalized = normalize_non_temp<node>(nn);
	const auto& t = tau::get(normalized);
	assert((t.equals_T() || t.equals_F()
		|| t.find_top(is<node, tau::constraint>)));
	return t.equals_F();
}

/** @internal @copydoc are_nso_equivalent */
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

	tref imp1 = tau::build_wff_imply(n1, n2);
	tref imp2 = tau::build_wff_imply(n2, n1);
	const trefs& vars = get_free_vars<node>(tau::build_wff_and(n1, n2));
	DBG(for (tref v : vars) LOG_DEBUG << "var: " << LOG_FM(v);)
	imp1 = tau::build_wff_all_many(vars, imp1);
	imp2 = tau::build_wff_all_many(vars, imp2);

	LOG_DEBUG << "wff: " << LOG_FM(tau::build_wff_and(imp1, imp2));

	const tau& tdir1 = tau::get(normalize_non_temp<node>(imp1));
	DBG(assert((tdir1.equals_T() || tdir1.equals_F()
		|| tdir1.find_top(is<node, tau::constraint>)));)
	if (tdir1.equals_F()) {
		LOG_DEBUG << "End are_nso_equivalent: " << LOG_FM(tdir1.get());
		return false;
	}
	const tau& tdir2 = tau::get(normalize_non_temp<node>(imp2));
	DBG(assert((tdir2.equals_T() || tdir2.equals_F()
		|| tdir2.find_top(is<node, tau::constraint>))));
	const bool res = (tdir1.equals_T() && tdir2.equals_T());
	LOG_DEBUG << "End are_nso_equivalent: " << res;
	return res;
}

/** @brief Returns `true` if @p n is logically equivalent to any formula in @p previous.
 *  @internal
 *
 *  Equivalence is checked via `are_nso_equivalent`.
 *  @tparam node Tree node type.
 *  @param n The formula to test.
 *  @param previous The list of candidate formulas to compare against.
 *  @return `true` if @p n is equivalent to at least one formula in @p previous.
 */
template <NodeType node>
bool is_nso_equivalent_to_any_of(tref n, trefs& previous) {
	return std::any_of(previous.begin(), previous.end(), [n] (tref& p) {
			return are_nso_equivalent<node>(n, p);
		});
}

/** @internal @copydoc is_nso_impl */
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

	tref imp = tau::build_wff_imply(n1, n2);
	const trefs& vars = get_free_vars<node>(imp);
	imp = tau::build_wff_all_many(vars, imp);

	LOG_DEBUG << "wff: " << LOG_FM(imp);

	const tau& res = tau::get(normalize_non_temp<node>(imp));
	DBG(assert((res.equals_T() || res.equals_F()
		|| res.find_top(is<node, tau::constraint>)));)
	LOG_DEBUG << "End is_nso_impl: " << res.get();
	return res.equals_T();
}

/** @brief Checks whether two Boolean functions @p n1 and @p n2 are equal.
 *  @internal
 *
 *  Builds `(n1 XOR n2) = 0`, universally quantifies all free variables,
 *  normalizes, and returns `true` if the result is `T`.
 *  @tparam node Tree node type.
 *  @param n1 The first Boolean function.
 *  @param n2 The second Boolean function.
 *  @return `true` if @p n1 and @p n2 are semantically equal.
 */
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

	tref bf_equal_fm = tau::build_bf_eq_0(tau::build_bf_xor(n1, n2));
	const trefs& vars = get_free_vars<node>(bf_equal_fm);
	bf_equal_fm = tau::build_wff_all_many(vars, bf_equal_fm);
	LOG_TRACE << "wff: " << LOG_FM(bf_equal_fm);

	tref normalized = normalize_non_temp<node>(bf_equal_fm);
	LOG_TRACE << "Normalized: " << LOG_FM(normalized);

	auto check = tt(normalized) | tau::wff_t;
	LOG_DEBUG << "End are_bf_equal: " << check.has_value();
	return check.has_value();
}

/** @brief Returns `true` if Boolean function @p n is equal to any formula in @p previous.
 *  @internal
 *
 *  Equality is checked via `are_bf_equal`.
 *  @tparam node Tree node type.
 *  @param n The Boolean function to test.
 *  @param previous The list of candidate Boolean functions to compare against.
 *  @return `true` if @p n equals at least one formula in @p previous.
 */
template <NodeType node>
bool is_bf_same_to_any_of(tref n, trefs& previous) {
	return std::any_of(previous.begin(), previous.end(), [n](tref p) {
		return are_bf_equal<node>(n, p);
	});
}

/** @brief Applies registered function and predicate definitions to @p spec.
 *  @internal
 *
 *  Builds an `rr` with the global symbol definitions appended and unfolds it
 *  via `nso_rr_apply`. Returns @p spec unchanged if no `ref` nodes are present.
 *  @tparam node Tree node type.
 *  @param spec The specification formula to expand.
 *  @return The formula with all applicable definitions unfolded.
 */
template <NodeType node>
tref apply_defs_to_spec (tref spec) {
	using tau = tree<node>;
	rr<node> spec_with_defs {tau::geth(spec)};
	if (tau::get(spec).find_top(is<node, tau::ref>)) {
		const auto& defs = definitions<node>::instance().get_sym_defs();
		spec_with_defs.rec_relations.insert(spec_with_defs.rec_relations.end(),
		       defs.begin(), defs.end());
		return nso_rr_apply(spec_with_defs);
	}
	return spec;
}

// Folds quantifiers whose body is a constant: ex x T = all x T = T and
// ex x F = all x F = F. Such residues can be left behind by substitution
// based eliminations, which rebuild nodes without running the construction
// hooks.
/** @internal @copydoc fold_trivial_quantifiers */
template <NodeType node>
tref fold_trivial_quantifiers(tref fm) {
	using tau = tree<node>;
	auto f = [](tref n) -> tref {
		const auto& t = tau::get(n);
		if (!t.is(tau::wff)) return n;
		const auto& c = t[0];
		// ex x T/F → T/F, all x T/F → T/F
		if (c.is(tau::wff_ex) || c.is(tau::wff_all)) {
			tref body = c.second();
			if (tau::get(body).equals_T() || tau::get(body).equals_F())
				return body;
		}
		// Boolean identities: T/F with && and ||
		if (c.is(tau::wff_and)) {
			tref l = c.first(), r = c.second();
			if (tau::get(l).equals_T()) return r;
			if (tau::get(r).equals_T()) return l;
			if (tau::get(l).equals_F() || tau::get(r).equals_F())
				return tau::_F();
		}
		if (c.is(tau::wff_or)) {
			tref l = c.first(), r = c.second();
			if (tau::get(l).equals_T() || tau::get(r).equals_T())
				return tau::_T();
			if (tau::get(l).equals_F()) return r;
			if (tau::get(r).equals_F()) return l;
		}
		return n;
	};
	return post_order<node>(fm).apply_unique(f);
}

/** @brief Simplifies a single clause from the outer DNF of a normalized temporal formula.
 *  @internal
 *
 *  Removes `always`/`sometimes` sub-formulas that are implied by others in the
 *  same clause, and checks for unsatisfiable `always ∧ sometimes` pairs.
 *  @tparam node Tree node type.
 *  @param clause A conjunctive clause from the temporal DNF to simplify.
 *  @return The simplified clause, or `std::nullopt` if the clause is unsatisfiable.
 */
// Simplifies one temporal clause from the outer DNF of a normalized formula.
// Returns the simplified clause, or nullopt if the clause is unsatisfiable.
template<NodeType node>
std::optional<tref> simplify_temporal_clause(tref clause) {
	using tau = tree<node>;
	auto trim_q = [](tref n) {
		const auto& t = tau::get(n);
		if (t.child_is(tau::wff_sometimes)
			|| t.child_is(tau::wff_always)) return t[0].first();
		return n;
	};
	const auto& t = tau::get(clause);
	trefs aw_parts = t.select_top(is_child<node, tau::wff_always>);
	trefs st_parts = t.select_top(is_child<node, tau::wff_sometimes>);
	if ((aw_parts.size() == 1 && st_parts.empty()) ||
		(aw_parts.empty() && st_parts.size() == 1))
		return clause;

	// Replace all temporal parts with T to isolate the non-temporal skeleton.
	subtree_map<node, tref> changes;
	for (tref aw : aw_parts) changes.emplace(aw, tau::_T());
	for (tref st : st_parts) changes.emplace(st, tau::_T());
	tref new_clause = rewriter::replace<node>(clause, changes);
	DBG(LOG_TRACE << "    new clause: " << LOG_FM(new_clause);)

	// Eliminate parts in a group that are implied by another part in the same group.
	// repr(parts[i]) returns the formula to use for implication checking.
	auto eliminate_implied = [](trefs& parts, auto&& repr) {
		for (size_t i = 0; i < parts.size(); ++i)
			for (size_t j = i + 1; j < parts.size(); ++j) {
				if (is_nso_impl<node>(repr(parts[i]), repr(parts[j])))
					parts[j] = tau::_T();
				else if (is_nso_impl<node>(repr(parts[j]), repr(parts[i])))
					parts[i] = tau::_T();
			}
	};
	// Eliminate always parts implied by other always parts.
	eliminate_implied(aw_parts, [](tref x) { return x; });

	// Clause is unsatisfiable if any always ∧ sometimes pair is unsat.
	for (tref aw : aw_parts) for (tref st : st_parts) {
		tref f = tau::build_wff_and(trim_q(aw), trim_q(st));
		if (is_non_temp_nso_unsat<node>(f)) return std::nullopt;
	}

	// Eliminate sometimes parts implied by any always part.
	for (tref aw : aw_parts) for (tref& st : st_parts)
		if (is_nso_impl<node>(aw, trim_q(st))) st = tau::_T();

	// Eliminate sometimes parts implied by other sometimes parts.
	eliminate_implied(st_parts, trim_q);

	new_clause = tau::build_wff_and(new_clause, tau::build_wff_and(
				tau::build_wff_and(aw_parts),
				tau::build_wff_and(st_parts)));
	return new_clause;
}

/** @internal @copydoc normalize_with_temp_simp */
template <NodeType node>
tref normalize_with_temp_simp(tref fm) {
	using tau = tree<node>;
	fm = normalize<node>(fm);
	// Substitution based eliminations rebuild nodes without running the
	// construction hooks, so trivially foldable residues (constant
	// equations, quantifiers over T/F...) may survive; rebuild with hooks
	// and fold the remaining trivial quantifiers.
	fm = fold_trivial_quantifiers<node>(tau::reget(fm));
	// Residual quantified bv subformulas can survive the substitution
	// based eliminations (the closed formula check in resolve_quantifiers
	// runs before they are created); resolve them late and fold again.
	if (tau::get(fm).find_top(is_quantifier<node>)) {
		if (auto resolved = resolve_quantifiers<node>(fm);
			resolved && resolved != fm)
			fm = fold_trivial_quantifiers<node>(
				tau::reget(resolved));
	}
	// Apply present function/predicate definitions
	bool changed;
	do {
		changed = false;
		// Unresolved symbol is still present
		if (tau::get(fm).find_top(is<node, tau::ref>)) {
			tref resolved_red_fm = apply_defs_to_spec<node>(fm);
			if (tau::get(resolved_red_fm) != tau::get(fm)) {
				fm = normalize<node>(resolved_red_fm);
				changed = true;
			}
		}
	} while (changed);

	DBG(LOG_TRACE << "fm: " << LOG_FM(fm) << "\n";)
	if (tau::get(fm).equals_T() || tau::get(fm).equals_F())
		return fm;
	// If after normalization no temporal quantifier is present, the formula
	// is non-temporal
	if (!tau::get(fm).find_top(is_temporal_quantifier<node>))
		return fm;
	tref nn = tau::_F();
	// The temporal layer of a formula is in DNF
	for (tref clause : expression_paths<node>(fm)) {
		DBG(LOG_TRACE << "    clause: " << LOG_FM(clause);)
		if (auto simplified = simplify_temporal_clause<node>(clause)) {
			nn = tau::build_wff_or(nn, *simplified);
			DBG(LOG_TRACE << "    nn: " << LOG_FM(nn);)
		} else {
			DBG(LOG_TRACE << "    clause false";)
		}
	}
	DBG(assert(nn != nullptr);)
	DBG(LOG_TRACE << "normalize_with_temp_simp result: " << LOG_FM(nn);)
	return nn;
}

/** @brief Returns the maximum integer lookback offset found in `offsets` nodes of @p form.
 *  @internal
 *  @tparam node Tree node type.
 *  @param form The formula to scan for integer offset values.
 *  @return The largest integer offset value found, or 0 if none exist.
 */
template <NodeType node>
size_t get_max_lookback_in_rr(tref form) {
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

/** @brief Builds a new shift node from an existing `shift` reference adjusted to `step - offset`.
 *  @internal
 *
 *  Returns the capture node directly when `step == offset`.
 *  @tparam node Tree node type.
 *  @param shift The existing shift node whose numeric offset will be adjusted.
 *  @param step The target step value used to compute the new offset.
 *  @return A new shift node with the adjusted offset, or the capture node if `step == offset`.
 */
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

/** @brief Replaces all `shift` sub-nodes in @p form with adjusted shifts for the given @p step.
 *  @internal
 *
 *  Each shift is rewritten by `build_shift_from_shift` to reflect the current step.
 *  @tparam node Tree node type.
 *  @param form The recurrence main formula containing shift nodes.
 *  @param step The current enumeration step used to adjust each shift offset.
 *  @return The formula with all shift nodes replaced by their step-adjusted equivalents.
 */
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

/** @brief Creates an enumerated (concrete) step of the recurrence main formula.
 *  @internal
 *
 *  Fixes the first offset to @p i and all remaining offsets to 0, then
 *  delegates to `build_main_step` for shift adjustment.
 *  @tparam node Tree node type.
 *  @param form The recurrence main formula to instantiate.
 *  @param i The concrete index to assign to the first offset.
 *  @param offset_arity The total number of offsets to generate.
 *  @return The main formula with the first offset set to @p i and the rest to 0.
 */
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

/** @brief Validates a recurrence relation.
 *  @internal
 *
 *  Checks that the main formula has no relative offsets, that no rule's head
 *  contains a shift offset, and that integer-indexed rules do not depend on
 *  future states.
 *  @tparam node Tree node type.
 *  @param nso_rr The recurrence relation to validate.
 *  @return `true` if all validity conditions are satisfied, `false` otherwise.
 */
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

/** @brief Checks that a recurrence relation is well-founded.
 *  @internal
 *
 *  Requires at least one relative (capture-offset) rule and verifies that the
 *  dependency graph among rule signatures is acyclic.
 *  @tparam node Tree node type.
 *  @param nso_rr The recurrence relation to check.
 *  @return `true` if the relation is well-founded, `false` otherwise.
 */
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

/** @brief Iterates the recurrence relation to find a fixed point starting from `max_lookback`.
 *  @internal
 *
 *  Applies all rules at each step until the result stabilizes (fixed point) or
 *  a loop is detected. Returns the fixed-point formula, or a fallback value when
 *  a loop is found.
 *  @tparam node Tree node type.
 *  @param nso_rr The recurrence relation driving the iteration.
 *  @param form The main formula template to enumerate.
 *  @param nt The non-terminal type (e.g. `wff` or `bf`) determining normalization and equivalence checks.
 *  @param offset_arity The number of offsets in the main formula's reference.
 *  @param fallback The formula to return when a loop (no fixed point) is detected.
 *  @return The fixed-point formula, or @p fallback if the iteration loops without converging.
 */
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

	size_t max_lookback = 0;
	std::vector<size_t> lookbacks;
	for (const auto& r : nso_rr.rec_relations) {
		size_t lookback = std::max(
			get_max_lookback_in_rr<node>(r.first->get()),
			get_max_lookback_in_rr<node>(r.second->get()));
		lookbacks.push_back(lookback);
		max_lookback = std::max(max_lookback, lookback);
	}
	LOG_DEBUG << "max lookback " << max_lookback;

	for (size_t i = max_lookback; ; i++) {
		current = build_enumerated_main_step<node>(
							form, i, offset_arity);
		bool changed;
		do { // apply rec relation rules and check for cycle dependency
			changed = false;
			for (size_t ri = 0;
				ri != nso_rr.rec_relations.size(); ++ri)
			{
				const auto& r = nso_rr.rec_relations[ri];
				if (lookbacks[ri] > i) {
					// LOG_DEBUG << "(I) -- current step " << i << " < " << lookbacks[ri] << " lookback, skipping " << r;
					continue; // skip steps depending on future fixed offsets
				}
				auto prev = current;
				current = nso_rr_apply<node>(r, prev);
				if (tau::get(current) != tau::get(prev)) changed = true;
			}
		} while (changed);

		LOG_DEBUG << "Begin enumeration step";
		LOG_DEBUG << "current: " << LOG_FM(current);

		LOG_DEBUG << "Normalize step";
		current = nt == tau::wff ? normalize<node>(current)
					 : bf_reduced_dnf<node>(current);
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

// Normalizes a Boolean function having no recurrence relation
/** @internal @copydoc bf_normalizer_without_rec_relation */
template <NodeType node>
tref bf_normalizer_without_rec_relation(tref bf) {
	using tau = tree<node>;
	LOG_DEBUG << "Begin Boolean function normalizer";

	bf = syntactic_path_simplification<node>::on(bf);
	tref result = bf_reduced_dnf<node>(bf);
	// Apply present function/predicate definitions
	bool changed;
	do {
		changed = false;
		// Unresolved symbol is still present
		if (tau::get(result).find_top(is<node, tau::ref>)) {
			result = syntactic_path_simplification<node>::on(result);
			auto resolved_res = apply_defs_to_spec<node>(result);
			if (resolved_res != result) {
				result = bf_reduced_dnf<node>(resolved_res);
				changed = true;
			}
		}
	} while (changed);

	LOG_DEBUG << "End Boolean function normalizer";

	return result;
}

// Normalizes a Boolean function in which recurrence relations are present
/** @internal @copydoc bf_normalizer_with_rec_relation */
template <NodeType node>
tref bf_normalizer_with_rec_relation(const rr<node> &bf) {
	tref bf_unfolded = nso_rr_apply<node>(bf);

	if (!bf_unfolded) return nullptr;

	LOG_DEBUG << "Begin Boolean function normalizer";
	auto result = bf_normalizer_without_rec_relation<node>(bf_unfolded);
	LOG_DEBUG << "End Boolean function normalizer";

	return result;
}

// REVIEW (HIGH) review overall execution
/** @internal @copydoc normalizer(const rr<node>&) */
template <NodeType node>
tref normalizer(const rr<node>& nso_rr) {
	// IDEA extract this to an operator| overload

	LOG_DEBUG << "Begin normalizer";
	LOG_DEBUG << "Spec: " << LOG_RR(nso_rr);

	tref fm = nso_rr_apply<node>(nso_rr);
	if (!fm) return nullptr;
	tref res = normalize_with_temp_simp<node>(fm);

	LOG_DEBUG << "End normalizer";
	LOG_DEBUG << "Result: " << LOG_FM(res);
	return res;
}

/** @internal @copydoc normalizer(tref) */
template <NodeType node>
tref normalizer(tref fm) {
	return normalize_with_temp_simp<node>(fm);
}

/**
 * @brief Converts the temporal layer of a formula to reduced DNF, squeezes the always
 * statements and ensures that formulas containing temporal variables are
 * explicitly quantified while non-temporal formulas are not quantified temporally.
 * @internal
 * @tparam node Tree node type
 * @tparam normalize_scopes If true, temporally quantified formulas are converted to Boole normal form
 * @param fm The formula that is to be temporally normalized
 * @return The resulting formula after normalizing the temporal quantifiers
 */
template <NodeType node, bool normalize_scopes>
tref normalize_temporal_quantifiers(tref fm) {
	using tau = tree<node>;
	auto norm = [](tref arg) {
		return normalize_scopes
					? term_boole_normal_form<node>(arg)
					: arg;
	};
	auto st_aw = [](tref n) {
		return is_child<node>(n, tau::wff_sometimes)
			|| is_child<node>(n, tau::wff_always);
	};
	auto rm_temp_quant = [&st_aw](tref n) {
		if (st_aw(n)) return tau::trim2(n);
		return n;
	};
	if (has_temp_var<node>(fm)) {
		const bool has_temp_quant = tau::get(fm).find_top(st_aw);
		if (has_temp_quant) {
			// By assumption, all temporal variables are explicitly
			// quantified by temporal quantifier without nesting.
			// DNF conversion is only done on temporal level
			fm = temporal_layer_to_dnf<node>(fm);
			// Simplify temporal layer
			fm = reduce<node>(fm);
			trefs clauses = get_dnf_wff_clauses<node>(fm);
			tref non_temp_clauses = tau::_F();
			tref res = tau::_F();
			for (tref clause : clauses) {
				if (!has_temp_var<node>(clause)) {
					// Remove all temporal quantifiers
					clause = pre_order<node>(clause).
							apply_unique(rm_temp_quant);
					non_temp_clauses = tau::build_wff_or(
						non_temp_clauses, clause);
					continue;
				}
				tref always_part = tau::_T();
				tref staying = tau::_T();
				// In each clause squeeze all always statements
				for (tref conj : get_cnf_wff_clauses<node>(clause)) {
					// All parts are temporally quantified
					DBG(assert(st_aw(conj) ||
						!has_temp_var<node>(conj));)
					if (!has_temp_var<node>(conj))
						always_part = tau::build_wff_and(
							always_part, rm_temp_quant(conj));
					// TODO: always conjunction is inefficient
					else if (!is_child<node>(conj, tau::wff_sometimes))
						always_part = always_conjunction<node>(
							always_part, conj);
					else staying = tau::build_wff_and(
						staying,
						tau::build_wff_sometimes(
							norm(tau::trim2(conj))));
				}
				always_part = tau::build_wff_always(norm(always_part));
				clause = tau::build_wff_and(always_part, staying);
				res = tau::build_wff_or(res, clause);
			}
			non_temp_clauses = tau::build_wff_always(
				norm(non_temp_clauses));
			res = tau::build_wff_or(res, non_temp_clauses);
			return res;
		} else {
			// Temporal variable without temporal quantifier
			// By assumption we quantify fm universally
			return build_wff_always<node>(norm(fm));
		}
	} else {
		// No temporal variable, so no temporal quantifier needed
		fm = pre_order<node>(fm).apply_unique(rm_temp_quant);
		return norm(fm);
	}
}

} // namespace idni::tau_lang
