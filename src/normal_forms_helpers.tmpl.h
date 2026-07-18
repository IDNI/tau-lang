// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// normal_forms_helpers.tmpl.h - Basic NF utilities, squeeze/unsqueeze, ONF
// Split from normal_forms.tmpl.h for readability.

namespace idni::tau_lang {

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "normal_forms"

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

template<NodeType node>
tref normalize_atomic_formula_operators(tref fm) {
	using tau = tree<node>;
	LOG_TRACE << "Begin normalize_atomic_formula_operators: " << LOG_FM(fm);
	auto normalize_operators = [](tref n) {
		if (!tau::get(n).is(tau::wff)) return n;
		const tau& c = tau::get(n)[0];
		switch (c.value.nt) {
			case tau::bf_neq:
				return tau::build_wff_neg(
					tau::build_bf_eq(c.first(), c.second()));
			case tau::bf_nlteq:
				return tau::build_bf_lt(c.second(), c.first());
				break;
			case tau::bf_nlt:
				return tau::build_wff_neg(
					tau::build_bf_lt(c.first(), c.second()));
			case tau::bf_gteq:
				return tau::build_wff_neg(
					tau::build_bf_lt(c.first(), c.second()));
			case tau::bf_gt:
				return tau::build_bf_lt(c.second(), c.first());
			case tau::bf_ngteq:
				return tau::build_bf_lt(c.first(), c.second());
			case tau::bf_ngt: return tau::build_wff_neg(
					tau::build_bf_lt(c.second(), c.first()));
			default: return n;
		}
	};
	tref result = pre_order<node>(fm)
				.apply_unique(normalize_operators, visit_wff<node>);
	LOG_TRACE << "End normalize_atomic_formula_operators: " << LOG_FM(result);
	return result;
}

template<NodeType node>
tref gt_gteq_to_lt_lteq(tref fm) {
	using tau = tree<node>;
	LOG_TRACE << "gt_gteq_to_lt_lteq: " << LOG_FM(fm);
	auto normalize_operators = [](tref n) {
		if (!tau::get(n).is(tau::wff)) return n;
		const tau& c = tau::get(n)[0];
		switch (c.value.nt) {
			case tau::bf_gteq:
				return tau::build_bf_lteq(c.second(), c.first());
			case tau::bf_gt:
				return tau::build_bf_lt(c.second(), c.first());
			case tau::bf_ngteq:
				return tau::build_bf_nlteq(c.second(), c.first());
			case tau::bf_ngt:
				return tau::build_bf_nlt(c.second(), c.first());
			default: return n;
		}
	};
	tref result = pre_order<node>(fm)
				.apply_unique(normalize_operators, visit_wff<node>);
	LOG_TRACE << "gt_gteq_to_lt_lteq: " << LOG_FM(result);
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

// -----------------------------------------------------------------------------

// This function traverses a term fm and normalizes all Boolean algebra constants
template <NodeType node>
tref normalize_ba(tref fm) {
	using tau = tree<node>;
	DBG(LOG_TRACE << "normalize_ba: " << LOG_FM(fm));
	using tau = tree<node>;
	DBG(assert(tau::get(fm).is(tau::bf));)
	auto push_negation = [&](tref n) {
		const tau& t = tau::get(n);
		// Push negation into constants
		if (t.is(tau::bf_neg) && t[0].child_is(tau::ba_constant)) {
			const auto c = t[0][0].get_ba_constant();
			return tau::get_ba_constant(~c, t.get_ba_type());
		}
		// Push negations in
		n = push_negation_one_in<node, false>(n);
		return n;
	};
	auto norm_ba = [](tref n) {
		const tau& t = tau::get(n);
		// Check if node is a constant
		if (!t.is(tau::ba_constant)) return n;
		// Node has a Boolean algebra element
		auto c = t.get_ba_constant();
		auto nc = node::ba::normalize(c);
		if (c == nc) return n;
		return tau::get_ba_constant(nc, t.get_ba_type());
	};
	fm = pre_order<node>(fm).apply_unique(push_negation);
	tref r = pre_order<node>(fm).template apply_unique<normalize_ba_m>(norm_ba);
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
	// FIXME take into account quantifiers
	return tt(n) | tt::f(to_dnf<node, true>)
		     | tt::f(onf_wff<node>(var))
		     | tt::f(to_dnf<node, true>) | tt::ref;
}

} // namespace idni::tau_lang
