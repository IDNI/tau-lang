// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <numeric>

#include "tau_tree.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "builders"

namespace idni::tau_lang {

template <NodeType node>
tref tree<node>::apply_builder(const rewriter::builder& b, trefs n) {
	subtree_map<node, tref> changes;
	trefs vars = (tt(b.first->get()) || capture).values();
	for (size_t i = 0; i < vars.size(); ++i) changes[vars[i]] = n[i];
	return rewriter::replace<node>(b.second->get(), changes);
}

template<NodeType node>
tref canonize_quantifier_ids(tref fm) {
	using tau = tree<node>;
	DBG(LOG_DEBUG << "Start canonize_quantifier_ids with: " << tau::get(fm) << "\n";)
	// Going down fm once, collect quantifiers and calculate quantifier ids
	// during this traversal. The quantifier id is the maximal quantifier
	// depth below the current quantifier + 1
	// Going down fm again, replace found bound variables with corresponding
	// quantifier id variable
	subtree_map<node, size_t> scope_to_id;
	std::vector<size_t> scope_id;
	auto populate_scope = [&](tref n) {
		if (is_quantifier<node>(n)) {
			scope_id.push_back(1);
			for (size_t i = scope_id.size(); i > 1; --i) {
				if (scope_id[i-2] == scope_id[i-1])
					++scope_id[i-2];
			}
		}
		return !tree<node>::get(n).is_term();
	};
	auto populate_ids = [&](tref n) {
		if (is_quantifier<node>(n)) {
			scope_to_id.emplace(n, scope_id.back());
			scope_id.pop_back();
		}
	};
	pre_order<node>(fm).visit(populate_scope, all, populate_ids);
	// Next update variables using the information from var_to_id
	subtree_map<node, std::vector<size_t>> var_to_id;
	subtree_map<node, tref> old_name;
	auto update_var = [&](tref n) {
		if (is_quantifier<node>(n)) {
			DBG(assert(scope_to_id.contains(n));)
			if (auto it = var_to_id.find(tau::trim(n)); it != var_to_id.end()) {
				it->second.push_back(scope_to_id.find(n)->second);
			} else var_to_id.emplace(tau::trim(n), std::vector{scope_to_id.find(n)->second});
		} else if (is<node, tau::variable>(n)) {
			if (auto it = var_to_id.find(n); it != var_to_id.end()) {
				tref nvar = tau::build_variable(
					std::to_string(it->second.back()),
					tau::get(n).get_ba_type());
				old_name.emplace(nvar, n);
				return tau::build_variable(
					std::to_string(it->second.back()),
					tau::get(n).get_ba_type());
			}
		}
		return n;
	};
	auto update_var_up = [&](tref n) {
		if (is_quantifier<node>(n)) {
			DBG(assert(old_name.contains(tau::trim(n)));)
			DBG(assert(var_to_id.contains(
				old_name.find(tau::trim(n))->second));)
			auto it = var_to_id.find(old_name.find(tau::trim(n))->second);
			old_name.erase(tau::trim(n));
			if ( it->second.size() == 1) var_to_id.erase(it);
			else it->second.pop_back();
		}
		return n;
	};
	fm = pre_order<node>(fm).apply(update_var, all, update_var_up);
	DBG(LOG_DEBUG << "End canonize_quantifier_ids with: " << tau::get(fm) << "\n";)
	return fm;
}

//------------------------------------------------------------------------------
// builders

template <NodeType node>
tref _0(size_t type_id) { return get_0<node>(type_id).get(); }
template <NodeType node>
tref _1(size_t type_id) { return get_1<node>(type_id).get(); }
template <NodeType node>
tref _F() { return get_F<node>().get(); }
template <NodeType node>
tref _T() { return get_T<node>().get(); }

template <NodeType node>
tref _0_trimmed(size_t type_id) { return get_0_trimmed<node>(type_id).get(); }
template <NodeType node>
tref _1_trimmed(size_t type_id) { return get_1_trimmed<node>(type_id).get(); }
template <NodeType node>
tref _F_trimmed() { return get_F_trimmed<node>().get(); }
template <NodeType node>
tref _T_trimmed() { return get_T_trimmed<node>().get(); }

template <NodeType node>
const tree<node>& get_0(size_t type_id) {
	using tau = tree<node>;
	return tau::get(tau::get_typed(tau::bf,
		_0_trimmed<node>(type_id), type_id));
}

template <NodeType node>
const tree<node>& get_1(size_t type_id) {
	using tau = tree<node>;
	return tau::get(tau::get_typed(tau::bf,
		_1_trimmed<node>(type_id), type_id));
}

template <NodeType node>
const tree<node>& get_F() {
	using tau = tree<node>;
	static htref cached = tau::geth(tau::get(tau::wff, _F_trimmed<node>()));
	return tau::get(cached);
}

template <NodeType node>
const tree<node>& get_T() {
	using tau = tree<node>;
	static htref cached = tau::geth(tau::get(tau::wff, _T_trimmed<node>()));
	return tau::get(cached);
}

template <NodeType node>
const tree<node>& get_0_trimmed(size_t type_id) {
	using tau = tree<node>;
	return tau::get(tau::get_typed(tau::bf_f, type_id));
}

template <NodeType node>
const tree<node>& get_1_trimmed(size_t type_id) {
	using tau = tree<node>;
	return tau::get(tau::get_typed(tau::bf_t, type_id));
}

template <NodeType node>
const tree<node>& get_F_trimmed() {
	using tau = tree<node>;
	static htref cached = tau::geth(tau::get(tau::wff_f));
	return tau::get(cached);
}

template <NodeType node>
const tree<node>& get_T_trimmed() {
	using tau = tree<node>;
	static htref cached = tau::geth(tau::get(tau::wff_t));
	return tau::get(cached);
}

// -----------------------------------------------------------------------------
// formula builders

template <NodeType node>
tref build_wff_sometimes(tref l) {
	DBG(assert(l != nullptr);)
	using tau = tree<node>;
	DBG(assert(tau::get(l).is(tau::wff));)
	return tau::get(tau::wff, tau::get(tau::wff_sometimes, l));
}

template <NodeType node>
tref build_wff_always(tref l) {
	DBG(assert(l != nullptr);)
	using tau = tree<node>;
	DBG(assert(tau::get(l).is(tau::wff));)
	return tau::get(tau::wff, tau::get(tau::wff_always, l));
}

template <NodeType node>
tref build_wff_conditional(tref x, tref y, tref z) {
	DBG(assert(x != nullptr && y != nullptr && z != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::wff_and,
		build_wff_imply<node>(x, y),
		build_wff_imply<node>(build_wff_neg<node>(x), z)));
}

template <NodeType node>
int_t find_biggest_quant_id(tref fm) {
	using tau = tree<node>;
	// Find the biggest quantifier id in fm
	int_t id = 0;
	auto is_number = [](const std::string& s) static {
		if (s.empty()) return false;
		for (const unsigned char c : s) if (!std::isdigit(c)) return false;
		return true;
	};
	auto f = [&](tref n) {
		if (is_quantifier<node>(n)) {
			if (auto name = get_var_name<node>(tau::trim(n));
				is_number(name)) {
				id = std::max(id, std::stoi(name));
				return false;
			}
		}
		return !tau::get(n).is_term();
	};
	pre_order<node>(fm).visit_unique(f);
	return id;
}

// If calculate_quant_id is false no variable renaming in subformula
// is performed, and it is assumed that bound_var has correct representation
template <NodeType node>
tref build_wff_all(tref bound_var, tref subformula, bool calculate_quant_id) {
	DBG(assert(bound_var != nullptr && subformula != nullptr);)
	using tau = tree<node>;
	DBG(assert(tau::get(bound_var).is(tau::variable) && tau::get(subformula).is(tau::wff));)
	tref res = tau::get(tau::wff, tau::get(tau::wff_all, bound_var, subformula));
	if (calculate_quant_id) {
		// Find the biggest quantifier id in subformula and rename
		// bound var to id + 1
		const int_t id = find_biggest_quant_id<node>(subformula);
		return tau::get(res).replace(bound_var,
			tau::build_variable(std::to_string(id + 1),
				tau::get(bound_var).get_ba_type()));
	} else return res;
}

// Quantification pattern is build such that the last bound variable in
// bound_vars is innermost
template <NodeType node>
tref build_wff_all_many(const trefs& bound_vars, tref subformula) {
	using tau = tree<node>;
#ifdef DEBUG
	for (tref bv : bound_vars)
		assert(bv != nullptr && tau::get(bv).is(tau::variable));
	assert(subformula != nullptr && tau::get(subformula).is(tau::wff));
#endif
	// Find the biggest quantifier id in subformula and rename
	// first bound variable to id + 1
	int_t id = find_biggest_quant_id<node>(subformula);
	// Prepare renaming changes and build formula
	subtree_map<node, tref> changes;
	for (tref bv : bound_vars | std::views::reverse) {
		changes.emplace(bv, tau::build_variable(std::to_string(++id),
			tau::get(bv).get_ba_type()));
		subformula = build_wff_all<node>(bv, subformula, false);
	}
	// Rename bound variables
	return rewriter::replace(subformula, changes);
}

// If calculate_quant_id is false no variable renaming in subformula
// is performed, and it is assumed that bound_var has correct representation
template <NodeType node>
tref build_wff_ex(tref bound_var, tref subformula, bool calculate_quant_id) {
	DBG(assert(bound_var != nullptr && subformula != nullptr);)
	using tau = tree<node>;
	DBG(assert(tau::get(bound_var).is(tau::variable) && tau::get(subformula).is(tau::wff));)
	tref res = tau::get(tau::wff, tau::get(tau::wff_ex, bound_var, subformula));
	if (calculate_quant_id) {
		// Find the biggest quantifier id in subformula and rename
		// bound var to id + 1
		const int_t id = find_biggest_quant_id<node>(subformula);
		return tau::get(res).replace(bound_var,
			tau::build_variable(std::to_string(id + 1),
				tau::get(bound_var).get_ba_type()));
	} else return res;
}

// Quantification pattern is build such that the last bound variable in
// bound_vars is innermost
template <NodeType node>
tref build_wff_ex_many(const trefs& bound_vars, tref subformula) {
	using tau = tree<node>;
#ifdef DEBUG
	for (tref bv : bound_vars)
		assert(bv != nullptr && tau::get(bv).is(tau::variable));
	assert(subformula != nullptr && tau::get(subformula).is(tau::wff));
#endif
	// Find the biggest quantifier id in subformula and rename
	// first bound variable to id + 1
	int_t id = find_biggest_quant_id<node>(subformula);
	// Prepare renaming changes and build formula
	subtree_map<node, tref> changes;
	for (tref bv : bound_vars | std::views::reverse) {
		changes.emplace(bv, tau::build_variable(std::to_string(++id),
			tau::get(bv).get_ba_type()));
		subformula = build_wff_ex<node>(bv, subformula, false);
	}
	// Rename bound variables
	return rewriter::replace(subformula, changes);
}

template <NodeType node>
tref build_wff_imply(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	DBG(assert(tau::get(l).is(tau::wff) && tau::get(r).is(tau::wff));)
	return tau::get(tau::wff, tau::get(tau::wff_or,
		build_wff_neg<node>(l), r));
}

template <NodeType node>
tref build_wff_rimply(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	DBG(assert(tau::get(l).is(tau::wff) && tau::get(r).is(tau::wff));)
	return tau::get(tau::wff, tau::get(tau::wff_or,
		build_wff_neg<node>(r), l));
}

template <NodeType node>
tref build_wff_equiv(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	DBG(assert(tau::get(l).is(tau::wff) && tau::get(r).is(tau::wff));)
	return tau::get(tau::wff, tau::get(tau::wff_and,
		build_wff_imply<node>(l, r),
		build_wff_imply<node>(r, l)));
}

template <NodeType node>
tref build_wff_or(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	DBG(assert(tau::get(l).is(tau::wff) && tau::get(r).is(tau::wff));)
	return tau::get(tau::wff, tau::get(tau::wff_or, l, r));
}

template <NodeType node>
tref build_wff_or(const auto& wffs) {
	return std::accumulate(wffs.begin(), wffs.end(), _F<node>(),
		[](tref l, tref r) { return build_wff_or<node>(l, r); });
}


template <NodeType node>
tref build_wff_xor(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	DBG(assert(tau::get(l).is(tau::wff) && tau::get(r).is(tau::wff));)
	return tau::get(tau::wff, tau::get(tau::wff_or,
		build_wff_and<node>(build_wff_neg<node>(l), r),
		build_wff_and<node>(build_wff_neg<node>(r), l)));
}

template <NodeType node>
tref build_wff_and(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	DBG(assert(tau::get(l).is(tau::wff) && tau::get(r).is(tau::wff));)
	return tau::get(tau::wff, tau::get(tau::wff_and, l, r));
}

template <NodeType node>
tref build_wff_and(const auto& wffs) {
	return std::accumulate(wffs.begin(), wffs.end(), _T<node>(),
		[](tref l, tref r) { return build_wff_and<node>(l, r); });
}

template <NodeType node>
tref build_wff_neg(tref l) {
	DBG(assert(l != nullptr);)
	using tau = tree<node>;
	DBG(assert(tau::get(l).is(tau::wff));)
	return tau::get(tau::wff, tau::get(tau::wff_neg, l));
}

// -----------------------------------------------------------------------------
// constraint builders

template <NodeType node>
tref build_wff_ctn_gteq(tref ctnvar, tref num) {
	DBG(assert(ctnvar != nullptr && num != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::constraint,
		tau::get(tau::ctn_gteq, { ctnvar, num })));
}

template <NodeType node>
tref build_wff_ctn_gt(tref ctnvar, tref num) {
	DBG(assert(ctnvar != nullptr && num != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::constraint,
		tau::get(tau::ctn_gt, { ctnvar, num })));
}

template <NodeType node>
tref build_wff_ctn_lteq(tref ctnvar, tref num) {
	DBG(assert(ctnvar != nullptr && num != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::constraint,
		tau::get(tau::ctn_lteq, { ctnvar, num })));
}

template <NodeType node>
tref build_wff_ctn_lt(tref ctnvar, tref num) {
	DBG(assert(ctnvar != nullptr && num != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::constraint,
		tau::get(tau::ctn_lt, { ctnvar, num })));
}

template <NodeType node>
tref build_wff_ctn_eq(tref ctnvar, tref num) {
	DBG(assert(ctnvar != nullptr && num != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::constraint,
		tau::get(tau::ctn_eq, { ctnvar, num })));
}

template <NodeType node>
tref build_wff_ctn_neq(tref ctnvar, tref num) {
	DBG(assert(ctnvar != nullptr && num != nullptr);)
	using tau = tree<node>;
	return tau::get(tau::wff, tau::get(tau::constraint,
		tau::get(tau::ctn_neq, { ctnvar, num })));
}

// -----------------------------------------------------------------------------
// wff relational operators of terms

template <NodeType node>
tref build_bf_interval(tref x, tref y, tref z) {
	DBG(assert(x != nullptr && y != nullptr && z != nullptr);)
	return build_wff_and<node>(build_bf_lteq<node>(x, y),
		build_bf_lteq<node>(y, z));
}

template <NodeType node>
tref build_bf_eq(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	DBG(assert(tau::get(l).is(tau::bf) && tau::get(r).is(tau::bf));)
	return tau::get(tau::wff, tau::get(tau::bf_eq, l, r));
}

template <NodeType node>
tref build_bf_eq_0(tref l) {
	DBG(assert(l != nullptr);)
	using tau = tree<node>;
	DBG(assert(tau::get(l).is(tau::bf));)
	return tau::get(tau::wff, tau::get(tau::bf_eq, l, _0<node>(tau::get(l).get_ba_type())));
}

template<NodeType node>
tref build_bf_neq(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	DBG(assert(tau::get(l).is(tau::bf) && tau::get(r).is(tau::bf));)
	return tau::get(tau::wff, tau::get(tau::bf_neq, l, r));
}

template <NodeType node>
tref build_bf_neq_0(tref l) {
	DBG(assert(l != nullptr);)
	using tau = tree<node>;
	DBG(assert(tau::get(l).is(tau::bf));)
	return tau::get(tau::wff, tau::get(tau::bf_neq, l, _0<node>(
		tau::get(l).get_ba_type())));
}

template <NodeType node>
tref build_bf_lteq(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	DBG(assert(tau::get(l).is(tau::bf) && tau::get(r).is(tau::bf));)
	return tau::get(tau::wff, tau::get(tau::bf_lteq, l, r));
}

template <NodeType node>
tref build_bf_nlteq(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	DBG(assert(tau::get(l).is(tau::bf) && tau::get(r).is(tau::bf));)
	return tau::get(tau::wff, tau::get(tau::bf_nlteq, l, r));
}

template <NodeType node>
tref build_bf_gt(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	DBG(assert(tau::get(l).is(tau::bf) && tau::get(r).is(tau::bf));)
	return tau::get(tau::wff, tau::get(tau::bf_gt, l, r));
}

template <NodeType node>
tref build_bf_ngt(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	DBG(assert(tau::get(l).is(tau::bf) && tau::get(r).is(tau::bf));)
	return tau::get(tau::wff, tau::get(tau::bf_ngt, l, r));
}

template <NodeType node>
tref build_bf_gteq(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	DBG(assert(tau::get(l).is(tau::bf) && tau::get(r).is(tau::bf));)
	return tau::get(tau::wff, tau::get(tau::bf_gteq, l, r));
}

template <NodeType node>
tref build_bf_ngteq(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	DBG(assert(tau::get(l).is(tau::bf) && tau::get(r).is(tau::bf));)
	return tau::get(tau::wff, tau::get(tau::bf_ngteq, l, r));
}

template <NodeType node>
tref build_bf_lt(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	DBG(assert(tau::get(l).is(tau::bf) && tau::get(r).is(tau::bf));)
	return tau::get(tau::wff, tau::get(tau::bf_lt, l, r));
}

template <NodeType node>
tref build_bf_nlt(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	DBG(assert(tau::get(l).is(tau::bf) && tau::get(r).is(tau::bf));)
	return tau::get(tau::wff, tau::get(tau::bf_nlt, l, r));
}

// -----------------------------------------------------------------------------
// term builders

template <NodeType node>
tref build_bf_fall(tref l, tref r) {
	using tau = tree<node>;
	DBG(assert(l != nullptr && r != nullptr);)
	DBG(assert(tau::get(l).is(tau::variable) && tau::get(r).is(tau::bf));)
	return tau::get(tau::bf, tau::get(tau::bf_fall, l, r));
}

template <NodeType node>
tref build_bf_fex(tref l, tref r) {
	using tau = tree<node>;
	DBG(assert(l != nullptr && r != nullptr);)
	DBG(assert(tau::get(l).is(tau::variable) && tau::get(r).is(tau::bf));)
	return tau::get(tau::bf, tau::get(tau::bf_fex, l, r));
}

template <NodeType node>
tref build_bf_or(tref l, tref r) {
	using tau = tree<node>;
	DBG(assert(l != nullptr && r != nullptr);)
	DBG(assert(tau::get(l).is(tau::bf) && tau::get(r).is(tau::bf));)
	return tau::get(tau::bf, tau::get(tau::bf_or, l, r));
}

template <NodeType node>
tref build_bf_or(const auto& bfs, size_t type_id) {
	return std::accumulate(bfs.begin(), bfs.end(), _0<node>(type_id),
	[](tref l, tref r) { return build_bf_or<node>(l, r); });
}

template <NodeType node>
tref build_bf_nor(tref l, tref r) {
	using tau = tree<node>;
	DBG(assert(l != nullptr && r != nullptr);)
	DBG(assert(tau::get(l).is(tau::bf) && tau::get(r).is(tau::bf));)
	return tau::get(tau::bf, tau::get(tau::bf_nor, l, r));
}

template <NodeType node>
tref build_bf_xor(tref l, tref r) {
	DBG(assert(l != nullptr && r != nullptr);)
	using tau = tree<node>;
	DBG(assert(tau::get(l).is(tau::bf) && tau::get(r).is(tau::bf));)
	return tau::get(tau::bf, tau::get(tau::bf_xor, l, r));
}

template <NodeType node>
tref build_bf_xnor(tref l, tref r) {
	using tau = tree<node>;
	DBG(assert(l != nullptr && r != nullptr);)
	DBG(assert(tau::get(l).is(tau::bf) && tau::get(r).is(tau::bf));)
	return tau::get(tau::bf, tau::get(tau::bf_xnor, l, r));
}

template <NodeType node>
tref build_bf_and(tref l, tref r) {
	using tau = tree<node>;
	DBG(assert(l != nullptr && r != nullptr);)
	DBG(assert(tau::get(l).is(tau::bf) && tau::get(r).is(tau::bf));)
	return tau::get(tau::bf, tau::get(tau::bf_and, l, r));
}

template <NodeType node>
tref build_bf_and(const auto& bfs, size_t type_id) {
	return std::accumulate(bfs.begin(), bfs.end(), _1<node>(type_id),
		[](tref l, tref r) { return build_bf_and<node>(l, r);});
}

template <NodeType node>
tref build_bf_nand(tref l, tref r) {
	using tau = tree<node>;
	DBG(assert(l != nullptr && r != nullptr);)
	DBG(assert(tau::get(l).is(tau::bf) && tau::get(r).is(tau::bf));)
	return tau::get(tau::bf, tau::get(tau::bf_nand, l, r));
}

template <NodeType node>
tref build_bf_neg(tref l) {
	using tau = tree<node>;

	DBG(assert(l != nullptr);)
	using tau = tree<node>;
	DBG(assert(tau::get(l).is(tau::bf));)
	LOG_TRACE << "build_bf_neg";
	DBG(LOG_TRACE << "l: " << LOG_FM_DUMP(l);)
	DBG(LOG_TRACE << "bf_neg: " << LOG_FM_DUMP(tau::get(tau::bf_neg, l));)
	return tau::get(tau::bf, tau::get(tau::bf_neg, l));
}

template <NodeType node>
tref build_bf_shl(tref l, tref r) {
	using tau = tree<node>;

	DBG(assert(l != nullptr && r != nullptr);)
	DBG(assert(tau::get(l).is(tau::bf) && tau::get(r).is(tau::bf));)
	return tau::get(tau::bf, tau::get(tau::bf_shl, l, r));
}

template <NodeType node>
tref build_bf_shr(tref l, tref r) {
	using tau = tree<node>;

	DBG(assert(l != nullptr && r != nullptr);)
	DBG(assert(tau::get(l).is(tau::bf) && tau::get(r).is(tau::bf));)
	return tau::get(tau::bf, tau::get(tau::bf_shr, l, r));
}

template <NodeType node>
tref build_bf_add(tref l, tref r) {
	using tau = tree<node>;

	DBG(assert(l != nullptr && r != nullptr);)
	DBG(assert(tau::get(l).is(tau::bf) && tau::get(r).is(tau::bf));)
	return tau::get(tau::bf, tau::get(tau::bf_add, l, r));
}

template <NodeType node>
tref build_bf_sub(tref l, tref r) {
	using tau = tree<node>;

	DBG(assert(l != nullptr && r != nullptr);)
	DBG(assert(tau::get(l).is(tau::bf) && tau::get(r).is(tau::bf));)
	return tau::get(tau::bf, tau::get(tau::bf_sub, l, r));
}

template <NodeType node>
tref build_bf_mul(tref l, tref r) {
	using tau = tree<node>;

	DBG(assert(l != nullptr && r != nullptr);)
	DBG(assert(tau::get(l).is(tau::bf) && tau::get(r).is(tau::bf));)
	return tau::get(tau::bf, tau::get(tau::bf_mul, l, r));
}

template <NodeType node>
tref build_bf_div(tref l, tref r) {
	using tau = tree<node>;

	DBG(assert(l != nullptr && r != nullptr);)
	DBG(assert(tau::get(l).is(tau::bf) && tau::get(r).is(tau::bf));)
	return tau::get(tau::bf, tau::get(tau::bf_div, l, r));
}

template <NodeType node>
tref build_bf_mod(tref l, tref r) {
	using tau = tree<node>;

	DBG(assert(l != nullptr && r != nullptr);)
	DBG(assert(tau::get(l).is(tau::bf) && tau::get(r).is(tau::bf));)
	return tau::get(tau::bf, tau::get(tau::bf_mod, l, r));
}

// -----------------------------------------------------------------------------
// terminals, variables and constants

template <NodeType node>
tref build_bf_t_type(size_t ba_tid) {
	DBG(assert(ba_tid > 0);)
	using tau = tree<node>;
	return tau::get(tau::bf, tau::get(node::ba_typed(tau::bf_t, ba_tid)));
}

template <NodeType node>
tref build_bf_t_type(const std::string& type) {
	return build_bf_t_type<node>(get_ba_type_id<node>(type));
}

template <NodeType node>
tref build_bf_f_type(size_t ba_tid) {
	DBG(assert(ba_tid > 0);)
	using tau = tree<node>;
	return tau::get(tau::bf, tau::get(node::ba_typed(tau::bf_f, ba_tid)));
}

template <NodeType node>
tref build_bf_f_type(const std::string& type) {
	return build_bf_f_type<node>(get_ba_type_id<node>(type));
}

template <NodeType node>
tref build_ba_constant(const typename node::constant& constant,
		       size_t ba_type_id)
{
	return tree<node>::get_ba_constant(constant, ba_type_id);
}

template <NodeType node>
tref build_bf_ba_constant(const typename node::constant& constant,
		size_t ba_type_id, tref right) {
	using tau = tree<node>;
	return tau::get(tau::bf, {
		tau::get_ba_constant(constant, ba_type_id) }, right);
}

/*template <NodeType node>
tref build_bv_constant(const typename node::constant& constant, size_t ba_type_id) {
	return tree<node>::get_bv_constant(constant, ba_type_id);
}*/

template <NodeType node>
tref build_bv_ba_constant(const typename node::constant& constant, size_t ba_type_id) {
	using tau = tree<node>;

	return tau::get(tau::bv, {
		tau::get_ba_constant(constant, ba_type_id) });
}

template <NodeType node>
tref build_bf_uconst(const std::string& n1, const std::string& n2, size_t type_id) {
	using tau = tree<node>;

	return tau::get(tau::bf, tau::get_typed(tau::variable,
			tau::get(tau::uconst_name, n1 + ":" + n2), type_id));
}

template <NodeType node>
tref build_var_name(size_t sid) {
	using tau = tree<node>;
	return tau::get(node(tau::var_name, sid));
}

template <NodeType node>
tref build_var_name(const std::string& name) {
	using tau = tree<node>;
	return tau::get(tau::var_name, name);
}

template <NodeType node>
tref build_var_name_indexed(size_t index) {
	std::stringstream name;
	return build_var_name<node>((name << "i" << index, name.str()));
}

template <NodeType node>
tref build_variable(tref var_name_node, size_t type_id) {
	using tau = tree<node>;
	return tau::get_typed(tau::variable, var_name_node, type_id);
}

template <NodeType node>
tref build_variable(const std::string& name, size_t type_id) {
	using tau = tree<node>;
	return tau::get_typed(tau::variable, build_var_name<node>(name), type_id);
}

template <NodeType node>
tref build_bf_variable(tref var_name_node, size_t type_id) {
	using tau = tree<node>;
	return tau::get(tau::bf, build_variable<node>(var_name_node, type_id));
}

template <NodeType node>
tref build_bf_variable(const std::string& name, size_t type_id) {
	using tau = tree<node>;
	return tau::get(tau::bf, build_variable<node>(name, type_id));
}

template <NodeType node>
tref build_in_var(tref var_name_node, size_t type_id) {
	using tau = tree<node>;
	return tau::get(tau::bf, tau::get_typed(tau::variable,
			tau::get(node::input_variable(), var_name_node), type_id));
}

template <NodeType node>
tref build_in_var(tref var_name_node, tref offset_node, size_t type_id) {
	using tau = tree<node>;
	return tau::get(tau::bf, tau::get_typed(tau::variable,
			tau::get(node::input_variable(), var_name_node, offset_node), type_id));
}

template <NodeType node>
tref build_in_var_at_n(tref var_name_node, int_t pos, size_t type_id) {
	using tau = tree<node>;
	DBG(assert(var_name_node != nullptr);)
	DBG(assert(tau::get(var_name_node).is(tau::var_name));)
	return build_in_var<node>(var_name_node,
		tau::get(tau::offset, tau::get_integer(pos)), type_id);
}

template <NodeType node>
tref build_in_var_at_n(const std::string& name, int_t pos, size_t type_id) {
	return build_in_var_at_n<node>(build_var_name<node>(name), pos, type_id);
}

template <NodeType node>
tref build_in_var_at_n_indexed(size_t index, int_t pos, size_t type_id) {
	return build_in_var_at_n<node>(
		build_var_name_indexed<node>(index), pos, type_id);
}

template <NodeType node>
tref build_in_var_at_t(tref var_name_node, size_t type_id, std::string t) {
	using tau = tree<node>;
	DBG(assert(var_name_node != nullptr);)
	DBG(assert(tau::get(var_name_node).is(tau::var_name));)
	return build_in_var<node>(var_name_node,
		tau::get(tau::offset, build_variable<node>(t, get_ba_type_id<node>(nat_type<node>()))), type_id);
}

template <NodeType node>
tref build_in_var_at_t_indexed(size_t index, size_t type_id, std::string t) {
	return build_in_var_at_t<node>(build_var_name_indexed<node>(index), type_id, t);
}

template <NodeType node>
tref build_in_var_at_t_minus(tref var_name_node, size_t shift, size_t type_id, std::string t) {
	using tau = tree<node>;
	DBG(assert(var_name_node != nullptr);)
	DBG(assert(tau::get(var_name_node).is(tau::var_name));)
	return build_in_var<node>(var_name_node,
		tau::get(tau::offset, tau::get(tau::shift,
			build_variable<node>(t, get_ba_type_id<node>(nat_type<node>())), tau::get_num(shift))), type_id);
}

template <NodeType node>
tref build_in_var_at_t_minus(const std::string& var_name, size_t shift, size_t type_id, std::string t) {
	return build_in_var_at_t_minus<node>(
		build_var_name<node>(var_name), shift, t, type_id);
}

template <NodeType node>
tref build_in_var_at_t_minus_indexed(size_t index, size_t shift, size_t type_id, std::string t) {
	return build_in_var_at_t_minus<node>(
		build_var_name_indexed<node>(index), shift, t, type_id);
}

template <NodeType node>
tref build_out_var(tref var_name_node, size_t type_id) {
	using tau = tree<node>;
	return tau::get(tau::bf, tau::get_typed(tau::variable,
			tau::get(node::output_variable(), var_name_node), type_id));
}

template <NodeType node>
tref build_out_var(tref var_name_node, tref offset_node, size_t type_id) {
	using tau = tree<node>;
	return tau::get(tau::bf, tau::get_typed(tau::variable,
			tau::get(node::output_variable(), var_name_node, offset_node), type_id));
}

template <NodeType node>
tref build_out_var_at_n(tref var_name_node, int_t pos, size_t type_id) {
	using tau = tree<node>;
	return build_out_var<node>(var_name_node,
		tau::get(tau::offset, tau::get_integer(pos)), type_id);
}

template <NodeType node>
tref build_out_var_at_n(const std::string& name, int_t pos, size_t type_id) {
	return build_out_var_at_n<node>(build_var_name<node>(name), pos, type_id);
}

template <NodeType node>
tref build_out_var_at_n_indexed(size_t index, int_t pos, size_t type_id) {
	return build_out_var_at_n<node>(
		build_var_name_indexed<node>(index), pos, type_id);
}

template <NodeType node>
tref build_out_var_at_t(tref var_name_node, size_t type_id, std::string t) {
	using tau = tree<node>;
	DBG(assert(var_name_node != nullptr);)
	DBG(assert(tau::get(var_name_node).is(tau::var_name));)
	return build_out_var<node>(var_name_node,
		tau::get(tau::offset, build_variable<node>(t,
			get_ba_type_id<node>(nat_type<node>()))), type_id);
}

template <NodeType node>
tref build_out_var_at_t_indexed(size_t index, size_t type_id, std::string t) {
	return build_out_var_at_t<node>(
		build_var_name_indexed<node>(index), type_id, t);
}

template <NodeType node>
tref build_out_var_at_t_minus(tref var_name_node, size_t shift, size_t type_id, std::string t) {
	using tau = tree<node>;
	return build_out_var<node>(var_name_node,
		tau::get(tau::offset, tau::get(tau::shift,
			build_variable<node>(t,
				get_ba_type_id<node>(nat_type<node>())),
				tau::get_num(shift))), type_id);
}

template <NodeType node>
tref build_out_var_at_t_minus(const std::string& name, size_t shift, size_t type_id, std::string t) {
	return build_out_var_at_t_minus<node>(build_var_name<node>(name), shift, type_id, t);
}

template <NodeType node>
tref build_out_var_at_t_minus_indexed(size_t index, size_t shift, size_t type_id, std::string t) {
	return build_out_var_at_t_minus<node>(
		build_var_name_indexed<node>(index), shift, type_id, t);
}

template <NodeType node>
tref build_spec(const rr<node>& nso_rr) {
	using tau = tree<node>;
	trefs rels(nso_rr.rec_relations.size());
	for (size_t i = 0; i < nso_rr.rec_relations.size(); ++i) {
		auto& r = nso_rr.rec_relations[i];
		rels[i] = tau::get(tau::rec_relation, r.first->get(), r.second->get());
	}
	return tau::get(tau::spec,
		tau::get(tau::definitions, rels),
		tau::get(tau::main, nso_rr.main->get()));
}

// -----------------------------------------------------------------------------
// same builders on the tree API
// -----------------------------------------------------------------------------

template <NodeType node>
tref tree<node>::_0(size_t type_id) { return tau_lang::_0<node>(type_id); }
template <NodeType node>
tref tree<node>::_1(size_t type_id) { return tau_lang::_1<node>(type_id); }
template <NodeType node>
tref tree<node>::_F() { return tau_lang::_F<node>(); }
template <NodeType node>
tref tree<node>::_T() { return tau_lang::_T<node>(); }
template <NodeType node>
tref tree<node>::_0_trimmed(size_t type_id) { return tau_lang::_0_trimmed<node>(type_id); }
template <NodeType node>
tref tree<node>::_1_trimmed(size_t type_id) { return tau_lang::_1_trimmed<node>(type_id); }
template <NodeType node>
tref tree<node>::_F_trimmed() { return tau_lang::_F_trimmed<node>(); }
template <NodeType node>
tref tree<node>::_T_trimmed() { return tau_lang::_T_trimmed<node>(); }

template <NodeType node>
const tree<node>& tree<node>::get_0(size_t type_id) { return tau_lang::get_0<node>(type_id); }
template <NodeType node>
const tree<node>& tree<node>::get_1(size_t type_id) { return tau_lang::get_1<node>(type_id); }
template <NodeType node>
const tree<node>& tree<node>::get_F() { return tau_lang::get_F<node>(); }
template <NodeType node>
const tree<node>& tree<node>::get_T() { return tau_lang::get_T<node>(); }
template <NodeType node>
const tree<node>& tree<node>::get_0_trimmed(size_t type_id) { return tau_lang::get_0_trimmed<node>(type_id); }
template <NodeType node>
const tree<node>& tree<node>::get_1_trimmed(size_t type_id) { return tau_lang::get_1_trimmed<node>(type_id); }
template <NodeType node>
const tree<node>& tree<node>::get_F_trimmed() { return tau_lang::get_F_trimmed<node>(); }
template <NodeType node>
const tree<node>& tree<node>::get_T_trimmed() { return tau_lang::get_T_trimmed<node>(); }

// -----------------------------------------------------------------------------
// formula builders

template <NodeType node>
tref tree<node>::build_wff_sometimes(tref l) {
	return tau_lang::build_wff_sometimes<node>(l);
}

template <NodeType node>
tref tree<node>::build_wff_always(tref l) {
	return tau_lang::build_wff_always<node>(l);
}

template <NodeType node>
tref tree<node>::build_wff_conditional(tref x, tref y, tref z) {
	return tau_lang::build_wff_conditional<node>(x, y, z);
}

template <NodeType node>
tref tree<node>::build_wff_all(tref bound_var, tref subformula, bool calculate_quant_id) {
	return tau_lang::build_wff_all<node>(bound_var, subformula, calculate_quant_id);
}

template <NodeType node>
tref tree<node>::build_wff_all_many(const trefs& bound_vars, tref subformula) {
	return tau_lang::build_wff_all_many<node>(bound_vars, subformula);
}

template <NodeType node>
tref tree<node>::build_wff_ex(tref bound_var, tref subformula, bool calculate_quant_id) {
	return tau_lang::build_wff_ex<node>(bound_var, subformula, calculate_quant_id);
}

template <NodeType node>
tref tree<node>::build_wff_ex_many(const trefs& bound_vars, tref subformula) {
	return tau_lang::build_wff_ex_many<node>(bound_vars, subformula);
}

template <NodeType node>
tref tree<node>::build_wff_imply(tref l, tref r) {
	return tau_lang::build_wff_imply<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_wff_rimply(tref l, tref r) {
	return tau_lang::build_wff_rimply<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_wff_equiv(tref l, tref r) {
	return tau_lang::build_wff_equiv<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_wff_or(tref l, tref r) {
	return tau_lang::build_wff_or<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_wff_or(const auto& wffs) {
	return tau_lang::build_wff_or<node>(wffs);
}

template <NodeType node>
tref tree<node>::build_wff_xor(tref l, tref r) {
	return tau_lang::build_wff_xor<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_wff_and(tref l, tref r) {
	return tau_lang::build_wff_and<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_wff_and(const auto& wffs) {
	return tau_lang::build_wff_and<node>(wffs);
}

template <NodeType node>
tref tree<node>::build_wff_neg(tref l) {
	return tau_lang::build_wff_neg<node>(l);
}

// -----------------------------------------------------------------------------
// constraint builders

template <NodeType node>
tref tree<node>::build_wff_ctn_gteq(tref ctnvar, tref num) {
	return tau_lang::build_wff_ctn_gteq<node>(ctnvar, num);
}

template <NodeType node>
tref tree<node>::build_wff_ctn_gt(tref ctnvar, tref num) {
	return tau_lang::build_wff_ctn_gt<node>(ctnvar, num);
}

template <NodeType node>
tref tree<node>::build_wff_ctn_lteq(tref ctnvar, tref num) {
	return tau_lang::build_wff_ctn_lteq<node>(ctnvar, num);
}

template <NodeType node>
tref tree<node>::build_wff_ctn_lt(tref ctnvar, tref num) {
	return tau_lang::build_wff_ctn_lt<node>(ctnvar, num);
}

template <NodeType node>
tref tree<node>::build_wff_ctn_eq(tref ctnvar, tref num) {
	return tau_lang::build_wff_ctn_eq<node>(ctnvar, num);
}

template <NodeType node>
tref tree<node>::build_wff_ctn_neq(tref ctnvar, tref num) {
	return tau_lang::build_wff_ctn_neq<node>(ctnvar, num);
}

// -----------------------------------------------------------------------------
// wff relational operators of terms

template <NodeType node>
tref tree<node>::build_bf_interval(tref x, tref y, tref z) {
	return tau_lang::build_bf_interval<node>(x, y, z);
}

template <NodeType node>
tref tree<node>::build_bf_eq(tref l, tref r) {
	return tau_lang::build_bf_eq<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_eq_0(tref l) {
	return tau_lang::build_bf_eq_0<node>(l);
}

template<NodeType node>
tref tree<node>::build_bf_neq(tref l, tref r) {
	return tau_lang::build_bf_neq<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_neq_0(tref l) {
	return tau_lang::build_bf_neq_0<node>(l);
}

template <NodeType node>
tref tree<node>::build_bf_lteq(tref l, tref r) {
	return tau_lang::build_bf_lteq<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_nlteq(tref l, tref r) {
	return tau_lang::build_bf_nlteq<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_gt(tref l, tref r) {
	return tau_lang::build_bf_gt<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_ngt(tref l, tref r) {
	return tau_lang::build_bf_ngt<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_gteq(tref l, tref r) {
	return tau_lang::build_bf_gteq<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_ngteq(tref l, tref r) {
	return tau_lang::build_bf_ngteq<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_lt(tref l, tref r) {
	return tau_lang::build_bf_lt<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_nlt(tref l, tref r) {
	return tau_lang::build_bf_nlt<node>(l, r);
}

// -----------------------------------------------------------------------------
// term builders

template <NodeType node>
tref tree<node>::build_bf_fall(tref l, tref r) {
	return tau_lang::build_bf_fall<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_fex(tref l, tref r) {
	return tau_lang::build_bf_fex<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_or(tref l, tref r) {
	return tau_lang::build_bf_or<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_or(const auto& bfs, size_t type_id) {
	return tau_lang::build_bf_or<node>(bfs, type_id);
}

template <NodeType node>
tref tree<node>::build_bf_nor(tref l, tref r) {
	return tau_lang::build_bf_nor<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_xor(tref l, tref r) {
	return tau_lang::build_bf_xor<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_xnor(tref l, tref r) {
	return tau_lang::build_bf_xnor<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_and(tref l, tref r) {
	return tau_lang::build_bf_and<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_and(const auto& bfs, size_t type_id) {
	return tau_lang::build_bf_and<node>(bfs, type_id);
}

template <NodeType node>
tref tree<node>::build_bf_nand(tref l, tref r) {
	return tau_lang::build_bf_nand<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_neg(tref l) {
	return tau_lang::build_bf_neg<node>(l);
}

template <NodeType node>
tref tree<node>::build_bf_shl(tref l, tref r) {
	return tau_lang::build_bf_shl<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_shr(tref l, tref r) {
	return tau_lang::build_bf_shr<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_add(tref l, tref r) {
	return tau_lang::build_bf_add<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_sub(tref l, tref r) {
	return tau_lang::build_bf_sub<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_mul(tref l, tref r) {
	return tau_lang::build_bf_mul<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_div(tref l, tref r) {
	return tau_lang::build_bf_div<node>(l, r);
}

template <NodeType node>
tref tree<node>::build_bf_mod(tref l, tref r) {
	return tau_lang::build_bf_mod<node>(l, r);
}

// -----------------------------------------------------------------------------
// terminals, variables and constants

template <NodeType node>
tref tree<node>::build_bf_t_type(size_t ba_tid) {
	return tau_lang::build_bf_t_type<node>(ba_tid);
}

template <NodeType node>
tref tree<node>::build_bf_f_type(size_t ba_tid) {
	return tau_lang::build_bf_f_type<node>(ba_tid);
}

template <NodeType node>
tref tree<node>::build_ba_constant(const constant& constant,
	size_t ba_type_id)
{
	return tau_lang::build_ba_constant<node>(constant, ba_type_id);
}

template <NodeType node>
tref tree<node>::build_bf_ba_constant(const constant& constant,
	size_t ba_type_id, tref right)
{
	return tau_lang::build_bf_ba_constant<node>(constant, ba_type_id, right);
}

template <NodeType node>
tref tree<node>::build_bv_ba_constant(const constant& constant, size_t type_id)
{
	return tau_lang::build_bv_ba_constant<node>(constant, type_id);
}

template <NodeType node>
tref tree<node>::build_bf_uconst(const std::string& n1, const std::string& n2, size_t type_id) {
	return tau_lang::build_bf_uconst<node>(n1, n2, type_id);
}

template <NodeType node>
tref tree<node>::build_var_name(size_t sid) {
	return tau_lang::build_var_name<node>(sid);
}

template <NodeType node>
tref tree<node>::build_var_name(const std::string& name) {
	return tau_lang::build_var_name<node>(name);
}

template <NodeType node>
tref tree<node>::build_var_name_indexed(size_t index) {
	return tau_lang::build_var_name_indexed<node>(index);
}

template <NodeType node>
tref tree<node>::build_variable(tref var_name_node, size_t type_id) {
	return tau_lang::build_variable<node>(var_name_node, type_id);
}

template <NodeType node>
tref tree<node>::build_variable(const std::string& name, size_t type_id) {
	return tau_lang::build_variable<node>(name, type_id);
}

template <NodeType node>
tref tree<node>::build_bf_variable(tref var_name_node, size_t type_id) {
	return tau_lang::build_bf_variable<node>(var_name_node, type_id);
}

template <NodeType node>
tref tree<node>::build_bf_variable(const std::string& name, size_t type_id) {
	return tau_lang::build_bf_variable<node>(name, type_id);
}

template <NodeType node>
tref tree<node>::build_in_var(tref var_name_node, size_t type_id) {
	return tau_lang::build_in_var<node>(var_name_node, type_id);
}

template <NodeType node>
tref tree<node>::build_in_var(tref var_name_node, tref offset_node, size_t type_id) {
	return tau_lang::build_in_var<node>(var_name_node, offset_node, type_id);
}

template <NodeType node>
tref tree<node>::build_in_var_at_n(tref var_name_node, int_t pos, size_t type_id) {
	return tau_lang::build_in_var_at_n<node>(var_name_node, pos, type_id);
}

template <NodeType node>
tref tree<node>::build_in_var_at_n(const std::string& name, int_t pos, size_t type_id) {
	return tau_lang::build_in_var_at_n<node>(name, pos, type_id);
}

template <NodeType node>
tref tree<node>::build_in_var_at_n_indexed(size_t index, int_t pos, size_t type_id) {
	return tau_lang::build_in_var_at_n_indexed<node>(index, pos, type_id);
}

template <NodeType node>
tref tree<node>::build_in_var_at_t(tref var_name_node, size_t type_id, std::string t) {
	return tau_lang::build_in_var_at_t<node>(var_name_node, type_id, t);
}

template <NodeType node>
tref tree<node>::build_in_var_at_t_indexed(size_t index, size_t type_id, std::string t) {
	return tau_lang::build_in_var_at_t_indexed<node>(index, type_id, t);
}

template <NodeType node>
tref tree<node>::build_in_var_at_t_minus(
	tref var_name_node, size_t shift, size_t type_id, std::string t)
{
	return tau_lang::build_in_var_at_t_minus<node>(var_name_node, shift, t, type_id);
}

template <NodeType node>
tref tree<node>::build_in_var_at_t_minus(
	const std::string& name, size_t shift, size_t type_id, std::string t)
{
	return tau_lang::build_in_var_at_t_minus<node>(name, shift, type_id, t);
}

template <NodeType node>
tref tree<node>::build_in_var_at_t_minus_indexed(
	size_t index, size_t shift, size_t type_id, std::string t)
{
	return tau_lang::build_in_var_at_t_minus_indexed<node>(index, shift, type_id, t);
}

template <NodeType node>
tref tree<node>::build_out_var(tref var_name_node, size_t type_id) {
	return tau_lang::build_out_var<node>(var_name_node, type_id);
}

template <NodeType node>
tref tree<node>::build_out_var(tref var_name_node, tref offset_node, size_t type_id) {
	return tau_lang::build_out_var<node>(var_name_node, offset_node, type_id);
}

template <NodeType node>
tref tree<node>::build_out_var_at_n(tref var_name_node, int_t pos, size_t type_id) {
	return tau_lang::build_out_var_at_n<node>(var_name_node, pos, type_id);
}

template <NodeType node>
tref tree<node>::build_out_var_at_n(const std::string& name, int_t pos, size_t type_id){
	return tau_lang::build_out_var_at_n<node>(name, pos, type_id);
}

template <NodeType node>
tref tree<node>::build_out_var_at_n_indexed(size_t index, int_t pos, size_t type_id) {
	return tau_lang::build_out_var_at_n_indexed<node>(index, pos, type_id);
}

template <NodeType node>
tref tree<node>::build_out_var_at_t(tref var_name_node, size_t type_id, std::string t) {
	return tau_lang::build_out_var_at_t<node>(var_name_node, type_id, t);
}

template <NodeType node>
tref tree<node>::build_out_var_at_t_indexed(size_t index, size_t type_id, std::string t) {
	return tau_lang::build_out_var_at_t_indexed<node>(index, type_id, t);
}

template <NodeType node>
tref tree<node>::build_out_var_at_t_minus(
	tref var_name_node, size_t shift, size_t type_id, std::string t)
{
	return tau_lang::build_out_var_at_t_minus<node>(var_name_node, shift, type_id, t);
}

template <NodeType node>
tref tree<node>::build_out_var_at_t_minus(
	const std::string& name, size_t shift, size_t type_id, std::string t)
{
	return tau_lang::build_out_var_at_t_minus<node>(name, shift, type_id, t);
}

template <NodeType node>
tref tree<node>::build_out_var_at_t_minus_indexed(
	size_t index, size_t shift, size_t type_id, std::string t)
{
	return tau_lang::build_out_var_at_t_minus_indexed<node>(index, shift, type_id, t);
}

template <NodeType node>
tref tree<node>::build_spec(const rr<node>& nso_rr) {
	return tau_lang::build_spec<node>(nso_rr);
}

} // namespace idni::tau_lang
