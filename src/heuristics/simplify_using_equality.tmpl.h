// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "union_find_with_sets.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "simplify_using_equality"

namespace idni::tau_lang {

// Forward declaration: defined in normal_forms.tmpl.h, included after this file.
template <NodeType node>
tref syntactic_atomic_formula_simplification(tref atomic_formula);

// ── Internal helpers ────────────────────────────────────────────────────────

/**
 * @internal
 * @brief Strict term ordering used to pick canonical union-find representatives.
 *
 * Order: `0` < `1` < `ba_constant` < uninterpreted constant < input stream
 * variable < output stream variable < plain variable < everything else (by
 * `subtree_less`).
 * @tparam node Tree node type.
 * @param l Left term.
 * @param r Right term.
 * @return `true` if @p l strictly precedes @p r in this order.
 *
 * @par Example
 * @code{.cpp}
 * // 0 precedes any variable (see
 * // tests/integration/test_integration-heuristics-simplify_using_equality.cpp:20-26).
 * tref atm    = get_nso_rr("x = 0.").value().main->get();
 * tref x_t    = tau::get(atm)[0].first();
 * tref zero_t = tau::get(atm)[0].second();
 * CHECK( simplify_using_equality_term_comp<node_t>(zero_t, x_t) == true );
 * CHECK( simplify_using_equality_term_comp<node_t>(x_t, zero_t) == false );
 * @endcode
 * @endinternal
 */
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

/**
 * @internal
 * @brief Stable-sorts @p conjs in place so equality conjuncts come before
 * non-equality ones, and among equalities, equational assignments
 * (`variable = ...`) come before non-assignment equalities.
 * @tparam node Tree node type.
 * @param conjs Conjuncts to sort in place.
 *
 * @par Example
 * @code{.cpp}
 * // "x != 0 && y = 0": the equality moves to the front (see
 * // tests/integration/test_integration-heuristics-simplify_using_equality.cpp:78-85).
 * tref fm = get_nso_rr("x != 0 && y = 0.").value().main->get();
 * trefs conjs = get_cnf_wff_clauses<node_t>(fm);
 * simplify_using_equality_sort_atms<node_t>(conjs);
 * CHECK( tau::get(conjs[0]).child_is(tau::bf_eq) );
 * CHECK( !tau::get(conjs[1]).child_is(tau::bf_eq) );
 * @endcode
 * @endinternal
 */
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

/**
 * @internal
 * @brief Reorients an equality/inequality atom so its `term_comp`-larger
 * side is on the left; non-equality atoms are returned unchanged.
 * @tparam node Tree node type.
 * @param atm Atomic formula to reorient.
 * @return The reoriented atom, or @p atm unchanged if it isn't `=`/`!=` or
 * is already correctly oriented.
 *
 * @par Example
 * @code{.cpp}
 * // "0 = x" is reoriented to "x = 0" (see
 * // tests/integration/test_integration-heuristics-simplify_using_equality.cpp:144-151).
 * tref orig   = get_nso_rr("x = 0.").value().main->get();
 * tref x_t    = tau::get(orig)[0].first();
 * tref zero_t = tau::get(orig)[0].second();
 * tref rev    = tau::build_bf_eq(zero_t, x_t);
 * tref result = simplify_using_equality_direct_atm<node_t>(rev);
 * // tau::get(result).to_str() == "x = 0"
 * @endcode
 * @endinternal
 */
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

struct simplify_using_equality_cached_consequences {
	tref lhs_neg = nullptr;
	tref rhs_neg = nullptr;
	tref lhs_and_rhs_neg = nullptr;
	tref rhs_and_lhs_neg = nullptr;
	tref lhs_or_rhs_neg = nullptr;
	tref rhs_or_lhs_neg = nullptr;
};

/**
 * @internal
 * @brief Registers `lhs = rhs` (from @p eq) in @p uf, also merging the
 * derived negation/conjunction/disjunction consequences (`!lhs~!rhs`,
 * `lhs&&!rhs~0`, `lhs||!rhs~1`, and their mirror images).
 * @tparam node Tree node type.
 * @param uf Union-find to register the equality (and consequences) in.
 * @param eq The equality atom to add (`T`/`F` are accepted as trivial cases).
 * @return `false` if a syntactic contradiction is detected (e.g. `lhs` and
 * `!lhs` become connected, or `0` and `1` become connected); `true` otherwise.
 *
 * @par Example
 * @code{.cpp}
 * // x = 0 then x = 1 is a contradiction (see
 * // tests/integration/test_integration-heuristics-simplify_using_equality.cpp:202-210).
 * auto uf = union_find_with_sets<
 *     decltype(simplify_using_equality_term_comp<node_t>), node_t>(
 *         simplify_using_equality_term_comp<node_t>);
 * tref eq0 = get_nso_rr("x = 0.").value().main->get();
 * tref eq1 = get_nso_rr("x = 1.").value().main->get();
 * CHECK( simplify_using_equality_add_raw_equality<node_t>(uf, eq0) == true );
 * CHECK( simplify_using_equality_add_raw_equality<node_t>(uf, eq1) == false );
 * @endcode
 * @endinternal
 */
template <NodeType node>
bool simplify_using_equality_add_raw_equality(auto& uf, tref eq) {
	using tau = tree<node>;
	if (tau::get(eq).equals_T()) return true;
	if (tau::get(eq).equals_F()) return false;
	DBG(LOG_TRACE << "Adding equality: " << tau::get(eq) << "\n";)
	tref lhs = tau::trim2(eq);
	tref rhs = tau::get(tau::trim(eq)).child(1);
	size_t type_id = find_ba_type<node>(eq);
	static std::map<std::pair<tref, tref>, simplify_using_equality_cached_consequences> cache;
	auto key = std::make_pair(lhs, rhs);
	auto it = cache.find(key);
	if (it == cache.end()) {
		simplify_using_equality_cached_consequences derived;
		derived.lhs_neg = push_negation_in<node, false>(build_bf_neg<node>(lhs));
		derived.rhs_neg = push_negation_in<node, false>(build_bf_neg<node>(rhs));
		derived.lhs_and_rhs_neg = build_bf_and<node>(lhs, derived.rhs_neg);
		derived.rhs_and_lhs_neg = build_bf_and<node>(rhs, derived.lhs_neg);
		derived.lhs_or_rhs_neg = build_bf_or<node>(lhs, derived.rhs_neg);
		derived.rhs_or_lhs_neg = build_bf_or<node>(rhs, derived.lhs_neg);
		it = cache.emplace(key, derived).first;
	}
	const auto& derived = it->second;
	uf.merge(lhs, rhs);
	uf.merge(derived.lhs_neg, derived.rhs_neg);
	uf.merge(derived.lhs_and_rhs_neg, _0<node>(type_id));
	uf.merge(derived.rhs_and_lhs_neg, _0<node>(type_id));
	uf.merge(derived.lhs_or_rhs_neg, _1<node>(type_id));
	uf.merge(derived.rhs_or_lhs_neg, _1<node>(type_id));
	return !uf.connected(lhs, derived.lhs_neg)
		&& !uf.connected(rhs, derived.rhs_neg)
		&& !uf.connected(_0<node>(type_id), _1<node>(type_id));
}

/**
 * @internal
 * @brief Registers @p eq in @p uf, decomposing `f = 0` via DNF first so each
 * disjunct of `f` gets connected to `0` individually.
 * @tparam node Tree node type.
 * @param uf Union-find to register the equality (and derived facts) in.
 * @param eq The equality atom to add.
 * @return `false` if a contradiction is detected in any decomposed part;
 * `true` otherwise.
 *
 * @par Example
 * @code{.cpp}
 * // "x|y = 0" decomposes: x and y are each individually connected to 0
 * // (see tests/integration/test_integration-heuristics-simplify_using_equality.cpp:248-260).
 * auto uf = union_find_with_sets<
 *     decltype(simplify_using_equality_term_comp<node_t>), node_t>(
 *         simplify_using_equality_term_comp<node_t>);
 * tref eq = get_nso_rr("x|y = 0.").value().main->get();
 * tref eq_x = get_nso_rr("x = 0.").value().main->get();
 * tref x_t = tau::get(eq_x)[0].first();
 * tref zero_t = tau::get(eq_x)[0].second();
 * CHECK( simplify_using_equality_add_equality<node_t>(uf, eq) == true );
 * CHECK( uf.connected(x_t, zero_t) );
 * @endcode
 * @endinternal
 */
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

/**
 * @internal
 * @brief Rewrites sub-terms of @p eq with their union-find canonical
 * representatives from @p uf, leaving @p eq unchanged where no rewrite
 * applies. Preserves sub-terms whose variables are all input-stream
 * variables.
 * @tparam node Tree node type.
 * @param uf Union-find holding previously-registered equalities.
 * @param eq Equation to rewrite.
 * @return The rewritten equation (or @p eq itself if `T`/`F`, or if no
 * known equality applies).
 *
 * @par Example
 * @code{.cpp}
 * // x = 0 is known; simplifying "x = y" replaces x by 0 (see
 * // tests/integration/test_integration-heuristics-simplify_using_equality.cpp:296-304).
 * auto uf = union_find_with_sets<
 *     decltype(simplify_using_equality_term_comp<node_t>), node_t>(
 *         simplify_using_equality_term_comp<node_t>);
 * tref eq_x0 = get_nso_rr("x = 0.").value().main->get();
 * simplify_using_equality_add_raw_equality<node_t>(uf, eq_x0);
 * tref eq_xy = get_nso_rr("x = y.").value().main->get();
 * tref res = simplify_using_equality_simplify_equation<node_t>(uf, eq_xy);
 * CHECK( tau::get(res) != tau::get(eq_xy) );
 * @endcode
 * @endinternal
 */
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
			// Skip io_var subtrees: the time-offset inside io_var (e.g. the 't'
			// in i1[t]) is a plain 'variable' node that is not an input stream,
			// so visiting it would incorrectly set all_inputs = false.
			auto skip_io_var = [](tref cur) {
				return !tau::get(cur).is(tau::io_var);
			};
			pre_order<node>(n).search_unique(input_vars, skip_io_var, identity);
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
	const tref original_fm = fm;
	fm = to_nnf<node>(fm);
	if (tau::get(fm).equals_T() || tau::get(fm).equals_F())
		return fm;
	DBG(LOG_DEBUG << "simplify_using_equality on " << LOG_FM(fm) << "\n";)
	auto uf = union_find_with_sets<decltype(simplify_using_equality_term_comp<node>), node>(
		simplify_using_equality_term_comp<node>);
	std::vector<union_find_with_sets<decltype(simplify_using_equality_term_comp<node>), node>> uf_stack;
	uf_stack.emplace_back(std::move(uf));
	subtree_unordered_set<node> mark;
	bool uf_stack_imbalance = false;
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
	auto up = [&uf_stack, &mark, &uf_stack_imbalance](tref n, tref parent) {
		if (!is<node>(n, tau::wff)) return n;
		if (parent != nullptr && is<node>(parent, tau::wff_or)) {
			auto pop_scope = [&]() {
				if (uf_stack.size() <= 1) {
					uf_stack_imbalance = true;
					LOG_ERROR << "UF scope stack underflow in simplify_using_equality\n";
					return;
				}
				uf_stack.pop_back();
			};
			if (!is_child<node>(n, tau::wff_or)) {
				if (auto it = mark.find(parent); it != mark.end())
					mark.erase(it);
				else pop_scope();
			} else {
				if (auto it = mark.find(parent); it == mark.end())
					pop_scope();
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
	if (uf_stack_imbalance || uf_stack.size() != 1) {
		LOG_ERROR << "simplify_using_equality: union-find stack imbalance ("
			<< uf_stack.size() << " active scopes remain)\n";
		return original_fm;
	}
	DBG(assert(uf_stack.size() == 1);)
	return fm;
}

} // namespace idni::tau_lang
