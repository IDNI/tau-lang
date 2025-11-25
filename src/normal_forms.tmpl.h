// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "normal_forms.h"
#include "union_find_with_sets.h"

namespace idni::tau_lang {

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "normal_forms"

template <NodeType node>
tref unequal_to_not_equal(tref fm) {
	using tau = tree<node>;
	LOG_TRACE << "unequal_to_not_equal: " << LOG_FM(fm);
	auto neq_to_not_eq = [](tref n) {
		//$X != 0 ::= !($X = 0)
		if (is<node>(n, tau::bf_neq)) {
			return tau::trim(tau::build_wff_neg(
				tau::build_bf_eq(
					tau::get(n).first(),
					tau::get(n).second())));
		}
		return n;
	};
	tref result = pre_order<node>(fm)
				.apply_unique(neq_to_not_eq, visit_wff<node>);
	LOG_TRACE << "unequal_to_not_equal result: " << LOG_FM(result);
	return result;
}

template <NodeType node>
tref not_equal_to_unequal(tref fm) {
	using tau = tree<node>;
	LOG_TRACE << "not_equal_to_unequal: " << LOG_FM(fm);
	auto not_eq_to_neq = [](tref n) {
		//!($X = 0) ::= $X != 0
		const auto& t = tau::get(n);
		if (t.is(tau::wff_neg))
			if (const auto& e = t[0][0]; e.is(tau::bf_eq))
				return tau::trim(
					tau::build_bf_neq(
						e.first(), e.second()));
		return n;
	};
	tref result = pre_order<node>(fm)
				.apply_unique(not_eq_to_neq, visit_wff<node>);
	LOG_TRACE << "not_equal_to_unequal: " << LOG_FM(result);
	return result;
}

template <NodeType node>
tref unsqueeze_wff(const tref& fm) {
	// $X | $Y = 0 ::= $X = 0 && $Y = 0
	// $X | $Y != 0 ::= $X != 0 || $Y != 0
	using tau = tree<node>;
	LOG_TRACE << "unsqueeze_wff: " << LOG_FM(fm);
	auto f = [](tref n) {
		const auto& t = tau::get(n);
		if (t.is(tau::bf_eq) && t[1].equals_0()) {
			const auto& e = t[0][0];
			if (e.is(tau::bf_or)) {
				tref c1 = e.first();
				tref c2 = e.second();
				return tau::trim(tau::build_wff_and(
					tau::build_bf_eq_0(c1),
					tau::build_bf_eq_0(c2)));
			}
		}
		else if (t.is(tau::bf_neq) && t[1].equals_0()) {
			const auto& e = t[0][0];
			if (e.is(tau::bf_or)) {
				tref c1 = e.first();
				tref c2 = e.second();
				return tau::trim(tau::build_wff_or(
					tau::build_bf_neq_0(c1),
					tau::build_bf_neq_0(c2)));
			}
		}
		return n;
	};
	tref result = pre_order<node>(fm).apply_unique(f, visit_wff<node>);
	LOG_TRACE << "unsqueeze_wff result: " << LOG_FM(result);
	return result;
}

template <NodeType node>
tref squeeze_wff(const tref& fm) {
	//$X = 0 && $Y = 0 ::= $X | $Y = 0
	// $X != 0 || $Y != 0 ::= $X | $Y != 0
	using tau = tree<node>;
	LOG_TRACE << "squeeze_wff: " << LOG_FM(fm);
	auto f = [](tref n) {
		const auto& t = tau::get(n);
		if (t.is(tau::wff_and)) {
			const auto& e1 = t[0], e2 = t[1];
			if (e1.child_is(tau::bf_eq) && e1[0][1].equals_0()
				&& e2.child_is(tau::bf_eq) && e2[0][1].equals_0()) {
				size_t t_e1 = find_ba_type<node>(e1.get());
				size_t t_e2 = find_ba_type<node>(e2.get());
				if (t_e1 == 0 || t_e2 == 0 || t_e1 == t_e2) {
					return tau::trim(tau::build_bf_eq_0(
						tau::build_bf_or(
						e1[0].first(), e2[0].first())));
				}
			}
		}
		else if (t.is(tau::wff_or)) {
			const auto& e1 = t[0], e2 = t[1];
			if (e1.child_is(tau::bf_neq) && e1[0][1].equals_0()
				&& e2.child_is(tau::bf_neq) && e2[0][1].equals_0())
			{
				size_t t_e1 = find_ba_type<node>(e1.get());
				size_t t_e2 = find_ba_type<node>(e2.get());
				if (t_e1 == 0 || t_e2 == 0 || t_e1 == t_e2) {
					return tau::trim(tau::build_bf_neq_0(
						tau::build_bf_or(
						e1[0].first(), e2[0].first())));
				}
			}
		}
		return n;
	};
	tref result = post_order<node>(fm).apply_unique(f, visit_wff<node>);
	LOG_TRACE << "squeeze_wff result: " << LOG_FM(result);
	return result;
}

template <NodeType node>
tref unsqueeze_wff_pos(tref fm) {
	// $X | $Y = 0 ::= $X = 0 && $Y = 0
	using tau = tree<node>;
	LOG_TRACE << "unsqueeze_wff_pos: " << LOG_FM(fm);
	auto f = [](tref n) {
		const auto& t = tau::get(n);
		if (t.is(tau::bf_eq) && t[1].equals_0()) {
			const auto& e = t[0][0];
			if (e.is(tau::bf_or)) {
				const auto& c1 = e.first(), c2 = e.second();
				return tau::trim(tau::build_wff_and(
					tau::build_bf_eq_0(c1),
					tau::build_bf_eq_0(c2)));
			}
		}
		return n;
	};
	auto result = pre_order<node>(fm).apply_unique(f, visit_wff<node>);
	LOG_TRACE << "unsqueeze_wff_pos result: " << LOG_FM(result);
	return result;
}

template <NodeType node>
tref squeeze_wff_pos(tref fm) {
	// $X = 0 && $Y = 0 ::= $X | $Y = 0
	using tau = tree<node>;
	LOG_TRACE << "squeeze_wff_pos: " << LOG_FM(fm);
	auto f = [](tref n) {
		const auto& t = tau::get(n);
		if (t.is(tau::wff_and)) {
			const auto& e1 = t[0], e2 = t[1];
			if (e1.child_is(tau::bf_eq) && e2.child_is(tau::bf_eq)
				&& e1[0][1].equals_0() && e2[0][1].equals_0()) {
				size_t t_e1 = find_ba_type<node>(e1.get());
				size_t t_e2 = find_ba_type<node>(e2.get());
				if (t_e1 == 0 || t_e2 == 0 || t_e1 == t_e2) {
					return tau::trim(tau::build_bf_eq_0(
						tau::build_bf_or(e1[0].first(), e2[0].first())));
				}
			}
		}
		return n;
	};
	tref result = post_order<node>(fm).apply_unique(f, visit_wff<node>);
	LOG_TRACE << "squeeze_wff_pos result: " << LOG_FM(result);
	return result;
}

template <NodeType node>
tref unsqueeze_wff_neg(tref fm) {
	// $X | $Y != 0 ::= $X != 0 || $Y != 0
	using tau = tree<node>;
	LOG_TRACE << "unsqueeze_wff_neg: " << LOG_FM(fm);
	auto f = [](tref n) {
		const auto& t = tau::get(n);
		if (t.is(tau::bf_neq) && t[1].equals_0()) {
			const auto& e = t[0][0];
			if (e.is(tau::bf_or)) {
				const auto& c1 = e.first(), c2 = e.second();
				return tau::trim(tau::build_wff_or(
					tau::build_bf_neq_0(c1),
					tau::build_bf_neq_0(c2)));
			}
		}
		return n;
	};
	auto result = pre_order<node>(fm).apply_unique(f, visit_wff<node>);
	LOG_TRACE << "unsqueeze_wff_neg result: " << LOG_FM(result);
	return result;
}

template <NodeType node>
tref squeeze_wff_neg(tref fm) {
	// $X != 0 || $Y != 0 ::= $X | $Y != 0
	using tau = tree<node>;
	LOG_TRACE << "squeeze_wff_neg: " << LOG_FM(fm);
	auto f = [](tref n) {
		const auto& t = tau::get(n);
		if (t.is(tau::wff_or)) {
			const auto& e1 = t[0], e2 = t[1];
			if (e1.child_is(tau::bf_neq) && e2.child_is(tau::bf_neq)
				&& e1[0][1].equals_0()
				&& e2[0][1].equals_0()) {
				size_t t_e1 = find_ba_type<node>(e1.get());
				size_t t_e2 = find_ba_type<node>(e2.get());
				if (t_e1 == 0 || t_e2 == 0 || t_e1 == t_e2) {
					return tau::trim(tau::build_bf_neq_0(
						tau::build_bf_or(
							e1[0].first(), e2[0].first())));
				}
			}
		}
		return n;
	};
	auto result = post_order<node>(fm).apply_unique(f, visit_wff<node>);
	LOG_TRACE << "squeeze_wff_neg result: " << LOG_FM(result);
	return result;
}

template <NodeType node>
tref to_nnf(tref fm) {
	LOG_TRACE << "to_nnf: " << LOG_FM(fm);
	auto result = push_negation_in<node>(fm);
	LOG_TRACE << "to_nnf result: " << LOG_FM(result);
	return result;
}

// Convert X =(!=) Y to X + Y =(!=) 0
template<NodeType node>
tref norm_equation(tref eq) {
	using tau = tree<node>;
	tau e = tau::get(eq);
	if (e.child_is(tau::bf_eq)) {
		return tau::build_bf_eq_0(tau::build_bf_xor(e[0].first(), e[0].second()));
	} else if (e.child_is(tau::bf_neq)) {
		return tau::build_bf_neq_0(tau::build_bf_xor(e[0].first(), e[0].second()));
	} else return eq;
}

// Convert X =(!=) Y to X + Y =(!=) 0
template<NodeType node>
tref norm_trimmed_equation(tref eq) {
	using tau = tree<node>;
	tau e = tau::get(eq);
	if (e.is(tau::bf_eq)) {
		return tau::trim(tau::build_bf_eq_0(tau::build_bf_xor(e.first(), e.second())));
	} else if (e.is(tau::bf_neq)) {
		return tau::trim(tau::build_bf_neq_0(tau::build_bf_xor(e.first(), e.second())));
	} else return eq;
}

template<NodeType node>
tref denorm_equation(tref eq) {
	using tau = tree<node>;
	tau e = tau::get(eq);
	if (e.child_is(tau::bf_eq) && e[0][1].equals_0()) {
		const tau& ce = e[0][0];
		if (ce.child_is(tau::bf_xor))
			return tau::build_bf_eq(ce[0].first(), ce[0].second());
	} else if (e.child_is(tau::bf_neq) && e[0][1].equals_0()) {
		const tau& ce = e[0][0];
		if (ce.child_is(tau::bf_xor))
			return tau::build_bf_neq(ce[0].first(), ce[0].second());
	}
	return eq;
}

// Convert all occurrences of X =(!=) Y to X + Y =(!=) 0 in fm
template <NodeType node>
tref norm_all_equations (tref fm) {
	return pre_order<node>(fm).apply_unique(norm_equation<node>,
						visit_wff<node>);
}

template<NodeType node>
tref apply_xor_def(tref fm) {
	using tau = tree<node>;
	tau t = tau::get(fm);
	if (t.child_is(tau::bf_xor)) {
		return tau::build_bf_or(
			tau::build_bf_and(t[0].first(),
					tau::build_bf_neg(t[0].second())),
			tau::build_bf_and(tau::build_bf_neg(t[0].first()),
					t[0].second()));
	}
	return fm;
}

template<NodeType node>
tref apply_all_xor_def(tref fm) {
	return pre_order<node>(fm).apply_unique(apply_xor_def<node>);
}

template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& fm, const tau_f<node>& func)
{
	return tt(func(fm.value()));
}
// -----------------------------------------------------------------------------

// This function traverses n and normalizes coefficients in a BF
template <NodeType node>
tref normalize_ba(tref fm) {
	using tau = tree<node>;
	DBG(LOG_TRACE << "normalize_ba: " << LOG_FM(fm));
	using tau = tree<node>;
	assert(tau::get(fm).is(tau::bf));
	auto norm_ba = [&](tref n) {
		const auto& t = tau::get(n);
		if (!t.is(tau::ba_constant)) return n;
		// Node has a Boolean algebra element
		auto c = t.get_ba_constant();
		auto nc = normalize_ba<node>(c);
		if (c == nc) return n;
		return tau::get_ba_constant(nc, t.get_ba_type());
	};
	tref r = pre_order<node>(fm).template apply_unique_until_change<
					MemorySlotPre::normalize_ba_m>(norm_ba);
	DBG(LOG_TRACE << "normalize_ba result: " << LOG_FM(r));
	return r;
}

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "normal_forms"

// return the inner quantifier or the top wff if the formula is not quantified
template <NodeType node>
std::pair<typename tree<node>::traverser, tref> get_inner_quantified_wff(tref n) {
	using tau = tree<node>;
	using tt = tau::traverser;
	// TODO (LOW) extract to a utils file
	auto quantified = [](const auto& n) -> bool {
		auto const& t = tau::get(n);
		return t.is(tau::wff_ex)
			|| t.is(tau::wff_all)
			|| t.is(tau::wff_sometimes)
			|| t.is(tau::wff_always);
	};
	if (auto quantifier = tau::get(n).find_bottom(quantified); quantifier) {
		return { tt(quantifier) | tau::variable,
			 tt(quantifier) | tau::wff | tt::ref };
	}
	return { tt{}, n };
}

template <NodeType node>
onf_wff<node>::onf_wff(tref _var) {
	using tau = tree<node>;
	const auto& t = tau::get(_var);
	if (!t.is(tau::bf)) var = tau::get(tau::bf, _var);
	else var = _var;
}

template <NodeType node>
tref onf_wff<node>::operator()(tref n) const {
	using tau = tree<node>;
	auto [ _ , nn] = get_inner_quantified_wff<node>(n);
	// We assume that the formula is in DNF. In particular nn is in DNF
	// For each disjunct calculate the onf
	subtree_map<node, tref> changes;
	bool no_disjunction = true;
	for (tref disjunct_ref : tau::get(nn).select_all(is<node, tau::wff_or>))
	{
		no_disjunction = false;
		const auto& disjunct = tau::get(disjunct_ref);
		DBG(assert(disjunct.children_size() == 2);)
		if (!disjunct[0][0].is(tau::wff_or))
			changes[disjunct[0].first()] =
					onf_subformula(disjunct[0].first());
		if (!disjunct[1][0].is(tau::wff_or))
			changes[disjunct[1].first()] =
					onf_subformula(disjunct[1].first());
	}
	if (no_disjunction) changes[nn] = onf_subformula(nn);
	return rewriter::replace<node>(nn, changes);
}

template <NodeType node>
tref onf_wff<node>::onf_subformula(tref n) const {
	using tau = tree<node>;
	using tt = tau::traverser;
	auto invalid = [](tref n) {
		if (tau::get(n).is_term() && is_non_boolean_term<node>(n))
			return true;
		else return false;
	};
	if (tau::get(n).find_top(invalid)) return n;
	auto has_var = [&](const auto& el) {
		return tau::get(el) == tau::get(var);
	};
	const auto& t = tau::get(n);
	tref eq = t.find_bottom(is<node, tau::bf_eq>);
	subtree_map<node, tref> changes;
	if (eq && tau::get(eq)[0].find_top(has_var)) {
		eq = norm_trimmed_equation<node>(eq);
		const auto& eq_v = tau::get(eq);
		DBG(assert(eq_v[1][0].is(tau::bf_f));)
		tref f_0 = tt(rewriter::replace<node>(
			eq_v.first(), var, tau::_0(find_ba_type<node>(var))))
				| bf_reduce_canonical<node>() | tt::ref;
		tref f_1 = tt(rewriter::replace<node>(
			tau::build_bf_neg(eq_v.first()), var,tau::_1(find_ba_type<node>(var))))
				| bf_reduce_canonical<node>() | tt::ref;

		changes[eq_v.get()] = tau::trim(tau::build_bf_interval(
							f_0, var, f_1));
	}
	for (tref neq_ref : t.select_all(is<node, tau::bf_neq>)) {
		neq_ref = norm_trimmed_equation<node>(neq_ref);
		const auto& neq = tau::get(neq_ref);
		DBG(assert(neq[1][0].is(tau::bf_f));)
		if (!neq[0].find_top(has_var)) continue;
		tref f_0 = tt(rewriter::replace<node>(
			neq.first(), var,
			tau::_0(find_ba_type<node>(var))))
				| bf_reduce_canonical<node>() | tt::ref;
		tref f_1 = tt(rewriter::replace<node>(
			tau::build_bf_neg(neq.first()), var,
			tau::_1(find_ba_type<node>(var))))
				| bf_reduce_canonical<node>() | tt::ref;
		changes[neq.get()] = tau::trim(tau::build_wff_or(
			tau::build_bf_nlteq(f_0, var),
			tau::build_bf_nlteq(var, f_1)));
	}
	return rewriter::replace<node>(n, changes);
}

template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& t, const onf_wff_t<node>& r)
{
	return tt(r(t.value()));
}

template <NodeType node>
tref onf(tref n, tref var) {
	using tau = tree<node>;
	using tt = tau::traverser;
	// FIXME take into account quiantifiers
	return tt(n) | tt::f(to_dnf<node, true>)
		     | tt::f(onf_wff<node>(var))
		     | tt::f(to_dnf<node, true>) | tt::ref;
}

// Reduce currrent dnf due to update by coeff and variable assignment i
inline bool reduce_paths(std::vector<int_t>& i,
	std::vector<std::vector<int_t>>& paths, int_t p, bool surface)
{
	for (size_t j = 0; j < paths.size(); ++j) {
		if (paths[j].empty()) continue;
		// Get Hamming distance between i and path and position of last difference
		// while different irrelevant variables make assignments incompatible
		int_t dist = 0, pos = 0;
		for (int_t k = 0; k < p; ++k) {
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

inline void join_paths(std::vector<std::vector<int_t>>& paths) {
	for (int_t i = 0; i < (int_t)paths.size(); ++i) {
		for (int_t j = 0; j < (int_t)paths.size(); ++j) {
			if (i == j) continue;
			int_t dist = 0, pos = 0;
			bool subset_relation_decided = false, is_i_subset_of_j = true,
				subset_check = true, equal = true;
			for (size_t k=0; k < paths[i].size(); ++k) {
			if (paths[i][k] == paths[j][k]) continue;
			else if (dist == 2) break;
			else if (paths[i][k] == 2) {
				if (!subset_relation_decided) {
					subset_relation_decided = true;
					is_i_subset_of_j = true;
					if (paths[j][k] != 2)
						equal = false;
				} else {
					if (!is_i_subset_of_j) {
						subset_check = false;
						break;
					}
					if (paths[j][k] != 2)
						equal = false;
				}
			}
			else if (paths[j][k] == 2) {
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
				paths[j][pos] = 2;
				if (equal) {
					paths.erase(paths.begin()+i);
					--i;
					break;
				}
			} else {
				// Resolve variable in i
				paths[i][pos] = 2;
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

// Starting from variable at position p+1 in vars write to i which variables are irrelevant in assignment
template <NodeType node>
void elim_vars_in_assignment(tref fm, const auto& vars, auto& i,
	const int_t p, const auto& is_var)
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

// Create assignment in formula and reduce resulting clause
template <NodeType node>
bool assign_and_reduce(tref fm, const trefs& vars, std::vector<int_t>& i,
	auto& dnf, const auto& is_var, int_t p, bool is_wff)
{
	using tau = tree<node>;
#ifdef DEBUG
	LOG_TRACE << "Begin assign_and_reduce [" << LOG_NT_COLOR
		<< (is_wff ? "wff" : "bf") << TC.CLEAR() << "]: "
		<< LOG_FM_DUMP(fm);
	for (auto v : vars) LOG_TRACE << "v: " << TAU_TO_STR(v);
	LOG_TRACE << "p: " << p;
#endif // DEBUG
	auto report = [&](bool result) {
		DBG(LOG_TRACE << (result ? "result is ok" : "no result")
				<< " for input: " << LOG_FM(fm);)
		return result;
	};

	// Check if all variables are assigned
	if((int_t) vars.size() == p) {
		tref fm_simp = nullptr;
		if (!is_wff) {
			// Do not add to dnf if the coefficient is 0
			if (tau::get(fm).equals_0()) return report(false);
			// fm is a Boolean function
			// Normalize tau subformulas
			fm_simp = normalize_ba<node>(fm);
			DBG(LOG_TRACE << "normalize_ba result: " << LOG_FM(fm_simp);)
			if (tau::get(fm_simp).equals_0()) return report(false);
			fm_simp = to_dnf<node, false>(fm_simp);
			DBG(LOG_TRACE << "to_dnf result: " << LOG_FM(fm_simp);)
			if (tau::get(fm_simp).equals_0()) return report(false);
			fm_simp = reduce<node>(fm_simp);
			DBG(LOG_TRACE << "reduce result: " << LOG_FM(fm_simp);)
			if (tau::get(fm_simp).equals_0()) return report(false);
		} else {
			if (tau::get(fm).equals_F()) return report(false);
			// fm is a Tau formula
			fm_simp = to_dnf<node, false>(fm);
			DBG(LOG_TRACE << "to_dnf result: " << LOG_FM(fm_simp);)
			if (tau::get(fm_simp).equals_F()) return report(false);
			fm_simp = reduce<node>(fm_simp);
			DBG(LOG_TRACE << "reduce result: " << LOG_FM(fm_simp);)
			if (tau::get(fm_simp).equals_F()) return report(false);
		}
		if (std::ranges::all_of(i, [](const auto el){return el == 2;})){
			//bool t = is<node>(fm->child[0], tau::bf_t);
			return dnf.emplace(fm_simp, std::vector(0, i)), true;
		}

		auto it = dnf.find(fm_simp);
		if (it == dnf.end()) return dnf.emplace(fm_simp,
				std::vector(p == 0 ? 0 : 1, i)), report(false);
		else if (!reduce_paths(i, it->second, p)) {
			// Place coefficient together with variable assignment if no reduction happend
			it->second.push_back(i);
		} else std::erase_if(it->second,
				[](const auto& v) { return v.empty(); });
		return it->second.empty();
	}
	// variable was already eliminated
	if (i[p] == 2) {
		if (assign_and_reduce<node>(fm, vars, i, dnf, is_var, p + 1,
						is_wff)) return report(true);
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
		if (assign_and_reduce<node>(fm_v1, vars, i, dnf, is_var, p + 1,
						is_wff)) return report(true);
		i[p] = 0;
	} else {
		i[p] = 1;
		if (assign_and_reduce<node>(fm_v1, vars, i, dnf, is_var, p + 1,
						is_wff)) return report(true);
		i[p] = 0;
		elim_vars_in_assignment<node>(fm_v0, vars, i, p, is_var);
		i[p] = -1;
		if (assign_and_reduce<node>(fm_v0, vars, i, dnf, is_var, p + 1,
						is_wff)) return report(true);
		i[p] = 0;
	}
	return report(false);
}

// Given a BF b, calculate the Boole normal form (DNF corresponding to the paths to true in the BDD) of b
// where the variable order is given by the function lex_var_comp
template <NodeType node>
tref bf_reduced_dnf(tref fm, bool make_paths_disjoint) {
	using tau = tree<node>;
	LOG_TRACE << "bf_boole_normal_form: " << LOG_FM(fm);
	static auto trace = [&](tref fm) {
		LOG_TRACE << "bf_boole_normal_form result: " << LOG_FM(fm);
		return fm;
	};
	// We do not treat terms that contain a non-Boolean operation
	if (rewriter::find_top<node>(fm, is_non_boolean_term<node>))
		return fm;
	fm = apply_all_xor_def<node>(fm);
	// Function can only be applied to a BF
	const auto& t = tau::get(fm);
	DBG(assert(t.is(tau::bf));)
#ifdef TAU_CACHE
	using cache_t = std::map<std::pair<tref, bool>, tref,
				subtree_pair_less<node, bool>>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(std::make_pair(fm, make_paths_disjoint));
		it != cache.end()) return trace(it->second);
#endif //TAU_CACHE
	// This defines the variable order used to calculate DNF
	// It is made canonical by sorting the variables
	auto is_var = [](tref n) {
		return tau::get(n).child_is(tau::variable);
	};
	auto vars = t.select_top(is_var);
	std::sort(vars.begin(), vars.end(), lex_var_comp<node>);

	std::vector<int_t> i(vars.size()); // Record assignments of vars

	// Resulting DNF - make it ordered for consistency
	// Key is coefficient, value is possible variable assignments for coefficient
	// unordered_tau_map<std::vector<std::vector<int_t>>, BAs...> dnf;
	subtree_map<node, std::vector<std::vector<int_t>>> dnf;

	if (assign_and_reduce<node>(fm, vars, i, dnf, is_var, 0)) {
		assert(dnf.size() == 1);
		return trace(dnf.begin()->first);
	}
	if (dnf.empty()) return trace(_0<node>(find_ba_type<node>(fm)));
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
	return trace(reduced_dnf);
}

// The needed class in order to make bf_reduced_dnf work with rule applying process
template <NodeType node>
tref bf_reduce_canonical<node>::operator() (tref fm) const {
	using tau = tree<node>;
	const auto& t = tau::get(fm);
	LOG_TRACE << "bf reduce canonical: " << LOG_FM(fm);
	subtree_map<node, tref> changes = {};
	for (tref bf : t.select_top(is<node, tau::bf>)) {
		if (tau::get(bf).child_is(tau::bf_ref)) {
			for (tref arg : t[0][0].select_top(is<node, tau::bf>)) {
				tref dnf = bf_reduced_dnf<node>(arg);
				if (tau::get(dnf) != tau::get(arg))
					changes.emplace(arg, dnf);
			}
		}
		tref dnf = bf_reduced_dnf<node>(bf);
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

inline bool is_contained_in(const std::vector<int_t>& i, auto& paths) {
	// Check if there is a containment of i in any path of paths
	for (auto& path : paths) {
		bool is_contained = true, is_i_smaller,
			containment_dir_known = false;
		for (int_t k = 0; k < (int_t)i.size(); ++k) {
			if (i[k] == path[k]) continue;
			else if (i[k] == 2) {
				if (containment_dir_known) {
					if (!is_i_smaller) {
						is_contained = false; break; }
				} else {
					containment_dir_known = true;
					is_i_smaller = true;
				}
			}
			else if (path[k] == 2) {
				if (containment_dir_known) {
					if (is_i_smaller) {
						is_contained = false; break; }
				} else {
					containment_dir_known = true;
					is_i_smaller = false;
				}
			}
			else if (i[k] != path[k]) {is_contained = false; break;}
		}
		if (is_contained) {
			if (is_i_smaller) {
				// keep i and delete current path
				path = {};
			} else return true;
		}
	}
	return false;
}

template <NodeType node>
std::pair<std::vector<int_t>, bool> clause_to_vector(tref clause,
	const auto& var_pos, const bool wff, const bool is_cnf)
{
	using tau = tree<node>;
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
			assert(it != var_pos.end());
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
	return std::make_pair(std::move(i), clause_is_decided);
}

template <NodeType node>
std::vector<std::vector<int_t>> collect_paths(tref new_fm, bool wff,
	const auto& vars, bool& decided, bool is_cnf, bool all_reductions)
{
	using tau = tree<node>;
	std::vector<std::vector<int_t>> paths;
	// unordered_tau_map<int_t, BAs...> var_pos;
	subtree_map<node, int_t> var_pos;
	for (int_t k = 0; k < (int_t) vars.size(); ++k)
		var_pos.emplace(vars[k], k);
	for (tref clause : get_leaves<node>(new_fm, is_cnf
					? (wff ? tau::wff_and : tau::bf_and)
					: (wff ? tau::wff_or  : tau::bf_or))) {
		auto [i, clause_is_decided] =
			clause_to_vector<node>(clause, var_pos, wff, is_cnf);
		if (clause_is_decided) continue;
		// There is at least one satisfiable clause
		decided = false;
		if (std::ranges::all_of(i, [](const auto el) {return el == 2;}))
			return {};
		if (all_reductions) {
			if (!reduce_paths(i, paths, vars.size()))
				paths.emplace_back(std::move(i));
			else {
				std::erase_if(paths,
					[](const auto& v){return v.empty();});
				if (paths.empty()) return {};
			}
		} else paths.emplace_back(std::move(i));
	}
	return paths;
}

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
	for (size_t k = 0; k < vars.size(); ++k) {
		DBG(assert(path.size() == vars.size());)
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
	assert(reduced_fm != nullptr);
	return not_equal_to_unequal<node>(reduced_fm);
}

//TODO: decide if to treat xor in bf case
template<NodeType node>
std::pair<std::vector<std::vector<int_t>>, trefs> dnf_cnf_to_reduced(tref fm,
	bool is_cnf) {
	using tau = tree<node>;
	auto smt_replace = [](tref n) {
		if (is_child<node>(n, tau::wff_sometimes)) {
			n = tau::trim2(n); // Remove quantifier
			n = tau::build_wff_neg(n);
			n = syntactic_formula_simplification<node>(n);
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
		fm = unequal_to_not_equal<node>(fm);
	} else fm = apply_all_xor_def<node>(fm); // term case
	trefs vars = is_wff ? tau::get(fm).select_top(is_wff_bdd_var<node>)
			 : tau::get(fm).select_top(is_bf_bdd_var<node>);
	LOG_TRACE << "dnf_cnf_to_reduced / vars.size(): " << vars.size();
	if (vars.empty()) {
		if (tau::get(fm).equals_T() || tau::get(fm).equals_1()) {
			if (is_cnf) return {};
			std::vector<std::vector<int_t>> paths;
			paths.emplace_back();
			return std::make_pair(std::move(paths), std::move(vars));
		} else {
			if (is_cnf) {
				std::vector<std::vector<int_t>> paths;
				paths.emplace_back();
				return std::make_pair(std::move(paths),
						      std::move(vars));
			}
			return {};
		}
	}
	bool decided = true;
	auto paths = collect_paths<node>(fm, is_wff, vars, decided, is_cnf,
					true);
	join_paths(paths);
	if (paths.empty() && !decided) paths.emplace_back();
	return std::make_pair(std::move(paths), std::move(vars));
}

template <NodeType node>
tref group_dnf_expression(tref fm) {
	using tau = tree<node>;
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, tref>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	if (auto it = cache.find(fm); it != end(cache)) return it->second;
#endif // TAU_CACHE
	LOG_DEBUG << "Begin group_dnf_expression";
	LOG_DEBUG << "Expression to factor:" << LOG_FM(fm);
	auto count_common = [](const auto& v1, const auto& v2) {
		int_t count = 0;
		auto it1 = begin(v1);
		auto it2 = begin(v2);
		while (it1 != end(v1) && it2 != end(v2))
        		if (*it1 < *it2) ++it1;
        		else {
				if (!(*it2 < *it1)) ++count, ++it1;
                		++it2;
			}
		return count;
	};

	bool wff = tau::get(fm).is(tau::wff);
	size_t type_id = wff ? 0 : find_ba_type<node>(fm);
	auto clauses = wff ? get_dnf_wff_clauses<node>(fm)
			   : get_dnf_bf_clauses<node>(fm);
	if (clauses.size() < 2) {
		LOG_TRACE << "group_dnf_expression result: " << LOG_FM(fm);
#ifdef TAU_CACHE
		return cache.emplace(fm, fm).first->second;
#endif // TAU_CACHE
		return fm;
	}

	std::vector<trefs> atoms_of_clauses;
	for (tref clause : clauses) {
		auto atoms = wff ? get_cnf_wff_clauses<node>(clause)
				 : get_cnf_bf_clauses<node>(clause);
		if(wff) std::ranges::sort(atoms);
		else std::ranges::sort(atoms, lex_var_comp<node>);
		atoms_of_clauses.emplace_back(std::move(atoms));
	}
	tref grouped_fm = wff ? tau::_F() : tau::_0(type_id);
	for (int_t i = 0; i < (int_t) atoms_of_clauses.size(); ++i) {
		std::pair max_common = { 0,0 };
		for (size_t j = i + 1; j < atoms_of_clauses.size(); ++j) {
			int_t count = count_common(atoms_of_clauses[i],
							atoms_of_clauses[j]);
			if (count > max_common.second) {
				max_common.first = j;
				max_common.second = count;
			}
		}
		if (max_common.first == 0) {
			auto atoms = wff
				? tau::build_wff_and(atoms_of_clauses[i])
				: tau::build_bf_and(atoms_of_clauses[i]);
			grouped_fm = wff
				? tau::build_wff_or(grouped_fm, atoms)
				: tau::build_bf_or(grouped_fm, atoms);
			continue;
		}
		trefs common;
		std::ranges::set_intersection(atoms_of_clauses[i],
					atoms_of_clauses[max_common.first],
					back_inserter(common));

		tref cl1 = wff ? tau::_T() : tau::_1(type_id);
		tref cl2 = wff ? tau::_T() : tau::_1(type_id);
		size_t p = 0;
		for (tref atom : atoms_of_clauses[i]) {
			if (p < common.size() && tau::get(common[p])
							== tau::get(atom)) ++p;
			else cl1 = wff ? tau::build_wff_and(cl1, atom)
				       : tau::build_bf_and(cl1, atom);
		}
		p = 0;
		for (tref atom : atoms_of_clauses[max_common.first]) {
			if (p < common.size() && tau::get(common[p])
							== tau::get(atom)) ++p;
			else cl2 = wff ? tau::build_wff_and(cl2, atom)
				       : tau::build_bf_and(cl2, atom);
		}
		// We need the canonical order for the reduction in "group_paths_and_simplify"
		if (!lex_var_comp<node>(cl1, cl2)) std::swap(cl1, cl2);
		tref grouped = wff ? tau::build_wff_or(cl1, cl2)
				   : tau::build_bf_or(get_dnf_bf_clauses<node>(
					to_dnf<node, false>(
						tau::build_bf_or(cl1, cl2))));

		common.emplace_back(std::move(grouped));
		atoms_of_clauses[i] = std::move(common);
		atoms_of_clauses.erase(atoms_of_clauses.begin()
							+ max_common.first);
		--i;
	}
	assert(grouped_fm != nullptr);
#ifdef TAU_CACHE
	cache.emplace(grouped_fm, grouped_fm);
	return cache.emplace(fm, grouped_fm).first->second;
#endif // TAU_CACHE
	LOG_DEBUG << "End group_dnf_expression";
	LOG_DEBUG << "Factored expression: " << LOG_FM(grouped_fm);
	return grouped_fm;
}

// Assume that fm is in DNF (or CNF -> set is_cnf to true)
template<NodeType node, bool is_cnf>
tref reduce(tref fm) {
	using tau = tree<node>;
	bool is_wff = !tau::get(fm).is_term();
	size_t type_id = is_wff ? 0 : find_ba_type<node>(fm);
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, tref>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(fm); it != end(cache)) return it->second;
#endif // TAU_CACHE
	DBG(LOG_TRACE << "Begin reduce with is_cnf set to " << is_cnf;)
	DBG(LOG_TRACE << "Formula to reduce: " << LOG_FM(fm);)
	// Terms can only contain bf_neg, bf_and, bf_xor and bf_or
	if (!is_wff) {
		if (tau::get(fm).find_top(is_non_boolean_term<node>))
			return syntactic_path_simplification<node>::on(fm);
	}
	auto [paths, vars] = dnf_cnf_to_reduced<node>(fm, is_cnf);
	if (paths.empty()) {
		auto res = is_cnf ? (is_wff ? tau::_T() : tau::_1(type_id))
				  : (is_wff ? tau::_F() : tau::_0(type_id));
#ifdef TAU_CACHE
		return cache.emplace(fm, res).first->second;
#endif // TAU_CACHE
		return res;
	}
	if (paths.size() == 1 && paths[0].empty()) {
		auto res = is_cnf ? (is_wff ? tau::_F() : tau::_0(type_id))
				  : (is_wff ? tau::_T() : tau::_1(type_id));
#ifdef TAU_CACHE
		return cache.emplace(fm, res).first->second;
#endif // TAU_CACHE
		return res;
	}
	auto reduced_fm = build_reduced_formula<node>(paths, vars, is_cnf, is_wff, type_id);
	if (is_wff) reduced_fm = push_negation_in<node>(reduced_fm);
	else reduced_fm = push_negation_in<node, false>(reduced_fm);
	DBG(LOG_TRACE << "End reduce";)
	DBG(LOG_TRACE << "Reduced formula: " << LOG_FM(reduced_fm);)
#ifdef TAU_CACHE
	return cache.emplace(fm, reduced_fm).first->second;
#endif // TAU_CACHE
	return reduced_fm;
}

template <NodeType node>
bool is_ordered_subset(const auto& v1, const auto& v2) {
	using tau = tree<node>;
	if (v1.size() > v2.size()) return false;
	if (v1.size() == 0) return true;
	size_t j = 0;
	for (size_t i = 0; i < v2.size(); ++i) {
		if (tau::get(v1[j]) == tau::get(v2[i])) ++j;
		if (j == v1.size()) return true;
	}
	return false;
}

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

template <NodeType node>
tref wff_reduce_dnf<node>::operator() (tref fm) const {
	return reduce<node>(fm);
}

template <NodeType node>
tref wff_reduce_cnf<node>::operator() (tref fm) const {
	return reduce<node, true>(fm);
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

// Can be used for Tau formula and Boolean function
template <NodeType node, bool is_wff>
tref push_negation_one_in(tref fm) {
	using tau = tree<node>;
	const auto& t = tau::get(fm);
	// Tau formula rules
	if constexpr (is_wff) if (t.child_is(tau::wff_neg)) {
		const tau& ct = t[0][0];
		if (!ct.has_child()) return fm;
		switch (ct[0].value.nt) {
			case tau::wff_and: return tau::build_wff_or(
						tau::build_wff_neg(ct[0].first()),
						tau::build_wff_neg(ct[0].second()));
			case tau::wff_or: return tau::build_wff_and(
						tau::build_wff_neg(ct[0].first()),
						tau::build_wff_neg(ct[0].second()));
			case tau::bf_eq: return tau::build_bf_neq(ct[0].first(), ct[0].second());
			case tau::bf_neq: return tau::build_bf_eq(ct[0].first(), ct[0].second());
			case tau::wff_ex: return tau::build_wff_all(ct[0].first(),
						tau::build_wff_neg(ct[0].second()), false);
			case tau::wff_all: return tau::build_wff_ex(ct[0].first(),
						tau::build_wff_neg(ct[0].second()), false);
			case tau::wff_always: return tau::build_wff_sometimes(
						tau::build_wff_neg(ct[0].first()));
			case tau::wff_sometimes: return tau::build_wff_always(
						tau::build_wff_neg(ct[0].first()));
			case tau::bf_lt: return tau::build_bf_nlt(ct[0].first(), ct[0].second());
			case tau::bf_nlt: return tau::build_bf_lt(ct[0].first(), ct[0].second());
			case tau::bf_lteq: return tau::build_bf_nlteq(ct[0].first(), ct[0].second());
			case tau::bf_nlteq: return tau::build_bf_lteq(ct[0].first(), ct[0].second());
			case tau::bf_gt: return tau::build_bf_ngt(ct[0].first(), ct[0].second());
			case tau::bf_ngt: return tau::build_bf_gt(ct[0].first(), ct[0].second());
			case tau::bf_gteq: return tau::build_bf_ngteq(ct[0].first(), ct[0].second());
			case tau::bf_ngteq: return tau::build_bf_gteq(ct[0].first(), ct[0].second());
			default: return fm;
		}
	}
	// Boolean function rules
	if constexpr (!is_wff) if (t.child_is(tau::bf_neg)) {
		const tau& ct = t[0][0];
		if (!ct.has_child()) return fm;
		switch (ct[0].value.nt) {
			case tau::bf_and: return tau::build_bf_or(
				tau::build_bf_neg(ct[0].first()),
				tau::build_bf_neg(ct[0].second()));
			case tau::bf_or: return tau::build_bf_and(
				tau::build_bf_neg(ct[0].first()),
				tau::build_bf_neg(ct[0].second()));
			case tau::bf_xor: {
				// TODO: strategy for negating first or second argument
				return tau::build_bf_xor(tau::build_bf_neg(ct[0].first()), ct[0].second());
			}
			default: return fm;
		}
	}
	return fm;
}

// Can be used for Tau formula and Boolean function
template <NodeType node, bool is_wff>
tref push_negation_in(tref fm) {
	auto pn = [](tref n) {
		return push_negation_one_in<node, is_wff>(n);
	};
	if constexpr (is_wff) return pre_order<node>(fm)
		.template apply_unique<MemorySlotPre::push_negation_in_m>(
							pn, visit_wff<node>);
	else return pre_order<node>(fm)
		.template apply_unique<MemorySlotPre::push_negation_in_m>(pn);
}

// Conversion to dnf while applying reductions during the process
template <NodeType node, bool is_wff>
tref to_dnf(tref fm) {
	using tau = tree<node>;
	using tt = tau::traverser;
	LOG_TRACE << "to_dnf: " << LOG_FM(fm);
	auto layer_to_dnf = [](tref n) {
		const auto& t = tau::get(n);
		if constexpr (is_wff) if (t.is(tau::wff)) {
			if (t.child_is(tau::wff_and)) {
				auto conj = conjunct_dnfs_to_dnf<node>(
					t[0].first(), t[0].second());
				// Perform simplification
				if (tau::get(conj) != tau::get(n)) return tt(conj)
					| wff_reduce_dnf<node>() | tt::ref;
				else return n;
			}
		}
		if constexpr (!is_wff) if (t.is(tau::bf)) {
			if (t.child_is(tau::bf_and)) {
				auto conj = conjunct_dnfs_to_dnf<node>(
					t[0].first(), t[0].second());
				// Perform simplification
				if (conj != n)
					return reduce<node>(conj);
				else return n;
			}
		}
		return n;
	};
	auto pn = [](const auto& n) {
		return push_negation_one_in<node, is_wff>(n);
	};
	tref r;
	if constexpr (is_wff) r = pre_order<node>(fm)
		.template apply_unique<MemorySlotPre::to_dnf_m>(
					pn, visit_wff<node>, layer_to_dnf);
	else r = pre_order<node>(fm)
		.template apply_unique<MemorySlotPre::to_dnf_m>(
					pn, all, layer_to_dnf);
	LOG_TRACE << "to_dnf result: " << LOG_FM(r);
	return r;
}

// Conversion of temporal layer to dnf
template <NodeType node>
tref temporal_layer_to_dnf(tref fm) {
	using tau = tree<node>;
	auto layer_to_dnf = [](tref n) {
		const auto& t = tau::get(n);
		if (t.child_is(tau::wff_and)) {
			auto conj = conjunct_dnfs_to_dnf<node>(
				t[0].first(), t[0].second());
			// Perform simplification
			if (tau::get(conj) != tau::get(n))
				return reduce<node>(conj);
			else return n;
		}
		return n;
	};
	auto pn = [](tref n) {
		return push_negation_one_in<node>(n);
	};
	auto visit = [](tref n){
		if (is_temporal_quantifier<node>(n)) return false;
		return true;
	};
	return pre_order<node>(fm).apply_unique(pn, visit, layer_to_dnf);
}

// Conversion to cnf while applying reductions during the process
template <NodeType node, bool is_wff>
tref to_cnf(tref fm) {
	using tau = tree<node>;
	using tt = tau::traverser;
	auto layer_to_cnf = [](tref n) {
		const auto& t = tau::get(n);
		if constexpr (is_wff) if (t.is(tau::wff)) {
			if (t.child_is(tau::wff_or)) {
				auto dis = disjunct_cnfs_to_cnf<node>(
					t[0].first(), t[0].second());
				// Perform simplification
				if (tau::get(dis) != tau::get(n)) return tt(dis)
					| wff_reduce_cnf<node>() | tt::ref;
				else return n;
			}
		}
		if constexpr (!is_wff) if (t.child_is(tau::bf_or)) {
				auto dis = disjunct_cnfs_to_cnf<node>(
						t[0].first(), t[0].second());
				// Perform simplification
				if (dis != n)
					return reduce<node, true>(dis);
				else return n;
			}
		return n;
	};
	auto pn = [](tref n) {
		return push_negation_one_in<node, is_wff>(n);
	};
	if constexpr (is_wff) return pre_order<node>(fm)
		.template apply_unique<MemorySlotPre::to_cnf_m>(
					pn, visit_wff<node>, layer_to_cnf);
	else return pre_order<node>(fm)
		.template apply_unique<MemorySlotPre::to_cnf_m>(
					pn, all, layer_to_cnf);
}

// Shift the lookback in a formula
template <NodeType node>
tref shift_io_vars_in_fm(tref fm, const auto& io_vars, const int_t shift) {
	using tau = tree<node>;
	if (shift <= 0) return fm;
	subtree_map<node, tref> changes;
	for (tref io_var : io_vars) {
		// Skip initial conditions
		if (is_io_initial<node>(io_var)) continue;
		int_t var_shift = get_io_var_shift<node>(io_var);
		size_t type = tau::get(io_var).get_ba_type();
		changes[io_var] = tau::get(io_var).is_input_variable()
			? tau::trim(build_in_var_at_t_minus<node>(get_var_name_node<node>(
				io_var), var_shift + shift, type))
			: tau::trim(build_out_var_at_t_minus<node>(get_var_name_node<node>(
				io_var), var_shift + shift, type));
	}
	return rewriter::replace<node>(fm, changes);
}

template <NodeType node>
tref shift_const_io_vars_in_fm(tref fm, const auto& io_vars, const int_t shift){
	using tau = tree<node>;
	if (shift <= 0) return fm;
	subtree_map<node, tref> changes;
	for (tref io_var : io_vars) {
		if (!is_io_initial<node>(io_var)) continue;
		int_t tp = get_io_time_point<node>(io_var);
		// Make sure that the resulting time point is positive
		if (tp + shift < 0) return tau::_F();
		size_t type = tau::get(io_var).get_ba_type();
		changes.emplace(io_var, tau::get(io_var).is_input_variable()
			? tau::trim(build_in_var_at_n<node>(
				get_var_name_node<node>(io_var), tp + shift, type))
			: tau::trim(build_out_var_at_n<node>(
				get_var_name_node<node>(io_var), tp + shift, type)));
	}
	return rewriter::replace<node>(fm, changes);
}

// Adjust the lookback before conjunction of fm1 and fm2
template <NodeType node>
tref always_conjunction(tref fm1_aw, tref fm2_aw) {
	using tau = tree<node>;
	// Trim the always node if present
	auto fm1 = is_child<node>(fm1_aw, tau::wff_always)
				? tau::trim2(fm1_aw) : fm1_aw;
	auto fm2 = is_child<node>(fm2_aw, tau::wff_always)
				? tau::trim2(fm2_aw) : fm2_aw;
	auto io_vars1 = tau::get(fm1)
		.select_top(is_child<node, tau::io_var>);
	auto io_vars2 = tau::get(fm2)
		.select_top(is_child<node, tau::io_var>);
	// Get lookbacks
	int_t lb1 = get_max_shift<node>(io_vars1);
	int_t lb2 = get_max_shift<node>(io_vars2);
	if (lb1 < lb2) {
		// adjust fm1 by lb2 - lb1
		return tau::build_wff_and(
			shift_io_vars_in_fm<node>(fm1, io_vars1, lb2 - lb1),
			fm2);
	} else if (lb2 < lb1) {
		// adjust fm2 by lb1 - lb2
		return tau::build_wff_and(fm1,
			shift_io_vars_in_fm<node>(fm2, io_vars2, lb1 - lb2));
	} else {
		// no adjustment needed
		return tau::build_wff_and(fm1, fm2);
	}
}

template <NodeType node>
tref push_existential_quantifier_one(tref fm, subtree_set<node>* excluded) {
	using tau = tree<node>;
	LOG_DEBUG << "push_existential_quantifier_one: " << LOG_FM_DUMP(fm);
	const auto& t = tau::get(fm);
	DBG(assert(t.child_is(tau::wff_ex));)
	const tref scoped_fm = t[0].second();
	const tref quant_var = t[0].first();

	const auto& st = tau::get(scoped_fm);
	if (st.child_is(tau::wff_or)) {
		// Push quantifier in
		tref c0 = tau::build_wff_ex(quant_var, st[0].first(), false);
		tref c1 = tau::build_wff_ex(quant_var, st[0].second(), false);
		return tau::build_wff_or(c0, c1);
	}
	else if (st.child_is(tau::wff_and)) {
		// Remove existential, if quant_var does not appear in clause
		trefs clauses = get_cnf_wff_clauses<node>(scoped_fm);
		tref no_q_fm = tau::_T();
		for (tref& clause : clauses) {
			if (!contains<node>(clause, quant_var)) {
				no_q_fm = tau::build_wff_and(no_q_fm, clause);
				clause = tau::_T();
			}
		}
		tref q_fm = tau::build_wff_and(clauses);
		if (tau::get(q_fm).equals_T()) {
			if (excluded) excluded->insert(scoped_fm);
			return scoped_fm;
		}
		else if (tau::get(no_q_fm).equals_T()) return fm;
		else {
			if (excluded) excluded->insert(no_q_fm);
			return tau::build_wff_and(
				tau::build_wff_ex(quant_var, q_fm, false), no_q_fm);
		}
	}
	else if (st.child_is(tau::wff_ex)) {
		//other ex quant, hence can switch them
		tref c = tau::build_wff_ex(quant_var, st[0].second());
		return tau::build_wff_ex(st[0].first(), c);
	}
	// Else check if quant_var is contained in subtree
	else if (contains<node>(scoped_fm, quant_var)) {
		if (excluded) excluded->insert(fm);
		return fm;
	}
	else {
		if (excluded) excluded->insert(scoped_fm);
		return scoped_fm;
	}
}

template <NodeType node>
tref push_universal_quantifier_one(tref fm) {
	using tau = tree<node>;
	const auto& t = tau::get(fm);
	DBG(assert(t.child_is(tau::wff_all));)
	const tref scoped_fm = t[0].second();
	const tref quant_var = t[0].first();

	const auto& st = tau::get(scoped_fm);
	if (st.child_is(tau::wff_and)) {
		// Push quantifier in
		const auto c0 = tau::build_wff_all(quant_var, st[0].first(), false);
		const auto c1 = tau::build_wff_all(quant_var, st[0].second(), false);
		return tau::build_wff_and(c0, c1);
	}
	else if (st.child_is(tau::wff_or)) {
		// Remove existential, if quant_var does not appear in clause
		auto clauses = get_dnf_wff_clauses<node>(scoped_fm);
		tref no_q_fm = tau::_F();
		for (tref& clause : clauses) {
			if (!contains<node>(clause, quant_var)) {
				no_q_fm = tau::build_wff_or(no_q_fm, clause);
				clause = tau::_F();
			}
		}
		tref q_fm = tau::build_wff_or(clauses);
		if (tau::get(q_fm).equals_F()) return scoped_fm;
		else if (tau::get(no_q_fm).equals_F()) return fm;
		else return tau::build_wff_or(
			tau::build_wff_all(quant_var, q_fm, false), no_q_fm);
	}
	else if (st.child_is(tau::wff_all)) {
		//other all quant, hence can switch them
		tref c = tau::build_wff_all(quant_var, st[0].second());
		return tau::build_wff_all(st[0].first(), c);
	}
	// Else check if quant_var is contained in subtree
	else if (contains<node>(scoped_fm, quant_var)) return fm;
	else return scoped_fm;
}

template <NodeType node>
tref push_quantifiers_in(tref formula) {
	using tau = tree<node>;
	subtree_unordered_set<node> excluded_nodes;
	auto push_quantifiers = [&excluded_nodes](tref n) {
		// static size_t counter = 0; DBG(assert(counter++ < 12);)
		if (is_child<node>(n, tau::wff_ex)) {
			LOG_DEBUG << "push_quantifiers existential: " << LOG_FM(n);
			tref pushed = push_existential_quantifier_one<node>(n);
			if (tau::get(pushed) == tau::get(n)) {
				// Quantifier cannot be pushed deeper
				for (tref c : tau::get(n).children())
					excluded_nodes.insert(c);
				return n;
			} else return pushed;
		} else if (is_child<node>(n, tau::wff_all)) {
			LOG_DEBUG << "push_quantifiers universal: " << LOG_FM(n);
			tref pushed = push_universal_quantifier_one<node>(n);
			if (tau::get(pushed) == tau::get(n)) {
				// Quantifier cannot be pushed deeper
				for (tref c : tau::get(n).children())
					excluded_nodes.insert(c);
				return n;
			} else return pushed;
		}
		LOG_DEBUG << "push_quantifiers nothing to do: " << LOG_FM(n);
		return n;
	};
	auto visit = [&excluded_nodes](tref n) {
		return visit_wff<node>(n) && !excluded_nodes.contains(n);
	};
	return pre_order<node>(formula).apply_unique(push_quantifiers, visit);
}

// Squeeze all equalities found in n
template <NodeType node>
tref squeeze_positives(tref n, size_t type_id) {
	using tau = tree<node>;
	using tt = tau::traverser;
	LOG_TRACE << "squeeze_positives: " << LOG_FM(n);
	auto match = [&type_id](tref n) {
		return is<node, tau::bf_eq>(n) &&
			find_ba_type<node>(n) == type_id;
	};
	if (trefs eqs = tau::get(n).select_top(match);
		eqs.size() > 0)
	{
		for (tref& eq : eqs) {
			eq = norm_trimmed_equation<node>(eq);
		}
		eqs = tt(eqs) | tt::children | tt::refs;
		tref res = tau::build_bf_or(eqs, find_ba_type<node>(eqs[0]));
		LOG_TRACE << "squeeze_positives result: " << LOG_FM(res);
		return res;
	}
	LOG_TRACE << "(I) squeeze_positives result: none";
	return nullptr;
}

template <NodeType node>
tref replace_free_vars_by(tref fm, tref val) {
	DBG(using tau = tree<node>;)
	DBG(assert(!is<node>(val, tau::bf));)
	const trefs& free_vars = get_free_vars<node>(fm);
	if (free_vars.size()) {
		subtree_map<node, tref> free_var_assgm;
		for (tref free_var : free_vars)
			free_var_assgm.emplace(free_var, val);
		return rewriter::replace<node>(fm, free_var_assgm);
	} else return fm;
}

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "anti_prenex"

/**
 * @brief The procedure tries to detect, using 0/1 substitutions for the provided
 * variable and syntactic comparison, if the atomic formula is equivalent to T
 * or F or independent of the provided variable.
 * @tparam node Type of tree node
 * @param atomic_fm The atomic formula, ie an equation, to simplify
 * @param var The variable to base the simplifications on
 * @return The simplified atomic formula
 */
template<NodeType node>
tref syntactic_variable_simplification(tref atomic_fm, tref var) {
	using tau = tree<node>;
	DBG(assert(tau::get(var).is(tau::variable));)
#ifdef TAU_CACHE
	using cache_t = std::unordered_map<std::pair<tref, tref>, tref>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(std::make_pair(tau::trim_right_sibling(atomic_fm),
		tau::trim_right_sibling(var))); it != end(cache))
		return it->second;
#endif // TAU_CACHE
	// Return early if atomic_fm is either T or F
	if (tau::get(atomic_fm).equals_T() || tau::get(atomic_fm).equals_F())
		return atomic_fm;
	DBG(LOG_TRACE << "Syntactic_variable_simplification on " << LOG_FM(atomic_fm) << "\n";)
	DBG(LOG_TRACE << "with var: " << LOG_FM(var) << "\n";)
	var = tau::get(tau::bf, var);
	tref func = tau::trim2(norm_equation<node>(atomic_fm));
	// Make sure that it works only on Boolean parts
	tref func_v_0 = rewriter::replace_if<node>(func, var,
		_0<node>(find_ba_type<node>(var)), is_boolean_operation<node>);
	tref func_v_1 = rewriter::replace_if<node>(func, var,
		_1<node>(find_ba_type<node>(var)), is_boolean_operation<node>);
	// Check if atomic formula is equality or inequality
	tref res = atomic_fm;
	if (tau::get(atomic_fm).child_is(tau::bf_eq)) {
		// Is func syntactically identically 0
		if (tau::get(func_v_0).equals_0() && tau::get(func_v_1).equals_0())
			res = _T<node>();
		// Is func syntactically identically 1
		if (tau::get(func_v_0).equals_1() && tau::get(func_v_1).equals_1())
			res = _F<node>();
		// func is not dependent on var
		else if (!contains<node>(func_v_0, var) && tau::get(func_v_0) == tau::get(func_v_1))
			res = rewriter::replace<node>(atomic_fm, var,
				_0<node>(find_ba_type<node>(var)));
	} else {
		DBG(assert(tau::get(atomic_fm).child_is(tau::bf_neq));)
		// Is func syntactically identically 0
		if (tau::get(func_v_0).equals_0() && tau::get(func_v_1).equals_0())
			res = _F<node>();
		// Is func syntactically identically 1
		if (tau::get(func_v_0).equals_1() && tau::get(func_v_1).equals_1())
			res = _T<node>();
		// func is not dependent on var
		else if (!contains<node>(func_v_0, var) && tau::get(func_v_0) == tau::get(func_v_1))
			res = rewriter::replace<node>(atomic_fm, var,
				_0<node>(find_ba_type<node>(var)));
	}
	DBG(LOG_TRACE << "Syntactic_variable_simplification result: " << LOG_FM(res) << "\n";)
#ifdef TAU_CACHE
	cache.emplace(std::make_pair(tau::trim_right_sibling(res),
		tau::trim_right_sibling(var)), res);
	return cache.emplace(std::make_pair(tau::trim_right_sibling(atomic_fm),
		tau::trim_right_sibling(var)), res).first->second;
#endif // TAU_CACHE
	return res;
}

/**
 * @brief Simplify using assumptions from equalities in formula.
 * @tparam node Type of tree node
 */
template <NodeType node>
struct simplify_using_equality {
	using tau = tree<node>;
	// TODO: For variables, make input < output and lower time step < higher time step
	// Create comparator function that orders bfs by making constants smallest
	// We have 0 < 1 < bf_constant < uninterpreted_constant < variable < rest by node count
	static bool term_comp(tref l, tref r) {
		if (tau::get(l).equals_0()) {
			if (!tau::get(r).equals_0()) return true;
			else return false;
		}
		if (tau::get(r).equals_0()) return false;
		// 1 is automatically rewritten to 0
		// if (l == _1<node>()) return true;
		// if (r == _1<node>()) return false;
		if (is_child<node>(l, tau::ba_constant)) {
			if (!is_child<node>(r, tau::ba_constant)) return true;
			else return tau::subtree_less(l,r);
		}
		if (is_child<node>(r, tau::ba_constant)) return false;
		if (is_child<node>(l, tau::variable)) {
			if (is_child<node>(r, tau::variable)) {
				// Check for uninterpreted constant
				if (is_child<node>(tau::trim(l), tau::uconst)) {
					if (!is_child<node>(tau::trim(r), tau::uconst))
						return true;
					else return tau::subtree_less(l,r);
				}
				if (is_child<node>(tau::trim(r), tau::uconst)) return false;
				return tau::subtree_less(l,r);
			} else return true;
		}
		if (is_child<node>(r, tau::variable)) return false;
		// TODO: also use free_vars count once constant time
		return tau::subtree_less(l,r);
	};

	// Given a formula, traverse the formula and
	// apply equality simplifications along the way
	static tref on (tref fm) {
		// Push negation all the way in
		fm = to_nnf<node>(fm);
		if (tau::get(fm).equals_T() || tau::get(fm).equals_F())
			return fm;
		DBG(LOG_DEBUG << "Simplify_using_equality on " << LOG_FM(fm) << "\n";)
		// Create union find data structure to hold equality information
		auto uf = union_find_with_sets<decltype(term_comp), node>(term_comp);
		// Create stack of union find data structures
		std::vector<union_find_with_sets<decltype(term_comp), node>> uf_stack {uf};
		// We need to mark disjunctions that do not cause a push to the
		// stack, in order to make sure they are not popped later
		// due to intermediate simplifications
		subtree_unordered_set<node> mark;
		// Traverse the formula: on encounter of = or !=, first apply simplification and, if equality, add it
		auto f = [&uf_stack, &mark](tref n, tref parent) {
			if (!is<node>(n, tau::wff)) return n;
			const tau& cn = tau::get(n)[0];
			if (parent != nullptr && is<node>(parent, tau::wff_or)) {
				if (!cn.is(tau::wff_or)) {
					// Push new uf to stack
					uf_stack.push_back(uf_stack.back());
				} else mark.insert(parent);
			}
			if (cn.is(tau::bf_eq)) {
				n = syntactic_atomic_formula_simplification<node>(n);
				tref s = simplify_equation(uf_stack.back(), n);
				// If equation was simplified away
				if (!is_child<node>(s, tau::bf_eq)) return s;
				if (add_equality(uf_stack.back(), s)) return s;
				else return _F<node>();
			} else if (cn.is(tau::bf_neq)) {
				n = syntactic_atomic_formula_simplification<node>(n);
				return simplify_equation(uf_stack.back(), n);
			} else if (cn.is(tau::wff_and)) {
				// Check if conjunction was already processed
				if (mark.contains(n)) return n;
				// We need to reorder all conjunctions in order
				// to correctly collect all equalities
				trefs conjs = get_cnf_wff_clauses<node>(n);
				sort_equations(conjs);
				n = conjs[0];
				for (size_t i = 1; i < conjs.size(); ++i) {
					n = tau::build_wff_and(n, conjs[i]);
					mark.insert(n);
				}
				return n;
			} else return n;
		};
		auto up = [&uf_stack, &mark](tref n, tref parent) {
			if (!is<node>(n, tau::wff)) return n;
			if (parent != nullptr && is<node>(parent, tau::wff_or)) {
				if (!is_child<node>(n, tau::wff_or)) {
					// If parent was marked, child was a disjunction
					// before and nothing was pushed on stack
					if (auto it = mark.find(parent); it != mark.end()) {
						mark.erase(it);
					} else uf_stack.pop_back();
				} else {
					// Encounter of disjunction below disjunction
					// If not marked, this happened due to simplification
					// and we can safely pop
					if (auto it = mark.find(parent); it == mark.end())
						uf_stack.pop_back();
					else mark.erase(it);
				}
			}
			return n;
		};
		auto visit = [](tref n) {
			if (is_quantifier<node>(n)) return false;
			if (is_temporal_quantifier<node>(n)) return false;
			return visit_wff<node>(n);
		};
		fm = pre_order<node>(fm).apply(f, visit, up);
		DBG(LOG_DEBUG << "Simplify_using_equality result: " << LOG_FM(fm) << "\n";)
		// In DEBUG make sure that push and pop on stack is correct
		DBG(assert(uf_stack.size() == 1);)
		return fm;
	}

	// Given an equality, add it to the given union find data structure
	// Returns false, if a resulting equality set contains a contradiction
	static bool add_raw_equality (auto& uf, tref eq) {
		if (tau::get(eq).equals_T()) return true;
		if (tau::get(eq).equals_F()) return false;
		DBG(LOG_TRACE << "Adding equality: " << tau::get(eq) << "\n";)
		// Here we know that eq is still bf_eq
		// We both add a = b and a' = b' in order to detect syntactic contradictions
		tref left_hand_side = tau::trim2(eq);
		tref right_hand_side = tau::get(tau::trim(eq)).child(1);
		tref left_hand_side_neg =
			push_negation_in<node, false>(build_bf_neg<node>(left_hand_side));
		tref right_hand_side_neg =
			push_negation_in<node, false>(build_bf_neg<node>(right_hand_side));

		size_t type_id = find_ba_type<node>(eq);
		uf.merge(left_hand_side, right_hand_side);
		uf.merge(left_hand_side_neg, right_hand_side_neg);
		uf.merge(build_bf_and<node>(left_hand_side, right_hand_side_neg), _0<node>(type_id));
		uf.merge(build_bf_and<node>(right_hand_side, left_hand_side_neg), _0<node>(type_id));
		uf.merge(build_bf_or<node>(left_hand_side, right_hand_side_neg), _1<node>(type_id));
		uf.merge(build_bf_or<node>(right_hand_side, left_hand_side_neg), _1<node>(type_id));
		// Check if syntactic contradiction occurred
		return !uf.connected(left_hand_side, left_hand_side_neg)
				&& !uf.connected(right_hand_side, right_hand_side_neg)
					&& !uf.connected(_0<node>(type_id), _1<node>(type_id));
	}

	// In case eq is of form f = 0, unsqueeze disjunctions in f
	static bool add_equality(auto& uf, tref eq) {
		if (const tau& c = tau::get(eq)[0]; c[1].equals_0()) {
			tref func = push_negation_in<node, false>(c.first());
			bool valid = true;
			for (tref disj : get_dnf_bf_clauses<node>(func)) {
				valid = valid && add_raw_equality(uf,
					tau::build_bf_eq_0(disj));
			}
			return valid;
		} else return add_raw_equality(uf, eq);
	}

	// Given current equalities in union find, simplify the equation
	static tref simplify_equation (auto& uf, tref eq) {
		DBG(LOG_TRACE << "Simplifying: " << tau::get(eq) << "\n";)
		if (tau::get(eq).equals_T() || tau::get(eq).equals_F()) return eq;
		auto uf_find = [&uf](tref n) {
			if (uf.contains(n)) return uf.find(n);
			return n;
		};
		tref simp_eq = pre_order<node>(eq).apply(uf_find);
		DBG(LOG_TRACE << "Simplified to: " << tau::get(simp_eq) << "\n";)
		if (tau::get(simp_eq) != tau::get(eq))
			return simp_eq;
		else return eq;
	}

private:
	static void sort_equations (auto& conjs) {
		auto direct_eq = [](tref eq) {
			if (!is_child<node>(eq, tau::bf_eq)) return eq;
			tref c1 = tau::get(eq)[0].first();
			tref c2 = tau::get(eq)[0].second();
			if (term_comp(c1, c2))
				return tau::build_bf_eq(c2, c1);
			return eq;
		};
		auto eq_comp = [](tref l, tref r) {
			const tau& nl = tau::get(l);
			const tau& nr = tau::get(r);
			if (nl.child_is(tau::bf_eq)) {
				if (!nr.child_is(tau::bf_eq)) return true;
				// Both are equalities
				if (term_comp(nl[0].first(), nr[0].first()))
					return true;
				return false;
			} else return false;
		};
		std::ranges::transform(conjs, conjs.begin(), direct_eq);
		std::ranges::sort(conjs, eq_comp);
	}
};

/**
 * @brief Syntactic simplifications for a formula or a term based on its paths.
 * @tparam node Type of tree node
 */
template <NodeType node>
class syntactic_path_simplification {
	using tau = tree<node>;

	static bool wff_comp (tref l, tref r) {
		if (tau::get(l).child_is(tau::bf_eq))
			if (tau::get(r).child_is(tau::bf_eq))
				return tau::subtree_less(l,r);
			else return true;
		else {
			if (tau::get(r).child_is(tau::bf_eq))
				return false;
			else return tau::subtree_less(l, r);
		}
	}

	static tref simplify_wff (tref root) {
		tref skip = nullptr;
		auto down = [&skip](tref n) {
			// Skip intermediate nodes
			if (!tau::get(n).is(tau::wff)) return n;
			const tau& t_n = tau::get(n)[0];
			// Skip or
			if (t_n.is(tau::wff_or))
				return n;
			// If no conjunction is found, no simplification is possible
			if (!t_n.is(tau::wff_and)) return n;
			subtree_map<node, tref> assignments;
			// Get all conjuncted assumptions
			for (tref l : get_cnf_wff_clauses<node>(n)) {
				if (tau::get(l).child_is(tau::wff_or))
					continue;
				// Treat subtree l as variable
				if (tau::get(l).child_is(tau::wff_neg))
					assignments.emplace(tau::trim2(l), _F<node>());
				else assignments.emplace( l, _T<node>());
			}
			tref simp = rewriter::replace(n, assignments);
			// If simp is false, current branch is not sat
			if (tau::get(simp).equals_F()) {
				// Remove branch
				return _F<node>();
			}
			// Rebuild assumptions
			trefs sorted_assms;
			sorted_assms.reserve(assignments.size());
			for (auto [v, k] : assignments)
				if (k == tau::_F())
					sorted_assms.push_back(tau::build_wff_neg(v));
				else sorted_assms.push_back(v);
			// Canonical sorting
			std::ranges::stable_sort(sorted_assms, wff_comp);
			tref assms = tau::build_wff_and(sorted_assms);
			// Make sure to not revisit build assumptions
			if (tau::get(simp) == get_T<node>())
				skip = tau::get(assms).first();
			else skip = assms;
			// Conjunct assumptions with simplified tree
			return tau::build_wff_and(assms, simp);
		};
		auto visit = [&skip](tref n) {
			if (skip != nullptr && tau::get(n) == tau::get(skip))
				return skip = nullptr, false;
			return visit_wff<node>(n);
		};
		return pre_order<node>(root).template
			apply_unique<synt_path_simp_m>(down, visit);
	}

	static tref simplify_bf(tref root) {
		tref skip = nullptr;
		auto down = [&skip](tref n) {
			// Skip intermediate nodes
			if (!tau::get(n).is(tau::bf)) return n;
			const tau& t_n = tau::get(n)[0];
			// Skip or and xor
			if (t_n.is(tau::bf_or) || t_n.is(tau::bf_xor))
				return n;
			// If no conjunction is found, no simplification is possible
			if (!t_n.is(tau::bf_and)) return n;
			subtree_map<node, tref> assignments;
			// Get all conjuncted assumptions
			for (tref l : get_cnf_bf_clauses<node>(n)) {
				if (tau::get(l).child_is(tau::bf_or))
					continue;
				if (tau::get(l).child_is(tau::bf_xor))
					continue;
				// Treat subtree l as variable
				if (tau::get(l).child_is(tau::bf_neg))
					assignments.emplace(tau::trim2(l),
						_0<node>(find_ba_type<node>(l)));
				else assignments.emplace( l,
					_1<node>(find_ba_type<node>(l)));
			}
			tref simp = rewriter::replace_if(n, assignments,
				is_boolean_operation<node>);
			// If simp is false, current branch is not sat
			if (tau::get(simp).equals_0()) {
				// Remove branch
				return simp;
			}
			// Rebuild assumptions
			trefs sorted_assms (assignments.size());
			// Canonical sorting
			std::ranges::partial_sort_copy(std::views::keys(assignments),
				sorted_assms, tau::subtree_less);
			tref assms = _1<node>(find_ba_type<node>(n));
			for (tref a : sorted_assms) {
				assms = tau::get(assignments[a]).equals_1()
						? tau::build_bf_and(assms, a)
						: tau::build_bf_and(assms,
							tau::build_bf_neg(a));
			}
			// Make sure to not revisit build assumptions
			if (tau::get(simp).equals_1())
				skip = tau::get(assms).first();
			else skip = assms;
			// Conjunct assumptions with simplified tree
			return tau::build_bf_and(assms, simp);
		};
		auto visit = [&](tref n) {
			if (skip != nullptr && tau::get(n) == tau::get(skip))
				return skip = nullptr, false;
			return is_boolean_operation<node>(n);
		};
		return pre_order<node>(root).template
			apply_unique<synt_path_simp_m>(down, visit);
	}

public:
	/**
	 * @brief Simplify contradictions on the formula that appear along a path.
	 * Then do the same for the negated formula, resolving simple tautologies.
	 * The procedure pushes negations all the way in.
	 * @param fm Formula to simplify
	 * @return Simplified formula
	 */
	static tref on (tref fm) {
		DBG(LOG_DEBUG << "Syntactic_path_simplification on " << LOG_FM(fm) << "\n";)
		tref res = nullptr;
		if (tau::get(fm).is_term()) {
			if (tau::get(fm).equals_0() || tau::get(fm).equals_1())
				return fm;
			// Resolve contradictions
			fm = push_negation_in<node, false>(fm);
			fm = simplify_bf(fm);
			// Resolve tautologies
			fm = push_negation_in<node, false>(tau::build_bf_neg(fm));
			fm = simplify_bf(fm);
			res = push_negation_in<node, false>(tau::build_bf_neg(fm));
		} else {
			if (tau::get(fm).equals_F() || tau::get(fm).equals_T())
				return fm;
			// Resolve contradictions
			fm = unequal_to_not_equal<node>(to_nnf<node>(fm));
			fm = simplify_wff(fm);
			// Resolve tautologies
			fm = unequal_to_not_equal<node>(to_nnf<node>(tau::build_wff_neg(fm)));
			fm = simplify_wff(fm);
			res = to_nnf<node>(tau::build_wff_neg(fm));
		}
		DBG(LOG_DEBUG << "Syntactic_path_simplification result: " << LOG_FM(res) << "\n";)
		return res;
	}

	/**
	 * @brief Only simplify contradictions on paths of formula while not manipulating
	 * present negations.
	 * @param fm Formula to simplify
	 * @return Simplified formula
	 */
	static tref unsat_on_unchanged_negations (tref fm) {
		if (tau::get(fm).is_term()) {
			if (tau::get(fm).equals_0() || tau::get(fm).equals_1())
				return fm;
			// Resolve contradictions
			return simplify_bf(fm);
		} else {
			// Resolve contradiction
			return simplify_wff(fm);
		}
	}
};

/**
 * @brief Function to apply syntactical simplifications to formula in almost
 * linear time in the formula size and the number of paths found in terms
 * @tparam node tree node type
 * @param formula The formula to simplify
 * @return The simplified formula
 */
template<NodeType node>
tref syntactic_formula_simplification(tref formula) {
	formula = simplify_using_equality<node>::on(formula);
	return syntactic_path_simplification<node>::on(formula);
}

/**
 * @brief Comparator for the BDD variable order used during simplification of
 * terms. It is used with std::stable_sort in order to preserve initial order
 * in the found BDD variables.
 * @tparam node Type of tree node
 */
template<NodeType node>
auto variable_order_for_simplification = [](tref l, tref r) static {
	// Order is: smaller time point first, if equal
	// constant input < constant output < input < output < other variable
	// while all variables are ordered by var_order
	using tau = tree<node>;
	DBG(assert(tau::get(l).is(tau::variable));)
	DBG(assert(tau::get(r).is(tau::variable));)
	// Reject equal
	if (tau::get(l) == tau::get(r)) return false;
	if (is_io_var<node>(l)) {
		// Check if r is also stream
		if (is_io_var<node>(r)) {
			if (is_io_initial<node>(l)) {
				if (is_io_initial<node>(r)) {
					// Both are initial
					const int_t tl = get_io_time_point<node>(l),
						tr = get_io_time_point<node>(r);
					if (tl < tr) {
						// l is earlier
						return true;
					} else if (tl == tr) {
						// Check input < output
						if (tau::get(l).is_input_variable()) {
							if (tau::get(r).is_input_variable()) {
								// Both input -> compare equal
								return false;
							} else return true;
						} if (tau::get(r).is_input_variable()) {
							// Same time step, r is input and l is not
							return false;
						} else return false;
					} else return false;
				} else return true;
			} else if (is_io_initial<node>(r)) {
				// r initial but l not
				return false;
			} else {
				// Both are non initial
				const int_t sl = get_io_var_shift<node>(l),
					sr = get_io_var_shift<node>(r);
				if (sl > sr) {
					// l is earlier
					return true;
				} else if (sl == sr) {
					// Check input < output
					if (tau::get(l).is_input_variable()) {
						if (tau::get(r).is_input_variable()) {
							// Both input -> compare equal
							return false;
						} else return true;
					} if (tau::get(r).is_input_variable()) {
						// Same time step, r is input and l is not
						return false;
					} else return false;
				} else return false;
			}
		} else return false; // compare equal
	} else return false; // compare equal
};

/**
 * @brief Comparator for the BDD variable order used during simplification of a
 * formula. It is used with std::stable_sort in order to preserve initial order
 * in the found BDD variables.
 * @tparam node Type of tree node
 */
template<NodeType node>
auto atm_formula_order_for_simplification = [](tref l, tref r) static {
	// Order is decided by three factors
	// 1) lowest time points in free variables have priority, then
	// 2) lowest highest time points in free variables and last
	// 3) number of free io variables
	DBG(using tau = tree<node>;)
	DBG(assert(tau::get(l).child_is(tau::bf_eq));)
	DBG(assert(tau::get(r).child_is(tau::bf_eq));)
	// For l
	std::pair<bool, int_t> low_t_l {true, 0}, high_t_l {true, 0};
	bool is_high_init = false;
	const trefs& free_vars_l = get_free_vars<node>(l);
	size_t io_free_var_size_l = 0;
	for (tref v : free_vars_l) {
		if (!is_io_var<node>(v)) continue;
		++io_free_var_size_l;
		if (is_io_initial<node>(v)) {
			const int_t t = get_io_time_point<node>(v);
			if (low_t_l.second >= t) {
				low_t_l.first = false;
				low_t_l.second = t;
			}
			if (high_t_l.second <= t || !is_high_init) {
				high_t_l.first = false;
				high_t_l.second = t;
				is_high_init = true;
			}
		} else {
			if (low_t_l.first)
				if (const int_t t = get_io_var_shift<node>(v);
					low_t_l.second < t)
					low_t_l.second = t;
			 if (high_t_l.first)
				if (const int_t t = get_io_var_shift<node>(v);
					high_t_l.second > t || !is_high_init) {
					high_t_l.second = t;
					is_high_init = true;
				}
		}
	}
	// For r
	std::pair<bool, int_t> low_t_r {true, 0}, high_t_r {true, 0};
	is_high_init = false;
	const trefs& free_vars_r = get_free_vars<node>(r);
	size_t io_free_var_size_r = 0;
	for (tref v : free_vars_r) {
		if (!is_io_var<node>(v)) continue;
		++io_free_var_size_r;
		if (is_io_initial<node>(v)) {
			const int_t t = get_io_time_point<node>(v);
			if (low_t_r.second >= t) {
				low_t_r.first = false;
				low_t_r.second = t;
			}
			if (high_t_r.second <= t || !is_high_init) {
				high_t_r.first = false;
				high_t_r.second = t;
				is_high_init = true;
			}
		} else {
			if (low_t_r.first)
				if (const int_t t = get_io_var_shift<node>(v);
					low_t_r.second < t)
					low_t_r.second = t;
			if (high_t_r.first)
				if (const int_t t = get_io_var_shift<node>(v);
					high_t_r.second > t || !is_high_init) {
					high_t_r.second = t;
					is_high_init = true;
				}
		}
	}
	// Check that both have io variables
	if (io_free_var_size_l == 0)
		return false;
	if (io_free_var_size_r == 0) {
		return true;
	}
	// l and r both have at least one io variable
	if (low_t_l.first < low_t_r.first) return true;
	if (low_t_r.first < low_t_l.first) return false;
	// Here low_t_l.first == low_t_r.first
	if (low_t_l.first) {
		// No initial condition, so higher lookback wins
		if (low_t_l.second > low_t_r.second) return true;
		if (low_t_r.second > low_t_l.second) return false;
		// Here lookback is same, compare highest time point
		// We also know that no initial condition is present
		if (high_t_l.second > high_t_r.second) return true;
		if (high_t_r.second > high_t_l.second) return false;
		// Here lookback is also same
		return io_free_var_size_l < io_free_var_size_r;
	} else {
		// Compare initial conditions
		if (low_t_l.second < low_t_r.second) return true;
		if (low_t_r.second < low_t_l.second) return false;
		// Smallest initials are same, compare highest, also initial conditions by assumption
		if (high_t_l.second < high_t_r.second) return true;
		if (high_t_r.second < high_t_l.second) return false;
		// Here highest initials are same
		return io_free_var_size_l < io_free_var_size_r;
	}
};

/**
 * @brief Comparator for the BDD variable order used during anti-prenex algorithm.
 * @tparam node Type of tree node
 */
template<NodeType node>
auto atm_formula_order_for_quant_elim(auto& quant_pattern) {
	auto comp = [&quant_pattern](tref l, tref r) {
		const trefs& free_vars_l = get_free_vars<node>(l);
		int_t min_l = 0, max_l = 0;
		bool is_min_init = false;
		for (tref v : free_vars_l) {
			auto it = quant_pattern.find(v);
			// Not contained, means not quantified
			if (it == quant_pattern.end()) {
				min_l = 0;
				is_min_init = true;
				continue;
			}
			if (it->second < min_l || !is_min_init)
				min_l = it->second, is_min_init = true;
			if (max_l < it->second) max_l = it->second;
		}
		const trefs& free_vars_r = get_free_vars<node>(r);
		int_t min_r = 0, max_r = 0;
		is_min_init = false;
		for (tref v : free_vars_r) {
			auto it = quant_pattern.find(v);
			// Not contained, means not quantified
			if (it == quant_pattern.end()) {
				min_r = 0;
				is_min_init = true;
				continue;
			}
			if (it->second < min_r || !is_min_init)
				min_r = it->second, is_min_init = true;
			if (max_r < it->second) max_r = it->second;
		}
		if (max_l > max_r) return true;
		if (max_r > max_l) return false;
		if (min_l > min_r) return true;
		if (min_r > min_l) return false;
		if (free_vars_l < free_vars_r) return true;
		return false;
	};
	return comp;
}

/**
 * @brief Applies syntactic simplifications to an atomic formula, ie an equation.
 * @tparam node Tree node type
 * @param atomic_formula Formula to simplify
 * @return Simplified formula
 */
template<NodeType node>
tref syntactic_atomic_formula_simplification(tref atomic_formula) {
	using tau = tree<node>;
	bool is_eq = tau::get(atomic_formula).child_is(tau::bf_eq);
	DBG(assert(is_eq || tau::get(atomic_formula).child_is(tau::bf_neq));)
	// Bring the equation to (!)= 0
	atomic_formula = norm_equation<node>(atomic_formula);
	if (tau::get(atomic_formula).equals_T() ||
		tau::get(atomic_formula).equals_F()) return atomic_formula;
	tref func = tau::trim2(atomic_formula);
	// Apply syntactic path simplification
	func = syntactic_path_simplification<node>::on(func);
	// Apply syntactic variable simplification for each found free variable
	atomic_formula = is_eq
				 ? tau::build_bf_eq_0(func)
				 : tau::build_bf_neq_0(func);
	// Bring the equation back to its original form
	atomic_formula = denorm_equation<node>(atomic_formula);
	auto& free_vars = get_free_vars<node>(atomic_formula);
	for (tref v : free_vars) {
		atomic_formula =
			syntactic_variable_simplification<node>(atomic_formula, v);
	}
	return atomic_formula;
}

// Squeeze two equations equal/unequal to zero into one equation
template <NodeType node>
tref squeeze(tref eq1, tref eq2) {
	using tau = tree<node>;
	// Both equations must be !(=) 0
	DBG(assert(tau::get(eq1)[0][1].equals_0() &&
		tau::get(eq2)[0][1].equals_0());)
	if (tau::get(eq1).child_is(tau::bf_eq)) {
		DBG(assert(tau::get(eq2).child_is(tau::bf_eq));)
		// Squeeze positive equations
		return tau::build_bf_eq_0(
			tau::build_bf_or(tau::trim2(eq1), tau::trim2(eq2)));
	} else if (tau::get(eq1).child_is(tau::bf_neq)) {
		DBG(assert(tau::get(eq2).child_is(tau::bf_neq));)
		// Squeeze negative equations
		return tau::build_bf_neq_0(
		tau::build_bf_or(tau::trim2(eq1), tau::trim2(eq2)));
	}
	DBG(assert(false);)
	return nullptr;
}

template <NodeType node>
tref apply_assms(tref eq, const auto& assms, auto& joins, trefs& additions, bool dual = false) {
	using tau = tree<node>;
	DBG(assert(tau::get(eq)[0][1].equals_0());)
	const trefs& fv = get_free_vars<node>(eq);
	DBG(assert(tau::get(eq).child_is(tau::bf_eq) ||
		tau::get(eq).child_is(tau::bf_neq));)
	const bool is_eq_pos = tau::get(eq).child_is(tau::bf_eq);
	bool joined = false;
	for (tref assm : assms.back()) {
		// Check for overlap between current assumption and eq
		const trefs& fv_a = get_free_vars<node>(assm);
		if (const int_t count = get_ordered_overlap<node>(fv, fv_a); count > 1 ||
			(count == 1 && fv.size() == 1)) {
			// Apply assumption
			if (is_eq_pos) {
				if (dual) {
					eq = tau::build_bf_eq_0(tau::build_bf_and(
						tau::trim2(eq), tau::build_bf_neg(assm)));
				} else {
					DBG(LOG_TRACE << "Merging " << tau::get(assm) << " and " << tau::get(tau::trim2(eq)) << "\n";)
					joins.merge(assm, tau::trim2(eq));
					joined = true;
					eq = tau::build_bf_eq_0(tau::build_bf_or(
						tau::trim2(eq), assm));
				}
			} else {
				if (dual) {
					DBG(LOG_TRACE << "Merging " << tau::get(assm) << " and " << tau::get(tau::trim2(eq)) << "\n";)
					joins.merge(assm, tau::trim2(eq));
					joined = true;
					eq = tau::build_bf_neq_0(tau::build_bf_or(
						tau::trim2(eq), assm));
				}
				eq = tau::build_bf_neq_0(tau::build_bf_and(
					tau::trim2(eq), tau::build_bf_neg(assm)));
			}
		} else if (count == 1 && is_eq_pos) {
			DBG(LOG_TRACE << "Merging " << tau::get(assm) << " and " << tau::get(tau::trim2(eq)) << "\n";)
			joins.merge(assm, tau::trim2(eq));
			joined = true;
		}
	}
	if (!joined) if ((is_eq_pos && !dual) || (!is_eq_pos && dual)) {
		DBG(LOG_TRACE << "Addition: " << tau::get(tau::trim2(eq)) << "\n";)
		additions.emplace_back(tau::trim2(eq));
	}
	return eq;
}

template <NodeType node>
tref apply_assms(tref eq, const auto& assms, bool dual = false) {
	using tau = tree<node>;
	DBG(assert(tau::get(eq)[0][1].equals_0());)
	const trefs& fv = get_free_vars<node>(eq);
	DBG(assert(tau::get(eq).child_is(tau::bf_eq) ||
		tau::get(eq).child_is(tau::bf_neq));)
	const bool is_eq_pos = tau::get(eq).child_is(tau::bf_eq);
	for (tref assm : assms.back()) {
		// Check for overlap between current assumption and eq
		const trefs& fv_a = get_free_vars<node>(assm);
		if (const int_t count = get_ordered_overlap<node>(fv, fv_a); count > 1 ||
			(count == 1 && fv.size() == 1)) {
			// Apply assumption
			if (is_eq_pos) {
				if (dual) eq = tau::build_bf_eq_0(tau::build_bf_and(
					tau::trim2(eq), tau::build_bf_neg(assm)));
				else eq = tau::build_bf_eq_0(tau::build_bf_or(
					tau::trim2(eq), assm));
				DBG(LOG_TRACE << "From assumption " << tau::get(assm) << " produce " << tau::get(eq) << "\n";)
			} else {
				if (dual) eq = tau::build_bf_neq_0(tau::build_bf_or(
					tau::trim2(eq), assm));
				else eq = tau::build_bf_neq_0(tau::build_bf_and(
					tau::trim2(eq), tau::build_bf_neg(assm)));
				DBG(LOG_TRACE << "From assumption " << tau::get(assm) << " produce " << tau::get(eq) << "\n";)
			}
		}
	}
	return eq;
}

template <NodeType node>
tref apply_assms(tref eq, tref assm, tref var, const bool dual = false) {
	using tau = tree<node>;
	DBG(assert(tau::get(eq)[0][1].equals_0());)
	const trefs& fv = get_free_vars<node>(eq);
	if (!std::binary_search(fv.begin(), fv.end(), var, tau::subtree_less))
		return eq;
	DBG(assert(tau::get(eq).child_is(tau::bf_eq) ||
		tau::get(eq).child_is(tau::bf_neq));)
	const bool is_eq_pos = tau::get(eq).child_is(tau::bf_eq);
	// Check for overlap between current assumption and eq
	const trefs& fv_a = get_free_vars<node>(assm);
	if (fv.size() == 1 || is_ordered_overlap_at_least<node>(2,fv, fv_a)) {
		// Apply assumption
		if (is_eq_pos) {
			if (dual) eq = tau::build_bf_eq_0(tau::build_bf_and(
				tau::trim2(eq), tau::build_bf_neg(assm)));
			else eq = tau::build_bf_eq_0(tau::build_bf_or(
				tau::trim2(eq), assm));
			DBG(LOG_TRACE << "From assumption " << tau::get(assm) << " produce " << tau::get(eq) << "\n";)
		} else {
			if (dual) eq = tau::build_bf_neq_0(tau::build_bf_or(
				tau::trim2(eq), assm));
			else eq = tau::build_bf_neq_0(tau::build_bf_and(
				tau::trim2(eq), tau::build_bf_neg(assm)));
			DBG(LOG_TRACE << "From assumption " << tau::get(assm) << " produce " << tau::get(eq) << "\n";)
		}
	}
	return eq;
}

template <NodeType node>
tref apply_assms(tref eq, tref assm, tref var, trefs& updates, const bool dual = false) {
	using tau = tree<node>;
	DBG(assert(tau::get(eq)[0][1].equals_0());)
	const trefs& fv = get_free_vars<node>(eq);
	if (!std::binary_search(fv.begin(), fv.end(), var, tau::subtree_less))
		return eq;
	DBG(assert(tau::get(eq).child_is(tau::bf_eq) ||
		tau::get(eq).child_is(tau::bf_neq));)
	const bool is_eq_pos = tau::get(eq).child_is(tau::bf_eq);
	if ((is_eq_pos && !dual) || (!is_eq_pos && dual))
		updates.emplace_back(eq);
	// Check for overlap between current assumption and eq
	const trefs& fv_a = get_free_vars<node>(assm);
	if (fv.size() == 1 || is_ordered_overlap_at_least<node>(2,fv, fv_a)) {
		// Apply assumption
		if (is_eq_pos) {
			if (dual) eq = tau::build_bf_eq_0(tau::build_bf_and(
				tau::trim2(eq), tau::build_bf_neg(assm)));
			else eq = tau::build_bf_eq_0(tau::build_bf_or(
				tau::trim2(eq), assm));
			DBG(LOG_TRACE << "From assumption " << tau::get(assm) << " produce " << tau::get(eq) << "\n";)
		} else {
			if (dual) eq = tau::build_bf_neq_0(tau::build_bf_or(
				tau::trim2(eq), assm));
			else eq = tau::build_bf_neq_0(tau::build_bf_and(
				tau::trim2(eq), tau::build_bf_neg(assm)));
			DBG(LOG_TRACE << "From assumption " << tau::get(assm) << " produce " << tau::get(eq) << "\n";)
		}
	}
	return eq;
}

template <NodeType node>
void update_assms(auto& assms, auto& joins, trefs& additions) {
	using tau = tree<node>;
	subtree_set<node> excluded;
	trefs& A = assms.back();
	for (size_t i = 0; i < A.size(); ++i) {
		if (excluded.contains(A[i])) {
			A.erase(A.begin()+i);
			DBG(assert(i > 0);)
			--i;
			continue;
		}
		// Get the set of joinable assumptions from joins
		trefs col = joins.get_set(A[i]);
		for (tref c : col) excluded.insert(c);
		DBG(LOG_TRACE << "Updated assumption from " << tau::get(A[i])
			<< " to " << tau::get(tau::build_bf_or(col,
				find_ba_type<node>(A[i]))) << "\n";)
		A[i] = tau::build_bf_or(col, find_ba_type<node>(A[i]));
	}
	for (tref add : additions) {
		DBG(LOG_TRACE << "Added to assumption: " << tau::get(add) << "\n";)
		A.emplace_back(add);
	}
}

/**
 * @brief The procedure collects all = 0 and != 0 equations within scope
 * (conjunction and disjunction respectively) and squeezes
 * those together that share at least one variable. Each squeezed formula A is
 * then integrated into terms in scope that share at least 2 variables including
 * the provided one in the following way:
 * - if A is = 0 and given f = 0, we produce f|A = 0
 * - if A is = 0 and given f != 0, we produce f & A' != 0
 * - if A is != 0 and given f != 0, we produce f|A != 0
 * - if A is != 0 and given f = 0, we produce f & A' = 0
 * @tparam node Tree node type
 * @param formula The formula to apply the procedure to
 * @return The mutated formula
 */
template<NodeType node>
tref squeeze_absorb(tref formula) {
	using tau = tree<node>;
	DBG(LOG_DEBUG << "Started squeeze_absorb");
	std::vector<trefs> assms { trefs {} };
	std::vector<trefs> dual_assms { trefs {} };
	subtree_unordered_set<node> mark;
	auto uf_comp = [](tref l, tref r) {
		return tau::subtree_less(l, r);
	};
	auto f = [&](tref n, tref parent) {
		if (!tau::get(n).is(tau::wff)) return n;
		const tau& cn = tau::get(n)[0];
		if (parent != nullptr && is<node>(parent, tau::wff_or)) {
			if (!cn.is(tau::wff_or)) {
				// Push new assumption to stack
				assms.push_back(assms.back());
			}
		} else if (parent != nullptr && is<node>(parent, tau::wff_and)) {
			if (!cn.is(tau::wff_and)) {
				// Push new assumption to stack
				dual_assms.push_back(dual_assms.back());
			}
		}
		// Try getting conjunctions
		if (trefs conjs = get_cnf_wff_clauses<node>(n); conjs.size() > 1) {
			// Sort the equations in conjs up front and prepare
			// union find to compute variable overlaps
			size_t eq_idx = 0;
			auto uf = union_find_with_sets<decltype(uf_comp), node>(uf_comp);
			for (tref& conj : conjs) {
				const tau& conj_t = tau::get(conj);
				if (conj_t.child_is(tau::bf_neq)) {
					// Only treat != 0 equations
					if (!conj_t[0][1].equals_0()) continue;
					std::swap(conj, conjs[eq_idx++]);
				} else if (conj_t.child_is(tau::bf_eq)) {
					// Only treat = 0 equations
					if (!conj_t[0][1].equals_0()) continue;
					// merge variables of = 0 equations
					const trefs& fv = get_free_vars<node>(conj);
					for (tref v : fv) uf.merge(fv[0], v);
					std::swap(conj, conjs[eq_idx++]);
				}
			}
			// Return early if no equation present
			if (eq_idx == 0) return n;
#ifdef DEBUG
			// Print sorted conjuncts
			LOG_TRACE << "Conjuncts with eqs first: ";
			for (tref el : conjs) LOG_TRACE << tau::get(el) << ", ";
#endif
			// Apply dual assumptions to equations
			for (size_t i = 0; i < eq_idx; ++i)
				conjs[i] = apply_assms<node>(conjs[i], dual_assms, true);
			// Squeeze = 0 equations that share a variable
			// First sort = 0 up front
			size_t pos_eq_idx = 0;
			for (size_t i = 0; i < eq_idx; ++i) {
				if (tau::get(conjs[i]).child_is(tau::bf_eq))
					std::swap(conjs[i], conjs[pos_eq_idx++]);
			}
			for (size_t i = 0; i+1 < pos_eq_idx; ++i) {
				// Get free variables at i
				const trefs& fv1 = get_free_vars<node>(conjs[i]);
				if (fv1.empty()) continue;
				for (size_t j = i+1; j < pos_eq_idx; ++j) {
					// Get free variables at i + 1
					const trefs& fv2 = get_free_vars<node>(conjs[j]);
					if (fv2.empty()) continue;
					// Squeeze overlapping terms
					if (uf.connected(fv1[0], fv2[0])) {
						conjs[i] = squeeze<node>(conjs[i], conjs[j]);
						conjs.erase(conjs.begin()+j);
						--j;
						--pos_eq_idx;
						--eq_idx;
					}
				}
			}
			// Apply assumptions to equations while updating them on = 0 equations
			uf.clear();
			trefs additions;
			for (size_t i = 0; i < pos_eq_idx; ++i)
				conjs[i] = apply_assms<node>(conjs[i], assms, uf, additions);
			update_assms<node>(assms, uf, additions);
			// Apply updated assumptions to != 0 equations
			for (size_t i = pos_eq_idx; i < eq_idx; ++i)
				conjs[i] = apply_assms<node>(conjs[i], assms);
			// Build result node
			n = conjs[0];
			for (size_t i = 1; i < eq_idx; ++i)
				n = tau::build_wff_and(n, conjs[i]);
			// Exclude equations from being visited again
			mark.insert(n);
			auto cr = tau::get(n).children();
			mark.insert(cr.begin(), cr.end());
			for (size_t i = eq_idx; i < conjs.size(); ++i)
				n = tau::build_wff_and(n, conjs[i]);
			return n;
		// Try getting disjunctions
		} else if (trefs disjs = get_dnf_wff_clauses<node>(n); disjs.size() > 1) {
			// Sort equations in disjs up front and prepare union find
			// to compute variable overlap
			size_t eq_idx = 0;
			auto uf = union_find_with_sets<decltype(uf_comp), node>(uf_comp);
			for (tref& disj : disjs) {
				const tau& disj_t = tau::get(disj);
				if (disj_t.child_is(tau::bf_eq)) {
					// Only treat = 0 equations
					if (!disj_t[0][1].equals_0()) continue;
					std::swap(disj, disjs[eq_idx++]);
				} else if (disj_t.child_is(tau::bf_neq)) {
					// Only treat != 0 equations
					if (!disj_t[0][1].equals_0()) continue;
					// merge variables of != 0 equations
					const trefs& fv = get_free_vars<node>(disj);
					for (tref v : fv) uf.merge(fv[0], v);
					std::swap(disj, disjs[eq_idx++]);
				}
			}
			// Return early if no equation present
			if (eq_idx == 0) return n;
#ifdef DEBUG
			// Print sorted conjuncts
			LOG_TRACE << "Disjuncts with eqs first: ";
			for (tref el : disjs) LOG_TRACE << tau::get(el) << ", ";
#endif
			// Apply assumptions to equations
			for (size_t i = 0; i < eq_idx; ++i)
				disjs[i] = apply_assms<node>(disjs[i], assms);
			// Dual squeeze != 0 equations that share a variable
			// First sort != 0 up front
			size_t neg_eq_idx = 0;
			for (size_t i = 0; i < eq_idx; ++i) {
				if (tau::get(disjs[i]).child_is(tau::bf_neq))
					std::swap(disjs[i], disjs[neg_eq_idx++]);
			}
			for (size_t i = 0; i+1 < neg_eq_idx; ++i) {
				// Get free variables at i
				const trefs& fv1 = get_free_vars<node>(disjs[i]);
				if (fv1.empty()) continue;
				for (size_t j = i+1; j < neg_eq_idx; ++j) {
					// Get free variables at i + 1
					const trefs& fv2 = get_free_vars<node>(disjs[j]);
					if (fv2.empty()) continue;
					// Squeeze overlapping terms
					if (uf.connected(fv1[0], fv2[0])) {
						disjs[i] = squeeze<node>(disjs[i], disjs[j]);
						disjs.erase(disjs.begin()+j);
						--j;
						--neg_eq_idx;
						--eq_idx;
					}
				}
			}
			// Apply dual assumptions to equations while updating them on != 0 equations
			uf.clear();
			trefs additions;
			for (size_t i = 0; i < neg_eq_idx; ++i)
				disjs[i] = apply_assms<node>(disjs[i],
					dual_assms, uf, additions, true);
			update_assms<node>(dual_assms, uf, additions);
			// Apply updated assumptions to = 0 equations
			for (size_t i = neg_eq_idx; i < eq_idx; ++i)
				disjs[i] = apply_assms<node>(disjs[i], dual_assms, true);
			// Build result node
			n = disjs[0];
			for (size_t i = 1; i < eq_idx; ++i)
				n = tau::build_wff_or(n, disjs[i]);
			// Exclude equations from being visited again
			mark.insert(n);
			auto cr = tau::get(n).children();
			mark.insert(cr.begin(), cr.end());
			for (size_t i = eq_idx; i < disjs.size(); ++i)
				n = tau::build_wff_or(n, disjs[i]);
			return n;
		} else {
			// No disjuncts or conjuncts present
			// Simply apply dual assumptions and assumptions
			return n;
		}
	};
	auto up = [&](tref n, tref parent) {
		if (!is<node>(n, tau::wff)) return n;
		if (parent != nullptr && is<node>(parent, tau::wff_or)) {
			if (!is_child<node>(n, tau::wff_or)) {
				assms.pop_back();
			}
		} else if (parent != nullptr && is<node>(parent, tau::wff_and)) {
			if (!is_child<node>(n, tau::wff_and)) {
				dual_assms.pop_back();
			}
		}
		return n;
	};
	auto visit = [&mark](tref n) {
		if (is_quantifier<node>(n)) return false;
		if (is_temporal_quantifier<node>(n)) return false;
		if (mark.contains(n)) return false;
		return visit_wff<node>(n);
	};
	// Disable intermediate simplifications for the moment
	tau::use_hooks = false;
	tref res = pre_order<node>(formula).apply(f, visit, up);
	DBG(assert(assms.size() == 1);)
	DBG(assert(dual_assms.size() == 1);)
	// Re-enable intermediate simplifications
	tau::use_hooks = true;
	DBG(LOG_DEBUG << "Ended squeeze_absorb");
	return tau::reget(res);
}

/**
 * @brief The procedure collects all = 0 and != 0 equations within scope
 * (conjunction and disjunction respectively) and squeezes
 * those together that share var. Each squeezed formula A is
 * then integrated into terms in scope that share at least 2 variables including
 * var in the following way:
 * - if A is = 0 and given f = 0, we produce f|A = 0
 * - if A is = 0 and given f != 0, we produce f & A' != 0
 * - if A is != 0 and given f != 0, we produce f|A != 0
 * - if A is != 0 and given f = 0, we produce f & A' = 0
 * @tparam node Tree node type
 * @param formula The formula to apply the procedure to
 * @param var The shared variable for squeeze/absorb
 * @return The mutated formula
*/
template<NodeType node>
tref squeeze_absorb(tref formula, tref var) {
	using tau = tree<node>;
	DBG(LOG_DEBUG << "Started squeeze_absorb on var:" << tau::get(var));
	trefs assms {tau::_0(find_ba_type<node>(var))};
	trefs dual_assms {tau::_0(find_ba_type<node>(var))};
	subtree_unordered_set<node> mark;
	auto f = [&](tref n, tref parent) {
		if (!tau::get(n).is(tau::wff)) return n;
		const tau& cn = tau::get(n)[0];
		if (parent != nullptr && is<node>(parent, tau::wff_or)) {
			if (!cn.is(tau::wff_or)) {
				// Push new assumption to stack
				assms.push_back(assms.back());
			}
		} else if (parent != nullptr && is<node>(parent, tau::wff_and)) {
			if (!cn.is(tau::wff_and)) {
				// Push new assumption to stack
				dual_assms.push_back(dual_assms.back());
			}
		}
		// Try getting conjunctions
		if (trefs conjs = get_cnf_wff_clauses<node>(n); conjs.size() > 1) {
			// Sort the equations in conjs up front
			size_t eq_idx = 0;
			for (tref& conj : conjs) {
				const tau& conj_t = tau::get(conj);
				if (conj_t.child_is(tau::bf_neq) ||
					conj_t.child_is(tau::bf_eq)) {
					// Only treat (!)= 0 equations
					if (!conj_t[0][1].equals_0()) continue;
					std::swap(conj, conjs[eq_idx++]);
				}
			}
			// Return early if no equation present
			if (eq_idx == 0) return n;
#ifdef DEBUG
			// Print sorted conjuncts
			LOG_TRACE << "Conjuncts with eqs first: ";
			for (tref el : conjs) LOG_TRACE << tau::get(el) << ", ";
#endif
			// Apply dual assumptions to equations
			for (size_t i = 0; i < eq_idx; ++i)
				conjs[i] = apply_assms<node>(conjs[i],
					dual_assms.back(), var, true);
			// First sort = 0 up front
			size_t pos_eq_idx = 0;
			for (size_t i = 0; i < eq_idx; ++i) {
				if (tau::get(conjs[i]).child_is(tau::bf_eq))
					std::swap(conjs[i], conjs[pos_eq_idx++]);
			}
			// Squeeze = 0 equations that contain var
			for (size_t i = 0; i+1 < pos_eq_idx; ++i) {
				// Get free variables at i
				const trefs& fv1 = get_free_vars<node>(conjs[i]);
				if (fv1.empty()) continue;
				if (!std::binary_search(fv1.begin(),
					fv1.end(), var, tau::subtree_less))
					continue;
				for (size_t j = i+1; j < pos_eq_idx; ++j) {
					// Get free variables at i + 1
					const trefs& fv2 = get_free_vars<node>(conjs[j]);
					if (fv2.empty()) continue;
					if (!std::binary_search(fv2.begin(),
						fv2.end(), var, tau::subtree_less))
						continue;
					// Squeeze overlapping terms
					conjs[i] = squeeze<node>(conjs[i], conjs[j]);
					conjs.erase(conjs.begin()+j);
					--j;
					--pos_eq_idx;
					--eq_idx;
				}
			}
			// Apply assumptions to equations while updating them on = 0 equations
			trefs updates;
			for (size_t i = 0; i < pos_eq_idx; ++i)
				conjs[i] = apply_assms<node>(conjs[i],
					assms.back(), var, updates);
			for (tref upd : updates) {
				assms.back() = tau::build_bf_or(
					assms.back(), tau::trim2(upd));
			}
			// Apply updated assumptions to != 0 equations
			for (size_t i = pos_eq_idx; i < eq_idx; ++i)
				conjs[i] = apply_assms<node>(conjs[i],
					assms.back(), var);
			// Build result node
			n = conjs[0];
			for (size_t i = 1; i < eq_idx; ++i)
				n = tau::build_wff_and(n, conjs[i]);
			// Exclude equations from being visited again
			mark.insert(n);
			auto cr = tau::get(n).children();
			mark.insert(cr.begin(), cr.end());
			for (size_t i = eq_idx; i < conjs.size(); ++i)
				n = tau::build_wff_and(n, conjs[i]);
			return n;
		// Try getting disjunctions
		} else if (trefs disjs = get_dnf_wff_clauses<node>(n); disjs.size() > 1) {
			// Sort equations in disjs up front
			size_t eq_idx = 0;
			for (tref& disj : disjs) {
				const tau& disj_t = tau::get(disj);
				if (disj_t.child_is(tau::bf_eq) ||
					disj_t.child_is(tau::bf_neq)) {
					if (!disj_t[0][1].equals_0()) continue;
					std::swap(disj, disjs[eq_idx++]);
				}
			}
			// Return early if no equation present
			if (eq_idx == 0) return n;
#ifdef DEBUG
			// Print sorted conjuncts
			LOG_TRACE << "Disjuncts with eqs first: ";
			for (tref el : disjs) LOG_TRACE << tau::get(el) << ", ";
#endif
			// Apply assumptions to equations
			for (size_t i = 0; i < eq_idx; ++i)
				disjs[i] = apply_assms<node>(disjs[i],
					assms.back(), var);
			// First sort != 0 up front
			size_t neg_eq_idx = 0;
			for (size_t i = 0; i < eq_idx; ++i) {
				if (tau::get(disjs[i]).child_is(tau::bf_neq))
					std::swap(disjs[i], disjs[neg_eq_idx++]);
			}
			// Dual squeeze != 0 equations that contain var
			for (size_t i = 0; i+1 < neg_eq_idx; ++i) {
				// Get free variables at i
				const trefs& fv1 = get_free_vars<node>(disjs[i]);
				if (fv1.empty()) continue;
				if (!std::binary_search(fv1.begin(),
					fv1.end(), var, tau::subtree_less))
					continue;
				for (size_t j = i+1; j < neg_eq_idx; ++j) {
					// Get free variables at j
					const trefs& fv2 = get_free_vars<node>(disjs[j]);
					if (fv2.empty()) continue;
					if (!std::binary_search(fv2.begin(),
					fv2.end(), var, tau::subtree_less))
						continue;
					// Squeeze overlapping terms
					disjs[i] = squeeze<node>(disjs[i], disjs[j]);
					disjs.erase(disjs.begin()+j);
					--j;
					--neg_eq_idx;
					--eq_idx;
				}
			}
			// Apply dual assumptions to equations while updating them on != 0 equations
			trefs updates;
			for (size_t i = 0; i < neg_eq_idx; ++i)
				disjs[i] = apply_assms<node>(disjs[i],
					dual_assms.back(), var, updates, true);
			for (tref upd : updates) {
				dual_assms.back() = tau::build_bf_or(
					dual_assms.back(), tau::trim2(upd));
			}
			// Apply updated assumptions to = 0 equations
			for (size_t i = neg_eq_idx; i < eq_idx; ++i)
				disjs[i] = apply_assms<node>(disjs[i],
					dual_assms.back(), var, true);
			// Build result node
			n = disjs[0];
			for (size_t i = 1; i < eq_idx; ++i)
				n = tau::build_wff_or(n, disjs[i]);
			// Exclude equations from being visited again
			mark.insert(n);
			auto cr = tau::get(n).children();
			mark.insert(cr.begin(), cr.end());
			for (size_t i = eq_idx; i < disjs.size(); ++i)
				n = tau::build_wff_or(n, disjs[i]);
			return n;
		} else {
			// No disjuncts or conjuncts present
			// Simply apply dual assumptions and assumptions
			return n;
		}
	};
	auto up = [&](tref n, tref parent) {
		if (!is<node>(n, tau::wff)) return n;
		if (parent != nullptr && is<node>(parent, tau::wff_or)) {
			if (!is_child<node>(n, tau::wff_or)) {
				assms.pop_back();
			}
		} else if (parent != nullptr && is<node>(parent, tau::wff_and)) {
			if (!is_child<node>(n, tau::wff_and)) {
				dual_assms.pop_back();
			}
		}
		return n;
	};
	auto visit = [&mark](tref n) {
		if (is_quantifier<node>(n)) return false;
		if (is_temporal_quantifier<node>(n)) return false;
		if (mark.contains(n)) return false;
		return visit_wff<node>(n);
	};
	// Disable intermediate simplifications for the moment
	tau::use_hooks = false;
	tref res = pre_order<node>(formula).apply(f, visit, up);
	DBG(assert(assms.size() == 1);)
	DBG(assert(dual_assms.size() == 1);)
	// Re-enable intermediate simplifications
	tau::use_hooks = true;
	DBG(LOG_DEBUG << "Ended squeeze_absorb");
	return tau::reget(res);
}

/**
 * @brief Do a single Boole decomposition step on term given the provided variable.
 * Assumes that the decomposition is valid.
 * @tparam node Tree node type
 * @param term The term on which to apply Boole decomposition step
 * @param var The variable on which to do Boole decomposition
 * @return The resulting term
 */
template<NodeType node>
tref term_boole_decomposition(tref term, tref var) {
	using tau = tree<node>;
	DBG(assert(tau::get(var).is(tau::variable));)
	if (tau::get(term).find_top(is_non_boolean_term<node>)) {
		DBG(LOG_TRACE << "term_boole_decomposition/Non boolean term: "
			<< tau::get(term) << "\n");
		return term;
	}
	var = tau::get(tau::bf, var);
	tref p1 = tau::get(term).replace(var, tau::_1(find_ba_type<node>(var)));
	// Ensure early detection of F
	p1 = syntactic_path_simplification<node>::unsat_on_unchanged_negations(p1);
	tref p2 = tau::get(term).replace(var, tau::_0(find_ba_type<node>(var)));
	// Ensure early detection of F
	p2 = syntactic_path_simplification<node>::unsat_on_unchanged_negations(p2);
	if (tau::get(p1) == tau::get(p2)) return p1;
	// Build Boole decomposition step
	return tau::build_bf_or(
		tau::build_bf_and(var, p1), tau::build_bf_and(tau::build_bf_neg(var), p2)
	);
}

// Note: Recursion depth is bound by the number of variables, which should
// prevent a stack overflow due to tree size in all use cases
/**
 * @brief Recursively do Boole decomposition on term using the provided variables
 * starting at idx.
 * Assumes that the decomposition is valid for all provided variables.
 * @tparam node Tree node type
 * @param term Term on which to do Boole decomposition
 * @param vars The variables to do Boole decomposition on
 * @param idx The current variable index
 * @return The resulting Boole decomposition
 */
template<NodeType node>
tref rec_term_boole_decomposition(tref term, const trefs& vars, const int_t idx) {
	using tau = tree<node>;
	DBG(LOG_TRACE << "Step on " << LOG_FM(term) << "\n";)
	if (tau::get(term).equals_0()) return term;
	if (tau::get(term).equals_1()) return term;
	if (idx == (int_t)vars.size()) {
		term = normalize_ba<node>(term);
		DBG(LOG_TRACE << "Result: " << LOG_FM(term) << "\n";)
		return term;
	}
	DBG(assert(tau::get(vars[idx]).is(tau::variable));)
	tref p1 = tau::get(term).replace(vars[idx], tau::_1_trimmed(find_ba_type<node>(vars[idx])));
	// Ensure early detection of F
	p1 = syntactic_path_simplification<node>::unsat_on_unchanged_negations(p1);
	tref p2 = tau::get(term).replace(vars[idx], tau::_0_trimmed(find_ba_type<node>(vars[idx])));
	// Ensure early detection of F
	p2 = syntactic_path_simplification<node>::unsat_on_unchanged_negations(p2);
	if (tau::get(p1) == tau::get(p2)) {
		DBG(LOG_TRACE << "Result: " << LOG_FM(p1) << "\n";)
		return rec_term_boole_decomposition<node>(p1, vars, idx + 1);
	}
	p1 = rec_term_boole_decomposition<node>(p1, vars, idx + 1);
	p2 = rec_term_boole_decomposition<node>(p2, vars, idx + 1);
	tref var = tau::get(tau::bf, vars[idx]);
	// Build Boole decomposition
	if (tau::get(p1).equals_1())
		term = tau::build_bf_or(var,  p2);
	else if (tau::get(p2).equals_1())
		term = tau::build_bf_or(p1, tau::build_bf_neg(var));
	else term = tau::build_bf_or(tau::build_bf_and(var, p1),
		tau::build_bf_and(tau::build_bf_neg(var), p2));
	DBG(LOG_TRACE << "Result: " << LOG_FM(term) << "\n";)
	return term;
}

/**
 * @brief Convert term to Boole normal form. Also treats normalization of
 * encountered tau constants.
 * @tparam node Tree node type
 * @param term The term to do the Boole decomposition on
 * @return The resulting Boole decomposition
 */
template<NodeType node>
tref term_boole_decomposition(tref term) {
	using tau = tree<node>;
	DBG(LOG_DEBUG << "Term_boole_decomposition on " << LOG_FM(term) << "\n";)
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, tref>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(term); it != cache.end())
		return it->second;
#endif // TAU_CACHE
	if (tau::get(term).find_top(is_non_boolean_term<node>)) {
		DBG(LOG_TRACE << "term_boole_decomposition/Non boolean term: "
			<< tau::get(term) << "\n");
		return term;
	}
	// Simple cases
	if (tau::get(term).equals_0() || tau::get(term).equals_1())
		return term;
	tref bd = push_negation_in<node, false>(term);
	auto vars = get_free_vars_appearance_order<node>(bd);
	// No free var, so no boole decomposition step
	if (vars.empty()) {
#ifdef TAU_CACHE
		cache.emplace(bd, bd);
		return cache.emplace(term, bd).first->second;
#endif // TAU_CACHE
		return normalize_ba<node>(bd);
	}
	std::ranges::stable_sort(vars, variable_order_for_simplification<node>);
	bd = rec_term_boole_decomposition<node>(bd, vars, 0);
	DBG(LOG_DEBUG << "Term_boole_decomposition result: " << LOG_FM(term) << "\n";)
#ifdef TAU_CACHE
	cache.emplace(bd, bd);
	return cache.emplace(term, bd).first->second;
#endif // TAU_CACHE
	return bd;
}

// Note: Recursion depth is bound by the number of variables, which should
// prevent a stack overflow due to tree size in all use cases
 /**
 * @brief Recursively do Boole decomposition on formula using the provided variables
 * starting at idx.
 * Assumes that the decomposition is valid for all provided variables.
 * @tparam node Tree node type
 * @param formula The formula to do Boole decomposition on
 * @param vars The variable to perform the Boole decomposition on
 * @param idx The current variable index
 * @return The resulting Boole decomposition
 */
template<NodeType node>
tref rec_boole_decomposition(tref formula, const trefs& vars, const int_t idx) {
	using tau = tree<node>;
	DBG(LOG_TRACE << "Step on " << LOG_FM(formula) << "\n";)
	if (tau::get(formula).equals_F()) return formula;
	if (tau::get(formula).equals_T()) return formula;
	if (idx == (int_t)vars.size()) {
		DBG(LOG_TRACE << "Result: " << LOG_FM(formula) << "\n";)
		return formula;
	}
	DBG(assert(tau::get(vars[idx]).child_is(tau::bf_eq));)
	tref p1 = tau::get(formula).replace(vars[idx], tau::_T());
	// Ensure early detection of F
	p1 = syntactic_path_simplification<node>::unsat_on_unchanged_negations(p1);
	tref p2 = tau::get(formula).replace(vars[idx], tau::_F());
	// Ensure early detection of F
	p2 = syntactic_path_simplification<node>::unsat_on_unchanged_negations(p2);
	if (tau::get(p1) == tau::get(p2)) {
		DBG(LOG_TRACE << "Result: " << LOG_FM(p1) << "\n";)
		return rec_boole_decomposition<node>(p1, vars, idx + 1);
	}
	p1 = rec_boole_decomposition<node>(p1, vars, idx + 1);
	p2 = rec_boole_decomposition<node>(p2, vars, idx + 1);
	// Build Boole decomposition
	if (tau::get(p1).equals_T())
		formula = tau::build_wff_or(vars[idx], p2);
	else if (tau::get(p2).equals_T())
		formula = tau::build_wff_or(p1, tau::build_wff_neg(vars[idx]));
	else formula = tau::build_wff_or(tau::build_wff_and(vars[idx], p1),
		tau::build_wff_and(tau::build_wff_neg(vars[idx]), p2));
	DBG(LOG_TRACE << "Result: " << LOG_FM(formula) << "\n";)
	return formula;
}

/**
 * This procedure converts the formula to Boole normal form. It also converts all
 * terms to Boole normal form.
 * @tparam node Tree node type
 * @param bnf The formula to convert to Boole normal form
 * @return The resulting Boole normal form
 */
template<NodeType node>
tref boole_normal_form(tref formula) {
	using tau = tree<node>;
	DBG(LOG_DEBUG << "Boole_normal_form on " << LOG_FM(formula) << "\n";)
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, tref>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(formula); it != cache.end())
		return it->second;
#endif // TAU_CACHE
	if (tau::get(formula).equals_T() || tau::get(formula).equals_F())
		return formula;
	// Step 1: Syntactically simplify formula
	tref bnf = syntactic_formula_simplification<node>(formula);
	DBG(LOG_DEBUG << "After syntactic_formula_simplification: " << LOG_FM(formula) << "\n";)
	// Squeeze and absorb for additional simplifications during term normalization
	// -> causes mayor blow ups
	bnf = squeeze_absorb<node>(bnf);
	// Step 2: Traverse formula, simplify all encountered equations
	auto simp_eqs = [](tref n) {
		if (is_atomic_fm<node>(n) && is_bv_type_family<node>(find_ba_type<node>(n))) {
			return simplify_bv<node>(n);
		} else if (tau::get(n).child_is(tau::bf_eq)) {
			if (tau::get(n).equals_T() || tau::get(n).equals_F())
				return n;
			tref c1 = tau::get(n)[0].first();
			tref c2 = tau::get(n)[0].second();
			// Apply Boole decomposition
			c1 = term_boole_decomposition<node>(c1);
			c2 = term_boole_decomposition<node>(c2);
			return tau::build_bf_eq(c1, c2);
		} else if (tau::get(n).child_is(tau::bf_neq)) {
			if (tau::get(n).equals_T() || tau::get(n).equals_F())
				return n;
			tref c1 = tau::get(n)[0].first();
			tref c2 = tau::get(n)[0].second();
			// Apply Boole decomposition
			c1 = term_boole_decomposition<node>(c1);
			c2 = term_boole_decomposition<node>(c2);
			return tau::build_bf_neq(c1, c2);
		}
		return n;
	};
	bnf = pre_order<node>(bnf).apply_unique_until_change(simp_eqs, visit_wff<node>);
	DBG(LOG_DEBUG << "After term_boole_decomposition: " << LOG_FM(formula) << "\n";)
	// Step 3: Syntactically simplify resulting formula again after normalization of terms
	bnf = syntactic_formula_simplification<node>(bnf);
	DBG(LOG_DEBUG << "After syntactic_formula_simplification: " << LOG_FM(formula) << "\n";)
	// Step 4: Convert formula to Boole normal form
	// First get atomic formulas without !=
	tref eq_bnf = unequal_to_not_equal<node>(bnf);
	trefs atms = rewriter::select_top_until<node>(eq_bnf,
		is_child<node, tau::bf_eq>, is_quantifier<node>);
	// No variables for Boole decomposition
	if (atms.empty()) {
#ifdef TAU_CACHE
		cache.emplace(bnf, bnf);
		return cache.emplace(formula, bnf).first->second;
#endif // TAU_CACHE
		return bnf;
	}
	// Sort the BDD variables
	std::ranges::stable_sort(atms, atm_formula_order_for_simplification<node>);
	// Apply Boole decomposition
	eq_bnf = rec_boole_decomposition<node>(eq_bnf, atms, 0);
	// Convert !(=) to != again
	eq_bnf = not_equal_to_unequal<node>(eq_bnf);
	eq_bnf = simplify_using_equality<node>::on(eq_bnf);
	DBG(LOG_DEBUG << "Boole_normal_form result: " << LOG_FM(eq_bnf) << "\n";)
#ifdef TAU_CACHE
	cache.emplace(eq_bnf, eq_bnf);
	return cache.emplace(formula, eq_bnf).first->second;
#endif // TAU_CACHE
	return eq_bnf;
}

template<NodeType node>
tref term_boole_normal_form(tref formula) {
	using tau = tree<node>;
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, tref>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(formula); it != cache.end())
		return it->second;
#endif // TAU_CACHE
	if (tau::get(formula).equals_T() || tau::get(formula).equals_F())
		return formula;
	// Step 1: Syntactically simplify formula
	tref tbnf = syntactic_formula_simplification<node>(formula);
	DBG(LOG_DEBUG << "After syntactic_formula_simplification: " << LOG_FM(formula) << "\n";)
	auto simp_eqs = [](tref n) {
		if (is_atomic_fm<node>(n) && is_bv_type_family<node>(find_ba_type<node>(n))) {
			return simplify_bv<node>(n);
		} else if (tau::get(n).child_is(tau::bf_eq)) {
			if (tau::get(n).equals_T() || tau::get(n).equals_F())
				return n;
			tref c1 = tau::get(n)[0].first();
			tref c2 = tau::get(n)[0].second();
			// Apply Boole decomposition
			c1 = term_boole_decomposition<node>(c1);
			c2 = term_boole_decomposition<node>(c2);
			return tau::build_bf_eq(c1, c2);
		} else if (tau::get(n).child_is(tau::bf_neq)) {
			if (tau::get(n).equals_T() || tau::get(n).equals_F())
				return n;
			tref c1 = tau::get(n)[0].first();
			tref c2 = tau::get(n)[0].second();
			// Apply Boole decomposition
			c1 = term_boole_decomposition<node>(c1);
			c2 = term_boole_decomposition<node>(c2);
			return tau::build_bf_neq(c1, c2);
		}
		return n;
	};
	tbnf = pre_order<node>(tbnf).apply_unique_until_change(simp_eqs, visit_wff<node>);
	DBG(LOG_DEBUG << "After term_boole_decomposition: " << LOG_FM(tbnf) << "\n";)
	// Step 3: Syntactically simplify resulting formula again after normalization of terms
	tbnf = syntactic_formula_simplification<node>(tbnf);
	DBG(LOG_DEBUG << "After syntactic_formula_simplification: " << LOG_FM(tbnf) << "\n";)
#ifdef TAU_CACHE
	cache.emplace(tbnf, tbnf);
	return cache.emplace(formula, tbnf).first->second;
#endif // TAU_CACHE
	return tbnf;
}

/**
 * @brief Performs a Boole decomposition step on the formula with the goal to push
 * the existential quantifier further in. Since the BDD variables are atomic formulas,
 * the procedure investigates the best Boole decomposition in each situation.
 * @tparam node Tree node type
 * @param ex_quant_fm Existentially quantified formula on which to perform Boole decomposition step
 * @param pool The pool of variables that can be used for the Boole decomposition
 * @param quant_pattern Map from quantified variables to their priority used for BDD variable ordering
 * @param excluded Collection of nodes that are not visited by anti prenex algorithm
 * @param no_atms Indicates if there are atomic formulas left to do Boole decomposition on
 * @return The resulting Boole decomposition with the existential quantifier pushed further in
 */
template<NodeType node>
tref ex_quantified_boole_decomposition(tref ex_quant_fm, auto& pool,
	auto& quant_pattern, subtree_set<node>* excluded, bool& no_atms) {
	using tau = tree<node>;
	DBG(assert(!tau::get(ex_quant_fm).find_top(is<node, tau::bf_neq>)));

	// Get atomic formulas from pool
	auto it = pool.find(ex_quant_fm);
	tref curr_pool = it != pool.end()
				 ? it->second
				 : tau::get(ex_quant_fm)[0].second();
	trefs atms = rewriter::select_top_until<node>(curr_pool,
		is_child<node, tau::bf_eq>, is_quantifier<node>);
	if (atms.empty()) {
		// std::cout << "ex_quant_fm: " << tau::get(ex_quant_fm) << "\n";
		no_atms = true;
		if (excluded) excluded->insert(ex_quant_fm);
		return ex_quant_fm;
	}
	// Sort the atomic formulas and get first
	tref atm = *std::ranges::min_element(atms,
		atm_formula_order_for_quant_elim<node>(quant_pattern));
	// Get quantified variable
	tref var = tau::get(tau::bf, tau::trim2(ex_quant_fm));
	// Try syntactic simplifications
	{
	tref func = tau::trim2(norm_equation<node>(atm));
	func = apply_xor_def<node>(func);
	// We use is_boolean_operation to enable the procedure on non-boolean functions
	tref func_v_0 = rewriter::replace_if<node>(func, var, tau::_0(find_ba_type<node>(var)), is_boolean_operation<node>);
	func_v_0 = syntactic_path_simplification<node>::on(func_v_0);
	tref func_v_1 = rewriter::replace_if<node>(func, var, tau::_1(find_ba_type<node>(var)), is_boolean_operation<node>);
	func_v_1 = syntactic_path_simplification<node>::on(func_v_1);
	// Check identically zero
	if (tau::get(func_v_0).equals_0() && tau::get(func_v_1).equals_0()) {
		return rewriter::replace<node>(ex_quant_fm, atm, tau::_T());
	}
	// Check identically one
	else if (tau::get(func_v_0).equals_1() && tau::get(func_v_1).equals_1()) {
		return rewriter::replace<node>(ex_quant_fm, atm, tau::_F());
	}
	// Check does not dependent on var
	else if (tau::get(func_v_0) == tau::get(func_v_1) && !contains<node>(func_v_0, var)) {
		tref fm = tau::get(ex_quant_fm)[0].second();
		tref l = rewriter::replace<node>(fm, atm, tau::_T());
		l = syntactic_path_simplification<node>::unsat_on_unchanged_negations(l);
		tref r = rewriter::replace<node>(fm, atm, tau::_F());
		r = syntactic_path_simplification<node>::unsat_on_unchanged_negations(r);
		if (tau::get(l) == tau::get(r)) return tau::build_wff_ex(tau::trim(var), l, false);
		atm = rewriter::replace<node>(atm, var, tau::_T());
		return tau::build_wff_or(
			tau::build_wff_and(atm, tau::build_wff_ex(tau::trim(var), l, false)),
			tau::build_wff_and(tau::build_wff_neg(atm), tau::build_wff_ex(tau::trim(var), r, false))
			);
	}
	// Check has a unique zero
	func_v_1 = push_negation_in<node, false>(tau::build_bf_neg(func_v_1));
	if (tau::get(func_v_0) == tau::get(func_v_1) && !contains<node>(func_v_0, var)) {
		tref fm = tau::get(ex_quant_fm)[0].second();
		tref l = rewriter::replace<node>(fm, atm, tau::_T());
		l = rewriter::replace<node>(l, var, func_v_0);
		l = syntactic_path_simplification<node>::unsat_on_unchanged_negations(l);
		tref r = rewriter::replace<node>(fm, atm, tau::_F());
		r = syntactic_path_simplification<node>::unsat_on_unchanged_negations(r);
		if (tau::get(l) == tau::get(r)) {
			if (excluded) excluded->insert(l);
			return l;
		}
		tref boole_atm = tau::build_bf_eq(
		term_boole_decomposition<node>(tau::get(atm)[0].first(), tau::trim(var)),
		term_boole_decomposition<node>(tau::get(atm)[0].second(), tau::trim(var))
		);
		tref nr = tau::build_wff_ex(tau::trim(var),
					    tau::build_wff_and(tau::build_wff_neg(boole_atm), r), false);
		pool.insert_or_assign(nr,
			rewriter::replace<node>(curr_pool, atm, tau::_F()));
		atm = rewriter::replace<node>(atm, var, func_v_0);
		atm = tau::build_wff_and(atm, l);
		if (excluded) excluded->insert(atm);
		return tau::build_wff_or(atm, nr);
	}
	}
	// No simplification applied, build Boole decomposition
	tref fm = tau::get(ex_quant_fm)[0].second();
	tref l = rewriter::replace<node>(fm, atm, tau::_T());
	l = syntactic_path_simplification<node>::unsat_on_unchanged_negations(l);
	tref r = rewriter::replace<node>(fm, atm, tau::_F());
	r = syntactic_path_simplification<node>::unsat_on_unchanged_negations(r);
	if (tau::get(l) == tau::get(r)) return tau::build_wff_ex(tau::trim(var), l, false);
	tref boole_atm = tau::build_bf_eq(
		term_boole_decomposition<node>(tau::get(atm)[0].first(), tau::trim(var)),
		term_boole_decomposition<node>(tau::get(atm)[0].second(), tau::trim(var))
		);
	tref nl = tau::build_wff_ex(tau::trim(var),
				    tau::build_wff_and(boole_atm, l), false);
	tref nr = tau::build_wff_ex(tau::trim(var),
				    tau::build_wff_and(tau::build_wff_neg(boole_atm), r), false);
	// Update available pool for further BDD variables
	pool.insert_or_assign(nl,
		rewriter::replace<node>(curr_pool, atm, tau::_T()));
	pool.insert_or_assign(nr,
		rewriter::replace<node>(curr_pool, atm, tau::_F()));
	return tau::build_wff_or(nl, nr);
}

// TODO: How to adjust for bitvector that are boolean?
/**
 * @brief Eliminate the existential quantifier scoping a clause.
 * @tparam node Tree node type
 * @param ex_clause Existentially quantified clause
 * @param quant_eliminated Indicates whether the quantifier was successfully removed
 * @return The resulting clause after removing the existential quantifier
 */
template <NodeType node>
tref treat_ex_quantified_clause(tref ex_clause, bool& quant_eliminated) {
	using tau = tree<node>;
	// Following Corollary 2.3 from Taba book from Ohad
	tref var = tau::trim2(ex_clause);
	tref formula = tau::get(ex_clause)[0].second();
	if (tau::get(formula).equals_T() || tau::get(formula).equals_F())
		return formula;
	tref new_fm = tau::_T();
	bool is_quant_removable_in_clause = true;
	trefs conjs = get_cnf_wff_clauses<node>(formula);
	for (tref& conj : conjs) {
		if (!contains<node>(conj, var)) {
			new_fm = tau::build_wff_and(new_fm, conj);
			conj = tau::_T();
			continue;
		}
		// Check that conjunct is not an unresolved reference
		if (tau::get(conj).child_is(tau::wff_ref)) {
			// If the reference contains the quantified variable at this point
			// we cannot resolve the quantifier in this clause
			is_quant_removable_in_clause = false;
		}
	}
	tref scoped_fm = tau::build_wff_and(conjs);
	if (!is_quant_removable_in_clause) {
		// Since we cannot remove the quantifier in this
		// clause it needs to be maintained
		quant_eliminated = false;
		return tau::build_wff_and(
			tau::build_wff_ex(var, scoped_fm, false), new_fm);
	}
	// Check that quantified variable appears
	if (tau::get(scoped_fm).equals_T()) return new_fm;

	// Check if quantified variable is bitvector
	if (is_bv_type_family<node>(tau::get(var).get_ba_type())) {
		if (const trefs& free_vars = get_free_vars<node>(scoped_fm);
			free_vars.size() == 1 &&
			tau::get(free_vars[0]) == tau::get(var)) {
				// By assumption quantifier is pushed in all the way
				// Closed bv formula, simplify to T/F
				if (is_bv_formula_sat<node>(tau::build_wff_ex(var, scoped_fm, false)))
					return new_fm;
				else return tau::_F();
			} else {
				// Quantifier is not resolvable
				quant_eliminated = false;
				return tau::build_wff_and(
					tau::build_wff_ex(var, scoped_fm, false), new_fm);
			}
	}
	// Continue with quantifier elimination for atomless BA
	size_t type_v = find_ba_type<node>(var);
	tref f = squeeze_positives<node>(scoped_fm, type_v);
	tref f_0 = f ? rewriter::replace<node>(f, var, tau::_0_trimmed(type_v)) : tau::_0(type_v);
	// std::cout << "f_0: " << tau::get(f_0) << "\n";
	tref f_1 = f ? rewriter::replace<node>(f, var, tau::_1_trimmed(type_v)) : tau::_0(type_v);
	// std::cout << "f_1: " << tau::get(f_1) << "\n";
	// TODO: instead of != use !(=)
	trefs neqs = tau::get(scoped_fm).select_top(is<node, tau::bf_neq>);
	if (neqs.size()) {
		tref nneqs = tau::_T();
		for (tref neq : neqs) {
			// Convert to != 0
			neq = norm_trimmed_equation<node>(neq);
			// Get term
			tref g = tau::trim(neq);
			tref g_0 = rewriter::replace<node>(g, var,
						tau::_0_trimmed(find_ba_type<node>(var)));
			// std::cout << "g_0: " << tau::get(g_0) << "\n";
			tref g_1 = rewriter::replace<node>(g, var,
						tau::_1_trimmed(find_ba_type<node>(var)));
			// std::cout << "g_1: " << tau::get(g_1) << "\n";
			// If both are 1 then inequality is implied by f_0f_1 = 0
			if (tau::get(g_0).equals_1() && tau::get(g_1).equals_1())
				continue;
			// If f_0 is equal to f_1 we can use assumption f_0 = 0 and f_1 = 0
			if (tau::get(f_0) == tau::get(f_1)) {
				nneqs = tau::build_wff_and(nneqs,
					tau::build_bf_neq_0(
						tau::build_bf_or(g_0,
								 g_1)));
			} else if (tau::get(g_0) == tau::get(g_1)) {
				nneqs = tau::build_wff_and(nneqs,
					tau::build_bf_neq_0(g_0));
			} else nneqs = tau::build_wff_and(
				nneqs,
				tau::build_bf_neq_0(
					tau::build_bf_or(
						tau::build_bf_and(
						tau::build_bf_neg(f_1),
						g_1),
						tau::build_bf_and(
						tau::build_bf_neg(f_0),
						g_0)))
					);
		}
		new_fm = tau::build_wff_and(new_fm, tau::build_wff_and(
			tau::build_bf_eq_0(tau::build_bf_and(f_0, f_1)),
			nneqs));
	} else if (f) {
		new_fm = tau::build_wff_and(new_fm,
			tau::build_bf_eq_0(
			tau::build_bf_and(f_0, f_1)));
	}
	return term_boole_normal_form<node>(new_fm);
}

/**
 * @brief The procedure pushes all quantifiers that are present in formula as far
 * in as possible.
 * @tparam node Tree node type
 * @param formula The formula to apply the procedure to
 * @return The resulting formula
 */
template<NodeType node>
tref anti_prenex(tref formula) {
	using tau = tree<node>;
	subtree_unordered_map<node, tref> atomic_pool;
	subtree_unordered_map<node, int_t> quant_pattern;
	int_t qid = 1;
	auto inner_quant = [&](tref n) {
		bool quant_eliminated = true;
		subtree_set<node> excluded;
		auto anti_prenex_step = [&](tref n) {
			while (tau::get(n).child_is(tau::wff_ex)) {
				// TODO: if all atomic formulas are !=

				// If n is single DNF clause -> treat quantifier
				if (!tau::get(n).find_top(is<node, tau::wff_or>)) {
					DBG(LOG_TRACE << "Before treat_ex_quantified_clause: " << LOG_FM(n) << "\n";)
					n = treat_ex_quantified_clause<node>(n, quant_eliminated);
					// std::cout << "excluded: " << tau::get(n) << "\n";
					excluded.insert(n);
					DBG(LOG_TRACE << "After treat_ex_quantified_clause: " << LOG_FM(n) << "\n";)
					return n;
				}
				// Try push quant down
				// std::cout << "push n: " << tau::get(n) << "\n";
				auto pushed = push_existential_quantifier_one<node>(n, &excluded);
				if (pushed != n) {
					// TODO: sort disjunctions to priorities
					DBG(LOG_TRACE << "Before push_existential_quantifier_one: " << LOG_FM(n) << "\n";)
					DBG(LOG_TRACE << "After push_existential_quantifier_one: " << LOG_FM(pushed) << "\n";)
					return pushed;
				}
				// Smart Boole decomposition
				n = unequal_to_not_equal<node>(n);
				DBG(LOG_TRACE << "Before ex_quantified_boole_decomposition: " << LOG_FM(n) << "\n";)
				bool no_atms = false;
				n = ex_quantified_boole_decomposition<node>(n,
					atomic_pool, quant_pattern, &excluded, no_atms);
				// Quantifier is pushed in as far as possible but cannot
				// be resolved yet
				if (no_atms) return n;
				n = not_equal_to_unequal<node>(n);
				DBG(LOG_TRACE << "After ex_quantified_boole_decomposition: " << LOG_FM(n) << "\n";)
			}
			return n;
		};
		auto step_visit = [&] (tref t) {
			if (excluded.contains(t)) return false;
			return visit_wff<node>(t);
		};
		if (!is_child_quantifier<node>(n)) return n;
		// Here child is quantifier
		DBG(LOG_TRACE << "Inner_quant on " << LOG_FM(n) << "\n";)
#ifdef TAU_CACHE
		using cache_t = subtree_unordered_map<node, tref>;
		static cache_t& cache = tree<node>::template create_cache<cache_t>();
		if (auto it = cache.find(n); it != cache.end()) {
			// Update the quantifier pattern after elimination
			quant_pattern.insert_or_assign(tau::trim2(n), 0);
			return it->second;
		}
#endif // TAU_CACHE
		tref n_elim = syntactic_formula_simplification<node>(n);
		n_elim = squeeze_absorb<node>(n_elim, tau::trim2(n_elim));
		DBG(LOG_TRACE << "After squeeze_absorb_down " << LOG_FM(n) << "\n";)
		tref res = nullptr;
		if (is_child<node>(n_elim, tau::wff_all)) {
			tref n_neg = to_nnf<node>(tau::build_wff_neg(n_elim));
			res = pre_order<node>(n_neg).
				apply_unique(anti_prenex_step, step_visit);
			res = to_nnf<node>(tau::build_wff_neg(res));
		} else {
			res = pre_order<node>(n_elim).
				apply_unique(anti_prenex_step, step_visit);
		}
		// Update the quantifier pattern after a successful elimination
		if (quant_eliminated)
			quant_pattern.insert_or_assign(tau::trim2(n_elim), 0);
#ifdef TAU_CACHE
		return cache.emplace(n, res).first->second;
#endif // TAU_CACHE
		return res;
	};
	auto visit = [&](tref n) {
		if (is_quantifier<node>(n))
			quant_pattern.insert_or_assign(tau::trim(n), qid++);
		return visit_wff<node>(n);
	};
	DBG(LOG_DEBUG << "Anti_prenex on " << LOG_FM(formula) << "\n";)
	// Initial simplification of formula
	formula = syntactic_formula_simplification<node>(formula);
	DBG(LOG_TRACE << "After syntactic_formula_simplification: " << LOG_FM(formula) << "\n";)
	// Apply anti prenex procedure
	formula = post_order<node>(formula).apply_unique(inner_quant, visit);
	DBG(LOG_TRACE << "Anti_prenex result: " << LOG_FM(formula) << "\n";)
	formula = syntactic_formula_simplification<node>(formula);
	DBG(LOG_DEBUG << "Anti_prenex result after syntactic simp: " << LOG_FM(formula) << "\n";)
	return formula;
}

template<NodeType node>
tref resolve_quantifiers(tref formula) {
	using tau = tree<node>;
	subtree_set<node> excluded;
	auto resolver = [&](tref n) {
		if (is_child_quantifier<node>(n)) {
			// Check if the formula is closed and proceed to eliminate
			// the quantifier
			tref var = tau::trim2(n);
			if (is_bv_type_family<node>(tau::get(var).get_ba_type())) {
				if (const trefs& free_vars = get_free_vars<node>(n);
					free_vars.empty()) {
					// By assumption quantifier is pushed in all the way
					// Closed bv formula, simplify to T/F
					if (is_bv_formula_sat<node>(n))
						return tau::_T();
					else return tau::_F();
				} else excluded.insert(n);
			}
		}
		return n;
	};
	auto visit = [&](tref n) {
		if (excluded.contains(n)) return false;
		return visit_wff<node>(n);
	};
	return pre_order<node>(formula).apply_unique(resolver, visit);
}

/**
 * @brief Converts the temporal layer of a formula to reduced DNF, squeezes the always
 * statements and ensures that formulas containing temporal variables are
 * explicitly quantified while non-temporal formulas are not quantified temporally.
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
					? boole_normal_form<node>(arg)
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
	bool has_temp_quant = tau::get(fm).find_top(st_aw);
	if (has_temp_var<node>(fm)) {
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
							always_part, conj);
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
		if (st_aw(fm)) return norm(tau::trim2(fm));
		else return norm(fm);
	}
}

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "to_snf"


template <NodeType node>
tref build_split_wff_using(typename node::type type, tref a, tref b) {
	using tau = tree<node>;
	// TODO (HIGH) check formulas, should depend on the type
	if (type == tau::bf_eq)
		return tau::build_wff_or(
			tau::build_wff_and(a, b),
			tau::build_wff_and(tau::build_wff_neg(a),
					   tau::build_wff_neg(b)));
	else return tau::build_wff_and(
		tau::build_wff_or(a, tau::build_wff_neg(b)),
		tau::build_wff_or(tau::build_wff_neg(a), b));
}

template <NodeType node, size_t type>
tref anf(tref n) {
	// TODO (MEDIUM) write anf (using?)
	std::cout << "Not implemented yet.\n";
	return n;
}

template <NodeType node>
tref pnf(tref n) {
	// TODO (MEDIUM) write pnf (using?)
	std::cout << "Not implemented yet.\n";
	return n;
}

} // namespace idni::tau_lang
