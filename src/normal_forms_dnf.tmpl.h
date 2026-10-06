// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// normal_forms_dnf.tmpl.h - DNF/CNF core: reduce_paths, bf_reduced_dnf, reduce
// Split from normal_forms.tmpl.h for readability.
//
// Path-vector encoding, shared by reduce_paths, join_paths,
// clause_to_vector, collect_paths, build_reduced_formula and
// dnf_cnf_to_reduced: a clause is a std::vector<int_t> indexed by the
// position of its BDD variable in `vars`, with
//     1  = the variable occurs positively,
//    -1  = negated,
//     2  = irrelevant (eliminated / don't-care).
// An EMPTY path denotes the constant clause (T in DNF, F in CNF); an EMPTY
// paths-VECTOR denotes the constant formula (F in DNF, T in CNF).
// dnf_cnf_to_reduced's {paths, vars} result flips both readings when
// `is_cnf` is set. `reduce` special-cases both empties on exit.

namespace idni::tau_lang {


// Merges the assignment `i` into `paths` by resolution, comparing the first
// `p` positions: a path that differs from `i` in exactly one decided
// position (and in no irrelevant one) is resolved on that position, which
// becomes 2, and the merged clause is reduced again recursively. `surface`
// is true when `i` is not yet in `paths`; a nested call empties the merged
// path instead, and the caller erases empty paths. If a merged clause
// becomes all-irrelevant, `paths` is cleared (the constant clause). Returns
// true when some merge happened, false when `i` must be added as a new path.
inline bool reduce_paths(std::vector<int_t>& i,
	std::vector<std::vector<int_t>>& paths, size_t p, bool surface = true)
{
	for (size_t j = 0; j < paths.size(); ++j) {
		if (paths[j].empty()) continue;
		// Get Hamming distance between i and path and position of last difference
		// while different irrelevant variables make assignments incompatible
		int_t dist = 0;
		size_t pos = 0;
		for (size_t k = 0; k < p; ++k) {
			if (i[k] == paths[j][k]) continue;
			else if (dist == 2) break;
			else if (i[k] == 2 || paths[j][k] == 2) { dist = 2; break; }
			else dist += 1, pos = k;
		}
		if (dist == 1) {
			// Remove i from paths if recursion depth is greater 0
			if(!surface) {
				paths[j] = {};
				// Resolve variable
				i[pos] = 2;
				if(std::ranges::all_of(i, [](const auto el) {return el == 2;}))
					return paths = {}, true;
				// Continue with resulting assignment
				reduce_paths(i, paths, p, false);
			} else {
				// Resolve variable
				paths[j][pos] = 2;
				if(std::ranges::all_of(paths[j], [](const auto el) {return el == 2;}))
					return paths = {}, true;
				// Continue with resulting assignment
				reduce_paths(paths[j], paths, p, false);
			}
			return true;
		}
	}
	return false;
}

// Simplifies `paths` in place: a path subsumed by another (the same decided
// literals plus further ones) is erased, and two paths differing in one
// decided literal, one subsuming the rest of the other, are resolved on it.
inline void join_paths(std::vector<std::vector<int_t>>& paths) {
	for (int_t i = 0; i < (int_t)paths.size(); ++i) {
		for (int_t j = 0; j < (int_t)paths.size(); ++j) {
			if (i == j) continue;
			// i and j stay signed for the erase offsets below, so each
			// subscript into paths is converted once here.
			const size_t i_pos = static_cast<size_t>(i),
				j_pos = static_cast<size_t>(j);
			int_t dist = 0;
			size_t pos = 0;
			bool subset_relation_decided = false, is_i_subset_of_j = true,
				subset_check = true, equal = true;
			for (size_t k=0; k < paths[i_pos].size(); ++k) {
			if (paths[i_pos][k] == paths[j_pos][k]) continue;
			else if (dist == 2) break;
			else if (paths[i_pos][k] == 2) {
				if (!subset_relation_decided) {
					subset_relation_decided = true;
					is_i_subset_of_j = true;
					if (paths[j_pos][k] != 2)
						equal = false;
				} else {
					if (!is_i_subset_of_j) {
						subset_check = false;
						break;
					}
					if (paths[j_pos][k] != 2)
						equal = false;
				}
			}
			else if (paths[j_pos][k] == 2) {
				if (!subset_relation_decided) {
					subset_relation_decided = true;
					is_i_subset_of_j = false;
					// paths[i][k] != 2
					equal = false;
				} else {
					// paths[i][k] != 2
					equal = false;
					if (is_i_subset_of_j) {
						subset_check = false;
						break;
					}
				}
			}
			else dist += 1, pos = k;
		}
		if (subset_check && dist == 1) {
			if (is_i_subset_of_j) {
				// Resovle variable in paths
				paths[j_pos][pos] = 2;
				if (equal) {
					paths.erase(paths.begin()+i);
					--i;
					break;
				}
			} else {
				// Resolve variable in i
				paths[i_pos][pos] = 2;
			}
		} else if (subset_check && dist == 0) {
			// True subset relation between i and j
			if (is_i_subset_of_j) {
				// i -> j
				paths.erase(paths.begin()+j);
				if (i > j) --i;
				--j;
			} else {
				paths.erase(paths.begin()+i);
				if (j > i) --j;
				--i;
				break;
			}
		}
		}
	}
}

// ------------------------------

// Starting from variable at position p+1 in vars write to i which variables are irrelevant in assignment:
// every var no longer selected by `is_var` in `fm` is set to 2.
template <NodeType node>
void elim_vars_in_assignment(tref fm, const auto& vars, auto& i,
	const size_t p, const auto& is_var)
{
	using tau = tree<node>;
	// auto is_var = [](tref n){return
	// 	is_child<node>(n, tau::variable) ||
	// 		is_child<node>(n, tau::uconst_name);};
	auto cvars = tau::get(fm).select_all(is_var);
	subtree_set<node> cur_vars(
		std::make_move_iterator(cvars.begin()),
		std::make_move_iterator(cvars.end()));

	for (size_t v_iter = p + 1; v_iter < vars.size(); ++v_iter)
		if (!cur_vars.contains(vars[v_iter])) i[v_iter] = 2;
}

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "assign_and_reduce"

// Enumerates the assignments of vars[p..] in `fm` (recording 1, -1 or 2 per
// position in `i`), and at each complete assignment normalizes the remaining
// coefficient and files its path under it in `dnf` (coefficient -> paths),
// merging by reduce_paths. A zero coefficient is dropped. `is_wff` selects
// T/F instead of 1/0. Returns true once some coefficient's paths reduce to
// the whole space (the formula is that coefficient alone), which stops the
// enumeration; the error of normalize_ba, to_dnf or reduce otherwise.
template <NodeType node>
result<bool> assign_and_reduce(tref fm, const trefs& vars, std::vector<int_t>& i,
	auto& dnf, const auto& is_var, size_t p, bool is_wff)
{
	using tau = tree<node>;
	result<bool> r;
#ifdef DEBUG
	LOG_TRACE << "Begin assign_and_reduce [" << LOG_NT_COLOR
		<< (is_wff ? "wff" : "bf") << TC.CLEAR() << "]: "
		<< LOG_FM_DUMP(fm);
	for (auto v : vars) LOG_TRACE << "v: " << TAU_TO_STR(v);
	LOG_TRACE << "p: " << p;
#endif // DEBUG
	auto report = [&](bool v) -> result<bool> {
		DBG(LOG_TRACE << (v ? "result is ok" : "no result")
				<< " for input: " << LOG_FM(fm);)
		return r.with_value(v);
	};

	// Check if all variables are assigned
	if(vars.size() == p) {
		tref fm_simp = nullptr;
		if (!is_wff) {
			// Do not add to dnf if the coefficient is 0
			if (tau::get(fm).equals_0()) return report(false);
			TAU_TRY(fm_simp, normalize_ba<node>(fm));
			DBG(LOG_TRACE << "normalize_ba result: " << LOG_FM(fm_simp);)
			if (tau::get(fm_simp).equals_0()) return report(false);
			TAU_TRY(fm_simp, (to_dnf<node, false>(fm_simp)));
			DBG(LOG_TRACE << "to_dnf result: " << LOG_FM(fm_simp);)
			if (tau::get(fm_simp).equals_0()) return report(false);
			TAU_TRY(fm_simp, reduce<node>(fm_simp));
			DBG(LOG_TRACE << "reduce result: " << LOG_FM(fm_simp);)
			if (tau::get(fm_simp).equals_0()) return report(false);
		} else {
			if (tau::get(fm).equals_F()) return report(false);
			// fm is a Tau formula
			TAU_TRY(fm_simp, (to_dnf<node, false>(fm)));
			DBG(LOG_TRACE << "to_dnf result: " << LOG_FM(fm_simp);)
			if (tau::get(fm_simp).equals_F()) return report(false);
			TAU_TRY(fm_simp, reduce<node>(fm_simp));
			DBG(LOG_TRACE << "reduce result: " << LOG_FM(fm_simp);)
			if (tau::get(fm_simp).equals_F()) return report(false);
		}
		if (std::ranges::all_of(i, [](const auto el){return el == 2;})){
			//bool t = is<node>(fm->child[0], tau::bf_t);
			dnf.emplace(fm_simp, std::vector(0, i));
			return r.with_value(true);
		}

		auto it = dnf.find(fm_simp);
		// p != 0 always here (the p == 0 arm returned above), so the vector
		// size is simply 1.
		if (it == dnf.end()) {
			dnf.emplace(fm_simp, std::vector(1, i));
			return report(false);
		}
		else if (!reduce_paths(i, it->second, p)) {
			// Place coefficient together with variable assignment if no reduction happend
			it->second.push_back(i);
		} else std::erase_if(it->second,
				[](const auto& v) { return v.empty(); });
		return r.with_value(it->second.empty());
	}
	// variable was already eliminated
	if (i[p] == 2) {
		TAU_TRY(auto sub, assign_and_reduce<node>(fm, vars, i, dnf, is_var,
						p + 1, is_wff));
		if (sub) return report(true);
		i[p] = 0;
		return report(false);
	}
	// Substitute 1 and 0 for v and simplify
	const auto& v = vars[p];
	tref t = is_wff ? tau::_T() : tau::_1(find_ba_type<node>(v));
	tref f = is_wff ? tau::_F() : tau::_0(find_ba_type<node>(v));
	tref fm_v1 = rewriter::replace<node>(fm, v, t);
	tref fm_v0 = rewriter::replace<node>(fm, v, f);

	elim_vars_in_assignment<node>(fm_v1, vars, i, p, is_var);
	if (tau::get(fm_v1) == tau::get(fm_v0)) {
		i[p] = 2;
		TAU_TRY(auto sub, assign_and_reduce<node>(fm_v1, vars, i, dnf, is_var,
						p + 1, is_wff));
		if (sub) return report(true);
		i[p] = 0;
	} else {
		i[p] = 1;
		TAU_TRY(auto sub_v1, assign_and_reduce<node>(fm_v1, vars, i, dnf, is_var,
						p + 1, is_wff));
		if (sub_v1) return report(true);
		i[p] = 0;
		elim_vars_in_assignment<node>(fm_v0, vars, i, p, is_var);
		i[p] = -1;
		TAU_TRY(auto sub_v0, assign_and_reduce<node>(fm_v0, vars, i, dnf, is_var,
						p + 1, is_wff));
		if (sub_v0) return report(true);
		i[p] = 0;
	}
	return report(false);
}

// Given a BF b, calculate the Boole normal form (DNF corresponding to the paths to true in the BDD) of b
// where the variable order is given by the function lex_var_comp. A term with
// a non-Boolean operation is returned unchanged.
/** @internal @copydoc bf_reduced_dnf @endinternal */
template <NodeType node>
result<tref> bf_reduced_dnf(tref fm, bool make_paths_disjoint) {
	using tau = tree<node>;
	result<tref> r;
	LOG_TRACE << "bf_boole_normal_form: " << LOG_FM(fm);
	// Not static: a static [&] lambda dangles on later calls.
	auto trace = [&](tref fm) {
		LOG_TRACE << "bf_boole_normal_form result: " << LOG_FM(fm);
		return fm;
	};
	// We do not treat terms that contain a non-Boolean operation
	if (rewriter::find_top<node>(fm, is_non_boolean_term<node>))
		return r.with_value(fm);
	fm = apply_all_xor_def<node>(fm);
	// Function can only be applied to a BF
	const auto& t = tau::get(fm);
	DBG(assert(t.is(tau::bf));)
#ifdef TAU_CACHE
	using cache_t = std::map<std::pair<tref, bool>, tref,
				subtree_pair_less<node, bool>>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(std::make_pair(fm, make_paths_disjoint));
		it != cache.end()) return r.with_value(trace(it->second));
#endif //TAU_CACHE
	// This defines the variable order used to calculate DNF
	// It is made canonical by sorting the variables
	auto is_var = [](tref n) {
		return tau::get(n).child_is(tau::variable)
			|| tau::get(n).child_is(tau::bf_fall)
			|| tau::get(n).child_is(tau::bf_fex);
	};
	auto vars = t.select_top(is_var);
	std::sort(vars.begin(), vars.end(), lex_var_comp<node>);

	std::vector<int_t> i(vars.size()); // Record assignments of vars

	// Resulting DNF - make it ordered for consistency
	// Key is coefficient, value is possible variable assignments for coefficient
	// unordered_tau_map<std::vector<std::vector<int_t>>, BAs...> dnf;
	subtree_map<node, std::vector<std::vector<int_t>>> dnf;

	TAU_TRY(auto assigned,
		assign_and_reduce<node>(fm, vars, i, dnf, is_var, 0, false));
	if (assigned) {
		// A fully reduced formula has exactly one coefficient. Anything
		// else is an upstream bug, so the reduction fails rather than
		// returning a DNF built from a corrupt coefficient map.
		DBG(assert(dnf.size() == 1);)
		if (dnf.size() == 1) return r.with_value(trace(dnf.begin()->first));
		return r.with_error(code::internal_error,
			"bf_boole_normal_form: a reduced formula yielded "
			+ std::to_string(dnf.size())
			+ " coefficients instead of one",
			{{label::value, truncate_for_message(TAU_TO_STR(fm))}});
	}
	if (dnf.empty()) return r.with_value(trace(_0<node>(find_ba_type<node>(fm))));
	if (!make_paths_disjoint)
		for (auto& [coeff, paths] : dnf) join_paths(paths);

	// Convert map structure dnf back to rewrite tree
	tref reduced_dnf = nullptr;
	bool first = true;
	for (const auto& [coeff, paths] : dnf) {
		const auto& t = tau::get(coeff);
		bool is_one = t[0].is(tau::bf_t);
		if (paths.empty()) {
			DBG(assert(!is_one);)
			if (first) reduced_dnf = coeff;
			else reduced_dnf = tau::build_bf_or(reduced_dnf, coeff);
			continue;
		}
		for (const auto& path : paths) {
			bool first_var = true;
			tref var_path = nullptr;
			for (size_t k = 0; k < vars.size(); ++k) {
				if (path[k] == 2) continue;
				if (first_var) var_path = path[k] == 1 ? vars[k]
					: tau::build_bf_neg(vars[k]),
					first_var = false;
				else var_path = path[k] == 1
					? tau::build_bf_and(var_path, vars[k])
					: tau::build_bf_and(var_path,
						tau::build_bf_neg(vars[k]));
			}
			if (first) reduced_dnf = is_one ? var_path
				: tau::build_bf_and(coeff, var_path),
				first = false;
			else reduced_dnf = is_one
				? tau::build_bf_or(reduced_dnf, var_path)
				: tau::build_bf_or(reduced_dnf,
					tau::build_bf_and(coeff, var_path));
		}
	}
#ifdef TAU_CACHE
	cache.emplace(std::make_pair(fm, make_paths_disjoint), reduced_dnf);
	cache.emplace(std::make_pair(reduced_dnf, make_paths_disjoint),
								reduced_dnf);
#endif //TAU_CACHE
	return r.with_value(trace(reduced_dnf));
}

// The needed class in order to make bf_reduced_dnf work with rule applying process.
// Returns nullptr when a reduction fails.
/** @internal @copydoc bf_reduce_canonical::operator() @endinternal */
template <NodeType node>
tref bf_reduce_canonical<node>::operator() (tref fm) const {
	using tau = tree<node>;
	// TODO (HIGH) dropped error: bf_reduced_dnf's report -- this traverser
	// functor is fixed to tref by the operator| pipeline.
	const auto& t = tau::get(fm);
	LOG_TRACE << "bf reduce canonical: " << LOG_FM(fm);
	subtree_map<node, tref> changes = {};
	for (tref bf : t.select_top(is<node, tau::bf>)) {
		if (tau::get(bf).child_is(tau::bf_ref)) {
			// Reduce the matched bf's own ref arguments, not the
			// input's grandchild, which only coincides with the ref
			// when the input is the bf itself.
			for (tref arg : tau::get(bf)[0]
					.select_top(is<node, tau::bf>)) {
				auto dnf_r = bf_reduced_dnf<node>(arg);
				if (!dnf_r.has_value()) return nullptr;
				tref dnf = dnf_r.value();
				if (tau::get(dnf) != tau::get(arg))
					changes.emplace(arg, dnf);
			}
		}
		auto dnf_r = bf_reduced_dnf<node>(bf);
		if (!dnf_r.has_value()) return nullptr;
		tref dnf = dnf_r.value();
		if (tau::get(dnf) != tau::get(bf)) changes[bf] = dnf;
	}
	tref x = changes.empty()? fm : rewriter::replace<node>(fm, changes);
	LOG_TRACE << "bf reduce canonical result: " << LOG_FM(x);
	return x;
}

template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& t,
	const bf_reduce_canonical<node>& r)
{
	return typename tree<node>::traverser(r(t.value()));
}

// template<typename... BAs>
// std::optional<tref> operator|(const std::optional<tref>& fm, const bf_reduce_canonical<BAs...>& r) {
// 	return fm.has_value() ? r(fm.value()) : std::optional<tref>{};
// }

// The path vector of one DNF (or CNF) clause over the variable positions
// `var_pos`, and whether the clause is decided: constantly F in DNF / T in
// CNF (a constant literal, or a variable together with its negation), in
// which case the vector is incomplete. An internal_error when a negated
// literal is missing from `var_pos`.
template <NodeType node>
result<std::pair<std::vector<int_t>, bool>> clause_to_vector(tref clause,
	const auto& var_pos, const bool wff, const bool is_cnf)
{
	using tau = tree<node>;
	result<std::pair<std::vector<int_t>, bool>> r;
	std::vector<int_t> i(var_pos.size());
	for (size_t k = 0; k < var_pos.size(); ++k) i[k] = 2;
	bool clause_is_decided = false;
	auto var_assigner = [&](tref n) {
		if (clause_is_decided) return false;
		const auto& t = tau::get(n);
		if (!is_cnf && t.is(wff ? tau::wff_f : tau::bf_f)) {
			clause_is_decided = true;
			return false;
		}
		if (is_cnf && t.is(wff ? tau::wff_t : tau::bf_t)) {
			clause_is_decided = true;
			return false;
		}
		if (t.is(wff ? tau::wff_neg : tau::bf_neg)) {
			const auto& v = t[0];
			// Check if v is a T/F or 1/0
			if (v.equals_T() || v.equals_1()) {
				if (!is_cnf) clause_is_decided = true;
				return false;
			} else if (v.equals_F() || v.equals_0()) {
				if (is_cnf) clause_is_decided = true;
				return false;
			}
			auto it = var_pos.find(v.get());
			// Every literal of the clause was collected into var_pos
			// by the caller; a miss is an upstream bug. Without the
			// assertion (release) `i[it->second]` would read through
			// end(), so the clause is refused instead.
			DBG(assert(it != var_pos.end());)
			if (it == var_pos.end()) {
				r.error(code::internal_error,
					"get_assignment: negated literal missing "
					"from the clause's variable positions",
					{{label::value, truncate_for_message(
						TAU_TO_STR(clause))}});
				return false;
			}
			if (i[it->second] == 1) {
				// clause is false for DNF, true for CNF
				clause_is_decided = true;
				return false;
			}
			i[it->second] = -1;
			return false;
		}
		if (auto it = var_pos.find(n); it != var_pos.end()) {
			if (i[it->second] == -1) {
				// clause is false for DNF, true for CNF
				clause_is_decided = true;
				return false;
			}
			i[it->second] = 1;
			return false;
		}
		else return true;
	};
	pre_order<node>(clause).visit_unique(var_assigner);
	if (r.has_error()) return r;
	return r.with_value(std::make_pair(std::move(i), clause_is_decided));
}

// The path vectors of the clauses of `new_fm` over `vars`, decided clauses
// skipped. `decided` is set false as soon as one clause is not decided. With
// `all_reductions` each path is merged by reduce_paths as it is added. An
// empty result with `decided` false means a constant clause (T in DNF, F in
// CNF) absorbed the formula; errors come from clause_to_vector.
template <NodeType node>
result<std::vector<std::vector<int_t>>> collect_paths(tref new_fm, bool wff,
	const auto& vars, bool& decided, bool is_cnf, bool all_reductions)
{
	using tau = tree<node>;
	result<std::vector<std::vector<int_t>>> r;
	std::vector<std::vector<int_t>> paths;
	// unordered_tau_map<int_t, BAs...> var_pos;
	subtree_map<node, size_t> var_pos;
	for (size_t k = 0; k < vars.size(); ++k)
		var_pos.emplace(vars[k], k);
	for (tref clause : get_leaves<node>(new_fm, is_cnf
					? (wff ? tau::wff_and : tau::bf_and)
					: (wff ? tau::wff_or  : tau::bf_or))) {
		TAU_TRY(auto cv,
			clause_to_vector<node>(clause, var_pos, wff, is_cnf));
		auto [i, clause_is_decided] = std::move(cv);
		if (clause_is_decided) continue;
		// There is at least one satisfiable clause
		decided = false;
		if (std::ranges::all_of(i, [](const auto el) {return el == 2;}))
			return r.with_value(std::vector<std::vector<int_t>>{});
		if (all_reductions) {
			if (!reduce_paths(i, paths, vars.size()))
				paths.emplace_back(std::move(i));
			else {
				std::erase_if(paths,
					[](const auto& v){return v.empty();});
				if (paths.empty())
					return r.with_value(
						std::vector<std::vector<int_t>>{});
			}
		} else paths.emplace_back(std::move(i));
	}
	return r.with_value(std::move(paths));
}

// Rebuilds a wff (or a bf of type `type_id` when !wff) from `paths` over
// `vars` in DNF (CNF when is_cnf), with `!(a = b)` turned back into `a != b`;
// an empty `paths` gives the constant formula (F in DNF, T in CNF).
template <NodeType node>
tref build_reduced_formula(const auto& paths, const auto& vars, bool is_cnf,
	bool wff, size_t type_id)
{
	using tau = tree<node>;
	if (paths.empty()) return is_cnf ? (wff ? tau::_T() : tau::_1(type_id))
					 : (wff ? tau::_F() : tau::_0(type_id));
	tref reduced_fm = is_cnf ? (wff ? tau::_F() : tau::_0(type_id))
				 : (wff ? tau::_T() : tau::_1(type_id));
	bool first = true;
	for (const auto& path : paths) {
		if (path.empty()) continue;
		bool first_var = true;
		tref var_path = is_cnf  ? (wff ? tau::_F() : tau::_0(type_id))
					: (wff ? tau::_T() : tau::_1(type_id));
		DBG(assert(path.size() == vars.size());)
	for (size_t k = 0; k < vars.size(); ++k) {
		if (path[k] == 2) continue;
		if (first_var) var_path = path[k] == 1 ? vars[k]
			: wff ? tau::build_wff_neg(vars[k])
				: tau::build_bf_neg(vars[k]), first_var = false;
		else {
			if (!is_cnf) {
				if (wff) var_path = path[k] == 1
					? tau::build_wff_and(var_path, vars[k])
					: tau::build_wff_and(var_path,
						tau::build_wff_neg(vars[k]));
				else var_path = path[k] == 1
					? tau::build_bf_and(var_path, vars[k])
					: tau::build_bf_and(var_path,
						tau::build_bf_neg(vars[k]));
			}
			else {
				if (wff) var_path = path[k] == 1
					? tau::build_wff_or(var_path, vars[k])
					: tau::build_wff_or(var_path,
						tau::build_wff_neg(vars[k]));
				else var_path = path[k] == 1
					? tau::build_bf_or(var_path, vars[k])
					: tau::build_bf_or(var_path,
						tau::build_bf_neg(vars[k]));
			}
		}
	}
	if (first) reduced_fm = var_path, first = false;
	else reduced_fm = is_cnf
		? (wff  ? tau::build_wff_and(reduced_fm, var_path)
			: tau::build_bf_and( reduced_fm, var_path))
		: (wff  ? tau::build_wff_or( reduced_fm, var_path)
			: tau::build_bf_or(  reduced_fm, var_path));
	}
	DBG(assert(reduced_fm != nullptr);)
	return not_equal_to_unequal<node>(reduced_fm);
}

//TODO: decide if to treat xor in bf case
// The reduced path representation of a DNF (CNF when is_cnf) formula or
// term: its BDD variables and the joined, resolved paths over them, in the
// encoding described at the top of this file. For a wff, negations are
// pushed in, `sometimes φ` becomes `!always !φ`, and disequalities and order
// atoms become literals first. Errors come from the simplification of a
// `sometimes` body or from collect_paths.
template<NodeType node>
result<std::pair<std::vector<std::vector<int_t>>, trefs>>
dnf_cnf_to_reduced(tref fm, bool is_cnf) {
	using tau = tree<node>;
	result<std::pair<std::vector<std::vector<int_t>>, trefs>> r;
	auto smt_replace = [&](tref n) -> tref {
		if (is_child<node>(n, tau::wff_sometimes)) {
			n = tau::trim2(n); // Remove quantifier
			n = tau::build_wff_neg(n);
			auto simp = syntactic_formula_simplification<node>(n);
			if (!simp.has_value()) {
				r.merge(std::move(simp));
				return nullptr;
			}
			n = simp.value();
			n = tau::build_wff_neg(tau::build_wff_always(n));
			return n;
		} else return n;
	};
	LOG_TRACE << "dnf_cnf_to_reduced: " << LOG_FM(fm);
	bool is_wff = !tau::get(fm).is_term();
	if (is_wff) fm = push_negation_in<node>(fm);
	else fm = push_negation_in<node, false>(fm);
	// Pull negation out of equality
	if (is_wff) {
		// Substitute all sometimes by !always! and push inner equality in
		fm = pre_order<node>(fm).apply_unique(smt_replace);
		if (r.has_error()) return r;
		fm = unequal_to_not_equal<node>(fm);
		fm = order_atoms_to_literals<node>(fm);
	} else fm = apply_all_xor_def<node>(fm); // term case
	trefs vars = is_wff ? tau::get(fm).select_top(is_wff_bdd_var<node>)
			 : tau::get(fm).select_top(is_bf_bdd_var<node>);
	LOG_TRACE << "dnf_cnf_to_reduced / vars.size(): " << vars.size();
	if (vars.empty()) {
		if (tau::get(fm).equals_T() || tau::get(fm).equals_1()) {
			if (is_cnf)
				return r.with_value(
					std::pair<std::vector<std::vector<int_t>>,
						trefs>{});
			std::vector<std::vector<int_t>> paths;
			paths.emplace_back();
			return r.with_value(std::make_pair(std::move(paths),
				std::move(vars)));
		} else {
			if (is_cnf) {
				std::vector<std::vector<int_t>> paths;
				paths.emplace_back();
				return r.with_value(std::make_pair(std::move(paths),
							      std::move(vars)));
			}
			return r.with_value(
				std::pair<std::vector<std::vector<int_t>>, trefs>{});
		}
	}
	bool decided = true;
	TAU_TRY(auto paths, collect_paths<node>(fm, is_wff, vars, decided,
					is_cnf, true));
	join_paths(paths);
	if (paths.empty() && !decided) paths.emplace_back();
	return r.with_value(std::make_pair(std::move(paths), std::move(vars)));
}

// Assume that fm is in DNF (or CNF -> set is_cnf to true)
/** @internal @copydoc reduce @endinternal */
template<NodeType node, bool is_cnf>
result<tref> reduce(tref fm) {
	using tau = tree<node>;
	result<tref> r;
	bool is_wff = !tau::get(fm).is_term();
	size_t type_id = is_wff ? 0 : find_ba_type<node>(fm);
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, tref>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(fm); it != end(cache))
		return r.with_value(it->second);
#endif // TAU_CACHE
	DBG(LOG_TRACE << "Begin reduce with is_cnf set to " << is_cnf;)
	DBG(LOG_TRACE << "Formula to reduce: " << LOG_FM(fm);)
	// Terms can only contain bf_neg, bf_and, bf_xor and bf_or
	if (!is_wff) {
		if (tau::get(fm).find_top(is_non_boolean_term<node>)) {
			tref res = syntactic_path_simplification<node>(fm);
			// Cache this branch too, like every other exit: bv and
			// tau-constant terms would otherwise be re-simplified on
			// every call.
#ifdef TAU_CACHE
			return r.with_value(cache.emplace(fm, res).first->second);
#endif // TAU_CACHE
			return r.with_value(res);
		}
	}
	TAU_TRY(auto reduced, dnf_cnf_to_reduced<node>(fm, is_cnf));
	auto [paths, vars] = std::move(reduced);
	if (paths.empty()) {
		auto res = is_cnf ? (is_wff ? tau::_T() : tau::_1(type_id))
				  : (is_wff ? tau::_F() : tau::_0(type_id));
#ifdef TAU_CACHE
		return r.with_value(cache.emplace(fm, res).first->second);
#endif // TAU_CACHE
		return r.with_value(res);
	}
	if (paths.size() == 1 && paths[0].empty()) {
		auto res = is_cnf ? (is_wff ? tau::_F() : tau::_0(type_id))
				  : (is_wff ? tau::_T() : tau::_1(type_id));
#ifdef TAU_CACHE
		return r.with_value(cache.emplace(fm, res).first->second);
#endif // TAU_CACHE
		return r.with_value(res);
	}
	auto reduced_fm = build_reduced_formula<node>(paths, vars, is_cnf, is_wff, type_id);
	if (is_wff) reduced_fm = push_negation_in<node>(reduced_fm);
	else reduced_fm = push_negation_in<node, false>(reduced_fm);
	DBG(LOG_TRACE << "End reduce";)
	DBG(LOG_TRACE << "Reduced formula: " << LOG_FM(reduced_fm);)
#ifdef TAU_CACHE
	return r.with_value(cache.emplace(fm, reduced_fm).first->second);
#endif // TAU_CACHE
	return r.with_value(reduced_fm);
}

// `true` when the sorted (by subtree_less) lists v1 and v2 share at least i
// elements.
template<NodeType node>
bool is_ordered_overlap_at_least(size_t i, const trefs& v1, const trefs& v2) {
	using tau = tree<node>;
	if (v1.size() < i) return false;
	if (v2.size() < i) return false;
	size_t j = 0, k = 0;
	while (j < v1.size() && k < v2.size()) {
		if (i == 0) return true;
		// Check match
		if (tau::get(v1[j]) == tau::get(v2[k])) --i, ++j, ++k;
		else if (tau::subtree_less(v1[j], v2[k])) ++j;
		else ++k;
	}
	return i == 0;
}

// The number of elements the sorted (by subtree_less) lists v1 and v2
// share.
template<NodeType node>
int_t get_ordered_overlap(const trefs& v1, const trefs& v2) {
	using tau = tree<node>;
	int_t i = 0;
	size_t j = 0, k = 0;
	while (j < v1.size() && k < v2.size()) {
		// Check match
		if (tau::get(v1[j]) == tau::get(v2[k])) ++i, ++j, ++k;
		else if (tau::subtree_less(v1[j], v2[k])) ++j;
		else ++k;
	}
	return i;
}

/** @internal @copydoc wff_reduce_dnf::operator() @endinternal */
template <NodeType node>
tref wff_reduce_dnf<node>::operator() (tref fm) const {
	// TODO (HIGH) dropped error: reduce's report -- this traverser functor
	// is fixed to tref by the operator| pipeline.
	auto r = reduce<node>(fm);
	return r.has_value() ? r.value() : nullptr;
}

/** @internal @copydoc wff_reduce_cnf::operator() @endinternal */
template <NodeType node>
tref wff_reduce_cnf<node>::operator() (tref fm) const {
	// TODO (HIGH) dropped error: reduce's report -- this traverser functor
	// is fixed to tref by the operator| pipeline.
	auto r = reduce<node, true>(fm);
	return r.has_value() ? r.value() : nullptr;
}

template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& fm,
	const wff_reduce_dnf<node>& r)
{
	return typename tree<node>::traverser(r(fm.value()));
}

template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& fm,
	const wff_reduce_cnf<node>& r)
{
	return typename tree<node>::traverser(r(fm.value()));
}

// The DNF of `d1 && d2` (or `d1 & d2` for bf) for two DNFs of the same kind,
// by distributing every clause of d1 over every clause of d2.
template <NodeType node>
tref conjunct_dnfs_to_dnf(tref d1, tref d2) {
	using tau = tree<node>;
	const auto& t = tau::get(d1);
	if (t.is(tau::wff)) {
		DBG(assert(tau::get(d2).is(tau::wff));)
		tref res = tau::_F();
		auto clauses_d1 = get_dnf_wff_clauses<node>(d1);
		auto clauses_d2 = get_dnf_wff_clauses<node>(d2);
		for (tref dis1 : clauses_d1)
			for (tref dis2 : clauses_d2)
				res = tau::build_wff_or(res,
					tau::build_wff_and(dis1, dis2));
		return res;
	} else {
		DBG(assert(t.is(tau::bf) && tau::get(d2).is(tau::bf));)
		tref res = tau::_0(find_ba_type<node>(d1));
		auto clauses_d1 = get_dnf_bf_clauses<node>(d1);
		auto clauses_d2 = get_dnf_bf_clauses<node>(d2);
		for (tref dis1 : clauses_d1)
			for (tref dis2 : clauses_d2)
				res = tau::build_bf_or(res,
					tau::build_bf_and(dis1, dis2));
		return res;
	}
}

// The CNF of `c1 || c2` (or `c1 | c2` for bf) for two CNFs of the same kind,
// by distributing every clause of c1 over every clause of c2.
template <NodeType node>
tref disjunct_cnfs_to_cnf(tref c1, tref c2) {
	using tau = tree<node>;
	const auto& t = tau::get(c1);
	if (t.is(tau::wff)) {
		DBG(assert(tau::get(c2).is(tau::wff));)
		tref res = tau::_T();
		auto clauses_c1 = get_cnf_wff_clauses<node>(c1);
		auto clauses_c2 = get_cnf_wff_clauses<node>(c2);
		for (tref dis1 : clauses_c1)
			for (tref dis2 : clauses_c2)
				res = tau::build_wff_and(res,
					tau::build_wff_or(dis1, dis2));
		return res;
	} else {
		DBG(assert(t.is(tau::bf) && tau::get(c2).is(tau::bf));)
		tref res = tau::_1(find_ba_type<node>(c1));
		auto clauses_c1 = get_cnf_bf_clauses<node>(c1);
		auto clauses_c2 = get_cnf_bf_clauses<node>(c2);
		for (tref dis1 : clauses_c1)
			for (tref dis2 : clauses_c2)
				res = tau::build_bf_and(res,
					tau::build_bf_or(dis1, dis2));
		return res;
	}
}

} // namespace idni::tau_lang
