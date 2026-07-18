// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// normal_forms_nnf.tmpl.h - Negation normal form, push_negation, to_dnf/to_cnf
// Split from normal_forms.tmpl.h for readability.

namespace idni::tau_lang {


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
			case tau::wff_F: return tau::build_wff_always(
						tau::build_wff_neg(ct[0].first()));
			// LTL(ABA) NNF duals:
			//   ¬(φ U ψ) = (¬φ) R (¬ψ)
			//   ¬(φ R ψ) = (¬φ) U (¬ψ)
			//   ¬(φ W ψ) = (¬φ R ¬ψ) ∧ F(¬φ)  [dual is strong-release M]
			case tau::wff_U: return tau::build_wff_R(
						tau::build_wff_neg(ct[0].first()),
						tau::build_wff_neg(ct[0].second()));
			case tau::wff_R: return tau::build_wff_U(
						tau::build_wff_neg(ct[0].first()),
						tau::build_wff_neg(ct[0].second()));
			case tau::wff_W: {
				tref neg_l = tau::build_wff_neg(ct[0].first());
				tref neg_r = tau::build_wff_neg(ct[0].second());
				// ¬(φ W ψ) = (¬φ R ¬ψ) ∧ F(¬φ)
				return tau::build_wff_and(
					tau::build_wff_R(neg_l, neg_r),
					tau::build_wff_F(neg_l));
			}
			// ¬(φ S ψ) = ¬φ T ¬ψ  (past duals)
			case tau::wff_S: return tau::build_wff_T(
						tau::build_wff_neg(ct[0].first()),
						tau::build_wff_neg(ct[0].second()));
			case tau::wff_T: return tau::build_wff_S(
						tau::build_wff_neg(ct[0].first()),
						tau::build_wff_neg(ct[0].second()));
			// CTL* NNF duals: ¬A χ = E ¬χ,  ¬E χ = A ¬χ
			case tau::wff_A: return tau::build_wff_E(
						tau::build_wff_neg(ct[0].first()));
			case tau::wff_E: return tau::build_wff_A(
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
	// Short-circuit identity: T ∧ x = x
	// _T() returns a hash-wrapped node, so use address comparison.
	if (fm1 == tau::_T()) return fm2;
	if (fm2 == tau::_T()) return fm1;
	if (fm1 == tau::_F()) return fm1;
	if (fm2 == tau::_F()) return fm2;
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
tref push_existential_quantifier_one(tref fm, subtree_set<node>* excluded = nullptr) {
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


} // namespace idni::tau_lang
