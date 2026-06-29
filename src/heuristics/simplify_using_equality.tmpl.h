// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "union_find_with_sets.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "heuristics"

namespace idni::tau_lang {

// Forward declaration: defined in normal_forms.tmpl.h, included after this file.
template <NodeType node>
tref syntactic_atomic_formula_simplification(tref atomic_formula);

// ── Internal helpers ────────────────────────────────────────────────────────

// Term ordering: 0 < 1 < ba_constant < uconst < input_stream < output_stream
// < variable < rest (by subtree_less). Used to choose UF canonical representatives.
// TODO: For variables, make lower time step < higher time step
template <NodeType node>
bool simplify_using_equality_term_comp(tref l, tref r) {
	using tau = tree<node>;
	if (tau::get(l).equals_0()) {
		if (!tau::get(r).equals_0()) return true;
		else return false;
	}
	if (tau::get(r).equals_0()) return false;
	if (tau::get(l).equals_1()) {
		if (!tau::get(r).equals_1()) return true;
		else return false;
	}
	if (tau::get(r).equals_1()) return false;
	DBG(assert(tau::get(l).has_child() && tau::get(r).has_child()));
	const tau& lc = tau::get(l)[0];
	const tau& rc = tau::get(r)[0];
	if (lc.is(tau::ba_constant)) {
		if (!rc.is(tau::ba_constant)) return true;
		else return tau::subtree_less(l, r);
	}
	if (rc.is(tau::ba_constant)) return false;
	if (lc.is(tau::variable)) {
		if (rc.is(tau::variable)) {
			if (lc[0].is(tau::uconst_name)) {
				if (!rc[0].is(tau::uconst_name))
					return true;
				else return tau::subtree_less(l, r);
			}
			if (rc[0].is(tau::uconst_name)) return false;
			if (lc.is_input_variable()) {
				if (!rc.is_input_variable())
					return true;
				else return tau::subtree_less(l, r);
			}
			if (rc.is_input_variable()) return false;
			if (lc.is_output_variable()) {
				if (!rc.is_output_variable())
					return true;
				else return tau::subtree_less(l, r);
			}
			if (rc.is_output_variable()) return false;
			return tau::subtree_less(l, r);
		} else return true;
	}
	if (rc.is(tau::variable)) return false;
	// TODO: also use free_vars count once constant time
	return tau::subtree_less(l, r);
}

// Sort conjuncts so equalities (and equational assignments) come first.
template <NodeType node>
void simplify_using_equality_sort_atms(auto& conjs) {
	using tau = tree<node>;
	auto eq_comp = [](tref l, tref r) {
		const tau& nl = tau::get(l);
		const tau& nr = tau::get(r);
		if (nl.child_is(tau::bf_eq)) {
			if (!nr.child_is(tau::bf_eq)) return true;
			if (is_equational_assignment<node>(l))
				return !is_equational_assignment<node>(r);
			return false;
		} else return false;
	};
	std::ranges::sort(conjs, eq_comp);
}

// Orient a = b / a != b so the larger side (per term_comp) is on the left.
template <NodeType node>
tref simplify_using_equality_direct_atm(tref atm) {
	using tau = tree<node>;
	const tau& c = tau::get(atm)[0];
	if (!c.is(tau::bf_eq) && !c.is(tau::bf_neq)) return atm;
	const size_t sym = c.get_type();
	tref c1 = tau::get(atm)[0].first();
	tref c2 = tau::get(atm)[0].second();
	if (simplify_using_equality_term_comp<node>(c1, c2))
		return tau::get(tau::wff, tau::get(sym, c2, c1));
	return atm;
}

// ── Public helpers ───────────────────────────────────────────────────────────

// Register a = b in uf, also recording negation/product/sum consequences.
// Returns false if a syntactic contradiction is detected.
template <NodeType node>
bool simplify_using_equality_add_raw_equality(auto& uf, tref eq) {
	using tau = tree<node>;
	if (tau::get(eq).equals_T()) return true;
	if (tau::get(eq).equals_F()) return false;
	DBG(LOG_TRACE << "Adding equality: " << tau::get(eq) << "\n";)
	tref lhs = tau::trim2(eq);
	tref rhs = tau::get(tau::trim(eq)).child(1);
	tref lhs_neg = push_negation_in<node, false>(build_bf_neg<node>(lhs));
	tref rhs_neg = push_negation_in<node, false>(build_bf_neg<node>(rhs));
	size_t type_id = find_ba_type<node>(eq);
	uf.merge(lhs, rhs);
	uf.merge(lhs_neg, rhs_neg);
	uf.merge(build_bf_and<node>(lhs, rhs_neg), _0<node>(type_id));
	uf.merge(build_bf_and<node>(rhs, lhs_neg), _0<node>(type_id));
	uf.merge(build_bf_or<node>(lhs, rhs_neg), _1<node>(type_id));
	uf.merge(build_bf_or<node>(rhs, lhs_neg), _1<node>(type_id));
	return !uf.connected(lhs, lhs_neg)
		&& !uf.connected(rhs, rhs_neg)
		&& !uf.connected(_0<node>(type_id), _1<node>(type_id));
}

// Register eq in uf. If eq is f=0 decomposes f via DNF first.
// Returns false on contradiction.
template <NodeType node>
bool simplify_using_equality_add_equality(auto& uf, tref eq) {
	using tau = tree<node>;
	if (const tau& c = tau::get(eq)[0]; c[1].equals_0()) {
		tref func = push_negation_in<node, false>(c.first());
		bool valid = true;
		for (tref disj : get_dnf_bf_clauses<node>(func))
			valid = valid && simplify_using_equality_add_raw_equality<node>(uf,
				tau::build_bf_eq_0(disj));
		return valid;
	} else return simplify_using_equality_add_raw_equality<node>(uf, eq);
}

// Rewrite sub-terms of eq with their UF representatives.
// Preserves sub-terms whose variables are all inputs.
template <NodeType node>
tref simplify_using_equality_simplify_equation(auto& uf, tref eq) {
	using tau = tree<node>;
	DBG(LOG_TRACE << "Simplifying: " << tau::get(eq) << "\n";)
	if (tau::get(eq).equals_T() || tau::get(eq).equals_F()) return eq;
	auto uf_find = [&uf](tref n) {
		if (uf.contains(n)) {
			bool all_inputs = false;
			auto input_vars = [&all_inputs](tref cur) {
				if (tau::get(cur).is(tau::variable)) {
					if (tau::get(cur).is_input_variable()) {
						all_inputs = true;
						return true;
					}
					all_inputs = false;
					return false;
				}
				return true;
			};
			pre_order<node>(n).search_unique(input_vars);
			return all_inputs ? n : uf.find(n);
		}
		return n;
	};
	tref simp_eq = pre_order<node>(eq).apply(uf_find);
	DBG(LOG_TRACE << "Simplified to: " << tau::get(simp_eq) << "\n";)
	if (tau::get(simp_eq) != tau::get(eq))
		return simp_eq;
	else return eq;
}

// ── Main entry point ─────────────────────────────────────────────────────────

template <NodeType node>
tref simplify_using_equality(tref fm) {
	using tau = tree<node>;
	fm = to_nnf<node>(fm);
	if (tau::get(fm).equals_T() || tau::get(fm).equals_F())
		return fm;
	DBG(LOG_DEBUG << "simplify_using_equality on " << LOG_FM(fm) << "\n";)
	auto uf = union_find_with_sets<decltype(simplify_using_equality_term_comp<node>), node>(
		simplify_using_equality_term_comp<node>);
	std::vector<union_find_with_sets<decltype(simplify_using_equality_term_comp<node>), node>> uf_stack;
	uf_stack.emplace_back(std::move(uf));
	subtree_unordered_set<node> mark;
	auto f = [&uf_stack, &mark](tref n, tref parent) {
		if (!is<node>(n, tau::wff)) return n;
		const tau& cn = tau::get(n)[0];
		if (parent != nullptr && is<node>(parent, tau::wff_or)) {
			if (!cn.is(tau::wff_or))
				uf_stack.push_back(uf_stack.back());
			else mark.insert(parent);
		}
		if (cn.is(tau::bf_eq)) {
			n = syntactic_atomic_formula_simplification<node>(n);
			n = simplify_using_equality_direct_atm<node>(n);
			tref s = simplify_using_equality_simplify_equation<node>(uf_stack.back(), n);
			if (!is_child<node>(s, tau::bf_eq)) return s;
			if (simplify_using_equality_add_equality<node>(uf_stack.back(), s)) return s;
			else return _F<node>();
		} else if (is_atomic_fm<node>(n)) {
			n = syntactic_atomic_formula_simplification<node>(n);
			n = simplify_using_equality_direct_atm<node>(n);
			return simplify_using_equality_simplify_equation<node>(uf_stack.back(), n);
		} else if (cn.is(tau::wff_and)) {
			if (mark.contains(n)) return n;
			trefs conjs = get_cnf_wff_clauses<node>(n);
			simplify_using_equality_sort_atms<node>(conjs);
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
				if (auto it = mark.find(parent); it != mark.end())
					mark.erase(it);
				else uf_stack.pop_back();
			} else {
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
		return is_formula<node>(n);
	};
	fm = pre_order<node>(fm).apply(f, visit, up);
	DBG(LOG_DEBUG << "simplify_using_equality result: " << LOG_FM(fm) << "\n";)
	DBG(assert(uf_stack.size() == 1);)
	return fm;
}

} // namespace idni::tau_lang
