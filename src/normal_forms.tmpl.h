// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "normal_forms.h"
#include "union_find.h"

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
		if (!t.is(tau::bf_constant)) return n;
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
#define LOG_CHANNEL_NAME "reduce_deprecated"

// TODO (VERY_HIGH) properly implement it
template <NodeType node, node::type type>
tref reduce_deprecated<node, type>::operator()(tref form) const {
	subtree_map<node, tref> changes;
	// for all type dnfs do...
	for (tref dnf : tau::get(form).select_top(is<node, type>)) {
		auto simplified = simplify(dnf);
		if (simplified != dnf) changes[dnf] = simplified;
	}
	return rewriter::replace<node>(form, changes);
}

template <NodeType node, node::type type>
typename reduce_deprecated<node,type>::tt
	reduce_deprecated<node, type>::operator()(const tt& t) const
{
	return tt(this->operator()(t.value()));
}

template <NodeType node, node::type type>
void reduce_deprecated<node, type>::get_literals(tref clause, literals& lits) const {
	LOG_TRACE << "get_bf_literals of: " << LOG_FM(clause);
	if constexpr (type == tau::bf) {
		if (auto check = tt(clause) | tau::bf_and; check.has_value())
			for (tref c : (check || tau::bf).values())
				get_literals(c, lits);
		else {
			lits.insert(clause);
			LOG_TRACE << "found literal: " << LOG_FM(clause);
		}
	} else {
		if (auto check = tt(clause) | tau::wff_and; check.has_value())
			for (tref c : (check || tau::wff).values())
				get_literals(c , lits);
		else {
			lits.insert(clause);
			LOG_TRACE << "found literal: " << LOG_FM(clause);
		}
	}
}

template <NodeType node, node::type type>
typename reduce_deprecated<node, type>::literals
	reduce_deprecated<node, type>::get_literals(tref clause) const
{
	literals lits;
	get_literals(clause, lits);
	return lits;
}

template <NodeType node, node::type type>
std::pair<typename reduce_deprecated<node, type>::literals,
	typename reduce_deprecated<node, type>::literals>
		reduce_deprecated<node, type>::get_positive_negative_literals(
							tref clause) const
{
	literals positives, negatives;
	for(tref l : get_literals(clause)) if constexpr (type == tau::bf) {
		if (auto check = tt(l) | tau::bf_neg; !check.has_value()) {
			positives.insert(l);
			LOG_TRACE << "found positive: " << LOG_FM(l);
		} else {
			negatives.insert(l);
			LOG_TRACE << "found negative: " << LOG_FM(l);
		}
	} else {
		if (auto check = tt(l) | tau::wff_neg; !check.has_value()) {
			positives.insert(l);
			LOG_TRACE << "found positive: " << LOG_FM(l);
		} else {
			negatives.insert(l);
			LOG_TRACE << "found negative: " << LOG_FM(l);
		}
	}
	return { positives, negatives };
}

template <NodeType node, node::type type>
subtree_set<node> reduce_deprecated<node, type>::get_dnf_clauses(
					tref n, subtree_set<node> clauses) const
{
	if constexpr (type == tau::bf)
		if (auto check = tt(n) | tau::bf_or; check)
			for (tref clause : (check || tau::bf).values())
				clauses = get_dnf_clauses(clause, clauses);
		else clauses.insert(n);
	else
		if (auto check = tt(n) | tau::wff_or; check)
			for (tref clause : (check || tau::wff).values())
				clauses = get_dnf_clauses(clause, clauses);
		else clauses.insert(n);

#ifdef DEBUG
	if (clauses.empty()) {
		LOG_TRACE << "found clause: " << LOG_FM(n);
	} else for (tref clause : clauses)
		LOG_TRACE << "found clause: " << LOG_FM(clause);
#endif // DEBUG

	return clauses;
}

template <NodeType node, node::type type>
tref reduce_deprecated<node, type>::build_dnf_clause_from_literals(
	const literals& positives, const literals& negatives) const
{
	if (positives.empty() && negatives.empty()) {
		LOG_DEBUG << "(F) {}"; return nullptr; }

	trefs lits;
	lits.insert(lits.end(), positives.begin(), positives.end());
	lits.insert(lits.end(), negatives.begin(), negatives.end());

	if (lits.size() == 1) return lits[0];
	tref clause = lits[0];
	for (size_t i = 1; i < lits.size(); ++i)
		if constexpr (type == tau::bf)
			clause = tau::build_bf_and(clause, lits[i]);
		else clause = tau::build_wff_and(clause, lits[i]);

	LOG_DEBUG << "dnf clause built from literals: " << LOG_FM(clause);
	return clause;
}

template <NodeType node, node::type type>
tref reduce_deprecated<node, type>::to_minterm(tref clause) const {
	auto [positives, negatives] = get_positive_negative_literals(clause);
	if constexpr (type == tau::bf) {
		for (tref negation : negatives) {
			auto negated =
				tt(negation) | tau::bf_neg | tau::bf | tt::ref;
			for (tref positive : positives) {
				LOG_TRACE << "are literals " << LOG_FM(positive)
					<< " and " << LOG_FM(negation)
					<< " clashing? ";
				if (tau::get(positive).equals_0()) {
					LOG_TRACE << "yes\n"; return nullptr; }
				else if (tau::get(positive)==tau::get(negated)){
					LOG_TRACE << "yes\n"; return nullptr; }
				else LOG_TRACE << "no\n";
			}
		}
	} else {
		for (tref negation : negatives) {
			auto neq_bf = tt(negation)
					| tau::wff_neg | tau::wff | tt::ref;
			for (tref positive : positives) {
				auto eq_bf = tt(positive)
					| tt::first | tau::bf | tt::ref;
				LOG_TRACE << "are literals " << LOG_FM(positive)
					<< " and " << LOG_FM(negation)
					<< " clashing: ";
				if (tau::get(eq_bf).equals_F()) {
					LOG_TRACE << "yes\n"; return nullptr; }
				else if (tau::get(eq_bf) == tau::get(neq_bf)) {
					LOG_TRACE << "yes\n"; return nullptr; }
				else LOG_TRACE << "no\n";
			}
		}
	}
	return build_dnf_clause_from_literals(positives, negatives);
}

template <NodeType node, node::type type>
tref reduce_deprecated<node, type>::build_dnf_from_clauses(
	const subtree_set<node>& clauses) const
{
	if (clauses.empty()) {
		if constexpr (type == tau::bf) {
			LOG_DEBUG<<"(F) dnf from clauses: 0"; return tau::_0();}
		else  { LOG_DEBUG<<"(F) dnf from clauses: F"; return tau::_F();}
	}
	tref dnf = *clauses.begin();
	for (auto it = ++clauses.begin(); it != clauses.end(); ++it)
		if constexpr (type == tau::bf) dnf = tau::build_bf_or( dnf,*it);
		else                           dnf = tau::build_wff_or(dnf,*it);

	LOG_DEBUG << "dnf from clauses: " << LOG_FM(dnf);
	return dnf;
}
template <NodeType node, node::type type>
tref reduce_deprecated<node, type>::simplify(tref form) const {
	subtree_set<node> clauses;
	LOG_DEBUG << "Begin simplifying of " << LOG_FM(form);
	for (tref clause : get_dnf_clauses(form))
		if (tref dnf = to_minterm(clause); dnf) clauses.insert(dnf);
	tref dnf = build_dnf_from_clauses(clauses);
	LOG_DEBUG << "End simplifying" << LOG_FM(dnf);
	return dnf;
}

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "normal_forms"

template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& t, const reduce_bf_t<node>& r)
{
	using tt = typename tree<node>::traverser;
	return tt(r(t.value()));
}

template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& t, const reduce_wff_t<node>& r)
{
	using tt = typename tree<node>::traverser;
	return tt(r(t.value()));
}


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
			eq_v.first(), var, tau::_0()))
				| bf_reduce_canonical<node>() | tt::ref;
		tref f_1 = tt(rewriter::replace<node>(
			tau::build_bf_neg(eq_v.first()), var,tau::_1()))
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
			neq.first(), var, tau::_0()))
				| bf_reduce_canonical<node>() | tt::ref;
		tref f_1 = tt(rewriter::replace<node>(
			tau::build_bf_neg(neq.first()), var, tau::_1()))
				| bf_reduce_canonical<node>() | tt::ref;
		changes[neq.get()] = tau::trim(tau::build_wff_or(
			tau::build_bf_nlteq_upper(f_0, var),
			tau::build_bf_nlteq_lower(f_1, var)));
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
			fm_simp = reduce_depreciated<node>(fm_simp, tau::bf);
			DBG(LOG_TRACE << "reduce result: " << LOG_FM(fm_simp);)
			if (tau::get(fm_simp).equals_0()) return report(false);
		} else {
			if (tau::get(fm).equals_F()) return report(false);
			// fm is a Tau formula
			fm_simp = to_dnf<node, false>(fm);
			DBG(LOG_TRACE << "to_dnf result: " << LOG_FM(fm_simp);)
			if (tau::get(fm_simp).equals_F()) return report(false);
			fm_simp = reduce_depreciated<node>(fm_simp, tau::wff);
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
	tref t = is_wff ? tau::_T() : tau::_1();
	tref f = is_wff ? tau::_F() : tau::_0();
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

// Given a BF b, calculate the Boole normal form (DNF corresponding to the pathes to true in the BDD) of b
// where the variable order is given by the function lex_var_comp
template <NodeType node>
tref bf_reduced_dnf(tref fm, bool make_paths_disjoint) {
	using tau = tree<node>;
	LOG_TRACE << "bf_boole_normal_form: " << LOG_FM(fm);
	static auto trace = [&](tref fm) {
		LOG_TRACE << "bf_boole_normal_form result: " << LOG_FM(fm);
		return fm;
	};
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
	sort(vars.begin(), vars.end(), lex_var_comp<node>);

	std::vector<int_t> i(vars.size()); // Record assignments of vars

	// Resulting DNF - make it ordered for consistency
	// Key is coefficient, value is possible variable assignments for coefficient
	// unordered_tau_map<std::vector<std::vector<int_t>>, BAs...> dnf;
	subtree_map<node, std::vector<std::vector<int_t>>> dnf;

	if (assign_and_reduce<node>(fm, vars, i, dnf, is_var, 0)) {
		assert(dnf.size() == 1);
		return trace(dnf.begin()->first);
	}
	if (dnf.empty()) return trace(_0<node>());
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

// The needed class in order to make bf_to_reduce_dnf work with rule applying process
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
	bool wff)
{
	using tau = tree<node>;
	if (paths.empty()) return is_cnf ? (wff ? tau::_T() : tau::_1())
					 : (wff ? tau::_F() : tau::_0());
	tref reduced_fm = is_cnf ? (wff ? tau::_F() : tau::_0())
				 : (wff ? tau::_T() : tau::_1());
	bool first = true;
	for (const auto& path : paths) {
		if (path.empty()) continue;
		bool first_var = true;
		tref var_path = is_cnf  ? (wff ? tau::_F() : tau::_0())
					: (wff ? tau::_T() : tau::_1());
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

template <NodeType node>
tref sort_var(tref var) {
	using tau = tree<node>;
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, tref>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	if (auto it = cache.find(var); it != end(cache))
		return it->second;
#endif // TAU_CACHE
	const auto& t = tau::get(var);
	DBG(LOG_TRACE << "sort var: " << LOG_FM(var);)
	if (t.child_is(tau::bf_eq)) {
		trefs clauses = get_dnf_bf_clauses<node>(t[0].first());
		LOG_TRACE << "sort_var / child clauses size: " <<clauses.size();
		std::ranges::sort(clauses, lcrs_tree<node>::subtree_less);
		tref res = nullptr;
		for (tref c : clauses) {
			LOG_TRACE << "sort_var / child clause: " << LOG_FM(c);
			trefs lits = get_cnf_bf_clauses<node>(c);
			for (tref l : lits) LOG_TRACE << "sort_var / literal: "
							<< LOG_FM_DUMP(l);
			std::ranges::sort(lits, lex_var_comp<node>);
			if (res) res = tau::build_bf_or(res,
					tau::build_bf_and(lits));
			else res = tau::build_bf_and(lits);
			LOG_TRACE << "sort_var / res: " << LOG_FM(res);
		}
		res = tau::build_bf_eq_0(res);
		LOG_TRACE << "sort_var / res: " << LOG_FM(res);
#ifdef TAU_CACHE
		cache.emplace(res, res);
		return cache.emplace(var, res).first->second;
#endif // TAU_CACHE
		return res;
	}
	return var;
}

//TODO: decide if to treat xor in bf case
template <NodeType node>
std::pair<std::vector<std::vector<int_t>>, trefs> dnf_cnf_to_bdd(
	tref fm, size_t type, bool is_cnf, bool all_reductions,bool enable_sort)
{
	using tau = tree<node>;
	LOG_TRACE << "dnf_cnf_to_bdd: " << LOG_FM(fm);
	LOG_TRACE << "type: " << type << " is_cnf: " << is_cnf
		<< " all_reductions: " << all_reductions
		<< " enable_sort: " << enable_sort;
	DBG(assert(tau::get(fm).is(type));)
	// Pull negation out of equality
	bool wff = type == tau::wff;
	tref new_fm = wff ? unequal_to_not_equal<node>(fm) : apply_all_xor_def<node>(fm);
	if (wff) {
		LOG_TRACE << "dnf_cnf_to_bdd / mnf: " << LOG_FM(new_fm);
		// Make equalities canonical
		subtree_map<node, tref> changes;
		trefs eqs = tau::get(new_fm)
			.select_top(is_child<node, tau::bf_eq>);
		for (tref eq : eqs) {
			tref sorted_eq = sort_var<node>(eq);
			LOG_TRACE << "dnf_cnf_to_bdd / sorted_eq: "
				<< LOG_FM(sorted_eq);
			if (tau::get(sorted_eq) != tau::get(eq))
				changes.emplace(eq, sorted_eq);
		}
		new_fm = rewriter::replace<node>(new_fm, changes);
		LOG_TRACE << "dnf_cnf_to_bdd / sorted: " << LOG_FM(new_fm);
	}

	trefs vars = wff ? tau::get(new_fm).select_top(is_wff_bdd_var<node>)
			 : tau::get(new_fm).select_top(is_bf_bdd_var<node>);
	LOG_TRACE << "dnf_cnf_to_bdd / vars.size(): " << vars.size();
	if (vars.empty()) {
		// std::cout << "new_fm: " << new_fm << "\n";
		//assert(tau::get(new_fm).equals_T() || tau::get(new_fm).equals_F() ||
		//	tau::get(new_fm).equals_1() || tau::get(new_fm).equals_0());
		if (tau::get(new_fm).equals_T() || tau::get(new_fm).equals_1()) {
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
	// Sort bf variables canonically
	if (!wff && enable_sort) std::ranges::sort(vars, lex_var_comp<node>);

	bool decided = true;
	auto paths = collect_paths<node>(new_fm, wff, vars, decided, is_cnf,
					all_reductions);

	if (all_reductions) join_paths(paths);
	if (paths.empty() && !decided) paths.emplace_back();
	if (enable_sort) std::ranges::sort(paths);	

#ifdef DEBUG
	std::stringstream ss;
	for (tref v : vars)
		ss << "\t\t\t\t(I) var: \t" << TAU_TO_STR(v) << "\n";
	for (const auto& p : paths) {
		ss << "\t\t\t\t(I) path:\t";
		for (const auto& i : p) ss << i << " ";
		ss << "\n";
	}
	LOG_TRACE << "dnf_cnf_to_bdd / vars\n" << ss.str();
#endif // DEBUG
	return std::make_pair(std::move(paths), std::move(vars));
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
	tref grouped_fm = wff ? tau::_F() : tau::_0();
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

		tref cl1 = wff ? tau::_T() : tau::_1();
		tref cl2 = wff ? tau::_T() : tau::_1();
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

// Simplifies for example abx'|aby'|xy to ab|xy
// General idea is to eliminate (xyz...)' | xyz... after factorization
template <NodeType node>
tref simp_general_excluded_middle(tref fm) {
	using tau = tree<node>;
	LOG_TRACE << "simp_general_excluded_middle: " << LOG_FM(fm);
	assert(tau::get(fm).is(tau::bf));
	bool was_simplified = false;
	auto grouped = group_dnf_expression<node>(fm);
	trefs clauses = get_dnf_bf_clauses<node>(grouped);
	for (int_t i = 0; i < (int_t) clauses.size(); ++i) {
		bool erase_clause = false;
		tref cn = push_negation_in<node, false>(
			tau::build_bf_neg(clauses[i]));
		for (int_t j = 0; j < (int_t) clauses.size(); ++j) {
			if (i == j) continue;
			bool has_simp = false;
			// TODO: generalize to recursion on grouped expression, for example for abx' | aby' | axy
			trefs cnf = get_cnf_bf_clauses<node>(clauses[j]);
			for (size_t k = 0; k < cnf.size(); ++k) {
				// TODO: generalize to subset
				if (tau::get(cnf[k]) == tau::get(cn)) {
					cnf.erase(cnf.begin() + k);
					has_simp = true;
					was_simplified = true;
					if (cnf.empty()) erase_clause = true;
					break;
				}
			}
			if (has_simp) {
				clauses.erase(clauses.begin() + j);
				clauses.emplace_back(tau::build_bf_and(cnf));
				if (i >= j) --i;
				--j;
			}
		}
		if (erase_clause) {
			clauses.erase(clauses.begin() + i);
			--i;
		}
	}
	auto r = was_simplified
			? to_dnf<node, false>(tau::build_bf_or(clauses)) : fm;
	LOG_TRACE << "simp_general_excluded_middle result: " << LOG_FM(r);
	return r;
}

// Assume that fm is in DNF (or CNF -> set is_cnf to true)
// TODO: Normalize Tau constants in case type == bf
template <NodeType node>
tref reduce_depreciated(tref fm, size_t type, bool is_cnf,
	bool all_reductions, bool enable_sort)
{
	using tau = tree<node>;
#ifdef TAU_CACHE
	using cache_t = std::unordered_map<std::tuple<tref, bool, bool>, tref,
				std::hash<std::tuple<tref, bool, bool>>,
				subtree_bool_bool_tuple_equality<node>>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(std::make_tuple(
					fm, all_reductions, enable_sort));
		it != end(cache)) return it->second;
#endif // TAU_CACHE
	LOG_TRACE << "Begin reduce with is_cnf set to " << is_cnf;
	LOG_TRACE << "Formula to reduce: " << LOG_FM(fm);
	auto [paths, vars] = dnf_cnf_to_bdd<node>(fm, type, is_cnf,
						all_reductions, enable_sort);

	bool wff = type == tau::wff;
	if (paths.empty()) {
		auto res = is_cnf ? (wff ? tau::_T() : tau::_1())
				  : (wff ? tau::_F() : tau::_0());
#ifdef TAU_CACHE
		return cache.emplace(std::make_tuple(fm, all_reductions, enable_sort), res).first->second;
#endif // TAU_CACHE
		return res;
	}
	if (paths.size() == 1 && paths[0].empty()) {
		auto res = is_cnf ? (wff ? tau::_F() : tau::_0())
				  : (wff ? tau::_T() : tau::_1());
#ifdef TAU_CACHE
		return cache.emplace(std::make_tuple(fm, all_reductions, enable_sort), res).first->second;
#endif // TAU_CACHE
		return res;
	}

	auto reduced_fm = build_reduced_formula<node>(paths, vars, is_cnf, wff);
	LOG_TRACE << "End reduce";
	LOG_TRACE << "Reduced formula: " << LOG_FM(reduced_fm);
#ifdef TAU_CACHE
	return cache.emplace(std::make_tuple(
		fm, all_reductions, enable_sort), reduced_fm).first->second;
#endif // TAU_CACHE
	return reduced_fm;
}

// Assume that fm is in DNF (or CNF -> set is_cnf to true)
template<NodeType node, bool is_cnf>
tref reduce(tref fm) {
	using tau = tree<node>;
	bool is_wff = !tau::get(fm).is_term();
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, tref>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(fm); it != end(cache)) return it->second;
#endif // TAU_CACHE
	DBG(LOG_TRACE << "Begin reduce with is_cnf set to " << is_cnf;)
	DBG(LOG_TRACE << "Formula to reduce: " << LOG_FM(fm);)
	auto [paths, vars] = dnf_cnf_to_reduced<node>(fm, is_cnf);
	if (paths.empty()) {
		auto res = is_cnf ? (is_wff ? tau::_T() : tau::_1())
				  : (is_wff ? tau::_F() : tau::_0());
#ifdef TAU_CACHE
		return cache.emplace(fm, res).first->second;
#endif // TAU_CACHE
		return res;
	}
	if (paths.size() == 1 && paths[0].empty()) {
		auto res = is_cnf ? (is_wff ? tau::_F() : tau::_0())
				  : (is_wff ? tau::_T() : tau::_1());
#ifdef TAU_CACHE
		return cache.emplace(fm, res).first->second;
#endif // TAU_CACHE
		return res;
	}
	auto reduced_fm = build_reduced_formula<node>(paths, vars, is_cnf, is_wff);
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
tref reduce_terms(tref fm, bool with_sorting) {
	using tau = tree<node>;
	LOG_TRACE << "reduce_terms: " << LOG_FM(fm);
	subtree_map<node, tref> changes = {};
	for (const auto& bf : tau::get(fm)
		.select_top(is<node, tau::bf>))
	{
		auto dnf = to_dnf<node, false>(bf);
		dnf = reduce_depreciated<node>(dnf, tau::bf, false, true, with_sorting);
		if (dnf != bf) changes[bf] = dnf;
	}

	if (changes.empty()) {
		LOG_TRACE << "reduce_terms result: (no change) " << LOG_FM(fm);
		return fm;
	}
	fm = rewriter::replace<node>(fm, changes);
	LOG_TRACE << "reduce_terms result: " << LOG_FM(fm);
	return fm;
}

// Apply equalities across clauses for further simplifications
// Assumes that fm is in DNF with equalities pushed in and
// inequalities pushed out
template <NodeType node>
tref apply_eqs_across_clauses(tref fm) {
	using tau = tree<node>;
	trefs eq_reductions, neq_reductions;
	auto clauses = get_dnf_wff_clauses<node>(fm);
	for (tref clause : clauses) {
		auto lits = get_cnf_wff_clauses<node>(clause);
		if (lits.size() == 1) {
			const auto& t = tau::get(lits[0]);
			if (t.child_is(tau::bf_eq))
				eq_reductions.emplace_back(t[0].first());
			else if (t.child_is(tau::bf_neq)) {
				auto dnf_lits =
					get_dnf_bf_clauses<node>(t[0].first());
				for (tref lit : dnf_lits)
					neq_reductions.emplace_back(lit);
			}
		}
	}
	if (eq_reductions.empty() && neq_reductions.empty()) return fm;
	tref new_fm = nullptr;
	subtree_map<node, tref> changes;
	for (tref clause : clauses) {
		auto neqs = tau::get(clause)
			.select_top(is_child<node, tau::bf_neq>);
		for (tref neq : neqs) {
			bool replace = false;
			auto neq_lits = get_dnf_bf_clauses<node>(
				tau::get(neq)[0].first());
			for (tref eq : eq_reductions) {
				for (tref neq_lit : neq_lits) {
					if (tau::get(eq) == tau::get(neq_lit)) {
						changes.emplace(neq, tau::_T());
						replace = true;
					}
					if (replace) break;
				}
				if (replace) break;
			}
		}
	}
	if (!changes.empty()) {
		new_fm = rewriter::replace<node>(fm, changes);
		if (neq_reductions.empty()) return new_fm;
		clauses = get_dnf_wff_clauses<node>(new_fm);
	}
	if (neq_reductions.empty()) return fm;
	changes.clear();
	for (tref clause : clauses) {
		auto eqs = tau::get(clause)
			.select_top(is_child<node, tau::bf_eq>);
		for (tref eq : eqs) {
			for (tref neq : neq_reductions) {
				if (tau::get(eq)[0][0] == tau::get(neq)) {
					changes.emplace(eq, tau::_T());
					break;
				}
			}
		}
	}
	if (!changes.empty()) {
		if (new_fm) return rewriter::replace<node>(new_fm, changes);
		else return rewriter::replace<node>(fm, changes);
	}
	if (new_fm) return new_fm;
	else return fm;
}

template <NodeType node>
trefs push_eq_and_get_vars(trefs fm) {
	using tau = tree<node>;
	using tt = tau::traverser;
 // First push in equalities all the way (bf != 0 is converted to !(bf = 0))
	fm = tt(fm) | (tau_f<node>) unsqueeze_wff<node>
		    | (tau_f<node>) unequal_to_not_equal<node>;
	// Find atomic formulas
	return tree<node>::get(fm).select_top(is_wff_bdd_var<node>);
}

// Requires fm to have Boolean functions in DNF and equalities pushed in
// vars contains variable order
template <NodeType node>
std::vector<std::vector<int_t>> wff_to_bdd(tref fm, auto& vars) {
	using tau = tree<node>;
	using tt = tau::traverser;
	// Find atomic formulas
	auto pushed_in_fm = fm;
	if (vars.empty()) vars = push_eq_and_get_vars<node>(pushed_in_fm);
	else pushed_in_fm = tt(fm) | (tau_f<node>) unsqueeze_wff<node>
				   | (tau_f<node>) unequal_to_not_equal<node> | tt::ref;
	std::vector<int_t> i(vars.size());
	subtree_map<node, std::vector<std::vector<int_t>>> dnf;
	// dnf empty means false and and size 1 with empty paths means true
	assign_and_reduce<node>(pushed_in_fm, vars, i, dnf,
					is_wff_bdd_var<node>, 0, true);
	assert((dnf.size() == 1 || dnf.empty()));
	if (dnf.empty()) return {};
	join_paths(dnf.begin()->second);

	if (dnf.begin()->second.empty())
		dnf.begin()->second.emplace_back();
	return dnf.begin()->second;
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
		// Check match
		if (tau::get(v1[j]) == tau::get(v2[k])) --i, ++j, ++k;
		else if (tau::subtree_less(v1[j], v2[k])) ++j;
		else ++k;
	}
	return i == 0;
}

template <NodeType node>
std::vector<std::vector<trefs>> get_cnf_inequality_lits(tref fm) {
	using tau = tree<node>;
	tref neq_pushed_in = unsqueeze_wff_neg<node>(fm);
	if (tau::get(neq_pushed_in).equals_T()) return {};
	// cout << "neq_pushed_in: " << neq_pushed_in << "\n";
	std::vector<std::vector<trefs>> cnf_lits;
	for (tref clause : get_cnf_wff_clauses<node>(neq_pushed_in)) {
		std::vector<trefs> c;
		for (tref neqs : get_dnf_wff_clauses<node>(clause))
			c.emplace_back(get_cnf_bf_clauses<node>(tau::trim2(neqs)));
		cnf_lits.emplace_back(std::move(c));
	}
	return cnf_lits;
}

template <NodeType node>
std::pair<std::vector<int_t>, bool> simplify_path(
	const std::vector<int_t>& path, trefs& vars)
{
	using tau = tree<node>;
	using tt = tau::traverser;
	// std::cout << vars << "\n";
	std::vector<trefs> pos;
	// Build clause for non-equality terms
	// and new representation for equality terms
	tref clause = tau::_T();
	tref pos_bf = tau::_0();
	tref negs_wff = tau::_T();
	for (size_t p = 0; p < path.size(); ++p) {
		if (path[p] == 2) continue;
		const auto& t = tau::get(vars[p]);
		LOG_TRACE << "simplify path / vars[p]: " << LOG_FM(vars[p]);
		if (t.child_is(tau::bf_eq)) {
			if (path[p] == -1) {
				auto vs = simp_general_excluded_middle<node>(
								t[0].first());
				negs_wff = tau::build_wff_and(negs_wff,
							tau::build_bf_neq_0(vs));
			} else pos_bf = tau::build_bf_or(pos_bf, t[0].first());
		}
		else if (path[p] == -1) clause = tau::build_wff_and(clause,
						tau::build_wff_neg(vars[p]));
		else if (path[p] == 1) clause = tau::build_wff_and(clause,
								vars[p]);
		else assert(false);
	}
	// Simplify equalities/inequalities
	// Here new variables can be created
	LOG_TRACE << "simplify path / pos_bf: " << LOG_FM(pos_bf);
	pos_bf = reduce_depreciated<node>(pos_bf, tau::bf, false, true, false);
	pos_bf = simp_general_excluded_middle<node>(pos_bf);
	// std::cout << "pos_bf after reduce: " << LOG_FM(pos_bf) << "\n";
	tref new_pos_bf = nullptr;
	for (tref c : get_dnf_bf_clauses<node>(pos_bf)) {
		// If pos_bf simplifies to 0, do not add to pos
		if (!tau::get(c).equals_0())
			pos.emplace_back(get_cnf_bf_clauses<node>(c));
		if (new_pos_bf) new_pos_bf = tau::build_wff_and(new_pos_bf,
							tau::build_bf_eq_0(c));
		else new_pos_bf = tau::build_bf_eq_0(c);
	}
	// std::cout << "New_pos_bf: " << LOG_FM(new_pos_bf) << "\n";
	clause = tau::build_wff_and(clause, new_pos_bf);

	// cout << "Pos clause: " << LOG_FM(clause) << "\n";
	auto cnf_neq_lits = get_cnf_inequality_lits<node>(negs_wff);
	// Check if any term in pos is subset of any term in negs
	for (const auto& p : pos) {
		for (auto& neq_clause : cnf_neq_lits){
			for (auto& n : neq_clause) {
				// Simplify cases p' = 0 && p != 0 and p = 0 && p' != 0
				if (p.size() == 1 && n.size() == 1) {
					const auto& t = tau::get(p[0]);
					const auto& u = tau::get(n[0]);
					if (t.child_is(tau::bf_neg)) {
						if (t[0][0] == u) {
							n[0] = tau::_1();
							continue;
						}
					} else if (u.child_is(tau::bf_neg)) {
						if (t == u[0][0]) {
							n[0] = tau::_1();
							continue;
						}
					}
				}
				// Inequality n is unsat if p is subset
				if (is_ordered_subset<node>(p, n)) n = {};
			}
			// If all inequalities of the cnf clause are unsat, this path is unsat
			if (std::ranges::all_of(neq_clause,
				[](const auto& el) { return el.empty(); }))
			{
				return {{}, true};
			}
		}
	}
	// Eliminate redundant != 0 in cnf
	for (int_t c1 = 0; c1 < (int_t) cnf_neq_lits.size(); ++c1) {
		for (int_t c2 = 0; c2 < (int_t) cnf_neq_lits.size(); ++c2) {
			if (c1 == c2) continue;
			bool is_impl = true;
			for (size_t i = 0; i < cnf_neq_lits[c1].size(); ++i) {
				if (cnf_neq_lits[c1][i].empty()) continue;
				bool exists_superset = false;
				for (size_t j = 0;
					j < cnf_neq_lits[c2].size(); ++j)
				{
					if (cnf_neq_lits[c2][j].empty())
						continue;
					if (is_ordered_subset<node>(
						cnf_neq_lits[c2][j],
						cnf_neq_lits[c1][i]))
					{
						exists_superset = true;
						break;
					}
				}
				// If not exists_superset, c1[i] is not a superset for some c2[j]
				// Hence, c1 cannot imply c2
				if (!exists_superset) {
					is_impl = false;
					break;
				}
			}
			if (is_impl) {
				// c1 implies c2
				cnf_neq_lits.erase(cnf_neq_lits.begin() + c2);
				if (c1 >= c2) --c1;
				break;
			}
		}
	}

	tref neq_cnf = nullptr;
	for (const auto& neq_clause : cnf_neq_lits) {
		if (neq_clause.empty()) continue;
		tref neqs = nullptr;
		for (const auto& n : neq_clause) {
			if (n.empty()) continue;
			if (neqs) neqs = tau::build_wff_or(neqs,
				tau::build_bf_neq_0(tau::build_bf_and(n)));
			else neqs = tau::build_bf_neq_0(tau::build_bf_and(n));
		}
		if (neqs) {
			if (neq_cnf) neq_cnf = tau::build_wff_and(neq_cnf, neqs);
			else neq_cnf = neqs;
		}
	}
	if (neq_cnf) {
		// Convert back to cnf and push inequalities back out
		// std::cout << "neq_cnf: " << neq_cnf << "\n";
		neq_cnf = reduce_depreciated<node>(neq_cnf, tau::wff, true, true, false);
		// std::cout << "neq_cnf after reduce: " << neq_cnf << "\n";
		neq_cnf = tt(neq_cnf) | tt::f(squeeze_wff_neg<node>)
				| tt::f(unequal_to_not_equal<node>) | tt::ref;
		// cout << "neq_cnf simplified: " << neq_cnf << "\n";
		clause = tau::build_wff_and(clause, neq_cnf);
	}
	// std::cout << "new clause: " << clause << "\n";
	auto new_vars = tau::get(clause).select_top(is_wff_bdd_var<node>);

	// unordered_tau_map<size_t, BAs...> var_to_idx;
	subtree_map<node, size_t> var_to_idx;
	for (size_t i = 0; i < vars.size(); ++i) var_to_idx.emplace(vars[i], i);

	for (tref v : new_vars) {
		if (auto it = var_to_idx.find(v); it == end(var_to_idx)) {
			// First check if it is not sorted canonically
			auto sorted_v = sort_var<node>(v);
			// std::cout << "old var: " << tau::get(v) << "\n";
			// std::cout << "new var: " << tau::get(sorted_v) << "\n";
			if (sorted_v != v && var_to_idx.contains(sorted_v)) {
				// Rename variable in current clause
				clause = rewriter::replace<node>(
					clause, v, sorted_v);
				continue;
			}
			if (sorted_v != v) {
				clause = rewriter::replace<node>(
					clause, v, sorted_v);
			}
			// There is a new variable
			DBG(assert(sorted_v != tau::build_bf_eq_0(tau::_T())
				&& sorted_v != tau::build_bf_eq_0(tau::_F()));)
			vars.push_back(sorted_v);
			var_to_idx.emplace(sorted_v, vars.size() - 1);
		}
	}
	return clause_to_vector<node>(clause, var_to_idx, true, false);
}

template <NodeType node>
std::pair<tref, bool> group_paths_and_simplify(
	std::vector<std::vector<int_t>>& paths, const auto& vars)
{
	LOG_DEBUG << "Start group_paths_and_simplify";
	using tau = tree<node>;
	if (paths.empty()) return { tau::_F(), false};
	if (paths.size() == 1 && paths[0].empty()) return { tau::_T(), false };
	auto is_neq = [&](const int_t path, const int_t idx) {
		if (tau::get(vars[idx]).child_is(tau::bf_eq))
			return paths[path][idx] != 1;
		return false;
	};

	// Group those paths together which have the same non != part
	std::vector<std::vector<std::vector<int_t>>> groups(paths.size());
	bool is_simp = false;
	for (size_t i = 0; i < paths.size(); ++i) {
		for (size_t j = i + 1; j < paths.size(); ++j) {
			// Check if paths agree on non != vars
			bool is_equal = true;
			for (size_t k = 0; k < paths[i].size(); ++k) {
				if (is_neq(i, k) && is_neq(j, k)) continue;
				if (paths[i][k] != paths[j][k]) {
					is_equal = false;
					break;
				}
			}
			// Here paths agree
			if (is_equal) {
				groups[i].emplace_back(std::move(paths[j]));
				paths.erase(paths.begin() + j);
				--j;
			}
		}
	}

	// paths[0] is connected to all in groups[0]
	auto neq_from_path = [&](const auto& path) {
		tref clause = tau::_T();
		for (size_t k = 0; k < path.size(); ++k) {
			const auto& t = tau::get(vars[k]);
			if (path[k] == -1 && t.child_is(tau::bf_eq))
				clause = tau::build_wff_and(clause,
					tau::build_bf_neq_0(t[0].first()));
		}
		return clause;
	};
	tref result = tau::_F();
	for (size_t i = 0; i < paths.size(); ++i) {
		// Now add from paths[i] and groups[i]
		tref neqs = neq_from_path(paths[i]);
		LOG_DEBUG << "neqs: " << LOG_FM(neqs);
		for (const auto& path : groups[i])
			neqs = tau::build_wff_or(neqs, neq_from_path(path));
		LOG_DEBUG << "neqs: " << LOG_FM(neqs);
		LOG_DEBUG << "groups[" << i << "].size(): " << groups[i].size();
		if (!groups[i].empty()) {
			// simp to cnf
			neqs = to_cnf<node>(neqs);
			neqs = reduce_depreciated<node>(neqs, tau::wff, true);
			// push != out
			tref neqs_squeezed = squeeze_wff_neg<node>(neqs);
			if (neqs_squeezed != neqs) {
				// Further simplification might be possible
				is_simp = true;
				neqs = neqs_squeezed;
			}
		}
		trefs neq_clauses = get_cnf_wff_clauses<node>(neqs);
		// std::cout << "neq_clauses: " << neq_clauses << "\n";
		// Simplify bfs and drop !=
		bool clause_false = false;
		for (tref& neq : neq_clauses) {
			LOG_DEBUG << "neq: " << LOG_FM(neq);
			if (tau::get(neq).equals_T()) continue;
			auto f = [](tref n) {
				if (tau::get(n).child_is(tau::bf_neq)) {
					tref nn = reduce_depreciated<node>(tau::trim2(n), tau::bf);
					return tau::build_bf_neq_0(simp_general_excluded_middle<node>(nn));
				}
				return n;
			};
			neq = pre_order<node>(neq).apply_unique(f, visit_wff<node>);
			if (tau::get(neq).equals_F()) clause_false = true;
		}
		if (clause_false) continue;
		// std::cout << "neq_clause after reduce: " << neq_clauses << "\n";
		// Simplify neqs with assumptions from = part
		tref rest = tau::_T();
		for (size_t k = 0; k < paths[i].size(); ++k) {
			if (paths[i][k] == 1) {
				const auto& t = tau::get(vars[k]);
				if (t.child_is(tau::bf_eq)) {
					for (tref clause :
						get_dnf_bf_clauses<node>(
							t[0].first()))
					{
						// TODO (HIGH) refactor - is too deep
						// Here single assumption
						auto neg_eq = push_negation_in<node, false>(
							tau::build_bf_neg(clause));
						// std::cout << "neg_eq: " << neg_eq << "\n";
						for (tref& neq : neq_clauses) {
							if (tau::get(neq).equals_T()
								|| tau::get(neq).equals_1()) continue;
							auto f = [&neg_eq, &is_simp](tref n) {
								if (tau::get(n).child_is(tau::bf_neq)) {
									tref grouped_bf = group_dnf_expression<node>(tau::trim2(n));
									tref simp_neq = rewriter::replace<node>(grouped_bf, neg_eq, tau::_1());
									if (tau::get(grouped_bf) != tau::get(simp_neq)) {
										simp_neq = to_dnf<node, false>(simp_neq);
										simp_neq = reduce_depreciated<node>(simp_neq, tau::bf);
										is_simp = true;
										return tau::build_bf_neq_0(simp_neq);
									}
								}
								return n;
							};
							neq = pre_order<node>(neq).apply_unique(f, visit_wff<node>);
						}
					}
				}
				rest = tau::build_wff_and(rest, vars[k]);
			} else if (paths[i][k] == -1
				&& !tau::get(vars[k]).child_is(tau::bf_eq))
				rest = tau::build_wff_and(rest,
					tau::build_wff_neg(vars[k]));
		}
		result = tau::build_wff_or(result,
			tau::build_wff_and(rest,
				tau::build_wff_and(neq_clauses)));
	}
	DBG(assert(result != nullptr);)
	result = to_dnf<node>(not_equal_to_unequal<node>(result));
	LOG_DEBUG << "End group_paths_and_simplify";
	LOG_DEBUG << "result: " << LOG_FM(result);
	LOG_DEBUG << "is_simp: " << is_simp;
	return { result, is_simp };
}

template <NodeType node>
tref reduce_across_bfs(tref fm, bool to_cnf) {
	using tau = tree<node>;
	LOG_DEBUG << "Start reduce_across_bfs (to_cnf=" << to_cnf << "): "
							<< LOG_FM(fm);
	auto squeezed_fm = (to_cnf
			? push_negation_in<node>(tau::build_wff_neg(fm)) : fm);
	// Squeeze all equalities and inequalities
	squeezed_fm = squeeze_wff<node>(squeezed_fm);
	LOG_TRACE << "squeezed wff: " << LOG_FM(squeezed_fm);
	squeezed_fm = reduce_terms<node>(to_dnf<node>(squeezed_fm));
	LOG_TRACE << "to dnf and reduced terms: " << LOG_FM(squeezed_fm);
	// We work with unsqueezed equality
	squeezed_fm  = unsqueeze_wff_pos<node>(squeezed_fm);
	// std::cout << "squeezed_fm: " << squeezed_fm << "\n";
	LOG_DEBUG << "Formula in DNF: " << LOG_FM(squeezed_fm);
#ifdef TAU_CACHE
	using cache_t = std::map<std::pair<tref, bool>, tref,
				subtree_pair_less<node, bool>>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(std::make_pair(squeezed_fm, to_cnf));
			it != end(cache)) return it->second;
#endif // TAU_CACHE
	auto [paths, vars] = dnf_cnf_to_bdd<node>(squeezed_fm, tau::wff,
							false, true, false);
	// Vars might not be sorted canonically
	// std::cout << "original vars: " << vars << "\n";

#ifdef DEBUG
	std::stringstream ss;
	ss << "vars: " << vars.size() << "\n";
	for (tref v : vars)
		ss << "\t\t\t\tvar: " << TAU_TO_STR(v) << "\n";
	ss << "\t\t\t(F) paths.size(): " << paths.size() << "\n";
	for (const auto& p : paths) {
		ss << "\t\t\t\t\tpath:\t";
		for (const auto& i : p) ss << i << " ";
		ss << "\n";
	}
	LOG_TRACE << ss.str();
#endif // DEBUG

	if (paths.empty()) {
		tref res = to_cnf ? tau::_T() : tau::_F();
#ifdef TAU_CACHE
		return cache.emplace(std::make_pair(
				squeezed_fm, to_cnf), res).first->second;
#endif // TAU_CACHE
		LOG_DEBUG << "End reduce_across_bfs";
		LOG_DEBUG << "res: " << LOG_FM(res);
		return res;
	} else if (paths.size() == 1 && paths[0].empty()) {
		tref res = to_cnf ? tau::_F() : tau::_T();
#ifdef TAU_CACHE
		return cache.emplace(std::make_pair(
				squeezed_fm, to_cnf), res).first->second;
#endif // TAU_CACHE
		LOG_DEBUG << "End reduce_across_bfs";
		LOG_DEBUG << "res: " << LOG_FM(res);
		return res;
	}

#ifdef DEBUG
	for (tref var : vars) // Check that vars are not T or F.
		assert(!tau::get(var).equals_T() && !tau::get(var).equals_F());
#endif // DEBUG

	bool has_simp = true;
	while (has_simp) {
		LOG_DEBUG << "New fix point run";
		has_simp = false;
		for (int_t i = 0; i < (int_t) paths.size(); ++i) {
			auto len_vars = vars.size();
			auto [simp_path, f]
				= simplify_path<node>(paths[i], vars);
			if (vars.size() != len_vars) {
				// New variable was created -> append 2 to each path
				for (size_t k=0; k < vars.size()-len_vars; ++k)
					std::ranges::for_each(paths,
						[](auto& path) {
							path.emplace_back(2);});
			}

			if (f) {
				DBG(assert(len_vars == vars.size());)
				paths.erase(paths.begin()+i); --i;
			}
			else if (simp_path != paths[i]) {

				// std::cout << "(I) Path simplification happened: \n";
				// std::vector<std::vector<int_t> > tmp1{paths[i]};
				// std::vector<std::vector<int_t> > tmp2{simp_path};
				// std::cout << "(F) Current path: " <<
						// tau::get(build_reduced_formula<node>(
							// tmp1, vars, false,
							// true)) << "\n";
				// std::cout << "(F) Simplified path: " <<
						// tau::get(build_reduced_formula<node>(
							// tmp2, vars, false,
							// true)) << "\n";
// #ifdef DEBUG
// 				LOG_DEBUG << "Path simplification happened: ";
// 				std::vector<std::vector<int_t> > tmp1{paths[i]};
// 				std::vector<std::vector<int_t> > tmp2{simp_path};
// 				LOG_DEBUG << "Current path: " << LOG_FM(
// 						build_reduced_formula<BAs...>(
// 							tmp1, vars, false,
// 							true));
// 				LOG_DEBUG << "Simplified path: " << LOG_FM(
// 						build_reduced_formula<BAs...>(
// 							tmp2, vars, false,
// 							true));
// #endif // DEBUG
				if (std::ranges::all_of(simp_path,
					[](const auto el) { return el == 2; }))
				{
					auto res = to_cnf ? tau::_F()
							  : tau::_T();
					LOG_DEBUG << "End reduce_across_bfs";
					LOG_DEBUG << "res: " << LOG_FM(res);
#ifdef TAU_CACHE
					return cache.emplace(std::make_pair(
							squeezed_fm, to_cnf),
						res).first->second;
#endif // TAU_CACHE
					return res;
				}
				has_simp = true;
				paths[i] = std::move(simp_path);
			}
		}
		if (has_simp) join_paths(paths);
	}

	while (true) {
		auto [simp_fm, has_further_simp] =
			group_paths_and_simplify<node>(paths, vars);
#ifdef DEBUG
		if (has_further_simp) {
			LOG_DEBUG << "Before factoring simplification: "
				<< LOG_FM(build_reduced_formula<node>(
					paths, vars, false, true));
			LOG_DEBUG << "After factoring simplification: "
				<< LOG_FM(simp_fm);
		}
#endif // DEBUG
		auto further_simp = apply_eqs_across_clauses<node>(simp_fm);
		// std::cout << "further_simp: " << further_simp << "\n";
		if (further_simp != simp_fm) has_further_simp = true;
		simp_fm = further_simp;
		if (!has_further_simp) {
			assert(simp_fm != nullptr);
			auto res = to_cnf ? push_negation_in<node>(
				tau::build_wff_neg(simp_fm)) : simp_fm;
#ifdef TAU_CACHE
			return cache.emplace(std::make_pair(
				squeezed_fm, to_cnf), res).first->second;
#endif // TAU_CACHE
			LOG_DEBUG << "End reduce_across_bfs";
			LOG_DEBUG << "res: " << LOG_FM(res);
			return res;
		}
		auto [simp_paths, simp_vars]
			= dnf_cnf_to_bdd<node>(simp_fm, tau::wff);
		paths = std::move(simp_paths);
		vars = std::move(simp_vars);
	}
}

template <NodeType node>
tref wff_reduce_dnf<node>::operator() (tref fm) const {
	return reduce_depreciated<node>(fm, tree<node>::wff);
}

template <NodeType node>
tref wff_reduce_cnf<node>::operator() (tref fm) const {
	return reduce_depreciated<node>(fm, tree<node>::wff, true);
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
		tref res = tau::_0();
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
		tref res = tau::_1();
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
		tref c = t[0].first();
		const auto& ct = tau::get(c);
		if (ct.child_is(tau::wff_and))
			return tau::build_wff_or(
				tau::build_wff_neg(ct[0].first()),
				tau::build_wff_neg(ct[0].second()));
		if (ct.child_is(tau::wff_or))
			return tau::build_wff_and(
				tau::build_wff_neg(ct[0].first()),
				tau::build_wff_neg(ct[0].second()));
		if (ct.child_is(tau::bf_eq))
			return tau::build_bf_neq(ct[0].first(), ct[0].second());
		if (ct.child_is(tau::bf_neq))
			return tau::build_bf_eq(ct[0].first(), ct[0].second());
		if (ct.child_is(tau::wff_ex))
			return tau::build_wff_all(ct[0].first(),
				tau::build_wff_neg(ct[0].second()));
		if (ct.child_is(tau::wff_all))
			return tau::build_wff_ex(ct[0].first(),
				tau::build_wff_neg(ct[0].second()));
		if (ct.child_is(tau::wff_always))
			return tau::build_wff_sometimes(
				tau::build_wff_neg(ct[0].first()));
		if (ct.child_is(tau::wff_sometimes))
			return tau::build_wff_always(
				tau::build_wff_neg(ct[0].first()));
	}
	// Boolean function rules
	if constexpr (!is_wff) if (t.child_is(tau::bf_neg)) {
		const auto& ct = t[0][0];
		if (ct.child_is(tau::bf_and))
			return tau::build_bf_or(
				tau::build_bf_neg(ct[0].first()),
				tau::build_bf_neg(ct[0].second()));
		if (ct.child_is(tau::bf_or)) {
			return tau::build_bf_and(
				tau::build_bf_neg(ct[0].first()),
				tau::build_bf_neg(ct[0].second()));
		}
		if (ct.child_is(tau::bf_xor)) {
			// TODO: strategy for negating first or second argument
			return tau::build_bf_xor(tau::build_bf_neg(ct[0].first()), ct[0].second());
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
					return reduce_depreciated<node>(conj, tau::bf);
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

template <NodeType node>
tref single_dnf_lift(tref fm) {
	using tau = tree<node>;
	auto layer_to_dnf = [](tref n) {
		const auto& t = tau::get(n);
		if (t.child_is(tau::wff_and)) {
			// If left child is a disjunction
			if (t[0][0].child_is(tau::wff_or)) {
				const auto& c = t[0][0];
				return tau::build_wff_or(
					tau::build_wff_and(c[0].first(),
								t[0].second()),
					tau::build_wff_and(c[0].second(),
								t[0].second()));
			}
			// If right child is a disjunction
			if (t[0][1].child_is(tau::wff_or)) {
				const auto& c = t[0][1];
				return tau::build_wff_or(
					tau::build_wff_and(t[0].first(),
								c[0].first()),
					tau::build_wff_and(t[0].first(),
								c[0].second()));
			}        
		}
		return n;
	};
	bool found_or = false;
	auto decend = [&found_or](tref n) {
		if (found_or) return false;
		const auto& t = tau::get(n);
		if (t.is(tau::wff_or)) return found_or = true, false;
		if (t.is(tau::bf)) return false;
		return true;
	};
	return post_order<node>(fm).apply_unique(layer_to_dnf, decend);
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
					return reduce_depreciated<node>(dis, tau::bf, true);
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

// Assumes that fm is a single DNF always clause
template <NodeType node>
tref rm_temporary_lookback(tref fm) {
	using tau = tree<node>;
	const auto& t = tau::get(fm);
	trefs io_vars = t.select_top(is_child<node, tau::io_var>);
	bool has_var = std::ranges::any_of(io_vars,
		[](tref el){ return !is_io_initial<node>(el); });
	int_t lookback = get_max_shift<node>(io_vars, true);
	std::map<tref, tref> changes;
	tref max_temp = nullptr;
	for (tref io_var : io_vars) {
		auto n = get_var_name<node>(io_var);
		// Only eliminate lookback temporary variables
		if (n[0] == '_' && n[1] == 'l') {
			if (!has_var)
				changes.emplace(io_var, tau::_0_trimmed());
			else if (lookback >= get_io_var_shift<node>(io_var))
				changes.emplace(io_var, tau::_0_trimmed());
			else if (max_temp) {
				if (get_io_var_shift<node>(max_temp)
					< get_io_var_shift<node>(io_var))
				{
					changes.emplace(max_temp,
							tau::_0_trimmed());
					max_temp = io_var;
				} else changes.emplace(io_var,
							tau::_0_trimmed());
			} else max_temp = io_var;
		}
	}
	return replace<node>(fm, changes);
}

// Assumes a single sometimes DNF clause with negation pushed in containing no wff_or with max nesting depth 1
template <NodeType node>
tref extract_sometimes (tref fm) {
	using tau = tree<node>;
	std::map<tref, tref> l_changes = {};
	// Collect remaining nested "sometimes" formulas
	trefs sometimes_extractions = {};
	const auto& t = tau::get(fm);
	for (tref inner_st
		: t[0][0].select_top(is_child<node, tau::wff_sometimes>))
	{
		l_changes[inner_st] = tau::_T();
		sometimes_extractions.push_back(inner_st);
	}
	// Collect remaining nested "always" formulas
	trefs always_extractions = {};
	for (tref inner_aw
		: t[0][0].select_top(is_child<node, tau::wff_always>))
	{
		l_changes[inner_aw] = tau::_T();
		always_extractions.push_back(inner_aw);
	}
	// Apply always/sometimes extractions to fm
	if (!l_changes.empty()) fm = replace<node>(fm, l_changes);

	tref extracted_fm = tau::_T();
	for (tref se : sometimes_extractions)
		extracted_fm = tau::build_wff_and(extracted_fm, se);
	for (tref ae : always_extractions)
		extracted_fm = tau::build_wff_and(extracted_fm, ae);

	const auto& t2 = tau::get(fm);
	// Check if everything under sometimes was removed
	if (t2.equals_T()) return extracted_fm;
	if (t2.equals_F()) return tau::_F();

	trefs extracted = {}, staying = {};
	auto clauses = get_leaves<node>(t2[0].first(), tau::wff_and);
	for (tref clause : clauses) {
		DBG(assert(!is<node>(tau::trim(clause),tau::wff_sometimes)
			&& !is<node>(tau::trim(clause),tau::wff_always));)
		if (!has_temp_var<node>(clause)) extracted.push_back(clause);
		else staying.push_back(clause);
	}

	// From here we build the formula which we will return
	for (tref e : extracted)
		extracted_fm = tau::build_wff_and(extracted_fm, e);

	if (staying.empty()) return extracted_fm;
	tref staying_fm = tau::_T();
	for (tref s : staying) staying_fm = tau::build_wff_and(staying_fm, s);

	if (tau::get(extracted_fm).equals_T())
		return tau::build_wff_sometimes(staying_fm);
	return tau::build_wff_and(tau::build_wff_sometimes(staying_fm),
				  extracted_fm);
}

// Assumes a single DNF always clause in cnf with negation pushed in containing no wff_and with max nesting depth 1
template <NodeType node>
tref extract_always (tref fm) {
	using tau = tree<node>;
	subtree_map<node, tref> l_changes = {};
	// Collect remaining nested "sometimes" formulas
	trefs sometimes_extractions = {};
	const auto& t = tau::get(fm);
	for (tref inner_st : t[0][0]
		.select_top(is_child<node, tau::wff_sometimes>))
	{
		l_changes[inner_st] = tau::_F();
		sometimes_extractions.push_back(inner_st);
	}
	// Collect remaining nested "always" formulas
	trefs always_extractions = {};
	for (tref inner_aw : t[0][0]
		.select_top(is_child<node, tau::wff_always>))
	{
		l_changes[inner_aw] = tau::_F();
		always_extractions.push_back(inner_aw);
	}
	// Apply always/sometimes extractions to flat_st
	if (!l_changes.empty()) fm = replace<node>(fm, l_changes);

	tref extracted_fm = tau::_F();
	for (tref se : sometimes_extractions)
		extracted_fm = tau::build_wff_or(extracted_fm, se);
	for (tref ae : always_extractions)
		extracted_fm = tau::build_wff_or(extracted_fm, ae);

	// Check if everything under fm was removed
	if (tau::get(fm).equals_F()) return extracted_fm;
	if (tau::get(fm).equals_T()) return tau::_T();

	// Now extract from all disjuncts
	trefs extracted = {}, staying = {};
	const auto& t2 = tau::get(fm);
	auto clauses = t2[0][0].get_leaves(tau::wff_or);
	for (const auto& clause : clauses) {
		DBG(assert(!is<node>(tau::trim(clause), tau::wff_sometimes)
			&& !is<node>(tau::trim(clause), tau::wff_always));)
		if (!has_temp_var<node>(clause)) extracted.push_back(clause);
		else staying.push_back(clause);
	}

	// From here we build the formula to return based on the extractions
	for (tref e : extracted)
		extracted_fm = tau::build_wff_or(extracted_fm, e);

	if (staying.empty()) return extracted_fm;
	tref staying_fm = tau::_F();
	for (tref s : staying) staying_fm = tau::build_wff_or(staying_fm, s);

	if (tau::get(extracted_fm).equals_F())
		return tau::build_wff_always(staying_fm);
	return tau::build_wff_or(tau::build_wff_always(staying_fm),
				 extracted_fm);
}

// Recursively extract non-dependend formulas under sometimes
template <NodeType node>
tref push_sometimes_always_in(tref fm, auto& visited) {
	using tau = tree<node>;
	subtree_map<node, tref> g_changes = {};
	for (tref st : tau::get(fm).select_top_until(
		is_child<node, tau::wff_sometimes>,
		is_child<node, tau::wff_always>))
	{
		// Recursively denest sometimes and always statements contained in sometimes statement st
		auto flat_st = push_sometimes_always_in<node>(
					tau::get(st)[0].first(), visited);
		// Simplyfy current formula and convert to DNF
		// Reductions done in order to prevent blow up
		flat_st = tau::build_wff_sometimes(to_dnf(flat_st));
		flat_st = push_temp_in<node>(
					flat_st, tau::wff_sometimes, visited);
		flat_st = reduce_depreciated<node>(flat_st, tau::wff);
		if (flat_st != st) g_changes[st] = flat_st;
	}
	// Apply changes
	if (!g_changes.empty()) {
		fm = replace<node>(fm, g_changes);
		g_changes = {};
	}
	for (tref st : tau::get(fm).select_top_until(
		is_child<node, tau::wff_sometimes>,
		is_child<node, tau::wff_always>))
	{
		// Here a formula under "sometimes" is of the form (phi_1 && ... && phi_n)
		// Pull out each phi_i that does not contain a time variable or is "sometimes/always"
		auto simp_st = extract_sometimes<node>(st);
		if (st != simp_st) g_changes[st] = simp_st;
	}
	// Apply changes
	if (!g_changes.empty()) {
		fm = replace<node>(fm, g_changes);
		g_changes = {};
	}
	for (tref aw : tau::get(fm).select_top_until(
		is_child<node, tau::wff_always>,
		is_child<node, tau::wff_sometimes>))
	{
		// Recursively denest sometimes and always statements contained in always statement aw
		auto flat_aw = push_sometimes_always_in<node>(
					tau::get(aw)[0].first(), visited);
		// std::cout << "After push_sometimes_always_in: " << flat_aw << "\n";
		// Simplyfy current formula and convert to CNF
		// Reductions done in order to prevent blow up
		flat_aw = tau::build_wff_always(to_cnf<node>(flat_aw));
		// std::cout << "After to_cnf2: " << flat_aw << "\n";
		flat_aw = push_temp_in<node>(flat_aw, tau::wff_always, visited);
		// std::cout << "After push_temp_in: " << flat_aw << "\n";
		flat_aw = reduce_depreciated<node>(flat_aw, tau::wff, true);
		// std::cout << "After reduce2: " << flat_aw << "\n";
		if (flat_aw != aw) g_changes[aw] = flat_aw;
	}
	// Apply changes
	if (!g_changes.empty()) {
		fm = replace<node>(fm, g_changes);
		g_changes = {};
	}
	for (tref aw : tau::get(fm).select_top_until(
		is_child<node, tau::wff_always, node>,
		is_child<node, tau::wff_sometimes, node>))
	{
		// Here the formula under "always" is of the form (phi_1 || ... || phi_n)
		// Pull out each phi_i that does not contain a time variable or is "sometimes/always"
		// std::cout << "simp_aw: " << aw << "\n";
		auto simp_aw = extract_always<node>(aw);
		if (aw != simp_aw) g_changes[aw] = simp_aw;
	}
	// Apply changes and return
	if (!g_changes.empty()) fm = replace<node>(fm, g_changes);
	return fm;
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

// Assumes a single DNF clause and normalizes the "always" parts into one
template <NodeType node>
tref pull_always_out(tref fm) {
	using tau = tree<node>;
	subtree_map<node, tref> l_changes = {};
	std::vector<tref> collected_always_fms;
	// Collect all always statments in the clause fm
	// by analyzing conjuncts
	auto clauses = tau::get(fm).get_leaves(tau::wff_and);
	for (tref clause : clauses) {
		// if clause is a sometimes statement -> skip
		if (is_child<node>(clause, tau::wff_sometimes))
			continue;
		if (is_child<node>(clause, tau::wff_always)) {
			l_changes[clause] = tau::_T();
			collected_always_fms.push_back(tau::trim2(clause));
		}
		else {
			// In case there is no temporal quantifier in clause
			l_changes[clause] = tau::_T();
			collected_always_fms.push_back(clause);
		}
	}
	if (collected_always_fms.empty()) return fm;
	// Rebuild formula based on the extraction
	tref always_part = nullptr;
	int_t lookback = 0;
	bool first = true;
	for (tref& fa : collected_always_fms) {
		auto io_vars = tau::get(fa)
				.select_top(is_child<node, tau::io_var>);
		auto current_lb = get_max_shift<node>(io_vars);
		if (first) first = false, always_part = fa;
		else {
			// Adjust lookback when joining always statements
			if (current_lb < lookback)
				fa = shift_io_vars_in_fm<node>(fa, io_vars,
							lookback - current_lb);
			if (current_lb > lookback) {
				io_vars = tau::get(always_part).select_top(
					is_child<node, tau::io_var>);
				always_part = shift_io_vars_in_fm<node>(
					always_part, io_vars,
							current_lb - lookback);
			}
			always_part = tau::build_wff_and(always_part, fa);
		}
		lookback = std::max(lookback, current_lb);
	}
	assert(!l_changes.empty());
	// Now remove all temporary lookback variables which are not needed anymore
	// after joining of the always statements
	always_part = rm_temporary_lookback<node>(always_part);
	if (!has_temp_var<node>(fm)) {
		// No input/output variable present, hence return without always
		return tau::build_wff_and(always_part,
					rewriter::replace<node>(fm, l_changes));
	}
	return tau::build_wff_and(tau::build_wff_always(always_part),
				  rewriter::replace<node>(fm, l_changes));
}

// We assume that there is no nesting of "sometimes" and "always" in fm
// and that fm is in DNF
template <NodeType node>
tref pull_sometimes_always_out(tref fm) {
	using tau = tree<node>;
	subtree_map<node, tref> changes = {};
	trefs collected_no_temp_fms;
	tref pure_always_clause = nullptr;
	// Collect all disjuncts which have temporal variables and call pull_always_out on the others
	auto clauses = tau::get(fm).get_leaves(tau::wff_or);
	if (clauses.empty()) clauses.push_back(fm);
	for (tref clause : clauses) {
        auto r = pull_always_out<node>(clause);
        if (!has_temp_var<node>(r)) {
        	changes[clause] = tau::_F();
        	collected_no_temp_fms.push_back(r);
        }
        else if (clause != r) {
        	if (is_child<node>(r, tau::wff_always))
        		pure_always_clause = tau::trim2(r);
	        changes[clause] = r;
        } else if (is_child<node>(clause, tau::wff_always))
        	pure_always_clause = tau::trim2(clause);
	}
	if (!changes.empty()) fm = rewriter::replace<node>(fm, changes);
	if (!collected_no_temp_fms.empty()) {
		tref no_temp_fm = nullptr;
		bool first = true;
		for (tref f : collected_no_temp_fms) {
			if (first) first = false, no_temp_fm = f;
			else no_temp_fm = tau::build_wff_or(no_temp_fm, f);
		}
		if (pure_always_clause) {
			changes = {};
			changes[pure_always_clause] = tau::build_wff_or(
						pure_always_clause, no_temp_fm);
			return rewriter::replace<node>(fm, changes);
		}
		fm = tau::build_wff_or(fm, no_temp_fm);
	}
	return fm;
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
tref sometimes_always_normalization<node>::operator()(tref fm) const {
	using tau = tree<node>;
	auto st_aw = [](const auto& n) {
		return is_child<node>(n, tau::wff_sometimes)
			|| is_child<node>(n, tau::wff_always);
	};
	if (tau::get(fm).find_top(st_aw) == nullptr
		&& !has_temp_var<node>(fm))
			return reduce_across_bfs<node>(fm, false);
	// Delete all always/sometimes if they scope no temporal variable
	auto temps = tau::get(fm).select_top(st_aw);
	subtree_map<node, tref> changes;
	for (tref temp : temps) {
		if (!has_temp_var<node>(temp))
			changes.emplace(temp, tau::trim2(temp));
	}
	auto clauses = get_dnf_wff_clauses<node>(
		reduce_across_bfs<node>(changes.empty() ? fm
				: rewriter::replace<node>(fm, changes), false));
	tref res = tau::_F();
	tref always_disjuncts = tau::_F();
	for (tref clause : clauses) {
		// If clause does not contain sometimes/always but temporal variables, we add it to always_disjuncts
		if (!tau::get(clause).find_top(st_aw)) {
			always_disjuncts = tau::build_wff_or(
					always_disjuncts, clause);
			continue;
		}
		auto conjuncts = get_cnf_wff_clauses<node>(clause);
		tref always_part = tau::_T();
		tref staying = tau::_T();
		for (tref conj : conjuncts) {
			if (!st_aw(conj))
				always_part = always_conjunction<node>(
							always_part, conj);
			else if (is_child<node>(conj, tau::wff_always))
				always_part = always_conjunction<node>(
							always_part, conj);
			else staying = tau::build_wff_and(staying, conj);
		}
		always_part = tau::build_wff_always(always_part);
		res = tau::build_wff_or(res, tau::build_wff_and(
							always_part, staying));
	}
	res = tau::build_wff_or(tau::build_wff_always(always_disjuncts), res);
	auto temp_inner = tau::get(res).select_top(st_aw);
	if (temp_inner.empty()) return res;
	changes.clear();
	for (tref f : temp_inner) {
		// Reduction done to normalize again now that sometimes/always are all the way out
		changes[tau::trim2(f)] = reduce_across_bfs<node>(
							tau::trim2(f), false);
	}
	return reduce_across_bfs<node>(
		rewriter::replace<node>(res, changes), false);
}

// Assumes that fm is normalized
template <NodeType node>
tref pull_always_out_for_inf(tref fm) {
	using tau = tree<node>;
	// Get all always statments on each clause and merge
	auto clauses = get_dnf_wff_clauses<node>(fm);
	tref res = tau::_F();
	tref non_temps = tau::_F();
	tref last_always = nullptr;
	for (tref clause : clauses) {
		// Clauses not containing temporal variables need to be added under always
		if (!has_temp_var<node>(clause)) {
			non_temps = tau::build_wff_or(non_temps, clause);
			continue;
		}
		auto conjuncts = get_cnf_wff_clauses<node>(clause);
		tref always_statements = tau::_T();
		tref staying = tau::_T();
		for (tref conj : conjuncts) {
			if (is_child<node>(conj, tau::wff_always))
				always_statements = tau::build_wff_and(
					always_statements, tau::trim2(conj));
			else if (!is_child<node>(conj, tau::wff_sometimes))
				always_statements = tau::build_wff_and(
					always_statements, conj);
			else staying = tau::build_wff_and(staying, conj);
		}
		// Add the non temporal clauses under always
		last_always = reduce_across_bfs<node>(always_statements, false);
		always_statements = tau::build_wff_always(last_always);
		res = tau::build_wff_or(res, tau::build_wff_and(
					always_statements, staying));
	}
	if (last_always) {
		res = replace<node>(res, last_always, tau::build_wff_or(
					last_always, non_temps));
	} else {
		assert(tau::get(res).equals_F());
		res = tau::build_wff_always(non_temps);
	}
	return reduce_across_bfs<node>(res, false);
}

template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& fm,
	const sometimes_always_normalization<node>& r)
{
	return typename tree<node>::traverser(r(fm.value()));
}

template <NodeType node>
tref push_existential_quantifier_one(tref fm) {
	using tau = tree<node>;
	LOG_DEBUG << "push_existential_quantifier_one: " << LOG_FM_DUMP(fm);
	const auto& t = tau::get(fm);
	DBG(assert(t.child_is(tau::wff_ex));)
	const tref scoped_fm = t[0].second();
	const tref quant_var = t[0].first();

	const auto& st = tau::get(scoped_fm);
	if (st.child_is(tau::wff_or)) {
		// Push quantifier in
		tref c0 = tau::build_wff_ex(quant_var, st[0].first());
		tref c1 = tau::build_wff_ex(quant_var, st[0].second());
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
		if (tau::get(q_fm).equals_T()) return scoped_fm;
		else if (tau::get(no_q_fm).equals_T()) return fm;
		else return tau::build_wff_and(
			tau::build_wff_ex(quant_var, q_fm), no_q_fm);
	}
	else if (st.child_is(tau::wff_ex)) {
		//other ex quant, hence can switch them
		tref c = tau::build_wff_ex(quant_var, st[0].second());
		return tau::build_wff_ex(st[0].first(), c);
	}
	// Else check if quant_var is contained in subtree
	else if (contains<node>(scoped_fm, quant_var)) return fm;
	else return scoped_fm;
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
		const auto c0 = tau::build_wff_all(quant_var, st[0].first());
		const auto c1 = tau::build_wff_all(quant_var, st[0].second());
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
			tau::build_wff_all(quant_var, q_fm), no_q_fm);
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
// Only use on equal types!!!
template <NodeType node>
tref squeeze_positives(tref n) {
	using tau = tree<node>;
	using tt = tau::traverser;
	LOG_TRACE << "squeeze_positives: " << LOG_FM(n);
	if (trefs eqs = tau::get(n).select_top(is<node, tau::bf_eq>);
		eqs.size() > 0)
	{
		for (tref& eq : eqs)
			eq = norm_trimmed_equation<node>(eq);
		eqs = tt(eqs) | tt::children | tt::refs;
		tref res = tau::build_bf_or(eqs);
		LOG_TRACE << "squeeze_positives result: " << LOG_FM(res);
		return res;
	}
	LOG_TRACE << "(I) squeeze_positives result: none";
	return nullptr;
}

template <NodeType node>
tref wff_remove_existential(tref var, tref wff) {
	using tau = tree<node>;
	using tt = tau::traverser;
	// Following Corollary 2.3 from Taba book from Ohad
	std::map<tref, tref> changes;
	for (tref l : get_leaves<node>(wff, tau::wff_or)) {
		// if var does not appear in the clause, we can skip it
		if (!contains<node>(l, var)) continue;
		// Get each conjunct in clause
		tref nl = tau::_T();
		bool is_quant_removable_in_clause = true;
		auto conjs = get_cnf_wff_clauses<node>(l);
		for (tref& conj : conjs) {
			if (!contains<node>(conj, var)) {
				nl = tau::build_wff_and(nl, conj);
				conj = tau::_T();
				continue;
			}
			// Check if conjunct is of form = 0 or != 0
			if ((tt(conj) | tau::bf_eq) || (tt(conj) | tau::bf_neq))
				continue;
			// If the conjunct contains the quantified variable at this point
			// we cannot resolve the quantifier in this clause
			is_quant_removable_in_clause = false;
			break;
		}
		tref new_l = tau::build_wff_and(conjs);
		if (!is_quant_removable_in_clause) {
			// Since we cannot remove the quantifier in this
			// clause it needs to be maintained
			changes[l] = tau::build_wff_and(
				tau::build_wff_ex(var, new_l), nl);
			continue;
		}

		tref f = squeeze_positives<node>(new_l);
		tref f_0 = f ? rewriter::replace<node>(
				f, var, tau::_0_trimmed())
			: tau::_0();
		tref f_1 = f ? rewriter::replace<node>(
				f, var, tau::_1_trimmed())
			: tau::_0();

		if (auto neqs = tau::get(new_l).select_all(
			is<node, tau::bf_neq>); neqs.size() > 0)
		{
			auto nneqs = tau::_T();
			for (tref neq : neqs) {
				auto g = tt(neq) | tau::bf | tt::ref;
				auto g_0 = rewriter::replace<node>(g, var,
							tau::_0_trimmed());
				auto g_1 = rewriter::replace<node>(g, var,
							tau::_1_trimmed());
				// If both are 1 then inequality is implied by f_0f_1 = 0
				if (       tau::get(g_0).equals_1()
					&& tau::get(g_1).equals_1()) continue;
				// If f_0 is equal to f_1 we can use assumption f_0 = 0 and f_1 = 0
				if (tau::get(f_0) == tau::get(f_1)) {
					nneqs = tau::build_wff_and(nneqs,
						tau::build_bf_neq_0(
							tau::build_bf_or(g_0,
									 g_1)));
				} else if (tau::get(g_0) == tau::get(g_1)) {
					nneqs = tau::build_wff_and(nneqs,
						tau::build_bf_neq_0(g_0));
				} else nneqs = tau::build_wff_and(nneqs,
					tau::build_bf_neq_0(tau::build_bf_or(
						tau::build_bf_and(
							tau::build_bf_neg(f_1),
							g_1),
						tau::build_bf_and(
							tau::build_bf_neg(f_0),
							g_0))));
			}
			nl = tau::build_wff_and(nl, tau::build_wff_and(
				tau::build_bf_eq_0(tau::build_bf_and(f_0, f_1)),
				nneqs));
		} else if (f) {
			nl = tau::build_wff_and(nl, tau::build_bf_eq_0(
				tau::build_bf_and(f_0, f_1)));
		}
		changes[l] = nl;
	}
	return rewriter::replace<node>(wff, changes);
}

template <NodeType node>
tref eliminate_existential_quantifier(tref inner_fm, tref scoped_fm) {
	using tau = tree<node>;
	// Reductions to prevent blow ups and achieve DNF
	LOG_DEBUG << "Start existential quantifier elimination";
	LOG_DEBUG << "Quantified variable: " << LOG_FM(tau::trim2(inner_fm));
	LOG_DEBUG << "Quantified formula: " << LOG_FM(scoped_fm);
	// scoped_fm = scoped_fm | bf_reduce_canonical<node>();
	scoped_fm = reduce_across_bfs<node>(scoped_fm, false);

#ifdef TAU_CACHE
	using cache_t = std::map<std::pair<tref, tref>, tref,
				subtree_pair_less<node, tref>>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(std::make_pair(inner_fm, scoped_fm));
		it != end(cache)) return it->second;
#endif // TAU_CACHE

	auto clauses = get_leaves<node>(scoped_fm, tau::wff_or);
	tref res = nullptr;
	for (tref clause : clauses) {
		// Check if every conjunct in clause is of form f = 0 or f != 0
		trefs conjuncts = get_leaves<node>(clause, tau::wff_and);
		bool all_equal_zero = true, all_unequal_zero = true;
		for (tref c : conjuncts) {
			if (!is_child<node>(c, tau::bf_eq))
				all_equal_zero = false;
			if (!is_child<node>(c, tau::bf_neq))
				all_unequal_zero = false;
		}
		if (all_unequal_zero) {
			tref new_conjunct = nullptr;
			// Push quantifier inside conjunction
			for (tref c : conjuncts) {
				auto new_c = wff_remove_existential<node>(
					tau::trim2(inner_fm), c);
				if (new_conjunct) new_conjunct =
					tau::build_wff_and(new_conjunct, new_c);
				else new_conjunct = new_c;
			}
			if (tau::get(new_conjunct).equals_T()) return tau::_T();
			if (res) res = tau::build_wff_or(res, new_conjunct);
			else res = new_conjunct;
		}
		else if (all_equal_zero) {
			std::map<size_t, tref> new_funcs;
			for (tref d : conjuncts) {
				size_t type_raw = find_ba_type<node>(d);
				size_t type = type_raw == 0
							? 0
							: get_ba_type_id<node>(
								node::nso_factory::instance()
								.default_type());
				if (auto it = new_funcs.find(type); it != new_funcs.end())
					it->second = tau::build_bf_or(it->second, tau::trim2(d));
				else new_funcs.emplace(type, tau::trim2(d));
			}
			tref new_fm = nullptr;
			for (tref func: new_funcs | std::views::values) {
				auto reduced_eq = tau::build_bf_eq_0(bf_reduce_canonical<node>()(func));
				if (new_fm) new_fm = tau::build_wff_and(new_fm, reduced_eq);
				else new_fm = reduced_eq;
			}
			new_fm = wff_remove_existential<node>(
				tau::trim2(inner_fm), new_fm);
			if (tau::get(new_fm).equals_T()) return tau::_T();
			if (res) res = tau::build_wff_or(res, new_fm);
			else res = new_fm;
		}
		else {
			// Resolve quantified variable in scoped_fm
			auto rem_clause = wff_remove_existential<node>(
				tau::trim2(inner_fm), clause);
			if (tau::get(rem_clause).equals_T()) return tau::_T();
			if (res) res = tau::build_wff_or(res, rem_clause);
			else res = rem_clause;
		}
	}
	// Simplify elimination result
	res = reduce_across_bfs<node>(res, false);
	LOG_DEBUG << "End existential quantifier elimination";
	LOG_DEBUG << "res: " << LOG_FM(res);
#ifdef TAU_CACHE
	return cache.emplace(std::make_pair(inner_fm, scoped_fm),
			     res).first->second;
#endif // TAU_CACHE
	return res;
}

template <NodeType node>
tref eliminate_universal_quantifier(tref inner_fm, tref scoped_fm) {
	using tau = tree<node>;
	LOG_DEBUG << "Start universal quantifier elimination";
	LOG_DEBUG << "Quantified variable: " << LOG_FM(tau::trim2(inner_fm));
	LOG_DEBUG << "Quantified formula: " << LOG_FM(scoped_fm);
	// Reductions to prevent blow ups and achieve CNF
	// scoped_fm = scoped_fm | bf_reduce_canonical<BAs...>();
	scoped_fm = reduce_across_bfs<node>(scoped_fm, true);
// Add cache after reductions; reductions are cached as well
#ifdef TAU_CACHE
	using cache_t = std::map<std::pair<tref, tref>, tref,
				subtree_pair_less<node, tref>>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(std::make_pair(inner_fm, scoped_fm));
		it != end(cache)) return it->second;
#endif // TAU_CACHE

	auto clauses = get_leaves<node>(scoped_fm, tau::wff_and);
	tref res = nullptr;
	for (tref clause : clauses) {
		LOG_DEBUG << "eliminate_universal_quantifier / clause: "
							<< LOG_FM(clause);
		// Check if every disjunct in clause is of form f = 0 or f != 0
		auto disjuncts = get_leaves<node>(clause, tau::wff_or);
		bool all_equal_zero = true, all_unequal_zero = true;
		for (tref d : disjuncts) {
			if (!is_child<node>(d, tau::bf_eq))
				all_equal_zero = false;
			if (!is_child<node>(d, tau::bf_neq))
				all_unequal_zero = false;
		}
		if (all_equal_zero) {
			tref new_disjunct = nullptr;
			// Push quantifier inside disjunction
			for (tref d : disjuncts) {
				auto new_d = push_negation_in<node>(
							tau::build_wff_neg(d));
				new_d = push_negation_in<node>(
					tau::build_wff_neg(
						wff_remove_existential<node>(
							tau::trim2(inner_fm),
							new_d)));
				if (new_disjunct) new_disjunct =
					tau::build_wff_or(new_disjunct, new_d);
				else new_disjunct = new_d;
			}
			if (tau::get(new_disjunct).equals_F()) return tau::_F();
			if (res) res = tau::build_wff_and(res, new_disjunct);
			else res = new_disjunct;
		}
		if (all_unequal_zero) {
			std::map<size_t, tref> new_funcs;
			for (tref d : disjuncts) {
				size_t type_raw = find_ba_type<node>(d);
				size_t type = type_raw == 0
							? 0
							: get_ba_type_id<node>(
								node::nso_factory::instance()
								.default_type());
				if (auto it = new_funcs.find(type); it != new_funcs.end())
					it->second = tau::build_bf_or(it->second, tau::trim2(d));
				else new_funcs.emplace(type, tau::trim2(d));
			}
			tref new_fm = nullptr;
			for (tref func : new_funcs | std::views::values) {
				tref reduced_eq = tau::build_bf_eq_0(bf_reduce_canonical<node>()(func));
				if (new_fm) new_fm = tau::build_wff_or(new_fm, reduced_eq);
				else new_fm = reduced_eq;
			}
			new_fm = push_negation_in<node>(
				tau::build_wff_neg(wff_remove_existential<node>(
					tau::trim2(inner_fm), new_fm)));
			if (tau::get(new_fm).equals_F()) return tau::_F();
			if (res) res = tau::build_wff_and(res, new_fm);
			else res = new_fm;
		} else {
			// Turn universal into existential quantifier and eliminate
			auto new_clause = push_negation_in<node>(
					tau::build_wff_neg(clause));
			new_clause = push_negation_in<node>(
				tau::build_wff_neg(wff_remove_existential<node>(
					tau::trim2(inner_fm), new_clause)));
			if (tau::get(new_clause).equals_F()) return tau::_F();
			if (!res) res = new_clause;
			else res = tau::build_wff_and(res, new_clause);
		}
	}
	// Simplify elimination result
	res = reduce_across_bfs<node>(res, true);
	LOG_DEBUG << "End universal quantifier elimination";
	LOG_DEBUG << "res: " << LOG_FM(res);
#ifdef TAU_CACHE
	return cache.emplace(std::make_pair(inner_fm, scoped_fm),
			     res).first->second;
#endif // TAU_CACHE
	return res;
}

// Pushes all universal and existential quantifiers as deep as possible into the formula
// and then eliminate them, returning a quantifier free formula
template <NodeType node>
tref eliminate_quantifiers(tref fm) {
	using tau = tree<node>;
	LOG_DEBUG << "eliminate_quantifiers: " << LOG_FM_DUMP(fm);
	// Lambda is applied to nodes of fm in post order after quantifiers have
	// been pushed in
	auto elim_quant = [](tref inner_fm) -> tref {
		// Find out if current node is a quantifier
		LOG_DEBUG << "elim_quant inner_fm: " << LOG_FM(inner_fm);
		bool is_ex_quant;
		if (is_child<node>(inner_fm, tau::wff_ex))
			is_ex_quant = true;
		else if (is_child<node>(inner_fm, tau::wff_all))
			is_ex_quant = false;
		else return inner_fm;

		auto has_var = [&inner_fm](tref n) {
			return tau::get(n) == tau::get(tau::trim2(inner_fm)); };
		tref scoped_fm = tau::get(inner_fm)[0].second();
		if (scoped_fm != nullptr) {
			LOG_DEBUG <<"elim_quant scoped_fm: "<<LOG_FM(scoped_fm);
		}
		if (!tau::get(scoped_fm).find_top(has_var)) {
			// If the scoped formula does not contain
			// the quantified variable, remove the quantifier
			return scoped_fm;
		}
		// Scoped formula contains the quantified variable
		if (is_ex_quant)
			return eliminate_existential_quantifier<node>(
							inner_fm, scoped_fm);
		else    return eliminate_universal_quantifier<node>(
							inner_fm, scoped_fm);
	};
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
		if (is<node>(n, tau::bf)) return false;
		// Do not visit subtrees below a maximally pushed quantifier
		if (excluded_nodes.contains(n)) return false;
		return true;
	};
	// Push quantifiers in during pre-order traversal
	// and eliminate quantifiers during the traversal back up (post-order)
	auto push_and_elim = [&elim_quant, &push_quantifiers, visit](tref n) {
		if (is_child_quantifier<node>(n))
			return pre_order<node>(n).template apply_unique<
				MemorySlotPre::eliminate_quantifiers_m>(
					push_quantifiers, visit, elim_quant);
		else return n;
	};
	return post_order<node>(fm).apply_unique(push_and_elim, visit_wff<node>);
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
	// Return early if atomic_fm is either T or F
	if (tau::get(atomic_fm).equals_T() || tau::get(atomic_fm).equals_F())
		return atomic_fm;
	DBG(LOG_TRACE << "Syntactic_variable_simplification on " << LOG_FM(atomic_fm) << "\n";)
	DBG(LOG_TRACE << "with var: " << LOG_FM(var) << "\n";)
	var = tau::get(tau::bf, var);
	tref func = tau::trim2(norm_equation<node>(atomic_fm));
	// Make sure that it works only on Boolean parts
	tref func_v_0 = rewriter::replace_if<node>(func, var, _0<node>(), is_boolean_operation<node>);
	tref func_v_1 = rewriter::replace_if<node>(func, var, _1<node>(), is_boolean_operation<node>);
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
			res = rewriter::replace<node>(atomic_fm, var, _0<node>());
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
			res = rewriter::replace<node>(atomic_fm, var, _0<node>());
	}
	DBG(LOG_TRACE << "Syntactic_variable_simplification result: " << LOG_FM(res) << "\n";)
	return res;
}

/**
 * @brief Simplify using assumptions from equalities in formula. More effective
 * if terms/bfs are in a normal form
 * @tparam node Type of tree node
 */
template <NodeType node>
struct simplify_using_equality {
	using tau = tree<node>;
	// TODO: For variables, make input < output and lower time step < higher time step
	// Create comparator function that orders bfs by making constants smallest
	// We have 0 < 1 < bf_constant < uninterpreted_constant < variable < rest by node count
	static bool term_comp(tref l, tref r) {
		if (l == _0<node>()) return true;
		if (r == _0<node>()) return false;
		// 1 is automatically rewritten to 0
		// if (l == _1<node>()) return true;
		// if (r == _1<node>()) return false;
		if (is_child<node>(l, tau::bf_constant)) return true;
		if (is_child<node>(r, tau::bf_constant)) return false;
		if (is_child<node>(l, tau::variable)) {
			if (is_child<node>(r, tau::variable)) {
				// Check for uninterpreted constant
				if (is_child<node>(tau::trim(l), tau::uconst)) return true;
				if (is_child<node>(tau::trim(r), tau::uconst)) return false;
			} else return true;
		}
		if (is_child<node>(r, tau::variable)) return false;
		// TODO: maybe use free_vars instead of node_count?
		return node_count<node>(l) <= node_count<node>(r);
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
		auto uf = union_find<decltype(term_comp), node>(term_comp);
		// Create stack of union find data structures
		std::vector<union_find<decltype(term_comp), node>> uf_stack {uf};
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
		// Here we know that eq is still bf_eq
		// We both add a = b and a' = b' in order to detect syntactic contradictions
		tref left_hand_side = tau::trim2(eq);
		tref right_hand_side = tau::get(tau::trim(eq)).child(1);
		tref left_hand_side_neg =
			push_negation_in<node, false>(build_bf_neg<node>(left_hand_side));
		tref right_hand_side_neg =
			push_negation_in<node, false>(build_bf_neg<node>(right_hand_side));

		uf.merge(left_hand_side, right_hand_side);
		uf.merge(left_hand_side_neg, right_hand_side_neg);
		uf.merge(build_bf_and<node>(left_hand_side, right_hand_side_neg), _0<node>());
		uf.merge(build_bf_and<node>(right_hand_side, left_hand_side_neg), _0<node>());
		uf.merge(build_bf_or<node>(left_hand_side, right_hand_side_neg), _1<node>());
		uf.merge(build_bf_or<node>(right_hand_side, left_hand_side_neg), _1<node>());
		// Check if syntactic contradiction occurred
		return !uf.connected(left_hand_side, left_hand_side_neg)
				&& !uf.connected(right_hand_side, right_hand_side_neg)
					&& !uf.connected(_0<node>(), _1<node>());
	}

	// In case eq is of form f = 0, unsqueeze disjunctions in f
	static bool add_equality(auto& uf, tref eq) {
		if (const tau& c = tau::get(eq)[0]; c[1].equals_0()) {
			tref func = push_negation_in<node, false>(c.first());
			bool valid = true;
			for (tref path : expression_paths<node>(func)) {
				// Sort path
				trefs p = get_cnf_bf_clauses<node>(path);
				std::ranges::sort(p, tau::subtree_less);
				tref sorted_path = tau::build_bf_and(p);
				valid = valid && add_raw_equality(uf,
					tau::build_bf_eq_0(sorted_path));
			}
			return valid;
		} else return add_raw_equality(uf, eq);
	}

	// Given current equalities in union find, simplify the equation
	static tref simplify_equation (auto& uf, tref eq) {
		// For each node, check if contained in uf -> if yes, replace
		auto simp_path = [&uf](tref path) {
			// Sort path
			trefs p = get_cnf_bf_clauses<node>(path);
			std::ranges::sort(p, tau::subtree_less);
			tref sorted_path = tau::build_bf_and(p);
			auto uf_find = [&uf](tref n) {
				if (uf.contains(n)) return uf.find(n);
				return n;
			};
			tref simp_sorted_path = pre_order<node>(sorted_path).apply(uf_find);
			if (tau::get(simp_sorted_path) != tau::get(sorted_path))
				return simp_sorted_path;
			else return path;
		};
		if (tau::get(eq).equals_T() || tau::get(eq).equals_F()) return eq;
		tref c1 = expression_paths<node>(tau::get(eq)[0].first()).apply(simp_path);
		tref c2 = expression_paths<node>(tau::get(eq)[0].second()).apply(simp_path);
		return is_child<node>(eq, tau::bf_eq)
			       ? tau::build_bf_eq(c1, c2)
			       : tau::build_bf_neq(c1, c2);
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

//TODO: add caching
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
		return pre_order<node>(root).apply_unique(down, visit);
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
					assignments.emplace(tau::trim2(l), _0<node>());
				else assignments.emplace( l, _1<node>());
			}
			tref simp = rewriter::replace_if(n, assignments,
				is_boolean_operation<node>);
			// If simp is false, current branch is not sat
			if (tau::get(simp).equals_0()) {
				// Remove branch
				return _0<node>();
			}
			// Rebuild assumptions
			trefs sorted_assms (assignments.size());
			// Canonical sorting
			std::ranges::partial_sort_copy(std::views::keys(assignments),
				sorted_assms, tau::subtree_less);
			tref assms = _1<node>();
			for (tref a : sorted_assms) {
				assms = assignments[a] == _1<node>()
						? tau::build_bf_and(assms, a)
						: tau::build_bf_and(assms,
							tau::build_bf_neg(a));
			}
			// Make sure to not revisit build assumptions
			if (tau::get(simp) == get_1<node>())
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
		return pre_order<node>(root).apply_unique(down, visit);
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

/**
 * @brief The procedure collects all = 0 equalities within scope that share
 * the provided variable. These collected assumptions are squeezed together,
 * call the squeezed result A, and are then integrated into terms in scope that
 * share at least 2 variables including the provided one in the following way:
 * - Given f = 0, we produce f|A = 0
 * - Given f != 0, we produce f & A' != 0
 * @tparam node Tree node type
 * @param formula The formula to apply the procedure to
 * @param var The variable to base the procedure on
 * @return The mutated formula
 */
template<NodeType node>
tref squeeze_absorb_down(tref formula, tref var) {
	using tau = tree<node>;
	trefs assms { tau::_0() };
	subtree_unordered_set<node> mark;
	auto f = [&assms, &mark, &var](tref n, tref parent) {
		if (!tau::get(n).is(tau::wff)) return n;
		const tau& cn = tau::get(n)[0];
		if (parent != nullptr && is<node>(parent, tau::wff_or)) {
			if (!cn.is(tau::wff_or)) {
				// Push new assumption to stack
				assms.push_back(assms.back());
			}
			// Mark disjunctions whose child is not a disjunction
			else mark.insert(parent);
		}
		if (cn.is(tau::wff_and)) {
			// Squeeze = 0 if they share var
			trefs conj = get_cnf_wff_clauses<node>(n);
			tref squeezed = tau::_0();
			for (tref& c : conj) {
				const tau& c_t = tau::get(c);
				if (c_t.child_is(tau::bf_eq) &&
					c_t[0][1].equals_0()) {
					if (contains<node>(c_t[0].first(), var)) {
						squeezed = tau::build_bf_or(
							squeezed, c_t[0].first());
						c = tau::_T();
					}
				}
			}
			squeezed = tau::build_bf_eq_0(squeezed);
			return tau::build_wff_and(squeezed,
				tau::build_wff_and(conj));
		}
		if (cn.is(tau::bf_eq) && cn[1].equals_0()) {
			if (!contains<node>(cn.first(), var)) return n;
			if (is_ordered_overlap_at_least<node>(2, get_free_vars<node>(n),
				get_free_vars<node>(assms.back()))) {
				// Add assm to n
				return tau::build_bf_eq_0(
					term_boole_decomposition<node>(
						tau::build_bf_or(cn.first(),
							assms.back()), var)
					);
			}
			// Add n to assm
			assms.back() = tau::build_bf_or(
				assms.back(), cn.first());
		} else if (cn.is(tau::bf_neq) && cn[1].equals_0()) {
			if (!contains<node>(cn.first(), var)) return n;
			if (is_ordered_overlap_at_least<node>(2, get_free_vars<node>(n),
				get_free_vars<node>(assms.back()))) {
				// Add assm to n
				return tau::build_bf_neq_0(
					term_boole_decomposition<node>(
						tau::build_bf_and(cn.first(),
							tau::build_bf_neg(
								assms.back())),
									var)
					);
			}
		}
		return n;
	};
	auto up = [&assms, &mark](tref n, tref parent) {
		if (!is<node>(n, tau::wff)) return n;
		if (parent != nullptr && is<node>(parent, tau::wff_or)) {
			if (!is_child<node>(n, tau::wff_or)) {
				// If parent was marked, child was a disjunction
				// before and nothing was pushed on stack
				if (auto it = mark.find(parent); it != mark.end()) {
					mark.erase(it);
				} else assms.pop_back();
			} else {
				// Encounter of disjunction below disjunction
				// If not marked, this happened due to simplification
				// and we can safely pop
				if (auto it = mark.find(parent); it == mark.end())
					assms.pop_back();
				else mark.erase(it);
			}
		}
		return n;
	};
	tref res = pre_order<node>(formula).apply(f, visit_wff<node>, up);
	DBG(assert(assms.size() == 1);)
	return res;
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
	var = tau::get(tau::bf, var);
	tref p1 = tau::get(term).replace(var, tau::_1());
	// Ensure early detection of F
	p1 = syntactic_path_simplification<node>::unsat_on_unchanged_negations(p1);
	tref p2 = tau::get(term).replace(var, tau::_0());
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
	tref p1 = tau::get(term).replace(vars[idx], tau::_1_trimmed());
	// Ensure early detection of F
	p1 = syntactic_path_simplification<node>::unsat_on_unchanged_negations(p1);
	tref p2 = tau::get(term).replace(vars[idx], tau::_0_trimmed());
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

//TODO: Does not work for bitvector
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
	// Simple cases
	if (tau::get(term).equals_0() || tau::get(term).equals_1())
		return term;
	term = push_negation_in<node, false>(term);
	auto vars = get_free_vars_appearance_order<node>(term);
	// No free var, so no boole decomposition step
	if (vars.empty()) return normalize_ba<node>(term);
	std::ranges::stable_sort(vars, variable_order_for_simplification<node>);
	term = rec_term_boole_decomposition<node>(term, vars, 0);
	DBG(LOG_DEBUG << "Term_boole_decomposition result: " << LOG_FM(term) << "\n";)
	return term;
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
 * @param formula The formula to convert to Boole normal form
 * @return The resulting Boole normal form
 */
template<NodeType node>
tref boole_normal_form(tref formula) {
	using tau = tree<node>;
	DBG(LOG_DEBUG << "Boole_normal_form on " << LOG_FM(formula) << "\n";)
	if (tau::get(formula).equals_T() || tau::get(formula).equals_F())
		return formula;
	// Step 1: Syntactically simplify formula
	formula = syntactic_formula_simplification<node>(formula);
	DBG(LOG_DEBUG << "After syntactic_formula_simplification: " << LOG_FM(formula) << "\n";)
	// Squeeze all = 0 conjunctions together for additional simplifications during term normalization
	// TODO: Don't squeeze all but only sharing
	formula = squeeze_wff_pos<node>(formula);
	// TODO: squeeze in scope if free vars are subset
	// TODO: absorb in scope if free vars are subset
	// Step 2: Traverse formula, simplify all encountered equations
	auto simp_eqs = [](tref n) {
		if (tau::get(n).child_is(tau::bf_eq)) {
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
	formula = pre_order<node>(formula).apply_unique_until_change(simp_eqs, visit_wff<node>);
	DBG(LOG_DEBUG << "After term_boole_decomposition: " << LOG_FM(formula) << "\n";)
	// Step 3: Syntactically simplify resulting formula again after normalization of terms
	formula = syntactic_formula_simplification<node>(formula);
	DBG(LOG_DEBUG << "After syntactic_formula_simplification: " << LOG_FM(formula) << "\n";)
	// Step 4: Convert formula to Boole normal form
	//TODO: Here squeeze all before atomic formulas are collected

	// First get atomic formulas without !=
	tref eq_formula = unequal_to_not_equal<node>(formula);
	trefs atms = tau::get(eq_formula).select_top(is_child<node, tau::bf_eq>);
	// No variables for Boole decomposition
	if (atms.empty()) return formula;
	// Sort the BDD variables
	std::ranges::stable_sort(atms, atm_formula_order_for_simplification<node>);
	// Apply Boole decomposition
	eq_formula = rec_boole_decomposition<node>(eq_formula, atms, 0);
	// Convert !(=) to != again
	eq_formula = not_equal_to_unequal<node>(eq_formula);
	DBG(LOG_DEBUG << "Boole_normal_form result: " << LOG_FM(eq_formula) << "\n";)
	return eq_formula;
}

/**
 * @brief Performs a Boole decomposition step on the formula with the goal to push
 * the existential quantifier further in. Since the BDD variables are atomic formulas,
 * the procedure investigates the best Boole decomposition in each situation.
 * @tparam node Tree node type
 * @param ex_quant_fm Existentially quantified formula on which to perform Boole decomposition step
 * @param pool The pool of variables that can be used for the Boole decomposition
 * @return The resulting Boole decomposition with the existential quantifier pushed further in
 */
template<NodeType node>
tref ex_quantified_boole_decomposition(tref ex_quant_fm, auto& pool,
	auto& quant_pattern) {
	using tau = tree<node>;
	DBG(assert(!tau::get(ex_quant_fm).find_top(is<node, tau::bf_neq>)));

	// Get atomic formulas from pool
	auto it = pool.find(ex_quant_fm);
	tref curr_pool = it != pool.end()
				 ? it->second
				 : tau::get(ex_quant_fm)[0].second();
	trefs atms = tau::get(curr_pool).select_top(is_child<node, tau::bf_eq>);
	if (atms.empty()) {
		DBG(assert(false));
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
	tref func_v_0 = rewriter::replace_if<node>(func, var, tau::_0(), is_boolean_operation<node>);
	func_v_0 = syntactic_path_simplification<node>::on(func_v_0);
	tref func_v_1 = rewriter::replace_if<node>(func, var, tau::_1(), is_boolean_operation<node>);
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
		if (tau::get(l) == tau::get(r)) return tau::build_wff_ex(tau::trim(var), l);
		atm = rewriter::replace<node>(atm, var, tau::_T());
		return tau::build_wff_or(
			tau::build_wff_and(atm, tau::build_wff_ex(tau::trim(var), l)),
			tau::build_wff_and(tau::build_wff_neg(atm), tau::build_wff_ex(tau::trim(var), r))
			);
	}
	// Check has a unique zero
	func_v_1 = syntactic_path_simplification<node>::on(tau::build_bf_neg(func_v_1));
	if (tau::get(func_v_0) == tau::get(func_v_1) && !contains<node>(func_v_0, var)) {
		tref fm = tau::get(ex_quant_fm)[0].second();
		tref l = rewriter::replace<node>(fm, atm, tau::_T());
		l = rewriter::replace<node>(l, var, func_v_0);
		l = syntactic_path_simplification<node>::unsat_on_unchanged_negations(l);
		tref r = rewriter::replace<node>(fm, atm, tau::_F());
		r = syntactic_path_simplification<node>::unsat_on_unchanged_negations(r);
		if (tau::get(l) == tau::get(r)) return l;
		tref boole_atm = tau::build_bf_eq(
		term_boole_decomposition<node>(tau::get(atm)[0].first(), tau::trim(var)),
		term_boole_decomposition<node>(tau::get(atm)[0].second(), tau::trim(var))
		);
		tref nr = tau::build_wff_ex(tau::trim(var),
			tau::build_wff_and(tau::build_wff_neg(boole_atm), r));
		pool.insert_or_assign(nr,
			rewriter::replace<node>(curr_pool, atm, tau::_F()));
		atm = rewriter::replace<node>(atm, var, func_v_0);
		return tau::build_wff_or(tau::build_wff_and(atm, l), nr);
	}
	}
	// No simplification applied, build Boole decomposition
	tref fm = tau::get(ex_quant_fm)[0].second();
	tref l = rewriter::replace<node>(fm, atm, tau::_T());
	l = syntactic_path_simplification<node>::unsat_on_unchanged_negations(l);
	tref r = rewriter::replace<node>(fm, atm, tau::_F());
	r = syntactic_path_simplification<node>::unsat_on_unchanged_negations(r);
	if (tau::get(l) == tau::get(r)) return tau::build_wff_ex(tau::trim(var), l);
	tref boole_atm = tau::build_bf_eq(
		term_boole_decomposition<node>(tau::get(atm)[0].first(), tau::trim(var)),
		term_boole_decomposition<node>(tau::get(atm)[0].second(), tau::trim(var))
		);
	tref nl = tau::build_wff_ex(tau::trim(var),
		tau::build_wff_and(boole_atm, l));
	tref nr = tau::build_wff_ex(tau::trim(var),
		tau::build_wff_and(tau::build_wff_neg(boole_atm), r));
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
 * @return The resulting clause after removing the existential quantifier
 */
template <NodeType node>
tref treat_ex_quantified_clause(tref ex_clause) {
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
		// Check if conjunct is of form = or !=
		if (tau::get(conj).child_is(tau::bf_eq) ||
			tau::get(conj).child_is(tau::bf_neq))
			continue;
		// If the conjunct contains the quantified variable at this point
		// we cannot resolve the quantifier in this clause
		is_quant_removable_in_clause = false;
		break;
	}
	tref scoped_fm = tau::build_wff_and(conjs);
	if (!is_quant_removable_in_clause) {
		// Since we cannot remove the quantifier in this
		// clause it needs to be maintained
		return tau::build_wff_and(
			tau::build_wff_ex(var, scoped_fm), new_fm);
	}
	// Check that quantified variable appears
	if (tau::get(scoped_fm).equals_T()) return new_fm;
	tref f = squeeze_positives<node>(scoped_fm);
	tref f_0 = f ? rewriter::replace<node>(f, var, tau::_0_trimmed()) : tau::_0();
	// std::cout << "f_0: " << tau::get(f_0) << "\n";
	tref f_1 = f ? rewriter::replace<node>(f, var, tau::_1_trimmed()) : tau::_0();
	// std::cout << "f_1: " << tau::get(f_1) << "\n";
	trefs neqs = tau::get(scoped_fm).select_top(is<node, tau::bf_neq>);
	if (neqs.size()) {
		tref nneqs = tau::_T();
		for (tref neq : neqs) {
			// Convert to != 0
			neq = norm_trimmed_equation<node>(neq);
			// Get term
			tref g = tau::trim(neq);
			tref g_0 = rewriter::replace<node>(g, var,
						tau::_0_trimmed());
			// std::cout << "g_0: " << tau::get(g_0) << "\n";
			tref g_1 = rewriter::replace<node>(g, var,
						tau::_1_trimmed());
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
	// TODO: maybe unsqueeze? Only simp atomic formulas
	return boole_normal_form<node>(new_fm);
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
	auto anti_prenex_step = [&atomic_pool, &quant_pattern](tref n) {
		while (is_child_quantifier<node>(n)) {
			// By assumption is existential

			// TODO: if all atomic formulas are !=

			// If n is single DNF clause -> treat quantifier
			if (!tau::get(n).find_top(is<node, tau::wff_or>)) {
				DBG(LOG_TRACE << "Before treat_ex_quantified_clause: " << LOG_FM(n) << "\n";)
				n = treat_ex_quantified_clause<node>(n);
				DBG(LOG_TRACE << "After treat_ex_quantified_clause: " << LOG_FM(n) << "\n";)
				return n;
			}
			// Try push quant down
			auto pushed = push_existential_quantifier_one<node>(n);
			if (pushed != n) {
				// TODO: sort disjunctions to priorities
				DBG(LOG_TRACE << "Before push_existential_quantifier_one: " << LOG_FM(n) << "\n";)
				DBG(LOG_TRACE << "After push_existential_quantifier_one: " << LOG_FM(pushed) << "\n";)
				return pushed;
			}
			// Smart Boole decomposition
			n = unequal_to_not_equal<node>(n);
			DBG(LOG_TRACE << "Before ex_quantified_boole_decomposition: " << LOG_FM(n) << "\n";)
			n = ex_quantified_boole_decomposition<node>(n,
				atomic_pool, quant_pattern);
			n = not_equal_to_unequal<node>(n);
			DBG(LOG_TRACE << "After ex_quantified_boole_decomposition: " << LOG_FM(n) << "\n";)
		}
		return n;
	};
	auto inner_quant = [&anti_prenex_step, &quant_pattern](tref n) {
		if (!is_child_quantifier<node>(n)) return n;
		// Here child is quantifier
		DBG(LOG_TRACE << "Inner_quant on " << LOG_FM(n) << "\n";)
		n = syntactic_formula_simplification<node>(n);
		n = squeeze_absorb_down<node>(n, tau::trim2(n));
		DBG(LOG_TRACE << "After squeeze_absorb_down " << LOG_FM(n) << "\n";)
		tref res = nullptr;
		if (is_child<node>(n, tau::wff_all)) {
			tref n_neg = to_nnf<node>(tau::build_wff_neg(n));
			res = pre_order<node>(n_neg).
				apply_unique(anti_prenex_step, visit_wff<node>);
			res = to_nnf<node>(tau::build_wff_neg(res));
		} else {
			res = pre_order<node>(n).
				apply_unique(anti_prenex_step, visit_wff<node>);
		}
		// Update the quantifier pattern after elimination
		quant_pattern.insert_or_assign(tau::trim2(n), 0);
		return res;
	};
	auto visit = [&quant_pattern, &qid](tref n) {
		if (is_quantifier<node>(n))
			quant_pattern.insert_or_assign(tau::trim(n), qid++);
		return visit_wff<node>(n);
	};
	DBG(LOG_DEBUG << "Anti_prenex on " << LOG_FM(formula) << "\n";)
	// Initial simplification of formula
	formula = syntactic_formula_simplification<node>(formula);
	DBG(LOG_TRACE << "After syntactic_formula_simplification: " << LOG_FM(formula) << "\n";)
	// Apply anti prenex procedure
	formula = post_order<node>(formula).template
		apply_unique<anti_prenex_m>(inner_quant, visit);
	DBG(LOG_TRACE << "Anti_prenex result: " << LOG_FM(formula) << "\n";)
	formula = syntactic_formula_simplification<node>(formula);
	DBG(LOG_DEBUG << "Anti_prenex result after syntactic simp: " << LOG_FM(formula) << "\n";)
	return formula;
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
tref to_snf_step<node>::operator()(tref form) const {
	// we select all literals, i.e. wff equalities or it negations.
	static const auto is_literal = [](tref n) -> bool {
		return tau::get(n).child_is(tau::bf_eq);
	};
	if (auto literals = tau::get(form).select_all(is_literal);
		literals.size())
	{
		// we call the recursive method traverse to traverse all the paths
		// of the BDD.
		subtree_set<node> remaining(literals.begin(), literals.end());
		bdd_path path;
		return tt(traverse(path, remaining, form))
			| bf_reduce_canonical<node>()
			| reduce_wff_deprecated<node> | tt::ref;
	}
	return form;
}

template <NodeType node>
tref to_snf_step<node>::bdd_path_to_snf(const bdd_path& path, tref form) const {
	// we simplify the constant part of the formula
	// TODO (HIGH) fix simplification
	// auto simplified = tt(form) | simplify_snf<node>();
	return tt(tau::build_wff_and(normalize(path), form))
					| simplify_snf<node>() | tt::ref;
}

template <NodeType node>
typename to_snf_step<node>::bdd_path to_snf_step<node>::add_to_negative_path(
	const bdd_path& path, tref lit) const
{
	bdd_path npath;
	npath.first = path.first;
	auto lit_exp = get_exponent(lit);

	for (auto& [exp, negatives]: path.second)
		if (exp != lit_exp) npath.second[exp] = negatives;
	if (!path.second.contains(lit_exp))
		npath.second[lit_exp] = { lit };
	else {
		bool insert = true;
		for (auto& n : path.second.at(lit_exp)){
			// careful with the order of the statements
			if (is_less_eq_than(n, lit)) insert = false;
			if (is_less_eq_than(lit, n)) continue;
			npath.second[lit_exp].insert(n);
		}
		if (insert) npath.second[lit_exp].insert(lit);
	}
	return npath;
}

template <NodeType node>
typename to_snf_step<node>::bdd_path to_snf_step<node>::add_to_positive_path(
	const bdd_path& path, tref lit) const
{
	bdd_path npath;
	npath.second = path.second;
	auto lit_exp = get_exponent(lit);

	for (auto& [exp, positives] : path.first)
		if (exp != lit_exp) npath.first[exp] = positives;
	if (!path.first.contains(lit_exp))
		npath.first[lit_exp] = { lit };
	else {
		bool insert = true;
		for (auto& n : path.first.at(lit_exp)) {
			// careful with the order of the statements
			if (is_less_eq_than(lit, n)) insert = false;
			if (is_less_eq_than(n, lit)) continue;
			npath.first[lit_exp].insert(n);
		}
		if (insert) npath.first[lit_exp].insert(lit);
	}
	return npath;
}

template <NodeType node>
tref to_snf_step<node>::traverse(const bdd_path& path,
	const literals& remaining, tref form) const
{
	// we only cache results in release mode
#ifdef TAU_CACHE
	using cache_t = std::map<std::tuple<bdd_path, literals, tref>, tref>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find({path, remaining, form});
		it != cache.end()) return it->second;
#endif // TAU_CACHE

	if (remaining.empty()) return bdd_path_to_snf(path, form);

	auto lit = *remaining.begin();
	auto exponent = get_exponent(lit);
	tref f = tau::get(normalize_negative(path, lit)).equals_F()
		? tau::_F()
		: tt(rewriter::replace<node>(form, lit, tau::_F()))
			| simplify_snf<node>() | tt::ref;
	tref t = tau::get(normalize_positive(path, lit)).equals_F()
		? tau::_F()
		: tt(rewriter::replace<node>(form, lit, tau::_T()))
			| simplify_snf<node>() | tt::ref;

	if (tau::get(f).equals_F() && tau::get(t).equals_F()) {
		// we only cache results in release mode
#ifdef TAU_CACHE
		cache[{ path, remaining, form }] = tau::_F();
#endif // TAU_CACHE
		return tau::_F();
	}

	literals nremaining(++remaining.begin(), remaining.end());

	if (tau::get(f) == tau::get(t)) return traverse(path, nremaining, t);

	tref t_snf = tau::_F(), f_snf = tau::_F();

	if (!tau::get(f).equals_F()) {
		auto f_path = add_to_negative_path(path, lit);
		f_snf = traverse(f_path, nremaining, f);
	}

	if (!tau::get(t).equals_F()) {
		auto t_path = add_to_positive_path(path, lit);
		t_snf = traverse(t_path, nremaining, t);
	}

	if (tau::get(f_snf).equals_F()) {
#ifdef TAU_CACHE
		cache[{ path, remaining, form }] = t_snf;
#endif // TAU_CACHE
		return t_snf;
	}

	if (tau::get(t_snf).equals_F()) {
#ifdef TAU_CACHE
		cache[{ path, remaining, form }] = f_snf;
#endif // TAU_CACHE
		return f_snf;
	}

	tref result = tau::build_wff_or(t_snf, f_snf);
	// we only cache results in release mode
#ifdef TAU_CACHE
	cache[{ path, remaining, form }] = result;
#endif // TAU_CACHE
	return result;
}

template <NodeType node>
typename to_snf_step<node>::exponent to_snf_step<node>::get_exponent(
	const tref n) const
{
	auto is_bf_literal = [](tref n) -> bool {
		return (tt(n) | tau::variable).has_value()
			|| (tt(n) | tau::bf_neg | tau::bf
					| tau::variable).has_value();
	};
	auto all_vs = tau::get(n).select_top(is_bf_literal);
	return exponent(all_vs.begin(), all_vs.end());
}

template <NodeType node>
tref to_snf_step<node>::get_bf_constant(tref lit) const {
	return tau::get(lit).find_top(is<node, tau::bf_constant>);
}

template <NodeType node>
std::optional<typename to_snf_step<node>::constant>
	to_snf_step<node>::get_constant(tref lit) const
{
	if (tref c = get_bf_constant(lit); c)
		return tau::get(c).get_ba_constant();
	return {};
}

template <NodeType node>
typename to_snf_step<node>::partition
	to_snf_step<node>::make_partition_by_exponent(const literals& s) const
{
	partition p;
	for (tref e: s) p[get_exponent(e)].insert(e);
	return p;
}

template <NodeType node>
tref to_snf_step<node>::squeeze_positives(const literals& positives,
					  const exponent& exp) const
{
	// find first element with non trivial constant
	auto first = std::find_if(positives.begin(), positives.end(),
		[&](tref l) { return get_bf_constant(l) != nullptr; });
	// if there is no such element we return the first element
	if (first == positives.end()) return *positives.begin();
	// otherwise...
	auto first_cte = tau::build_bf_ba_constant(get_constant(*first).value(), find_ba_type<node>(*first));
	auto cte = std::accumulate(++positives.begin(), positives.end(),
		first_cte, [&](tref l, tref r) {
			auto l_cte = get_constant(l), r_cte = get_constant(r);
			if (l_cte && r_cte)
				return tau::build_bf_ba_constant(
					std::visit(_or, l_cte.value(),
							r_cte.value()),
					find_ba_type<node>(l));
			return l;
		});

	// return the conjunction of all the same exponent literals and the accumulated constant
	auto term = std::accumulate(exp.begin(), exp.end(), cte,
		[](tref l, tref r) { return tau::build_bf_and(l, r); });
	// return the corresponding wff
	return tau::build_bf_eq_0(term);
}

template <NodeType node>
bool to_snf_step<node>::is_less_eq_than(tref ll, const exponent& le, tref rl,
	const exponent& re) const
{
	return std::includes(le.begin(), le.end(), re.begin(), re.end(), lcrs_tree<node>::subtree_less)
			&& is_less_eq_than(ll, rl);
}

// remove redundant positives
template <NodeType node>
std::map<typename to_snf_step<node>::exponent, tref>
	to_snf_step<node>::remove_redundant_positives(
		const std::map<exponent, tref>& positives) const
{
	std::map<exponent, tref> nonredundant;

	for (auto& [exp, lit]: positives) {
		bool insert = true;
		std::set<exponent> to_remove;
		for (auto& [e, l]: nonredundant) {
			if (is_less_eq_than(lit, exp, l, e)) {
				insert = false; break;
			} else if (is_less_eq_than(l, e, lit, exp))
				to_remove.insert(e);
		}
		if (insert) nonredundant[exp] = lit;
		for (auto& e: to_remove) nonredundant.erase(e);
	}

	return nonredundant;
}

template <NodeType node>
typename to_snf_step<node>::partition
	to_snf_step<node>::remove_redundant_negatives(
		const partition& negatives) const
{
	partition nonredundant;

	for (auto& [exp, lits] : negatives)
		for (auto& lit : lits) {
			bool insert = true;
			std::set<exponent> to_remove;
			for (auto& [e, ls] : nonredundant)
				for(auto& l : ls) {
					if (is_less_eq_than(l, e, lit, exp)) {
						insert = false; break;
					} else if (is_less_eq_than(
								lit, exp, l, e))
						to_remove.insert(e);
				}
			if (insert) nonredundant[exp].insert(lit);
			for (auto& e: to_remove) nonredundant.erase(e);
		}

	return nonredundant;
}

// squeezed positives as much as possible possible.
template <NodeType node>
std::map<typename to_snf_step<node>::exponent,
	 typename to_snf_step<node>::literal>
		to_snf_step<node>::squeeze_positives(
			const partition& positives) const
{
	// first we squeeze positives by exponent
	std::map<exponent, literal> squeezed;
	for (auto& [exponent, literals] : positives)
		squeezed[exponent] = squeeze_positives(literals, exponent);
	// then we remove redundant positives
	return remove_redundant_positives(squeezed);
}

template <NodeType node>
bool to_snf_step<node>::is_less_eq_than(literal l, literal r) const {
	auto l_cte = get_constant(l), r_cte = get_constant(r);
	if (!l_cte) return !r_cte;
	if (!r_cte) return true;
	return std::visit(_leq, l_cte.value(), r_cte.value());
}

template <NodeType node>
typename to_snf_step<node>::literal to_snf_step<node>::normalize(
	const literals& negatives, literal positive, const exponent& exp) const
{
	// we tacitely assume that the positive literal has a constant
	// different from 1. Otherwise, the normalizer should already
	// return F.
	auto neg_positive_cte = tau::build_bf_neg(
		tau::build_bf_ba_constant(get_constant(positive).value(), find_ba_type<node>(positive)));
	// now we conjunct the previous result with the constant of n
	literals lits; lits.insert(positive);
	for (tref negative : negatives) {
		auto n_cte = get_constant(negative);
		tref nn_cte = n_cte
			? tau::build_bf_and(neg_positive_cte, tau::build_bf_ba_constant(n_cte.value(), find_ba_type<node>(negative)))
			: neg_positive_cte;
		auto nn = tau::build_bf_and(nn_cte, tau::build_bf_and(exp));
		lits.insert(tau::build_bf_neq_0(nn));
	}
	return tau::build_wff_and(lits);
}

// normalize each bdd path applying Corollary 3.1 from TABA book with few
// improvements related to the handling of negative literals.
template <NodeType node>
tref to_snf_step<node>::normalize(const bdd_path& path) const {
	// if we have no positive literals we return the conjunction of all the
	// negative literals negated. PLease note that we store the positive
	// versions of the literals in the second component of the path. Thus,
	// we need to negate them or, equivalently, to build the conjunction of
	// them, we compute the negation of the the disjunction.
	if (path.first.empty()) {
		literals negs;
		for (auto& [_, lits] : remove_redundant_negatives(path.second))
			negs.insert(lits.begin(), lits.end());
		return tau::build_wff_neg(tau::build_wff_or(negs));
	}

	// otherwise, let us consider lits the set of literals to be returned
	// conjuncted.
	partition squeezed_negatives;
	// first we squeezed positive literals...
	auto squeezed_positives = squeeze_positives(path.first);
	// ...the negatives are already squeezed (by order)
	// for every negative class (same exponent) of literals...
	for (auto& [negative_exponent, negatives] : path.second) {
		// - if no positive literal has the same exponent as n, we add n to
		//   the literals
		if (!squeezed_positives.contains(negative_exponent)) {
			squeezed_negatives[negative_exponent].insert(
				tau::build_wff_neg(
					tau::build_wff_or(negatives)));
			continue;
		}
		// - if the positive literal has 1 as constant we return F,
		if (!get_bf_constant(squeezed_positives[negative_exponent]))
			return tau::_F();
	// otherwise we compute the new negated literal following the Corollary 3.1
		// from TABA book.
		squeezed_negatives[negative_exponent].insert(normalize(
			negatives, squeezed_positives.at(negative_exponent),
			negative_exponent));
	}
	// we remove redundant negatives
	squeezed_negatives = remove_redundant_negatives(squeezed_negatives);
	// and add the positive terms...
	literals result;
	for (auto [_, negatives]: squeezed_negatives)
		result.insert(negatives.begin(), negatives.end());

	for (auto [_, positive]: squeezed_positives)
		result.insert(positive);
	// and return the conjunction of all the lits
	return tau::build_wff_and(result);
}

template <NodeType node>
typename to_snf_step<node>::bdd_path to_snf_step<node>::get_relative_path(
	const bdd_path& path, literal lit) const
{
	bdd_path relative_path;
	auto exp = get_exponent(lit);
	if (path.first.contains(exp))
		relative_path.first[exp] = path.first.at(exp);
	if (path.second.contains(exp))
		relative_path.second[exp] = path.second.at(exp);
	return relative_path;
}

template <NodeType node>
tref to_snf_step<node>::normalize_positive(
	const bdd_path& path, literal positive) const
{
	auto relative_path = get_relative_path(path, positive);
	return normalize(add_to_positive_path(relative_path, positive));
}

template <NodeType node>
tref to_snf_step<node>::normalize_negative(
	const bdd_path& path, literal negative) const
{
	auto relative_path = get_relative_path(path, negative);
	return normalize(add_to_negative_path(relative_path, negative));
}

template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& n, const to_snf_step<node>& r)
{
	return typename tree<node>::traverser(r(n.value()));
}

template <NodeType node>
tref snf_bf(tref n) {
	using tau = tree<node>;
	using tt = tau::traverser;
	// TODO (HIGH) give a proper implementation (call to_bdd...)
	return tt(n) | bf_reduce_canonical<node>()
		| tt::f(to_dnf<node, false>)
		| repeat_all<node, step<node>>(elim_eqs<node>())
		// TODO (MEDIUM) review after we fully normalize bf & wff
		| reduce_bf_deprecated<node>
		| tt::ref;
}

// We mostly follow the Remark 3.5 from the TABA book. However, we deviate at
// some points. In particular, we assume that the formula is in MNF,
// instead of in MNF+BDD. The reason behind this is that we want to avoid the
// construction of the BDD form and then traverse it. Our aim is to build the
// BDD form and traverse it afterwards.
template <NodeType node>
tref snf_wff(tref n) {
	using tau = tree<node>;
	using tt = tau::traverser;
	LOG_DEBUG << "--------------------------------";
	LOG_DEBUG << "to_snf formula: " << LOG_FM(n);
	LOG_DEBUG << "--------------------------------";
	auto [_, nn] = get_inner_quantified_wff<node>(n);
	// in the first step we apply compute the SNF of the formula, as a result we get
	// the formula in SNF with positive equal exponent literals sqeezed.
	tref first_step = tt(tau::build_wff_neg(nn))
		| bf_reduce_canonical<node>()
		| tt::f(unsqueeze_wff<node>)
		| simplify_snf<node>()
		| repeat_all<node, to_snf_step<node>>(to_snf_step<node>())
		| tt::ref;
	LOG_DEBUG << "--------------------------------";
	LOG_DEBUG << "to_snf first_step result: " << LOG_FM(first_step);
	LOG_DEBUG << "--------------------------------";
	// in the second step we compute the SNF of the negation of the the result
	// of the first step in order to squeeze the negative equal exponent literals.
	// Note that in this case we don't need to unsqueeze the formula.
	tref second_step = tt(tau::build_wff_neg(first_step))
		| repeat_all<node, to_snf_step<node>>(to_snf_step<node>())
		| repeat_all<node, step<node> >(to_steps<node>({
			elim_eqs<node>(), fix_neg_in_snf<node>()
		}))
		| bf_reduce_canonical<node>()
		| tt::ref;
	LOG_DEBUG << "--------------------------------";
	LOG_DEBUG << "to_snf second_step result: " << LOG_FM(second_step);
	LOG_DEBUG << "--------------------------------";
	return rewriter::replace<node>(n, nn, second_step);
}

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
